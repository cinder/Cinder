#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

#include <sstream>
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

struct Circle {
	Circle( const Vec2f &aCenter )
		: mCenter( aCenter )
	{
		mRadiusOfTravel = aCenter.x / 10.0f;
		mRadius = Rand::randFloat( 5, 15 );
		mAngle = 0.0f;
		mColor = Color( CM_HSV, Rand::randFloat( 0, 1 ), Rand::randFloat( 0.6f, 0.7f ), 1 );
		mSpeed = Rand::randPosNegFloat( 0.001f, 0.05f );
	}
	
	void update() {
		mAngle += mSpeed;
	}
	
	void draw() {
		gl::color( mColor );
		gl::drawSolidCircle( mCenter + Vec2f( cos( mAngle ), sin( mAngle ) ) * mRadiusOfTravel, mRadius );
	}
		
	Vec2f	mCenter;
	float	mSpeed, mAngle;
	float	mRadiusOfTravel, mRadius;
	Color	mColor;
};

class SaveImageApp : public AppBasic {
 public:
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();

	int				mCurrentFrame;
	list<Circle>	mCircles;
};

void SaveImageApp::setup()
{
	mCurrentFrame = 0;
	
	for( size_t s = 0; s < 100; ++s )
		mCircles.push_back( Circle( Vec2f( Rand::randFloat( 0, 640 ), Rand::randFloat( 0, 480 ) ) ) );
}

void SaveImageApp::mouseDown( MouseEvent event )
{
	// pull down the current window as a surface and pass it to writeImage
	writeImage( getHomeDirectory() + "cinder" + getPathSeparator() + "saveImage_" + toString( mCurrentFrame ) + ".png", copyWindowSurface() );	
	mCurrentFrame++;
}

void SaveImageApp::update()
{
	for( list<Circle>::iterator circleIter = mCircles.begin(); circleIter != mCircles.end(); ++circleIter )
		circleIter->update();
}

void SaveImageApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.2f ) );
	
	for( list<Circle>::iterator circleIter = mCircles.begin(); circleIter != mCircles.end(); ++circleIter ) {
		circleIter->draw();
	}
}

CINDER_APP_BASIC( SaveImageApp, RendererGl )