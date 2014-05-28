#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/CinderAssert.h"
#include "cinder/audio/Debug.h"

#include "../../common/AudioTestGui.h"

#define TEST_LOW_LATENCY 0

using namespace ci;
using namespace ci::app;
using namespace std;

class NodeEffectsTestApp : public AppNative {
  public:
	void setup();
	void update();
	void draw();

	void setupOne();
	void setupForceStereo();
	void setupDownMix();
	void setupDelay();
	void setupVariableDelay();
	void setupFeedback();
	void setupEcho();
	void setupCycle();

	void makeNodes();
	void switchTest( const string &currentTest );
	void applyChirp();

	void setupUI();
	void processDrag( Vec2i pos );
	void processTap( Vec2i pos );

	audio::GenNodeRef				mGen;
	audio::GainNodeRef				mGain;
	audio::Pan2dNodeRef			mPan;
	audio::FilterLowPassNodeRef	mLowPass;
	audio::DelayNodeRef			mDelay;

	vector<TestWidget *>	mWidgets;
	Button					mPlayButton, mGenButton, mGenEnabledButton, mChirpButton;
	VSelector				mTestSelector;
	HSlider					mGainSlider, mPanSlider, mLowPassFreqSlider, mFilterParam2Slider;
};

void NodeEffectsTestApp::setup()
{
	auto ctx = audio::master();

//	auto outputDevice = ctx->getOutput()->getDevice();

#if TEST_LOW_LATENCY
	auto lineOut = ctx->createOutputDeviceNode();
	lineOut->getDevice()->updateFormat( audio::Device::Format().framesPerBlock( 64 ) );
	ctx->setOutput( lineOut );
#endif

//	mGenButton.setEnabled( true ); // set to start with GenSineNode

	makeNodes();
	setupOne();
	setupDelay();
	//setupFeedback();
	//setupEcho();

	setupUI();

	PRINT_GRAPH( ctx );
	CI_LOG_V( "Context samplerate: " << ctx->getSampleRate() << ", frames per block: " << ctx->getFramesPerBlock() );
}

void NodeEffectsTestApp::makeNodes()
{
	auto ctx = audio::master();

	mGain = ctx->makeNode( new audio::GainNode );
	mGain->setValue( 0.25f );

	mPan = ctx->makeNode( new audio::Pan2dNode );

	CI_LOG_V( "gen button enabled: " << mGenButton.mEnabled );
	auto genFmt = audio::Node::Format().autoEnable();
	if( mGenButton.mEnabled )
		mGen = ctx->makeNode( new audio::GenSineNode( 220, genFmt ) );
	else
		mGen = ctx->makeNode( new audio::GenNoiseNode( genFmt ) );

	mLowPass = ctx->makeNode( new audio::FilterLowPassNode() );
	mLowPass->setCutoffFreq( 400 );
//	mLowPass = ctx->makeNode( new audio::FilterHighPassNode() );

	mDelay = ctx->makeNode( new audio::DelayNode );
	mDelay->setDelaySeconds( 0.5f );
//	mDelay->setDelaySeconds( 100.0f / (float)ctx->getSampleRate() );
}

void NodeEffectsTestApp::setupOne()
{
	mGen >> mLowPass >> mGain >> mPan >> audio::master()->getOutput();
}

// TODO: move to NodeTest
void NodeEffectsTestApp::setupForceStereo()
{
	mGen >> mLowPass >> mGain >> mPan >> audio::master()->getOutput();
}

// TODO: move to NodeTest
void NodeEffectsTestApp::setupDownMix()
{
	auto ctx = audio::master();
	auto mono = ctx->makeNode( new audio::GainNode( audio::Node::Format().channels( 1 ) ) );
	mGen >> mLowPass >> mGain >> mPan >> mono >> ctx->getOutput();
}

void NodeEffectsTestApp::setupDelay()
{
	mGen >> mGain >> mDelay >> audio::master()->getOutput();
}

void NodeEffectsTestApp::setupVariableDelay()
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

