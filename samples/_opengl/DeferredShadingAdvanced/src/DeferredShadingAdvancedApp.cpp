/* 
 * Deferred Shading Advanced sample application by:
 * Stephen Schieberl - Wieden+Kennedy
 * Michael Latzoni - Wieden+Kennedy
 * 
 * Additional code:
 * Simon Geilfus
 * 
 * This sample demonstrates how to create a robust deferred shading engine.
 * The scene is rendered into a frame buffer with multiple attachments
 * (G-buffer). A uniform buffer object is used to store a database of
 * material data on the GPU. Shadow casters are rendered into a shadow 
 * map FBO. The data from each is read while drawing light volumes into the 
 * light buffer (L-buffer). Finally, the L-buffer is drawn to the screen.
 * 
 * A screen space ambient occlusion (SSAO) pass provides extra detail with 
 * local shadows. Lights are accumulated to leave subtle trails, then bloomed 
 * to appear that they are glowing. We follow these with some post-processing 
 * passes, including depth of field to mimic camera focus, some color tweaks, 
 * and anti-aliasing.
 * 
 * This sample is intended for powerful GPUs. Check out the simpler 
 * DeferredShading sample if this is too taxing on your machine, 
 * or if you are not yet familiar with the concept of deferred shading.
 */

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/MayaCamUI.h"
#include "cinder/params/Params.h"

#include "Light.h"
#include "Material.h"

class DeferredShadingAdvancedApp : public ci::app::App
{
public:
	DeferredShadingAdvancedApp();

	void						draw() override;
	void						mouseDown( ci::app::MouseEvent event ) override;
	void						mouseDrag( ci::app::MouseEvent event ) override;
	void						resize() override;
	void						update() override;
private:
	ci::MayaCamUI				mMayaCam;

	Light						mCameraLight;
	std::vector<Light>			mLights;
	std::vector<Material>		mMaterials;

	ci::gl::UboRef				mUboMaterial;
	
	void						loadShaders();
	ci::gl::GlslProgRef			mGlslProgBlend;
	ci::gl::GlslProgRef			mGlslProgBlur;
	ci::gl::GlslProgRef			mGlslProgColor;
	ci::gl::GlslProgRef			mGlslProgComposite;
	ci::gl::GlslProgRef			mGlslProgDebug;
	ci::gl::GlslProgRef			mGlslProgDof;
	ci::gl::GlslProgRef			mGlslProgEmissive;
	ci::gl::GlslProgRef			mGlslProgFxaa;
	ci::gl::GlslProgRef			mGlslProgGBuffer;
	ci::gl::GlslProgRef			mGlslProgLBuffer;
	ci::gl::GlslProgRef			mGlslProgShadowMap;
	ci::gl::GlslProgRef			mGlslProgSsao;
	ci::gl::GlslProgRef			mGlslProgStockColor;
	ci::gl::GlslProgRef			mGlslProgStockTexture;

	ci::gl::FboRef				mFboGBuffer;
	ci::gl::FboRef				mFboPingPong;
	ci::gl::FboRef				mFboShadowMap;
	ci::gl::FboRef				mFboSmall;
	
	ci::gl::Texture2dRef		mTextureFboGBuffer[ 4 ];
	ci::gl::Texture2dRef		mTextureFboPingPong[ 2 ];
	ci::gl::Texture2dRef		mTextureFboSmall[ 5 ];
	ci::gl::Texture2dRef		mTextureFboShadowMap;
	ci::gl::TextureRef			mTextureRandom;

	ci::gl::VboMeshRef			mMeshCircle;
	ci::gl::VboMeshRef			mMeshCube;
	ci::gl::VboMeshRef			mMeshIcosahedron;
	ci::gl::VboMeshRef			mMeshSphere;
	ci::gl::VboMeshRef			mMeshSphereLow;

	bool						mEnabledBloom;
	bool						mEnabledColor;
	bool						mEnabledDoF;
	bool						mEnabledFxaa;
	bool						mEnabledSsao;
	bool						mEnabledShadow;

	ci::CameraPersp				mShadowCamera;

	float						mFloor;
	ci::vec3					mSpherePosition;
	float						mSphereVelocity;

