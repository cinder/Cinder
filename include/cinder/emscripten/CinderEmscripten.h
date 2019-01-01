/*
 Copyright (c) 2018, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/


#pragma once

#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <string>
#include <iostream>
#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/Filesystem.h"
#include "cinder/Utilities.h"
#include "cinder/DataSource.h"
#include "cinder/Surface.h"
#include "cinder/Url.h"
#include "cinder/Timer.h"

/**
 * A file of different functions that are likely going to be used or are helpful 
 * in some manner.
 */

namespace cinder { namespace em { namespace helpers {

		//! A helper function to add an event listener to an element in a more simplified form.
		template<typename A>
		static void addEventListener( emscripten::val element,std::string eventName,std::function<void( A )> functor ) {
		  auto window = emscripten::val::global( "window" );

			// this basically transforms your callback function so you don't have to call <function>.onload
			auto functor_adapter = emscripten::val( functor )[ "onload" ].call<emscripten::val>( "bind", emscripten::val(functor) );

			// add the listener to the element
			element.call<void>( "addEventListener", emscripten::val( eventName ), functor_adapter );
		}

		//! Retrieve helper JS functions
		static emscripten::val getCinderHelpers()
    {
			return emscripten::val::global( "window" )[ "CINDER_HELPERS" ];
		}

		//! retrieve audio JS helpers
		static emscripten::val getAudioHelpers()
    {
			return emscripten::val::global( "window" )[ "CINDER_AUDIO" ];
		}

		//! retrieve video JS helpers
		static emscripten::val getVideoHelpers()
    {
			return emscripten::val::global( "window" )[ "CINDER_VIDEO" ];
		}

    // TODO get rid of this since functions mostly un-needed - keeping to ensure we don't break anything. 
    static emscripten::val getFileHelpers()
    {
      return emscripten::val::global( "window" )[ "CINDER_FILEIO" ];
    }

    /**
      Copies a JS TypedArray to a C++ vector
      https://github.com/kripken/emscripten/issues/5519
    */
		template<typename T>
		static void copyToVector( const emscripten::val &typedArray,std::vector<T> &vec ) {
			unsigned int length = typedArray["length"].as<unsigned int>();
			emscripten::val memory = emscripten::val::module_property( "buffer" );
			vec.reserve( length );
			vec.resize( length );
			emscripten::val memoryView = typedArray["constructor"].new_( memory, reinterpret_cast<uintptr_t>(vec.data() ), length);
		    memoryView.call<void>( "set", typedArray );
    }

    /**
     * Same as copyToVector but works with regular arrays and floating point data. 
     */
    template<typename T>
    static void copyFloatArrayToVector(const emscripten::val &typedArray,std::vector<T> &vec)
    {
      unsigned int length = typedArray["length"].as<unsigned int>();
			emscripten::val memory = emscripten::val::module_property( "buffer" );
			vec.reserve( length );
			vec.resize( length );
			emscripten::val memoryView = emscripten::val::global( "Float32Array" ).new_( memory, reinterpret_cast<uintptr_t>(vec.data() ), length);
		  memoryView.call<void>( "set", typedArray );
    }

		//! Converts an std::function into a suitable value to use as a callback in the Javascript context.
		//! Note that this only works with std::functions that have been declared in cinder/emscripten/globalbindings.cpp (or elsewhere)
		//! that utilize the string "onload" as their function name.
		static emscripten::val generateCallback( std::function<void( emscripten::val )> functor )
    {
			return emscripten::val( functor )[ "onload" ].call<emscripten::val>( "bind", emscripten::val(functor) );
		}

    //! Returns reference to Worklet helpers
    static emscripten::val getWorkletHelpers()
    {
      return emscripten::val::global( "window" )[ "CINDER_WORKLETS" ];
    }

    //! Returns the current width of the browser window. 
    static int getWindowWidth()
    {
      return emscripten::val::global( "window" ) ["innerWidth" ];
    }

    //! Returns the current height of the browser window.
    static int getWindowHeight()
    {
      return emscripten::val::global( "window" )[ "innerHeight" ];
    }

    //! Returns an image. Assumes you don't need specialized processing.
    //! TODO return a Surface or something.
    static char * getImage( DataSourceRef url )
    {
      // determine the file type.
      fs::path filename =  fs::path( url->getUrl().str() );

      // fetch the file - put it in  the assets directory after it loads.
      emscripten_wget( url->getUrl().c_str(),filename.c_str() );
      
      int width, height;
      
      char *data = emscripten_get_preloaded_image_data( filename.c_str(), &width, &height );
      
      return data;
    }
}}}

extern "C" {
  extern void loadAsync(const char * url, void * data, int * size);
}

namespace cinder { namespace app {
  
    //! Loads an asset asyncronously. 
    //! Pass in a string url as well as a lambda that accepts a DataSourceRef as a parameter. 
    static void loadAssetAsync( std::string url,std::function<void( ci::DataSourceRef )> cb )
    {
      auto helpers = emscripten::val::global("window")["CINDER_ASYNC"];

      // callback that will construct the DataSourceRef 
      std::function<void( emscripten::val )> sourceConstructor = [=]( emscripten::val byteArray )-> void 
      {
        // turn emscripten::val into something more c++ friendly
        std::vector<uint8_t> data;
        em::helpers::copyToVector(byteArray,data);

        // construct data source 
        ci::BufferRef rawBuff = ci::Buffer::create( static_cast<void*>( data.data() ),data.size() );
	      ci::DataSourceBufferRef dbuff = ci::DataSourceBuffer::create( rawBuff );
        cb( dbuff );
      };

      // generate javascript friendly callback
      emscripten::val _cb = em::helpers::generateCallback( sourceConstructor );

      // prefix with "assets" 
      auto finalPath = "assets/" + url;

      helpers.call<void>( "loadAssetAsync",emscripten::val( finalPath ),_cb );  
    }
  }
}

