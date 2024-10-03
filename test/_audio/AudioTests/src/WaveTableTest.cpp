#include "WaveTableTest.h"

#include "cinder/app/App.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"
#include "cinder/CinderImGui.h"

#include "cinder/audio/Utilities.h"

#include "../../../../samples/_audio/common/AudioDrawUtils.h"

using namespace ci;
using namespace std;
namespace im = ImGui;

const float sDefaultFreq = 100;

WaveTableTest::WaveTableTest()
{
	mSubTests = {
		"sine",
		"square",
		"sawtooth",
		"triangle",
		"pulse",
		"sine (table)",
		"triangle (calc)"
	};

	auto ctx = audio::master();
	mGain = ctx->makeNode( new audio::GainNode );
	mGain->setValue( 0.075f );

	mMonitor = audio::master()->makeNode( new audio::MonitorSpectralNode( audio::MonitorSpectralNode::Format().fftSize( 1024 ).windowSize( 2048 ) ) );
	mMonitor->setSmoothingFactor( 0 );

	setupOsc( audio::WaveformType::SINE );
//	setupTable();
//	setupPulse();

	mGen >> mMonitor >> mGain >> ctx->getOutput();
}

void WaveTableTest::setupSubTest( const std::string &testName )
{
	mGen = nullptr;
	mGenPulse = nullptr;
	mGenOsc  = nullptr;

	mMonitor->disconnectAllInputs();

	if( testName == "sine" )
		setupOsc( audio::WaveformType::SINE );
	else if( testName == "square" )
		setupOsc( audio::WaveformType::SQUARE );
	else if( testName == "sawtooth" )
		setupOsc( audio::WaveformType::SAWTOOTH );
	else if( testName == "triangle" )
		setupOsc( audio::WaveformType::TRIANGLE );
	else if( testName == "pulse" )
		setupPulse();
	else if( testName == "sine (table)" )
		setupTable();
	else if( testName == "triangle (calc)" )
		setupTriangleCalc();

	mGen >> mMonitor;
}

void WaveTableTest::setupTable()
{
	auto ctx = audio::master();
	float freq = mGen ? mGen->getFreq() : sDefaultFreq;

	auto gen = ctx->makeNode( new audio::GenTableNode );
	gen->setFreq( freq );
	gen->enable();

	mGen = gen;
}

void WaveTableTest::setupOsc( audio::WaveformType type )
{
	auto ctx = audio::master();
	float freq = mGen ? mGen->getFreq() : sDefaultFreq;

	mGenOsc = ctx->makeNode( new audio::GenOscNode( type ) );
	mGenOsc->setFreq( freq );
	mGenOsc->enable();

	mGen = mGenOsc;
}

void WaveTableTest::setupPulse()
{
	float freq = mGen ? mGen->getFreq() : sDefaultFreq;

	if( ! mGenPulse ) {
		mGenPulse = audio::master()->makeNode( new audio::GenPulseNode );
		mGenPulse->setFreq( freq );
		mGenPulse->enable();
	}

	if( mGenOsc )
		mGenOsc->disconnectAll();

	mGenPulse >> mMonitor;
	mGen = mGenPulse;

#if 1
	// pwm
	auto mod = audio::master()->makeNode( new audio::GenTableNode );

	audio::master()->initializeNode( mod );

	vector<float> table( mod->getWaveTable()->getTableSize() );

	mod->getWaveTable()->copyTo( table.data() );

	// scale to [0.05 : 0.95]
	audio::dsp::mul( table.data(), 0.45f, table.data(), table.size() );
	audio::dsp::add( table.data(), 0.5f, table.data(), table.size() );

//	mTableCopy.setNumFrames( table.size() );
//	memmove( mTableCopy.getData(), table.data(), table.size() * sizeof( float ) );

	mod->getWaveTable()->copyFrom( table.data() );
	mod->setFreq( 0.6f );
	mod->enable();

	mGenPulse->getParamWidth()->setProcessor( mod );
#endif
}

// for comparison with GenOscNode's triangle spectra
void WaveTableTest::setupTriangleCalc()
{
	auto ctx = audio::master();
	float freq = mGen ? mGen->getFreq() : sDefaultFreq;

	auto gen = ctx->makeNode( new audio::GenTriangleNode );
	gen->setFreq( freq );
	gen->enable();

	mGen = gen;
}

void WaveTableTest::update()
{
	if( mGenOsc ) {
		mTableCopy.setNumFrames( mGenOsc->getTableSize() );
		mGenOsc->getWaveTable()->copyTo( mTableCopy.getData(), (size_t)mGenOsc->getWaveTable()->calcBandlimitedTableIndex( mGenOsc->getFreq() ) );
	}
	if( mGenPulse ) {
		// TODO: add to UI
		//mPulseWidthSlider.set( mGenPulse->getWidth() );
	}
}

void WaveTableTest::draw()
{
	gl::clear();

	const float padding = 10;
	const float scopeHeight = ( app::getWindowHeight() - padding * 4 ) / 3.0f;

	Rectf rect( padding, padding, app::getWindowWidth() - padding, scopeHeight + padding );
	drawAudioBuffer( mTableCopy, rect, true );

	rect += vec2( 0, scopeHeight + padding );
	drawAudioBuffer( mMonitor->getBuffer(), rect, true );

	rect += vec2( 0, scopeHeight + padding );
	mSpectrumPlot.setBounds( rect );
	mSpectrumPlot.draw( mMonitor->getMagSpectrum() );
}

// -----------------------------------------------------------------------
// UI
// -----------------------------------------------------------------------

void WaveTableTest::updateUI()
{
	float gain = mGain->getValue();
	if( im::SliderFloat( "gain", &gain, 0, 1 ) ) {
		mGain->getParam()->applyRamp( gain, 0.3f );
	}

	static float genRampTime = 0.2f;
	float freq = mGen->getFreq();
	if( im::SliderFloat( "qen freq", &freq, -200, 1200 ) ) {
		mGen->getParamFreq()->applyRamp( freq, genRampTime );
	}
	im::DragFloat( "freq ramp time", &genRampTime, 0.01f, 0, 5 );

	if( mGenPulse ) {
		float w = mGenPulse->getWidth();
		if( im::SliderFloat( "pulse width", &w, 0, 1 ) ) {
			mGenPulse->getParamWidth()->applyRamp( w, 0.2f );
		}
	}

	// TODO: UI + logic for modifying the WaveTable WaveTable2d
	// - will have to create a new WaveTable for thread-safety
	// - would be nice to do it async
	//int tableSize = mGenOsc->getTableSize();
	//if( im::InputInt( "table size", &tableSize, 100, 1000 ) ) {
	//	mGenOsc->setWaveform( mGenOsc->getWaveForm(), tableSize );
	//}

	if( im::ListBox( "sub-tests", &mCurrentSubTest, mSubTests, (int)mSubTests.size() ) ) {
		setupSubTest( mSubTests[mCurrentSubTest] );
	}
}