/*
 Copyright (c) 2012, Paul Houx
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

/*
	This sample will show how to use the CameraStereo class to setup and render stereoscopic images.
	The camera contains different matrices for the left and right eye of the viewer. By rendering the scene
	twice, once for each eye, we can view the scene in 3D on monitors or televisions that support 3D.

	Here, we divide the window into a left and right half and render the scene to each half. This is called
	side-by-side stereoscopic and is supported by most 3D televisions. Simply connect your computer to
	such a television, run the sample in full screen and enable the TV's 3D mode.

	When creating your own stereoscopic application, be careful how you choose your convergence.
	An excellent article can be found here:
	http://paulbourke.net/miscellaneous/stereographics/stereorender/

	The CameraStereo class is based on the Off-Axis method described in this article. 
*/

#include "cinder/app/AppBasic.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/StereoAutoFocuser.h"
#include "cinder/gl/Vbo.h"

#include "cinder/Camera.h"
#include "cinder/Font.h"
#include "cinder/ImageIo.h"
#include "cinder/MayaCamUI.h"
#include "cinder/ObjLoader.h"
#include "cinder/Rand.h"
#include "cinder/TriMesh.h"
#include "cinder/Utilities.h"

#include <boost/format.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

class StereoscopicRenderingApp : public AppBasic {
  public:
	typedef enum { SET_CONVERGENCE, SET_FOCUS, AUTO_FOCUS } FocusMethod;
	typedef enum { MONO, SIDE_BY_SIDE, OVER_UNDER, ANAGLYPH_RED_CYAN } RenderMethod;
public:
	void prepareSettings( Settings *settings );

	void setup();	
	void update();
	void draw();

	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );

	void keyDown( KeyEvent event );

	void resize( ResizeEvent event );
private:
	bool			mDrawAutoFocus;
	FocusMethod		mFocusMethod;
	RenderMethod	mRenderMethod;

	MayaCamUI		mMayaCam;
	CameraStereo	mCamera;

	gl::StereoAutoFocuser	mAF;

	gl::GlslProg	mShader;

	gl::VboMesh		mMesh;
	gl::VboMesh		mNote;

	gl::Fbo			mAnaglyphLeft;
	gl::Fbo			mAnaglyphRight;

	Color			mBackgroundColor;

	bool			mDrawUI;
	Font			mFont;

	void render();
	void renderUI();
};

void StereoscopicRenderingApp::prepareSettings( Settings *settings )
{
	// create a 16:9 window
	settings->setWindowSize(960, 540);
	settings->setTitle("Stereoscopic Rendering");

	// allow high frame rates to test performance
	settings->setFrameRate( 300.0f );
}

void StereoscopicRenderingApp::setup()
{
	// enable stereoscopic rendering
	mRenderMethod = SIDE_BY_SIDE;

	// enable auto-focussing
	mFocusMethod = AUTO_FOCUS;
	mDrawAutoFocus = false;

	// setup the camera
	mCamera.setEyePoint( Vec3f(0.2f, 1.3f, -11.5f) );
	mCamera.setCenterOfInterestPoint( Vec3f(0.5f, 1.5f, -0.1f) );
	mCamera.setFov( 60.0f );

	mMayaCam.setCurrentCam( mCamera );

	try {
		// load shader(s)
		mShader = gl::GlslProg( loadAsset("shaders/phong_vert.glsl"), loadAsset("shaders/phong_frag.glsl") );

		// load model(s)
		TriMesh		mesh;

		mesh.read( loadAsset("models/trombone.msh") );
		mMesh = gl::VboMesh( mesh );
		
		mesh.read( loadAsset("models/note.msh") );
		mNote = gl::VboMesh( mesh ); 
	}
	catch( const std::exception &e ) {
		// something went wrong, display error and quit
		console() << e.what() << std::endl;
		quit();
	}

	mBackgroundColor = Color( 0.8f, 0.8f, 0.8f );

	mFont = Font("Verdana", 24.0f);
	mDrawUI = true;
}

