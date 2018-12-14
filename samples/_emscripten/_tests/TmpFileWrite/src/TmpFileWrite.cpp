#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include <vector>
#include <emscripten.h>
#include <emscripten/val.h>
#include "cinder/emscripten/CinderEmscripten.h"

using namespace std;
using namespace ci;
using namespace ci::app;

#define STRINGIFY(A) #A

EM_JS(void,mountFS,(),{
  FS.mkdir('/cinder');
  FS.mount(MEMFS,{},'/cinder');
});

EM_JS(void,writeFile,(const char * filename, const char * contents),{
  var file = UTF8ToString(contents);

  FS.writeFile('/cinder/' + filename,file);

});



/**
 * Testing if it's possible to write a file to tmp memory filesystem with the Filesystem api, 
 * then load it on JS side. Possibly useful for AudioWorklets in the future. 
 * 
 * Conclusions on 11/30 
 * - Not possible :(
 * - but it looks like it's possible to use Blob urls to load Worklets and Workers!
 */
class TmpFileWrite : public App 
{

  public:
	void draw() override;
  void setup() override;
  void onLoad();


  std::string worklet = STRINGIFY(
    class Processor extends AudioWorkletProcessor {
      constructor(){
        super();
      }

      process(inputs,outputs,parameters){

      }
    }

    registerProcessor('cinder-worklet',Processor);
  );

  std::string worker = STRINGIFY(

    onmessage = function(e){
      console.log(e.status);
    }
  );

};

void prepareSettings( TmpFileWrite::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void TmpFileWrite::setup()
{
   emscripten::val helpers = em::helpers::getFileHelpers();


   helpers.call<void>("loadProcessor",emscripten::val(worklet));
   //helpers.call<void>("loadWorker",emscripten::val(worker));
}

void TmpFileWrite::draw()

{
  gl::clear( Color( 0,0,0 ));


}

CINDER_APP( TmpFileWrite, RendererGl, prepareSettings )
