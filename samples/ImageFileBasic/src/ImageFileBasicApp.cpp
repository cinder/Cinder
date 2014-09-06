#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererGl.h"
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
	
	gl::TextureRef		mTexture;
};

void ImageFileBasicApp::setup()
{
	try {
		fs::path path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
		if( ! path.empty() ) {
			mTexture = gl::Texture::create( loadImage( path ) );
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
			mTexture = gl::Texture::create( loadImage( path ) );
	}
	else if( event.getChar() == 's' ) {
		fs::path path = getSaveFilePath();
		if( ! path.empty() ) {
			Surface s8( mTexture->createSource() );
			writeImage( writeFile( path ), s8 );
		}
	}
}

void ImageFileBasicApp::fileDrop( FileDropEvent event )
{
	try {
		mTexture = gl::Texture::create( loadImage( event.getFile( 0 ) ) );
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
		gl::draw( mTexture, vec2( 0, 0 ) );
}

CINDER_APP_BASIC( ImageFileBasicApp, RendererGl )
