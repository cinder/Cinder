#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicAppendTweenApp : public App {
  public:
	void setup() override;
	void setDestinations();
	void startTweening();
	void mouseDown( MouseEvent event ) override;	
	void draw() override;
	
	Anim<vec2>		mPos;
	
	int				mNumDestinations;
	vector<vec2>	mDestinations;
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
		vec2 v( Rand::randFloat( getWindowWidth() ), Rand::randFloat( getWindowHeight() ) );
		mDestinations.push_back( v );
	}
}

void BasicAppendTweenApp::startTweening()
{
	timeline().apply( &mPos, (vec2)mDestinations[0], 0.5f, EaseInOutQuad() );
	for( int i=1; i<mNumDestinations; i++ ){
		timeline().appendTo( &mPos, (vec2)mDestinations[i], 0.5f, EaseInOutQuad() );
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


CINDER_APP( BasicAppendTweenApp, RendererGl )
