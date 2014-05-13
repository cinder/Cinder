#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/audio2/Voice.h"
#include "cinder/audio2/ScopeNode.h"
#include "cinder/audio2/CinderAssert.h"
#include "cinder/audio2/Debug.h"

#include "Resources.h"
#include "../../common/AudioTestGui.h"
#include "../../../samples/common/AudioDrawUtils.h"

#include "cinder/audio2/Utilities.h"

const float DEFAULT_VOLUME = 0.4f;

using namespace ci;
using namespace ci::app;
using namespace std;

class VoiceTestApp : public AppNative {
public:
	void setup();
	void resize();
	void draw();

	void setupBasic();
	void setupBasicStereo();
	void setupDifferentFile();
	void setupScope();

	void setupUI();
	void processDrag( Vec2i pos );
	void processTap( Vec2i pos );

	audio2::VoiceRef		mVoice;
	audio2::ScopeNodeRef	mScope;

	vector<TestWidget *>	mWidgets;
	Button					mPlayButton, mEnableNoiseButton, mEnableSineButton;
	VSelector				mTestSelector;
	HSlider					mVolumeSlider;

	enum InputBus { SINE, NOISE };
};

void VoiceTestApp::setup()
{
	mVolumeSlider.set( DEFAULT_VOLUME );

	setupBasic();

	audio2::master()->printGraph();

	setupUI();
}

void VoiceTestApp::setupBasic()
{
	mVoice = audio2::Voice::create( audio2::load( loadResource( RES_DRAIN_OGG ) ) );
	mVoice->setVolume( mVolumeSlider.mValueScaled );
}

void VoiceTestApp::setupBasicStereo()
{
	mVoice = audio2::Voice::create( audio2::load( loadResource( RES_DRAIN_OGG ) ), audio2::Voice::Options().channels( 2 ) );
	mVoice->setVolume( mVolumeSlider.mValueScaled );
}

void VoiceTestApp::setupDifferentFile()
{
	mVoice = audio2::Voice::create( audio2::load( loadResource( RES_TONE_440L220R ) ) );
	mVoice->setVolume( mVolumeSlider.mValueScaled );

}
void VoiceTestApp::setupScope()
{
	auto sourceFile = audio2::load( loadResource( RES_DRAIN_OGG ) );
	mVoice = audio2::Voice::create( sourceFile, audio2::Voice::Options().connectToMaster( false ) );
	mVoice->setVolume( mVolumeSlider.mValueScaled );

	auto ctx = audio2::master();
	mScope = ctx->makeNode( new audio2::ScopeNode );

	mVoice->getOutputNode() >> mScope >> ctx->getOutput();
}

void VoiceTestApp::setupUI()
{
	mPlayButton = Button( false, "start" );
	mPlayButton.mBounds = Rectf( 0, 0, 200, 60 );
	mWidgets.push_back( &mPlayButton );

	mTestSelector.mSegments.push_back( "basic" );
	mTestSelector.mSegments.push_back( "basic stereo" );
	mTestSelector.mSegments.push_back( "file 2" );
	mTestSelector.mSegments.push_back( "scope" );
	mWidgets.push_back( &mTestSelector );

	mVolumeSlider.mTitle = "Volume";
	mVolumeSlider.mMax = 1;
	mVolumeSlider.set( mVoice->getVolume() );
	mWidgets.push_back( &mVolumeSlider );

//	mEnableSineButton.mIsToggle = true;
//	mEnableSineButton.mTitleNormal = "sine disabled";
//	mEnableSineButton.mTitleEnabled = "sine enabled";
//	mEnableSineButton.mBounds = Rectf( 0, 70, 200, 120 );
//	mWidgets.push_back( &mEnableSineButton );


	getWindow()->getSignalMouseDown().connect( [this] ( MouseEvent &event ) { processTap( event.getPos() ); } );
	getWindow()->getSignalMouseDrag().connect( [this] ( MouseEvent &event ) { processDrag( event.getPos() ); } );
	getWindow()->getSignalTouchesBegan().connect( [this] ( TouchEvent &event ) { processTap( event.getTouches().front().getPos() ); } );
	getWindow()->getSignalTouchesMoved().connect( [this] ( TouchEvent &event ) {
		for( const TouchEvent::Touch &touch : getActiveTouches() )
			processDrag( touch.getPos() );
	} );

	gl::enableAlphaBlending();
}

void VoiceTestApp::processDrag( Vec2i pos )
{
	if( mVolumeSlider.hitTest( pos ) )
		mVoice->setVolume( mVolumeSlider.mValueScaled );
}

void VoiceTestApp::processTap( Vec2i pos )
{
	if( mPlayButton.hitTest( pos ) )
		mVoice->start();

	size_t currentIndex = mTestSelector.mCurrentSectionIndex;
	if( mTestSelector.hitTest( pos ) && currentIndex != mTestSelector.mCurrentSectionIndex ) {
		string currentTest = mTestSelector.currentSection();
		CI_LOG_V( "selected: " << currentTest );

		if( currentTest == "basic" )
			setupBasic();
		if( currentTest == "basic stereo" )
			setupBasicStereo();
		else if( currentTest == "scope" )
			setupScope();
		else if( currentTest == "file 2" )
			setupDifferentFile();

		audio2::master()->printGraph();
	}
}

void VoiceTestApp::resize()
{
	mTestSelector.mBounds = Rectf( (float)getWindowWidth() * 0.6f, 0, (float)getWindowWidth(), (float)getWindowHeight() * 0.3f );

	const float padding = 10;
	const float sliderHeght = 50;
	Rectf sliderRect( padding, getWindowHeight() - sliderHeght - padding, getWindowCenter().x, getWindowHeight() - padding );
	mVolumeSlider.mBounds = sliderRect;
}

void VoiceTestApp::draw()
{
	gl::clear();

	if( mScope && mScope->getNumConnectedInputs() ) {
		Vec2f padding( 20, 4 );

		Rectf scopeRect( padding.x, padding.y, getWindowWidth() - padding.x, getWindowHeight() - padding.y );
		drawAudioBuffer( mScope->getBuffer(), scopeRect, true );
	}

	drawWidgets( mWidgets );
}

CINDER_APP_NATIVE( VoiceTestApp, RendererGl )
