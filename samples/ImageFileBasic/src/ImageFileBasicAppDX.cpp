#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#include "cinder/dx/DxTexture.h"
#include "cinder/dx/dx.h"

// Uncomment this line to enable specialized PNG handling
//#include "cinder/ImageSourcePng.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ImageFileBasicApp : public App {
  public:
	void setup();
	void keyDown( KeyEvent event );
	void draw();
	
	dx::Texture		mTexture;	
};

void ImageFileBasicApp::setup()
{
	try {
		fs::path path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
		if( ! path.empty() ) {
			mTexture = dx::Texture( loadImage( path ) );
		}
	}
	catch( ci::Exception &exc ) {
		console() << "unable to load the texture file, what: " << exc.what() << endl;
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
		dx::draw( mTexture, vec2( 0, 0 ) );
}

CINDER_APP( ImageFileBasicApp, RendererDx )
