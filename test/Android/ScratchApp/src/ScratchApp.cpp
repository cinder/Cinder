#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Texture.h"
#include <list>
using namespace ci;
using namespace ci::app;
using namespace std;

#include "cinder/android/video/VideoPlayer.h"

// We'll create a new Cinder Application by deriving from the App class
class ScratchApp : public App {
  public:
	void setup();
	void mouseDrag( MouseEvent event );
#if defined( CINDER_ANDROID )
    void touchesBegan( TouchEvent event );
#endif
	void update();
	void draw();

	// This will maintain a list of points which we will draw line segments between
	list<vec2>		mPoints;

private:
	ci::android::video::VideoPlayerRef 	mVideoPlayer;
	gl::TextureRef 	mTexture;
};

void ScratchApp::setup()
{
	try {
		mVideoPlayer = ci::android::video::VideoPlayer::create( getAssetPath( "bbb.mp4" ) );
		mVideoPlayer->play();
	}
	catch( const std::exception& e ) {
		console() << "ERROR: " << e.what() << std::endl;
	}
}

void ScratchApp::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

#if defined( CINDER_ANDROID )
void ScratchApp::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mPoints.push_back( touchIt->getPos() );
	}
}
#endif

void ScratchApp::update()
{
	if( mVideoPlayer->isNewFrameAvailable() ) {
		mTexture = mVideoPlayer->getTexture();
	}
}

void ScratchApp::draw()
{
	gl::clear();

	gl::color( 1.0f, 1.0f, 1.0f );

	if( mTexture ) {
		mTexture->setTopDown( true );
    	gl::pushModelMatrix();
		gl::scale( 2.0f, 2.0f );
		gl::draw( mTexture );
		gl::popModelMatrix();
	}

	/*
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	gl::color( 1.0f, 0.5f, 0.25f );	
	gl::begin( GL_LINE_STRIP );
	for( auto pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		gl::vertex( *pointIter );
	}
	gl::end();
	*/
}

// This line tells Cinder to actually create the application
CINDER_APP( ScratchApp, RendererGl )
