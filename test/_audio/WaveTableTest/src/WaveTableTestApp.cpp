#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/audio2/GenNode.h"
#include "cinder/audio2/GainNode.h"
#include "cinder/audio2/ScopeNode.h"
#include "cinder/audio2/CinderAssert.h"
#include "cinder/audio2/Debug.h"

#include "cinder/audio2/Utilities.h"

#include "../../common/AudioTestGui.h"
#include "../../../samples/common/AudioDrawUtils.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class WaveTableTestApp : public AppNative {
public:
	void prepareSettings( Settings *settings );
	void setup();
	void update();
	void draw();

	void setupUI();
	void processDrag( Vec2i pos );
	void processTap( Vec2i pos );
	void keyDown( KeyEvent event );

	void setupTable();
	void setupOsc( audio2::WaveformType type );
	void setupPulse();
	void setupTriangleCalc();

	audio2::GainNodeRef				mGain;
	audio2::ScopeSpectralNodeRef	mScope;
	audio2::GenOscNodeRef			mGenOsc;
	audio2::GenPulseRef				mGenPulse;
	audio2::GenNodeRef				mGen;

	audio2::BufferDynamic		mTableCopy;

	vector<TestWidget *>	mWidgets;
	Button					mPlayButton;
	VSelector				mTestSelector;
	HSlider					mGainSlider, mFreqSlider, mFreqRampSlider, mPulseWidthSlider;
	TextInput				mNumPartialsInput, mTableSizeInput;
	SpectrumPlot			mSpectrumPlot;

};

void WaveTableTestApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1000, 800 );
}

void WaveTableTestApp::setup()
{
	auto ctx = audio2::master();
	mGain = ctx->makeNode( new audio2::GainNode );
	mGain->setValue( 0.075f );

	mScope = audio2::master()->makeNode( new audio2::ScopeSpectralNode( audio2::ScopeSpectralNode::Format().fftSize( 1024 ).windowSize( 2048 ) ) );
	mScope->setSmoothingFactor( 0 );

	mFreqSlider.set( 100 );

	setupOsc( audio2::WaveformType::SINE );
//	setupTable();
//	setupPulse();

	mGen >> mScope >> mGain >> ctx->getOutput();

	ctx->printGraph();

	setupUI();
}

void WaveTableTestApp::setupTable()
{
	auto ctx = audio2::master();

	auto gen = ctx->makeNode( new audio2::GenTableNode );
	gen->setFreq( mFreqSlider.mValueScaled );
	gen->enable();

	mGen = gen;
}

void WaveTableTestApp::setupOsc( audio2::WaveformType type )
{
	auto ctx = audio2::master();

	mGenOsc = ctx->makeNode( new audio2::GenOscNode( audio2::GenOscNode::Format().waveform( type ) ) );
	mGenOsc->setFreq( mFreqSlider.mValueScaled );
	mGenOsc->enable();

	mGen = mGenOsc;
}

void WaveTableTestApp::setupPulse()
{
	if( ! mGenPulse ) {
		mGenPulse = audio2::master()->makeNode( new audio2::GenPulse );
		mGenPulse->setFreq( mFreqSlider.mValueScaled );
		mGenPulse->enable();
	}

	if( mGenOsc )
		mGenOsc->disconnectAll();

	mGenPulse >> mScope;
	mGen = mGenPulse;

#if 1
	// pwm
	auto mod = audio2::master()->makeNode( new audio2::GenTableNode );

	audio2::master()->initializeNode( mod );

	vector<float> table( mod->getWaveTable()->getTableSize() );

	mod->getWaveTable()->copyTo( table.data() );

	// scale to [0.05 : 0.95]
	audio2::dsp::mul( table.data(), 0.45f, table.data(), table.size() );
	audio2::dsp::add( table.data(), 0.5f, table.data(), table.size() );

//	mTableCopy.setNumFrames( table.size() );
//	memmove( mTableCopy.getData(), table.data(), table.size() * sizeof( float ) );

	mod->getWaveTable()->copyFrom( table.data() );
	mod->setFreq( 0.6f );
	mod->enable();

	mGenPulse->getParamWidth()->setProcessor( mod );
#endif

	audio2::master()->printGraph();
}

// for comparison with GenOscNode's triangle spectra
void WaveTableTestApp::setupTriangleCalc()
{
	auto ctx = audio2::master();

	auto gen = ctx->makeNode( new audio2::GenTriangleNode );
	gen->setFreq( mFreqSlider.mValueScaled );
	gen->enable();

	mGen = gen;
}

