#include "cinder/Cinder.h"

#if defined( CINDER_COCOA_TOUCH )
	#include "cinder/app/AppCocoaTouch.h"
	typedef ci::app::AppCocoaTouch AppBase;
#else
	#include "cinder/app/AppBasic.h"
	#include "cinder/audio/FftProcessor.h"
	typedef ci::app::AppBasic AppBase;
#endif

#include "cinder/audio/Input.h"
#include <iostream>
#include <vector>

using namespace ci;
using namespace ci::app;

class AudioInputSampleApp : public AppBase {
 public:
	void setup();
	void update();
	void draw();
	void drawWaveForm( float height );

#if defined(CINDER_MAC)
	void drawFft();
#endif
	
	audio::Input mInput;
	boost::shared_ptr<float> mFftDataRef;
	audio::PcmBuffer32fRef mPcmBuffer;
};

void AudioInputSampleApp::setup()
{
	//iterate input devices and print their names to the console
	const std::vector<audio::InputDeviceRef>& devices = audio::Input::getDevices();
	for( std::vector<audio::InputDeviceRef>::const_iterator iter = devices.begin(); iter != devices.end(); ++iter ) {
		console() << (*iter)->getName() << std::endl;
	}

	//initialize the audio Input, using the default input device
	mInput = audio::Input();
	
	//tell the input to start capturing audio
	mInput.start();
	
}

void AudioInputSampleApp::update()
{
	mPcmBuffer = mInput.getPcmBuffer();
	if( ! mPcmBuffer ) {
		return;
	}
#if defined( CINDER_MAC )
	uint16_t bandCount = 512;
	//presently FFT only works on OS X, not iOS
	mFftDataRef = audio::calculateFft( mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT ), bandCount );
#endif
}

void AudioInputSampleApp::draw()
{
#if defined( CINDER_COCOA_TOUCH )
	float waveFormHeight = getWindowWidth() / 2;
#else
	float waveFormHeight = 100.0;
#endif

	gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );
	
	glPushMatrix();
		drawWaveForm( waveFormHeight );
#if defined(CINDER_MAC)
		glTranslatef( 0.0f, 200.0f, 0.0f );
		drawFft();
#endif
	glPopMatrix();
}

void AudioInputSampleApp::drawWaveForm( float height )
{
	if( ! mPcmBuffer ) {
		return;
	}
	
	uint32_t bufferSamples = mPcmBuffer->getSampleCount();
	audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
	//audio::Buffer32fRef rightBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_RIGHT );

	int displaySize = getWindowWidth();
	int endIdx = bufferSamples;
	
	//only draw the last 1024 samples or less
	int32_t startIdx = ( endIdx - 1024 );
	startIdx = math<int32_t>::clamp( startIdx, 0, endIdx );
	
	float scale = displaySize / (float)( endIdx - startIdx );
	
	PolyLine<Vec2f>	line;
	
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	for( uint32_t i = startIdx, c = 0; i < endIdx; i++, c++ ) {
		float y = ( ( leftBuffer->mData[i] - 1 ) * - 100 );
		line.push_back( Vec2f( ( c * scale ), y ) );
	}
	gl::draw( line );
	

}

#if defined(CINDER_MAC)
void AudioInputSampleApp::drawFft()
{
	uint16_t bandCount = 512;
	float ht = 1000.0f;
	float bottom = 150.0f;
	
	if( ! mFftDataRef ) {
		return;
	}
	
	float * fftBuffer = mFftDataRef.get();
	
	for( int i = 0; i < ( bandCount ); i++ ) {
		float barY = fftBuffer[i] / bandCount * ht;
		glBegin( GL_QUADS );
			glColor3f( 255.0f, 255.0f, 0.0f );
			glVertex2f( i * 3, bottom );
			glVertex2f( i * 3 + 1, bottom );
			glColor3f( 0.0f, 255.0f, 0.0f );
			glVertex2f( i * 3 + 1, bottom - barY );
			glVertex2f( i * 3, bottom - barY );
		glEnd();
	}
}
#endif

#if defined( CINDER_COCOA_TOUCH )
CINDER_APP_COCOA_TOUCH( AudioInputSampleApp, RendererGl )
#else
CINDER_APP_BASIC( AudioInputSampleApp, RendererGl )
#endif
