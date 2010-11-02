#include "cinder/app/AppBasic.h"
#include "Earth.h"
#include "POV.h"
#include "Resources.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"
#include "cinder/Url.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/gl/TileRender.h"
using namespace ci;
using namespace ci::app;

#include <vector>
#include <sstream>
using std::vector;
using std::string;
using std::istringstream;
using std::stringstream;


class EarthquakeApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void keyDown( KeyEvent event );
	void mouseMove( MouseEvent event );
	void mouseWheel( MouseEvent event );
	void parseEarthquakes( const string &url );
	void setup();
	void update();
	void draw();
	
	gl::GlslProg	mEarthShader;
	gl::GlslProg	mQuakeShader;
	
	gl::Texture		mStars;
	
	POV				mPov;
	Earth			mEarth;
	
	Vec2f			mLastMouse;
	Vec2f			mCurrentMouse;
	
	Vec3f			sBillboardUp, sBillboardRight;
	Vec3f			billboardVecs[2];
	
	Vec3f			mLightDir;
	
	float			mCounter;
	int				mCurrentFrame;
	bool			mSaveFrames;
	bool			mShowEarth;
	bool			mShowLand;
	bool			mShowOcean;
	bool			mShowText;
	bool			mShowQuakes;
};


void EarthquakeApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1024, 768 );
	settings->setFrameRate( 60.0f );
	settings->setResizable( true );
	settings->setFullScreen( false );
}



void EarthquakeApp::setup()
{
	gl::Texture earthDiffuse	= gl::Texture( loadImage( loadResource( RES_EARTHDIFFUSE ) ) );
	gl::Texture earthNormal		= gl::Texture( loadImage( loadResource( RES_EARTHNORMAL ) ) );
	gl::Texture earthMask		= gl::Texture( loadImage( loadResource( RES_EARTHMASK ) ) );
	earthDiffuse.setWrap( GL_REPEAT, GL_REPEAT );
	earthNormal.setWrap( GL_REPEAT, GL_REPEAT );
	earthMask.setWrap( GL_REPEAT, GL_REPEAT );

	mStars						= gl::Texture( loadImage( loadResource( RES_STARS_PNG ) ) );

	
	mEarthShader = gl::GlslProg( loadResource( RES_PASSTHRU_VERT ), loadResource( RES_EARTH_FRAG ) );
	mQuakeShader = gl::GlslProg( loadResource( RES_QUAKE_VERT ), loadResource( RES_QUAKE_FRAG ) );

	
	mCounter		= 0.0f;
	mCurrentFrame	= 0;
	mSaveFrames		= false;
	mShowEarth		= true;
	mShowText		= true;
	mShowQuakes		= true;
	mLightDir		= Vec3f( 0.025f, 0.25f, 1.0f );
	mLightDir.normalize();
	mPov			= POV( this, ci::Vec3f( 0.0f, 0.0f, 1000.0f ), ci::Vec3f( 0.0f, 0.0f, 0.0f ) );
	mEarth			= Earth( earthDiffuse, earthNormal, earthMask );
	
	parseEarthquakes( "http://earthquake.usgs.gov/earthquakes/catalogs/7day-M2.5.xml" );
	
	mEarth.setQuakeLocTip();
}

void EarthquakeApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getCode() == app::KeyEvent::KEY_ESCAPE ) {
		setFullScreen( false );
	}
	else if( event.getChar() == 's' ) {
		mSaveFrames = ! mSaveFrames;
	}
	else if( event.getChar() == 'e' ) {
		mShowEarth = ! mShowEarth;
	}
	else if( event.getChar() == 't' ) {
		mShowText = ! mShowText;
	}
	else if( event.getChar() == 'q' ) {
		mShowQuakes = ! mShowQuakes;
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
	else if( event.getChar() == ' ' ) {
		gl::TileRender tr( 5000, 5000 );
		CameraPersp cam;
		cam.lookAt( mPov.mEye, mPov.mCenter );
		cam.setPerspective( 60.0f, tr.getImageAspectRatio(), 1, 20000 );
		tr.setMatrices( cam );
		while( tr.nextTile() ) {
			draw();
		}
		writeImage( getHomeDirectory() + "output.png", tr.getSurface() );
	}
}


