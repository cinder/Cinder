#include "ParamTest.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/CinderImGui.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/Target.h"

using namespace ci;
using namespace std;
namespace im = ImGui;

ParamTest::ParamTest()
{
	mSubTests = {
		"basic",
		"filter-pan"
	};

	auto ctx = audio::master();
	mGain = ctx->makeNode( new audio::GainNode() );
	mGain->setValue( 0.3f );

	mPan = ctx->makeNode( new audio::Pan2dNode() );

	mGen = ctx->makeNode( new audio::GenSineNode() );
//	mGen = ctx->makeNode( new audio::GenTriangleNode() );
//	mGen = ctx->makeNode( new audio::GenPhasorNode() );
	mGen = ctx->makeNode( new audio::GenPulseNode );

	mGen->setFreq( 220 );

	mLowPass = ctx->makeNode( new audio::FilterLowPassNode() );

	setupBasic();

	//setupUI();
	//PRINT_GRAPH( ctx );

	testApply();
//	testApply2();
//	connectProcessor();
}

void ParamTest::setupSubTest( const std::string &testName )
{
	auto ctx = audio::master();

	bool enabled = ctx->isEnabled();
	ctx->disable();
	ctx->disconnectAllNodes();

	if( testName == "basic" )
		setupBasic();
	else if( testName == "filter-pan" )
		setupFilter();

	ctx->setEnabled( enabled );

	CI_LOG_I( "Finished setup for test: " << testName );
}

void ParamTest::setupBasic()
{
	mGen >> mGain >> audio::master()->getOutput();
	mGen->enable();
}

void ParamTest::setupFilter()
{
	mGen >> mLowPass >> mGain >> mPan >> audio::master()->getOutput();
	mGen->enable();
}

void ParamTest::testApply()
{
	// (a): ramp volume to 0.7 of 0.2 seconds
//	mGain->getParam()->applyRamp( 0.7f, 0.2f );

	mGen->getParamFreq()->applyRamp( 220, 440, 2 );

	// PSEUDO CODE: possible syntax where context keeps references to Params, calling updateValueArray() (or just process() ?) on them each block:
	// - problem I have with this right now is that its alot more syntax for the common case (see: (a)) of ramping up volume
//	Context::master()->timeline()->apply( mGen->getParamFreq(), 220, 440, 1 );
	// - a bit shorter:
//	audio::timeline()->apply( mGen->getParamFreq(), 220, 440, 1 );

	CI_LOG_V( "num events: " << mGen->getParamFreq()->getNumEvents() );
}

// same as testApply(), but ramp time = 0. end value should still be set.
void ParamTest::testApply0()
{
	mGen->getParamFreq()->applyRamp( 220, 440, 0 );

	CI_LOG_V( "num events: " << mGen->getParamFreq()->getNumEvents() );
}

// 2 events - first apply the ramp, blowing away anything else, then append another event to happen after that
void ParamTest::testApply2()
{
	mGen->getParamFreq()->applyRamp( 220, 880, 1 );
	mGen->getParamFreq()->appendRamp( 369.994f, 1 ); // F#4

	CI_LOG_V( "num events: " << mGen->getParamFreq()->getNumEvents() );

//	writeParamEval( mGen->getParamFreq() );
}

// append an event with random frequency and duration 1 second, allowing them to build up. new events begin from the end of the last event
void ParamTest::testAppend()
{
	mGen->getParamFreq()->appendRamp( randFloat( 50, 800 ), 1.0f );

	CI_LOG_V( "num events: " << mGen->getParamFreq()->getNumEvents() );
}

// make a ramp after a 1 second delay
void ParamTest::testDelay()
{
	mGen->getParamFreq()->applyRamp( 50, 440, 1, audio::Param::Options().delay( 1 ) );
	CI_LOG_V( "num events: " << mGen->getParamFreq()->getNumEvents() );
}

