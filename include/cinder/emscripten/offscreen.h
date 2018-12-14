#pragma once
#include <emscripten.h>
#include <emscripten/bind.h>

#if defined(CINDER_EMSCRIPTEN)

namespace cinder { namespace em {

  /**
   * This is for interacting with the offscreen canvas api. 
   * Currently not tested and not fully implemented.
   */
  class OffscreenCanvas {


    public:
      OffscreenCanvas(){}

        //! Sets things up. Note that this must be called before calling any
        //! other function.
        void setup()
        {
          // initialize attributes
          emscripten_webgl_init_context_attributes( &glAttribs );
          glAttribs.explicitSwapControl = EM_TRUE;

          // create the new context
          altCtx = emscripten_webgl_create_context( 0,&glAttribs );
          emscripten_webgl_make_context_current( altCtx );

        }

        void commitFrame()
        {
          EMSCRIPTEN_RESULT r = emscripten_webgl_commit_frame();
        
          if( r != EMSCRIPTEN_RESULT_SUCCESS )
          {
            CI_LOG_E( "There was an issue commiting the frame to the offscreen canvas" );
          }
        }

        //! Makes the OffscreenCanvas's context the current context we draw to
        void makeCurrent()
        {
          EMSCRIPTEN_RESULT result = emscripten_webgl_make_context_current(altCtx);
          if( result != EMSCRIPTEN_RESULT_SUCCESS )
          {
            CI_LOG_E( "Unable to make OffscreenCanvas instance the current context" );
          }
        }

        //! Holds attributes about how to build the context
        //! See
        //! https://kripken.github.io/emscripten-site/docs/api_reference/html5.h.html#webgl-context
        //! for options and what can be used.
        //! Note that explicitSwapControl will always be set to true for this
        EmscriptenWebGLContextAttributes glAttribs;

    private:
      //! Holds a reference to the alternate context that gets created
      EMSCRIPTEN_WEBGL_CONTEXT_HANDLE altCtx;
    };
  }}
#endif
