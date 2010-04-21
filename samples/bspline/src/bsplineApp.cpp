#include "cinder/app/AppBasic.h"
#include "cinder/BSpline.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

#include <vector>

using namespace ci;
using namespace ci::app;
using std::vector;

class bsplineApp : public AppBasic {
 public:
	bsplineApp() : mTrackedPoint( -1 ), mDegree( 3 ), mOpen( true ), mLoop( false ) {}
	
	int		findNearestPt( const Vec2f &aPt );
	void	calcLength();
	
	void	mouseDown( MouseEvent event );
	void	mouseUp( MouseEvent event );
	void	mouseDrag( MouseEvent event );
	void	keyDown( KeyEvent event );

	void	drawBSpline( cairo::Context &ctx );
	void	draw();

	vector<Vec2f>		mPoints;
	int					mTrackedPoint;
	int					mDegree;
	bool				mOpen, mLoop;
};

void bsplineApp::mouseDown( MouseEvent event )
{
	const float MIN_CLICK_DISTANCE = 6.0f;
	if( event.isLeft() ) { // line
		Vec2f clickPt = Vec2f( event.getPos() );
		int nearestIdx = findNearestPt( clickPt );
		if( ( nearestIdx < 0 ) || ( mPoints[nearestIdx].distance( clickPt ) > MIN_CLICK_DISTANCE ) ) {
			mPoints.push_back( Vec2f( event.getPos() ) );
			mTrackedPoint = -1;
		}
		else
			mTrackedPoint = nearestIdx;
		calcLength();
	}
}

void bsplineApp::mouseDrag( MouseEvent event )
{
	if( mTrackedPoint >= 0 ) {
		mPoints[mTrackedPoint] = Vec2f( event.getPos() );
		calcLength();
	}
}

void bsplineApp::mouseUp( MouseEvent event )
{
	mTrackedPoint = -1;
}

void bsplineApp::keyDown( KeyEvent event ) {
	if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
		setFullScreen( false );
	}
	else if( event.getChar() == 'x' ) {
		mPoints.clear();
	}
	else if( event.getChar() == 'd' ) { // reduce the degree
		mDegree = ( mDegree > 1 ) ? mDegree - 1 : mDegree;
		calcLength();
	}
	else if( event.getChar() == 'D' ) { // increase the degree
		mDegree++;
		calcLength();
	}
	else if( event.getChar() == 'o' ) { // toggle between open/periodic
		mOpen = ! mOpen;
		calcLength();
	}
	else if( event.getChar() == 'l' ) { // toggle between looping
		mLoop = ! mLoop;
		calcLength();
	}
	else if( event.getChar() == 'i' ) { // export to png image
		writeImage( getHomeDirectory() + "bsplineOutput.png", copyWindowSurface() );
	}
	else if( event.getChar() == 's' ) { // export to svg
		cairo::Context ctx( cairo::SurfaceSvg( getHomeDirectory() + "output.svg", getWindowWidth(), getWindowHeight() ) );
		drawBSpline( ctx );
	}
	else if( event.getChar() == 'e' ) { // export to eps
		cairo::Context ctx( cairo::SurfaceEps( getHomeDirectory() + "output.eps", getWindowWidth(), getWindowHeight() ) );
		drawBSpline( ctx );
	}
	else if( event.getChar() == 'f' ) { // export to pdf
		cairo::Context ctx( cairo::SurfacePdf( getHomeDirectory() + "output.pdf", getWindowWidth(), getWindowHeight() ) );
		drawBSpline( ctx );
	}
	else if( event.getChar() == 'p' ) { // export to postscript
		cairo::Context ctx( cairo::SurfacePs( getHomeDirectory() + "output.ps", getWindowWidth(), getWindowHeight() ) );
		drawBSpline( ctx );
	}
}

int bsplineApp::findNearestPt( const Vec2f &aPt )
{
	if( mPoints.empty() ) return -1;
	
	int result = 0;
	float nearestDist = mPoints[0].distance( aPt );
	for( size_t i = 1; i < mPoints.size(); ++i ) {
		if( mPoints[i].distance( aPt ) < nearestDist ) {
			result = i;
			nearestDist = mPoints[i].distance( aPt );
		}
	}
	
	return result;
}

void bsplineApp::calcLength()
{
	if( mPoints.size() > (size_t)mDegree ) {
		BSpline2f spline( mPoints, mDegree, mLoop, mOpen );
		console() << "Arc Length: " << spline.getLength( 0, 1 ) << std::endl;
	}
}

void bsplineApp::drawBSpline( cairo::Context &ctx )
{
	if( mPoints.size() > (size_t)mDegree ) {
		ctx.setLineWidth( 2.5f );
		ctx.setSourceRgb( 1.0f, 0.5f, 0.25f );
		ctx.appendPath( Path2d( BSpline2f( mPoints, mDegree, mLoop, mOpen ) ) );
		ctx.stroke();
//		ctx.fill();
	}
}

void bsplineApp::draw()
{
	// clear to the background color
	cairo::Context ctx( cairo::createWindowSurface() );
	ctx.setSourceRgb( 0.0f, 0.1f, 0.2f );
	ctx.paint();
	
	// draw the control points
	ctx.setSourceRgb( 1.0f, 1.0f, 0.0f );
	for( size_t p = 0; p < mPoints.size(); ++p ) {
		ctx.newSubPath();
		ctx.arc( mPoints[p], 2.5f, 0, 2 * 3.14159 );
	}
	ctx.stroke();

	if( mPoints.size() > (size_t)mDegree ) {
		// draw the curve by approximating via linear subdivision
		BSpline2f spline( mPoints, mDegree, mLoop, mOpen );
		ctx.setLineWidth( 8.0f );
		ctx.setSourceRgb( 0.25f, 1.0f, 0.5f );
		ctx.moveTo( spline.getPosition( 0 ) );
		for( float t = 0; t < 1.0f; t += 0.001f ) {
			ctx.lineTo( spline.getPosition( t ) );
		}
		
		ctx.stroke();
		
		// draw points 1/4, 1/2 and 3/4 along the length
		ctx.setSourceRgb( 0.0f, 0.7f, 1.0f );
		float totalLength = spline.getLength( 0, 1 );
		for( float p = 0.25f; p < 0.99f; p += 0.25f ) {
			ctx.newSubPath();
			ctx.arc( spline.getPosition( spline.getTime( p * totalLength ) ), 2.5f, 0, 2 * 3.14159f );
		}		
		
		ctx.stroke();
	}

	// draw the curve by bezier path
	drawBSpline( ctx );	
}

CINDER_APP_BASIC( bsplineApp, Renderer2d )