// apply a ramp from 220 to 880 over 2 seconds and then after a 1 second delay, cancel it. result should be ~ 550: 220 + (880 - 220) / 2.
void ParamTest::testAppendCancel()
{
	audio::EventRef ramp = mGen->getParamFreq()->applyRamp( 220, 880, 2 );

	CI_LOG_V( "num events: " << mGen->getParamFreq()->getNumEvents() );

	app::timeline().add( [ramp] {
		CI_LOG_V( "canceling." );
		ramp->cancel();
	}, (float)app::getElapsedSeconds() + 1 );
}

void ParamTest::testProcessor()
{
	auto ctx = audio::master();
	auto mod = ctx->makeNode( new audio::GenSineNode( audio::Node::Format().autoEnable() ) );
	mod->setFreq( 2 );

	mGain->getParam()->setProcessor( mod );
}

void ParamTest::testSchedule()
{
	bool enabled = mGen->isEnabled();
	mGen->setEnabled( ! enabled, audio::master()->getNumProcessedSeconds() + 0.5f );
}

// TODO: formalize once there is an offline audio context and OutputFileNode.
void ParamTest::writeParamEval( audio::Param *param )
{
	auto ctx = audio::master();
	float duration = param->findDuration();
	float currTime = (float)ctx->getNumProcessedSeconds();
	size_t sampleRate = ctx->getSampleRate();
	audio::Buffer audioBuffer( (size_t)duration * sampleRate );

	param->eval( currTime, audioBuffer.getData(), audioBuffer.getSize(), sampleRate );

	auto target = audio::TargetFile::create( "param.wav", sampleRate, 1 );
	target->write( &audioBuffer );

	CI_LOG_V( "write complete" );
}

// -----------------------------------------------------------------------
// UI
// -----------------------------------------------------------------------

void ParamTest::updateUI()
{
	float gain = mGain->getValue();
	if( im::SliderFloat( "Gain", &gain, 0, 1 ) ) {
		//mGain->setValue( gain );
		//mGain->getParam()->applyRamp( gain );
		mGain->getParam()->applyRamp( gain, 0.15f );
	}

	float panPos = mPan->getPos();
	if( im::SliderFloat( "Pan", &panPos, 0, 1 ) ) {
		//mPan->setPos( panPos );
		mPan->getParamPos()->applyRamp( panPos, 0.3f, audio::Param::Options().rampFn( &audio::rampOutQuad ) );
	}

	float genFreq = mGen->getFreq();
	if( im::SliderFloat( "Gen Freqency", &genFreq, -200, 1200 ) ) {
		//mGen->setFreq( genFreq );
		//mGen->getParamFreq()->applyRamp( genFreq, 0.3f );
		mGen->getParamFreq()->applyRamp( genFreq, 0.3f, audio::Param::Options().rampFn( &audio::rampOutQuad ) );
	}

	float lowpassCuttoff = mLowPass->getCutoffFreq();
	if( im::SliderFloat( "Lowpass Cuttoff Freq", &lowpassCuttoff, 0, 1000 ) ) {
		mLowPass->setCutoffFreq( lowpassCuttoff );
	}

	im::Separator();
	if( im::Button( "apply" ) ) {
		testApply();
	}
	im::SameLine();
	if( im::Button( "apply0" ) ) {
		testApply0();
	}
	im::SameLine();
	if( im::Button( "apply 2" ) ) {
		testApply2();
	}
	im::SameLine();
	if( im::Button( "append" ) ) {
		testAppend();
	}

	if( im::Button( "delay" ) ) {
		testDelay();
	}
	im::SameLine();
	if( im::Button( "processor" ) ) {
		testProcessor();
	}
	im::SameLine();
	if( im::Button( "append cancel" ) ) {
		testAppendCancel();
	}
	im::SameLine();
	if( im::Button( "schedule" ) ) {
		testSchedule();
	}


	im::Separator();
	if( im::ListBox( "sub-tests", &mCurrentSubTest, mSubTests, mSubTests.size() ) ) {
		setupSubTest( mSubTests[mCurrentSubTest] );
	}
}
