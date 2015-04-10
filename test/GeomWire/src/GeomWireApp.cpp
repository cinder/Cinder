#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/GeomIo.h"
#include "cinder/MayaCamUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class GeomWireApp : public App {
public:
	GeomWireApp() : mTransparent( false ) {}

	void setup() override;
	void update() override;
	void draw() override;

	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;

	void keyDown( KeyEvent event ) override;

	void resize() override;

private:
	bool         mTransparent;

	CameraPersp  mCamera;
	MayaCamUI    mMayaCam;

	gl::BatchRef  mSolid;
	gl::BatchRef  mWireframe;
};

void GeomWireApp::setup()
{
	auto shader = gl::context()->getStockShader( gl::ShaderDef().color() );

	//mSolid = gl::Batch::create( geom::Cube(), shader );
	//mWireframe = gl::Batch::create( geom::WireCube().subdivisions(2), shader );

	mSolid = gl::Batch::create( geom::Sphere().subdivisions( 72 ), shader );
	mWireframe = gl::Batch::create( geom::WireSphere().subdivisions( 72 ), shader );

	//mSolid = gl::Batch::create( geom::Cylinder().subdivisionsHeight( 4 ), shader );
	//mWireframe = gl::Batch::create( geom::WireCylinder().subdivisionsHeight( 4 ), shader );

	//mSolid = gl::Batch::create( geom::Cone().subdivisionsHeight( 4 ), shader );
	//mWireframe = gl::Batch::create( geom::WireCone().subdivisionsHeight( 4 ), shader );

	//
	mCamera.setPerspective( 35.0f, 1.0f, 0.05f, 100.0f );
	mCamera.setEyePoint( vec3( 0, 1, 5 ) );
	mCamera.setCenterOfInterestPoint( vec3( 0 ) );
}

void GeomWireApp::update()
{
}

void GeomWireApp::draw()
{
	gl::clear();
	gl::setMatrices( mCamera );

	gl::enableDepthRead();
	gl::enableDepthWrite();

	gl::color( Color( 1, 1, 1 ) );
	mWireframe->draw();

	if( mTransparent ) {
		gl::disableDepthWrite();
		gl::enableAlphaBlending();
	}
	else {
		gl::enableDepthWrite();
		gl::disableAlphaBlending();
	}

	gl::color( ColorA( 0.2f, 0.2f, 0.2f, 0.75f ) );
	mSolid->draw();

	gl::drawCoordinateFrame( 1.0f );
}

void GeomWireApp::mouseDown( MouseEvent event )
{
	mMayaCam.setCurrentCam( mCamera );
	mMayaCam.mouseDown( event );
}

void GeomWireApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event );
	mCamera = mMayaCam.getCamera();
}

void GeomWireApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
	case KeyEvent::KEY_f:
		setFullScreen( !isFullScreen() );
		break;
	case KeyEvent::KEY_ESCAPE:
		if( isFullScreen() )
			setFullScreen( false );
		else
			quit();
		break;
	case KeyEvent::KEY_t:
		mTransparent = !mTransparent;
		break;
	default:
		return;
	}

	event.setHandled();
}

void GeomWireApp::resize()
{
	mCamera.setAspectRatio( getWindowAspectRatio() );
}

CINDER_APP( GeomWireApp, RendererGl )
