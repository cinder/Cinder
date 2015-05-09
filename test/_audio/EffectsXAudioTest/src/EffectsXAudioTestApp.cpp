#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/audio/GenNode.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/audio/Debug.h"

#include "cinder/audio/msw/ContextXAudio.h"
#include "cinder/audio/msw/MswUtil.h"
#include "cinder/msw/CinderMsw.h"
#include "cinder/CinderAssert.h"

#include "../../common/AudioTestGui.h"

#include <xaudio2.h>
#include <xaudio2fx.h>

#include "XAPOFX.h"

#if( _WIN32_WINNT >= 0x0602 )
	#pragma comment(lib, "xaudio2.lib")
	#pragma comment(lib, "xapobase.lib")
#else
	#pragma comment(lib, "XAPOFX.lib")
#endif

// copied from xaudio2.h to I don't have to define XAUDIO2_HELPER_FUNCTIONS
__inline float XAudio2FrequencyRatioToSemitones(float FrequencyRatio)
{
	// Semitones = 12 * log2(FrequencyRatio)
	//           = 12 * log2(10) * log10(FrequencyRatio)
	return 39.86313713864835f * log10f(FrequencyRatio);
}

__inline float XAudio2CutoffFrequencyToRadians(float CutoffFrequency, UINT32 SampleRate)
{
	if ((UINT32)(CutoffFrequency * 6.0f) >= SampleRate)
	{
		return XAUDIO2_MAX_FILTER_FREQUENCY;
	}
	return 2.0f * sinf((float)M_PI * CutoffFrequency / SampleRate);
}

using namespace ci;
using namespace ci::app;
using namespace std;

class EffectXAudioTestApp : public AppNative {
public:
	void setup();
	void draw();

	void setupReverb();
	void setupFilter();

	void initReverbParams();
	void updateReverb();
	void updateLowpass();

	void setupUI();
	void processDrag( Vec2i pos );
	void processTap( Vec2i pos );


	audio::GenNodeRef mGen;
	audio::GainNodeRef	mGain;

	audio::msw::ContextXAudio*	mContextXAudio;

	std::unique_ptr<::IUnknown, audio::msw::ComReleaser>	mXapo;
	XAUDIO2_EFFECT_DESCRIPTOR								mEffectDesc;
	XAUDIO2FX_REVERB_PARAMETERS								mReverbParams;
	XAUDIO2_FILTER_PARAMETERS								mFilterParams;

	vector<TestWidget *> mWidgets;
	Button mPlayButton;
	HSlider mFreqSlider, mLowpassCutoffSlider, mReverbDecaySlider;
};

void EffectXAudioTestApp::setup()
{
	mContextXAudio = new audio::msw::ContextXAudio;
	audio::Context::setMaster( mContextXAudio, new audio::msw::DeviceManagerXAudio );

	auto ctx = audio::master();

	//mGen = ctx->makeNode( new audio::GenPulse );
	mGen = ctx->makeNode( new audio::GenTriangle );

	mGen->setFreq( 100 );
	mGen->setAutoEnabled();

	mGain = ctx->makeNode( new audio::GainNode( 0.6f ) );
	
	mGen >> mGain >> ctx->getOutput();

	audio::master()->printGraph();

	setupReverb();
	setupFilter();

	setupUI();
}

void EffectXAudioTestApp::setupReverb()
{
	::IUnknown *reverb;
	HRESULT hr = XAudio2CreateReverb( &reverb );
	CI_ASSERT( hr == S_OK );

	mXapo = audio::msw::makeComUnique( reverb );
	mEffectDesc.InitialState = true;
	mEffectDesc.OutputChannels = 2;
	mEffectDesc.pEffect = mXapo.get();

	::XAUDIO2_EFFECT_CHAIN effectsChain;
	effectsChain.EffectCount = 1;
	effectsChain.pEffectDescriptors = &mEffectDesc;

	// you can only call SetEffectChain once in the lifetime of the IXAudio2SourceVoice, so re-init it first.
	auto lineOut = mContextXAudio->getLineOutXAudio();

	mContextXAudio->uninitializeNode( lineOut );
	mContextXAudio->initializeNode( lineOut );

	IXAudio2SourceVoice *sourceVoice = lineOut->getSourceVoice(); 
	CI_ASSERT( sourceVoice );

	hr = sourceVoice->SetEffectChain( &effectsChain );
	CI_ASSERT( hr == S_OK );

	// init params after connecting
	initReverbParams();
}

void EffectXAudioTestApp::setupFilter()
{
	IXAudio2SourceVoice *sourceVoice = mContextXAudio->getLineOutXAudio()->getSourceVoice(); 

	sourceVoice->GetFilterParameters( &mFilterParams );

	mFilterParams.Type = LowPassFilter;
	float cutoff = XAudio2FrequencyRatioToSemitones( mFilterParams.Frequency );

	HRESULT hr = sourceVoice->SetFilterParameters( &mFilterParams );
	CI_ASSERT( hr == S_OK );

	mLowpassCutoffSlider.set( cutoff );
}

