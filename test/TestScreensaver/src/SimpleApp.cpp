#if ! defined( STANDALONE_BUILD )
	#include "cinder/app/AppScreenSaver.h"
	typedef ci::app::AppScreenSaver AppType;
#else
#	include "cinder/app/AppBasic.h"
typedef ci::app::AppBasic AppType;
#endif
#include "cinder/Color.h"

#if defined( USE_QUARTZ ) || defined( USE_GDI ) || defined( USE_CAIRO )
	#define RENDERER Renderer2d
#else
	#define RENDERER RendererGl
#endif


using namespace ci;
using namespace ci::app;
static float anim = 0.0f;

//#define USE_QUARTZ
#define USE_CAIRO
//#define USE_GDI

#if defined( USE_CAIRO )
#include "cinder/cairo/Cairo.h"
#endif

class SimpleApp : public AppType {
 public:
	virtual Renderer*	prepareRenderer();
	virtual void prepareSettings( Settings *settings );
	
	virtual void setup();
	virtual void update();
	virtual void draw();
	
 protected:
	ci::Color	mColor, mBackgroundColor;
	int			mSubdivision;
	float		mRadius;
};


void SimpleApp::setup()
{
	mSubdivision = 50;
	mColor = Color( 0.1f, 1.0f, 0.85f );
#if defined( USE_QUARTZ )
	mBackgroundColor = Color( 0.1f, 0.2f, 0.3f );
#elif defined( USE_CAIRO )
	mBackgroundColor = Color( 0.4f, 0.2f, 0.0f );
#elif defined( USE_GDI )
	mBackgroundColor = Color( 0.2f, 0.4f, 0.0f );
#else
	mBackgroundColor = Color( 0.3f, 0.2f, 0.1f );
#endif
}

Renderer* SimpleApp::prepareRenderer()
{
}

void SimpleApp::prepareSettings( Settings *settings )
{
}

void SimpleApp::update()
{
	anim += 0.01f;
	mRadius = abs( cos( anim ) * 200 );
}

#if defined( USE_QUARTZ )
void SimpleApp::draw()
{
	CGContextRef ctx( getCGContext() );

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
void SimpleApp::draw()
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
void SimpleApp::draw()
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
void SimpleApp::draw()
{
	glClearColor( mBackgroundColor.r, mBackgroundColor.g, 1.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	

	glColor3fv( mColor );
	glBegin( GL_POLYGON );
	for( int s = 0; s < mSubdivision; ++s ) {
		glVertex3f( getWindowWidth() / 2 + mRadius * cos( s / (float)mSubdivision * 6.28f ), getWindowHeight() / 2 + mRadius * sin( s / (float)mSubdivision * 6.28f ), 0 );
	}
	glEnd();
}
#endif

#if ! defined( STANDALONE_BUILD )
CINDER_APP_SCREENSAVER( SimpleApp, RENDERER )
#else
CINDER_APP_BASIC( SimpleApp, RENDERER )
#endif
