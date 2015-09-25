/*
 Copyright (C)2010-2015 Paul Houx
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted.

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
#include "cinder/CameraUi.h"
#include "cinder/TriMesh.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Picking3DApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;

	void mouseMove( MouseEvent event ) override;

	bool performPicking( vec3 *pickedPoint, vec3 *pickedNormal );
	void drawCube( const AxisAlignedBox &bounds, const Color &color );

private:
	TriMeshRef			mTriMesh;		//! The 3D mesh.
	AxisAlignedBox		mObjectBounds; 	//! The object space bounding box of the mesh.
	mat4				mTransform;		//! Transformations (translate, rotate, scale) of the mesh.
										
	//! By caching a 3D model and its shader on the GPU, we can draw it faster.
	gl::BatchRef		mWireCube;		
	gl::BatchRef		mWirePlane;
	gl::BatchRef		mMesh;

	CameraPersp			mCamera;
	CameraUi			mCamUi;

	ivec2				mMousePos;		//! Keep track of the mouse.
};

void Picking3DApp::setup()
{
	// Create the mesh.
	mTriMesh = TriMesh::create( geom::Teapot().subdivisions( 6 ) );

	// Get the object space bounding box of the model, for fast intersection testing.
	mObjectBounds = mTriMesh->calcBoundingBox();

	// Set up the camera.
	mCamera.lookAt( vec3( 2.0f, 3.0f, 1.0f ), vec3( 0 ) );
	mCamera.setPerspective( 40.0f, getWindowAspectRatio(), 0.01f, 100.0f );
	mCamUi = CameraUi( &mCamera, getWindow() );

	// Create batches that render fast.
	auto lambertShader = gl::getStockShader( gl::ShaderDef().color().lambert() );
	auto colorShader = gl::getStockShader( gl::ShaderDef().color() );

	mMesh = gl::Batch::create( *mTriMesh, lambertShader );
	mWirePlane = gl::Batch::create( geom::WirePlane().size( vec2( 10 ) ).subdivisions( ivec2( 10 ) ), colorShader );
	mWireCube = gl::Batch::create( geom::WireCube(), colorShader );
}

void Picking3DApp::update()
{
	// Animate our mesh.
	mTransform = mat4( 1.0f );
	mTransform *= rotate( sin( (float) getElapsedSeconds() * 3.0f ) * 0.08f, vec3( 1, 0, 0 ) );
	mTransform *= rotate( (float) getElapsedSeconds() * 0.1f, vec3( 0, 1, 0 ) );
	mTransform *= rotate( sin( (float) getElapsedSeconds() * 4.3f ) * 0.09f, vec3( 0, 0, 1 ) );
}

void Picking3DApp::draw()
{
	// Gray background.
	gl::clear( Color::gray( 0.5f ) );

	// Set up the camera.
	gl::ScopedMatrices push;
	gl::setMatrices( mCamera );

	// Enable depth buffer.
	gl::ScopedDepth depth( true );

	// Draw the grid on the floor.
	{
		gl::ScopedColor color( Color::gray( 0.2f ) );
		mWirePlane->draw();
	}

	// Draw the mesh.
	{
		gl::ScopedColor color( Color::white() );

		gl::ScopedModelMatrix model;
		gl::multModelMatrix( mTransform );

		mMesh->draw();
	}

	// Perform 3D picking now, so we can draw the result as a vector.
	vec3 pickedPoint, pickedNormal;
	if( performPicking( &pickedPoint, &pickedNormal ) ) {
		gl::ScopedColor color( Color( 0, 1, 0 ) );

		// Draw an arrow to the picked point along its normal.
		gl::ScopedGlslProg shader( gl::getStockShader( gl::ShaderDef().color().lambert() ) );
		gl::drawVector( pickedPoint + pickedNormal, pickedPoint );
	}
}

void Picking3DApp::mouseMove( MouseEvent event )
{
	// Keep track of the mouse.
	mMousePos = event.getPos();
}

bool Picking3DApp::performPicking( vec3 *pickedPoint, vec3 *pickedNormal )
{
	// Generate a ray from the camera into our world. Note that we have to
	// flip the vertical coordinate.
	float u = mMousePos.x / (float) getWindowWidth();
	float v = mMousePos.y / (float) getWindowHeight();
	Ray ray = mCamera.generateRay( u, 1.0f - v, mCamera.getAspectRatio() );

	// The coordinates of the bounding box are in object space, not world space,
	// so if the model was translated, rotated or scaled, the bounding box would not
	// reflect that. One solution would be to pass the transformation to the calcBoundingBox() function:
	AxisAlignedBox worldBoundsExact = mTriMesh->calcBoundingBox( mTransform ); // slow

	// But if you already have an object space bounding box, it's much faster to
	// approximate the world space bounding box like this:
	AxisAlignedBox worldBoundsApprox = mObjectBounds.transformed( mTransform ); // fast

	// Draw the object space bounding box in yellow. It will not animate,
	// because animation is done in world space.
	drawCube( mObjectBounds, Color( 1, 1, 0 ) );

	// Draw the exact bounding box in orange.
	drawCube( worldBoundsExact, Color( 1, 0.5f, 0 ) );

	// Draw the approximated bounding box in cyan.
	drawCube( worldBoundsApprox, Color( 0, 1, 1 ) );

	// Perform fast detection first - test against the bounding box itself.
	if( ! worldBoundsExact.intersects( ray ) )
		return false;

	// Set initial distance to something far, far away.
	float result = FLT_MAX;

	// Traverse triangle list and find the closest intersecting triangle.
	const size_t polycount = mTriMesh->getNumTriangles();

	float distance = 0.0f;
	for( size_t i = 0; i < polycount; ++i ) {
		// Get a single triangle from the mesh.
		vec3 v0, v1, v2;
		mTriMesh->getTriangleVertices( i, &v0, &v1, &v2 );

		// Transform triangle to world space.
		v0 = vec3( mTransform * vec4( v0, 1.0 ) );
		v1 = vec3( mTransform * vec4( v1, 1.0 ) );
		v2 = vec3( mTransform * vec4( v2, 1.0 ) );

		// Test to see if the ray intersects this triangle.
		if( ray.calcTriangleIntersection( v0, v1, v2, &distance ) ) {
			// Keep the result if it's closer than any intersection we've had so far.
			if( distance < result ) {
				result = distance;

				// Assuming this is the closest triangle, we'll calculate our normal
				// while we've got all the points handy.
				*pickedNormal = normalize( cross( v1 - v0, v2 - v0 ) );
			}
		}
	}

	// Did we have a hit?
	if( distance > 0 ) {
		// Calculate the exact position of the hit.
		*pickedPoint = ray.calcPosition( result );

		return true;
	}
	else
		return false;
}

void Picking3DApp::drawCube( const AxisAlignedBox &bounds, const Color & color )
{
	gl::ScopedColor clr( color );
	gl::ScopedModelMatrix model;

	gl::multModelMatrix( glm::translate( bounds.getCenter() ) * glm::scale( bounds.getSize() ) );
	mWireCube->draw();
}

CINDER_APP( Picking3DApp, RendererGl( RendererGl::Options().msaa( 8 ) ) )