#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class «PREFIX»App : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void «PREFIX»App::setup()
{
}

void «PREFIX»App::mouseDown( MouseEvent event )
{
}

void «PREFIX»App::update()
{
}

void «PREFIX»App::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}


CINDER_APP_BASIC( «PREFIX»App, RendererGl )
