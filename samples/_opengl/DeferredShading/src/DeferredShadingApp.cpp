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
#include "cinder/MayaCamUI.h"
#include "cinder/params/Params.h"

#include "Light.h"

class DeferredShadingApp : public ci::app::App
{
public:
	DeferredShadingApp();

	void						draw() override;
	void						mouseDown( ci::app::MouseEvent event ) override;
	void						mouseDrag( ci::app::MouseEvent event ) override;
	void						resize() override;
	void						update() override;
private:
	ci::MayaCamUI				mMayaCam;

	std::vector<Light>			mLights;
	
	ci::gl::GlslProgRef			mGlslProgDebug;
	ci::gl::GlslProgRef			mGlslProgFxaa;
	ci::gl::GlslProgRef			mGlslProgGBuffer;
	ci::gl::GlslProgRef			mGlslProgLBuffer;
	ci::gl::GlslProgRef			mGlslProgShadowMap;
	ci::gl::GlslProgRef			mGlslProgStockColor;
	ci::gl::GlslProgRef			mGlslProgStockTexture;

	ci::gl::FboRef				mFbo;
	ci::gl::FboRef				mFboShadowMap;
	
	ci::gl::Texture2dRef		mTextureFbo[ 4 ];
	ci::gl::Texture2dRef		mTextureFboShadowMap;
	ci::gl::TextureRef			mTextureRandom;
	
	ci::gl::VboMeshRef			mMeshCube;
	ci::gl::VboMeshRef			mMeshRect;
	ci::gl::VboMeshRef			mMeshSphere;

	bool						mEnabledFxaa;
	bool						mEnabledShadow;

	ci::CameraPersp				mShadowCamera;

	float						mFloor;

	bool						mDebugMode;
	float						mFrameRate;
	bool						mFullScreen;
	ci::params::InterfaceGlRef	mParams;
	void						screenShot();
};

#include "cinder/app/RendererGl.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

DeferredShadingApp::DeferredShadingApp()
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
	DataSourceRef passThrough	= loadAsset( "pass_through.vert" );
	mGlslProgDebug				= loadGlslProg( "Debug",		passThrough,					loadAsset( "debug.frag" ) );
	mGlslProgFxaa				= loadGlslProg( "FXAA",			passThrough,					loadAsset( "fxaa.frag" ) );
	mGlslProgGBuffer			= loadGlslProg( "G-buffer",		loadAsset( "gbuffer.vert" ),	loadAsset( "gbuffer.frag" ) );
	mGlslProgLBuffer			= loadGlslProg( "L-buffer",		passThrough,					loadAsset( "lbuffer.frag" ) );
	mGlslProgShadowMap			= loadGlslProg( "Shadow map",	loadAsset( "shadow_map.vert" ),	loadAsset( "shadow_map.frag" ) );
	mGlslProgStockColor			= gl::context()->getStockShader( gl::ShaderDef().color() );
	mGlslProgStockTexture		= gl::context()->getStockShader( gl::ShaderDef().texture( GL_TEXTURE_2D ) );
	
	// Set default values for all properties
	mDebugMode		= false;
	mEnabledFxaa	= true;
	mEnabledShadow	= true;
	mFloor			= -7.0f;
	mFrameRate		= 0.0f;
	mFullScreen		= isFullScreen();
	mMeshCube		= gl::VboMesh::create( geom::Cube() );
	mMeshRect		= gl::VboMesh::create( geom::Rect() );
	mMeshSphere		= gl::VboMesh::create( geom::Sphere().subdivisions( 64 ) );
	mTextureRandom	= gl::Texture::create( loadImage( loadAsset( "random.png" ) ) );

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
	CameraPersp cam( windowSize.x, windowSize.y, 45.0f, 1.0f, 100.0f );
	cam.setEyePoint( vec3( -2.221f, -4.083f, 15.859f ) );
	cam.setCenterOfInterestPoint( vec3( -0.635f, -4.266f, 1.565f ) );
	mMayaCam.setCurrentCam( cam );

	// Set up parameters
	mParams = params::InterfaceGl::create( "Params", ivec2( 220, 220 ) );
	mParams->addParam( "Frame rate",	&mFrameRate,				"", true );
	mParams->addParam( "Debug mode",	&mDebugMode ).key( "d" );
	mParams->addParam( "Fullscreen",	&mFullScreen ).key( "f" );
	mParams->addButton( "Screen shot",	[ & ]() { screenShot(); },	"key=space" );
	mParams->addButton( "Quit",			[ & ]() { quit(); },		"key=q" );
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
								  mMayaCam.getCamera().getNearClip(), mMayaCam.getCamera().getFarClip() );
	mShadowCamera.lookAt( vec3( 0.0 ), vec3( 0.0f, mFloor, 0.0f ) );

	// Call resize to create FBOs
	resize();
}

