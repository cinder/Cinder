#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/params/Params.h"
#include "cinder/CameraUi.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CubeMapLayoutApp : public App {
  public:
	static void prepareSettings ( Settings * settings );
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void draw() override;

	std::vector<gl::TextureCubeMapRef> mCubeMaps;
	int mSelectedCubeMap;
	gl::BatchRef mCube;
	params::InterfaceGlRef mParams;
	CameraUi mMayaCam;
	CameraPersp mMayaCamPersp;
};

void CubeMapLayoutApp::prepareSettings( Settings * settings )
{
	settings->setWindowSize( 1280, 720 );
}

void CubeMapLayoutApp::setup()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();

	mParams = params::InterfaceGl::create( "Settings", ivec2(200, 200) );
	mParams->addParam( "Cubemap", { "Horizontal Cross", "Vertical Cross", "Vertical Cross Layout", "Horizontal", "Vertical" }, &mSelectedCubeMap );

	gl::GlslProgRef shader = gl::GlslProg::create( loadAsset( "sky_box.vert" ), loadAsset( "sky_box.frag" ) );
	shader->uniform( "uCubeMapTex", 0 );

	mCube = gl::Batch::create( geom::Cube().size( vec3( 100.0f ) ), shader );

	mSelectedCubeMap = 0;
	mCubeMaps.push_back( gl::TextureCubeMap::create( loadImage( loadAsset( "horizontal_cross.jpg" ) ) ) );
	mCubeMaps.push_back( gl::TextureCubeMap::create( loadImage( loadAsset( "vertical_cross.png" ) ) ) );
	mCubeMaps.push_back( gl::TextureCubeMap::create( loadImage( loadAsset( "vertical_cross_layout.png" ) ) ) );
	mCubeMaps.push_back( gl::TextureCubeMap::create( loadImage( loadAsset( "horizontal.png" ) ) ) );
	mCubeMaps.push_back( gl::TextureCubeMap::create( loadImage( loadAsset( "vertical.hdr" ) ) ) );

	mMayaCamPersp = CameraPersp( getWindowWidth(), getWindowHeight(), 60.0, 0.1f, 1000.0f );
	mMayaCamPersp.setEyePoint( vec3( 0, 0, 10 ) );
	mMayaCamPersp.lookAt( vec3(0) );

	mMayaCam.setCamera( &mMayaCamPersp );
}

void CubeMapLayoutApp::mouseDown( MouseEvent event )
{
	mMayaCam.mouseDown( event.getPos() );
}

void CubeMapLayoutApp::mouseDrag( MouseEvent event )
{
	mMayaCam.mouseDrag( event.getPos(), event.isLeftDown(), event.isMiddleDown(), event.isRightDown() || event.isShiftDown() );
}

void CubeMapLayoutApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	{
		gl::setMatrices( mMayaCam.getCamera() );
		gl::ScopedTextureBind texture( mCubeMaps[mSelectedCubeMap] );
		mCube->draw();
	}

	mParams->draw();
}

CINDER_APP( CubeMapLayoutApp, RendererGl, &CubeMapLayoutApp::prepareSettings )
