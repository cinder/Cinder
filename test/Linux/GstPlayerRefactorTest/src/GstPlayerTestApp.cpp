#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTimeGl.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class GstPlayerTestApp : public App {
  public:
	enum TestCase{
		TEST_NONE = 0,
		TEST_PLAY_PAUSE,
		TEST_SEEKING,
		TEST_LOOPING,
		TEST_RELOADING
	};
	static void prepareSettings( Settings *settings ) { }

	void setup() override;

	void keyDown( KeyEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void fileDrop( FileDropEvent event ) override;

	void update() override;
	void draw() override;

	void loadMovieFile( const fs::path &path );

	void testPlayPause();
	void toggleVideoPlayback();

	void testSeek();
	void newSeek();

	void testCurrentCase();

	gl::TextureRef				mFrameTexture;
	qtime::MovieGlRef			mMovie;
	Timer						mTriggerTimer;

	// Play / Pause
	double 						mTriggerPlayPauseRate = 4.0;
	bool 						mRandomizePlayPauseRate = true;
	double 						mTimeLastPlayPauseTrigger;

	// Seek
	double 						mTriggerSeekRate = 4.0;
	bool						mRandomizeSeekRate = true;
	double 						mTimeLastSeekTrigger;

	TestCase					mCurrentTestCase = TEST_SEEKING;
};

void GstPlayerTestApp::setup()
{
	CI_LOG_I( "\n--- TESTING LEVELS START----");
	
	CI_LOG_V( "verbose" );
	CI_LOG_D( "debug" );
	CI_LOG_I( "info" );
	CI_LOG_W( "warning" );
	CI_LOG_E( "error" );
	CI_LOG_F( "fatal" );
	
	CI_LOG_I( "\n--- TESTING LEVELS END----\n\n");

	fs::path moviePath = getAssetPath( "bbb.mp4" );
	if( ! moviePath.empty() )
		loadMovieFile( moviePath );
	mTriggerTimer.start();
	mTimeLastPlayPauseTrigger = mTriggerTimer.getSeconds();
}

void GstPlayerTestApp::testCurrentCase() 
{
	switch( mCurrentTestCase ) {
		case TEST_PLAY_PAUSE:
		{
			testPlayPause();
			break;
		}
		case TEST_SEEKING:
		{
			testSeek();
			break;
		}
		default:
		break;
	}
}

void GstPlayerTestApp::toggleVideoPlayback()
{
	if( ! mMovie ) return;
	CI_LOG_I( "---------- TOGGLE VIDEO START ----------" );
	if( mMovie->isPlaying() ) {
		CI_LOG_I( "Video is playing -- Will attempt to STOP." );
		mMovie->stop();
		if( mMovie->isPlaying() ) {
			CI_LOG_E( "Video still playing after STOP requested!! " );
			CI_LOG_I( "---------- TOGGLE VIDEO FAILED ----------" );
			return;
		}
	}
	else {
		CI_LOG_I( "Video is stopped -- Will attempt to PLAY." );
		mMovie->play();
		if( ! mMovie->isPlaying() ) {
			CI_LOG_E( "Video did not start playing after PLAY requested!! " );
			CI_LOG_I( "---------- TOGGLE VIDEO FAILED ----------" );
			return;
		}
	}
	CI_LOG_I( "---------- TOGGLE VIDEO SUCCESS ----------" );
}

void GstPlayerTestApp::testPlayPause()
{
	auto now = mTriggerTimer.getSeconds();
	if( now - mTimeLastPlayPauseTrigger >= mTriggerPlayPauseRate ) {
		toggleVideoPlayback();
		mTimeLastPlayPauseTrigger = now;
		if( mRandomizePlayPauseRate ) mTriggerPlayPauseRate = randFloat( .1f, .4f );
	}
}

void GstPlayerTestApp::testSeek()
{
	auto now = mTriggerTimer.getSeconds();
	if( now - mTimeLastSeekTrigger >= mTriggerSeekRate ) {
		newSeek();
		mTimeLastSeekTrigger = now;
		if( mRandomizeSeekRate ) mTriggerSeekRate = randFloat( .1f, .4f );
	}
}

void GstPlayerTestApp::newSeek()
{
	if( ! mMovie ) return;

	CI_LOG_I( "---------- NEW SEEK START ----------" );
	CI_LOG_I( "CURRENT POS : " << mMovie->getCurrentTime() );
	auto videoDuration = mMovie->getDuration();
	auto jumpTo = randFloat( 0.0f, videoDuration );
	CI_LOG_I( "NEW SEEK JUMP TO : " << jumpTo );
	mMovie->seekToTime( jumpTo );
	CI_LOG_I( "---------- NEW SEEK END ----------" );
}

void GstPlayerTestApp::update()
{

	testCurrentCase();

	if( mMovie ) {
		mFrameTexture = mMovie->getTexture();
	}

	static bool sPrintedDone = false;
	if( ! sPrintedDone && mMovie->isDone() ) {
		console() << "Done Playing" << std::endl;
		sPrintedDone = true;
	}
}

void GstPlayerTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	if( mFrameTexture ) {
		Rectf centeredRect = Rectf( mFrameTexture->getBounds() ).getCenteredFit( getWindowBounds(), true );
		gl::draw( mFrameTexture, centeredRect );
	}
}

void GstPlayerTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'o' ) {
		fs::path moviePath = getOpenFilePath();
		if( ! moviePath.empty() )
			loadMovieFile( moviePath );
	}
	else if( event.getChar() == 'p' ) {
		if( mMovie ) mMovie->stop();
	}
	else if( event.getChar() == 'l' ) {
		if( mMovie ) mMovie->play();
	}
	else if( event.getChar() == 't' ) {
	}
}

void GstPlayerTestApp::loadMovieFile( const fs::path &moviePath )
{
	try {
		// load up the movie, set it to loop, and begin playing
		mMovie = qtime::MovieGl::create( moviePath );
		mMovie->setLoop();
		mMovie->play();
		console() << "Playing: " << mMovie->isPlaying() << std::endl;
	}
	catch( ci::Exception &exc ) {
		console() << "Exception caught trying to load the movie from path: " << moviePath << ", what: " << exc.what() << std::endl;
		mMovie.reset();
	}

	mFrameTexture.reset();
}

void GstPlayerTestApp::mouseDown( MouseEvent event )
{

}

void GstPlayerTestApp::fileDrop( FileDropEvent event )
{
	loadMovieFile( event.getFile( 0 ) );
}

CINDER_APP( GstPlayerTestApp, RendererGl, GstPlayerTestApp::prepareSettings );