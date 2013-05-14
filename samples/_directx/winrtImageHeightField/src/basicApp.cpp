#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererDx.h"
#include "cinder/Arcball.h"
#include "cinder/Rand.h"
#include "cinder/Camera.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"

#include "cinder/Font.h"

#include "cinder/dx/dx.h"
#include "cinder/dx/DxVbo.h"
#include "cinder/dx/DxTexture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public AppBasic {

  public:

	void			setup();
	void			mouseDown( MouseEvent event );
	void		    mouseDrag( MouseEvent event );
	void		    keyDown( KeyEvent event );
	void			draw();
	void			openFile();

	Font				mFont;

  private:

	enum ColorSwitch {
		kColor, kRed, kGreen, kBlue
	};

	void updateData( ColorSwitch whichColor );

	CameraPersp		mCam;
	Arcball			mArcball;

	uint32_t		mWidth, mHeight;
	Surface32f		mImage;
	dx::VboMesh		mVboMesh;
};

void BasicApp::setup()
{
	mFont = Font( "Arial", 24.0f );

	dx::enableAlphaBlending();

	mArcball.setQuat( Quatf( Vec3f( 0.0577576f, -0.956794f, 0.284971f ), 3.68f ) );
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( Vec2f( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f ) );
	mArcball.setRadius( getWindowHeight() / 2.0f );

	mCam.lookAt( Vec3f( 0.0f, 0.0f, -150 ), Vec3f::zero() );
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 1000.0f );
	dx::setMatrices( mCam );

	openFile();
}

void BasicApp::openFile()
{
	mImage = loadImage( loadAsset( "mona.jpg" ) );

	mWidth = mImage.getWidth();
	mHeight = mImage.getHeight();

	dx::VboMesh::Layout layout;
	layout.setDynamicColorsRGB();
	layout.setDynamicPositions();
	mVboMesh = dx::VboMesh( mWidth * mHeight, 0, layout, D3D_PRIMITIVE_TOPOLOGY_POINTLIST );

	updateData( kColor );		
}

void BasicApp::mouseDown( MouseEvent event )
{
	mArcball.mouseDown( event.getPos() );
}

void BasicApp::mouseDrag( MouseEvent event )
{
	mArcball.mouseDrag( event.getPos() );
}

void BasicApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case 'R':
			updateData( kRed );
		break;
		case 'G':
			updateData( kGreen );
		break;
		case 'B':
			updateData( kBlue );
		break;
		case 'C':
			updateData( kColor );
		break;
	}
}

void BasicApp::updateData( BasicApp::ColorSwitch whichColor )
{
	Surface32f::Iter pixelIter = mImage.getIter();
	dx::VboMesh::VertexIter vertexIter( mVboMesh );

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

            vertexIter.setPosition( x, height * 60.0f, z );
			vertexIter.setColorRGB( color );
			++vertexIter;
		}
	}
}

void BasicApp::draw()
{
	dx::clear( Color( 0, 0, 0 ), true );

	dx::pushModelView();
	dx::translate( Vec3f( 800.0f, 500.0f, mHeight / 2.0f ) );
	dx::rotate( mArcball.getQuat() );
	if ( mVboMesh )
		dx::draw( mVboMesh );
	dx::popModelView();

	std::stringstream s;
	s << "Framerate:" << getAverageFps();
	dx::drawString(s.str(),Vec2f(10.0f,10.0f),Color::white(),mFont);
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )