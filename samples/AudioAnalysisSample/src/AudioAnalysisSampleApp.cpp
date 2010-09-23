#include "cinder/app/AppBasic.h"
#include "cinder/audio/Io.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/FftProcessor.h"
#include "cinder/audio/PcmBuffer.h"

#include "Resources.h"
using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the AppBasic class
class AudioAnalysisSampleApp : public AppBasic {
 public:
	void setup();
	void update();
	void draw();
	void keyDown( KeyEvent e );
	
	void drawWaveForm();
	void drawFft();
	
	audio::TrackRef mTrack;
	audio::PcmBuffer32fRef mPcmBuffer;
};

void AudioAnalysisSampleApp::setup()
{
	//add the audio track the default audio output
	mTrack = audio::Output::addTrack( audio::load( loadResource( RES_GUITAR ) ) );
	
	//you must enable enable PCM buffering on the track to be able to call getPcmBuffer on it later
	mTrack->enablePcmBuffering( true );
}

void AudioAnalysisSampleApp::keyDown( KeyEvent e ) {
	if( e.getChar() == 'p' ) {
		( mTrack->isPlaying() ) ? mTrack->stop() : mTrack->play();
	}
}

void AudioAnalysisSampleApp::update()
{
	//get the latest pcm buffer from the track
	mPcmBuffer = mTrack->getPcmBuffer();
}

void AudioAnalysisSampleApp::draw()
{
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	
	glPushMatrix();
		glTranslatef( 0.0, 0.0, 0.0 );
		drawWaveForm();
#if defined( CINDER_MAC )
		glTranslatef( 0.0, 200.0, 0.0 );
		drawFft();
#endif
	glPopMatrix();
}

void AudioAnalysisSampleApp::drawWaveForm()
{	
	//if the buffer is null, for example if this gets called before any PCM data has been buffered
	//don't do anything
	if( ! mPcmBuffer ) {
		return;
	}
	
	uint32_t bufferLength = mPcmBuffer->getSampleCount();
	audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
	audio::Buffer32fRef rightBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_RIGHT );

	int displaySize = getWindowWidth();
	float scale = displaySize / (float)bufferLength;
	
	PolyLine<Vec2f>	leftBufferLine;
	PolyLine<Vec2f>	rightBufferLine;
	
	for( int i = 0; i < bufferLength; i++ ) {
		float x = ( i * scale );
	
		//get the PCM value from the left channel buffer
		float y = ( ( leftBuffer->mData[i] - 1 ) * - 100 );
		leftBufferLine.push_back( Vec2f( x , y) );
		
		y = ( ( rightBuffer->mData[i] - 1 ) * - 100 );
		rightBufferLine.push_back( Vec2f( x , y) );
	}
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	gl::draw( leftBufferLine );
	gl::draw( rightBufferLine );
	
}

#if defined(CINDER_MAC)
void AudioAnalysisSampleApp::drawFft()
{
	float ht = 100.0f;
	uint16_t bandCount = 32;
	
	if( ! mPcmBuffer ) return;
	
	//use the most recent Pcm data to calculate the Fft
	boost::shared_ptr<float> fftRef = audio::calculateFft( mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT ), bandCount );
	if( ! fftRef ) {
		return;
	}
	
	float * fftBuffer = fftRef.get();
	
	//draw the bands
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
#endif

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( AudioAnalysisSampleApp, RendererGl )
