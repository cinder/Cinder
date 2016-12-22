#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderDLLApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void CinderDLLApp::setup()
{
}

void CinderDLLApp::mouseDown( MouseEvent event )
{
}

void CinderDLLApp::update()
{
}

void CinderDLLApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( CinderDLLApp, RendererGl )