void WaveTableTestApp::setupUI()
{
	Rectf buttonRect( (float)getWindowWidth() - 200, 10, (float)getWindowWidth(), 60 );
	mPlayButton = Button( true, "stopped", "playing" );
	mPlayButton.mBounds = buttonRect;
	mWidgets.push_back( &mPlayButton );

	mTestSelector.mSegments.push_back( "sine" );
	mTestSelector.mSegments.push_back( "square" );
	mTestSelector.mSegments.push_back( "sawtooth" );
	mTestSelector.mSegments.push_back( "triangle" );
	mTestSelector.mSegments.push_back( "pulse" );
	mTestSelector.mSegments.push_back( "sine (table)" );
	mTestSelector.mSegments.push_back( "triangle (calc)" );
	mTestSelector.mBounds = Rectf( (float)getWindowWidth() - 200, buttonRect.y2 + 10, (float)getWindowWidth(), buttonRect.y2 + 190 );
	mWidgets.push_back( &mTestSelector );

	// freq slider is longer, along top
	mFreqSlider.mBounds = Rectf( 10, 10, getWindowWidth() - 210, 40 );
	mFreqSlider.mTitle = "freq";
	mFreqSlider.mMin = -220;
	mFreqSlider.mMax = 5000;

	mFreqSlider.set( mGen->getFreq() );
	mWidgets.push_back( &mFreqSlider );

	Rectf sliderRect = mTestSelector.mBounds;
	sliderRect.y1 = sliderRect.y2 + 10;
	sliderRect.y2 = sliderRect.y1 + 30;
	mGainSlider.mBounds = sliderRect;
	mGainSlider.mTitle = "gain";
	mGainSlider.mMax = 1;
	mGainSlider.set( mGain->getValue() );
	mWidgets.push_back( &mGainSlider );

	sliderRect += Vec2f( 0, sliderRect.getHeight() + 10 );
	mFreqRampSlider.mBounds = sliderRect;
	mFreqRampSlider.mTitle = "freq ramp";
	mFreqRampSlider.mMin = -5;
	mFreqRampSlider.mMax = 10;
	mFreqRampSlider.set( 1.00f );
	mWidgets.push_back( &mFreqRampSlider );

	sliderRect += Vec2f( 0, sliderRect.getHeight() + 10 );
	mPulseWidthSlider.mBounds = sliderRect;
	mPulseWidthSlider.mTitle = "pulse width";
	mPulseWidthSlider.set( 0.05f );
	mWidgets.push_back( &mPulseWidthSlider );


	sliderRect += Vec2f( 0, sliderRect.getHeight() + 30 );
	mNumPartialsInput.mBounds = sliderRect;
	mNumPartialsInput.mTitle = "num partials";
//	mNumPartialsInput.setValue( mGen->getWaveformNumPartials() );
	mWidgets.push_back( &mNumPartialsInput );

	sliderRect += Vec2f( 0, sliderRect.getHeight() + 30 );
	mTableSizeInput.mBounds = sliderRect;
	mTableSizeInput.mTitle = "table size";
	mTableSizeInput.setValue( mGenOsc ? mGenOsc->getTableSize() : 0 );
	mWidgets.push_back( &mTableSizeInput );

	getWindow()->getSignalMouseDown().connect( [this] ( MouseEvent &event ) { processTap( event.getPos() ); } );
	getWindow()->getSignalMouseDrag().connect( [this] ( MouseEvent &event ) { processDrag( event.getPos() ); } );
	getWindow()->getSignalTouchesBegan().connect( [this] ( TouchEvent &event ) { processTap( event.getTouches().front().getPos() ); } );
	getWindow()->getSignalTouchesMoved().connect( [this] ( TouchEvent &event ) {
		for( const TouchEvent::Touch &touch : getActiveTouches() )
			processDrag( touch.getPos() );
	} );

	mSpectrumPlot.setBorderColor( ColorA( 0, 0.9f, 0, 1 ) );

	gl::enableAlphaBlending();
}

