#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Json.h"
#include "cinder/MayaCamUI.h"
#include "cinder/params/Params.h"

#include "Light.h"
#include "Material.h"

/* This sample demonstrates how create a robust deferred shading engine.
 * The scene is rendered into a frame buffer with multiple attachments
 * (G-buffer). A uniform buffer object is used to store a database of
 * material data on the GPU. Shadow casters are rendered into a shadow 
 * map FBO. The data from each is read while drawing light volumes into the 
 * light buffer (L-buffer). Finally, the L-buffer is draw to the screen.
 * 
 * A screen space ambient occlusion (SSAO) pass provides extra detail with 
 * local shadows. Lights are bloomed to appear that they are glowing. 
 * We follow these with some post-processing passes, including depth of 
 * field (to mimic camera focus), some color tweaks, and anti-aliasing.
 * 
 * This sample is intended for powerful GPUs. Check out the simpler 
 * DeferredShading sample if this is too taxing on your machine, 
 * or if you are not yet familiar with the concept of deferred shading.
 */
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
	ci::gl::GlslProgRef			mGlslProgBloom;
	ci::gl::GlslProgRef			mGlslProgBlur;
	ci::gl::GlslProgRef			mGlslProgColor;
	ci::gl::GlslProgRef			mGlslProgComposite;
	ci::gl::GlslProgRef			mGlslProgDebugGbuffer;
	ci::gl::GlslProgRef			mGlslProgDebugMaterial;
	ci::gl::GlslProgRef			mGlslProgDof;
	ci::gl::GlslProgRef			mGlslProgFxaa;
	ci::gl::GlslProgRef			mGlslProgGBuffer;
	ci::gl::GlslProgRef			mGlslProgLBuffer;
	ci::gl::GlslProgRef			mGlslProgShadowMap;
	ci::gl::GlslProgRef			mGlslProgSsao;
	ci::gl::GlslProgRef			mGlslProgStockColor;
	ci::gl::GlslProgRef			mGlslProgStockTexture;

	ci::gl::FboRef				mFbo;
	ci::gl::FboRef				mFboHalf;
	ci::gl::FboRef				mFboShadowMap;
	
	ci::gl::Texture2dRef		mTextureFbo[ 6 ];
	ci::gl::Texture2dRef		mTextureFboHalf[ 4 ];
	ci::gl::TextureRef			mTextureRandom;
	ci::gl::Texture2dRef		mTextureFboShadowMap;

	size_t						mPingPong;

	ci::gl::VboMeshRef			mMeshCircle;
	ci::gl::VboMeshRef			mMeshCube;
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
	mPingPong			= 0;
	mFloor				= -7.0f;
	mFrameRate			= 0.0f;
	mFullScreen			= isFullScreen();
	mMeshCircle			= gl::VboMesh::create( geom::Circle().subdivisions( 64 ) );
	mMeshCube			= gl::VboMesh::create( geom::Cube() );
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
	}
	
	// Set up shadow camera
	mShadowCamera.setPerspective( 120.0f, mFboShadowMap->getAspectRatio(),
								 mMayaCam.getCamera().getNearClip(),
								 mMayaCam.getCamera().getFarClip() );
	mShadowCamera.lookAt( mLights.at( 0 ).getPosition(), vec3( 0.0f, mFloor, 0.0f ) );
	
	// Call resize to create FBOs
	resize();
}

