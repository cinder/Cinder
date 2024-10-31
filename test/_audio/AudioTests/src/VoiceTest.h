#pragma once

#include "AudioTest.h"

#include "cinder/audio/Voice.h"
#include "cinder/audio/MonitorNode.h"

class VoiceTest : public AudioTest {
  public:
	VoiceTest();

	void openFile( const ci::fs::path &fullPath ) override;
	void draw() override;
	void updateUI() override;

  private:
	void setupSubTest( const std::string &testName );
	void setupBasic();
	void setupBasicStereo();
	void setupDifferentFile();
	void setupScope();

	ci::audio::VoiceRef			mVoice;
	ci::audio::MonitorNodeRef	mMonitor;

	//enum InputBus { SINE, NOISE };

	float						mVolume = 0.4f;
	float						mPan = 0.5f;
};

