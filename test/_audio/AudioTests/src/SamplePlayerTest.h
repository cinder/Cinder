#pragma once

#include "AudioTest.h"
#include "cinder/Tween.h"

#include "cinder/audio/Source.h"
#include "cinder/audio/Target.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/SampleRecorderNode.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/audio/MonitorNode.h"

#include "../../../../samples/_audio/common/AudioDrawUtils.h"

class SamplePlayerTest : public AudioTest {
  public:
    SamplePlayerTest();

	void openFile( const ci::fs::path &fullPath ) override;
	void update() override;
	void draw() override;
	void updateUI() override;

  private:
	void setupSubTest( const std::string &testName );
	void setupBufferPlayerNode();
	void setupFilePlayerNode();
	void setupBufferRecorderNode();
	void setSourceFile( const ci::DataSourceRef &dataSource );
	void writeRecordedToFile();
	void triggerStartStop( bool start );

	void seek( size_t xPos );
	void printBufferSamples( size_t xPos );
	void printSupportedExtensions();

	void testConverter();
	void testWrite();

	ci::audio::SamplePlayerNodeRef		mSamplePlayerNode;
	ci::audio::SourceFileRef			mSourceFile;
	ci::audio::MonitorNodeRef			mMonitor;
	ci::audio::GainNodeRef				mGain;
	ci::audio::Pan2dNodeRef				mPan;
	ci::audio::BufferRecorderNodeRef	mRecorder;

	WaveformPlot				mWaveformPlot;

	ci::Anim<float>				mUnderrunFade, mOverrunFade, mRecorderOverrunFade;
	ci::Rectf					mUnderrunRect, mOverrunRect, mRecorderOverrunRect;
	bool						mSamplePlayerNodeEnabledState;
	bool						mLoadAsync = false;
	bool						mRecording = false;
	bool						mRecordAutoResize = true;
	float						mTriggerDelaySeconds = 1.0f;
	std::future<void>			mAsyncLoadFuture;
};