void NodeEffectsTestApp::setupFeedback()
{
	// delay + feedback
	auto ctx = audio::master();

	auto feedbackGain = ctx->makeNode( new audio::GainNode( 0.5f ) );
	feedbackGain->setName( "FeedbackGain" );

	mGen >> mDelay >> feedbackGain >> mDelay >> ctx->getOutput();
}

void NodeEffectsTestApp::setupEcho()
{
	// a more complex feedback graph, but more accurate to what you'd use in the real world
	// - sends dry signal to output so you hear it immediately.

	auto feedbackGain = audio::master()->makeNode( new audio::GainNode( 0.5f ) );
	feedbackGain->setName( "FeedbackGain" );

	mGen >> mGain;

	mGain >> audio::master()->getOutput();										// dry
	mGain >> mDelay >> feedbackGain >> mDelay >> audio::master()->getOutput(); // wet
}

void NodeEffectsTestApp::setupCycle()
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

void NodeEffectsTestApp::applyChirp()
{
	mGen->getParamFreq()->applyRamp( 440, 220, 0.15f );
}

void NodeEffectsTestApp::setupUI()
{
	const float padding = 8;
	Rectf buttonRect( 0, 0, 200, 50 );

	mPlayButton = Button( true, "stopped", "playing" );
	mPlayButton.mBounds = buttonRect;
	mWidgets.push_back( &mPlayButton );

	buttonRect += Vec2f( 0, buttonRect.getHeight() + padding );
	mGenButton.mIsToggle = true;
	mGenButton.mTitleNormal = "noise";
	mGenButton.mTitleEnabled = "sine";
	mGenButton.mBounds = buttonRect;
	mWidgets.push_back( &mGenButton );

	buttonRect += Vec2f( 0, buttonRect.getHeight() + padding );
	mGenEnabledButton.mIsToggle = true;
	mGenEnabledButton.mTitleNormal = "gen disabled";
	mGenEnabledButton.mTitleEnabled = "gen enabled";
	mGenEnabledButton.setEnabled( true );
	mGenEnabledButton.mBounds = buttonRect;
	mWidgets.push_back( &mGenEnabledButton );

	buttonRect += Vec2f( 0, buttonRect.getHeight() + padding );
	mChirpButton = Button( false, "chirp" );
	mChirpButton.mBounds = buttonRect;
	mWidgets.push_back( &mChirpButton );

	mTestSelector.mSegments.push_back( "one" );
	mTestSelector.mSegments.push_back( "force stereo" );
	mTestSelector.mSegments.push_back( "down-mix" );
	mTestSelector.mSegments.push_back( "delay" );
	mTestSelector.mSegments.push_back( "variable delay" );
	mTestSelector.mSegments.push_back( "feedback" );
	mTestSelector.mSegments.push_back( "echo" );
	mTestSelector.mSegments.push_back( "cycle" );
	mTestSelector.mBounds = Rectf( (float)getWindowWidth() * 0.67f, 0, (float)getWindowWidth(), 200 );
	mWidgets.push_back( &mTestSelector );

	float width = std::min( (float)getWindowWidth() - 20,  440.0f );
	Rectf sliderRect( getWindowCenter().x - width / 2, 250, getWindowCenter().x + width / 2, 300 );
	mGainSlider.mBounds = sliderRect;
	mGainSlider.mTitle = "GainNode";
	mGainSlider.set( mGain->getValue() );
	mWidgets.push_back( &mGainSlider );

	sliderRect += Vec2f( 0, sliderRect.getHeight() + padding );
	mPanSlider.mBounds = sliderRect;
	mPanSlider.mTitle = "Pan";
	mPanSlider.set( mPan->getPos() );
	mWidgets.push_back( &mPanSlider );

	sliderRect += Vec2f( 0, sliderRect.getHeight() + padding );
	mLowPassFreqSlider.mBounds = sliderRect;
	mLowPassFreqSlider.mTitle = "LowPass Freq";
	mLowPassFreqSlider.mMax = 1000;
	mLowPassFreqSlider.set( mLowPass->getCutoffFreq() );
	mWidgets.push_back( &mLowPassFreqSlider );

	sliderRect += Vec2f( 0, sliderRect.getHeight() + padding );
	mFilterParam2Slider.mBounds = sliderRect;
	mFilterParam2Slider.mTitle = "filter resonance";
	mFilterParam2Slider.mMax = 50;
	mFilterParam2Slider.set( mLowPass->getResonance() );
	mWidgets.push_back( &mFilterParam2Slider );

	getWindow()->getSignalMouseDown().connect( [this] ( MouseEvent &event ) { processTap( event.getPos() ); } );
	getWindow()->getSignalMouseDrag().connect( [this] ( MouseEvent &event ) { processDrag( event.getPos() ); } );
	getWindow()->getSignalTouchesBegan().connect( [this] ( TouchEvent &event ) { processTap( event.getTouches().front().getPos() ); } );
	getWindow()->getSignalTouchesMoved().connect( [this] ( TouchEvent &event ) {
		for( const TouchEvent::Touch &touch : getActiveTouches() )
			processDrag( touch.getPos() );
	} );

	gl::enableAlphaBlending();
}

