/*
 * Deferred Shading sample application by:
 * Stephen Schieberl - Wieden+Kennedy
 * Michael Latzoni - Wieden+Kennedy
 *
 * License: BSD Simplified
 *
 * Deferred shading is a technique where a 3D scene's geometry data
 * is rendered into screen space and shading is deferred until
 * a second pass when lights are drawn.
 *
 * This sample demonstrates how to create a basic deferred shading
 * pipeline. The scene is rendered into a frame buffer with multiple
 * attachments (G-buffer). Shadow casters are rendered into a shadow
 * map FBO. The data from each is read while drawing light volumes
 * into the light buffer (L-buffer). Finally, the L-buffer is drawn
 * to the screen.
 */

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#if !defined( CINDER_GL_ES )
#include "cinder/CinderImGui.h"
#endif

#include "Light.h"

class DeferredShadingApp : public ci::app::App
{
public:
	DeferredShadingApp();
	
	void						draw() override;
	void						resize() override;
	void						update() override;
	void						keyDown( ci::app::KeyEvent event ) override;
private:
	ci::CameraPersp				mCamera;
	ci::CameraUi				mCamUi;
	ci::CameraPersp				mShadowCamera;
	
	std::vector<Light>			mLights;
	
	void						loadShaders();
	ci::gl::BatchRef			mBatchDebugRect;
	ci::gl::BatchRef			mBatchFxaaRect;
	ci::gl::BatchRef			mBatchGBufferPlane;
	ci::gl::BatchRef			mBatchGBufferSphere;
	ci::gl::BatchRef			mBatchLBufferCube;
	ci::gl::BatchRef			mBatchShadowSphere;
	ci::gl::BatchRef			mBatchStockColorRect;
	ci::gl::BatchRef			mBatchStockColorSphere;
	ci::gl::BatchRef			mBatchStockTextureRect;
	
	ci::gl::FboRef				mFboGBuffer;
	ci::gl::FboRef				mFboLBuffer;
	ci::gl::FboRef				mFboShadowMap;
	
	ci::gl::Texture2dRef		mTextureFboGBuffer[ 3 ];
	
	bool						mDebugMode;
	bool						mEnabledFxaa;
	bool						mEnabledShadow;
#if !defined( CINDER_GL_ES )
	float						mFrameRate;
	bool						mFullScreen;
	void						screenShot();
#endif
};

#include "cinder/app/RendererGl.h"
#include "cinder/Log.h"
#if !defined( CINDER_GL_ES )
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#endif

using namespace ci;
using namespace ci::app;
using namespace std;

DeferredShadingApp::DeferredShadingApp()
{
	gl::enableVerticalSync();
	gl::color( ColorAf::white() );
	gl::disableAlphaBlending();
	
#if defined( CINDER_GL_ES ) && ! ( CINDER_GL_VERSION >= CINDER_GL_VERSION_3 )
	CI_LOG_V( "CINDER_GL_ES_3 must be defined in Cinder and this application when targeting OpenGL ES." );
	quit();
	return;
#endif
	
	// Set draw flags
	mDebugMode		= false;
	mEnabledFxaa	= true;
	mEnabledShadow	= true;
	
	// Set up lights
	mLights.push_back( Light().colorDiffuse( ColorAf( 0.95f, 1.0f, 0.92f, 1.0f ) )
					  .intensity( 0.8f ).position( vec3( 0.0f, 0.0f, 0.0f ) )
					  .radius( 0.1f ).volume( 15.0f ) );
	for ( size_t i = 0; i < 8; ++i ) {
		const float t = ( (float)i / 8.0f ) * 0.2f;
		mLights.push_back( Light().colorDiffuse( ColorAf( 0.75f + t * 0.5f, 0.92f - t, 0.7f + t * t, 1.0f ) )
						  .intensity( 0.6f ).radius( 0.05f ).volume( 5.0f ) );
	}
	
	// Set up camera
	const vec2 windowSize = toPixels( getWindowSize() );
	mCamera = CameraPersp( windowSize.x, windowSize.y, 45.0f, 0.01f, 100.0f );
	mCamera.lookAt( vec3( -2.221f, 2.0f, 15.859f ), vec3( 0.0f, 2.0f, 0.0f ) );
	mCamUi = CameraUi( &mCamera, getWindow(), -1 );
	
	// Create shadow map buffer
	{
		const GLsizei sz = 2048;
		mFboShadowMap = gl::Fbo::create( sz, sz, gl::Fbo::Format().depthTexture() );
		const gl::ScopedFramebuffer scopedFramebuffer( mFboShadowMap );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboShadowMap->getSize() );
		gl::clear();
		mFboShadowMap->getDepthTexture()->setCompareMode( GL_COMPARE_REF_TO_TEXTURE );
	}
	
	// Set up shadow camera
	mShadowCamera.setPerspective( 120.0f, mFboShadowMap->getAspectRatio(),
								 mCamera.getNearClip(), mCamera.getFarClip() );
	mShadowCamera.lookAt( vec3( 0.0f, 7.0f, 0.0f ), vec3( 0.0f ) );
	
	// Load shaders and create batches
	loadShaders();
	
	// Call resize to create FBOs
	resize();
	
