#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/audio/Voice.h"
#include "cinder/audio/Source.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

class VoiceBasicApp : public App {
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void draw() override;

	audio::VoiceRef mVoice;
};

void VoiceBasicApp::setup()
{
	mVoice = audio::Voice::create( audio::load( loadResource( RES_DRAIN_OGG ) ) );
}

void VoiceBasicApp::mouseDown( MouseEvent event )
{
	// scale volume and pan from window coordinates to 0:1
	float volume = 1.0f - (float)event.getPos().y / (float)getWindowHeight();
	float pan = (float)event.getPos().x / (float)getWindowWidth();

	mVoice->setVolume( volume );
	mVoice->setPan( pan );

	// By stopping the Voice first if it is already playing, start will play from the beginning
	if( mVoice->isPlaying() )
		mVoice->stop();

	mVoice->start();
}

void VoiceBasicApp::keyDown( KeyEvent event )
{
	// space toggles the voice between playing and pausing
	if( event.getCode() == KeyEvent::KEY_SPACE ) {
		if( mVoice->isPlaying() )
			mVoice->pause();
		else
			mVoice->start();
	}
}

void VoiceBasicApp::draw()
{
	gl::clear( Color( 0, 0, 0.2f ) );
}

CINDER_APP( VoiceBasicApp, RendererGl, []( App::Settings *settings ) {
	settings->setMultiTouchEnabled( false );
} )
