#include "StressTest.h"

#include "cinder/app/App.h"
#include "cinder/Rand.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"
#include "cinder/CinderImGui.h"

#include "cinder/audio/Utilities.h"

using namespace ci;
using namespace std;
namespace im = ImGui;

StressTest::StressTest()
{
	mSubTests = {
		"sine",
		"triangle",
		"osc sine",
		"osc sawtooth",
		"osc square",
		"osc triangle",
	};

	auto ctx = audio::master();
	mGain = ctx->makeNode( new audio::GainNode );
	mGain->setValue( 0.1f );

	mMonitor = audio::master()->makeNode( new audio::MonitorSpectralNode( audio::MonitorSpectralNode::Format().fftSize( 1024 ).windowSize( 2048 ) ) );
	mMonitor->setSmoothingFactor( 0.4f );

	mGain >> mMonitor >> ctx->getOutput();

	addGens();
}

void StressTest::setupSubTest( const string &testName )
{
	if( testName == "sine" )
		mSelectedGenType = SINE;
	else if( testName == "triangle" )
		mSelectedGenType = TRIANGLE;
	else if( testName == "osc sine" ) {
		mSelectedGenType = OSC_SINE;
		mWaveTable.reset();
	}
	else if( testName == "osc sawtooth" ) {
		mSelectedGenType = OSC_SAW;
		mWaveTable.reset();
	}
	else if( testName == "osc square" ) {
		mSelectedGenType = OSC_SQUARE;
		mWaveTable.reset();
	}
	else if( testName == "osc triangle" ) {
		mSelectedGenType = OSC_TRIANGLE;
		mWaveTable.reset();
	}
}

void StressTest::addGens()
{
	auto ctx = audio::master();

	for( size_t i = 0; i < mAddIncr; i++ ) {
		auto gen = makeSelectedGenType();
		gen->setFreq( audio::midiToFreq( randInt( 40, 60 ) ) );

		gen->connect( mGain );
		gen->enable();

		mGenBank.push_back( gen );
	}
}

void StressTest::removeGens()
{
	for( size_t i = 0; i < mAddIncr; i++ ) {
		mGenBank.back()->disconnectAll();
		mGenBank.pop_back();
	}
}

void StressTest::clearGens()
{
	while( ! mGenBank.empty() ) {
		mGenBank.back()->disconnectAll();
		mGenBank.pop_back();
	}
}

audio::GenNodeRef StressTest::makeSelectedGenType()
{
	switch( mSelectedGenType ) {
		case SINE:			return audio::master()->makeNode( new audio::GenSineNode );
		case TRIANGLE:		return audio::master()->makeNode( new audio::GenTriangleNode );
		case OSC_SINE:		return makeOsc( audio::WaveformType::SINE );
		case OSC_SAW:		return makeOsc( audio::WaveformType::SAWTOOTH );
		case OSC_SQUARE:	return makeOsc( audio::WaveformType::SQUARE );
		case OSC_TRIANGLE:	return makeOsc( audio::WaveformType::TRIANGLE );
		default: CI_ASSERT_NOT_REACHABLE();
	}

	return {};
}

audio::GenNodeRef StressTest::makeOsc( audio::WaveformType type )
{
	auto ctx = audio::master();
	auto result = ctx->makeNode( new audio::GenOscNode( type ) );

	if( mWaveTable )
		result->setWaveTable( mWaveTable );
	else {
		ctx->initializeNode( result );

		mWaveTable = result->getWaveTable();
	}

	return result;
}

void StressTest::draw()
{
	if( ! mDrawingEnabled ) {
		return;
	}

	const float padding = 10;
	const float scopeHeight = ( app::getWindowHeight() - padding * 3 ) / 2;

	Rectf rect( padding, padding, app::getWindowWidth() - padding - 200, scopeHeight + padding );

	drawAudioBuffer( mMonitor->getBuffer(), rect, true );

	rect += vec2( 0, scopeHeight + padding );
	mSpectrumPlot.setBounds( rect );
	mSpectrumPlot.draw( mMonitor->getMagSpectrum() );

}

// -----------------------------------------------------------------------
// UI
// -----------------------------------------------------------------------

void StressTest::updateUI()
{
	im::Text( "Gen count: %d", mGenBank.size() );

	float gain = mGain->getValue();
	if( im::SliderFloat( "gain", &gain, 0, 1 ) ) {
		mGain->setValue( gain );
	}
	im::InputInt( "add incr", &mAddIncr, 1, 10 );
	if( im::Button( "add gens" ) ) {
		addGens();
	}
	im::SameLine();
	if( im::Button( "remove gens" ) ) {
		removeGens();
	}
	im::SameLine();
	if( im::Button( "clear" ) ) {
		clearGens();
	}

	im::Checkbox( "draw waveforms", &mDrawingEnabled );

	if( im::ListBox( "sub-tests", &mCurrentSubTest, mSubTests, mSubTests.size() ) ) {
		setupSubTest( mSubTests[mCurrentSubTest] );
	}
}