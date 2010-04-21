#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FBOBasicApp : public AppBasic {
  public:
	virtual void	setup();
	virtual void	update();
	virtual void	draw();
	virtual void	keyDown( KeyEvent event );

	void			renderScene();
	
	gl::Fbo			mFbo;
	gl::GlslProg	mPixelateShader;
	gl::Texture		mCubeTexture;

	Matrix44f		mCubeRotation;
	
	static const int		FBO_WIDTH = 640, FBO_HEIGHT = 480;
};

void FBOBasicApp::setup()
{
	gl::Fbo::Format format;
	format.setSamples( 4 ); // enable 4x antialiasing
	mFbo = gl::Fbo( FBO_WIDTH, FBO_HEIGHT, format );

	// echo a little bit of info just for demonstration purposes
	console() << "Maximum # samples for FBOs: " << gl::Fbo::getMaxSamples() << endl;

	mPixelateShader = gl::GlslProg( loadResource( RES_PASS_THRU_VERT ), loadResource( RES_PIXELATE_FRAG ) );
	mCubeTexture = gl::Texture( loadImage( loadResource( RES_LOGO ) ) );
	
	mCubeRotation.setToIdentity();
}

void FBOBasicApp::renderScene()
{
	CameraPersp cam( getWindowWidth(), getWindowHeight(), 60.0f );
	cam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	cam.lookAt( Vec3f( 3, 2, -3 ), Vec3f::zero() );
	gl::setMatrices( cam );
	gl::multModelView( mCubeRotation );
	
	gl::color( Color::white() );
	mCubeTexture.enableAndBind();
	gl::drawCube( Vec3f::zero(), Vec3f( 2.0f, 2.0f, 2.0f ) );
}

void FBOBasicApp::update()
{
	// Rotate the cube by .03 radians around an arbitrary axis
	mCubeRotation.rotate( Vec3f( 0.16666f, 0.333333f, 0.666666f ).normalized(), 0.03f );
	
	// bind the FBO and render the scene to it
	mFbo.bindFramebuffer();
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	renderScene();

	// restore rendering to the screen
	mFbo.unbindFramebuffer();
}

void FBOBasicApp::draw()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();	
	gl::enableAlphaBlending();
	gl::clear( Color( 0.2f, 0.2f, 0.25f ) );

	gl::setMatricesWindow( getWindowSize() );

	// use the scene we rendered into the FBO as a texture with a shader that pixelates
	glEnable( mFbo.getTarget() );
	mFbo.bindTexture();
	mPixelateShader.bind();
	mPixelateShader.uniform( "tex0", 0 );
	// modulate the pixel size between 1 and 48 every second
	float pixelSize = 1 + 48 * abs( math<float>::sin( getElapsedSeconds() ) );
	mPixelateShader.uniform( "sampleDivisor", Vec2f( pixelSize, pixelSize ) / Vec2f( getWindowSize() ) );
	gl::drawSolidRect( getWindowBounds() );
	mPixelateShader.unbind();
}

void FBOBasicApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case 's': // saves a copy of the unpixelated framebuffer to user's home directory
			writeImage( getHomeDirectory() + "FBOBasic_fbo_snap.png", mFbo.getTexture() );
		break;
	}
}

CINDER_APP_BASIC( FBOBasicApp, RendererGl )
