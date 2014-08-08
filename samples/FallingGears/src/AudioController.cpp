#include "AudioController.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/Utilities.h"
#include "cinder/audio/Debug.h"

#define ALTO_BANK_SIZE 70
#define BASS_BANK_SIZE 40
#define DEFAULT_TABLE_SIZE 4096
#define DEFAULT_BANDLIMITED_TABLES 40
#define ALTO_LOWPASS_CENTER_FREQ 500

using namespace ci;
using namespace std;

namespace {

template <typename SynthT>
shared_ptr<SynthT> getFreeSynth( const vector<shared_ptr<SynthT> > &bank )
{
	shared_ptr<SynthT> oldest;

	size_t i = 0;
	for( const auto &synth : bank ) {
		if( ! synth->mInUse )
			return synth;
		else if( ! oldest || oldest->mTimeLastUsed < synth->mTimeLastUsed )
			oldest = synth;
		i++;
	}

	return oldest;
}

} // anonymous namespace

void AudioController::setup()
{
	auto ctx = audio::master();
	mMasterGain = ctx->makeNode( new audio::GainNode( 0.1 ) );
	mAltoLowpass = ctx->makeNode( new audio::FilterLowPassNode );

	mSawtoothTable.reset( new audio::WaveTable2d( ctx->getSampleRate(), DEFAULT_TABLE_SIZE, DEFAULT_BANDLIMITED_TABLES ) );
	mSawtoothTable->fillBandlimited( audio::WaveformType::SAWTOOTH );

	for( size_t i = 0; i < ALTO_BANK_SIZE; i++ ) {
		auto synth = make_shared<AltoSynth>( this );
		synth->getOutput() >> mAltoLowpass;

		mAltoSynthBank.push_back( synth );
		mAllSynths.push_back( synth );
	}

	for( size_t i = 0; i < BASS_BANK_SIZE; i++ ) {
		auto synth = make_shared<BassSynth>( this );
		synth->getOutput() >> mMasterGain;

		mBassSynthBank.push_back( synth );
		mAllSynths.push_back( synth );
	}

	mAltoLowpass >> mMasterGain;
	mMasterGain >> ctx->getOutput();
	ctx->enable();

//	CI_LOG_V( "audio graph:\n" << ctx->printGraphToString() );
}

void AudioController::setMasterGain( float gainDb )
{
	float gainLinear = audio::decibelToLinear( gainDb );
	mMasterGain->getParam()->applyRamp( gainLinear, 0.3f );
}

float AudioController::getMasterGain() const
{
	return audio::linearToDecibel( mMasterGain->getValue() );
}

void AudioController::update()
{
	uint64_t lastClip = audio::master()->getOutput()->getLastClip();
	if( lastClip ) {

		float gainVal = mMasterGain->getValue() * 0.6f;
		mMasterGain->getParam()->applyRamp( gainVal, 0.3f );

		CI_LOG_V( "WARNING: output clipped after processing frames: " << lastClip << ", new gain: " << gainVal );
	}

	mAltoLowpass->setCutoffFreq( mParams.mAltoLowPassFreq );

	for( auto &synth : mAllSynths )
		synth->update();
}

void AudioController::reset()
{
	for( auto &synth : mAllSynths )
		synth->recycle();
}

AltoSynthRef AudioController::getNextAltoSynth()
{
	return getFreeSynth( mAltoSynthBank );
}

BassSynthRef AudioController::getNextBassSynth()
{
	return getFreeSynth( mBassSynthBank );
}

size_t AudioController::getNumUsedSynths() const
{
	size_t result = 0;
	for( const auto &synth : mAllSynths ) {
		if( synth->mInUse )
			result++;
	}

	return result;
}
