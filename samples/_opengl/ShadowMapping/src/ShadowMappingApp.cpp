/**
 
 Eric Renaud-Houde - August 2014
 
 This sample illustrates common shadow mapping techniques.
 
 Overview ~
 
 A first pass stores the scene's depth information (from the light's POV)
 into a FBO.  When the shaded scene is rendered, a depth test is performed on
 each fragment. In the light's projective space, a fragment whose depth is
 greater than that of the shadow map must be occluded: it is shadowed.
 
 Common problems - Tradeoffs ~
 
 Aliasing: Other than increasing the resolution of the depth map, additionnal
 techniques can be used to soften the shadow edges. We demonstrate
 percentage-closer filtering (PCF) and random sampling. Note that sometimes
 lower resolution on the shadow map may help soften/blur the shadow.
 
 Surface acne/self-shadowing: Also occurring with traditional ray-tracing, this
 surface noise occurs on false depth tests (due to imprecision of a fragment's
 depth). Various offsets (such as glPolygonOffset & z-offsets in the light's
 projective space) can be used to prevent this problem.
 
 Peter Panning: The shadows don't reach the objects that cast them. This
 problem occurs when the offsets are too large. Offsets must be tweaked
 carefully to avoid problems on either end.
 
 Sampling noise: The random sampling method exhibits noise (which should
 still be less visually objectionable than aliasing). This is due to a low
 number of samples. More advanced GPU techniques allow one to increase this
 sample count.
 
 References ~
 
 OpenGL 4.0 Shading Language Cookbook by David Wolff
 https://github.com/daw42/glslcookbook
 Tutorial 16 : Shadow mapping
 http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/
 Common Techniques to Improve Shadow Depth Maps
 http://msdn.microsoft.com/en-us/library/windows/desktop/ee416324(v=vs.85).aspx
 Soft Shadow Mapping
 http://codeflow.org/entries/2013/feb/15/soft-shadow-mapping/
 
 */


#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/GeomIo.h"
#include "cinder/Rand.h"
#include "cinder/CameraUi.h"
#include "cinder/Log.h"
#include "cinder/Color.h"
#if ! defined( CINDER_GL_ES )
	#include "cinder/params/Params.h"
#endif
#include "glm/gtx/euler_angles.hpp"

using namespace ci;
using namespace ci::app;

typedef std::shared_ptr<class ShadowMap> ShadowMapRef;

class ShadowMap {
public:
	static ShadowMapRef create( int size ) { return ShadowMapRef( new ShadowMap{ size } ); }
	ShadowMap( int size )
	{
		reset( size );
	}
	
	void reset( int size )
	{
		gl::Texture2d::Format depthFormat;
		depthFormat.setInternalFormat( GL_DEPTH_COMPONENT32F );
		depthFormat.setMagFilter( GL_LINEAR );
		depthFormat.setMinFilter( GL_LINEAR );
		depthFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		depthFormat.setCompareMode( GL_COMPARE_REF_TO_TEXTURE );
		depthFormat.setCompareFunc( GL_LEQUAL );
		mTextureShadowMap = gl::Texture2d::create( size, size, depthFormat );
		
		gl::Fbo::Format fboFormat;
		fboFormat.attachment( GL_DEPTH_ATTACHMENT, mTextureShadowMap );
		mShadowMap = gl::Fbo::create( size, size, fboFormat );
	}

	const gl::FboRef&		getFbo() const { return mShadowMap; }
	const gl::Texture2dRef&	getTexture() const { return mTextureShadowMap; }
	
	float					getAspectRatio() const { return mShadowMap->getAspectRatio(); }
	ivec2					getSize() const { return mShadowMap->getSize(); }
private:
	gl::FboRef				mShadowMap;
	gl::Texture2dRef		mTextureShadowMap;
};

struct LightData {
	bool						toggleViewpoint;
	float						distanceRadius;
	float						fov;
	CameraPersp					camera;
	vec3						viewpoint;
	vec3						target;
};

class ShadowMappingApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
	
	void keyDown( KeyEvent event ) override;
  private:
	void drawScene( float spinAngle, const gl::GlslProgRef& glsl = nullptr );
#if ! defined( CINDER_GL_ES )
	params::InterfaceGlRef		mParams;
#endif	

	float						mFrameRate;
	CameraPersp					mCamera;
	CameraUi					mCamUi;
	
	gl::BatchRef				mTeapot, mTeapotShadowed;
	gl::BatchRef				mSphere, mSphereShadowed;
	std::vector<std::pair<mat4, vec3>>	mTransforms;
	
	
	gl::GlslProgRef				mShadowShader;
	ShadowMapRef				mShadowMap;
	int							mShadowMapSize;
	bool						mOnlyShadowmap;
	
	LightData					mLight;

	int							mShadowTechnique;
	
	float						mDepthBias;
	bool						mEnableNormSlopeOffset;
	float						mRandomOffset;
	int							mNumRandomSamples;
	float						mPolygonOffsetFactor, mPolygonOffsetUnits;
};

