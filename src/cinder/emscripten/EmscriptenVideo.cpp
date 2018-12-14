#include "cinder/emscripten/EmscriptenVideo.h"
#include "cinder/emscripten/CinderEmscripten.h"
//#include "cinder/emscripten/globalbindings.h"


using namespace ci;
using namespace emscripten;

namespace cinder { namespace em {

    EmscriptenVideo::EmscriptenVideo() 
    {
      mElement = val::global( "window" )[ "CINDER_VIDEO" ].call<val>( "createVideo" );
      mIsLoaded = false;
      mCustomFormatSet = false;
    }
    
    EmscriptenVideo::EmscriptenVideo( std::string source ) 
    {
      mElement = val::global( "window" )[ "CINDER_VIDEO" ].call<val>( "createVideo" );
      mIsLoaded = false;
      mCustomFormatSet = false;

      setSource(source);
    }

    EmscriptenVideo::EmscriptenVideo( ci::DataSourceRef source )
    {
      mElement = val::global( "window" )[ "CINDER_VIDEO" ].call<val>( "createVideo" );
      mIsLoaded = false;   
      mCustomFormatSet = false;
      setSource(source->getFilePath().string());
    }

    void EmscriptenVideo::setSource( std::string url )  
    {
      mElement.set( "src", val( url ) );
      std::function<void( emscripten::val )> functor = std::bind( &EmscriptenVideo::onLoadMetadata, this, std::placeholders::_1 );
      helpers::addEventListener( mElement, "loadeddata", functor );
    }

    ci::gl::TextureRef EmscriptenVideo::getTexture() 
    {
      return mTexture;
    }

    bool EmscriptenVideo::hasVideoLoaded() 
    {
      return mIsLoaded;
    }

    int EmscriptenVideo::getHeight() 
    {
      return mTexture->getHeight();
    }

    int EmscriptenVideo::getWidth() 
    {
      return mTexture->getWidth();
    }

    void EmscriptenVideo::updateTexture()
    {
      mElement.call<void>( "updateTexture" );
    }

    bool EmscriptenVideo::isPlaying()
    {
      auto v =  mElement[ "paused" ].as<bool>();

      if(v){
        return false;
      }else{
        return true;
      }
    }

    void EmscriptenVideo::setTextureFormat(gl::Texture::Format fmt)
    {
      mVideoTextureFormat = fmt;
    }

    void EmscriptenVideo::pause()
    {
      mElement.call<void>( "pause" );
    }

    void EmscriptenVideo::onLoadMetadata( emscripten::val e ) 
    {
      this->mIsLoaded = true;

      auto element = e[ "target" ];

      // TODO Wierdness - need to assign to a temp value otherwise you loose height for some reason
      auto width = element.call<int>( "getWidth" );
      auto height  = element.call<int>( "getHeight" );

      // TODO should this be user changeable? Texture creation fails with
      // most of the default settings.
      if( !mCustomFormatSet )
      {
        mVideoTextureFormat.wrapS( GL_CLAMP_TO_EDGE );
        mVideoTextureFormat.wrapT( GL_CLAMP_TO_EDGE );
        mVideoTextureFormat.internalFormat( GL_RGBA );
        mVideoTextureFormat.minFilter( GL_NEAREST );
        mVideoTextureFormat.magFilter( GL_NEAREST );
        mVideoTextureFormat.loadTopDown( true );
      }

      mTexture = gl::Texture::create( width, height, mVideoTextureFormat );

      element.call<void>( "setTexture", mTexture->getId() );
      element.call<void>( "updateTexture" );

      // need to re-assign because apparently it gets deleted? According to a cryptic error message
      mElement = element;

    }

    void EmscriptenVideo::play() 
    {
      mElement.call<void>( "play" );
    }
  }
}
