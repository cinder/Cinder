#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Batch.h"
#include "cinder/ImageIo.h"
#include "cinder/Shape2d.h"
#include "cinder/Utilities.h"
#include "cinder/Timeline.h"
#include "cinder/LocationManager.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

class LocationApp : public AppNative {
  public:
 	virtual void	setup(); 
	virtual void	draw();

#if defined( CINDER_COCOA_TOUCH )
	void	headingChanged( HeadingEvent event );
#endif
	void	locationChanged( LocationEvent event );
	
  private:
	Anim<float>		mRotationAngle;
	Shape2d			mArrow;
	float			mHeading;
	vec3			mLocation;
	
	Anim<float>			mDotRadius;
	
	gl::BatchRef		mEarthBatch;
	gl::Texture2dRef	mTexture;
};

void LocationApp::setup()
{
	// Define properties
	mHeading = 0.0f;
	timeline().apply( &mRotationAngle, 0.0f, (float)(2 * M_PI), 8.0f ).loop();
	timeline().apply( &mDotRadius, 0.0f, 0.1f, 0.5f ).loop();
	
	LocationManager::enable();
	LocationManager::getSignalLocationChanged().connect(
		std::bind( &LocationApp::locationChanged, this, std::placeholders::_1 ) );
#if defined( CINDER_COCOA_TOUCH )
	LocationManager::getSignalHeadingChanged().connect(
		std::bind( &LocationApp::headingChanged, this, std::placeholders::_1 ) );
#endif

	// Load globe texture
	mTexture = gl::Texture2d::create( loadImage( loadResource( RES_EARTH_JPG ) ) );

	// Set up view
	gl::enableAlphaBlending();

	// Build the heading arrow
	float size = 80.0f;
	mArrow.moveTo( vec2(         0.0f, -size * 0.5f  ) );
	mArrow.lineTo( vec2(  size * 0.5f,  size * 0.5f  ) );
	mArrow.lineTo( vec2(         0.0f,  size * 0.25f ) );
	mArrow.lineTo( vec2( -size * 0.5f,  size * 0.5f  ) );
	mArrow.close();
	
	mEarthBatch = gl::Batch::create( geom::Sphere(), gl::getStockShader( gl::ShaderDef().texture() ) );
}

void LocationApp::draw()
{
	// Clear the screen
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::clear( Color::gray( 0.843f ) );
	
	CameraPersp camera;
	camera.setPerspective( 60.0f, getWindowAspectRatio(), 0.01f, 10.0f );
	camera.lookAt( vec3( 0.0f, 0.0f, 3.0f ), vec3( 0 ) );
	gl::setMatrices( camera );

	// Rotate the globe
	gl::rotate( angleAxis( mRotationAngle(), vec3(0, 1, 0) ) );
	
	// Draw the globe with shading. Rotate it 90 degrees on 
	// its Y axis to line up the texture with the location
	gl::color( ColorAf::white() );
	mTexture->bind( 0 );
	gl::pushMatrices();
		gl::rotate( angleAxis( (float)(M_PI), vec3(0,1,0) ) );
		mEarthBatch->draw();
	gl::popMatrices();
	mTexture->unbind();
	
	// Draw location
	gl::color( ColorAf( 1.0f, 0.2f, 0.18f, 0.667f ) );
	gl::drawSphere( mLocation, mDotRadius, 32 );
	
	////////////////////////////////////////////////////
#if defined( CINDER_COCOA_TOUCH )
	gl::setMatricesWindow( getWindowSize() );
	gl::enableDepthRead( false );
	gl::enableDepthWrite( false );
	
	// Plot arrow position
	float radius = 256.0f;
	float rotation = toRadians( mHeading ) - (float)M_PI * 0.5f;
	float x = math<float>::cos( rotation );
	float y = math<float>::sin( rotation );
	vec2 position = getWindowCenter() + vec2( x, y ) * radius;
	
	gl::translate( position );
	gl::rotate( angleAxis( -mHeading, vec3( 0, 0, 1 ) ) );
	gl::translate( position * -1.0f );
	gl::translate( position );
	
	gl::color( Colorf( 0, 0, 1 ) );
	gl::drawSolid( mArrow );
#endif
}

#if defined( CINDER_COCOA_TOUCH )
void LocationApp::headingChanged( HeadingEvent event )
{
	mHeading = event.getTrueHeading();
	console() << "Heading: " << mHeading << std::endl;
}
#endif

void LocationApp::locationChanged( LocationEvent event )
{
	// Get the location coordinate
	vec2 coord = event.getCoordinate();
	console() << "Location: " << coord << std::endl;
	
	// Convert the location to radians
	coord.x = toRadians( 180.0f - ( 90.0f + coord.x ) );
	coord.y = toRadians( coord.y );

	// Project GPS coordinates to sphere
	float x = math<float>::sin( coord.x ) * math<float>::sin( coord.y );
	float y = math<float>::cos( coord.x );
	float z = math<float>::sin( coord.x ) * math<float>::cos( coord.y );
	mLocation = vec3( x, y, z );
}

CINDER_APP_NATIVE( LocationApp, RendererGl )