#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

// Uncomment this line to enable specialized PNG handling
//#include "cinder/ImageSourcePng.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ImageFileBasicApp : public AppBasic {
  public:
	void setup();
	void keyDown( KeyEvent event );
	void fileDrop( FileDropEvent event );	
	void draw();
	
	gl::Texture		mTexture;	
};

void ImageFileBasicApp::setup()
{
	try {
		std::string path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
		if( ! path.empty() ) {
			mTexture = gl::Texture( loadImage( path ) );
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
		std::string path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
		if( ! path.empty() )
			mTexture = gl::Texture( loadImage( path ) );
	}
}

void ImageFileBasicApp::fileDrop( FileDropEvent event )
{
	try {
		mTexture = gl::Texture( loadImage( event.getFile( 0 ) ) );
	}
	catch( ... ) {
		console() << "unable to load the texture file!" << std::endl;
	};
}

void ImageFileBasicApp::draw()
{
	gl::clear( Color( 0.5f, 0.5f, 0.5f ) );
	gl::enableAlphaBlending();
	
	if( mTexture )
		gl::draw( mTexture, Vec2f( 0, 0 ) );
}

CINDER_APP_BASIC( ImageFileBasicApp, RendererGl )
