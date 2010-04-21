#include "Resources.h"

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

using std::vector;


/*** This sample demonstrates the Vbo class by creating a simple grid mesh with a texture mapped onto it.
 * The mesh has static indices and texture coordinates, but its vertex positions are dynamic.
 * It also creates a second mesh which shares static and index buffers, but has its own dynamic buffer ***/

class VboSampleApp : public AppBasic {
 public:
	void setup();
	void update();
	void draw();

	static const int VERTICES_X = 250, VERTICES_Z = 50;

	gl::VboMesh		mVboMesh, mVboMesh2;
	gl::Texture		mTexture;
	CameraPersp		mCamera;
};

void VboSampleApp::setup()
{
	// setup the parameters of the Vbo
	int totalVertices = VERTICES_X * VERTICES_Z;
	int totalQuads = ( VERTICES_X - 1 ) * ( VERTICES_Z - 1 );
	gl::VboMesh::Layout layout;
	layout.setStaticIndices();
	layout.setDynamicPositions();
	layout.setStaticTexCoords2d();
	mVboMesh = gl::VboMesh( totalVertices, totalQuads * 4, layout, GL_QUADS );
	
	// buffer our static data - the texcoords and the indices
	vector<uint32_t> indices;
	vector<Vec2f> texCoords;
	for( int x = 0; x < VERTICES_X; ++x ) {
		for( int z = 0; z < VERTICES_Z; ++z ) {
			// create a quad for each vertex, except for along the bottom and right edges
			if( ( x + 1 < VERTICES_X ) && ( z + 1 < VERTICES_Z ) ) {
				indices.push_back( (x+0) * VERTICES_Z + (z+0) );
				indices.push_back( (x+1) * VERTICES_Z + (z+0) );
				indices.push_back( (x+1) * VERTICES_Z + (z+1) );
				indices.push_back( (x+0) * VERTICES_Z + (z+1) );
			}
			// the texture coordinates are mapped to [0,1.0)
			texCoords.push_back( Vec2f( x / (float)VERTICES_X, z / (float)VERTICES_Z ) );
		}
	}
	
	mVboMesh.bufferIndices( indices );
	mVboMesh.bufferTexCoords2d( 0, texCoords );
	
	// make a second Vbo that uses the statics from the first
	mVboMesh2 = gl::VboMesh( totalVertices, totalQuads * 4, mVboMesh.getLayout(), GL_QUADS, &mVboMesh.getIndexVbo(), &mVboMesh.getStaticVbo(), NULL );
	mVboMesh2.setTexCoordOffset( 0, mVboMesh.getTexCoordOffset( 0 ) );
	
	mTexture = gl::Texture( loadImage( loadResource( RES_IMAGE ) ) );
}

void VboSampleApp::update()
{
	gl::setMatrices( mCamera );

	const float timeFreq = 5.0f;
	const float zFreq = 3.0f;
	const float xFreq = 7.0f;
	float offset = getElapsedSeconds() * timeFreq;

	// dynmaically generate our new positions based on a simple sine wave
	gl::VboMesh::VertexIter iter = mVboMesh.mapVertexBuffer();
	for( int x = 0; x < VERTICES_X; ++x ) {
		for( int z = 0; z < VERTICES_Z; ++z ) {
			float height = sin( z / (float)VERTICES_Z * zFreq + x / (float)VERTICES_X * xFreq + offset ) / 5.0f;
			iter.setPosition( Vec3f( x / (float)VERTICES_X, height, z / (float)VERTICES_Z ) );
			++iter;
		}
	}

	// dynmaically generate our new positions based on a simple sine wave for mesh2
	gl::VboMesh::VertexIter iter2 = mVboMesh2.mapVertexBuffer();
	for( int x = 0; x < VERTICES_X; ++x ) {
		for( int z = 0; z < VERTICES_Z; ++z ) {
			float height = sin( z / (float)VERTICES_Z * zFreq * 2 + x / (float)VERTICES_X * xFreq * 3 + offset ) / 10.0f;
			iter2.setPosition( Vec3f( x / (float)VERTICES_X, height, z / (float)VERTICES_Z ) + Vec3f( 0, 0.5, 0 ) );
			++iter2;
		}
	}
}

void VboSampleApp::draw()
{
	// this pair of lines is the standard way to clear the screen in OpenGL
	gl::clear( Color( 0.15f, 0.15f, 0.15f ) );

	gl::scale( Vec3f( 10, 10, 10 ) );
	mTexture.enableAndBind();
	gl::draw( mVboMesh );
	gl::draw( mVboMesh2 );
}


CINDER_APP_BASIC( VboSampleApp, RendererGl )
