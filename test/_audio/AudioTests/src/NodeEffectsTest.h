#pragma once

#include "AudioTest.h"

#include "cinder/audio/GenNode.h"
#include "cinder/audio/NodeEffects.h"

class NodeEffectsTest : public AudioTest {
public:
	NodeEffectsTest();

	void update() override;
	void updateUI() override;

private:
	void makeNodes();
	void setupSubTest( const std::string &testName );

	void setupOne();
	void setupForceStereo();
	void setupDownMix();
	void setupDelay();
	void setupVariableDelay();
	void setupFeedback();
	void setupEcho();
	void setupCycle();

	void applyChirp();

	std::vector<std::string>	mSubTests;
	int							mCurrentSubTest = 0;
	bool						mUseSine = false; // if false, uses noise instead of sine

	ci::audio::GenNodeRef			mGen;
	ci::audio::GainNodeRef			mGain;
	ci::audio::Pan2dNodeRef			mPan;
	ci::audio::FilterLowPassNodeRef	mLowPass;
	ci::audio::DelayNodeRef			mDelay;
};

