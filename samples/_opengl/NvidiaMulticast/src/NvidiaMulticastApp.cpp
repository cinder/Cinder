#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/Log.h"
#include "cinder/CameraUi.h"
#include "cinder/gl/Ubo.h"
#include "cinder/gl/Ssbo.h"
#include "cinder/gl/nv/Multicast.h"

#include "jsoncpp/json.h"

using namespace ci;
using namespace ci::app;
namespace multicast = ci::gl::nv::multicast;

static Json::Value settings;

#define ASYMMETRICAL_CAMERAS

namespace cinder {
	namespace gl {
		struct ScopedSsboBind {
			ScopedSsboBind( const gl::SsboRef& bufferObj, uint8_t bufferUnit )
				: mSsbo{ bufferObj }
			{
				mSsbo->bindBase( static_cast<GLuint>( bufferUnit ) );
				gl::context()->pushBufferBinding( mSsbo->getTarget(), mSsbo->getId() );
			}

			~ScopedSsboBind()
			{
				mSsbo->unbindBase();
				gl::context()->popBufferBinding( mSsbo->getTarget() );
			}
		private:
			gl::SsboRef mSsbo;
		};
	}
}

struct CameraMatrices {
	glm::mat4 view;
	glm::mat4 projection;
};

class NvidiaMulticastApp : public App {
public:
	NvidiaMulticastApp();
	void	setup() override;
	void	resize() override;
	void	update() override;
	void	draw() override;
	void	drawScene();
	void	keyDown( KeyEvent event ) override;

	CameraPersp			mCam;
	CameraUi			mCamUi;
	gl::UboRef			mCamUbo;
	CameraMatrices		mCameraMatrices;

	gl::BatchRef		mBatch;
	gl::Texture2dRef	mTexture;
	mat4				mCubeRotation;

	vec3				mColor;
	gl::UboRef			mUbo;
	gl::SsboRef			mSsbo;
	size_t				mInstanceCount;
	gl::FboRef			mFbo;
	gl::Texture2dRef	mTextureSecondary;

	std::vector<multicast::Device> mGPUs;
};

NvidiaMulticastApp::NvidiaMulticastApp()
	: mCamUi{ &mCam, app::getWindow() }
	, mGPUs{ multicast::getDevices() }
{
	if( mGPUs.size() < 2 ) {
		throw ci::Exception( "This samples required (at least) two NVIDIA GPU devices in Mosaic or SLI mode." );
	}

	log::makeLogger<log::LoggerFileRotating>( getAppPath(), "%Y_%m_%d_%H_%M_%S.log" )->setTimestampEnabled( true );

	std::vector<glm::vec4> positions;
	for( int x = -25; x <= 25; ++x ) {
		for( int z = -25; z <= 25; ++z ) {
			positions.push_back( 2.0f * vec4( x, 0, z, 1 ) );
		}
	}
	mSsbo = gl::Ssbo::create( sizeof( vec4 ) * positions.size(), positions.data(), GL_STATIC_DRAW );
	mInstanceCount = positions.size();

	if( settings["vsync"].asBool() ) {
		gl::enableVerticalSync();
	}
}

void NvidiaMulticastApp::setup()
{
	CI_LOG_I( gl::getString( GL_VERSION ) );

	mCam.lookAt( vec3( 3, 2, 4 ), vec3( 0 ) );

	try {
		auto shaderPreproc = std::make_shared<gl::ShaderPreprocessor>();
#if defined( ASYMMETRICAL_CAMERAS )
		shaderPreproc->addDefine( "ASYMMETRICAL" );
#endif
		mBatch = gl::Batch::create( geom::Teapot(), gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "shader.vert" ) ).fragment( loadAsset( "shader.frag" ) ).preprocessor( shaderPreproc ) ) );
		mBatch->getGlslProg()->uniformBlock( "uMaterial", 0 );
	}
	catch( const std::exception & e ) {
		CI_LOG_E( "Shader Error: " << e.what() );
	}

	gl::enable( GL_SCISSOR_TEST, GL_TRUE );
	gl::enableDepth();

	// Same format for primary and staging texture
	auto texFmt = gl::Texture::Format().perGpuStorageNV( true ).mipmap( true );
	auto img0 = loadImage( loadAsset( "texture.png" ) );
	auto img1 = loadImage( loadAsset( "tiles.png" ) );
	// Ensure that both texture have the same size, format & properties.
	CI_ASSERT( img0->getWidth() == img1->getWidth() );
	CI_ASSERT( img0->getHeight() == img1->getHeight() );
	CI_ASSERT( img0->getChannelOrder() == img1->getChannelOrder() );
	CI_ASSERT( img0->getColorModel() == img1->getColorModel() );
	CI_ASSERT( img0->getDataType() == img1->getDataType() );
	CI_ASSERT( img0->hasAlpha() == img1->hasAlpha() );
	mTexture = gl::Texture2d::create( img0->getWidth(), img0->getHeight(), texFmt );
	
	multicast::enableUploadMask( mGPUs[0] );
	mTexture->update( ci::Surface{ img0 } );
	// Replace texture on secondary GPU
	multicast::enableUploadMask( mGPUs[1] );
	mTexture->update( ci::Surface{ img1 } );
	multicast::disableUploadMask();

	// Per gpu color buffer (Red/Green)
	mColor = vec3( 1, 0, 0 );
	mUbo = gl::Ubo::create();
	mUbo->namedBufferStorage( sizeof( vec3 ), &mColor, GL_DYNAMIC_STORAGE_BIT | GL_PER_GPU_STORAGE_BIT_NV );
	mColor = vec3( 0, 1, 0 );
	multicast::bufferSubData( mUbo, sizeof( vec3 ), &mColor, mGPUs[1] );
	mUbo->bindBufferBase( 0 );

	// Per-gpu camera transformation (for the subdivided view columns).
	mCamUbo = gl::Ubo::create();
	mCamUbo->namedBufferStorage( sizeof( CameraMatrices ), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_PER_GPU_STORAGE_BIT_NV );
	mCamUbo->bindBufferBase( 1 );
}

