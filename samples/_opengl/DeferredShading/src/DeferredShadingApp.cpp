#include "cinder/app/App.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Ubo.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/Json.h"
#include "cinder/MayaCamUI.h"
#include "cinder/params/Params.h"

#include "Light.h"
class DeferredShadingApp : public ci::app::App
{
public:
	void						draw() override;
	void						mouseDown( ci::app::MouseEvent event ) override;
	void						mouseDrag( ci::app::MouseEvent event ) override;
	void						resize() override;
	void						setup() override;
	void						update() override;
private:
	void						drawRect( const ci::ivec2& sz );
	void						drawRect( const ci::vec2& pos, const ci::ivec2& sz );

	ci::MayaCamUI				mMayaCam;

	std::vector<Light>			mLights;
	
	ci::gl::GlslProgRef			mGlslProgDebugGbuffer;
	ci::gl::GlslProgRef			mGlslProgFxaa;
	ci::gl::GlslProgRef			mGlslProgGBuffer;
	ci::gl::GlslProgRef			mGlslProgLBuffer;
	ci::gl::GlslProgRef			mGlslProgShadowMap;
	ci::gl::GlslProgRef			mGlslProgStockColor;
	ci::gl::GlslProgRef			mGlslProgStockTexture;

	ci::gl::FboRef				mFboGBuffer;
	ci::gl::FboRef				mFboLBuffer;
	ci::gl::FboRef				mFboShadowMap;
	
	ci::gl::TextureRef			mTextureRandom;
	ci::gl::Texture2dRef		mTextureFboGBufferAlbedo;
	ci::gl::Texture2dRef		mTextureFboGBufferDepth;
	ci::gl::Texture2dRef		mTextureFboGBufferNormalDepth;
	ci::gl::Texture2dRef		mTextureFboGBufferPosition;
	ci::gl::Texture2dRef		mTextureFboLBuffer;
	ci::gl::Texture2dRef		mTextureFboShadowMap;

	ci::gl::VboMeshRef			mMeshCube;
	ci::gl::VboMeshRef			mMeshRect;
	ci::gl::VboMeshRef			mMeshSphere;

	bool						mEnabledFxaa;
	bool						mEnabledShadow;

	float						mDepthScale;
	
	ci::CameraPersp				mShadowCamera;

	float						mFloor;
	ci::vec3					mSpherePosition;

	bool						mDebugMode;
	float						mFrameRate;
	bool						mFullScreen;
	ci::params::InterfaceGlRef	mParams;
	void						screenShot();
};

#include "cinder/gl/Context.h"
#include "cinder/app/RendererGl.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

