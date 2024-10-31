#include "SamplePlayerTest.h"

#include "cinder/app/App.h"
#include "cinder/Timeline.h"
#include "cinder/Timer.h"
#include "cinder/Log.h"
#include "cinder/gl/draw.h"
#include "cinder/CinderImGui.h"

#define TEST_STEREO_INPUT_PANNING 0

using namespace ci;
using namespace std;
namespace im = ImGui;

SamplePlayerTest::SamplePlayerTest()
{
	mSubTests = {
		"BufferPlayerNode",
		"FilePlayerNode",
		"recorder",
	};

	mUnderrunFade = mOverrunFade = mRecorderOverrunFade = 0;
	mSamplePlayerNodeEnabledState = false;

	printSupportedExtensions();

	setSourceFile( app::loadAsset( "tone440L220R.wav" ) );

	auto ctx = audio::master();

	mPan = ctx->makeNode( new audio::Pan2dNode() );

	// TODO: make a runtime bool and move this to makeNodes() method that can be triggered from gui
#if TEST_STEREO_INPUT_PANNING
	mPan->setStereoInputModeEnabled( true );
#endif

	mGain = ctx->makeNode( new audio::GainNode() );
	mGain->setValue( 0.6f );

	mGain >> mPan >> ctx->getOutput();

	setupBufferPlayerNode();
//	setupFilePlayerNode();

	//ctx->enable();

	//CI_LOG_V( "context samplerate: " << ctx->getSampleRate() );
}

void SamplePlayerTest::setupSubTest( const string &testName )
{
	auto ctx = audio::master();

	bool enabled = ctx->isEnabled();
	ctx->disable();
	ctx->disconnectAllNodes();

	if( testName == "BufferPlayerNode" )
		setupBufferPlayerNode();
	else if( testName == "FilePlayerNode" )
		setupFilePlayerNode();
	else if( testName == "recorder" )
		setupBufferRecorderNode();

	ctx->setEnabled( enabled );

	CI_LOG_I( "Finished setup for test: " << testName );
}

void SamplePlayerTest::setupBufferPlayerNode()
{
	auto bufferPlayer = audio::master()->makeNode( new audio::BufferPlayerNode() );

	auto loadFn = [bufferPlayer, this] {
		bufferPlayer->loadBuffer( mSourceFile->clone() );
		CI_LOG_I( "loaded source buffer, frames: " << bufferPlayer->getBuffer()->getNumFrames() );
	};

	auto connectFn = [bufferPlayer, this] {
		bool loopEnabled = mSamplePlayerNode ? mSamplePlayerNode->isLoopEnabled() : false;

		mGain->disconnectAllInputs();
		mSamplePlayerNode = bufferPlayer;
		mSamplePlayerNode >> mGain >> mPan >> audio::master()->getOutput();

		mSamplePlayerNode->setLoopEnabled( loopEnabled );
		mSamplePlayerNode->setLoopBeginTime( 0 );
		mSamplePlayerNode->setLoopEndTime( mSamplePlayerNode->getNumSeconds() );
	};

	if( mLoadAsync ) {
		mWaveformPlot.clear();
		mAsyncLoadFuture = std::async( [=] {
			loadFn();
			app::App::get()->dispatchAsync( [=] {
				mWaveformPlot.load( bufferPlayer->getBuffer(), app::getWindowBounds() );
				connectFn();
			} );
		} );
	}
	else {
		loadFn();
		mWaveformPlot.load( bufferPlayer->getBuffer(), app::getWindowBounds() );
		connectFn();
	};
}