void NvidiaMulticastApp::resize()
{
	mFbo = gl::Fbo::create( app::getWindowWidth() / 2, app::getWindowHeight() );
	mTextureSecondary = gl::Texture2d::create( mFbo->getWidth(), mFbo->getHeight() );

	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
}

void NvidiaMulticastApp::update()
{
	getWindow()->setTitle( std::to_string( getAverageFps() ) );
}

void NvidiaMulticastApp::draw()
{
	gl::clear();
	gl::setMatrices( mCam );

	if( settings["copy_to_primary"].asBool() ) {
		{
			gl::ScopedFramebuffer pusFbo{ mFbo };
			gl::clear();
#if defined( ASYMMETRICAL_CAMERAS )
			gl::viewport( ivec2( getWindowWidth() / 2, getWindowHeight() ) );
#else
			gl::viewport( app::getWindowSize() );
			multicast::viewport( mGPUs[0], ivec2( 0 ), getWindowSize() );
			multicast::viewport( mGPUs[1], ivec2( -getWindowWidth() / 2, 0 ), getWindowSize() );
#endif
			drawScene();
		}

		multicast::copyImageSubData( mFbo->getColorTexture(), mTextureSecondary, mGPUs[1], mGPUs[0] );

		multicast::signalWaitSync( mGPUs[1], mGPUs[0] );
		multicast::enableRenderMask( mGPUs[0] );

		gl::viewport( app::getWindowSize() );
		gl::setMatricesWindow( app::getWindowSize() );
		gl::draw( mFbo->getColorTexture() );
		gl::translate( vec2( mFbo->getWidth(), 0.0f ) );
		gl::draw( mTextureSecondary );

		//GPU 1 should wait for GPU0 to be done with screen rendering.
		multicast::signalWaitSync( mGPUs[0], mGPUs[1] );
		multicast::disableRenderMask();
	}
	else {
#if defined( ASYMMETRICAL_CAMERAS )
		multicast::viewport( mGPUs[0], ivec2( 0 ), ivec2( getWindowWidth() / 2, getWindowHeight() ) );
		multicast::viewport( mGPUs[1], ivec2( getWindowWidth() / 2, 0 ), ivec2( getWindowWidth() / 2, getWindowHeight() ) );
#else
		gl::viewport( app::getWindowSize() );
		multicast::scissor( mGPUs[0], ivec2( 0 ), ivec2( getWindowWidth() / 2, getWindowHeight() ) );
		multicast::scissor( mGPUs[1], ivec2( getWindowWidth() / 2, 0 ), ivec2( getWindowWidth() / 2, getWindowHeight() ) );
#endif
		drawScene();
	}
}

void NvidiaMulticastApp::drawScene()
{
#if defined( ASYMMETRICAL_CAMERAS )
	for( size_t i = 0; i < mGPUs.size(); ++i ) {
		auto cameraTile = mCam.subdivide( uvec2( 1, mGPUs.size() ), uvec2( 0, i ) );
		mCameraMatrices.view = cameraTile.getViewMatrix();
		mCameraMatrices.projection = cameraTile.getProjectionMatrix();
		multicast::bufferSubData( mCamUbo, sizeof( CameraMatrices ), &mCameraMatrices, mGPUs.at(i) );
	}
#endif
	gl::ScopedTextureBind push( mTexture, 0 );
	gl::ScopedSsboBind pushBuffer{ mSsbo, 0 };
	mBatch->drawInstanced( (GLsizei)mInstanceCount );
}

void NvidiaMulticastApp::keyDown( KeyEvent event )
{
	if( event.isControlDown() && event.getCode() == KeyEvent::KEY_q ) {
		quit();
	}
}

RendererGl::Options getRendererGlOptions()
{
	// Load settings first!
	std::ifstream in{ getAssetPath( "settings.json" ) };
	Json::Reader reader;
	reader.parse( in, settings );

	RendererGl::Options options;
	if( settings["multicast_multidisplay"].asBool() ) {
		//! This options should be use with Quadro devices with Mosaic enabled, with each display connected to independent GPUs.
		options.multiGpuMultiDisplayMulticastNV();
	}
	else {
		//! This option can be used with GeForce or Quadro devices, with SLI "Maximize 3D Performance" enabled.
		options.multiGpuMulticastNV();
	}
	options.msaa( 0 );
	options.setVersion( 4, 5 );
	options.debug();
	//options.debugLog();
	options.debugBreak();
	return options;
}

void prepareSettings( App::Settings* set )
{
	set->setWindowSize( settings["resolution"][0].asInt(), settings["resolution"][1].asInt() );
	set->setFullScreen( settings["fullscreen"].asBool() );
	set->setBorderless( settings["borderless"].asBool() );
	if( settings["vsync"].asBool() ) set->disableFrameRate();
}


CINDER_APP( NvidiaMulticastApp, RendererGl( getRendererGlOptions() ), prepareSettings )
