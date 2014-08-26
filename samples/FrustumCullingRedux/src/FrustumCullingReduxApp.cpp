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

#include "cinder/AxisAlignedBox.h"	
#include "cinder/Filesystem.h"
#include "cinder/Frustum.h"
#include "cinder/ImageIo.h"
#include "cinder/MayaCamUI.h"
#include "cinder/ObjLoader.h"
#include "cinder/Rand.h"
#include "cinder/Text.h"
#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Context.h"

#include "CullableObject.h"

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

	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );

	void keyDown( KeyEvent event );
	
  protected:
	//! load the heart shaped mesh 
	void			loadObject();
	//! draws a grid to visualize the ground plane
	void			drawGrid(float size=100.0f, float step=10.0f);
	//! toggles vertical sync on both Windows and Macs
	void			toggleVerticalSync();
	//! renders the help text
	void			renderHelpToTexture();
  
  protected:
	static const int NUM_OBJECTS = 1500;

	//! keep track of time
	double			mCurrentSeconds;

	//! flags
	bool			bPerformCulling;
	bool			bMoveCamera;
	bool			bDrawEstimatedBoundingBoxes;
	bool			bDrawPreciseBoundingBoxes;
	bool			bShowHelp;

	ci::TriMeshRef		mTriMesh;
	gl::BatchRef		mBatch;

	//! caches the heart's bounding box in object space coordinates
	AxisAlignedBox3f	mObjectBoundingBox;

	//! render assets
	gl::GlslProgRef		mShader;

	//! objects
	std::list<CullableObjectRef>	mObjects;
	std::vector<CullableObjectRef>	mPotentiallyVisibleSet;

	//! camera
	MayaCamUI		mMayaCam;
	CameraPersp		mRenderCam;

	//! help text
	gl::Texture2dRef	mHelp;
};

void FrustumCullingReduxApp::prepareSettings(Settings *settings)
{
	//! setup our window
	settings->setWindowSize(1200, 675);
	settings->setTitle("Frustum Culling Redux");
	//! set the frame rate to something very high, so we can
	//! easily see the effect frustum culling has on performance.
	//! Disable vertical sync to achieve the actual frame rate.
	settings->setFrameRate(300);
}

void FrustumCullingReduxApp::setup()
{
	//! intialize settings
	bPerformCulling = true;
	bMoveCamera = true;
	bDrawEstimatedBoundingBoxes = false;
	bDrawPreciseBoundingBoxes = false;
	bShowHelp = true;

	//! render help texture
	renderHelpToTexture();
	
	//! load and compile shader
	try {
		mShader = gl::GlslProg::create( loadAsset("shaders/phong.vert"), loadAsset("shaders/phong.frag") );
	}
	catch( const std::exception &e ) {
		app::console() << "Could not load and compile shader:" << e.what() << std::endl;
	}
	
	//! setup material
	//  we set it up once because all of our objects will
	//  share the same values
	mShader->uniform( "cAmbient", Color(0.1f, 0.1f, 0.1f) );
	mShader->uniform( "cDiffuse", Color(1.0f, 0.0f, 0.0f) );
	mShader->uniform( "cSpecular", Color(1.0f, 1.0f, 1.0f) );
	mShader->uniform( "shininess", 50.0f );

	//! load assets
	loadObject();

	//! create a few hearts
	Rand::randomize();
	mObjects.resize( NUM_OBJECTS );
	for( auto & objIt : mObjects ) {
		vec3 p( Rand::randFloat(-2000.0f, 2000.0f), 0.0f, Rand::randFloat(-2000.0f, 2000.0f) );
		vec3 r( 0.0f, Rand::randFloat(-360.0f, 360.0f), 0.0f );
		vec3 s( 50.10f, 50.10f, 50.10f );

		objIt = CullableObjectRef( new CullableObject( mBatch ) );
		objIt->setTransform( p, r, s );
	}

	//! setup cameras
	mRenderCam.setPerspective( 60.0f, getWindowAspectRatio(), .01f, 10000.0f );
	mRenderCam.lookAt( vec3( 200.0f ), vec3( 0.0f ) );
	
	mMayaCam.setCurrentCam( mRenderCam );

	//! track current time so we can calculate elapsed time
	mCurrentSeconds = getElapsedSeconds();
}