void NodeEffectsTestApp::processDrag( Vec2i pos )
{
	if( mGainSlider.hitTest( pos ) )
		mGain->getParam()->applyRamp( mGainSlider.mValueScaled, 0.015f );
	if( mPanSlider.hitTest( pos ) )
		mPan->setPos( mPanSlider.mValueScaled );
	if( mLowPassFreqSlider.hitTest( pos ) )
		mLowPass->setCutoffFreq( mLowPassFreqSlider.mValueScaled );
	if( mFilterParam2Slider.hitTest( pos ) )
		mLowPass->setResonance( mFilterParam2Slider.mValueScaled );
}

void NodeEffectsTestApp::processTap( Vec2i pos )
{
	auto ctx = audio::master();

	if( mPlayButton.hitTest( pos ) )
		ctx->setEnabled( ! ctx->isEnabled() );
	else if( mGenButton.hitTest( pos ) ) {
		makeNodes();
		switchTest( mTestSelector.currentSection() );

		if( mGenButton.mEnabled )
			applyChirp();
	}
	else if( mGenEnabledButton.hitTest( pos ) )
		mGen->setEnabled( mGenEnabledButton.mEnabled );
	else if( mChirpButton.hitTest( pos ) )
		applyChirp();


	size_t currentIndex = mTestSelector.mCurrentSectionIndex;
	if( mTestSelector.hitTest( pos ) && currentIndex != mTestSelector.mCurrentSectionIndex ) {
		string currentTest = mTestSelector.currentSection();
		CI_LOG_V( "selected: " << currentTest );
		switchTest( currentTest );
	}

	processDrag( pos );
}

void NodeEffectsTestApp::switchTest( const string &currentTest )
{
	auto ctx = audio::master();

	bool enabled = ctx->isEnabled();
	ctx->disable();

	ctx->disconnectAllNodes();

	if( currentTest == "one" )
		setupOne();
	else if( currentTest == "force stereo" )
		setupForceStereo();
	else if( currentTest == "down-mix" )
		setupDownMix();
	else if( currentTest == "delay" )
		setupDelay();
	else if( currentTest == "variable delay" )
		setupVariableDelay();
	else if( currentTest == "feedback" )
		setupFeedback();
	else if( currentTest == "echo" )
		setupEcho();
	else if( currentTest == "cycle" )
		setupCycle();

	ctx->setEnabled( enabled );
	PRINT_GRAPH( ctx );
}

void NodeEffectsTestApp::update()
{
	if( mDelay->getParamDelaySeconds()->getNumEvents() )
		CI_LOG_V( "ramping delay seconds val: " << mDelay->getParamDelaySeconds()->getValue() );
}

void NodeEffectsTestApp::draw()
{
	gl::clear();
	drawWidgets( mWidgets );
}

CINDER_APP_NATIVE( NodeEffectsTestApp, RendererGl )
