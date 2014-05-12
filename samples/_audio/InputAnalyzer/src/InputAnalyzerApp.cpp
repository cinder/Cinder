#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/TextureFont.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/ScopeNode.h"
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

	audio::InputDeviceNodeRef	mInputDeviceNode;
	audio::ScopeSpectralNodeRef	mScopeSpectralNode;
	vector<float>				mMagSpectrum;

	SpectrumPlot				mSpectrumPlot;
	gl::TextureFontRef			mTextureFont;
};

void InputAnalyzer::setup()
{
	auto ctx = audio::Context::master();

	mInputDeviceNode = ctx->createInputDeviceNode();

	auto scopeFmt = audio::ScopeSpectralNode::Format().fftSize( 2048 ).windowSize( 1024 );
	mScopeSpectralNode = ctx->makeNode( new audio::ScopeSpectralNode( scopeFmt ) );

	mInputDeviceNode >> mScopeSpectralNode;

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

	mMagSpectrum = mScopeSpectralNode->getMagSpectrum();
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
	string dbLabel = "Magnitude (decibels)";
	gl::pushModelView();
		gl::translate( 30, getWindowCenter().y + mTextureFont->measureString( dbLabel ).x / 2 );
		gl::rotate( -90 );
		mTextureFont->drawString( dbLabel, Vec2f::zero() );
	gl::popModelView();
}

void InputAnalyzer::printBinInfo( int mouseX )
{
	size_t numBins = mScopeSpectralNode->getFftSize() / 2;
	size_t bin = min( numBins - 1, size_t( ( numBins * ( mouseX - mSpectrumPlot.getBounds().x1 ) ) / mSpectrumPlot.getBounds().getWidth() ) );

	float binFreqWidth = mScopeSpectralNode->getFreqForBin( 1 ) - mScopeSpectralNode->getFreqForBin( 0 );
	float freq = mScopeSpectralNode->getFreqForBin( bin );
	float mag = audio::toDecibels( mMagSpectrum[bin] );

	console() << "bin: " << bin << ", freqency (hertz): " << freq << " - " << freq + binFreqWidth << ", magnitude (decibels): " << mag << endl;
}

CINDER_APP_NATIVE( InputAnalyzer, RendererGl )
