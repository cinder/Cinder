#include "cinder/app/AppNative.h"
#include "cinder/Camera.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/Texture.h"
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
	Vec3f			mLocation;
	
	Anim<float>		mDotRadius;
	
	gl::Light*		mLight;
	gl::Texture		mTexture;
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

	// Load globe texture,
	setFrameRate( 60.0f );
	mTexture = gl::Texture( loadImage( loadResource( RES_EARTH_JPG ) ) );
	mTexture.setFlipped( true );

	// Set up view
	gl::enable( GL_TEXTURE_2D );
	gl::enableAlphaBlending();

	// Set up light
	mLight = new gl::Light( gl::Light::DIRECTIONAL, 0 );
	mLight->setDirection( Vec3f( 0.0f, 0.1f, 0.3f ).normalized() );
	mLight->setAmbient( ColorAf::gray( 0.843f ) );
	mLight->setDiffuse( ColorAf( 1.0f, 1.0f, 1.0f, 1.0f ) );
	mLight->enable();

	// Build the heading arrow
	float size = 80.0f;
	mArrow.moveTo( Vec2f(         0.0f, -size * 0.5f  ) );
	mArrow.lineTo( Vec2f(  size * 0.5f,  size * 0.5f  ) );
	mArrow.lineTo( Vec2f(         0.0f,  size * 0.25f ) );
	mArrow.lineTo( Vec2f( -size * 0.5f,  size * 0.5f  ) );
	mArrow.close();
}

void LocationApp::draw()
{
	// Clear the screen
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::clear( Color::gray( 0.843f ) );
	
	CameraPersp camera;
	camera.setPerspective( 60.0f, getWindowAspectRatio(), 0.01f, 10.0f );
	camera.lookAt( Vec3f( 0.0f, 0.0f, 3.0f ), Vec3f::zero() );
	gl::setMatrices( camera );
	mLight->update( camera );

	// Rotate the globe
	gl::multModelView( Quatf(Vec3f::yAxis(), mRotationAngle ).normalized() );
	
	// Draw the globe with shading. Rotate it 90 degrees on 
	// its Y axis to line up the texture with the location
	gl::color( ColorAf::white() );
	gl::enable( GL_LIGHTING );
	mTexture.bind( 0 );
	gl::pushMatrices();
		gl::rotate( Vec3f( 0.0f, 90.0f, 0.0f ) );
		gl::drawSphere( Vec3f::zero(), 1.0f, 32 );
	gl::popMatrices();
	mTexture.unbind();
	gl::disable( GL_LIGHTING );
	
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
	Vec2f position = getWindowCenter() + Vec2f( x, y ) * radius;
	
	gl::translate( position );
	gl::rotate( Vec3f( 0.0f, 0.0f, -mHeading ) );
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
	Vec2f coord = event.getCoordinate();
	console() << "Location: " << coord << std::endl;
	
	// Convert the location to radians
	coord.x = toRadians( 180.0f - ( 90.0f + coord.x ) );
	coord.y = toRadians( coord.y );

	// Project GPS coordinates to sphere
	float x = math<float>::sin( coord.x ) * math<float>::sin( coord.y );
	float y = math<float>::cos( coord.x );
	float z = math<float>::sin( coord.x ) * math<float>::cos( coord.y );
	mLocation = Vec3f( x, y, z );
}

CINDER_APP_NATIVE( LocationApp, RendererGl )