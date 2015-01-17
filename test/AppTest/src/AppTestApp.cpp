#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/CinderAssert.h"

#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class AppTestApp : public AppNative {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	gl::TextureRef mImgAsset, mImgResource;

};

void AppTestApp::setup()
{
	auto asset = loadAsset( "mustache-green.png" );
	mImgAsset = gl::Texture::create( loadImage( asset ) );

	auto resource = loadResource( RES_IMAGE );
	mImgResource = gl::Texture::create( loadImage( resource ) );

	gl::enableAlphaBlending();
	
	CI_LOG_I( "complete" );
}

void AppTestApp::mouseDown( MouseEvent event )
{
}

void AppTestApp::update()
{
}

void AppTestApp::draw()
{
	gl::clear( Color( 0.2f, 0.1f, 0 ) );

	auto offset = vec2( 0 );

	if( mImgAsset ) {
		gl::draw( mImgAsset, offset );
		offset += vec2( mImgAsset->getWidth(), 0 );
	}

	if( mImgResource ) {
		gl::draw( mImgResource, offset );
	}
}

CINDER_APP_NATIVE( AppTestApp, RendererGl )
