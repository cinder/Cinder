#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/emscripten/CinderEmscripten.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "Voice.h"
#include "ContextWebAudio.h"
#include "DeviceManagerWebAudio.h"


using namespace std;
using namespace ci;
using namespace ci::app;
using namespace emscripten;
using namespace ci::audio;
using namespace ci::audio::em;

class WebAudioApp : public App {

  public:

	void mouseDown( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void draw() override;
  void setup() override;
  void onLoad();

  audio::em::VoiceRef mVoice;
};

void prepareSettings( WebAudioApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

bool has_suffix(const string&s, const string&suffix){
  return (s.size() >= suffix.size()) && equal(suffix.rbegin(),suffix.rend(),suffix.rbegin());
}



void WebAudioApp::setup(){

  Context::setMaster( new ContextWebAudio, new DeviceManagerWebAudio);
  auto ctx = audio::master();

  // Minor change on Emscripten - because all audio is loaded in an asynchronous manner,
  // it's necessary to provide a callback instead of assignment to a variable,
  // after which, once the callback runs you could then assign to mVoice.
  try {
    audio::em::Voice::create( audio::load( loadResource( "test.ogg" ) ) ,[=](VoiceSamplePlayerNodeRef ref)->void{
    mVoice = ref;
  });

  }catch(ci::Exception &e){
    CI_LOG_E(e.what());
  }
}

void WebAudioApp::mouseDown( MouseEvent event )
{
  // you can call "isDoneLoading()" to determine whether or not
  // your file is done loading and is able to be played back / manipulated.
  if(mVoice->isDoneLoading()){
    CI_LOG_I("done loading");
    // scale volume and pan from window coordinates to 0:1
    float volume = 1.0f - (float)event.getPos().y / (float)getWindowHeight();
    float pan = (float)event.getPos().x / (float)getWindowWidth();

    mVoice->setVolume( volume );
    mVoice->setPan( pan );

    // By stopping the Voice first if it is already playing, start will play from the beginning
    if( mVoice->isPlaying() )
      mVoice->stop();

    mVoice->start();
  }else{
    CI_LOG_I("not done parsing");
  }
}


void WebAudioApp::keyDown( KeyEvent event )
{

  

}

void WebAudioApp::draw()
{
  gl::clear(Color(0,0,0));


}


// This line tells Cinder to actually create and run the application.
CINDER_APP( WebAudioApp, RendererGl, prepareSettings )
