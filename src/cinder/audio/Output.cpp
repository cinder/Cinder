/*
 Copyright (c) 2009, The Barbarian Group
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

#include "cinder/audio/Output.h"

#include "cinder/CinderMath.h"
#include "cinder/Rand.h"

#include <iostream>

#if defined(CINDER_MAC)
	#include <CoreServices/CoreServices.h>
	#include <CoreAudio/CoreAudioTypes.h>
	#include <AudioUnit/AudioUnit.h>
	#include <AudioToolbox/AUGraph.h>
	
	#include <boost/thread/mutex.hpp>
#elif defined(CINDER_MSW)
	#include "cinder/msw/CinderMSW.h"
	#include <windows.h>
	#include <xaudio2.h>
	#include <boost/thread/thread.hpp>
#endif

namespace cinder { namespace audio {

#if defined(CINDER_MAC)
class OutputAudioUnit;

class TargetOutputAudioUnit : public Target {
  public: 
	static shared_ptr<TargetOutputAudioUnit> createRef( const OutputAudioUnit *aOutput ){ return shared_ptr<TargetOutputAudioUnit>( new TargetOutputAudioUnit( aOutput ) );  };
	~TargetOutputAudioUnit() {}
  private:
	TargetOutputAudioUnit( const OutputAudioUnit *aOutput );
	
	//uint64_t						mPacketOffset;
	//AudioStreamPacketDescription	* mCurrentPacketDescriptions;
};

class OutputAudioUnit : public OutputImpl {
  public:
	OutputAudioUnit();
	~OutputAudioUnit();
	
	TrackRef	addTrack( SourceRef source, bool autoplay );
	void		removeTrack( TrackId trackId );
	
	void setVolume( float aVolume );
	float getVolume() const;
  private:
	AudioDeviceID					mOutputDeviceId;
	AUGraph							mGraph;
	AUNode							mMixerNode;
	AUNode							mOutputNode;
	AudioUnit						mOutputUnit;
	AudioUnit						mMixerUnit;
	AudioStreamBasicDescription		* mPlayerDescription;
	
	class Track : public cinder::audio::Track
	{
	  public:
		Track( SourceRef source, OutputAudioUnit * output );
		~Track();
		void play();
		void stop();
		
		TrackId getTrackId() const { return mInputBus; }
		
		void setVolume( float aVolume );
		float getVolume() const;
		
		double getTime() const { return ( mLoader->getSampleOffset() / (double)mSource->getSampleRate() ); }
		void setTime( double aTime );
		
		bool isLooping() const { return mIsLooping; }
		void setLooping( bool isLooping ) { mIsLooping = isLooping; }
	  private:
		static OSStatus renderCallback( void * audioLoader, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData);
		
		SourceRef	mSource;
		OutputAudioUnit	* mOutput;
		TrackId		mInputBus;
		LoaderRef	mLoader;
		bool		mIsPlaying;
		bool		mIsLooping;
		uint64_t	mFrameOffset;
	};
	
	std::map<TrackId,shared_ptr<OutputAudioUnit::Track> >	mTracks;
	
	friend class TargetOutputAudioUnit;
};

TargetOutputAudioUnit::TargetOutputAudioUnit( const OutputAudioUnit *aOutput ) {
		loadFromCaAudioStreamBasicDescription( this, aOutput->mPlayerDescription );
}

OutputAudioUnit::Track::Track( SourceRef source, OutputAudioUnit * output )
	: cinder::audio::Track(), mSource( source ), mOutput( output ), mIsPlaying( false), mIsLooping( false ), mFrameOffset( 0 )
{
	shared_ptr<TargetOutputAudioUnit> aTarget = TargetOutputAudioUnit::createRef( output );
	mLoader = source->getLoader( aTarget.get() );
	mInputBus = output->availableTrackId();
}

OutputAudioUnit::Track::~Track() 
{
	if( mIsPlaying ) {
		stop();
	}
}

void OutputAudioUnit::Track::play() 
{
	AURenderCallbackStruct rcbs;
	rcbs.inputProc = &OutputAudioUnit::Track::renderCallback;
	rcbs.inputProcRefCon = (void *)this;
	
	OSStatus err = AudioUnitSetProperty( mOutput->mMixerUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, mInputBus, &rcbs, sizeof(rcbs) );
	if( err ) {
		//throw
	}
	mIsPlaying = true;
}

void OutputAudioUnit::Track::stop()
{
	AURenderCallbackStruct rcbs;
	rcbs.inputProc = NULL;
	rcbs.inputProcRefCon = NULL;
	OSStatus err = AudioUnitSetProperty( mOutput->mMixerUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, mInputBus, &rcbs, sizeof(rcbs) );
	if( err ) {
		//don't throw here because this is called from the deconstructor
	}
	mIsPlaying = false;
}

float OutputAudioUnit::Track::getVolume() const
{
	float aValue = 0.0;
	OSStatus err = AudioUnitGetParameter( mOutput->mMixerUnit, kStereoMixerParam_Volume, kAudioUnitScope_Input, mInputBus, &aValue );
	if( err ) {
		//throw
	}
	return aValue;
}

void OutputAudioUnit::Track::setVolume( float aVolume )
{
	aVolume = math<float>::clamp( aVolume, 0.0, 1.0 );
	OSStatus err = AudioUnitSetParameter( mOutput->mMixerUnit, kStereoMixerParam_Volume, kAudioUnitScope_Input, mInputBus, aVolume, 0 );
	if( err ) {
		//throw
	}
}

void OutputAudioUnit::Track::setTime( double aTime )
{
	if( mIsLooping ) {
		//perform modulous on double
		uint32_t result = static_cast<uint32_t>( aTime / mSource->getDuration() );
		aTime = aTime - static_cast<double>( result ) * mSource->getDuration();
	} else {
		aTime = math<double>::clamp( aTime, 0.0, mSource->getDuration() );
	}
	uint64_t aSample = (uint64_t)( mSource->getSampleRate() * aTime );
	mLoader->setSampleOffset( aSample );
}

OSStatus OutputAudioUnit::Track::renderCallback( void * audioTrack, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{	
	OSStatus err = noErr;
	
	OutputAudioUnit::Track *theTrack = reinterpret_cast<OutputAudioUnit::Track *>( audioTrack );
	LoaderRef theLoader = theTrack->mLoader;
	
	
	if( ! theTrack->mIsPlaying ) {		
		for( int i = 0; i < ioData->mNumberBuffers; i++ ) {
			 memset( ioData->mBuffers[i].mData, 0, ioData->mBuffers[i].mDataByteSize );
		}
		
	} else {
		BufferList bufferList;
		bufferList.mNumberBuffers = ioData->mNumberBuffers;
		bufferList.mBuffers = new Buffer[bufferList.mNumberBuffers];
		for( int i = 0; i < bufferList.mNumberBuffers; i++ ) {
			bufferList.mBuffers[i].mNumberChannels = ioData->mBuffers[i].mNumberChannels;
			bufferList.mBuffers[i].mDataByteSize = ioData->mBuffers[i].mDataByteSize;
			bufferList.mBuffers[i].mData = ioData->mBuffers[i].mData;
		}
		
		theLoader->loadData( (uint32_t *)&inNumberFrames, &bufferList );
		delete [] bufferList.mBuffers;
		
		ioData->mNumberBuffers = bufferList.mNumberBuffers;
		for( int i = 0; i < bufferList.mNumberBuffers; i++ ) {
			ioData->mBuffers[i].mNumberChannels = bufferList.mBuffers[i].mNumberChannels;
			ioData->mBuffers[i].mDataByteSize = bufferList.mBuffers[i].mDataByteSize;
			ioData->mBuffers[i].mData = bufferList.mBuffers[i].mData;
		}
		
	}
	//save data into pcm buffer if it's enabled
	/*if( theTrack->mPCMBufferEnabled ) {
		if( theTrack->mPCMBuffer.mSampleIdx + ( ioData->mBuffers[0].mDataByteSize / sizeof(Float32) ) > theTrack->mPCMBuffer.mSamplesPerBuffer ) {
			theTrack->mPCMBuffer.mSampleIdx = 0;
		}
		for( int i = 0; i < theTrack->mPCMBuffer.mBufferCount; i++ ) {
			memcpy( (void *)( theTrack->mPCMBuffer.mBuffers[i] + theTrack->mPCMBuffer.mSampleIdx ), ioData->mBuffers[i].mData, ioData->mBuffers[i].mDataByteSize );
		}
		theTrack->mPCMBuffer.mSampleIdx += ioData->mBuffers[0].mDataByteSize / 4;
		
	}*/
	
	if( ioData->mBuffers[0].mDataByteSize == 0 ) {
		if( ! theTrack->mIsLooping ) {
			theTrack->stop();
			theTrack->mOutput->removeTrack( theTrack->getTrackId() );
			//the track is dead at this point, don't do anything else
			return err;
		}
	}
	if( theTrack->getTime() >= theTrack->mSource->getDuration() && theTrack->mIsLooping ) {
		theTrack->setTime( 0.0 );
	}
	
    return err;
}

