#ifndef FLINT_AUDIO_FILE_WAV_H_
#define FLINT_AUDIO_FILE_WAV_H_

#include <iostream>
#include "flint/Stream.h"
#include "flint/Utilities.h"
#include "AudioSource.h"

namespace fli {

class AudioFileWAV : public AudioSource {
public:
	AudioFileWAV( shared_ptr<IStream> aStream );
	~AudioFileWAV();
	
	int		getLength() const { return mDataLength; };
	double	getDuration() const;
	
	void	load( char * dataContainer, int length, int offset = 0 );
	void	getData( void ** aBuffer, uint32_t * size, uint32_t startSample, uint32_t sampleCount );
private:
	void						loadHeader();
	void						readFormatChunk();
	
	shared_ptr<IStream>			mStream;
	/*
		TODO: set this up so that it can be passed to an AudioBuffer and the buffer 
		knows how to ask it to load the bits into it's data pointer
	*/
	int16_t		mAudioFormat;
	int32_t		mByteRate;
	int32_t		mDataLength;
	int32_t		mDataStart;
	int32_t		mSampleCount;
	
	int16_t		* mData;
};

shared_ptr<AudioFileWAV> loadAudio( const std::string &aFilename );
shared_ptr<AudioFileWAV> loadAudio( shared_ptr<IStream> stream );

class AudioFileExc {
};

class AudioFileInvalidExc : public AudioFileExc {
};

} //namespace

#endif