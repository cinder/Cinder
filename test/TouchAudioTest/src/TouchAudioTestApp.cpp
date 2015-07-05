#include "cinder/app/App.h"
#include "cinder/audio/Input.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

class TouchAudioTestApp : public AppCocoaTouch {
  public:
	virtual void	setup();
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
	gl::setMatricesWindow( getWindowWidth(), getWindowHeight() );
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	
	drawWaveForm();
}

void TouchAudioTestApp::drawWaveForm()
{
	if( ! mPcmBuffer ) {
		return;
	}
	
	uint32_t bufferSamples = mPcmBuffer->getSampleCount();
	audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
	//audio::Buffer32fRef rightBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_RIGHT );
	
	float center = getWindowWidth() / 2;
	float displaySize = getWindowHeight();
	uint32_t endIdx = bufferSamples;
	
	//only draw the last 1024 samples or less
	int32_t startIdx = ( endIdx - 1024 );
	startIdx = math<int32_t>::clamp( startIdx, 0, endIdx );
	
	float scale = displaySize / (float)( endIdx - startIdx );
	//float scale = displaySize / (float)( endIdx );
	
	PolyLine<vec2>	line;
	
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	for( uint32_t i = startIdx, c = 0; i < endIdx; i++, c++ ) {
		float y = ( ( leftBuffer->mData[i] - 1 ) * - center );
		line.push_back( vec2( y, ( c * scale ) ) );
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
