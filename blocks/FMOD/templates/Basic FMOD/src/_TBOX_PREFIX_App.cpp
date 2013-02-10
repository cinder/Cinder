#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "FMOD.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class _TBOX_PREFIX_App : public AppNative {
  public:
	void setup();
	void draw();

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
	gl::clear( Color( 0, 0, 0 ) );
	
	// grab 512 samples of the wave data
	float waveData[512];
	mSystem->getWaveData( waveData, 512, 0 );
	
	// prep 512 Vec2fs as the positions to render our waveform
	vector<Vec2f> vertices;
	for( int i = 0; i < 512; ++i )
		vertices.push_back( Vec2f( getWindowWidth() / 512.0f * i, getWindowCenter().y + 100 * waveData[i] ) );

	// draw the points as a line strip
	glEnableClientState( GL_VERTEX_ARRAY );
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
    glVertexPointer( 2, GL_FLOAT, 0, &vertices[0] );
    glDrawArrays( GL_LINE_STRIP, 0, vertices.size() );
}

CINDER_APP_NATIVE( _TBOX_PREFIX_App, RendererGl )
