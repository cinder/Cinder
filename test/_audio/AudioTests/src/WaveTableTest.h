#pragma once

#include "AudioTest.h"

#include "cinder/audio/GenNode.h"
#include "cinder/audio/GainNode.h"
#include "cinder/audio/MonitorNode.h"

#include "../../../../samples/_audio/common/AudioDrawUtils.h"

class WaveTableTest : public AudioTest {
  public:
	WaveTableTest();

	void update() override;
	void draw() override;
	void updateUI() override;

private:
	void setupSubTest( const std::string &testName );
	void setupTable();
	void setupOsc( ci::audio::WaveformType type );
	void setupPulse();
	void setupTriangleCalc();

	ci::audio::GainNodeRef				mGain;
	ci::audio::MonitorSpectralNodeRef	mMonitor;
	ci::audio::GenOscNodeRef			mGenOsc;
	ci::audio::GenPulseNodeRef			mGenPulse;
	ci::audio::GenNodeRef				mGen;

	ci::audio::BufferDynamic	mTableCopy;
	SpectrumPlot				mSpectrumPlot;
};
