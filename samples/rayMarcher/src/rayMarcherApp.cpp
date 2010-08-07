#include "cinder/app/AppBasic.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/Texture.h"
#include "cinder/MayaCamUI.h"

#include "RayMarcher.h"

using namespace ci;
using namespace ci::app;

class rayMarcherApp : public AppBasic {
 public:	
	rayMarcherApp() : mMarcher( &mMayaCam.getCamera() ) {}
	
	void		prepareSettings( Settings *settings );

	void		setup();
	void		mouseDown( MouseEvent event );
	void		mouseDrag( MouseEvent event );
	void		keyDown( KeyEvent event );	
	void		resize( ResizeEvent event );
	void		update();
	void		draw();

	shared_ptr<Surface8u>	mImageSurface;
	gl::Texture				mImageTexture;

	RayMarcher		mMarcher;
	MayaCamUI		mMayaCam;
	Vec3f			mStartEyePoint;
	int				mCurrentLine;
};

void rayMarcherApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 400, 300 );
	settings->setFullScreen( false );
	settings->setResizable( true );
	settings->setFrameRate( 30.0f );
}

void rayMarcherApp::setup()
{
	CameraPersp cam;
	mStartEyePoint = Vec3f( 15, 21, 27.5 ) * 0.65f;
	cam.lookAt( mStartEyePoint, Vec3f::zero(), Vec3f::yAxis() );
	cam.setCenterOfInterest( mStartEyePoint.distance( Vec3f::zero() ) );
	mMayaCam.setCurrentCam( cam );
}

void rayMarcherApp::mouseDown( MouseEvent event )
{		
	mMayaCam.mouseDown( event.getPos() );
}

void rayMarcherApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
	mCurrentLine = 0;	
}

void rayMarcherApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 's' ) {
		mMarcher.randomScene();
		mCurrentLine = 0;
	}
}

void rayMarcherApp::resize( ResizeEvent event )
{
	mImageSurface = shared_ptr<Surface8u>( new Surface8u( getWindowWidth(), getWindowHeight(), false ) );
	mImageTexture = gl::Texture( *mImageSurface );
	mCurrentLine = 0;
	
	CameraPersp cam = mMayaCam.getCamera();
	cam.setPerspective( 45, getWindowWidth() / (float)getWindowHeight(), 0.1f, 100.0f );
	mMayaCam.setCurrentCam( cam );
}

void rayMarcherApp::update()
{
	if( mCurrentLine < getWindowHeight() ) {
		mMarcher.renderScanline( mCurrentLine, mImageSurface.get() );
		mImageTexture.update( *mImageSurface, Area( 0, mCurrentLine, mImageSurface->getWidth(), mCurrentLine + 1 ) );
		mCurrentLine++;
	}
}

void rayMarcherApp::draw()
{
	glClearColor( 0, 0, 0, 0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	glDepthMask( GL_TRUE );
	glDisable( GL_TEXTURE_2D );
	
	gl::setMatrices( mMayaCam.getCamera() );
	mMarcher.renderSceneGL();

	gl::setMatricesWindow( getWindowSize() );

	// draw as much of the texture as we've rendered
	glDisable( GL_LIGHTING );
	glDepthMask( GL_TRUE );
	glDisable( GL_DEPTH_TEST );
	
	glColor3f( 1, 1, 1 );
	mImageTexture.enableAndBind();
	glBegin( GL_QUADS );
		glTexCoord2f( mImageTexture.getLeft(), mImageTexture.getTop() );
		glVertex2f( 0, 0 );
		glTexCoord2f( mImageTexture.getLeft(), mImageTexture.getBottom() * mCurrentLine / mImageTexture.getHeight() );
		glVertex2f( 0, mCurrentLine );
		glTexCoord2f( mImageTexture.getRight(), mImageTexture.getBottom() * mCurrentLine / mImageTexture.getHeight() );
		glVertex2f( mImageTexture.getWidth(), mCurrentLine );
		glTexCoord2f( mImageTexture.getRight(), mImageTexture.getTop() );
		glVertex2f( mImageTexture.getWidth(), 0 );
	glEnd();
}

CINDER_APP_BASIC( rayMarcherApp, RendererGl )