void DeferredShadingApp::draw()
{
	gl::disableAlphaBlending();

	const mat4 shadowMatrix = mShadowCamera.getProjectionMatrix() * mShadowCamera.getViewMatrix();
	vec2 winSize			= vec2( getWindowSize() );
	float e					= (float)getElapsedSeconds();
	
	auto drawSpheres = [ & ]()
	{
		{
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( mSpherePosition );
			gl::draw( mMeshSphere );
		}

		size_t numSpheres	= 4;
		float t				= e * 0.165f;
		float d				= ( (float)M_PI * 2.0f ) / (float)numSpheres;
		float r				= 4.5f;
		for ( size_t i = 0; i < numSpheres; ++i, t += d ) {
			float x			= glm::cos( t );
			float z			= glm::sin( t );
			vec3 p			= vec3( x, 0.0f, z ) * r;
			p.y				= mFloor + 0.5f;

			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( p );
			gl::scale( vec3( 0.5f ) );
			gl::draw( mMeshSphere );
		}
	};
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// G-BUFFER
	
	{
		// Bind the G-buffer FBO and draw to all attachments
		gl::ScopedFramebuffer scopedFrameBuffer( mFboGBuffer );
		{
			const static GLenum buffers[] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3
			};
			gl::drawBuffers( 4, buffers );
		}
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboGBuffer->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::enableDepthRead( true );
		gl::enableDepthWrite( true );
		gl::clear();
		gl::setMatrices( mMayaCam.getCamera() );

		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgGBuffer );
			mGlslProgGBuffer->uniform( "uDepthScale",	mDepthScale );
			mGlslProgGBuffer->uniform( "uEmissive",		0.0f );

			// Draw shadow casters (spheres)
			drawSpheres();
	
			// Draw floor
			gl::ScopedModelMatrix scopedModelMatrix;
			{
				gl::ScopedModelMatrix scopedModelMatrix;
				gl::translate( vec3( 0.0f, mFloor, 0.0f ) );
				gl::rotate( quat( vec3( 4.71f, 0.0f, 0.0f ) ) );
				gl::scale( vec3( 125.0f ) );
				gl::draw( mMeshRect );
			}
			
			// Draw light sources
			mGlslProgGBuffer->uniform( "uEmissive", 1.0f );
			for ( const Light& light : mLights ) {
				gl::ScopedModelMatrix scopedModelMatrix;
				gl::translate( light.getPosition() );
				gl::scale( vec3( light.getRadius() ) );
				gl::draw( mMeshSphere );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// SHADOW MAP

	// Draw shadow casters into FBO from view of shadow camera
	if ( mEnabledShadow ) {
		gl::ScopedFramebuffer scopedFrameBuffer( mFboShadowMap );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboShadowMap->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::ScopedFaceCulling scopedFaceCulling( true, GL_FRONT );
		gl::ScopedFrontFace scopedFrontFace( GL_CW );
		gl::enableDepthRead( true );
		gl::enableDepthWrite( true );
		gl::clear();
		gl::setMatrices( mShadowCamera );
		gl::ScopedGlslProg scopedGlslProg( mGlslProgShadowMap );
		drawSpheres();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// L-BUFFER

	{
		// Set up window and clear buffers
		gl::ScopedFramebuffer scopedFrameBuffer( mFboLBuffer );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboLBuffer->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::ScopedAdditiveBlend scopedAdditiveBlend;
		gl::ScopedState scopedState( GL_DEPTH_TEST, false );
		gl::ScopedFaceCulling scopedFaceCulling( true, GL_FRONT );
		gl::clear();
		gl::setMatrices( mMayaCam.getCamera() );
	
		// Bind G-buffer textures and shadow map
		gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBufferAlbedo,			0 );
		gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBufferNormalDepth,	1 );
		gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBufferPosition,		2 );
		gl::ScopedTextureBind scopedTextureBind3( mTextureFboShadowMap,				3 );

		// Draw light volumes
		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgLBuffer );
			mGlslProgLBuffer->uniform( "uSamplerAlbedo",			0 );
			mGlslProgLBuffer->uniform( "uSamplerNormalDepth",		1 );
			mGlslProgLBuffer->uniform( "uSamplerPosition",			2 );
			mGlslProgLBuffer->uniform( "uSamplerShadowMap",			3 );

			mGlslProgLBuffer->uniform( "uShadowBlurSize",			0.0025f );
			mGlslProgLBuffer->uniform( "uShadowEnabled",			mEnabledShadow );
			mGlslProgLBuffer->uniform( "uShadowMatrix",				shadowMatrix );
			mGlslProgLBuffer->uniform( "uShadowMix",				0.5f );
			mGlslProgLBuffer->uniform( "uShadowSamples",			4.0f );
			mGlslProgLBuffer->uniform( "uViewMatrixInverse",		mMayaCam.getCamera().getInverseViewMatrix() );
			mGlslProgLBuffer->uniformBlock( 0, 0 );

			for ( const Light& light : mLights ) {
				mGlslProgLBuffer->uniform( "uLightColorAmbient",	light.getColorAmbient() );
				mGlslProgLBuffer->uniform( "uLightColorDiffuse",	light.getColorDiffuse() );
				mGlslProgLBuffer->uniform( "uLightColorSpecular",	light.getColorSpecular() );
				mGlslProgLBuffer->uniform( "uLightPosition",
										  vec3( ( mMayaCam.getCamera().getViewMatrix() * vec4( light.getPosition(), 1.0 ) ) ) );
				mGlslProgLBuffer->uniform( "uLightIntensity",		light.getIntensity() );
				mGlslProgLBuffer->uniform( "uLightRadius",			light.getVolume() );
				mGlslProgLBuffer->uniform( "uWindowSize",			vec2( getWindowSize() ) );

				gl::ScopedModelMatrix scopedModelMatrix;
				gl::translate( light.getPosition() );
				gl::scale( vec3( light.getVolume() ) );
				gl::draw( mMeshCube );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// FINAL RENDER

	// Set up window for screen render
	gl::ScopedViewport scopedViewport( ivec2( 0 ), getWindowSize() );
	gl::ScopedMatrices scopedMatrices;
	gl::disableDepthRead();
	gl::disableDepthWrite();

	if ( mDebugMode ) {
		gl::clear( Colorf::gray( 0.4f ) );
		gl::setMatricesWindow( getWindowSize() );
		
		vec2 sz;
		vec2 pos	= vec2( 0.0f );
		float w		= (float)getWindowWidth();
		sz.x		= w / 4.0f;
		sz.y		= sz.x / getWindowAspectRatio();
		pos			= sz * 0.5f;

		auto moveCursor = [ &pos, &sz, &w ]()
		{
			pos.x += sz.x;
			if ( pos.x >= w ) {
				pos.x = sz.x * 0.5f;
				pos.y += sz.y;
			}
		};
	
		// G-buffer
		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgDebugGbuffer );
			mGlslProgDebugGbuffer->uniform( "uSamplerAlbedo",		0 );
			mGlslProgDebugGbuffer->uniform( "uSamplerNormalDepth",	1 );
			mGlslProgDebugGbuffer->uniform( "uSamplerPosition",		2 );
			gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBufferAlbedo,			0 );
			gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBufferNormalDepth,	1 );
			gl::ScopedTextureBind scopedTextureBind3( mTextureFboGBufferPosition,		2 );
		
			for ( int32_t i = 0; i < 4; ++i ) {
				if ( i > 0 ) {
					moveCursor();
				}
				mGlslProgDebugGbuffer->uniform( "uMode", i );
				drawRect( pos, sz );
			}
		}
		
		// L-buffer
		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgStockTexture );
			moveCursor();
			mTextureFboLBuffer->setTopDown( true );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboLBuffer, 0 );
				drawRect( pos, sz );
			}
		}
	} else {
		gl::clear();
		gl::setMatricesWindow( getWindowSize() );

		// Perform FXAA
		if ( mEnabledFxaa ) {
			gl::ScopedGlslProg scopedGlslProg( mGlslProgFxaa );
			gl::ScopedTextureBind scopedTextureBind( mTextureFboLBuffer, 0 );
			mGlslProgFxaa->uniform( "uPixel",	vec2( 1.0f ) / winSize );
			mGlslProgFxaa->uniform( "uSampler",	0 );
			drawRect( getWindowSize() );
		} else {
			gl::draw( mTextureFboLBuffer );
		}
	}

	mParams->draw();
}

