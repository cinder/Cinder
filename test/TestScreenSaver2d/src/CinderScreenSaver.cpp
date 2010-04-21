#include "cinder/Cinder.h"
#include "cinder/app/AppScreenSaver.h"
#include "cinder/Color.h"

using namespace ci;
using namespace ci::app;

//#define USE_QUARTZ
//#define USE_CAIRO
//#define USE_GDI

#if defined( USE_QUARTZ ) || defined( USE_GDI ) || defined( USE_CAIRO )
typedef ci::app::Renderer2d RendererType;
#else
typedef ci::app::RendererGl RendererType;
#endif

#if defined( USE_CAIRO )
#include "cinder/cairo/Cairo.h"
#endif

class CinderScreenSaver2d : public AppScreenSaver {
 public:
	virtual void setup();
	virtual void update();
	virtual void draw();
	
 protected:
	ci::Color	mColor, mBackgroundColor;
	float		mRadius;
};


void CinderScreenSaver2d::setup()
{
	mColor = Color( 0.8f, 1.0f, 0.85f );
#if defined( USE_QUARTZ )
	mBackgroundColor = Color( 0.1f, 0.2f, 0.3f );
#elif defined( USE_CAIRO )
	mBackgroundColor = Color( 0.4f, 0.2f, 0.0f );
#elif defined( USE_GDI )
	mBackgroundColor = Color( 0.2f, 0.4f, 0.0f );
#else
	mBackgroundColor = Color( 0.3f, 0.2f, 0.1f );
#endif

	console() << "App path: " << getAppPath() << std::endl;
	console() << "Screen size: " << getWindowSize() << std::endl;
}

void CinderScreenSaver2d::update()
{
	mRadius = abs( cos( getElapsedSeconds() ) * 200 );
}

#if defined( USE_QUARTZ )
void CinderScreenSaver2d::draw()
{
	CGContextRef ctx( createWindowCgContext() );

	// Clear out to bkg color
	CGContextBeginPath( ctx );
	CGContextSetRGBFillColor( ctx, mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, 1.0f );	
	CGContextAddRect( ctx, CGRectMake( 0, 0, getWindowWidth(), getWindowHeight() ) );
	CGContextFillPath( ctx );

	CGContextBeginPath( ctx );
	CGContextSetRGBFillColor( ctx, mColor.r, mColor.g, mColor.b, 1.0f );
	CGContextAddEllipseInRect( ctx, CGRectMake( getWindowWidth()/2 - mRadius / 2, getWindowHeight()/2 - mRadius / 2, mRadius, mRadius ) );
	CGContextFillPath( ctx );
}
#elif defined( USE_CAIRO )
void CinderScreenSaver2d::draw()
{
	cairo::Context ctx( cairo::createWindowSurface() );
	
	ctx.setSource( mBackgroundColor );
	ctx.paint();
	
	ctx.setSource( mColor );
	ctx.newPath();
	ctx.arc( Vec2f( getWindowWidth(), getWindowHeight() ) / 2.0f, mRadius, 0, 2 * 3.14159f );
	ctx.fill();
}
#elif defined( USE_GDI )
void CinderScreenSaver2d::draw()
{
	HDC dc = getDC();
	HBRUSH brush = ::CreateSolidBrush( RGB( mBackgroundColor.r * 255, mBackgroundColor.g * 255, mBackgroundColor.b * 255 ) );
	::SelectObject( dc, brush );
	::Rectangle( dc, 0, 0, getWindowWidth(), getWindowHeight() );
	::DeleteObject( brush );

	brush = ::CreateSolidBrush( RGB( mColor.r * 255, mColor.g * 255, mColor.b * 255 ) );
	::SelectObject( dc, brush );
	int x = getWindowWidth() / 2;
	int y = getWindowHeight() / 2;
	::Ellipse( dc, x - mRadius, y - mRadius, x + mRadius, y + mRadius );
	::DeleteObject( brush );	
}
#else
void CinderScreenSaver2d::draw()
{
	gl::clear( mBackgroundColor );
	glColor3f( mColor );
	gl::drawSolidCircle( getWindowCenter(), mRadius );
	glColor3f( Color( 1, 0.5, 0.2f ) );
	gl::drawSolidCircle( Vec2f( 100, 100 ), 50 );
}
#endif

CINDER_APP_SCREENSAVER( CinderScreenSaver2d, RendererType )