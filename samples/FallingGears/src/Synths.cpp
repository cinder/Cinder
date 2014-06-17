#include "Synths.h"
#include "AudioController.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/Utilities.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace std;

Synth::Synth( AudioController *parent )
	: mParentController( parent ), mInUse( false ), mTimeLastUsed( 0 )
{
	mGain = audio::master()->makeNode( new audio::GainNode( audio::Node::Format().autoEnable( false )) );
}

void Synth::update()
{
	if( mGainRamp && mGainRamp->isComplete() )
		recycle();
}

void Synth::recycle()
{
	mGain->disable();
	mInUse = false;
}

void Synth::setInUse()
{
	mInUse = true;
	mTimeLastUsed = app::getElapsedSeconds();
}

AltoSynth::AltoSynth( AudioController *parent )
	: Synth( parent )
{
	mGen = audio::master()->makeNode( new audio::GenOscNode );
	mGen->setWaveTable( mParentController->getSawToothTable() );

	mGen >> mGain;
}

void AltoSynth::recycle()
{
	mGen->disable();

	Synth::recycle();
}

void AltoSynth::trigger( float freqMidi, float decaySeconds )
{
	const float initialMaxVol = 0.6f;

	setInUse();

	mGen->setFreq( audio::midiToFreq( freqMidi ) );
	mGainRamp = mGain->getParam()->applyRamp( initialMaxVol, 0, decaySeconds );

	mGain->enable();
	mGen->enable();
}

BassSynth::BassSynth( AudioController *parent )
	: Synth( parent )
{
	auto ctx = audio::master();

	mGen = ctx->makeNode( new audio::GenTriangleNode );
	mPan = ctx->makeNode( new audio::Pan2dNode );

	mGen >> mPan >> mGain;
}

void BassSynth::recycle()
{
	mGen->disable();

	Synth::recycle();
}

void BassSynth::trigger( float freqMidi, float decaySeconds )
{
	setInUse();

	mGen->setFreq( audio::midiToFreq( freqMidi ) );

	mGain->getParam()->applyRamp( 0, 1, 0.014f );
	mGainRamp = mGain->getParam()->appendRamp( 0, decaySeconds );

	mGain->enable();
	mGen->enable();
}

void BassSynth::setPan( float pan )
{
	mPan->setPos( pan );
}
