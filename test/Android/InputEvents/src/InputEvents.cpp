#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the App class
class InputEvents : public App {
  public:
	void setup();

	void mouseDown( MouseEvent event );
	void mouseUp( MouseEvent event );
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );

    void touchesBegan( TouchEvent event );
    void touchesMoved( TouchEvent event );
    void touchesEnded( TouchEvent event );

	void draw();

private:
	bool 	mUseGetMousePos = false;
	ivec2 	mMousePos;
};

void prepareSettings( InputEvents::Settings *settings )
{
	settings->setMultiTouchEnabled();
}

void InputEvents::setup()
{
	mUseGetMousePos = true;
}

void InputEvents::mouseDown( MouseEvent event )
{
	console() << "mouseDown event" << std::endl;
	mMousePos = event.getPos();
}

void InputEvents::mouseUp( MouseEvent event )
{
	console() << "mouseUp event" << std::endl;
}

void InputEvents::mouseMove( MouseEvent event )
{
	console() << "mouseMove event" << std::endl;
}

void InputEvents::mouseDrag( MouseEvent event )
{
	console() << "mouseDrag event" << std::endl;
	mMousePos = event.getPos();
}

void InputEvents::touchesBegan( TouchEvent event )
{
	console() << "touchesBegan" << std::endl;

	/*
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mPoints.push_back( touchIt->getPos() );
	}
	*/
}

void InputEvents::touchesMoved( TouchEvent event )
{
	console() << "touchesMoved" << std::endl;
}

void InputEvents::touchesEnded( TouchEvent event )
{
	console() << "touchesEnded" << std::endl;
}

void InputEvents::draw()
{
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );

	if( mUseGetMousePos ) {
		gl::color( 0.0f, 1.0f, 0.0f );
		mMousePos = getMousePos();
	}
	else {
		gl::color( 1.0f, 1.0f, 0.0f );
	}
	gl::drawStrokedCircle( mMousePos, 100.0f );
}

// This line tells Cinder to actually create the application
CINDER_APP( InputEvents, RendererGl, prepareSettings )
