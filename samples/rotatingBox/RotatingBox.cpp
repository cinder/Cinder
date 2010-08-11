#include "cinder/app/AppBasic.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Capture.h"
#include "cinder/Camera.h"
#include "cinder/Text.h"

using namespace ci;
using namespace ci::app;

class RotatingCubeApp : public AppBasic {
  public:	
	void setup();
	void resize( ResizeEvent event );
	void update();
	void draw();
	
	CameraPersp			mCam;
	Capture				mCapture;
	gl::Texture			mTexture;
	Matrix44f			mCubeRotation;
};

void RotatingCubeApp::setup()
{
	try {
		mCapture = Capture( 320, 240 );
		mCapture.start();
	}
	catch( ... ) { // failed to initialize the webcam, create a warning texture
		// if we threw in the start, we'll set the Capture to null
		mCapture.reset();
		
		TextLayout layout;
		layout.clear( Color( 0.3f, 0.3f, 0.3f ) );
		layout.setColor( Color( 1, 1, 1 ) );
		layout.setFont( Font( "Arial", 96 ) );
		layout.addCenteredLine( "No Webcam" );
		layout.addCenteredLine( "Detected" );
		mTexture = gl::Texture( layout.render() );
	}
	
	mCam.lookAt( Vec3f( 3, 2, -3 ), Vec3f::zero() );
	mCubeRotation.setToIdentity();
	
	glEnable( GL_TEXTURE_2D );
	gl::enableDepthRead();
	gl::enableDepthWrite();	
}

void RotatingCubeApp::resize( ResizeEvent event )
{
	// now tell our Camera that the window aspect ratio has changed
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	
	// and in turn, let OpenGL know we have a new camera
	gl::setMatrices( mCam );
}

void RotatingCubeApp::update()
{
	if( mCapture && mCapture.checkNewFrame() ) {
		mTexture = gl::Texture( mCapture.getSurface() );
	}
	
	// Rotate the cube by .03 radians around an arbitrary axis
	mCubeRotation.rotate( Vec3f( 1, 1, 1 ), 0.03f );
}

void RotatingCubeApp::draw()
{
	// if we haven't gotten a texture from the Capture yet, don't draw anything
	if( ! mTexture )
		return;

	gl::clear( Color::black() );
	mTexture.bind();
	glPushMatrix();
		gl::multModelView( mCubeRotation );
		gl::drawCube( Vec3f::zero(), Vec3f( 2.0f, 2.0f, 2.0f ) );
	glPopMatrix();
}


CINDER_APP_BASIC( RotatingCubeApp, RendererGl )
