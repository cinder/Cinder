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
	mFont = Font("Arial", 48.0f);
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
	static const std::string formatter = "Elapsed time: %02.2f seconds\nLeft mouse button will stop/resume\nRight mouse button will stop/start";
	std::string result = ( boost::format(formatter) % mTimer.getSeconds() ).str();

	gl::enableAlphaBlending();
	gl::drawString( result, Vec2f(10.0f, 10.0f), Color::white(), mFont );
	gl::disableAlphaBlending();
}

void TimerResumeTestApp::mouseDown( MouseEvent event )
{
	if( event.isLeftDown() ) {
		if(mTimer.isStopped())
			mTimer.resume();
		else
			mTimer.stop();
	}
	else {
		if(mTimer.isStopped())
			mTimer.start();
		else
			mTimer.stop();
	}
}

CINDER_APP_BASIC( TimerResumeTestApp, RendererGl )
