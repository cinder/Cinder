#ifndef FLINT_AUDIO_SOURCE_H_
#define FLINT_AUDIO_SOURCE_H_

#include "flint/Flint.h"

#if defined( FLI_MAC )
	#include <CoreAudio/CoreAudioTypes.h>
#endif

namespace fli {

class AudioSource {
public:
	virtual			~AudioSource() {}

	int				getChannelCount() const { return mChannelCount; }
	int				getSampleRate() const { return mSampleRate; }
	int				getBitsPerSample() const { return mBitsPerSample; }
	int				getBlockAlign() const { return mBlockAlign; }
	
	virtual	void	getData( void ** aBuffer, uint32_t * size, AudioStreamPacketDescription * packetDescriptions, uint32_t startSample, uint32_t sampleCount, bool looping );
	
#if defined( FLI_MAC )	
	uint32_t	mNativeFormatID;
	uint32_t	mNativeFormatFlags;
	uint32_t	mBytesPerPacket;
	uint32_t	mFramesPerPacket;
	uint32_t	mBytesPerFrame;
	uint32_t	mReserved;
#endif
protected:
	AudioSource() {}

	int32_t			mSampleRate;
	int16_t			mChannelCount;
	int16_t			mBitsPerSample; //Significant Bits Per Sample
	int16_t			mBlockAlign; //BytesPerSample
};

} //namespace fli

#endif