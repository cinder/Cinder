#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

#include <cmath>
#include <cstring>
#include <iostream>
#include <cassert>
#include <limits>

#include "cinder/CinderMath.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Vec_isNaNApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void Vec_isNaNApp::setup()
{
	Vec2f testVec = Vec2f( -0.1, 4.0 );
//	isNaN( 0.1f );
//	isnan( 0.1f );
	console() << isNaN(testVec.x) << endl;
}

void Vec_isNaNApp::mouseDown( MouseEvent event )
{
}

void Vec_isNaNApp::update()
{
}

void Vec_isNaNApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_BASIC( Vec_isNaNApp, RendererGl )
