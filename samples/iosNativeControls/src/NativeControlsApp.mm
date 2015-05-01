// NativeControls sample by Rich Eakin
// This sample demonstrates how to integrate your app with native iOS controls and view controllers.
// It demostrates the following:
//
// - how to specify your own root UIViewController and later add CinderView's to it
// - how add a CinderView as a child of another ViewController
// - how to connect native controls back up to your App class so your callbacks are in C++
//
// Note: this sample is compiled with ARC enabled

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/app/CinderViewCocoaTouch.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Shader.h"
#include "cinder/Camera.h"
#include "cinder/gl/Texture.h"
#include "cinder/Font.h"

#include "NativeViewController.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class NativeControlsApp : public App {
  public:
	void prepareSettings( Settings *settings );
	void setup();
	void draw();

  private:
	void setupVisuals();
	void infoTapped();

	NativeViewController*	mNativeController;
	gl::TextureRef			mTex;
};

void NativeControlsApp::prepareSettings( Settings *settings )
{
	mNativeController = [NativeViewController new];
	settings->prepareWindow( Window::Format().rootViewController( mNativeController ) );
}

void NativeControlsApp::setup()
{
	setupVisuals();

	// Example of how to add Cinder's UIViewController to your native root UIViewViewController
	[mNativeController addCinderViewToFront];

	// Example of how to add Cinder's UIView to your view heirarchy (comment above out first)
//	[mNativeController addCinderViewAsBarButton];

	// Example of how to add a std::function callback from a UIControl (NativeViewController's info button in the upper right)
	[mNativeController setInfoButtonCallback: bind( &NativeControlsApp::infoTapped, this ) ];
}

void NativeControlsApp::setupVisuals()
{
	// Create a blue-green gradient as an OpenGL texture
	Surface8u surface( 256, 256, false );
	Surface8u::Iter iter = surface.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			iter.r() = 0;
			iter.g() = iter.x();
			iter.b() = iter.y();
		}
	}

	mTex = gl::Texture::create( surface );
	gl::enableDepthRead();
	gl::enableDepthWrite();
}

void NativeControlsApp::infoTapped()
{
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Bonzo!!"
													message:@"This button callback was triggered by a std::function."
												   delegate:nil
										  cancelButtonTitle:@"Ok"
										  otherButtonTitles:nil];
	[alert show];
}

void NativeControlsApp::draw()
{
	gl::clear( Color( 0, 0, 0.3f ) );

	CameraPersp cam;
	cam.lookAt( vec3( 3, 2, -3 ), vec3( 0 ) );
	cam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	gl::setMatrices( cam );

	gl::bindStockShader( gl::ShaderDef().texture() );
	gl::ScopedTextureBind textureScope( mTex );
	gl::ScopedModelMatrix modelScope;

	gl::rotate( getElapsedSeconds() * 1.5f, vec3( 1 ) );
	gl::drawCube( vec3( 0 ), vec3( 2 ) );
}

CINDER_APP( NativeControlsApp, RendererGl )