void DeferredShadingApp::draw()
{
	gl::disableAlphaBlending();

	const mat4 shadowMatrix = mShadowCamera.getProjectionMatrix() * mShadowCamera.getViewMatrix();
	vec2 winSize			= vec2( getWindowSize() );
	float e					= (float)getElapsedSeconds();

	// This draws our shadow casters
	auto drawSpheres = [ & ]()
	{
		{
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( vec3( 0.0f, -4.0f, 0.0f ) );
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
		gl::enableDepthRead( true );
		gl::enableDepthWrite( true );
		gl::clear();
		gl::setMatrices( mMayaCam.getCamera() );

		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgGBuffer );
			mGlslProgGBuffer->uniform( "uEmissive",	0.0f );

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
		// Set up window and clear L-buffer
		gl::ScopedFramebuffer scopedFrameBuffer( mFbo );
		gl::drawBuffer( GL_COLOR_ATTACHMENT3 );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFbo->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::ScopedAdditiveBlend scopedAdditiveBlend;
		gl::ScopedState scopedState( GL_DEPTH_TEST, false );
		gl::ScopedFaceCulling scopedFaceCulling( true, GL_FRONT );
		gl::clear();
		gl::setMatrices( mMayaCam.getCamera() );
	
		// Bind G-buffer textures and shadow map
		gl::ScopedTextureBind scopedTextureBind0( mTextureFbo[ 0 ],		0 );
		gl::ScopedTextureBind scopedTextureBind1( mTextureFbo[ 1 ],		1 );
		gl::ScopedTextureBind scopedTextureBind2( mTextureFbo[ 2 ],		2 );
		gl::ScopedTextureBind scopedTextureBind3( mTextureFboShadowMap,	3 );

		// Draw light volumes
		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgLBuffer );
			mGlslProgLBuffer->uniform( "uSamplerAlbedo",			0 );
			mGlslProgLBuffer->uniform( "uSamplerNormalEmissive",	1 );
			mGlslProgLBuffer->uniform( "uSamplerPosition",			2 );
			mGlslProgLBuffer->uniform( "uSamplerShadowMap",			3 );

			mGlslProgLBuffer->uniform( "uShadowEnabled",			mEnabledShadow );
			mGlslProgLBuffer->uniform( "uShadowMatrix",				shadowMatrix );
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
	gl::clear();

	if ( mDebugMode ) {

		// Draw G-buffer
		gl::clear( Colorf::gray( 0.4f ) );
		gl::setMatricesWindow( getWindowSize() );
		gl::ScopedGlslProg scopedGlslProg( mGlslProgDebug );
		mGlslProgDebug->uniform( "uSamplerAlbedo",			0 );
		mGlslProgDebug->uniform( "uSamplerNormalEmissive",	1 );
		mGlslProgDebug->uniform( "uSamplerPosition",			2 );
		gl::ScopedTextureBind scopedTextureBind0( mTextureFbo[ 0 ],	0 );
		gl::ScopedTextureBind scopedTextureBind2( mTextureFbo[ 1 ],	1 );
		gl::ScopedTextureBind scopedTextureBind3( mTextureFbo[ 2 ],	2 );
		
		// Albedo   | Normals
		// --------------------
		// Position | Emissive
		vec2 sz = getWindowCenter();
		for ( int32_t i = 0; i < 4; ++i ) {
			mGlslProgDebug->uniform( "uMode", i );
			vec2 pos( ( i % 2 ) * sz.x, glm::floor( (float)i * 0.5f ) * sz.y );
			gl::drawSolidRect( Rectf( pos, pos + sz ) );
		}

	} else {
		gl::ScopedTextureBind scopedTextureBind( mTextureFbo[ 3 ], 0 );
		if ( mEnabledFxaa ) {

			// Perform FXAA
			gl::ScopedGlslProg scopedGlslProg( mGlslProgFxaa );
			mGlslProgFxaa->uniform( "uPixel",	vec2( 1.0f ) / winSize );
			mGlslProgFxaa->uniform( "uSampler",	0 );
			gl::drawSolidRect( Rectf( vec2( 0.0f ), getWindowSize() ) );
		} else {

			// Draw without anti-aliasing
			gl::ScopedGlslProg scopedGlslProg( mGlslProgStockTexture );
			gl::drawSolidRect( Rectf( vec2( 0.0f ), getWindowSize() ) );
		}
	}

	mParams->draw();
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
	writeImage( getAppPath() / fs::path( "frame" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
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
 