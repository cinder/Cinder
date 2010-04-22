#include "cinder/app/AppBasic.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Io.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

class AudioPlaybackApp : public AppBasic {
 public:
	void setup();
	void mouseDown( MouseEvent );
	void draw();
	
	audio::SourceRef mAudioSource;
};

void AudioPlaybackApp::setup()
{
	mAudioSource = audio::load( loadResource( "booyah.mp3", RES_BOOYAH_MP3, "MP3" ) );
}

void AudioPlaybackApp::mouseDown( MouseEvent event )
{
	audio::Output::play( mAudioSource );
}

void AudioPlaybackApp::draw()
{
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

CINDER_APP_BASIC( AudioPlaybackApp, RendererGl )
