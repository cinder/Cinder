#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Pbo.h"

// NOTE: this will only fill top ROWS_TO_FILL lines of each Texture in order to reduce CPU load.
// The full Texture is still being replaced each frame.

using namespace ci;
using namespace ci::app;
using namespace std;

static const int IMAGE_WIDTH = 1920;
static const int IMAGE_HEIGHT = 1080 * 12;
static const int IMAGE_CHANNELS = 4; // set to 3 to experiment with no alpha channel
static const int NUM_BUFFERS = 2;
static const int ROWS_TO_FILL = 50;

// Enable or disable this to experiment with PBOs on or off
#define USE_PBO
// Enable or disable to experiment with double buffering the Texture and PBOs
#define DOUBLE_BUFFER

class PboUploadTestApp : public App {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

	gl::TextureRef	mTexs[NUM_BUFFERS];
	gl::PboRef		mPbos[NUM_BUFFERS];
	int				mCurrentTex, mCurrentPbo;
	double			lastFpsUpdate;
};

void PboUploadTestApp::setup()
{
	for( int b = 0; b < NUM_BUFFERS; ++b )
		mTexs[b] = gl::Texture::create( IMAGE_WIDTH, IMAGE_HEIGHT, gl::Texture::Format().internalFormat( (IMAGE_CHANNELS==4)?GL_RGBA:GL_RGB ) );
	mCurrentTex = 0;
	
	for( int b = 0; b < NUM_BUFFERS; ++b )
		mPbos[b] = gl::Pbo::create( GL_PIXEL_UNPACK_BUFFER, IMAGE_WIDTH * IMAGE_HEIGHT * IMAGE_CHANNELS, nullptr, GL_STREAM_DRAW );
	mCurrentPbo = 0;
	
	console() << *(mPbos[0]) << std::endl;
	lastFpsUpdate = getElapsedSeconds();
}

void PboUploadTestApp::mouseDown( MouseEvent event )
{
}

void PboUploadTestApp::update()
{
	static uint8_t rowData[IMAGE_WIDTH * IMAGE_CHANNELS];
#if ! defined( USE_PBO )
	static Surface8u surface( IMAGE_WIDTH, IMAGE_HEIGHT, (IMAGE_CHANNELS==4)?true:false );
#endif
	// fill current PBO with new color
	float hue = sin( getElapsedSeconds() ) / 2 + 0.5f;
	Color8u color( Color( CM_HSV, hue, 1.0f, 1.0f ) );
	// fill a row
	for( int p = 0; p < IMAGE_WIDTH; ++p ) {
		rowData[p*IMAGE_CHANNELS+0] = color.r;
		rowData[p*IMAGE_CHANNELS+1] = color.g;
		rowData[p*IMAGE_CHANNELS+2] = color.b;
		if( IMAGE_CHANNELS == 4 )
			rowData[p*IMAGE_CHANNELS+3]	= 255;
	}
	// fill all the rows
#if defined( USE_PBO )
	gl::ScopedBuffer bscp( mPbos[mCurrentPbo] );
	// why does this slow things down on the Mac?
//	mPbos[mCurrentPbo]->bufferData( mPbos[mCurrentPbo]->getSize(), nullptr, GL_STREAM_DRAW );
	void *pboData = mPbos[mCurrentPbo]->map( GL_WRITE_ONLY );
	for( int row = 0; row < ROWS_TO_FILL; ++row ) {
		memcpy( (uint8_t*)pboData + IMAGE_WIDTH * IMAGE_CHANNELS * row, rowData, IMAGE_WIDTH * IMAGE_CHANNELS );
	}
	mPbos[mCurrentPbo]->unmap();
	
	mTexs[mCurrentTex]->update( mPbos[mCurrentPbo], (IMAGE_CHANNELS==4)?GL_RGBA:GL_RGB, GL_UNSIGNED_BYTE );

  #if defined( DOUBLE_BUFFER )
	mCurrentPbo = ( mCurrentPbo + 1 ) % NUM_BUFFERS;
  #endif
#else
	for( int row = 0; row < ROWS_TO_FILL; ++row ) {
		memcpy( surface.getData( ivec2( 0, row ) ), rowData, IMAGE_WIDTH * IMAGE_CHANNELS );
	}	
	
	mTexs[mCurrentTex]->update( surface );
#endif

#if defined( DOUBLE_BUFFER )
	mCurrentTex = ( mCurrentTex + 1 ) % NUM_BUFFERS;
#endif
}

void PboUploadTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	
	gl::draw( mTexs[mCurrentTex], getWindowBounds() );
	
	if( getElapsedSeconds() - lastFpsUpdate > 2.0f ) {
		console() << getAverageFps() << std::endl;
		lastFpsUpdate = getElapsedSeconds();
	}
}

CINDER_APP( PboUploadTestApp, RendererGl )
