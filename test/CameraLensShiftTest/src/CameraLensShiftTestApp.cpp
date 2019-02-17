#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Camera.h"
#include "cinder/Font.h"
#include "cinder/CameraUi.h"

#include <boost/format.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

class CameraLensShiftTestApp : public App {
public:
	static void prepareSettings( Settings *settings );

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
	CameraPersp			mOverviewCamera;
	CameraUi			mOverview;

	Font				mFont;

	vec2				mLensShift;
};

void CameraLensShiftTestApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize(1200, 450);
}

void CameraLensShiftTestApp::setup()
{
	mOverviewCamera = CameraPersp( getWindowWidth() * 0.5f, getWindowHeight(), 40.0f, 0.1f, 1000.0f );
	mOverviewCamera.setEyePoint( vec3(-50.0f, 0.0f, 0.0f) );
	mOverviewCamera.lookAt( vec3(0) );

	mOverview = CameraUi( &mOverviewCamera, getWindow() );
	mOverview.setWindowSize( getWindow()->getSize() );

	// set camera to size of half the window
	mCamera = CameraPersp( getWindowWidth() * 0.5f, getWindowHeight(), 40.0f, 2.5f, 20.0f );
	mCamera.setEyePoint( vec3(0.0f, 0.0f, -15.0f) );
	mCamera.lookAt( vec3(0) );

	//
	mFont = Font("Tahoma", 18);

	mLensShift = mCamera.getLensShift();
}

void CameraLensShiftTestApp::update()
{
	// gradually shift to desired value
	vec2 shift = mCamera.getLensShift();
	mCamera.setLensShift( shift + 0.1f * (mLensShift - shift) );
}

void CameraLensShiftTestApp::draw()
{
	gl::clear(); 

	gl::enableDepthRead();
	gl::enableDepthWrite();

	// draw the overview of the scene in the left half of the window
	gl::pushViewport();
	gl::viewport( ivec2(0, 0), ivec2(getWindowWidth() / 2, getWindowHeight()) );

	gl::pushMatrices();
	gl::setMatrices( mOverview.getCamera() );

	render();
	
	gl::color( Color(0, 1, 1) );
	gl::drawFrustum( mCamera );

	gl::popMatrices();

	gl::popViewport();

	// draw what the camera sees in the right half of the window
	gl::pushViewport();
	gl::viewport( ivec2(getWindowWidth() / 2, 0), ivec2(getWindowWidth() / 2, getWindowHeight()) );

	gl::pushMatrices();
	gl::setMatrices( mCamera );

	render();

	gl::popMatrices();

	gl::popViewport();
	
	//
	gl::disableDepthWrite();
	gl::disableDepthRead();

	// draw separator
	gl::color( Color(0.25f, 0.25f, 0.25f) );
	gl::drawLine( vec2( getWindowWidth() * 0.5f, 0.0f ),  vec2( getWindowWidth() * 0.5f, getWindowHeight() ) );

	// draw info
	gl::enableAlphaBlending();
	gl::drawString( 
		(boost::format("Lens Shift X: %02.2f\nLens Shift Y: %02.2f\n\nUse cursor keys to adjust lens shift,\nuse mouse to control overview camera") 
			% mCamera.getLensShiftHorizontal() % mCamera.getLensShiftVertical() ).str(),
		vec2( 10, 10 ), Color::white(), mFont );
	gl::drawString( "Overview of the scene",
		vec2( 10, getWindowHeight() - 28 ), Color::white(), mFont );
	gl::drawString( "View from the camera",
		vec2( 0.5f * getWindowWidth() + 10, getWindowHeight() - 28 ), Color::white(), mFont );
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
	mCamera.setAspectRatio( getWindow()->getAspectRatio() * 0.5f );

	mOverviewCamera.setAspectRatio( getWindow()->getAspectRatio() * 0.5f );
}

void CameraLensShiftTestApp::render()
{
	// draw simple scene
	gl::color( Color(0.25f, 0.25f, 0.25f) );
	for(int i=-10; i<=10; ++i) {
		gl::drawLine( vec3((float) i, 0.0f, -10.0f), vec3((float) i, 0.0f, 10.0f) );
		gl::drawLine( vec3(-10.0f, 0.0f, (float) i), vec3(10.0f, 0.0f, (float) i) );
	}

	gl::color( Color(0.75f, 0.75f, 0.75f) );
	gl::enableWireframe();
	gl::pushModelView();
	gl::rotate( 5.0f * getElapsedSeconds(), 0.0f, 1.0f, 0.0f );
	gl::drawSphere( vec3(0), 4.0f, 24 );
	gl::popModelView();
	gl::disableWireframe();

}

CINDER_APP( CameraLensShiftTestApp, RendererGl, &CameraLensShiftTestApp::prepareSettings )
