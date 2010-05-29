#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"

using namespace ci;
using namespace ci::app;

class iPhoneAccelerometerApp : public AppCocoaTouch {
  public:
	virtual void	setup();
	virtual void	accelerated( AccelEvent event );
	virtual void	update();
	virtual void	draw();
	
	Matrix44f		mModelView;
};

void iPhoneAccelerometerApp::setup()
{
	enableAccelerometer( 30.0f, 0.5f );
}

void iPhoneAccelerometerApp::accelerated( AccelEvent event )
{
	mModelView = event.getMatrix();
}

void iPhoneAccelerometerApp::update()
{
}

void iPhoneAccelerometerApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.3f ) );
	gl::enableDepthRead();

	CameraPersp cam = CameraPersp( getWindowWidth(), getWindowHeight(), 45.0f );
	cam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	cam.lookAt( Vec3f( 0, 0, 3 ), Vec3f::zero() );
	gl::setMatrices( cam );
	gl::multModelView( mModelView );
	gl::drawColorCube( Vec3f::zero(), Vec3f( 1, 1, 1 ) );
}

CINDER_APP_COCOA_TOUCH( iPhoneAccelerometerApp, RendererGl )
