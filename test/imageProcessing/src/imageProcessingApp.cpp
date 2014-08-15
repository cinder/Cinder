#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class imageProcessingApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	
	
};

void imageProcessingApp::setup()
{
}

void imageProcessingApp::mouseDown( MouseEvent event )
{
}

void imageProcessingApp::update()
{
}

void imageProcessingApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( imageProcessingApp, RendererGl )
