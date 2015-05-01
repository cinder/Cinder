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
#include "cinder/CameraUi.h"
#include "cinder/params/Params.h"
#include "cinder/app/RendererGl.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"

#include "Light.h"
#include "Material.h"

class DeferredShadingAdvancedApp : public ci::app::App
{
  public:
	DeferredShadingAdvancedApp();

	void						draw() override;

	void						resize() override;
	void						update() override;

  private:
	ci::CameraPersp				mCamera;
	ci::CameraUi				mCamUi;

	std::vector<Light>			mLights;
	std::vector<Material>		mMaterials;

	ci::gl::UboRef				mUboMaterial;

	ci::gl::FboRef				mFboGBuffer;
	ci::gl::FboRef				mFboPingPong;
	ci::gl::FboRef				mFboShadowMap;
	ci::gl::FboRef				mFboSmall;
	
	ci::gl::Texture2dRef		mTextureFboGBuffer[ 3 ];
	ci::gl::Texture2dRef		mTextureFboPingPong[ 2 ];
	ci::gl::Texture2dRef		mTextureFboSmall[ 5 ];
	ci::gl::Texture2dRef		mTextureFboShadowMap;
	ci::gl::TextureRef			mTextureRandom;

	void						loadShaders();
	ci::gl::BatchRef			mBatchBlurRect;
	ci::gl::BatchRef			mBatchColorRect;
	ci::gl::BatchRef			mBatchCompositeRect;
	ci::gl::BatchRef			mBatchDebugRect;
	ci::gl::BatchRef			mBatchDofRect;
	ci::gl::BatchRef			mBatchEmissiveRect;
	ci::gl::BatchRef			mBatchFxaaRect;
	ci::gl::BatchRef			mBatchGBufferCircle;
	ci::gl::BatchRef			mBatchGBufferIcosahedron;
	ci::gl::BatchRef			mBatchGBufferSphere;
	ci::gl::BatchRef			mBatchLBufferCube;
	ci::gl::BatchRef			mBatchLightRect;
	ci::gl::BatchRef			mBatchShadowIcosahedron;
	ci::gl::BatchRef			mBatchShadowSphere;
	ci::gl::BatchRef			mBatchSsaoRect;
	ci::gl::BatchRef			mBatchStockColorCircle;
	ci::gl::BatchRef			mBatchStockColorRect;
	ci::gl::BatchRef			mBatchStockColorSphere;
	ci::gl::BatchRef			mBatchStockColorSphereLow;
	ci::gl::BatchRef			mBatchStockTextureRect;

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
	bool						mQuit;
	void						screenShot();
};

using namespace ci;
using namespace ci::app;
using namespace std;

DeferredShadingAdvancedApp::DeferredShadingAdvancedApp()
{
	gl::enableVerticalSync();
		
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
	mQuit				= false;
	mSpherePosition		= vec3( 0.0f, -4.5f, 0.0f );
	mSphereVelocity		= -0.1f;
	mTextureRandom		= gl::Texture::create( loadImage( loadAsset( "random.png" ) ) );

	// Set up lights
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
	mCamera = CameraPersp( windowSize.x, windowSize.y, 60.0f, 1.0f, 100.0f );
	mCamera.lookAt( vec3( 2.664f, -6.484f, 5.939f ), vec3( 0.469f, -5.430f, 1.146f ) );
	mCamUi = CameraUi( &mCamera, getWindow(), -1 );

	// Set up parameters
	mParams = params::InterfaceGl::create( "Params", ivec2( 220, 300 ) );
	mParams->addParam( "Frame rate",		&mFrameRate,				"", true );
	mParams->addParam( "Fullscreen",		&mFullScreen ).key( "f" );
	mParams->addButton( "Load shaders",		[ & ]() { loadShaders(); },	"key=l" );
	mParams->addButton( "Screen shot",		[ & ]() { screenShot(); },	"key=space" );
	mParams->addParam( "Quit",				&mQuit ).key( "q" );
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
		size_t sz = 2048;
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
		gl::ScopedFramebuffer scopedFramebuffer( mFboShadowMap );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboShadowMap->getSize() );
		gl::clear();
	}
	
	// Set up shadow camera
	mShadowCamera.setPerspective( 120.0f, mFboShadowMap->getAspectRatio(),
								 mCamera.getNearClip(),
								 mCamera.getFarClip() );
	mShadowCamera.lookAt( vec3( 0.0 ), vec3( 0.0f, mFloor, 0.0f ) );
	
	// Load shaders and create batches
	loadShaders();

	// Call resize to create FBOs
	resize();
}

