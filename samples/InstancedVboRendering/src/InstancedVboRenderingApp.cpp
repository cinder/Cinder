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
	// creates a Vertex Array Object containing 
	// a transform matrix for each instance
	void initializeBuffer();
	// renders a help text to a Texture
	void renderHelp();
private:
	bool			mDrawInstanced;

	// our controlable camera
	MayaCamUI		mCamera;

	// we're using two shaders: a standard, non-instanced one and an instanced one
	gl::GlslProg	mShader;
	gl::GlslProg	mShaderInstanced;

	// VBO containing one hexagon mesh
	gl::VboMesh		mVboMesh;
	// a VBO containing a list of matrices, 
	// that we can apply as a vertex shader attribute
	gl::Vbo			mBuffer;
	// a vertex array object that encapsulates our buffer
	GLuint			mVAO;

	// transform matrices for each instance we're rendering
	std::vector< Matrix44f >	mModelMatrices;

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
	Surface image( loadImage( loadAsset("andrew_bell.jpg") ) );

	// initialize transforms for every instance
	mModelMatrices.reserve( NUM_INSTANCES );

	for(size_t i=0;i<NUM_INSTANCES;++i)
	{
		// determine position for this hexagon
		float x = math<float>::fmod( float(i), INSTANCES_PER_ROW );
		float y = math<float>::floor( float(i) / INSTANCES_PER_ROW );

		// get pixel luminance from image and convert to angle
		Vec2f pixel = Vec2f( 1.0f - x / INSTANCES_PER_ROW, 1.0f - y / (3 * INSTANCES_PER_ROW) );
		Vec2i pos = pixel * Vec2f( image.getSize() );
		ColorA clr = image.getPixel( pos );
		float luminance = 0.3f * clr.r + 0.59f * clr.g + 0.11f * clr.b;
		float angle = math<float>::acos( luminance );

		// create transform matrix, then rotate and translate it
		Matrix44f model;
		model.translate( Vec3f( 3.0f * x + 1.5f * math<float>::fmod( y, 2.0f ) , 0.866025f * y, 0.0f ) );
		model.rotate( Vec3f::xAxis(), angle );
		mModelMatrices.push_back( model );
	}

	// load shaders
	try { 
		mShader = gl::GlslProg( loadAsset("phong_vert.glsl"), loadAsset("phong_frag.glsl") ); 
		mShaderInstanced = gl::GlslProg( loadAsset("instanced_phong_vert.glsl"), loadAsset("phong_frag.glsl") ); 
	}
	catch( const std::exception &e ) { console() << e.what() << std::endl; }

	// create a vertex array object 
	initializeBuffer();

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
		if( mDrawInstanced && mShaderInstanced && mBuffer ) 
		{
			// bind the shader, which will do all the hard work for us
			mShaderInstanced.bind();

			// bind the buffer containing the model matrix for each instance,
			// this will allow us to pass this information as a vertex shader attribute.
			// See: initializeBuffer()
			glBindVertexArray(mVAO);

			// we do all positioning in the shader,
			// and therefor we only need a single draw call
			// to render all instances. It's fast,
			// but a little less flexible.
			gl::drawInstanced( mVboMesh, NUM_INSTANCES );
			
			// unbind vertex array object containing our buffer
			glBindVertexArray(0);

			// unbind shader
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
				gl::pushModelView();
				gl::multModelView( mModelMatrices[i] );
				gl::draw( mVboMesh );
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

void InstancedVboRenderingApp::initializeBuffer()
{
	// To pass the model matrix for each instance, we will use
	// a custom vertex shader attribute. Using an attribute will
	// give us more freedom and more storage capacity than using
	// a uniform buffer, which often has a capacity limit of 64KB
	// or less, which is just enough for 1024 instances.
	//
	// Because we want the attribute to stay the same for all
	// vertices of an instance, we set the divisor to '1' instead
	// of '0', so the attribute will only change after all
	// vertices of the instance have been drawn.

	// retrieve attribute location from the shader
	// (note: make sure the shader is loaded and compiled before calling this function)
	GLint ulocation = mShaderInstanced.getAttribLocation( "model_matrix" );

	// if found...
	if( ulocation != -1 )
	{ 
		// create vertex array object to hold our buffer
		// (note: this is required for OpenGL 3.1 and above)
		glGenVertexArrays(1, &mVAO);   
		glBindVertexArray(mVAO);

		// create array buffer to store model matrices
		mBuffer = gl::Vbo( GL_ARRAY_BUFFER );

		// setup the buffer to contain space for all matrices.
		// we need 4 attributes to contain the 16 floats of a single matrix,
		// because the maximum size of an attribute is 4 floats (16 bytes).
		// When adding a 'mat4' attribute, OpenGL will make sure that 
		// the attribute locations are sequential, e.g.: 3, 4, 5 and 6
		mBuffer.bind();
		for (unsigned int i = 0; i < 4 ; i++) {
			glEnableVertexAttribArray(ulocation + i);
			glVertexAttribPointer( ulocation + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix44f), (const GLvoid*) (4 * sizeof(GLfloat) * i) );

			// get the next matrix after each instance, instead of each vertex
			glVertexAttribDivisor( ulocation + i, 1 );
		}

		// fill the buffer with our data
		mBuffer.bufferData( mModelMatrices.size() * sizeof(Matrix44f), &mModelMatrices.front(), GL_STATIC_READ );
		mBuffer.unbind();

		// unbind the VAO
		glBindVertexArray(0);
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
	case KeyEvent::KEY_f:	{
		// toggle full screen
		bool wasVerticalSynced = gl::isVerticalSyncEnabled();
		setFullScreen( ! isFullScreen() );
		gl::enableVerticalSync( wasVerticalSynced );

		// when switching to/from full screen, the buffer or shader might be lost
		initializeBuffer();	}
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
