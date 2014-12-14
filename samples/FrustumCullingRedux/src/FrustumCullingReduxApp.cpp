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

#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererGl.h"

#include "cinder/AxisAlignedBox.h"
#include "cinder/Frustum.h"
#include "cinder/MayaCamUI.h"
#include "cinder/ObjLoader.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"

#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Context.h"

#include "CullableObject.h"

const int NUM_OBJECTS = 1500;

using namespace ci;
using namespace ci::app;
using namespace std;

class FrustumCullingReduxApp : public AppBasic 
{
  public:
	void prepareSettings( Settings *settings );
	void setup();
	void update();
	void draw();
	
	void toggleCullableFov();
	void drawCullableFov();

	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );

	void keyDown( KeyEvent event );
	
  protected:
	//! load the heart shaped mesh 
	void			loadObject();
	//! draws a grid to visualize the ground plane
	void			drawGrid( float size = 100, float step = 10 );
	//! toggles vertical sync on both Windows and Macs
	void			toggleVerticalSync();
	//! renders the help text
	void			renderHelpToTexture();
  
  protected:

	// keep track of time
	double			mCurrentSeconds;

	// flags
	bool			mPerformCulling;
	bool			mDrawEstimatedBoundingBoxes;
	bool			mDrawPreciseBoundingBoxes;
	bool			mShowRevealingFov;
	bool			mShowHelp;

	Anim<float>		mCullingFov;

	ci::TriMeshRef		mTriMesh;
	gl::BatchRef		mBatch;

	// caches the heart's bounding box in object space coordinates
	AxisAlignedBox3f	mObjectBoundingBox;

	// render assets
	gl::GlslProgRef		mShader;

	// objects
	std::list<CullableObjectRef>	mObjects;

	// camera
	MayaCamUI		mMayaCam;
	CameraPersp		mRenderCam;

	// help text
	gl::Texture2dRef	mHelp;
};

void FrustumCullingReduxApp::prepareSettings(Settings *settings)
{
	// setup our window
	settings->setWindowSize( 1200, 675 );
	settings->setTitle( "Frustum Culling Redux" );
	// set the frame rate to something very high, so we can easily see the effect frustum culling has on performance.
	// Disable vertical sync to achieve the actual frame rate.
	settings->setFrameRate( 300 );
}

void FrustumCullingReduxApp::setup()
{
	// Intialize settings
	mPerformCulling = true;
	mDrawEstimatedBoundingBoxes = false;
	mDrawPreciseBoundingBoxes = false;
	mShowRevealingFov = false;
	mShowHelp = true;

	//! Render help texture
	renderHelpToTexture();

	//! Load and compile shader
	try {
		mShader = gl::GlslProg::create( loadAsset( "shaders/phong.vert" ), loadAsset( "shaders/phong.frag" ) );
	}
	catch( const std::exception &e ) {
		app::console() << "Could not load and compile shader:" << e.what() << std::endl;
	}

	// Setup material uniforms
	// We set it up once because all of our objects will share the same values.
	mShader->uniform( "uDiffuse", Color( 1, 0, 0 ) );
	mShader->uniform( "uSpecular", Color( 1, 1, 1 ) );
	mShader->uniform( "uShininess", 50.0f );

	// load assets
	loadObject();

	// create a few hearts
	Rand::randomize();
	mObjects.resize( NUM_OBJECTS );
	for( auto & obj : mObjects ) {
		vec3 pos( Rand::randFloat( -2000, 2000 ), 0, Rand::randFloat( -2000, 2000 ) );
		vec3 rot( 0, Rand::randFloat( -360, 360 ), 0 );
		vec3 scale( 50 );

		obj = CullableObjectRef( new CullableObject( mBatch ) );
		obj->setTransform( pos, rot, scale );
	}

	// setup cameras
	mCullingFov = 60;
	mRenderCam.setPerspective( mCullingFov, getWindowAspectRatio(), 10, 10000 );
	mRenderCam.lookAt( vec3( 200 ), vec3( 0 ) );
	
	mMayaCam.setCurrentCam( mRenderCam );

	// track current time so we can calculate elapsed time
	mCurrentSeconds = getElapsedSeconds();
}

