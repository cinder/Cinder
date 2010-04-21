//#define USE_QUARTZ
//#define USE_CAIRO

#include "SimpleApp.h"
#include <CoreGraphics/CGContext.h>
#if defined( USE_CAIRO )
#include "flint/cairo/Cairo.h"
#endif

using namespace fli;
using namespace fli::app;

SimpleApp::SimpleApp()
	: AppFlintView()
{
	mSubdivision = 10;
	mColor = Color( 1.0f, 0.5f, 0.25f );
	mBackgroundColor = Color( 0.1f, 0.2f, 0.3f );
	mAnimation = 0.0f;
}

void SimpleApp::update()
{
	mAnimation += 0.01f;
}

#if defined( USE_QUARTZ )
void SimpleApp::draw()
{
	CGContextRef ctx = getCGContext();

	// fill with background color
	CGContextBeginPath( ctx );
	CGContextSetRGBFillColor( ctx, mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, 1.0f );	
	CGContextAddRect( ctx, CGRectMake( 0, 0, getWindowWidth(), getWindowHeight() ) );
	CGContextFillPath( ctx );
	
	// draw the circle
	float radius = abs( cos( mAnimation ) * 200 );
	CGContextBeginPath( ctx );
	CGContextSetRGBFillColor( ctx, mColor.r, mColor.g, mColor.b, 1.0f );
	CGContextAddEllipseInRect( ctx, CGRectMake( getWindowWidth() / 2 - radius / 2, getWindowHeight() / 2 - radius / 2, radius, radius ) );
	CGContextFillPath( ctx );
}
#elif defined( USE_CAIRO )
void SimpleApp::draw()
{
	fli::cairo::Context ctx( getCairoSurface() );

	// fill with background color
	ctx.setSourceColor( mBackgroundColor );
	ctx.paint();
	
	// draw the circle
	float radius = abs( cos( mAnimation ) * 200 );
	ctx.setSourceColor( mColor );
	ctx.newPath();
	ctx.arc( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f, radius, 0.0f, 2 * M_PI );
	ctx.fill();
}
#else
void SimpleApp::draw()
{
	glClearColor( mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	

	float radius = abs( cos( mAnimation ) * 200 );
	
	glColor3fv( mColor );
	glBegin( GL_POLYGON );
	for( int s = 0; s < mSubdivision; ++s ) {
		glVertex3f( getWindowWidth() / 2 + radius * cos( s / (float)mSubdivision * 6.28f ), getWindowHeight() / 2 + radius * sin( s / (float)mSubdivision * 6.28f ), 0 );
	}
	glEnd();
}
#endif // defined( USE_OPENGL )

void SimpleApp::setColor( const Color &aColor )
{
	mColor = aColor;
}

void SimpleApp::setSubdivision( int aSubdivision )
{
	mSubdivision = aSubdivision;
}

void SimpleApp::keyDown( fli::app::KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
	quit();
}