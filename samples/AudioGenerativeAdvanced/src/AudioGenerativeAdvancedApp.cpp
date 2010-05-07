#include "cinder/app/AppBasic.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Callback.h"
#include "Wave.h"

using namespace ci;
using namespace ci::app;

class AudioGenerativeAdvancedApp : public AppBasic {
 public:
	void setup();
	void mouseDown( MouseEvent event );
	void draw();
};

void AudioGenerativeAdvancedApp::setup()
{
	SineWave * sw = new SineWave( 550, 2 );
	audio::Output::play( audio::createCallback( *sw, &SineWave::getData ) );
}

void AudioGenerativeAdvancedApp::mouseDown( MouseEvent event )
{
	
}

void AudioGenerativeAdvancedApp::draw()
{
	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

CINDER_APP_BASIC( AudioGenerativeAdvancedApp, RendererGl )