void SamplePlayerTest::setupFilePlayerNode()
{
	bool loopEnabled = mSamplePlayerNode ? mSamplePlayerNode->isLoopEnabled() : false;
	double loopBegin = mSamplePlayerNode ? mSamplePlayerNode->getLoopBeginTime() : 0.0;
	double loopEnd = mSamplePlayerNode ? mSamplePlayerNode->getLoopEndTime() : 0.0;

	mGain->disconnectAllInputs();

	auto ctx = audio::master();

//	mSourceFile->setMaxFramesPerRead( 8192 );
	mSamplePlayerNode = ctx->makeNode( new audio::FilePlayerNode( mSourceFile->clone(), mLoadAsync ) );

	// TODO: it is pretty surprising when you recreate mMonitor here without checking if there has already been one added.
	//	- user will no longer see the old mMonitor, but the context still owns a reference to it, so another gets added each time we call this method.
	if( ! mMonitor )
		mMonitor = ctx->makeNode( new audio::MonitorNode( audio::MonitorNode::Format().windowSize( 1024 ) ) );

	// when these connections are called, some (GainNode and Pan) will already be connected, but this is okay, they should silently no-op.

	// or connect in series (it is added to the Context's 'auto pulled list')
	mSamplePlayerNode >> mGain >> mPan >> ctx->getOutput();
	mPan >> mMonitor;

	mSamplePlayerNode->setLoopEnabled( loopEnabled );
	mSamplePlayerNode->setLoopBeginTime( loopBegin );
	mSamplePlayerNode->setLoopEndTime( loopEnd );
}

void SamplePlayerTest::setupBufferRecorderNode()
{
	auto ctx = audio::master();

	mRecorder = ctx->makeNode( new audio::BufferRecorderNode( 2 * ctx->getSampleRate() ) );
//	mRecorder->setNumSeconds( 2 ); // can also set seconds afterwards, which causes a lock and buffer resize

	CI_ASSERT( mSamplePlayerNode );

	mSamplePlayerNode >> mRecorder;
}

void SamplePlayerTest::setSourceFile( const DataSourceRef &dataSource )
{
	mSourceFile = audio::load( dataSource, audio::master()->getSampleRate() );

	app::getWindow()->setTitle( dataSource->getFilePath().filename().string() );

	CI_LOG_I( "SourceFile info: " );
	app::console() << "samplerate: " << mSourceFile->getSampleRate() << endl;
	app::console() << "native samplerate: " << mSourceFile->getSampleRateNative() << endl;
	app::console() << "channels: " << mSourceFile->getNumChannels() << endl;
	app::console() << "frames: " << mSourceFile->getNumFrames() << endl;
	app::console() << "metadata:\n" << mSourceFile->getMetaData() << endl;
}

void SamplePlayerTest::writeRecordedToFile()
{
	const string fileName = "recorder_out.wav";
	CI_LOG_V( "writing to: " << fileName );

	mRecorder->writeToFile( fileName );

	CI_LOG_V( "...complete." );
}

void SamplePlayerTest::triggerStartStop( bool start )
{
	if( mTriggerDelaySeconds <= 0.001f ) {
		if( start )
			mSamplePlayerNode->start();
		else
			mSamplePlayerNode->stop();
	}
	else {
		CI_LOG_I( "scheduling " << ( start ? "start" : "stop" ) << " with delay: " << mTriggerDelaySeconds
				 << "\n\tprocessed frames: " << audio::master()->getNumProcessedFrames() << ", seconds: " << audio::master()->getNumProcessedSeconds() );

		double when = audio::master()->getNumProcessedSeconds() + (double)mTriggerDelaySeconds;
		if( start )
			mSamplePlayerNode->start( when );
		else
			mSamplePlayerNode->stop( when );
	}
}

void SamplePlayerTest::printSupportedExtensions()
{
	CI_LOG_I( "supported SourceFile extensions: " );
	for( const auto &ext : audio::SourceFile::getSupportedExtensions() )
		app::console() << ext << ", ";

	app::console() << endl;
}

