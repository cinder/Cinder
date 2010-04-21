#include "AudioBuffer.h"
#include <cmath>
#include <iostream>

namespace fli {

AudioBuffer::AudioBuffer( double * aData, int aSampleRate, int aChannelCount, int aBitsPerSample, int aBlockAlign )
	: AudioSource()
{
	mSampleRate = aSampleRate;
	mChannelCount = aChannelCount;
	mBitsPerSample = aBitsPerSample; 
	mBlockAlign = aBlockAlign;

	mData = aData;
	//TODO: do something about the data
}

/*AudioBuffer::AudioBuffer( const AudioFileWAV * aFile )
{
	mSampleRate = aFile->getSampleRate();
	mChannelCount = aFile->getChannelCount();
	mBitsPerSample = aFile->getBitsPerSample();
	mBlockAlign = aFile->getBlockAlign();
	
	mData = new char[aFile->getLength()];
	aFile->load( mData, aFile->getLength() );
	
}*/

void AudioBuffer::getData( void ** aBuffer, int startSample, int sampleCount )
{
	float ** buffers = (float **)aBuffer;
	int now = startSample;
	
	double twoPi = 2 * 3.1415926535;
	float freq = 220.0;
	float mult = freq / mSampleRate;
	
    for ( int i = 0; i < sampleCount; i++ ){
		for( int j = 0; j < mChannelCount; j++ ) {
			buffers[j][i] = sin( twoPi * now * mult );
		}
		now += 1;
    }
}

} // namesapce fli