#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/audio2/GenNode.h"
#include "cinder/audio2/GainNode.h"
#include "cinder/audio2/ChannelRouterNode.h"
#include "cinder/audio2/ScopeNode.h"
#include "cinder/audio2/CinderAssert.h"
#include "cinder/audio2/dsp/Converter.h"
#include "cinder/audio2/Debug.h"

#include "InterleavedPassThruNode.h"
#include "../../common/AudioTestGui.h"
#include "../../../samples/common/AudioDrawUtils.h"

#include "cinder/audio2/Utilities.h"

// FIXME: not all switching between tests is producing the expected graph

using namespace ci;
using namespace ci::app;
using namespace std;

class NodeTestApp : public AppNative {
public:
	void setup();
	void resize();
	void draw();

	void setupGen();
	void setup2to1();
	void setup1to2();
	void setupInterleavedPassThru();
	void setupAutoPulled();
	void setupFunnelCase();
	void setupMerge();
	void setupMerge4();
	void setupSplitStereo();
	void setupSplitMerge();

	void printDefaultOutput();
	void setupUI();
	void processDrag( Vec2i pos );
	void processTap( Vec2i pos );

	audio2::GainNodeRef			mGain;
	audio2::ScopeNodeRef	mScope;
	audio2::GenNodeRef		mGen, mNoise;

	vector<TestWidget *>	mWidgets;
	Button					mPlayButton, mEnableNoiseButton, mEnableSineButton;
	VSelector				mTestSelector;
	HSlider					mGainSlider;

	enum InputBus { SINE, NOISE };
};

void NodeTestApp::setup()
{
	printDefaultOutput();
	
	auto ctx = audio2::master();
	mGain = ctx->makeNode( new audio2::GainNode( 0.04f ) );
	mGen = ctx->makeNode( new audio2::GenSineNode( 440 ) );
	mNoise = ctx->makeNode( new audio2::GenNoiseNode() );

	mScope = audio2::master()->makeNode( new audio2::ScopeNode( audio2::ScopeNode::Format().windowSize( 2048 ) ) );

	setupGen();
//	setupMerge();
//	setupMerge4();
//	setupSplitStereo();
//	setupSplitMerge();

	ctx->printGraph();
	
	setupUI();
}

void NodeTestApp::setupGen()
{
	if( mScope )
		mScope->disconnectAll();

	mGain->disconnectAllInputs();

	mGen >> audio2::master()->getOutput();
	mGen->enable();

	mEnableNoiseButton.setEnabled( false );
	mEnableSineButton.setEnabled( true );
}

void NodeTestApp::setup2to1()
{
	if( mScope )
		mScope->disconnectAll();

	mGen >> mGain;
	mNoise >> mGain;

	mGain >> audio2::master()->getOutput();

	mGen->enable();
	mNoise->enable();

	mEnableSineButton.setEnabled( true );
	mEnableNoiseButton.setEnabled( true );
}

// note: this enables the scope as a secondary output of mGen, and as no one ever disconnects that, it harmlessly remains when the test is switched.
void NodeTestApp::setup1to2()
{
	auto ctx = audio2::master();
	ctx->disconnectAllNodes();

	mGen >> mGain >> audio2::master()->getOutput();
	mGen->enable();

	mGen->connect( mScope );

	mEnableNoiseButton.setEnabled( false );
	mEnableSineButton.setEnabled( true );
}

void NodeTestApp::setupInterleavedPassThru()
{
	if( mScope )
		mScope->disconnectAll();

	auto ctx = audio2::master();

	mGain->disconnectAllInputs();

	auto interleaved = ctx->makeNode( new InterleavedPassThruNode() );
	mGen >> interleaved >> mGain >> ctx->getOutput();
	mGen->enable();

	mEnableNoiseButton.setEnabled( false );
	mEnableSineButton.setEnabled( true );
}

void NodeTestApp::setupAutoPulled()
{
	auto ctx = audio2::master();
	ctx->disconnectAllNodes();

	mGen >> mScope;

	mGen->enable();

}

void NodeTestApp::setupFunnelCase()
{
	auto ctx = audio2::master();
	ctx->disconnectAllNodes();

	auto gain1 = ctx->makeNode( new audio2::GainNode );
	auto gain2 = ctx->makeNode( new audio2::GainNode );
//	auto gain2 = ctx->makeNode( new audio2::GainNode( audio2::Node::Format().autoEnable( false ) ) );

	mGen >> gain1 >> mScope;
	mNoise >> gain2 >> mScope;

	mScope >> mGain >> ctx->getOutput();

	mNoise->disable();
	mGen->enable();

	mEnableNoiseButton.setEnabled( false );
	mEnableSineButton.setEnabled( true );
}

