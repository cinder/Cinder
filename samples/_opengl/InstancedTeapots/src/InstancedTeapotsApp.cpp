#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ObjLoader.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class InstancedTeapotsApp : public App {
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
	mCam.lookAt( vec3( 0, CAMERA_Y_RANGE.first, 0 ), vec3( 0 ) );
	
	mTexture = gl::Texture::create( loadImage( loadAsset( "texture.jpg" ) ), gl::Texture::Format().mipmap() );
#if defined( CINDER_GL_ES_2 )
	mGlsl = gl::GlslProg::create( loadAsset( "shader_es2.vert" ), loadAsset( "shader_es2.frag" ) );
#elif defined( CINDER_GL_ES_3 )
	mGlsl = gl::GlslProg::create(loadAsset("shader_es3.vert"), loadAsset("shader_es3.frag"));
#else
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
#endif

	gl::VboMeshRef mesh = gl::VboMesh::create( geom::Teapot().subdivisions( 4 ) );

	// create an array of initial per-instance positions laid out in a 2D grid
	std::vector<vec3> positions;
	for( size_t potX = 0; potX < NUM_INSTANCES_X; ++potX ) {
		for( size_t potY = 0; potY < NUM_INSTANCES_Y; ++potY ) {
			float instanceX = potX / (float)NUM_INSTANCES_X - 0.5f;
			float instanceY = potY / (float)NUM_INSTANCES_Y - 0.5f;
			positions.push_back( vec3( instanceX * vec3( DRAW_SCALE, 0, 0 ) + instanceY * vec3( 0, 0, DRAW_SCALE ) ) );
		}
	}
	
	// create the VBO which will contain per-instance (rather than per-vertex) data
	mInstanceDataVbo = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_DYNAMIC_DRAW );

	// we need a geom::BufferLayout to describe this data as mapping to the CUSTOM_0 semantic, and the 1 (rather than 0) as the last param indicates per-instance (rather than per-vertex)
	geom::BufferLayout instanceDataLayout;
	instanceDataLayout.append( geom::Attrib::CUSTOM_0, 3, 0, 0, 1 /* per instance */ );
	
	// now add it to the VboMesh we already made of the Teapot
	mesh->appendVbo( instanceDataLayout, mInstanceDataVbo );

	// and finally, build our batch, mapping our CUSTOM_0 attribute to the "vInstancePosition" GLSL vertex attribute
	mBatch = gl::Batch::create( mesh, mGlsl, { { geom::Attrib::CUSTOM_0, "vInstancePosition" } } );

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
	mCam.lookAt( vec3( 0, CAMERA_Y_RANGE.first + abs(sin( getElapsedSeconds() / 4)) * (CAMERA_Y_RANGE.second - CAMERA_Y_RANGE.first), 0 ), vec3( 0 ) );
	
	// update our instance positions; map our instance data VBO, write new positions, unmap
	vec3 *positions = (vec3*)mInstanceDataVbo->mapReplace();
	for( size_t potX = 0; potX < NUM_INSTANCES_X; ++potX ) {
		for( size_t potY = 0; potY < NUM_INSTANCES_Y; ++potY ) {
			float instanceX = potX / (float)NUM_INSTANCES_X - 0.5f;
			float instanceY = potY / (float)NUM_INSTANCES_Y - 0.5f;
			// just some nonsense math to move the teapots in a wave
			vec3 newPos(	instanceX * vec3( DRAW_SCALE, 0, 0 ) +
							instanceY * vec3( 0, 0, DRAW_SCALE ) +
							vec3( 0, 30, 0 ) * sinf( getElapsedSeconds() * 3 + instanceX * 3 + instanceY * 3 ) );
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

#if defined( CINDER_MSW ) && ! defined( CINDER_GL_ANGLE )
auto options = RendererGl::Options().version( 3, 3 ); // instancing functions are technically only in GL 3.3
#else
auto options = RendererGl::Options(); // implemented as extensions in Mac OS 10.7+
#endif
CINDER_APP( InstancedTeapotsApp, RendererGl( options ) )
