#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Batch.h"
#include "cinder/CinderImGui.h"
#include "cinder/CameraUi.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CameraPerspApp : public App {
  public:
	
	static void prepareSettings ( Settings* settings );
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void mouseMove( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;
	
	void setDefaultValues();
	void renderObjectToFbo();
	
	static const int			FBO_WIDTH = 1024, FBO_HEIGHT = 768;
	gl::FboRef					mObjectFbo;
	CameraPersp					mObjectCam;
	CameraPersp					mViewCam;
	CameraPersp					mViewCamInit;
	CameraUi					mCamUi;
	gl::GlslProgRef				mObjectGlsl;
	gl::TextureCubeMapRef		mCubeMap;
	
	std::vector<gl::BatchRef>	mObjects;
	size_t						mCurrObject;
	
	// params for the main camera
	vec3						mEyePoint;
	vec3						mLookAt;
	float						mFov;
	float						mAspectRatio;
	float						mNearPlane;
	float						mFarPlane;
	vec2						mLensShift;
	
	vector<string>				mObjectNames;
	int							mObjectSelection;

	ivec2						mLastMousePos;
};

void CameraPerspApp::prepareSettings( Settings* settings )
{
	settings->setResizable( false );
}

void CameraPerspApp::setup()
{
	setWindowSize( 1280, 480 );
	
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableAlphaBlending();
	
	setDefaultValues();
	
	gl::Fbo::Format format;
	mObjectFbo	= gl::Fbo::create( FBO_WIDTH, FBO_HEIGHT, format.depthTexture() );
	
	try {
		mCubeMap = gl::TextureCubeMap::create( loadImage( loadResource( RES_ENV_MAP ) ), gl::TextureCubeMap::Format().mipmap() );
		mObjectGlsl = gl::GlslProg::create( loadAsset( "objectshader.vert" ), loadAsset( "objectshader.frag" ) );
	} catch ( std::exception& e ) {
		console() << e.what() << endl;
		quit();
	}
	
	gl::BatchRef ref = gl::Batch::create( geom::Teapot().subdivisions( 16 ), mObjectGlsl );
	ref->getGlslProg()->uniform( "uCubeMapTex", 0 );
	mObjects.push_back( ref );
	mObjects.push_back( gl::Batch::create( geom::Torus().subdivisionsAxis( 32 ), mObjectGlsl ) );
	mObjects.push_back( gl::Batch::create( geom::Cone().subdivisionsAxis( 32 ), mObjectGlsl ) );
	mObjects.push_back( gl::Batch::create( geom::Helix().subdivisionsAxis( 32 ), mObjectGlsl ) );
	mObjects.push_back( gl::Batch::create( geom::Cylinder().subdivisionsAxis( 32 ), mObjectGlsl ) );
	
	ImGui::Initialize();

	mObjectSelection = 0;
	mObjectNames = { "Teapot", "Torus", "Cone", "Helix", "Cylinder" };
	
	mViewCam.setEyePoint( vec3( 0.0f, 0.0f, 10.0f) );
	mViewCam.setPerspective( 60, getWindowWidth() * 0.5f / getWindowHeight(), 1, 1000 );
	mViewCam.lookAt( vec3( 0 ) );
	mViewCamInit	= mViewCam;
	mCamUi			= CameraUi( &mViewCam );
}

void CameraPerspApp::keyDown( KeyEvent event )
{
	if ( event.getCode() == KeyEvent::KEY_ESCAPE ) quit();
}

void CameraPerspApp::mouseDown( MouseEvent event )
{
	mCamUi.mouseDown( event );
}

void CameraPerspApp::mouseMove( MouseEvent event )
{
	mLastMousePos = event.getPos();
}

void CameraPerspApp::mouseDrag( MouseEvent event )
{
	Rectf r	= Rectf( getWindowWidth() / 2, 0, getWindowWidth(), getWindowHeight() );
	if ( r.contains( event.getPos() )) {
		mCamUi.mouseDrag( event );
	}
}

void CameraPerspApp::update()
{
	ImGui::Begin( "CameraPersp" );
	if( ImGui::Combo( "Object", &mObjectSelection, mObjectNames ) ) {
		mCurrObject = (size_t)mObjectSelection;
	}
	ImGui::Separator();
	ImGui::DragFloat3( "Eye Point", &mEyePoint, 0.01f );
	ImGui::DragFloat3( "Look At", &mLookAt, 0.01f );
	ImGui::DragFloat( "FOV", &mFov, 1.0f, 1.0f, 179.0f );
	ImGui::DragFloat( "Near Plane", &mNearPlane, 0.02f, 0.1f, FLT_MAX );
	ImGui::DragFloat( "Far Plane", &mFarPlane, 0.02f, 0.1f, FLT_MAX );
	ImGui::DragFloat2( "Lens Shift X", &mLensShift, 0.01f );
	ImGui::Separator();
	if( ImGui::Button( "Reset Defaults" ) ) {
		setDefaultValues();
	}
	ImGui::End();

	renderObjectToFbo();
}

