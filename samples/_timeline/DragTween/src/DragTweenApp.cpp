#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Easing.h"
#include "cinder/Timeline.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

class Circle {
  public:
	Circle( Color color, float radius, vec2 initialPos, vec2 homePos )
		: mColor( color ), mRadius( radius ), mPos( initialPos ), mHomePos( homePos )
	{}
	
	void draw() const {
		gl::color( ColorA( mColor, 0.75f ) );
		gl::drawSolidCircle( mPos, mRadius );
	}

	void startDrag() {
		mPos.stop();
	}
	
	void dragRelease() {
		// return to our home position in 1sec, easing using EaseOutBack
		app::timeline().apply( &mPos, mHomePos, 1.0f, EaseOutBack( 3 ) );
	}
	
	Color				mColor;
	vec2				mHomePos;
	Anim<vec2>			mPos;
	Anim<float>			mRadius;
};

class DragTweenApp : public App {
  public:
    static void prepareSettings( Settings *settings ) { settings->setMultiTouchEnabled( false ); }
	void setup();
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseUp( MouseEvent event );
	void draw();
	
	vector<Circle>			mCircles;
	Circle					*mCurrentDragCircle;
};

void DragTweenApp::setup()
{
	// setup the initial animation
	const size_t numCircles = 35;
	for( size_t c = 0; c < numCircles; ++c ) {
		float angle = c / (float)numCircles * 4 * M_PI;
		vec2 pos = getWindowCenter() + ( 50 + c / (float)numCircles * 200 ) * vec2( cos( angle ), sin( angle ) );
		mCircles.push_back( Circle( Color( CM_HSV, c / (float)numCircles, 1, 1 ), 0, getWindowCenter(), pos ) );
		timeline().apply( &mCircles.back().mPos, pos, 0.5f, EaseOutAtan( 10 ) ).timelineEnd( -0.45f );
		timeline().apply( &mCircles.back().mRadius, 30.0f, 0.5f, EaseOutAtan( 10 ) ).timelineEnd( -0.5f );
	}

	mCurrentDragCircle = 0;
}

void DragTweenApp::mouseDown( MouseEvent event )
{
	// see if the mouse is in any of the circles
	vector<Circle>::iterator circleIt = mCircles.end();
	for( circleIt = mCircles.begin(); circleIt != mCircles.end(); ++circleIt )
		if( glm::distance( circleIt->mPos(), (vec2)event.getPos() ) <= circleIt->mRadius )
			break;

	// if we hit one, tell it to startDrag()
	if( circleIt != mCircles.end() ) {
		mCurrentDragCircle = &(*circleIt);
		mCurrentDragCircle->startDrag();
	}
}

void DragTweenApp::mouseDrag( MouseEvent event )
{
	// if we're dragging a circle, set its position to be where the mouse is
	if( mCurrentDragCircle )
		mCurrentDragCircle->mPos = event.getPos();
}

void DragTweenApp::mouseUp( MouseEvent event )
{
	// if we were dragging a circle, tell it we're done
	if( mCurrentDragCircle )
		mCurrentDragCircle->dragRelease();
		
	mCurrentDragCircle = 0;
}
void DragTweenApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
	gl::enableAlphaBlending();
	
	for( vector<Circle>::const_iterator circleIt = mCircles.begin(); circleIt != mCircles.end(); ++circleIt )
		circleIt->draw();
}


CINDER_APP( DragTweenApp, RendererGl )
