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

	void fileDrop( FileDropEvent event ) override;
	void resize() override;
	void update() override;
	void draw() override;
	void updateUI() override;

  private:
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

	//vector<TestWidget *>			mWidgets;
	//Button							mEnableGraphButton, mPlaybackButton, mLoopButton, mScaleDecibelsButton;
	//VSelector						mTestSelector;
	//HSlider							mSmoothingFactorSlider, mFreqSlider;
	SpectrumPlot					mSpectrumPlot;
	float							mSpectroMargin = 40;
};