void DeferredShadingAdvancedApp::draw()
{
	gl::disableAlphaBlending();

	float e							= (float)getElapsedSeconds();
	const vec2 winSize				= vec2( getWindowSize() );
	const mat4 shadowMatrix			= mShadowCamera.getProjectionMatrix() * mShadowCamera.getViewMatrix();
	const mat4 projMatrixInverse	= glm::inverse( mCamera.getProjectionMatrix() );
	float nearClip					= mCamera.getNearClip();
	float farClip					= mCamera.getFarClip();
	vec2 projectionParams			= vec2( farClip / ( farClip - nearClip ), 
											( -farClip * nearClip ) / ( farClip - nearClip ) );
	
	vector<mat4> icosahedrons;
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
		
		icosahedrons.push_back( m );
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////

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
		
		{
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( mSpherePosition );
			mBatchShadowSphere->draw();
		}
		for ( const mat4& m : icosahedrons ) {
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::multModelMatrix( m );
			mBatchShadowIcosahedron->draw();
		}
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
		gl::setMatrices( mCamera );
		gl::enableDepthRead( true );
		gl::enableDepthWrite( true );

		////// BEGIN DRAW STUFF ////////////////////////////////////////////////
		
		// Draw floor
		mBatchGBufferCircle->getGlslProg()->uniform( "uMaterialId", 1 );
		{
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( vec3( 0.0f, mFloor, 0.0f ) );
			gl::rotate( quat( vec3( 4.71f, 0.0f, 0.0f ) ) );
			gl::scale( vec3( 10.0f ) );
			mBatchGBufferCircle->draw();
		}

		// Draw shadow casters
		mBatchGBufferSphere->getGlslProg()->uniform( "uMaterialId", 0 );
		{
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( mSpherePosition );
			mBatchGBufferSphere->draw();
		}
		mBatchGBufferIcosahedron->getGlslProg()->uniform( "uMaterialId", 0 );
		for ( const mat4& m : icosahedrons ) {
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::multModelMatrix( m );
			mBatchGBufferIcosahedron->draw();
		}

		// Draw light sources
		mBatchGBufferSphere->getGlslProg()->uniform( "uMaterialId", 2 );
		for ( const Light& light : mLights ) {
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::ScopedColor scopedColor( light.getColorDiffuse() );
			gl::translate( light.getPosition() );
			gl::scale( vec3( light.getRadius() ) );
			mBatchGBufferSphere->draw();
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
		gl::ScopedFaceCulling scopedFaceCulling( true, GL_FRONT );
		gl::setMatrices( mCamera );
		gl::enableDepthRead();
		gl::disableDepthWrite();
		
		// Bind G-buffer textures and shadow map
		gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBuffer[ 0 ],			0 );
		gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBuffer[ 1 ],			1 );
		gl::ScopedTextureBind scopedTextureBind3( mTextureFboGBuffer[ 2 ],			2 );
		gl::ScopedTextureBind scopedTextureBind0( mFboGBuffer->getDepthTexture(),	3 );
		gl::ScopedTextureBind scopedTextureBind4( mTextureFboShadowMap,				4 );

		// Draw light volumes
		mBatchLBufferCube->getGlslProg()->uniform( "uProjMatrixInverse",	projMatrixInverse );
		mBatchLBufferCube->getGlslProg()->uniform( "uProjectionParams",		projectionParams );
		mBatchLBufferCube->getGlslProg()->uniform( "uShadowEnabled",		mEnabledShadow );
		mBatchLBufferCube->getGlslProg()->uniform( "uShadowMatrix",			shadowMatrix );
		mBatchLBufferCube->getGlslProg()->uniform( "uViewMatrixInverse",	mCamera.getInverseViewMatrix() );
		mBatchLBufferCube->getGlslProg()->uniform( "uWindowSize",			vec2( mFboPingPong->getSize() ) );
		
		for ( const Light& light : mLights ) {
			vec3 p = vec3( ( mCamera.getViewMatrix() * vec4( light.getPosition(), 1.0 ) ) );
					
			mBatchLBufferCube->getGlslProg()->uniform( "uLightColorAmbient",	light.getColorAmbient() );
			mBatchLBufferCube->getGlslProg()->uniform( "uLightColorDiffuse",	light.getColorDiffuse() );
			mBatchLBufferCube->getGlslProg()->uniform( "uLightColorSpecular",	light.getColorSpecular() );
			mBatchLBufferCube->getGlslProg()->uniform( "uLightPosition",		p );
			mBatchLBufferCube->getGlslProg()->uniform( "uLightIntensity",		light.getIntensity() );
			mBatchLBufferCube->getGlslProg()->uniform( "uLightRadius",			light.getVolume() );
					
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( light.getPosition() );
			gl::scale( vec3( light.getVolume() ) );
			mBatchLBufferCube->draw();
		}

		ping = pong;
		pong = ( pong + 1 ) % 2;
	}

	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFboSmall );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboSmall->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::setMatricesWindow( mFboSmall->getSize() );
		gl::disableDepthRead();
		gl::disableDepthWrite();		
		gl::translate( mFboSmall->getSize() / 2 );
		gl::scale( mFboSmall->getSize() );

		////////////////////////////////////////////////////////////////////////////////////////////
		// LIGHT ACCUMULATION AND BLOOM

		// Light accumulation
		gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
				
		// Dim last frame (produces light trails)
		{
			gl::ScopedAlphaBlend scopedAlphaBlend( false );
			gl::ScopedColor scopedColor( ColorAf( Colorf::black(), 0.43f ) );
			mBatchStockColorRect->draw();
		}
		{
			gl::ScopedAdditiveBlend scopedAdditiveBlend;
			gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBuffer[ 0 ], 0 );
			gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBuffer[ 1 ], 1 );
			mBatchEmissiveRect->draw();
		}

		// Bloom
		if ( mEnabledBloom ) {
			float bloomAtt	= 1.0f;
			vec2 bloomSize	= vec2( 1.0f ) / winSize * 3.0f;
			bloomSize		*= vec2( mFboSmall->getSize() ) / winSize;
			
			// Horizontal pass
			mBatchBlurRect->getGlslProg()->uniform( "uAttenuation",	bloomAtt );
			gl::drawBuffer( GL_COLOR_ATTACHMENT1 );	
			mBatchBlurRect->getGlslProg()->uniform( "uSize", vec2( bloomSize.x, 0.0f ) );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ 0 ], 0 );
				mBatchBlurRect->draw();
			}
			
			// Vertical pass
			gl::drawBuffer( GL_COLOR_ATTACHMENT2 );
			mBatchBlurRect->getGlslProg()->uniform( "uSize", vec2( 0.0f, bloomSize.y ) );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ 1 ], 0 );
				mBatchBlurRect->draw();
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// SSAO

		if ( mEnabledSsao ) {
			gl::ScopedAlphaBlend scopedAlphaBlend( true );
			
			// SSAO pass
			{
				gl::drawBuffer( GL_COLOR_ATTACHMENT3 );
				gl::ScopedTextureBind scopedTextureBind0( mFboGBuffer->getDepthTexture(),	0 );
				gl::ScopedTextureBind scopedTextureBind1( mTextureRandom,					1 );
				gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBuffer[ 2 ],			2 );
				mBatchSsaoRect->draw();
			}

			// Calculate blur pixel size
			vec2 ssaoBlurSize	= vec2( 1.0f ) / winSize;
			ssaoBlurSize		*= vec2( mFboSmall->getSize() ) / winSize;

			// Horizontal blur pass
			mBatchBlurRect->getGlslProg()->uniform( "uAttenuation",	1.0f );
			mBatchBlurRect->getGlslProg()->uniform( "uSize",		vec2( ssaoBlurSize.x, 0.0f ) );
			gl::drawBuffer( GL_COLOR_ATTACHMENT4 );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ 2 ], 0 );
				mBatchBlurRect->draw();
			}

			// Vertical blur pass
			mBatchBlurRect->getGlslProg()->uniform( "uSize", vec2( 0.0f, ssaoBlurSize.y ) );
			gl::drawBuffer( GL_COLOR_ATTACHMENT3 );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ 3 ], 0 );
				mBatchBlurRect->draw();
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
			gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBuffer[ 0 ],			0 );
			gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBuffer[ 1 ],			1 );
			gl::ScopedTextureBind scopedTextureBind3( mTextureFboGBuffer[ 2 ],			2 );
			gl::ScopedTextureBind scopedTextureBind0( mFboGBuffer->getDepthTexture(),	3 );

			mBatchDebugRect->getGlslProg()->uniform( "uProjectionParams",	projectionParams );
			mBatchDebugRect->getGlslProg()->uniform( "uProjMatrixInverse",	projMatrixInverse );
			for ( int32_t i = 0; i < 10; ++i, ++index ) {
				vec2 pos = calcPosition( index );
				gl::ScopedModelMatrix scopedModelMatrix;
				gl::translate( pos + sz * 0.5f );
				gl::scale( sz );
				mBatchDebugRect->getGlslProg()->uniform( "uMode", i );
				mBatchDebugRect->draw();
			}
		}

		// Bloom
		{
			gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ mEnabledBloom ? 2 : 0 ], 0 );
			vec2 pos = calcPosition( index );
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( pos + sz * 0.5f );
			gl::scale( sz );
			mBatchStockTextureRect->draw();
			++index;
		}

		// SSAO
		{
			gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ 3 ], 0 );
			vec2 pos = calcPosition( index );
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( pos + sz * 0.5f );
			gl::scale( sz );
			mBatchStockTextureRect->draw();
		}
	} else {
		
		//////////////////////////////////////////////////////////////////////////////////////////////
		// COMPOSITE
		
		{
			gl::ScopedFramebuffer scopedFrameBuffer( mFboPingPong );
			gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboPingPong->getSize() );
			gl::ScopedMatrices scopedMatrices;
			gl::setMatricesWindow( mFboPingPong->getSize() );
			gl::translate( mFboPingPong->getSize() / 2 );
			gl::scale( mFboPingPong->getSize() );
			gl::disableDepthRead();
			gl::disableDepthWrite();

			{
				// Blend L-buffer and SSAO
				gl::drawBuffer( GL_COLOR_ATTACHMENT0 + ping );
				if ( mEnabledSsao ) {
					gl::ScopedTextureBind scopedTextureBind0( mTextureFboPingPong[ pong ],	0 );
					gl::ScopedTextureBind scopedTextureBind1( mTextureFboSmall[ 3 ],		1 );
					mBatchCompositeRect->draw();
				} else {
					gl::ScopedTextureBind scopedTextureBind( mTextureFboPingPong[ pong ], 0 );
					mBatchStockTextureRect->draw();
				}

				ping = pong;
				pong = ( pong + 1 ) % 2;
			}
			
			//////////////////////////////////////////////////////////////////////////////////////////////
			// DEPTH OF FIELD
			
			if ( mEnabledDoF ) {
				gl::drawBuffer( GL_COLOR_ATTACHMENT0 + ping );
				gl::ScopedTextureBind scopedTextureBind0( mFboGBuffer->getDepthTexture(),	0 );
				gl::ScopedTextureBind scopedTextureBind1( mTextureFboPingPong[ pong ],		1 );
				mBatchDofRect->getGlslProg()->uniform( "uAspect", mTextureFboPingPong[ pong ]->getAspectRatio() );
				mBatchDofRect->draw();

				ping = pong;
				pong = ( pong + 1 ) % 2;
			}
			
			//////////////////////////////////////////////////////////////////////////////////////////////
			// COLOR
			
			if ( mEnabledColor ) {
				gl::drawBuffer( GL_COLOR_ATTACHMENT0 + ping );
				gl::ScopedTextureBind scopedTextureBind( mTextureFboPingPong[ pong ], 0 );
				mBatchColorRect->draw();
				
				ping = pong;
				pong = ( pong + 1 ) % 2;
			}
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////
		// RENDER TO SCREEN
		
		gl::ScopedViewport scopedViewport( ivec2( 0 ), getWindowSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::setMatricesWindow( getWindowSize() );
		gl::translate( getWindowCenter() );
		gl::scale( getWindowSize() );
		gl::disableDepthRead();
		gl::disableDepthWrite();
		
		// Perform FXAA
		{
			gl::ScopedTextureBind scopedTextureBind( mTextureFboPingPong[ pong ], 0 );
			if ( mEnabledFxaa ) {
				mBatchFxaaRect->getGlslProg()->uniform( "uPixel",	vec2( 1.0f ) / winSize );
				mBatchFxaaRect->draw();
			} else {
				mBatchStockTextureRect->draw();
			}
		}

		// Draw light accumulation
		{
			gl::ScopedAdditiveBlend scopedAdditiveBlend;
			gl::ScopedTextureBind scopedTextureBind( mTextureFboSmall[ mEnabledBloom ? 2 : 0 ], 0 );
			mBatchStockTextureRect->draw();
		}

		// Draw light volumes
		if ( mDrawLightVolume ) {
			gl::ScopedAlphaBlend scopedAlphaBlend( false );
			gl::ScopedPolygonMode scopedPolygonMode( GL_LINE );
			gl::ScopedMatrices scopedMatrices;
			gl::setMatrices( mCamera );

			for ( const Light& light : mLights ) {
				gl::ScopedModelMatrix scopedModelMatrix;
				gl::ScopedColor scopedColor( light.getColorDiffuse() * ColorAf( Colorf::white(), 0.08f ) );
				gl::translate( light.getPosition() );
				gl::scale( vec3( light.getVolume() ) );
				mBatchStockColorSphereLow->draw();
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
			glslProg = gl::GlslProg::create( gl::GlslProg::Format().version( 330 ).vertex( vertex ).fragment( fragment ) );
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

	gl::VboMeshRef circle		= gl::VboMesh::create( geom::Circle().subdivisions( 64 ) );
	gl::VboMeshRef cube			= gl::VboMesh::create( geom::Cube() );
	gl::VboMeshRef icosahedron	= gl::VboMesh::create( geom::Icosahedron() );
	gl::VboMeshRef rect			= gl::VboMesh::create( geom::Rect() );
	gl::VboMeshRef sphere		= gl::VboMesh::create( geom::Sphere().subdivisions( 64 ) );
	gl::VboMeshRef sphereLow	= gl::VboMesh::create( geom::Sphere().subdivisions( 12 ) );

	// Load shaders
	DataSourceRef passThrough		= loadAsset( "pass_through.vert" );
	gl::GlslProgRef blur			= loadGlslProg( "Blur",				passThrough,					loadAsset( "blur.frag" ) );
	gl::GlslProgRef color			= loadGlslProg( "Color",			passThrough,					loadAsset( "color.frag" ) );
	gl::GlslProgRef composite		= loadGlslProg( "Composite",		passThrough,					loadAsset( "composite.frag" ) );
	gl::GlslProgRef debug			= loadGlslProg( "Debug",			passThrough,					loadAsset( "debug.frag" ) );
	gl::GlslProgRef dof				= loadGlslProg( "Depth of field",	passThrough,					loadAsset( "dof.frag" ) );
	gl::GlslProgRef emissive		= loadGlslProg( "Emissive",			passThrough,					loadAsset( "emissive.frag" ) );
	gl::GlslProgRef fxaa			= loadGlslProg( "FXAA",				passThrough,					loadAsset( "fxaa.frag" ) );
	gl::GlslProgRef gBuffer			= loadGlslProg( "G-buffer",			loadAsset( "gbuffer.vert" ),	loadAsset( "gbuffer.frag" ) );
	gl::GlslProgRef lBuffer			= loadGlslProg( "L-buffer",			passThrough,					loadAsset( "lbuffer.frag" ) );
	gl::GlslProgRef shadowMap		= loadGlslProg( "Shadow map",		loadAsset( "shadow_map.vert" ),	loadAsset( "shadow_map.frag" ) );
	gl::GlslProgRef ssao			= loadGlslProg( "SSAO",				loadAsset( "ssao.vert" ),		loadAsset( "ssao.frag" ) );
	gl::GlslProgRef stockColor		= gl::context()->getStockShader( gl::ShaderDef().color() );
	gl::GlslProgRef stockTexture	= gl::context()->getStockShader( gl::ShaderDef().texture( GL_TEXTURE_2D ) );

	// Create geometry + shader batches
	mBatchBlurRect				= gl::Batch::create( rect,			blur );
	mBatchColorRect				= gl::Batch::create( rect,			color );
	mBatchCompositeRect			= gl::Batch::create( rect,			composite );
	mBatchDebugRect				= gl::Batch::create( rect,			debug );
	mBatchDofRect				= gl::Batch::create( rect,			dof );
	mBatchEmissiveRect			= gl::Batch::create( rect,			emissive );
	mBatchFxaaRect				= gl::Batch::create( rect,			fxaa );
	mBatchGBufferCircle			= gl::Batch::create( circle,		gBuffer );
	mBatchGBufferIcosahedron	= gl::Batch::create( icosahedron,	gBuffer );
	mBatchGBufferSphere			= gl::Batch::create( sphere,		gBuffer );
	mBatchLBufferCube			= gl::Batch::create( cube,			lBuffer );
	mBatchShadowIcosahedron		= gl::Batch::create( icosahedron,	shadowMap );
	mBatchShadowSphere			= gl::Batch::create( sphere,		shadowMap );
	mBatchSsaoRect				= gl::Batch::create( rect,			ssao );
	mBatchStockColorCircle		= gl::Batch::create( circle,		stockColor );
	mBatchStockColorRect		= gl::Batch::create( rect,			stockColor );
	mBatchStockColorSphere		= gl::Batch::create( sphere,		stockColor );
	mBatchStockColorSphereLow	= gl::Batch::create( sphereLow,		stockColor );
	mBatchStockTextureRect		= gl::Batch::create( rect,			stockTexture );

	// Set sampler bindings
	mBatchBlurRect->getGlslProg()->uniform(				"uSampler",				0 );
	mBatchColorRect->getGlslProg()->uniform(			"uSampler",				0 );
	mBatchCompositeRect->getGlslProg()->uniform(		"uSamplerLBuffer",		0 );
	mBatchCompositeRect->getGlslProg()->uniform(		"uSamplerSsao",			1 );
	mBatchDebugRect->getGlslProg()->uniform(			"uSamplerAlbedo",		0 );
	mBatchDebugRect->getGlslProg()->uniform(			"uSamplerMaterial",		1 );
	mBatchDebugRect->getGlslProg()->uniform(			"uSamplerNormal",		2 );
	mBatchDebugRect->getGlslProg()->uniform(			"uSamplerDepth",		3 );
	mBatchDofRect->getGlslProg()->uniform(				"uSamplerDepth",		0 );
	mBatchDofRect->getGlslProg()->uniform(				"uSamplerColor",		1 );
	mBatchEmissiveRect->getGlslProg()->uniform(			"uSamplerAlbedo",		0 );
	mBatchEmissiveRect->getGlslProg()->uniform(			"uSamplerMaterial",		1 );
	mBatchFxaaRect->getGlslProg()->uniform(				"uSampler",				0 );
	mBatchLBufferCube->getGlslProg()->uniform(			"uSamplerAlbedo",		0 );
	mBatchLBufferCube->getGlslProg()->uniform(			"uSamplerMaterial",		1 );
	mBatchLBufferCube->getGlslProg()->uniform(			"uSamplerNormal",		2 );
	mBatchLBufferCube->getGlslProg()->uniform(			"uSamplerDepth",		3 );
	mBatchLBufferCube->getGlslProg()->uniform(			"uSamplerShadowMap",	4 );
	mBatchSsaoRect->getGlslProg()->uniform(				"uSamplerDepth",		0 );
	mBatchSsaoRect->getGlslProg()->uniform(				"uSamplerNoise",		1 );
	mBatchSsaoRect->getGlslProg()->uniform(				"uSamplerNormal",		2 );
	
	// Bind material uniform buffer to shaders
	mBatchDebugRect->getGlslProg()->uniformBlock( 0, 0 );
	mBatchEmissiveRect->getGlslProg()->uniformBlock( 0, 0 );
	mBatchLBufferCube->getGlslProg()->uniformBlock( 0, 0 );
}

void DeferredShadingAdvancedApp::resize()
{
	mCamera.setAspectRatio( getWindowAspectRatio() );

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
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboPingPong->getSize() );
		gl::clear();
	}

	// Set up the G-buffer buffer
	// 0 GL_COLOR_ATTACHMENT0	Albedo
	// 1 GL_COLOR_ATTACHMENT1	Material ID
	// 2 GL_COLOR_ATTACHMENT2	Encoded normals
	{
		mTextureFboGBuffer[ 0 ] = gl::Texture2d::create( w, h, textureFormat );
		mTextureFboGBuffer[ 1 ] = gl::Texture2d::create( w, h, 
														 gl::Texture2d::Format()
														 .internalFormat( GL_R8I )
														 .magFilter( GL_NEAREST )
														 .minFilter( GL_NEAREST )
														 .wrap( GL_CLAMP_TO_EDGE )
														 .dataType( GL_BYTE ) );
		mTextureFboGBuffer[ 2 ] = gl::Texture2d::create( w, h, 
														 gl::Texture2d::Format()
														 .internalFormat( GL_RG8 )
														 .magFilter( GL_NEAREST )
														 .minFilter( GL_NEAREST )
														 .wrap( GL_CLAMP_TO_EDGE )
														 .dataType( GL_BYTE ) );
		gl::Fbo::Format fboFormat;
		fboFormat.depthTexture( gl::Texture2d::Format()
								.internalFormat( GL_DEPTH_COMPONENT32F )
								.magFilter( GL_LINEAR )
								.minFilter( GL_LINEAR )
								.wrap( GL_CLAMP_TO_EDGE )
								.dataType( GL_FLOAT ) );
		for ( size_t i = 0; i < 3; ++i ) {
			 fboFormat.attachment( GL_COLOR_ATTACHMENT0 + i, mTextureFboGBuffer[ i ] );
		}
		try {
			mFboGBuffer = gl::Fbo::create( w, h, fboFormat );
			gl::ScopedFramebuffer scopedFramebuffer( mFboGBuffer );
			gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboGBuffer->getSize() );
			gl::clear();
		} catch ( gl::FboExceptionInvalidSpecification ex ) {
			console() << ex.what() << endl;
			quit();
			return;
		}
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
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboSmall->getSize() );
		gl::clear();
	}
}

void DeferredShadingAdvancedApp::screenShot()
{
	writeImage( getAppPath() / fs::path( "frame" + toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
}

void DeferredShadingAdvancedApp::update()
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
 