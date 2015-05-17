#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/ImageIo.h"
#include "cinder/Json.h"
#include "cinder/Log.h"
#include "cinder/Url.h"
#include "cinder/Utilities.h"
#include "cinder/Vector.h"
//#include "cinder/gl/TileRender.h"

#include "Earth.h"
#include "POV.h"
#include "Resources.h"

#include <vector>
#include <sstream>

using namespace ci;
using namespace ci::app;
using namespace std;

class EarthquakeApp : public App {
public:
	static void prepareSettings( Settings *settings );

	void setup();
	void update();
	void draw();

	void keyDown( KeyEvent event );
	void mouseMove( MouseEvent event );
	void mouseWheel( MouseEvent event );

	void parseEarthquakes( const string &url );

public:
	gl::GlslProgRef   mEarthShader;
	gl::GlslProgRef   mQuakeShader;

	gl::Texture2dRef  mStars;

	POV               mPov;
	Earth             mEarth;

	vec2              mLastMouse;
	vec2              mCurrentMouse;

	vec3              sBillboardUp, sBillboardRight;
	vec3              billboardVecs[2];

	vec3			mLightDir;

	float			mCounter;
	int				mCurrentFrame;
	bool			mSaveFrames;
	bool			mShowStars;
	bool			mShowEarth;
	bool			mShowLand;
	bool			mShowOcean;
	bool			mShowText;
	bool			mShowQuakes;
};

void EarthquakeApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1024, 768 );
	settings->disableFrameRate();
	settings->setResizable( true );
	settings->setFullScreen( false );
}

void EarthquakeApp::setup()
{
	auto earthDiffuse = gl::Texture2d::create( loadImage( loadResource( RES_EARTHDIFFUSE ) ) );
	auto earthNormal = gl::Texture2d::create( loadImage( loadResource( RES_EARTHNORMAL ) ) );
	auto earthMask = gl::Texture2d::create( loadImage( loadResource( RES_EARTHMASK ) ) );
	earthDiffuse->setWrap( GL_REPEAT, GL_REPEAT );
	earthNormal->setWrap( GL_REPEAT, GL_REPEAT );
	earthMask->setWrap( GL_REPEAT, GL_REPEAT );

	mStars = gl::Texture2d::create( loadImage( loadResource( RES_STARS_PNG ) ) );

	mEarthShader = gl::GlslProg::create( loadResource( RES_PASSTHRU_VERT ), loadResource( RES_EARTH_FRAG ) );
	mQuakeShader = gl::GlslProg::create( loadResource( RES_QUAKE_VERT ), loadResource( RES_QUAKE_FRAG ) );

	mCounter = 0.0f;
	mCurrentFrame = 0;
	mSaveFrames = false;
	mShowStars = false;
	mShowEarth = false;
	mShowQuakes = true;
	mShowText = false;
	mLightDir = glm::normalize( vec3( 0.025f, 0.25f, 1.0f ) );
	mPov = POV( this, ci::vec3( 0.0f, 0.0f, 1000.0f ), ci::vec3( 0.0f, 0.0f, 0.0f ) );
	mEarth = Earth( earthDiffuse, earthNormal, earthMask );

	parseEarthquakes( "http://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/2.5_week.geojson" );

	mEarth.setQuakeLocTip();
}

void EarthquakeApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( !isFullScreen() );
	}
	else if( event.getCode() == app::KeyEvent::KEY_ESCAPE ) {
		setFullScreen( false );
	}
	else if( event.getChar() == 's' ) {
		mSaveFrames = !mSaveFrames;
	}
	else if( event.getChar() == 'e' ) {
		mShowEarth = !mShowEarth;
	}
	else if( event.getChar() == 't' ) {
		mShowText = !mShowText;
	}
	else if( event.getChar() == 'q' ) {
		mShowQuakes = !mShowQuakes;
	}
	else if( event.getChar() == 'm' ) {
		mEarth.setMinMagToRender( -1.0f );
	}
	else if( event.getChar() == 'M' ) {
		mEarth.setMinMagToRender( 1.0f );
	}
	else if( event.getCode() == app::KeyEvent::KEY_UP ) {
		mPov.adjustDist( -10.0f );
	}
	else if( event.getCode() == app::KeyEvent::KEY_DOWN ) {
		mPov.adjustDist( 10.0f );
	}
	/*//
	else if( event.getChar() == ' ' ) {
		gl::TileRender tr( 5000, 5000 );
		CameraPersp cam;
		cam.lookAt( mPov.mEye, mPov.mCenter );
		cam.setPerspective( 60.0f, tr.getImageAspectRatio(), 1, 20000 );
		tr.setMatrices( cam );
		while( tr.nextTile() ) {
			draw();
		}
		writeImage( getHomeDirectory() / "output.png", tr.getSurface() );
	}
	//*/
}


