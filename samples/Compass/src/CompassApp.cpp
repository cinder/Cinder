#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/MotionManager.h"
#include "cinder/Utilities.h"

#include "cinder/gl/Fbo.h"
#include "cinder/Triangulate.h"

#include <map>

using namespace ci;
using namespace ci::app;
using namespace std;

static const float kTargetSize = 2.0f;
static const float kTargetDistance = 10.0f;
static const float kHorizontalSize = kTargetSize * 4.0f;


class CompassApp : public AppNative {
  public:
	void setup();
	void update();
	void resize();
	void draw();

  private:

	// create mCardinalTex
	void generateCardinalDirections();

	// render direction to the attached Fbo
	void renderDirection( char d, const Vec2f &offset, const Font &font, const Color &color );

	// draw the tex area corresponding to \a d at the specified \a location
	void drawCardinalTex( char d, const Vec3f &location, const Vec3f &rotation );

	void drawFps();

	CameraPersp mCam;

	// programatically generated spritesheet for cardinal directions:
	gl::Fbo mFbo;
	gl::Texture mCardinalTex;
	map<char, Vec2i> mCardinalPositions;
	Vec2i mCardinalSize;
};

void CompassApp::setup()
{
	getSignalSupportedOrientations().connect( [] { return InterfaceOrientation::All; } );

	if( ! MotionManager::isNorthReliable() )
		console() << "Warning, North is not dependable on your device.\n";

    MotionManager::enable( 60.0f );

	mCam.setEyePoint( Vec3f::zero() );
	generateCardinalDirections();
}

void CompassApp::resize()
{
	bool isPortrait = getWindowHeight() > getWindowWidth();
	float adjustedViewWidth = ( isPortrait ? kHorizontalSize : (kHorizontalSize / kTargetSize) * getWindowAspectRatio() );
	float theta = 2.0f * math<float>::atan2( adjustedViewWidth / 2.0f, kTargetDistance );

	mCam.setPerspective( toDegrees( theta ) , getWindowAspectRatio(), 1, 1000 );
}

void CompassApp::update()
{
	mCam.setOrientation( MotionManager::getRotation( getOrientation() ) );
}

void CompassApp::draw()
{
	gl::clear( Color::black() );
	gl::color( Color::white() );

    gl::pushMatrices();
    gl::setMatrices( mCam );

	static const float kD = kTargetDistance;
	drawCardinalTex( 'N', Vec3f(  0,  0, -kD ),	Vec3f(  0,  180, 0 ) );
	drawCardinalTex( 'S', Vec3f(  0,  0,  kD ),	Vec3f(  0,  0,   0 ) );
	drawCardinalTex( 'E', Vec3f(  kD, 0,  0 ),	Vec3f(  0,  90,  0 ) );
	drawCardinalTex( 'W', Vec3f( -kD, 0,  0 ),	Vec3f(  0, -90,  0 ) );
	drawCardinalTex( 'U', Vec3f(  0,  kD, 0 ),	Vec3f( -90, 0,   0 ) );
	drawCardinalTex( 'D', Vec3f(  0, -kD, 0 ),	Vec3f(  90, 0,   0 ) );

	gl::popMatrices();

	drawFps();
}

void CompassApp::drawCardinalTex( char d, const Vec3f &location, const Vec3f &rotation )
{
	const float kRectHalfWidth = kTargetSize / 2.0f;
	Rectf destRect( kRectHalfWidth, kRectHalfWidth, -kRectHalfWidth, -kRectHalfWidth );
	Vec2i texPos = mCardinalPositions[d];
	Area srcArea( texPos.x, texPos.y, texPos.x + mCardinalSize.x, texPos.y + mCardinalSize.y);

	gl::pushModelView();

	gl::translate( location );

#if 1
	gl::rotate( rotation );
#else
	// draw billboarded, flip to face eye point
	gl::rotate( mCam.getOrientation() );
	gl::rotate( Vec3f( 0, 180, 0 ) );
#endif
	gl::draw( mCardinalTex, srcArea, destRect );

	gl::popModelView();
}

void CompassApp::drawFps()
{
	gl::enableAlphaBlending();
	gl::drawStringRight( toString( floor(getAverageFps()) ) + " fps", Vec2f( getWindowWidth(), 40.0f), Color( 0, 0, 1 ), Font( "Helvetica", 30 ) );
	gl::disableAlphaBlending();
}

void CompassApp::generateCardinalDirections()
{
	mCardinalSize = Vec2i( 300, 300 );
	Font directionFont( "Baskerville-SemiBold", 260 );

	gl::SaveFramebufferBinding bindingSaver;

	mFbo = gl::Fbo( mCardinalSize.x * 2, mCardinalSize.y * 3 );
	mFbo.bindFramebuffer();

	gl::setViewport( mFbo.getBounds() );
	gl::clear( ColorA::zero() );
	gl::setMatricesWindow( mFbo.getWidth(), mFbo.getHeight(), false );

	Vec2f offset = Vec2f::zero();
	renderDirection( 'N', offset, directionFont, Color( 1, 0, 0 ) );
	mCardinalPositions['N'] = offset;

	offset.x = mCardinalSize.x;
	renderDirection( 'E', offset, directionFont, Color( 0, 0, 1 ) );
	mCardinalPositions['E'] = offset;

	offset = Vec2f( 0, 300 );
	renderDirection( 'S', offset, directionFont, Color( 0, 1, 0 ) );
	mCardinalPositions['S'] = offset;

	offset.x = mCardinalSize.x;
	renderDirection( 'W', offset, directionFont, Color( 1, 0.5, 0 ) );
	mCardinalPositions['W'] = offset;

	offset = Vec2f( 0, 600 );
	renderDirection( 'U', offset, directionFont, Color( 1, 1, 0 ) );
	mCardinalPositions['U'] = offset;

	offset.x = mCardinalSize.x;
	renderDirection( 'D', offset, directionFont, Color( 0.6, 0.4, 0.2 ) );
	mCardinalPositions['D'] = offset;

	mCardinalTex = mFbo.getTexture();
}

void CompassApp::renderDirection( char d, const Vec2f &offset, const Font &font, const Color &color )
{
	Rectf rect( offset.x, offset.y, offset.x + mCardinalSize.x, offset.y + mCardinalSize.y );
	Shape2d glyphShape = font.getGlyphShape( font.getGlyphChar( d ) );
	Rectf glypBounds = glyphShape.calcBoundingBox();

	TriMesh2d mesh = Triangulator( glyphShape, 1.0f ).calcMesh( Triangulator::WINDING_ODD );

	float xCorrect = -glypBounds.getCenter().x + glypBounds.x1 / 2.0f;
	float yCorrect = glypBounds.getCenter().y - glypBounds.y1;

	gl::color( color );
	gl::drawSolidRoundedRect( rect, rect.getWidth() / 5 );

	gl::pushModelView();
	gl::translate( rect.getCenter().x + xCorrect, rect.getCenter().y + yCorrect );
	gl::color( Color::black() );
	gl::draw( mesh );
	gl::popModelView();
}


CINDER_APP_NATIVE( CompassApp, RendererGl )
