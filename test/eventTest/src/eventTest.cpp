#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class EventTestApp : public App {
 public:	
	void mouseDown( MouseEvent event );
	void mouseUp( MouseEvent event );
	void mouseWheel( MouseEvent event );
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	void keyUp( KeyEvent event );
	void fileDrop( FileDropEvent event );
	void draw();
};


string mouseModifiersToText( const MouseEvent &event )
{
	stringstream ss;

	if( event.isShiftDown() )
		ss << "[shift]";
	if( event.isControlDown() )
		ss << "[ctrl]";
	if( event.isMetaDown() )
		ss << "[meta]";
	if( event.isAltDown() )
		ss << "[alt]";
	if( event.isLeftDown() )
		ss << "[left mouse]";
	if( event.isRightDown() )
		ss << "[right mouse]";
	if( event.isMiddleDown() )
		ss << "[middle mouse]";

	return ss.str();
}

string keyModifiersToText( const KeyEvent &event )
{
	stringstream ss;

	if( event.isShiftDown() )
		ss << "[shift]";
	if( event.isControlDown() )
		ss << "[ctrl]";
	if( event.isMetaDown() )
		ss << "[meta]";
	if( event.isAltDown() )
		ss << "[alt]";

	return ss.str();
}

string mouseInitiatorToText( const MouseEvent &event )
{
	if( event.isLeft() )
		return string( "left" );
	else if( event.isRight() )
		return string( "right" );
	else if( event.isMiddle() )
		return string( "middle" );
	else
		return string( "none" );
}

void EventTestApp::mouseDown( MouseEvent event )
{
	console() << "You clicked the " << mouseInitiatorToText( event ) << " mouse button @ " << event.getPos() << "pt " << toPixels( event.getPos() ) << "px " << " " << mouseModifiersToText( event ) << endl;
}

void EventTestApp::mouseUp( MouseEvent event )
{
	console() << "You released the " << mouseInitiatorToText( event ) << " mouse button @ " << event.getPos() << "pt " << toPixels( event.getPos() ) << "px " << mouseModifiersToText( event ) << endl;
}

void EventTestApp::mouseWheel( MouseEvent event )
{
	console() << "You scrolled the mouse @ " << event.getPos() << "pt " << toPixels( event.getPos() ) << "px by " << event.getWheelIncrement() << " detents " << mouseModifiersToText( event ) << endl;
}

void EventTestApp::mouseMove( MouseEvent event )
{
	console() << "You moved the mouse @ " << event.getPos() << "pt " << toPixels( event.getPos() ) << "px " << mouseModifiersToText( event ) << endl;
}

void EventTestApp::mouseDrag( MouseEvent event )
{
	console() << "You dragged the mouse @ " << event.getPos() << "pt " << toPixels( event.getPos() ) << "px " << mouseModifiersToText( event ) << endl;
}

void EventTestApp::keyDown( KeyEvent event )
{
	stringstream ss;
	char ch = ( event.getChar() == 0 ) ? ' ' : event.getChar();
	ss << "You pressed the '" << ch << "' key with code: " << event.getCode() << keyModifiersToText( event );
	console() << ss.str() << endl;	
}

void EventTestApp::keyUp( KeyEvent event )
{
	stringstream ss;
	char ch = ( event.getChar() == 0 ) ? ' ' : event.getChar();
	ss << "You keyupped the '" << ch << "' key with keyCode: " << event.getCode() << keyModifiersToText( event );
	console() << ss.str() << endl;
}

void EventTestApp::fileDrop( FileDropEvent event )
{
	stringstream ss;
	ss << "You dropped files @ " << event.getPos() << "pt " << toPixels( event.getPos() ) << "px and the files were: " << endl;
	for( size_t s = 0; s < event.getNumFiles(); ++s )
		ss << event.getFile( s ) << endl;
	console() << ss.str() << endl;
}

void EventTestApp::draw()
{
	gl::clear();
}

CINDER_APP( EventTestApp, RendererGl, []( EventTestApp::Settings *settings ) {
	settings->setHighDensityDisplayEnabled( true );
} )