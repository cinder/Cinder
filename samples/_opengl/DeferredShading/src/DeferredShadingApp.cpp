/* 
 * Deferred Shading sample application by:
 * Stephen Schieberl - Wieden+Kennedy
 * Michael Latzoni - Wieden+Kennedy
 * 
 * This sample demonstrates how to do basic deferred shading.
 * The scene is rendered into a frame buffer with multiple attachments
 * (G-buffer). Shadow casters are rendered into a shadow map FBO. 
 * The data from each is read while drawing light volumes into the 
 * light buffer (L-buffer). Finally, the L-buffer is drawn to the screen.
 */

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/params/Params.h"
#include "cinder/app/RendererGl.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"

#include "Light.h"

class DeferredShadingApp : public ci::app::App
{
  public:
	DeferredShadingApp();

	void						draw() override;
	void						resize() override;
	void						update() override;
	
  private:
	ci::CameraPersp				mCamera;
	ci::CameraUi				mCamUi;

	std::vector<Light>			mLights;
	
	void						loadShaders();
	ci::gl::BatchRef			mBatchDebugRect;
	ci::gl::BatchRef			mBatchFxaaRect;
	ci::gl::BatchRef			mBatchGBufferRect;
	ci::gl::BatchRef			mBatchGBufferSphere;
	ci::gl::BatchRef			mBatchLBufferCube;
	ci::gl::BatchRef			mBatchShadowSphere;
	ci::gl::BatchRef			mBatchStockColorRect;
	ci::gl::BatchRef			mBatchStockColorSphere;
	ci::gl::BatchRef			mBatchStockTextureRect;

	ci::gl::FboRef				mFbo;
	ci::gl::FboRef				mFboShadowMap;
	
	ci::gl::Texture2dRef		mTextureFbo[ 4 ];
	ci::gl::Texture2dRef		mTextureFboShadowMap;
	
	bool						mEnabledFxaa;
	bool						mEnabledShadow;

	ci::CameraPersp				mShadowCamera;

	float						mFloor;

	bool						mDebugMode;
	float						mFrameRate;
	bool						mFullScreen;
	ci::params::InterfaceGlRef	mParams;
	bool						mQuit;
	void						screenShot();
};

using namespace ci;
using namespace ci::app;
using namespace std;

DeferredShadingApp::DeferredShadingApp()
{
	gl::enableVerticalSync();
	
	// Set default values for all properties
	mDebugMode		= false;
	mEnabledFxaa	= true;
	mEnabledShadow	= true;
	mFloor			= -7.0f;
	mFrameRate		= 0.0f;
	mFullScreen		= isFullScreen();
	mQuit			= false;

	// Set up lights
	mLights.push_back( Light().colorDiffuse( ColorAf( 0.95f, 1.0f, 0.92f, 1.0f ) )
					  .intensity( 0.8f ).position( vec3( 0.0f, 0.0f, 0.0f ) )
					  .radius( 0.1f ).volume( 15.0f ) );
	for ( size_t i = 0; i < 8; ++i ) {
		mLights.push_back( Light().colorDiffuse( ColorAf( 0.95f, 1.0f, 0.92f, 1.0f ) )
						  .intensity( 0.6f ).radius( 0.05f ).volume( 5.0f ) );
	}

	// Set up camera
	ivec2 windowSize = toPixels( getWindowSize() );
	mCamera = CameraPersp( windowSize.x, windowSize.y, 45.0f, 1.0f, 100.0f );
	mCamera.lookAt( vec3( -2.221f, -4.083f, 15.859f ), vec3( -0.635f, -4.266f, 1.565f ) );
	mCamUi = CameraUi( &mCamera, getWindow(), -1 );

	// Set up parameters
	mParams = params::InterfaceGl::create( "Params", ivec2( 220, 220 ) );
	mParams->addParam( "Frame rate",	&mFrameRate,				"", true );
	mParams->addParam( "Debug mode",	&mDebugMode ).key( "d" );
	mParams->addParam( "Fullscreen",	&mFullScreen ).key( "f" );
	mParams->addButton( "Screen shot",	[ & ]() { screenShot(); },	"key=space" );
	mParams->addParam( "Quit",			&mQuit ).key( "q" );
	mParams->addSeparator();
	mParams->addParam( "FXAA",			&mEnabledFxaa ).key( "a" );
	mParams->addParam( "Shadows",		&mEnabledShadow ).key( "s" );

	// Create shadow map buffer
	size_t sz = 1024;
	mTextureFboShadowMap = gl::Texture2d::create( sz, sz, gl::Texture2d::Format()
												 .internalFormat( GL_DEPTH_COMPONENT32F )
												 .magFilter( GL_LINEAR )
												 .minFilter( GL_LINEAR )
												 .wrap( GL_CLAMP_TO_EDGE )
												 .dataType( GL_FLOAT ) );
	gl::ScopedTextureBind scopeTextureBind( mTextureFboShadowMap );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
	gl::Fbo::Format fboFormat;
	fboFormat.attachment( GL_DEPTH_ATTACHMENT, mTextureFboShadowMap );
	mFboShadowMap = gl::Fbo::create( sz, sz, fboFormat );
	
	// Set up shadow camera
	mShadowCamera.setPerspective( 120.0f, mFboShadowMap->getAspectRatio(), 
								  mCamera.getNearClip(), mCamera.getFarClip() );
	mShadowCamera.lookAt( vec3( 0.0 ), vec3( 0.0f, mFloor, 0.0f ) );

	// Load shaders and create batches
	loadShaders();

	// Call resize to create FBOs
	resize();
}

