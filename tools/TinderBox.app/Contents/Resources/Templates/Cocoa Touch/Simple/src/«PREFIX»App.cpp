#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/Surface.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"

using namespace ci;
using namespace ci::app;

class «PREFIX»App : public AppCocoaTouch {
  public:
	virtual void	setup();
	virtual void	resize( ResizeEvent event );
	virtual void	update();
	virtual void	draw();
	virtual void	mouseDown( MouseEvent event );
		
	Matrix44f	mCubeRotation;
	gl::Texture mTex;
	CameraPersp	mCam;
};

void «PREFIX»App::setup()
{
	mCubeRotation.setToIdentity();

	// Create a blue-green gradient as an OpenGL texture
	Surface8u surface( 256, 256, false );
	Surface8u::Iter iter = surface.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			iter.r() = 0;
			iter.g() = iter.x();
			iter.b() = iter.y();
		}
	}
	
	mTex = gl::Texture( surface );
}

void «PREFIX»App::resize( ResizeEvent event )
{
	mCam.lookAt( Vec3f( 3, 2, -3 ), Vec3f::zero() );
	mCam.setPerspective( 60, event.getAspectRatio(), 1, 1000 );
}

void «PREFIX»App::mouseDown( MouseEvent event )
{
	console() << "Mouse down @ " << event.getPos() << std::endl;
}

void «PREFIX»App::update()
{
	mCubeRotation.rotate( Vec3f( 1, 1, 1 ), 0.03f );
}

void «PREFIX»App::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.3f ) );
	gl::enable( GL_TEXTURE_2D );
	gl::enableDepthRead();
	
	mTex.bind();
	gl::setMatrices( mCam );
	glPushMatrix();
		gl::multModelView( mCubeRotation );
		gl::drawCube( Vec3f::zero(), Vec3f( 2.0f, 2.0f, 2.0f ) );
	glPopMatrix();
}

CINDER_APP_COCOA_TOUCH( «PREFIX»App, RendererGl )
