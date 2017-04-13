#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"
#include "cinder/Json.h"
#include "cinder/ip/Fill.h"
#include "jsoncpp/json.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderDLLApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	gl::FboRef		mFbo;
	gl::BatchRef	mObject;
	CameraPersp		mCamera;
	CameraUi		mCameraUi;
};

void CinderDLLApp::setup()
{
	const RectT<float> rect = getWindowBounds();
	RectT<float> rect2 = RectT<float>( rect );

	JsonTree json = JsonTree( " { \"test\":123 } " );
	auto jsonTest = json.getChild( "test" );
	auto jsonTestValue = jsonTest.getValue();
	app::console() << jsonTestValue << endl;

	Json::Value value;

	auto rectGeom = geom::Rect().colors().rect( Rectf( vec2(0), vec2(10) ) );

	mCamera = CameraPersp( getWindowWidth(), getWindowHeight(), 50.0f, 0.01f, 10.0f );
	mCameraUi = CameraUi( &mCamera, getWindow() );
	mCamera.lookAt( vec3( 1.0f ), vec3( 0.0f ) );
	
	mObject = gl::Batch::create( geom::Teapot().subdivisions( 32 ), gl::getStockShader( gl::ShaderDef().color().lambert() ) );

	mFbo = gl::Fbo::create( getWindowWidth(), getWindowHeight(), gl::Fbo::Format().depthTexture().samples( 8 ) );

	fs::path path( "test/test_test.jpg" );
	path.stem().string();
	auto stem = path.stem();
	auto path2 = path / stem;

	App::get()->dispatchAsync( []() {} );

	Surface8u surface( 100, 100, false );
	ip::fill( &surface, Color( 1, 0, 0 ) );
}

void CinderDLLApp::mouseDown( MouseEvent event )
{
}

void CinderDLLApp::update()
{
	gl::ScopedFramebuffer scopedFbo( mFbo );
	gl::clear( Color::gray( 0.2f ) ); 
	gl::setMatrices( mCamera );

	gl::ScopedDepth scopedDepth( true );
	mObject->draw();
}

void CinderDLLApp::draw()
{
	gl::clear();
	gl::setMatricesWindow( getWindowSize() );
	gl::draw( mFbo->getColorTexture(), mFbo->getBounds() );
}

void prepareSettings( App::Settings *settings )
{
	auto app = app::App::get();
}

CINDER_APP( CinderDLLApp, RendererGl( RendererGl::Options().msaa( 8 ) ), prepareSettings )
