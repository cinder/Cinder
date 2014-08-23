#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Context.h"

#include "cinder/GeomIo.h"
#include "cinder/Rand.h"

#include "cinder/Log.h"
#include "cinder/params/Params.h"

#include "BlurrableThings.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MotionBlurVelocityBufferApp : public AppNative {
  public:
	void prepareSettings( Settings *settings ) override;
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;

	void createGeometry();
	void createBuffers();
	void loadShaders();
	void drawVelocityBuffers();

  private:
	gl::TextureRef					mBackground;
	std::vector<BlurrableMeshRef>	mMeshes;

	gl::GlslProgRef		mVelocityProg;		// Renders screen-space velocity to RG channels.
	gl::GlslProgRef		mTileProg;			// Downsamples velocity, preserving local maxima
	gl::GlslProgRef		mNeighborProg;		// Finds dominant velocities in downsampled map
	gl::GlslProgRef		mMotionBlurProg;	// Generates final image from color and velocity buffers
	gl::GlslProgRef		mVelocityRenderProg;// Debug rendering of velocity to screen.

	gl::FboRef			mColorBuffer;		// full-resolution RGBA color
	gl::FboRef			mVelocityBuffer;	// full-resolution velocity
	gl::FboRef			mTileMaxBuffer;		// downsampled velocity
	gl::FboRef			mNeighborMaxBuffer;	// dominant velocities in regions

	params::InterfaceGlRef	mParams;
	int						mTileSize = 20;
	int						mSampleCount = 31;
	float					mAnimationSpeed = 1.0f;
	float					mBlurNoise = 0.0f;
	bool					mBlurEnabled = true;
	bool					mPaused = false;
	bool					mDisplayVelocityBuffers = true;
};

void MotionBlurVelocityBufferApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1280, 720 );

	#if defined( CINDER_COCOA_TOUCH ) || defined( CINDER_COCOA_TOUCH_SIMULATOR )
		getSignalSupportedOrientations().connect( [] { return InterfaceOrientation::LandscapeAll; } );
	#endif // COCOA_TOUCH
}

void MotionBlurVelocityBufferApp::setup()
{
	mBackground = gl::Texture::create( loadImage( loadAsset( "background.jpg" ) ) );

	createGeometry();
	createBuffers();
	loadShaders();

	mParams = params::InterfaceGl::create( "Motion Blur Options", ivec2( 250, 300 ) );
	mParams->addParam( "Enable Blur", &mBlurEnabled );
	mParams->addParam( "Show Velocity Buffers", &mDisplayVelocityBuffers );
	mParams->addParam( "Pause Animation", &mPaused );
	mParams->addParam( "Animation Speed", &mAnimationSpeed ).min( 0.05f ).step( 0.2f );
	mParams->addParam( "Max Samples", &mSampleCount ).min( 1 ).step( 2 );
	mParams->addParam( "Blur Noise", &mBlurNoise ).min( 0.0f ).step( 0.01f );

}

void MotionBlurVelocityBufferApp::createGeometry()
{
	for( int i = 0; i < 20; ++i )
	{	// create some randomized geometry
		vec3 pos = vec3( randFloat( 200.0f, getWindowWidth() - 200.0f ), randFloat( 150.0f, getWindowHeight() - 150.0f ), randFloat( -50.0f, 10.0f ) );
		float base = randFloat( 50.0f, 100.0f );
		float height = randFloat( 100.0f, 300.0f );

		auto mesh = make_shared<BlurrableMesh>( gl::VboMesh::create( geom::Cone().height( height ).base( base ) ), pos );
		mesh->setAxis( randVec3f() );
		mesh->setColor( ColorA( CM_HSV, randFloat( 0.05f, 0.33f ), 1.0f, 1.0f ) );
		mesh->setOscillation( vec3( randFloat( -150.0f, 150.0f ), randFloat( -300.0f, 300.0f ), randFloat( -500.0f, 200.0f ) ) );
		mesh->setTheta( randFloat( M_PI * 2 ) );

		mMeshes.push_back( mesh );
	}
}