void DeferredShadingAdvancedApp::draw()
{
	gl::disableAlphaBlending();

	const mat4 shadowMatrix			= mShadowCamera.getProjectionMatrix() * mShadowCamera.getViewMatrix();
	const mat4 projMatrixInverse	= glm::inverse( mMayaCam.getCamera().getProjectionMatrix() );
	const vec2 winSize				= vec2( getWindowSize() );
	float e							= (float)getElapsedSeconds();
	
	float near						= mMayaCam.getCamera().getNearClip();
	float far						= mMayaCam.getCamera().getFarClip();
	vec2 projectionParams			= vec2( far / ( far - near ), ( -far * near ) / ( far - near ) );
	
	// Draw shadow casters
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
	
	// Clear frame buffers
	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFbo );
		const static GLenum buffers[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
			GL_COLOR_ATTACHMENT4
		};
		gl::drawBuffers( 5, buffers );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFbo->getSize() );
		gl::clear();
	}
	{
		gl::ScopedFramebuffer scopedFrameBuffer( mFboHalf );
		const static GLenum buffers[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
		};
		gl::drawBuffers( 4, buffers );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboHalf->getSize() );
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
		drawSpheres();
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	// G-BUFFER
	
	{
		// Bind the G-buffer FBO and draw to all attachments
		gl::ScopedFramebuffer scopedFrameBuffer( mFbo );
		const static GLenum buffers[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2
		};
		gl::drawBuffers( 3, buffers );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFbo->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::enableDepthRead( true );
		gl::enableDepthWrite( true );
		gl::setMatrices( mMayaCam.getCamera() );

		gl::ScopedGlslProg scopedGlslProg( mGlslProgGBuffer );

		// Draw shadow casters (spheres)
		mGlslProgGBuffer->uniform( "uMaterialId", 0 );
		drawSpheres();
	
		// Draw floor
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
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// L-BUFFER

	{
		// Set up window and clear buffers
		gl::ScopedFramebuffer scopedFrameBuffer( mFbo );
		gl::drawBuffer( GL_COLOR_ATTACHMENT3 );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFbo->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::ScopedAdditiveBlend scopedAdditiveBlend;
		gl::enableDepthRead( true );
		gl::enableDepthWrite( true );
		gl::setMatrices( mMayaCam.getCamera() );
		
		// Bind G-buffer textures and shadow map
		gl::ScopedTextureBind scopedTextureBind0( mTextureFbo[ 0 ],		0 );
		gl::ScopedTextureBind scopedTextureBind1( mTextureFbo[ 1 ],		1 );
		gl::ScopedTextureBind scopedTextureBind2( mTextureFbo[ 2 ],		2 );
		gl::ScopedTextureBind scopedTextureBind3( mTextureFbo[ 3 ],		3 );
		gl::ScopedTextureBind scopedTextureBind4( mTextureFboShadowMap,	4 );

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
		mGlslProgLBuffer->uniform( "uWindowSize",			vec2( mFbo->getSize() ) );
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
		gl::setMatricesWindow( mFbo->getSize() );
		mGlslProgLBuffer->uniform( "uLightColorAmbient",	mCameraLight.getColorAmbient() );
		mGlslProgLBuffer->uniform( "uLightColorDiffuse",	mCameraLight.getColorDiffuse() );
		mGlslProgLBuffer->uniform( "uLightColorSpecular",	mCameraLight.getColorSpecular() );
		mGlslProgLBuffer->uniform( "uLightPosition",		mCameraLight.getPosition() );
		mGlslProgLBuffer->uniform( "uLightIntensity",		mCameraLight.getIntensity() );
		mGlslProgLBuffer->uniform( "uLightRadius",			mCameraLight.getRadius() );
		gl::drawSolidRect( mFbo->getBounds() );
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// SSAO

	if ( mEnabledSsao ) {
		
		// Set up window and clear buffers
		gl::ScopedFramebuffer scopedFrameBuffer( mFboHalf );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFboHalf->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::ScopedAlphaBlend scopedAlphaBlend( true );
		gl::disableDepthRead();
		gl::disableDepthWrite();
		gl::setMatricesWindow( mFboHalf->getSize() );

		// SSAO pass
		{
			gl::drawBuffer( GL_COLOR_ATTACHMENT2 );
			gl::ScopedTextureBind scopedTextureBind0( mTextureFbo[ 0 ],	0 );
			gl::ScopedTextureBind scopedTextureBind1( mTextureRandom,	1 );
			gl::ScopedTextureBind scopedTextureBind2( mTextureFbo[ 3 ],	2 );
			gl::ScopedGlslProg scopedGlslProg( mGlslProgSsao );
			mGlslProgSsao->uniform( "uSamplerDepth",	0 );
			mGlslProgSsao->uniform( "uSamplerNoise",	1 );
			mGlslProgSsao->uniform( "uSamplerNormal",	2 );
			gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboHalf->getSize() ) );
		}

		// Calculate blur pixel size
		vec2 ssaoBlurSize	= vec2( 1.0f ) / winSize;
		ssaoBlurSize		*= vec2( mFboHalf->getSize() ) / winSize;

		// Horizontal blur pass
		gl::ScopedGlslProg scopedGlslProg( mGlslProgBlur );
		mGlslProgBlur->uniform( "uAttenuation",	1.0f );
		mGlslProgBlur->uniform( "uSize",		vec2( ssaoBlurSize.x, 0.0f ) );
		mGlslProgBlur->uniform( "uSampler",		0 );
		gl::drawBuffer( GL_COLOR_ATTACHMENT3 );
		{
			gl::ScopedTextureBind scopedTextureBind( mTextureFboHalf[ 2 ], 0 );
			gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboHalf->getSize() ) );
		}

		// Vertical blur pass
		mGlslProgBlur->uniform( "uSize", vec2( 0.0f, ssaoBlurSize.y ) );
		gl::drawBuffer( GL_COLOR_ATTACHMENT2 );
		gl::clear();
		{
			gl::ScopedTextureBind scopedTextureBind( mTextureFboHalf[ 3 ], 0 );
			gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboHalf->getSize() ) );
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// COMPOSITE

	{
		// Set up window for composite pass
		gl::ScopedFramebuffer scopedFrameBuffer( mFbo );
		gl::drawBuffer( GL_COLOR_ATTACHMENT4 );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFbo->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::disableDepthRead();
		gl::disableDepthWrite();
		gl::setMatricesWindow( mFbo->getSize() );
	
		// Blend L-buffer and shadows
		if ( mEnabledSsao ) {
			gl::ScopedTextureBind scopedTextureBind0( mTextureFbo[ 4 ],		0 );
			gl::ScopedTextureBind scopedTextureBind1( mTextureFboHalf[ 2 ],	1 );
			gl::ScopedGlslProg scopedGlslProg( mGlslProgComposite );
			mGlslProgComposite->uniform( "uSamplerLBuffer",	0 );
			mGlslProgComposite->uniform( "uSamplerSsao",	1 );
			gl::drawSolidRect( Rectf( vec2( 0.0f ), mFbo->getSize() ) );
		} else {
			gl::draw( mTextureFbo[ 4 ] );
		}
		
		mPingPong = 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// DEPTH OF FIELD

	if ( mEnabledDoF ) {
		size_t pong = ( mPingPong + 1 ) % 2;

		// Set up window for depth of field pass
		gl::ScopedFramebuffer scopedFrameBuffer( mFbo );
		gl::drawBuffer( GL_COLOR_ATTACHMENT3 + mPingPong );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFbo->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::disableDepthRead();
		gl::disableDepthWrite();
		gl::clear();
		gl::setMatricesWindow( mFbo->getSize() );

		gl::ScopedGlslProg scopedGlslProg( mGlslProgDof );
		mGlslProgDof->uniform( "uAspect",		mTextureFbo[ 4 + pong ]->getAspectRatio() );
		mGlslProgDof->uniform( "uSamplerDepth", 0 );
		mGlslProgDof->uniform( "uSampler",		1 );

		gl::ScopedTextureBind scopedTextureBind0( mTextureFbo[ 0 ],			0 );
		gl::ScopedTextureBind scopedTextureBind1( mTextureFbo[ 4 + pong ],	1 );
		gl::drawSolidRect( Rectf( vec2( 0.0f ), mFbo->getSize() ) );

		mPingPong = pong;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// COLOR

	// Set up window for color processing pass
	if ( mEnabledColor ) {
		size_t pong = ( mPingPong + 1 ) % 2;

		// Set up window for color pass
		gl::ScopedFramebuffer scopedFrameBuffer( mFbo );
		gl::drawBuffer( GL_COLOR_ATTACHMENT3 + mPingPong );
		gl::ScopedViewport scopedViewport( ivec2( 0 ), mFbo->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::disableDepthRead();
		gl::disableDepthWrite();
		gl::clear();
		gl::setMatricesWindow( mFbo->getSize() );
	
		// Perform color processing pass
		gl::ScopedTextureBind scopedTextureBind( mTextureFbo[ 4 + pong ], 0 );
		gl::ScopedGlslProg scopedGlslProg( mGlslProgColor );
		mGlslProgColor->uniform( "uSampler", 0 );
		gl::drawSolidRect( Rectf( vec2( 0.0f ),	mFbo->getSize() ) );

		mPingPong = pong;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////
	// BLOOM
	
	if ( mEnabledBloom ) {
		
		// Set up window and clear buffers
		gl::ScopedFramebuffer scopedFrameBuffer( mFboHalf );
		gl::ScopedViewport scopedViewport( ivec2( 0 ),mFboHalf->getSize() );
		gl::ScopedMatrices scopedMatrices;
		gl::disableDepthRead();
		gl::disableDepthWrite();
		gl::setMatricesWindow( mFboHalf->getSize(), false );
		
		// Calculate bloom pixel size
		float bloomAtt	= 1.0f;
		vec2 bloomSize	= vec2( 1.0f ) / winSize * 5.0f;
		bloomSize		*= vec2( mFboHalf->getSize() ) / winSize;
		
		// Horizontal pass
		{
			gl::drawBuffer( GL_COLOR_ATTACHMENT0 );
			gl::ScopedTextureBind scopedTextureBind0( mTextureFbo[ 1 ],	0 );
			gl::ScopedTextureBind scopedTextureBind1( mTextureFbo[ 2 ],	1 );
			gl::ScopedGlslProg scopedGlslProg( mGlslProgBloom );
			mGlslProgBloom->uniform( "uAttenuation",		bloomAtt );
			mGlslProgBloom->uniform( "uSize",				vec2( bloomSize.x, 0.0f ) );
			mGlslProgBloom->uniform( "uSamplerAlbedo",		0 );
			mGlslProgBloom->uniform( "uSamplerMaterial",	1 );
			gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboHalf->getSize() ) );
		}
		
		// Vertical pass
		{
			gl::drawBuffer( GL_COLOR_ATTACHMENT1 );
			gl::ScopedTextureBind scopedTextureBind( mTextureFboHalf[ 0 ], 0 );
			gl::ScopedGlslProg scopedGlslProg( mGlslProgBlur );
			mGlslProgBlur->uniform( "uAttenuation",	bloomAtt );
			mGlslProgBlur->uniform( "uSize",		vec2( 0.0f, bloomSize.y ) );
			mGlslProgBlur->uniform( "uSampler",		0 );
			gl::drawSolidRect( Rectf( vec2( 0.0f ), mFboHalf->getSize() ) );
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// FINAL RENDER

	// Set up window for screen render
	gl::ScopedViewport scopedViewport( ivec2( 0 ), getWindowSize() );
	gl::disableDepthRead();
	gl::disableDepthWrite();
	
	if ( mDrawDebug ) {
		gl::ScopedMatrices scopedMatrices;
		gl::clear( Colorf::gray( 0.4f ) );
		gl::setMatricesWindow( getWindowSize() );
		
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
	
		// G-buffer
		{
			gl::ScopedTextureBind scopedTextureBind0( mTextureFbo[ 0 ], 0 );
			gl::ScopedTextureBind scopedTextureBind1( mTextureFbo[ 1 ], 1 );
			gl::ScopedTextureBind scopedTextureBind2( mTextureFbo[ 3 ], 2 );

			gl::ScopedGlslProg scopedGlslProg( mGlslProgDebugGbuffer );
			mGlslProgDebugGbuffer->uniform( "uProjectionParams",	projectionParams );
			mGlslProgDebugGbuffer->uniform( "uProjMatrixInverse",	projMatrixInverse );
			mGlslProgDebugGbuffer->uniform( "uSamplerDepth",		0 );
			mGlslProgDebugGbuffer->uniform( "uSamplerAlbedo",		1 );
			mGlslProgDebugGbuffer->uniform( "uSamplerNormal",		2 );

			for ( int32_t i = 0; i < 4; ++i, ++index ) {
				mGlslProgDebugGbuffer->uniform( "uMode", i );
				vec2 pos = calcPosition( index );
				gl::drawSolidRect( Rectf( pos, pos + sz ) );
			}
		}

		// Bloom and SSAO
		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgStockTexture );
			for ( size_t i = 1; i < 4; i += 2, ++index ) {
				mTextureFboHalf[ i ]->setTopDown( i > 0 );
				gl::ScopedTextureBind scopedTextureBind( mTextureFboHalf[ i ], 0 );
				vec2 pos = calcPosition( index );
				gl::drawSolidRect( Rectf( pos, pos + sz ) );
			}
		}

		// Material
		{
			gl::ScopedGlslProg scopedGlslProg( mGlslProgDebugMaterial );
			gl::ScopedTextureBind scopedTextureBind( mTextureFbo[ 2 ], 0 );
			mGlslProgDebugMaterial->uniform( "uSampler", 0 );
			mGlslProgDebugMaterial->uniformBlock( 0, 0 );
			for ( int32_t i = 0; i < 6; ++i, ++index ) {
				mGlslProgDebugMaterial->uniform( "uMode", i );
				vec2 pos = calcPosition( index );
				gl::drawSolidRect( Rectf( pos, pos + sz ) );
			}
		}
	} else {
		gl::ScopedMatrices scopedMatrices;
		gl::clear();
		gl::setMatricesWindow( getWindowSize() );
		
		// Perform FXAA
		{
			gl::ScopedTextureBind scopedTextureBind( mTextureFbo[ 4 + mPingPong ], 0 );
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

		// Draw bloom on top
		if ( mEnabledBloom ) {
			gl::ScopedGlslProg scopedGlslProg( mGlslProgStockTexture );
			gl::ScopedAdditiveBlend scopedAdditiveBlend;
			gl::ScopedTextureBind scopedTextureBind( mTextureFboHalf[ 1 ], 0 );
			gl::drawSolidRect( Rectf( vec2( 0.0f ), getWindowSize() ) );
		}

		// Draw light volumes
		if ( mDrawLightVolume ) {
			gl::ScopedViewport scopedViewport( ivec2( 0 ), mFbo->getSize() );
			gl::ScopedMatrices scopedMatrices;
			gl::ScopedAlphaBlend scopedAlphaBlend( false );
			gl::setMatrices( mMayaCam.getCamera() );
			gl::ScopedPolygonMode scopedPolygonMode( GL_LINE );

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
	mGlslProgGBuffer			= loadGlslProg( "G-buffer",			loadAsset( "gbuffer_vert.glsl" ),		loadAsset( "gbuffer_frag.glsl" ) );
	mGlslProgLBuffer			= loadGlslProg( "L-buffer",			passThrough,							loadAsset( "lbuffer_frag.glsl" ) );
	mGlslProgShadowMap			= loadGlslProg( "Shadow map",		loadAsset( "shadow_map_vert.glsl" ),	loadAsset( "shadow_map_frag.glsl" ) );
	mGlslProgSsao				= loadGlslProg( "SSAO",				loadAsset( "ssao_vert.glsl" ),			loadAsset( "ssao_frag.glsl" ) );
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

	// Half window size framebuffer
	// 0 - 1 Bloom
	// 2 - 3 SSAO
	{
		int32_t h = getWindowHeight() / 2;
		int32_t w = getWindowWidth() / 2;
		gl::Fbo::Format fboFormat;
		for ( size_t i = 0; i < 4; ++i ) {
			mTextureFboHalf[ i ] = gl::Texture2d::create( w, h, textureFormat );
			fboFormat.attachment( GL_COLOR_ATTACHMENT0 + i, mTextureFboHalf[ i ],
								 gl::Renderbuffer::create( w, h, mTextureFboHalf[ i ]->getInternalFormat(), 0, 0 ) );
		}
		mFboHalf = gl::Fbo::create( w, h, fboFormat );
		gl::ScopedFramebuffer scopedFramebuffer( mFboHalf );
		gl::viewport( mFboHalf->getSize() );
		gl::clear();
	}

	// Set up the frame buffer
	// 0 GL_DEPTH_ATTACHMENT	Depth
	// 1 GL_COLOR_ATTACHMENT0	Albedo
	// 2 GL_COLOR_ATTACHMENT1	Material ID
	// 3 GL_COLOR_ATTACHMENT2	Encoded normals
	// 4 GL_COLOR_ATTACHMENT3	Ping
	// 5 GL_COLOR_ATTACHMENT4	Pong
	{
		int32_t h = getWindowHeight();
		int32_t w = getWindowWidth();
		mTextureFbo[ 0 ] = gl::Texture2d::create( w, h, gl::Texture2d::Format()
												 .internalFormat( GL_DEPTH_COMPONENT32F )
												 .magFilter( GL_LINEAR )
												 .minFilter( GL_LINEAR )
												 .wrap( GL_CLAMP_TO_EDGE )
												.dataType( GL_FLOAT ) );
		mTextureFbo[ 1 ] = gl::Texture2d::create( w, h, textureFormat );
		mTextureFbo[ 2 ] = gl::Texture2d::create( w, h, gl::Texture2d::Format()
												 .internalFormat( GL_R8I )
												 .magFilter( GL_NEAREST )
												 .minFilter( GL_NEAREST )
												 .wrap( GL_CLAMP_TO_EDGE )
												 .dataType( GL_BYTE ) );
		mTextureFbo[ 3 ] = gl::Texture2d::create( w, h, gl::Texture2d::Format()
												 .internalFormat( GL_RG8 )
												 .magFilter( GL_NEAREST )
												 .minFilter( GL_NEAREST )
												 .wrap( GL_CLAMP_TO_EDGE )
												 .dataType( GL_BYTE ) );
		mTextureFbo[ 4 ] = gl::Texture2d::create( w, h, textureFormat );
		mTextureFbo[ 5 ] = gl::Texture2d::create( w, h, textureFormat );
		gl::Fbo::Format fboFormat;
		for ( size_t i = 0; i <= 5; ++i ) {
			 fboFormat.attachment( i == 0 ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0 + ( i - 1 ),
								  mTextureFbo[ i ],
								  gl::Renderbuffer::create( w, h, mTextureFbo[ i ]->getInternalFormat(), 0, 0 ) );
		}
		mFbo = gl::Fbo::create( w, h, fboFormat );
		gl::ScopedFramebuffer scopedFramebuffer( mFbo );
		gl::viewport( mFbo->getSize() );
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
 