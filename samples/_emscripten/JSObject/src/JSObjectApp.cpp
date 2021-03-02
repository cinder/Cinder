#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include <vector>
#include "cinder/Log.h"
#include "cinder/emscripten/JSObject.h"

#define STRINGIFY(A) #A

using namespace ci;
using namespace ci::app;
using namespace em;
using namespace emscripten;

/**
 * This sample demonstrates how to work with the JSObject class, essentially 
 * a wrapper to help try and make it easier to work with DOM objects. 
 * It uses emscripten::val behind the scenes. 
 */
class JSObjectApp : public App 
{

  public:
	void draw() override;
  void setup() override;
  void onLoad();

  JSObjRef obj;
  int x,y;

};

void prepareSettings( JSObjectApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void JSObjectApp::setup()
{

  obj = JSObject::create();
  obj->set("xPos",app::getWindowWidth() / 2);
  obj->set("yPos",app::getWindowHeight() / 2);

  // right-click and click on inspect to bring up dev-tools (should be a menu item as well.)
  // you'll see a breakdown of the object in the Console tab.
  obj->log();

  x = obj->getIntValue("xPos");
  y = obj->getIntValue("yPos");
  CI_LOG_I(x);
}

void JSObjectApp::draw()
{
  gl::clear( Color( 0,0,0 ));

  gl::setMatricesWindow(app::getWindowSize());
  gl::viewport(app::getWindowSize());

  gl::drawSolidCircle(vec2(x,y),10);
}

CINDER_APP( JSObjectApp, RendererGl, prepareSettings )