void DeferredShadingApp::drawRect( const ivec2& sz )
{
	vec2 szf( sz );
	gl::ScopedModelMatrix scopedModelMatrix;
	gl::translate( szf * 0.5f );
	gl::scale( szf );
	gl::draw( mMeshRect );
};

void DeferredShadingApp::drawRect( const vec2& pos, const ivec2& sz )
{
	gl::ScopedModelMatrix scopedModelMatrix;
	gl::translate( pos );
	gl::scale( sz );
	gl::draw( mMeshRect );
}

void DeferredShadingApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void DeferredShadingApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void DeferredShadingApp::resize()
{
	CameraPersp camera = mMayaCam.getCamera();
	camera.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( camera );

	gl::disable( GL_CULL_FACE );
	gl::enable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	gl::clear();
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	auto clearFbo = []( gl::FboRef& fbo ) -> void {
		gl::ScopedFramebuffer fboScope( fbo );
		gl::viewport( fbo->getSize() );
		gl::clear();
	};

	auto createRenderbufferFromTexture = 
		[]( const gl::Texture2dRef& tex, size_t samples, size_t coverageSamples ) -> gl::RenderbufferRef
	{
		return gl::Renderbuffer::create( tex->getWidth(), tex->getHeight(), tex->getInternalFormat(), (int32_t)samples, (int32_t)coverageSamples );
	};
	
	const ivec2 windowSize		= getWindowSize();

	// Geometry buffer
	{
		gl::Texture2d::Format textureFormat10;
		textureFormat10.setInternalFormat( GL_RGB10_A2 );
		textureFormat10.setMagFilter( GL_NEAREST );
		textureFormat10.setMinFilter( GL_NEAREST );
		textureFormat10.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		textureFormat10.setDataType( GL_FLOAT );
		
		gl::Texture2d::Format textureFormat16;
		textureFormat16.setInternalFormat( GL_RGBA16F );
		textureFormat16.setMagFilter( GL_NEAREST );
		textureFormat16.setMinFilter( GL_NEAREST );
		textureFormat16.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		textureFormat16.setDataType( GL_FLOAT );

		mTextureFboGBufferAlbedo				= gl::Texture2d::create( windowSize.x, windowSize.y, textureFormat10 );
		mTextureFboGBufferNormalDepth			= gl::Texture2d::create( windowSize.x, windowSize.y, textureFormat16 );
		mTextureFboGBufferPosition				= gl::Texture2d::create( windowSize.x, windowSize.y, textureFormat16 );

		gl::RenderbufferRef albedoBuffer		= createRenderbufferFromTexture( mTextureFboGBufferAlbedo,		0, 0 );
		gl::RenderbufferRef normalDepthBuffer	= createRenderbufferFromTexture( mTextureFboGBufferNormalDepth, 0, 0 );
		gl::RenderbufferRef positionBuffer		= createRenderbufferFromTexture( mTextureFboGBufferPosition,	0, 0 );

		gl::Texture2d::Format depthFormat;
		depthFormat.setInternalFormat( GL_DEPTH_COMPONENT32F );
		depthFormat.setMagFilter( GL_NEAREST );
		depthFormat.setMinFilter( GL_NEAREST );
		depthFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		depthFormat.setDataType( GL_FLOAT );

		mTextureFboGBufferDepth			= gl::Texture2d::create( windowSize.x, windowSize.y, depthFormat );
		gl::RenderbufferRef depthBuffer	= createRenderbufferFromTexture( mTextureFboGBufferDepth, 0, 0 );

		gl::Fbo::Format fboFormat;
		fboFormat.attachment( GL_COLOR_ATTACHMENT0, mTextureFboGBufferAlbedo,		albedoBuffer );
		fboFormat.attachment( GL_COLOR_ATTACHMENT1, mTextureFboGBufferNormalDepth,	normalDepthBuffer );
		fboFormat.attachment( GL_COLOR_ATTACHMENT2, mTextureFboGBufferPosition,		positionBuffer );
		fboFormat.attachment( GL_DEPTH_ATTACHMENT,	mTextureFboGBufferDepth,		depthBuffer );
		try {
			mFboGBuffer = gl::Fbo::create( windowSize.x, windowSize.y, fboFormat );
			clearFbo( mFboGBuffer );
		} catch ( gl::FboExceptionInvalidSpecification ex ) {
			console() << "Failed to create G-buffer: " << ex.what() << endl;
			quit();
		}
	}

	// Light buffer
	{
		gl::Texture2d::Format textureFormat;
		textureFormat.setInternalFormat( GL_RGB10_A2 );
		textureFormat.setMagFilter( GL_NEAREST );
		textureFormat.setMinFilter( GL_NEAREST );
		textureFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		textureFormat.setDataType( GL_FLOAT );

		mTextureFboLBuffer				= gl::Texture2d::create( windowSize.x, windowSize.y, textureFormat );
		gl::RenderbufferRef lightBuffer = createRenderbufferFromTexture( mTextureFboLBuffer, 0, 0 );

		gl::Fbo::Format fboFormat;
		fboFormat.attachment( GL_COLOR_ATTACHMENT0, mTextureFboLBuffer, lightBuffer );
		mFboLBuffer = gl::Fbo::create( windowSize.x, windowSize.y, fboFormat );

		clearFbo( mFboLBuffer );
	}

	// Shadow buffer
	{
		uint32_t sz = 1024;
		gl::Texture2d::Format depthFormat;
		depthFormat.setInternalFormat( GL_DEPTH_COMPONENT32F );
		depthFormat.setMagFilter( GL_LINEAR );
		depthFormat.setMinFilter( GL_LINEAR );
		depthFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		depthFormat.setDataType( GL_FLOAT );
		mTextureFboShadowMap = gl::Texture2d::create( sz, sz, depthFormat );
		{
			gl::ScopedTextureBind scopeTextureBind( mTextureFboShadowMap );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
		}
		
		gl::Fbo::Format fboFormat;
		fboFormat.attachment( GL_DEPTH_ATTACHMENT, mTextureFboShadowMap );
		mFboShadowMap = gl::Fbo::create( sz, sz, fboFormat );
	}

	// Set up shadow camera
	mShadowCamera.setPerspective( 150.0f, mFboShadowMap->getAspectRatio(), mMayaCam.getCamera().getNearClip(), mMayaCam.getCamera().getFarClip() );
	mShadowCamera.lookAt( mLights.at( 0 ).getPosition(), vec3( 0.0f, mFloor, 0.0f ) );
}

