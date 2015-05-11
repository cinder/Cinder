#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"

#include "cinder/audio/GenNode.h"
#include "cinder/audio/GainNode.h"
#include "cinder/audio/MonitorNode.h"

#include "cinder/audio/Utilities.h"

#include "../../common/AudioTestGui.h"
#include "../../../../samples/_audio/common/AudioDrawUtils.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class WaveTableTestApp : public App {
public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyDown( KeyEvent event ) override;

	void setupUI();
	void processDrag( ivec2 pos );
	void processTap( ivec2 pos );

	void setupTable();
	void setupOsc( audio::WaveformType type );
	void setupPulse();
	void setupTriangleCalc();

	audio::GainNodeRef				mGain;
	audio::MonitorSpectralNodeRef	mMonitor;
	audio::GenOscNodeRef			mGenOsc;
	audio::GenPulseNodeRef			mGenPulse;
	audio::GenNodeRef				mGen;

	audio::BufferDynamic		mTableCopy;

	vector<TestWidget *>	mWidgets;
	Button					mPlayButton;
	VSelector				mTestSelector;
	HSlider					mGainSlider, mFreqSlider, mFreqRampSlider, mPulseWidthSlider;
	TextInput				mNumPartialsInput, mTableSizeInput;
	SpectrumPlot			mSpectrumPlot;

};

void WaveTableTestApp::setup()
{
	auto ctx = audio::master();
	mGain = ctx->makeNode( new audio::GainNode );
	mGain->setValue( 0.075f );

	mMonitor = audio::master()->makeNode( new audio::MonitorSpectralNode( audio::MonitorSpectralNode::Format().fftSize( 1024 ).windowSize( 2048 ) ) );
	mMonitor->setSmoothingFactor( 0 );

	mFreqSlider.set( 100 );

	setupOsc( audio::WaveformType::SINE );
//	setupTable();
//	setupPulse();

	mGen >> mMonitor >> mGain >> ctx->getOutput();

	PRINT_GRAPH( ctx );

	setupUI();
}

void WaveTableTestApp::setupTable()
{
	auto ctx = audio::master();

	auto gen = ctx->makeNode( new audio::GenTableNode );
	gen->setFreq( mFreqSlider.mValueScaled );
	gen->enable();

	mGen = gen;
}

void WaveTableTestApp::setupOsc( audio::WaveformType type )
{
	auto ctx = audio::master();

	mGenOsc = ctx->makeNode( new audio::GenOscNode( type ) );
	mGenOsc->setFreq( mFreqSlider.mValueScaled );
	mGenOsc->enable();

	mGen = mGenOsc;
}

void WaveTableTestApp::setupPulse()
{
	if( ! mGenPulse ) {
		mGenPulse = audio::master()->makeNode( new audio::GenPulseNode );
		mGenPulse->setFreq( mFreqSlider.mValueScaled );
		mGenPulse->enable();
	}

	if( mGenOsc )
		mGenOsc->disconnectAll();

	mGenPulse >> mMonitor;
	mGen = mGenPulse;

#if 1
	// pwm
	auto mod = audio::master()->makeNode( new audio::GenTableNode );

	audio::master()->initializeNode( mod );

	vector<float> table( mod->getWaveTable()->getTableSize() );

	mod->getWaveTable()->copyTo( table.data() );

	// scale to [0.05 : 0.95]
	audio::dsp::mul( table.data(), 0.45f, table.data(), table.size() );
	audio::dsp::add( table.data(), 0.5f, table.data(), table.size() );

//	mTableCopy.setNumFrames( table.size() );
//	memmove( mTableCopy.getData(), table.data(), table.size() * sizeof( float ) );

	mod->getWaveTable()->copyFrom( table.data() );
	mod->setFreq( 0.6f );
	mod->enable();

	mGenPulse->getParamWidth()->setProcessor( mod );
#endif

	PRINT_GRAPH( audio::master() );
}

// for comparison with GenOscNode's triangle spectra
void WaveTableTestApp::setupTriangleCalc()
{
	auto ctx = audio::master();

	auto gen = ctx->makeNode( new audio::GenTriangleNode );
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

	sliderRect += vec2( 0, sliderRect.getHeight() + 10 );
	mFreqRampSlider.mBounds = sliderRect;
	mFreqRampSlider.mTitle = "freq ramp";
	mFreqRampSlider.mMin = -5;
	mFreqRampSlider.mMax = 10;
	mFreqRampSlider.set( 0.2f );
	mWidgets.push_back( &mFreqRampSlider );

	sliderRect += vec2( 0, sliderRect.getHeight() + 10 );
	mPulseWidthSlider.mBounds = sliderRect;
	mPulseWidthSlider.mTitle = "pulse width";
	mPulseWidthSlider.set( 0.05f );
	mWidgets.push_back( &mPulseWidthSlider );


	sliderRect += vec2( 0, sliderRect.getHeight() + 30 );
	mNumPartialsInput.mBounds = sliderRect;
	mNumPartialsInput.mTitle = "num partials";
	mWidgets.push_back( &mNumPartialsInput );

	sliderRect += vec2( 0, sliderRect.getHeight() + 30 );
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

void WaveTableTestApp::processDrag( ivec2 pos )
{
	if( mGainSlider.hitTest( pos ) )
		mGain->getParam()->applyRamp( mGainSlider.mValueScaled, 0.03f );
	else if( mFreqSlider.hitTest( pos ) )
		mGen->getParamFreq()->applyRamp( mFreqSlider.mValueScaled, mFreqRampSlider.mValueScaled );
	else if( mFreqRampSlider.hitTest( pos ) ) {
	}
	else if( mGenPulse && mPulseWidthSlider.hitTest( pos ) ) {
//		mGenPulse->setWidth( mPulseWidthSlider.mValueScaled );
		mGenPulse->getParamWidth()->applyRamp( mPulseWidthSlider.mValueScaled, 0.5f );
	}

}

void WaveTableTestApp::processTap( ivec2 pos )
{
	auto ctx = audio::master();
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

		mMonitor->disconnectAllInputs();

		if( currentTest == "sine" )
			setupOsc( audio::WaveformType::SINE );
		else if( currentTest == "square" )
			setupOsc( audio::WaveformType::SQUARE );
		else if( currentTest == "sawtooth" )
			setupOsc( audio::WaveformType::SAWTOOTH );
		else if( currentTest == "triangle" )
			setupOsc( audio::WaveformType::TRIANGLE );
		else if( currentTest == "pulse" )
			setupPulse();
		else if( currentTest == "sine (table)" )
			setupTable();
		else if( currentTest == "triangle (calc)" )
			setupTriangleCalc();

		mGen >> mMonitor;
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

	rect += vec2( 0, scopeHeight + padding );
	drawAudioBuffer( mMonitor->getBuffer(), rect, true );

	rect += vec2( 0, scopeHeight + padding );
	mSpectrumPlot.setBounds( rect );
	mSpectrumPlot.draw( mMonitor->getMagSpectrum() );

	drawWidgets( mWidgets );
}

CINDER_APP( WaveTableTestApp, RendererGl, []( App::Settings *settings ) {
	settings->setWindowSize( 1000, 800 );
} )