void SamplePlayerTest::openFile( const ci::fs::path &fullPath )
{
	try {
		setSourceFile( loadFile( fullPath ) );
		CI_LOG_I( "loaded and set new source buffer, frames: " << mSourceFile->getNumFrames() );
	}
	catch( exception &e ) {
		CI_LOG_EXCEPTION( "failed to load sample file at path: " << fullPath, e );
	}

	mSamplePlayerNode->seek( 0 );

	CI_LOG_I( "output samplerate: " << mSourceFile->getSampleRate() );

	auto bufferPlayer = dynamic_pointer_cast<audio::BufferPlayerNode>( mSamplePlayerNode );
	if( bufferPlayer ) {
		if( mLoadAsync ) {
			mAsyncLoadFuture = std::async( [=] {
				bufferPlayer->loadBuffer( mSourceFile->clone() );
				app::App::get()->dispatchAsync( [=] {
					mWaveformPlot.load( bufferPlayer->getBuffer(), app::getWindowBounds() );
				} );
			} );
		}
		else {
			bufferPlayer->loadBuffer( mSourceFile->clone() );
			mWaveformPlot.load( bufferPlayer->getBuffer(), app::getWindowBounds() );
		}
	}
	else {
		auto filePlayer = dynamic_pointer_cast<audio::FilePlayerNode>( mSamplePlayerNode );
		CI_ASSERT_MSG( filePlayer, "expected sample player to be either BufferPlayerNode or FilePlayerNode" );

		filePlayer->setSourceFile( mSourceFile->clone() );
	}

	mSamplePlayerNode->setLoopEndTime( mSamplePlayerNode->getNumSeconds() );

	CI_LOG_I( "loaded and set new source buffer, channels: " << mSourceFile->getNumChannels() << ", frames: " << mSourceFile->getNumFrames() );
}

void SamplePlayerTest::resize()
{
	const float padding = 6.0f;
	const vec2 xrunSize( 80, 26 );
	mUnderrunRect = Rectf( padding, app::getWindowHeight() - xrunSize.y - padding, xrunSize.x + padding, app::getWindowHeight() - padding );
	mOverrunRect = mUnderrunRect + vec2( xrunSize.x + padding, 0 );
	mRecorderOverrunRect = mOverrunRect + vec2( xrunSize.x + padding, 0 );
}

void SamplePlayerTest::update()
{
	// light up rects if an xrun was detected
	const float xrunFadeTime = 1.3f;
	auto filePlayer = dynamic_pointer_cast<audio::FilePlayerNode>( mSamplePlayerNode );
	if( filePlayer ) {
		if( filePlayer->getLastUnderrun() )
			app::timeline().apply( &mUnderrunFade, 1.0f, 0.0f, xrunFadeTime );
		if( filePlayer->getLastOverrun() )
			app::timeline().apply( &mOverrunFade, 1.0f, 0.0f, xrunFadeTime );
	}

	// testing delayed trigger: print SamplePlayerNode start / stop times
	if( mSamplePlayerNode && mSamplePlayerNodeEnabledState != mSamplePlayerNode->isEnabled() ) {
		mSamplePlayerNodeEnabledState = mSamplePlayerNode->isEnabled();
		string stateStr = mSamplePlayerNodeEnabledState ? "started" : "stopped";
		CI_LOG_I( "mSamplePlayerNode " << stateStr << " at " << app::getElapsedSeconds() << ", isEof: " << boolalpha << mSamplePlayerNode->isEof() << dec );
	}

	// test auto resizing the Recorder's buffer depending on how full it is
	if( ( mSubTests[mCurrentSubTest] == "recorder" ) && mRecordAutoResize ) {
		CI_ASSERT( mRecorder );

		size_t writePos = mRecorder->getWritePosition();
		size_t numFrames = mRecorder->getNumFrames();

		if( writePos + mRecorder->getSampleRate() / 2 > numFrames ) {
			size_t resizeFrames = numFrames + mRecorder->getSampleRate();
			CI_LOG_I( "writePos: " << writePos << ", numFrames: " << numFrames << ", resizing frames to: " << resizeFrames );
			mRecorder->setNumFrames( resizeFrames );
		}

		if( mRecorder->getLastOverrun() )
			app::timeline().apply( &mRecorderOverrunFade, 1.0f, 0.0f, xrunFadeTime );
	}

}

