#ifndef FLINT_AUDIO_PLAYER_H
#define FLINT_AUDIO_PLAYER_H

#include "flint/Flint.h"
#include <iostream>
#include <vector>

#if defined( FLI_MAC )
	#include <CoreServices/CoreServices.h>
	#include <CoreAudio/CoreAudioTypes.h>
	#include <AudioToolbox/AudioFile.h>
	#include <AudioUnit/AudioUnit.h>
	#include <AudioToolbox/AUGraph.h>
	#include <AudioToolbox/AudioConverter.h>
#elif defined( FLI_MSW )
	#include <windows.h>
	#include <Dshow.h>
#endif

#include "AudioSource.h"

/*
TODO:
device selection
metering
seeking
device enumeration
*/

namespace fli {

#if defined( FLI_MAC )
OSStatus fli_renderCallback( void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32  inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData);
OSStatus fli_dataInputCallback( AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescription, void *audioTrack );
#endif

class AudioPlayer;

class AudioPlayerTrack {
public:
#if defined( FLI_MAC )
	friend OSStatus fli::fli_renderCallback( void*, AudioUnitRenderActionFlags*, const AudioTimeStamp*, UInt32, UInt32, AudioBufferList* );
	friend OSStatus fli::fli_dataInputCallback( AudioConverterRef, UInt32*, AudioBufferList*, AudioStreamPacketDescription**, void* );
#endif
	friend class AudioPlayer;

	~AudioPlayerTrack();
	shared_ptr<AudioSource> getSource() { return mAudioSource; }
	
	bool isPlaying() { return mIsLooping; }
	bool isLooping() { return mIsPlaying; }
	
	void play() { mIsPlaying = true; }
	void stop() { mIsPlaying = false; }
	
	void setLooping( bool shouldLoop ) { mIsLooping = shouldLoop; }
private:
	AudioPlayerTrack( shared_ptr<AudioSource> aAudioSource, AudioPlayer * aAudioPlayer );
	
	void setup();
	void cleanup();
	
	AudioPlayer						* mAudioPlayer;
	shared_ptr<AudioSource>			mAudioSource;
	
	void							* mSourceBuffer;
	uint32_t						mPacketOffset;
	
	bool							mIsLooping;
	bool							mIsPlaying;
	
#if defined( FLI_MAC )
	AudioConverterRef				* mSourceConverter;
	AudioStreamPacketDescription	* mCurrentPacketDescriptions;
#endif
};

class AudioPlayer {
public:
#if defined( FLI_MAC )
	static void onComplete(void *aData, ScheduledAudioFileRegion *fileRegion, OSStatus result);
	//friend OSStatus fli::fli_renderCallback( void*, AudioUnitRenderActionFlags*, const AudioTimeStamp*, UInt32, UInt32, AudioBufferList* );
	//friend OSStatus fli::fli_dataInputCallback( AudioConverterRef, UInt32*, AudioBufferList*, AudioStreamPacketDescription**, void* );
#endif
	friend class AudioPlayerTrack;
	
	//AudioPlayer( const AudioFile * aAudioFile );
	AudioPlayer();
	
	~AudioPlayer();
	
	/*const AudioFile& getSource() { return *mAudioSource; }
	void setSource( const AudioFile * aAudioFile );*/
	//shared_ptr<AudioSource> getSource() { return mAudioSource; }
	//void setSource( AudioSource * aAudioSource );
	//void setSource( shared_ptr<AudioSource> aAudioSource );
	
	const std::vector<AudioPlayerTrack *>& getTracks() const { return *mTracks; }
	
	void addSource( AudioSource * aAudioSource, bool autoPlay = true );
	void addSource( shared_ptr<AudioSource> aAudioSource, bool autoPlay = true );
	
	
	void play();
	void stop();
	void pause();
	
	bool isRunning() const;
	bool isPlaying() const;
	
	double getCurrentPlayTime() const; //returns playtime in milleseconds
	void doMeter();
	void getSample();
	
	void setVolume( float aValue ); //between 0.0 and 1.0
	float getVolume() const;
	
	/*
		\param aValue balance value ranging between -1.0 (full right) and 1.0 (full left)
	*/
	void setBalance( float aValue ); 
	float getBalance() const;
	
	void load( char * data, int length, int offset = 0 );
private:
	void setupGraph();
	void setupTrack( AudioPlayerTrack * aAudioTrack, int inputBusNo );
	//void setupSource();
	
	void onStop();
	
#if defined( FLI_MAC )
	bool							mIsPaused;
	std::vector<AudioPlayerTrack *>	*mTracks; 
	
	//int							mCurrentSampleTime;
	//int							mPacketOffset;
	//void							* mSourceBuffer;
	//shared_ptr<AudioSource>			mAudioSource;
	//AudioConverterRef				* mSourceConverter;
	
	AUGraph							mGraph;
	AUNode							mMixerNode;
	AUNode							mOutputNode;
	AudioUnit						mOutputUnit;
	AudioUnit						mMixerUnit;
	AudioUnit						mSourceUnit;
	AudioStreamBasicDescription		* mPlayerDescription;
#elif defined( FLI_MSW )
	IGraphBuilder					* mGraphBuilder;
	IMediaControl					* mMediaControl;
	IMediaEventEx					* mMediaEvent;
	IMediaSeeking					* mSeek;	
	IBasicAudio						* mBasicAudio;
#endif
};

} //namespace fli

#endif