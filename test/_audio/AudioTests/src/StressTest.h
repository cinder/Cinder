#pragma once

#include "AudioTest.h"

#include "cinder/audio/GenNode.h"
#include "cinder/audio/GainNode.h"
#include "cinder/audio/MonitorNode.h"

#include "../../../../samples/_audio/common/AudioDrawUtils.h"

class StressTest : public AudioTest {
  public:
	StressTest();

	void draw() override;
	void updateUI() override;

  private:
	void addGens();
	void removeGens();
	void clearGens();

	void setupNextGenType( const std::string &genType );

	ci::audio::GenNodeRef	makeSelectedGenType();
	ci::audio::GenNodeRef	makeOsc( ci::audio::WaveformType type );

	ci::audio::GainNodeRef				mGain;
	ci::audio::MonitorSpectralNodeRef	mMonitor;
	ci::audio::WaveTable2dRef			mWaveTable;
	std::vector<ci::audio::GenNodeRef>	mGenBank;

	SpectrumPlot		mSpectrumPlot;

	enum GenType { SINE, TRIANGLE, OSC_SINE, OSC_SAW, OSC_SQUARE, OSC_TRIANGLE };

	bool				mDrawingEnabled = true;
	int					mAddIncr = 1;
	GenType				mSelectedGenType = OSC_SQUARE;
};
