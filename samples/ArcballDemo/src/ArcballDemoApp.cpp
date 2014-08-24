#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Arcball.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;

class ArcballDemoApp : public AppBasic {
 public:	
	void setup();
	void resize();
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();
	void drawVerbose();	
	
	Arcball		mArcball;
	ivec2		mInitialMouseDown, mCurrentMouseDown;
	bool		mDrawVerbose;
	bool		mUseConstraintAxis;
	vec3		mConstraintAxis;
};

void ArcballDemoApp::setup()
{
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();
	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CW ); // the default camera inverts to a clockwise front-facing direction

	mDrawVerbose = true;
	mUseConstraintAxis = false;
	mCurrentMouseDown = mInitialMouseDown = ivec2( 200, 200 );
}

void ArcballDemoApp::resize()
{
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( 150 );
}

void ArcballDemoApp::mouseDown( MouseEvent event )
{
	mArcball.mouseDown( event.getPos() );
	mCurrentMouseDown = mInitialMouseDown = event.getPos();
}

void ArcballDemoApp::mouseDrag( MouseEvent event )
{
	mArcball.mouseDrag( event.getPos() );
	mCurrentMouseDown = event.getPos();
}

void ArcballDemoApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'v' )
		mDrawVerbose = ! mDrawVerbose;
	else if( event.getChar() == 'c' ) {
		mUseConstraintAxis = ! mUseConstraintAxis;
		if( mUseConstraintAxis ) {
			// make a random constraint axis
			mConstraintAxis = Rand::randVec3f();
			mArcball.setConstraintAxis( mConstraintAxis );
		}
		else
			mArcball.setNoConstraintAxis();
	}
}

// this draws with the sphere and axes to help undertand how arcball works
void ArcballDemoApp::drawVerbose()
{
	// draw the cube
	gl::pushModelMatrix();
		gl::translate( getWindowCenter() );
		gl::scale( vec3( 200.0f, 200.0f, 200.0f ) );	
		gl::rotate( mArcball.getQuat() );
		gl::drawColorCube( vec3( 0.0f ), vec3( 1, 1, 1 ) );
	gl::popModelMatrix();

	// draw the back faces of the sphere
	gl::pushModelMatrix();
		gl::color( 0.0, 0.0, 0.0, 0.3f );
		gl::disableDepthWrite();
		gl::translate( getWindowCenter() );
		glCullFace( GL_FRONT );
		gl::drawSphere( vec3( 0 ), mArcball.getRadius(), 50 );
	gl::popModelMatrix();

	gl::enableDepthWrite();
	glCullFace( GL_BACK );

	// draw the axes
	gl::pushModelMatrix();
		gl::translate( getWindowCenter() );
		
		// start
		gl::color( 1.0, 1.0, 0.0f, 1.0f );
		gl::drawVector( vec3( 0 ), mArcball.mouseOnSphere( mInitialMouseDown ) * mArcball.getRadius(), 10, 5 );
		
		// end
		gl::color( 0.0, 1.0, 0.0f, 1.0f );
		gl::drawVector( vec3( 0 ), mArcball.mouseOnSphere( mCurrentMouseDown ) * mArcball.getRadius(), 10, 5 );
		
		// constraint
		if( mUseConstraintAxis ) {
			gl::color( ColorAf( 0.0, 0.7f, 1.0f, 1.0f ) );
			gl::drawVector( mConstraintAxis * -mArcball.getRadius() * 1.5f, mConstraintAxis * mArcball.getRadius() * 1.5f, 10, 5 );
		}
	gl::pushModelMatrix();

	// draw the front faces of the sphere
	gl::pushModelMatrix();
		gl::color( ColorAf( 0.0, 0.0, 0.0, 0.3f ) );
		gl::translate( getWindowCenter() );
		gl::drawSphere( vec3( 0.0f ), mArcball.getRadius(), 50 );
	gl::popModelMatrix();
}

void ArcballDemoApp::draw()
{
	gl::clear( Color( 0, 0.1f, 0.2f ) );

	if( mDrawVerbose )
		drawVerbose();
	else {
		// the order of operations is important here - we don't want the rotation applied to the scale
		// and likewise, we don't want the translation applied to the scale 
		// So the code below:
		// 1) rotates the cube by the quaternion that arcball has provided
		// 2) scales the cube to be 200 pixels big
		// 3) moves the cube to the center of the window
		gl::pushModelMatrix();
			glCullFace( GL_BACK );
			gl::translate( getWindowSize() / 2 );
			gl::scale( vec3( 200.0f, 200.0f, 200.0f ) );	
			gl::rotate( mArcball.getQuat() );
			gl::drawColorCube( vec3( 0 ), vec3( 1 ) );
		gl::popModelMatrix();
	}
}


CINDER_APP_BASIC( ArcballDemoApp, RendererGl )