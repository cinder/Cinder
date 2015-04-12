#include "Resources.h"

#include "cinder/ObjLoader.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Arcball.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Sphere.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/ip/Checkerboard.h"

using namespace ci;
using namespace ci::app;

#include <list>
using std::list;

class ObjLoaderApp : public App {
  public:
	void	setup() override;
	void	resize() override;

	void	keyDown( KeyEvent event ) override;

	void	loadObjFile( const fs::path &filePath );
	void	frameCurrentObject();
	void	draw() override;
	
	MayaCamUI		mMayaCam;
	TriMeshRef		mMesh;
	gl::BatchRef	mBatch;
	gl::GlslProgRef	mGlsl;
	gl::TextureRef	mCheckerTexture;
};

void ObjLoaderApp::setup()
{	
#if defined( CINDER_GL_ES )
	mGlsl = gl::GlslProg::create( loadAsset( "shader_es2.vert" ), loadAsset( "shader_es2.frag" ) );
#else
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
#endif
	mGlsl->uniform( "uTex0", 0 );

	CameraPersp initialCam;
	initialCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1, 10000 );
	mMayaCam.setCurrentCam( initialCam );
	mMayaCam.connect( getWindow() );

	mCheckerTexture = gl::Texture::create( ip::checkerboard( 512, 512, 32 ) );
	mCheckerTexture->bind( 0 );

	loadObjFile( getAssetPath( "8lbs.obj" ) );

	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void ObjLoaderApp::resize()
{
	mMayaCam.setAspectRatio( getWindowAspectRatio() );
}

void ObjLoaderApp::loadObjFile( const fs::path &filePath )
{
	ObjLoader loader( (DataSourceRef)loadFile( filePath ) );
	mMesh = TriMesh::create( loader );
	if( ! loader.getAvailableAttribs().count( geom::NORMAL ) )
		mMesh->recalculateNormals();
	mBatch = gl::Batch::create( *mMesh, mGlsl );
}

void ObjLoaderApp::frameCurrentObject()
{
	Sphere boundingSphere = Sphere::calculateBoundingSphere( mMesh->getPositions<3>(), mMesh->getNumVertices() );
	
	mMayaCam.setCurrentCam( mMayaCam.getCamera().calcFraming( boundingSphere ) );
}

void ObjLoaderApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'o' ) {
		fs::path path = getOpenFilePath();
		if( ! path.empty() ) {
			loadObjFile( path );
		}
	}
	else if( event.getChar() == 'f' ) {
		frameCurrentObject();
	}
}

void ObjLoaderApp::draw()
{
	gl::clear( Color( 0.0f, 0.1f, 0.2f ) );
	gl::setMatrices( mMayaCam.getCamera() );

	mBatch->draw();
}


CINDER_APP( ObjLoaderApp, RendererGl, []( App::Settings *settings ) {
	settings->setMultiTouchEnabled( false );
} )