#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// Simple class to demonstrate custom lerping
struct Box {
	Box() : mColor( Color( 1, 0.5f, 0.25f ) ), mPos( 320, 240 ), mSize( 10, 10 ) {}
	Box( Color color, Vec2f pos, Vec2f size )
		: mColor( color ), mPos( pos ), mSize( size )
	{}
	
	void draw() const {
		gl::color( mColor );
		gl::drawSolidRect( Rectf( mPos, mPos + mSize ) );
	}
	
	Color	mColor;
	Vec2f	mPos, mSize;
};

// custom lerp function which simply lerps all of the member variables individually
Box boxLerp( const Box &start, const Box &end, float t )
{
	return Box( lerp( start.mColor, end.mColor, t ), lerp( start.mPos, end.mPos, t), lerp( start.mSize, end.mSize, t ) );
}


class CustomLerpApp : public AppBasic {
  public:
	void	setup();
	void	mouseDown( MouseEvent event );	
	Box		randomBox( Vec2f center );
	void	draw();
	
	Anim<Box>	mBox;
};

void CustomLerpApp::setup()
{
	mBox = randomBox( getWindowCenter() );
}

void CustomLerpApp::mouseDown( MouseEvent event )
{
	timeline().apply( &mBox, randomBox( event.getPos() ), 2.0f, EaseOutCubic(), boxLerp );
}

Box	CustomLerpApp::randomBox( Vec2f center )
{
	Color color( CM_HSV, Rand::randFloat(), 1, 1 );
	float size( Rand::randFloat( 20, 40 ) );
	Vec2f pos = center - Vec2f( size, size ) / 2;
	return Box( color, pos, Vec2f( size, size ) );
}

void CustomLerpApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0.7f, 0.7f, 0.7f ) );
	
	mBox().draw();
}


CINDER_APP_BASIC( CustomLerpApp, RendererGl )
