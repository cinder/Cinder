#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/System.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#include <vector>
#include <map>
#include <list>

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

class RetinaSampleApp : public AppNative {
public:
	void prepareSettings( Settings *settings ) override;
	
	void mouseDrag( MouseEvent event );
	void mouseDown( MouseEvent event );
	
	void touchesBegan( TouchEvent event );
	void touchesMoved( TouchEvent event );
	void touchesEnded( TouchEvent event );
		
	void setup();
	void draw();
	
	map<uint32_t,TouchPoint>	mActivePoints;
	list<TouchPoint>			mDyingPoints;
};

void RetinaSampleApp::prepareSettings( Settings *settings )
{
	settings->enableMultiTouch( true );
	settings->enableHighDensityDisplay();
	
	console() << "HDD Enabled = " << settings->isHighDensityDisplayEnabled() << endl;
	console() << "Multitouch Enabled = " << settings->isHighDensityDisplayEnabled() << endl;
	
}

void RetinaSampleApp::setup()
{
	console() << "MT: " << System::hasMultiTouch() << " Max points: " << System::getMaxMultiTouchPoints() << std::endl;

}

void RetinaSampleApp::mouseDrag( MouseEvent event )
{
	console() << "Mouse Drag: " << event.getPos()	<< endl;
}

void RetinaSampleApp::mouseDown( MouseEvent event )
{
	console() << "getWindowContentScale = " << getWindowContentScale() << endl;
	console() << "Event Pos: " << event.getPos()	<< endl;
	
	console() << "getWindowWidth =  " << getWindowWidth() << std::endl;
	console() << "ContentScale = " << getWindowContentScale() << endl;
	console() << "getWindowCenter() = " << getWindowCenter() << endl;
	console() << "toPixels( 1.0f ) = " << toPixels( 1.0f ) << endl;
}

void RetinaSampleApp::touchesBegan( TouchEvent event )
{
	console() << "Began Pos: " << event << std::endl;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		Color newColor( CM_HSV, Rand::randFloat(), 1, 1 );
		mActivePoints.insert( make_pair( touchIt->getId(), TouchPoint( touchIt->getPos(), newColor ) ) );
	}
}

void RetinaSampleApp::touchesMoved( TouchEvent event )
{
	// console() << "Moved: " << event << std::endl;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
		mActivePoints[touchIt->getId()].addPoint( touchIt->getPos() );
}

void RetinaSampleApp::touchesEnded( TouchEvent event )
{
	// console() << "Ended: " << event << std::endl;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mActivePoints[touchIt->getId()].startDying();
		mDyingPoints.push_back( mActivePoints[touchIt->getId()] );
		mActivePoints.erase( touchIt->getId() );
	}
}

void RetinaSampleApp::draw()
{
	gl::pushMatrices();
	gl::enableAlphaBlending();
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
	
	for( map<uint32_t,TouchPoint>::const_iterator activeIt = mActivePoints.begin(); activeIt != mActivePoints.end(); ++activeIt ) {
		activeIt->second.draw();
	}
	
	for( list<TouchPoint>::iterator dyingIt = mDyingPoints.begin(); dyingIt != mDyingPoints.end(); ) {
		dyingIt->draw();
		if( dyingIt->isDead() )
			dyingIt = mDyingPoints.erase( dyingIt );
		else
			++dyingIt;
	}
	
	// draw yellow circles at the active touch points
	gl::color( Color( 1, 1, 0 ) );
	for( vector<TouchEvent::Touch>::const_iterator touchIt = getActiveTouches().begin(); touchIt != getActiveTouches().end(); ++touchIt )
		gl::drawStrokedCircle( touchIt->getPos(), toPoints( 20.0f ) );
	
	gl::translate( getWindowCenter() );
	gl::rotate( getElapsedSeconds() * 5 );
	// gl::drawSolidRect( Rectf( toPixels( Area( -100, -100, 100, 100 ) ) ) );
	gl::drawSolidRect( Rectf( -100, -100, 100, 100 ) );
	gl::popMatrices();

}

CINDER_APP_NATIVE( RetinaSampleApp, RendererGl )
