// The idea behind this technique is to render the scene to an intermediate FBO repeatedly (SUBFRAMES times),
// advancing the scene forward a little at a time (in sub-frame increments). So rather than rendering @ time 1.0, time 2.0, etc
// we render at time 1.1, 1.2, etc.
// At the end of rendering each sub-frame, accumulate (add) it into a floating point FBO. And at the end, we get an average by dividing
// by the number of sub-frames we rendered. This is a slightly naive, but very general technique for rendering motion blur.
// We achieve the accumulation by drawing the final sub-frame into the accumulation FBO using addivite blending.
// To do the divide by SUBFRAMES at the end, we just set the color to be 1.0/SUBFRAMES and draw the final accumulation buffer color texture.

#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Shader.h"

using namespace ci;
using namespace ci::app;

class MotionBlurFboApp : public AppNative {
  public:	
	void prepareSettings( Settings *settings ) { settings->setResizable( false ); settings->enableHighDensityDisplay( false ); }
	void setup();
	void keyDown( KeyEvent event );
	void updateCubeRotation( double time );
	void draw();
	
	CameraPersp			mCam;
	Matrix44f			mCubeRotation;
	gl::BatchRef		mBatch;
	gl::FboRef			mFbo, mAccumFbo;
	bool				mPaused;
};

const int SUBFRAMES = 16; // increasing this number increases quality

void MotionBlurFboApp::setup()
{
	mCam.lookAt( Vec3f( 0, 0, 7 ), Vec3f::zero() );
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );
	mCubeRotation.setToIdentity();
	
	mBatch = gl::Batch::create( geom::Cube().enable( geom::Attrib::COLOR ), gl::getStockShader( gl::ShaderDef().color() ) );
	
	mFbo = gl::Fbo::create( toPixels( getWindowWidth() ), toPixels( getWindowHeight() ) );

#if defined( CINDER_GL_ES )
	mAccumFbo = gl::Fbo::create( toPixels( getWindowWidth() ), toPixels( getWindowHeight() ),
		gl::Fbo::Format().colorTexture( gl::Texture::Format().pixelDataType( GL_HALF_FLOAT_OES )
													.internalFormat( GL_RGB ) ).disableDepth() );
#else
	mAccumFbo = gl::Fbo::create( getWindowWidth(), getWindowHeight(),
		gl::Fbo::Format().colorTexture( gl::Texture::Format().internalFormat( GL_RGB16F ) ).disableDepth() );
#endif

	mPaused = false;
}

void MotionBlurFboApp::keyDown( KeyEvent event )
{
	if( event.getChar() == ' ' )
		mPaused = ! mPaused;
}

void MotionBlurFboApp::updateCubeRotation( double time )
{
	// Rotate the cube by .03 radians around an arbitrary axis (which changes over time)
	Vec3f axis( sin(time), cos( time * 3 ), 0.5f );
	mCubeRotation.rotate( axis.normalized(), sin( time ) * 0.02f );
}

void MotionBlurFboApp::draw()
{
	gl::viewport( Vec2f::zero(), mAccumFbo->getSize() );

	if( ! mPaused ) {
		// clear out both of our FBOs
		gl::context()->pushFramebuffer();
		mAccumFbo->bindFramebuffer();
		gl::clear( Color::black() );
		gl::color( 1, 1, 1, 1 );

		// iterate all the sub-frames
		double startTime = getElapsedSeconds();
		for( int i = 0; i < SUBFRAMES; ++i ) {
			// draw the Cube's sub-frame into mFbo
			gl::enableDepthRead();
			gl::enableDepthWrite();
			gl::enableAlphaBlending();
			mFbo->bindFramebuffer();
			gl::clear();
			gl::setMatrices( mCam );
			updateCubeRotation( startTime + i / (float)SUBFRAMES );
			gl::multModelMatrix( mCubeRotation );
			mBatch->draw();
			
			// now add this frame to the accumulation FBO
			mAccumFbo->bindFramebuffer();
			gl::setMatricesWindow( mAccumFbo->getSize() );
			gl::enableAdditiveBlending();
			gl::disableDepthWrite();
			gl::disableDepthRead();		
			gl::draw( mFbo->getColorTexture() );
			gl::enableAlphaBlending();
		}

		// prepare to draw the AccumFbo to the screen (after dividing it by # SUBFRAMES)
		gl::context()->popFramebuffer();
	}
	
	gl::disableDepthRead();
	gl::enableAlphaBlending();
	// set the color to be 1/SUBFRAMES, which divides the HDR image by the number of sub-frames we rendered
	gl::color( 1.0f / SUBFRAMES, 1.0f / SUBFRAMES, 1.0f / SUBFRAMES, 1 );
	gl::setMatricesWindow( getWindowSize() );
	gl::draw( mAccumFbo->getColorTexture() );
}

CINDER_APP_NATIVE( MotionBlurFboApp, RendererGl )