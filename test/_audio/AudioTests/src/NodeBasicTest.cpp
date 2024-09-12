#include "NodeBasicTest.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/CinderAssert.h"
#include "cinder/CinderImGui.h"

#include "cinder/audio/GenNode.h"
#include "cinder/audio/GainNode.h"
#include "cinder/audio/ChannelRouterNode.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/Utilities.h"

#include "InterleavedPassThruNode.h"
#include "../../../../samples/_audio/common/AudioDrawUtils.h"

using namespace ci;
using namespace std;
namespace im = ImGui;

NodeBasicTest::NodeBasicTest()
{	
	mName = "NodeTest";

	mSubTests = {
		"sine",
		"2 to 1",
		"1 to 2",
		"funnel case",
		"interleave pass-through",
		"auto-pulled",
		"merge",
		"merge4",
		"split stereo",
		"split merge"
	};

	auto ctx = audio::master();
	mGain = ctx->makeNode( new audio::GainNode( 0.04f ) );
	mGen = ctx->makeNode<audio::GenSineNode>( 440 );
	mNoise = ctx->makeNode<audio::GenNoiseNode>();

	mMonitor = audio::master()->makeNode( new audio::MonitorNode( audio::MonitorNode::Format().windowSize( 2048 ) ) );

	setupGen();
}

void NodeBasicTest::setupSubTest( const string &testName )
{
	if( testName == "sine" )
		setupGen();
	else if( testName == "2 to 1" )
		setup2to1();
	else if( testName == "1 to 2" )
		setup1to2();
	else if( testName == "funnel case" )
		setupFunnelCase();
	else if( testName == "interleave pass-through" )
		setupInterleavedPassThru();
	else if( testName == "auto-pulled" )
		setupAutoPulled();
	else if( testName == "merge" )
		setupMerge();
	else if( testName == "merge4" )
		setupMerge4();
	else if( testName == "split stereo" )
		setupSplitStereo();
	else if( testName == "split merge" )
		setupSplitMerge();

	CI_LOG_I( "Finished setup for test: " << testName );
}

void NodeBasicTest::setupGen()
{
	if( mMonitor )
		mMonitor->disconnectAll();

	mGain->disconnectAllInputs();

	//mGen >> audio::master()->getOutput();
	mGen >> mGain >> audio::master()->getOutput();
	mGen->enable();
}

void NodeBasicTest::setup2to1()
{
	if( mMonitor )
		mMonitor->disconnectAll();

	mGen >> mGain;
	mNoise >> mGain;

	mGain >> audio::master()->getOutput();

	mGen->enable();
	mNoise->enable();
}

// note: this enables the scope as a secondary output of mGen, and as no one ever disconnects that, it harmlessly remains when the test is switched.
void NodeBasicTest::setup1to2()
{
	auto ctx = audio::master();
	ctx->disconnectAllNodes();

	mGen >> mGain >> audio::master()->getOutput();
	mGen->enable();

	mGen->connect( mMonitor );
}

void NodeBasicTest::setupInterleavedPassThru()
{
	if( mMonitor )
		mMonitor->disconnectAll();

	auto ctx = audio::master();
	ctx->disconnectAllNodes();

	auto interleaved = ctx->makeNode( new InterleavedPassThruNode() );
	mGen >> interleaved >> mGain >> ctx->getOutput();
	mGen->enable();
}

void NodeBasicTest::setupAutoPulled()
{
	auto ctx = audio::master();
	ctx->disconnectAllNodes();

	mGen >> mMonitor;
	mGen->enable();
}

void NodeBasicTest::setupFunnelCase()
{
	auto ctx = audio::master();
	ctx->disconnectAllNodes();

	auto gain1 = ctx->makeNode( new audio::GainNode );
	auto gain2 = ctx->makeNode( new audio::GainNode );
//	auto gain2 = ctx->makeNode( new audio::GainNode( audio::Node::Format().autoEnable( false ) ) );

	mGen >> gain1 >> mMonitor;
	mNoise >> gain2 >> mMonitor;

	mMonitor >> mGain >> ctx->getOutput();

	mNoise->disable();
	mGen->enable();
}

