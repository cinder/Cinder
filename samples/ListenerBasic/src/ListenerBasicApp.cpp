#include "cinder/app/AppBasic.h"
#include "cinder/Color.h"
#include <list>
#include <utility>

#include "SquareListener.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ListenerBasicApp : public AppBasic {
 public:
	void setup();
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void draw();
	
	list<pair<Vec2f,Color> >		mCircles;
	SquareListener					*mSquare;
};


void ListenerBasicApp::setup()
{
	// This installs a listener which intercepts mouse-down events
	// when the user clicks inside the square
	mSquare = new SquareListener( this );
}

void ListenerBasicApp::mouseDown( MouseEvent event )
{
	// add a new circle with a color based on position
	Color color( CM_HSV, event.getX() / (float)getWindowWidth(), event.getY() / (float)getWindowHeight(), 1.0f );
	mCircles.push_back( make_pair( Vec2f( event.getPos() ), color ) );
}

void ListenerBasicApp::mouseDrag( MouseEvent event )
{
	mouseDown( event );
}

void ListenerBasicApp::draw()
{
	gl::clear( Color( 0.3f, 0.3f, 0.3f ) );
	
	for( list<pair<Vec2f,Color> >::iterator circleIter = mCircles.begin(); circleIter != mCircles.end(); ++circleIter ) {
		gl::color( circleIter->second );
		gl::drawSolidCircle( circleIter->first, 20 );
	}
	
	mSquare->draw();
}

CINDER_APP_BASIC( ListenerBasicApp, RendererGl )