	bool						mDrawDebug;
	bool						mDrawLightVolume;

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

DeferredShadingAdvancedApp::DeferredShadingAdvancedApp()
{
	gl::enableVerticalSync();
	
	loadShaders();
	
	// Set default values for all properties
	mDrawDebug			= false;
	mDrawLightVolume	= false;
	mEnabledBloom		= true;
	mEnabledColor		= true;
	mEnabledDoF			= true;
	mEnabledFxaa		= true;
	mEnabledShadow		= true;
	mEnabledSsao		= true;
	mFloor				= -7.0f;
	mFrameRate			= 0.0f;
	mFullScreen			= isFullScreen();
	mMeshCircle			= gl::VboMesh::create( geom::Circle().subdivisions( 64 ) );
	mMeshCube			= gl::VboMesh::create( geom::Cube() );
	mMeshIcosahedron	= gl::VboMesh::create( geom::Icosahedron() );
	mMeshSphere			= gl::VboMesh::create( geom::Sphere().subdivisions( 64 ) );
	mMeshSphereLow		= gl::VboMesh::create( geom::Sphere().subdivisions( 12 ) );
	mSpherePosition		= vec3( 0.0f, -4.5f, 0.0f );
	mSphereVelocity		= -0.1f;
	mTextureRandom		= gl::Texture::create( loadImage( loadAsset( "random.png" ) ) );

	// Set up lights
	mCameraLight = Light().colorAmbient( ColorAf::gray( 0.3f ) ).colorSpecular( ColorAf::gray( 0.1f ) )
		.radius( mMayaCam.getCamera().getFarClip() * 0.5f ).intensity( 0.5f );
	for ( size_t i = 0; i < 8; ++i ) {
		float t = (float)i / 8.0f;
		ColorAf c( 0.91f + t * 0.1f, 0.5f + t * 0.5f, 0.9f - t * 0.25f, 1.0f );
		mLights.push_back( Light().colorDiffuse( c )
						  .intensity( 1.0f ).radius( 0.1f ).volume( 5.0f ) );
	}
	float d	= ( (float)M_PI * 2.0f ) / 5.0f;
	float r = 9.0f;
	float t = 0.0f;
	for ( size_t i = 0; i < 5; ++i, t += d ) {
		vec3 p( glm::cos( t ) * r, mFloor + 0.5f, glm::sin( t ) * r );
		mLights.push_back( Light().colorDiffuse( ColorAf( 0.85f, 0.7f, 1.0f, 1.0f ) )
						  .intensity( 1.0f ).position( p ).radius( 0.1f ).volume( 3.0f ) );
	}
	
	// Set up materials
	mMaterials.push_back( Material().colorDiffuse( ColorAf::white() ).colorSpecular( ColorAf::white() )
						  .shininess( 300.0f ) ); // Sphere
	mMaterials.push_back( Material().colorAmbient( ColorAf::gray( 0.18f ) )
						  .colorDiffuse( ColorAf::gray( 0.5f ) ).colorSpecular( ColorAf::white() )
						  .shininess( 500.0f ) ); // Floor
	mMaterials.push_back( Material().colorAmbient( ColorAf::black() )
						  .colorDiffuse( ColorAf::black() ).colorEmission( ColorAf::white() )
						  .shininess( 100.0f ) ); // Light
	mUboMaterial = gl::Ubo::create( sizeof( Material ) * mMaterials.size(), (GLvoid*)&mMaterials[ 0 ] );
	gl::context()->bindBufferBase( mUboMaterial->getTarget(), 0, mUboMaterial );

	// Set up camera
	ivec2 windowSize = toPixels( getWindowSize() );
	CameraPersp cam( windowSize.x, windowSize.y, 60.0f, 1.0f, 100.0f );
	cam.setEyePoint( vec3( 2.664f, -6.484f, 5.939f ) );
	cam.setCenterOfInterestPoint( vec3( 0.469f, -5.430f, 1.146f ) );
	mMayaCam.setCurrentCam( cam );

	// Set up parameters
	mParams = params::InterfaceGl::create( "Params", ivec2( 220, 300 ) );
	mParams->addParam( "Frame rate",		&mFrameRate,				"", true );
	mParams->addParam( "Fullscreen",		&mFullScreen ).key( "f" );
	mParams->addButton( "Load shaders",		[ & ]() { loadShaders(); },	"key=l" );
	mParams->addButton( "Screen shot",		[ & ]() { screenShot(); },	"key=space" );
	mParams->addButton( "Quit",				[ & ]() { quit(); },		"key=q" );
	mParams->addSeparator();
	mParams->addParam( "Debug",				&mDrawDebug ).key( "d" ).group( "Draw" );
	mParams->addParam( "Light volume",		&mDrawLightVolume ).key( "v" ).group( "Draw" );
	mParams->addSeparator();
	mParams->addParam( "Bloom",				&mEnabledBloom ).key( "1" ).group( "Pass" );
	mParams->addParam( "Color",				&mEnabledColor ).key( "2" ).group( "Pass" );
	mParams->addParam( "Depth of field",	&mEnabledDoF ).key( "3" ).group( "Pass" );
	mParams->addParam( "FXAA",				&mEnabledFxaa ).key( "4" ).group( "Pass" );
	mParams->addParam( "Shadows",			&mEnabledShadow ).key( "5" ).group( "Pass" );
	mParams->addParam( "SSAO",				&mEnabledSsao ).key( "6" ).group( "Pass" );

	// Create shadow map buffer
	{
		size_t sz = 512;
		mTextureFboShadowMap = gl::Texture2d::create( sz, sz, gl::Texture2d::Format()
													 .internalFormat( GL_DEPTH_COMPONENT32F )
													 .magFilter( GL_LINEAR )
													 .minFilter( GL_LINEAR )
													 .wrap( GL_CLAMP_TO_EDGE )
													 .dataType( GL_FLOAT ) );
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
	mShadowCamera.setPerspective( 120.0f, mFboShadowMap->getAspectRatio(),
								 mMayaCam.getCamera().getNearClip(),
								 mMayaCam.getCamera().getFarClip() );
	mShadowCamera.lookAt( vec3( 0.0 ), vec3( 0.0f, mFloor, 0.0f ) );
	
	// Call resize to create FBOs
	resize();
}

void DeferredShadingAdvancedApp::draw()
{
	gl::disableAlphaBlending();

	float e							= (float)getElapsedSeconds();
	const vec2 winSize				= vec2( getWindowSize() );
	const mat4 shadowMatrix			= mShadowCamera.getProjectionMatrix() * mShadowCamera.getViewMatrix();
	const mat4 projMatrixInverse	= glm::inverse( mMayaCam.getCamera().getProjectionMatrix() );
	float nearClip					= mMayaCam.getCamera().getNearClip();
	float farClip					= mMayaCam.getCamera().getFarClip();
	vec2 projectionParams			= vec2( farClip / ( farClip - nearClip ), 
											( -farClip * nearClip ) / ( farClip - nearClip ) );
	
	// Draws shadow casting objects
	auto drawShadowCasters = [ & ]()
	{
		{
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( mSpherePosition );
			gl::draw( mMeshSphere );
		}

		float t		= e * 0.165f;
		float d		= ( (float)M_PI * 2.0f ) / 4.0f;
		float r		= 4.5f;
		for ( size_t i = 0; i < 4; ++i, t += d ) {
			float x	= glm::cos( t );
			float z	= glm::sin( t );
			vec3 p	= vec3( x, 0.0f, z ) * r;
			p.y		= mFloor + 0.5f;

			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( p );
			gl::rotate( e );
			gl::scale( vec3( 0.5f ) );
			gl::draw( mMeshIcosahedron );
		}
	};
	
	// Clear frame buffers
	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFboGBuffer );
		const static GLenum buffers[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2
		};
		gl::drawBuffers( 3, buffers );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboGBuffer->getSize() );
		gl::clear();
	}
	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFboPingPong );
		const static GLenum buffers[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1
		};
		gl::drawBuffers( 2, buffers );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboPingPong->getSize() );
		gl::clear();
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
		drawShadowCasters();
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// G-BUFFER
		
	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFboGBuffer );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboGBuffer->getSize() );
		const static GLenum buffers[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2
		};
		gl::drawBuffers( 3, buffers );
		gl::ScopedMatrices scopedMatrices;
		gl::setMatrices( mMayaCam.getCamera() );
		gl::enableDepthRead( true );
		gl::enableDepthWrite( true );

		gl::ScopedGlslProg scopedGlslProg( mGlslProgGBuffer );

		////// BEGIN DRAW STUFF ////////////////////////////////////////////////
			
		// Draw shadow casters (spheres)
		mGlslProgGBuffer->uniform( "uMaterialId", 0 );
		drawShadowCasters();
		
		// Draw non shadow casters (floor)
		{
			gl::ScopedModelMatrix scopedModelMatrix;
			mGlslProgGBuffer->uniform( "uMaterialId", 1 );
			gl::translate( vec3( 0.0f, mFloor, 0.0f ) );
			gl::rotate( quat( vec3( 4.71f, 0.0f, 0.0f ) ) );
			gl::scale( vec3( 10.0f ) );
			gl::draw( mMeshCircle );
		}

		// Draw light sources
		mGlslProgGBuffer->uniform( "uMaterialId", 2 );
		for ( const Light& light : mLights ) {
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::ScopedColor scopedColor( light.getColorDiffuse() );
			gl::translate( light.getPosition() );
			gl::scale( vec3( light.getRadius() ) );
			gl::draw( mMeshSphere );
		}
			
		////// END DRAW STUFF //////////////////////////////////////////////////
			
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// L-BUFFER

	size_t ping = 0;
	size_t pong = 1;

	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFboPingPong );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboPingPong->getSize() );
		gl::drawBuffer( GL_COLOR_ATTACHMENT0 + ping );
		gl::ScopedAdditiveBlend scopedAdditiveBlend;
		gl::ScopedMatrices scopedMatrices;
		gl::setMatrices( mMayaCam.getCamera() );
		gl::enableDepthRead( true );
		gl::enableDepthWrite( true );
			
		// Bind G-buffer textures and shadow map
		gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBuffer[ 0 ],	0 );
		gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBuffer[ 1 ],	1 );
		gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBuffer[ 2 ],	2 );
		gl::ScopedTextureBind scopedTextureBind3( mTextureFboGBuffer[ 3 ],	3 );
		gl::ScopedTextureBind scopedTextureBind4( mTextureFboShadowMap,		4 );

		// Draw light volumes
		gl::ScopedGlslProg scopedGlslProg( mGlslProgLBuffer );
		mGlslProgLBuffer->uniform( "uProjMatrixInverse",	projMatrixInverse );
		mGlslProgLBuffer->uniform( "uProjectionParams",		projectionParams );
		mGlslProgLBuffer->uniform( "uSamplerDepth",			0 );
		mGlslProgLBuffer->uniform( "uSamplerAlbedo",		1 );
		mGlslProgLBuffer->uniform( "uSamplerMaterial",		2 );
		mGlslProgLBuffer->uniform( "uSamplerNormal",		3 );
		mGlslProgLBuffer->uniform( "uSamplerShadowMap",		4 );
		mGlslProgLBuffer->uniform( "uShadowEnabled",		mEnabledShadow );
		mGlslProgLBuffer->uniform( "uShadowMatrix",			shadowMatrix );
		mGlslProgLBuffer->uniform( "uViewMatrixInverse",	mMayaCam.getCamera().getInverseViewMatrix() );
		mGlslProgLBuffer->uniform( "uWindowSize",			vec2( mFboPingPong->getSize() ) );
		mGlslProgLBuffer->uniformBlock( 0, 0 );
		{
			gl::ScopedState scopedState( GL_DEPTH_TEST, false );
			gl::ScopedFaceCulling scopedFaceCulling( true, GL_FRONT );
			
			for ( const Light& light : mLights ) {
				vec3 p = vec3( ( mMayaCam.getCamera().getViewMatrix() * vec4( light.getPosition(), 1.0 ) ) );
					
				mGlslProgLBuffer->uniform( "uLightColorAmbient",	light.getColorAmbient() );
				mGlslProgLBuffer->uniform( "uLightColorDiffuse",	light.getColorDiffuse() );
				mGlslProgLBuffer->uniform( "uLightColorSpecular",	light.getColorSpecular() );
				mGlslProgLBuffer->uniform( "uLightPosition",		p );
				mGlslProgLBuffer->uniform( "uLightIntensity",		light.getIntensity() );
				mGlslProgLBuffer->uniform( "uLightRadius",			light.getVolume() );
					
				gl::ScopedModelMatrix scopedModelMatrix;
				gl::translate( light.getPosition() );
				gl::scale( vec3( light.getVolume() ) );
				gl::draw( mMeshCube );
			}
		}

		// Draw camera light as full screen rectangle to reduce geometry
		gl::setMatricesWindow( mFboPingPong->getSize() );
		mGlslProgLBuffer->uniform( "uLightColorAmbient",	mCameraLight.getColorAmbient() );
		mGlslProgLBuffer->uniform( "uLightColorDiffuse",	mCameraLight.getColorDiffuse() );
		mGlslProgLBuffer->uniform( "uLightColorSpecular",	mCameraLight.getColorSpecular() );
		mGlslProgLBuffer->uniform( "uLightPosition",		mCameraLight.getPosition() );
		mGlslProgLBuffer->uniform( "uLightIntensity",		mCameraLight.getIntensity() );
		mGlslProgLBuffer->uniform( "uLightRadius",			mCameraLight.getRadius() );
		gl::drawSolidRect( mFboPingPong->getBounds() );

		ping = pong;
		pong = ( pong + 1 ) % 2;
	}

	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFboSmall );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboSmall->getSize() );
		
		////////////////////////////////////////////////////////////////////////////////////////////
		// LIGHT ACCUMULATION AND BLOOM

		gl::ScopedMatrices scopedMatrices;
		gl::setMatricesWindow( mFboSmall->getSize() );
		gl::disableDepthRead();
		gl::disableDepthWrite();
			
		// Light accumulation
		gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
				
		// Dim last frame (produces light trails)
		{
			gl::ScopedAlphaBlend scopedAlphaBlend( false );
			gl::ScopedGlslProg scopedGlslProg( mGlslProgStockColor );
			gl::ScopedColor scopedColor( ColorAf( Colorf::black(), 0.43f ) );
			gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboSmall->getSize() ) );
		}
		{
			gl::ScopedAdditiveBlend scopedAdditiveBlend;
			gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBuffer[ 1 ], 0 );
			gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBuffer[ 2 ], 1 );
			gl::ScopedGlslProg scopedGlslProg( mGlslProgEmissive );
			mGlslProgEmissive->uniform( "uSamplerAlbedo",	0 );
			mGlslProgEmissive->uniform( "uSamplerMaterial",	1 );
			gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboSmall->getSize() ) );
		}

		// Bloom
		if ( mEnabledBloom ) {
			float bloomAtt	= 1.0f;
			vec2 bloomSize	= vec2( 1.0f ) / winSize * 3.0f;
			bloomSize		*= vec2( mFboSmall->getSize() ) / winSize;
			
			gl::ScopedGlslProg scopedGlslProg( mGlslProgBlur );
			mGlslProgBlur->uniform( "uAttenuation",	bloomAtt );
			mGlslProgBlur->uniform( "uSampler",		0 );
			
			// Horizontal pass
			gl::drawBuffer( GL_COLOR_ATTACHMENT1 );	
			mGlslProgBlur->uniform( "uSize", vec2( bloomSize.x, 0.0f ) );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ 0 ], 0 );
				gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboSmall->getSize() ) );
			}
			
			// Vertical pass
			gl::drawBuffer( GL_COLOR_ATTACHMENT2 );
			mGlslProgBlur->uniform( "uSize", vec2( 0.0f, bloomSize.y ) );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ 1 ], 0 );
				gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboSmall->getSize() ) );
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// SSAO

		if ( mEnabledSsao ) {
			gl::ScopedAlphaBlend scopedAlphaBlend( true );
			gl::ScopedMatrices scopedMatrices;
			gl::setMatricesWindow( mFboSmall->getSize() );
			gl::disableDepthRead();
			gl::disableDepthWrite();

			// SSAO pass
			{
				gl::drawBuffer( GL_COLOR_ATTACHMENT3 );
				gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBuffer[ 0 ],	0 );
				gl::ScopedTextureBind scopedTextureBind1( mTextureRandom,			1 );
				gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBuffer[ 3 ],	2 );
				gl::ScopedGlslProg scopedGlslProg( mGlslProgSsao );
				mGlslProgSsao->uniform( "uSamplerDepth",	0 );
				mGlslProgSsao->uniform( "uSamplerNoise",	1 );
				mGlslProgSsao->uniform( "uSamplerNormal",	2 );
				gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboSmall->getSize() ) );
			}

			// Calculate blur pixel size
			vec2 ssaoBlurSize	= vec2( 1.0f ) / winSize;
			ssaoBlurSize		*= vec2( mFboSmall->getSize() ) / winSize;

			// Horizontal blur pass
			gl::ScopedGlslProg scopedGlslProg( mGlslProgBlur );
			mGlslProgBlur->uniform( "uAttenuation",	1.0f );
			mGlslProgBlur->uniform( "uSize",		vec2( ssaoBlurSize.x, 0.0f ) );
			mGlslProgBlur->uniform( "uSampler",		0 );
			gl::drawBuffer( GL_COLOR_ATTACHMENT4 );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ 2 ], 0 );
				gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboSmall->getSize() ) );
			}

			// Vertical blur pass
			mGlslProgBlur->uniform( "uSize", vec2( 0.0f, ssaoBlurSize.y ) );
			gl::drawBuffer( GL_COLOR_ATTACHMENT3 );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ 3 ], 0 );
				gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboSmall->getSize() ) );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// DEBUG VIEW

	if ( mDrawDebug ) {
		gl::ScopedViewport scopedViewport( ivec2( 0 ), getWindowSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::setMatricesWindow( getWindowSize() );
		gl::disableDepthRead();
		gl::disableDepthWrite();
		gl::clear();
		
		// Handy way to draw buffers across the screen
		vec2 sz;
		size_t columns	= 4;
		float w			= (float)getWindowWidth();
		sz.x			= w / (float)columns;
		sz.y			= sz.x / getWindowAspectRatio();
		size_t index	= 0;
		auto calcPosition = [ &columns, &sz ]( size_t i ) -> vec2
		{
			return vec2( ( i % columns ) * sz.x, glm::floor( (float)i / (float)columns ) * sz.y );
		};
	
		// G-buffer and materials
		{
			gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBuffer[ 0 ], 0 );
			gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBuffer[ 1 ], 1 );
			gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBuffer[ 2 ], 2 );
			gl::ScopedTextureBind scopedTextureBind3( mTextureFboGBuffer[ 3 ], 3 );

			gl::ScopedGlslProg scopedGlslProg( mGlslProgDebug );
			mGlslProgDebug->uniform( "uProjectionParams",	projectionParams );
			mGlslProgDebug->uniform( "uProjMatrixInverse",	projMatrixInverse );
			mGlslProgDebug->uniform( "uSamplerDepth",		0 );
			mGlslProgDebug->uniform( "uSamplerAlbedo",		1 );
			mGlslProgDebug->uniform( "uSamplerMaterial",	2 );
			mGlslProgDebug->uniform( "uSamplerNormal",		3 );

			for ( int32_t i = 0; i < 10; ++i, ++index ) {
				mGlslProgDebug->uniform( "uMode", i );
				vec2 pos = calcPosition( index );
				gl::drawSolidRect( Rectf( pos, pos + sz ) );
			}
		}

		// Bloom
		gl::ScopedGlslProg scopedGlslProg( mGlslProgStockTexture );
		{
			gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ mEnabledBloom ? 2 : 0 ], 0 );
			vec2 pos = calcPosition( index );
			gl::drawSolidRect( Rectf( pos, pos + sz ) );
			++index;
		}
		{
			gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ 3 ], 0 );
			vec2 pos = calcPosition( index );
			gl::drawSolidRect( Rectf( pos, pos + sz ) );
		}
	} else {
		
		//////////////////////////////////////////////////////////////////////////////////////////////
		// COMPOSITE
		
		{
			gl::ScopedFramebuffer scopedFrameBuffer( mFboPingPong );
			gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboPingPong->getSize() );
			
			{
				gl::drawBuffer( GL_COLOR_ATTACHMENT0 + ping );
				gl::ScopedMatrices scopedMatrices;
				gl::setMatricesWindow( mFboPingPong->getSize() );
				gl::disableDepthRead();
				gl::disableDepthWrite();
				
				// Blend L-buffer and SSAO
				if ( mEnabledSsao ) {
					gl::ScopedTextureBind scopedTextureBind0( mTextureFboPingPong[ pong ],	0 );
					gl::ScopedTextureBind scopedTextureBind1( mTextureFboSmall[ 3 ],		1 );
					gl::ScopedGlslProg scopedGlslProg( mGlslProgComposite );
					mGlslProgComposite->uniform( "uSamplerLBuffer",	0 );
					mGlslProgComposite->uniform( "uSamplerSsao",	1 );
					gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboPingPong->getSize() ) );
				} else {
					gl::draw( mTextureFboPingPong[ pong ] );
				}

				ping = pong;
				pong = ( pong + 1 ) % 2;
			}
			
			//////////////////////////////////////////////////////////////////////////////////////////////
			// DEPTH OF FIELD
			
			if ( mEnabledDoF ) {
				gl::drawBuffer( GL_COLOR_ATTACHMENT0 + ping );
				gl::ScopedMatrices scopedMatrices;
				gl::setMatricesWindow( mFboPingPong->getSize() );
				gl::disableDepthRead();
				gl::disableDepthWrite();
				
				gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBuffer[ 0 ],		0 );
				gl::ScopedTextureBind scopedTextureBind1( mTextureFboPingPong[ pong ],	1 );
				gl::ScopedGlslProg scopedGlslProg( mGlslProgDof );
				mGlslProgDof->uniform( "uAspect",		mTextureFboPingPong[ pong ]->getAspectRatio() );
				mGlslProgDof->uniform( "uSamplerDepth", 0 );
				mGlslProgDof->uniform( "uSamplerColor", 1 );
				gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboPingPong->getSize() ) );
				
				ping = pong;
				pong = ( pong + 1 ) % 2;
			}
			
			//////////////////////////////////////////////////////////////////////////////////////////////
			// COLOR
			
			if ( mEnabledColor ) {
				gl::drawBuffer( GL_COLOR_ATTACHMENT0 + ping );
				gl::ScopedMatrices scopedMatrices;
				gl::setMatricesWindow( mFboPingPong->getSize() );
				gl::disableDepthRead();
				gl::disableDepthWrite();
				
				// Perform color processing pass
				gl::ScopedTextureBind scopedTextureBind( mTextureFboPingPong[ pong ], 0 );
				gl::ScopedGlslProg scopedGlslProg( mGlslProgColor );
				mGlslProgColor->uniform( "uSampler", 0 );
				gl::drawSolidRect( Rectf( vec2( 0.0f ),	mFboPingPong->getSize() ) );
				
				ping = pong;
				pong = ( pong + 1 ) % 2;
			}
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////
		// RENDER TO SCREEN
		
		gl::ScopedViewport scopedViewport( ivec2( 0 ), getWindowSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::setMatricesWindow( getWindowSize() );
		gl::disableDepthRead();
		gl::disableDepthWrite();
		
		// Perform FXAA
		{
			gl::ScopedTextureBind scopedTextureBind( mTextureFboPingPong[ pong ], 0 );
			if ( mEnabledFxaa ) {
				gl::ScopedGlslProg scopedGlslProg( mGlslProgFxaa );
				mGlslProgFxaa->uniform( "uPixel",	vec2( 1.0f ) / winSize );
				mGlslProgFxaa->uniform( "uSampler",	0 );
				gl::drawSolidRect( Rectf( vec2( 0.0f ), getWindowSize() ) );
			} else {
				gl::ScopedGlslProg scopedGlslProg( mGlslProgStockTexture );
				gl::drawSolidRect( Rectf( vec2( 0.0f ), getWindowSize() ) );
			}
		}

		// Draw light accumulation
		gl::ScopedGlslProg scopedGlslProg( mGlslProgStockTexture );
		gl::ScopedAdditiveBlend scopedAdditiveBlend;
		gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ mEnabledBloom ? 2 : 0 ], 0 );
		gl::drawSolidRect( Rectf( vec2( 0.0f ), getWindowSize() ) );

		// Draw light volumes
		if ( mDrawLightVolume ) {
			gl::ScopedAlphaBlend scopedAlphaBlend( false );
			gl::ScopedPolygonMode scopedPolygonMode( GL_LINE );
			gl::ScopedMatrices scopedMatrices;
			gl::setMatrices( mMayaCam.getCamera() );

			gl::ScopedGlslProg scopedGlslProg( mGlslProgStockColor );
			for ( const Light& light : mLights ) {
				gl::ScopedModelMatrix scopedModelMatrix;
				gl::ScopedColor scopedColor( light.getColorDiffuse() * ColorAf( Colorf::white(), 0.08f ) );
				gl::translate( light.getPosition() );
				gl::scale( vec3( light.getVolume() ) );
				gl::draw( mMeshSphereLow );
			}
		}
	}

	mParams->draw();
}

