#include "cinder/app/AppNative.h"
#include "cinder/app/RendererDx.h"
#include "cinder/dx/dx.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class _TBOX_PREFIX_App : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void _TBOX_PREFIX_App::setup()
{
}

void _TBOX_PREFIX_App::mouseDown( MouseEvent event )
{
}

void _TBOX_PREFIX_App::update()
{
}

void _TBOX_PREFIX_App::draw()
{
	// clear out the window with black
	dx::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( _TBOX_PREFIX_App, RendererDx )