void NodeTestApp::setupMerge()
{
	auto ctx = audio2::master();
	ctx->disconnectAllNodes();

	auto router = ctx->makeNode( new audio2::ChannelRouterNode( audio2::Node::Format().channels( 2 ) ) );

	mGen >> router->route( 0, 0 );
	mNoise >> router->route( 0, 1 );

	router >> mGain >> mScope >> ctx->getOutput();

	mGen->enable();
	mNoise->enable();
	mEnableNoiseButton.setEnabled( false );
	mEnableSineButton.setEnabled( true );
}

void NodeTestApp::setupMerge4()
{
	auto ctx = audio2::master();
	ctx->disconnectAllNodes();

	auto router = ctx->makeNode( new audio2::ChannelRouterNode( audio2::Node::Format().channels( 4 ) ) );

	auto upmixStereo1 = ctx->makeNode( new audio2::Node( audio2::Node::Format().channels( 2 ) ) );
	auto upmixStereo2 = ctx->makeNode( new audio2::Node( audio2::Node::Format().channels( 2 ) ) );

	mGen >> upmixStereo1 >> router->route( 0, 0 );
	mNoise >> upmixStereo2 >> router->route( 0, 2 );

	router >> mGain >> mScope >> ctx->getOutput();

	mGen->enable();
	mNoise->enable();
	mEnableNoiseButton.setEnabled( false );
	mEnableSineButton.setEnabled( true );
}

void NodeTestApp::setupSplitStereo()
{
	auto ctx = audio2::master();
	ctx->disconnectAllNodes();

	auto gainStereo = ctx->makeNode( new audio2::GainNode( audio2::Node::Format().channels( 2 ) ) );
	auto router = ctx->makeNode( new audio2::ChannelRouterNode( audio2::Node::Format().channels( 2 ) ) );

	mGen >> gainStereo >> router->route( 0, 0, 1 ) >> mGain >> mScope >> ctx->getOutput();

	mGen->enable();
	mEnableNoiseButton.setEnabled( false );
	mEnableSineButton.setEnabled( true );
}

void NodeTestApp::setupSplitMerge()
{
	auto ctx = audio2::master();
	ctx->disconnectAllNodes();

	auto upmixStereo = ctx->makeNode( new audio2::Node( audio2::Node::Format().channels( 2 ) ) );

	auto splitRouter0 = ctx->makeNode( new audio2::ChannelRouterNode( audio2::Node::Format().channels( 1 ) ) );
	auto splitRouter1 = ctx->makeNode( new audio2::ChannelRouterNode( audio2::Node::Format().channels( 1 ) ) );
	auto stereoRouter = ctx->makeNode( new audio2::ChannelRouterNode( audio2::Node::Format().channels( 2 ) ) );

	// up-mix to stereo, then split the channels into two separate Node's
	mGen >> upmixStereo;
	upmixStereo >> splitRouter0->route( 0, 0 );
	upmixStereo >> splitRouter1->route( 1, 0 );

	// merge the two split Nodes back into one stereo output
	splitRouter0 >> stereoRouter->route( 0, 0 );
	splitRouter1 >> stereoRouter->route( 0, 1 );

	stereoRouter >> mGain >> mScope >> ctx->getOutput();

	mGen->enable();
	mEnableNoiseButton.setEnabled( false );
	mEnableSineButton.setEnabled( true );
}

void NodeTestApp::printDefaultOutput()
{
	audio2::DeviceRef device = audio2::Device::getDefaultOutput();

	CI_LOG_V( "device name: " << device->getName() );
	console() << "\t input channels: " << device->getNumInputChannels() << endl;
	console() << "\t output channels: " << device->getNumOutputChannels() << endl;
	console() << "\t samplerate: " << device->getSampleRate() << endl;
	console() << "\t frames per block: " << device->getFramesPerBlock() << endl;
}

