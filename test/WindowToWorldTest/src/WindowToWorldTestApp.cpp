#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#undef near
#undef far

using namespace ci;
using namespace ci::app;
using namespace std;

//! A rectangle that can be transformed.
struct EditableRect {
	Rectf  area;
	vec2   position;
	vec2   scale;
	quat   rotation;

	EditableRect() : scale( 1 ) {}
	~EditableRect() {}

	//! Returns the rectangle's model matrix.
	mat4  matrix()
	{
		mat4 m = glm::translate( vec3( position, 0 ) );
		m *= glm::toMat4( rotation );
		m *= glm::scale( vec3( scale, 1 ) );
		m *= glm::translate( vec3( -area.getSize() * 0.5f, 0 ) );

		return m;
	}
};

class WindowToWorldTestApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;

	void mouseMove( MouseEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;

	void resize() override;
private:
	EditableRect   mRectangle;

	ivec2          mMouseInitial;
	EditableRect   mRectangleInitial;

	ivec2          mMousePos;
	bool           mIsOver;
	bool           mIsClicked;
};

void WindowToWorldTestApp::setup()
{
	gl::enableVerticalSync( false );

	mRectangle.area = Rectf( 0, 0, 320, 240 );
	mRectangle.position = getWindowSize() / 2;

	mIsClicked = false;
	mIsOver = false;
}

void WindowToWorldTestApp::update()
{
	// Check if mouse is inside rectangle, by converting the mouse coordinates to object space.
	vec3 object = gl::windowToObjectCoord( mRectangle.matrix(), mMousePos );
	mIsOver = mRectangle.area.contains( vec2( object ) );
}

void WindowToWorldTestApp::draw()
{
	gl::clear();

	// Create a list of (object space) corners, so we can easily draw them.
	vec2 corners[] = { mRectangle.area.getUpperLeft(), mRectangle.area.getUpperRight(),
		mRectangle.area.getLowerRight(), mRectangle.area.getLowerLeft() };

	// Either use setMatricesWindow() or setMatricesWindowPersp() to enable 2D rendering.
	gl::setMatricesWindow( getWindowSize(), false );

	// Draw the transformed rectangle.
	gl::pushModelMatrix();
	gl::multModelMatrix( mRectangle.matrix() );

	// Draw a stroked rect in magenta (if mouse inside) or white (if mouse outside).
	if( mIsOver )
		gl::color( 1, 0, 1 );
	else
		gl::color( 1, 1, 1 );
	gl::drawStrokedRect( mRectangle.area );

	// Draw the same rect as line segments.
	gl::drawLine( corners[0], corners[1] );
	gl::drawLine( corners[1], corners[2] );
	gl::drawLine( corners[2], corners[3] );
	gl::drawLine( corners[3], corners[0] );

	// Draw the 4 corners as yellow crosses.
	for( vec2 &corner : corners ) {
		gl::color( 1, 1, 0 );
		gl::drawLine( vec2( corner.x, corner.y - 5 ), vec2( corner.x, corner.y + 5 ) );
		gl::drawLine( vec2( corner.x - 5, corner.y ), vec2( corner.x + 5, corner.y ) );
	}

	gl::popModelMatrix();

	// To test worldToWindowCoord, let's draw the 4 corners again,
	// this time as red crosses without changing the model matrix. [ Excercising gl::worldToWindowCoord() ].
	for( vec2 &corner : corners ) {
		vec4 world = mRectangle.matrix() * vec4( corner, 0, 1 );
		vec2 window = gl::worldToWindowCoord( vec3( world ) );
		vec2 scale = mRectangle.scale;

		gl::color( 1, 0, 0 );
		gl::drawLine( vec2( window.x, window.y - 5 * scale.y ), vec2( window.x, window.y + 5 * scale.y ) );
		gl::drawLine( vec2( window.x - 5 * scale.x, window.y ), vec2( window.x + 5 * scale.x, window.y ) );
	}
}

void WindowToWorldTestApp::mouseMove( MouseEvent event )
{
	// Keep track of mouse position.
	mMousePos = event.getPos();
}

void WindowToWorldTestApp::mouseDown( MouseEvent event )
{
	// Check if mouse is inside rectangle, by converting the mouse coordinates
	// to world space and then to object space. [ Excercising gl::windowToWorldCoord() ].
	vec3 world = gl::windowToWorldCoord( event.getPos() );
	vec4 object = glm::inverse( mRectangle.matrix() ) * vec4( world, 1 );

	// Now we can simply use Area::contains() to find out if the mouse is inside.
	mIsClicked = mRectangle.area.contains( vec2( object ) );
	if( mIsClicked ) {
		mMouseInitial = event.getPos();
		mRectangleInitial = mRectangle;
	}
}

void WindowToWorldTestApp::mouseDrag( MouseEvent event )
{
	// Keep track of mouse position.
	mMousePos = event.getPos();

	// Scale and rotate the rectangle.
	if( mIsClicked ) {
		// Calculate the initial click position and the current mouse position, in world coordinates relative to the rectangle's center.
		vec3 initial = gl::windowToWorldCoord( mMouseInitial ) - vec3( mRectangle.position, 0 );
		vec3 current = gl::windowToWorldCoord( event.getPos() ) - vec3( mRectangle.position, 0 );

		// Calculate scale by using the distance to the center of the rectangle.
		float d0 = glm::length( initial );
		float d1 = glm::length( current );
		mRectangle.scale = vec2( mRectangleInitial.scale * ( d1 / d0 ) );

		// Calculate rotation by taking the angle with the X-axis for both positions and calculating the difference.
		float a0 = math<float>::atan2( initial.y, initial.x );
		float a1 = math<float>::atan2( current.y, current.x );
		mRectangle.rotation = mRectangleInitial.rotation * glm::angleAxis( a1 - a0, vec3( 0, 0, 1 ) );
	}
}

void WindowToWorldTestApp::resize()
{
	// Keep the rectangle centered in the window.
	mRectangle.position = getWindowSize() / 2;
}

CINDER_APP( WindowToWorldTestApp, RendererGl )
