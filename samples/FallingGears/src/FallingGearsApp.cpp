// Copyright (c) 2014, Richard Eakin.
//
// Falling Gears - demo 2d physics scroller with interactive audio synthesis.
//
// Author:			Richard Eakin.
// Gear artwork:	Boris Poletaev
// license:			BSD
//
// Outline: Falling gears bounce into shapes (the 'Island' class) and Walls, triggering Synth's.
// AudioController maintain's a bank of Synth subclasses - of type AltoSynth for Island collisions
// and BassSynth for wall colissions.  Musical chords are created by the collisions that descend
// through the circle of fifths.  The SceneController class maintains a Box2D physics world, which
// triggers the interaction between visuals and audio.

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"
#include "cinder/Rand.h"
#include "cinder/Text.h"
#include "cinder/params/Params.h"

#include "cinder/audio/Utilities.h"
#include "cinder/audio/Debug.h"
#include "AudioController.h"

#include "Config.h"
#include "SceneController.h"
#include "Box2dUtils.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FallingGearsApp : public AppNative {
  public:
	void prepareSettings( Settings *settings );
	void setup();
	void keyDown( KeyEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseUp( MouseEvent event );
	void update();
	void draw();

	void setupGraphics();
	void setupParams();
	void drawBackground();
	void drawDebug();
	void drawInfo();

	SceneController			mScene;
	box2d::DebugDraw		mDebugDraw;
	bool					mDrawDebug, mDrawInfo;
	AudioController			mAudio;
	gl::TextureRef			mBackgroundTex;
	params::InterfaceGlRef	mParams;
	float					mFps;
	float					mMasterGain;
};

void FallingGearsApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1200, 800 );
}

void FallingGearsApp::setup()
{
	mDrawDebug = false;
	mDrawInfo = false;
	mMasterGain = 84.0f;

	mAudio.setup();
	mAudio.setMasterGain( mMasterGain );

	mScene.setup( &mAudio );
	mScene.getWorld()->SetDebugDraw( &mDebugDraw );

	setupGraphics();
	setupParams();
}

void FallingGearsApp::setupGraphics()
{
	auto imageSource = Config::instance()->getBackgroundImage();
	if( imageSource ) {
		auto format = gl::Texture::Format();
		format.setWrap( GL_REPEAT, GL_REPEAT );
		mBackgroundTex = gl::Texture::create( imageSource, format );
	}
}

void FallingGearsApp::setupParams()
{
	mParams = params::InterfaceGl::create( "params", Vec2i( 250, 350 ) );
	mParams->minimize();
	
	mParams->addParam( "fps", &mFps, "", true );
	mParams->addParam( "show info", &mDrawInfo ).key( "i" );
	mParams->addParam( "show debug", &mDrawDebug ).key( "d" );
	mParams->addSeparator();
	mParams->addParam( "gain (db)", &mMasterGain ).step( 5 ).keyIncr( "=" ).keyDecr( "-" ).updateFn( [this] { mAudio.setMasterGain( mMasterGain ); } );
	mParams->addParam( "island lowpass", &mAudio.getParams().mAltoLowPassFreq );
	mParams->addSeparator();
	mParams->addParam( "gear scale", &mScene.getParams().mGearScale );
	mParams->addParam( "decent speed", &mScene.getParams().mDecentSpeed );
}

void FallingGearsApp::keyDown( KeyEvent event )
{
#if defined( CINDER_COCOA )
	bool isModDown = event.isMetaDown();
#else // windows
	bool isModDown = event.isControlDown();
#endif

	if( isModDown ) {
		if( event.getCode() == KeyEvent::KEY_SLASH )
			audio::master()->setEnabled( ! audio::master()->isEnabled() );
		else if( event.getChar() == 'r' ) {
			Config::instance()->reload();

			setupGraphics();

			mScene.reload();
			mAudio.reset();
		}
		else if( event.getChar() == 'c' )
			mDebugDraw.setCenterOfMassEnabled( ! mDebugDraw.isCenterOfMassEnabled() );
	}
}

void FallingGearsApp::mouseDown( MouseEvent event )
{
	mScene.addGear( event.getPos() );
}

void FallingGearsApp::mouseDrag( MouseEvent event )
{
	mScene.addGear( event.getPos() );
}

void FallingGearsApp::mouseUp( MouseEvent event )
{
}

void FallingGearsApp::update()
{
	mScene.update();

	mAudio.update();

	mFps = getAverageFps();
}

void FallingGearsApp::draw()
{
	gl::clear();

	if( mDrawDebug )
		drawDebug();
	else {
		gl::enableAlphaBlending();
		gl::color( Color::white() );
		drawBackground();

		for( const auto &gear : mScene.getGears() )
			gear->draw();

		for( const auto &islandVec : mScene.getIslands() ) {
			for( const auto &island : islandVec )
				island->draw();
		}

		for( const auto &wall : mScene.getWalls() )
			wall->draw();
	}

	if( mDrawInfo )
		drawInfo();

	mParams->draw();
}

void FallingGearsApp::drawBackground()
{
	if( ! mBackgroundTex )
		return;

	Area destRect = mBackgroundTex->getBounds();
	destRect.x2 *= 8;
	destRect.y2 *= 8;

	float decentMod = fmodf( mScene.getCurrentDecent(), mBackgroundTex->getHeight() * 2 );

	gl::color( Color::white() );

	gl::pushModelView();
	gl::translate( 0, decentMod * 0.5f );

	gl::draw( mBackgroundTex, destRect, destRect );

	gl::popModelView();
}

void FallingGearsApp::drawDebug()
{
	float pointsPerMeter = mScene.getPointsPerMeter();
	gl::pushModelView();
		gl::scale( pointsPerMeter, pointsPerMeter );
		mScene.getWorld()->DrawDebugData();
	gl::popModelView();
}

void FallingGearsApp::drawInfo()
{
	gl::enableAlphaBlending();

	TextLayout layout;
	layout.setFont( Font( "Arial", 14 ) );
	layout.setColor( Color( 1, 1, 0 ) );

	layout.addLine( string( "gears: " + to_string( mScene.getGears().size() ) ) );
	layout.addLine( string( "synths: " + to_string( mAudio.getNumUsedSynths() ) + " / " + to_string( mAudio.getNumTotalSynths() ) ) );

	auto tex = gl::Texture::create( layout.render( true ) );

	Vec2f offset( getWindowWidth() - tex->getWidth() - 16, 10 );
	gl::color( Color::white() );
	gl::draw( tex, offset );

	gl::disableAlphaBlending();
}

CINDER_APP_NATIVE( FallingGearsApp, RendererGl )
