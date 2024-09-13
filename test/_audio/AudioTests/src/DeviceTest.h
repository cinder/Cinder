#pragma once

#include "AudioTest.h"

#include "cinder/Rect.h"
#include "cinder/Tween.h"

#include "cinder/audio/GainNode.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/OutputNode.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/SampleRecorderNode.h"

const bool SET_FRAMES_PER_BLOCK = 0; // TODO: move to main app
const size_t FRAMES_PER_BLOCK = 256; // TODO: move to main app

class DeviceTest : public AudioTest {
  public:
	DeviceTest();

	void update() override;
	void draw() override;
	void updateUI() override;

	void setOutputDevice( const ci::audio::DeviceRef &device, size_t numChannels = 0 );
	void setInputDevice( const ci::audio::DeviceRef &device, size_t numChannels = 0 );
	void setupMultiChannelDevice( const std::string &deviceName );
	void setupMultiChannelDeviceWindows( const std::string &deviceName );
	void setupRolandOctaCaptureInputMonitoring();
	void printDeviceDetails( const ci::audio::DeviceRef &device );
	void startRecording();
	void writeRecordedToFile();

	void setupSine();
	void setupNoise();
	void setupInputPulled();
	void setupIOClean();
	void setupIOProcessed();
	void setupIOAndSine();
	void setupSend();
	void setupSendStereo();
	void updateUIParams();

	void setupSubTest( const std::string &testName );

	int							mCurrentSubTest = 0;
	std::vector<std::string>	mSubTests;

	ci::audio::InputDeviceNodeRef		mInputDeviceNode;
	ci::audio::OutputDeviceNodeRef		mOutputDeviceNode;
	ci::audio::MonitorNodeRef			mMonitor;
	ci::audio::BufferRecorderNodeRef	mRecorder;
	ci::audio::GainNodeRef				mGain;
	ci::audio::GenNodeRef				mGen;

	//vector<TestWidget *> mWidgets;
	//VSelector mTestSelector, mInputSelector, mOutputSelector;
	//Button mPlayButton, mRecordButton;
	//HSlider mGainSlider;
	//TextInput mSamplerateInput, mFramesPerBlockInput, mNumInChannelsInput, mNumOutChannelsInput, mSendChannelInput;

	ci::Anim<float> mInputDeviceNodeUnderrunFade, mInputDeviceNodeOverrunFade, mOutputDeviceNodeClipFade;
	ci::Anim<float> mViewYOffset; // for iOS keyboard
	ci::Rectf mUnderrunRect, mOverrunRect, mClipRect;
};
