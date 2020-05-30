#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include <vector>
#include "cinder/emscripten/HTML.h"
#include <emscripten/emscripten.h>
using namespace std;
using namespace ci;
using namespace ci::app;
using namespace ci::em::html;
using namespace emscripten;



class HTMLApp : public App 
{

  public:
	void draw() override;
  void setup() override;
  void onLoad();
  
  //! Helper element so you don't have to worry about nuaunces of val elements
  //HTMLElement img;

};

void prepareSettings( HTMLApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void HTMLApp::setup()
{

  auto el = emscripten::val::global( "document" );


    //img = HTMLElement("img");
    //img.setAttribute("src","https://upload.wikimedia.org/wikipedia/commons/thumb/5/52/Flickr_wordmark.svg/514px-Flickr_wordmark.svg.png");

    //img.appendToEl(".emscripten_border");
    //img.setAttribute("style", "left: 50%;position: relative;transform: translateX(-50%);");
}

void HTMLApp::draw()
{
  gl::clear( Color( 0,0,0 ));

	
}

CINDER_APP( HTMLApp, RendererGl, prepareSettings )
