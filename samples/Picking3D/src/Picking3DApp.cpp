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

#include "cinder/app/AppBasic.h"
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
	Bubble(const Vec3f &origin) 
		:	age(0.0), 
			lifespan(3.0),
			position(origin),
			speed( Vec3f::zero() )
		{};

	void update(double elapsed);
	void draw();

	bool isDead();
  protected:
	double age;
	double lifespan;

	Vec3f  speed;
	Vec3f  position;
};

void Bubble::update(double elapsed)
{
	age += elapsed;
	speed += 0.1f * Vec3f(Rand::randFloat() -0.5f, 1.0f, Rand::randFloat() -0.5f);
	position += speed * (float) elapsed;
}

void Bubble::draw()
{
	gl::drawSphere(position, 0.05f, 8);
}

bool Bubble::isDead(void)
{
	return (age >= lifespan);
}


// our main application
class Picking3DApp : public AppBasic 
{
  public:
	void setup();
	void update();
	void draw();

	void drawGrid(float size=100.0f, float step=10.0f);
		
	bool performPicking( Vec3f *pickedPoint, Vec3f *pickedNormal );
		
	void mouseMove( MouseEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void resize( ResizeEvent event );

  protected:
	// shader and texture for our model
	gl::GlslProg	mShader;
	gl::Texture		mTexture;

	// the model of a rubber ducky
	TriMesh		mMesh;

	// transformations (translate, rotate, scale) of the model
	Matrix44f	mTransform;

	// our camera
	MayaCamUI	mMayaCam;

	// keep track of the mouse
	Vec2i		mMousePos;

	// keep track of time
	double		mTime;

	// particles
	vector<BubbleRef>	mBubbles;
};

void Picking3DApp::setup()
{
	// initialize stuff
	mTime = getElapsedSeconds();
	mTransform.setToIdentity();

	// load and compile the shader
	//  (note: a shader is not required, but gives a much better visual appearance.
	//	See for yourself by disabling the 'mShader.bind()' call in the draw method.)
	mShader = gl::GlslProg( loadResource( RES_SHADER_VERT ), loadResource( RES_SHADER_FRAG ) );

	// load the texture
	//  (note: enable mip-mapping by default)
	gl::Texture::Format format;
	format.enableMipmapping(true);			
	ImageSourceRef img = loadImage( loadResource( RES_DUCKY_TEX ) );
	if(img) mTexture = gl::Texture( img, format );


	// load the mesh 
	//  (note: the mesh was created from an OBJ file
	//  using the ObjLoader class. The data was then saved using the
	//  TriMesh::write() method. Reading binary files is much quicker.)
	mMesh.read( loadResource( RES_DUCKY_MESH ) );

	// set up the camera
	CameraPersp cam;
	cam.setEyePoint( Vec3f(5.0f, 10.0f, 10.0f) );
	cam.setCenterOfInterestPoint( Vec3f(0.0f, 2.5f, 0.0f) );
	cam.setPerspective( 60.0f, getWindowAspectRatio(), 1.0f, 1000.0f );
	mMayaCam.setCurrentCam( cam );
}

void Picking3DApp::update(void)
{
	// calculate elapsed time
	double elapsed = getElapsedSeconds() - mTime;
	mTime = getElapsedSeconds();

	// animate our little ducky
	mTransform.setToIdentity();
	mTransform.rotate( Vec3f::xAxis(), sinf( (float) getElapsedSeconds() * 3.0f ) * 0.08f );
	mTransform.rotate( Vec3f::yAxis(), (float) getElapsedSeconds() * 0.1f );
	mTransform.rotate( Vec3f::zAxis(), sinf( (float) getElapsedSeconds() * 4.3f ) * 0.09f );

	// animate our bubbles
	vector<BubbleRef>::iterator itr;
	for(itr = mBubbles.begin();itr!=mBubbles.end();)
	{
		(*itr)->update(elapsed);

		// remove dead bubbles
		if( (*itr)->isDead() )
			itr = mBubbles.erase(itr);
		else
			++itr;
	}
}

void Picking3DApp::draw()
{
	// gray background
	gl::clear( Colorf(0.5f, 0.5f, 0.5f) );

	// set up the camera 
	gl::pushMatrices();
	gl::setMatrices( mMayaCam.getCamera() );

	// enable the depth buffer (after all, we are doing 3D)
	gl::enableDepthRead();
	gl::enableDepthWrite();

	// draw the grid on the floor
	drawGrid();

	// bind the texture
	mTexture.bind();

	// bind the shader and tell it to use our texture
	mShader.bind();
	mShader.uniform("tex0", 0);

	// draw the mesh 
	//  (note: reset current color to white so the actual texture colors show up)
	gl::color( Color::white() );
	//  (note: apply transformations to the model)
	gl::pushModelView();
		gl::multModelView( mTransform );
		gl::draw( mMesh );
	gl::popModelView();

	// unbind the shader and texture
	mShader.unbind();
	mTexture.unbind();

	// perform 3D picking now, so we can draw the intersection as a sphere
	Vec3f pickedPoint, pickedNormal;
	if( performPicking( &pickedPoint, &pickedNormal ) ) {
		gl::color( ColorAf(0.0f, 1.0f, 0.0f, 0.5f) );
		// draw an arrow to the picked point along its normal
		gl::drawVector( pickedPoint + pickedNormal, pickedPoint );

		// add one bubble at the point of intersection
		mBubbles.push_back( BubbleRef( new Bubble( pickedPoint ) ) );
	}

	// draw bubbles 
	gl::color( Colorf(0.1f, 0.2f, 0.25f) );
	gl::enableAdditiveBlending();

	vector<BubbleRef>::iterator itr;
	for(itr = mBubbles.begin();itr!=mBubbles.end();++itr)
		(*itr)->draw();

	gl::disableAlphaBlending();

	gl::popMatrices();
}

void Picking3DApp::drawGrid(float size, float step)
{
	gl::color( Colorf(0.2f, 0.2f, 0.2f) );
	for(float i=-size;i<=size;i+=step) {
		gl::drawLine( Vec3f(i, 0.0f, -size), Vec3f(i, 0.0f, size) );
		gl::drawLine( Vec3f(-size, 0.0f, i), Vec3f(size, 0.0f, i) );
	}
}

bool Picking3DApp::performPicking( Vec3f *pickedPoint, Vec3f *pickedNormal )
{
	// get our camera 
	CameraPersp cam = mMayaCam.getCamera();

	// generate a ray from the camera into our world
	float u = mMousePos.x / (float) getWindowWidth();
	float v = mMousePos.y / (float) getWindowHeight();
	// because OpenGL and Cinder use a coordinate system
	// where (0, 0) is in the LOWERleft corner, we have to flip the v-coordinate
	Ray ray = cam.generateRay(u , 1.0f - v, cam.getAspectRatio() );

	// get the bounding box of the model, for fast intersection testing
	AxisAlignedBox3f objectBounds = mMesh.calcBoundingBox();

	// draw this untransformed box in yellow
	gl::color( Color(1, 1, 0) );
	gl::drawStrokedCube(objectBounds);

	// the coordinates of the bounding box are in object space, not world space,
	// so if the model was translated, rotated or scaled, the bounding box would not
	// reflect that.
	AxisAlignedBox3f worldBounds = mMesh.calcBoundingBox(mTransform);

	// draw this transformed box in cyan
	gl::color( Color(0, 1, 1) );
	gl::drawStrokedCube(worldBounds);

	// set initial distance to something far, far away
	float result = 1.0e6f;

	// fast detection first - test against the bounding box itself
	if( ! worldBounds.intersects(ray) )
		return false;

	// traverse triangle list and find the picked triangle
	size_t polycount = mMesh.getNumTriangles();
	float distance = 0.0f;
	for(size_t i=0;i<polycount;++i)
	{
		Vec3f v0, v1, v2;
		// get a single triangle from the mesh
		mMesh.getTriangleVertices(i, &v0, &v1, &v2);

		// transform triangle to world space
		v0 = mTransform.transformPointAffine(v0);
		v1 = mTransform.transformPointAffine(v1);
		v2 = mTransform.transformPointAffine(v2);

		// test to see if the ray intersects with this triangle
		if( ray.calcTriangleIntersection(v0, v1, v2, &distance) ) {
			// set our result to this if its closer than any intersection we've had so far
			if( distance < result ) {
				result = distance;
				// assuming this is the closest triangle, we'll set our normal
				// while we've got all the points handy
				*pickedNormal = ( v1 - v0 ).cross( v2 - v0 ).normalized();
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

void Picking3DApp::resize( ResizeEvent event )
{
	// adjust aspect ratio
	CameraPersp cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
}


CINDER_APP_BASIC( Picking3DApp, ci::app::RendererGl )