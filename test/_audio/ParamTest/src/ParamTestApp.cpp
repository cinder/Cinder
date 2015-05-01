#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/audio/FilterNode.h"
#include "cinder/audio/Target.h"

#include "../../common/AudioTestGui.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ParamTestApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyDown( KeyEvent event ) override;

	void setupBasic();
	void setupFilter();

	void setupUI();
	void processDrag( ivec2 pos );
	void processTap( ivec2 pos );

	void testApply();
	void testApply0();
	void testApply2();
	void testAppend();
	void testDelay();
	void testAppendCancel();
	void testProcessor();
	void testSchedule();

	void writeParamEval( audio::Param *param );

	audio::GenNodeRef				mGen;
	audio::GainNodeRef				mGain;
	audio::Pan2dNodeRef				mPan;
	audio::FilterLowPassNodeRef		mLowPass;

	vector<TestWidget *>	mWidgets;
	Button					mPlayButton, mApplyButton, mApply0Button, mApplyAppendButton, mAppendButton;
	Button					mDelayButton, mProcessorButton, mAppendCancelButton, mScheduleButton;
	VSelector				mTestSelector;
	HSlider					mGainSlider, mPanSlider, mLowPassFreqSlider, mGenFreqSlider;
};

void ParamTestApp::setup()
{
	auto ctx = audio::master();
	mGain = ctx->makeNode( new audio::GainNode() );
	mGain->setValue( 0.8f );

	mPan = ctx->makeNode( new audio::Pan2dNode() );

	mGen = ctx->makeNode( new audio::GenSineNode() );
//	mGen = ctx->makeNode( new audio::GenTriangleNode() );
//	mGen = ctx->makeNode( new audio::GenPhasorNode() );
	mGen = ctx->makeNode( new audio::GenPulseNode );

	mGen->setFreq( 220 );

	mLowPass = ctx->makeNode( new audio::FilterLowPassNode() );

	setupBasic();

	setupUI();

	PRINT_GRAPH( ctx );

	testApply();
//	testApply2();
//	connectProcessor();
}

void ParamTestApp::setupBasic()
{
	mGen >> mGain >> audio::master()->getOutput();
	mGen->enable();
}

void ParamTestApp::setupFilter()
{
	mGen >> mLowPass >> mGain >> mPan >> audio::master()->getOutput();
	mGen->enable();
}

void ParamTestApp::testApply()
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
void ParamTestApp::testApply0()
{
	mGen->getParamFreq()->applyRamp( 220, 440, 0 );

	CI_LOG_V( "num events: " << mGen->getParamFreq()->getNumEvents() );
}

// 2 events - first apply the ramp, blowing away anything else, then append another event to happen after that
void ParamTestApp::testApply2()
{
	mGen->getParamFreq()->applyRamp( 220, 880, 1 );
	mGen->getParamFreq()->appendRamp( 369.994f, 1 ); // F#4

	CI_LOG_V( "num events: " << mGen->getParamFreq()->getNumEvents() );

//	writeParamEval( mGen->getParamFreq() );
}

// append an event with random frequency and duration 1 second, allowing them to build up. new events begin from the end of the last event
void ParamTestApp::testAppend()
{
	mGen->getParamFreq()->appendRamp( randFloat( 50, 800 ), 1.0f );

	CI_LOG_V( "num events: " << mGen->getParamFreq()->getNumEvents() );
}

// make a ramp after a 1 second delay
void ParamTestApp::testDelay()
{
	mGen->getParamFreq()->applyRamp( 50, 440, 1, audio::Param::Options().delay( 1 ) );
	CI_LOG_V( "num events: " << mGen->getParamFreq()->getNumEvents() );
}

// apply a ramp from 220 to 880 over 2 seconds and then after a 1 second delay, cancel it. result should be ~ 550: 220 + (880 - 220) / 2.
void ParamTestApp::testAppendCancel()
{
	audio::EventRef ramp = mGen->getParamFreq()->applyRamp( 220, 880, 2 );

	CI_LOG_V( "num events: " << mGen->getParamFreq()->getNumEvents() );

	timeline().add( [ramp] {
		CI_LOG_V( "canceling." );
		ramp->cancel();
	}, (float)getElapsedSeconds() + 1 );
}

void ParamTestApp::testProcessor()
{
	auto ctx = audio::master();
	auto mod = ctx->makeNode( new audio::GenSineNode( audio::Node::Format().autoEnable() ) );
	mod->setFreq( 2 );

	mGain->getParam()->setProcessor( mod );
}

