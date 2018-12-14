#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/emscripten/CinderEmscripten.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/GainNode.h"
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

	void mouseDrag( MouseEvent event ) override;
	void draw() override;
  void setup() override;
  void onLoad();

  val context = val::undefined();
  val buffer = val::undefined();
  val sourceBuffer = val::undefined();
  val script = val::undefined();
  audio::GenNodeRef	mGen;	// Gen's generate audio signals
	audio::GainNodeRef	mGain;

};

void prepareSettings( WebAudioApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void WebAudioApp::setup()
{

  Context::setMaster( new ContextWebAudio, new DeviceManagerWebAudio );

  auto ctx = audio::master();
  mGen = ctx->makeNode( new audio::GenSineNode );
  mGain = ctx->makeNode( new audio::GainNode );

  mGen->setFreq( 220 );
  mGain->setValue( 0.5f );

  // connections can be made this way or with connect(). The master Context's getOutput() is the speakers by default.
  mGen >> mGain >> ctx->getOutput();
  mGen->enable();
  ctx->enable();
}

void WebAudioApp::mouseDrag( MouseEvent event )
{
  mGen->setFreq( event.getPos().x );
	mGain->setValue( 1.0f - ( float )event.getPos().y / ( float )getWindowHeight() );
}

void WebAudioApp::draw()
{
  gl::clear(Color(0,0,0));
}

// This line tells Cinder to actually create and run the application.
CINDER_APP( WebAudioApp, RendererGl, prepareSettings )
