// Basic implementation of shadow mapping
// Keith Butters - 2014 - http://www.keithbutters.com

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/Camera.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int	FBO_WIDTH = 2048;
const int	FBO_HEIGHT = 2048;

class ShadowMappingBasic : public App {
  public:
	static void prepareSettings( Settings *settings );
	void setup() override;
	void resize() override;
	void update() override;
	void draw() override;
	
	void drawScene( bool shadowMap );
	void renderDepthFbo();
	
  private:
	gl::FboRef			mFbo;

	CameraPersp			mCam;
	CameraPersp			mLightCam;
	vec3				mLightPos;
	
	gl::GlslProgRef		mGlsl;
	gl::Texture2dRef	mShadowMapTex;
	
	gl::BatchRef		mTeapotBatch;
	gl::BatchRef		mTeapotShadowedBatch;
	gl::BatchRef		mFloorBatch;
	gl::BatchRef		mFloorShadowedBatch;
	
	float				mTime;
};

void ShadowMappingBasic::prepareSettings( Settings *settings )
{
	settings->setHighDensityDisplayEnabled();
	settings->setWindowSize( 1024, 768 );
}

void ShadowMappingBasic::setup()
{
	mLightPos = vec3( 0.0f, 5.0f, 1.0f );
	
/*
	try {
		gl::Texture2d::Format depthFormat;
		depthFormat.setInternalFormat( GL_DEPTH_COMPONENT16 );
		depthFormat.setDataType( GL_UNSIGNED_INT );
		depthFormat.setMagFilter( GL_NEAREST );
		depthFormat.setMinFilter( GL_NEAREST );
		auto tex = gl::Texture2d::create( 100, 100, depthFormat );
		std::cout << std::endl;

		gl::Fbo::Format fmt;
		fmt.attachment( GL_DEPTH_ATTACHMENT, tex);
		auto fbo = gl::Fbo::create( 100, 100, fmt );
	}
	catch( const std::exception& e ) {
		console() << "ERROR: " << e.what() << std::endl;
	}
	std::cout << std::endl;
*/

	gl::Texture2d::Format depthFormat;
	
#if defined( CINDER_GL_ES )
	depthFormat.setInternalFormat( GL_DEPTH_COMPONENT16 );
	depthFormat.setDataType( GL_UNSIGNED_INT );
	depthFormat.setMagFilter( GL_NEAREST );
	depthFormat.setMinFilter( GL_NEAREST );
#else
	depthFormat.setInternalFormat( GL_DEPTH_COMPONENT32F );
	depthFormat.setCompareMode( GL_COMPARE_REF_TO_TEXTURE );
	depthFormat.setMagFilter( GL_LINEAR );
	depthFormat.setMinFilter( GL_LINEAR );
	depthFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );	
#endif
	depthFormat.setCompareFunc( GL_LEQUAL );
	
	mShadowMapTex = gl::Texture2d::create( FBO_WIDTH, FBO_HEIGHT, depthFormat );

	mCam.setPerspective( 40.0f, getWindowAspectRatio(), 0.5f, 500.0f );
	
	try {	
		gl::Fbo::Format fboFormat;
		fboFormat.attachment( GL_DEPTH_ATTACHMENT, mShadowMapTex );
		mFbo = gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, fboFormat );
	}
	catch( const std::exception& e ) {
		console() << "FBO ERROR: " << e.what() << std::endl;
	}
	
	// Set up camera from the light's viewpoint
	mLightCam.setPerspective( 100.0f, mFbo->getAspectRatio(), 0.5f, 10.0f );
	mLightCam.lookAt( mLightPos, vec3( 0.0f ) );
	
	try {
#if defined( CINDER_GL_ES )
		mGlsl = gl::GlslProg::create( loadAsset( "shadow_shader_es2.vert" ), loadAsset( "shadow_shader_es2.frag" ) );
#else
		mGlsl = gl::GlslProg::create( loadAsset( "shadow_shader.vert" ), loadAsset( "shadow_shader.frag" ) );
#endif
	}
	catch ( Exception &exc ) {
		CI_LOG_EXCEPTION( "glsl load failed", exc );
		std::terminate();
	}
	
	auto teapot				= geom::Teapot().subdivisions( 8 );
	mTeapotBatch			= gl::Batch::create( teapot, gl::getStockShader( gl::ShaderDef() ) );
	mTeapotShadowedBatch	= gl::Batch::create( teapot, mGlsl );
	
	auto floor				= geom::Cube().size( 10.0f, 0.5f, 10.0f );
	mFloorBatch				= gl::Batch::create( floor, gl::getStockShader( gl::ShaderDef() ) );
	mFloorShadowedBatch		= gl::Batch::create( floor, mGlsl );
	
	gl::enableDepthRead();
	gl::enableDepthWrite();
}

void ShadowMappingBasic::resize()
{
	mCam.setAspectRatio( getWindowAspectRatio() );
}

void ShadowMappingBasic::update()
{
	// Store time so each render pass uses the same value
	mTime = getElapsedSeconds();
	mCam.lookAt( vec3( sin( mTime ) * 5.0f, sin( mTime ) * 2.5f + 2, 5.0f ), vec3( 0.0f ) );
}

void ShadowMappingBasic::renderDepthFbo()
{
	// Set polygon offset to battle shadow acne
	gl::enable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( 2.0f, 2.0f );

	// Render scene to fbo from the view of the light
	gl::ScopedFramebuffer fbo( mFbo );
	gl::ScopedViewport viewport( vec2( 0.0f ), mFbo->getSize() );
	gl::clear( Color::black() );
	gl::color( Color::white() );
	gl::setMatrices( mLightCam );

	drawScene( true );
	
	// Disable polygon offset for final render
	gl::disable( GL_POLYGON_OFFSET_FILL );
}

void ShadowMappingBasic::drawScene( bool shadowMap )
{
	gl::pushModelMatrix();
	gl::color( Color( 0.4f, 0.6f, 0.9f ) );
	gl::rotate( mTime * 2.0f, 1.0f, 1.0f, 1.0f );

	if( shadowMap )
		mTeapotBatch->draw();
	else
		mTeapotShadowedBatch->draw();

	gl::popModelMatrix();
	
	gl::color( Color( 0.7f, 0.7f, 0.7f ) );
	gl::translate( 0.0f, -2.0f, 0.0f );
	
	if( shadowMap )
		mFloorBatch->draw();
	else
		mFloorShadowedBatch->draw();
}

void ShadowMappingBasic::draw()
{
	renderDepthFbo();

	gl::clear( Color::black() );
	gl::setMatrices( mCam );

	gl::ScopedTextureBind texScope( mShadowMapTex, (uint8_t) 0 );

	vec3 mvLightPos	= vec3( gl::getModelView() * vec4( mLightPos, 1.0f ) ) ;
	mat4 shadowMatrix = mLightCam.getProjectionMatrix() * mLightCam.getViewMatrix();

	mGlsl->uniform( "uShadowMap", 0 );
	mGlsl->uniform( "uLightPos", mvLightPos );
	mGlsl->uniform( "uShadowMatrix", shadowMatrix );
	
	drawScene( false );
    
    // Uncomment for debug
    /*
    gl::setMatricesWindow( getWindowSize() );
    gl::color( 1.0f, 1.0f, 1.0f );
    float size = 0.5f*std::min( getWindowWidth(), getWindowHeight() );
    gl::draw( mShadowMapTex, Rectf( 0, 0, size, size ) );
    */
}

CINDER_APP( ShadowMappingBasic, RendererGl, ShadowMappingBasic::prepareSettings )
