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
class TuioMultiTouchBasicApp : public App {
public:
	TuioMultiTouchBasicApp();

	void	touchesBegan( TouchEvent event ) override;
	void	touchesMoved( TouchEvent event ) override;
	void	touchesEnded( TouchEvent event ) override;

	void	setup() override;
	void	draw() override;
	void	keyDown( KeyEvent event ) override;

	map<uint32_t, TouchPoint>	mActivePoints;
	list<TouchPoint>			mDyingPoints;
	osc::ReceiverUdp			mOscReceiver;
	tuio::Receiver				mTuio;
};

TuioMultiTouchBasicApp::TuioMultiTouchBasicApp()
: App(), mOscReceiver( tuio::Receiver::DEFAULT_TUIO_PORT ),
	mTuio( app::getWindow(), &mOscReceiver )
{
}

void TuioMultiTouchBasicApp::setup()
{
	// Bind the Osc Receiver...
	try {
		mOscReceiver.bind();
	}
	catch( const ci::Exception &ex ) {
		CI_LOG_EXCEPTION( "OscReceiver bind", ex );
		quit();
	}
	// And listen for messages.
	mOscReceiver.listen(
		[]( asio::error_code ec, asio::ip::udp::endpoint ep ) -> bool {
		if( ec ) {
			CI_LOG_E( "Error on listener: " << ec.message() << " Error Value: " << ec.value() );
			return false;
		}
		else
			return true;
	} );
}

void TuioMultiTouchBasicApp::touchesBegan( TouchEvent event )
{
	for( auto & touch : event.getTouches() ) {
		Color newColor( CM_HSV, Rand::randFloat(), 1, 1 );
		mActivePoints.insert( make_pair( touch.getId(), TouchPoint( touch.getPos(), newColor ) ) );
	}
}

void TuioMultiTouchBasicApp::touchesMoved( TouchEvent event )
{
	for( auto & touch : event.getTouches() )
		mActivePoints[touch.getId()].addPoint( touch.getPos() );
}

void TuioMultiTouchBasicApp::touchesEnded( TouchEvent event )
{
	for( auto & touch : event.getTouches() ) {
		mActivePoints[touch.getId()].startDying();
		mDyingPoints.push_back( mActivePoints[touch.getId()] );
		mActivePoints.erase( touch.getId() );
	}
}

void TuioMultiTouchBasicApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( !isFullScreen() );
}

void TuioMultiTouchBasicApp::draw()
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

CINDER_APP( TuioMultiTouchBasicApp, RendererGl, []( App::Settings *settings ) {
	settings->setMultiTouchEnabled( false );
} )
