#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include <vector>
#include <emscripten.h>
#include <emscripten/val.h>
#include "cinder/emscripten/CinderEmscripten.h"
#include "Filewatcher.h"

using namespace std;
using namespace ci;
using namespace ci::app;

#define STRINGIFY(A) #A

/**
 * Test to see if it might be possible to build a filewatcher which can then let you know if a 
 * file was changed. 
 */ 
class FilewatcherApp : public App 
{

  public:
	void draw() override;
  void setup() override;
  void onLoad();

  Filewatcher watcher;

  gl::GlslProgRef mShader;

};

void prepareSettings( FilewatcherApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void FilewatcherApp::setup()
{
    emscripten::val helpers = em::helpers::getFileHelpers();
    gl::GlslProg::Format fmt;
    fmt.vertex( app::loadAsset("quad.glsl") );
    fmt.fragment( app::loadAsset("colors.glsl") );

    mShader = gl::GlslProg::create( fmt );


    // note - paths to files will be relative to where you run the server. 
    watcher.addFile( "../emscripten/build/Debug/assets/colors.glsl",[=]( emscripten::val e )->void
    {
      
      // note - you have to completely re-init, not sure why. 
      gl::GlslProg::Format fmt;
      fmt.vertex(app::loadAsset("quad.glsl"));
      fmt.fragment(app::loadAsset("colors.glsl"));

      mShader = gl::GlslProg::create(fmt);

    });
}

void FilewatcherApp::draw()
{
  gl::clear( Color( 0,0,0 )) ;
  gl::ScopedGlslProg shader( mShader );
  mShader->uniform( "time", ( float )app::getElapsedSeconds() );
  mShader->uniform( "resolution",vec2( app::getWindowWidth(),app::getWindowHeight() ) );
  gl::drawSolidRect(sapp::getWindowBounds() );
}

CINDER_APP( FilewatcherApp, RendererGl, prepareSettings )
