#include "cinder/app/AppBasic.h"
#include "cinder/audio/Io.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/Callback.h"
#include "cinder/audio/PcmBuffer.h"

#if defined(CINDER_MSW)
	#include "cinder/audio/SourceFileWindowsMedia.h"
#endif

#include "Resources.h"

using namespace ci;	
using namespace ci::app;
using namespace std;


class AudioNewTestApp : public AppBasic {
 public:
	void setup();
	
	void mouseDown( MouseEvent event );
	void keyDown( KeyEvent event );
	void fileDrop( FileDropEvent event );

	void update();
	void draw();
	
	audio::SourceRef mAudioSource;
	audio::TrackRef mTrack1;
	audio::TrackRef mTrack2;
//	audio::Callback mSineCallback;
};

void AudioNewTestApp::setup()
{
	//mAudioSource = audio::load( loadResource( "guitar.mp3", RES_GUITAR_MP3, "MP3" ) );
	//console() << mAudioSource->getDuration() << std::endl;
	//mAudioSource = audio::load( "..\\data\\guitar.mp3" );
	mAudioSource = audio::load( "../../../data/booyah_short.wav" );
	audio::TargetRef target( audio::Output::getTarget() );
	audio::LoaderRef loader( mAudioSource->createLoader( target.get() ) );
	
	audio::BufferList32fRef buffer( audio::createBufferList<float>( 1024, 2, target->isInterleaved() ) );
	loader->loadData( (audio::BufferList *)buffer.get() );
	
	for( int i = 0; i < 1024; i++ ) {
		console() << buffer->mBuffers[0].mData[i] << std::endl;
	}
	
	
	
	//mTrack1 = audio::Output::addTrack( audio::load( loadResource( "guitar.mp3", RES_GUITAR_MP3, "MP3" ) ) );
	//mTrack2 = audio::Output::addTrack( audio::load( loadResource( "drums.mp3", RES_DRUMS_MP3, "MP3" ) ) );
	
	//mTrack1 = audio::Output::addTrack( audio::load( loadResource( "booyah.mp3", RES_BOOYAH_MP3, "MP3" ) ) );
	//mTrack1->setLooping( true );
	//mSineCallback = audio::Callback( &sineWave );
	
	
}

void AudioNewTestApp::mouseDown( MouseEvent event )
{
	//mTrack1->setTime( 1.0 );
	//mTrack1 = audio::Output::addTrack( mAudioSource );
}

void AudioNewTestApp::keyDown( KeyEvent event )
{
	//float volume = audio::Output::getVolume();
	/*float volume = mTrack1->getVolume();
	switch( event.getChar() ) {
		case 'q':
			//audio::Output::setVolume( volume + 0.1f );
			mTrack1->setVolume( volume + 0.1f );
		break;
		case 'a':
			//audio::Output::setVolume( volume - 0.1f );
			mTrack1->setVolume( volume - 0.1f );
		break;
		case 'w':
			mTrack1->setTime( 1.5 );
			console() << mTrack1->getTime() << std::endl;
		break;
	}
	console() << mTrack1->getVolume() << std::endl;*/
}

void AudioNewTestApp::fileDrop( FileDropEvent event )
{
	
}

void AudioNewTestApp::update()
{
	//console() << mTrack1->getTime() << std::endl;
}

void AudioNewTestApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableAlphaBlending();
}

CINDER_APP_BASIC( AudioNewTestApp, RendererGl );