void SamplePlayerTest::draw()
{
	gl::clear();

	if( mSubTests[mCurrentSubTest] == "recorder" ) {
		audio::BufferRef recordedBuffer = mRecorder->getRecordedCopy();
		drawAudioBuffer( *recordedBuffer, app::getWindowBounds() );
	}
	else if( mSamplePlayerNode ) {
		auto bufferPlayer = dynamic_pointer_cast<audio::BufferPlayerNode>( mSamplePlayerNode );
		if( bufferPlayer )
			mWaveformPlot.draw();
		else if( mMonitor && mMonitor->isInitialized() )
			drawAudioBuffer( mMonitor->getBuffer(), app::getWindowBounds() );

		float readPos = (float)app::getWindowWidth() * mSamplePlayerNode->getReadPosition() / mSamplePlayerNode->getNumFrames();
		gl::color( ColorA( 0, 1, 0, 0.7f ) );
		gl::drawSolidRect( Rectf( readPos - 2, 0, readPos + 2, (float)app::getWindowHeight() ) );
	}

	if( mUnderrunFade > 0.0001f ) {
		gl::color( ColorA( 1, 0.5f, 0, mUnderrunFade ) );
		gl::drawSolidRect( mUnderrunRect );
		gl::drawStringCentered( "play underrun", mUnderrunRect.getCenter(), Color::black() );
	}
	if( mOverrunFade > 0.0001f ) {
		gl::color( ColorA( 1, 0.5f, 0, mOverrunFade ) );
		gl::drawSolidRect( mOverrunRect );
		gl::drawStringCentered( "play overrun", mOverrunRect.getCenter(), Color::black() );
	}
	if( mRecorderOverrunFade > 0.0001f ) {
		gl::color( ColorA( 1, 0.5f, 0, mRecorderOverrunFade ) );
		gl::drawSolidRect( mRecorderOverrunRect );
		gl::drawStringCentered( "rec overrun", mRecorderOverrunRect.getCenter(), Color::black() );
	}
}

void SamplePlayerTest::testConverter()
{
	audio::BufferRef audioBuffer = mSourceFile->loadBuffer();

	size_t destSampleRate = 48000;
	size_t destChannels = 1;
	size_t sourceMaxFramesPerBlock = 512;
	auto converter = audio::dsp::Converter::create( mSourceFile->getSampleRate(), destSampleRate, mSourceFile->getNumChannels(), destChannels, sourceMaxFramesPerBlock );

	CI_LOG_I( "FROM samplerate: " << converter->getSourceSampleRate() << ", channels: " << converter->getSourceNumChannels() << ", frames per block: " << converter->getSourceMaxFramesPerBlock() );
	CI_LOG_I( "TO samplerate: " << converter->getDestSampleRate() << ", channels: " << converter->getDestNumChannels() << ", frames per block: " << converter->getDestMaxFramesPerBlock() );

	audio::BufferDynamic sourceBuffer( converter->getSourceMaxFramesPerBlock(), converter->getSourceNumChannels() );
	audio::Buffer destBuffer( converter->getDestMaxFramesPerBlock(), converter->getDestNumChannels() );

	audio::TargetFileRef target = audio::TargetFile::create( "resampled.wav", converter->getDestSampleRate(), converter->getDestNumChannels() );

	size_t numFramesConverted = 0;

	Timer timer( true );

	while( numFramesConverted < audioBuffer->getNumFrames() ) {
		if( audioBuffer->getNumFrames() - numFramesConverted > sourceMaxFramesPerBlock ) {
			for( size_t ch = 0; ch < audioBuffer->getNumChannels(); ch++ )
				memcpy( sourceBuffer.getChannel( ch ), audioBuffer->getChannel( ch ) + numFramesConverted, sourceMaxFramesPerBlock * sizeof( float ) );
				//copy( audioBuffer->getChannel( ch ) + numFramesConverted, audioBuffer->getChannel( ch ) + numFramesConverted + sourceMaxFramesPerBlock, sourceBuffer.getChannel( ch ) );
		}
		else {
			// EOF, shrink sourceBuffer to match remaining
			sourceBuffer.setNumFrames( audioBuffer->getNumFrames() - numFramesConverted );
			for( size_t ch = 0; ch < audioBuffer->getNumChannels(); ch++ )
				memcpy( sourceBuffer.getChannel( ch ), audioBuffer->getChannel( ch ) + numFramesConverted, audioBuffer->getNumFrames() * sizeof( float ) );
				//copy( audioBuffer->getChannel( ch ) + numFramesConverted, audioBuffer->getChannel( ch ) + audioBuffer->getNumFrames(), sourceBuffer.getChannel( ch ) );
		}

		pair<size_t, size_t> result = converter->convert( &sourceBuffer, &destBuffer );
		numFramesConverted += result.first;

		target->write( &destBuffer, 0, result.second );
	}

	CI_LOG_I( "seconds: " << timer.getSeconds() );
}

