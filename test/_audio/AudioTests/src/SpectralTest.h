#pragma once

#include "AudioTest.h"

#include "cinder/audio/GenNode.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/dsp/Dsp.h"

#include "../../../../samples/_audio/common/AudioDrawUtils.h"

class SpectralTest : public AudioTest {
  public:
	SpectralTest();

	void openFile( const ci::fs::path &fullPath ) override;
	void resize() override;
	void draw() override;
	void updateUI() override;

  private:
	void setupSubTest( const std::string &testName );
	void setupSine();
	void setupSineNoOutput();
	void setupSample();
	void setupUI();
	void processTap( ci::ivec2 pos );
	void processDrag( ci::ivec2 pos );
	void printBinFreq( size_t xPos );


	ci::audio::BufferPlayerNodeRef		mPlayerNode;
	ci::audio::GenNodeRef				mGen;
	ci::audio::MonitorSpectralNodeRef	mMonitorSpectralNode;
	ci::audio::SourceFileRef			mSourceFile;

	std::vector<std::string>	mSubTests;
	int							mCurrentSubTest = 0;

	// TODO: add gui for these
	//Button							mLoopButton, mScaleDecibelsButton;
	//HSlider							mSmoothingFactorSlider, mFreqSlider;

	SpectrumPlot					mSpectrumPlot;
	float							mSpectroMargin = 40;
	bool							mUseScaleDecibels = true;
	bool							mPlaybackEnabled = false;
	bool							mLoopEnabled = false;
	float							mLoopBegin = 0;
	float							mLoopEnd = 1;
};