OutputAudioUnit::OutputAudioUnit()
	: OutputImpl()
{
	OSStatus err = noErr;

	NewAUGraph( &mGraph );
	
#if MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_5
	AudioComponentDescription cd;
#else
	ComponentDescription cd;
#endif
	cd.componentManufacturer = kAudioUnitManufacturer_Apple;
	cd.componentFlags = 0;
	cd.componentFlagsMask = 0;
	
	//output node
	cd.componentType = kAudioUnitType_Output;
	cd.componentSubType = kAudioUnitSubType_DefaultOutput;
	
	//connect & setup
	AUGraphOpen( mGraph );
	
	//initialize component - todo add error checking
	if( AUGraphAddNode( mGraph, &cd, &mOutputNode ) != noErr ) {
		std::cout << "Error 1!" << std::endl;
	}
	
	if( AUGraphNodeInfo( mGraph, mOutputNode, NULL, &mOutputUnit ) != noErr ) {
		std::cout << "Error 2!" << std::endl;	
	}
	
	//get default output device id and set it as the outdevice for the output unit
	UInt32 dsize = sizeof( AudioDeviceID );
	err = AudioHardwareGetProperty( kAudioHardwarePropertyDefaultOutputDevice, &dsize, &mOutputDeviceId );
	if( err != noErr ) {
		std::cout << "Error getting default output device" << std::endl;
	}
	
	err = AudioUnitSetProperty( mOutputUnit, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &mOutputDeviceId, sizeof( mOutputDeviceId ) );
	if( err != noErr ) {
		std::cout << "Error setting current output device" << std::endl;
	}
	
	//Tell the output unit not to reset timestamps 
	//Otherwise sample rate changes will cause sync los
	UInt32 startAtZero = 0;
	err = AudioUnitSetProperty( mOutputUnit, kAudioOutputUnitProperty_StartTimestampsAtZero, kAudioUnitScope_Global, 0, &startAtZero, sizeof( startAtZero ) );
	if( err != noErr ) {
		std::cout << "Error telling output unit not to reset timestamps" << std::endl;
	}
	
	//stereo mixer node
	cd.componentType = kAudioUnitType_Mixer;
	cd.componentSubType = kAudioUnitSubType_StereoMixer;
	AUGraphAddNode( mGraph, &cd, &mMixerNode );
	
	//setup mixer AU
	err = AUGraphNodeInfo( mGraph, mMixerNode, NULL, &mMixerUnit );
	if( err ) {
		std::cout << "Error 4" << std::endl;
	}
	
	
	ComponentResult err2 = noErr;
	dsize = sizeof( AudioStreamBasicDescription );
	mPlayerDescription = new AudioStreamBasicDescription;
	err2 = AudioUnitGetProperty( mOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, mPlayerDescription, &dsize );
	if( err2 ) {
		std::cout << "Error reading output unit stream format" << std::endl;
	}
	
	//TODO: cleanup error checking in all of this
	err2 = AudioUnitSetProperty( mMixerUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, mPlayerDescription, dsize );
	if( err2 ) {
		std::cout << "Error setting output unit output stream format" << std::endl;
	}
	
	err2 = AudioUnitSetProperty( mMixerUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, mPlayerDescription, dsize );
	if( err2 ) {
		std::cout << "Error setting mixer unit input stream format" << std::endl;
	}
	
	err2 = AudioUnitSetProperty( mOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, mPlayerDescription, dsize );
	if( err2 ) {
		std::cout << "Error setting output unit input stream format" << std::endl;
	}
	
	AUGraphConnectNodeInput( mGraph, mMixerNode, 0, mOutputNode, 0 );
	
	AudioStreamBasicDescription outDesc;
	UInt32 size = sizeof(outDesc);
	AudioUnitGetProperty( mOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &outDesc, &size );
	
	AUGraphInitialize( mGraph );
	
	//race condition work around??
	usleep( 10 * 1000 );
	
	//TODO: tell the output AU about the order of the channels if there are more than 2 

	// turn metering ON
	UInt32 data = 1;
	AudioUnitSetProperty( mMixerUnit, kAudioUnitProperty_MeteringMode, kAudioUnitScope_Global, 0, &data, sizeof(data) );
	
	err = AUGraphStart( mGraph );
	if( err ) {
		//throw
	}
}

