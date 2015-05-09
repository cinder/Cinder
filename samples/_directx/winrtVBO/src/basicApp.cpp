#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererDx.h"
#include "cinder/ImageIo.h"
#include "cinder/Font.h"
#include "cinder/dx/dx.h"
#include "cinder/dx/DxVbo.h"
#include "cinder/dx/DxTexture.h"
#include "cinder/Camera.h"

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
	dx::TextureRef	mTexture;
	CameraPersp		mCamera;

	Font		mFont;
};

void BasicApp::setup()
{
	mFont = Font( "Arial", 18  );

	int totalVertices = VERTICES_X * VERTICES_Z;
	int totalQuads = ( VERTICES_X - 1 ) * ( VERTICES_Z - 1);

	cinder::dx::VboMesh::Layout layout;
	layout.setStaticIndices();
	layout.setDynamicPositions();
	layout.setStaticTexCoords2d();
	mVboMesh = dx::VboMesh( totalVertices, totalQuads * 4, layout, true );

	// buffer our static data - the texcoords and the indices
	vector<uint32_t> indices;
	vector<Vec2f> texCoords;
	vector<Vec3f> normals;
	vector<ColorA> colors;

	for( int x = 0; x < VERTICES_X; ++x ) {
		for( int z = 0; z < VERTICES_Z; ++z ) {
			// create a quad for each vertex, except for along the bottom and right edges
			if( ( x + 1 < VERTICES_X ) && ( z + 1 < VERTICES_Z ) ) {
				indices.push_back( (x+0) * VERTICES_Z + (z+0) );
				indices.push_back( (x+1) * VERTICES_Z + (z+0) );
				indices.push_back( (x+1) * VERTICES_Z + (z+1) );
				indices.push_back( (x+0) * VERTICES_Z + (z+1) );
			}
			// the texture coordinates are mapped to [0.0, 1.0]
			texCoords.push_back( Vec2f( 1.5*x / (float)(VERTICES_X + 0), (z+1) / (float)VERTICES_Z ) );
			normals.push_back(Vec3f(0, 0, 0));
			colors.push_back(ColorA(1, 1, 1, 0.2f));
		}
	}

	mVboMesh.bufferIndices( indices );
	mVboMesh.bufferTexCoords2d( 0, texCoords );

	// make a second VBO that uses the statics from the first
	mVboMesh2 = dx::VboMesh( totalVertices, totalQuads * 4, mVboMesh.getLayout(), true );
	
	mVboMesh2.setTexCoordOffset( 0, mVboMesh.getTexCoordOffset( 0 ) );
	mVboMesh2.bufferIndices( indices );
	mVboMesh2.bufferTexCoords2d( 0, texCoords );

	mTexture = dx::Texture::create( loadImage( loadAsset( "testPattern.png" ) ) );
}

void BasicApp::update()
{
	const float timeFreq = 2.50f;
	const float zFreq = 2.0f;
	const float xFreq = 5.0f;
	float offset = getElapsedSeconds() * timeFreq;

	dx::VboMesh::VertexIter iter = mVboMesh.mapVertexBuffer();
	for ( int x = 0; x < VERTICES_X; ++x ) {
		for ( int z = 0; z < VERTICES_Z; ++z) {
			float height = sin( z / (float)VERTICES_Z * zFreq + x / (float)VERTICES_X * xFreq + offset ) / 4.0f;
			iter.setPosition( Vec3f( x / (float)VERTICES_X, height, z / (float)VERTICES_Z ) );
			++iter;
		}
	}

	// dynmaically generate our new positions based on a simple sine wave for mesh2
	dx::VboMesh::VertexIter iter2 = mVboMesh2.mapVertexBuffer();
	for( int x = 0; x < VERTICES_X; ++x ) {
		for( int z = 0; z < VERTICES_Z; ++z ) {
			float height = sin( z / (float)VERTICES_Z * zFreq * 2 + x / (float)VERTICES_X * xFreq * 2 + offset ) / 8.0f;
			iter2.setPosition( Vec3f( x / (float)VERTICES_X, height, z / (float)VERTICES_Z ) + Vec3f( 0, 0.7, 0 ) );
			++iter2;
		}
	}
}

void BasicApp::draw()
{
	dx::clear( Color( 0.15f, 0.15f, 0.15f ), true );
	dx::enableAlphaBlending();

	dx::pushMatrices();
	dx::setMatrices( mCamera );

	dx::scale( Vec3f(10, 10, 10 ) );
	mTexture->bind();
	dx::draw( mVboMesh );
	dx::draw( mVboMesh2 );

	dx::popMatrices();


	std::stringstream s;
	s << "Framerate:" << getAverageFps();
	dx::color( Color::white() );
	dx::drawString(s.str(),Vec2f(10.0f,10.0f),Color(0.0f, 0.0f, 1.0f) ,mFont);
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )