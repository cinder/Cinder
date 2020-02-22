#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/PolyLine.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/Rand.h"
#include "cinder/Path2d.h"
#include "cinder/Timer.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

typedef PolyLineT<vec3>		PolyLine3;


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
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
  private:
	void drawBasicOverview();
	void drawSourcesStressTest();

	Path2d		mPath;
	PolyLine2	mPolyline2D;
	PolyLine3	mPolyline3D;
	bool		mPrintFrameRate = false;
};

void ConvenienceDrawingMethodsApp::setup()
{

	mPath.arc( vec2( 0 ), cCircleRadius, 0, M_PI * 1.66f );
	mPath.lineTo( vec2( 0 ) );

	Rand r;
	for( int i = 0; i < 50; ++i ) {
		mPolyline2D.push_back( r.nextVec2() * cCircleRadius );
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

void ConvenienceDrawingMethodsApp::keyDown( KeyEvent event )
{
#if ! defined( CINDER_GL_ES )
	if( event.getChar() == 'w' )
		gl::setWireframeEnabled( ! gl::isWireframeEnabled() );
#endif
	if( event.getChar() == 'f' )
		mPrintFrameRate = ! mPrintFrameRate;
}

void ConvenienceDrawingMethodsApp::update()
{
	if( mPrintFrameRate && getElapsedFrames() % 30 == 0 )
		console() << "fps: " << getAverageFps()  << endl;
}

void ConvenienceDrawingMethodsApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.2f ) );

	drawBasicOverview();
//	drawSourcesStressTest();

	CI_CHECK_GL();
}

void ConvenienceDrawingMethodsApp::drawBasicOverview()
{
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
		gl::draw( mPolyline3D.getPoints() );
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

	// draw a 3D geom::Source that uses geom::Attrib::TRIANGLES
	{
		gl::ScopedDepth depthScope( true );

		gl::ScopedModelMatrix modelScope;
		gl::translate( cGridStep * 2, cGridStep * 5, 2 );
		gl::rotate( M_PI / 2, 0.7f, 0.3f, 0.3f );
		gl::rotate( getElapsedSeconds(), 0, 1, 0 );
		gl::ScopedColor color( Color( 0, 1, 1 ) );

		auto geom = geom::Torus().radius( cCircleRadius, cCircleRadius * 0.7f ).colors();
//		auto geom = geom::Sphere().radius( cCircleRadius ).colors();
		gl::draw( geom );
	}

	// draw a 3D geom::Source that uses geom::Attrib::LINES
	{
		gl::ScopedDepth depthScope( true );

		gl::ScopedModelMatrix modelScope;
		gl::translate( cGridStep * 3, cGridStep * 5, 2 );
		gl::rotate( M_PI / 2, 0.7f, 0.3f, 0.3f );
		gl::rotate( getElapsedSeconds(), 0, 1, 0 );
		gl::ScopedColor color( Color( 0, 1, 1 ) );

		auto geom = geom::WireTorus().radius( cCircleRadius, cCircleRadius * 0.7f );
		gl::draw( geom );
	}
}

void ConvenienceDrawingMethodsApp::drawSourcesStressTest()
{
	gl::setMatricesWindowPersp( getWindowSize() );

	const int drawCount = 2000;

	vec2 incr = vec2( getWindowSize() ) / (float)drawCount;
//	auto circleGeom = geom::Circle().center( vec2( 0 ) ).radius( cCircleRadius );
	auto sphereGeom = geom::Sphere().radius( cCircleRadius ).subdivisions( 50 ).colors();
	auto cubeGeom = geom::Cube().subdivisions( 10 ).colors() >> geom::Scale( 10 );

	gl::VboMeshRef sphereVboMesh = gl::VboMesh::create( sphereGeom );
	gl::VboMeshRef cubeVboMesh = gl::VboMesh::create( cubeGeom );

	gl::ScopedModelMatrix modelScope;

	Timer timer( true );
	for( int i = 0; i < drawCount; i++ ) {
		float perc = (float)i / (float)drawCount;
		gl::translate( incr.x, incr.y );
		gl::ScopedColor color( Color( 0, 0.5f + perc, perc ) );

//		gl::draw( circleGeom );
//		gl::drawSolidCircle( vec2( 0 ), cCircleRadius );

//		gl::drawSphere( vec3(), cCircleRadius, 50 );
		if( i % 2 == 0 )
//			gl::draw( sphereGeom );
			gl::draw( sphereVboMesh );
		else
//			gl::draw( cubeGeom );
			gl::draw( cubeVboMesh );
	}

	if( getElapsedFrames() % 30 == 0 )
		CI_LOG_V( "draw cpu time (ms): " << timer.getSeconds() * 1000 );
}

CINDER_APP( ConvenienceDrawingMethodsApp, RendererGl, []( App::Settings *settings ) {
	settings->setWindowPos( 0, 0 );
	settings->setWindowSize( 960, 564 );
} )
