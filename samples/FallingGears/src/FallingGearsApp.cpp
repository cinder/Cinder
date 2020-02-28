// Copyright (c) 2014, Richard Eakin.
//
// Falling Gears - demo 2d physics scroller with interactive audio synthesis.
//
// Author:			Richard Eakin.
// Gear artwork:	Boris Poletaev
// license:			BSD
//
// Outline: Falling gears bounce into shapes (the 'Island' class) and Walls, triggering Synth's.
// AudioController maintains a bank of Synth subclasses - of type AltoSynth for Island collisions
// and BassSynth for wall collisions.  Musical chords are created by the collisions that descend
// through the circle of fifths.  The SceneController class maintains a Box2D physics world, which
// triggers the interaction between visuals and audio.

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"
#include "cinder/Rand.h"
#include "cinder/Text.h"
#include "cinder/CinderImGui.h"

#include "cinder/audio/Utilities.h"
#include "AudioController.h"

#include "Config.h"
#include "SceneController.h"
#include "Box2dUtils.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FallingGearsApp : public App {
  public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void update() override;
	void draw() override;

  private:
	void setupGraphics();
	void drawBackground();
	void drawDebug();
	void drawInfo();

	SceneController			mScene;
	box2d::DebugDraw		mDebugDraw;
	bool					mDrawDebug, mDrawInfo;
	AudioController			mAudio;
	gl::TextureRef			mBackgroundTex;
	float					mFps;
	float					mMasterGain;
};

void FallingGearsApp::setup()
{
	mDrawDebug = false;
	mDrawInfo = false;
	mMasterGain = 84;

	mAudio.setup();
	mAudio.setMasterGain( mMasterGain );

	mScene.setup( &mAudio );
	mScene.getWorld()->SetDebugDraw( &mDebugDraw );

	setupGraphics();
	
	ImGui::Initialize();
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

	if( event.getCode() == KeyEvent::KEY_i ) {
		mDrawInfo = ! mDrawInfo;
	}
	else if( event.getCode() == KeyEvent::KEY_d ) {
		mDrawDebug = ! mDrawDebug;
	}
	else if( event.getCode() == KeyEvent::KEY_EQUALS ) {
		mMasterGain += 5.0f;
	}
	else if( event.getCode() == KeyEvent::KEY_MINUS ) {
		mMasterGain -= 5.0f;
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
	ImGui::Begin( "Settings" );
	ImGui::Text( "framerate: %f", mFps );
	ImGui::Checkbox( "show info", &mDrawInfo );
	ImGui::Checkbox( "show debug", &mDrawDebug );
	if( ImGui::DragFloat( "gain (db)", &mMasterGain, 5.0f ) ) {
		mAudio.setMasterGain( mMasterGain );
	}
	ImGui::DragFloat( "island lowpass", &mAudio.getParams().mAltoLowPassFreq );
	ImGui::Separator();
	ImGui::DragFloat( "gear scale", &mScene.getParams().mGearScale );
	ImGui::DragFloat( "decent speed", &mScene.getParams().mDecentSpeed );
	ImGui::End();

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

	gl::ScopedModelMatrix modelScope;
	gl::translate( 0, decentMod * 0.5f );

	gl::draw( mBackgroundTex, destRect, destRect );

}

void FallingGearsApp::drawDebug()
{
	float pointsPerMeter = mScene.getPointsPerMeter();

	gl::ScopedModelMatrix modelScope;
	gl::scale( pointsPerMeter, pointsPerMeter );
	mScene.getWorld()->DrawDebugData();
}

void FallingGearsApp::drawInfo()
{
	gl::ScopedBlendAlpha blendScope;

	TextLayout layout;
	layout.setFont( Font( "Arial", 14 ) );
	layout.setColor( Color( 1, 1, 0 ) );

	layout.addLine( string( "gears: " + to_string( mScene.getGears().size() ) ) );
	layout.addLine( string( "synths: " + to_string( mAudio.getNumUsedSynths() ) + " / " + to_string( mAudio.getNumTotalSynths() ) ) );

	auto tex = gl::Texture::create( layout.render( true ) );

	vec2 offset( getWindowWidth() - tex->getWidth() - 16, 10 );
	gl::color( Color::white() );
	gl::draw( tex, offset );

}

CINDER_APP( FallingGearsApp, RendererGl( RendererGl::Options().msaa( 8 ) ), []( App::Settings *settings ) {
	settings->setWindowSize( 1200, 800 );
} )