OutputAudioUnit::~OutputAudioUnit()
{
	delete mPlayerDescription;
	AUGraphStop( mGraph );
	AUGraphUninitialize( mGraph );
	DisposeAUGraph( mGraph );
}

TrackRef OutputAudioUnit::addTrack( SourceRef aSource, bool autoplay )
{
	shared_ptr<OutputAudioUnit::Track> track = shared_ptr<OutputAudioUnit::Track>( new OutputAudioUnit::Track( aSource, this ) );
	TrackId inputBus = track->getTrackId();
	mTracks.insert( std::pair<TrackId,shared_ptr<OutputAudioUnit::Track> >( inputBus, track ) );
	if( autoplay ) {
		track->play();
	}
	return track;
}

void OutputAudioUnit::removeTrack( TrackId trackId )
{
	if( mTracks.find( trackId ) == mTracks.end() ) {
		//TODO: throw OutputInvalidTrackExc();
		return;
	}
	
	mTracks.erase( trackId );
}

float OutputAudioUnit::getVolume() const
{
	float aValue = 0.0;
	OSStatus err = AudioUnitGetParameter( mMixerUnit, kStereoMixerParam_Volume, kAudioUnitScope_Output, 0, &aValue );
	if( err ) {
		//throw
	}
	return aValue;
}

