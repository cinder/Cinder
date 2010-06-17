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


template<typename T>
Vec2<T> toPolar( Vec2<T> car )
{
	const T epsilon = (T)0.0000001;
	T theta;
	if( math<T>::abs( car.x ) < epsilon ) { // x == 0
		if( math<T>::abs( car.y ) < epsilon ) theta = 0;
		else if( car.y > 0 ) theta = (T)M_PI / 2;
		else theta = ( (T)M_PI * 3 ) / 2;
	}
	else if ( car.x > 0 ) {
		if( car.y < 0 ) theta = math<T>::atan( car.y / car.x ) + 2 * (T)M_PI;
		else theta = math<T>::atan( car.y / car.x );
	}
	else // car.x < 0
		theta = (math<T>::atan( car.y / car.x ) + M_PI );

	T lenSquared = car.lengthSquared();
	return Vec2<T>( car.length(), theta );
}

template<typename T>
Vec2<T> fromPolar( Vec2<T> pol ){
	return Vec2<T>( math<T>::cos( pol.y ) *  pol.x , math<T>::sin( pol.y ) *  pol.x );
}

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
	
	toPolar( Vec2f::zero() );

	for( list<pair<Vec2f,Color> >::iterator circleIter = mCircles.begin(); circleIter != mCircles.end(); ++circleIter ) {
		gl::color( circleIter->second );
		gl::drawSolidCircle( circleIter->first, 20 );
	}
	
	mSquare->draw();
}

CINDER_APP_BASIC( ListenerBasicApp, RendererGl )
