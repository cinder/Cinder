#pragma once

#include "AudioTest.h"

//#include "cinder/Cinder.h"
#include "cinder/audio/audio.h" // TODO: only include what is needed here


class NodeTest : public AudioTest {
public:
	void setup() override;
	void resize() override;
	void draw() override;

	void setupGen();
	void setup2to1();
	void setup1to2();
	void setupInterleavedPassThru();
	void setupAutoPulled();
	void setupFunnelCase();
	void setupMerge();
	void setupMerge4();
	void setupSplitStereo();
	void setupSplitMerge();

	void setupUI();
	void processDrag( ivec2 pos );
	void processTap( ivec2 pos );

	ci::audio::GainNodeRef		mGain;
	ci::audio::MonitorNodeRef	mMonitor;
	ci::audio::GenNodeRef		mGen, mNoise;

	//vector<TestWidget *>	mWidgets;
	//Button					mPlayButton, mEnableNoiseButton, mEnableSineButton, mDelayedEnableButton;
	//VSelector				mTestSelector;
	//HSlider					mGainSlider;

	enum InputBus { SINE, NOISE };
};