void NodeBasicTest::setupMerge()
{
	auto ctx = audio::master();
	ctx->disconnectAllNodes();

	auto router = ctx->makeNode( new audio::ChannelRouterNode( audio::Node::Format().channels( 2 ) ) );

	mGen >> router->route( 0, 0 );
	mNoise >> router->route( 0, 1 );

	router >> mGain >> mMonitor >> ctx->getOutput();

	mGen->enable();
	mNoise->enable();
}

void NodeBasicTest::setupMerge4()
{
	auto ctx = audio::master();
	ctx->disconnectAllNodes();

	auto router = ctx->makeNode( new audio::ChannelRouterNode( audio::Node::Format().channels( 4 ) ) );

	auto upmixStereo1 = ctx->makeNode( new audio::Node( audio::Node::Format().channels( 2 ) ) );
	auto upmixStereo2 = ctx->makeNode( new audio::Node( audio::Node::Format().channels( 2 ) ) );

	mGen >> upmixStereo1 >> router->route( 0, 0 );
	mNoise >> upmixStereo2 >> router->route( 0, 2 );

	router >> mGain >> mMonitor >> ctx->getOutput();

	mGen->enable();
	mNoise->enable();
}

void NodeBasicTest::setupSplitStereo()
{
	auto ctx = audio::master();
	ctx->disconnectAllNodes();

	auto gainStereo = ctx->makeNode( new audio::GainNode( audio::Node::Format().channels( 2 ) ) );
	auto router = ctx->makeNode( new audio::ChannelRouterNode( audio::Node::Format().channels( 2 ) ) );

	mGen >> gainStereo >> router->route( 0, 0, 1 ) >> mGain >> mMonitor >> ctx->getOutput();

	mGen->enable();
}

void NodeBasicTest::setupSplitMerge()
{
	auto ctx = audio::master();
	ctx->disconnectAllNodes();

	auto upmixStereo = ctx->makeNode( new audio::Node( audio::Node::Format().channels( 2 ) ) );

	auto splitRouter0 = ctx->makeNode( new audio::ChannelRouterNode( audio::Node::Format().channels( 1 ) ) );
	auto splitRouter1 = ctx->makeNode( new audio::ChannelRouterNode( audio::Node::Format().channels( 1 ) ) );
	auto stereoRouter = ctx->makeNode( new audio::ChannelRouterNode( audio::Node::Format().channels( 2 ) ) );

	// up-mix to stereo, then split the channels into two separate Node's
	mGen >> upmixStereo;
	upmixStereo >> splitRouter0->route( 0, 0 );
	upmixStereo >> splitRouter1->route( 1, 0 );

	// merge the two split Nodes back into one stereo output
	splitRouter0 >> stereoRouter->route( 0, 0 );
	splitRouter1 >> stereoRouter->route( 0, 1 );

	stereoRouter >> mGain >> mMonitor >> ctx->getOutput();

	mGen->enable();
}

void NodeBasicTest::draw()
{
	if( mMonitor && mMonitor->getNumConnectedInputs() ) {
		vec2 padding( 20, 20 );

		Rectf scopeRect( padding.x, padding.y, app::getWindowWidth() - padding.x, app::getWindowHeight() - padding.y );
		drawAudioBuffer( mMonitor->getBuffer(), scopeRect, true );
	}
}

// -----------------------------------------------------------------------
// ImGui
// -----------------------------------------------------------------------

void NodeBasicTest::updateUI()
{
	float gain = mGain->getValue();
	if( im::SliderFloat( "gain", &gain, 0, 1 ) ) {
		mGain->setValue( gain );
	}

	bool sineEnabled = mGen->isEnabled();
	if( im::Checkbox( "sine", &sineEnabled ) ) {
		mGen->setEnabled( sineEnabled );
	}
	im::SameLine();
	bool noiseEnabled = mNoise->isEnabled();
	if( im::Checkbox( "noise", &noiseEnabled ) ) {
		mNoise->setEnabled( noiseEnabled );
	}

	static float delaySeconds = 1.0f;
	if( im::Button( "delayed enable / disable" ) ) {
		mGen->setEnabled( ! mGen->isEnabled(), audio::master()->getNumProcessedSeconds() + (double)delaySeconds );
	}
	im::SameLine();
	im::InputFloat( "seconds", &delaySeconds, 0.5f, 5.0f );

	if( im::ListBox( "sub-tests", &mCurrentSubTest, mSubTests, mSubTests.size() ) ) {
		setupSubTest( mSubTests[mCurrentSubTest] );
	}
}