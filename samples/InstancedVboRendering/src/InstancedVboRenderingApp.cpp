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

#include "cinder/ImageIo.h"
#include "cinder/MayaCamUI.h"
#include "cinder/ObjLoader.h"
#include "cinder/Rand.h"
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

#define NUM_INSTANCES		4800	
#define INSTANCES_PER_ROW	40

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
	// loads the hexagon mesh into a VBO using ObjLoader
	void loadMesh();
	// renders a help text to a Texture
	void renderHelp();
private:
	bool			mDrawInstanced;

	// our controlable camera
	MayaCamUI		mCamera;

	// an image, stored both as a Surface and a Texture
	Surface			mImageSurface;
	gl::Texture		mImageTexture;

	// we're using two shaders: a standard, non-instanced one and an instanced one
	gl::GlslProg	mShader;
	gl::GlslProg	mShaderInstanced;

	// VBO containing one hexagon mesh
	gl::VboMesh		mVboMesh;

	// our help text, rendered to a Texture
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
	cam.setEyePoint( Vec3f(50, 50, 60) );
	cam.setCenterOfInterestPoint( Vec3f(50, 50, 0) );
	cam.setFov( 60.0f );
	mCamera.setCurrentCam( cam );

	// load an image
	mImageSurface = Surface( loadImage( loadAsset("andrew_bell.jpg") ) );
	mImageTexture = gl::Texture( mImageSurface );

	// load shaders
	try { 
		mShader = gl::GlslProg( loadAsset("phong_vert.glsl"), loadAsset("phong_frag.glsl") ); 
		mShaderInstanced = gl::GlslProg( loadAsset("instanced_phong_vert.glsl"), loadAsset("phong_frag.glsl") ); 
	}
	catch( const std::exception &e ) { console() << e.what() << std::endl; }

	// load hexagon mesh
	loadMesh();

	// calculate FPS 4 times a second
	setFpsSampleInterval( 0.25 );
}

void InstancedVboRenderingApp::update()
{
	// update the help panel
	renderHelp();
}

void InstancedVboRenderingApp::draw()
{
	// clear the window
	gl::clear();

	// activate our camera
	gl::pushMatrices();
	gl::setMatrices( mCamera.getCamera() );

	// set render states
	gl::enable( GL_CULL_FACE );
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::color( Color::white() );

	if(mVboMesh ) 
	{
		if( mDrawInstanced && mShaderInstanced ) 
		{
			// bind the shader, which will do all the hard work for us
			mShaderInstanced.bind();
			mShaderInstanced.uniform( "INSTANCES_PER_ROW", float(INSTANCES_PER_ROW) );
			mShaderInstanced.uniform( "tex0", 0 );

			// bind image to texture unit 0
			mImageTexture.bind(0);

			// we do all positioning in the shader,
			// and therefor we only need a single draw call
			// to render all instances. It's fast,
			// but a little less flexible.
			gl::drawInstanced( mVboMesh, NUM_INSTANCES );

			// unbind texture and shader
			mImageTexture.unbind();
			mShaderInstanced.unbind();
		}
		else if( mShader )
		{
			// bind shader
			mShader.bind();

			// this is what we need to do if we draw each
			// instance separately. It's slower for static objects, 
			// because you need a separate draw call for each instance.
			// However, it's easier, more flexible and usually faster
			// for dynamic objects.
			for(int i=0;i<NUM_INSTANCES;i++) 
			{ 
				// determine position for this hexagon
				float x = math<float>::fmod( float(i), INSTANCES_PER_ROW );
				float y = math<float>::floor( float(i) / INSTANCES_PER_ROW );

				// sample color from image
				Vec2f pixel = Vec2f( x / INSTANCES_PER_ROW, 1.0f - y / (3 * INSTANCES_PER_ROW) ) * Vec2f( mImageSurface.getSize() );
				ColorA clr = mImageSurface.getPixel( Vec2i( pixel ) );

				// convert to luminance and calculate corresponding angle in radians
				float luminance = 0.3f * clr.r + 0.59f * clr.g + 0.11f * clr.b;
				float angle = math<float>::acos( luminance );

				// translate and rotate hexagon, then draw it
				gl::pushModelView();
				{
					gl::translate( 3.0f * x + 1.5f * math<float>::fmod( y, 2.0f ) , 0.866025f * y, 0.0f );
					gl::rotate( Vec3f::xAxis() * toDegrees( angle ) );

					gl::draw( mVboMesh );
				}
				gl::popModelView();
			}
			
			// unbind shader
			mShader.unbind();
		}

		// make sure our VBO is no longer bound
		mVboMesh.unbindBuffers();
	}			
	
	// reset render states
	gl::disableDepthWrite();
	gl::disableDepthRead();
	gl::disable( GL_CULL_FACE );

	// restore 2D drawing
	gl::popMatrices();

	// draw help
	if( mHelpTexture )
	{
		gl::enableAlphaBlending();
		gl::draw( mHelpTexture, Vec2f( 0.5f * getWindowWidth() - 200.0f, getWindowHeight() - 125.0f ) );
		gl::disableAlphaBlending();
	}
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
		mVboMesh = gl::VboMesh( mesh );
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
	bool wasVerticalSynced;

	switch( event.getCode() )
	{
	case KeyEvent::KEY_ESCAPE:
		quit();
		break;
	case KeyEvent::KEY_f:
		// toggle full screen while preserving vertical sync state
		wasVerticalSynced = gl::isVerticalSyncEnabled();
		setFullScreen( ! isFullScreen() );
		gl::enableVerticalSync( wasVerticalSynced );
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
