#pragma once

#include "AudioTest.h"

#include "cinder/audio/GainNode.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/MonitorNode.h"

class NodeTest : public AudioTest {
public:
	NodeTest();

	void resize() override;
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

	ci::audio::GainNodeRef		mGain;
	ci::audio::MonitorNodeRef	mMonitor;
	ci::audio::GenNodeRef		mGen, mNoise;
};

