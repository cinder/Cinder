#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MotionBlurVelocityBufferApp : public AppNative {
  public:
	void prepareSettings( Settings *settings );
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
  private:
	gl::TextureRef	mBackground;
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
}

void MotionBlurVelocityBufferApp::mouseDown( MouseEvent event )
{
}

void MotionBlurVelocityBufferApp::update()
{
}

void MotionBlurVelocityBufferApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::draw( mBackground, getWindowBounds() );
}

CINDER_APP_NATIVE( MotionBlurVelocityBufferApp, RendererGl )