void StereoscopicRenderingApp::update()
{
	float	d, f;
	Area	area;

	switch( mFocusMethod )
	{
	case SET_CONVERGENCE:
		// auto-focus by calculating distance to center of interest
		d = (mCamera.getCenterOfInterestPoint() - mCamera.getEyePoint()).length();
		f = math<float>::min( 5.0f, d * 0.5f );

		// The setConvergence() method will not change the eye separation distance, 
		// which may cause the parallax effect to become uncomfortably big. 
		mCamera.setConvergence( f );
		mCamera.setEyeSeparation( 0.05f );
		break;
	case SET_FOCUS:
		// auto-focus by calculating distance to center of interest
		d = (mCamera.getCenterOfInterestPoint() - mCamera.getEyePoint()).length();
		f = math<float>::min( 5.0f, d * 0.5f );

		// The setConvergence( value, true ) method will automatically calculate a fitting value for the eye separation distance.
		// There is still no guarantee that the parallax effect stays within comfortable levels,
		// because there may be objects very near to the camera compared to the point we are looking at.
		mCamera.setConvergence( f, true );
		break;
	case AUTO_FOCUS:
		// Here, we use the gl::StereoAutoFocuser class to determine the best focal length,
		// based on the contents of the current depth buffer. This is by far the best method of
		// the three, because it guarantees the parallax effect will never be out of bounds.
		// Depending on the rendering method, we can sample different area's of the screen
		// to optimally detect details. This is not required, however.
		// Use the UP and DOWN keys to adjust the intensity of the parallax effect.
		switch( mRenderMethod ) 
		{
		case MONO:
			break;
		case SIDE_BY_SIDE:
			// sample half the left eye, half the right eye
			area = gl::getViewport();
			area.expand( -area.getWidth()/4, 0 );
			mAF.autoFocus( &mCamera, area );
			break;
		case OVER_UNDER:
			// sample half the left eye, half the right eye
			area = gl::getViewport();
			area.expand( 0, -area.getHeight()/4 );
			mAF.autoFocus( &mCamera, area );
			break;
		case ANAGLYPH_RED_CYAN:
			// sample the depth buffer of one of the FBO's
			mAF.autoFocus( &mCamera, mAnaglyphLeft );
			break;
		}
		break;
	}
}

void StereoscopicRenderingApp::draw()
{
	// find dimensions of each viewport 
	int w = getWindowWidth();
	int h = getWindowHeight();

	// clear color and depth buffers
	gl::clear( mBackgroundColor ); 
	
	// stereoscopic rendering
	switch( mRenderMethod ) 
	{
	case MONO:
		// render mono camera
		mCamera.disableStereo();
		render();
		break;
	case SIDE_BY_SIDE:
		// store current viewport
		glPushAttrib( GL_VIEWPORT_BIT );

		// draw to left half of window only
		gl::setViewport( Area(0, 0, w / 2, h) );

		// render left camera
		mCamera.enableStereoLeft();
		render();

		// draw to right half of window only
		gl::setViewport( Area(w / 2, 0, w, h) );

		// render right camera
		mCamera.enableStereoRight();
		render();

		// restore viewport
		glPopAttrib();
		break;
	case OVER_UNDER:
		// store current viewport
		glPushAttrib( GL_VIEWPORT_BIT );

		// draw to top half of window only
		gl::setViewport( Area(0, 0, w, h / 2) );

		// render left camera
		mCamera.enableStereoLeft();
		render();

		// draw to bottom half of window only
		gl::setViewport( Area(0, h / 2, w, h) );

		// render right camera
		mCamera.enableStereoRight();
		render();

		// restore viewport
		glPopAttrib();
		break;
	case ANAGLYPH_RED_CYAN:
		// store current viewport
		glPushAttrib( GL_VIEWPORT_BIT );

		// bind the left FBO and adjust the viewport to its bounds
		mAnaglyphLeft.bindFramebuffer();
		gl::setViewport( mAnaglyphLeft.getBounds() );

		// because glClear() does not respect the color mask, 
		// clear the color (and depth) buffers using a red filtered background color
		gl::clear( mBackgroundColor * Color( 1, 0, 0 ) );

		// set up color mask to only draw red and render left camera
		glColorMask( true, false, false, true );
		mCamera.enableStereoLeft();
		render();
		glColorMask( true, true, true, true );

		mAnaglyphLeft.unbindFramebuffer();
		
		// bind the right FBO and adjust the viewport to its bounds
		mAnaglyphRight.bindFramebuffer();
		gl::setViewport( mAnaglyphRight.getBounds() );
		
		// because glClear() does not respect the color mask, 
		// clear the color (and depth) buffers using a cyan filtered background color
		gl::clear( mBackgroundColor * Color( 0, 1, 1 ) );
		
		// set up color mask to only draw cyan and render right camera
		glColorMask( false, true, true, true );
		mCamera.enableStereoRight();
		render();
		glColorMask( true, true, true, true );

		mAnaglyphRight.unbindFramebuffer();

		// restore viewport
		glPopAttrib();

		// draw the FBO's on top of each other using a special additive blending operation
		gl::color( Color::white() );
		
		gl::draw( mAnaglyphLeft.getTexture(), Rectf( 0, (float) h, (float) w, 0 ) );	

		glEnable( GL_BLEND );
		glBlendFunc( GL_ONE, GL_ONE );
		gl::draw( mAnaglyphRight.getTexture(), Rectf( 0, (float) h, (float) w, 0) ); 
		glDisable( GL_BLEND );
		break;
	}

	// draw auto focus visualizer
	if( mDrawAutoFocus ) mAF.draw();
}

