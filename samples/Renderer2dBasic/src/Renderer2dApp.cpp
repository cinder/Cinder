#include "cinder/app/App.h"
#include "cinder/ImageIo.h"
#if defined( CINDER_COCOA )
	#include "cinder/cocoa/CinderCocoa.h"
	#include <CoreGraphics/CoreGraphics.h>
#elif defined( CINDER_MSW )
	#include "cinder/msw/CinderMswGdiplus.h"
#endif

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Renderer2dApp : public App {
  public:
	void setup() override;
	void draw() override;
	
#if defined( CINDER_COCOA )
	CGImageRef	mImage;
#elif defined( CINDER_MSW )
	Gdiplus::Bitmap *mImage;
	Surface8u		mImageSurface;
#endif
};

void Renderer2dApp::setup()
{
#if defined( CINDER_COCOA )
	mImage = cocoa::createCgImage( loadImage( loadResource( RES_CINDER_LOGO ) ) );
#elif defined( CINDER_MSW )
	mImageSurface = Surface( loadImage( loadResource( RES_CINDER_LOGO ) ), SurfaceConstraintsGdiPlus() );
	// because this only wraps the Surface, we need to keep it around
	mImage = msw::createGdiplusBitmap( mImageSurface );
#endif
}

void Renderer2dApp::draw()
{
	// Render using CoreGraphics on the mac; origin is lower left
#if defined( CINDER_COCOA )
	CGContextRef context = cocoa::getWindowContext();
	CGColorSpaceRef baseSpace = CGColorSpaceCreateDeviceRGB();
	CGFloat colors[8] = { 0, 0, 0, 1, 0.866, 0.866, 0.866, 1 };
	CGGradientRef gradient = CGGradientCreateWithColorComponents( baseSpace, colors, NULL, 2 );
	::CGColorSpaceRelease( baseSpace ), baseSpace = NULL;
	::CGContextDrawLinearGradient( context, gradient, CGPointMake( 0, getWindowHeight() ), CGPointMake( 0, 0 ), 0 );
	::CGGradientRelease(gradient), gradient = NULL;

	ivec2 imgSize( ::CGImageGetWidth( mImage ), ::CGImageGetHeight( mImage ) );
	ivec2 centerMargin( ( getWindowWidth() - imgSize.x ) / 2, ( getWindowHeight() - imgSize.y ) / 2 );
	::CGContextTranslateCTM( context, centerMargin.x, centerMargin.y );
	::CGContextDrawImage( context, CGRectMake( 0, 0, imgSize.x, imgSize.y ), mImage );
#elif defined( CINDER_MSW ) // Render using GDI+ on Windows; origin is upper left
	Gdiplus::Graphics graphics( getWindow()->getDc() );
	Gdiplus::LinearGradientBrush brush( Gdiplus::Rect( 0, 0, getWindowWidth(), getWindowHeight() ),
		Gdiplus::Color( 0, 0, 0 ), Gdiplus::Color( 220, 220, 220 ), Gdiplus::LinearGradientModeVertical );
	graphics.FillRectangle( &brush, 0, 0, getWindowWidth(), getWindowHeight() ); 
	graphics.DrawImage( mImage, ( getWindowWidth() - mImageSurface.getWidth() ) / 2, ( getWindowHeight() - mImageSurface.getHeight() ) / 2,
		mImageSurface.getWidth(), mImageSurface.getHeight() );
#endif
}

CINDER_APP( Renderer2dApp, Renderer2d )
