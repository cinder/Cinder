#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/PolyLine.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Path2d.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int cCircleRadius = 40.0f;
const int cGridStep = cCircleRadius * 2.2f;

/**
 ConvenienceDrawingMethodsApp:

 Demonstrates most of Cinder's convenience drawing methods.
 These are not the most CPU/GPU-efficient methods of drawing, but they
 are a quick way to get something on screen.
 */
class ConvenienceDrawingMethodsApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
private:
	Path2d			mPath;
	PolyLine<vec2>	mPolyline2D;
	PolyLine<vec3>	mPolyline3D;
};

void ConvenienceDrawingMethodsApp::setup()
{
	mPath.arc( vec2::zero(), cCircleRadius, 0.0f, M_PI * 1.66f );
	mPath.lineTo( vec2::zero() );

	Rand r;
	for( int i = 0; i < 50; ++i )
	{
		mPolyline2D.push_back( r.nextVec2f() * cCircleRadius );
	}

	// line wrapping around a sphere
	const int numVertices = 48;
	const float azimuth = 12.0f * M_PI / numVertices;
	// divide total inclination change by particles (from straight down to straight up)
	const float inclination = M_PI / numVertices;
	// fixed radius to generate a sphere
	for( int i = 0; i < numVertices; ++i )
	{	// give each particle its starting values
		float x = cCircleRadius * sin( inclination * i ) * cos( azimuth * i );
		float y = cCircleRadius * cos( inclination * i );
		float z = cCircleRadius * sin( inclination * i ) * sin( azimuth * i );
		mPolyline3D.push_back( vec3( x, y, z ) );
	}
}

void ConvenienceDrawingMethodsApp::mouseDown( MouseEvent event )
{
}

void ConvenienceDrawingMethodsApp::update()
{
}

void ConvenienceDrawingMethodsApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp( getWindowSize() );

	// Draw some rows of circles
	const int numCircles = 4;
	gl::color( Color( 1.0f, 1.0f, 0.0f ) );
	for( int i = 0; i < numCircles; ++i ) {
		gl::drawSolidCircle( vec2( (i + 1.0f) * cGridStep, cGridStep ), cCircleRadius, i * 3 );
	}

	gl::color( Color( 1.0f, 0.0f, 0.0f ) );
	for( int i = 0; i < numCircles; ++i ) {
		gl::drawStrokedCircle( vec2( (i + 1.0f) * cGridStep, cGridStep ), cCircleRadius, i * 3 );
	}

	// Draw a line in 2D
	gl::drawLine( vec2( 10.0f, cGridStep * 1.5f ), vec2( getWindowWidth() - 10.0f, cGridStep * 1.5f ) );

	// Draw a Path2d both stroked and filled
	gl::pushModelMatrix();
	gl::translate( cGridStep, cGridStep * 2.0f );
	gl::draw( mPath );
	gl::translate( cGridStep, 0.0f );
	gl::color( Color( 1.0f, 1.0f, 0.0f ) );
	gl::drawSolid( mPath );
	gl::popModelMatrix();

	// Draw a 2D PolyLine both stroked and filled
	gl::pushModelMatrix();
	gl::translate( cGridStep, cGridStep * 3.0f );
	gl::color( Color( 1.0f, 0.0f, 0.0f ) );
	gl::draw( mPolyline2D );
	gl::translate( cGridStep, 0.0f );
	gl::color( Color( 1.0f, 1.0f, 0.0f ) );
	gl::drawSolid( mPolyline2D );
	gl::popModelMatrix();

	// Draw a 3D PolyLine
	gl::pushModelMatrix();
	gl::translate( cGridStep, cGridStep * 4.0f );
	gl::pushModelMatrix();
	gl::rotate( getElapsedSeconds() * 45.0f, 0.0f, 1.0f, 0.0f );
	gl::color( Color( 1.0f, 0.0f, 0.0f ) );
	gl::draw( mPolyline3D );
	gl::popModelMatrix();
	// draw a 3D line back into space
	gl::translate( cGridStep, 0.0f );
	gl::pushModelMatrix();
	gl::rotate( -getElapsedSeconds() * 90.0f, 0.0f, 0.5f, 1.0f );
	gl::drawLine( vec3( 0.0f, 0.0f, cCircleRadius ), vec3( 0.0f, 0.0f, -cCircleRadius ) );
	gl::drawLine( vec3( -cCircleRadius, 0.0f, 0.0f ), vec3( cCircleRadius, 0.0f, 0.0f ) );
	gl::drawLine( vec3( 0.0f, -cCircleRadius, 0.0f ), vec3( 0.0f, cCircleRadius, 0.0f ) );
	gl::popModelMatrix();
	gl::popModelMatrix();
}

CINDER_APP_NATIVE( ConvenienceDrawingMethodsApp, RendererGl )