void EarthquakeApp::mouseWheel( MouseEvent event )
{
	mPov.adjustDist( event.getWheelIncrement() * -2.0f );
}


void EarthquakeApp::mouseMove( MouseEvent event )
{
	static bool firstMouseMove = true;
	if( !firstMouseMove )
		mLastMouse = mCurrentMouse;
	else {
		mLastMouse = event.getPos();
		firstMouseMove = false;
	}
	mCurrentMouse = event.getPos();;

	float xd = ( mCurrentMouse.x - mLastMouse.x ) * 0.01f;

	mPov.adjustAngle( -xd, mCurrentMouse.y - ( getWindowHeight() * 0.5f ) );
}

void EarthquakeApp::update()
{
	mPov.update();
	mPov.mCam.getBillboardVectors( &sBillboardRight, &sBillboardUp );

	//mLightDir = vec3( sin( mCounter ), 0.25f, cos( mCounter ) );
	mEarth.update();

	mCounter += 0.1f;
}

void EarthquakeApp::draw()
{
	gl::clear( Color( 1, 0, 0 ) );

	gl::ScopedDepth       depth( true, true );
	gl::ScopedBlendAlpha  blend;
	gl::ScopedColor       color( 1, 1, 1 );

	// Draw stars.
	if( mShowStars ) {
		gl::ScopedTextureBind tex0( mStars, 0 );
		gl::ScopedFaceCulling cull( true, GL_FRONT );
		gl::ScopedGlslProg shader( gl::getStockShader( gl::ShaderDef().texture() ) );
		gl::drawSphere( vec3( 0, 0, 0 ), 15000.0f, 64 );
	}

	// Draw Earth.
	if( mShowEarth ) {
		gl::ScopedGlslProg shader( mEarthShader );
		mEarthShader->uniform( "texDiffuse", 0 );
		mEarthShader->uniform( "texNormal", 1 );
		mEarthShader->uniform( "texMask", 2 );
		mEarthShader->uniform( "counter", mCounter );
		mEarthShader->uniform( "lightDir", mLightDir );
		mEarth.draw();
	}

	// Draw quakes.
	if( mShowQuakes ) {
		gl::ScopedGlslProg shader( mQuakeShader );
		mQuakeShader->uniform( "lightDir", mLightDir );
		mEarth.drawQuakes();
	}

	if( mShowText ) {
		gl::ScopedDepth depth( true, false );
		//mEarth.drawQuakeLabelsOnBillboard( sBillboardRight, sBillboardUp );
		mEarth.drawQuakeLabelsOnSphere( mPov.mEyeNormal, mPov.mDist );
	}

	if( mSaveFrames ) {
		writeImage( getHomeDirectory() / "CinderScreengrabs" / ( "Highoutput_" + toString( mCurrentFrame ) + ".png" ), copyWindowSurface() );
		mCurrentFrame++;
	}
}

void EarthquakeApp::parseEarthquakes( const string &url )
{
	try {
		const JsonTree json( loadUrl( url ) );
		for( auto &feature : json["features"].getChildren() ) {
			auto &coords = feature["geometry"]["coordinates"];
			float mag = feature["properties"]["mag"].getValue<float>();
			string title = feature["properties"]["title"].getValue();
			CI_LOG_I( "Adding quake:" << title );
			mEarth.addQuake( coords[0].getValue<float>(), coords[1].getValue<float>(), mag, title );
		}
	}
	catch( ci::Exception &exc ) {
		console() << "Failed to parse json, what: " << exc.what() << std::endl;
	}

	//mEarth.addQuake( 37.7f, -122.0f, 8.6f, "San Francisco" );
}


CINDER_APP( EarthquakeApp, RendererGl, &EarthquakeApp::prepareSettings )