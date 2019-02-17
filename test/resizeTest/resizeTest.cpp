#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/ip/Resize.h"
#include "cinder/ImageIo.h"

#include "Resources.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
using namespace ci;
using namespace ci::app;

class ResizeTestApp : public App {
 public:	
	
	static void prepareSettings( Settings *settings );
	void setup();
	void draw();

	static const int TEXTURE_WIDTH = 600, TEXTURE_HEIGHT = 200;
	
	Surface			mSurfaceComposite;
	gl::TextureRef	mTexture;
};

void ResizeTestApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( TEXTURE_WIDTH, TEXTURE_HEIGHT );
	settings->setResizable( false );
}

void ResizeTestApp::setup()
{
	Surface imageSurface( loadImage( loadResource( RES_IMAGE ) ) );
	mSurfaceComposite = Surface( TEXTURE_WIDTH, TEXTURE_HEIGHT, false ); 
	
	// Copy the texture three times, using three different filters
	Area srcArea( 33, 351, 33 + 125, 351 + 125 );
	ci::ip::resize( imageSurface, srcArea, &mSurfaceComposite, Area( 0, 0, 200, 200 ), FilterBox() );
	ci::ip::resize( imageSurface, srcArea, &mSurfaceComposite, Area( 200, 0, 400, 200 ), FilterCubic() );
	ci::ip::resize( imageSurface, srcArea, &mSurfaceComposite, Area( 400, 0, 600, 200 ), FilterSincBlackman() );
	
	mTexture = gl::Texture::create( mSurfaceComposite );
}

void ResizeTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::draw( mTexture, vec2( 0, 0 ) );
}


CINDER_APP( ResizeTestApp, RendererGl, &ResizeTestApp::prepareSettings )