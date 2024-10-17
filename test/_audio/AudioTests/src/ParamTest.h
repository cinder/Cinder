#pragma once

#include "AudioTest.h"

#include "cinder/audio/GainNode.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/PanNode.h"
#include "cinder/audio/FilterNode.h"


class ParamTest : public AudioTest {
  public:
	ParamTest();

	void updateUI() override;

private:
	void setupSubTest( const std::string &testName );
	void setupBasic();
	void setupFilter();

	void testApply();
	void testApply0();
	void testApply2();
	void testAppend();
	void testDelay();
	void testAppendCancel();
	void testProcessor();
	void testSchedule();

	void writeParamEval( ci::audio::Param *param );

	ci::audio::GenNodeRef				mGen;
	ci::audio::GainNodeRef				mGain;
	ci::audio::Pan2dNodeRef				mPan;
	ci::audio::FilterLowPassNodeRef		mLowPass;
};

