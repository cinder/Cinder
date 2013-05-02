#include "cinder/app/AppBasic.h"
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

	dx::Texture		mTexture;
	Font			mFont;

	Vec2i			mMousePosition;
	Vec2f			mFollowerPosition;

};

void BasicApp::setup()
{
	mFont = Font( "Arial", 18  );
	mTexture = dx::Texture( loadImage( loadAsset( "bkg.jpg" ) ) );
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
	Vec2f animationStep = (mMousePosition - mFollowerPosition)*.2;
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
	dx::color( Color::white());

	// This is optional, but without it, the transparent background of the
	// drawString() command will appear black.
	// AlphaBlending() is the most common, but AdditiveBlending() is frequently
	// used to create glowing effects ( for example see project: \_winrtSpriteParticles )
	dx::enableAlphaBlending();

	// although we cleared out the  background to a solid color, let's use the
	// texture we loaded in setup ( bkg.jpg ) as a full screen background image
	if ( mTexture )
		dx::draw( mTexture, Rectf(getWindowBounds()) );

	// lets draw some shapes to show the immediate position of any screen pointer
	dx::color( Color(1.0f, 1.0f, 1.0f) );
	dx::drawLine( Vec2f( 0, mMousePosition.y), Vec2f(getWindowWidth(), mMousePosition.y ) );
	dx::drawLine( Vec2f( mMousePosition.x, 0), Vec2f(mMousePosition.x, getWindowHeight() ) );
	dx::drawStrokedCircle( mMousePosition, 14.0f );

	// lets also illustrate the animation logic we added in update() and visualize the 
	// follower as a square while it eases to the pointer target ring.
	dx::color( Color(1.0f, 0.4f, 0.8f ) );
	Vec2f radius(5.0f, 5.0f);
	Vec2f upperLeft = mFollowerPosition - radius;
	Vec2f lowerRight = mFollowerPosition + radius;
	dx::drawSolidRect( Rectf( upperLeft, lowerRight ));

	// additional geometries
	dx::drawStrokedCircle( Vec2f( 100.0f, 100.0f ), 50.0f, 7 );
	dx::drawSolidCircle( Vec2f( 100.0f, 100.0f ), 40.0f, 7 );
	dx::drawSolidTriangle( Vec2f( 50.0f, 10.0f ), Vec2f( 30.0f, 30.0f), Vec2f(70.0f, 30.0f)); 


	// here is a simple way of drawing basic messages on the screen
	std::stringstream s;
	s << "Framerate:" << getAverageFps();
	dx::drawString(s.str(),Vec2f(10.0f,10.0f),Color(1.0f, 1.0f, 1.0f) ,mFont);
}

// This line tells Cinder to actually create the application and use the DirectX required
// for Windows Store apps instead of the default OpenGL renderer that comes with the framework
CINDER_APP_BASIC( BasicApp, RendererDx )