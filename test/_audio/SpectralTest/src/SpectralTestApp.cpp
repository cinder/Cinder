#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"

#include "cinder/audio/GenNode.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/dsp/Dsp.h"

#include "../../common/AudioTestGui.h"
#include "../../../../samples/_audio/common/AudioDrawUtils.h"
#include "Resources.h"

// TODO: make these runtime configurable
#define FFT_SIZE 2048
#define WINDOW_SIZE 1024
#define WINDOW_TYPE audio::dsp::WindowType::BLACKMAN

using namespace ci;
using namespace ci::app;
using namespace std;


class SpectralTestApp : public App {
  public:
	void fileDrop( FileDropEvent event ) override;
	void setup() override;
	void resize() override;
	void update() override;
	void draw() override;

	void setupSine();
	void setupSineNoOutput();
	void setupSample();
	void setupUI();
	void processTap( ivec2 pos );
	void processDrag( ivec2 pos );
	void printBinFreq( size_t xPos );


	audio::BufferPlayerNodeRef		mPlayerNode;
	audio::GenNodeRef				mGen;
	audio::MonitorSpectralNodeRef	mMonitorSpectralNode;
	audio::SourceFileRef			mSourceFile;

	vector<TestWidget *>			mWidgets;
	Button							mEnableGraphButton, mPlaybackButton, mLoopButton, mScaleDecibelsButton;
	VSelector						mTestSelector;
	HSlider							mSmoothingFactorSlider, mFreqSlider;
	SpectrumPlot					mSpectrumPlot;
	float							mSpectroMargin;
};

void SpectralTestApp::setup()
{
	mSpectroMargin = 40.0f;

	auto ctx = audio::master();

	auto format = audio::MonitorSpectralNode::Format().fftSize( FFT_SIZE ).windowSize( WINDOW_SIZE ).windowType( WINDOW_TYPE );
	mMonitorSpectralNode = ctx->makeNode( new audio::MonitorSpectralNode( format ) );
	mMonitorSpectralNode->setAutoEnabled();

	mGen = ctx->makeNode( new audio::GenSineNode() );
	//mGen = ctx->makeNode( new audio::GenTriangleNode() );
	mGen->setFreq( 440.0f );

	mSourceFile = audio::load( loadResource( RES_TONE440L220R_MP3 ), ctx->getSampleRate() );

	auto audioBuffer = mSourceFile->loadBuffer();
	CI_LOG_V( "loaded source buffer, frames: " << audioBuffer->getNumFrames() );

	mPlayerNode = ctx->makeNode( new audio::BufferPlayerNode( audioBuffer ) );

	setupSine();

	setupUI();

	ctx->enable();
	mEnableGraphButton.setEnabled( true );

	mScaleDecibelsButton.setEnabled( mSpectrumPlot.getScaleDecibels() );

	PRINT_GRAPH( ctx );

	CI_LOG_V( "MonitorSpectralNode fftSize: " << mMonitorSpectralNode->getFftSize() << ", windowSize: " << mMonitorSpectralNode->getWindowSize() );
}

void SpectralTestApp::setupSine()
{
	mGen >> mMonitorSpectralNode >> audio::master()->getOutput();
	if( mPlaybackButton.mEnabled )
		mGen->enable();
}

void SpectralTestApp::setupSineNoOutput()
{
	mGen->connect( mMonitorSpectralNode );
	if( mPlaybackButton.mEnabled )
		mGen->enable();
}

void SpectralTestApp::setupSample()
{
	mPlayerNode >> mMonitorSpectralNode >> audio::master()->getOutput();
	if( mPlaybackButton.mEnabled )
		mPlayerNode->enable();
}

void SpectralTestApp::setupUI()
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

void SpectralTestApp::printBinFreq( size_t xPos )
{
	if( xPos < mSpectroMargin || xPos > getWindowWidth() - mSpectroMargin )
		return;

//	freq = bin * samplerate / sizeFft

	size_t numBins = mMonitorSpectralNode->getFftSize() / 2;
	size_t spectroWidth = getWindowWidth() - mSpectroMargin * 2;
	size_t bin = ( numBins * ( xPos - mSpectroMargin ) ) / spectroWidth;
	float freq = bin * audio::master()->getSampleRate() / float( mMonitorSpectralNode->getFftSize() );

	CI_LOG_V( "bin: " << bin << ", freq: " << freq );
}

// TODO: currently makes sense to enable processor + tap together - consider making these enabled together.
// - possible solution: add a silent flag that is settable by client
void SpectralTestApp::processTap( ivec2 pos )
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

void SpectralTestApp::processDrag( ivec2 pos )
{
	if( mSmoothingFactorSlider.hitTest( pos ) )
		mMonitorSpectralNode->setSmoothingFactor( mSmoothingFactorSlider.mValueScaled );
	if( mFreqSlider.hitTest( pos ) )
		mGen->setFreq( mFreqSlider.mValueScaled );
}

void SpectralTestApp::fileDrop( FileDropEvent event )
{
	const fs::path &filePath = event.getFile( 0 );
	CI_LOG_V( "File dropped: " << filePath );

	mSourceFile = audio::load( loadFile( filePath ), audio::master()->getSampleRate() );

	mPlayerNode->setBuffer( mSourceFile->loadBuffer() );

	CI_LOG_V( "loaded and set new source buffer, frames: " << mSourceFile->getNumFrames() );
}

void SpectralTestApp::resize()
{
	mSpectrumPlot.setBounds( Rectf( mSpectroMargin, mSpectroMargin, getWindowWidth() - mSpectroMargin, getWindowHeight() - mSpectroMargin ) );
}

void SpectralTestApp::update()
{
	// update playback button, since the player node may stop itself at the end of a file.
	if( mTestSelector.currentSection() == "sample" && ! mPlayerNode->isEnabled() )
		mPlaybackButton.setEnabled( false );
}

void SpectralTestApp::draw()
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

	drawWidgets( mWidgets );
}

CINDER_APP( SpectralTestApp, RendererGl, []( App::Settings *settings ) {
	settings->setWindowSize( 1200, 500 );
} )