void WaveTableTestApp::processDrag( Vec2i pos )
{
	if( mGainSlider.hitTest( pos ) )
		mGain->getParam()->applyRamp( mGainSlider.mValueScaled, 0.03f );
	else if( mFreqSlider.hitTest( pos ) )
		mGen->getParamFreq()->applyRamp( mFreqSlider.mValueScaled, mFreqRampSlider.mValueScaled );
	else if( mFreqRampSlider.hitTest( pos ) ) {
		float val = mFreqRampSlider.mValueScaled;
		float wrapped = audio2::wrap( val );
		CI_LOG_V( "val: " << val << ", wrapped: " << wrapped );
	}
	else if( mGenPulse && mPulseWidthSlider.hitTest( pos ) ) {
//		mGenPulse->setWidth( mPulseWidthSlider.mValueScaled );
		mGenPulse->getParamWidth()->applyRamp( mPulseWidthSlider.mValueScaled, 0.5f );
	}

}

void WaveTableTestApp::processTap( Vec2i pos )
{
	auto ctx = audio2::master();
	size_t currentIndex = mTestSelector.mCurrentSectionIndex;

	if( mPlayButton.hitTest( pos ) )
		ctx->setEnabled( ! ctx->isEnabled() );
	else if( mNumPartialsInput.hitTest( pos ) ) {
	}
	else if( mTableSizeInput.hitTest( pos ) ) {
	}
	else if( mTestSelector.hitTest( pos ) && currentIndex != mTestSelector.mCurrentSectionIndex ) {
		string currentTest = mTestSelector.currentSection();
		CI_LOG_V( "selected: " << currentTest );

		mScope->disconnectAllInputs();

		if( currentTest == "sine" )
			setupOsc( audio2::WaveformType::SINE );
		else if( currentTest == "square" )
			setupOsc( audio2::WaveformType::SQUARE );
		else if( currentTest == "sawtooth" )
			setupOsc( audio2::WaveformType::SAWTOOTH );
		else if( currentTest == "triangle" )
			setupOsc( audio2::WaveformType::TRIANGLE );
		else if( currentTest == "pulse" )
			setupPulse();
		else if( currentTest == "sine (table)" )
			setupTable();
		else if( currentTest == "triangle (calc)" )
			setupTriangleCalc();

		mGen >> mScope;
	}
	else
		processDrag( pos );
}

void WaveTableTestApp::keyDown( KeyEvent event )
{
	TextInput *currentSelected = TextInput::getCurrentSelected();
	if( ! currentSelected )
		return;

	if( event.getCode() == KeyEvent::KEY_RETURN ) {
//		if( currentSelected == &mTableSizeInput ) {
//			int tableSize = currentSelected->getValue();
//			CI_LOG_V( "updating table size from: " << mGen->getTableSize() << " to: " << tableSize );
//			mGen->setWaveform( mGen->getWaveForm(), tableSize );
//			mTableCopy.setNumFrames( tableSize );
//			mGen->copyFromTable( mTableCopy.getData() );
//		}

	}
	else {
		if( event.getCode() == KeyEvent::KEY_BACKSPACE )
			currentSelected->processBackspace();
		else
			currentSelected->processChar( event.getChar() );
	}
}

void WaveTableTestApp::update()
{
	if( mGenOsc ) {
		mTableCopy.setNumFrames( mGenOsc->getTableSize() );
		mGenOsc->getWaveTable()->copyTo( mTableCopy.getData(), mGenOsc->getWaveTable()->calcBandlimitedTableIndex( mGenOsc->getFreq() ) );
	}
	if( mGenPulse )
		mPulseWidthSlider.set( mGenPulse->getWidth() );

	mFreqSlider.set( mGen->getFreq() );
}

void WaveTableTestApp::draw()
{
	gl::clear();

	const float padding = 10;
	const float scopeHeight = ( getWindowHeight() - padding * 4 - mFreqSlider.mBounds.y2 ) / 3;

	Rectf rect( padding, padding + mFreqSlider.mBounds.y2, getWindowWidth() - padding - 200, mFreqSlider.mBounds.y2 + scopeHeight + padding );
	drawAudioBuffer( mTableCopy, rect, true );

	rect += Vec2f( 0, scopeHeight + padding );
	drawAudioBuffer( mScope->getBuffer(), rect, true );

	rect += Vec2f( 0, scopeHeight + padding );
	mSpectrumPlot.setBounds( rect );
	mSpectrumPlot.draw( mScope->getMagSpectrum() );

	drawWidgets( mWidgets );
}

CINDER_APP_NATIVE( WaveTableTestApp, RendererGl )
