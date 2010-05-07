#include "Wave.h"
#include "cinder/CinderMath.h"


SineWave::SineWave( uint32_t freq, uint32_t duration ) 
	: mFreq( freq ), mDuration( duration )
{
}

void SineWave::getData( uint64_t inSampleOffset, uint32_t ioSampleCount, ci::audio::Buffer *ioBuffer )
{
	float * buffer = (float *)ioBuffer->mData;
	
	uint64_t idx = inSampleOffset;
	
	for( int  i = 0; i < ioSampleCount; i++ ) {
		
		float val = ci::math<float>::sin( idx * ( mFreq / 44100.0f ) * 2.0f * M_PI );
		
		buffer[i*ioBuffer->mNumberChannels] = val;
		buffer[i*ioBuffer->mNumberChannels + 1] = val;
		idx++;
	}
}

