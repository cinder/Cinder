#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Pbo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define USE_PBO

static const int IMAGE_WIDTH = 1920;
static const int IMAGE_HEIGHT = 1080 * 12;

class ImageSourcePboApp : public App {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	
	double				lastFpsUpdate;
	Surface				mSurface;
	gl::TextureRef		mTex[2];
	gl::PboRef			mPbos[2];
	int					mCurrentTex;
};

void ImageSourcePboApp::setup()
{
	// prep our huge image by tiling this one
	Surface s = loadImage( loadAsset( "compression_test.png" ) );
	
	mSurface = Surface( IMAGE_WIDTH, IMAGE_HEIGHT, false );
	for( int x = 0; x < ceil( IMAGE_WIDTH / (float)s.getWidth()); ++x ) {
		for( int y = 0; y < ceil( IMAGE_HEIGHT / (float)s.getHeight()); ++y ) {
			mSurface.copyFrom( s, s.getBounds(), ivec2( x * s.getWidth(), y * s.getHeight() ) );
		}
	}
	
	lastFpsUpdate = getElapsedSeconds();
	
	mCurrentTex = 0;
	mTex[0] = gl::Texture::create( (ImageSourceRef)mSurface );
	mTex[1] = gl::Texture::create( (ImageSourceRef)mSurface );
	
#if defined( USE_PBO )
	mPbos[0] = gl::Pbo::create( GL_PIXEL_UNPACK_BUFFER, IMAGE_WIDTH * IMAGE_HEIGHT * 3, nullptr, GL_STREAM_DRAW );
	mPbos[1] = gl::Pbo::create( GL_PIXEL_UNPACK_BUFFER, IMAGE_WIDTH * IMAGE_HEIGHT * 3, nullptr, GL_STREAM_DRAW );
#endif
}

void ImageSourcePboApp::mouseDown( MouseEvent event )
{
}

void ImageSourcePboApp::update()
{
	gl::Texture::Format fmt;
#if defined( USE_PBO )
	fmt.intermediatePbo( mPbos[0] );
#endif
	mTex[mCurrentTex] = gl::Texture::create( (ImageSourceRef)mSurface, fmt );
	mCurrentTex = ( mCurrentTex + 1 ) % 2;
}

void ImageSourcePboApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
	
	gl::draw( mTex[mCurrentTex], getWindowBounds() );

	if( getElapsedSeconds() - lastFpsUpdate > 2.0f ) {
		console() << getAverageFps() << std::endl;
		lastFpsUpdate = getElapsedSeconds();
	}
}

CINDER_APP( ImageSourcePboApp, RendererGl )
