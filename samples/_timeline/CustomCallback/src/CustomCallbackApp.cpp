// This sample demonstrates the use of custom callbacks for tween start, update, and completion
// * The start callback sets the background green using a functor
// * The completion callback sets the background to blue using a free function
// * The update callback makes the radius of the circle the distance to the nearest edge using a member function

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// global variable for background color
Color	gBackgroundColor;

struct Circle {
	void posUpdate() { // make the radius the distance to the closest edge
		mRadius = mPos().x;
		mRadius = std::min( mRadius, getWindowWidth() - mPos().x );
		mRadius = std::min( mRadius, mPos().y );
		mRadius = std::min( mRadius, getWindowHeight() - mPos().y );		
	}

	void draw() {
		gl::color( Color( 1.0f, 0.5f, 0.25f ) );
		gl::drawSolidCircle( mPos, mRadius );
	}
	
	Anim<vec2>	mPos;
	float		mRadius;
};

// Functor which sets the Color pointed to by colorPtr to green
struct ColorToGreenFunctor {
	ColorToGreenFunctor( Color *colorPtr )
		: mColorPtr( colorPtr )
	{}
	
	void operator()() {
		*mColorPtr = Color( 0.5f, 0.9f, 0.5f );
	}
	
	Color		*mColorPtr;
};

class CustomCallbackApp : public App {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void draw();
	
	Circle		mCircle;
};

// a free function which sets gBackgroundColor to blue
void setBackgroundToBlue()
{
	gBackgroundColor = Color( 0.4f, 0.4f, 0.9f );
}

void CustomCallbackApp::setup()
{
	setBackgroundToBlue();

	mCircle.mPos = vec2( 50, 50 );
	mCircle.mRadius = 50;
}

void CustomCallbackApp::mouseDown( MouseEvent event )
{
	timeline().apply( &mCircle.mPos, vec2( event.getPos() ), 2.0f, EaseInOutCubic() )
			.startFn( ColorToGreenFunctor( &gBackgroundColor ) )
			.updateFn( std::bind( &Circle::posUpdate, &mCircle ) )
			.finishFn( setBackgroundToBlue );
}

void CustomCallbackApp::draw()
{
	gl::clear( gBackgroundColor ); 	
	mCircle.draw();
}

CINDER_APP( CustomCallbackApp, RendererGl )
