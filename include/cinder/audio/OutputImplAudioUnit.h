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
#include <CoreAudio/CoreAudioTypes.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AUGraph.h>
#include <boost/thread/mutex.hpp>
#include <stack>

namespace cinder { namespace audio {

class OutputImplAudioUnit;

class TargetOutputImplAudioUnit : public Target {
  public: 
	static shared_ptr<TargetOutputImplAudioUnit> createRef( const OutputImplAudioUnit *aOutput ){ return shared_ptr<TargetOutputImplAudioUnit>( new TargetOutputImplAudioUnit( aOutput ) );  };
	~TargetOutputImplAudioUnit() {}
  private:
	TargetOutputImplAudioUnit( const OutputImplAudioUnit *aOutput );
	
	//uint64_t						mPacketOffset;
	//AudioStreamPacketDescription	* mCurrentPacketDescriptions;
};

class OutputImplAudioUnit : public OutputImpl {
  public:
	OutputImplAudioUnit();
	~OutputImplAudioUnit();
	
	TrackRef	addTrack( SourceRef source, bool autoplay );
	void		removeTrack( TrackId trackId );
	
	void setVolume( float aVolume );
	float getVolume() const;
	
	//TargetRef getTarget();
  protected:
	TrackId							availableTrackId() { 
										if( mAvailableBuses.size() < 1 ) return -1;
										TrackId bus = mAvailableBuses.top(); 
										mAvailableBuses.pop(); return bus; 
									}
  private:
  
	static const uint32_t			sDefaultNumberBuses;
	std::stack<TrackId>				mAvailableBuses;
	uint32_t						mNumberBuses;
#if defined(CINDER_MAC)
	AudioDeviceID					mOutputDeviceId;
#endif
	AUGraph							mGraph;
	AUNode							mMixerNode;
	AUNode							mOutputNode;
	AudioUnit						mOutputUnit;
	AudioUnit						mMixerUnit;
	AudioStreamBasicDescription		* mPlayerDescription;
	
	class Track : public cinder::audio::Track
	{
	  public:
		Track( SourceRef source, OutputImplAudioUnit * output );
		~Track();
		void play();
		void stop();
		bool isPlaying() const { return mIsPlaying; } 
		
		TrackId getTrackId() const { return mInputBus; }
		
		void setVolume( float aVolume );
		float getVolume() const;
		
		double getTime() const { return ( mLoader->getSampleOffset() / (double)mSource->getSampleRate() ); }
		void setTime( double aTime );
		
		bool isLooping() const { return mIsLooping; }
		void setLooping( bool isLooping ) { mIsLooping = isLooping; }
		
		void enablePcmBuffering( bool isBuffering ) { mIsPcmBuffering = isBuffering; }
		bool isPcmBuffering() { return mIsPcmBuffering; }
		
		PcmBuffer32fRef getPcmBuffer();
	  private:
		static OSStatus renderCallback( void * audioLoader, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData );
		static OSStatus	renderNotifyCallback( void * audioTrack, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData );
		
		void createPcmBuffer();
		
		SourceRef		mSource;
		shared_ptr<TargetOutputImplAudioUnit> mTarget;
		OutputImplAudioUnit	* mOutput;
		TrackId			mInputBus;
		LoaderRef		mLoader;
		bool			mIsPlaying;
		bool			mIsLooping;
		bool			mIsPcmBuffering;
		
		PcmBuffer32fRef	mLoadingPcmBuffer;
		PcmBuffer32fRef	mLoadedPcmBuffer;
		boost::mutex	mPcmBufferMutex;
	};
	
	std::map<TrackId,shared_ptr<OutputImplAudioUnit::Track> >	mTracks;
	
	friend class TargetOutputImplAudioUnit;
};

}} //namespace