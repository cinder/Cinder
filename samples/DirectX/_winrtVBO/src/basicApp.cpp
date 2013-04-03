#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Font.h"
#include "cinder/dx/dx.h"
#include "cinder/dx/DxVbo.h"
#include "cinder/dx/DxTexture.h"

#include "cinder/Camera.h"

#define NEW_PARTICLES 5

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public AppBasic {
  public:
	void setup();
	void update();
	void draw();
	
	static const int VERTICES_X = 250, VERTICES_Z = 50;

	dx::VboMesh		mVboMesh, mVboMesh2;
	dx::Texture		mTexture;
	CameraPersp		mCamera;

	Font		mFont;
};

void BasicApp::setup()
{
	mFont = Font( "Arial", 24.0f );

	int totalVertices = VERTICES_X * VERTICES_Z;
	int totalQuadIndices = ( VERTICES_X - 1 ) * ( VERTICES_Z - 1 ) * 4;

	cinder::dx::VboMesh::Layout layout;
	layout.setStaticIndices();
	layout.setDynamicPositions();
	layout.setStaticTexCoords2d();

	mVboMesh = dx::VboMesh( totalVertices, totalQuadIndices, layout, GL_QUADS );

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
	mVboMesh2.bufferTexCoords2d( 0, texCoords );

	// make a second VBO that uses the statics from the first
	mVboMesh2 = dx::VboMesh( totalVertices, totalQuadIndices, mVboMesh.getLayout(), GL_QUADS );
	mVboMesh2.setTexCoordOffset( 0, mVboMesh.getTexCoordOffset( 0 ) );

	mTexture = dx::Texture( loadImage( loadAsset( "testPattern.png" ) ) );

}

void BasicApp::update()
{

	dx::setMatrices( mCamera );

	const float timeFreq = 5.0f;
	const float zFreq = 3.0f;
	const float xFreq = 7.0f;
	float offset = getElapsedSeconds() * timeFreq;

	dx::VboMesh::VertexIter iter = mVboMesh.mapVertexBuffer();
	for ( int x = 0; x < VERTICES_X; ++x ) {
		for ( int z = 0; z < VERTICES_Z; ++z) {
			float height = sin( z / (float)VERTICES_Z * zFreq + x / (float)VERTICES_X * xFreq + offset ) / 5.0f;
			iter.setPosition( Vec3f( x / (float)VERTICES_X, height, z / (float)VERTICES_Z ) );
			++iter;
		}
	}

	// dynmaically generate our new positions based on a simple sine wave for mesh2
	dx::VboMesh::VertexIter iter2 = mVboMesh2.mapVertexBuffer();
	for( int x = 0; x < VERTICES_X; ++x ) {
		for( int z = 0; z < VERTICES_Z; ++z ) {
			float height = sin( z / (float)VERTICES_Z * zFreq * 2 + x / (float)VERTICES_X * xFreq * 3 + offset ) / 10.0f;
			iter2.setPosition( Vec3f( x / (float)VERTICES_X, height, z / (float)VERTICES_Z ) + Vec3f( 0, 0.5, 0 ) );
			++iter2;
		}
	}
}

void BasicApp::draw()
{
	dx::clear( Color( 0, 0, 0 ), true );
	dx::enableAdditiveBlending();

	dx::scale( Vec3f(10, 10, 10 ) );
	mTexture.bind();
	dx::draw( mVboMesh );
	dx::draw( mVboMesh2 );

	std::stringstream s;
	s << "Framerate:" << getAverageFps();
	dx::drawString(s.str(),Vec2f(10.0f,10.0f),Color::white(),mFont);
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )