#include "cinder/app/AppCocoaTouch.h"
#include "cinder/cocoa/CinderCocoaTouch.h"
#include "cinder/app/Renderer.h"
#include "cinder/ImageIo.h"
#include "cinder/Url.h"
#include "TestApp.h"
#include "cinder/Surface.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "cinder/Text.h"
#include "cinder/Xml.h"
#include "cinder/System.h"

using namespace ci;
using namespace ci::app;

void TestApp::setup()
{
	mCubeRotation.setToIdentity();

	Surface8u surface( 256, 256, false );
	Surface8u::Iter iter = surface.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			iter.r() = 0;
			iter.g() = iter.x();
			iter.b() = iter.y();
		}
	}
	
	mTex = gl::Texture( surface );
	mTex = gl::Texture( loadImage( loadResource( "cinder_logo_alpha.png" ) ) );
	
	//mTempSurface = renderString( "Frame: " + toString( getElapsedFrames() ), Font( "Arial", 56 ), Color( 1, 0.5, 0.25 ) );	
	mFont = Font( "Arial", 56 );
	//Buffer buffer = loadStreamBuffer( loadFileStream( getResourcePath( "Beyonce.jpg" ) ) );
	//mTex = gl::Texture( loadImage( DataSourceBuffer::createRef( buffer, ".jpg" ) ) );
	mTex = gl::Texture( loadImage( loadUrl( Url( "http://images.apple.com/home/images/promo_iphone_case_program_20100723.jpg" ) ) ) );

	XmlDocument doc( loadUrlStream( "http://www.feedparser.org/docs/http-authentication.html", "test", "basic" ) );
	console() << doc << std::endl;

	console() << "System version: " << System::getOsMajorVersion() << "." << System::getOsMinorVersion() << "." << System::getOsBugFixVersion() << std::endl;
	
console() << "size: " << getWindowSize() << std::endl;	
}

void TestApp::resize( int width, int height )
{
	mCam.lookAt( Vec3f( 3, 2, -3 ), Vec3f::zero() );
	mCam.setPerspective( 60, width / (float)height, 1, 1000 );
console() << "height: " << getWindowHeight() << std::endl;
console() << "path: " << getAppPath() << std::endl;
console() << "Frames: " << getElapsedFrames() << std::endl;
}

void TestApp::mouseDown( MouseEvent event )
{
	std::cout << "Mouse down @ " << event.getPos() << std::endl;
//	writeImage( getDocumentsDirectory() + "whatever.png", copyWindowSurface() );
	Surface8u surface( 256, 256, false );
	Surface8u::Iter iter = surface.getIter();
	while( iter.line() ) {
		while( iter.pixel() ) {
			iter.r() = 0;
			iter.g() = iter.x();
			iter.b() = iter.y();
		}
	}

	cocoa::writeToSavedPhotosAlbum( surface );
}

void TestApp::update()
{
	mCubeRotation.rotate( Vec3f( 1, 1, 1 ), 0.03f );
}

void TestApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.3f ) );
	gl::enableAlphaBlending();
	gl::enableDepthRead();
console() << getWindowWidth() << std::endl;	
	/*mTex.bind();
	gl::setMatrices( mCam );
	glPushMatrix();
		gl::multModelView( mCubeRotation );
		gl::drawCube( Vec3f::zero(), Vec3f( 2.0f, 2.0f, 2.0f ) );
	glPopMatrix();*/
	
	gl::setMatricesWindow( getWindowSize() );
	gl::setViewport( getWindowBounds() );
//	gl::drawString( (Rand::randBool() ? ( "Fjrameyq: " ) : ( "Frame: " )) + toString( getElapsedFrames() ), Vec2f( 10, 40 ), Color( 1, 0.5, 0.25 ), mFont );
	gl::draw( mTex );
}

CINDER_APP_COCOA_TOUCH( TestApp, RendererGl )