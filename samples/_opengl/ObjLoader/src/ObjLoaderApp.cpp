#include "Resources.h"

#include "cinder/ObjLoader.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Arcball.h"
#include "cinder/CameraUi.h"
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

	void	mouseDown( MouseEvent event ) override;
	void	mouseDrag( MouseEvent event ) override;
	void	keyDown( KeyEvent event ) override;

	void	loadObjFile( const fs::path &filePath );
	void	frameCurrentObject();
	void	draw() override;
	
	Arcball			mArcball;
	CameraUi		mCamUi;
	CameraPersp		mCam;
	TriMeshRef		mMesh;
	Sphere			mBoundingSphere;
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

	mCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1, 10000 );
	mCamUi = CameraUi( &mCam );

	mCheckerTexture = gl::Texture::create( ip::checkerboard( 512, 512, 32 ) );
	mCheckerTexture->bind( 0 );

	loadObjFile( getAssetPath( "8lbs.obj" ) );
	mArcball = Arcball( &mCam, mBoundingSphere );
}

void ObjLoaderApp::mouseDown( MouseEvent event )
{
	if( event.isMetaDown() )
		mCamUi.mouseDown( event );
	else
		mArcball.mouseDown( event );
}

void ObjLoaderApp::mouseDrag( MouseEvent event )
{
	if( event.isMetaDown() )
		mCamUi.mouseDrag( event );
	else
		mArcball.mouseDrag( event );
}

void ObjLoaderApp::loadObjFile( const fs::path &filePath )
{
	ObjLoader loader( (DataSourceRef)loadFile( filePath ) );
	mMesh = TriMesh::create( loader );
	if( ! loader.getAvailableAttribs().count( geom::NORMAL ) )
		mMesh->recalculateNormals();
	mBatch = gl::Batch::create( *mMesh, mGlsl );
	
	mBoundingSphere = Sphere::calculateBoundingSphere( mMesh->getPositions<3>(), mMesh->getNumVertices() );
	mArcball.setSphere( mBoundingSphere );
}

void ObjLoaderApp::frameCurrentObject()
{
	mCam = mCam.calcFraming( mBoundingSphere );
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
	gl::enableDepthWrite();
	gl::enableDepthRead();
	
	gl::clear( Color( 0.0f, 0.1f, 0.2f ) );

	gl::setMatrices( mCam );

	gl::pushMatrices();
		gl::rotate( mArcball.getQuat() );
		mBatch->draw();
	gl::popMatrices();
}


CINDER_APP( ObjLoaderApp, RendererGl )