#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Utilities.h"

#include <string>
using std::string;

using namespace ci;
using namespace ci::app;

class ImageFileTestApp : public AppBasic {
 public:	
	void setup();
	void keyDown( KeyEvent event );
	void fileDrop( FileDropEvent event );	
	void draw();

	gl::Texture		mTexture;	
};

void ImageFileTestApp::setup() {
	try {
		std::string path = getOpenFilePath( "" );
		if( ! path.empty() ) {
			mTexture = gl::Texture( loadImage( DataSourcePath::createRef( path ) ) );
		}
	}
	catch( ... ) {
		std::cout << "unable to load the texture file!" << std::endl;
	}
}

void ImageFileTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getCode() == app::KeyEvent::KEY_ESCAPE ) {
		setFullScreen( false );
	}
	else if( event.getChar() == 'o' ) {
		std::string path = getOpenFilePath( "" );/*, &ImageFile::getExtensions() );*/
		if( ! path.empty() )
			mTexture = gl::Texture( loadImage( DataSourcePath::createRef( path ) ) );
	}
	else if( event.getChar() == 'u' ) {
		mTexture = gl::Texture( loadImage( DataSourceUrl::createRef( Url( "http://i.cdn.turner.com/cnn/.element/img/3.0/global/header/hdr-main.gif" ) ) ) );
	}
	else if( event.getChar() == 'p' ) { // pass thru
		std::string path = getOpenFilePath();
		if( path.empty() ) return;
		ImageSourceRef imgSrc = loadImage( DataSourcePath::createRef( path ) );
		path = getSaveFilePath();
		if( path.empty() ) return;		
		writeImage( DataTargetPath::createRef( path ), imgSrc );
	}
	else if( event.getChar() == 'b' ) {
		Buffer buffer( loadStreamBuffer( loadFileStream( "C:\\Images\\crunk.jpg" ) ) );
		mTexture = gl::Texture( loadImage( DataSourceBuffer::createRef( buffer ) ) );
	}
	else if( event.getChar() == '8' ) {
		writeImage( writeFile( "~/crunk.tif" ), Surface( 12, 12, true ) );
	}	
	else if( event.getChar() == 's' ) {
		writeImage( writeFile( "~/crunk.tif" ), loadImage( loadUrl( Url( "http://i.cdn.turner.com/cnn/.element/img/3.0/global/header/hdr-main.gif" ) ) ) );
	}
	else if( event.getChar() == 'm' ) {
		writeImage( DataTargetStream::createRef( writeFileStream( getHomeDirectory() + "crunk.png" ) ), mTexture );
		//writeImage( writeFile( getHomeDirectory() + "crunk.png" ), mTexture );
	}
}

void ImageFileTestApp::fileDrop( FileDropEvent event )
{
	try {
		mTexture = gl::Texture( loadImage( DataSourcePath::createRef( event.getFile( 0 ) ) ) );
	}
	catch( ... ) {
		std::cout << "unable to load the texture file!" << std::endl;
	};
}

void ImageFileTestApp::draw()
{
	glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	gl::enableAlphaBlending();

	glEnable( GL_TEXTURE_2D );

	if( mTexture ) {
		mTexture.draw( 0, 0, getWindowWidth(), getWindowHeight() );
	}
}


CINDER_APP_BASIC( ImageFileTestApp, RendererGL )
