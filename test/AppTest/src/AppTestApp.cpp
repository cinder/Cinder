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

void prepareSettings( AppNative::Settings *settings )
{
	CI_LOG_I( "bang" );
	settings->setWindowPos( 0, 0 );
//	settings->setBorderless(); // FIXME: causes an NSException on [CinderWindow alloc] initWithContentRect...], AppImplCocoaBasic:719
}

struct SomeMemberObj {
	gl::TextureRef mTex;

	SomeMemberObj()
	{
		CI_LOG_I( "bang" );

		Surface8u surface( 140, 140, false );
		Surface8u::Iter iter = surface.getIter();
		while( iter.line() ) {
			while( iter.pixel() ) {
				iter.r() = iter.x();
				iter.g() = 0;
				iter.b() = iter.y();
			}
		}

		mTex = gl::Texture::create( surface );
		CI_CHECK_GL();
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

	Surface8u surface( 140, 140, false );
	Surface8u::Iter iter = surface.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			iter.r() = 0;
			iter.g() = iter.x();
			iter.b() = iter.y();
		}
	}

	mTexStartup = gl::Texture::create( surface );
	CI_CHECK_GL();
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

	CI_CHECK_GL();

	CI_LOG_I( "complete" );
}

void AppTestApp::mouseDown( MouseEvent event )
{
	CI_LOG_I( "mouse pos: " << event.getPos() );
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
		offset.x += 256;
		gl::draw( mTexResource, offset );
	}

	if( mTexStartup ) {
		offset.x = 0;
		offset.y += 200;
		gl::draw( mTexStartup, offset );
	}

	if( mSomeMemberObj.mTex ) {
		offset.x += mSomeMemberObj.mTex->getWidth();
		gl::draw( mSomeMemberObj.mTex, offset );
	}

	CI_CHECK_GL();
}

// no settings fn:
//CINDER_APP_NATIVE( AppTestApp, RendererGl )

// settings fn from top of file:
CINDER_APP_NATIVE( AppTestApp, RendererGl, prepareSettings )

// settings fn by lambda
//CINDER_APP_NATIVE( AppTestApp, RendererGl, []( AppTestApp::Settings *settings ) {
//	CI_LOG_I( "bang" );
//} )