void OutputAudioUnit::setVolume( float aVolume )
{
	aVolume = math<float>::clamp( aVolume, 0.0, 1.0 );
	OSStatus err = AudioUnitSetParameter( mMixerUnit, kStereoMixerParam_Volume, kAudioUnitScope_Output, 0, aVolume, 0 );
	if( err ) {
		//throw
	}
}

#endif

#if defined(CINDER_MSW)

class OutputXAudio;

class TargetOutputXAudio : public Target {
  public: 
	static shared_ptr<TargetOutputXAudio> createRef( const WAVEFORMATEX *aOutDescription ){ return shared_ptr<TargetOutputXAudio>( new TargetOutputXAudio( aOutDescription ) );  }
	~TargetOutputXAudio() {}
  private:
	TargetOutputXAudio( const WAVEFORMATEX *aOutDescription );
};

class OutputXAudio : public OutputImpl 
{
  public:
	OutputXAudio();
	~OutputXAudio();
	TrackRef	addTrack( SourceRef aSource, bool autoplay );
	void		removeTrack( TrackId );
	
	void setVolume( float aVolume ) {}
	float getVolume() const { /*TODO*/ return 0.0; }
  protected:
	::IXAudio2							* mXAudio;
	::IXAudio2MasteringVoice			* mMasterVoice;
	
	class Track : public cinder::audio::Track
	{
	  public:
		Track( SourceRef source, OutputXAudio * output );
		~Track();
		void play();
		void stop();
		
		TrackId	getTrackId() const { return mTrackId; }
		
		void setVolume( float aVolume );
		float getVolume() const;
		
