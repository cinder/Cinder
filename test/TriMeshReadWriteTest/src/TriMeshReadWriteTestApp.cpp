#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Sphere.h"
#include "cinder/ObjLoader.h"
#include "cinder/ImageIo.h"
#include "cinder/ip/Checkerboard.h"

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
	void	draw() override;
	
	MayaCamUI		mMayaCam;
	TriMeshRef		mMesh;
	gl::BatchRef	mBatch;
	gl::GlslProgRef	mGlsl;
	gl::TextureRef	mCheckerTexture;
};

void TriMeshReadWriteTestApp::setup()
{	
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
	mGlsl->uniform( "uTex0", 0 );

	CameraPersp initialCam;
	initialCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1, 10000 );
	mMayaCam.setCurrentCam( initialCam );

	testObjFileWriteRead();

	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void TriMeshReadWriteTestApp::testObjFileWriteRead()
{
	Timer t( false );

	const string modelBinName = "model.bin";

#if defined( CINDER_MSW )
	fs::path writePath = getAppPath();
#else
	fs::path writePath = getAppPath().parent_path();
#endif
	
	// write
	t.start();
	ObjLoader loader( loadResource( RES_8LBS_OBJ ) );
	mMesh = TriMesh::create( loader );
	t.stop();
	console() << "Reading OBJ file took " << t.getSeconds() << " seconds." << std::endl;

	if( !mMesh->hasNormals() )
		mMesh->recalculateNormals();

	t.start();
	mMesh->write( writeFile( writePath / modelBinName, false ), { geom::POSITION, geom::TEX_COORD_0 } );
	t.stop();
	console() << "Writing binary file took " << t.getSeconds() << " seconds." << std::endl;

	// read
	mMesh = TriMesh::create();

	t.start();
	mMesh->read( loadFile( writePath / modelBinName ) );
	t.stop();
	console() << "Reading binary file took " << t.getSeconds() << " seconds." << std::endl;

	if( !mMesh->hasNormals() )
		mMesh->recalculateNormals();

	mBatch = gl::Batch::create( *mMesh, mGlsl );

	mCheckerTexture = gl::Texture::create( ip::checkerboard( 512, 512, 32 ) );
	mCheckerTexture->bind( 0 );
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

void TriMeshReadWriteTestApp::keyDown( KeyEvent event )
{
}

void TriMeshReadWriteTestApp::draw()
{
	gl::clear( Color( 0.45f, 0.22f, 0.1f ) );

	gl::setMatrices( mMayaCam.getCamera() );

	mBatch->draw();
}


CINDER_APP( TriMeshReadWriteTestApp, RendererGl )