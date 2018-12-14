#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include <vector>
#include <emscripten.h>
#include <emscripten/val.h>
#include "cinder/emscripten/CinderEmscripten.h"
#include "AudioContext.h"
#include "CustomNode.h"

using namespace std;
using namespace ci;
using namespace ci::app;

#define STRINGIFY(A) #A


/**
 * Test to see if it might be possible to build a filewatcher which can then let you know if a 
 * file was changed. 
 */ 
class AudioApi : public App 
{

  public:
	void draw() override;
  void setup() override;
  void onLoad();

  NativeNodeRef osc;

};

void prepareSettings( AudioApi::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void AudioApi::setup()
{
    emscripten::val helpers = em::helpers::getFileHelpers();
    auto ctx = AudioContext::Instance();

    osc = ctx->makeNode(new NativeNode(WebAudioApiNode::OscillatorNode));

    osc->setParam("frequency",255);
    osc >> ctx;

    ctx->enable();
}

void AudioApi::draw()
{
  gl::clear( Color( 0,0,0 )) ;

}


CINDER_APP( AudioApi, RendererGl, prepareSettings )
