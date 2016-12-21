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

static int sFrameNum = 0;

class GstPlayerTestApp : public App {
  public:
	enum TestCase{
		TEST_NONE = 0,
		TEST_PLAY_PAUSE,
		TEST_SEEKING,
		TEST_LOOPING,
		TEST_RELOADING,
                TEST_STEP_FORWARD
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

    void testStepForward();
    void newStepFwd();

    void testReload();
    void newLoad();

	void testCurrentCase();

	gl::TextureRef		mFrameTexture;
	qtime::MovieGlRef	mMovie;
	Timer			mTriggerTimer;

	// Play / Pause
	double 			mTriggerPlayPauseRate   = 4.0;
	bool 			mRandomizePlayPauseRate = true;
	double 			mTimeLastPlayPauseTrigger;

	// Seek
	double 			mTriggerSeekRate    = 4.0;
	bool			mRandomizeSeekRate  = true;
	double 			mTimeLastSeekTrigger;

    // Step
	double 			mTriggerStepFwdRate    = 4.0;
	bool			mRandomizeStepFwdRate  = true;
	double 			mTimeLastStepFwdTrigger;
	float           mLastStepPosition   = 0.0f;

    // Reload
    double          mTriggerReloadRate = 4.0;
    bool            mRandomizeReloadRate = true;
    double          mTimeLastReloadTrigger;
    std::vector<fs::path> mReloadPaths;

	TestCase		mCurrentTestCase = TEST_RELOADING;

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

    loadMovieFile( "https://download.blender.org/durian/trailer/sintel_trailer-480p.mp4" );
    mReloadPaths.push_back( "http://pdl.warnerbros.com/wbol/us/dd/med/northbynorthwest/quicktime_page/nbnf_airplane_explosion_qt_500.mov" );
    mReloadPaths.push_back( "https://download.blender.org/durian/trailer/sintel_trailer-480p.mp4" );
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
        case TEST_STEP_FORWARD:
        {
            testStepForward();
            break;
        }
        case TEST_RELOADING:
        {
            testReload();
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

void GstPlayerTestApp::testReload()
{
    auto now = mTriggerTimer.getSeconds();
    if( now - mTimeLastReloadTrigger >= mTriggerReloadRate ) {
        newLoad();
        mTimeLastReloadTrigger = now;
        if( mRandomizeReloadRate ) mTriggerReloadRate = randFloat( 23.5f, 45.f );
    }
}

void GstPlayerTestApp::newLoad()
{
    static int currentMovieIndex = 0;
    fs::path movieToLoad = mReloadPaths[ currentMovieIndex % 2 ];
    currentMovieIndex++;
    loadMovieFile( movieToLoad );
}

void GstPlayerTestApp::testStepForward()
{
    auto now = mTriggerTimer.getSeconds();
    if( now - mTimeLastStepFwdTrigger >= mTriggerStepFwdRate ) {
        newStepFwd();
        mTimeLastStepFwdTrigger = now;
        if( mRandomizeStepFwdRate ) mTriggerStepFwdRate = randFloat( .1f, .4f );
    }
}

void GstPlayerTestApp::newStepFwd()
{
    if( ! mMovie ) return;
    CI_LOG_I( "---------- NEW STEP FWD START ----------" );
    CI_LOG_I( "POS BEFORE STEP : " << mMovie->getCurrentTime() );

    // Pause the pipeline in order to get a better feeling of 
    // the frame-by-frame action.
    if( mMovie->isPlaying() ) mMovie->stop();

    if( ! mMovie->stepForward() ) {
        CI_LOG_W( "Failed to step forward!!!" );
    }
    // We have reached the end, so wrap and start over..
    if( mMovie->getCurrentTime() == mLastStepPosition ) {
        mMovie->seekToTime( 0.0f );
    }
    mLastStepPosition = mMovie->getCurrentTime();
    CI_LOG_I( "POS AFTER STEP : " << mMovie->getCurrentTime() );
    CI_LOG_I( "---------- NEW STEP FWD END ----------" );
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
    if( event.getCode() == KeyEvent::KEY_o ) {
        fs::path moviePath = getOpenFilePath();
        if( ! moviePath.empty() )
            loadMovieFile( moviePath );
    }
    else if( event.getCode() == KeyEvent::KEY_p ) {
        if( mMovie ) mMovie->stop();
    }
    else if( event.getCode() == KeyEvent::KEY_l ) {
        if( mMovie ) mMovie->play();
    }
    else if( event.getCode() == KeyEvent::KEY_t ) {
        if( mMovie ) mMovie->stepForward();
    }
    else if( event.getCode() == KeyEvent::KEY_f ) {
        if( mMovie ) mMovie->stop();
        mMovie->seekToFrame( sFrameNum++ );
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
