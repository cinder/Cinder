#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererDx.h"
#include "cinder/ImageIo.h"
#include "cinder/Font.h"
#include "cinder/dx/dx.h"
#include "cinder/dx/DxTexture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public AppBasic {
  public:
	void			setup();
	void			mouseMove( MouseEvent event );
	void			update();
	void			draw();

	dx::TextureRef	mTexture;
	Font			mFont;

	Vec2f			mMousePosition;
	Vec2f			mFollowerPosition;

};

void BasicApp::setup()
{
	mFont = Font( "Arial", 18  );
	mTexture = dx::Texture::create( loadImage( loadAsset( "bkg.jpg" ) ) );
	mMousePosition = getWindowCenter();
	mFollowerPosition = getWindowCenter();
}

void BasicApp::mouseMove( MouseEvent event )
{
	mMousePosition = event.getPos();
}

void BasicApp::update()
{
	// simple easing with procedural code.
	// The logic for calculating the step never changes:
	// Where you want to be MINUS where you are it the total distance.
	// Unless you want to cover the entire distance in one huge step, 
	// multiply by a small percentage. 
	Vec2f animationStep = (mMousePosition - mFollowerPosition)*.2f;
	// Then ADD that step to your current position.
	// Since we are working with percentages of the whole, and not absolute
	// fractions, the percentage of the distance remaining will always be relative,
	// i.e. the steps will get smaller the closer you get to your target, EASING IN.
	mFollowerPosition += animationStep;

}

void BasicApp::draw()
{
	// This clears the screen and set's it to a background color
	dx::clear( Color( 0.0f, 0.0f, 0.0f ), true );
	dx::color( Color::white() );

	// This is optional, but without it, the transparent background of the
	// drawString() command will appear black.
	// AlphaBlending() is the most common, but AdditiveBlending() is frequently
	// used to create glowing effects ( for example see project: \_winrtSpriteParticles )
	dx::enableAlphaBlending();

	// although we cleared out the  background to a solid color, let's use the
	// texture we loaded in setup ( bkg.jpg ) as a full screen background image
	if( mTexture )
		dx::draw( mTexture, Rectf(getWindowBounds()) );

	Vec2f radius( 8.0f, 8.0f );
	Vec2f smallerRadius( 6.0f, 6.0f );

	// lets draw some shapes to show the immediate position of any screen pointer
	// we will draw some semi-transparent cross hairs...
	dx::color( ColorA(1.0f, 1.0f, 1.0f, 0.3f) );
	dx::drawLine( Vec2f( 0, mMousePosition.y ), Vec2f( getWindowWidth(), mMousePosition.y ) );
	dx::drawLine( Vec2f( mMousePosition.x, 0 ), Vec2f( mMousePosition.x, getWindowHeight() ) );
	// and a solid white targeting square...
	dx::color( Color::white() );
	dx::drawSolidRect( Rectf( mMousePosition - radius, mMousePosition + radius) );
	
	// lets also illustrate the animation logic we added in update() and visualize the 
	// follower as a square while it eases to the immediate pointer target
	dx::color( Color( 1.0f, 0.4f, 0.8f ) );
	dx::drawSolidRect( Rectf( mFollowerPosition - smallerRadius, mFollowerPosition + smallerRadius ) );

	// lets explore drawing some additional, static geometries as well...
	dx::color( ColorA( 0.0f, 1.0f, 1.0f, 0.5) );
	dx::drawStrokedCircle( Vec2f( 40.0f, 90.0f ), 30.0f, 7 );
	dx::drawSolidCircle( Vec2f(   40.0f, 90.0f ), 20.0f, 7 );
	dx::drawSolidTriangle( Vec2f( 40.0f, 144.0f ), Vec2f( 14.0f, 196.0f), Vec2f(66.0f, 196.0f) ); 
	dx::drawStrokedRect( Rectf(10.0f, 140.0f, 70.0f, 200.0f ) );

	// here is a simple way of drawing basic messages on the screen
	std::stringstream s;
	s << "Framerate:" << getAverageFps();
	dx::drawString( s.str(), Vec2f(10.0f,10.0f), Color(1.0f, 1.0f, 1.0f), mFont );
}

// This line tells Cinder to actually create the application and use the DirectX required
// for Windows Store apps instead of the default OpenGL renderer that comes with the framework
CINDER_APP_BASIC( BasicApp, RendererDx )