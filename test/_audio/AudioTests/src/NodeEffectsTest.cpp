#include "NodeEffectsTest.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/CinderImGui.h"
#include "cinder/audio/Context.h"

using namespace ci;
using namespace std;
namespace im = ImGui;


NodeEffectsTest::NodeEffectsTest()
{	
	mSubTests = {
		"one",
		"force stereo",
		"down-mix",
		"delay",
		"variable delay",
		"feedback",
		"echo",
		"cycle"
	};

	//mUseSine = true; // set to start with GenSineNode

	makeNodes();
	setupOne();
	setupDelay();
	//setupFeedback();
	//setupEcho();
}


void NodeEffectsTest::setupSubTest( const string &testName )
{
	auto ctx = audio::master();

	bool enabled = ctx->isEnabled();
	ctx->disable();
	ctx->disconnectAllNodes();

	if( testName == "one" )
		setupOne();
	else if( testName == "force stereo" )
		setupForceStereo();
	else if( testName == "down-mix" )
		setupDownMix();
	else if( testName == "delay" )
		setupDelay();
	else if( testName == "variable delay" )
		setupVariableDelay();
	else if( testName == "feedback" )
		setupFeedback();
	else if( testName == "echo" )
		setupEcho();
	else if( testName == "cycle" )
		setupCycle();

	ctx->setEnabled( enabled );

	CI_LOG_I( "Finished setup for test: " << testName );
}

void NodeEffectsTest::makeNodes()
{
	auto ctx = audio::master();

	mGain = ctx->makeNode( new audio::GainNode );
	mGain->setValue( 0.25f );

	mPan = ctx->makeNode( new audio::Pan2dNode );

	auto genFmt = audio::Node::Format().autoEnable();

	if( mUseSine ) {
		mGen = ctx->makeNode( new audio::GenSineNode( 220, genFmt ) );
	}
	else {
		mGen = ctx->makeNode( new audio::GenNoiseNode( genFmt ) );
	}

	mLowPass = ctx->makeNode( new audio::FilterLowPassNode() );
	mLowPass->setCutoffFreq( 400 );
	//	mLowPass = ctx->makeNode( new audio::FilterHighPassNode() );

	mDelay = ctx->makeNode( new audio::DelayNode );
	mDelay->setDelaySeconds( 0.5f );
	//	mDelay->setDelaySeconds( 100.0f / (float)ctx->getSampleRate() );
}

void NodeEffectsTest::setupOne()
{
	mGen >> mLowPass >> mGain >> mPan >> audio::master()->getOutput();
}

// TODO: move to NodeTest
void NodeEffectsTest::setupForceStereo()
{
	mGen >> mLowPass >> mGain >> mPan >> audio::master()->getOutput();
}

// TODO: move to NodeTest
void NodeEffectsTest::setupDownMix()
{
	auto ctx = audio::master();
	auto mono = ctx->makeNode( new audio::GainNode( audio::Node::Format().channels( 1 ) ) );
	mGen >> mLowPass >> mGain >> mPan >> mono >> ctx->getOutput();
}

void NodeEffectsTest::setupDelay()
{
	mGen >> mGain >> mDelay >> audio::master()->getOutput();
}

void NodeEffectsTest::setupVariableDelay()
{
#if TEST_LOW_LATENCY
	float maxDelaySeconds = 20.0f / (float)audio::master()->getSampleRate();
	mDelay->setMaxDelaySeconds( maxDelaySeconds );
#else
	mDelay->setMaxDelaySeconds( 1.0f );
#endif

	auto ctx = audio::master();

	auto modGen = ctx->makeNode( new audio::GenSineNode( 0.3f, audio::Node::Format().autoEnable() ) );
	auto modMul = ctx->makeNode( new audio::GainNode( 0.1f ) );
	auto modAdd = ctx->makeNode( new audio::AddNode( 0.5f ) );

	modGen >> modMul >> modAdd;
	mDelay->getParamDelaySeconds()->setProcessor( modAdd );

	mGen >> mGain >> mDelay >> audio::master()->getOutput();
}

void NodeEffectsTest::setupFeedback()
{
	// delay + feedback
	auto ctx = audio::master();

	auto feedbackGain = ctx->makeNode( new audio::GainNode( 0.5f ) );
	feedbackGain->setName( "FeedbackGain" );

	mGen >> mDelay >> feedbackGain >> mDelay >> ctx->getOutput();
}

void NodeEffectsTest::setupEcho()
{
	// a more complex feedback graph, but more accurate to what you'd use in the real world
	// - sends dry signal to output so you hear it immediately.

	auto feedbackGain = audio::master()->makeNode( new audio::GainNode( 0.5f ) );
	feedbackGain->setName( "FeedbackGain" );

	mGen >> mGain;

	mGain >> audio::master()->getOutput();										// dry
	mGain >> mDelay >> feedbackGain >> mDelay >> audio::master()->getOutput(); // wet
}

void NodeEffectsTest::setupCycle()
{
	// this throws NodeCycleExc

	try {
		mGen >> mLowPass >> mGain >> mLowPass >> audio::master()->getOutput();
		CI_ASSERT_NOT_REACHABLE();
	}
	catch( audio::NodeCycleExc &exc ) {
		CI_LOG_E( "audio::NodeCycleExc, what: " << exc.what() );
	}
}

void NodeEffectsTest::applyChirp()
{
	mGen->getParamFreq()->applyRamp( 440, 220, 0.15f );
}

void NodeEffectsTest::update()
{
	if( mDelay->getParamDelaySeconds()->getNumEvents() ) {
		CI_LOG_I( "ramping delay seconds val: " << mDelay->getParamDelaySeconds()->getValue() );
	}
}

// -----------------------------------------------------------------------
// UI
// -----------------------------------------------------------------------

void NodeEffectsTest::updateUI()
{
	float gain = mGain->getValue();
	if( im::SliderFloat( "gain", &gain, 0, 1 ) ) {
		mGain->setValue( gain );
	}

	if( im::Checkbox( "use sine", &mUseSine ) ) {
		makeNodes();
		setupSubTest( mSubTests[mCurrentSubTest] );
	}

	bool genEnabled = mGen->isEnabled();
	if( im::Checkbox( "Gen enabled", &genEnabled ) ) {
		mGen->setEnabled( genEnabled );
	}

	if( im::Button( "chirp" ) ) {
		applyChirp();
	}

	if( im::ListBox( "sub-tests", &mCurrentSubTest, mSubTests, mSubTests.size() ) ) {
		setupSubTest( mSubTests[mCurrentSubTest] );
	}
}