#include "SpectralTest.h"

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"
#include "cinder/CinderImGui.h"
#include "cinder/gl/gl.h"

// TODO: make these runtime configurable
#define FFT_SIZE 2048
#define WINDOW_SIZE 1024
#define WINDOW_TYPE audio::dsp::WindowType::BLACKMAN

using namespace ci;
using namespace std;
namespace im = ImGui;

SpectralTest::SpectralTest()
{
	auto ctx = audio::master();

	auto format = audio::MonitorSpectralNode::Format().fftSize( FFT_SIZE ).windowSize( WINDOW_SIZE ).windowType( WINDOW_TYPE );
	mMonitorSpectralNode = ctx->makeNode( new audio::MonitorSpectralNode( format ) );
	mMonitorSpectralNode->setAutoEnabled();

	mGen = ctx->makeNode( new audio::GenSineNode() );
	//mGen = ctx->makeNode( new audio::GenTriangleNode() );
	mGen->setFreq( 440.0f );

	mSourceFile = audio::load( app::loadAsset( "tone440L220R.mp3" ), ctx->getSampleRate() );

	auto audioBuffer = mSourceFile->loadBuffer();
	CI_LOG_V( "loaded source buffer, frames: " << audioBuffer->getNumFrames() );

	mPlayerNode = ctx->makeNode( new audio::BufferPlayerNode( audioBuffer ) );

	setupSine();

	setupUI();

	ctx->enable();
	mEnableGraphButton.setEnabled( true );

	mScaleDecibelsButton.setEnabled( mSpectrumPlot.getScaleDecibels() );

	CI_LOG_V( "MonitorSpectralNode fftSize: " << mMonitorSpectralNode->getFftSize() << ", windowSize: " << mMonitorSpectralNode->getWindowSize() );
}

void SpectralTest::setupSine()
{
	mGen >> mMonitorSpectralNode >> audio::master()->getOutput();
	if( mPlaybackButton.mEnabled )
		mGen->enable();
}

void SpectralTest::setupSineNoOutput()
{
	mGen->connect( mMonitorSpectralNode );
	if( mPlaybackButton.mEnabled )
		mGen->enable();
}

void SpectralTest::setupSample()
{
	mPlayerNode >> mMonitorSpectralNode >> audio::master()->getOutput();
	if( mPlaybackButton.mEnabled )
		mPlayerNode->enable();
}

