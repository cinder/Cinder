#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Utilities.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the App class
class LaunchWebBrowser : public App {
public:
	void setup();
  	void touchesBegan( TouchEvent event );
	void draw();

private:
	bool 	mLaunched = false;
};

void LaunchWebBrowser::setup()
{
}

void LaunchWebBrowser::touchesBegan( TouchEvent event )
{
	if( ! mLaunched ) {
		launchWebBrowser( Url( "http://libcinder.org" ) );
		mLaunched = true;

		console() << "Web browser launched!" << std::endl;
	}
}

void LaunchWebBrowser::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );
}

// This line tells Cinder to actually create the application
CINDER_APP( LaunchWebBrowser, RendererGl )
