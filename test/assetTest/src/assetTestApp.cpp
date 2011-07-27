#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class assetTestApp : public AppBasic {
  public:
	void setup();
	void draw();

	gl::Texture img;
};

void assetTestApp::setup()
{
	// test addAssetDirectory
	addAssetDirectory( getHomeDirectory() + "tmp" );
	app::console() << "File at ~/dummy.dat: " << getAssetPath( "dummy.dat" ) << std::endl;
	
	app::console() << "The full path to asset1 is " << getAssetPath( "asset1.png" ) << std::endl;
	img = loadImage( loadAsset( "asset1.png" ) );
}

void assetTestApp::draw()
{
	gl::enableAlphaBlending();
	gl::clear( Color( 1, 1, 1 ) );

	if( img )
		gl::draw( img );
}


CINDER_APP_BASIC( assetTestApp, RendererGl )
