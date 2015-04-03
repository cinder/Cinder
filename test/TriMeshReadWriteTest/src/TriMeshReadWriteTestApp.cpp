#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Sphere.h"
#include "cinder/ObjLoader.h"
#include "cinder/ImageIo.h"

#include "Resources.h"

using namespace std;
using namespace ci;
using namespace ci::app;

class TriMeshReadWriteTestApp : public App {
  public:
	void	setup() override;
	void	resize() override;

	void	mouseDown( MouseEvent event ) override;
	void	mouseDrag( MouseEvent event ) override;
	void	keyDown( KeyEvent event ) override;

	void	testObjFileWriteRead();
	void	loadObjFile( const fs::path &filePath );
	void	frameCurrentObject();
	void	draw() override;
	
	MayaCamUI		mMayaCam;
	TriMeshRef		mMesh;
	gl::BatchRef	mBatch;
	gl::GlslProgRef	mGlsl;
};

void TriMeshReadWriteTestApp::setup()
{	
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );

	CameraPersp initialCam;
	initialCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1, 10000 );
	mMayaCam.setCurrentCam( initialCam );

	testObjFileWriteRead();

	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void TriMeshReadWriteTestApp::testObjFileWriteRead()
{
	const string modelObjName = "8lbs.obj";
	const string modelBinName = "model.bin";

	ObjLoader loader( loadAsset( modelObjName ) );
	mMesh = TriMesh::create( loader );
	if( ! loader.getAvailableAttribs().count( geom::NORMAL ) )
		mMesh->recalculateNormals();

#if defined( CINDER_MSW )
	fs::path writePath = getAppPath();
#else
	fs::path writePath = getAppPath().parent_path();
#endif

	mMesh->write( writeFile( writePath / modelBinName, false ) );
	mMesh->clear();
	mMesh->read( loadFile( writePath / modelBinName ) );

	mBatch = gl::Batch::create( *mMesh, mGlsl );
}

void TriMeshReadWriteTestApp::resize()
{
	auto cam = mMayaCam.getCamera();
	cam.setAspectRatio( getWindowAspectRatio() );
	mMayaCam.setCurrentCam( cam );
}

void TriMeshReadWriteTestApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void TriMeshReadWriteTestApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
}

void TriMeshReadWriteTestApp::frameCurrentObject()
{
	Sphere boundingSphere = Sphere::calculateBoundingSphere( mMesh->getPositions<3>(), mMesh->getNumVertices() );
	
	mMayaCam.setCurrentCam( mMayaCam.getCamera().getFrameSphere( boundingSphere, 100 ) );
}

void TriMeshReadWriteTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		frameCurrentObject();
	}
}

void TriMeshReadWriteTestApp::draw()
{
	gl::clear( Color( 0.0f, 0.1f, 0.2f ) );

	gl::setMatrices( mMayaCam.getCamera() );

	mBatch->draw();
}


CINDER_APP( TriMeshReadWriteTestApp, RendererGl )