#include "cinder/audio/Source.h"
#include "cinder/emscripten/CinderEmscripten.h"
#include "cinder/audio/Buffer.h"
#include "cinder/Log.h"
#include <emscripten/bind.h>
#include <emscripten.h>

namespace cinder { namespace audio { namespace em {
  class SourceFileWebAudio : public SourceFile 
  {
    
    public:

      SourceFileWebAudio();
      
      SourceFileWebAudio( const DataSourceRef &dataSource );
  

      BufferRef getAudioData() { return mAudioData; }
      
      void init( std::function<void()> cb );

	    size_t getNumChannels() const override
      {
        return mNumChannels;
      }

	    size_t getSampleRateNative() const override { return mNativeSampleRate; }

      void performSeek( size_t readPositionFrames ) override {}
      SourceFileRef cloneWithSampleRate( size_t sampleRate ) const {}
      size_t performRead( Buffer *buffer, size_t bufferFrameOffset, size_t numFramesNeeded )
      {
        return 1;
      }

    private:
      ci::DataSourceRef mDataSource;
      BufferRef	mAudioData;

      int mNumChannels;
      int mSampleRate;
      int mNativeSampleRate;
      int mNumFrames;

      emscripten::val mHelpers;
    };

}}}