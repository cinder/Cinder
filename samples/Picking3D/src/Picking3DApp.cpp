/*
 Copyright (C)2010 Paul Houx
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

#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIo.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Rand.h"
#include "cinder/TriMesh.h"
#include "Resources.h"

#include <vector>
#include <utility>

using namespace ci;
using namespace ci::app;
using namespace std;

// our bubbles
typedef boost::shared_ptr<class Bubble> BubbleRef;

class Bubble
{
  public:
	Bubble( const vec3 &origin )
		:	age( 0.0 ),
			lifespan( 3.0 ),
			position( origin ),
			speed( vec3( 0 ) )
		{ };

	void update( double elapsed ) {
		age += elapsed;
		speed += 0.1f * vec3( Rand::randFloat() -0.5f, 1.0f, Rand::randFloat() -0.5f );
		position += speed * (float) elapsed;
	}
	
	void draw() const {
		gl::drawSphere( position, 0.05f, 8 );
	}

	bool isDead() const {
		return age >= lifespan;
	}
  protected:
	double age;
	double lifespan;

	vec3  speed;
	vec3  position;
};

// our main application
class Picking3DApp : public AppBasic 
{
  public:
	void setup() override;
	void update() override;
	void draw() override;
	
	void mouseMove( MouseEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void resize() override;
  protected:
	void drawGrid(float size=100.0f, float step=10.0f);
	bool performPicking( vec3 *pickedPoint, vec3 *pickedNormal );
	
	
	gl::GlslProgRef		mShader;		// shader for our model
	gl::Texture2dRef	mTexture;		// texture for our model
	TriMeshRef			mMesh;			// the model of a rubber duckys
	AxisAlignedBox3f	mObjectBounds; 	// the object space bounding box of the mesh
	mat4				mTransform;		// transformations (translate, rotate, scale) of the model
	MayaCamUI			mMayaCam;		// our camera
	ivec2				mMousePos;		// keep track of the mouse
	double				mTime;			// keep track of time

	vector<BubbleRef>	mBubbles;		// particles
};

void Picking3DApp::setup()
{
	// initialize stuff
	mTime = getElapsedSeconds();

	// load and compile the shader
	mShader = gl::GlslProg::create( loadResource( RES_SHADER_VERT ), loadResource( RES_SHADER_FRAG ) );

	// load the texture
	mTexture = gl::Texture::create( loadImage( loadResource( RES_DUCKY_TEX ) ), gl::Texture::Format().loadTopDown( true ).mipmap() );

	// load the mesh 
	//  (note: the mesh was created from an OBJ file
	//  using the ObjLoader class. The data was then saved using the
	//  TriMesh::write() method. Reading binary files is much quicker.)
	mMesh = TriMesh::create();
	mMesh->read( loadResource( RES_DUCKY_MESH ) );

	// get the object space bounding box of the model, for fast intersection testing
	mObjectBounds = mMesh->calcBoundingBox();

	// set up the camera
	CameraPersp cam;
	cam.setEyePoint( vec3( 5.0f, 10.0f, 10.0f ) );
	cam.setCenterOfInterestPoint( vec3( 0.0f, 2.5f, 0.0f ) );
	cam.setPerspective( 60.0f, getWindowAspectRatio(), 1.0f, 1000.0f );
	mMayaCam.setCurrentCam( cam );
	
	// enable the depth buffer (after all, we are doing 3D)
	gl::enableDepthRead();
	gl::enableDepthWrite();
}

void Picking3DApp::update()
{
	// calculate elapsed time
	double elapsed = getElapsedSeconds() - mTime;
	mTime = getElapsedSeconds();

	// animate our little ducky
	mTransform = mat4( 1.0 );
	mTransform *= rotate( sin( (float) getElapsedSeconds() * 3.0f ) * 0.08f, vec3( 1, 0, 0 ) );
	mTransform *= rotate( (float) getElapsedSeconds() * 0.1f, vec3( 0, 1, 0 ) );
	mTransform *= rotate( sin( (float) getElapsedSeconds() * 4.3f ) * 0.09f, vec3( 0, 0, 1 ) );

	// animate our bubbles
	for( auto& bubble : mBubbles ) {
		bubble->update( elapsed );
	}
	
	// remove dead bubbles
	auto newEnd = std::remove_if( mBubbles.begin(), mBubbles.end(), []( const BubbleRef& b ) -> bool { return b->isDead(); } );
	mBubbles.erase( newEnd, mBubbles.end() );
}

void Picking3DApp::draw()
{
	// gray background
	gl::clear( Colorf(0.5f, 0.5f, 0.5f) );

	// set up the camera 
	gl::ScopedMatrices push;
	gl::setMatrices( mMayaCam.getCamera() );

	// draw the grid on the floor
	drawGrid();
	
	{
		gl::ScopedModelMatrix model;
		gl::ScopedTextureBind tex0( mTexture );
		gl::ScopedGlslProg prog( mShader );
		mShader->uniform( "tex0", 0 );
		//  (note: reset current color to white so the actual texture colors show up)
		gl::color( Color::white() );
		//  (note: apply transformations to the model)
		gl::multModelMatrix( mTransform );
		// Draw the trimesh
		gl::draw( *mMesh );
	}

	// perform 3D picking now, so we can draw the intersection as a sphere
	vec3 pickedPoint, pickedNormal;
	if( performPicking( &pickedPoint, &pickedNormal ) ) {
		gl::color( Color( 0, 1, 0 ) );
		// draw an arrow to the picked point along its normal
		gl::drawVector( pickedPoint + pickedNormal, pickedPoint );

		// add one bubble at the point of intersection
		mBubbles.push_back( BubbleRef( new Bubble( pickedPoint ) ) );
	}

	// draw bubbles 
	gl::color( Colorf( 0.1f, 0.2f, 0.25f ) );
	gl::enableAdditiveBlending();
	for( const auto& bubble : mBubbles ) {
		bubble->draw();
	}
	gl::disableAlphaBlending();
}

void Picking3DApp::drawGrid(float size, float step)
{
	gl::color( Colorf( 0.2f, 0.2f, 0.2f ) );
	gl::begin( GL_LINES );
	for( float i=-size; i <= size; i += step ) {
		gl::vertex( vec3( i, 0.0f, -size ) );
		gl::vertex( vec3( i, 0.0f, size ) );
		gl::vertex( vec3( -size, 0.0f, i ) );
		gl::vertex( vec3( size, 0.0f, i ) );
	}
	gl::end();
}

bool Picking3DApp::performPicking( vec3 *pickedPoint, vec3 *pickedNormal )
{
	// get our camera 
	CameraPersp cam = mMayaCam.getCamera();

	// generate a ray from the camera into our world
	float u = mMousePos.x / (float) getWindowWidth();
	float v = mMousePos.y / (float) getWindowHeight();
	// because OpenGL and Cinder use a coordinate system
	// where (0, 0) is in the LOWERleft corner, we have to flip the v-coordinate
	Ray ray = cam.generateRay(u , 1.0f - v, cam.getAspectRatio() );

	// draw the object space bounding box in yellow
	gl::color( Color(1, 1, 0) );
	gl::drawStrokedCube( mObjectBounds );

	// the coordinates of the bounding box are in object space, not world space,
	// so if the model was translated, rotated or scaled, the bounding box would not
	// reflect that. 
	//
	// One solution would be to pass the transformation to the calcBoundingBox() function: 
	AxisAlignedBox3f worldBoundsExact = mMesh->calcBoundingBox( mTransform ); // slow

	// draw this transformed box in orange
	gl::color( Color( 1, 0.5, 0 ) );
	gl::drawStrokedCube( worldBoundsExact );
	
	// But if you already have an object space bounding box, it's much faster to
	// approximate the world space bounding box like this:
	AxisAlignedBox3f worldBoundsApprox = mObjectBounds.transformed( mTransform ); // fast

	// draw this transformed box in cyan
	gl::color( Color( 0, 1, 1 ) );
	gl::drawStrokedCube( worldBoundsApprox );

	// fast detection first - test against the bounding box itself
	if( ! worldBoundsExact.intersects( ray ) )
		return false;

	// set initial distance to something far, far away
	float result = 1.0e6f;

	// traverse triangle list and find the picked triangle
	size_t polycount = mMesh->getNumTriangles();
	float distance = 0.0f;
	for( size_t i=0; i<polycount; ++i ) {
		vec3 v0, v1, v2;
		// get a single triangle from the mesh
		mMesh->getTriangleVertices( i, &v0, &v1, &v2 );

		// transform triangle to world space
		v0 = vec3( mTransform * vec4( v0, 1.0 ) );
		v1 = vec3( mTransform * vec4( v1, 1.0 ) );
		v2 = vec3( mTransform * vec4( v2, 1.0 ) );

		// test to see if the ray intersects with this triangle
		if( ray.calcTriangleIntersection( v0, v1, v2, &distance ) ) {
			// set our result to this if its closer than any intersection we've had so far
			if( distance < result ) {
				result = distance;
				// assuming this is the closest triangle, we'll set our normal
				// while we've got all the points handy
				*pickedNormal = normalize( cross( v1 - v0, v2 - v0 ) );
			}
		}
	}

	// did we have a hit?
	if( distance > 0 ) {
		*pickedPoint = ray.calcPosition( result );
		return true;
	}
	else
		return false;
}

void Picking3DApp::mouseMove( MouseEvent event )
{
	// keep track of the mouse
	mMousePos = event.getPos();
}

void Picking3DApp::mouseDown( MouseEvent event )
{	
	// let the camera handle the interaction
	mMayaCam.mouseDown( event.getPos() );
}

void Picking3DApp::mouseDrag( MouseEvent event )
{
	// keep track of the mouse
	mMousePos = event.getPos();

	// let the camera handle the interaction
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void Picking3DApp::resize()
{
	// adjust aspect ratio
	CameraPersp cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
}


CINDER_APP_BASIC( Picking3DApp, ci::app::RendererGl )