#include "cinder/app/AppCocoaTouch.h"
#include "cinder/audio/Input.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

class TouchAudioTestApp : public AppCocoaTouch {
  public:
	virtual void	setup();
	virtual void	resize( int width, int height );
	virtual void	update();
	virtual void	draw();
	virtual void	touchesBegan( TouchEvent event );
	
	audio::Input mInput;
	audio::PcmBuffer32fRef mPcmBuffer;
	void drawWaveForm();
};

void TouchAudioTestApp::setup()
{
	//mAudioSource = audio::load( loadResource( RES_BOOYAH ) );
	//initialize the audio Input, using the default input device
	mInput = audio::Input();
	
	//tell the input to start capturing audio
	mInput.start();
	
}

void TouchAudioTestApp::resize( int width, int height )
{
	
}

void TouchAudioTestApp::touchesBegan( TouchEvent event )
{
	//std::cout << "Touch" << std::endl;
	//audio::Output::play( mAudioSource );
}

void TouchAudioTestApp::update()
{
	mPcmBuffer = mInput.getPcmBuffer();
	if( ! mPcmBuffer ) {
		return;
	}
}

void TouchAudioTestApp::draw()
{
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
}

void TouchAudioTestApp::drawWaveForm()
{
	if( ! mPcmBuffer ) {
		return;
	}
	
	uint32_t bufferSamples = mPcmBuffer->getSampleCount();
	audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
	//audio::Buffer32fRef rightBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_RIGHT );

	int displaySize = getWindowWidth();
	int endIdx = bufferSamples;
	
	float scale = displaySize / (float)endIdx;
	
	PolyLine<Vec2f>	line;
	
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	for( int i = 0; i < endIdx; i++ ) {
		float y = ( ( leftBuffer->mData[i] - 1 ) * - 100 );
		line.push_back( Vec2f( ( i * scale ) , y) );
	}
	gl::draw( line );
	
	/*glColor3f( 1.0f, 0.96f, 0.0f );
	glBegin( GL_LINE_STRIP );
	for( int i = 0; i < endIdx; i++ ) {
		float y = ( ( rightBuffer->mData[i] - 1 ) * - 100 );
		glVertex2f( ( i * scale ) , y );
	}
	glEnd();*/
}


CINDER_APP_COCOA_TOUCH( TouchAudioTestApp, RendererGl )