void DeferredShadingAdvancedApp::loadShaders()
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

	// Load shaders
	DataSourceRef passThrough	= loadAsset( "pass_through.vert" );
	mGlslProgBlend				= loadGlslProg( "Blend",			passThrough,					loadAsset( "blend.frag" ) );
	mGlslProgBlur				= loadGlslProg( "Blur",				passThrough,					loadAsset( "blur.frag" ) );
	mGlslProgColor				= loadGlslProg( "Color",			passThrough,					loadAsset( "color.frag" ) );
	mGlslProgComposite			= loadGlslProg( "Composite",		passThrough,					loadAsset( "composite.frag" ) );
	mGlslProgDebug				= loadGlslProg( "Debug",			passThrough,					loadAsset( "debug.frag" ) );
	mGlslProgDof				= loadGlslProg( "Depth of field",	passThrough,					loadAsset( "dof.frag" ) );
	mGlslProgEmissive			= loadGlslProg( "Emissive",			passThrough,					loadAsset( "emissive.frag" ) );
	mGlslProgFxaa				= loadGlslProg( "FXAA",				passThrough,					loadAsset( "fxaa.frag" ) );
	mGlslProgGBuffer			= loadGlslProg( "G-buffer",			loadAsset( "gbuffer.vert" ),	loadAsset( "gbuffer.frag" ) );
	mGlslProgLBuffer			= loadGlslProg( "L-buffer",			passThrough,					loadAsset( "lbuffer.frag" ) );
	mGlslProgShadowMap			= loadGlslProg( "Shadow map",		loadAsset( "shadow_map.vert" ),	loadAsset( "shadow_map.frag" ) );
	mGlslProgSsao				= loadGlslProg( "SSAO",				loadAsset( "ssao.vert" ),		loadAsset( "ssao.frag" ) );
	mGlslProgStockColor			= gl::context()->getStockShader( gl::ShaderDef().color() );
	mGlslProgStockTexture		= gl::context()->getStockShader( gl::ShaderDef().texture( GL_TEXTURE_2D ) );
}

void DeferredShadingAdvancedApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void DeferredShadingAdvancedApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void DeferredShadingAdvancedApp::resize()
{
	CameraPersp camera = mMayaCam.getCamera();
	camera.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( camera );

	// Texture format for color buffers
	gl::Texture2d::Format textureFormat = gl::Texture2d::Format()
		.internalFormat( GL_RGB10_A2 )
		.magFilter( GL_NEAREST )
		.minFilter( GL_NEAREST )
		.wrap( GL_CLAMP_TO_EDGE )
		.dataType( GL_FLOAT );

	int32_t h = getWindowHeight();
	int32_t w = getWindowWidth();

	// Set up the ping pong frame buffer. We'll use this FBO to render
	// the scene and perform post-processing passes.
	{
		gl::Fbo::Format fboFormat;
		for ( size_t i = 0; i < 2; ++i ) {
			mTextureFboPingPong[ i ] = gl::Texture2d::create( w, h, textureFormat );
			fboFormat.attachment( GL_COLOR_ATTACHMENT0 + i, mTextureFboPingPong[ i ] );
		}
		mFboPingPong = gl::Fbo::create( w, h, fboFormat );
		gl::ScopedFramebuffer scopedFramebuffer( mFboPingPong );
		gl::viewport( mFboPingPong->getSize() );
		gl::clear();
	}

	// Set up the G-buffer buffer
	// 0 GL_DEPTH_ATTACHMENT	Depth
	// 1 GL_COLOR_ATTACHMENT0	Albedo
	// 2 GL_COLOR_ATTACHMENT1	Material ID
	// 3 GL_COLOR_ATTACHMENT2	Encoded normals
	{
		mTextureFboGBuffer[ 0 ] = gl::Texture2d::create( w, h, 
														 gl::Texture2d::Format()
														 .internalFormat( GL_DEPTH_COMPONENT32F )
														 .magFilter( GL_LINEAR )
														 .minFilter( GL_LINEAR )
														 .wrap( GL_CLAMP_TO_EDGE )
														 .dataType( GL_FLOAT ) );
		mTextureFboGBuffer[ 1 ] = gl::Texture2d::create( w, h, textureFormat );
		mTextureFboGBuffer[ 2 ] = gl::Texture2d::create( w, h, 
														 gl::Texture2d::Format()
														 .internalFormat( GL_R8I )
														 .magFilter( GL_NEAREST )
														 .minFilter( GL_NEAREST )
														 .wrap( GL_CLAMP_TO_EDGE )
														 .dataType( GL_BYTE ) );
		mTextureFboGBuffer[ 3 ] = gl::Texture2d::create( w, h, 
														 gl::Texture2d::Format()
														 .internalFormat( GL_RG8 )
														 .magFilter( GL_NEAREST )
														 .minFilter( GL_NEAREST )
														 .wrap( GL_CLAMP_TO_EDGE )
														 .dataType( GL_BYTE ) );
		gl::Fbo::Format fboFormat;
		fboFormat.attachment( GL_DEPTH_ATTACHMENT, mTextureFboGBuffer[ 0 ] );
		for ( size_t i = 1; i <= 3; ++i ) {
			 fboFormat.attachment( GL_COLOR_ATTACHMENT0 + ( i - 1 ), mTextureFboGBuffer[ i ] );
		}
		mFboGBuffer = gl::Fbo::create( w, h, fboFormat );
		gl::ScopedFramebuffer scopedFramebuffer( mFboGBuffer );
		gl::viewport( mFboGBuffer->getSize() );
		gl::clear();
	}
	
	// Half-size framebuffer
	// 0 GL_COLOR_ATTACHMENT0 Light accumulation
	// 1 GL_COLOR_ATTACHMENT1 Bloom ping
	// 2 GL_COLOR_ATTACHMENT2 Bloom pong
	// 3 GL_COLOR_ATTACHMENT3 SSAO ping
	// 4 GL_COLOR_ATTACHMENT4 SSAO pong
	{
		gl::Fbo::Format fboFormat;
		for ( size_t i = 0; i < 5; ++i ) {
			mTextureFboSmall[ i ] = gl::Texture2d::create( w / 2, h / 2, textureFormat );
			fboFormat.attachment( GL_COLOR_ATTACHMENT0 + i, mTextureFboSmall[ i ] );
		}
		mFboSmall = gl::Fbo::create( w / 2, h / 2, fboFormat );
		gl::ScopedFramebuffer scopedFramebuffer( mFboSmall );
		gl::viewport( mFboSmall->getSize() );
		gl::clear();
	}
}

