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
	void drawFft( audio::TrackRef track );
	void keyDown( KeyEvent e );
	
	audio::TrackRef mTrack1;
	audio::TrackRef mTrack2;
};

void AudioAnalysisSampleApp::setup()
{
	//mTrack1 = audio::Output::addTrack( audio::load( "C:\\code\\cinder\\samples\\AudioPlayback\\resources\\booyah.mp3" ) );
	//mTrack1->setPcmBuffering( true );
	mTrack1 = audio::Output::addTrack( audio::load( loadResource( RES_GUITAR ) ) );
	//mTrack1 = audio::Output::addTrack( audio::load( "../../../../AudioPlayback/resources/booyah.mp3" ) );
	mTrack1->setPcmBuffering( true );
	//mTrack2 = audio::Output::addTrack( audio::load( loadResource( RES_DRUMS ) ) );
	//mTrack2->setPcmBuffering( true );
}

void AudioAnalysisSampleApp::keyDown( KeyEvent e ) {
	if( e.getChar() == 'p' ) {
		( mTrack1->isPlaying() ) ? mTrack1->stop() : mTrack1->play();
	}
}

void AudioAnalysisSampleApp::drawWaveForm( audio::TrackRef track )
{
	audio::PcmBuffer32fRef aPcmBuffer = track->getPcmBuffer();
	if( ! aPcmBuffer ) {
		return;
	}
	
	uint32_t bufferSamples = aPcmBuffer->getSampleCount();
	audio::Buffer32fRef leftBuffer = aPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
	audio::Buffer32fRef rightBuffer = aPcmBuffer->getChannelData( audio::CHANNEL_FRONT_RIGHT );

	int displaySize = getWindowWidth();
	int endIdx = bufferSamples;
	
	float scale = displaySize / (float)endIdx;
	
	glColor3f( 1.0f, 0.5f, 0.25f );
	glBegin( GL_LINE_STRIP );
	for( int i = 0; i < endIdx; i++ ) {
		float y = ( ( leftBuffer->mData[i] - 1 ) * - 100 );
		glVertex2f( ( i * scale ) , y );
	}
	glEnd();
	
	glColor3f( 1.0f, 0.96f, 0.0f );
	glBegin( GL_LINE_STRIP );
	for( int i = 0; i < endIdx; i++ ) {
		float y = ( ( rightBuffer->mData[i] - 1 ) * - 100 );
		glVertex2f( ( i * scale ) , y );
	}
	glEnd();
}

void AudioAnalysisSampleApp::drawFft( audio::TrackRef track )
{
	float ht = 100.0f;
	uint16_t bandCount = 32;
	boost::shared_ptr<float> fftRef = track->computeFft( audio::CHANNEL_FRONT_LEFT, bandCount );
	if( ! fftRef ) {
		return;
	}
	
	float * fftBuffer = fftRef.get();
	
	for( int i = 0; i < ( bandCount ); i++ ) {
		float barY = fftBuffer[i] / bandCount * ht;
		glBegin( GL_QUADS );
			glColor3f( 255.0f, 255.0f, 0.0f );
			glVertex2f( i * 3, ht );
			glVertex2f( i * 3 + 1, ht );
			glColor3f( 0.0f, 255.0f, 0.0f );
			glVertex2f( i * 3 + 1, ht - barY );
			glVertex2f( i * 3, ht - barY );
		glEnd();
	}
}

void AudioAnalysisSampleApp::draw()
{
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	
	glPushMatrix();
		glTranslatef( 0.0, 0.0, 0.0 );
		drawFft( mTrack1 );
		//drawWaveForm( mTrack1 );
		glTranslatef( 0.0, 120.0, 0.0 );
		//drawFft( mTrack2 );
		//drawWaveForm( mTrack2 );
	glPopMatrix();
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( AudioAnalysisSampleApp, RendererGl )