void FrustumCullingReduxApp::update()
{
	// clear vector
	vector<CullableObjectRef>().swap(mPotentiallyVisibleSet);
	
	//! calculate elapsed time
	double elapsed = getElapsedSeconds() - mCurrentSeconds;
	mCurrentSeconds += elapsed;

	//! perform frustum culling **********************************************************************************
	// Set the frustum to a lower field of view angle to show the culling
	mRenderCam.setFov( 30.0f );
	Frustumf visibleWorld( mRenderCam );

	for( auto & objIt : mObjects ) {
		// update object (so it rotates slowly around its axis)
		objIt->update(elapsed);

		if(bPerformCulling) {
			// create a fast approximation of the world space bounding box by transforming the
			// eight corners of the object space bounding box and using them to create a new axis aligned bounding box 
			AxisAlignedBox3f worldBoundingBox = mObjectBoundingBox.transformed( objIt->getTransform() );

			// check if the bounding box intersects the visible world
			if( visibleWorld.intersects( worldBoundingBox ) ) {
				objIt->setCulled( false );
				mPotentiallyVisibleSet.push_back( objIt );
			}
			else
				objIt->setCulled( true );
		}
		else {
			objIt->setCulled( false );
			mPotentiallyVisibleSet.push_back( objIt );
		}
	}
	// Must reset the culling window so that we can see the effect.
	mRenderCam.setFov( 60.0f );
	//! **********************************************************************************************************
	
	auto eye = mRenderCam.getEyePoint();
	// Depth Sort the potentially visible set
	std::sort( mPotentiallyVisibleSet.begin(), mPotentiallyVisibleSet.end(),
			  [&, eye]( const CullableObjectRef & objA, const CullableObjectRef & objB ) {
				  auto distA = ci::distance( eye, objA->getPosition() );
				  auto distB = ci::distance( eye, objB->getPosition() );
				  return distA < distB;
			  });
}

void FrustumCullingReduxApp::draw()
{
	// clear the window
	gl::clear( Color(0.25f, 0.25f, 0.25f) );
	
	{
		// setup camera
		gl::ScopedMatrices scopeMatrix;
		gl::setMatrices( mRenderCam );
		
		gl::ScopedState scopeState( GL_CULL_FACE, true );
		gl::enableDepthRead();
		gl::enableDepthWrite();
		
		{
			gl::ScopedGlslProg scopeGlsl( mShader );
			// draw hearts in depth sorted order, to make sure no pixel is drawn into twice
			for( auto & objIt : mPotentiallyVisibleSet )
				objIt->draw();
		}
		
		// draw helper objects
		gl::drawCoordinateFrame(100.0f, 5.0f, 2.0f);
		
		AxisAlignedBox3f worldBoundingBox;
		for( auto & objIt : mPotentiallyVisibleSet ) {
			if(bDrawEstimatedBoundingBoxes) {
				// create a fast approximation of the world space bounding box by transforming the
				// eight corners and using them to create a new axis aligned bounding box
				worldBoundingBox = mObjectBoundingBox.transformed( objIt->getTransform() );
				
				if( ! objIt->isCulled() )
					gl::color( Color(0, 1, 1) );
				else
					gl::color( Color(1, 0.5f, 0) );
				
				gl::drawStrokedCube( worldBoundingBox );
			}
			
			if(bDrawPreciseBoundingBoxes && ! objIt->isCulled()) {
				// you can see how much the approximated bounding boxes differ
				// from the precise ones by enabling this code
				worldBoundingBox = mTriMesh->calcBoundingBox( objIt->getTransform() );
				gl::color( Color(1, 1, 0) );
				gl::drawStrokedCube( worldBoundingBox );
			}
		}
		
		// Reset the field of view once again to visualize
		// the actual window we're using to cull with.
		gl::color( Color(1, 1, 1) );
		mRenderCam.setFov( 30.0f );
		gl::drawFrustum( mRenderCam );
		mRenderCam.setFov( 60.0f );
		
		drawGrid(2000.0f, 25.0f);
		
		// disable 3D rendering
		gl::disableDepthWrite();
		gl::disableDepthRead();
	}
	
	// render help
	if(bShowHelp && mHelp) {
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
				bDrawPreciseBoundingBoxes = ! bDrawPreciseBoundingBoxes;
			else
				bDrawEstimatedBoundingBoxes = ! bDrawEstimatedBoundingBoxes;
		break;
		case KeyEvent::KEY_c:
			bPerformCulling = !bPerformCulling;
		break;
		case KeyEvent::KEY_f: {
			bool verticalSyncEnabled = gl::isVerticalSyncEnabled();
			setFullScreen( ! isFullScreen() );
			gl::enableVerticalSync( verticalSyncEnabled );
		}
		break;
		case KeyEvent::KEY_h:
			bShowHelp = !bShowHelp;
		break;
		case KeyEvent::KEY_v:
			toggleVerticalSync();
		break;
	}
	
	// update info
	renderHelpToTexture();
}

