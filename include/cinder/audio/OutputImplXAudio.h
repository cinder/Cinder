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

#pragma once

#include "cinder/audio/Output.h"
#include "cinder/msw/CinderMSW.h"
#include "cinder/CinderMath.h"
#include "cinder/audio/FftProcessor.h"

#include <windows.h>
#include <xaudio2.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

namespace cinder { namespace audio {

class OutputImplXAudio;

class TargetOutputImplXAudio : public Target {
  public: 
	static shared_ptr<TargetOutputImplXAudio> createRef( const WAVEFORMATEX *aOutDescription ){ return shared_ptr<TargetOutputImplXAudio>( new TargetOutputImplXAudio( aOutDescription ) );  }
	~TargetOutputImplXAudio() {}
  private:
	TargetOutputImplXAudio( const WAVEFORMATEX *aOutDescription );
};

class OutputImplXAudio : public OutputImpl 
{
  public:
	OutputImplXAudio();
	~OutputImplXAudio();
	TrackRef	addTrack( SourceRef aSource, bool autoplay );
	void		removeTrack( TrackId );
	
	void setVolume( float aVolume ) {}
	float getVolume() const { /*TODO*/ return 0.0; }

	//TargetRef getTarget();
  protected:
	::IXAudio2							* mXAudio;
	::IXAudio2MasteringVoice			* mMasterVoice;
	
	class Track : public cinder::audio::Track
	{
	  public:
		Track( SourceRef source, OutputImplXAudio * output );
		~Track();
		void play();
		void stop();
		bool isPlaying() const { return mIsPlaying; }
		
		TrackId	getTrackId() const { return mTrackId; }
		
		void setVolume( float aVolume );
		float getVolume() const;
		
		double getTime() const { return mLoader->getSampleOffset() / mVoiceDescription.nChannels / (double)mVoiceDescription.nSamplesPerSec; }
		void setTime( double aTime ) { mLoader->setSampleOffset( aTime * mVoiceDescription.nSamplesPerSec * mVoiceDescription.nChannels ); }

		void setLooping( bool isLooping ) { mIsLooping = isLooping; }
		bool isLooping() const { return mIsLooping; }
		
		void enablePcmBuffering( bool isBuffering ) { mIsPcmBuffering = isBuffering; }
		bool isPcmBuffering() { return mIsPcmBuffering; }

		PcmBuffer32fRef getPcmBuffer();
	  private:
		//static ::HRESULT				dataInputCallback( void * audioData, uint32_t dataSize, void * theTrack, uint64_t sampleTime, uint32_t sampleDuration );
		void fillBuffer();

		static const int				sMaxBufferCount = 3;
		
		bool							mIsLooping;
		bool							mIsPlaying;
		TrackId							mTrackId;
		SourceRef						mSource;
		LoaderRef						mLoader;
		OutputImplXAudio				* mOutput;
		
		IXAudio2SourceVoice				* mSourceVoice;
		WAVEFORMATEX					mVoiceDescription;
		uint8_t							* mDecodedBuffers;

		uint32_t						mBufferSize;
		uint32_t						mSamplesPerBuffer;
		uint32_t						mCurrentBuffer;
		uint64_t						mCurrentTime;

		HANDLE								mBufferEndEvent;
		shared_ptr<boost::thread>			mQueueThread;

		bool mIsPcmBuffering;
		PcmBuffer32fRef	mLoadingPcmBuffer;
		PcmBuffer32fRef	mLoadedPcmBuffer;
		boost::mutex	mPcmBufferMutex;

		class SourceCallback : public IXAudio2VoiceCallback
		{
		public:
			//HANDLE							mBufferEndEvent;
			OutputImplXAudio::Track				* mTrack;
				
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

	std::map<TrackId,shared_ptr<OutputImplXAudio::Track> >	mTracks;	
};

}} //namespaces