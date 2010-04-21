#ifndef FLINT_AUDIO_FILE_H_
#define FLINT_AUDIO_FILE_H_

#include "flint/Flint.h"

#include <string>
#include <iostream>

#if defined( FLI_MAC )
	#include <CoreServices/CoreServices.h>
	#include <AudioToolbox/AudioFile.h>
#endif

#include "AudioSource.h"

namespace fli {

class AudioFile : public AudioSource {
 public:
	AudioFile( const std::string &aFilePath );
	~AudioFile();
	
	int		getLength() const { return mByteCount; };
	double	getDuration() const;
	
	bool isInterleaved() const {
#if defined( FLI_MAC )
		return !isPCM() || !( mNativeFormatFlags & kAudioFormatFlagIsNonInterleaved );
#elif defined( FLI_MSW )
		//TODO
		return true;
#endif
	}
	
	bool isPCM() const {
#if defined( FLI_MAC )
		return ( mNativeFormatID == kAudioFormatLinearPCM );
#elif defined( FLI_MSW )
		//TODO
		return true;
#endif
	}
	
	void	load();
	void	getData( void ** aBuffer, uint32_t * size, AudioStreamPacketDescription * packetDescriptions, uint32_t startSample, uint32_t sampleCount, bool looping );
 private:
	void						loadHeader();
#if defined( FLI_MAC )
	AudioFileID					mNativeFileRef;
#endif
	
	/*
		TODO: set this up so that it can be passed to an AudioBuffer and the buffer 
		knows how to ask it to load the bits into it's data pointer
	*/
	
	uint64_t	mPacketCount;
	uint64_t	mByteCount;
	uint32_t	mMaxPacketSize;
	
	void		* mData;
};

//shared_ptr<AudioFile> loadAudio( const std::string &aFilename );
//shared_ptr<AudioFile> loadAudio( shared_ptr<IStream> stream );

/*class AudioFileExc {
};

class AudioFileInvalidExc : public AudioFileExc {
};*/

} //namespace

#endif