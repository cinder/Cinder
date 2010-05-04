#include "cinder/Cinder.h"

#if defined( CINDER_COCOA_TOUCH )
#include "cinder/app/AppCocoaTouch.h"
typedef ci::app::AppCocoaTouch AppBase;
#else
#include "cinder/app/AppBasic.h"
typedef ci::app::AppBasic AppBase;
#endif

#include "cinder/System.h"
#include "cinder/Rand.h"
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
			gl::color( ColorA( mColor, ( mTimeOfDeath - getElapsedSeconds() ) / 2.0 ) );
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
class MultiTouchApp : public AppBase {
 public:
	void	prepareSettings( Settings *settings );

	void	touchesBegan( TouchEvent event );
	void	touchesMoved( TouchEvent event );
	void	touchesEnded( TouchEvent event );

	void	setup();
	void	draw();
	
	map<uint32_t,TouchPoint>	mActivePoints;
	list<TouchPoint>			mDyingPoints;
};

void MultiTouchApp::setup()
{
	console() << "MT: " << System::hasMultiTouch() << " Max points: " << System::getMaxMultiTouchPoints() << std::endl;
}

void MultiTouchApp::prepareSettings( Settings *settings )
{
	settings->enableMultiTouch();
}

void MultiTouchApp::touchesBegan( TouchEvent event )
{
	console() << "Touches began:" << std::endl << event << std::endl;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		Color newColor( CM_HSV, Rand::randFloat(), 1, 1 );
		mActivePoints.insert( make_pair( touchIt->getId(), TouchPoint( touchIt->getPos(), newColor ) ) );
	}
}

void MultiTouchApp::touchesMoved( TouchEvent event )
{
	console() << "Touches moved:" << std::endl << event << std::endl;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt )
		mActivePoints[touchIt->getId()].addPoint( touchIt->getPos() );
}

void MultiTouchApp::touchesEnded( TouchEvent event )
{
	console() << "Touches ended:" << std::endl << event << std::endl;
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
		mActivePoints[touchIt->getId()].startDying();
		mDyingPoints.push_back( mActivePoints[touchIt->getId()] );
		mActivePoints.erase( touchIt->getId() );
	}
}


void MultiTouchApp::draw()
{
	gl::enableAlphaBlending();
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
	gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );

	glColor4ub( 255, 128, 64, 255 );
	gl::drawStrokedCircle( Vec2f( 123, 45 ), 50 );
	glColor4ub( 129, 128, 64, 255 );
	gl::drawLine( getWindowCenter(), getWindowSize() );
	glColor4ub( 129, 255, 64, 255 );
	gl::drawLine( Vec3f( getWindowCenter(), 0 ), Vec3f::zero() );

//	gl::drawSolidRect( Rectf( 0, 0, 200, 200 ) );

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
}

// This line tells Cinder to actually create the application
#if defined( CINDER_COCOA_TOUCH )
CINDER_APP_COCOA_TOUCH( MultiTouchApp, RendererGl )
#else
CINDER_APP_BASIC( MultiTouchApp, RendererGl )
#endif