void StereoscopicRenderingApp::mouseDown( MouseEvent event )
{
	// handle camera
	mMayaCam.mouseDown( event.getPos() );
}

void StereoscopicRenderingApp::mouseDrag( MouseEvent event )
{
	// handle camera
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
	
	// update stereoscopic camera
	mCamera.setEyePoint( mMayaCam.getCamera().getEyePoint() );
	mCamera.setCenterOfInterestPoint( mMayaCam.getCamera().getCenterOfInterestPoint() );
}

void StereoscopicRenderingApp::keyDown( KeyEvent event )
{
	switch( event.getCode() )
	{
	case KeyEvent::KEY_ESCAPE:
		quit();
		break;
	case KeyEvent::KEY_f:
		// toggle full screen
		setFullScreen( ! isFullScreen() );
		break;
	case KeyEvent::KEY_v:
		// toggle vertical sync
		gl::enableVerticalSync( !gl::isVerticalSyncEnabled() );
		break;
	case KeyEvent::KEY_d:
		// toggle visualizer
		mDrawAutoFocus = !mDrawAutoFocus;
		break;
	case KeyEvent::KEY_u:
		// toggle interface
		mDrawUI = !mDrawUI;
		break;
	case KeyEvent::KEY_UP:
		// increase the parallax effect (towards negative parallax) 
		if(mFocusMethod == AUTO_FOCUS)
			mAF.setDepth( mAF.getDepth() + 0.01f );
		break;
	case KeyEvent::KEY_DOWN:
		// decrease the parallax effect (towards positive parallax) 
		if(mFocusMethod == AUTO_FOCUS)
			mAF.setDepth( mAF.getDepth() - 0.01f );
		break;
	case KeyEvent::KEY_SPACE:
		// reset the parallax effect to 'no parallax for the nearest object'
		mAF.setDepth( 1.0f );
		break;
	case KeyEvent::KEY_LEFT:
		// reduce the auto focus speed
		mAF.setSpeed( mAF.getSpeed() - 0.01f );
		break;
	case KeyEvent::KEY_RIGHT:
		// increase the auto focus speed
		mAF.setSpeed( mAF.getSpeed() + 0.01f );
		break;
	case KeyEvent::KEY_1:
		mFocusMethod = SET_CONVERGENCE;
		break;
	case KeyEvent::KEY_2:
		mFocusMethod = SET_FOCUS;
		break;
	case KeyEvent::KEY_3:
		mFocusMethod = AUTO_FOCUS;
		break;
	case KeyEvent::KEY_F1:
		mRenderMethod = MONO;
		break;
	case KeyEvent::KEY_F2:
		mRenderMethod = SIDE_BY_SIDE;
		break;
	case KeyEvent::KEY_F3:
		mRenderMethod = OVER_UNDER;
		break;
	case KeyEvent::KEY_F4:
		mRenderMethod = ANAGLYPH_RED_CYAN;
		break;
	}
}

void StereoscopicRenderingApp::resize( ResizeEvent event )
{
	// make sure the camera's aspect ratio remains correct
	mCamera.setAspectRatio( event.getAspectRatio() );	
	mMayaCam.setCurrentCam( mCamera );

	// create/resize the FBO's required for anaglyph rendering
	gl::Fbo::Format fmt;
	fmt.setMagFilter( GL_LINEAR );
	fmt.setSamples(8);

	mAnaglyphLeft = gl::Fbo( event.getWidth(), event.getHeight(), fmt );
	mAnaglyphRight = gl::Fbo( event.getWidth(), event.getHeight(), fmt );
}

void StereoscopicRenderingApp::render()
{	
	float seconds = (float) getElapsedSeconds();

	// enable 3D rendering
	gl::enableDepthRead();
	gl::enableDepthWrite();

	// set 3D camera matrices
	gl::pushMatrices();
	gl::setMatrices( mCamera );

	if( mShader && mMesh && mNote ) {
		// enable phong shading
		mShader.bind();	
		
		// draw trombone
		gl::pushModelView();
		{
			gl::color( Color(0.7f, 0.6f, 0.0f) );
			gl::rotate( Vec3f::yAxis() * 10.0f * seconds );
			gl::draw( mMesh );

			// reflection
			gl::scale( 1.0f, -1.0f, 1.0f );
			gl::draw( mMesh );
		}
		gl::popModelView();	

		// draw animated notes
		Rand rnd;
		for(int i=-100; i<=100; ++i) {
			rnd.seed(i);

			float t = rnd.nextFloat() * 200.0f + 2.0f * seconds;
			float r = rnd.nextFloat() * 360.0f + 60.0f * seconds;
			float z = fmodf( 5.0f * t, 200.0f ) - 100.0f;		

			gl::pushModelView();
			{
				gl::color( Color( CM_HSV, rnd.nextFloat(), 1.0f, 1.0f ) );

				gl::pushModelView();
				gl::translate( i * 0.5f, 0.15f + 1.0f * math<float>::abs( sinf(3.0f * t) ), -z );
				gl::rotate( Vec3f::yAxis() * r );
				gl::draw( mNote );
				gl::popModelView();
				
				// reflection
				gl::pushModelView();
				gl::scale( 1.0f, -1.0f, 1.0f );
				gl::translate( i * 0.5f, 0.15f + 1.0f * math<float>::abs( sinf(3.0f * t) ), -z );
				gl::rotate( Vec3f::yAxis() * r );
				gl::draw( mNote );
				gl::popModelView();
			}
			gl::popModelView();
		}

		mShader.unbind();
	}

	// draw grid
	gl::color( Color(0.8f, 0.8f, 0.8f) );
	for(int i=-100; i<=100; ++i) {
		gl::drawLine( Vec3f((float) i, 0, -100), Vec3f((float) i, 0, 100) );
		gl::drawLine( Vec3f(-100, 0, (float) i), Vec3f(100, 0, (float) i) );
	}

	// draw floor
	gl::enableAlphaBlending();
	gl::color( ColorA(1,1,1,0.75f) );
	gl::drawCube( Vec3f(0.0f, -0.5f, 0.0f), Vec3f(200.0f, 1.0f, 200.0f) );
	gl::disableAlphaBlending();

	// restore 2D rendering
	gl::popMatrices();
	gl::disableDepthWrite();
	gl::disableDepthRead();

	// render UI
	if( mDrawUI ) renderUI();
}

void StereoscopicRenderingApp::renderUI()
{   
    float w = (float) getWindowWidth() * 0.5f;
    float h = (float) getWindowHeight();

	std::string renderMode, focusMode;
	switch(mRenderMethod) {
		case MONO: renderMode = "Mono"; break;
		case SIDE_BY_SIDE: renderMode = "Side By Side"; break;
		case OVER_UNDER: renderMode = "Over Under"; break;
		case ANAGLYPH_RED_CYAN: renderMode = "Anaglyph Red Cyan"; break;
	}
	switch(mFocusMethod) {
		case SET_CONVERGENCE: focusMode = "CameraStereo::setConvergence(d, false);"; break;
		case SET_FOCUS: focusMode = "CameraStereo::setConvergence(d, true);"; break;
		case AUTO_FOCUS: focusMode = "gl::StereoAutoFocuser::autoFocus(cam);"; break;
	}

    std::string labels( "Render mode (F1-F4):\nFocus mode (1-3):\nFocal Length:\nEye Distance:\nAuto Focus Depth (Up/Down):\nAuto Focus Speed (Left/Right):" );
    boost::format values = boost::format( "%s\n%s\n%.2f\n%.2f\n%.2f\n%.2f" ) % renderMode % focusMode % mCamera.getConvergence() % mCamera.getEyeSeparation() % mAF.getDepth() % mAF.getSpeed();

#if(defined CINDER_MSW)
    gl::enableAlphaBlending();
    gl::drawString( labels, Vec2f( w - 350.0f, h - 150.0f ), Color::black(), mFont );
    gl::drawStringRight( values.str(), Vec2f( w + 350.0f, h - 150.0f ), Color::black(), mFont );
    gl::disableAlphaBlending();
#else
    // \n is not supported on the mac, so we draw separate strings
    std::vector< std::string > left, right;
	left = ci::split( labels, "\n", false );
	right = ci::split( values.str(), "\n", false );

    gl::enableAlphaBlending();
    for(size_t i=0;i<4;++i) {       
        gl::drawString( left[i], Vec2f( w - 350.0f, h - 150.0f + i * mFont.getSize() * 0.9f ), Color::black(), mFont );
        gl::drawStringRight( right[i], Vec2f( w + 350.0f, h - 150.0f + i * mFont.getSize() * 0.9f ), Color::black(), mFont );
    }
    gl::disableAlphaBlending();
#endif
}

CINDER_APP_BASIC( StereoscopicRenderingApp, RendererGl )