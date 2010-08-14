#include "cinder/app/AppBasic.h"
#include "cinder/ArcBall.h"
#include "cinder/Rand.h"
#include "cinder/Camera.h"
#include "cinder/Surface.h"
#include "cinder/gl/Vbo.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

class ImageHFApp : public AppBasic {
 public:
	void    setup();
	void    resize( ResizeEvent event );
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
	gl::VboMesh		mVboMesh;
};

void ImageHFApp::setup()
{
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();    

	// initialize the arcball with a semi-arbitrary rotation just to give an interesting angle
	mArcball.setQuat( Quatf( Vec3f( 0.0577576f, -0.956794f, 0.284971f ), 3.68f ) );

	openFile();
}

void ImageHFApp::openFile()
{
	std::string path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
	if( ! path.empty() ) {
		mImage = loadImage( path );
	 
		mWidth = mImage.getWidth();
		mHeight = mImage.getHeight();

		gl::VboMesh::Layout layout;
		layout.setDynamicColorsRGB();
		layout.setDynamicPositions();
		mVboMesh = gl::VboMesh( mWidth * mHeight, 0, layout, GL_POINTS );

		updateData( kColor );		
	}
}

void ImageHFApp::resize( ResizeEvent event )
{
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( Vec2f( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f ) );
	mArcball.setRadius( getWindowHeight() / 2.0f );

	mCam.lookAt( Vec3f( 0.0f, 0.0f, -150 ), Vec3f::zero() );
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
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

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
	gl::VboMesh::VertexIter vertexIter( mVboMesh );

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

            vertexIter.setPosition( x, height * 30.0f, z );
			vertexIter.setColorRGB( color );
			++vertexIter;
		}
	}
}

CINDER_APP_BASIC( ImageHFApp, RendererGl );
