#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Rand.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"

#include "cinder/audio/GenNode.h"
#include "cinder/audio/GainNode.h"
#include "cinder/audio/MonitorNode.h"

#include "cinder/audio/Utilities.h"

#include "../../common/AudioTestGui.h"
#include "../../../../samples/_audio/common/AudioDrawUtils.h"

// FIXME: type switching should clear the cached wavetable

using namespace ci;
using namespace ci::app;
using namespace std;

enum GenType { SINE, TRIANGLE, OSC_SINE, OSC_SAW, OSC_SQUARE, OSC_TRIANGLE };

class StressTestApp : public App {
public:
	void setup() override;
	void draw() override;
	void keyDown( KeyEvent event ) override;

	void setupUI();
	void processDrag( ivec2 pos );
	void processTap( ivec2 pos );

	void addGens();
	void removeGens();
	void clearGens();

	audio::GenNodeRef	makeSelectedGenType();
	audio::GenNodeRef	makeOsc( audio::WaveformType type );

	audio::GainNodeRef				mGain;
	audio::MonitorSpectralNodeRef	mMonitor;
	audio::WaveTable2dRef	mWaveTable;
	vector<audio::GenNodeRef>		mGenBank;

	vector<TestWidget *>	mWidgets;
	Button					mPlayButton, mAddGens, mRemoveGens, mClearGens;
	VSelector				mTestSelector;
	HSlider					mGainSlider;
	TextInput				mAddIncrInput;
	SpectrumPlot			mSpectrumPlot;

	bool					mEnableDrawing;
	size_t					mAddIncr;
	GenType					mSelectedGenType;
};

void StressTestApp::setup()
{
	mAddIncr = 1;
	mEnableDrawing = true;
	mSelectedGenType = OSC_SQUARE;

	auto ctx = audio::master();
	mGain = ctx->makeNode( new audio::GainNode );
	mGain->setValue( 0.1f );

	mMonitor = audio::master()->makeNode( new audio::MonitorSpectralNode( audio::MonitorSpectralNode::Format().fftSize( 1024 ).windowSize( 2048 ) ) );
	mMonitor->setSmoothingFactor( 0.4 );

	mGain >> mMonitor >> ctx->getOutput();

	addGens();

	setupUI();

}

void StressTestApp::addGens()
{
	auto ctx = audio::master();

	for( size_t i = 0; i < mAddIncr; i++ ) {
		auto gen = makeSelectedGenType();
		gen->setFreq( audio::midiToFreq( randInt( 40, 60 ) ) );

		gen->connect( mGain );
		gen->enable();

		mGenBank.push_back( gen );
	}

	PRINT_GRAPH( ctx );
	CI_LOG_V( "gen count: " << mGenBank.size() );
}

void StressTestApp::removeGens()
{
	for( size_t i = 0; i < mAddIncr; i++ ) {
		mGenBank.back()->disconnectAll();
		mGenBank.pop_back();
	}

	PRINT_GRAPH( audio::master() );
	CI_LOG_V( "gen count: " << mGenBank.size() );
}

void StressTestApp::clearGens()
{
	while( ! mGenBank.empty() ) {
		mGenBank.back()->disconnectAll();
		mGenBank.pop_back();
	}

	PRINT_GRAPH( audio::master() );
	CI_LOG_V( "gen count: " << mGenBank.size() );
}

audio::GenNodeRef StressTestApp::makeSelectedGenType()
{
	switch( mSelectedGenType ) {
		case SINE: return audio::master()->makeNode( new audio::GenSineNode );
		case TRIANGLE: return audio::master()->makeNode( new audio::GenTriangleNode );
		case OSC_SINE: return makeOsc( audio::WaveformType::SINE );
		case OSC_SAW: return makeOsc( audio::WaveformType::SAWTOOTH );
		case OSC_SQUARE: return makeOsc( audio::WaveformType::SQUARE );
		case OSC_TRIANGLE: return makeOsc( audio::WaveformType::TRIANGLE );

		default: CI_ASSERT_NOT_REACHABLE();
	}

	return audio::GenNodeRef();
}

audio::GenNodeRef StressTestApp::makeOsc( audio::WaveformType type )
{
	auto ctx = audio::master();
	auto result = ctx->makeNode( new audio::GenOscNode( type ) );

	if( mWaveTable )
		result->setWaveTable( mWaveTable );
	else {
		ctx->initializeNode( result );

		mWaveTable = result->getWaveTable();
	}

	return result;
}

