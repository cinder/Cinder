#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CameraUi.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FloatCameraApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;
    
    CameraPersp mCam;
    CameraUi mCamUi;
    gl::BatchRef mCube;
    Anim<vec2> mMouse;
};

void FloatCameraApp::setup()
{
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    mCam = CameraPersp( getWindowWidth(), getWindowHeight(), 60, 0.1, 1000.0f );
    mCam.lookAt( vec3(0, 0, 5), vec3(0) );
    
    mCamUi = CameraUi( &mCam );
    
    mCube = gl::Batch::create( geom::Cube(), gl::getStockShader( gl::ShaderDef().color().lambert() ) );
}

void FloatCameraApp::mouseDown( MouseEvent event )
{
    mMouse.stop();
    mMouse = event.getPos();
    mCamUi.mouseDown( mMouse() );
}

void FloatCameraApp::mouseDrag( MouseEvent event )
{
    timeline().apply( &mMouse, vec2(event.getPos()), 1.0f, EaseOutQuint() ).updateFn([=]
    {
        mCamUi.mouseDrag( mMouse(), event.isLeftDown(), false, event.isRightDown() );
    });
}

void FloatCameraApp::update()
{
}

void FloatCameraApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    gl::setMatrices( mCam );
    mCube->draw();
}

CINDER_APP( FloatCameraApp, RendererGl( RendererGl::Options().msaa(16) ) )