		double getTime() const { return mLoader->getSampleOffset() / mVoiceDescription.nChannels / (double)mVoiceDescription.nSamplesPerSec; }
		void setTime( double aTime ) { mLoader->setSampleOffset( aTime * mVoiceDescription.nSamplesPerSec * mVoiceDescription.nChannels ); }

		void setLooping( bool isLooping ) { mIsLooping = isLooping; }
		bool isLooping() const { return mIsLooping; }
		
	  private:
		//static ::HRESULT				dataInputCallback( void * audioData, uint32_t dataSize, void * theTrack, uint64_t sampleTime, uint32_t sampleDuration );
		void fillBuffer();

		static const int				sMaxBufferCount = 3;
		
		bool							mIsLooping;
		bool							mIsPlaying;
		TrackId							mTrackId;
		SourceRef						mSource;
		LoaderRef						mLoader;
		OutputXAudio					* mOutput;
		
		IXAudio2SourceVoice				* mSourceVoice;
		WAVEFORMATEX					mVoiceDescription;
		uint8_t							* mDecodedBuffers;

		uint32_t						mBufferSize;
		uint32_t						mSamplesPerBuffer;
		uint32_t						mCurrentBuffer;
		uint64_t						mCurrentTime;

		HANDLE								mBufferEndEvent;
		shared_ptr<boost::thread>			mQueueThread;

		class SourceCallback : public IXAudio2VoiceCallback
		{
		public:
			//HANDLE							mBufferEndEvent;
			OutputXAudio::Track				* mTrack;
				
			SourceCallback()
				/*: mBufferEndEvent( CreateEvent( NULL, FALSE, FALSE, NULL ) )*/{}
			~SourceCallback(){ /*CloseHandle( mBufferEndEvent );*/ }
			void STDMETHODCALLTYPE OnBufferEnd( void* pvContext ) {
				SetEvent( mTrack->mBufferEndEvent );
			}

			//Unused methods are stubs
			void STDMETHODCALLTYPE OnStreamEnd() {}
			void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() { }
			void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 SamplesRequired) {    }
			void STDMETHODCALLTYPE OnBufferStart(void * pBufferContext) {  }
			void STDMETHODCALLTYPE OnLoopEnd(void * pBufferContext) {    }
			void STDMETHODCALLTYPE OnVoiceError(void * pBufferContext, HRESULT Error) { }
		};

		SourceCallback					mVoiceCallback;
	};

	std::map<TrackId,shared_ptr<OutputXAudio::Track> >	mTracks;
	
};

//TODO: don't make these values fixed
TargetOutputXAudio::TargetOutputXAudio( const WAVEFORMATEX *aOutDescription )
	: Target()
{
	mSampleRate = aOutDescription->nSamplesPerSec;
	mChannelCount = aOutDescription->nChannels;
	mBitsPerSample = aOutDescription->wBitsPerSample;
	mBlockAlign = aOutDescription->nBlockAlign;
	mIsInterleaved = TRUE;
	mIsPcm = TRUE;
	mIsBigEndian = TRUE;
	mDataType = DATA_UNKNOWN;
	if( aOutDescription->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ) {
		mDataType = FLOAT32;
	} else if( aOutDescription->wBitsPerSample == 32 ) {
		mDataType = INT32;
	} else if( aOutDescription->wBitsPerSample == 16 ) {
		mDataType = INT16;
	} else if( aOutDescription->wBitsPerSample == 8 ) {
		mDataType = UINT8;
	}
}

