#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"

#include "cinder/audio/Voice.h"
#include "cinder/audio/MonitorNode.h"

#include "Resources.h"
#include "../../common/AudioTestGui.h"
#include "../../../../samples/_audio/common/AudioDrawUtils.h"

#include "cinder/audio/Utilities.h"

const float DEFAULT_VOLUME = 0.4f;

using namespace ci;
using namespace ci::app;
using namespace std;

class VoiceTestApp : public App {
public:
	void setup() override;
	void fileDrop( FileDropEvent event ) override;
	void resize() override;
	void draw() override;

	void setupBasic();
	void setupBasicStereo();
	void setupDifferentFile();
	void setupScope();

	void setupUI();
	void processDrag( ivec2 pos );
	void processTap( ivec2 pos );

	audio::VoiceRef		mVoice;
	audio::MonitorNodeRef	mMonitor;

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
	setupUI();

	PRINT_GRAPH( audio::master() );
	CI_LOG_I( "complete. context samplerate: " << audio::master()->getSampleRate() );
}

void VoiceTestApp::fileDrop( FileDropEvent event )
{
	const fs::path &filePath = event.getFile( 0 );
	CI_LOG_I( "File dropped: " << filePath );

	mVoice = audio::Voice::create( audio::load( loadFile( filePath ) ) );
	mVoice->setVolume( mVolumeSlider.mValueScaled );
}

void VoiceTestApp::setupBasic()
{
	mVoice = audio::Voice::create( audio::load( loadResource( RES_DRAIN_OGG ) ) );
	mVoice->setVolume( mVolumeSlider.mValueScaled );
}

void VoiceTestApp::setupBasicStereo()
{
	mVoice = audio::Voice::create( audio::load( loadResource( RES_DRAIN_OGG ) ), audio::Voice::Options().channels( 2 ) );
	mVoice->setVolume( mVolumeSlider.mValueScaled );
}

void VoiceTestApp::setupDifferentFile()
{
	mVoice = audio::Voice::create( audio::load( loadResource( RES_TONE_440L220R ) ) );
	mVoice->setVolume( mVolumeSlider.mValueScaled );

}
void VoiceTestApp::setupScope()
{
	auto sourceFile = audio::load( loadResource( RES_DRAIN_OGG ) );
	mVoice = audio::Voice::create( sourceFile, audio::Voice::Options().connectToMaster( false ) );
	mVoice->setVolume( mVolumeSlider.mValueScaled );

	auto ctx = audio::master();
	mMonitor = ctx->makeNode( new audio::MonitorNode );

	mVoice->getOutputNode() >> mMonitor >> ctx->getOutput();
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

void VoiceTestApp::processDrag( ivec2 pos )
{
	if( mVolumeSlider.hitTest( pos ) )
		mVoice->setVolume( mVolumeSlider.mValueScaled );
}

void VoiceTestApp::processTap( ivec2 pos )
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

		PRINT_GRAPH( audio::master() );
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

	if( mMonitor && mMonitor->getNumConnectedInputs() ) {
		vec2 padding( 20, 4 );

		Rectf scopeRect( padding.x, padding.y, getWindowWidth() - padding.x, getWindowHeight() - padding.y );
		drawAudioBuffer( mMonitor->getBuffer(), scopeRect, true );
	}

	drawWidgets( mWidgets );
}

CINDER_APP( VoiceTestApp, RendererGl )
