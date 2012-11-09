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
#include "cinder/TriMesh.h"
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
	
	void mouseMove( MouseEvent event );	
	void mouseDown( MouseEvent event );	
	void mouseDrag( MouseEvent event );	
	void mouseUp( MouseEvent event );	
	
	void keyDown( KeyEvent event );
	void keyUp( KeyEvent event );
private:
	void loadMesh();
private:
	bool			mDrawInstanced;

	MayaCamUI		mCamera;

	gl::GlslProg	mShader;
	gl::VboMesh		mHexagon;
};

void InstancedVboRenderingApp::prepareSettings(Settings *settings)
{
	settings->setTitle("Instanced VBO Drawing Sample");
	settings->setFrameRate( 500.0f );
}

void InstancedVboRenderingApp::setup()
{
	CameraPersp	cam;
	cam.setEyePoint( Vec3f(60, 70, 20) );
	cam.setCenterOfInterestPoint( Vec3f(70, 65, 0) );
	cam.setFov( 60.0f );
	mCamera.setCurrentCam( cam );

	try { 		
		//mShader = gl::GlslProg( 
		//	loadAsset("wireframe_vert.glsl"), loadAsset("wireframe_frag.glsl"), loadAsset("wireframe_geom.glsl"), GL_TRIANGLES, GL_TRIANGLES, 3
		//); 
				
		mShader = gl::GlslProg( loadAsset("phong_vert.glsl"), loadAsset("phong_frag.glsl") ); 
	}
	catch( const std::exception &e ) {
		console() << e.what() << std::endl;
	}

	loadMesh();

	mDrawInstanced = true;
}

void InstancedVboRenderingApp::update()
{
}

void InstancedVboRenderingApp::draw()
{
	// the model_matrix will animate each hexagon by rotating it a bit (same for all hexagons)
	Matrix44f model_matrix; //model_matrix *= Quatf( Vec3f::xAxis(), 0.2f * float( sin( 0.5 * getElapsedSeconds() ) ) );

	// the offset_matrix tells the shader how to position each instance relative to the others ( 3.0 * x + 1.5 * mod(y, 2), 0.866025 * y, 0.0 )
	const Matrix44f offset_matrix( 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.866025f, 0.0f, 0.0f, 1.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false );

	//
	gl::clear();

	gl::pushMatrices();
	gl::setMatrices( mCamera.getCamera() );

	gl::enable( GL_CULL_FACE );
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();

	if(mHexagon && mShader ) {
		// bind the shader, which will do all the hard work for us
		mShader.bind();
		mShader.uniform( "offset_matrix", offset_matrix );
		mShader.uniform( "viewport_size", Vec2f( getWindowSize() ) );

		gl::color( Color::white() );


		if( mDrawInstanced ) 
		{
			gl::pushModelView();
			gl::multModelView( model_matrix );
			gl::draw( mHexagon, 10000 );
			gl::popModelView();
		}
		else
		{
			for(int x=0;x<100;x++) 
			{ 
				for(int y=0;y<100;y++) {
					Vec3f p( float(x), float(y), math<float>::fmod( float(y), 2.0f ) );

					gl::pushModelView();
					gl::translate( offset_matrix.transformPointAffine(p) ); 
					gl::multModelView( model_matrix );	
					gl::draw( mHexagon );
					gl::popModelView();
				}
			}
		}
		mShader.unbind();
	}
	
	gl::disableDepthWrite();
	gl::disableDepthRead();
	gl::disableAlphaBlending();
	gl::disable( GL_CULL_FACE );

	gl::popMatrices();
}

void InstancedVboRenderingApp::resize( ResizeEvent event )
{
	CameraPersp cam = mCamera.getCamera();
	cam.setAspectRatio( event.getAspectRatio() );
	mCamera.setCurrentCam( cam );
}

void InstancedVboRenderingApp::loadMesh()
{
	ObjLoader	loader( loadAsset("hexagon_smooth.obj") );
	TriMesh		mesh;

	try {
		loader.load( &mesh, true, false, false );
		//mesh.recalculateNormals();
		mHexagon = gl::VboMesh( mesh );
	}
	catch( const std::exception &e ) {
		console() << e.what() << std::endl;
	}
}

void InstancedVboRenderingApp::mouseMove( MouseEvent event )
{
}

void InstancedVboRenderingApp::mouseDown( MouseEvent event )
{
	mCamera.mouseDown( event.getPos() );
}

void InstancedVboRenderingApp::mouseDrag( MouseEvent event )
{
	mCamera.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void InstancedVboRenderingApp::mouseUp( MouseEvent event )
{
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

void InstancedVboRenderingApp::keyUp( KeyEvent event )
{
}

CINDER_APP_BASIC( InstancedVboRenderingApp, RendererGl )