void ShadowMappingApp::setup()
{
	Rand::randomize();
	
	mFrameRate				= 0;
	mShadowMapSize			= 2048;
	
	mLight.distanceRadius	= 100.0f;
	mLight.viewpoint		= vec3( mLight.distanceRadius );
	mLight.fov				= 10.0f;
	mLight.target			= vec3( 0 );
	mLight.toggleViewpoint	= false;
	
	mShadowTechnique		= 1;
	mDepthBias				= -0.0005f;
	mRandomOffset			= 1.2f;
	mNumRandomSamples		= 32;
	mEnableNormSlopeOffset	= false;
	mOnlyShadowmap			= false;
	mPolygonOffsetFactor	= mPolygonOffsetUnits = 3.0f;
	
	
	try {
#if defined( CINDER_GL_ES )
		mShadowShader	= gl::GlslProg::create( loadAsset( "shadow_mapping_es3.vert"), loadAsset("shadow_mapping_es3.frag") );
#else
		mShadowShader	= gl::GlslProg::create( loadAsset( "shadow_mapping.vert"), loadAsset("shadow_mapping.frag") );
#endif
	} catch ( const gl::GlslProgCompileExc& exc ) {
		console() << "Shader failed to load: " << exc.what() << std::endl;
	}
	
	mShadowMap		= ShadowMap::create( mShadowMapSize );
	mLight.camera.setPerspective( mLight.fov, mShadowMap->getAspectRatio(), 0.5, 500.0 );
	
#if ! defined( CINDER_GL_ES )
	mParams = params::InterfaceGl::create( "Settings", toPixels( ivec2( 300, 325 ) ) );
	mParams->addParam( "Framerate", &mFrameRate, "", true );
	mParams->addSeparator();
	mParams->addParam( "Light viewpoint", &mLight.toggleViewpoint );
	mParams->addParam( "Light distance radius", &mLight.distanceRadius ).min( 0 ).max( 450 ).step( 1 );
	mParams->addParam( "Render only shadow map", &mOnlyShadowmap );
	mParams->addSeparator();
	std::vector<std::string> techniques = { "Hard", "PCF3x3", "PCF4x4", "Random" };
	mParams->addParam( "Technique", techniques, &mShadowTechnique );
	mParams->addSeparator();
	mParams->addParam( "Polygon offset factor", &mPolygonOffsetFactor ).step( 0.025f ).min( 0.0f );
	mParams->addParam( "Polygon offset units", &mPolygonOffsetUnits ).step( 0.025f ).min( 0.0f );
	mParams->addParam( "Shadow map size",  &mShadowMapSize ).min( 16 ).step( 16 ).updateFn( [this]() {
		mShadowMap->reset( mShadowMapSize );
	} );
	mParams->addParam( "Depth bias", &mDepthBias ).step( 0.00001f ).max( 0.0 );
	mParams->addText( "(PCF radius is const: tweak in shader.)" );
	mParams->addSeparator();
	mParams->addText( "Random sampling params" );
	mParams->addParam( "Offset radius", &mRandomOffset ).min( 0.0f ).step( 0.05f );
	mParams->addParam( "Auto normal slope offset", &mEnableNormSlopeOffset );
	mParams->addParam( "Num samples", &mNumRandomSamples ).min( 1 );
//	mParams->minimize();
#endif
	
	auto positionGlsl = gl::getStockShader( gl::ShaderDef() );
	
	auto teapot = gl::VboMesh::create( geom::Teapot().subdivisions(5) );
	mTeapot = gl::Batch::create( teapot, positionGlsl );
	mTeapotShadowed = gl::Batch::create( teapot, mShadowShader );
	
	auto sphere = gl::VboMesh::create( geom::Icosphere() );
	mSphere = gl::Batch::create( sphere, positionGlsl );
	mSphereShadowed = gl::Batch::create( sphere, mShadowShader );
		
	for ( size_t i = 0; i < 10; ++i ) {
		vec3 v( 25.0f * randVec3() );
		mat4 m{};
		m *= translate( v );
		m *= scale( vec3( 6 * ( randFloat() + 1.1f ) ) );
		m *= rotate( 2 * glm::pi<float>() * randFloat(), randVec3() );
		mTransforms.emplace_back( m, randVec3() );
	}
	
	gl::enableDepthRead();
	gl::enableDepthWrite();

	mCamera.setFov( 30.0f );
	mCamera.setAspectRatio( getWindowAspectRatio() );
	mCamUi = CameraUi( &mCamera, getWindow() );
}