void StressTestApp::setupUI()
{

	Rectf buttonRect( (float)getWindowWidth() - 200, 10, (float)getWindowWidth(), 60 );
	mPlayButton = Button( true, "stopped", "playing" );
	mPlayButton.mBounds = buttonRect;
	mWidgets.push_back( &mPlayButton );

	buttonRect += vec2( 0, buttonRect.getHeight() + 10 );
	mAddGens = Button( false, "add gens" );
	mAddGens.mBounds = buttonRect;
	mWidgets.push_back( &mAddGens );

	buttonRect += vec2( 0, buttonRect.getHeight() + 10 );
	mRemoveGens = Button( false, "remove gens" );
	mRemoveGens.mBounds = buttonRect;
	mWidgets.push_back( &mRemoveGens );

	buttonRect += vec2( 0, buttonRect.getHeight() + 10 );
	mClearGens = Button( false, "clear gens" );
	mClearGens.mBounds = buttonRect;
	mWidgets.push_back( &mClearGens );

	mTestSelector.mSegments.push_back( "sine" );
	mTestSelector.mSegments.push_back( "triangle" );
	mTestSelector.mSegments.push_back( "osc sine" );
	mTestSelector.mSegments.push_back( "osc sawtooth" );
	mTestSelector.mSegments.push_back( "osc square" );
	mTestSelector.mSegments.push_back( "osc triangle" );
	mTestSelector.mBounds = Rectf( (float)getWindowWidth() - 200, buttonRect.y2 + 10, (float)getWindowWidth(), buttonRect.y2 + 190 );
	mWidgets.push_back( &mTestSelector );

	Rectf sliderRect = mTestSelector.mBounds;
	sliderRect.y1 = sliderRect.y2 + 10;
	sliderRect.y2 = sliderRect.y1 + 30;
	mGainSlider.mBounds = sliderRect;
	mGainSlider.mTitle = "gain";
	mGainSlider.mMax = 1;
	mGainSlider.set( mGain->getValue() );
	mWidgets.push_back( &mGainSlider );

	sliderRect += vec2( 0, sliderRect.getHeight() + 30 );
	mAddIncrInput.mBounds = sliderRect;
	mAddIncrInput.mFormat = TextInput::Format::NUMERICAL;
	mAddIncrInput.mTitle = "add incr";
	mAddIncrInput.setValue( mAddIncr );
	mWidgets.push_back( &mAddIncrInput );

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

void StressTestApp::processDrag( ivec2 pos )
{
	if( mGainSlider.hitTest( pos ) )
		mGain->getParam()->applyRamp( mGainSlider.mValueScaled, 0.03f );

}

void StressTestApp::processTap( ivec2 pos )
{
	auto ctx = audio::master();
	size_t currentIndex = mTestSelector.mCurrentSectionIndex;

	if( mPlayButton.hitTest( pos ) )
		ctx->setEnabled( ! ctx->isEnabled() );
	else if( mAddGens.hitTest( pos ) )
		addGens();
	else if( mRemoveGens.hitTest( pos ) )
		removeGens();
	else if( mClearGens.hitTest( pos ) )
		clearGens();
	else if( mAddIncrInput.hitTest( pos ) ) {
	}
	else if( mTestSelector.hitTest( pos ) && currentIndex != mTestSelector.mCurrentSectionIndex ) {
		string currentTest = mTestSelector.currentSection();
		CI_LOG_V( "selected: " << currentTest );

		if( currentTest == "sine" )
			mSelectedGenType = SINE;
		else if( currentTest == "triangle" )
			mSelectedGenType = TRIANGLE;
		else if( currentTest == "osc sine" ) {
			mSelectedGenType = OSC_SINE;
			mWaveTable.reset();
		}
		else if( currentTest == "osc sawtooth" ) {
			mSelectedGenType = OSC_SAW;
			mWaveTable.reset();
		}
		else if( currentTest == "osc square" ) {
			mSelectedGenType = OSC_SQUARE;
			mWaveTable.reset();
		}
		else if( currentTest == "osc triangle" ) {
			mSelectedGenType = OSC_TRIANGLE;
			mWaveTable.reset();
		}
	}
	else
		processDrag( pos );
}

void StressTestApp::keyDown( KeyEvent event )
{
	TextInput *currentSelected = TextInput::getCurrentSelected();
	if( currentSelected ) {
		if( event.getCode() == KeyEvent::KEY_RETURN ) {
			if( currentSelected == &mAddIncrInput ) {
				mAddIncr = currentSelected->getValue();
				CI_LOG_V( "add incriment: " << mAddIncr );
			}
			TextInput::disableAll();
		}
		else {
			if( event.getCode() == KeyEvent::KEY_BACKSPACE )
				currentSelected->processBackspace();
			else
				currentSelected->processChar( event.getChar() );
		}

	}
	else {
		if( event.getChar() == 'g' )
			mEnableDrawing = ! mEnableDrawing;
		else if( event.getChar() == 'a' )
			addGens();
	}
}

void StressTestApp::draw()
{
	gl::clear();

	if( ! mEnableDrawing )
		return;

	const float padding = 10;
	const float scopeHeight = ( getWindowHeight() - padding * 3 ) / 2;

	Rectf rect( padding, padding, getWindowWidth() - padding - 200, scopeHeight + padding );

	drawAudioBuffer( mMonitor->getBuffer(), rect, true );

	rect += vec2( 0, scopeHeight + padding );
	mSpectrumPlot.setBounds( rect );
	mSpectrumPlot.draw( mMonitor->getMagSpectrum() );

	drawWidgets( mWidgets );

	string countStr = string( "Gen count: " ) + to_string( mGenBank.size() );
	getTestWidgetTexFont()->drawString( countStr, vec2( mAddIncrInput.mBounds.x1, mAddIncrInput.mBounds.y2 + padding + getTestWidgetTexFont()->getFont().getAscent() + getTestWidgetTexFont()->getFont().getDescent() ) );
}

CINDER_APP( StressTestApp, RendererGl, []( App::Settings *settings ) {
	settings->setWindowSize( 1000, 600 );
} )
