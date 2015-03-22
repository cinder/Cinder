#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the App class
class BasicApp : public App {
  public:
	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void draw() override;

	// This will maintain a list of points which we will draw line segments between
	list<vec2>		mPoints;
};

void BasicApp::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

void BasicApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void BasicApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

	gl::color( 1.0f, 0.5f, 0.25f );	
	gl::begin( GL_LINE_STRIP );
	for( const vec2 &point : mPoints ) {
		gl::vertex( point );
	}
	gl::end();
}

// This line tells Cinder to actually create the application
CINDER_APP( BasicApp, RendererGl )