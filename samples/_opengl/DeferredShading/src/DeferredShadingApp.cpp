#include "cinder/app/App.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Ubo.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/Json.h"
#include "cinder/MayaCamUI.h"
#include "cinder/params/Params.h"

#include "Light.h"
#include "Material.h"

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
	ci::MayaCamUI				mMayaCam;

	std::vector<ci::mat4>		mCubes;
	std::vector<Light>			mLights;
	std::vector<Material>		mMaterials;

	ci::gl::UboRef				mUboMaterial;
	
	ci::gl::GlslProgRef			mGlslProgBlend;
	ci::gl::GlslProgRef			mGlslProgBloom;
	ci::gl::GlslProgRef			mGlslProgBlur;
	ci::gl::GlslProgRef			mGlslProgColor;
	ci::gl::GlslProgRef			mGlslProgComposite;
	ci::gl::GlslProgRef			mGlslProgCubeShadow;
	ci::gl::GlslProgRef			mGlslProgDebugGbuffer;
	ci::gl::GlslProgRef			mGlslProgDebugMaterial;
	ci::gl::GlslProgRef			mGlslProgDof;
	ci::gl::GlslProgRef			mGlslProgFxaa;
	ci::gl::GlslProgRef			mGlslProgGBuffer;
	ci::gl::GlslProgRef			mGlslProgGBufferLight;
	ci::gl::GlslProgRef			mGlslProgLightDirectional;
	ci::gl::GlslProgRef			mGlslProgLight;
	ci::gl::GlslProgRef			mGlslProgShadowMap;
	ci::gl::GlslProgRef			mGlslProgSsao;
	ci::gl::GlslProgRef			mGlslProgStockColor;
	ci::gl::GlslProgRef			mGlslProgStockTexture;

	ci::gl::FboRef				mFboBloom;
	ci::gl::FboRef				mFboColor;
	ci::gl::FboRef				mFboComposite;
	ci::gl::FboRef				mFboDof;
	ci::gl::FboRef				mFboGBuffer;
	ci::gl::FboRef				mFboLBuffer;
	ci::gl::FboRef				mFboShadowMap;
	ci::gl::FboRef				mFboSsao;
	
	ci::gl::TextureRef			mTextureCube;
	ci::gl::TextureRef			mTextureRandom;
	ci::gl::Texture2dRef		mTextureFboBloomHorizontal;
	ci::gl::Texture2dRef		mTextureFboBloomVertical;
	ci::gl::Texture2dRef		mTextureFboColor;
	ci::gl::Texture2dRef		mTextureFboComposite;
	ci::gl::Texture2dRef		mTextureFboDofHorizontal;
	ci::gl::Texture2dRef		mTextureFboDofVertical;
	ci::gl::Texture2dRef		mTextureFboGBufferAlbedo;
	ci::gl::Texture2dRef		mTextureFboGBufferDepth;
	ci::gl::Texture2dRef		mTextureFboGBufferMaterial;
	ci::gl::Texture2dRef		mTextureFboGBufferNormalDepth;
	ci::gl::Texture2dRef		mTextureFboGBufferPosition;
	ci::gl::Texture2dRef		mTextureFboLBuffer;
	ci::gl::Texture2dRef		mTextureFboShadowMap;
	ci::gl::Texture2dRef		mTextureFboSsaoHorizontal;
	ci::gl::Texture2dRef		mTextureFboSsaoVertical;
	ci::gl::Texture2dRef		mTextureFboSsao;

	ci::gl::VboMeshRef			mMeshCube;
	ci::gl::VboMeshRef			mMeshRect;
	ci::gl::VboMeshRef			mMeshSphere;

	void						drawRect( const ci::ivec2& sz );
	void						drawRect( const ci::vec2& pos, const ci::ivec2& sz );

	float						mDepthScale;

	ci::CameraPersp				mShadowCamera;
	
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
	const mat4 shadowMatrix = mShadowCamera.getProjectionMatrix() * mShadowCamera.getViewMatrix();
	vec2 winSize			= vec2( getWindowSize() );

	//////////////////////////////////////////////////////////////////////////////////////////////
	// LOCAL METHODS
	
	// Shortcut for configuring window
	enum : size_t
	{
		NONE, ADDITIVE, ALPHA
	} typedef BlendMode;
	auto setupWindow = [ & ]( const ivec2& sz, BlendMode blendMode, 
							  bool enableDepthRead, bool enableDepthWrite )
	{
		gl::viewport( sz );
		gl::clear();
		switch ( blendMode ) {
		case BlendMode::ADDITIVE:
			gl::enableAdditiveBlending();
			break;
		case BlendMode::ALPHA:
			gl::enableAlphaBlending();
			break;
		default:
			gl::disableAlphaBlending();
			break;
		}
		gl::enableDepthRead( enableDepthRead );
		gl::enableDepthWrite( enableDepthWrite );
		gl::color( Colorf::white() );
	};

	// Draw cubes
	auto drawCubes = [ & ]()
	{
		for ( const mat4& m : mCubes ) {
			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( vec3( 0.0f, -3.0f, 0.0 ) );
			{
				gl::ScopedModelMatrix scopedModelMatrix;
				gl::multModelMatrix( m );
				gl::draw( mMeshCube );
			}
		}
	};

	// Sets light uniforms for a light shader
	auto setLightUniforms = [ & ]( gl::GlslProgRef& glsl, const Light& light ) -> void
	{
		glsl->uniform( "uLightColorAmbient",	light.getColorAmbient() );
		glsl->uniform( "uLightColorDiffuse",	light.getColorDiffuse() );
		glsl->uniform( "uLightColorSpecular",	light.getColorSpecular() );
	};

	// Sets shadow uniforms for a light shader
	auto setShadowUniforms = [ & ]( gl::GlslProgRef& glsl ) -> void
	{
		glsl->uniform( "uShadowBlurSize",		0.0025f );
		glsl->uniform( "uShadowMatrix",			shadowMatrix );
		glsl->uniform( "uShadowMix",			0.5f );
		glsl->uniform( "uShadowSamples",		8.0f );
		glsl->uniform( "uViewMatrixInverse",	mMayaCam.getCamera().getInverseViewMatrix() );
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
		setupWindow( mFboGBuffer->getSize(), BlendMode::NONE, true, true );
		gl::setMatrices( mMayaCam.getCamera() );

		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgGBuffer );
			mGlslProgGBuffer->uniform( "uSampler",		0 );

			// Draw cubes
			mGlslProgGBuffer->uniform( "uMaterialId",	0 );
			mGlslProgGBuffer->uniform( "uSamplerMix",	0.7f );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureCube );
				drawCubes();
			}
	
			// Draw floor
			gl::ScopedModelMatrix scopedModelMatrix;
			mGlslProgGBuffer->uniform( "uDepthScale",	mDepthScale );
			mGlslProgGBuffer->uniform( "uMaterialId",	1 );
			mGlslProgGBuffer->uniform( "uSamplerMix",	0.0f );
			gl::translate( vec3( 0.0f, -7.0f, 0.0f ) );
			gl::rotate( quat( vec3( 4.71f, 0.0f, 0.0f ) ) );
			gl::scale( vec3( 125.0f ) );
			gl::draw( mMeshRect );
		}

		// Draw light sources
		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgGBufferLight );
			mGlslProgGBufferLight->uniform( "uDepthScale", mDepthScale );
			mGlslProgGBufferLight->uniform( "uMaterialId", 2 );
			for ( const Light& light : mLights ) {
				gl::ScopedModelMatrix scopedModelMatrix;
				gl::ScopedColor scopedColor( light.getColorDiffuse() );
				gl::translate( light.getPosition() );
				gl::scale( vec3( light.getRadius() ) );
				gl::draw( mMeshSphere );
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// SHADOW MAP

	// Draw cubes into FBO from view of shadow camera
	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFboShadowMap );
		setupWindow( mFboShadowMap->getSize(), BlendMode::NONE, true, true );
		gl::setMatrices( mShadowCamera );
		gl::ScopedGlslProg scopedGlslProg( mGlslProgShadowMap );
		gl::ScopedFaceCulling scopedFaceCulling( true, GL_FRONT );
		gl::ScopedFrontFace scopedFrontFace( GL_CW );
		drawCubes();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// L-BUFFER

	{
		// Set up window and clear buffers
		gl::ScopedFramebuffer scopedFrameBuffer( mFboLBuffer );
		setupWindow( mFboLBuffer->getSize(), BlendMode::ADDITIVE, true, true );
		gl::setMatrices( mMayaCam.getCamera() );

		gl::ScopedState scopedState( GL_DEPTH_TEST, false );
		gl::ScopedFaceCulling scopedFaceCulling( true, GL_FRONT );
	
		// Bind G-buffer textures and shadow map
		gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBufferAlbedo,			0 );
		gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBufferMaterial,		1 );
		gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBufferNormalDepth,	2 );
		gl::ScopedTextureBind scopedTextureBind3( mTextureFboGBufferPosition,		3 );
		gl::ScopedTextureBind scopedTextureBind4( mTextureFboShadowMap,				4 );

		gl::ScopedGlslProg scopedGlslProg( mGlslProgLight );
		mGlslProgLight->uniform( "uSamplerAlbedo",		0 );
		mGlslProgLight->uniform( "uSamplerMaterial",	1 );
		mGlslProgLight->uniform( "uSamplerNormalDepth",	2 );
		mGlslProgLight->uniform( "uSamplerPosition",	3 );
		mGlslProgLight->uniform( "uSamplerShadowMap",	4 );
		mGlslProgLight->uniformBlock( 0, 0 );
		setShadowUniforms( mGlslProgLight );

		for ( const Light& light : mLights ) {
			const vec3 lightPosition = vec3( ( mMayaCam.getCamera().getViewMatrix() * vec4( light.getPosition(), 1.0 ) ) );

			setLightUniforms( mGlslProgLight, light );
			mGlslProgLight->uniform( "uLightPosition",	lightPosition );
			mGlslProgLight->uniform( "uLightIntensity",	light.getIntensity() );
			mGlslProgLight->uniform( "uLightRadius",	light.getVolume() );
			mGlslProgLight->uniform( "uWindowSize",		vec2( getWindowSize() ) );

			gl::ScopedModelMatrix scopedModelMatrix;
			gl::translate( light.getPosition() );
			gl::scale( vec3( light.getVolume() ) );
			gl::draw( mMeshCube );
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// BLOOM

	{
		// Set up window and clear buffers
		gl::ScopedFramebuffer scopedFrameBuffer( mFboBloom );
		{	
			const static GLenum buffers[] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1
			};
			gl::drawBuffers( 2, buffers );
		}
		setupWindow( mFboBloom->getSize(), BlendMode::NONE, false, false );
		gl::setMatricesWindow( mFboBloom->getSize(), false );

		// Calculate bloom pixel size
		float bloomAtt	= 1.5f;
		vec2 bloomSize	= vec2( 1.0f ) / winSize * 3.0f;
		bloomSize		*= vec2( mFboBloom->getSize() ) / winSize;

		// Horizontal pass
		{
			gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
			gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBufferAlbedo,		0 );
			gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBufferMaterial,	1 );
			gl::ScopedGlslProg scopedGlslProg( mGlslProgBloom );
			mGlslProgBloom->uniform( "uAttenuation",		bloomAtt );
			mGlslProgBloom->uniform( "uSize",				vec2( bloomSize.x, 0.0f ) );
			mGlslProgBloom->uniform( "uSamplerAlbedo",		0 );
			mGlslProgBloom->uniform( "uSamplerMaterial",	1 );
			drawRect( mFboBloom->getSize() );
		}

		// Vertical pass
		{
			gl::drawBuffer( GL_COLOR_ATTACHMENT1 );
			gl::ScopedTextureBind scopedTextureBind( mTextureFboBloomHorizontal, 0 );
			gl::ScopedGlslProg scopedGlslProg( mGlslProgBlur );
			mGlslProgBlur->uniform( "uAttenuation",	bloomAtt );
			mGlslProgBlur->uniform( "uSize",		vec2( 0.0f, bloomSize.y ) );
			mGlslProgBlur->uniform( "uSampler",		0 );
			drawRect( mFboBloom->getSize() );
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// SSAO

	{
		// Set up window and clear buffers
		gl::ScopedFramebuffer scopedFrameBuffer( mFboSsao );
		{
			const static GLenum buffers[] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1, 
				GL_COLOR_ATTACHMENT2
			};
			gl::drawBuffers( 3, buffers );
		}
		setupWindow( mFboSsao->getSize(), BlendMode::ALPHA, false, false );
		gl::setMatricesWindow( mFboSsao->getSize(), false );

		// SSAO pass
		{
			gl::drawBuffer( GL_COLOR_ATTACHMENT2 );
			gl::ScopedTextureBind scopedTextureBind0( mTextureRandom,					0 );
			gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBufferNormalDepth,	1 );
			gl::ScopedGlslProg scopedGlslProg( mGlslProgSsao );
			mGlslProgSsao->uniform( "uFalloff",				0.0f );
			mGlslProgSsao->uniform( "uOffset",				0.05f );
			mGlslProgSsao->uniform( "uRadius",				0.05f );
			mGlslProgSsao->uniform( "uStrength",			1.0f );
			mGlslProgSsao->uniform( "uSamplerNoise",		0 );
			mGlslProgSsao->uniform( "uSamplerNormalDepth",	1 );
			drawRect( mFboSsao->getSize() );
		}

		// Calculate blur pixel size
		vec2 ssaoBlurSize	= vec2( 1.0f ) / winSize * 2.0f;
		ssaoBlurSize		*= vec2( mFboSsao->getSize() ) / winSize;

		// Horizontal blur pass
		gl::ScopedGlslProg scopedGlslProg( mGlslProgBlur );
		mGlslProgBlur->uniform( "uAttenuation",	1.0f );
		mGlslProgBlur->uniform( "uSize",		vec2( ssaoBlurSize.x, 0.0f ) );
		mGlslProgBlur->uniform( "uSampler",		0 );
		gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
		{
			gl::ScopedTextureBind scopedTextureBind( mTextureFboSsao, 0 );
			drawRect( mFboSsao->getSize() );
		}

		// Vertical blur pass
		mGlslProgBlur->uniform( "uSize", vec2( 0.0f, ssaoBlurSize.y ) );
		gl::drawBuffer( GL_COLOR_ATTACHMENT1 );
		{
			gl::ScopedTextureBind scopedTextureBind( mTextureFboSsaoHorizontal, 0 );
			drawRect( mFboSsao->getSize() );
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// COMPOSITE

	// Set up window for composite pass
	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFboComposite );
		setupWindow( mFboComposite->getSize(), BlendMode::NONE, false, false );
		gl::setMatricesWindow( mFboComposite->getSize() );
	
		// Blend L-buffer and shadows
		gl::ScopedTextureBind scopedTextureBind0( mTextureFboLBuffer,		0 );
		gl::ScopedTextureBind scopedTextureBind1( mTextureFboSsaoVertical,	1 );
		gl::ScopedGlslProg scopedGlslProg( mGlslProgComposite );
		mGlslProgComposite->uniform( "uSamplerLBuffer",	0 );
		mGlslProgComposite->uniform( "uSamplerSsao",	1 );
		drawRect( mFboComposite->getSize() );
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// DEPTH OF FIELD

	// Set up window for depth of field pass
	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFboDof );
		{
			const static GLenum buffers[] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1
			};
			gl::drawBuffers( 2, buffers );
		}
		setupWindow( mFboDof->getSize(), BlendMode::NONE, false, false );
		gl::setMatricesWindow( mFboDof->getSize() );

		// Calculate depth of field blur size
		vec2 dofBlurSize	= vec2( 1.0f ) / winSize * 0.5f;
		dofBlurSize			*= vec2( mFboDof->getSize() ) / winSize;

		{
			// Horizontal pass
			gl::ScopedGlslProg scopedGlslProg( mGlslProgDof );
			mGlslProgDof->uniform( "uBias",					0.005f );
			mGlslProgDof->uniform( "uFocalDepth",			0.75f );
			mGlslProgDof->uniform( "uRange",				0.85f );
			mGlslProgDof->uniform( "uSize",					vec2( dofBlurSize.x, 0.0f ) );
			mGlslProgDof->uniform( "uSampler",				0 );
			mGlslProgDof->uniform( "mTextureNormalDepth",	1 );
			gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
			{
				gl::ScopedTextureBind scopedTextureBind0( mTextureFboComposite,				0 );
				gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBufferNormalDepth,	1 );
				drawRect( mFboDof->getSize() );
			}

			// Vertical pass
			mGlslProgDof->uniform( "uSize",	vec2( 0.0f, dofBlurSize.y ) );	
			gl::drawBuffer( GL_COLOR_ATTACHMENT1 );
			gl::ScopedTextureBind scopedTextureBind( mTextureFboDofHorizontal, 0 );
			drawRect( mFboDof->getSize() );
		}
	}

	// Perform a blend pass between composite and DoF buffers
	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFboDof );
		gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
		setupWindow( mFboDof->getSize(), BlendMode::NONE, false, false );
		gl::setMatricesWindow( mFboDof->getSize() );

		gl::ScopedTextureBind scopedTextureBind0( mTextureFboComposite,		0 );
		gl::ScopedTextureBind scopedTextureBind1( mTextureFboDofVertical,	1 );

		gl::ScopedGlslProg scopedGlslProg( mGlslProgBlend );
		mGlslProgBlend->uniform( "uBlend",		0.6f );
		mGlslProgBlend->uniform( "uSampler0",	0 );
		mGlslProgBlend->uniform( "uSampler1",	1 );
		drawRect( mFboDof->getSize() );
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// COLOR

	// Set up window for color processing pass
	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFboColor );
		setupWindow( mFboColor->getSize(), BlendMode::NONE, false, false );
		gl::setMatricesWindow( mFboColor->getSize() );
	
		// Perform color processing pass
		gl::ScopedTextureBind scopedTextureBind( mTextureFboDofHorizontal, 0 );
		gl::ScopedGlslProg scopedGlslProg( mGlslProgColor );
		mGlslProgColor->uniform( "uBlend",			0.5f );
		mGlslProgColor->uniform( "uColorOffset",	0.0015f );
		mGlslProgColor->uniform( "uContrast",		0.5f );
		mGlslProgColor->uniform( "uMultiply",		16.0f );
		mGlslProgColor->uniform( "uSaturation",		ColorAf( 0.882f, 0.89f, 0.843f, 1.0f ) );
		mGlslProgColor->uniform( "uSampler",		0 );
		drawRect( mFboColor->getSize() );
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// FINAL RENDER

	// Set up window for screen render
	setupWindow( getWindowSize(), BlendMode::NONE, false, false );
	gl::setMatricesWindow( getWindowSize() );

	if ( mDebugMode ) {
		gl::viewport( getWindowSize() );
		gl::clear( Colorf::gray( 0.4f ) );
		gl::setMatricesWindow( getWindowSize() );
		gl::ScopedAlphaBlend scopedAlphaBlend( false );
		gl::disableDepthRead();
		gl::disableDepthWrite();
		
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
			gl::disableAlphaBlending();
			gl::ScopedGlslProg scopedGlslProg( mGlslProgDebugGbuffer );
			mGlslProgDebugGbuffer->uniform( "uSamplerAlbedo",		0 );
			mGlslProgDebugGbuffer->uniform( "uSamplerMaterial",		1 );
			mGlslProgDebugGbuffer->uniform( "uSamplerNormalDepth",	2 );
			mGlslProgDebugGbuffer->uniform( "uSamplerPosition",		3 );
			gl::ScopedTextureBind scopedTextureBind0( mTextureFboGBufferAlbedo,			0 );
			gl::ScopedTextureBind scopedTextureBind1( mTextureFboGBufferMaterial,		1 );
			gl::ScopedTextureBind scopedTextureBind2( mTextureFboGBufferNormalDepth,	2 );
			gl::ScopedTextureBind scopedTextureBind3( mTextureFboGBufferPosition,		3 );
		
			for ( int32_t i = 0; i < 5; ++i ) {
				if ( i > 0 ) {
					moveCursor();
				}
				mGlslProgDebugGbuffer->uniform( "uMode", i );
				drawRect( pos, sz );
			}
		}

		// Bloom
		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgStockTexture );
			moveCursor();
			mTextureFboBloomHorizontal->setTopDown( true );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboBloomHorizontal, 0 );
				drawRect( pos, sz );
			}
			moveCursor();
			mTextureFboBloomVertical->setTopDown( true );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboBloomVertical, 0 );
				drawRect( pos, sz );
			}
		}
	
		// SSAO
		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgStockTexture );
			moveCursor();
			mTextureFboSsao->setTopDown( true );
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboSsao, 0 );
				drawRect( pos, sz );
			}
			moveCursor();
			{
				gl::ScopedTextureBind scopedTextureBind( mTextureFboSsaoVertical, 0 );
				drawRect( pos, sz );
			}
		}

		// Material
		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgDebugMaterial );
			gl::ScopedTextureBind scopedTextureBind( mTextureFboGBufferMaterial, 0 );
			mGlslProgDebugMaterial->uniformBlock( 0, 0 );
			mGlslProgDebugMaterial->uniform( "uSamplerMaterial", 0 );
			for ( int32_t i = 0; i < 6; ++i ) {
				moveCursor();
				mGlslProgDebugMaterial->uniform( "uMode", i );
				drawRect( pos, sz );
			}
		}
	} else {

		// Perform FXAA
		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgFxaa );
			gl::ScopedTextureBind scopedTextureBind( mTextureFboColor, 0 );
			mGlslProgFxaa->uniform( "uPixel",	vec2( 1.0f ) / winSize );
			mGlslProgFxaa->uniform( "uSampler",	0 );
			drawRect( getWindowSize() );
		}

		// Draw bloom on top
		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgStockTexture );
			gl::enableAdditiveBlending();
			gl::ScopedTextureBind scopedTextureBind( mTextureFboBloomVertical, 0 );
			drawRect( mFboComposite->getSize() );
			gl::disableAlphaBlending();
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
	const ivec2 windowSizeHalf	= windowSize / 2;

	// Bloom buffer
	{
		gl::Texture2d::Format textureFormat;
		textureFormat.setInternalFormat( GL_RGBA32F );
		textureFormat.setMagFilter( GL_NEAREST );
		textureFormat.setMinFilter( GL_NEAREST );
		textureFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		textureFormat.setDataType( GL_FLOAT );

		mTextureFboBloomHorizontal	= gl::Texture2d::create( windowSizeHalf.x, windowSizeHalf.y, textureFormat );
		mTextureFboBloomVertical	= gl::Texture2d::create( windowSizeHalf.x, windowSizeHalf.y, textureFormat );

		gl::RenderbufferRef horizontalBuffer	= createRenderbufferFromTexture( mTextureFboBloomHorizontal,	0, 0 );
		gl::RenderbufferRef verticalBuffer		= createRenderbufferFromTexture( mTextureFboBloomVertical,		0, 0 );

		gl::Fbo::Format fboFormat;
		fboFormat.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
		fboFormat.attachment( GL_COLOR_ATTACHMENT0, mTextureFboBloomHorizontal, horizontalBuffer );
		fboFormat.attachment( GL_COLOR_ATTACHMENT1, mTextureFboBloomVertical,	verticalBuffer );
		mFboBloom = gl::Fbo::create( windowSizeHalf.x, windowSizeHalf.y, fboFormat );

		clearFbo( mFboBloom );
	}

	// Color buffer
	{
		gl::Texture2d::Format textureFormat;
		textureFormat.setInternalFormat( GL_RGBA32F );
		textureFormat.setMagFilter( GL_NEAREST );
		textureFormat.setMinFilter( GL_NEAREST );
		textureFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		textureFormat.setDataType( GL_FLOAT );

		mTextureFboColor				= gl::Texture2d::create( windowSize.x, windowSize.y, textureFormat );
		gl::RenderbufferRef colorBuffer = createRenderbufferFromTexture( mTextureFboColor, 0, 0 );

		gl::Fbo::Format fboFormat;
		fboFormat.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
		fboFormat.attachment( GL_COLOR_ATTACHMENT0, mTextureFboColor, colorBuffer );
		mFboColor	= gl::Fbo::create( windowSize.x, windowSize.y, fboFormat );

		clearFbo( mFboColor );
	}

	// Composite buffer
	{
		gl::Texture2d::Format textureFormat;
		textureFormat.setInternalFormat( GL_RGBA32F );
		textureFormat.setMagFilter( GL_NEAREST );
		textureFormat.setMinFilter( GL_NEAREST );
		textureFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		textureFormat.setDataType( GL_FLOAT );

		mTextureFboComposite				= gl::Texture2d::create( windowSize.x, windowSize.y, textureFormat );
		gl::RenderbufferRef compositeBuffer = createRenderbufferFromTexture( mTextureFboComposite, 0, 0 );

		gl::Fbo::Format fboFormat;
		fboFormat.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
		fboFormat.attachment( GL_COLOR_ATTACHMENT0, mTextureFboComposite, compositeBuffer );
		mFboComposite = gl::Fbo::create( windowSize.x, windowSize.y, fboFormat );

		clearFbo( mFboComposite );
	}

	// Depth of field buffer
	{
		gl::Texture2d::Format textureFormat;
		textureFormat.setInternalFormat( GL_RGBA32F );
		textureFormat.setMagFilter( GL_NEAREST );
		textureFormat.setMinFilter( GL_NEAREST );
		textureFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		textureFormat.setDataType( GL_FLOAT );

		mTextureFboDofHorizontal	= gl::Texture2d::create( windowSize.x, windowSize.y, textureFormat );
		mTextureFboDofVertical		= gl::Texture2d::create( windowSize.x, windowSize.y, textureFormat );

		gl::RenderbufferRef horizontalBuffer	= createRenderbufferFromTexture( mTextureFboDofHorizontal,	0, 0 );
		gl::RenderbufferRef verticalBuffer		= createRenderbufferFromTexture( mTextureFboDofVertical,	0, 0 );

		gl::Fbo::Format fboFormat;
		fboFormat.setColorTextureFormat( gl::Texture2d::Format().internalFormat( GL_RGBA32F ) );
		fboFormat.attachment( GL_COLOR_ATTACHMENT0, mTextureFboDofHorizontal,	horizontalBuffer );
		fboFormat.attachment( GL_COLOR_ATTACHMENT1, mTextureFboDofVertical,		verticalBuffer );
		mFboDof = gl::Fbo::create( windowSize.x, windowSize.y, fboFormat );

		clearFbo( mFboDof );
	}

	// Geometry buffer
	{
		gl::Texture2d::Format textureFormat;
		textureFormat.setInternalFormat( GL_RGBA32F );
		textureFormat.setMagFilter( GL_NEAREST );
		textureFormat.setMinFilter( GL_NEAREST );
		textureFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		textureFormat.setDataType( GL_FLOAT );

		gl::Texture2d::Format materialTextureFormat;
		materialTextureFormat.setInternalFormat( GL_R8UI );
		materialTextureFormat.setMagFilter( GL_NEAREST );
		materialTextureFormat.setMinFilter( GL_NEAREST );
		materialTextureFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		materialTextureFormat.setDataType( GL_UNSIGNED_BYTE );

		mTextureFboGBufferAlbedo				= gl::Texture2d::create( windowSize.x, windowSize.y, textureFormat );
		mTextureFboGBufferMaterial				= gl::Texture2d::create( windowSize.x, windowSize.y, materialTextureFormat );
		mTextureFboGBufferNormalDepth			= gl::Texture2d::create( windowSize.x, windowSize.y, textureFormat );
		mTextureFboGBufferPosition				= gl::Texture2d::create( windowSize.x, windowSize.y, textureFormat );

		gl::RenderbufferRef albedoBuffer		= createRenderbufferFromTexture( mTextureFboGBufferAlbedo,		0, 0 );
		gl::RenderbufferRef materialBuffer		= createRenderbufferFromTexture( mTextureFboGBufferMaterial,	0, 0 );
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
		fboFormat.attachment( GL_COLOR_ATTACHMENT1, mTextureFboGBufferMaterial,		materialBuffer );
		fboFormat.attachment( GL_COLOR_ATTACHMENT2, mTextureFboGBufferNormalDepth,	normalDepthBuffer );
		fboFormat.attachment( GL_COLOR_ATTACHMENT3, mTextureFboGBufferPosition,		positionBuffer );
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
		textureFormat.setInternalFormat( GL_RGBA32F );
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
		uint32_t sz = 2048;
		gl::Texture2d::Format depthFormat;
		depthFormat.setInternalFormat( GL_DEPTH_COMPONENT32F );
		depthFormat.setMagFilter( GL_LINEAR );
		depthFormat.setMinFilter( GL_LINEAR );
		depthFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		depthFormat.setDataType( GL_FLOAT );
		mTextureFboShadowMap = gl::Texture2d::create( sz, sz, depthFormat );
		mTextureFboShadowMap->bind();
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
		mTextureFboShadowMap->unbind();

		gl::Fbo::Format fboFormat;
		fboFormat.attachment( GL_DEPTH_ATTACHMENT, mTextureFboShadowMap );
		mFboShadowMap = gl::Fbo::create( sz, sz, fboFormat );
	}

	// Screen space ambient occlusion buffer
	{
		gl::Texture2d::Format textureFormat;
		textureFormat.setInternalFormat( GL_RGBA32F );
		textureFormat.setMagFilter( GL_NEAREST );
		textureFormat.setMinFilter( GL_NEAREST );
		textureFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		textureFormat.setDataType( GL_FLOAT );

		mTextureFboSsaoHorizontal	= gl::Texture2d::create( windowSizeHalf.x, windowSizeHalf.y, textureFormat );
		mTextureFboSsaoVertical		= gl::Texture2d::create( windowSizeHalf.x, windowSizeHalf.y, textureFormat );
		mTextureFboSsao				= gl::Texture2d::create( windowSizeHalf.x, windowSizeHalf.y, textureFormat );

		gl::RenderbufferRef horizontalBuffer	= createRenderbufferFromTexture( mTextureFboSsaoHorizontal, 0, 0 );
		gl::RenderbufferRef verticalBuffer		= createRenderbufferFromTexture( mTextureFboSsaoVertical,	0, 0 );
		gl::RenderbufferRef buffer				= createRenderbufferFromTexture( mTextureFboSsao,			0, 0 );

		gl::Fbo::Format fboFormat;
		fboFormat.attachment( GL_COLOR_ATTACHMENT0, mTextureFboSsaoHorizontal,	horizontalBuffer );
		fboFormat.attachment( GL_COLOR_ATTACHMENT1, mTextureFboSsaoVertical,	verticalBuffer );
		fboFormat.attachment( GL_COLOR_ATTACHMENT2, mTextureFboSsao,			buffer );
		mFboSsao = gl::Fbo::create( windowSizeHalf.x, windowSizeHalf.y, fboFormat );

		clearFbo( mFboSsao );
	}
	
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	// Set up shadow camera
	mShadowCamera.setPerspective( 150.0f, mFboShadowMap->getAspectRatio(), mMayaCam.getCamera().getNearClip(), mMayaCam.getCamera().getFarClip() );
	mShadowCamera.lookAt( mLights.at( 0 ).getPosition(), vec3( 0.0f, -7.0f, 0.0f ) );
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
	mGlslProgBlend				= loadGlslProg( "Blend",			passThrough,							loadAsset( "blend_frag.glsl" ) );
	mGlslProgBloom				= loadGlslProg( "Bloom",			passThrough,							loadAsset( "bloom_frag.glsl" ) );
	mGlslProgBlur				= loadGlslProg( "Blur",				passThrough,							loadAsset( "blur_frag.glsl" ) );
	mGlslProgColor				= loadGlslProg( "Color",			passThrough,							loadAsset( "color_frag.glsl" ) );
	mGlslProgComposite			= loadGlslProg( "Composite",		passThrough,							loadAsset( "composite_frag.glsl" ) );
	mGlslProgDebugGbuffer		= loadGlslProg( "Debug G-Buffer",	passThrough,							loadAsset( "debug_gbuffer_frag.glsl" ) );
	mGlslProgDebugMaterial		= loadGlslProg( "Debug material",	passThrough,							loadAsset( "debug_material_frag.glsl" ) );
	mGlslProgDof				= loadGlslProg( "Depth of field",	passThrough,							loadAsset( "dof_frag.glsl" ) );
	mGlslProgFxaa				= loadGlslProg( "FXAA",				passThrough,							loadAsset( "fxaa_frag.glsl" ) );
	mGlslProgGBuffer			= loadGlslProg( "G-buffer",			gBufferVert,							loadAsset( "gbuffer_frag.glsl" ) );
	mGlslProgGBufferLight		= loadGlslProg( "G-buffer light",	gBufferVert,							loadAsset( "gbuffer_light_frag.glsl" ) );
	mGlslProgLight				= loadGlslProg( "Light",			loadAsset( "light_vert.glsl" ),			loadAsset( "light_frag.glsl" ) );
	mGlslProgShadowMap			= loadGlslProg( "Shadow map",		loadAsset( "shadow_map_vert.glsl" ),	loadAsset( "shadow_map_frag.glsl" ) );
	mGlslProgSsao				= loadGlslProg( "SSAO",				loadAsset( "ssao_vert.glsl" ),			loadAsset( "ssao_frag.glsl" ) );
	mGlslProgStockColor			= gl::context()->getStockShader( gl::ShaderDef().color() );
	mGlslProgStockTexture		= gl::context()->getStockShader( gl::ShaderDef().texture( GL_TEXTURE_2D ) );
	
	// Set default values for all properties
	mDepthScale		= 0.02f;
	mDebugMode		= false;
	mFrameRate		= 0.0f;
	mFullScreen		= isFullScreen();
	mMeshCube		= gl::VboMesh::create( geom::Cube() );
	mMeshRect		= gl::VboMesh::create( geom::Rect() );
	mMeshSphere		= gl::VboMesh::create( geom::Sphere().subdivisions( 64 ) );
	mTextureCube	= gl::Texture::create( loadImage( loadAsset( "texture.jpg" ) ) );
	mTextureRandom	= gl::Texture::create( loadImage( loadAsset( "random.png" ) ) );

	// Set up lights
	mLights.push_back( Light().setIntensity( 1.25f ).setPosition( vec3( 0.0f, 2.5f, 0.057f ) ).setRadius( 1.0f ).setVolume( 12.0f ) );
	mLights.push_back( Light().setColorDiffuse( ColorAf( 1.0f, 0.7f, 0.5f, 1.0f ) ).setIntensity( 1.0f ).setPosition( vec3(  5.0f, 0.0f, -5.0f ) ).setRadius( 0.5f ).setVolume( 8.0f ) );
	mLights.push_back( Light().setColorDiffuse( ColorAf( 1.0f, 0.7f, 0.5f, 1.0f ) ).setIntensity( 1.0f ).setPosition( vec3(  5.0f, 0.0f,  5.0f ) ).setRadius( 0.5f ).setVolume( 8.0f ) );
	mLights.push_back( Light().setColorDiffuse( ColorAf( 1.0f, 0.7f, 0.5f, 1.0f ) ).setIntensity( 1.0f ).setPosition( vec3( -5.0f, 0.0f, -5.0f ) ).setRadius( 0.5f ).setVolume( 8.0f ) );
	mLights.push_back( Light().setColorDiffuse( ColorAf( 1.0f, 0.7f, 0.5f, 1.0f ) ).setIntensity( 1.0f ).setPosition( vec3( -5.0f, 0.0f,  5.0f ) ).setRadius( 0.5f ).setVolume( 8.0f ) );
	mLights.push_back( Light().setColorDiffuse( ColorAf( 1.0f, 0.7f, 0.8f, 1.0f ) ).setIntensity( 0.75f ).setPosition( vec3(  10.0f, -6.0f,   0.0f ) ).setRadius( 0.25f ).setVolume( 4.0f ) );
	mLights.push_back( Light().setColorDiffuse( ColorAf( 1.0f, 0.7f, 0.8f, 1.0f ) ).setIntensity( 0.75f ).setPosition( vec3(   0.0f, -6.0f,  10.0f ) ).setRadius( 0.25f ).setVolume( 4.0f ) );
	mLights.push_back( Light().setColorDiffuse( ColorAf( 1.0f, 0.7f, 0.8f, 1.0f ) ).setIntensity( 0.75f ).setPosition( vec3( -10.0f, -6.0f,   0.0f ) ).setRadius( 0.25f ).setVolume( 4.0f ) );
	mLights.push_back( Light().setColorDiffuse( ColorAf( 1.0f, 0.7f, 0.8f, 1.0f ) ).setIntensity( 0.75f ).setPosition( vec3(   0.0f, -6.0f, -10.0f ) ).setRadius( 0.25f ).setVolume( 4.0f ) );
	
	// Set up materials
	mMaterials.push_back( Material().setColorAmbient( ColorAf::black() ).setColorSpecular( ColorAf::gray( 0.2f ) ).setShininess( 100.0f ) ); // Cube
	mMaterials.push_back( Material().setColorAmbient( ColorAf::black() ).setColorDiffuse( ColorAf::gray( 0.5f ) ).setColorSpecular( ColorAf::gray( 0.5f ) ).setShininess( 100.0f ) ); // Floor
	mMaterials.push_back( Material().setColorAmbient( ColorAf::black() ).setColorDiffuse( ColorAf::black() ).setColorEmission( ColorAf::white() ).setShininess( 100.0f ) ); // Light
	mUboMaterial = gl::Ubo::create( sizeof( Material ) * mMaterials.size(), (GLvoid*)&mMaterials[ 0 ] );
	gl::context()->bindBufferBase( mUboMaterial->getTarget(), 0, mUboMaterial );

	// Set up camera
	ivec2 windowSize = toPixels( getWindowSize() );
	CameraPersp cam( windowSize.x, windowSize.y, 45.0f, 1.0f, 100.0f );
	cam.setEyePoint( vec3( 6.0f, 3.0f, 15.0f ) );
	cam.setCenterOfInterestPoint( vec3( 0.0f ) );
	mMayaCam.setCurrentCam( cam );

	// Create random cubes
	for ( int32_t i = 0; i < 70; ++i ) {
		mat4 m( 1.0f );
		vec3 v( 
			randFloat( 5.0f, 15.0f ), 
			randFloat( 0.0f, 3.0f ), 
			randFloat( 5.0f, 15.0f ) );
		v *= randVec3f();
		m = glm::translate( m, v );
		m = glm::rotate( m, (float)M_PI, randVec3f() );
		m = glm::scale( m, vec3( randFloat( 0.25f, 0.75f ) ) );
		mCubes.push_back( m );
	}

	// Set up parameters
	mParams = params::InterfaceGl::create( "Params", ivec2( 220, 120 ) );
	mParams->addParam( "Frame rate",	&mFrameRate,				"", true );
	mParams->addParam( "Debug mode",	&mDebugMode ).key( "d" );
	mParams->addParam( "Fullscreen",	&mFullScreen ).key( "f" );
	mParams->addButton( "Screen shot",	[ & ]() { screenShot(); },	"key=space" );
	mParams->addButton( "Quit",			[ & ]() { quit(); },		"key=q" );

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

	// Update the cubes
	float c = math<float>::cos( e );
	float s	= math<float>::sin( e );
	for ( mat4& m : mCubes ) {
		m = glm::rotate( m, 0.01f, vec3( c, s, -c ) );
	}
}

CINDER_APP( DeferredShadingApp, RendererGl( RendererGl::Options().msaa( 0 ).coreProfile( true ).version( 4, 1 ) ), []( App::Settings* settings )
{
	settings->disableFrameRate();
	settings->setTitle( "Deferred" );
	settings->setWindowSize( 1920, 1080 );
} )
 