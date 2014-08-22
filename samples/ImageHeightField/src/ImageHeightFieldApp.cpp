#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererGl.h"
#include "cinder/ArcBall.h"
#include "cinder/Camera.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Shader.h"

using namespace ci;
using namespace ci::app;

class ImageHFApp : public AppBasic {
 public:
	void    setup();
	void    resize();
	void    mouseDown( MouseEvent event );
	void    mouseDrag( MouseEvent event );
	void    keyDown( KeyEvent event );
	void    draw();
	void	openFile();

 private:
 	enum ColorSwitch {
		kColor, kRed, kGreen, kBlue
	};
	void updateData( ColorSwitch whichColor );
 
	CameraPersp mCam;
	Arcball     mArcball;

	uint32_t    mWidth, mHeight;

	Surface32f		mImage;
	gl::VboMeshRef	mVboMesh;
	gl::BatchRef	mPointsBatch;
};

void ImageHFApp::setup()
{
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();    

	// initialize the arcball with a semi-arbitrary rotation just to give an interesting angle
	mArcball.setQuat( quat( 0.28f, vec3( 0.0577576f, -0.956794f, 0.284971f ) ) );

	openFile();
}

void ImageHFApp::openFile()
{
	fs::path path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
	if( ! path.empty() ) {
		mImage = loadImage( path );
	 
		mWidth = mImage.getWidth();
		mHeight = mImage.getHeight();

		mVboMesh = gl::VboMesh::create( mWidth * mHeight, GL_POINTS, { gl::VboMesh::Layout().usage(GL_STATIC_DRAW).attrib(geom::POSITION, 3).attrib(geom::COLOR, 3) } );
		mPointsBatch = gl::Batch::create( mVboMesh, gl::getStockShader( gl::ShaderDef().color() ) );

		updateData( kColor );		
	}
}

void ImageHFApp::resize()
{
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( Vec2f( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f ) );
	mArcball.setRadius( getWindowHeight() / 2.0f );

	mCam.lookAt( vec3( 0.0f, 0.0f, -150 ), vec3( 0, 0, 0 ) );
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 1000.0f );
	gl::setMatrices( mCam );
}

void ImageHFApp::mouseDown( MouseEvent event )
{
    mArcball.mouseDown( event.getPos() );
}

void ImageHFApp::mouseDrag( MouseEvent event )
{
    mArcball.mouseDrag( event.getPos() );
}

void ImageHFApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case 'r':
			updateData( kRed );
		break;
		case 'g':
			updateData( kGreen );
		break;
		case 'b':
			updateData( kBlue );
		break;
		case 'c':
			updateData( kColor );
		break;
		case 'o':
			openFile();
		break;
	}
}

void ImageHFApp::draw()
{
    gl::clear();

    gl::pushModelView();
		gl::translate( Vec3f( 0.0f, 0.0f, mHeight / 2.0f ) );
		gl::rotate( mArcball.getQuat() );
		if( mVboMesh )
			gl::draw( mVboMesh );
    gl::popModelView();
}

void ImageHFApp::updateData( ImageHFApp::ColorSwitch whichColor )
{
	Surface32f::Iter pixelIter = mImage.getIter();
	auto vertPosIter = mVboMesh->mapAttrib3f( geom::POSITION );
	auto vertColorIter = mVboMesh->mapAttrib3f( geom::COLOR );

	while( pixelIter.line() ) {
		while( pixelIter.pixel() ) {
			Color color( pixelIter.r(), pixelIter.g(), pixelIter.b() );
			float height;
			const float muteColor = 0.2f;

			// calculate the height based on a weighted average of the RGB, and emphasize either the red green or blue color in each of those modes
			switch( whichColor ) {
				case kColor:
					height = color.dot( Color( 0.3333f, 0.3333f, 0.3333f ) );
				break;
				case kRed:
					height = color.dot( Color( 1, 0, 0 ) );
					color *= Color( 1, muteColor, muteColor );
				break;
				case kGreen:
					height = color.dot( Color( 0, 1, 0 ) );
					color *= Color( muteColor, 1, muteColor );
				break;
				case kBlue:
					height = color.dot( Color( 0, 0, 1 ) );
					color *= Color( muteColor, muteColor, 1 );					
				break;            
			}

			// the x and the z coordinates correspond to the pixel's x & y
			float x = pixelIter.x() - mWidth / 2.0f;
			float z = pixelIter.y() - mHeight / 2.0f;

			*vertPosIter++ = Vec3f( x, height * 30.0f, z );
			*vertColorIter++ = Vec3f( color.r, color.g, color.b );
		}
	}

	vertPosIter.unmap();
	vertColorIter.unmap();
}

CINDER_APP_BASIC( ImageHFApp, RendererGl )
