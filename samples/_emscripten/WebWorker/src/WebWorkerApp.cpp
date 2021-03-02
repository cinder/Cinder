#include "cinder/emscripten/Worker.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/GlslProg.h"
#include "cube.h"

using namespace ci;
using namespace ci::app;
using namespace ci::em;
using namespace std;


/**
 A perhaps inefficent but nevertheless hopefully simple
 demonstration of how WebWorkers could be utilized.

 A Cube's position is updated along a sphere with the new
 position being calculated inside of a Web Worker.

 Note that this makes use of C++ based workers though you could theoretically
 write your own in Javascript as well via the embind api.
*/


vec3 cubePosition;

class WorkerApp : public App
{

  public:
  vector<ci::vec3> positions;
  vector<uint16_t> indices;
	void draw() override;
  void setup() override;
  void onLoad();
  CameraPersp mCam;
  CameraUi mCamUi;
  ci::gl::BatchRef mBatch;
  ci::gl::GlslProgRef mShader;
  ci::gl::VboMeshRef mMesh;
  Worker worker;
};

void prepareSettings( WorkerApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void workerCallback( char*data,int size,void* args )
{
  ci::vec3 * vec = ( ci::vec3* )data;
  auto pt = *vec;
  cubePosition = *vec;
}

void WorkerApp::setup()
{
  float fov = 45 * ( 3.14149 / 10 );
	float aspect = ci::app::getWindowAspectRatio();
	float near = 0.1;
	float far = 40000.0;
	ci::vec3 eye = ci::vec3( 0, 0, 500 );
	ci::vec3 target = ci::vec3( 0, 0, 0 );

  mCam = CameraPersp( getWindowWidth(), getWindowHeight(), fov, near, far );
  mCam.lookAt( eye, target );
  mCamUi = CameraUi( &mCam );

  mMesh = gl::VboMesh::create( geom::Cube().size( vec3( 10 ) ) );

  mShader = gl::getStockShader( gl::ShaderDef().color() );
  mBatch = gl::Batch::create( mMesh,mShader );

  worker.loadWorker("worker.js");
  worker.setOnMessageCallback(workerCallback);
  worker.postMessage( "onmessage",( char* )glm::value_ptr( cubePosition ),sizeof( vec3 ) );
}

void WorkerApp::draw()
{
  gl::clear( Color( 0,0,0 ) );
  gl::setMatrices( mCam );
  worker.postMessage( "onmessage",( char* )glm::value_ptr( cubePosition ),sizeof( vec3 ) );

  gl::translate( cubePosition );
  mBatch->draw();
}


// This line tells Cinder to actually create and run the application.
CINDER_APP( WorkerApp, RendererGl, prepareSettings )
