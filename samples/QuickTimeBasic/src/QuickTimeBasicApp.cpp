#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
//#if defined( CINDER_ANDROID )
//	#include "cinder/android/video/MovieGl.h"
//#else
	#include "cinder/qtime/QuickTimeGl.h"
//#endif

using namespace ci;
using namespace ci::app;
using namespace std;

class QuickTimeSampleApp : public App {
  public:
	static void prepareSettings( Settings *settings ) { settings->setMultiTouchEnabled( false ); }

	void setup() override;

	void keyDown( KeyEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void fileDrop( FileDropEvent event ) override;

	void update() override;
	void draw() override;

	void loadMovieFile( const fs::path &path );

	gl::TextureRef			mFrameTexture;
	qtime::MovieGlRef		mMovie;
};

void QuickTimeSampleApp::setup()
{
#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
	fs::path moviePath = getAssetPath( "bbb.mp4" );
#else
	fs::path moviePath = getOpenFilePath();
	console() << "moviePath: " << moviePath << std::endl;
#endif
	if( ! moviePath.empty() )
		loadMovieFile( moviePath );
}

void QuickTimeSampleApp::keyDown( KeyEvent event )
{
#if ! defined( CINDER_ANDROID )
	if( event.getChar() == 'o' ) {
		fs::path moviePath = getOpenFilePath();
		if( ! moviePath.empty() )
			loadMovieFile( moviePath );
	}
#endif
}

void QuickTimeSampleApp::loadMovieFile( const fs::path &moviePath )
{
	try {
		// load up the movie, set it to loop, and begin playing
		mMovie = qtime::MovieGl::create( moviePath );
		//mMovie->setLoop();
		mMovie->play();
		console() << "Playing: " << mMovie->isPlaying() << std::endl;
	}
	catch( ci::Exception &exc ) {
		console() << "Exception caught trying to load the movie from path: " << moviePath << ", what: " << exc.what() << std::endl;
		mMovie.reset();
	}

	mFrameTexture.reset();
}

void QuickTimeSampleApp::mouseDown( MouseEvent event )
{
#if defined( CINDER_ANDROID )
	mMovie->seekToStart();
#endif
}

void QuickTimeSampleApp::fileDrop( FileDropEvent event )
{
	loadMovieFile( event.getFile( 0 ) );
}

void QuickTimeSampleApp::update()
{
	if( mMovie )
		mFrameTexture = mMovie->getTexture();

	static bool sPrintedDone = false;
	if( ! sPrintedDone && mMovie->isDone() ) {
		console() << "Done Playing" << std::endl;
		sPrintedDone = true;
	}
}

void QuickTimeSampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	if( mFrameTexture ) {
		Rectf centeredRect = Rectf( mFrameTexture->getBounds() ).getCenteredFit( getWindowBounds(), true );
#if defined( CINDER_ANDROID )
		std::swap( centeredRect.y1, centeredRect.y2 );
#endif
		gl::draw( mFrameTexture, centeredRect );
	}
}

CINDER_APP( QuickTimeSampleApp, RendererGl, QuickTimeSampleApp::prepareSettings );
