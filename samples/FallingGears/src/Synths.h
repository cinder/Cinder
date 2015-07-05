
#pragma once

#include "cinder/audio/GenNode.h"
#include "cinder/audio/NodeEffects.h"

typedef std::shared_ptr<class Synth>		SynthRef;
typedef std::shared_ptr<class AltoSynth>	AltoSynthRef;
typedef std::shared_ptr<class BassSynth>	BassSynthRef;

class AudioController;

class Synth {
  public:
	Synth( AudioController *parent );

	ci::audio::NodeRef	getOutput() const	{ return mGain; }

	void setInUse();
	virtual void update();
	virtual void recycle();
	virtual void trigger( float freqMidi, float decaySeconds ) = 0;

	AudioController*				mParentController;
	bool							mInUse;
	double							mTimeLastUsed;
	ci::audio::GainNodeRef			mGain;
	ci::audio::EventRef				mGainRamp;
};

class AltoSynth : public Synth {
  public:
	AltoSynth( AudioController *parent );

	void recycle() override;

	void trigger( float freqMidi, float decaySeconds ) override;

	ci::audio::GenOscNodeRef		mGen;
};

class BassSynth : public Synth {
  public:
	BassSynth( AudioController *parent );

	void recycle() override;

	void trigger( float freqMidi, float decaySeconds ) override;

	void setPan( float pan );

	ci::audio::GenNodeRef		mGen;
	ci::audio::Pan2dNodeRef		mPan;
};