#if !defined( CINDER_GL_ES )
	mFrameRate	= 0.0f;
	mFullScreen	= isFullScreen();
	ImGui::Initialize();
#endif
}

void DeferredShadingApp::draw()
{
	vector<mat4> spheres;
	{
		mat4 m( 1.0f );
		m = glm::translate( m, vec3( 0.0f, 4.0f, 0.0f ) );
		spheres.push_back( m );
	}
	const float e		= (float)getElapsedSeconds();
	float t				= e * 0.165f;
	const float d = ( (float)M_PI * 2.0f ) / 4.0f;
	const float r = 4.5f;
	for ( size_t i = 0; i < 4; ++i, t += d ) {
		const float x	= glm::cos( t );
		const float z	= glm::sin( t );
		vec3 p			= vec3( x, 0.0f, z ) * r;
		p.y				= 0.5f;
		
		mat4 m( 1.0f );
		m = glm::translate( m, p );
		m = glm::rotate( m, e, vec3( 1.0f ) );
		m = glm::scale( m, vec3( 0.5f ) );
		
		spheres.push_back( m );
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// G-BUFFER
	
	{
		// Bind the G-buffer FBO and draw to all attachments
		const gl::ScopedFramebuffer scopedFrameBuffer( mFboGBuffer );
		{
			const static GLenum buffers[] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2
			};
			gl::drawBuffers( 3, buffers );
		}
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboGBuffer->getSize() );
		const gl::ScopedMatrices scopedMatrices;
		gl::enableDepthRead();
		gl::enableDepthWrite();
		gl::clear();
		gl::setMatrices( mCamera );
		
		// Draw floor
		mBatchGBufferPlane->getGlslProg()->uniform( "uEmissive", 0.0f );
		mBatchGBufferPlane->draw();
		
		// Draw shadow casters
		const gl::ScopedFaceCulling scopedFaceCulling( true, GL_BACK );
		mBatchGBufferSphere->getGlslProg()->uniform( "uEmissive", 0.0f );
		for ( const mat4& m : spheres ) {
			const gl::ScopedModelMatrix scopedModelMatrix;
			gl::multModelMatrix( m );
			mBatchGBufferSphere->draw();
		}
		
		// Draw light sources
		mBatchGBufferSphere->getGlslProg()->uniform( "uEmissive", 1.0f );
		for ( const Light& light : mLights ) {
			const gl::ScopedModelMatrix scopedModelMatrix;
			const gl::ScopedColor scopedColor( light.getColorDiffuse() );
			gl::translate( light.getPosition() );
			gl::scale( vec3( light.getRadius() ) );
			mBatchGBufferSphere->draw();
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// SHADOW MAP
	
	// Draw shadow casters into FBO from view of shadow camera
	if ( mEnabledShadow ) {
		const gl::ScopedFramebuffer scopedFrameBuffer( mFboShadowMap );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboShadowMap->getSize() );
		const gl::ScopedMatrices scopedMatrices;
		gl::enableDepthRead( true );
		gl::enableDepthWrite( true );
		gl::clear();
		gl::setMatrices( mShadowCamera );
		for ( const mat4& m : spheres ) {
			const gl::ScopedModelMatrix scopedModelMatrix;
			gl::multModelMatrix( m );
			mBatchShadowSphere->draw();
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// L-BUFFER
	
	{
		const gl::ScopedFramebuffer scopedFrameBuffer( mFboLBuffer );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboLBuffer->getSize() );
		gl::clear();
		const gl::ScopedMatrices scopedMatrices;
		const gl::ScopedBlendAdditive scopedAdditiveBlend;
		const gl::ScopedFaceCulling scopedFaceCulling( true, GL_FRONT );
		gl::setMatrices( mCamera );
		gl::enableDepthRead();
		gl::disableDepthWrite();
		
		// Bind G-buffer textures and shadow map
		const gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBuffer[ 0 ],			0 );
		const gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBuffer[ 1 ],			1 );
		const gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBuffer[ 2 ],			2 );
		const gl::ScopedTextureBind scopedTextureBind3( mFboShadowMap->getDepthTexture(),	3 );
		
		// Draw light volumes
		mBatchLBufferCube->getGlslProg()->uniform( "uShadowEnabled",			mEnabledShadow );
		mBatchLBufferCube->getGlslProg()->uniform( "uShadowMatrix",
												  mShadowCamera.getProjectionMatrix() * mShadowCamera.getViewMatrix() );
		mBatchLBufferCube->getGlslProg()->uniform( "uViewMatrixInverse",		 mCamera.getInverseViewMatrix() );
		
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
	const gl::ScopedViewport scopedViewport( ivec2( 0 ), toPixels( getWindowSize() ) );
	const gl::ScopedMatrices scopedMatrices;
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::clear();
	
	if ( mDebugMode ) {
		
		// Draw G-buffer
		gl::clear( Colorf::gray( 0.4f ) );
		gl::setMatricesWindow( getWindowSize() );
		const gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBuffer[ 0 ], 0 );
		const gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBuffer[ 1 ], 1 );
		const gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBuffer[ 2 ], 2 );
		
		// Albedo   | Normals
		// --------------------
		// Position | Emissive
		vec2 sz = getWindowCenter();
		for ( int32_t i = 0; i < 4; ++i ) {
			const vec2 pos( ( i % 2 ) * sz.x, glm::floor( (float)i * 0.5f ) * sz.y );
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( pos + sz * 0.5f );
			gl::scale( sz );
			mBatchDebugRect->getGlslProg()->uniform( "uMode", i );
			mBatchDebugRect->draw();
		}
	} else {
		gl::translate( getWindowCenter() );
		gl::scale( getWindowSize() );
		
		const gl::ScopedTextureBind scopedTextureBind( mFboLBuffer->getColorTexture(), 0 );
		if ( mEnabledFxaa ) {
			
			// Perform FXAA
			mBatchFxaaRect->getGlslProg()->uniform( "uPixel", vec2( 1.0f ) / vec2( toPixels( getWindowSize() ) ) );
			mBatchFxaaRect->draw();
		} else {
			
			// Draw without anti-aliasing
			mBatchStockTextureRect->draw();
		}
	}
}

