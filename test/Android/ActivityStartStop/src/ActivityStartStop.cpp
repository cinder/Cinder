#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/android/CinderAndroid.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the App class
class ActivityStartStop : public App {
  public:
	ActivityStartStop();

	void setup();
	void mouseDrag( MouseEvent event );
#if defined( CINDER_ANDROID )
    void touchesBegan( TouchEvent event );
#endif
	void keyDown( KeyEvent event );
	void draw();

	void onActivityGainedFocus();
	void onActivityLostFocus();

	// This will maintain a list of points which we will draw line segments between
	list<vec2>		mPoints;
};

ActivityStartStop::ActivityStartStop()
{
	ci::android::setActivityGainedFocusCallback( std::bind( &ActivityStartStop::onActivityGainedFocus, this ) );
	ci::android::setActivityLostFocusCallback( std::bind( &ActivityStartStop::onActivityLostFocus, this ) );
}

void ActivityStartStop::setup()
{
	console() << "=========> ActivityStartStop::setup() <=========" << std::endl;

}

void ActivityStartStop::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

#if defined( CINDER_ANDROID )
void ActivityStartStop::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mPoints.push_back( touchIt->getPos() );
	}
}
#endif

void ActivityStartStop::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void ActivityStartStop::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	gl::color( 1.0f, 0.5f, 0.25f );	
	gl::begin( GL_LINE_STRIP );
	for( auto pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		gl::vertex( *pointIter );
	}
	gl::end();
}

void ActivityStartStop::onActivityGainedFocus()
{
	console() << "=========> ActivityStartStop::onActivityGainedFocus() <=========" << std::endl;
}

void ActivityStartStop::onActivityLostFocus()
{
	console() << "=========> ActivityStartStop::onActivityLostFocus() <=========" << std::endl;
}

// This line tells Cinder to actually create the application
CINDER_APP( ActivityStartStop, RendererGl )