void DeferredShadingApp::screenShot()
{
	writeImage( getAppPath() / fs::path( "frame" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
}

void DeferredShadingApp::setup()
{
	gl::enableVerticalSync();
	
	// Shortcut for shader loading and error handling
	auto loadGlslProg = [ & ]( const string& name, DataSourceRef vertex, DataSourceRef fragment ) -> gl::GlslProgRef
	{
		gl::GlslProgRef glslProg;
		try {
			glslProg = gl::GlslProg::create( vertex, fragment );
		} catch ( gl::GlslProgCompileExc ex ) {
			console() << name << ": GLSL Error: " << ex.what() << endl;
			quit();
		} catch ( gl::GlslNullProgramExc ex ) {
			console() << name << ": GLSL Error: " << ex.what() << endl;
			quit();
		} catch ( ... ) {
			console() << name << ": Unknown GLSL Error" << endl;
			quit();
		}
		return glslProg;
	};

	// Load shaders
	DataSourceRef gBufferVert	= loadAsset( "gbuffer_vert.glsl" );
	DataSourceRef passThrough	= loadAsset( "passThrough_vert.glsl" );
	mGlslProgDebugGbuffer		= loadGlslProg( "Debug G-Buffer",	passThrough,							loadAsset( "debug_gbuffer_frag.glsl" ) );
	mGlslProgFxaa				= loadGlslProg( "FXAA",				passThrough,							loadAsset( "fxaa_frag.glsl" ) );
	mGlslProgGBuffer			= loadGlslProg( "G-buffer",			gBufferVert,							loadAsset( "gbuffer_frag.glsl" ) );
	mGlslProgLBuffer			= loadGlslProg( "L-buffer",			passThrough,							loadAsset( "lbuffer_frag.glsl" ) );
	mGlslProgShadowMap			= loadGlslProg( "Shadow map",		loadAsset( "shadow_map_vert.glsl" ),	loadAsset( "shadow_map_frag.glsl" ) );
	mGlslProgStockColor			= gl::context()->getStockShader( gl::ShaderDef().color() );
	mGlslProgStockTexture		= gl::context()->getStockShader( gl::ShaderDef().texture( GL_TEXTURE_2D ) );
	
	// Set default values for all properties
	mDepthScale		= 0.01f;
	mDebugMode		= false;
	mEnabledFxaa	= true;
	mEnabledShadow	= true;
	mFloor			= -7.0f;
	mFrameRate		= 0.0f;
	mFullScreen		= isFullScreen();
	mMeshCube		= gl::VboMesh::create( geom::Cube() );
	mMeshRect		= gl::VboMesh::create( geom::Rect() );
	mMeshSphere		= gl::VboMesh::create( geom::Sphere().subdivisions( 64 ) );
	mSpherePosition	= vec3( 0.0f, -4.0f, 0.0f );
	mTextureRandom	= gl::Texture::create( loadImage( loadAsset( "random.png" ) ) );

	// Set up lights
	mLights.push_back( Light()
					  .setColorDiffuse( ColorAf( 0.95f, 1.0f, 0.92f, 1.0f ) )
					  .setIntensity( 0.5f )
					  .setPosition( vec3( 0.0f, 0.0f, 0.0f ) )
					  .setRadius( 0.125f )
					  .setVolume( 15.0f ) );
	for ( size_t i = 0; i < 8; ++i ) {
		mLights.push_back( Light()
						  .setColorDiffuse( ColorAf( 0.95f, 1.0f, 0.92f, 1.0f ) )
						  .setIntensity( 0.6f )
						  .setRadius( 0.1f )
						  .setVolume( 5.0f ) );
	}

	// Set up camera
	ivec2 windowSize = toPixels( getWindowSize() );
	CameraPersp cam( windowSize.x, windowSize.y, 45.0f, 1.0f, 100.0f );
	cam.setEyePoint( vec3( -2.221f, -4.083f, 15.859f ) );
	cam.setCenterOfInterestPoint( vec3( -0.635f, -4.266f, 1.565f ) );
	mMayaCam.setCurrentCam( cam );

	// Set up parameters
	mParams = params::InterfaceGl::create( "Params", ivec2( 220, 220 ) );
	mParams->addParam( "Frame rate",		&mFrameRate,				"", true );
	mParams->addParam( "Debug mode",		&mDebugMode ).key( "d" );
	mParams->addParam( "Fullscreen",		&mFullScreen ).key( "f" );
	mParams->addButton( "Screen shot",		[ & ]() { screenShot(); },	"key=space" );
	mParams->addButton( "Quit",				[ & ]() { quit(); },		"key=q" );
	mParams->addSeparator();
	mParams->addParam( "FXAA",				&mEnabledFxaa );
	mParams->addParam( "Shadows",			&mEnabledShadow );

	// Call resize to create FBOs
	resize();
}

void DeferredShadingApp::update()
{
	float e		= (float)getElapsedSeconds();
	mFrameRate	= getAverageFps();

	if ( mFullScreen != isFullScreen() ) {
		setFullScreen( mFullScreen );
	}
	
	// Update light positions
	if ( !mLights.empty() ) {
		size_t numLights	= mLights.size() - 1;
		float t				= e;
		float d				= ( (float)M_PI * 2.0f ) / (float)numLights;
		float r				= 3.5f;
		for ( size_t i = 0; i < numLights; ++i, t += d ) {
			float ground	= mFloor + 0.1f;
			float x			= glm::cos( t );
			float z			= glm::sin( t );
			vec3 p			= vec3( x, 0.0f, z ) * r;
			p.y				= ground + 1.0f;
			mLights.at( i + 1 ).setPosition( p );
		}
	}
}

CINDER_APP( DeferredShadingApp, RendererGl( RendererGl::Options().msaa( 0 ).coreProfile( true ).version( 4, 0 ) ), []( App::Settings* settings )
{
	settings->disableFrameRate();
	settings->setWindowSize( 1280, 720 );
} )
 