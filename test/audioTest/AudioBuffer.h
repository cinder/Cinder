#ifndef FLINT_AUDIO_BUFFER_H_
#define FLINT_AUDIO_BUFFER_H_

#include "AudioSource.h"

namespace fli {

class AudioBuffer : public AudioSource {
public:
	AudioBuffer( double *aData, int aSampleRate, int aChannelCount, int aBitsPerSample, int aBlockAlign );
	//AudioBuffer( const AudioFile * aFile );
	//AudioBuffer( const AudioFileWAV * aFile );
	
	~AudioBuffer() {}
	
	//int		getLength() const { return mDataLength; };
	
	void getData( void ** aBuffer, int startSample, int sampleCount );
	
private:
	double *		mData;
};

} // namespace fli

#endif