void FrustumCullingReduxApp::update()
{
	// calculate elapsed time
	double elapsed = getElapsedSeconds() - mCurrentSeconds;
	mCurrentSeconds += elapsed;

	// perform frustum culling **********************************************************************************

	// Save the current culling field of view. If mShowRevealingFov = true, this will narrow the cam's FOV so that you can see the culling effect
	float originalFov = mRenderCam.getFov();
	mRenderCam.setFov( mCullingFov );
	Frustumf visibleWorld( mRenderCam );

	for( auto & obj : mObjects ) {
		// update object (so it rotates slowly around its axis)
		obj->update(elapsed);

		if( mPerformCulling ) {
			// create a fast approximation of the world space bounding box by transforming the
			// eight corners of the object space bounding box and using them to create a new axis aligned bounding box 
			AxisAlignedBox3f worldBoundingBox = mObjectBoundingBox.transformed( obj->getTransform() );

			// check if the bounding box intersects the visible world
			
			obj->setCulled( ! visibleWorld.intersects( worldBoundingBox ) );
		}
		else {
			obj->setCulled( false );
		}
	}

	// Set FOV to original
	mRenderCam.setFov( originalFov );
}

void FrustumCullingReduxApp::draw()
{
	// clear the window
	gl::clear( Color::gray( 0.25f ) );
	
	{
		// setup camera
		gl::ScopedMatrices scopeMatrix;
		gl::setMatrices( mRenderCam );
		
		gl::ScopedState scopeState( GL_CULL_FACE, true );
		gl::enableDepthRead();
		gl::enableDepthWrite();

		{
			gl::ScopedGlslProg scopeGlsl( mShader );
			for( auto & obj : mObjects )
				obj->draw();
		}
		
		AxisAlignedBox3f worldBoundingBox;
		for( auto & obj : mObjects ) {
			if( mDrawEstimatedBoundingBoxes ) {
				// create a fast approximation of the world space bounding box by transforming the
				// eight corners and using them to create a new axis aligned bounding box
				worldBoundingBox = mObjectBoundingBox.transformed( obj->getTransform() );
				
				if( ! obj->isCulled() )
					gl::color( Color( 0, 1, 1 ) );
				else
					gl::color( Color( 1, 0.5f, 0 ) );
				
				gl::drawStrokedCube( worldBoundingBox );
			}
			
			if( mDrawPreciseBoundingBoxes && ! obj->isCulled() ) {
				// you can see how much the approximated bounding boxes differ
				// from the precise ones by enabling this code
				worldBoundingBox = mTriMesh->calcBoundingBox( obj->getTransform() );
				gl::color( Color(1, 1, 0) );
				gl::drawStrokedCube( worldBoundingBox );
			}
		}

		// disable writing to depth buffer before drawing lines so they don't flicker with geometry
		gl::disableDepthWrite();

		drawGrid( 2000, 25 );
		gl::drawCoordinateFrame( 100, 8, 3 );
		drawCullableFov();

		// disable depth testing
		gl::disableDepthRead();
	}
	
	// render help
	if( mShowHelp && mHelp ) {
		gl::enableAlphaBlending();
		gl::color( Color::white() );
		gl::draw( mHelp );
		gl::disableAlphaBlending();
	}
}

void FrustumCullingReduxApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void FrustumCullingReduxApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
	mRenderCam = mMayaCam.getCamera();
}

void FrustumCullingReduxApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
		case KeyEvent::KEY_ESCAPE:
			quit();
		break;
		case KeyEvent::KEY_b:
			if(event.isShiftDown())
				mDrawPreciseBoundingBoxes = ! mDrawPreciseBoundingBoxes;
			else
				mDrawEstimatedBoundingBoxes = ! mDrawEstimatedBoundingBoxes;
		break;
		case KeyEvent::KEY_c:
			mPerformCulling = !mPerformCulling;
		break;
		case KeyEvent::KEY_f: {
			bool verticalSyncEnabled = gl::isVerticalSyncEnabled();
			setFullScreen( ! isFullScreen() );
			gl::enableVerticalSync( verticalSyncEnabled );
		}
		break;
		case KeyEvent::KEY_h:
			mShowHelp = !mShowHelp;
		break;
		case KeyEvent::KEY_v:
			toggleVerticalSync();
		break;
		case KeyEvent::KEY_SPACE:
			toggleCullableFov();
		break;
	}
	
	// update info
	renderHelpToTexture();
}