void SamplePlayerTest::testWrite()
{
	audio::BufferRef audioBuffer = mSourceFile->loadBuffer();

	try {
		const string fileName = "out.wav";
		audio::TargetFileRef target = audio::TargetFile::create( fileName, mSourceFile->getSampleRate(), mSourceFile->getNumChannels() ); // INT_16
		//	audio::TargetFileRef target = audio::TargetFile::create( fileName, mSourceFile->getSampleRate(), mSourceFile->getNumChannels(), audio::SampleType::FLOAT_32 );

		CI_LOG_I( "writing " << audioBuffer->getNumFrames() << " frames at samplerate: " << mSourceFile->getSampleRate() << ", num channels: " << mSourceFile->getNumChannels() );
		target->write( audioBuffer.get() );
		CI_LOG_I( "...complete." );

//		size_t writeCount = 0;
//		while( numFramesConverted < audioBuffer->getNumFrames() ) {
//			for( size_t ch = 0; ch < audioBuffer->getNumChannels(); ch++ )
//				copy( audioBuffer->getChannel( ch ) + writeCount, audioBuffer->getChannel( ch ) + writeCount + sourceFormat.getFramesPerBlock(), sourceBuffer.getChannel( ch ) );
//		}
	}
	catch( audio::AudioFileExc &exc ) {
		CI_LOG_E( "AudioFileExc, what: " << exc.what() );
	}
}

// -----------------------------------------------------------------------
// UI
// -----------------------------------------------------------------------

void SamplePlayerTest::updateUI()
{
	float gain = mGain->getValue();
	if( im::SliderFloat( "gain", &gain, 0, 1 ) ) {
		mGain->setValue( gain );
	}
	float pan = mPan->getPos();
	if( im::SliderFloat( "pan", &pan, 0, 1 ) ) {
		mPan->setPos( pan );
	}

	im::Checkbox( "load async", &mLoadAsync );

	im::Separator();
	im::Text( "SamplePlayer" );
	if( mSamplePlayerNode ) {
		bool playerEnabled = mSamplePlayerNode->isEnabled();
		if( im::Checkbox( "enabled", &playerEnabled ) ) {
			mSamplePlayerNode->setEnabled( playerEnabled );
		}
		im::SameLine();
		bool loopEnabled = mSamplePlayerNode->isLoopEnabled();
		if( im::Checkbox( "loop", &loopEnabled ) ) {
			mSamplePlayerNode->setLoopEnabled( loopEnabled );
		}
		float readPos = (float)mSamplePlayerNode->getReadPositionTime();
		if( im::SliderFloat( "read pos (s)", &readPos, 0, mSamplePlayerNode->getNumSeconds() ) ) {
			mSamplePlayerNode->seekToTime( readPos );
		}
		float loopBegin = (float)mSamplePlayerNode->getLoopBeginTime();
		float loopEnd = (float)mSamplePlayerNode->getLoopEndTime();
		if( im::SliderFloat( "loop begin (s)", &loopBegin, 0, mSamplePlayerNode->getNumSeconds() ) ) {
			loopBegin - min<float>( loopBegin, loopEnd );
			mSamplePlayerNode->setLoopBeginTime( loopBegin );
		}
		if( im::SliderFloat( "loop end (s)", &loopEnd, 0, mSamplePlayerNode->getNumSeconds() ) ) {
			loopEnd = max<float>( loopBegin, loopEnd );
			mSamplePlayerNode->setLoopEndTime( loopEnd );
		}
	}

	im::Separator();
	im::DragFloat( "start / stop delay (s)", &mTriggerDelaySeconds, 0.02f, 0.0f, 5.0f );

	if( mSubTests[mCurrentSubTest] == "recorder" ) {
		im::Separator();
		im::Text( "Recording" );
		im::Checkbox( "record", &mRecording );
		im::SameLine();
		im::Checkbox( "auto-resize", &mRecordAutoResize );

		if( im::Button( "write to file" ) ) {
			writeRecordedToFile();
		}
	}

	if( im::ListBox( "sub-tests", &mCurrentSubTest, mSubTests, mSubTests.size() ) ) {
		setupSubTest( mSubTests[mCurrentSubTest] );
	}
}