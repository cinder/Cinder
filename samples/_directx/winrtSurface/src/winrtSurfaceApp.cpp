#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererDx.h"
#include "cinder/ImageIo.h"
#include "cinder/dx/dx.h"
#include "cinder/dx/DxTexture.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public AppBasic {
  public:
	void setup();
	void draw();	

	dx::TextureRef	mProcessedImageTex;
	Vec2i			mImageCenter;
};

// We can invert the image by subtracting each R,G,B value from 255
void invertArea( Surface *surface, Area area )
{
	Surface::Iter iter = surface->getIter( area );
	while( iter.line() ) {
		while( iter.pixel() ) {
			iter.r() = 255 - iter.r();
			iter.g() = 255 - iter.g();
			iter.b() = 255 - iter.b();
		}
	}
}

// We can create a thresholded image by taking the grayscale value of each pixel and assigning the
// result to be black for gray <= 50%, and white for gray > 50%
void thresholdArea( Surface *surface, Area area )
{
	Surface::Iter iter = surface->getIter( area );
	while( iter.line() ) {
		while( iter.pixel() ) {
			// this is not the best way to determine grayscale - see CHANTRAIT<>::grayscale()
			uint8_t gray = ( iter.r() + iter.g() + iter.b() ) / 3;
			if( gray > 128 )
				iter.r() = iter.g() = iter.b() = 255;
			else
				iter.r() = iter.g() = iter.b() = 0;
		}
	}
}

// We can perform an edge-detection by performing a weighted average of the neighboring pixels like this:
//
// 0  1  0
// 1 -4  1
// 0  1  0
//
// So for each input pixel, we take its value * -4 and add it to the values of its immediate neighbors
// Also we need make a copy of the surface before we work on it because otherwise we'll be tainting
// the results of "later" pixels
void edgeDetectArea( Surface *surface, Area area )
{
	// make a copy of the original before we start writing on it
	Surface inputSurface( surface->clone( area ) );

	// we'll need to iterate the inputSurface as well as the output surface
	Surface::ConstIter inputIter( inputSurface.getIter() );
	Surface::Iter outputIter( surface->getIter( area ) );

	while( inputIter.line() ) {
		outputIter.line();
		while( inputIter.pixel() ) {
			outputIter.pixel();
			int32_t sumRed = inputIter.rClamped( 0, -1 ) + inputIter.rClamped( -1, 0 ) + inputIter.r() * -4 + inputIter.rClamped( 1, 0 ) + inputIter.rClamped( 0, 1 );
			outputIter.r() = constrain<int32_t>( abs( sumRed ), 0, 255 );
			int32_t sumGreen = inputIter.gClamped( 0, -1 ) + inputIter.gClamped( -1, 0 ) + inputIter.g() * -4 + inputIter.gClamped( 1, 0 ) + inputIter.gClamped( 0, 1 );
			outputIter.g() = constrain<int32_t>( abs( sumGreen ), 0, 255 );
			int32_t sumBlue = inputIter.bClamped( 0, -1 ) + inputIter.bClamped( -1, 0 ) + inputIter.b() * -4 + inputIter.bClamped( 1, 0 ) + inputIter.bClamped( 0, 1 );
			outputIter.b() = constrain<int32_t>( abs( sumBlue ), 0, 255 );
		}
	}
}

// Returns a Surface of the same dimensions as 'inputSurface', but separated into 4 quadrants
// listed clockwise: the original image, edge-detected, inverted, and thresholded
Surface processImage( const Surface input )
{
	// make the result be a copy of input, taking care of our upper-left quadrant as well
	Surface resultSurface( input.clone() );
	
	// let's do the upper-right, an inverted image
	invertArea( &resultSurface, Area( input.getWidth() / 2, 0, input.getWidth(), input.getHeight() / 2 ) );

	// now the lower-right, a thresholded image
	edgeDetectArea( &resultSurface, Area( input.getWidth() / 2, input.getHeight() / 2, input.getWidth(), input.getHeight() ) );

	// and finally the lower left, an edge-detected image
	thresholdArea( &resultSurface, Area( 0, input.getHeight() / 2, input.getWidth() / 2, input.getHeight() ) ); 
	
	return resultSurface;
}

void BasicApp::setup()
{
	Surface processedImage( processImage( loadImage( loadResource( RES_IMAGE ) ) ) );
	mProcessedImageTex = dx::Texture::create( processedImage );
	mImageCenter = Vec2i( getWindowCenter().x - mProcessedImageTex->getWidth() / 2, getWindowCenter().y - mProcessedImageTex->getHeight() / 2 );
}

void BasicApp::draw()
{
	dx::clear( Color::black(), false );
	dx::draw( mProcessedImageTex, mImageCenter );
}

CINDER_APP_BASIC( BasicApp, RendererDx )