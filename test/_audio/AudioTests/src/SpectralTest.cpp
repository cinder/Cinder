#include "SpectralTest.h"

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Utilities.h"
#include "cinder/Log.h"
#include "cinder/CinderImGui.h"
#include "cinder/gl/gl.h"

#include "cinder/audio/Context.h"

// TODO: make these runtime configurable
#define FFT_SIZE 2048
#define WINDOW_SIZE 1024
#define WINDOW_TYPE audio::dsp::WindowType::BLACKMAN

using namespace ci;
using namespace std;
namespace im = ImGui;

SpectralTest::SpectralTest()
{
	mSubTests = {
		"sine",
		"sine (no output)",
		"sample",
	};

	auto ctx = audio::master();

	auto format = audio::MonitorSpectralNode::Format().fftSize( FFT_SIZE ).windowSize( WINDOW_SIZE ).windowType( WINDOW_TYPE );
	mMonitorSpectralNode = ctx->makeNode( new audio::MonitorSpectralNode( format ) );
	mMonitorSpectralNode->setAutoEnabled();

	mGen = ctx->makeNode( new audio::GenSineNode() );
	//mGen = ctx->makeNode( new audio::GenTriangleNode() );
	mGen->setFreq( 440.0f );

	mSourceFile = audio::load( app::loadAsset( "tone440L220R.mp3" ), ctx->getSampleRate() );

	mGain = ctx->makeNode( new audio::GainNode() );
	mGain->setValue( 0.6f );

	auto audioBuffer = mSourceFile->loadBuffer();
	CI_LOG_V( "loaded source buffer, frames: " << audioBuffer->getNumFrames() );

	mPlayerNode = ctx->makeNode( new audio::BufferPlayerNode( audioBuffer ) );

	setupSubTest( "sine" );

	CI_LOG_V( "MonitorSpectralNode fftSize: " << mMonitorSpectralNode->getFftSize() << ", windowSize: " << mMonitorSpectralNode->getWindowSize() );
}

void SpectralTest::setupSubTest( const string &testName )
{
	auto ctx = audio::master();

	bool enabled = ctx->isEnabled();
	ctx->disable();
	ctx->disconnectAllNodes();

	if( testName == "sine" )
		setupSine();
	else if( testName == "sine (no output)" )
		setupSineNoOutput();
	else if( testName == "sample" )
		setupSample();

	ctx->setEnabled( enabled );

	CI_LOG_I( "Finished setup for test: " << testName );
}

void SpectralTest::setupSine()
{
	mGen >> mMonitorSpectralNode >> mGain >> audio::master()->getOutput();
	if( mPlaybackEnabled )
		mGen->enable();
}

void SpectralTest::setupSineNoOutput()
{
	mGen->connect( mMonitorSpectralNode );
	if( mPlaybackEnabled )
		mGen->enable();
}

void SpectralTest::setupSample()
{
	mPlayerNode >> mMonitorSpectralNode >> mGain >> audio::master()->getOutput();
	if( mPlaybackEnabled )
		mPlayerNode->enable();
}

void SpectralTest::openFile( const ci::fs::path &fullPath )
{
	try {
		mSourceFile = audio::load( loadFile( fullPath ), audio::master()->getSampleRate() );
		mPlayerNode->setBuffer( mSourceFile->loadBuffer() );
		CI_LOG_I( "loaded and set new source buffer, frames: " << mSourceFile->getNumFrames() );
	}
	catch( exception &e ) {
		CI_LOG_EXCEPTION( "failed to load sample file at path: " << fullPath, e );
	}
}

//	freq = bin * samplerate / sizeFft
void SpectralTest::printBinFreq( size_t xPos )
{
	if( xPos < mSpectroMargin || xPos > app::getWindowWidth() - mSpectroMargin )
		return;


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

void SpectralTest::draw()
{
	// draw magnitude spectrum
	auto &mag = mMonitorSpectralNode->getMagSpectrum();
	mSpectrumPlot.draw( mag );

	if( ! mag.empty() ) {
		auto min = min_element( mag.begin(), mag.end() );
		auto max = max_element( mag.begin(), mag.end() );

		string info = string( "min: " ) + toString( *min ) + string( ", max: " ) + toString( *max );
		gl::drawString( info, vec2( mSpectroMargin, app::getWindowHeight() - 30.0f ) );
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

	}
}


// -----------------------------------------------------------------------
// UI
// -----------------------------------------------------------------------

void SpectralTest::updateUI()
{
	if( mSpectrumPlot.getScaleDecibels() ) {
		if( im::Button( "decibels scale" ) ) {
			mSpectrumPlot.enableScaleDecibels( false );
		}
	}
	else {
		if( im::Button( "linear scale" ) ) {
			mSpectrumPlot.enableScaleDecibels( true );
		}
	}
	im::SameLine();
	im::Text( "Spectral centroid: %0.3f", mMonitorSpectralNode->getSpectralCentroid() );

	float gain = mGain->getValue();
	if( im::SliderFloat( "gain", &gain, 0, 1 ) ) {
		mGain->getParam()->applyRamp( gain, 0.3f );
	}
	float smoothing = mMonitorSpectralNode->getSmoothingFactor();
	if( im::SliderFloat( "spectral smoothing", &smoothing, 0, 1 ) ) {
		mMonitorSpectralNode->setSmoothingFactor( smoothing );
	}

	im::Checkbox( "playing", &mPlaybackEnabled );

	im::Separator();

	if( mSubTests[mCurrentSubTest] == "sample" ) {
		mPlayerNode->setEnabled( mPlaybackEnabled );
		im::Text( "Sample" );
		bool loopEnabled = mPlayerNode->isLoopEnabled();
		if( im::Checkbox( "loop", &loopEnabled ) ) {
			mPlayerNode->setLoopEnabled( loopEnabled );
		}
		im::SameLine();
		if( im::Button( "reset" ) ) {
			mPlayerNode->seek( 0 );
		}

		float readPos = (float)mPlayerNode->getReadPositionTime();
		if( im::SliderFloat( "read pos (s)", &readPos, 0, mPlayerNode->getNumSeconds() ) ) {
			mPlayerNode->seekToTime( readPos );
		}

	}
	else {
		mGen->setEnabled( mPlaybackEnabled );
		im::Text( "Gen" );
		float genFreq = mGen->getFreq();
		if( im::SliderFloat( "Gen Freqency", &genFreq, -200, 1200 ) ) {
			mGen->getParamFreq()->applyRamp( genFreq, 0.05f );
		}
	}


	im::Separator();
	if( im::ListBox( "sub-tests", &mCurrentSubTest, mSubTests, mSubTests.size() ) ) {
		setupSubTest( mSubTests[mCurrentSubTest] );
	}
}