void DeferredShadingApp::draw()
{
	gl::disableAlphaBlending();

	const mat4 shadowMatrix = mShadowCamera.getProjectionMatrix() * mShadowCamera.getViewMatrix();
	vec2 winSize			= vec2( getWindowSize() );
	float e					= (float)getElapsedSeconds();

	vector<mat4> spheres;
	{
		mat4 m( 1.0f );
		m = glm::translate( m, vec3( 0.0f, -4.0f, 0.0f ) );
		spheres.push_back( m );
	}
	float t		= e * 0.165f;
	float d		= ( (float)M_PI * 2.0f ) / 4.0f;
	float r		= 4.5f;
	for ( size_t i = 0; i < 4; ++i, t += d ) {
		float x	= glm::cos( t );
		float z	= glm::sin( t );
		vec3 p	= vec3( x, 0.0f, z ) * r;
		p.y		= mFloor + 0.5f;

		mat4 m( 1.0f );
		m = glm::translate( m, p );
		m = glm::rotate( m, e, vec3( 1.0f ) );
		m = glm::scale( m, vec3( 0.5f ) );
		
		spheres.push_back( m );
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
		for ( const mat4& m : spheres ) {
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::multModelMatrix( m );
			mBatchShadowSphere->draw();
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// G-BUFFER
	
	{
		// Bind the G-buffer FBO and draw to all attachments
		gl::ScopedFramebuffer scopedFrameBuffer( mFbo );
		{
			const static GLenum buffers[] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2
			};
			gl::drawBuffers( 3, buffers );
		}
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFbo->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::enableDepthRead();
		gl::enableDepthWrite( true );
		gl::clear();
		gl::setMatrices( mCamera );

		// Draw floor
		mBatchGBufferRect->getGlslProg()->uniform( "uEmissive", 0.0f );
		{
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( vec3( 0.0f, mFloor, 0.0f ) );
			gl::rotate( quat( vec3( 4.71f, 0.0f, 0.0f ) ) );
			gl::scale( vec3( 20.0f ) );
			mBatchGBufferRect->draw();
		}

		// Draw shadow casters
		mBatchGBufferSphere->getGlslProg()->uniform( "uEmissive", 0.0f );
		for ( const mat4& m : spheres ) {
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::multModelMatrix( m );
			mBatchGBufferSphere->draw();
		}
			
		// Draw light sources
		mBatchGBufferSphere->getGlslProg()->uniform( "uEmissive", 1.0f );
		for ( const Light& light : mLights ) {
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( light.getPosition() );
			gl::scale( vec3( light.getRadius() ) );
			mBatchGBufferSphere->draw();
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// L-BUFFER

	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFbo );
		gl::drawBuffer( GL_COLOR_ATTACHMENT3 );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFbo->getSize() );
		gl::clear();
		gl::ScopedMatrices scopedMatrices;
		gl::ScopedAdditiveBlend scopedAdditiveBlend;
		gl::ScopedFaceCulling scopedFaceCulling( true, GL_FRONT );
		gl::setMatrices( mCamera );
		gl::enableDepthRead();
		gl::disableDepthWrite();
	
		// Bind G-buffer textures and shadow map
		gl::ScopedTextureBind scopedTextureBind0( mTextureFbo[ 0 ],		0 );
		gl::ScopedTextureBind scopedTextureBind1( mTextureFbo[ 1 ],		1 );
		gl::ScopedTextureBind scopedTextureBind2( mTextureFbo[ 2 ],		2 );
		gl::ScopedTextureBind scopedTextureBind3( mTextureFboShadowMap,	3 );

		// Draw light volumes
		mBatchLBufferCube->getGlslProg()->uniform( "uShadowEnabled",			mEnabledShadow );
		mBatchLBufferCube->getGlslProg()->uniform( "uShadowMatrix",				shadowMatrix );
		mBatchLBufferCube->getGlslProg()->uniform( "uViewMatrixInverse",		mCamera.getInverseViewMatrix() );
		
		for ( const Light& light : mLights ) {
			mBatchLBufferCube->getGlslProg()->uniform( "uLightColorAmbient",	light.getColorAmbient() );
			mBatchLBufferCube->getGlslProg()->uniform( "uLightColorDiffuse",	light.getColorDiffuse() );
			mBatchLBufferCube->getGlslProg()->uniform( "uLightColorSpecular",	light.getColorSpecular() );
			mBatchLBufferCube->getGlslProg()->uniform( "uLightPosition", 
										vec3( ( mCamera.getViewMatrix() * vec4( light.getPosition(), 1.0 ) ) ) );
			mBatchLBufferCube->getGlslProg()->uniform( "uLightIntensity",		light.getIntensity() );
			mBatchLBufferCube->getGlslProg()->uniform( "uLightRadius",			light.getVolume() );
			mBatchLBufferCube->getGlslProg()->uniform( "uWindowSize",			vec2( getWindowSize() ) );

			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( light.getPosition() );
			gl::scale( vec3( light.getVolume() ) );
			mBatchLBufferCube->draw();
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// FINAL RENDER

	// Set up window for screen render
	gl::ScopedViewport scopedViewport( ivec2( 0 ), getWindowSize() );
	gl::ScopedMatrices scopedMatrices;
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::clear();

	if ( mDebugMode ) {

		// Draw G-buffer
		gl::clear( Colorf::gray( 0.4f ) );
		gl::setMatricesWindow( getWindowSize() );
		gl::ScopedTextureBind scopedTextureBind0( mTextureFbo[ 0 ],	0 );
		gl::ScopedTextureBind scopedTextureBind1( mTextureFbo[ 1 ],	1 );
		gl::ScopedTextureBind scopedTextureBind2( mTextureFbo[ 2 ],	2 );
		
		// Albedo   | Normals
		// --------------------
		// Position | Emissive
		vec2 sz = getWindowCenter();
		for ( int32_t i = 0; i < 4; ++i ) {
			vec2 pos( ( i % 2 ) * sz.x, glm::floor( (float)i * 0.5f ) * sz.y );
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( pos + sz * 0.5f );
			gl::scale( sz );
			mBatchDebugRect->getGlslProg()->uniform( "uMode", i );
			mBatchDebugRect->draw();
		}

	} else {
		gl::translate( getWindowCenter() );
		gl::scale( getWindowSize() );

		gl::ScopedTextureBind scopedTextureBind( mTextureFbo[ 3 ], 0 );
		if ( mEnabledFxaa ) {

			// Perform FXAA
			mBatchFxaaRect->getGlslProg()->uniform( "uPixel", vec2( 1.0f ) / winSize );
			mBatchFxaaRect->draw();
		} else {

			// Draw without anti-aliasing
			mBatchStockTextureRect->draw();
		}
	}

	mParams->draw();
}

void DeferredShadingApp::loadShaders()
{
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

	gl::VboMeshRef cube		= gl::VboMesh::create( geom::Cube() );
	gl::VboMeshRef rect		= gl::VboMesh::create( geom::Rect() );
	gl::VboMeshRef sphere	= gl::VboMesh::create( geom::Sphere().subdivisions( 64 ) );

	// Load shaders
	DataSourceRef passThrough		= loadAsset( "pass_through.vert" );
	gl::GlslProgRef debug			= loadGlslProg( "Debug",		passThrough,					loadAsset( "debug.frag" ) );
	gl::GlslProgRef fxaa			= loadGlslProg( "FXAA",			passThrough,					loadAsset( "fxaa.frag" ) );
	gl::GlslProgRef gBuffer			= loadGlslProg( "G-buffer",		loadAsset( "gbuffer.vert" ),	loadAsset( "gbuffer.frag" ) );
	gl::GlslProgRef lBuffer			= loadGlslProg( "L-buffer",		passThrough,					loadAsset( "lbuffer.frag" ) );
	gl::GlslProgRef shadowMap		= loadGlslProg( "Shadow map",	loadAsset( "shadow_map.vert" ),	loadAsset( "shadow_map.frag" ) );
	gl::GlslProgRef stockColor		= gl::context()->getStockShader( gl::ShaderDef().color() );
	gl::GlslProgRef stockTexture	= gl::context()->getStockShader( gl::ShaderDef().texture( GL_TEXTURE_2D ) );
	
	mBatchDebugRect			= gl::Batch::create( rect,		debug );
	mBatchFxaaRect			= gl::Batch::create( rect,		fxaa );
	mBatchGBufferRect		= gl::Batch::create( rect,		gBuffer );
	mBatchGBufferSphere		= gl::Batch::create( sphere,	gBuffer );
	mBatchLBufferCube		= gl::Batch::create( cube,		lBuffer );
	mBatchShadowSphere		= gl::Batch::create( sphere,	shadowMap );
	mBatchStockColorRect	= gl::Batch::create( rect,		stockColor );
	mBatchStockColorSphere	= gl::Batch::create( rect,		stockColor );
	mBatchStockTextureRect	= gl::Batch::create( rect,		stockTexture );

	mBatchDebugRect->getGlslProg()->uniform(	"uSamplerAlbedo",			0 );
	mBatchDebugRect->getGlslProg()->uniform(	"uSamplerNormalEmissive",	1 );
	mBatchDebugRect->getGlslProg()->uniform(	"uSamplerPosition",			2 );
	mBatchFxaaRect->getGlslProg()->uniform(		"uSampler",					0 );
	mBatchLBufferCube->getGlslProg()->uniform(	"uSamplerAlbedo",			0 );
	mBatchLBufferCube->getGlslProg()->uniform(	"uSamplerNormalEmissive",	1 );
	mBatchLBufferCube->getGlslProg()->uniform(	"uSamplerPosition",			2 );
	mBatchLBufferCube->getGlslProg()->uniform(	"uSamplerShadowMap",		3 );
}

void DeferredShadingApp::resize()
{
	mCamera.setAspectRatio( getWindowAspectRatio() );

	// Texture format
	gl::Texture2d::Format textureFormat = gl::Texture2d::Format()
		.internalFormat( GL_RGBA16F )
		.magFilter( GL_NEAREST )
		.minFilter( GL_NEAREST )
		.wrap( GL_CLAMP_TO_EDGE )
		.dataType( GL_FLOAT );

	// Create FBO for the the geometry buffer (G-buffer) and light buffer
	// (L-buffer). This G-buffer uses three olor attachments to store
	// position, normal, emissive (light), and color (albedo) data. The fourth
	// attachment read the first three to render the scene inside light volumes.
	int32_t h = getWindowHeight();
	int32_t w = getWindowWidth();
	gl::Fbo::Format fboFormat;
	for ( size_t i = 0; i < 4; ++i ) {
		mTextureFbo[ i ] = gl::Texture2d::create( w, h, textureFormat );
		fboFormat.attachment( GL_COLOR_ATTACHMENT0 + i, mTextureFbo[ i ] );
	}
	mFbo = gl::Fbo::create( w, h, fboFormat );
	gl::ScopedFramebuffer scopedFramebuffer( mFbo );
	gl::ScopedViewport( ivec2( 0 ), mFbo->getSize() );
	gl::clear();
}

void DeferredShadingApp::screenShot()
{
#if defined( CINDER_MSW )
	fs::path path = getAppPath();
#else
	fs::path path = getAppPath().parent_path();
#endif
	writeImage( path / fs::path( "frame" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
}

void DeferredShadingApp::update()
{
	if ( mQuit ) {
		quit();
		return;
	}

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
			p.y				= ground + 1.5f;
			mLights.at( i + 1 ).setPosition( p );
		}
	}
}

CINDER_APP( DeferredShadingApp, RendererGl( RendererGl::Options().msaa( 0 ).coreProfile( true ).version( 3, 3 ) ), 
			[]( App::Settings* settings )
{
	settings->disableFrameRate();
	settings->setWindowSize( 1280, 720 );
} )
 