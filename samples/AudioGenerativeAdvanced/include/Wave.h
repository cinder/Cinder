#pragma once

#include <iostream>
#include "cinder/Cinder.h"
#include "cinder/audio/Buffer.h"

class SineWave {
 public:
	SineWave( uint32_t freq, float duration );
	~SineWave() {}
	void getData( uint64_t inSampleOffset, uint32_t inSampleCount, ci::audio::Buffer *ioBuffer );
 private:
	uint32_t mFreq;
	float mDuration;
};