#include "cinder/emscripten/AudioPlayer.h"

using namespace ci;
using namespace emscripten;

namespace cinder { namespace em {

    AudioPlayer::AudioPlayer( fs::path src, bool useControls, bool loop ):audioNode( val::undefined() )
    {

        audioNode = val::global( "document" ).call<val>( "createElement", val( "audio" ) );

        // take the filename as the id of the element.
        auto id = src.stem();
        audioNode.set( "id", val( id ) );

        // set cross-origin properties
        audioNode.set( "crossorigin", val( "anonymous" ) );

        // set whether or not it will loop
        audioNode.set( "loop",val( loop ) );

        if( useControls )
        {
            audioNode.set( "controls", useControls );
        }

        if( src != "" )
        {
            audioNode.set( "src",src.string() );
        }

        val::global( "document" )[ "body" ].call<void>( "appendChild", audioNode );
    }

    void AudioPlayer::play()
    {
        audioNode.call<void>( "play" );
    }

    void AudioPlayer::pause()
    {
        audioNode.call<void>( "pause" );
    }

    AudioPlayer& AudioPlayer::addSource( fs::path src )
    {
        val source = val::global( "document" ).call<val>( "createElement", val( "source" ) );
        source.set( "src",val( src.string() ) );
        audioNode.call<void>( "append", source );
        return *this;
    }
}}