#include "cinder/app/AppCocoaTouch.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Io.h"

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
	
	audio::SourceRef mAudioSource;
};

void TouchAudioTestApp::setup()
{
	mAudioSource = audio::load( loadResource( RES_BOOYAH ) );
}

void TouchAudioTestApp::resize( int width, int height )
{
	
}

void TouchAudioTestApp::touchesBegan( TouchEvent event )
{
	std::cout << "Touch" << std::endl;
	audio::Output::play( mAudioSource );
}

void TouchAudioTestApp::update()
{
	
}

void TouchAudioTestApp::draw()
{
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );
}

CINDER_APP_COCOA_TOUCH( TouchAudioTestApp, RendererGl )
