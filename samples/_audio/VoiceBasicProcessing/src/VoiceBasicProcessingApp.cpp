#include "cinder/app/AppNative.h"
#include "cinder/audio/Voice.h"

#include <atomic>

using namespace ci;
using namespace ci::app;

class VoiceBasicProcessingApp : public AppNative {
public:
	void setup();
	void mouseDown( MouseEvent );
	void mouseDrag( MouseEvent );
	void handleMove( Vec2f pos );
	void draw();

	audio::VoiceRef mVoice;

	// Any a variable can be modified from both the audio thread and any other thread,
	// it should use be protected by some sort of synchronization mechanism. std::atomic is a good choice for simple data types.
	std::atomic<float> mFreq;

	//! mPhase does not need to be atomic, since it is only updated on the audio thread after processing begins.
	float mPhase;
};

void VoiceBasicProcessingApp::setup()
{
	mFreq = 0;
	mPhase = 0;

	mVoice = audio::Voice::create( [this] ( audio::Buffer *buffer, size_t sampleRate ) {

		float *channel0 = buffer->getChannel( 0 );

#if 1
		// generate a sine wave
		float phaseIncr = ( mFreq / (float)sampleRate ) * 2 * (float)M_PI;
		for( size_t i = 0; i < buffer->getNumFrames(); i++ )	{
			mPhase = fmodf( mPhase + phaseIncr, 2 * M_PI );
			channel0[i] = std::sin( mPhase );
		}
#else
		// generate a triangle wave
		float phaseIncr = ( mFreq / (float)sampleRate );
		for( size_t i = 0; i < buffer->getNumFrames(); i++ )	{
			mPhase = fmodf( mPhase + phaseIncr, 1 );
			channel0[i] = std::min( mPhase, 1 - mPhase );
		}
#endif
	} );

	mVoice->start();
}

void VoiceBasicProcessingApp::mouseDown( MouseEvent event )
{
	handleMove( event.getPos() );
}

void VoiceBasicProcessingApp::mouseDrag( MouseEvent event )
{
	handleMove( event.getPos() );
}

void VoiceBasicProcessingApp::handleMove( Vec2f pos )
{
	mFreq = pos.x;

	float volume = 1.0f - (float)pos.y / (float)getWindowHeight();
	mVoice->setVolume( volume );
}

void VoiceBasicProcessingApp::draw()
{
	Color backgroundColor( 0, mVoice->getVolume(), mFreq / (float)getWindowWidth() );
	gl::clear( backgroundColor );
}

CINDER_APP_NATIVE( VoiceBasicProcessingApp, RendererGl )