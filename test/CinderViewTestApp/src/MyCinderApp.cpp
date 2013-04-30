#include "MyCinderApp.h"
#if defined( USE_RENDERER2D )
	#include "cinder/cocoa/CinderCocoa.h"
#else
	#include "cinder/gl/gl.h"
#endif

using namespace ci;
using namespace ci::app;

void MyCinderApp::prepareSettings( Settings *settings )
{
	testCbo.setState( TestCallbackOrder::PREPARESETTINGS );
	settings->enableMultiTouch( false );
}

void MyCinderApp::setup()
{
	testCbo.setState( TestCallbackOrder::SETUP );
	mRadius = 50;
	mAnimatedRadius = 0;
	mColor = cinder::Color::white();
	app::WindowRef win = getWindow();
	console() << "NSView: " << win->getNative() << std::endl;
	win->setTitle( "Behold." );
	console() << "Display: " << *(win->getDisplay()) << std::endl;
}

void MyCinderApp::resize()
{
	testCbo.setState( TestCallbackOrder::RESIZE );
}

void MyCinderApp::update()
{
	testCbo.setState( TestCallbackOrder::UPDATE );
	mAnimatedRadius = sin( getElapsedFrames() / 60.0f ) * getWindowHeight() / 2;
}

void MyCinderApp::draw()
{
	testCbo.setState( TestCallbackOrder::DRAW );

#if defined( USE_RENDERER2D )
	CGContextRef ctx = cocoa::getWindowContext();
	::CGContextSetRGBFillColor( ctx, 0, 0, 0, 1 );
	::CGContextFillRect( ctx, CGRectMake( 0, 0, getWindowWidth(), getWindowHeight() ) );

	::CGContextSetRGBFillColor( ctx, 1, 0.5, 0.25, 1 );
	::CGContextFillEllipseInRect( ctx, CGRectMake( getWindowCenter().x - mAnimatedRadius, getWindowCenter().y - mAnimatedRadius,
			mAnimatedRadius * 2, mAnimatedRadius * 2 ) );
	::CGContextSetRGBFillColor( ctx, mColor.r, mColor.g, mColor.b, 0.5f );
	::CGContextFillEllipseInRect( ctx, CGRectMake( getWindowCenter().x - mRadius, getWindowCenter().y - mRadius,
			mRadius * 2, mRadius * 2 ) );
#else
	gl::clear();	
	gl::color( Color( 1, 0.5f, 0.25f ) );
	gl::drawSolidCircle( getWindowCenter(), mAnimatedRadius );
	gl::color( mColor );
	gl::drawSolidCircle( getWindowCenter(), mRadius );
#endif
}

void MyCinderApp::shutdown()
{
	console() << "Shutdown" << std::endl;
	testCbo.setState( TestCallbackOrder::SHUTDOWN );
}

void MyCinderApp::mouseDown( MouseEvent event )
{
	console() << "Down: " << event.getPos() << std::endl;
}

void MyCinderApp::mouseUp( MouseEvent event )
{
	console() << "Up:" << event.getPos() << std::endl;
}

void MyCinderApp::mouseDrag( MouseEvent event )
{
	console() << "Drag:" << event.getPos() << std::endl;	
}

void MyCinderApp::mouseMove( MouseEvent event )
{
	console() << "Move:" << event.getPos() << std::endl;	
}

void MyCinderApp::touchesMoved( TouchEvent event )
{
	console() << "Touches Moved:" << event << std::endl;	
}

void MyCinderApp::keyDown( KeyEvent event )
{
	console() << "Key down: " << event.getChar();
	console() << "  shift: " << event.isShiftDown() << std::endl;
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void MyCinderApp::fileDrop( FileDropEvent event )
{
	console() << "File Drop:" << event.getPos() << std::endl;	
}
