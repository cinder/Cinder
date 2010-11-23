#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Path2d.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PathBasicApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void drawPath( const Path2d &p );

void PathBasicApp::setup()
{
}

void PathBasicApp::mouseDown( MouseEvent event )
{
}

void PathBasicApp::update()
{
}

void drawPath( const Path2d &p )
{
	gl::color( Color( 1, 0.5f, 0.25f ) );
	for( size_t i = 0; i < p.getNumPoints(); ++i )
		gl::drawSolidCircle( p.getPoint( i ), 3 );


	gl::color( Color( 0, 1, 0.25f ) );
	glBegin( GL_LINE_STRIP );
	for( float t = 0; t < 1.0f; t += 0.01f ) {
		gl::vertex( p.getPosition( t ) );
		app::console() << p.getPosition( t ) << std::endl;
	}
	glEnd();	
}

void PathBasicApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	
	Path2d p1;
	p1.moveTo( 10, 10 );
	//p1.lineTo( 20, 30 );
	p1.quadTo( 100, 50, 100, 100 );
	p1.curveTo( 125, 125, 155, 125, 165, 100 );
	p1.close();
	drawPath( p1 );
}


CINDER_APP_BASIC( PathBasicApp, RendererGl )
