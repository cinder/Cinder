
#include <emscripten.h>
#include <emscripten/bind.h>
#include <string>
#include <memory>
#include "cinder/emscripten/CinderEmscripten.h"

namespace cinder { namespace em {

  typedef std::shared_ptr<class AudioPlayer>AudioPlayerRef;

  /**
   * This is a very basic audio playback class utilizing the html <audio> tag. 
   * It's purpose is solely playing back audio and has no built-in mechanism to 
   * allow you to extract the audio information. 
   */ 
  class AudioPlayer
  {
    
    //! Represents the DOM <audio> tag
    emscripten::val audioNode;

  public:

    AudioPlayer( std::string src, bool useControls, bool loop);

    static AudioPlayerRef create( std::string src, bool useControls=false, bool loop=false){
      return AudioPlayerRef( new AudioPlayer( src, useControls, loop ) );
    }

    //! Starts playback
    void play();

    //! Pauses playback
    void pause();

    //! Adds an additional source to the player. If the browser doesn't support
    //! a particular file format, the browser will cycle through any additional sources
    //! until a suitable one is found, or simply not play. 
    //! Note that this currently only supports either a string based asset path or a blob source. 
    AudioPlayer& addSource(std::string src);
  };
}}