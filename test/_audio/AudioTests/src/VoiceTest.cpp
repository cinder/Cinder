#include "VoiceTest.h"

#include "cinder/app/App.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"
#include "cinder/CinderImGui.h"

#include "cinder/audio/Voice.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/Utilities.h"

#include "../../../../samples/_audio/common/AudioDrawUtils.h"

using namespace ci;
using namespace std;
namespace im = ImGui;

const string DEFAULT_SOUNDFILE = "tone440.ogg";

VoiceTest::VoiceTest()
{
	mSubTests = {
		"basic",
		"basic stereo",
		"file 2",
		"scope",
	};

	setupBasic();
}

void VoiceTest::openFile( const ci::fs::path &fullPath )
{
	mVoice = audio::Voice::create( audio::load( loadFile( fullPath ) ) );
	mVoice->setVolume( mVolume );
}

void VoiceTest::setupSubTest( const std::string &testName )
{
	if( testName == "basic" )
		setupBasic();
	if( testName == "basic stereo" )
		setupBasicStereo();
	else if( testName == "scope" )
		setupScope();
	else if( testName == "file 2" )
		setupDifferentFile();
}

void VoiceTest::setupBasic()
{
	mVoice = audio::Voice::create( audio::load( app::loadAsset( DEFAULT_SOUNDFILE ) ) );
	mVoice->setVolume( mVolume );
}

void VoiceTest::setupBasicStereo()
{
	mVoice = audio::Voice::create( audio::load( app::loadAsset( DEFAULT_SOUNDFILE ) ), audio::Voice::Options().channels( 2 ) );
	mVoice->setVolume( mVolume );
}

void VoiceTest::setupDifferentFile()
{
	mVoice = audio::Voice::create( audio::load( app::loadAsset( "tone440L220R.mp3" ) ) );
	mVoice->setVolume( mVolume );

}
void VoiceTest::setupScope()
{
	auto sourceFile = audio::load( app::loadAsset( DEFAULT_SOUNDFILE ) );
	mVoice = audio::Voice::create( sourceFile, audio::Voice::Options().connectToMaster( false ) );
	mVoice->setVolume( mVolume );

	auto ctx = audio::master();
	mMonitor = ctx->makeNode( new audio::MonitorNode );

	mVoice->getOutputNode() >> mMonitor >> ctx->getOutput();
}

void VoiceTest::draw()
{
	if( mMonitor && mMonitor->getNumConnectedInputs() ) {
		vec2 padding( 20, 4 );

		Rectf scopeRect( padding.x, padding.y, app::getWindowWidth() - padding.x, app::getWindowHeight() - padding.y );
		drawAudioBuffer( mMonitor->getBuffer(), scopeRect, true );
	}
}

// -----------------------------------------------------------------------
// UI
// -----------------------------------------------------------------------

void VoiceTest::updateUI()
{
	if( im::Button( "start" ) ) {
		mVoice->start();
	}
	if( im::SliderFloat( "volume", &mVolume, 0, 1 ) ) {
		mVoice->setVolume( mVolume );
	}

	if( im::ListBox( "sub-tests", &mCurrentSubTest, mSubTests, mSubTests.size() ) ) {
		setupSubTest( mSubTests[mCurrentSubTest] );
	}


}