/*
	Based loosely on http://www.songho.ca/opengl/gl_pbo.html#pack
*/

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"

#include "cinder/Utilities.h"
#include "cinder/Surface.h"
#include "cinder/Camera.h"
#include "cinder/Timer.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Pbo.h"

const uint32_t WIDTH = 1024;
const uint32_t HEIGHT = 1024;
const uint8_t CHANNELS = 4;
const uint32_t DATA_SIZE = WIDTH * HEIGHT * CHANNELS;
const GLenum PIXEL_FORMAT = GL_RGBA;

using namespace ci;
using namespace ci::app;
using namespace std;

class PBOReadBackApp : public App {
  public:
	static void prepareSettings( Settings *settings );
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;
	
	float mRotate;

	bool mUsePbo;

	uint8_t mIndexNew, mIndexOld;
	
	gl::FboRef mFbo[2];
	gl::PboRef mPbo[2];
	
	Surface8u mSurf;
	gl::TextureRef mTex;
	
	CameraPersp mCam;

	Timer mTimer;
};

void PBOReadBackApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 512, 256 );
	settings->setResizable( false );
}

void PBOReadBackApp::setup()
{	
	mRotate = 0.0;
	
	mUsePbo = true;

	mIndexNew = 0;
	mIndexOld = 1;
	
	mFbo[0] = gl::Fbo::create( WIDTH, HEIGHT, false );
	mFbo[1] = gl::Fbo::create( WIDTH, HEIGHT, false );
	
	mPbo[0] = gl::Pbo::create( GL_PIXEL_PACK_BUFFER, DATA_SIZE, 0, GL_STREAM_READ );
	mPbo[1] = gl::Pbo::create( GL_PIXEL_PACK_BUFFER, DATA_SIZE, 0, GL_STREAM_READ );
	
	mSurf = Surface8u( WIDTH, HEIGHT, true );
	mTex = gl::Texture::create( mSurf );
	mTex->setTopDown( true );
	
	mCam.setPerspective( 60.0, WIDTH / float( HEIGHT ), 0.01, 100.0 );
	mCam.lookAt( vec3( 0.0, 20.0, 50.0 ), vec3( 0.0 ) );
	
	disableFrameRate();
	gl::enableVerticalSync( false );
}

void PBOReadBackApp::keyDown( KeyEvent event )
{
	switch ( event.getCode() ) {
		case KeyEvent::KEY_SPACE:
			mUsePbo = ! mUsePbo;
		break;
	}
}

void PBOReadBackApp::update()
{
	mRotate += 0.05;
}

void PBOReadBackApp::draw()
{
	mTimer.start();
	
	// Read from old Fbo
	if( mUsePbo ) {
		
		// Read to new Pbo (non-blocking)
		{
			gl::ScopedFramebuffer sFbo( mFbo[mIndexOld] );
			gl::ScopedBuffer sPbo( mPbo[mIndexNew] );
			
			gl::readBuffer( GL_COLOR_ATTACHMENT0 );
			gl::readPixels( 0, 0, WIDTH, HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, 0 );
		}
		
		// Receive data from old Pbo (potentially blocking hence the Pbo ping-pong)
		mPbo[mIndexOld]->getBufferSubData( 0, DATA_SIZE, mSurf.getData() );
		
	} else {
		
		// Read directly (blocking)
		gl::ScopedFramebuffer sFbo( mFbo[mIndexOld] );
		
		gl::readBuffer( GL_COLOR_ATTACHMENT0 );
		gl::readPixels( 0, 0, WIDTH, HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, mSurf.getData() );
	}

	mTimer.stop();
	
	// Render to new Fbo
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	{
		gl::ScopedFramebuffer sFbo( mFbo[mIndexNew] );
		gl::ScopedViewport sVp( 0, 0, WIDTH, HEIGHT );
		gl::ScopedMatrices sMat;
		
		gl::setMatrices( mCam );
		
		gl::clear( Color( 0.2, 0.4, 0.8 ) );
		
		gl::rotate( mRotate, vec3( 0.0, 1.0, 0.0 ) );
		gl::drawColorCube( vec3( 0 ) , vec3( 15 ) );
	}
	
	// Render to screen
	gl::disableDepthRead();
	gl::disableDepthWrite();
	
	gl::setMatricesWindow( getWindowSize() );
	gl::clear();
	
	gl::draw( mFbo[mIndexNew]->getColorTexture(), Area( 0, 0, 256, 256 ) );
	
	// Show the change to the data
	mTex->setTopDown( true );
	mTex->update( mSurf );
	mTex->setTopDown( false );
	gl::draw( mTex, Rectf( 256, 0, 512, 256 ) );

	{
		gl::ScopedBlendAlpha sAb;
		
		ostringstream oss( ostringstream::ate );
		oss << fixed << setprecision(3);
		
		oss.str( "Fps: " );
		oss << getAverageFps();
		gl::drawString( oss.str(), ivec2( 10, 15 ) );
		
		oss.str( "Use Pbo: " );
		oss << ( mUsePbo ? "on" : "off" );
		gl::drawString( oss.str(), ivec2( 10, 30 ) );
		
		oss.str( "Read time: " );
		oss << mTimer.getSeconds() * 1000.0 << "ms";
		gl::drawString( oss.str(), ivec2( 10, 240 ) );
	}
	
	swap( mIndexNew, mIndexOld );
}

CINDER_APP( PBOReadBackApp, RendererGl, PBOReadBackApp::prepareSettings )