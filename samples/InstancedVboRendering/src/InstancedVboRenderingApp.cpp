/*
 Copyright (c) 2010-2012, Paul Houx - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/MayaCamUI.h"
#include "cinder/ObjLoader.h"
#include "cinder/Text.h"
#include "cinder/TriMesh.h"
#include "cinder/Utilities.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class InstancedVboRenderingApp : public AppBasic {
public:
	void prepareSettings( Settings *settings );
	
	void setup();
	void update();
	void draw();
	
	void resize( ResizeEvent event );
	
	void mouseDown( MouseEvent event );	
	void mouseDrag( MouseEvent event );
	
	void keyDown( KeyEvent event );
private:
	void loadMesh();
	void renderHelp();
private:
	bool			mDrawInstanced;

	MayaCamUI		mCamera;

	gl::GlslProg	mShader;
	gl::VboMesh		mHexagon;

	gl::Texture		mHelpTexture;
};

void InstancedVboRenderingApp::prepareSettings(Settings *settings)
{
	settings->setTitle("Instanced VBO Drawing Sample");
	settings->setFrameRate( 500.0f );
}

void InstancedVboRenderingApp::setup()
{
	mDrawInstanced = true;

	// disable vertical sync, so we can see the impact
	// of instanced rendering on the frame rate
	gl::disableVerticalSync();

	// initialize camera
	CameraPersp	cam;
	cam.setEyePoint( Vec3f(60, 70, 20) );
	cam.setCenterOfInterestPoint( Vec3f(70, 65, 0) );
	cam.setFov( 60.0f );
	mCamera.setCurrentCam( cam );

	// load shader
	try { mShader = gl::GlslProg( loadAsset("phong_vert.glsl"), loadAsset("phong_frag.glsl") ); }
	catch( const std::exception &e ) { console() << e.what() << std::endl; }

	// load hexagon mesh
	loadMesh();
}

void InstancedVboRenderingApp::update()
{
	// update the help panel
	renderHelp();
}

void InstancedVboRenderingApp::draw()
{
	// the offset_matrix instructs the shader how to position 
	// each hexagon relative to the others. The position of each
	// hexagon is defined as ( 3.0 * x + 1.5 * mod(y, 2), 0.866025 * y, z )
	const Matrix44f offset_matrix( 
		3.0f, 0.0f, 0.0f, 1.5f, 
		0.0f, 0.866025f, 0.0f, 0.0f, 
		0.0f, 0.0f, 1.0f, 0.0f, 
		0.0f, 0.0f, 0.0f, 0.0f, true );

	// clear the window
	gl::clear();

	// activate our camera
	gl::pushMatrices();
	gl::setMatrices( mCamera.getCamera() );

	// set render states
	gl::enable( GL_CULL_FACE );
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::color( Color::white() );

	if(mHexagon && mShader ) {
		// bind the shader, which will do all the hard work for us
		mShader.bind();
		mShader.uniform( "offset_matrix", offset_matrix );
		mShader.uniform( "row_size", 50.0f );

		if( mDrawInstanced ) 
		{
			// we do all positioning in the shader,
			// and therefor we only need a single draw call
			// to render 10000 instances. It's fast,
			// but a little less flexible.
			gl::drawInstanced( mHexagon, 10000 );
		}
		else
		{
			// this is what we need to do if we draw each 
			// instance separately. It's slower, because you
			// need a separate draw call for each instance.
			// However, it's easier and more flexible.
			for(int x=0;x<50;x++) 
			{ 
				for(int y=0;y<200;y++) 
				{
					Vec4f p = offset_matrix * Vec4f( float(x), float(y), 0.0f, math<float>::fmod( float(y), 2.0f ) );

					gl::pushModelView();
					gl::translate( p.x, p.y, p.z ); 
					gl::draw( mHexagon );
					gl::popModelView();
				}
			}
		}

		// unbind the shader
		mShader.unbind();
	}
	
	// reset render states
	gl::disableDepthWrite();
	gl::disableDepthRead();
	gl::disableAlphaBlending();
	gl::disable( GL_CULL_FACE );

	// restore 2D drawing
	gl::popMatrices();

	// draw help
	gl::enableAlphaBlending();
	gl::draw( mHelpTexture, Vec2f( 0.5f * getWindowWidth() - 200.0f, getWindowHeight() - 125.0f ) );
	gl::disableAlphaBlending();
}

void InstancedVboRenderingApp::resize( ResizeEvent event )
{
	// adjust the camera aspect ratio
	CameraPersp cam = mCamera.getCamera();
	cam.setAspectRatio( event.getAspectRatio() );
	mCamera.setCurrentCam( cam );
}

void InstancedVboRenderingApp::loadMesh()
{
	ObjLoader	loader( loadAsset("hexagon.obj") );
	TriMesh		mesh;

	try {
		loader.load( &mesh, true, false, false );
		mHexagon = gl::VboMesh( mesh );
	}
	catch( const std::exception &e ) {
		console() << e.what() << std::endl;
	}
}

void InstancedVboRenderingApp::renderHelp()
{
	TextBox textbox = TextBox().font( Font( "Verdana", 20.0f ) ).backgroundColor( ColorA(0, 0, 0, 0.5f) ).size(400, 100).alignment( TextBox::CENTER );

	textbox.appendText( "Average frame rate: " + toString( int( getAverageFps() + 0.5f ) ) + "\n\n" );

	if( gl::isVerticalSyncEnabled() )
		textbox.appendText( "(V) Toggle Vertical Sync (currently ON)\n" );
	else
		textbox.appendText( "(V) Toggle Vertical Sync (currently OFF)\n" );

	if( mDrawInstanced )
		textbox.appendText( "(I) Toggle Instanced Rendering (currently ON)\n" );
	else
		textbox.appendText( "(I) Toggle Instanced Rendering (currently OFF)\n" );

	mHelpTexture = textbox.render();
}

void InstancedVboRenderingApp::mouseDown( MouseEvent event )
{
	mCamera.mouseDown( event.getPos() );
}

void InstancedVboRenderingApp::mouseDrag( MouseEvent event )
{
	mCamera.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void InstancedVboRenderingApp::keyDown( KeyEvent event )
{
	switch( event.getCode() )
	{
	case KeyEvent::KEY_ESCAPE:
		quit();
		break;
	case KeyEvent::KEY_f:
		setFullScreen( ! isFullScreen() );
		break;
	case KeyEvent::KEY_i:
		mDrawInstanced = !mDrawInstanced;
		break;
	case KeyEvent::KEY_v:
		gl::enableVerticalSync( ! gl::isVerticalSyncEnabled() );
		break;
	}
}

CINDER_APP_BASIC( InstancedVboRenderingApp, RendererGl )
