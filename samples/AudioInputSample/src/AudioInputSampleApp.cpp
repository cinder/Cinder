#include "cinder/app/AppBasic.h"
#include "cinder/audio/Input.h"

using namespace ci;
using namespace ci::app;

class AudioInputSampleApp : public AppBasic {
 public:
	void setup();
	void update();
	void draw();
	
	audio::Input * mInput;
};

void AudioInputSampleApp::setup()
{
	//initialize the audio Input, using the default input device
	mInput = new audio::Input();
	
	//tell the input to start capturing audio
	mInput->start();
	
}

void AudioInputSampleApp::update()
{

}

void AudioInputSampleApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( AudioInputSampleApp, RendererGl )
