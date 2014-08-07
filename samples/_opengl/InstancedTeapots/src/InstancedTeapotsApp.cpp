#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/ObjLoader.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class InstancedTeapotsApp : public AppNative {
  public:	
	void setup();
	void resize();
	void update();
	void draw();
	
	CameraPersp			mCam;
	gl::BatchRef		mBatch;
	gl::TextureRef		mTexture;
	gl::GlslProgRef		mGlsl;
	gl::VboRef			mInstanceDataVbo;
};

const int NUM_INSTANCES_X = 33;
const int NUM_INSTANCES_Y = 33;
const float DRAW_SCALE = 200;
const pair<float,float> CAMERA_Y_RANGE( 32, 80 );

void InstancedTeapotsApp::setup()
{
	mCam.lookAt( Vec3f( 0, CAMERA_Y_RANGE.first, 0 ), Vec3f::zero() );
	
	mTexture = gl::Texture::create( loadImage( loadAsset( "texture.jpg" ) ), gl::Texture::Format().mipmap() );
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );

	gl::VboMeshRef mesh = gl::VboMesh::create( geom::Teapot().subdivision( 4 ) );

	// create an array of initial per-instance positions laid out in a 2D grid
	std::vector<Vec3f> positions;
	for( size_t potX = 0; potX < NUM_INSTANCES_X; ++potX ) {
		for( size_t potY = 0; potY < NUM_INSTANCES_Y; ++potY ) {
			float instanceX = potX / (float)NUM_INSTANCES_X - 0.5f;
			float instanceY = potY / (float)NUM_INSTANCES_Y - 0.5f;
			positions.push_back( Vec3f( instanceX * Vec3f( DRAW_SCALE, 0, 0 ) + instanceY * Vec3f( 0, 0, DRAW_SCALE ) ) );
		}
	}
	
	// create the VBO which will contain per-instance (rather than per-vertex) data
	mInstanceDataVbo = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof(Vec3f), positions.data(), GL_DYNAMIC_DRAW );

	// we need a geom::BufferLayout to describe this data as mapping to the CUSTOM_0 semantic, and the 1 (rather than 0) as the last param indicates per-instance (rather than per-vertex)
	geom::BufferLayout instanceDataLayout;
	instanceDataLayout.append( geom::Attrib::CUSTOM_0, 3, 0, 0, 1 /* per instance */ );
	
	// now add it to the VboMesh we already made of the Teapot
	mesh->appendVbo( instanceDataLayout, mInstanceDataVbo );

	// and finally, build our batch, mapping our CUSTOM_0 attribute to the "vInstancePosition" GLSL vertex attribute
#if ! defined( CINDER_MSW )
	mBatch = gl::Batch::create( mesh, mGlsl, { { geom::Attrib::CUSTOM_0, "vInstancePosition" } } );
#else
	// unfortunately VC2012 doesn't support initializer lists yet
	gl::Batch::AttributeMapping mapping;
	mapping[geom::Attrib::CUSTOM_0] = "vInstancePosition";
	mBatch = gl::Batch::create( mesh, mGlsl, mapping );
#endif

	gl::enableDepthWrite();
	gl::enableDepthRead();
	
	mTexture->bind();	
}

void InstancedTeapotsApp::resize()
{
	// now tell our Camera that the window aspect ratio has changed
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	
	// and in turn, let OpenGL know we have a new camera
	gl::setMatrices( mCam );
}

void InstancedTeapotsApp::update()
{
	// move the camera up and down on Y
	mCam.lookAt( Vec3f( 0, CAMERA_Y_RANGE.first + abs(sin( getElapsedSeconds() / 4)) * (CAMERA_Y_RANGE.second - CAMERA_Y_RANGE.first), 0 ), Vec3f::zero() );	
	
	// update our instance positions; map our instance data VBO, write new positions, unmap
	Vec3f *positions = (Vec3f*)mInstanceDataVbo->map( GL_WRITE_ONLY );
	for( size_t potX = 0; potX < NUM_INSTANCES_X; ++potX ) {
		for( size_t potY = 0; potY < NUM_INSTANCES_Y; ++potY ) {
			float instanceX = potX / (float)NUM_INSTANCES_X - 0.5f;
			float instanceY = potY / (float)NUM_INSTANCES_Y - 0.5f;
			// just some nonsense math to move the teapots in a wave
			Vec3f newPos(	instanceX * Vec3f( DRAW_SCALE, 0, 0 ) +
							instanceY * Vec3f( 0, 0, DRAW_SCALE ) +
							Vec3f( 0, 30, 0 ) * sin( getElapsedSeconds() * 3 + instanceX * 3 + instanceY * 3 ) );
			*positions++ = newPos;
		}
	}
	mInstanceDataVbo->unmap();
}

void InstancedTeapotsApp::draw()
{
	gl::clear( Color::black() );
	gl::setMatrices( mCam );

	mBatch->drawInstanced( NUM_INSTANCES_X * NUM_INSTANCES_Y );
}

#if defined( CINDER_MSW )
auto options = RendererGl::Options().version( 3, 3 ); // instancing functions are technically only in GL 3.3
#else
auto options = RendererGl::Options(); // implemented as extensions in Mac OS 10.7+
#endif
CINDER_APP_NATIVE( InstancedTeapotsApp, RendererGl( options ) )