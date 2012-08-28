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

	When creating your own stereoscopic application, be careful how you choose your focal length.
	An excellent article can be found here:
	http://paulbourke.net/miscellaneous/stereographics/stereorender/

	The CameraStereo class is based on the Off-Axis method described in this article. 
*/

#include "cinder/app/AppBasic.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"

#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Rand.h"
#include "cinder/TriMesh.h"
#include "cinder/ObjLoader.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class StereoscopicRenderingApp : public AppBasic {
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
	bool			mIsStereo;

	MayaCamUI		mMayaCam;
	CameraStereo	mCamera;

	gl::GlslProg	mShader;

	gl::VboMesh		mMesh;
	gl::VboMesh		mNote;

	void render();
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
	// enable stereoscopic rendering (press S to toggle)
	mIsStereo = true;

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
}

void StereoscopicRenderingApp::update()
{
	// auto-focus by calculating distance from object
	float d = mCamera.getEyePoint().length();
	float f = math<float>::min( 5.0f, d * 0.5f );

	// The setFocus() method will automatically calculate a fitting value eye separation.
	// If you want to specify your own values, use setFocalLength() and setEyeSeparation().
	mCamera.setFocus( f );
}

void StereoscopicRenderingApp::draw()
{
	// gray background
	gl::clear( Color(0.8f, 0.8f, 0.8f) ); 

	// enable 3D rendering
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::pushMatrices();

	// stereoscopic rendering
	if( mIsStereo ) {
		// find dimensions of each viewport (side by side)
		int w = getWindowWidth();
		int h = getWindowHeight();

		// store current viewport
		glPushAttrib( GL_VIEWPORT_BIT );

		// draw to left half of window only
		gl::setViewport( Area(0, 0, w / 2, h) );

		// activate left camera
		mCamera.enableStereoLeft();
		gl::setMatrices( mCamera );

		// render scene
		render();

		// draw to right half of window only
		gl::setViewport( Area(w / 2, 0, w, h) );

		// activate right camera
		mCamera.enableStereoRight();
		gl::setMatrices( mCamera );

		// render scene
		render();

		// restore viewport
		glPopAttrib();
	} 
	else {
		// activate mono camera
		mCamera.disableStereo();
		gl::setMatrices( mCamera );

		// render scene
		render();
	}

	// restore 2D
	gl::popMatrices();
	gl::disableDepthWrite();
	gl::disableDepthRead();
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
	case KeyEvent::KEY_s:
		// toggle stereoscopic rendering
		mIsStereo = !mIsStereo;
		break;
	case KeyEvent::KEY_v:
		// toggle vertical sync
		gl::enableVerticalSync( !gl::isVerticalSyncEnabled() );
		break;
	}
}

void StereoscopicRenderingApp::resize( ResizeEvent event )
{
	// make sure the camera's aspect ratio remains correct
	mCamera.setAspectRatio( event.getAspectRatio() );	
	mMayaCam.setCurrentCam( mCamera );
}

void StereoscopicRenderingApp::render()
{	
	float seconds = (float) getElapsedSeconds();

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
}

CINDER_APP_BASIC( StereoscopicRenderingApp, RendererGl )