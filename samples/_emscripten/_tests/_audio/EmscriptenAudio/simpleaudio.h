
#include <emscripten.h>
#include <emscripten/bind.h>
#include "cinder/emscripten/CinderEmscripten.h"
using namespace emscripten;

// a simpler audio player for when all you need to do is play back sound.
namespace cinder { namespace audio { namespace em {

  class SimpleAudioPlayer
  {
    //! Represents the DOM <audio> tag
    val audioNode;
  public:

    SimpleAudioPlayer(
      fs::path src,
      bool useControls=false,
      bool loop=false
    ):audioNode( val::undefined() )
    {
      audioNode = val::global( "document" ).createElement( "audio" );

      // take the filename as the id of the element.
      auto id = p.stem();
      audioNode.set( "id",val( id ) );

      // set cross-origin properties
      audioNode.set( "crossorigin",val( "anonymous" ) );

      // set whether or not it will loop
      audioNode.set( "loop",val( loop ) );

      if( src != "" )
      {
        audioNode.set( "src",src.string() );
      }

      val::global( "document" )[  "body" ].call<void>( "appendChild",audioNode );
    }

    void play()
    {
      audioNode.call<void>( "play" );
    }

    void pause()
    {
      audioNode.call<void>( "pause" );
    }
  }
}}}