#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// Simple class to demonstrate custom lerping
struct Box {
	Box() : mColor( Color( 1, 0.5f, 0.25f ) ), mPos( 320, 240 ), mSize( 10, 10 ) {}
	Box( Color color, vec2 pos, vec2 size )
		: mColor( color ), mPos( pos ), mSize( size )
	{}
	
	void draw() const {
		gl::color( mColor );
		gl::drawSolidRect( Rectf( mPos, mPos + mSize ) );
	}
	
	Color	mColor;
	vec2	mPos, mSize;
};

// custom lerp function which simply lerps all of the member variables individually
Box boxLerp( const Box &start, const Box &end, float t )
{
	return Box( lerp( start.mColor, end.mColor, t ), lerp( start.mPos, end.mPos, t), lerp( start.mSize, end.mSize, t ) );
}


class CustomLerpApp : public App {
  public:
	void	setup();
	void	mouseDown( MouseEvent event );	
	Box		randomBox( vec2 center );
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

Box	CustomLerpApp::randomBox( vec2 center )
{
	Color color( CM_HSV, Rand::randFloat(), 1, 1 );
	float size( Rand::randFloat( 20, 40 ) );
	vec2 pos = center - vec2( size, size ) / 2.0f;
	return Box( color, pos, vec2( size, size ) );
}

void CustomLerpApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0.7f, 0.7f, 0.7f ) );
	
	mBox().draw();
}


CINDER_APP( CustomLerpApp, RendererGl( RendererGl::Options().msaa( 4 ) ) )
