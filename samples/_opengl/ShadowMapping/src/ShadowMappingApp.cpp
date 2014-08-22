#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/params/Params.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/GeomIo.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/Rand.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Log.h"
#include "cinder/Color.h"

#include "glm/gtx/euler_angles.hpp"

using namespace ci;
using namespace ci::app;

typedef std::shared_ptr<class ShadowMap> ShadowMapRef;

class ShadowMap {
public:
	static ShadowMapRef create( size_t size ) { return ShadowMapRef( new ShadowMap{ size } ); }
	ShadowMap( size_t size )
	{
		reset( size );
	}
	
	void reset( size_t size )
	{
		gl::Texture2d::Format depthFormat;
		depthFormat.setInternalFormat( GL_DEPTH_COMPONENT32F );
		depthFormat.setMagFilter( GL_LINEAR );
		depthFormat.setMinFilter( GL_LINEAR );
		depthFormat.setWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
		depthFormat.setPixelDataFormat( GL_DEPTH_COMPONENT );
		depthFormat.setPixelDataType( GL_FLOAT );
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

class ShadowMappingApp : public AppNative {
public:
	void prepareSettings( AppBasic::Settings *settings ) override;
	void setup() override;
	void resize() override;
	void update() override;
	void draw() override;
	
	void mouseMove( MouseEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
private:
	void drawScene( float spinAngle, const gl::GlslProgRef& glsl = nullptr );
	params::InterfaceGlRef		mParams;
	bool						mToggleLightViewpoint;
	float						mFrameRate;
	MayaCamUI					mMayaCam;
	ivec2						mMousePos;
	
	gl::BatchRef				mTeapot;
	gl::BatchRef				mTeapotShadowed;
	gl::BatchRef				mSphere;
	gl::BatchRef				mSphereShadowed;
	std::vector< std::pair<mat4, vec3>>	mTransforms;
	
	float						mLightDistanceRadius;
	
	gl::GlslProgRef				mShadowShader;
	ShadowMapRef				mShadowMap;
	
	CameraPersp					mLightCamera;
	vec3						mLightViewpoint;
	float						mLightFov;
	vec3						mLightTarget;
	
	int							mShadowTechnique;
	int							mShadowMapSize;
	float						mDepthBias;
	bool						mEnableNormSlopeOffset;
	float						mRandomOffset;
	bool						mOnlyShadowmap;
	float						mPolygonOffsetFactor, mPolygonOffsetUnits;
};

void ShadowMappingApp::prepareSettings( AppBasic::Settings *settings )
{
//	settings->enableHighDensityDisplay();
	settings->setWindowSize(900, 900);
}

void ShadowMappingApp::setup()
{
	mFrameRate				= 0;
	mLightDistanceRadius	= 100.0f;
	mShadowMapSize			= 2048;
	mLightViewpoint			= vec3(1) * mLightDistanceRadius;
	mLightFov				= 10.0f;
	mLightTarget			= vec3(0);
	mShadowTechnique		= 1;
	mDepthBias				= -0.0005f;
	mRandomOffset			= 1.2f;
	mEnableNormSlopeOffset	= false;
	mOnlyShadowmap			= false;
	mPolygonOffsetFactor	= mPolygonOffsetUnits = 3.0f;
	mToggleLightViewpoint	= false;
	
	try {
		mShadowShader	= gl::GlslProg::create( loadAsset( "shadow_mapping.vert"), loadAsset("shadow_mapping.frag") );
	} catch ( const gl::GlslProgCompileExc& exc ) {
		CI_LOG_E( "Shader failed to load: " << exc.what() );
		quit();
	}
	
	mShadowMap		= ShadowMap::create( mShadowMapSize );
	mLightCamera.setPerspective( mLightFov, mShadowMap->getAspectRatio(), 0.5, 500.0 );
	
	mParams = params::InterfaceGl::create( "Settings", toPixels( Vec2i( 300, 325 ) ) );
	mParams->addParam( "Framerate", &mFrameRate, "", true );
	mParams->addSeparator();
	mParams->addParam( "Light viewpoint", &mToggleLightViewpoint );
	mParams->addParam( "Light distance radius", &mLightDistanceRadius ).min(0).max(450).step(1);
	mParams->addParam( "Render only shadow map", &mOnlyShadowmap );
	mParams->addSeparator();
	mParams->addText( "Technique: Hard, PCF3x3, PCF4x4, Random" );
	mParams->addParam( "Index", &mShadowTechnique ).min(0).max(3);
	mParams->addSeparator();
	mParams->addParam( "Polygon offset factor", &mPolygonOffsetFactor ).step(0.025).min(0);
	mParams->addParam( "Polygon offset units", &mPolygonOffsetUnits ).step(0.025).min(0);
	mParams->addParam( "Shadow map size",  &mShadowMapSize ).min(16).step(16).updateFn( [this]() {
		mShadowMap->reset( mShadowMapSize );
	} );
	mParams->addParam( "Depth bias", &mDepthBias ).step(0.00001).max(0.0);
	mParams->addText( "(PCF radius is const: tweak in shader.)" );
	mParams->addSeparator();
	mParams->addText( "Random sampling params" );
	mParams->addParam( "Offset radius", &mRandomOffset ).min(0).step(0.05);
	mParams->addParam( "Auto normal slope offset", &mEnableNormSlopeOffset );
//	mParams->minimize();
	
	auto positionGlsl = gl::getStockShader( gl::ShaderDef() );
	
	auto teapot = gl::VboMesh::create( geom::Teapot()
									  .enable( geom::Attrib::POSITION )
									  .enable( geom::Attrib::TEX_COORD_0 )
									  .enable( geom::Attrib::NORMAL )
									  .subdivision(5) );
	mTeapot = gl::Batch::create( teapot, positionGlsl );
	mTeapotShadowed = gl::Batch::create( teapot, mShadowShader );
	
	auto sphere = gl::VboMesh::create( geom::Icosphere()
									  .enable( geom::Attrib::POSITION )
									  .enable( geom::Attrib::TEX_COORD_0 )
									  .enable( geom::Attrib::NORMAL ) );
	mSphere = gl::Batch::create( sphere, positionGlsl );
	mSphereShadowed = gl::Batch::create( sphere, mShadowShader );
		
	for ( size_t i = 0; i < 10; ++i ) {
		vec3 v( 25.0f * randVec3f() );
		mat4 m{};
		m *= translate( v );
		m *= scale( vec3( 6 * ( randFloat() + 1 ) ) );
		m *= rotate( 0.0f, randVec3f() );
		mTransforms.emplace_back( m, randVec3f() );
	}
	
	gl::enableDepthRead();
	gl::enableDepthWrite();
}

void ShadowMappingApp::resize()
{
	CameraPersp cam = mMayaCam.getCamera();
	cam.setFov( 30.0f );
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
}

void ShadowMappingApp::update()
{
	float e	= (float) getElapsedSeconds();
	float c = cos( e );
	float s	= sin( e );
	
	for ( auto& obj : mTransforms ) {
		obj.first *= orientate4( vec3( c, s, -c ) * 0.01f );
	}
	
	
	mLightViewpoint.x = mLightDistanceRadius * sin( 0.25f * e );
	mLightViewpoint.z = mLightDistanceRadius * cos( 0.25f * e );
	mLightCamera.lookAt( mLightViewpoint, mLightTarget );
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
		} else {
			mSphere->draw();
		}
	}
	
	{
		gl::ScopedColor white( Color( 0.90f, 0.97f, 0.97f ) );
		for ( const auto& obj : mTransforms ) {
			gl::ScopedModelMatrix push;
			gl::scale( vec3(0.25) );
			gl::multModelMatrix( rotate( spinAngle, obj.second ) * obj.first );
			if( shadowGlsl ) {
				mTeapotShadowed->draw();
			} else {
				mTeapot->draw();
			}
		}
	}
}

void ShadowMappingApp::draw()
{
	gl::clear();
	
	// Elapsed time called here: the scene must be absolutely identical on both renders!
	float spinAngle = 0.25f * (float) app::getElapsedSeconds();
	
	gl::enable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset( mPolygonOffsetFactor, mPolygonOffsetUnits );
	
	// Render scene into shadow map
	gl::setMatrices( mLightCamera );
	gl::viewport( mShadowMap->getSize() );
	{
		gl::ScopedFramebuffer bindFbo( mShadowMap->getFbo() );
		gl::clear();
		drawScene( spinAngle );
	}

	// Render shadowed scene
	gl::setMatrices( mToggleLightViewpoint ? mLightCamera : mMayaCam.getCamera() );
	gl::viewport( toPixels( getWindowSize() ) );
	{
		gl::ScopedGlslProg bind( mShadowShader );
		gl::ScopedTextureBind texture( mShadowMap->getTexture() );
		
		mShadowShader->uniform( "uShadowMap", 0 );
		mShadowShader->uniform( "uShadowMatrix", mLightCamera.getProjectionMatrix() * mLightCamera.getViewMatrix() );
		mShadowShader->uniform( "uShadowTechnique", mShadowTechnique );
		mShadowShader->uniform( "uDepthBias", mDepthBias );
		mShadowShader->uniform( "uOnlyShadowmap", mOnlyShadowmap );
		mShadowShader->uniform( "uRandomOffset", mRandomOffset );
		mShadowShader->uniform( "uEnableNormSlopeOffset", mEnableNormSlopeOffset );
		mShadowShader->uniform( "uLightPos", vec3( gl::getModelView() * vec4( mLightViewpoint, 1.0 ) ) );
		
		drawScene( spinAngle, mShadowShader );
	}
	
	gl::disable( GL_POLYGON_OFFSET_FILL );
	
	// Render light direction vector
	gl::drawLine( mLightViewpoint, vec3(0) );
	
	gl::context()->sanityCheck();
	
	mParams->draw();
	CI_CHECK_GL();
}

void ShadowMappingApp::mouseDown( MouseEvent event )
{
	mMousePos = event.getPos();
	mMayaCam.mouseDown( mMousePos );
}

void ShadowMappingApp::mouseMove( MouseEvent event )
{
	mMousePos = event.getPos();
}

void ShadowMappingApp::mouseDrag( MouseEvent event )
{
	mMousePos = event.getPos();
	
	// Added/hacked support for international mac laptop keyboards.
	bool middle = event.isMiddleDown() || ( event.isMetaDown() && event.isLeftDown() );
	bool right = event.isRightDown() || ( event.isControlDown() && event.isLeftDown() );
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown() && !middle && !right, middle, right );
}

void ShadowMappingApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		app::setFullScreen( !app::isFullScreen() );
	} else if( event.getChar() == KeyEvent::KEY_SPACE ) {
		mParams->maximize( ! mParams->isMaximized() );
	}
}

CINDER_APP_NATIVE( ShadowMappingApp, RendererGl )
