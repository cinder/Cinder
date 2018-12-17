#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include <vector>
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>


/**
 * This sample shows one way of how you might integrate a JS library like dat.gui.
 * There are essentially 2 ways of going about this. 
 * 
 * 1. Write JS - this is the easy way at the end of the day. There's less setup and less 
 * weirdness to reason about. 
 * 
 * 2. Write C++ - You can also write pure C++ as well if that's what you're more comfortable with, 
 * that being said, the setup is considerably more tedious and error prone. 
 * 
 * In addition to the following links further down, also see 
 * https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/embind.html#using-val-to-transliterate-javascript-to-c
 * 
 */


#define USE_CPLUSPLUS

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace emscripten;

/**
 * There's a helpful macro called EM_JS that's part of emscripten.h. 
 * It allows you to write JS directly in your C++ code without having to 
 * bother with CMake settings. You can even pass in simple parameters as well if you 
 * want. 
 * 
 * Note that for some reason, it appears that semi-colons are not optional when using this macro. 
 * See 
 * https://kripken.github.io/emscripten-site/docs/api_reference/emscripten.h.html?highlight=em_js#c.EM_JS
 * 
 */
EM_JS(void,load_gui,(),{
  
  window.gui = new dat.GUI();

  window.settings = {
    radius:5
  }
   // remember to call "load_gui" in your c++
});


/**
 * You can also set up C++ instead if you're more comfortable with that. 
 * The process though requires you to be a bit verbose and sometimes the error 
 * messages aren't all that helpful. 
 * 
 * See
 * https://kripken.github.io/emscripten-site/docs/porting/connecting_cpp_and_javascript/embind.html
 * 
 */ 
class AppSettings {
  public:
  float radius = 5.0f;

  int getRadius() const { return radius; }
  void setRadius(int x_) { radius = x_; }

};

// need a proxy so we can return a pointer
AppSettings* makeAppSettings(){
  return new AppSettings();
}

// here we expose our C++ to the JS side. 
EMSCRIPTEN_BINDINGS(ApplicationSettings) {
  class_<AppSettings>("AppSettings")
    .constructor(&makeAppSettings, allow_raw_pointers())
    .property("radius", &AppSettings::getRadius, &AppSettings::setRadius)
    ;
}

// app start. 
class DatGuiApp : public App 
{

  public:
	void draw() override;
  void setup() override;
 
  //! declare placeholder for JS objects + settings.

  //! note for emscripten::val you MUST instantiate the var immediately otherwise 
  //! your build will crash. This happens cause there is no default construtctor .

  emscripten::val gui = emscripten::val::undefined();
#ifdef USE_CPLUSPLUS
  AppSettings * settings;
#else 
  emscripten::val settings = emscripten::val::undefined();
#endif 
};

void prepareSettings( DatGuiApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void DatGuiApp::setup()
{

 #ifdef USE_CPLUSPLUS 
    settings = new AppSettings();
    gui = val::global( "dat" )[ "GUI" ].new_();
    gui.call<void>( "add",val( settings ), val( "radius" ),val( 0 ),val( 100 ) );
 #else 

    load_gui();

    gui = val::global( "gui" );
    settings = val::global( "settings" );
    gui.call<void>( "add",settings,val( "radius" ),val( 0 ),val( 100 ) );
 #endif 
}

void DatGuiApp::draw()
{
  gl::clear( Color( 0,0,0 ) );
  gl::viewport( app::getWindowSize() );
  gl::setMatricesWindow( app::getWindowSize() );
  gl::color( Color( 1,1,0 ) );
  
  auto x = app::getWindowWidth() / 2;
  auto y = app::getWindowHeight() / 2;

  #ifdef USE_CPLUSPLUS
    gl::drawSolidCircle( vec2( x,y ),settings->radius );
  #else 
    auto radius = settings[ "radius" ].as<int>();
    gl::drawSolidCircle( vec2( x,y ),radius );
  #endif 
	
}

CINDER_APP( DatGuiApp, RendererGl, prepareSettings )