void DeferredShadingAdvancedApp::screenShot()
{
	writeImage( getAppPath() / fs::path( "frame" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
}

void DeferredShadingAdvancedApp::update()
{
	float e		= (float)getElapsedSeconds();
	mFrameRate	= getAverageFps();

	if ( mFullScreen != isFullScreen() ) {
		setFullScreen( mFullScreen );
	}

	// Update center sphere position
	float ground		= mFloor + 1.0f;
	float dampen		= 0.092f;
	mSpherePosition.y	+= mSphereVelocity;
	if ( mSphereVelocity > 0.0f ) {
		mSphereVelocity *= ( 1.0f - dampen );
	} else if ( mSphereVelocity < 0.0f ) {
		mSphereVelocity *= ( 1.0f + dampen );
	}
	if ( mSpherePosition.y < ground ) {
		mSpherePosition.y = ground;
		mSphereVelocity	= -mSphereVelocity;
	} else if ( mSphereVelocity > 0.0f && mSphereVelocity < 0.02f ) {
		mSphereVelocity	= -0.01f;
	}
	
	// Update light positions
	mCameraLight.setPosition( mMayaCam.getCamera().getEyePoint() );
	if ( mLights.size() >= 8 ) {
		size_t numLights	= 8;
		float t				= e;
		float d				= ( (float)M_PI * 2.0f ) / (float)numLights;
		float r				= 3.5f;
		for ( size_t i = 0; i < numLights; ++i, t += d ) {
			float y			= mFloor + 0.1f;
			float x			= glm::cos( t );
			float z			= glm::sin( t );
			vec3 p			= vec3( x, 0.0f, z ) * r;
			p.y				= y + glm::sin( t + e * (float)M_PI ) * 2.0f;
			if ( p.y < y ) {
				p.y			+= ( y - p.y ) * 2.0f;
			}
			mLights.at( i ).setPosition( p );
		}
	}
}

CINDER_APP( DeferredShadingAdvancedApp, RendererGl( RendererGl::Options().msaa( 0 ).coreProfile( true ).version( 3, 3 ) ), 
			[]( App::Settings* settings )
{
	settings->disableFrameRate();
	settings->setWindowSize( 1280, 720 );
} )
 