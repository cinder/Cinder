/*
 Copyright (c) 2012-2015, Paul Houx
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

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"

#include "cinder/gl/gl.h"

#include "cinder/AxisAlignedBox.h"
#include "cinder/Frustum.h"
#include "cinder/CameraUi.h"
#include "cinder/ObjLoader.h"
#include "cinder/Text.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/Utilities.h"

#include "CullableObject.h"

const int NUM_OBJECTS = 1600;

using namespace ci;
using namespace ci::app;
using namespace std;

class FrustumCullingReduxApp : public App {
public:
	static void prepareSettings( Settings *settings );

	void setup();
	void update();
	void draw();

	void toggleCullableFov();
	void drawCullableFov();

	void keyDown( KeyEvent event );

	void resize() override;

protected:
	//! Load the heart shaped mesh.
	void loadObject();
	//! Renders the help menu.
	void renderHelpToTexture();

protected:
	// keep track of time
	double  mCurrentSeconds;

	// flags
	bool  mPerformCulling;
	bool  mCullWithSpheres;
	bool  mDrawWorldSpaceBounds;
	bool  mDrawObjectSpaceBounds;
	bool  mShowRevealingFov;
	bool  mShowHelp;

	Anim<float>                     mCullingFov;

	ci::TriMeshRef                  mTriMesh;
	gl::BatchRef                    mBatch, mSphereBatch, mBoxBatch, mGridBatch;

	// caches the heart's bounding box and sphere in object space coordinates
	AxisAlignedBox                mObjectBoundingBox;
	Sphere                          mObjectBoundingSphere;

	// objects
	std::vector<CullableObjectRef>  mObjects;

	// camera
	CameraUi                        mCamUi;
	CameraPersp                     mRenderCam;

	// help text
	gl::Texture2dRef                mHelp;
};

void FrustumCullingReduxApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1200, 675 );
	settings->disableFrameRate();
}

void FrustumCullingReduxApp::setup()
{
	// Disable vertical sync, so we can see the difference
	// in frame rate if culling is enabled.
	gl::enableVerticalSync( false );

	// Intialize settings.
	mPerformCulling = true;
	mCullWithSpheres = true;
	mDrawWorldSpaceBounds = false;
	mDrawObjectSpaceBounds = true;
	mShowRevealingFov = true;
	mShowHelp = true;

	// Render help texture.
	renderHelpToTexture();

	// Load assets.
	loadObject();

	// Create a few hearts.
	int sz = (int) math<double>::sqrt( NUM_OBJECTS );

	Rand::randomize();
	mObjects.resize( NUM_OBJECTS );
	for( int i = 0; i < NUM_OBJECTS; ++i ) {
		vec3 pos = 100.0f * vec3( i % sz - sz / 2, 0, i / sz - sz / 2 );
		vec3 rot = vec3( 0, Rand::randFloat( -360, 360 ), 0 );
		float scale = 50.0f;

		auto &obj = mObjects[i];
		obj = CullableObjectRef( new CullableObject( mBatch ) );
		obj->setTransform( pos, rot, scale );
	}

	// Create bounding box and sphere representations.
	mSphereBatch = gl::Batch::create( geom::WireSphere(), gl::getStockShader( gl::ShaderDef().color() ) );
	mBoxBatch = gl::Batch::create( geom::WireCube(), gl::getStockShader( gl::ShaderDef().color() ) );

	// Create grid.
	mGridBatch = gl::Batch::create( geom::WirePlane().size( vec2( 4000 ) ).subdivisions( ivec2( 80, 80 ) ), gl::getStockShader( gl::ShaderDef().color() ) );

	// Setup the camera.
	mCullingFov = 45.0f;

	mRenderCam.setPerspective( 60.0f, getWindowAspectRatio(), 10, 10000 );
	mRenderCam.lookAt( vec3( 200 ), vec3( 0 ) );
	mCamUi = CameraUi( &mRenderCam, getWindow() );

	// Track current time so we can calculate elapsed time.
	mCurrentSeconds = getElapsedSeconds();
}

void FrustumCullingReduxApp::update()
{
	// Calculate elapsed time.
	double elapsed = getElapsedSeconds() - mCurrentSeconds;
	mCurrentSeconds += elapsed;

	// perform frustum culling **********************************************************************************

	// Save the current culling field of view. If mShowRevealingFov = true, 
	// this will narrow the camera's FOV so that you can see the culling effect.
	float originalFov = mRenderCam.getFov();
	mRenderCam.setFov( mCullingFov );

	// Obtain the camera's view frustum, allowing us to perform frustum culling.
	Frustumf visibleWorld( mRenderCam );

	// Restore FOV to original
	mRenderCam.setFov( originalFov );

	for( auto & obj : mObjects ) {
		// Update object (so it rotates slowly around its axis).
		obj->update( elapsed );

		if( !mPerformCulling ) {
			// Don't cull the object. All objects will be drawn,
			// even if they are outside the camera's view frustum.
			obj->setCulled( false );
		}
		else if( mCullWithSpheres ) {
			// Use the object's bounding sphere, converted to world space.
			Sphere worldBoundingSphere = mObjectBoundingSphere.transformed( obj->getTransform() );

			// Check if the bounding sphere intersects the camera's view frustum.
			obj->setCulled( !visibleWorld.intersects( worldBoundingSphere ) );
		}
		else {
			// Use the object's bounding box, converted to world space.
			AxisAlignedBox worldBoundingBox = mObjectBoundingBox.transformed( obj->getTransform() );

			// Check if the bounding box intersects the camera's view frustum.
			obj->setCulled( !visibleWorld.intersects( worldBoundingBox ) );
		}
	}

	// Update window title.
	if( getElapsedFrames() % 60 == 0 )
		getWindow()->setTitle( "Frustum Culling Redux - " + toString( (int) getAverageFps() ) + " FPS" );
}

void FrustumCullingReduxApp::draw()
{
	// Clear the window.
	gl::clear( Color::gray( 0.25f ) );

	{
		// Get ready to render a 3D world.
		gl::ScopedMatrices scopeMatrix;
		gl::setMatrices( mRenderCam );

		gl::ScopedState scopeState( GL_CULL_FACE, true );
		gl::enableDepthRead();
		gl::enableDepthWrite();

		// Draw all objects.
		for( auto & obj : mObjects )
			obj->draw();

		// Draw bounding volumes.
		if( mDrawWorldSpaceBounds ) {
			for( auto & obj : mObjects ) {
				// Use cyan bounds if not culled, orange bounds if culled.
				if( !obj->isCulled() )
					gl::color( Color( 0, 1, 1 ) );
				else
					gl::color( Color( 1, 0.5f, 0 ) );

				if( mCullWithSpheres ) {
					// Create a fast approximation of the world space bounding sphere.
					Sphere worldBoundingSphere = mObjectBoundingSphere.transformed( obj->getTransform() );

					gl::ScopedModelMatrix mtx;
					gl::translate( worldBoundingSphere.getCenter() );
					gl::scale( vec3( worldBoundingSphere.getRadius() ) );
					mSphereBatch->draw();
				}
				else {
					// Create a fast approximation of the world space bounding box by transforming the
					// eight corners and using them to create a new axis aligned bounding box.
					AxisAlignedBox worldBoundingBox = mObjectBoundingBox.transformed( obj->getTransform() );

					gl::ScopedModelMatrix mtx;
					gl::translate( worldBoundingBox.getCenter() );
					gl::scale( worldBoundingBox.getSize() );
					mBoxBatch->draw();
				}

				// Don't draw object space bounds if culled.
				if( obj->isCulled() )
					continue;

				if( mDrawObjectSpaceBounds ) {
					if( mCullWithSpheres ) {
						// Draw the precise bounding sphere in yellow. You will be able to
						// see how much it differs from the world space bounding sphere approximation.
						gl::color( Color( 1, 1, 0 ) );

						gl::ScopedModelMatrix mtx;
						gl::multModelMatrix( obj->getTransform() );
						gl::scale( vec3( mObjectBoundingSphere.getRadius() ) );
						gl::translate( mObjectBoundingBox.getCenter() );
						mSphereBatch->draw();
					}
					else {
						// Draw the precise bounding box in yellow. You will be able to
						// see how much it differs from the world space bounding box approximation.
						gl::color( Color( 1, 1, 0 ) );

						gl::ScopedModelMatrix mtx;
						gl::multModelMatrix( obj->getTransform() );
						gl::translate( mObjectBoundingBox.getCenter() );
						gl::scale( mObjectBoundingBox.getSize() );
						mBoxBatch->draw();
					}
				}
			}
		}

		// Disable writing to depth buffer before drawing the grid.
		gl::disableDepthWrite();

		gl::color( Colorf( 0.2f, 0.2f, 0.2f ) );
		mGridBatch->draw();

		// Disable depth testing.
		gl::disableDepthRead();

		drawCullableFov();
	}

	// Render help menu.
	if( mShowHelp && mHelp ) {
		gl::enableAlphaBlending();
		gl::color( Color::white() );
		gl::draw( mHelp );
		gl::disableAlphaBlending();
	}
}

void FrustumCullingReduxApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
	case KeyEvent::KEY_ESCAPE:
		if( isFullScreen() )
			setFullScreen( false );
		else
			quit();
		// Use 'return' instead of 'break' to prevent rendering
		// the help menu on quit.
		return;
	case KeyEvent::KEY_b:
		if( event.isShiftDown() ) {
			mDrawWorldSpaceBounds = true;
			mDrawObjectSpaceBounds = !mDrawObjectSpaceBounds;
		}
		else
			mDrawWorldSpaceBounds = !mDrawWorldSpaceBounds;
		break;
	case KeyEvent::KEY_c:
		mPerformCulling = !mPerformCulling;
		break;
	case KeyEvent::KEY_s:
		mCullWithSpheres = !mCullWithSpheres;
		break;
	case KeyEvent::KEY_f:
		setFullScreen( !isFullScreen() );
		break;
	case KeyEvent::KEY_h:
		mShowHelp = !mShowHelp;
		break;
	case KeyEvent::KEY_v:
		gl::enableVerticalSync( !gl::isVerticalSyncEnabled() );
		break;
	case KeyEvent::KEY_SPACE:
		toggleCullableFov();
		break;
	}

	// Update help menu.
	renderHelpToTexture();
}

void FrustumCullingReduxApp::resize()
{
	// Adjust the camera's aspect ratio.
	mRenderCam.setAspectRatio( getWindowAspectRatio() );
}

void FrustumCullingReduxApp::toggleCullableFov()
{
	// When enabled, reduces the camera's field of view for culling only.
	// This allows you to see what culling does.
	mShowRevealingFov = !mShowRevealingFov;

	timeline().apply( &mCullingFov, mShowRevealingFov ? 45.0f : 60.0f, 0.6f, EaseInOutCubic() );
}

void FrustumCullingReduxApp::drawCullableFov()
{
	// Visualize the camera's field of view as a rectangle.
	gl::ScopedColor color( Color( 1, 1, 1 ) );

	float cullable = mCullingFov / 60.0f;
	if( cullable >= 1.0f )
		return;

	gl::ScopedMatrices scopeMat;
	gl::setMatricesWindow( getWindowSize() );
	gl::translate( getWindowCenter() );
	gl::scale( vec3( cullable * getWindowCenter().x, cullable * getWindowCenter().y, 1 ) );
	gl::drawStrokedRect( Rectf( -1, -1, 1, 1 ) );
}

void FrustumCullingReduxApp::loadObject()
{
	try {
		// We first convert our obj to a trimesh, so we can calculate the bounding box and sphere.
		mTriMesh = TriMesh::create( ObjLoader( loadAsset( "models/heart.obj" ) ) );

		// We then use trimesh to calculate the bounding box and sphere.
		mObjectBoundingBox = mTriMesh->calcBoundingBox();
		mObjectBoundingSphere = Sphere::calculateBoundingSphere( mTriMesh->getPositions<3>(), mTriMesh->getNumVertices() );

		// Then use our trimesh and a shader to create a batch.
		auto shader = gl::GlslProg::create( loadAsset( "shaders/phong.vert" ), loadAsset( "shaders/phong.frag" ) );
		mBatch = gl::Batch::create( *mTriMesh, shader );
	}
	catch( const std::exception &e ) {
		app::console() << "Failed to load object:" << e.what() << std::endl;
	}
}

void FrustumCullingReduxApp::renderHelpToTexture()
{
	TextLayout layout;
	layout.setFont( Font( "Arial", 18 ) );
	layout.setColor( ColorA( 1, 1, 1, 1 ) );
	layout.setLeadingOffset( 3 );

	layout.clear( ColorA::gray( 0.2f, 0.75f ) );

	if( mShowRevealingFov )
		layout.addLine( "(Space) Toggle reveal culling (currently ON)" );
	else
		layout.addLine( "(Space) Toggle reveal culling (currently OFF)" );

	if( mPerformCulling )
		layout.addLine( "(C) Toggle culling (currently ON)" );
	else
		layout.addLine( "(C) Toggle culling (currently OFF)" );

	if( mCullWithSpheres )
		layout.addLine( "(S) Toggle using spheres for culling (currently ON)" );
	else
		layout.addLine( "(S) Toggle using spheres for culling (currently OFF)" );

	if( mDrawWorldSpaceBounds )
		layout.addLine( "(B) Toggle world space bounding boxes (currently ON)" );
	else
		layout.addLine( "(B) Toggle world space bounding boxes (currently OFF)" );

	if( mDrawObjectSpaceBounds )
		layout.addLine( "(B)+(Shift) Toggle object space bounding boxes (currently ON)" );
	else
		layout.addLine( "(B)+(Shift) Toggle object space bounding boxes (currently OFF)" );

	if( gl::isVerticalSyncEnabled() )
		layout.addLine( "(V) Toggle vertical sync (currently ON)" );
	else
		layout.addLine( "(V) Toggle vertical sync (currently OFF)" );

	layout.addLine( "(H) Toggle this help panel" );

	mHelp = gl::Texture::create( layout.render( true, false ) );
}

CINDER_APP( FrustumCullingReduxApp, RendererGl, &FrustumCullingReduxApp::prepareSettings )