void EffectXAudioTestApp::initReverbParams()
{
	// set reverb defaults
	mReverbParams.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;
	mReverbParams.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
	mReverbParams.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
	mReverbParams.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
	mReverbParams.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	mReverbParams.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	mReverbParams.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	mReverbParams.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	mReverbParams.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
	mReverbParams.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
	mReverbParams.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
	mReverbParams.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
	mReverbParams.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
	mReverbParams.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
	mReverbParams.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
	mReverbParams.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
	mReverbParams.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
	mReverbParams.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
	mReverbParams.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
	mReverbParams.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
	mReverbParams.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
	mReverbParams.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
#if defined( CINDER_XAUDIO_2_8 )
	mReverbParams.DisableLateField = XAUDIO2FX_REVERB_DEFAULT_DISABLE_LATE_FIELD;
#endif

	IXAudio2SourceVoice *sourceVoice = mContextXAudio->getLineOutXAudio()->getSourceVoice();
	HRESULT hr = sourceVoice->SetEffectParameters( 0, &mReverbParams, sizeof( mReverbParams ) );
	CI_ASSERT( hr == S_OK );
}

void EffectXAudioTestApp::setupUI()
{
	mPlayButton = Button( true, "stopped", "playing" );
	mPlayButton.mBounds = Rectf( 0, 0, 200, 60 );
	mWidgets.push_back( &mPlayButton );

	float width = std::min( (float)getWindowWidth() - 20.0f,  440.0f );
	Rectf sliderRect( getWindowCenter().x - width / 2.0f, 200, getWindowCenter().x + width / 2.0f, 250 );

	mFreqSlider.mBounds = sliderRect;
	mFreqSlider.mTitle = "Gen Freq";
	mFreqSlider.mMax = 800.0f;
	mFreqSlider.set( mGen->getFreq() );
	mWidgets.push_back( &mFreqSlider);

	sliderRect += Vec2f( 0, sliderRect.getHeight() + 10 );
	mLowpassCutoffSlider.mBounds = sliderRect;
	mLowpassCutoffSlider.mTitle = "Lowpass Cutoff";
	mLowpassCutoffSlider.mMax = 1500.0f;
	mLowpassCutoffSlider.set( mLowpassCutoffSlider.mValueScaled );
	mWidgets.push_back( &mLowpassCutoffSlider );

	sliderRect += Vec2f( 0, sliderRect.getHeight() + 10 );
	mReverbDecaySlider.mBounds = sliderRect;
	mReverbDecaySlider.mTitle = "Reverb Decay (s)";
	mReverbDecaySlider.mMin = XAUDIO2FX_REVERB_MIN_DECAY_TIME;
	mReverbDecaySlider.mMax = 10;
	mReverbDecaySlider.set( XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME );
	mWidgets.push_back( &mReverbDecaySlider );

	getWindow()->getSignalMouseDown().connect( [this] ( MouseEvent &event ) { processTap( event.getPos() ); } );
	getWindow()->getSignalMouseDrag().connect( [this] ( MouseEvent &event ) { processDrag( event.getPos() ); } );
	getWindow()->getSignalTouchesBegan().connect( [this] ( TouchEvent &event ) { processTap( event.getTouches().front().getPos() ); } );
	getWindow()->getSignalTouchesMoved().connect( [this] ( TouchEvent &event ) {
		for( const TouchEvent::Touch &touch : getActiveTouches() )
			processDrag( touch.getPos() );
	} );

	gl::enableAlphaBlending();
}

void EffectXAudioTestApp::updateLowpass()
{
	mFilterParams.Frequency = XAudio2CutoffFrequencyToRadians( mLowpassCutoffSlider.mValueScaled, audio::master()->getSampleRate() );

	IXAudio2SourceVoice *sourceVoice = mContextXAudio->getLineOutXAudio()->getSourceVoice();
	HRESULT hr = sourceVoice->SetFilterParameters( &mFilterParams );
	CI_ASSERT( hr == S_OK );
}

void EffectXAudioTestApp::updateReverb()
{
	if( mXapo ) {
		mReverbParams.DecayTime = mReverbDecaySlider.mValueScaled;

		IXAudio2SourceVoice *sourceVoice = mContextXAudio->getLineOutXAudio()->getSourceVoice();
		HRESULT hr = sourceVoice->SetEffectParameters( 0, &mReverbParams, sizeof( mReverbParams ) );
		CI_ASSERT( hr == S_OK );
	}
}

void EffectXAudioTestApp::processDrag( Vec2i pos )
{
	if( mFreqSlider.hitTest( pos ) )
		mGen->setFreq( mFreqSlider.mValueScaled );
	if( mLowpassCutoffSlider.hitTest( pos ) )
		updateLowpass();	
	if( mReverbDecaySlider.hitTest( pos ) )
		updateReverb();
}

void EffectXAudioTestApp::processTap( Vec2i pos )
{
	if( mPlayButton.hitTest( pos ) )
		audio::master()->setEnabled( ! audio::master()->isEnabled() );
	else
		processDrag( pos );
}

void EffectXAudioTestApp::draw()
{
	gl::clear();
	drawWidgets( mWidgets );
}

CINDER_APP_NATIVE( EffectXAudioTestApp, RendererGl )
