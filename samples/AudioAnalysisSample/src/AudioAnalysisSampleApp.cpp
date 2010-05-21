#include "cinder/app/AppBasic.h"
#include "cinder/audio/Io.h"
#include "cinder/audio/Output.h"

#include "Resources.h"
using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the AppBasic class
class AudioAnalysisSampleApp : public AppBasic {
 public:
	void setup();
	void draw();
	void drawWaveForm( audio::TrackRef track );
	
	audio::TrackRef mTrack1;
	audio::TrackRef mTrack2;
};

void AudioAnalysisSampleApp::setup()
{
	mTrack1 = audio::Output::addTrack( audio::load( loadResource( RES_GUITAR ) ) );
	mTrack1->setPcmBuffering( true );
	mTrack2 = audio::Output::addTrack( audio::load( loadResource( RES_DRUMS ) ) );
	mTrack2->setPcmBuffering( true );
}

void AudioAnalysisSampleApp::drawWaveForm( audio::TrackRef track )
{
	audio::PcmBuffer32fRef aPcmBuffer = track->getPcmBuffer();
	if( ! aPcmBuffer ) {
		return;
	}
	
	uint32_t bufferSamples = aPcmBuffer->getSampleCount();
	float * aBuffer = aPcmBuffer->getData();

	int displaySize = getWindowWidth();
	int endIdx = bufferSamples;
	
	if( ( endIdx - displaySize ) < 0 ) {
		return;
		//endIdx = aBuffer.mSamplesPerBuffer - 1;
	}
	
	
	int startIdx = endIdx - displaySize;
	
	glBegin( GL_LINE_STRIP );
	for( int i = startIdx; i < endIdx; i++ ) {
		float y = ( ( aBuffer[i] - 1 ) * - 100 );
		glVertex2f( ( i - startIdx ) , y );
	}
	glEnd();
}

void AudioAnalysisSampleApp::draw()
{
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	
	glColor3f( 1.0f, 0.5f, 0.25f );
	
	
	glPushMatrix();
		glTranslatef( 0.0, 0.0, 0.0 );
		drawWaveForm( mTrack1 );
		glTranslatef( 0.0, 120.0, 0.0 );
		drawWaveForm( mTrack2 );
	glPopMatrix();
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( AudioAnalysisSampleApp, RendererGl )
