#include "Resources.h"

#include "cinder/ObjLoader.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Arcball.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Sphere.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;

#include <list>
using std::list;

bool gDebug = false;

// We'll create a new Cinder Application by deriving from the BasicApp class
class ObjLoaderApp : public AppBasic {
 public:
	void	setup();
	void	resize( ResizeEvent event );

	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );

	void	frameCurrentObject();
	void	draw();
	
	Arcball			mArcball;
	MayaCamUI		mMayaCam;
	TriMesh			mMesh;
	gl::VboMesh		mVBO;
	gl::GlslProg	mShader;
	gl::Texture		mTexture;
};

void ObjLoaderApp::setup()
{
	ObjLoader loader( loadResource( RES_CUBE_OBJ )->createStream() );
	loader.load( &mMesh );
	mVBO = gl::VboMesh( mMesh );
	
	mTexture = gl::Texture( loadImage( loadResource( RES_IMAGE ) ) );
	mShader = gl::GlslProg( loadResource( RES_SHADER_VERT ), loadResource( RES_SHADER_FRAG ) );

	CameraPersp initialCam;
	initialCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1, 10000 );
	mMayaCam.setCurrentCam( initialCam );

	mTexture.bind();
	mShader.bind();
	mShader.uniform( "tex0", 0 );
}

void ObjLoaderApp::resize( ResizeEvent event )
{
	App::resize( event );
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( Vec2f( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f ) );
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

void ObjLoaderApp::frameCurrentObject()
{
	Sphere boundingSphere = Sphere::calculateBoundingSphere( mMesh.getVertices() );
	
	mMayaCam.setCurrentCam( mMayaCam.getCamera().getFrameSphere( boundingSphere, 100 ) );
}

void ObjLoaderApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'o' ) {
		std::string path = getOpenFilePath();
		if( ! path.empty() ) {
			ObjLoader loader( loadFileStream( path ) );
			loader.load( &mMesh, true );
			mVBO = gl::VboMesh( mMesh );
			console() << "Total verts: " << mMesh.getVertices().size() << std::endl;
		}
	}
	else if( event.getChar() == 's' ) {
		std::string path = getSaveFilePath( "output.trimesh" );
		if( ! path.empty() ) {
			console() << "Saving to " << path;
			mMesh.write( writeFile( path ) );
		}
	}
	else if( event.getChar() == 'f' ) {
		frameCurrentObject();
	}
	else if( event.getChar() == 'd' ) {
		gDebug = ! gDebug;
	}
}

void ObjLoaderApp::draw()
{
	gl::enableDepthWrite();
	gl::enableDepthRead();
	
	gl::clear( Color( 0.0f, 0.1f, 0.2f ) );
	glDisable( GL_CULL_FACE );

	gl::setMatrices( mMayaCam.getCamera() );

/*	Sphere boundingSphere = Sphere::calculateBoundingSphere( mMesh.getVertices() );
	glColor3f( 1.0f, 1.0f, 1.0f );
	gl::disableDepthWrite();
	mTexture->disable();
	mShader.unbind();
	gl::draw( boundingSphere, 30 );
	gl::enableDepthWrite();
*/
	mShader.bind();
	gl::pushMatrices();
		gl::rotate( mArcball.getQuat() );
		gl::draw( mVBO );
	gl::popMatrices();
}


CINDER_APP_BASIC( ObjLoaderApp, RendererGl )