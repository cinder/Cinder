#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include <vector>
#include "cinder/emscripten/HTML.h"
#include "cinder/emscripten/AudioPlayer.h"

using namespace ci;
using namespace ci::app;
using namespace em;
using namespace em::html;
using namespace emscripten;


class HTMLApp : public App 
{

  public:
	void draw() override;
  void setup() override;
  void onLoad();

  HTMLElement img;
  AudioPlayerRef player;
};

void prepareSettings( HTMLApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void HTMLApp::setup()
{

  CI_LOG_I( "This example tests the HTMLElement and AudioPlayer classes." );
  CI_LOG_I( "The AudioPlayer class adds a bit of audio using the <audio> tag. Scroll to the bottome to see the controls." );
  CI_LOG_I( "The HTMLElement class makes it easy to add HTML to your project if needed." );
  CI_LOG_I( "For the audio, controls can be turned off but the browser will need user interaction before the audio plays if you don't use the controls." );

  img = HTMLElement("img");
  img.setAttribute("src","https://www.html5rocks.com/static/images/html5rocks-logo-wings.png");

  img.appendToEl(".emscripten_border");
  img.setAttribute("style", "left: 50%;position: relative;transform: translateX(-50%);");


  auto el = emscripten::val::global( "document" );
  player = AudioPlayer::create("https://ia802900.us.archive.org/16/items/mythium/PVD_TSOWA.mp3",true);

  player->play();
}

void HTMLApp::draw()
{
  gl::clear( Color( 0,0,0 ));

	
}

CINDER_APP( HTMLApp, RendererGl, prepareSettings )