void FrustumCullingReduxApp::toggleCullableFov()
{
	// when this is on, it reduces the field of view for culling only, so that you can see the culling in action.
	mShowRevealingFov = ! mShowRevealingFov;
	float fov = mShowRevealingFov ? 45 : 60;

	timeline().apply( &mCullingFov, fov, 0.6f, EaseInOutCubic() );
}

void FrustumCullingReduxApp::drawCullableFov()
{
	// Reset the field of view once again to visualize
	// the actual window we're using to cull with.
	gl::color( Color( 1, 1, 1 ) );
	gl::clear( GL_DEPTH_BUFFER_BIT );
	
	// This visualizes the camera's viewport change for the culling field of view.
	float cullable = mCullingFov / 60.0f;
	
	gl::ScopedMatrices scopeMat;
	gl::setMatricesWindow( getWindowSize() );
	gl::translate( getWindowCenter() );
	gl::scale( vec3( cullable * getWindowCenter().x, cullable * getWindowCenter().y, 1 ) );
	gl::drawStrokedRect( Rectf( -1, -1, 1, 1 ));
}

void FrustumCullingReduxApp::loadObject()
{
	// We first convert our obj to a trimesh
	mTriMesh = TriMesh::create( ObjLoader( loadAsset( "models/heart.obj" ) ) );

	// Then use our trimesh and our shader to create a batch
	mBatch = gl::Batch::create( *mTriMesh, mShader );

	// We then use trimesh to calculate the Bounding Box
	mObjectBoundingBox = mTriMesh->calcBoundingBox();
}

void FrustumCullingReduxApp::drawGrid(float size, float step)
{
	gl::color( Colorf( 0.2f, 0.2f, 0.2f ) );
	gl::begin( GL_LINES );
	for( float i = -size; i <= size; i += step ) {
		gl::vertex( vec3( i, 0, -size ) );
		gl::vertex( vec3( i, 0,  size ) );
		gl::vertex( vec3( -size, 0, i ) );
		gl::vertex( vec3(  size, 0, i ) );
	}
	gl::end();
}

void FrustumCullingReduxApp::toggleVerticalSync()
{
	gl::enableVerticalSync( ! gl::isVerticalSyncEnabled() );
}

void FrustumCullingReduxApp::renderHelpToTexture()
{
	TextLayout layout;
	layout.setFont( Font( "Arial", 18 ) );
	layout.setColor( ColorA( 1, 1, 1, 1 ) );
	layout.setLeadingOffset( 3 );

	layout.clear( ColorA::gray( 0.2f, 0.5f ) );

	if( mPerformCulling )
		layout.addLine( "(C) Toggle culling (currently ON)" );
	else
		layout.addLine( "(C) Toggle culling (currently OFF)" );

	if( mDrawEstimatedBoundingBoxes )
		layout.addLine( "(B) Toggle estimated bounding boxes (currently ON)" );
	else
		layout.addLine( "(B) Toggle estimated bounding boxes (currently OFF)" );

	if( mDrawPreciseBoundingBoxes )
		layout.addLine( "(B)+(Shift) Toggle precise bounding boxes (currently ON)" );
	else
		layout.addLine( "(B)+(Shift) Toggle precise bounding boxes (currently OFF)" );

	if( mShowRevealingFov )
		layout.addLine("(Space) Toggle reveal culling (currently ON)");
	else
		layout.addLine("(Space) Toggle reveal culling (currently OFF)");

	if( gl::isVerticalSyncEnabled() )
		layout.addLine( "(V) Toggle vertical sync (currently ON)" );
	else
		layout.addLine( "(V) Toggle vertical sync (currently OFF)" );

	layout.addLine( "(H) Toggle this help panel" );

	mHelp = gl::Texture::create( layout.render( true, false ) );
}

CINDER_APP_BASIC( FrustumCullingReduxApp, RendererGl )