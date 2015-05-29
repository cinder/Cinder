#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/CameraUi.h"
#include "cinder/Camera.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Shader.h"

using namespace ci;
using namespace ci::app;

class ImageHFApp : public App {
 public:
	void    setup() override;
	void    keyDown( KeyEvent event ) override;
	void    draw() override;
	void	openFile();

 private:
 	enum ColorSwitch {
		kColor, kRed, kGreen, kBlue
	};
	void updateData( ColorSwitch whichColor );
 
	CameraPersp		mCam;
	CameraUi		mCamUi;

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

	mCamUi = CameraUi( &mCam, getWindow() );
	mCam.setNearClip( 10 );
	mCam.setFarClip( 2000 );

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
		mCam.lookAt( vec3( mWidth / 2, 50, mHeight / 2 ), vec3( 0 ) );
	}
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

	gl::setMatrices( mCam );
	if( mPointsBatch )
		mPointsBatch->draw();
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
					height = dot( color, Color( 0.3333f, 0.3333f, 0.3333f ) );
				break;
				case kRed:
					height = dot( color, Color( 1, 0, 0 ) );
					color *= Color( 1, muteColor, muteColor );
				break;
				case kGreen:
					height = dot( color, Color( 0, 1, 0 ) );
					color *= Color( muteColor, 1, muteColor );
				break;
				case kBlue:
					height = dot( color, Color( 0, 0, 1 ) );
					color *= Color( muteColor, muteColor, 1 );					
				break;            
			}

			// the x and the z coordinates correspond to the pixel's x & y
			float x = pixelIter.x() - mWidth / 2.0f;
			float z = pixelIter.y() - mHeight / 2.0f;

			*vertPosIter++ = vec3( x, height * 30.0f, z );
			*vertColorIter++ = vec3( color.r, color.g, color.b );
		}
	}

	vertPosIter.unmap();
	vertColorIter.unmap();
}

CINDER_APP( ImageHFApp, RendererGl )
