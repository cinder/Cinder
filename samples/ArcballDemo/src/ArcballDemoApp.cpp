#include "cinder/app/AppBasic.h"
#include "cinder/Arcball.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;

class ArcballDemoApp : public AppBasic {
 public:	
	void setup();
	void resize( ResizeEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();
	void drawVerbose();	
	
	Arcball		mArcball;
	Vec2i		mInitialMouseDown, mCurrentMouseDown;
	bool		mDrawVerbose;
	bool		mUseConstraintAxis;
	Vec3f		mConstraintAxis;
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
	mCurrentMouseDown = mInitialMouseDown = Vec2i( 200, 200 );
}

void ArcballDemoApp::resize( ResizeEvent event )
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
	gl::pushModelView();
		gl::translate( getWindowCenter() );
		gl::scale( Vec3f( 200.0f, 200.0f, 200.0f ) );	
		gl::rotate( mArcball.getQuat() );
		gl::drawColorCube( Vec3f::zero(), Vec3f( 1, 1, 1 ) );
	gl::popModelView();

	// draw the back faces of the sphere
	gl::pushModelView();
		glColor4f( 0.0, 0.0, 0.0, 0.3f );
		gl::disableDepthWrite();
		gl::translate( getWindowCenter() );
		glCullFace( GL_FRONT );
		gl::drawSphere( Vec3f::zero(), mArcball.getRadius(), 50 );
	gl::popModelView();

	gl::enableDepthWrite();
	glCullFace( GL_BACK );

	// draw the axes
	gl::pushModelView();
		gl::translate( getWindowCenter() );
		
		// start
		glColor4f( 1.0, 1.0, 0.0f, 1.0f );
		gl::drawVector( Vec3f::zero(), mArcball.mouseOnSphere( mInitialMouseDown ) * mArcball.getRadius(), 10, 5 );
		
		// end
		glColor4f( 0.0, 1.0, 0.0f, 1.0f );
		gl::drawVector( Vec3f::zero(), mArcball.mouseOnSphere( mCurrentMouseDown ) * mArcball.getRadius(), 10, 5 );
		
		// constraint
		if( mUseConstraintAxis ) {
			glColor4f( 0.0, 0.7f, 1.0f, 1.0f );
			gl::drawVector( mConstraintAxis * -mArcball.getRadius() * 1.5, mConstraintAxis * mArcball.getRadius() * 1.5, 10, 5 );		
		}
	gl::popModelView();

	// draw the front faces of the sphere
	gl::pushModelView();
		glColor4f( 0.0, 0.0, 0.0, 0.3f );
		gl::translate( getWindowCenter() );
		gl::drawSphere( Vec3f::zero(), mArcball.getRadius(), 50 );
	gl::popModelView();	
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
		gl::pushModelView();
			glCullFace( GL_BACK );
			gl::translate( getWindowSize() / 2.0f );
			gl::scale( Vec3f( 200.0f, 200.0f, 200.0f ) );	
			gl::rotate( mArcball.getQuat() );
			gl::drawColorCube( Vec3f::zero(), Vec3f( 1, 1, 1 ) );
		gl::popModelView();		
	}
}


CINDER_APP_BASIC( ArcballDemoApp, RendererGl )