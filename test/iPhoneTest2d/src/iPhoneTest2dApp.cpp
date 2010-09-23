#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"

#include <CoreGraphics/CoreGraphics.h>

using namespace ci;
using namespace app;

class TestApp : public ci::app::AppCocoaTouch {
  public:
	virtual void	setup();
	virtual void	resize( int width, int height );
	virtual void	mouseDown( MouseEvent event );
	virtual void	update();
	virtual void	draw();

	float			anim, mRadius;
};

void TestApp::setup()
{
	anim = 0;
	console() << "Launched app at " << getAppPath() << std::endl;
	console() << "Launched width: " << getWindowWidth() << std::endl;
}

void TestApp::resize( int width, int height )
{
}

void TestApp::mouseDown( MouseEvent event )
{
	std::cout << "Mouse down @ " << event.getPos() << std::endl;
	writeImage( getDocumentsDirectory() + "whatever.jpg", copyWindowSurface() );
}

void TestApp::update()
{
	anim += 0.01f;
	mRadius = abs( cos( anim ) * 200 );
}

void TestApp::draw()
{
	CGContextRef ctx( createWindowCgContext() );
console() << "Run size: " << getWindowSize() << std::endl;
	// Clear out to bkg color
	CGContextBeginPath( ctx );
	CGContextSetRGBFillColor( ctx, 0.1, 0.2, 0.6, 1.0f );	
	CGContextAddRect( ctx, CGRectMake( 0, 0, getWindowWidth(), getWindowHeight() ) );
	CGContextFillPath( ctx );

	CGContextBeginPath( ctx );
	CGContextSetRGBFillColor( ctx, 1.0, 0.5f, 0.2f, 1.0f );
	CGContextAddEllipseInRect( ctx, CGRectMake( getWindowWidth()/2 - mRadius / 2, getWindowHeight()/4 - mRadius / 2, mRadius, mRadius ) );
	CGContextFillPath( ctx );
}

CINDER_APP_COCOA_TOUCH( TestApp, Renderer2d )