void DeferredShadingApp::loadShaders()
{
	// Shortcut for shader loading and error handling
	auto loadGlslProg = [ & ]( const gl::GlslProg::Format& format ) -> gl::GlslProgRef
	{
		const string names = format.getVertexPath().string() + " + " + format.getFragmentPath().string();
		gl::GlslProgRef glslProg;
		try {
			glslProg = gl::GlslProg::create( format );
		} catch ( const Exception& ex ) {
			CI_LOG_EXCEPTION( names, ex );
			quit();
		}
		return glslProg;
	};
	
	// Load shader files from disk
	const DataSourceRef fragDebug		= loadAsset( "debug.frag" );
	const DataSourceRef fragFxaa		= loadAsset( "fxaa.frag" );
	const DataSourceRef fragGBuffer		= loadAsset( "gbuffer.frag" );
	const DataSourceRef fragLBuffer		= loadAsset( "lbuffer.frag" );
	const DataSourceRef fragShadowMap	= loadAsset( "shadow_map.frag" );
	const DataSourceRef vertGBuffer		= loadAsset( "gbuffer.vert" );
	const DataSourceRef vertPassThrough = loadAsset( "pass_through.vert" );
	const DataSourceRef vertShadowMap	= loadAsset( "shadow_map.vert" );
	
	// Create GLSL programs
#if defined( CINDER_GL_ES_3 )
	int32_t version = 300;
#else
	int32_t version = 330;
#endif
	const gl::GlslProgRef debug			= loadGlslProg( gl::GlslProg::Format().version( version )
													   .vertex( vertPassThrough ).fragment( fragDebug ) );
	const gl::GlslProgRef fxaa			= loadGlslProg( gl::GlslProg::Format().version( version )
													   .vertex( vertPassThrough ).fragment( fragFxaa ) );
	const gl::GlslProgRef gBuffer		= loadGlslProg( gl::GlslProg::Format().version( version )
													   .vertex( vertGBuffer ).fragment( fragGBuffer ) );
	const gl::GlslProgRef lBuffer		= loadGlslProg( gl::GlslProg::Format().version( version )
													   .vertex( vertPassThrough ).fragment( fragLBuffer ) );
	const gl::GlslProgRef shadowMap		= loadGlslProg( gl::GlslProg::Format().version( version )
													   .vertex( vertShadowMap ).fragment( fragShadowMap ) );
	const gl::GlslProgRef stockColor	= gl::context()->getStockShader( gl::ShaderDef().color() );
	const gl::GlslProgRef stockTexture	= gl::context()->getStockShader( gl::ShaderDef().texture( GL_TEXTURE_2D ) );
	
	// Create geometry
	const gl::VboMeshRef cube	= gl::VboMesh::create( geom::Cube().size( vec3( 2.0f ) ) );
	const gl::VboMeshRef plane	= gl::VboMesh::create( geom::Plane()
													  .axes( vec3( 0.0f, 1.0f, 0.0f ), vec3( 0.0f, 0.0f, 1.0f ) )
													  .normal( vec3( 0.0f, 1.0f, 0.0f ) )
													  .size( vec2( 20.0f ) ) );
	const gl::VboMeshRef rect	= gl::VboMesh::create( geom::Rect() );
	const gl::VboMeshRef sphere = gl::VboMesh::create( geom::Sphere().subdivisions( 64 ) );
	
	// Create batches
	mBatchDebugRect			= gl::Batch::create( rect,		debug );
	mBatchFxaaRect			= gl::Batch::create( rect,		fxaa );
	mBatchGBufferPlane		= gl::Batch::create( plane,		gBuffer );
	mBatchGBufferSphere		= gl::Batch::create( sphere,	gBuffer );
	mBatchLBufferCube		= gl::Batch::create( cube,		lBuffer );
	mBatchShadowSphere		= gl::Batch::create( sphere,	shadowMap );
	mBatchStockColorRect	= gl::Batch::create( rect,		stockColor );
	mBatchStockColorSphere	= gl::Batch::create( rect,		stockColor );
	mBatchStockTextureRect	= gl::Batch::create( rect,		stockTexture );
	
	// Set batch uniforms
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
	
	// Color texture format
	const gl::Texture2d::Format colorTextureFormat = gl::Texture2d::Format()
	.internalFormat( GL_RGBA8 )
	.magFilter( GL_NEAREST )
	.minFilter( GL_NEAREST )
	.wrap( GL_CLAMP_TO_EDGE );
	
	// Data texture format
	const gl::Texture2d::Format dataTextureFormat = gl::Texture2d::Format()
	.internalFormat( GL_RGBA16F )
	.magFilter( GL_NEAREST )
	.minFilter( GL_NEAREST )
	.wrap( GL_CLAMP_TO_EDGE );
	
	// Create FBO for the the geometry buffer (G-buffer). This G-buffer uses
	// three color attachments to store position, normal, emissive (light), and
	// color (albedo) data.
	const ivec2 winSize = getWindowSize();
	const int32_t h		= winSize.y;
	const int32_t w		= winSize.x;
	try {
		gl::Fbo::Format fboFormat;
		mTextureFboGBuffer[ 0 ] = gl::Texture2d::create( w, h, colorTextureFormat );
		mTextureFboGBuffer[ 1 ] = gl::Texture2d::create( w, h, dataTextureFormat );
		mTextureFboGBuffer[ 2 ] = gl::Texture2d::create( w, h, dataTextureFormat );
		for ( size_t i = 0; i < 3; ++i ) {
			fboFormat.attachment( GL_COLOR_ATTACHMENT0 + (GLenum)i, mTextureFboGBuffer[ i ] );
		}
		fboFormat.depthTexture();
		mFboGBuffer = gl::Fbo::create( w, h, fboFormat );
		const gl::ScopedFramebuffer scopedFramebuffer( mFboGBuffer );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboGBuffer->getSize() );
		gl::clear();
	}
	catch( const std::exception& e ) {
		console() << "mFboGBuffer failed: " << e.what() << std::endl;
	}
	
	// Create FBO for the light buffer (L-buffer). The L-buffer reads the
	// G-buffer textures to render the scene inside light volumes.
	try {
		mFboLBuffer	= gl::Fbo::create( w, h, gl::Fbo::Format().colorTexture() );
		const gl::ScopedFramebuffer scopedFramebuffer( mFboLBuffer );
		const gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboLBuffer->getSize() );
		gl::clear();
	}
	catch( const std::exception& e ) {
		console() << "mFboLBuffer failed: " << e.what() << std::endl;
	}
}

