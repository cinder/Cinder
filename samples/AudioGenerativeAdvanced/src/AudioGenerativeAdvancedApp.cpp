#include "cinder/app/AppBasic.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Callback.h"
#include "cinder/Rand.h"
#include "Wave.h"

using namespace ci;
using namespace ci::app;

uint32_t gFreqs[] = { 110, 220, 440, 550, 660, 770, 880, 990 };
uint32_t gFreqCount = 8;

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

CINDER_APP_BASIC( AudioGenerativeAdvancedApp, RendererGl )
