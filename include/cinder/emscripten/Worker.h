#pragma once
#include <emscripten.h>
#include <string>
#include <functional>
#include <iostream>

namespace cinder { namespace em {

    /**
     * A basic implementation for interfacing with WebWorkers. This is assuming you've already made a 
     * separate WebWorker project and have it compiled within your app. 
     * Note that this may not work with WebWorkers that were compiled normally and written in native Javascript. 
     */
    class Worker 
    {

      //! handle to the web worker.
      worker_handle worker;

      //! handle to callback function for the worker.
      em_worker_callback_func callback;

    public:
      Worker(){}

      //! Loads the worker. Pass in the path to the compiled worker script.
      void loadWorker( std::string path )
      {
        worker = emscripten_create_worker( path.c_str() );
      }

      //! Sets the callback function to use when a message is received from the worker.
      void setOnMessageCallback( em_worker_callback_func callback )
      {
        this->callback = callback;
      }

      //! Posts a message to the web worker.
      //! It assumes the function you want to send information to is called "onmessage"
      //! Accepts some user arguments.
      //! Takes in an char * of data used to transfer data back and forth.
      //! Needs the size of that data ( sizeof(<object>) (and if necessary * numItems)).
      void postMessage( const char * funcname="onmessage", char * data=0, int size=0, void* args = ( void* )0 )
      {
        emscripten_call_worker( worker,funcname,data,size,callback,args );
      }

      //! Posts a message to the web worker.
      //! It assumes the function you want to send information to is called "onmessage"
      //! Accepts some user arguments.
      //! Takes in an char * of data used to transfer data back and forth.
      //! Needs the size of that data ( sizeof(<object>) (and if necessary * numItems)).
      //! This version also allows you to specify a different callback to run as opposed to the main one in the class.
      void postMessageWithCallback( const char * funcname="onmessage", char * data=0, int size=0,em_worker_callback_func _callback=nullptr,void* args = (void*)0)
      {
        emscripten_call_worker( worker,funcname,data,size,_callback,args );
      }
    };
  }
}