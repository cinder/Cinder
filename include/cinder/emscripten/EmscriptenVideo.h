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
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "cinder/Log.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"

namespace cinder { namespace em {

    typedef std::shared_ptr< class EmscriptenVideo >EmscriptenVideoRef;

  /**
   * Basic handling of <video> elements within Emscripten. Note that this needs companion Javascript functions 
   * declared in helpers.js
   */
    class EmscriptenVideo 
    {

      public:
        EmscriptenVideo();
        EmscriptenVideo( std::string source ) ;
        EmscriptenVideo( ci::DataSourceRef source );

        static EmscriptenVideoRef create()
        {
          return EmscriptenVideoRef( new EmscriptenVideo );
        }

        static EmscriptenVideoRef create( std::string source )
        {
            return EmscriptenVideoRef( new EmscriptenVideo( source ) );
        }

        static EmscriptenVideoRef create( ci::DataSourceRef source )
        {
          return EmscriptenVideoRef( new EmscriptenVideo( source ) );
        }

        void setTextureFormat( ci::gl::Texture::Format fmt );

        //! Returns the texture for the video
        ci::gl::TextureRef getTexture();

        //! Returns whether or not there is enough data to qualifiy that the video has loaded.
        //! Note that this DOES NOT indicate that the video has completely downloaded.
        bool hasVideoLoaded();

        //! Returns the width of the video
        int getWidth();

        //! Returns the height of the video.
        int getHeight();

        //! Plays the video
        void play();

        //! Pauses the video
        void pause();

        //! Sets the source for the video element. When the video is done loading and we
        //! have the metadata for the video, the function onLoadMetadata gets called.
        void setSource( std::string url );

        //! Returns whether or not the video is playing.
        bool isPlaying();

        //! Updates texture for the video.
        void updateTexture();
        
      private:
        bool mCustomFormatSet;
        
        // video texture settings 
        ci::gl::Texture::Format mVideoTextureFormat;

        //! The element to hold the the html <video> element
        emscripten::val mElement = emscripten::val::undefined();

        //! Texture used to display the video
        ci::gl::TextureRef mTexture;

        //! Whether or not the video has loaded
        bool mIsLoaded;

        //! Internal callback - handles texture creation and setup when enough of the
        //! video has downloaded.
        void onLoadMetadata( emscripten::val e );
    };

 }}