void ShadowMappingApp::update()
{
	float e	= (float) getElapsedSeconds();
	float c = cos( e );
	float s	= sin( e );
	
	for ( auto& transform : mTransforms ) {
		transform.first *= orientate4( vec3( c, s, -c ) * 0.01f );
	}
	
	mLight.viewpoint.x = mLight.distanceRadius * sin( 0.25f * e );
	mLight.viewpoint.z = mLight.distanceRadius * cos( 0.25f * e );
	mLight.camera.lookAt( mLight.viewpoint, mLight.target );
	mFrameRate = getAverageFps();
}

void ShadowMappingApp::drawScene( float spinAngle, const gl::GlslProgRef& shadowGlsl )
{
	{
		gl::ScopedColor red( Color( 0.98f, 0.22f, 0.10f ));
		gl::ScopedModelMatrix push;
		gl::scale( vec3(4) );
		
		if( shadowGlsl ) {
			shadowGlsl->uniform( "uIsTeapot", false );
			mSphereShadowed->draw();
			shadowGlsl->uniform( "uIsTeapot", true );
		}
		else {
			mSphere->draw();
		}
	}
	
	{
		gl::ScopedColor white( Color( 0.90f, 0.97f, 0.97f ) );
		for ( const auto& transform : mTransforms ) {
			gl::ScopedModelMatrix push;
			gl::scale( vec3(0.25) );
			gl::multModelMatrix( rotate( spinAngle, transform.second ) * transform.first );
			if( shadowGlsl )
				mTeapotShadowed->draw();
			else
				mTeapot->draw();
		}
	}
}

void ShadowMappingApp::draw()
{
	gl::clear( Color( 0.07f, 0.05f, 0.1f ) );
	
	// Elapsed time called here: the scene must be absolutely identical on both renders!
	float spinAngle = 0.5f * (float) app::getElapsedSeconds();
	
	// Offset to help combat surface acne (self-shadowing)
	gl::enable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( mPolygonOffsetFactor, mPolygonOffsetUnits );
	
	// Render scene into shadow map
	gl::setMatrices( mLight.camera );
	gl::viewport( mShadowMap->getSize() );
	{
		gl::ScopedFramebuffer bindFbo( mShadowMap->getFbo() );
		gl::clear();
		drawScene( spinAngle );
	}

	// Render shadowed scene
	gl::setMatrices( mLight.toggleViewpoint ? mLight.camera : mCamera );
	gl::viewport( toPixels( getWindowSize() ) );
	{
		gl::ScopedGlslProg bind( mShadowShader );
		gl::ScopedTextureBind texture( mShadowMap->getTexture() );
		
		mShadowShader->uniform( "uShadowMap", 0 );
		mShadowShader->uniform( "uShadowMatrix", mLight.camera.getProjectionMatrix() * mLight.camera.getViewMatrix() );
		mShadowShader->uniform( "uShadowTechnique", mShadowTechnique );
		mShadowShader->uniform( "uDepthBias", mDepthBias );
		mShadowShader->uniform( "uOnlyShadowmap", mOnlyShadowmap );
		mShadowShader->uniform( "uRandomOffset", mRandomOffset );
		mShadowShader->uniform( "uNumRandomSamples", mNumRandomSamples );
		mShadowShader->uniform( "uEnableNormSlopeOffset", mEnableNormSlopeOffset );
		mShadowShader->uniform( "uLightPos", vec3( gl::getModelView() * vec4( mLight.viewpoint, 1.0 ) ) );
		
		drawScene( spinAngle, mShadowShader );
	}
	
	gl::disable( GL_POLYGON_OFFSET_FILL );
	
	// Render light direction vector
	gl::drawVector( mLight.viewpoint, 4.5f * normalize( mLight.viewpoint ) );

#if ! defined( CINDER_GL_ES )
	mParams->draw();
#endif
}

void ShadowMappingApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		app::setFullScreen( !app::isFullScreen() );
	}
	else if( event.getChar() == KeyEvent::KEY_SPACE ) {
#if ! defined( CINDER_GL_ES )
		mParams->maximize( ! mParams->isMaximized() );
#endif
	}
}

void prepareSettings( App::Settings *settings )
{
#if ! defined( CINDER_GL_ES )
	//settings->enableHighDensityDisplay();
	settings->setWindowSize( 900, 900 );
#endif
}

CINDER_APP( ShadowMappingApp, RendererGl( RendererGl::Options().msaa( 16 ) ), prepareSettings )
