#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/PolyLine.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Path2d.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const float cCircleRadius = 40;
const float cGridStep = cCircleRadius * 2.2f;

/**
 ConvenienceDrawingMethodsApp:

 Demonstrates most of Cinder's convenience drawing methods.
 These are not the most CPU/GPU-efficient methods of drawing, but they
 are a quick way to get something on screen.
 */
class ConvenienceDrawingMethodsApp : public App {
  public:
	void prepareSettings( Settings *settings ) override;
	void setup() override;
	void update() override;
	void draw() override;
  private:
	void drawBasicOverview();
	void drawSourcesStressTest();

	Path2d		mPath;
	PolyLine2	mPolyline2D;
	PolyLine3	mPolyline3D;
};

void ConvenienceDrawingMethodsApp::prepareSettings( Settings *settings )
{
	settings->setWindowPos( 0, 0 );
	settings->setWindowSize( 960, 564 );

}

void ConvenienceDrawingMethodsApp::setup()
{

	mPath.arc( vec2( 0 ), cCircleRadius, 0, M_PI * 1.66f );
	mPath.lineTo( vec2( 0 ) );

	Rand r;
	for( int i = 0; i < 50; ++i )
	{
		mPolyline2D.push_back( r.nextVec2f() * cCircleRadius );
	}

	// line wrapping around a sphere
	const int numVertices = 48;
	const float azimuth = 12 * M_PI / numVertices;
	// divide total inclination change by particles (from straight down to straight up)
	const float inclination = M_PI / numVertices;
	// fixed radius to generate a sphere
	for( int i = 0; i < numVertices; ++i ) {	// give each particle its starting values
		float x = cCircleRadius * sin( inclination * i ) * cos( azimuth * i );
		float y = cCircleRadius * cos( inclination * i );
		float z = cCircleRadius * sin( inclination * i ) * sin( azimuth * i );
		mPolyline3D.push_back( vec3( x, y, z ) );
	}
}

void ConvenienceDrawingMethodsApp::update()
{
	if( getElapsedFrames() % 30 == 0 )
		getWindow()->setTitle( "fps: " + to_string( getAverageFps() ) );
}

void ConvenienceDrawingMethodsApp::draw()
{
	drawBasicOverview();
	drawSourcesStressTest();
}

void ConvenienceDrawingMethodsApp::drawBasicOverview()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp( getWindowSize() );

	// Draw some rows of circles
	const int numCircles = 4;
	gl::color( Color( 1, 1, 0 ) );
	for( int i = 0; i < numCircles; ++i ) {
		gl::drawSolidCircle( vec2( (i + 1) * cGridStep, cGridStep ), cCircleRadius, i * 3 );
	}

	gl::color( Color( 1, 0, 0 ) );
	for( int i = 0; i < numCircles; ++i ) {
		gl::drawStrokedCircle( vec2( (i + 1) * cGridStep, cGridStep ), cCircleRadius, i * 3 );
	}

	// Draw a line in 2D
	gl::drawLine( vec2( 10, cGridStep * 1.5f ), vec2( getWindowWidth() - 10, cGridStep * 1.5f ) );

	// Draw a Path2d both stroked and filled
	{
		gl::ScopedModelMatrix modelScope;
		gl::translate( cGridStep, cGridStep * 2 );
		gl::draw( mPath );
		gl::translate( cGridStep, 0 );
		gl::color( Color( 1, 1, 0 ) );
		gl::drawSolid( mPath );
	}

	// Draw a 2D PolyLine both stroked and filled
	{
		gl::ScopedModelMatrix modelScope;
		gl::translate( cGridStep, cGridStep * 3 );
		gl::color( Color( 1, 0, 0 ) );
		gl::draw( mPolyline2D );
		gl::translate( cGridStep, 0 );
		gl::color( Color( 1, 1, 0 ) );
		gl::drawSolid( mPolyline2D );
	}

	// Draw a 3D PolyLine
	{
		gl::ScopedModelMatrix modelScope;
		gl::translate( cGridStep, cGridStep * 4 );
		gl::rotate( getElapsedSeconds(), 0, 1, 0 );
		gl::color( Color( 1, 0, 0 ) );
		gl::draw( mPolyline3D );
	}

	// draw a 3D line back into space
	{
		gl::ScopedModelMatrix modelScope;
		gl::translate( cGridStep * 2, cGridStep * 4 );
		gl::rotate( - getElapsedSeconds() * 2, 0, 0.5f, 1 );
		gl::drawLine( vec3( 0, 0, cCircleRadius ), vec3( 0, 0, -cCircleRadius ) );
		gl::drawLine( vec3( -cCircleRadius, 0, 0 ), vec3( cCircleRadius, 0, 0 ) );
		gl::drawLine( vec3( 0, -cCircleRadius, 0 ), vec3( 0, cCircleRadius, 0 ) );
	}

	// draw a geom::Source
	{
		gl::ScopedModelMatrix modelScope;
		gl::translate( cGridStep, cGridStep * 5 );
		gl::ScopedColor color( Color( 0, 0, 1 ) );

//		auto geom = geom::Rect( Rectf( -cCircleRadius, -cCircleRadius, cCircleRadius, cCircleRadius ) );
		auto geom = geom::Circle().center( vec2( 0 ) ).radius( cCircleRadius );
		gl::draw( geom );
	}
}

void ConvenienceDrawingMethodsApp::drawSourcesStressTest()
{
	const int drawCount = 1000;

	vec2 incr = vec2( getWindowSize() ) / (float)drawCount;

	gl::ScopedModelMatrix modelScope;

	for( int i = 0; i < drawCount; i++ ) {
		float perc = (float)i / (float)drawCount;
		gl::translate( incr.x, incr.y );
		gl::ScopedColor color( Color( 0, 0.5f + perc, perc ) );

		auto geom = geom::Circle().center( vec2( 0 ) ).radius( cCircleRadius );
		gl::draw( geom );
	}
}

CINDER_APP( ConvenienceDrawingMethodsApp, RendererGl )
