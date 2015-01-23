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

struct SomeMemberObj {
	gl::TextureRef mTex;

	SomeMemberObj()
	{
		CI_LOG_I( "bang" );

		Surface8u surface( 256, 256, false );
		Surface8u::Iter iter = surface.getIter();
		while( iter.line() ) {
			while( iter.pixel() ) {
				iter.r() = iter.x();
				iter.g() = 0;
				iter.b() = iter.y();
			}
		}

		mTex = gl::Texture::create( surface );
	}
};

class AppTestApp : public AppNative {
  public:
	AppTestApp();

	void prepareSettings( Settings *settings ) override;
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	SomeMemberObj mSomeMemberObj;

	gl::TextureRef mTexStartup, mTexAsset, mTexResource;

};

AppTestApp::AppTestApp()
{
	CI_LOG_I( "bang" );

	Surface8u surface( 256, 256, false );
	Surface8u::Iter iter = surface.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			iter.r() = 0;
			iter.g() = iter.x();
			iter.b() = iter.y();
		}
	}

	mTexStartup = gl::Texture::create( surface );
}

void AppTestApp::prepareSettings( Settings *settings )
{
	CI_LOG_I( "bang" );
}

void AppTestApp::setup()
{
	auto asset = loadAsset( "mustache-green.png" );
	mTexAsset = gl::Texture::create( loadImage( asset ) );

	auto resource = loadResource( RES_IMAGE );
	mTexResource = gl::Texture::create( loadImage( resource ) );

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

	if( mTexAsset ) {
		gl::draw( mTexAsset, offset );
	}

	if( mTexResource ) {
		offset.x += mTexAsset->getWidth();
		gl::draw( mTexResource, offset );
	}

	if( mTexStartup ) {
		offset.x = 0;
		offset.y += mTexStartup->getHeight();
		gl::draw( mTexStartup, offset );
	}

	if( mSomeMemberObj.mTex ) {
		offset.x += mSomeMemberObj.mTex->getWidth();
		gl::draw( mSomeMemberObj.mTex, offset );
	}

}

CINDER_APP_NATIVE( AppTestApp, RendererGl )
