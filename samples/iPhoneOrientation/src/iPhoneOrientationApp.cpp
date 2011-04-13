#include "cinder/app/AppCocoaTouch.h"
#include "cinder/Camera.h"

using namespace ci;
using namespace ci::app;

class iPhoneOrientationApp : public AppCocoaTouch {
  public:
	virtual void	setup();
	virtual void	orientationChanged( OrientationEvent event );
    virtual void    update();
	virtual void	draw();
	
	CameraPersp		mCam;
};

void iPhoneOrientationApp::setup()
{
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
    Vec3f upVector = Vec3f::yAxis();
    upVector.rotateZ( getAngleForOrientation( getInterfaceOrientation() ) );
    mCam.lookAt( Vec3f( 0, 0, 3 ), Vec3f::zero(), upVector );
}

void iPhoneOrientationApp::orientationChanged( OrientationEvent event )
{
    Vec3f upVector = Vec3f::yAxis();
    upVector.rotateZ( getAngleForOrientation( event.getInterfaceOrientation() ) );
    mCam.lookAt( Vec3f( 0, 0, 3 ), Vec3f::zero(), upVector );	
}

void iPhoneOrientationApp::update()
{
}

void iPhoneOrientationApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.3f ) );
	gl::enableDepthRead();

	gl::setMatrices( mCam );
	gl::pushModelView();
        gl::drawCoordinateFrame();
    gl::popModelView();
}

CINDER_APP_COCOA_TOUCH( iPhoneOrientationApp, RendererGl )
