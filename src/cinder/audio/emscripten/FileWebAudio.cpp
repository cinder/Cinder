#include "cinder/audio/emscripten/FileWebAudio.h"
using namespace ci;
using namespace emscripten;

namespace cinder { namespace audio { namespace em {
    SourceFileWebAudio::SourceFileWebAudio():SourceFile( 0 ),
    mHelpers( emscripten::val::undefined() ){}
    
    SourceFileWebAudio::SourceFileWebAudio( const DataSourceRef &dataSource ):
    mDataSource( dataSource ),SourceFile( 0 ),
    mHelpers( emscripten::val::undefined() ){}

    void SourceFileWebAudio::init( std::function<void()> cb ) {
        // grab reference to Javascript helpers
        mHelpers = emscripten::val::global( "window" )[ "CINDER_AUDIO" ];
    
        // binding to member function is weird for some reason - use lambda for time being.
        //std::function<void(val)> _func = std::bind(&SourceFileWebAudio::parseFile,this,std::placeholders::_1);
        std::function<void( val )> _func = [=]( val e )->void
        {
            mNumChannels = e[ "numberOfChannels" ].as<int>();
            mSampleRate = e[ "sampleRate" ].as<int>();
            mNativeSampleRate = e[ "duration" ].as<int>();
            mNumFrames = e[ "length" ].as<float>();
        
            // get channel data into buffer
            std::vector<float> dat;
            mAudioData = BufferRef( new Buffer( mNumFrames,mNumChannels ) );
        
            // go through channels and figure out
            // TODO how to handle multi-channel audio?
            for( int i = 0; i < mNumChannels; ++i )
            {
                auto data = e.call<val>( "getChannelData",val( i ) );
                // copy TypedArray data to vector
                ci::em::helpers::copyToVector( data,dat );
                // clear vector if we need it for a second channel
                //dat.clear();
            }
        
            // clone into mAudioData
            float * _dat = reinterpret_cast<float*>(mAudioData->getData());
        
            CI_ASSERT(mAudioData->getSize() == dat.size());
        
            for( int i = 0; i < dat.size();++i )
            {
                _dat[i] = dat[i];
            }
       
            // signal that we're done parsing.
            cb();
        };
     
        auto ctx = ( ContextWebAudio* )audio::master();
        if( mDataSource->isFilePath() )
        {
            auto src = val( mDataSource->getFilePath().c_str() );
       mHelpers.call<void>( "loadAudioFile",src,ctx->getRawContext(),ci::em::helpers::generateCallback( _func ) );
     }
     else {
       auto src = val( mDataSource->getUrl().c_str() );
       mHelpers.call<void>( "loadAudioFile",src,ctx->getRawContext(),ci::em::helpers::generateCallback( _func ) );
     }
    }
}}}