void NodeTestApp::setupUI()
{
	mPlayButton = Button( true, "stopped", "playing" );
	mPlayButton.mBounds = Rectf( 0, 0, 200, 60 );
	mWidgets.push_back( &mPlayButton );

	mTestSelector.mSegments.push_back( "sine" );
	mTestSelector.mSegments.push_back( "2 to 1" );
	mTestSelector.mSegments.push_back( "1 to 2" );
	mTestSelector.mSegments.push_back( "funnel case" );
	mTestSelector.mSegments.push_back( "interleave pass-thru" );
	mTestSelector.mSegments.push_back( "auto-pulled" );
	mTestSelector.mSegments.push_back( "merge" );
	mTestSelector.mSegments.push_back( "merge4" );
	mTestSelector.mSegments.push_back( "split stereo" );
	mTestSelector.mSegments.push_back( "split-merge" );
	mWidgets.push_back( &mTestSelector );

	mGainSlider.mTitle = "GainNode";
	mGainSlider.mMax = 1;
	mGainSlider.set( mGain->getValue() );
	mWidgets.push_back( &mGainSlider );

	mEnableSineButton.mIsToggle = true;
	mEnableSineButton.mTitleNormal = "sine disabled";
	mEnableSineButton.mTitleEnabled = "sine enabled";
	mEnableSineButton.mBounds = Rectf( 0, 70, 200, 120 );
	mWidgets.push_back( &mEnableSineButton );

	mEnableNoiseButton.mIsToggle = true;
	mEnableNoiseButton.mTitleNormal = "noise disabled";
	mEnableNoiseButton.mTitleEnabled = "noise enabled";
	mEnableNoiseButton.mBounds = mEnableSineButton.mBounds + Vec2f( 0, mEnableSineButton.mBounds.getHeight() + 10 );
	mWidgets.push_back( &mEnableNoiseButton );


	getWindow()->getSignalMouseDown().connect( [this] ( MouseEvent &event ) { processTap( event.getPos() ); } );
	getWindow()->getSignalMouseDrag().connect( [this] ( MouseEvent &event ) { processDrag( event.getPos() ); } );
	getWindow()->getSignalTouchesBegan().connect( [this] ( TouchEvent &event ) { processTap( event.getTouches().front().getPos() ); } );
	getWindow()->getSignalTouchesMoved().connect( [this] ( TouchEvent &event ) {
		for( const TouchEvent::Touch &touch : getActiveTouches() )
			processDrag( touch.getPos() );
	} );

	gl::enableAlphaBlending();
}

void NodeTestApp::processDrag( Vec2i pos )
{
	if( mGainSlider.hitTest( pos ) )
		mGain->setValue( mGainSlider.mValueScaled );
}

void NodeTestApp::processTap( Vec2i pos )
{
	auto ctx = audio2::master();

	if( mPlayButton.hitTest( pos ) )
		ctx->setEnabled( ! ctx->isEnabled() );
	if( mGen && mEnableSineButton.hitTest( pos ) )
		mGen->setEnabled( ! mGen->isEnabled() );
	if( mNoise && mEnableNoiseButton.hitTest( pos ) ) // FIXME: this check doesn't work any more because there is always an mNoise / mGen
		mNoise->setEnabled( ! mNoise->isEnabled() );

	size_t currentIndex = mTestSelector.mCurrentSectionIndex;
	if( mTestSelector.hitTest( pos ) && currentIndex != mTestSelector.mCurrentSectionIndex ) {
		string currentTest = mTestSelector.currentSection();
		CI_LOG_V( "selected: " << currentTest );

		if( currentTest == "sine" )
			setupGen();
		else if( currentTest == "2 to 1" )
			setup2to1();
		else if( currentTest == "1 to 2" )
			setup1to2();
		else if( currentTest == "interleave pass-thru" )
			setupInterleavedPassThru();
		else if( currentTest == "auto-pulled" )
			setupAutoPulled();
		else if( currentTest == "funnel case" )
			setupFunnelCase();
		else if( currentTest == "merge" )
			setupMerge();
		else if( currentTest == "merge4" )
			setupMerge4();
		else if( currentTest == "split stereo" )
			setupSplitStereo();
		else if( currentTest == "split-merge" )
			setupSplitMerge();

		ctx->printGraph();
	}
}

void NodeTestApp::resize()
{
	mTestSelector.mBounds = Rectf( (float)getWindowWidth() * 0.6f, 0, (float)getWindowWidth(), (float)getWindowHeight() * 0.6f );

	const float padding = 10;
	const float sliderHeght = 50;
	Rectf sliderRect( padding, getWindowHeight() - sliderHeght - padding, getWindowCenter().x, getWindowHeight() - padding );
	mGainSlider.mBounds = sliderRect;

}

void NodeTestApp::draw()
{
	gl::clear();

	if( mScope && mScope->getNumConnectedInputs() ) {
		Vec2f padding( 20, 20 );

		Rectf scopeRect( padding.x, padding.y, getWindowWidth() - padding.x, getWindowHeight() - padding.y );
		drawAudioBuffer( mScope->getBuffer(), scopeRect, true );
	}

	drawWidgets( mWidgets );
}

CINDER_APP_NATIVE( NodeTestApp, RendererGl )
