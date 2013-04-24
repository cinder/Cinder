#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

#include "cinder/Camera.h"
#include "cinder/Font.h"
#include "cinder/MayaCamUI.h"

#include <boost/format.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

class CameraLensShiftTestApp : public AppBasic {
public:
	void prepareSettings( Settings *settings );

	void setup();
	void update();
	void draw();

	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );

	void keyDown( KeyEvent event );

	void resize();
private:
	void render();
private:
	CameraPersp			mCamera;
	MayaCamUI			mOverview;

	Font				mFont;

	Vec2f				mLensShift;
};

void CameraLensShiftTestApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize(1200, 450);
}

void CameraLensShiftTestApp::setup()
{
	CameraPersp cam( getWindowWidth() * 0.5f, getWindowHeight(), 40.0f, 0.1f, 1000.0f );
	cam.setEyePoint( Vec3f(-50.0f, 0.0f, 0.0f) );
	cam.setCenterOfInterestPoint( Vec3f::zero() );

	mOverview.setCurrentCam(cam);

	// set camera to size of half the window
	mCamera = CameraPersp( getWindowWidth() * 0.5f, getWindowHeight(), 40.0f, 2.5f, 20.0f );
	mCamera.setEyePoint( Vec3f(0.0f, 0.0f, -15.0f) );
	mCamera.setCenterOfInterestPoint( Vec3f::zero() );

	//
	mFont = Font("Tahoma", 18);

	mLensShift = mCamera.getLensShift();
}

void CameraLensShiftTestApp::update()
{
	// gradually shift to desired value
	Vec2f shift = mCamera.getLensShift();
	mCamera.setLensShift( shift + 0.1f * (mLensShift - shift) );
}

void CameraLensShiftTestApp::draw()
{
	gl::clear(); 

	gl::enableDepthRead();
	gl::enableDepthWrite();

	// draw the overview of the scene in the left half of the window
	glPushAttrib( GL_VIEWPORT_BIT );
	gl::setViewport( Area( getWindowWidth() * 0.0f, 0, getWindowWidth() * 0.5f, getWindowHeight() ) );

	gl::pushMatrices();
	gl::setMatrices( mOverview.getCamera() );

	render();
	
	gl::color( Color(0, 1, 1) );
	gl::drawFrustum( mCamera );

	gl::popMatrices();

	glPopAttrib();

	// draw what the camera sees in the right half of the window
	glPushAttrib( GL_VIEWPORT_BIT );
	gl::setViewport( Area( getWindowWidth() * 0.5f, 0, getWindowWidth() * 1.0f, getWindowHeight() ) );

	gl::pushMatrices();
	gl::setMatrices( mCamera );

	render();

	gl::popMatrices();

	glPopAttrib();
	
	//
	gl::disableDepthWrite();
	gl::disableDepthRead();

	// draw separator
	gl::color( Color(0.25f, 0.25f, 0.25f) );
	gl::drawLine( Vec2f( getWindowWidth() * 0.5f, 0.0f ),  Vec2f( getWindowWidth() * 0.5f, getWindowHeight() ) );

	// draw info
	gl::enableAlphaBlending();
	gl::drawString( 
		(boost::format("Lens Shift X: %02.2f\nLens Shift Y: %02.2f\n\nUse cursor keys to adjust lens shift,\nuse mouse to control overview camera") 
			% mCamera.getLensShiftHorizontal() % mCamera.getLensShiftVertical() ).str(),
		Vec2f( 10, 10 ), Color::white(), mFont );
	gl::drawString( "Overview of the scene",
		Vec2f( 10, getWindowHeight() - 28 ), Color::white(), mFont );
	gl::drawString( "View from the camera",
		Vec2f( 0.5f * getWindowWidth() + 10, getWindowHeight() - 28 ), Color::white(), mFont );
	gl::disableAlphaBlending();
}

void CameraLensShiftTestApp::mouseDown( MouseEvent event )
{
	mOverview.mouseDown( event.getPos() );
}

void CameraLensShiftTestApp::mouseDrag( MouseEvent event )
{
	mOverview.mouseDrag( event.getPos(), event.isLeftDown(), false, event.isRightDown() );
}

void CameraLensShiftTestApp::keyDown( KeyEvent event )
{
	switch( event.getCode() )
	{
	case KeyEvent::KEY_UP:
		mLensShift.y += 0.1f;
		break;
	case KeyEvent::KEY_DOWN:
		mLensShift.y -= 0.1f;
		break;
	case KeyEvent::KEY_LEFT:
		mLensShift.x -= 0.1f;
		break;
	case KeyEvent::KEY_RIGHT:
		mLensShift.x += 0.1f;
		break;
	}
}

void CameraLensShiftTestApp::resize()
{
	mCamera.setAspectRatio( event.getAspectRatio() * 0.5f );

	CameraPersp cam( mOverview.getCamera() );
	cam.setAspectRatio( event.getAspectRatio() * 0.5f );
	mOverview.setCurrentCam( cam );
}

void CameraLensShiftTestApp::render()
{
	// draw simple scene
	gl::color( Color(0.25f, 0.25f, 0.25f) );
	for(int i=-10; i<=10; ++i) {
		gl::drawLine( Vec3f((float) i, 0.0f, -10.0f), Vec3f((float) i, 0.0f, 10.0f) );
		gl::drawLine( Vec3f(-10.0f, 0.0f, (float) i), Vec3f(10.0f, 0.0f, (float) i) );
	}

	gl::color( Color(0.75f, 0.75f, 0.75f) );
	gl::enableWireframe();
	gl::pushModelView();
	gl::rotate( Vec3f::yAxis() * 5.0f * getElapsedSeconds() );
	gl::drawSphere( Vec3f::zero(), 4.0f, 24 );
	gl::popModelView();
	gl::disableWireframe();

}

CINDER_APP_BASIC( CameraLensShiftTestApp, RendererGl )
