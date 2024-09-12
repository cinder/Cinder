#pragma once

#include "AudioTest.h"

#include "cinder/audio/GainNode.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/MonitorNode.h"

class NodeBasicTest : public AudioTest {
public:
	NodeBasicTest();

	void draw() override;
	void updateUI() override;

private:
	void setupSubTest( const std::string &testName );

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

	std::vector<std::string>	mSubTests;
	int							mCurrentSubTest = 0;

	ci::audio::GainNodeRef		mGain;
	ci::audio::MonitorNodeRef	mMonitor;
	ci::audio::GenNodeRef		mGen, mNoise;
};

