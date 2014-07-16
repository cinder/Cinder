/*
 * This sample illustrates how to get audio data from an input device, such as a microphone,
 * with audio::InputDeviceNode. It then visualizes the input in the frequency domain. The frequency
 * spectrum analysis is accomplished with an audio::MonitorSpectralNode.
 *
 * The plot is similar to a typical spectrogram, where the x-axis represents the linear
 * frequency bins (0 - samplerate / 2) and the y-axis is the magnitude of the frequency
 * bin in normalized decibels (0 - 100).
 *
 * author: Richard Eakin (2014)
 */

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/TextureFont.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/Utilities.h"

#include "../../common/AudioDrawUtils.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class InputAnalyzer : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );
	void update();
	void draw();

	void drawLabels();
	void printBinInfo( int mouseX );

	audio::InputDeviceNodeRef		mInputDeviceNode;
	audio::MonitorSpectralNodeRef	mMonitorSpectralNode;
	vector<float>					mMagSpectrum;

	SpectrumPlot					mSpectrumPlot;
	gl::TextureFontRef				mTextureFont;
};

void InputAnalyzer::setup()
{
	auto ctx = audio::Context::master();

	// The InputDeviceNode is platform-specific, so you create it using a special method on the Context:
	mInputDeviceNode = ctx->createInputDeviceNode();

	// By providing an FFT size double that of the window size, we 'zero-pad' the analysis data, which gives
	// an increase in resolution of the resulting spectrum data.
	auto monitorFormat = audio::MonitorSpectralNode::Format().fftSize( 2048 ).windowSize( 1024 );
	mMonitorSpectralNode = ctx->makeNode( new audio::MonitorSpectralNode( monitorFormat ) );

	mInputDeviceNode >> mMonitorSpectralNode;

	// InputDeviceNode (and all InputNode subclasses) need to be enabled()'s to process audio. So does the Context:
	mInputDeviceNode->enable();
	ctx->enable();

	getWindow()->setTitle( mInputDeviceNode->getDevice()->getName() );
}

void InputAnalyzer::mouseDown( MouseEvent event )
{
	if( mSpectrumPlot.getBounds().contains( event.getPos() ) )
		printBinInfo( event.getX() );
}

void InputAnalyzer::update()
{
	mSpectrumPlot.setBounds( Rectf( 40, 40, (float)getWindowWidth() - 40, (float)getWindowHeight() - 40 ) );

	// We copy the magnitude spectrum out from the Node on the main thread, once per update:
	mMagSpectrum = mMonitorSpectralNode->getMagSpectrum();
}

void InputAnalyzer::draw()
{
	gl::clear();
	gl::enableAlphaBlending();

	mSpectrumPlot.draw( mMagSpectrum );
	drawLabels();
}

void InputAnalyzer::drawLabels()
{
	if( ! mTextureFont )
		mTextureFont = gl::TextureFont::create( Font( Font::getDefault().getName(), 16 ) );

	gl::color( 0, 0.9f, 0.9f );

	// draw x-axis label
	string freqLabel = "Frequency (hertz)";
	mTextureFont->drawString( freqLabel, Vec2f( getWindowCenter().x - mTextureFont->measureString( freqLabel ).x / 2, (float)getWindowHeight() - 20 ) );

	// draw y-axis label
	string dbLabel = "Magnitude (decibels, linear)";
	gl::pushModelView();
		gl::translate( 30, getWindowCenter().y + mTextureFont->measureString( dbLabel ).x / 2 );
		gl::rotate( -90 );
		mTextureFont->drawString( dbLabel, Vec2f::zero() );
	gl::popModelView();
}

void InputAnalyzer::printBinInfo( int mouseX )
{
	size_t numBins = mMonitorSpectralNode->getFftSize() / 2;
	size_t bin = min( numBins - 1, size_t( ( numBins * ( mouseX - mSpectrumPlot.getBounds().x1 ) ) / mSpectrumPlot.getBounds().getWidth() ) );

	float binFreqWidth = mMonitorSpectralNode->getFreqForBin( 1 ) - mMonitorSpectralNode->getFreqForBin( 0 );
	float freq = mMonitorSpectralNode->getFreqForBin( bin );
	float mag = audio::linearToDecibel( mMagSpectrum[bin] );

	console() << "bin: " << bin << ", freqency (hertz): " << freq << " - " << freq + binFreqWidth << ", magnitude (decibels): " << mag << endl;
}

CINDER_APP_NATIVE( InputAnalyzer, RendererGl )