void EarthquakeApp::mouseWheel( MouseEvent event )
{
	mPov.adjustDist( event.getWheelIncrement() * -2.0f );
}


void EarthquakeApp::mouseMove( MouseEvent event )
{
	static bool firstMouseMove = true;
	if( ! firstMouseMove )
		mLastMouse = mCurrentMouse;
	else {
		mLastMouse = event.getPos();
		firstMouseMove = false;
	}
	mCurrentMouse = event.getPos();;
	
	float xd = ( mCurrentMouse.x - mLastMouse.x ) * 0.025f;
	
	mPov.adjustAngle( -xd, mCurrentMouse.y - ( getWindowHeight() * 0.5f ) );
}

void EarthquakeApp::update()
{
	mPov.update();
	mPov.mCam.getBillboardVectors( &sBillboardRight, &sBillboardUp );
	
	//mLightDir = Vec3f( sin( mCounter ), 0.25f, cos( mCounter ) );
	mEarth.update();
	
	mCounter += 0.1f;
}

void EarthquakeApp::draw()
{
	glClearColor( 1.0f, 0.0f, 0.0f, 1.0f );
	
	gl::enableAlphaBlending();
	gl::enableDepthRead( true );
	gl::enableDepthWrite( true );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_TEXTURE_RECTANGLE_ARB );
	gl::GlslProg::unbind();
	
	glColor4f( 1, 1, 1, 1 );
	mStars.enableAndBind();
	gl::drawSphere( Vec3f( 0, 0, 0 ), 15000.0f, 64 );
	
	//gl::rotate( Quatf( Vec3f::zAxis(), -0.2f ) );
	//gl::rotate( Quatf( Vec3f::yAxis(), mCounter*0.1f ) );
	
	if( mShowEarth ){
		mEarthShader.bind();
		mEarthShader.uniform( "texDiffuse", 0 );
		mEarthShader.uniform( "texNormal", 1 );
		mEarthShader.uniform( "texMask", 2 );
		mEarthShader.uniform( "counter", mCounter );
		mEarthShader.uniform( "lightDir", mLightDir );
		mEarth.draw();
		mEarthShader.unbind();
	}
	
	
	glDisable( GL_TEXTURE_2D );
	
	
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	if( mShowQuakes ){
		mQuakeShader.bind();
		mQuakeShader.uniform( "lightDir", mLightDir );
		mEarth.drawQuakeVectors();
		mQuakeShader.unbind();
	}
	
	if( mShowText ){
		gl::enableDepthWrite( false );
		glEnable( GL_TEXTURE_2D );
		//mEarth.drawQuakeLabelsOnBillboard( sBillboardRight, sBillboardUp );
		mEarth.drawQuakeLabelsOnSphere( mPov.mEyeNormal, mPov.mDist );
		glDisable( GL_TEXTURE_2D );
	}
	
	if( mSaveFrames ){
		writeImage( getHomeDirectory() + "CinderScreengrabs" + getPathSeparator() + "Highoutput_" + toString( mCurrentFrame ) + ".png", copyWindowSurface() );
		mCurrentFrame++;
	}
}

void EarthquakeApp::parseEarthquakes( const string &url )
{
	XmlDocument doc( loadUrlStream( url ) );
	vector<XmlElement> items( doc.rootNode().children() );

	for( vector<XmlElement>::iterator itemIter = items.begin(); itemIter != items.end(); ++itemIter ) {
		if( itemIter->name() == "entry" ) {
			// Title is in the form "M 3.3, Puerto Rico region"
			const XmlElement &titleEl = itemIter->findChild( "title" );
			istringstream titleString( titleEl.value() );
			string dummy, title;
			float magnitude;
			titleString.ignore( 2, ' ' );
			titleString >> magnitude;
			titleString.ignore( 2, ' ' );
			char lineBuffer[512];
			titleString.getline( lineBuffer, 511 );
			title = string( lineBuffer );
			
			const XmlElement &locationEl = itemIter->findChild( "georss:point" );
			istringstream locationString( locationEl.value() );
			Vec2f locationVector;
			locationString >> locationVector.x >> locationVector.y;
			
			mEarth.addQuake( locationVector.x, locationVector.y, magnitude, title );
		}
	}
}


CINDER_APP_BASIC( EarthquakeApp, RendererGl )