void MotionBlurVelocityBufferApp::createBuffers()
{
	const int bufferWidth = getWindowWidth();
	const int bufferHeight = getWindowHeight();

	{ // color
		gl::Fbo::Format format;
		format.colorTexture( gl::Texture::Format().internalFormat( GL_RGBA ) );
		mColorBuffer = gl::Fbo::create( bufferWidth, bufferHeight, format );
	}
	{ // velocity
		gl::Fbo::Format format;
		format.colorTexture( gl::Texture::Format().internalFormat( GL_RG16F ) );
		mVelocityBuffer = gl::Fbo::create( bufferWidth, bufferHeight, format );
	}

	{ // neighbor tile
		gl::Fbo::Format format;
		format.colorTexture( gl::Texture::Format().internalFormat( GL_RG16F ) ).disableDepth();
		mTileMaxBuffer = gl::Fbo::create( bufferWidth / mTileSize, bufferHeight / mTileSize, format );
		mNeighborMaxBuffer = gl::Fbo::create( bufferWidth / mTileSize, bufferHeight / mTileSize, format );
	}
}

void MotionBlurVelocityBufferApp::loadShaders()
{
	try
	{
		mVelocityProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "velocity.vs" ) )
						.fragment( loadAsset( "velocity.fs" ) ) );
		mMotionBlurProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "passthrough.vs" ) )
								.fragment( loadAsset( "motion-blur.fs" ) ) );
		mVelocityRenderProg	= gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "passthrough.vs" ) )
								.fragment( loadAsset( "velocity-render.fs" ) ) );
		mTileProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "passthrough.vs" ) )
					.fragment( loadAsset( "tilemax.fs" ) ) );
		mNeighborProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( loadAsset( "passthrough.vs" ) )
						.fragment( loadAsset( "neighbormax.fs" ) ) );
	}
	catch( ci::gl::GlslProgCompileExc &exc )
	{
		CI_LOG_E( "Shader load error: " << exc.what() );
	}
	catch( ci::Exception &exc )
	{
		CI_LOG_E( "Shader load error: " << exc.what() );
	}
}

void MotionBlurVelocityBufferApp::keyDown( KeyEvent event )
{
	switch ( event.getCode() )
	{
		case KeyEvent::KEY_SPACE:
			mPaused = ! mPaused;
		break;
		case KeyEvent::KEY_b:
			mBlurEnabled = ! mBlurEnabled;
		break;
		case KeyEvent::KEY_r:
			loadShaders();
		break;
		default:
		break;
	}
}

void MotionBlurVelocityBufferApp::update()
{
	if( ! mPaused )
	{
		for( auto &mesh : mMeshes ) {
			mesh->update( mAnimationSpeed / 60.0f );
		}
	}
}

void MotionBlurVelocityBufferApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp( getWindowSize(), 60.0f, 1.0f, 5000.0f );

	gl::draw( mBackground, getWindowBounds() );

	gl::enableDepthRead();
	gl::enableDepthWrite();

	{ // draw into color buffer
		gl::ScopedFramebuffer fbo( mColorBuffer );
		gl::ScopedAlphaBlend blend( false );
		gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 0.0f ) );

		for( auto &mesh : mMeshes )
		{
			gl::ScopedModelMatrix model;
			gl::ScopedColor meshColor( mesh->getColor() );
			gl::multModelMatrix( mesh->getTransform() );
			gl::draw( mesh->getMesh() );
		}

	}
	{ // draw into velocity buffer
		gl::ScopedFramebuffer fbo( mVelocityBuffer );
		gl::ScopedGlslProg prog( mVelocityProg );
		gl::clear( Color::black() );
		mVelocityProg->uniform( "uViewProjection", gl::getProjectionMatrix() * gl::getViewMatrix() );

		for( auto &mesh : mMeshes )
		{
			mVelocityProg->uniform( "uModelMatrix", mesh->getTransform() );
			mVelocityProg->uniform( "uPrevModelMatrix", mesh->getPreviousTransform() );
			gl::draw( mesh->getMesh() );
		}
	}
	{ // render velocity reconstruction buffers (dilation)
		gl::ScopedViewport viewport( ivec2( 0, 0 ), mTileMaxBuffer->getSize() );
		gl::ScopedMatrices	matrices;
		gl::setMatricesWindowPersp( mTileMaxBuffer->getSize() );

		{ // downsample velocity into tilemax
			gl::ScopedTextureBind tex( mVelocityBuffer->getColorTexture(), (std::uint8_t)0 );
			gl::ScopedGlslProg prog( mTileProg );
			gl::ScopedFramebuffer fbo( mTileMaxBuffer );

			mTileProg->uniform( "uVelocityMap", 0 );
			mTileProg->uniform( "uTileSize", mTileSize );

			gl::clear( Color::black() );
			gl::drawSolidRect( mTileMaxBuffer->getBounds() );
		}
		{ // find max neighbors within tilemax
			gl::ScopedTextureBind tex( mTileMaxBuffer->getColorTexture(), (std::uint8_t)0 );
			gl::ScopedGlslProg prog( mNeighborProg );
			gl::ScopedFramebuffer fbo( mNeighborMaxBuffer );

			mNeighborProg->uniform( "uTileMap", 0 );

			gl::clear( Color::white() );
			gl::drawSolidRect( mNeighborMaxBuffer->getBounds() );
		}
	}

	gl::disableDepthRead();
	gl::disableDepthWrite();
	if( ! mBlurEnabled )
	{ // draw to screen
		gl::ScopedAlphaBlend blend( false );
		gl::draw( mColorBuffer->getColorTexture() );
	}
	else
	{ // draw to screen with motion blur
		gl::ScopedAlphaBlend blend( true );
		gl::ScopedTextureBind colorTex( mColorBuffer->getColorTexture(), (std::uint8_t)0 );
		gl::ScopedTextureBind velTex( mVelocityBuffer->getColorTexture(), (std::uint8_t)1 );
		gl::ScopedTextureBind neigborTex( mNeighborMaxBuffer->getColorTexture(), (std::uint8_t)2 );
		gl::ScopedGlslProg prog( mMotionBlurProg );
		mMotionBlurProg->uniform( "uColorMap", 0 );
		mMotionBlurProg->uniform( "uVelocityMap", 1 );
		mMotionBlurProg->uniform( "uNeighborMaxMap", 2 );
		mMotionBlurProg->uniform( "uNoiseFactor", mBlurNoise );
		mMotionBlurProg->uniform( "uSamples", mSampleCount );
		gl::drawSolidRect( getWindowBounds() );
	}

	if( mDisplayVelocityBuffers ) {
		drawVelocityBuffers();
	}

	mParams->draw();
}

void MotionBlurVelocityBufferApp::drawVelocityBuffers()
{
	gl::ScopedGlslProg prog( mVelocityRenderProg );
	gl::ScopedModelMatrix matrix;
	gl::setDefaultShaderVars();

	float width = 200.0f;
	float height = width / Rectf( mNeighborMaxBuffer->getBounds() ).getAspectRatio();
	Rectf rect( 0.0f, 0.0f, width, height );

	gl::ScopedTextureBind velTex( mVelocityBuffer->getColorTexture(), (std::uint8_t)0 );
	gl::translate( getWindowWidth() - width - 10.0f, 10.0f );
	gl::drawSolidRect( rect );

	gl::ScopedTextureBind tileTex( mTileMaxBuffer->getColorTexture(), (std::uint8_t)0 );
	gl::translate( 0.0f, height + 10.0f );
	gl::drawSolidRect( rect );

	gl::ScopedTextureBind neigborTex( mNeighborMaxBuffer->getColorTexture(), (std::uint8_t)0 );
	gl::translate( 0.0f, height + 10.0f );
	gl::drawSolidRect( rect );
}

CINDER_APP_NATIVE( MotionBlurVelocityBufferApp, RendererGl( RendererGl::Options().antiAliasing( RendererGl::AA_NONE ) ) )
