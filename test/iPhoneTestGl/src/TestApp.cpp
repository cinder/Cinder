#include "cinder/app/App.h"
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

	for( auto &display : Display::getDisplays() )
		console() << display;

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
//	mTex = gl::Texture( loadImage( loadUrl( Url( "http://images.apple.com/home/images/hero_imac.jpg" ) ) ) );

	console() << "System version: " << System::getOsMajorVersion() << "." << System::getOsMinorVersion() << "." << System::getOsBugFixVersion() << std::endl;
	
console() << "size: " << getWindowSize() << " @ " << getWindow()->getContentScale() << std::endl;	
}

void TestApp::resize( int width, int height )
{
	mCam.lookAt( vec3( 3, 2, -3 ), vec3::zero() );
	mCam.setPerspective( 60, width / (float)height, 1, 1000 );
console() << "height: " << getWindowHeight() << std::endl;
console() << "path: " << getAppPath() << std::endl;
console() << "Frames: " << getElapsedFrames() << std::endl;
}

void TestApp::touchesBegan( TouchEvent event )
{
	vec2 pt = event.getTouches()[0].getPos();
	std::cout << "Mouse down @ " << pt << " points: " << getWindow()->toPoints( pt ) << std::endl;
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

//	cocoa::writeToSavedPhotosAlbum( surface );
}

void TestApp::update()
{
	mCubeRotation.rotate( vec3( 1, 1, 1 ), 0.03f );
}

void TestApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.3f ) );
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	/*mTex.bind();
	gl::setMatrices( mCam );
	glPushMatrix();
		gl::multModelView( mCubeRotation );
		gl::drawCube( vec3::zero(), vec3( 2.0f, 2.0f, 2.0f ) );
	glPopMatrix();*/
	
	gl::setMatricesWindow( getWindowSize() );
	gl::setViewport( getWindowBounds() );
//	gl::drawString( (Rand::randBool() ? ( "Fjrameyq: " ) : ( "Frame: " )) + toString( getElapsedFrames() ), vec2( 10, 40 ), Color( 1, 0.5, 0.25 ), mFont );
	gl::draw( mTex );
}

CINDER_APP_COCOA_TOUCH( TestApp, RendererGl )