OutputXAudio::Track::Track( SourceRef source, OutputXAudio * output )
	: cinder::audio::Track(), mSource( source ), mOutput( output )
{
	::HRESULT hr;

	mBufferEndEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	mTrackId = mOutput->availableTrackId();
	mVoiceCallback.mTrack = this;
	
	if( source->getDataType() == Io::DATA_UNKNOWN ) {
		// for compressed data setup Target/SourceVoice for ideal setup for XAudio output
		XAUDIO2_VOICE_DETAILS masterVoiceDetails;
		mOutput->mMasterVoice->GetVoiceDetails( &masterVoiceDetails );

		mVoiceDescription.wFormatTag = WAVE_FORMAT_PCM;
		mVoiceDescription.nSamplesPerSec = masterVoiceDetails.InputSampleRate;
		mVoiceDescription.nChannels = masterVoiceDetails.InputChannels;
		mVoiceDescription.wBitsPerSample = 16;
		mVoiceDescription.nBlockAlign = math<float>::ceil( ( mVoiceDescription.nChannels * mVoiceDescription.wBitsPerSample ) / 8.0 );
		mVoiceDescription.nAvgBytesPerSec = ( mVoiceDescription.nBlockAlign * mVoiceDescription.nSamplesPerSec );
		mVoiceDescription.cbSize = 0;
	} else {
		mVoiceDescription.wFormatTag = ( source->isFloat() )? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM;
		mVoiceDescription.nSamplesPerSec = source->getSampleRate();
		mVoiceDescription.nChannels = source->getChannelCount();
		mVoiceDescription.wBitsPerSample = source->getBitsPerSample();
		mVoiceDescription.nBlockAlign = source->getBlockAlign();//( sourceDescription.nChannels * sourceDescription.wBitsPerSample ) / 8;
		mVoiceDescription.nAvgBytesPerSec = ( mVoiceDescription.nBlockAlign * mVoiceDescription.nSamplesPerSec );
		mVoiceDescription.cbSize = 0;
	}

	if( mVoiceDescription.nChannels > XAUDIO2_MAX_AUDIO_CHANNELS ) {
		//throw
	}

	if( mVoiceDescription.nSamplesPerSec < XAUDIO2_MIN_SAMPLE_RATE || mVoiceDescription.nSamplesPerSec > XAUDIO2_MAX_SAMPLE_RATE ) {
		//throw
	}
	
	shared_ptr<TargetOutputXAudio> target = TargetOutputXAudio::createRef( &mVoiceDescription );
	mLoader = mSource->getLoader( target.get() );

	//mBufferSize = mLoader->mBufferSize;
	mBufferSize = mLoader->getOptimalBufferSize();
	if( mBufferSize <= 0 ) {
		mBufferSize = 2048 * mVoiceDescription.nBlockAlign;
	}
	mSamplesPerBuffer = mBufferSize / mVoiceDescription.nBlockAlign;

	//create buffers
	mDecodedBuffers = new uint8_t[ OutputXAudio::Track::sMaxBufferCount * mBufferSize ];
	mCurrentBuffer = 0;
	mCurrentTime = 0;

	hr = mOutput->mXAudio->CreateSourceVoice( &mSourceVoice, &mVoiceDescription, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &(mVoiceCallback), NULL, NULL );
	if( FAILED( hr ) )
	{
		//throw
		return;
	}
}

OutputXAudio::Track::~Track()
{
	stop();
	delete [] mDecodedBuffers;
	CloseHandle( mBufferEndEvent );
}

void OutputXAudio::Track::play()
{
	//mLoader->start();
	//fillBufferCallback();
	mIsPlaying = true;
	mQueueThread = shared_ptr<boost::thread>( new boost::thread( boost::bind( &OutputXAudio::Track::fillBuffer, this ) ) );

	::HRESULT hr = mSourceVoice->Start( 0, XAUDIO2_COMMIT_NOW );
	if( FAILED( hr ) ) {
		//throw
		return;
	}
}

void OutputXAudio::Track::stop()
{
	if( ! mIsPlaying ) return;
	mSourceVoice->Stop( 0, XAUDIO2_COMMIT_NOW ); //might not really need this
	mIsPlaying = false;
	SetEvent( mBufferEndEvent ); //signals event to end queuethread
	mQueueThread->join();
	mSourceVoice->FlushSourceBuffers();
}

void OutputXAudio::Track::setVolume( float aVolume )
{
	aVolume = math<float>::clamp( aVolume, 0.0, 1.0 );
	::HRESULT hr = mSourceVoice->SetVolume( aVolume );
	if( FAILED( hr ) ) {
		//throw
	}
}

float OutputXAudio::Track::getVolume() const
{
	float aValue = 0.0;
	mSourceVoice->GetVolume( &aValue );
	return aValue;
}

