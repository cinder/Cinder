#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/dx/DxTexture.h"
#include "cinder/dx/dx.h"

// Uncomment this line to enable specialized PNG handling
//#include "cinder/ImageSourcePng.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ImageFileBasicApp : public AppBasic {
  public:
	void setup();
	void keyDown( KeyEvent event );
	void draw();
	
	dx::Texture		mTexture;	
};

void ImageFileBasicApp::setup()
{
	try {
		console() << "unable to load the texture file!" << std::endl;
		fs::path path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
		if( ! path.empty() ) {
			mTexture = dx::Texture( loadImage( path ) );
		}
	}
	catch( ... ) {
		console() << "unable to load the texture file!" << std::endl;
	}
}

void ImageFileBasicApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getCode() == app::KeyEvent::KEY_ESCAPE ) {
		setFullScreen( false );
	}
	else if( event.getChar() == 'o' ) {
		fs::path path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
		if( ! path.empty() )
			mTexture = dx::Texture( loadImage( path.string() ) );
	}
}



void ImageFileBasicApp::draw()
{
	dx::clear();
	dx::enableAlphaBlending();
	
	if( mTexture )
		dx::draw( mTexture, Vec2f( 0, 0 ) );
}

CINDER_APP_BASIC( ImageFileBasicApp, RendererDx )
