#pragma once

#include "Synths.h"

#include "cinder/audio/Context.h"

class AudioController {
  public:
	void setup();
	void update();

	void reset();

	void setMasterGain( float gainDb );
	float getMasterGain() const;

	AltoSynthRef	getNextAltoSynth();
	BassSynthRef	getNextBassSynth();

	size_t getNumUsedSynths() const;
	size_t getNumTotalSynths() const	{ return mAllSynths.size(); }

	ci::audio::WaveTable2dRef	getSawToothTable() const	{ return mSawtoothTable; }

	struct Params {
		Params() : mAltoLowPassFreq( 3200 )	{}
		
		float	mAltoLowPassFreq;
	};

	Params& getParams()	{ return mParams; }

  private:

	std::vector<AltoSynthRef>			mAltoSynthBank;
	std::vector<BassSynthRef>			mBassSynthBank;
	std::vector<SynthRef>				mAllSynths;
	ci::audio::WaveTable2dRef			mSawtoothTable;
	ci::audio::GainNodeRef				mMasterGain;
	ci::audio::FilterLowPassNodeRef		mAltoLowpass;
	Params								mParams;
};