void ParamTestApp::testSchedule()
{
	bool enabled = mGen->isEnabled();
	mGen->setEnabled( ! enabled, audio::master()->getNumProcessedSeconds() + 0.5f );
}

void ParamTestApp::setupUI()
{
	const float padding = 10.0f;

	mPlayButton = Button( true, "stopped", "playing" );
	mPlayButton.mBounds = Rectf( 0, 0, 200, 60 );
	mWidgets.push_back( &mPlayButton );

	Rectf paramButtonRect( 0, mPlayButton.mBounds.y2 + padding, 120, mPlayButton.mBounds.y2 + padding + 40 );
	mApplyButton = Button( false, "apply" );
	mApplyButton.mBounds = paramButtonRect;
	mWidgets.push_back( &mApplyButton );

	paramButtonRect += vec2( paramButtonRect.getWidth() + padding, 0 );
	mApply0Button = Button( false, "apply0" );
	mApply0Button.mBounds = paramButtonRect;
	mWidgets.push_back( &mApply0Button );

	paramButtonRect += vec2( paramButtonRect.getWidth() + padding, 0 );
	mApplyAppendButton = Button( false, "apply 2" );
	mApplyAppendButton.mBounds = paramButtonRect;
	mWidgets.push_back( &mApplyAppendButton );

	paramButtonRect += vec2( paramButtonRect.getWidth() + padding, 0 );
	mAppendButton = Button( false, "append" );
	mAppendButton.mBounds = paramButtonRect;
	mWidgets.push_back( &mAppendButton );

	paramButtonRect = mApplyButton.mBounds + vec2( 0, mApplyButton.mBounds.getHeight() + padding );
	mDelayButton = Button( false, "delay" );
	mDelayButton.mBounds = paramButtonRect;
	mWidgets.push_back( &mDelayButton );

	paramButtonRect += vec2( paramButtonRect.getWidth() + padding, 0 );
	mProcessorButton = Button( false, "processor" );
	mProcessorButton.mBounds = paramButtonRect;
	mWidgets.push_back( &mProcessorButton );

	paramButtonRect += vec2( paramButtonRect.getWidth() + padding, 0 );
	mAppendCancelButton = Button( false, "cancel" );
	mAppendCancelButton.mBounds = paramButtonRect;
	mWidgets.push_back( &mAppendCancelButton );

	paramButtonRect += vec2( paramButtonRect.getWidth() + padding, 0 );
	mScheduleButton = Button( false, "schedule" );
	mScheduleButton.mBounds = paramButtonRect;
	mWidgets.push_back( &mScheduleButton );

	mTestSelector.mSegments.push_back( "basic" );
	mTestSelector.mSegments.push_back( "filter" );
	mTestSelector.mBounds = Rectf( (float)getWindowWidth() * 0.67f, 0, (float)getWindowWidth(), 160 );
	mWidgets.push_back( &mTestSelector );

	float width = std::min( (float)getWindowWidth() - 20.0f,  440.0f );
	Rectf sliderRect( getWindowCenter().x - width / 2.0f, 200, getWindowCenter().x + width / 2.0f, 250 );
	mGainSlider.mBounds = sliderRect;
	mGainSlider.mTitle = "GainNode";
	mGainSlider.set( mGain->getValue() );
	mWidgets.push_back( &mGainSlider );

	sliderRect += vec2( 0.0f, sliderRect.getHeight() + 10.0f );
	mPanSlider.mBounds = sliderRect;
	mPanSlider.mTitle = "Pan";
	mPanSlider.set( mPan->getPos() );
	mWidgets.push_back( &mPanSlider );

	sliderRect += vec2( 0.0f, sliderRect.getHeight() + 10.0f );
	mGenFreqSlider.mBounds = sliderRect;
	mGenFreqSlider.mTitle = "Gen Freq";
	mGenFreqSlider.mMin = -200.0f;
	mGenFreqSlider.mMax = 1200.0f;
	mGenFreqSlider.set( mGen->getFreq() );
	mWidgets.push_back( &mGenFreqSlider );

	sliderRect += vec2( 0.0f, sliderRect.getHeight() + 10.0f );
	mLowPassFreqSlider.mBounds = sliderRect;
	mLowPassFreqSlider.mTitle = "LowPass Freq";
	mLowPassFreqSlider.mMax = 1000.0f;
	mLowPassFreqSlider.set( mLowPass->getCutoffFreq() );
	mWidgets.push_back( &mLowPassFreqSlider );

	getWindow()->getSignalMouseDown().connect( [this] ( MouseEvent &event ) { processTap( event.getPos() ); } );
	getWindow()->getSignalMouseDrag().connect( [this] ( MouseEvent &event ) { processDrag( event.getPos() ); } );
	getWindow()->getSignalTouchesBegan().connect( [this] ( TouchEvent &event ) { processTap( event.getTouches().front().getPos() ); } );
	getWindow()->getSignalTouchesMoved().connect( [this] ( TouchEvent &event ) {
		for( const TouchEvent::Touch &touch : getActiveTouches() )
			processDrag( touch.getPos() );
	} );

	gl::enableAlphaBlending();
}

