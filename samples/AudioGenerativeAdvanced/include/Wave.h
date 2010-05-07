#pragma once

#include "cinder/Cinder.h"
#include "cinder/audio/Buffer.h"

class SineWave {
 public:
	SineWave( uint32_t freq, uint32_t duration );
	~SineWave() {}
	void getData( uint64_t inSampleOffset, uint32_t ioSampleCount, ci::audio::Buffer *ioBuffer );
 private:
	uint32_t mFreq;
	uint32_t mDuration;
};