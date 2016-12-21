#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

//! \class InputApp 
//!
//!
class InputApp : public App {
  public:
	void keyDown( KeyEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseMove( MouseEvent event ) override;
	void draw() override;
};

void InputApp::keyDown( KeyEvent event )
{
	if( event.getChar() > 0 ) { 
		std::cout << "keyDown: " << event.getCode() << " : " << event.getChar();
		std::cout << " " << "SHIFT:" << ( event.isShiftDown()   ? "1" : "0" );
		std::cout << " " <<  "CTRL:" << ( event.isControlDown() ? "1" : "0" );
		std::cout << " " <<   "ALT:" << ( event.isAltDown()     ? "1" : "0" );
		std::cout << " " <<  "META:" << ( event.isMetaDown()    ? "1" : "0" ) << std::endl;
	}
}

void InputApp::draw()
{
	gl::clear( Color::gray( 0.1f ) );
}
void InputApp::mouseDown( MouseEvent event )
{
	std::cout << "mouseDown: " << "(" << event.getPos().x << ", " << event.getPos().y << ")";
	std::cout << " " << "SHIFT:" << ( event.isShiftDown()   ? "1" : "0" );
	std::cout << " " <<  "CTRL:" << ( event.isControlDown() ? "1" : "0" );
	std::cout << " " <<   "ALT:" << ( event.isAltDown()     ? "1" : "0" );
	std::cout << " " <<  "META:" << ( event.isMetaDown()    ? "1" : "0" ) << std::endl;
}

void InputApp::mouseUp( MouseEvent event )
{
	std::cout << "mouseUp: " << "(" << event.getPos().x << ", " << event.getPos().y << ")";
	std::cout << " " << "SHIFT:" << ( event.isShiftDown()   ? "1" : "0" );
	std::cout << " " <<  "CTRL:" << ( event.isControlDown() ? "1" : "0" );
	std::cout << " " <<   "ALT:" << ( event.isAltDown()     ? "1" : "0" );
	std::cout << " " <<  "META:" << ( event.isMetaDown()    ? "1" : "0" ) << std::endl;
}

void InputApp::mouseDrag( MouseEvent event )
{
	std::cout << "mouseDrag: " << "(" << event.getPos().x << ", " << event.getPos().y << ")";
	std::cout << " " << "SHIFT:" << ( event.isShiftDown()   ? "1" : "0" );
	std::cout << " " <<  "CTRL:" << ( event.isControlDown() ? "1" : "0" );
	std::cout << " " <<   "ALT:" << ( event.isAltDown()     ? "1" : "0" );
	std::cout << " " <<  "META:" << ( event.isMetaDown()    ? "1" : "0" ) << std::endl;
}

void InputApp::mouseMove( MouseEvent event )
{
	std::cout << "mouseMove: " << "(" << event.getPos().x << ", " << event.getPos().y << ")";
	std::cout << " " << "SHIFT:" << ( event.isShiftDown()   ? "1" : "0" );
	std::cout << " " <<  "CTRL:" << ( event.isControlDown() ? "1" : "0" );
	std::cout << " " <<   "ALT:" << ( event.isAltDown()     ? "1" : "0" );
	std::cout << " " <<  "META:" << ( event.isMetaDown()    ? "1" : "0" ) << std::endl;
}

CINDER_APP( InputApp, RendererGl )
