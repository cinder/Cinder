#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class assetTestApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void assetTestApp::setup()
{
	// test addAssetDirectory
//	addAssetDirectory( getHomeDirectory() + "tmp" );
//	app::console() << "File at ~/dummy.dat: " << getAssetPath( "dummy.dat" ) << std::endl;
	
	app::console() << "The full path to asset1 is " << getAssetPath( "asset1.png" ) << std::endl;
	loadImage( loadAsset( "asset1.png" ) );
}

void assetTestApp::mouseDown( MouseEvent event )
{
}

void assetTestApp::update()
{
}

void assetTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}


CINDER_APP_BASIC( assetTestApp, RendererGl )
