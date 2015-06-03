#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/MotionManager.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"

#include <map>

using namespace ci;
using namespace ci::app;
using namespace std;

static const float kTargetSize = 2.0f;
static const float kTargetDistance = 4.0f;
static const float kHorizontalSize = kTargetSize * 4.0f;

class CompassApp : public App {
  public:
	void setup();
	void update();
	void resize();
	void draw();

  private:
	// draw the tex area corresponding to \a d at the specified \a location
	void drawCardinalTex( char d, const vec3 &location, const vec3 &localRotation );

	void drawFps();

	CameraPersp			mCam;
	gl::TextureRef		mCardinalTex;
	map<char, ivec2>	mCardinalPositions;
	ivec2				mCardinalSize;

	gl::BatchRef		mSphereBatch;
};

void CompassApp::setup()
{
	getSignalSupportedOrientations().connect( [] { return InterfaceOrientation::All; } );

	if( ! MotionManager::isNorthReliable() )
		CI_LOG_W( "North is not dependable on your device." );

    MotionManager::enable( 60.0f );

	mCam.setEyePoint( vec3( 0 ) );

	// directions.png contains an image of our 6 directions; this is a map of pixel offsets into that texture
	mCardinalSize = ivec2( 300, 300 );
	mCardinalPositions['U'] = vec2( 0, mCardinalSize.y * 2 );
	mCardinalPositions['D'] = vec2( mCardinalSize.x, mCardinalSize.y * 2 );
	mCardinalPositions['S'] = vec2( 0, mCardinalSize.y );
	mCardinalPositions['W'] = vec2( mCardinalSize.x, mCardinalSize.y );
	mCardinalPositions['N'] = vec2( 0, 0 );
	mCardinalPositions['E'] = vec2( mCardinalSize.x, 0 );

	mCardinalTex = gl::Texture::create( loadImage( loadAsset( "directions.png" ) ) );

	// add something to look at in the background
	auto geom = geom::Sphere().radius( kTargetSize * 4 ).colors() >> geom::Lines();
	auto glsl = gl::getStockShader( gl::ShaderDef().color() );
	mSphereBatch = gl::Batch::create( geom, glsl );
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
	gl::clear();
	gl::color( Color::white() );

    gl::pushMatrices();
    gl::setMatrices( mCam );

	mSphereBatch->draw();

	static const float kD = kTargetDistance;
	drawCardinalTex( 'N', vec3(  0,  0, -kD ),	vec3(  0,	  M_PI,	  0 ) );
	drawCardinalTex( 'S', vec3(  0,  0,  kD ),	vec3(  0,	   0,	  0 ) );
	drawCardinalTex( 'E', vec3(  kD, 0,  0 ),	vec3(  0,	  M_PI/2, 0 ) );
	drawCardinalTex( 'W', vec3( -kD, 0,  0 ),	vec3(  0,	 -M_PI/2, 0 ) );
	drawCardinalTex( 'U', vec3(  0,  kD, 0 ),	vec3( -M_PI/2, 0,     0 ) );
	drawCardinalTex( 'D', vec3(  0, -kD, 0 ),	vec3(  M_PI/2, 0,     0 ) );

	gl::popMatrices();

	drawFps();
}

void CompassApp::drawCardinalTex( char d, const vec3 &location, const vec3 &localRotation )
{
	const float kRectHalfWidth = kTargetSize / 2.0f;
	Rectf destRect( kRectHalfWidth, kRectHalfWidth, -kRectHalfWidth, -kRectHalfWidth );
	ivec2 texPos = mCardinalPositions[d];
	Area srcArea( texPos.x, texPos.y, texPos.x + mCardinalSize.x, texPos.y + mCardinalSize.y);

	gl::pushModelView();

	gl::translate( location );

#if 1
	gl::rotate( angleAxis( localRotation.x, vec3(1,0,0) ) );
	gl::rotate( angleAxis( localRotation.y, vec3(0,1,0) ) );
	gl::rotate( angleAxis( localRotation.z, vec3(0,0,1) ) );
#else
	// draw billboarded, flip to face eye point
	gl::rotate( mCam.getOrientation() );
	gl::rotate( M_PI, vec3( 0, 1, 0 ) );
#endif

	gl::draw( mCardinalTex, srcArea, destRect );

	gl::popModelView();
}

void CompassApp::drawFps()
{
	gl::enableAlphaBlending();
	gl::drawStringRight( toString( floor(getAverageFps()) ) + " fps", vec2( getWindowWidth(), 40.0f), Color( 0, 0, 1 ), Font( "Helvetica", 30 ) );
	gl::disableAlphaBlending();
}

CINDER_APP( CompassApp, RendererGl )
