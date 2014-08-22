#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/GeomIo.h"

#include "cinder/Rand.h"

#include "BlurrableThings.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MotionBlurVelocityBufferApp : public AppNative {
  public:
	void prepareSettings( Settings *settings ) override;
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
  private:
	gl::TextureRef					mBackground;
	std::vector<BlurrableMeshRef>	mMeshes;
	bool							mBlurEnabled = false;
	bool							mPaused = false;
};

void MotionBlurVelocityBufferApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1280, 720 );

	#if defined( CINDER_COCOA_TOUCH ) || defined( CINDER_COCOA_TOUCH_SIMULATOR )
		getSignalSupportedOrientations().connect( [] { return InterfaceOrientation::LandscapeAll; } );
	#endif // COCOA_TOUCH
}

void MotionBlurVelocityBufferApp::setup()
{
	mBackground = gl::Texture::create( loadImage( loadAsset( "background.jpg" ) ) );

	for( int i = 0; i < 20; ++i )
	{	// create some randomized geometry
		vec3 pos = vec3( randFloat( 200.0f, getWindowWidth() - 200.0f ), randFloat( 150.0f, getWindowHeight() - 150.0f ), randFloat( -50.0f, 10.0f ) );
		float base = randFloat( 50.0f, 100.0f );
		float height = randFloat( 100.0f, 300.0f );
		auto mesh = make_shared<BlurrableMesh>( gl::VboMesh::create( geom::Cone().height( height ).base( base ) ), pos );
		mesh->setAxis( randVec3f() );
		mesh->setColor( ColorA( CM_HSV, randFloat( 0.05f, 0.33f ), 1.0f, 1.0f ) );
		mesh->setOscillation( vec3( randFloat( -10.0f, 10.0f ), randFloat( -10.0f, 10.0f ), randFloat( -40.0f, 10.0f ) ) );
		mesh->setTheta( randFloat( M_PI * 2 ) );
		mMeshes.push_back( mesh );
	}
}

void MotionBlurVelocityBufferApp::keyDown( KeyEvent event )
{
	switch ( event.getCode() )
	{
		case KeyEvent::KEY_SPACE:
			mPaused = ! mPaused;
		break;
		case KeyEvent::KEY_b:
			mBlurEnabled = ! mBlurEnabled;
		break;
		default:
		break;
	}
}

void MotionBlurVelocityBufferApp::update()
{
	if( ! mPaused )
	{
		for( auto &mesh : mMeshes ) {
			mesh->update( 1.0f / 60.0f );
		}
	}
}

void MotionBlurVelocityBufferApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp( getWindowSize(), 60.0f, 1.0f, 5000.0f );

	gl::draw( mBackground, getWindowBounds() );

	if( mBlurEnabled )
	{	// render with motion blur

		// draw into velocity buffer
		// draw into color buffer
		// draw to screen
	}
	else
	{
		for( auto &mesh : mMeshes )
		{
			gl::ScopedModelMatrix model;
			gl::ScopedColor meshColor( mesh->getColor() );
			gl::multModelMatrix( mesh->getTransform() );
			gl::draw( mesh->getMesh() );
		}
	}
}

CINDER_APP_NATIVE( MotionBlurVelocityBufferApp, RendererGl )
