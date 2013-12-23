#include "cinder/Cinder.h"
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "TuioClient.h"

using namespace ci;
using namespace ci::app;

#include <vector>
#include <map>
#include <list>
using namespace std;

struct TouchPoint {
	TouchPoint() {}
	TouchPoint( const Vec2f &initialPt, const Color &color ) : mColor( color ), mTimeOfDeath( -1.0 ) 
	{
		mLine.push_back( initialPt ); 
	}
	
	void addPoint( const Vec2f &pt ) { mLine.push_back( pt ); }
	
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
	
	PolyLine<Vec2f>	mLine;
	Color			mColor;
	float			mTimeOfDeath;
};

// We'll create a new Cinder Application by deriving from the BasicApp class
class MultiTouchApp : public AppNative {
 public:
	void	prepareSettings( Settings *settings );

	void	mouseDown( MouseEvent event );
	void	mouseDrag( MouseEvent event );	

	void	touchesBegan( TouchEvent event );
	void	touchesMoved( TouchEvent event );
	void	touchesEnded( TouchEvent event );

	void	setup();
	void	draw();
	void	keyDown( KeyEvent event ) { setFullScreen( ! isFullScreen() ); }
	
	map<uint32_t,TouchPoint>	mActivePoints;
	list<TouchPoint>			mDyingPoints;
	tuio::Client				mTuio;
};

void MultiTouchApp::prepareSettings( Settings *settings )
{
	settings->enableMultiTouch();
}

void MultiTouchApp::setup()
{
	console() << "MT: " << System::hasMultiTouch() << " Max points: " << System::getMaxMultiTouchPoints() << std::endl;
	mTuio.registerTouches( this );
	mTuio.connect();
}

void MultiTouchApp::touchesBegan( TouchEvent event )
{
console() << "Began: " << event << std::endl;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		Color newColor( CM_HSV, Rand::randFloat(), 1, 1 );
		mActivePoints.insert( make_pair( touchIt->getId(), TouchPoint( touchIt->getPos(), newColor ) ) );
	}
}

void MultiTouchApp::touchesMoved( TouchEvent event )
{
console() << "Moved: " << event << std::endl;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
		mActivePoints[touchIt->getId()].addPoint( touchIt->getPos() );
}

void MultiTouchApp::touchesEnded( TouchEvent event )
{
console() << "Ended: " << event << std::endl;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mActivePoints[touchIt->getId()].startDying();
		mDyingPoints.push_back( mActivePoints[touchIt->getId()] );
		mActivePoints.erase( touchIt->getId() );
	}
}

void MultiTouchApp::mouseDown( MouseEvent event )
{
	console() << "Mouse down: " << event.isRight() << " & " << event.isControlDown() << std::endl;	
}

void MultiTouchApp::mouseDrag( MouseEvent event )
{
	console() << "Mouse drag: " << std::endl;	
}

void MultiTouchApp::draw()
{
	gl::enableAlphaBlending();
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );

	for( auto activeIt = mActivePoints.begin(); activeIt != mActivePoints.end(); ++activeIt ) {
		activeIt->second.draw();
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
	vector<TouchEvent::Touch> activeTouches( mTuio.getActiveTouches() );
	for( auto touchIt = activeTouches.begin(); touchIt != activeTouches.end(); ++touchIt )
		gl::drawStrokedCircle( touchIt->getPos(), 20.0f );
}

CINDER_APP_NATIVE( MultiTouchApp, RendererGl )