#if !defined( CINDER_GL_ES )
void DeferredShadingApp::screenShot()
{
	const fs::path path = getAppPath();
	writeImage( path / fs::path( "frame" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
}
#endif

void DeferredShadingApp::update()
{
#if ! defined( CINDER_GL_ES )
	mFrameRate	= getAverageFps();
	
	if( mFullScreen != isFullScreen() ) {
		setFullScreen( mFullScreen );
	}

	// Update parameters
	ImGui::Begin( "Parameters" );
	ImGui::Text( "Framerate: %f", mFrameRate );
	ImGui::Checkbox( "Debug mode", &mDebugMode );// .key( "d" );
	ImGui::Checkbox( "Fullscreen", &mFullScreen ); //.key( "f" );
	if( ImGui::Button( "Load shaders" ) ) {
		loadShaders();
	}
	if( ImGui::Button( "Screen shot" ) ) {
		screenShot();
	}
	if( ImGui::Button( "Quit" ) ) {
		quit();
	}
	ImGui::Separator();
	ImGui::Checkbox( "FXAA", &mEnabledFxaa );
	ImGui::Checkbox( "Shadows", &mEnabledShadow );
	ImGui::End();
#endif
	
	// Update light positions
	if ( !mLights.empty() ) {
		const float e			= (float)getElapsedSeconds();
		const size_t numLights = mLights.size() - 1;
		float t					= e;
		const float d			= ( (float)M_PI * 2.0f ) / (float)numLights;
		const float r			= 3.5f;
		for ( size_t i = 0; i < numLights; ++i, t += d ) {
			const float ground	= 0.1f;
			const float x		= glm::cos( t );
			const float z		= glm::sin( t );
			vec3 p				= vec3( x, 0.0f, z ) * r;
			p.y					= ground + 1.5f;
			mLights.at( i + 1 ).setPosition( p );
		}
		
		t					= e * 0.333f;
		mLights.front().setPosition( vec3( glm::sin( t ), 7.0f, glm::cos( t ) ) );
		mShadowCamera.setEyePoint( mLights.front().getPosition() );
	}
}

void DeferredShadingApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_d ) {
		mDebugMode = !mDebugMode;
	}
	else if( event.getCode() == KeyEvent::KEY_f ) {
		mFullScreen = !mFullScreen;
	}
	else if( event.getCode() == KeyEvent::KEY_l ) {
		loadShaders();
	}
	else if( event.getCode() == KeyEvent::KEY_SPACE ) {
		screenShot();
	}
	else if( event.getCode() == KeyEvent::KEY_q ) {
		quit();
	}
	else if( event.getCode() == KeyEvent::KEY_a ) {
		mEnabledFxaa = ! mEnabledFxaa;
	}
	else if( event.getCode() == KeyEvent::KEY_s ) {
		mEnabledShadow = !mEnabledShadow;
	}
}

#if defined( CINDER_GL_ES )
CINDER_APP( DeferredShadingApp, RendererGl, []( App::Settings* settings )
{
	settings->disableFrameRate();
} )
#else
CINDER_APP( DeferredShadingApp, RendererGl, []( App::Settings* settings )
{
	settings->disableFrameRate();
	settings->setWindowSize( 1280, 720 );
} )
#endif
