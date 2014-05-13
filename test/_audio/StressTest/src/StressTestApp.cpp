#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#include "cinder/audio2/GenNode.h"
#include "cinder/audio2/GainNode.h"
#include "cinder/audio2/ScopeNode.h"
#include "cinder/audio2/CinderAssert.h"
#include "cinder/audio2/Debug.h"

#include "cinder/audio2/Utilities.h"

#include "../../common/AudioTestGui.h"
#include "../../../samples/common/AudioDrawUtils.h"

// FIXME: type switching should clear the cached wavetable

using namespace ci;
using namespace ci::app;
using namespace std;

enum GenType { SINE, TRIANGLE, OSC_SINE, OSC_SAW, OSC_SQUARE, OSC_TRIANGLE };

class StressTestApp : public AppNative {
public:
	void prepareSettings( Settings *settings );
	void setup();
	void draw();

	void setupUI();
	void processDrag( Vec2i pos );
	void processTap( Vec2i pos );
	void keyDown( KeyEvent event );

	void addGens();
	void removeGens();
	void clearGens();

	audio2::GenNodeRef	makeSelectedGenType();
	audio2::GenNodeRef	makeOsc( audio2::WaveformType type );

	audio2::GainNodeRef				mGain;
	audio2::ScopeSpectralNodeRef	mScope;
	audio2::WaveTable2dRef	mWaveTable;
	vector<audio2::GenNodeRef>		mGenBank;

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

void StressTestApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1000, 600 );
}

void StressTestApp::setup()
{
	mAddIncr = 1;
	mEnableDrawing = true;
	mSelectedGenType = OSC_SQUARE;

	auto ctx = audio2::master();
	mGain = ctx->makeNode( new audio2::GainNode );
	mGain->setValue( 0.1f );

	mScope = audio2::master()->makeNode( new audio2::ScopeSpectralNode( audio2::ScopeSpectralNode::Format().fftSize( 1024 ).windowSize( 2048 ) ) );
	mScope->setSmoothingFactor( 0.4 );

	mGain >> mScope >> ctx->getOutput();

	addGens();

	setupUI();

}

void StressTestApp::addGens()
{
	auto ctx = audio2::master();

	for( size_t i = 0; i < mAddIncr; i++ ) {
		auto gen = makeSelectedGenType();
		gen->setFreq( audio2::toFreq( randInt( 40, 60 ) ) );

		gen->connect( mGain );
		gen->enable();

		mGenBank.push_back( gen );
	}

	ctx->printGraph();
	CI_LOG_V( "gen count: " << mGenBank.size() );
}

void StressTestApp::removeGens()
{
	for( size_t i = 0; i < mAddIncr; i++ ) {
		mGenBank.back()->disconnectAll();
		mGenBank.pop_back();
	}

	audio2::master()->printGraph();
	CI_LOG_V( "gen count: " << mGenBank.size() );
}

void StressTestApp::clearGens()
{
	while( ! mGenBank.empty() ) {
		mGenBank.back()->disconnectAll();
		mGenBank.pop_back();
	}

	audio2::master()->printGraph();
	CI_LOG_V( "gen count: " << mGenBank.size() );
}

audio2::GenNodeRef StressTestApp::makeSelectedGenType()
{
	switch( mSelectedGenType ) {
		case SINE: return audio2::master()->makeNode( new audio2::GenSineNode );
		case TRIANGLE: return audio2::master()->makeNode( new audio2::GenTriangleNode );
		case OSC_SINE: return makeOsc( audio2::WaveformType::SINE );
		case OSC_SAW: return makeOsc( audio2::WaveformType::SAWTOOTH );
		case OSC_SQUARE: return makeOsc( audio2::WaveformType::SQUARE );
		case OSC_TRIANGLE: return makeOsc( audio2::WaveformType::TRIANGLE );

		default: CI_ASSERT_NOT_REACHABLE();
	}

	return audio2::GenNodeRef();
}

audio2::GenNodeRef StressTestApp::makeOsc( audio2::WaveformType type )
{
	auto ctx = audio2::master();
	auto result = ctx->makeNode( new audio2::GenOscNode( audio2::GenOscNode::Format().waveform( type ) ) );

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

	buttonRect += Vec2f( 0, buttonRect.getHeight() + 10 );
	mAddGens = Button( false, "add gens" );
	mAddGens.mBounds = buttonRect;
	mWidgets.push_back( &mAddGens );

	buttonRect += Vec2f( 0, buttonRect.getHeight() + 10 );
	mRemoveGens = Button( false, "remove gens" );
	mRemoveGens.mBounds = buttonRect;
	mWidgets.push_back( &mRemoveGens );

	buttonRect += Vec2f( 0, buttonRect.getHeight() + 10 );
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

	sliderRect += Vec2f( 0, sliderRect.getHeight() + 30 );
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

void StressTestApp::processDrag( Vec2i pos )
{
	if( mGainSlider.hitTest( pos ) )
		mGain->getParam()->applyRamp( mGainSlider.mValueScaled, 0.03f );

}

void StressTestApp::processTap( Vec2i pos )
{
	auto ctx = audio2::master();
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

	drawAudioBuffer( mScope->getBuffer(), rect, true );

	rect += Vec2f( 0, scopeHeight + padding );
	mSpectrumPlot.setBounds( rect );
	mSpectrumPlot.draw( mScope->getMagSpectrum() );

	drawWidgets( mWidgets );

	string countStr = string( "Gen count: " ) + to_string( mGenBank.size() );
	getTestWidgetTexFont()->drawString( countStr, Vec2f( mAddIncrInput.mBounds.x1, mAddIncrInput.mBounds.y2 + padding + getTestWidgetTexFont()->getFont().getAscent() + getTestWidgetTexFont()->getFont().getDescent() ) );
}

CINDER_APP_NATIVE( StressTestApp, RendererGl )
