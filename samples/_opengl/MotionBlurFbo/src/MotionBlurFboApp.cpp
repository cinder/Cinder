// The idea behind this technique is to render the scene to an intermediate FBO repeatedly (SUBFRAMES times),
// advancing the scene forward a little at a time (in sub-frame increments). So rather than rendering @ time 1.0, time 2.0, etc
// we render at time 1.1, 1.2, etc.
// At the end of rendering each sub-frame, accumulate (add) it into a floating point FBO. And at the end, we get an average by dividing
// by the number of sub-frames we rendered. This is a slightly naive, but very general technique for rendering motion blur.
// We achieve the accumulation by drawing the final sub-frame into the accumulation FBO using addivite blending.
// To do the divide by SUBFRAMES at the end, we just set the color to be 1.0/SUBFRAMES and draw the final accumulation buffer color texture.

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

class MotionBlurFboApp : public App {
  public:	
	void setup();
	void keyDown( KeyEvent event );
	void updateCubeRotation( double time );
	void draw();
	
	CameraPersp			mCam;
	mat4			mCubeRotation;
	gl::BatchRef		mBatch;
	gl::FboRef			mFbo, mAccumFbo;
	bool				mPaused;
};

const int SUBFRAMES = 16; // increasing this number increases quality

void MotionBlurFboApp::setup()
{
	mCam.lookAt( vec3( 0, 0, 2.5 ), vec3( 0 ) );
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 1000 );

	mBatch = gl::Batch::create( geom::Cube().colors(), gl::getStockShader( gl::ShaderDef().color() ) );
	mFbo = gl::Fbo::create( toPixels( getWindowWidth() ), toPixels( getWindowHeight() ) );

#if defined( CINDER_GL_ES_2 )
	mAccumFbo = gl::Fbo::create( toPixels( getWindowWidth() ), toPixels( getWindowHeight() ),
		gl::Fbo::Format().colorTexture( gl::Texture::Format().dataType( GL_HALF_FLOAT_OES )
													.internalFormat( GL_RGB ) ).disableDepth() );
#else
	mAccumFbo = gl::Fbo::create( getWindowWidth(), getWindowHeight(),
						gl::Fbo::Format().colorTexture( gl::Fbo::Format().getDefaultColorTextureFormat().internalFormat( GL_RGB16F ) ).disableDepth() );
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
	auto axis = normalize( vec3( sin( time ), cos( time * 3 ), 0.5f ) );
	mCubeRotation *= rotate( sinf( time ) * 0.02f, axis );
}

void MotionBlurFboApp::draw()
{
	gl::viewport( vec2(), mAccumFbo->getSize() );

	if( ! mPaused ) {
		// make 'mAccumFbo' the active framebuffer
		gl::ScopedFramebuffer fbScp( mAccumFbo );
		// clear out both of our FBOs
		gl::clear( Color::black() );
		gl::color( 1, 1, 1, 1 );

		// iterate all the sub-frames
		double startTime = getElapsedSeconds();
		for( int i = 0; i < SUBFRAMES; ++i ) {
			// draw the Cube's sub-frame into mFbo
			gl::enableDepth();
			gl::enableAlphaBlending();
			mFbo->bindFramebuffer();
			gl::clear();
			gl::enableDepth();
			gl::setMatrices( mCam );
			updateCubeRotation( startTime + i / (float)SUBFRAMES );
			gl::multModelMatrix( mCubeRotation );
			mBatch->draw();
			
			// now add this frame to the accumulation FBO
			mAccumFbo->bindFramebuffer();
			gl::setMatricesWindow( mAccumFbo->getSize() );
			gl::enableAdditiveBlending();
			gl::enableDepth( false );
			gl::draw( mFbo->getColorTexture() );
		}
	}
	
	gl::disableDepthRead();
	gl::disableAlphaBlending();
	// set the color to be 1/SUBFRAMES, which divides the HDR image by the number of sub-frames we rendered
	gl::color( 1.0f / SUBFRAMES, 1.0f / SUBFRAMES, 1.0f / SUBFRAMES, 1 );
	gl::setMatricesWindow( getWindowSize() );
	gl::draw( mAccumFbo->getColorTexture() );
}

CINDER_APP( MotionBlurFboApp, RendererGl, []( App::Settings *settings ) {
	settings->setResizable( false );
	settings->setHighDensityDisplayEnabled( false );
} )