void FrustumCullingReduxApp::loadObject()
{
	mTriMesh = TriMesh::create( ObjLoader( loadAsset("models/heart.obj") ) );
	mBatch = gl::Batch::create( *mTriMesh, mShader );

	mObjectBoundingBox = mTriMesh->calcBoundingBox();
}

void FrustumCullingReduxApp::drawGrid(float size, float step)
{
	gl::color( Colorf(0.2f, 0.2f, 0.2f) );
	for(float i=-size;i<=size;i+=step) {
		gl::drawLine( vec3(i, 0.0f, -size), vec3(i, 0.0f, size) );
		gl::drawLine( vec3(-size, 0.0f, i), vec3(size, 0.0f, i) );
	}
}

void FrustumCullingReduxApp::toggleVerticalSync()
{
	gl::enableVerticalSync( ! gl::isVerticalSyncEnabled() );
}

void FrustumCullingReduxApp::renderHelpToTexture()
{
	TextLayout layout;
	layout.setFont( Font("Arial", 18) );
	layout.setColor( ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
	layout.setLeadingOffset(3.0f);

	layout.clear( ColorA(0.25f, 0.25f, 0.25f, 0.5f) );

	if(bPerformCulling) layout.addLine("(C) Toggle culling (currently ON)");
	else  layout.addLine("(C) Toggle culling (currently OFF)");

	if(bDrawEstimatedBoundingBoxes) layout.addLine("(B) Toggle estimated bounding boxes (currently ON)");
	else  layout.addLine("(B) Toggle estimated bounding boxes (currently OFF)");

	if(bDrawPreciseBoundingBoxes) layout.addLine("(B)+(Shift) Toggle precise bounding boxes (currently ON)");
	else  layout.addLine("(B)+(Shift) Toggle precise bounding boxes (currently OFF)");

	if(gl::isVerticalSyncEnabled()) layout.addLine("(V) Toggle vertical sync (currently ON)");
	else  layout.addLine("(V) Toggle vertical sync (currently OFF)");

	if(bMoveCamera) layout.addLine("(Space) Toggle camera control (currently ON)");
	else  layout.addLine("(Space) Toggle camera control (currently OFF)");

	layout.addLine("(H) Toggle this help panel");

	mHelp = gl::Texture::create( layout.render(true, false) );
}

CINDER_APP_BASIC( FrustumCullingReduxApp, RendererGl )