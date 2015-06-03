#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"

using namespace std;
using namespace ci;
using namespace ci::app;

class BasicTweenApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void draw() override;
  
	Anim<vec2>		mBlackPos, mWhitePos;
};

void BasicTweenApp::setup()
{	
	mBlackPos = mWhitePos = getWindowCenter();
}

void BasicTweenApp::mouseDown( MouseEvent event )
{
	// the call to apply() replaces any existing tweens on mBlackPos with this new one
	timeline().apply( &mBlackPos, (vec2)event.getPos(), 1.5f, EaseInCubic() );
	// the call to appendTo causes the white circle to start when the black one finishes
	timeline().apply( &mWhitePos, (vec2)event.getPos(), 0.35f, EaseOutQuint() ).appendTo( &mBlackPos );
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
CINDER_APP( BasicTweenApp, RendererGl( RendererGl::Options().msaa( 4 ) ) )