#if 0
void SpectralTest::setupUI()
{
	Rectf buttonRect( 0.0f, 0.0f, 200.0f, mSpectroMargin - 2.0f );
	float padding = 10.0f;
	mEnableGraphButton.mIsToggle = true;
	mEnableGraphButton.mTitleNormal = "graph off";
	mEnableGraphButton.mTitleEnabled = "graph on";
	mEnableGraphButton.mBounds = buttonRect;
	mWidgets.push_back( &mEnableGraphButton );

	buttonRect += vec2( buttonRect.getWidth() + padding, 0.0f );
	mPlaybackButton.mIsToggle = true;
	mPlaybackButton.mTitleNormal = "play";
	mPlaybackButton.mTitleEnabled = "stop";
	mPlaybackButton.mBounds = buttonRect;
	mWidgets.push_back( &mPlaybackButton );

	buttonRect += vec2( buttonRect.getWidth() + padding, 0.0f );
	mLoopButton.mIsToggle = true;
	mLoopButton.mTitleNormal = "loop off";
	mLoopButton.mTitleEnabled = "loop on";
	mLoopButton.mBounds = buttonRect;
	mWidgets.push_back( &mLoopButton );

	buttonRect += vec2( buttonRect.getWidth() + padding, 0.0f );
	mScaleDecibelsButton.mIsToggle = true;
	mScaleDecibelsButton.mTitleNormal = "linear";
	mScaleDecibelsButton.mTitleEnabled = "decibels";
	mScaleDecibelsButton.mBounds = buttonRect;
	mWidgets.push_back( &mScaleDecibelsButton );

	vec2 sliderSize( 200.0f, 30.0f );
	Rectf selectorRect( getWindowWidth() - sliderSize.x - mSpectroMargin, buttonRect.y2 + padding, getWindowWidth() - mSpectroMargin, buttonRect.y2 + padding + sliderSize.y * 3 );
	mTestSelector.mSegments.push_back( "sine" );
	mTestSelector.mSegments.push_back( "sine (no output)" );
	mTestSelector.mSegments.push_back( "sample" );
	mTestSelector.mBounds = selectorRect;
	mWidgets.push_back( &mTestSelector );

	Rectf sliderRect( selectorRect.x1, selectorRect.y2 + padding, selectorRect.x2, selectorRect.y2 + padding + sliderSize.y );
	mSmoothingFactorSlider.mBounds = sliderRect;
	mSmoothingFactorSlider.mTitle = "Smoothing";
	mSmoothingFactorSlider.mMin = 0.0f;
	mSmoothingFactorSlider.mMax = 1.0f;
	mSmoothingFactorSlider.set( mMonitorSpectralNode->getSmoothingFactor() );
	mWidgets.push_back( &mSmoothingFactorSlider );

	sliderRect += vec2( 0.0f, sliderSize.y + padding );
	mFreqSlider.mBounds = sliderRect;
	mFreqSlider.mTitle = "Sine Freq";
	mFreqSlider.mMin = 0.0f;
//	mFreqSlider.mMax = mContext->getSampleRate() / 2.0f;
	mFreqSlider.mMax = 800;
	mFreqSlider.set( mGen->getFreq() );
	mWidgets.push_back( &mFreqSlider );


	getWindow()->getSignalMouseDown().connect( [this] ( MouseEvent &event ) { processTap( event.getPos() ); } );
	getWindow()->getSignalTouchesBegan().connect( [this] ( TouchEvent &event ) { processTap( event.getTouches().front().getPos() ); } );
	getWindow()->getSignalMouseDrag().connect( [this] ( MouseEvent &event ) { processDrag( event.getPos() ); } );
	getWindow()->getSignalTouchesMoved().connect( [this] ( TouchEvent &event ) {
		for( const TouchEvent::Touch &touch : getActiveTouches() )
			processDrag( touch.getPos() );
	} );

	gl::enableAlphaBlending();
}

// TODO: currently makes sense to enable processor + tap together - consider making these enabled together.
// - possible solution: add a silent flag that is settable by client
void SpectralTest::processTap( ivec2 pos )
{
	auto ctx = audio::master();
	if( mEnableGraphButton.hitTest( pos ) )
		ctx->setEnabled( ! ctx->isEnabled() );
	else if( mPlaybackButton.hitTest( pos ) ) {
		if( mTestSelector.currentSection() == "sine" || mTestSelector.currentSection() == "sine (no output)" )
			mGen->setEnabled( mPlaybackButton.mEnabled );
		else {
			if( mPlaybackButton.mEnabled )
				mPlayerNode->start();
			else
				mPlayerNode->stop();
		}
	}
	else if( mLoopButton.hitTest( pos ) )
		mPlayerNode->setLoopEnabled( ! mPlayerNode->isLoopEnabled() );
	else if( mScaleDecibelsButton.hitTest( pos ) )
		mSpectrumPlot.enableScaleDecibels( ! mSpectrumPlot.getScaleDecibels() );
	else
		printBinFreq( pos.x );

	size_t currentIndex = mTestSelector.mCurrentSectionIndex;
	if( mTestSelector.hitTest( pos ) && currentIndex != mTestSelector.mCurrentSectionIndex ) {
		string currentTest = mTestSelector.currentSection();
		CI_LOG_V( "selected: " << currentTest );

		bool enabled = ctx->isEnabled();
		ctx->disconnectAllNodes();

		if( currentTest == "sine" )
			setupSine();
		if( currentTest == "sine (no output)" )
			setupSineNoOutput();
		if( currentTest == "sample" )
			setupSample();

		ctx->setEnabled( enabled );
	}

}