void ParamTestApp::processDrag( ivec2 pos )
{
	if( mGainSlider.hitTest( pos ) ) {
//		mGain->setValue( mGainSlider.mValueScaled );
//		mGain->getParam()->applyRamp( mGainSlider.mValueScaled );
		mGain->getParam()->applyRamp( mGainSlider.mValueScaled, 0.15f );
	}
	if( mPanSlider.hitTest( pos ) ) {
//		mPan->setPos( mPanSlider.mValueScaled );
		mPan->getParamPos()->applyRamp( mPanSlider.mValueScaled, 0.3f, audio::Param::Options().rampFn( &audio::rampOutQuad ) );
	}
	if( mGenFreqSlider.hitTest( pos ) ) {
//		mGen->setFreq( mGenFreqSlider.mValueScaled );
//		mGen->getParamFreq()->applyRamp( mGenFreqSlider.mValueScaled, 0.3f );
		mGen->getParamFreq()->applyRamp( mGenFreqSlider.mValueScaled, 0.3f, audio::Param::Options().rampFn( &audio::rampOutQuad ) );
	}
	if( mLowPassFreqSlider.hitTest( pos ) )
		mLowPass->setCutoffFreq( mLowPassFreqSlider.mValueScaled );
}

void ParamTestApp::processTap( ivec2 pos )
{
	auto ctx = audio::master();
	size_t selectorIndex = mTestSelector.mCurrentSectionIndex;

	if( mPlayButton.hitTest( pos ) )
		ctx->setEnabled( ! ctx->isEnabled() );
	else if( mApplyButton.hitTest( pos ) )
		testApply();
	else if( mApply0Button.hitTest( pos ) )
		testApply0();
	else if( mApplyAppendButton.hitTest( pos ) )
		testApply2();
	else if( mAppendButton.hitTest( pos ) )
		testAppend();
	else if( mDelayButton.hitTest( pos ) )
		testDelay();
	else if( mProcessorButton.hitTest( pos ) )
		testProcessor();
	else if( mAppendCancelButton.hitTest( pos ) )
		testAppendCancel();
	else if( mScheduleButton.hitTest( pos ) )
		testSchedule();
	else if( mTestSelector.hitTest( pos ) && selectorIndex != mTestSelector.mCurrentSectionIndex ) {
		string currentTest = mTestSelector.currentSection();
		CI_LOG_V( "selected: " << currentTest );

		bool enabled = ctx->isEnabled();
		ctx->disable();

		ctx->disconnectAllNodes();

		if( currentTest == "basic" )
			setupBasic();
		if( currentTest == "filter" )
			setupFilter();

		ctx->setEnabled( enabled );
		PRINT_GRAPH( ctx );
	}
	else
		processDrag( pos );
}

void ParamTestApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_e )
		CI_LOG_V( "mGen freq events: " << mGen->getParamFreq()->getNumEvents() );
}

void ParamTestApp::update()
{
	if( audio::master()->isEnabled() ) {
		mGainSlider.set( mGain->getValue() );
		mGenFreqSlider.set( mGen->getFreq() );
		mPanSlider.set( mPan->getPos() );
	}
}

void ParamTestApp::draw()
{
	gl::clear();
	drawWidgets( mWidgets );
}

// TODO: this will be formalized once there is an offline audio context and OutputFileNode.
void ParamTestApp::writeParamEval( audio::Param *param )
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

CINDER_APP( ParamTestApp, RendererGl, []( App::Settings *settings ) {
	settings->setWindowSize( 800, 600 );
} )
