#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MultiWindowAppApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void MultiWindowAppApp::setup()
{
	gl::disableVerticalSync();
}

void MultiWindowAppApp::mouseDown( MouseEvent event )
{
	if( isFrameRateEnabled() )
		disableFrameRate();
	else
		setFrameRate( 60 );
}

void MultiWindowAppApp::update()
{
	console() << getAverageFps() << std::endl;
}

void MultiWindowAppApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_BASIC( MultiWindowAppApp, RendererGl )