//HRESULT OutputXAudio::Track::dataInputCallback( void * audioData, uint32_t dataSize, void * track, uint64_t sampleTime, uint32_t sampleDuration )
void OutputXAudio::Track::fillBuffer()
{
	//OutputXAudio::Track *theTrack = reinterpret_cast<OutputXAudio::Track*>( track );
	XAUDIO2_VOICE_STATE state;
	while( 1 ) {	
		mSourceVoice->GetState( &state );
		if( state.BuffersQueued >= OutputXAudio::Track::sMaxBufferCount ) {
			::WaitForSingleObject( mBufferEndEvent, INFINITE );
		}

		if( ! mIsPlaying ) {
			//tell the loader to go fuck itself?
			break;
		}
		
		BufferList bufferList;
		bufferList.mNumberBuffers = 1;
		Buffer buffer;
		buffer.mData = &( mDecodedBuffers[mCurrentBuffer * mBufferSize] );
		buffer.mDataByteSize = mBufferSize;
		buffer.mNumberChannels = mVoiceDescription.nChannels;
		bufferList.mBuffers = &buffer;
		
		uint32_t sampleCount = mSamplesPerBuffer;
		mLoader->loadData( &sampleCount, &bufferList );
		
		if( sampleCount <= 0 ) {
			stop();
			return;
		}
		
		XAUDIO2_BUFFER xbuffer = {0};
		xbuffer.AudioBytes = buffer.mDataByteSize;
		xbuffer.Flags = 0;
		xbuffer.pAudioData = (BYTE *)buffer.mData;
		::HRESULT hr = mSourceVoice->SubmitSourceBuffer( &xbuffer );
		if( hr != S_OK ) {

		}
		
		mCurrentBuffer++;
		mCurrentBuffer %= OutputXAudio::Track::sMaxBufferCount;
	}
}

/*class TargetOutputXAudio : public Target
{
  public:
  protected:
	TargetOutputXAudio
}*/

OutputXAudio::OutputXAudio()
	: OutputImpl(), mXAudio( NULL ), mMasterVoice( NULL )
{
	::HRESULT hr;

	msw::initializeCom();
	
	if( FAILED( hr = XAudio2Create( &mXAudio, 0, XAUDIO2_DEFAULT_PROCESSOR ) ) ) {
		//throw
		return;
	}
	
	if( FAILED( hr = mXAudio->CreateMasteringVoice( &mMasterVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL ) ) ) {
		//throw
		return;
	}
}

OutputXAudio::~OutputXAudio()
{
	//TODO: cleanup
}

TrackRef OutputXAudio::addTrack( SourceRef aSource, bool autoPlay ) 
{
	//TargetOutputXAudioRef target = TargetOutputXAudio::createRef( this );
	//TODO: pass target instead of this?
	shared_ptr<OutputXAudio::Track> track = shared_ptr<OutputXAudio::Track>( new OutputXAudio::Track( aSource, this ) );
	TrackId inputBus = track->getTrackId();
	mTracks.insert( std::pair<TrackId,shared_ptr<OutputXAudio::Track> >( inputBus, track ) );
	if( autoPlay ) {
		track->play();
	}
	return track;
}

void OutputXAudio::removeTrack( TrackId trackId )
{
	if( mTracks.find( trackId ) == mTracks.end() ) {
		//TODO: throw OutputInvalidTrackExc();
		return;
	}
	
	mTracks.erase( trackId );
}

#endif

OutputImpl::OutputImpl()
	: mNextTrackId( 0 )
{}

OutputImpl* Output::instance()
{
#if defined(CINDER_MAC)
	static shared_ptr<OutputAudioUnit> sInst;
	if( ! sInst ) {
		sInst = shared_ptr<OutputAudioUnit>( new OutputAudioUnit() );
	}
	return sInst.get();
#elif defined(CINDER_MSW)
	static shared_ptr<OutputXAudio> sInst;
	if( ! sInst ) {
		sInst = shared_ptr<OutputXAudio>( new OutputXAudio() );
	}
	return sInst.get();
#else
	return NULL;
#endif
}

}} //namespace

