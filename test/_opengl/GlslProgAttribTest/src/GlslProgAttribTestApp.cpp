#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/GeomIo.h"

#include "cinder/gl/Batch.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Vbo.h"
#include "cinder/CameraUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int NUM_CUBES = 100;

class GlslProgAttribTestApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override { mCamUi.mouseDown( event ); }
	void mouseDrag( MouseEvent event ) override { mCamUi.mouseDrag( event ); }
	void update() override;
	void draw() override;
	
	void setupBuffers();
	
	gl::BatchRef		mCube;
	gl::GlslProgRef		mGlsl;
	gl::VboRef			mModelMatricesVbo;
	
	std::vector<mat4>	mModelMatricesCpu;
	
	CameraPersp			mCam;
	CameraUi			mCamUi;
};

void GlslProgAttribTestApp::setup()
{
	mModelMatricesCpu.resize( NUM_CUBES );
	
	mGlsl = gl::GlslProg::create( gl::GlslProg::Format()
								 .vertex( loadAsset( "InstancedModelMatrix.vert" ) )
								 .fragment( loadAsset( "InstancedModelMatrix.frag" ) ) );
	setupBuffers();
	
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), .01f, 1000.0f );
	mCam.lookAt( vec3( 0, 0, 10 ), vec3( 0 ) );
	mCamUi.setCamera( &mCam );
	
	gl::enableDepthRead();
	gl::enableDepthWrite();
}

void GlslProgAttribTestApp::setupBuffers()
{
	mModelMatricesVbo = gl::Vbo::create( GL_ARRAY_BUFFER,
										mModelMatricesCpu.size() * sizeof(mat4),
										nullptr,
										GL_DYNAMIC_DRAW );
	geom::BufferLayout layout;
	layout.append( geom::CUSTOM_0, 16, sizeof(mat4), 0, 1 );
	
	auto cubeVboMesh = gl::VboMesh::create( geom::Cube().size( vec3( 1 ) ).colors() );
	cubeVboMesh->appendVbo( layout, mModelMatricesVbo );
	
	mCube = gl::Batch::create( cubeVboMesh, mGlsl, { { geom::CUSTOM_0, "aModelMatrix" } } );
}

void GlslProgAttribTestApp::update()
{
	int i = 0;
	float seperation = 8.0f;
	for( auto & mat : mModelMatricesCpu ) {
		float zOffset	= ( ( ( float(i) / NUM_CUBES ) - 0.5f ) * seperation ) + ( seperation * .1 );
		float timeVal	= getElapsedSeconds() + i;
		float sinVal	= sin( timeVal );
		float cosVal	= cos( timeVal );
		
		auto position	= vec3( cosVal, sinVal, zOffset );
		auto rotation	= quat( vec3( 0, timeVal, 0 ) );
		auto scale		= vec3( ( sinVal + 1.0f ) / 2.0f + .3f  );
		
		mat = mat4();
		mat *= ci::translate( position );
		mat *= ci::toMat4( rotation );
		mat *= ci::scale( scale );
		i++;
	}
	
	mModelMatricesVbo->bufferSubData( 0,
									 mModelMatricesCpu.size() * sizeof(mat4),
									 mModelMatricesCpu.data() );
}

void GlslProgAttribTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatrices( mCam );
	mCube->drawInstanced( mModelMatricesCpu.size() );
}

CINDER_APP( GlslProgAttribTestApp, RendererGl, []( App::Settings *settings ){ settings->setMultiTouchEnabled( false ); } )