void CameraPerspApp::setDefaultValues()
{
	mCurrObject			= 0;
	mObjectSelection	= 0;
	mEyePoint			= vec3( -2.2f, -1.6f, 7.0f );
	mLookAt				= vec3( 0.0f );
	mFov				= 25.0f;
	mAspectRatio		= getWindowAspectRatio();
	mNearPlane			= 3.5f;
	mFarPlane			= 15.0f;
	mLensShift			= vec2 ( 0 );
	mViewCam			= mViewCamInit;
}

void CameraPerspApp::renderObjectToFbo()
{
	gl::ScopedFramebuffer fbScp( mObjectFbo );
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	gl::ScopedViewport scpVp( ivec2( 0 ), mObjectFbo->getSize() );
	
	mObjectCam.setEyePoint( mEyePoint );
	mObjectCam.setPerspective( mFov, mObjectFbo->getAspectRatio(), mNearPlane, mFarPlane );
	mObjectCam.setLensShift( mLensShift );
	mObjectCam.lookAt( mLookAt );
	gl::setMatrices( mObjectCam );
	
	gl::ScopedGlslProg shader( mObjectGlsl );
	gl::color( Color::white() );
	
	mCubeMap->bind();
	mObjects.at( mCurrObject )->draw();
}

void CameraPerspApp::draw()
{
	gl::clear( Color( 0.2, 0.2, 0.2 ) );
	gl::ScopedGlslProg shaderScp( gl::getStockShader( gl::ShaderDef().color() ) );

	gl::setMatrices( mViewCam );
	gl::viewport( getWindowWidth() / 2, 0.0f, getWindowWidth() / 2, getWindowHeight() );
	
	vec3 wTopLeft;
	vec3 wTopRight;
	vec3 wBottomLeft;
	vec3 wBottomRight;
	
	// Draw the far plane rect
	mObjectCam.getFarClipCoordinates( &wTopLeft, &wTopRight, &wBottomLeft, &wBottomRight );
	auto farPlane = gl::VertBatch( GL_TRIANGLE_STRIP );
	farPlane.color( ColorA ( 0.0f, 0.0f, 0.0f, 0.7f ) );
	farPlane.vertex( wTopLeft );
	farPlane.vertex( wBottomLeft );
	farPlane.vertex( wTopRight );
	farPlane.vertex( wBottomRight );
	farPlane.draw();
	
	// Draw a ray from the camera to the lookAt Point
	auto ray = gl::VertBatch( GL_LINES );
	ray.color( Color( 0.0f, 1.0f, 0.0f ) );
	ray.vertex( mEyePoint );
	ray.vertex( mLookAt );
	ray.draw();
	
	// Draw a ray from mouse to the far plane
	vec2 sTopLeft		= mObjectCam.worldToScreen( wTopLeft, getWindowWidth() / 2, getWindowHeight() );
	vec2 sBottomRight	= mObjectCam.worldToScreen( wBottomRight, getWindowWidth() / 2, getWindowHeight() );
	Rectf r				= Rectf( sTopLeft, sBottomRight );
	
	if ( r.contains( mLastMousePos ) ) {
		Ray clickRay = mObjectCam.generateRay( mLastMousePos, ivec2( getWindowWidth() / 2, getWindowHeight() ) );
		auto rayLine = gl::VertBatch( GL_LINES );
		rayLine.color( Color( 0.8f, 0.0f, 0.0f ) );
		rayLine.vertex( mEyePoint );
		rayLine.vertex( mEyePoint + clickRay.getDirection() * mObjectCam.getFarClip() );
		rayLine.draw();
	}

	// Draw camera frustrum
	gl::color( ColorA( 0.8f, 0.8f, 0.8f, 0.5f ) );
	gl::drawFrustum( mObjectCam );
	
	// Draw the object in the scene
	mObjectFbo->bindTexture();
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	mObjects.at( mCurrObject )->draw();

	// Draw the near plane with projection
	gl::ScopedGlslProg shaderScp2( gl::getStockShader( gl::ShaderDef().texture().color() ) );
	mObjectCam.getNearClipCoordinates( &wTopLeft, &wTopRight, &wBottomLeft, &wBottomRight );
	mObjectFbo->bindTexture();
	
	auto nearPlane = gl::VertBatch( GL_TRIANGLE_STRIP );
	nearPlane.color( ColorA ( 1.0f, 1.0f, 1.0f, 0.7f ) );
	nearPlane.texCoord( 0.0f, 1.0f );
	nearPlane.vertex( wTopLeft );
	nearPlane.texCoord( 0.0f, 0.0f );
	nearPlane.vertex( wBottomLeft );
	nearPlane.texCoord( 1.0f, 1.0f );
	nearPlane.vertex( wTopRight );
	nearPlane.texCoord( 1.0f, 0.0f );
	nearPlane.vertex( wBottomRight );
	nearPlane.draw();

	// Draw the fbo in screen space on the left
	gl::ScopedGlslProg shaderScp4( gl::getStockShader( gl::ShaderDef().texture() ) );
	gl::viewport( 0.0f, 0.0f, getWindowWidth(), getWindowHeight() );
	gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
	gl::drawSolidRect( Rectf( 0.0f, 0.0f, 640, 480 ) );

	mObjectFbo->unbindTexture();
}

CINDER_APP( CameraPerspApp, RendererGl( RendererGl::Options().msaa( 16 ) ), CameraPerspApp::prepareSettings )
