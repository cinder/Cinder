#include "cinder/emscripten/EmscriptenVideo.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/Camera.h"
#include <vector>
#include "dragdrop.h"

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace ci::em;
using namespace emscripten;

/**
 * An simple example to show two things. 
 * 1. How to playback video with Emscripten.
 * 2. How to use drag-n-drop functionality.
 * 
 * Drag a video file onto the canvas(the black box in the middle) and the video file should start playing.
 * Note that for simplicity, this sample skips over some things that you probably ought to be doing, 
 * such as checking for file compatibility with the current browser
 * in addition to ensuring the file you drop is actually a video file.
 */ 
class DragDropApp : public App 
{

  public:
	void draw() override;
  void setup() override;
  void onLoad();
  void onDropProcessed(emscripten::val e);
  CameraPersp mCam;
  gl::VboRef mInstancePosition,mInstanceUV;

  EmscriptenVideo video;
  ci::gl::VboMeshRef mMesh;
  ci::gl::BatchRef mBatch;
  ci::gl::GlslProgRef mShader;

  float numCount;
  int width;
  int height;
  int resolution = 1;

  DragDrop dropper;
};

void prepareSettings( DragDropApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void DragDropApp::onDropProcessed(emscripten::val e){
  auto url = val::global( "URL" ).call<val>( "createObjectURL" ,e);

  // NOTE: not checking to ensure we dropped a video - probably a good idea to do so
  video.setSource( url.as<string>() );
}

void DragDropApp::setup()
{
   
    std::function<void( emscripten::val )> func = std::bind( &DragDropApp::onDropProcessed, this, std::placeholders::_1) ;                                                                       
    dropper = DragDrop( "#canvas", func );

    width = app::getWindowWidth();
    height = app::getWindowHeight();

    float fov = 45 * ( 3.14149 / 10 );
		float aspect = ci::app::getWindowAspectRatio();
		float near = 0.1;
		float far = 40000.0;
		ci::vec3 eye = ci::vec3( 0, 0, 2000 );
		ci::vec3 target = ci::vec3( 0, 0, 0) ;

  	mCam = CameraPersp( getWindowWidth(), getWindowHeight(), fov, near, far );
  	mCam.lookAt( eye, target );

  // build positions
  vector<ci::vec3> positions;
  vector<ci::vec2> uvs;
  float sx = width;
	float sy = height;
	float resolution = 10;
	float nx = width / resolution;
	float ny = height / resolution;

	for ( int iy = 0; iy <= ny; iy++ )
  {
		for ( int ix = 0; ix <= nx; ix++ )
    {
			float u = ix / nx;
			float v = iy / ny;

      // find x,y,z with some corrections to center the final grid.
      float x = ( ix * resolution ) - ( sx / 2 );
      float y = ( iy * resolution * -1 ) + ( sy / 2 );

      uvs.push_back( vec2( u,v ) );
      positions.push_back( vec3( x,y,0 ) );
    }
  }

  numCount = positions.size();
  mInstancePosition = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof( vec3 ), positions.data(), GL_STATIC_DRAW );
  geom::BufferLayout instanceDataLayout;
  instanceDataLayout.append( geom::Attrib::CUSTOM_0, 3, 0, 0, 1 /* per instance */ );

  mInstanceUV = gl::Vbo::create( GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), uvs.data(), GL_STATIC_DRAW );
  geom::BufferLayout instanceDataLayout2;
  instanceDataLayout2.append( geom::Attrib::CUSTOM_1, 2, 0, 0, 1 /* per instance */ );

  // build mesh
  auto geo = geom::Cube().size( vec3( resolution ) );
  mMesh = gl::VboMesh::create( geo );

  mMesh->appendVbo( instanceDataLayout, mInstancePosition );
  mMesh->appendVbo( instanceDataLayout2, mInstanceUV );

  gl::GlslProg::Format fmt;
  fmt.vertex( app::loadAsset( "grid.vert" ) );
  fmt.fragment( app::loadAsset( "grid.frag" ) );
  mShader = gl::GlslProg::create( fmt );

  mBatch = gl::Batch::create( mMesh,mShader,{
    {geom::Attrib::CUSTOM_0, "instancePosition"},
    {geom::Attrib::CUSTOM_1, "instanceUV"}
  });
}

void DragDropApp::draw()
{
  gl::clear( Color( 0,0,0 ));

	// Clear the contents of the window. This call will clear
	// both the color and depth buffers.
  if( video.hasVideoLoaded() )
  {
    if( video.isPlaying() )
    {
      video.updateTexture();
    }
    else{
      video.play();
    }

    gl::ScopedTextureBind tex0( video.getTexture() );
    gl::setMatrices( mCam) ;

    mBatch->getGlslProg()->uniform( "uTex0",0 );
    mBatch->getGlslProg()->uniform( "time",( float )app::getElapsedSeconds() * 0.005f );
    mBatch->drawInstanced( numCount );
  }
}

CINDER_APP( DragDropApp, RendererGl, prepareSettings )
