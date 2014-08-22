#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MotionBlurVelocityBufferApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void MotionBlurVelocityBufferApp::setup()
{
}

void MotionBlurVelocityBufferApp::mouseDown( MouseEvent event )
{
}

void MotionBlurVelocityBufferApp::update()
{
}

void MotionBlurVelocityBufferApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( MotionBlurVelocityBufferApp, RendererGl )
