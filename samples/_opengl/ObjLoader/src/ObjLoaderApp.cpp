#include "Resources.h"

#include "cinder/ObjLoader.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Arcball.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Sphere.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Batch.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Shader.h"

using namespace ci;
using namespace ci::app;

#include <list>
using std::list;

class ObjLoaderApp : public App {
  public:
	void	setup() override;
	void	resize() override;

	void	mouseDown( MouseEvent event ) override;
	void	mouseDrag( MouseEvent event ) override;
	void	keyDown( KeyEvent event ) override;

	void	loadObjFile( const fs::path &filePath );
	void	frameCurrentObject();
	void	draw() override;
	
	Arcball			mArcball;
	MayaCamUI		mMayaCam;
	TriMeshRef		mMesh;
	gl::BatchRef	mBatch;
	gl::GlslProgRef	mGlsl;
};

void ObjLoaderApp::setup()
{	
#if defined( CINDER_GL_ES )
	mGlsl = gl::GlslProg::create( loadAsset( "shader_es2.vert" ), loadAsset( "shader_es2.frag" ) );
#else
	mGlsl = gl::GlslProg::create( loadAsset( "shader.vert" ), loadAsset( "shader.frag" ) );
#endif

	CameraPersp initialCam;
	initialCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1, 10000 );
	mMayaCam.setCurrentCam( initialCam );

	loadObjFile( getAssetPath( "8lbs.obj" ) );
}

void ObjLoaderApp::resize()
{
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( vec2( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f ) );
	mArcball.setRadius( 150 );
}

void ObjLoaderApp::mouseDown( MouseEvent event )
{
	if( event.isAltDown() )
		mMayaCam.mouseDown( event.getPos() );
	else
		mArcball.mouseDown( event.getPos() );
}

void ObjLoaderApp::mouseDrag( MouseEvent event )
{
	if( event.isAltDown() )
		mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() );
	else
		mArcball.mouseDrag( event.getPos() );
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
	
	mMayaCam.setCurrentCam( mMayaCam.getCamera().getFrameSphere( boundingSphere, 100 ) );
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

	gl::setMatrices( mMayaCam.getCamera() );

	gl::pushMatrices();
		gl::rotate( mArcball.getQuat() );
		mBatch->draw();
	gl::popMatrices();
}


CINDER_APP( ObjLoaderApp, RendererGl )