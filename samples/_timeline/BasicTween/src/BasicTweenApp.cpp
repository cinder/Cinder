#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"

using namespace std;
using namespace ci;
using namespace ci::app;

class BasicTweenApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );
	void draw();
  
	Anim<Vec2f>		mBlackPos, mWhitePos;
};

void BasicTweenApp::setup()
{	
	mBlackPos = mWhitePos = getWindowCenter();
}

void BasicTweenApp::mouseDown( MouseEvent event )
{
	// the call to apply() replaces any existing tweens on mBlackPos with this new one
	timeline().apply( &mBlackPos, (Vec2f)event.getPos(), 2.0f, EaseInCubic() );
	// the call to appendTo causes the white circle to start when the black one finishes
	timeline().apply( &mWhitePos, (Vec2f)event.getPos(), 0.35f, EaseOutQuint() ).appendTo( &mBlackPos );
}

void BasicTweenApp::draw()
{
	gl::clear( Color( 0.5f, 0.5f, 0.5f ) );
	
	gl::color( Color::black() );
	gl::drawSolidCircle( mBlackPos, 20.0f );
	
	gl::color( Color::white() );
	gl::drawSolidCircle( mWhitePos, 16.0f );
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicTweenApp, RendererGl )
