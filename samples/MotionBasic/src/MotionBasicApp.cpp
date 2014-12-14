#include "cinder/app/AppCocoaTouch.h"
#include "cinder/Camera.h"
#include "cinder/Timeline.h"
#include "cinder/MotionManager.h"

using namespace ci;
using namespace ci::app;

class MotionBasicApp : public AppCocoaTouch {
  public:
	virtual void	setup();
	virtual void	update();
	virtual void	draw();
	
	mat4		mModelView;
	CameraPersp		mCam;
	Anim<Color>		mBackgroundColor;
};

void MotionBasicApp::setup()
{
	console() << "gyro available: " << MotionManager::isGyroAvailable() << std::endl;
	
	MotionManager::enable( 60.0f/*, MotionManager::SensorMode::Accelerometer*/ );

	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	mCam.lookAt( vec3( 0, 0, 3 ), vec3::zero() );
}

void MotionBasicApp::update()
{
	mModelView = MotionManager::getRotationMatrix().inverted();

    if( MotionManager::isShaking( 1.5f ) ) {
		std::cout << "isShaking!\n";
		mBackgroundColor = Color::gray( MotionManager::getShakeDelta() / 10.0f );
		timeline().apply( &mBackgroundColor, Color::black(), 0.5f, EaseInQuad() );
    }
}

void MotionBasicApp::draw()
{
	gl::clear( mBackgroundColor() );
	gl::enableDepthRead();

	gl::setMatrices( mCam );
	gl::multModelView( mModelView );

//	gl::drawColorCube( vec3::zero(), vec3( 1, 1, 1 ) );
	gl::drawCoordinateFrame();
}

CINDER_APP_COCOA_TOUCH( MotionBasicApp, RendererGl )
