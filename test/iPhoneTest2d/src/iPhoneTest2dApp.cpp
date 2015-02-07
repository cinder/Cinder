#include "cinder/app/App.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"

#include <CoreGraphics/CoreGraphics.h>

using namespace ci;
using namespace ci::app;

class TestApp : public App {
  public:
	void	setup() override;
	void	mouseDown( MouseEvent event ) override;
	void	update() override;
	void	draw() override;

	float			anim, mRadius;
};

void TestApp::setup()
{
	anim = 0;
	console() << "Launched app at " << getAppPath() << std::endl;
	console() << "Launched width: " << getWindowWidth() << std::endl;
}

void TestApp::mouseDown( MouseEvent event )
{
	std::cout << "Mouse down @ " << event.getPos() << std::endl;
	writeImage( getDocumentsDirectory() / "whatever.jpg", copyWindowSurface() );
}

void TestApp::update()
{
	anim += 0.01f;
	mRadius = abs( cos( anim ) * 200 );
}

void TestApp::draw()
{
	CGContextRef ctx( createWindowCgContext() );

//console() << "Run size: " << getWindowSize() << std::endl;

	// Clear out to bkg color
	CGContextBeginPath( ctx );
	CGContextSetRGBFillColor( ctx, 0.1f, 0.2f, 0.6f, 1 );
	CGContextAddRect( ctx, CGRectMake( 0, 0, getWindowWidth(), getWindowHeight() ) );
	CGContextFillPath( ctx );

	CGContextBeginPath( ctx );
	CGContextSetRGBFillColor( ctx, 1, 0.5f, 0.2f, 1 );
	CGContextAddEllipseInRect( ctx, CGRectMake( getWindowWidth()/2 - mRadius / 2, getWindowHeight() / 4 - mRadius / 2, mRadius, mRadius ) );
	CGContextFillPath( ctx );
}

CINDER_APP( TestApp, Renderer2d )
