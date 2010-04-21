#include "flint/app/AppBasic.h"
#include "flint/Surface.h"
#include "flint/gl/Texture.h"
#include "flint/qtime/QuickTime.h"
#include "Resources.h"

using namespace fli;
using namespace fli::app;
using std::string;

#if defined( FLI_MAC )
#define USE_DISPLAY_LINK
#endif

class QuickTimeSampleApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void setup();

	void keyDown( KeyEvent event );
	void fileDrop( FileDropEvent event );

	void update();
	void draw();

	void loadMovieFile( const std::string &path );
	void loadMovieResource();

	gl::Texture					mTexture;
	qtime::MovieGL				mMovie;
shared_array<uint8_t> data;
size_t dataSize;
};

void newFrameCallback( void *refcon )
{
	qtime::MovieBase *movie = reinterpret_cast<qtime::MovieBase*>( refcon );
	std::cout << "called at time " << movie->getCurrentTime() << " seconds" << std::endl;
}

void QuickTimeSampleApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 640, 480 );
	settings->setFullScreen( false );
	settings->setResizable( true );
#if defined( USE_DISPLAY_LINK )
	settings->enableDisplayLink();
#else
	settings->setFrameRate( 1000.0f );
#endif
}

void QuickTimeSampleApp::setup()
{
	loadMovieResource();
}

void QuickTimeSampleApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getChar() == 'r' ) {
		loadMovieResource();
	}
	else if( event.getChar() == 'o' ) {
		string moviePath = getOpenFilePath();
		if( ! moviePath.empty() )
			loadMovieFile( moviePath );
	}
}

void QuickTimeSampleApp::loadMovieResource()
{
	try {
		loadResourceMemory( "Upgrade_U_small.mov", RES_UPGADE_U_MOV, "MOV", &data, &dataSize );
		
		mMovie = qtime::MovieGL( data.get(), dataSize, "Upgrade_U_small.mov", "video/quicktime" );
		std::cout << "Dimensions:" << mMovie.getWidth() << " x " << mMovie.getHeight() << std::endl;
		std::cout << "Duration:  " << mMovie.getDuration() << " seconds" << std::endl;
		std::cout << "Frames:    " << mMovie.getNumFrames() << std::endl;
		std::cout << "Framerate: " << mMovie.getFramerate() << std::endl;				
		mMovie.setLoop( true, true );
		
#ifdef USE_DISPLAY_LINK
		mMovie.enableDisplayLink( getDisplayLink() );
#endif
		mMovie.setNewFrameCallback( newFrameCallback, &mMovie );
		mMovie.play();
	}
	catch( ... ) {
		std::cout << "Unable to load the movie." << std::endl;
		mMovie.reset();
	}

	mTexture.reset();
}

void QuickTimeSampleApp::loadMovieFile( const string &moviePath )
{
	try {
		mMovie = qtime::MovieGL( moviePath );
		std::cout << "Dimensions:" << mMovie.getWidth() << " x " << mMovie.getHeight() << std::endl;
		std::cout << "Duration:  " << mMovie.getDuration() << " seconds" << std::endl;
		std::cout << "Frames:    " << mMovie.getNumFrames() << std::endl;
		std::cout << "Framerate: " << mMovie.getFramerate() << std::endl;				
		mMovie.setLoop( true, true );
		
#ifdef USE_DISPLAY_LINK
		mMovie.enableDisplayLink( getDisplayLink() );
#endif
		mMovie.setNewFrameCallback( newFrameCallback, &mMovie );
		mMovie.play();
	}
	catch( ... ) {
		std::cout << "Unable to load the movie." << std::endl;
		mMovie.reset();
	}

	mTexture.reset();
}

void QuickTimeSampleApp::fileDrop( FileDropEvent event )
{
	loadMovieFile( event.getFile( 0 ) );
}

void QuickTimeSampleApp::update()
{
	if( mMovie )
		mTexture = mMovie.getTexture();
	else
		mTexture.reset();
}

void QuickTimeSampleApp::draw()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );

	if( mMovie && mTexture ) {
		mTexture.draw( 0, 0, getWindowWidth(), getWindowHeight() );
	}
}

FLI_APP_BASIC( QuickTimeSampleApp, RendererGL );
