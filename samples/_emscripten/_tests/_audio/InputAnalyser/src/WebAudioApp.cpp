#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/emscripten/CinderEmscripten.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/GainNode.h"
#include "ContextWebAudio.h"
#include "DeviceManagerWebAudio.h"
#include "NativeWebAudioNode.h"



using namespace std;
using namespace ci;
using namespace ci::app;
using namespace emscripten;
using namespace ci::audio;
using namespace ci::audio::em;

class WebAudioApp : public App {

  public:

	void mouseDrag( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void draw() override;
  void setup() override;
  void onLoad();

  val context = val::undefined();
  val buffer = val::undefined();
  val sourceBuffer = val::undefined();
  val script = val::undefined();

  audio::InputDeviceNodeRef		mInputDeviceNode;
  audio::em::NativeWebAudioNodeRef mNativeNode;
  audio::em::NativeWebAudioNodeRef mNativeNode2;
  	audio::GainNodeRef	mGain;



};

void prepareSettings( WebAudioApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}



void WebAudioApp::setup(){

  Context::setMaster( new ContextWebAudio, new DeviceManagerWebAudio);

  auto ctx = audio::master();
  auto rawContext = (ContextWebAudio*)ctx;

  mGain = ctx->makeNode(new ci::audio::GainNode);

  // create an input device.
  mInputDeviceNode = ctx->createInputDeviceNode();

  // init native node. You can see a list of all possible types by iterating
  // cinder::audio::NativeWebTypes which is a map. 
  mNativeNode = ctx->makeNode(new audio::em::NativeWebAudioNode);

  // create a delay node
  mNativeNode->createNode("DelayNode");


  // connect to the context directly(will technically connect to ScriptProcessorNode underneath
  // in order to put the information into the main processing loop.)
  //rawContext->connectNativeNode(mNativeNode);

  //mNativeNode >> ctx->getOutput();
  mNativeNode >> mGain;

  // enable the context so audio starts processing. 
  ctx->enable();
}

void WebAudioApp::mouseDrag( MouseEvent event )
{

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
