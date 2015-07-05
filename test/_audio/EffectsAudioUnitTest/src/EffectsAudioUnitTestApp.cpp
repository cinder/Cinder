#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Log.h"
#include "cinder/audio/GenNode.h"
#include "cinder/CinderAssert.h"

#include "cinder/audio/cocoa/ContextAudioUnit.h"

#include "../../common/AudioTestGui.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace audio::cocoa;

class EffectsAudioUnitTestApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;

	void setupOne();
	void setupTwo();
	void setupNativeThenGeneric();

	void setupUI();
	void processDrag( ivec2 pos );
	void processTap( ivec2 pos );
	void initParams();

	audio::GenNodeRef mGen;

	shared_ptr<EffectAudioUnitNode> mEffect, mEffect2;

	VSelector mTestSelector;
	Button mPlayButton;
	HSlider mGenFreqSlider, mLowpassCutoffSlider, mBandpassSlider;
};

void EffectsAudioUnitTestApp::setup()
{
	auto ctx = audio::master();

	auto gen = ctx->makeNode( new audio::GenPulseNode( 100 ) );
	gen->setAutoEnabled();

	mGen = gen;

	setupOne();

	setupUI();
	initParams();

	PRINT_GRAPH( ctx );
}

void EffectsAudioUnitTestApp::setupOne()
{
	auto ctx = audio::master();

	mEffect = ctx->makeNode( new audio::cocoa::EffectAudioUnitNode( kAudioUnitSubType_LowPassFilter ) );
	mGen >> mEffect >> ctx->getOutput();

	mBandpassSlider.mHidden = true;
}

void EffectsAudioUnitTestApp::setupTwo()
{
	auto ctx = audio::master();

	mEffect = ctx->makeNode( new audio::cocoa::EffectAudioUnitNode( kAudioUnitSubType_LowPassFilter ) );
	mEffect2 = ctx->makeNode( new audio::cocoa::EffectAudioUnitNode( kAudioUnitSubType_BandPassFilter ) );

//	mEffect->getFormat().setNumChannels( 2 ); // force stereo

	mGen >> mEffect >> mEffect2 >> ctx->getOutput();

	mBandpassSlider.mHidden = false;
}

void EffectsAudioUnitTestApp::setupNativeThenGeneric()
{
	CI_LOG_E( "TODO: implement test" );
}

void EffectsAudioUnitTestApp::initParams()
{

	if( mEffect ) {
		mEffect->setParameter( kLowPassParam_CutoffFrequency, 500 );
		mLowpassCutoffSlider.set( 500 );
	}

	if( mEffect2 ) {
		mEffect2->setParameter( kBandpassParam_CenterFrequency, 1000 );
		mEffect2->setParameter( kBandpassParam_Bandwidth, 1200 );
		mBandpassSlider.set( 1000 );
	}
}

void EffectsAudioUnitTestApp::setupUI()
{
	mPlayButton = Button( true, "stopped", "playing" );
//	mPlayButton.bounds = Rectf( 0, 0, 200, 60 );

	mTestSelector.mSegments = { "one", "two", "native -> generic" };
//	mTestSelector.bounds = Rectf( getWindowCenter().x + 100, 0.0f, getWindowWidth(), 160.0f );

//#if defined( CINDER_COCOA_TOUCH )
//	mPlayButton.bounds = Rectf( 0, 0, 120, 60 );
//	mPlayButton.textIsCentered = false;
//	mTestSelector.bounds = Rectf( getWindowWidth() - 190, 0.0f, getWindowWidth(), 160.0f );
//	mTestSelector.textIsCentered = false;
//#else
	mPlayButton.mBounds = Rectf( 0, 0, 200, 60 );
	mTestSelector.mBounds = Rectf( getWindowCenter().x + 100, 0.0f, getWindowWidth(), 160.0f );
//#endif

	float width = std::min( (float)getWindowWidth() - 20.0f,  440.0f );
	Rectf sliderRect( getWindowCenter().x - width / 2.0f, 200, getWindowCenter().x + width / 2.0f, 250 );

	mGenFreqSlider.mTitle = "Gen Freq";
	mGenFreqSlider.mMin = 30.0f;
	mGenFreqSlider.mMax = 500.0f;
	mGenFreqSlider.set( mGen->getFreq() );
	mGenFreqSlider.mBounds = sliderRect;

	sliderRect += vec2( 0, sliderRect.getHeight() + 10 );
	mLowpassCutoffSlider.mBounds = sliderRect;
	mLowpassCutoffSlider.mTitle = "Lowpass Cutoff";
	mLowpassCutoffSlider.mMax = 1500.0f;

	sliderRect += vec2( 0, sliderRect.getHeight() + 10 );
	mBandpassSlider.mBounds = sliderRect;
	mBandpassSlider.mTitle = "Bandpass";
	mBandpassSlider.mMin = 100.0f;
	mBandpassSlider.mMax = 2000.0f;

	getWindow()->getSignalMouseDown().connect( [this] ( MouseEvent &event ) { processTap( event.getPos() ); } );
	getWindow()->getSignalMouseDrag().connect( [this] ( MouseEvent &event ) { processDrag( event.getPos() ); } );
	getWindow()->getSignalTouchesBegan().connect( [this] ( TouchEvent &event ) { processTap( event.getTouches().front().getPos() ); } );
	getWindow()->getSignalTouchesMoved().connect( [this] ( TouchEvent &event ) {
		for( const TouchEvent::Touch &touch : getActiveTouches() )
			processDrag( touch.getPos() );
	} );

	gl::enableAlphaBlending();
}

void EffectsAudioUnitTestApp::processDrag( ivec2 pos )
{
	if( mGenFreqSlider.hitTest( pos ) )
		mGen->getParamFreq()->applyRamp( mGenFreqSlider.mValueScaled, 0.02f );

	if( mEffect && mLowpassCutoffSlider.hitTest( pos ) )
		mEffect->setParameter( kLowPassParam_CutoffFrequency, mLowpassCutoffSlider.mValueScaled );

	if( mEffect2 && mBandpassSlider.hitTest( pos ) )
		mEffect2->setParameter( kBandpassParam_CenterFrequency, mBandpassSlider.mValueScaled );
}

void EffectsAudioUnitTestApp::processTap( ivec2 pos )
{
	auto ctx = audio::master();

	if( mPlayButton.hitTest( pos ) )
		ctx->setEnabled( ! ctx->isEnabled() );

	size_t currentIndex = mTestSelector.mCurrentSectionIndex;
	if( mTestSelector.hitTest( pos ) && currentIndex != mTestSelector.mCurrentSectionIndex ) {
		string currentTest = mTestSelector.currentSection();
		CI_LOG_V( "selected: " << currentTest );

		bool enabled = ctx->isEnabled();
		ctx->disconnectAllNodes();

		mGen->disconnectAll();
		mEffect->disconnectAll();
		if( mEffect2 )
			mEffect2->disconnectAll();

		if( currentTest == "one" )
			setupOne();
		if( currentTest == "two" )
			setupTwo();
		if( currentTest == "native -> generic" )
			setupNativeThenGeneric();
		initParams();

		ctx->setEnabled( enabled );
		PRINT_GRAPH( ctx );
	}
}

void EffectsAudioUnitTestApp::update()
{
}

void EffectsAudioUnitTestApp::draw()
{
	gl::clear();

	mPlayButton.draw();
	mGenFreqSlider.draw();

	if( mEffect )
		mLowpassCutoffSlider.draw();
	if( mEffect2 )
		mBandpassSlider.draw();

	mTestSelector.draw();
}

CINDER_APP( EffectsAudioUnitTestApp, RendererGl )
