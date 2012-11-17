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
	Vec2f testVec1 = Vec2f( -0.1, 4.0 );
	Vec2f testVec2 = Vec2f( NAN, 4.0 );
	Vec2f testVec3 = Vec2f( 5.7, NAN );
	Vec3f testVec4 = Vec3f( M_PI, 1.0, 2.0 );
	Vec3f testVec5 = Vec3f( M_PI, 5.7, NAN );
	Vec3f testVec6 = Vec3f( NAN, 5.7, NAN );
	Vec3f testVec7 = Vec3f( 1.0, NAN, NAN );
	Vec4f testVec8 = Vec4f( 1.0, 1.0, 1.0, 1.0 );
	Vec4f testVec9 = Vec4f( NAN, 1.0, 1.0, 1.0 );
	Vec4f testVec10 = Vec4f( 1.0, NAN, 1.0, 1.0 );
	Vec4f testVec11 = Vec4f( 1.0, 1.0, NAN, 1.0 );
	Vec4f testVec12 = Vec4f( 1.0, 1.0, 1.0, NAN );


	console() << "1: " << isNaN(testVec1) << endl;
	console() << "2: " << isNaN(testVec2) << endl;
	console() << "3: " << isNaN(testVec3) << endl;
	console() << "4: " << isNaN(testVec4) << endl;
	console() << "5: " << isNaN(testVec5) << endl;
	console() << "6: " << isNaN(testVec6) << endl;
	console() << "7: " << isNaN(testVec7) << endl;
	console() << "8: " << isNaN(testVec8) << endl;
	console() << "9: " << isNaN(testVec9) << endl;
	console() << "10: " << isNaN(testVec10) << endl;
	console() << "11: " << isNaN(testVec11) << endl;
	console() << "12: " << isNaN(testVec12) << endl;
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
