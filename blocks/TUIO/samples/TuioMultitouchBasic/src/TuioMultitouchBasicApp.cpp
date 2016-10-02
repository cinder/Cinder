#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"

#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/PolyLine.h"
#include "cinder/gl/gl.h"

#include "cinder/tuio/Tuio.h"

using namespace ci;
using namespace ci::app;

#include <vector>
#include <map>
#include <list>
using namespace std;

struct TouchPoint {
	TouchPoint() {}
	TouchPoint( const vec2 &initialPt, const Color &color )
		: mColor( color ), mTimeOfDeath( -1.0 )
	{
		mLine.push_back( initialPt ); 
	}
	
	void addPoint( const vec2 &pt ) { mLine.push_back( pt ); }
	
	void draw() const
	{
		if( mTimeOfDeath > 0 ) // are we dying? then fade out
			gl::color( ColorA( mColor, ( mTimeOfDeath - getElapsedSeconds() ) / 2.0f ) );
		else
			gl::color( mColor );
		
		gl::draw( mLine );
	}
	
	void startDying() { mTimeOfDeath = getElapsedSeconds() + 2.0f; } // two seconds til dead

	bool isDead() const { return getElapsedSeconds() > mTimeOfDeath; }
	
	PolyLine2f	mLine;
	Color		mColor;
	float		mTimeOfDeath;
};

// We'll create a new Cinder Application by deriving from the BasicApp class
class MultiTouchApp : public App {
 public:
	MultiTouchApp();

	void	touchesBegan( TouchEvent event ) override;
	void	touchesMoved( TouchEvent event ) override;
	void	touchesEnded( TouchEvent event ) override;

	void	setup() override;
	void	draw() override;
	void	keyDown( KeyEvent event ) override;
	
	map<uint32_t,TouchPoint>	mActivePoints;
	list<TouchPoint>			mDyingPoints;
	tuio::Receiver				mTuio;
};

MultiTouchApp::MultiTouchApp()
: App(), mTuio( app::getWindow() )
{
}

void MultiTouchApp::setup()
{
	mTuio.connect();
}

void MultiTouchApp::touchesBegan( TouchEvent event )
{
	for( auto & touch : event.getTouches() ) {
		Color newColor( CM_HSV, Rand::randFloat(), 1, 1 );
		mActivePoints.insert( make_pair( touch.getId(), TouchPoint( touch.getPos(), newColor ) ) );
	}
}

void MultiTouchApp::touchesMoved( TouchEvent event )
{
	for( auto & touch : event.getTouches() )
		mActivePoints[touch.getId()].addPoint( touch.getPos() );
}

void MultiTouchApp::touchesEnded( TouchEvent event )
{
	for( auto & touch : event.getTouches() ) {
		mActivePoints[touch.getId()].startDying();
		mDyingPoints.push_back( mActivePoints[touch.getId()] );
		mActivePoints.erase( touch.getId() );
	}
}

void MultiTouchApp::keyDown( KeyEvent event )
{
	 if( event.getChar() == 'f' )
		 setFullScreen( ! isFullScreen() );
}

void MultiTouchApp::draw()
{
	gl::enableAlphaBlending();
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );

	for( auto & active : mActivePoints ) {
		active.second.draw();
	}

	for( auto dyingIt = mDyingPoints.begin(); dyingIt != mDyingPoints.end(); ) {
		dyingIt->draw();
		if( dyingIt->isDead() )
			dyingIt = mDyingPoints.erase( dyingIt );
		else
			++dyingIt;
	}
	
	// draw yellow circles at the active touch points
	gl::color( Color( 1, 1, 0 ) );
//	vector<TouchEvent::Touch> activeTouches( mTuio.getActiveTouches() );
//	for( auto touchIt = activeTouches.begin(); touchIt != activeTouches.end(); ++touchIt )
//		gl::drawStrokedCircle( touchIt->getPos(), 20.0f );
}

CINDER_APP( MultiTouchApp, RendererGl, []( App::Settings *settings ) {
	settings->setMultiTouchEnabled( false );
} )
