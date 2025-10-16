#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/CameraUi.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/CinderImGui.h"
#include "cinder/Filesystem.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CubeMapLayoutApp : public App {
  public:
	static void prepareSettings ( Settings * settings );
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void draw() override;

    std::vector<gl::TextureCubeMapRef> mCubeMaps;
    int mSelectedCubeMap = 0;
    std::vector<std::string> mCubeMapLabels;
    gl::BatchRef mCube;
    CameraUi mMayaCam;
    CameraPersp mMayaCamPersp;
};

void CubeMapLayoutApp::prepareSettings( Settings * settings )
{
	settings->setWindowSize( 1280, 720 );
}

void CubeMapLayoutApp::setup()
{
	ImGui::Initialize();

	gl::enableDepthRead();
	gl::enableDepthWrite();

	gl::GlslProgRef shader = gl::GlslProg::create( loadAsset( "sky_box.vert" ), loadAsset( "sky_box.frag" ) );
	shader->uniform( "uCubeMapTex", 0 );

	mCube = gl::Batch::create( geom::Cube().size( vec3( 100.0f ) ), shader );

	mSelectedCubeMap = 0;
	auto pushCubeMap = [&]( const std::string &label, const std::string &assetPath ) {
		auto imageSource = loadImage( loadAsset( assetPath ) );
		gl::TextureCubeMap::Format format;
		if( fs::path( assetPath ).extension() == ".hdr" ) {
			format.internalFormat( GL_RGB16F );
			format.setDataType( GL_FLOAT );
		}
		mCubeMaps.push_back( gl::TextureCubeMap::create( imageSource, format ) );
		mCubeMapLabels.push_back( label );
	};

	pushCubeMap( "Horizontal Cross", "horizontal_cross.jpg" );
	pushCubeMap( "Vertical Cross", "vertical_cross.png" );
	pushCubeMap( "Vertical Cross Layout", "vertical_cross_layout.png" );
	pushCubeMap( "Horizontal Strip", "horizontal.png" );
	pushCubeMap( "Vertical Strip (HDR)", "vertical.hdr" );

	mMayaCamPersp = CameraPersp( getWindowWidth(), getWindowHeight(), 60.0, 0.1f, 1000.0f );
	mMayaCamPersp.setEyePoint( vec3( 0, 0, 10 ) );
	mMayaCamPersp.lookAt( vec3(0) );

	mMayaCam.setCamera( &mMayaCamPersp );
}




void CubeMapLayoutApp::keyDown( KeyEvent event )
{
	(void)event;
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

	if( ! mCubeMaps.empty() ) {
		ImGui::ScopedWindow window( "Cubemap Layout" );
		std::string currentLabel = mCubeMapLabels[mSelectedCubeMap];
		if( ImGui::BeginCombo( "Layout", currentLabel.c_str() ) ) {
			for( int i = 0; i < (int)mCubeMaps.size(); ++i ) {
				bool selected = ( i == mSelectedCubeMap );
				if( ImGui::Selectable( mCubeMapLabels[i].c_str(), selected ) ) {
					mSelectedCubeMap = i;
				}
				if( selected )
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Text( "Use mouse + CameraUi to orbit, pan, and dolly." );
	}
}

CINDER_APP( CubeMapLayoutApp, RendererGl, &CubeMapLayoutApp::prepareSettings )
