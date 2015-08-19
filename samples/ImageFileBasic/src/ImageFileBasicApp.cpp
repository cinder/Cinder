#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ImageFileBasicApp : public App {
  public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void fileDrop( FileDropEvent event ) override;
	void draw() override;
	
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
	catch( Exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load image.", exc );
	}
}

void ImageFileBasicApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'o' ) {
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
		mTexture = gl::Texture::create( loadImage( loadFile( event.getFile( 0 ) ) ) );
	}
	catch( Exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load image: " << event.getFile( 0 ), exc );
	}
}

void ImageFileBasicApp::draw()
{
	gl::clear( Color( 0.5f, 0.5f, 0.5f ) );
	gl::enableAlphaBlending();
	
	if( mTexture ) {
		Rectf destRect = Rectf( mTexture->getBounds() ).getCenteredFit( getWindowBounds(), true ).scaledCentered( 0.85f );
		gl::draw( mTexture, destRect );
	}
}

CINDER_APP( ImageFileBasicApp, RendererGl )
