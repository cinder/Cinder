#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Font.h"
#include "cinder/Timer.h"

#include <boost/format.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

class TimerResumeTestApp : public AppBasic {
public:
	void setup();
	void update();
	void draw();

	void mouseDown( MouseEvent event );	
private:
	Font	mFont;
	Timer	mTimer;
};

void TimerResumeTestApp::setup()
{
	mFont = Font("Verdana", 36.0f);
	mTimer.start();
}

void TimerResumeTestApp::update()
{
}

void TimerResumeTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 

	// draw elapsed time
	static const std::string formatter = "Elapsed time: %02.2f seconds";
	std::string result = ( boost::format(formatter) % mTimer.getSeconds() ).str();

	gl::enableAlphaBlending();
	gl::drawString( result, Vec2f(10.0f, 10.0f), Color::white(), mFont );
	gl::drawString( "Left mouse button will stop() / resume()", Vec2f(10.0f, 60.0f), Color::white(), mFont );
	gl::drawString( "Middle mouse button will stop() / start(15)", Vec2f(10.0f, 100.0f), Color::white(), mFont );
	gl::drawString( "Right mouse button will stop() / start()", Vec2f(10.0f, 140.0f), Color::white(), mFont );
	gl::disableAlphaBlending();
}

void TimerResumeTestApp::mouseDown( MouseEvent event )
{
	if( mTimer.isStopped() )
	{
		if( event.isLeftDown() ) 
			mTimer.resume();
		else if( event.isMiddleDown() )
			mTimer.start(15.0);
		else 
			mTimer.start();
	}
	else
		mTimer.stop();
}

CINDER_APP_BASIC( TimerResumeTestApp, RendererGl )
