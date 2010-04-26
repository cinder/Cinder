#include "cinder/app/AppBasic.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Callback.h"
#include "cinder/CinderMath.h"

using namespace ci;
using namespace ci::app;

float gFreqTarget = 0.0f;
float gPhase = 0.0f;
float gPhaseAdjust = 0.0f;
const float gMaxFreq = 2000.0f;

void sineWave( uint64_t inSampleOffset, uint32_t *ioSampleCount, audio::Buffer *ioBuffer ) {
	float * buffer = (float *)ioBuffer->mData;
	gPhaseAdjust = gPhaseAdjust * 0.95f + ( gFreqTarget / 44100.0f ) * 0.05f;
	for( int  i = 0; i < *ioSampleCount; i++ ) {
		gPhase += gPhaseAdjust;
		float r = gPhase - math<float>::floor( gPhase );
		float val = math<float>::sin( r * 2.0f * M_PI );
		
		buffer[i*ioBuffer->mNumberChannels] = val;
		buffer[i*ioBuffer->mNumberChannels + 1] = val;
	}
}

class AudioGenerativeApp : public AppBasic {
 public:
	void setup();
	void mouseMove( MouseEvent event );
	void draw();
	
};

void AudioGenerativeApp::setup()
{
	audio::Output::addTrack( audio::Callback( &sineWave ) );
}

void AudioGenerativeApp::mouseMove( MouseEvent event )
{
	int height = getWindowHeight();
	//gFreqTarget = ( height - event.getY() ) / (float)height * gMaxFreq/ 44100.0f;
	gFreqTarget = math<float>::clamp( ( height - event.getY() ) / (float)height * gMaxFreq, 0.0, 2000.0 );
}

void AudioGenerativeApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( AudioGenerativeApp, RendererGl )
