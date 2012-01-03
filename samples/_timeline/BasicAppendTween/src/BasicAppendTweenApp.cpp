#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicAppendTweenApp : public AppBasic {
  public:
	void setup();
	void setDestinations();
	void startTweening();
	void mouseDown( MouseEvent event );	
	void draw();
	
	Anim<Vec2f>		mPos;
	
	int				mNumDestinations;
	vector<Vec2f>	mDestinations;
};

void BasicAppendTweenApp::setup()
{
	mPos = getWindowCenter();
	mNumDestinations = 10;
	setDestinations();
	startTweening();
}

void BasicAppendTweenApp::mouseDown( MouseEvent event )
{
	setDestinations();
	startTweening();
}

void BasicAppendTweenApp::setDestinations()
{
	// clear all destinations
	mDestinations.clear();
	// start from current position
	mDestinations.push_back( mPos );
	// add more destinations
	for( int i=0; i<mNumDestinations-1; i++ ){
		Vec2f v( Rand::randFloat( getWindowWidth() ), Rand::randFloat( getWindowHeight() ) );
		mDestinations.push_back( v );
	}
}

void BasicAppendTweenApp::startTweening()
{
	timeline().apply( &mPos, (Vec2f)mDestinations[0], 0.5f, EaseInOutQuad() );
	for( int i=1; i<mNumDestinations; i++ ){
		timeline().appendTo( &mPos, (Vec2f)mDestinations[i], 0.5f, EaseInOutQuad() );
	}
}


void BasicAppendTweenApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.2f ) );
	gl::enableAlphaBlending();
	
	for( int i=0; i<mNumDestinations; i++ ){
		gl::color( Color( 1, 0, 0 ) );
		gl::drawSolidCircle( mDestinations[i], 3.0f );
		
		if( i > 0 ){
			gl::color( ColorA( 1, 0, 0, 0.25f ) );
			gl::drawLine( mDestinations[i], mDestinations[i-1] );
		}
	}
	
	gl::color( Color::white() );
	gl::drawSolidCircle( mPos, 20.0f );
}


CINDER_APP_BASIC( BasicAppendTweenApp, RendererGl )