void SpectralTest::processDrag( ivec2 pos )
{
	if( mSmoothingFactorSlider.hitTest( pos ) )
		mMonitorSpectralNode->setSmoothingFactor( mSmoothingFactorSlider.mValueScaled );
	if( mFreqSlider.hitTest( pos ) )
		mGen->setFreq( mFreqSlider.mValueScaled );
}

#endif

void SpectralTest::fileDrop( FileDropEvent event )
{
	const fs::path &filePath = event.getFile( 0 );
	CI_LOG_V( "File dropped: " << filePath );

	mSourceFile = audio::load( loadFile( filePath ), audio::master()->getSampleRate() );

	mPlayerNode->setBuffer( mSourceFile->loadBuffer() );

	CI_LOG_V( "loaded and set new source buffer, frames: " << mSourceFile->getNumFrames() );
}

void SpectralTest::printBinFreq( size_t xPos )
{
	if( xPos < mSpectroMargin || xPos > app::getWindowWidth() - mSpectroMargin )
		return;

	//	freq = bin * samplerate / sizeFft

	size_t numBins = mMonitorSpectralNode->getFftSize() / 2;
	size_t spectroWidth = app::getWindowWidth() - mSpectroMargin * 2;
	size_t bin = ( numBins * ( xPos - mSpectroMargin ) ) / spectroWidth;
	float freq = bin * audio::master()->getSampleRate() / float( mMonitorSpectralNode->getFftSize() );

	CI_LOG_V( "bin: " << bin << ", freq: " << freq );
}

void SpectralTest::resize()
{
	mSpectrumPlot.setBounds( Rectf( mSpectroMargin, mSpectroMargin, app::getWindowWidth() - mSpectroMargin, app::getWindowHeight() - mSpectroMargin ) );
}

void SpectralTest::update()
{
	// update playback button, since the player node may stop itself at the end of a file.
	if( mTestSelector.currentSection() == "sample" && ! mPlayerNode->isEnabled() )
		mPlaybackButton.setEnabled( false );
}

void SpectralTest::draw()
{
	gl::clear();

	// draw magnitude spectrum
	auto &mag = mMonitorSpectralNode->getMagSpectrum();
	mSpectrumPlot.draw( mag );

	if( ! mag.empty() ) {
		auto min = min_element( mag.begin(), mag.end() );
		auto max = max_element( mag.begin(), mag.end() );

		string info = string( "min: " ) + toString( *min ) + string( ", max: " ) + toString( *max );
		gl::drawString( info, vec2( mSpectroMargin, getWindowHeight() - 30.0f ) );
	}

	// draw vertical line for spectral centroid
	{
		float spectralCentroid = mMonitorSpectralNode->getSpectralCentroid();
		float nyquist = (float)audio::master()->getSampleRate() / 2.0f;
		Rectf bounds = mSpectrumPlot.getBounds();

		float freqNormalized = spectralCentroid / nyquist;
		float barCenter = bounds.x1 + freqNormalized * bounds.getWidth();
		Rectf bar = { barCenter - 2, bounds.y1, barCenter + 2, bounds.y2 };

		gl::ScopedColor colorScope( 0.85f, 0.45f, 0, 0.4f );
		gl::drawSolidRect( bar );

		//if( app::getElapsedFrames() % 30 == 0 ) {
		//	CI_LOG_I( "spectralCentroid: " << spectralCentroid );
		//}
	}

}


// -----------------------------------------------------------------------
// UI
// -----------------------------------------------------------------------

void SpectralTest::updateUI()
{
	float gain = mGain->getValue();
	if( im::SliderFloat( "gain", &gain, 0, 1 ) ) {
		mGain->setValue( gain );
	}


	if( im::ListBox( "sub-tests", &mCurrentSubTest, mSubTests, mSubTests.size() ) ) {
		setupSubTest( mSubTests[mCurrentSubTest] );
	}
}
