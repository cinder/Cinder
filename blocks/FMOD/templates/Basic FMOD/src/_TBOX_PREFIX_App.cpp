#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Batch.h"

#include "FMOD.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class _TBOX_PREFIX_App : public App {
  public:
	void setup() override;
	void draw() override;

	FMOD::System	*mSystem;
    FMOD::Sound    	*mSound;
	FMOD::Channel	*mChannel;
};

void _TBOX_PREFIX_App::setup()
{
    FMOD::System_Create( &mSystem );
    mSystem->init( 32, FMOD_INIT_NORMAL | FMOD_INIT_ENABLE_PROFILE, NULL );

    mSystem->createSound( getAssetPath( "Blank__Kytt_-_08_-_RSPN.mp3" ).string().c_str(), FMOD_SOFTWARE, NULL, &mSound );
	mSound->setMode( FMOD_LOOP_NORMAL );

	mSystem->playSound( FMOD_CHANNEL_FREE, mSound, false, &mChannel );
}

void _TBOX_PREFIX_App::draw()
{
	gl::clear();
	
	// grab 512 samples of the wave data
	float waveData[512];
	mSystem->getWaveData( waveData, 512, 0 );
	
	// render the 512 samples to a VertBatch
	gl::VertBatch vb( GL_LINE_STRIP );
	for( int i = 0; i < 512; ++i )
		vb.vertex( getWindowWidth() / 512.0f * i, getWindowCenter().y + 100 * waveData[i] );

	// draw the points as a line strip
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	vb.draw();
}

CINDER_APP( _TBOX_PREFIX_App, RendererGl )
