#include "cinder/app/AppBasic.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Callback.h"
#include "cinder/Rand.h"
#include "cinder/CinderMath.h"
#include "cinder/Cinder.h"
#include "cinder/audio/PcmBuffer.h"

using namespace ci;
using namespace ci::app;

const uint32_t gFreqs[] = { 220, 440, 550, 660, 770, 880, 990 };
const uint32_t gFreqCount = 7;

class SineWave {
 public:
	SineWave( uint32_t freq, float duration );
	~SineWave() {}
	void getData( uint64_t inSampleOffset, uint32_t inSampleCount, ci::audio::Buffer32f *ioBuffer );
 private:
	uint32_t mFreq;
	float mDuration;
};

class AudioGenerativeAdvancedApp : public AppBasic {
 public:
	void setup();
	void mouseDown( MouseEvent event );
	void draw();
	
};

void AudioGenerativeAdvancedApp::setup()
{
}

void AudioGenerativeAdvancedApp::mouseDown( MouseEvent event )
{
	int idx = ( event.getX() / (float)getWindowWidth() ) * gFreqCount;
	
	uint32_t freq = gFreqs[idx];
	audio::Output::play( audio::createCallback( new SineWave( freq, 0.5 ), &SineWave::getData, true ) );
}

void AudioGenerativeAdvancedApp::draw()
{
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

SineWave::SineWave( uint32_t freq, float duration ) 
	: mFreq( freq ), mDuration( duration )
{
}

void SineWave::getData( uint64_t inSampleOffset, uint32_t inSampleCount, ci::audio::Buffer32f *ioBuffer )
{
	if( ( inSampleOffset / 44100.0f ) > mDuration ) {
		ioBuffer->mDataByteSize = 0;
		return;
	}
	
	uint64_t idx = inSampleOffset;
	
	for( int  i = 0; i < inSampleCount; i++ ) {
		
		float val = ci::math<float>::sin( idx * ( mFreq / 44100.0f ) * 2.0f * M_PI );
		
		ioBuffer->mData[i*ioBuffer->mNumberChannels] = val;
		ioBuffer->mData[i*ioBuffer->mNumberChannels + 1] = val;
		idx++;
	}
}

CINDER_APP_BASIC( AudioGenerativeAdvancedApp, RendererGl )
