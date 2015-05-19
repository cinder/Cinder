#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/ImageIo.h"
#include "cinder/Json.h"
#include "cinder/Log.h"
#include "cinder/Url.h"
#include "cinder/Unicode.h"
#include "cinder/Utilities.h"
#include "cinder/Vector.h"

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
	gl::Texture2dRef  mStars;
	gl::BatchRef      mStarSphere;

	POV               mPov;
	Earth             mEarth;

	vec2              mLastMouse;
	vec2              mCurrentMouse;

	vec3              mBillboardUp, mBillboardRight;

	bool              mSaveFrames;
	bool              mShowStars;
	bool              mShowEarth;
	bool              mShowText;
	bool              mShowQuakes;
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
	mStars = gl::Texture2d::create( loadImage( loadResource( RES_STARS_PNG ) ) );
	mStarSphere = gl::Batch::create( geom::Sphere().radius( 15000 ).subdivisions( 60 ), gl::getStockShader( gl::ShaderDef().texture() ) );

	mSaveFrames = false;
	mShowStars = true;
	mShowEarth = true;
	mShowQuakes = true;
	mShowText = true;
	mPov = POV( this, ci::vec3( 0.0f, 0.0f, 1000.0f ), ci::vec3( 0.0f, 0.0f, 0.0f ) );

	parseEarthquakes( "http://earthquake.usgs.gov/earthquakes/feed/v1.0/summary/2.5_week.geojson" );
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
	mEarth.update();
}

void EarthquakeApp::draw()
{
	gl::clear( Color( 1, 0, 0 ) );

	gl::ScopedDepth       depth( true, true );
	gl::ScopedColor       color( 1, 1, 1 );

	// Draw stars.
	if( mShowStars ) {
		gl::ScopedTextureBind tex0( mStars, 0 );
		gl::ScopedFaceCulling cull( true, GL_FRONT );
		mStarSphere->draw();
	}

	// Draw Earth.
	if( mShowEarth ) {
		mEarth.draw();
	}

	// Draw quakes.
	if( mShowQuakes ) {
		mEarth.drawQuakes();
	}

	// Draw labels.
	if( mShowText ) {
		mEarth.drawQuakeLabelsOnSphere( mPov.mEyeNormal, mPov.mDist );

		//mPov.mCam.getBillboardVectors( &mBillboardRight, &mBillboardUp );
		//mEarth.drawQuakeLabelsOnBillboard( mBillboardRight, mBillboardUp );
	}

	if( mSaveFrames ) {
		static int currentFrame = 0;
		writeImage( getHomeDirectory() / "CinderScreengrabs" / ( "Highoutput_" + toString( currentFrame++ ) + ".png" ), copyWindowSurface() );
	}
}

void EarthquakeApp::parseEarthquakes( const string &url )
{
	try {
		const JsonTree json( loadUrl( url ) );
		for( auto &feature : json["features"].getChildren() ) {
			auto &coords = feature["geometry"]["coordinates"];
			float mag = feature["properties"]["mag"].getValue<float>();
			const string &title = feature["properties"]["title"].getValue();

			mEarth.addQuake( coords[0].getValue<float>(), coords[1].getValue<float>(), mag, title );
		}
	}
	catch( ci::Exception &exc ) {
		console() << "Failed to parse json, what: " << exc.what() << std::endl;
	}

	//mEarth.addQuake( 37.7833f, -122.4167f, 8.6f, "San Francisco" );

	mEarth.setQuakeLocTips();
}


CINDER_APP( EarthquakeApp, RendererGl( RendererGl::Options().msaa( 16 ) ), &EarthquakeApp::prepareSettings )