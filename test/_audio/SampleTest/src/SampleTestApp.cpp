#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Timeline.h"
#include "cinder/Timer.h"
#include "cinder/Log.h"

#include "cinder/audio/Source.h"
#include "cinder/audio/Target.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/SampleRecorderNode.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/audio/MonitorNode.h"

#include "Resources.h"

#include "../../common/AudioTestGui.h"
#include "../../../../samples/_audio/common/AudioDrawUtils.h"

//#define INITIAL_AUDIO_RES	RES_TONE440_WAV
#define INITIAL_AUDIO_RES	RES_TONE440L220R_WAV
//#define INITIAL_AUDIO_RES RES_TONE440_LOOP_WAV
//#define INITIAL_AUDIO_RES	RES_TONE440_MP3
//#define INITIAL_AUDIO_RES	RES_TONE440L220R_MP3
//#define INITIAL_AUDIO_RES	RES_TONE440_OGG
//#define INITIAL_AUDIO_RES	RES_TONE440L220R_OGG

#define TEST_STEREO_INPUT_PANNING 0

using namespace ci;
using namespace ci::app;
using namespace std;


class SamplePlayerNodeTestApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void fileDrop( FileDropEvent event ) override;
	void update() override;
	void draw() override;

	void setupBufferPlayerNode();
	void setupFilePlayerNode();
	void setupBufferRecorderNode();
	void setSourceFile( const DataSourceRef &dataSource );
	void writeRecordedToFile();
	void triggerStartStop( bool start );

	void setupUI();
	void processDrag( ivec2 pos );
	void processTap( ivec2 pos );

	void seek( size_t xPos );
	void printBufferSamples( size_t xPos );
	void printSupportedExtensions();

	void testConverter();
	void testWrite();

	audio::SamplePlayerNodeRef		mSamplePlayerNode;
	audio::SourceFileRef			mSourceFile;
	audio::MonitorNodeRef			mMonitor;
	audio::GainNodeRef				mGain;
	audio::Pan2dNodeRef				mPan;
	audio::BufferRecorderNodeRef	mRecorder;

	WaveformPlot				mWaveformPlot;
	vector<TestWidget *>		mWidgets;
	Button						mEnableSamplePlayerNodeButton, mStartPlaybackButton, mStopPlaybackButton, mLoopButton, mAsyncButton, mRecordButton, mWriteButton, mAutoResizeButton;
	VSelector					mTestSelector;
	HSlider						mGainSlider, mPanSlider, mLoopBeginSlider, mLoopEndSlider, mTriggerDelaySlider;

	Anim<float>					mUnderrunFade, mOverrunFade, mRecorderOverrunFade;
	Rectf						mUnderrunRect, mOverrunRect, mRecorderOverrunRect;
	bool						mSamplePlayerNodeEnabledState;
	std::future<void>			mAsyncLoadFuture;
};

void SamplePlayerNodeTestApp::setup()
{
	mUnderrunFade = mOverrunFade = mRecorderOverrunFade = 0;
	mSamplePlayerNodeEnabledState = false;

	printSupportedExtensions();

	setSourceFile( loadResource( INITIAL_AUDIO_RES ) );

	auto ctx = audio::master();

	mPan = ctx->makeNode( new audio::Pan2dNode() );
#if TEST_STEREO_INPUT_PANNING
	mPan->setStereoInputModeEnabled( true );
#endif

	mGain = ctx->makeNode( new audio::GainNode() );
	mGain->setValue( 0.6f );

	mGain >> mPan >> ctx->getOutput();

	setupBufferPlayerNode();
//	setupFilePlayerNode();

	setupUI();

	ctx->enable();
	mEnableSamplePlayerNodeButton.setEnabled( true );

	CI_LOG_V( "context samplerate: " << ctx->getSampleRate() );
}

void SamplePlayerNodeTestApp::setupBufferPlayerNode()
{
	auto bufferPlayer = audio::master()->makeNode( new audio::BufferPlayerNode() );

	auto loadFn = [bufferPlayer, this] {
		bufferPlayer->loadBuffer( mSourceFile->clone() );
		mWaveformPlot.load( bufferPlayer->getBuffer(), getWindowBounds() );
		CI_LOG_V( "loaded source buffer, frames: " << bufferPlayer->getBuffer()->getNumFrames() );

	};

	auto connectFn = [bufferPlayer, this] {
		mGain->disconnectAllInputs();
		mSamplePlayerNode = bufferPlayer;
		mSamplePlayerNode >> mGain >> mPan >> audio::master()->getOutput();
		PRINT_GRAPH( audio::master() );
		mSamplePlayerNode->setLoopEnabled( mLoopButton.mEnabled );
		mSamplePlayerNode->setLoopBeginTime( mLoopBeginSlider.mValueScaled );
		mSamplePlayerNode->setLoopEndTime( mLoopEndSlider.mValueScaled != 0 ? mLoopEndSlider.mValueScaled : mSamplePlayerNode->getNumSeconds() );
	};

	bool asyncLoad = mAsyncButton.mEnabled;
	CI_LOG_V( "async load: " << boolalpha << asyncLoad << dec );
	if( asyncLoad ) {
		mWaveformPlot.clear();
		mAsyncLoadFuture = std::async( [=] {
			loadFn();
			dispatchAsync( [=] {
				connectFn();
			} );
		} );
	}
	else {
		loadFn();
		connectFn();
	};
}

void SamplePlayerNodeTestApp::setupFilePlayerNode()
{
	mGain->disconnectAllInputs();

	auto ctx = audio::master();

//	mSourceFile->setMaxFramesPerRead( 8192 );
	bool asyncRead = mAsyncButton.mEnabled;
	CI_LOG_V( "async read: " << asyncRead );
	mSamplePlayerNode = ctx->makeNode( new audio::FilePlayerNode( mSourceFile->clone(), asyncRead ) );

	// TODO: it is pretty surprising when you recreate mMonitor here without checking if there has already been one added.
	//	- user will no longer see the old mMonitor, but the context still owns a reference to it, so another gets added each time we call this method.
	if( ! mMonitor )
		mMonitor = ctx->makeNode( new audio::MonitorNode( audio::MonitorNode::Format().windowSize( 1024 ) ) );

	// when these connections are called, some (GainNode and Pan) will already be connected, but this is okay, they should silently no-op.

	// or connect in series (it is added to the Context's 'auto pulled list')
	mSamplePlayerNode >> mGain >> mPan >> ctx->getOutput();
	mPan >> mMonitor;

	mSamplePlayerNode->setLoopEnabled( mLoopButton.mEnabled );
	mSamplePlayerNode->setLoopBeginTime( mLoopBeginSlider.mValueScaled );
	mSamplePlayerNode->setLoopEndTime( mLoopEndSlider.mValueScaled != 0 ? mLoopEndSlider.mValueScaled : mSamplePlayerNode->getNumSeconds() );

	PRINT_GRAPH( audio::master() );
}

void SamplePlayerNodeTestApp::setupBufferRecorderNode()
{
	auto ctx = audio::master();

	mRecorder = ctx->makeNode( new audio::BufferRecorderNode( 2 * ctx->getSampleRate() ) );
//	mRecorder->setNumSeconds( 2 ); // can also set seconds afterwards, which causes a lock and buffer resize

	CI_ASSERT( mSamplePlayerNode );

	mSamplePlayerNode >> mRecorder;

	PRINT_GRAPH( audio::master() );
}

void SamplePlayerNodeTestApp::setSourceFile( const DataSourceRef &dataSource )
{
	mSourceFile = audio::load( dataSource, audio::master()->getSampleRate() );

	getWindow()->setTitle( dataSource->getFilePath().filename().string() );

	CI_LOG_V( "SourceFile info: " );
	console() << "samplerate: " << mSourceFile->getSampleRate() << endl;
	console() << "native samplerate: " << mSourceFile->getSampleRateNative() << endl;
	console() << "channels: " << mSourceFile->getNumChannels() << endl;
	console() << "frames: " << mSourceFile->getNumFrames() << endl;
	console() << "metadata:\n" << mSourceFile->getMetaData() << endl;
}

void SamplePlayerNodeTestApp::writeRecordedToFile()
{
	const string fileName = "recorder_out.wav";
	CI_LOG_V( "writing to: " << fileName );

	mRecorder->writeToFile( fileName );

	CI_LOG_V( "...complete." );
}

void SamplePlayerNodeTestApp::triggerStartStop( bool start )
{
	float delaySeconds = mTriggerDelaySlider.mValueScaled;
	if( delaySeconds <= 0.001f ) {
		if( start )
			mSamplePlayerNode->start();
		else
			mSamplePlayerNode->stop();
	}
	else {
		CI_LOG_V( "scheduling " << ( start ? "start" : "stop" ) << " with delay: " << delaySeconds
				 << "\n\tprocessed frames: " << audio::master()->getNumProcessedFrames() << ", seconds: " << audio::master()->getNumProcessedSeconds() );

		double when = audio::master()->getNumProcessedSeconds() + delaySeconds;
		if( start )
			mSamplePlayerNode->start( when );
		else
			mSamplePlayerNode->stop( when );
	}
}

void SamplePlayerNodeTestApp::setupUI()
{
	const float padding = 6.0f;

	auto buttonRect = Rectf( padding, padding, 160, 50 );
	mEnableSamplePlayerNodeButton.mIsToggle = true;
	mEnableSamplePlayerNodeButton.mTitleNormal = "player off";
	mEnableSamplePlayerNodeButton.mTitleEnabled = "player on";
	mEnableSamplePlayerNodeButton.mBounds = buttonRect;
	mWidgets.push_back( &mEnableSamplePlayerNodeButton );

	buttonRect += vec2( buttonRect.getWidth() + padding, 0 );
	mStartPlaybackButton.mIsToggle = false;
	mStartPlaybackButton.mTitleNormal = "start";
	mStartPlaybackButton.mBounds = buttonRect;
	mWidgets.push_back( &mStartPlaybackButton );

	buttonRect += vec2( buttonRect.getWidth() + padding, 0 );
	mStopPlaybackButton.mIsToggle = false;
	mStopPlaybackButton.mTitleNormal = "stop";
	mStopPlaybackButton.mBounds = buttonRect;
	mWidgets.push_back( &mStopPlaybackButton );

	buttonRect += vec2( buttonRect.getWidth() + padding, 0 );
	buttonRect.x2 -= 30;
	mLoopButton.mIsToggle = true;
	mLoopButton.mTitleNormal = "loop off";
	mLoopButton.mTitleEnabled = "loop on";
	mLoopButton.setEnabled( mSamplePlayerNode->isLoopEnabled() );
	mLoopButton.mBounds = buttonRect;
	mWidgets.push_back( &mLoopButton );

	buttonRect += vec2( buttonRect.getWidth() + padding, 0 );
	mAsyncButton.mIsToggle = true;
	mAsyncButton.mTitleNormal = "async off";
	mAsyncButton.mTitleEnabled = "async on";
	mAsyncButton.mBounds = buttonRect;
	mWidgets.push_back( &mAsyncButton );

	buttonRect = Rectf( padding, buttonRect.y2 + padding, padding + buttonRect.getWidth(), buttonRect.y2 + buttonRect.getHeight() + padding );
	mRecordButton.mIsToggle = true;
	mRecordButton.mTitleNormal = "record off";
	mRecordButton.mTitleEnabled = "record on";
	mRecordButton.mBounds = buttonRect;
	mWidgets.push_back( &mRecordButton );

	buttonRect += vec2( buttonRect.getWidth() + padding, 0 );
	mWriteButton.mIsToggle = false;
	mWriteButton.mTitleNormal = "write to file";
	mWriteButton.mBounds = buttonRect;
	mWidgets.push_back( &mWriteButton );

	buttonRect += vec2( buttonRect.getWidth() + padding, 0 );
	mAutoResizeButton.mIsToggle = true;
	mAutoResizeButton.mTitleNormal = "auto resize off";
	mAutoResizeButton.mTitleEnabled = "auto resize on";
	mAutoResizeButton.mBounds = buttonRect;
	mWidgets.push_back( &mAutoResizeButton );

	vec2 sliderSize( 200, 30 );
	Rectf selectorRect( getWindowWidth() - sliderSize.x - padding, padding, getWindowWidth() - padding, sliderSize.y * 3 + padding );
	mTestSelector.mSegments.push_back( "BufferPlayerNode" );
	mTestSelector.mSegments.push_back( "FilePlayerNode" );
	mTestSelector.mSegments.push_back( "recorder" );
	mTestSelector.mBounds = selectorRect;
	mWidgets.push_back( &mTestSelector );

	Rectf sliderRect( selectorRect.x1, selectorRect.y2 + padding, selectorRect.x2, selectorRect.y2 + padding + sliderSize.y );
	mGainSlider.mBounds = sliderRect;
	mGainSlider.mTitle = "GainNode";
	mGainSlider.set( mGain->getValue() );
	mWidgets.push_back( &mGainSlider );

	sliderRect += vec2( 0, sliderRect.getHeight() + padding );
	mPanSlider.mBounds = sliderRect;
	mPanSlider.mTitle = "Pan";
	mPanSlider.set( mPan->getPos() );
	mWidgets.push_back( &mPanSlider );

	sliderRect += vec2( 0, sliderRect.getHeight() + padding );
	mLoopBeginSlider.mBounds = sliderRect;
	mLoopBeginSlider.mTitle = "Loop Begin";
	mLoopBeginSlider.mMax = (float)mSamplePlayerNode->getNumSeconds();
	mLoopBeginSlider.set( (float)mSamplePlayerNode->getLoopBeginTime() );
	mWidgets.push_back( &mLoopBeginSlider );

	sliderRect += vec2( 0, sliderRect.getHeight() + padding );
	mLoopEndSlider.mBounds = sliderRect;
	mLoopEndSlider.mTitle = "Loop End";
	mLoopEndSlider.mMax = (float)mSamplePlayerNode->getNumSeconds();
	mLoopEndSlider.set( (float)mSamplePlayerNode->getLoopEndTime() );
	mWidgets.push_back( &mLoopEndSlider );

	sliderRect += vec2( 0, sliderRect.getHeight() + padding );
	mTriggerDelaySlider.mBounds = sliderRect;
	mTriggerDelaySlider.mTitle = "Trigger Delay";
	mTriggerDelaySlider.mMin = 0.0f; // TODO: test negative numbers don't blow up
	mTriggerDelaySlider.mMax = 5.0f;
	mTriggerDelaySlider.set( 0 );
	mWidgets.push_back( &mTriggerDelaySlider );

	vec2 xrunSize( 80, 26 );
	mUnderrunRect = Rectf( padding, getWindowHeight() - xrunSize.y - padding, xrunSize.x + padding, getWindowHeight() - padding );
	mOverrunRect = mUnderrunRect + vec2( xrunSize.x + padding, 0 );
	mRecorderOverrunRect = mOverrunRect + vec2( xrunSize.x + padding, 0 );

	getWindow()->getSignalMouseDown().connect( [this] ( MouseEvent &event ) { processTap( event.getPos() ); } );
	getWindow()->getSignalMouseDrag().connect( [this] ( MouseEvent &event ) { processDrag( event.getPos() ); } );
	getWindow()->getSignalTouchesBegan().connect( [this] ( TouchEvent &event ) { processTap( event.getTouches().front().getPos() ); } );
	getWindow()->getSignalTouchesMoved().connect( [this] ( TouchEvent &event ) {
		for( const TouchEvent::Touch &touch : getActiveTouches() )
			processDrag( touch.getPos() );
	} );

	gl::enableAlphaBlending();
}

void SamplePlayerNodeTestApp::processDrag( ivec2 pos )
{
	if( mGainSlider.hitTest( pos ) )
		mGain->setValue( mGainSlider.mValueScaled );
	else if( mPanSlider.hitTest( pos ) ) {
#if TEST_STEREO_INPUT_PANNING
		mPan->getParamPos()->applyRamp( mPanSlider.mValueScaled, 0.6f );
#else
		mPan->setPos( mPanSlider.mValueScaled );
#endif
	}
	else if( mLoopBeginSlider.hitTest( pos ) )
		mSamplePlayerNode->setLoopBeginTime( mLoopBeginSlider.mValueScaled );
	else if( mLoopEndSlider.hitTest( pos ) )
		mSamplePlayerNode->setLoopEndTime( mLoopEndSlider.mValueScaled );
	else if( mTriggerDelaySlider.hitTest( pos ) ) {
	}
	else if( pos.y > getWindowCenter().y )
		seek( pos.x );
}

void SamplePlayerNodeTestApp::processTap( ivec2 pos )
{
	if( mEnableSamplePlayerNodeButton.hitTest( pos ) )
		mSamplePlayerNode->setEnabled( ! mSamplePlayerNode->isEnabled() );
	else if( mStartPlaybackButton.hitTest( pos ) )
		triggerStartStop( true );
	else if( mStopPlaybackButton.hitTest( pos ) )
		triggerStartStop( false );
	else if( mLoopButton.hitTest( pos ) )
		mSamplePlayerNode->setLoopEnabled( ! mSamplePlayerNode->isLoopEnabled() );
	else if( mRecordButton.hitTest( pos ) ) {
		if( mRecordButton.mEnabled )
			mRecorder->start();
		else
			mRecorder->disable();
	}
	else if( mWriteButton.hitTest( pos ) )
		writeRecordedToFile();
	else if( mAutoResizeButton.hitTest( pos ) )
		;
	else if( mAsyncButton.hitTest( pos ) )
		;
	else if( pos.y > getWindowCenter().y )
		seek( pos.x );

	size_t currentIndex = mTestSelector.mCurrentSectionIndex;
	if( mTestSelector.hitTest( pos ) && currentIndex != mTestSelector.mCurrentSectionIndex ) {
		string currentTest = mTestSelector.currentSection();
		CI_LOG_V( "selected: " << currentTest );

		if( currentTest == "BufferPlayerNode" )
			setupBufferPlayerNode();
		if( currentTest == "FilePlayerNode" )
			setupFilePlayerNode();
		if( currentTest == "recorder" )
			setupBufferRecorderNode();
	}
}

void SamplePlayerNodeTestApp::seek( size_t xPos )
{
	mSamplePlayerNode->seek( mSamplePlayerNode->getNumFrames() * xPos / getWindowWidth() );
}

void SamplePlayerNodeTestApp::printBufferSamples( size_t xPos )
{
	auto bufferPlayer = dynamic_pointer_cast<audio::BufferPlayerNode>( mSamplePlayerNode );
	if( ! bufferPlayer )
		return;

	auto buffer = bufferPlayer->getBuffer();
	size_t step = buffer->getNumFrames() / getWindowWidth();
	size_t xScaled = xPos * step;
	CI_LOG_V( "samples starting at " << xScaled << ":" );
	for( int i = 0; i < 100; i++ ) {
		if( buffer->getNumChannels() == 1 )
			console() << buffer->getChannel( 0 )[xScaled + i] << ", ";
		else
			console() << "[" << buffer->getChannel( 0 )[xScaled + i] << ", " << buffer->getChannel( 0 )[xScaled + i] << "], ";
	}
	console() << endl;
}

void SamplePlayerNodeTestApp::printSupportedExtensions()
{
	CI_LOG_V( "supported SourceFile extensions: " );
	for( const auto &ext : audio::SourceFile::getSupportedExtensions() )
		console() << ext << ", ";

	console() << endl;
}

void SamplePlayerNodeTestApp::mouseDown( MouseEvent event )
{
//	printBufferSamples( event.getX() );
}

void SamplePlayerNodeTestApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_c )
		testConverter();
	if( event.getCode() == KeyEvent::KEY_w )
		testWrite();
	if( event.getCode() == KeyEvent::KEY_s )
		mSamplePlayerNode->seekToTime( 1.0 );
}

void SamplePlayerNodeTestApp::fileDrop( FileDropEvent event )
{
	const fs::path &filePath = event.getFile( 0 );
	CI_LOG_V( "File dropped: " << filePath );

	setSourceFile( loadFile( filePath ) );
	mSamplePlayerNode->seek( 0 );

	CI_LOG_V( "output samplerate: " << mSourceFile->getSampleRate() );

	auto bufferPlayer = dynamic_pointer_cast<audio::BufferPlayerNode>( mSamplePlayerNode );
	if( bufferPlayer ) {
		bufferPlayer->loadBuffer( mSourceFile->clone() );
		mWaveformPlot.load( bufferPlayer->getBuffer(), getWindowBounds() );
	}
	else {
		auto filePlayer = dynamic_pointer_cast<audio::FilePlayerNode>( mSamplePlayerNode );
		CI_ASSERT_MSG( filePlayer, "expected sample player to be either BufferPlayerNode or FilePlayerNode" );

		filePlayer->setSourceFile( mSourceFile->clone() );
	}

	mLoopBeginSlider.mMax = mLoopEndSlider.mMax = (float)mSamplePlayerNode->getNumSeconds();
	mLoopBeginSlider.set( mSamplePlayerNode->getLoopBeginTime() );
	mLoopEndSlider.set( mSamplePlayerNode->getLoopEndTime() );

	CI_LOG_V( "loaded and set new source buffer, channels: " << mSourceFile->getNumChannels() << ", frames: " << mSourceFile->getNumFrames() );
	PRINT_GRAPH( audio::master() );
}


void SamplePlayerNodeTestApp::update()
{
	// light up rects if an xrun was detected
	const float xrunFadeTime = 1.3f;
	auto filePlayer = dynamic_pointer_cast<audio::FilePlayerNode>( mSamplePlayerNode );
	if( filePlayer ) {
		if( filePlayer->getLastUnderrun() )
			timeline().apply( &mUnderrunFade, 1.0f, 0.0f, xrunFadeTime );
		if( filePlayer->getLastOverrun() )
			timeline().apply( &mOverrunFade, 1.0f, 0.0f, xrunFadeTime );
	}

	// print SamplePlayerNode start / stop times
	if( mSamplePlayerNodeEnabledState != mSamplePlayerNode->isEnabled() ) {
		mSamplePlayerNodeEnabledState = mSamplePlayerNode->isEnabled();
		string stateStr = mSamplePlayerNodeEnabledState ? "started" : "stopped";
		CI_LOG_V( "mSamplePlayerNode " << stateStr << " at " << getElapsedSeconds() << ", isEof: " << boolalpha << mSamplePlayerNode->isEof() << dec );
	}

	bool testIsRecorder = ( mTestSelector.currentSection() == "recorder" );
	mRecordButton.mHidden = mWriteButton.mHidden = mAutoResizeButton.mHidden = ! testIsRecorder;

	// test auto resizing the Recorder's buffer depending on how full it is
	if( testIsRecorder && mAutoResizeButton.mEnabled ) {
		CI_ASSERT( mRecorder );

		size_t writePos = mRecorder->getWritePosition();
		size_t numFrames = mRecorder->getNumFrames();

		if( writePos + mRecorder->getSampleRate() / 2 > numFrames ) {
			size_t resizeFrames = numFrames + mRecorder->getSampleRate();
			CI_LOG_V( "writePos: " << writePos << ", numFrames: " << numFrames << ", resizing frames to: " << resizeFrames );
			mRecorder->setNumFrames( resizeFrames );
		}

		if( mRecorder->getLastOverrun() )
			timeline().apply( &mRecorderOverrunFade, 1.0f, 0.0f, xrunFadeTime );
	}

}

void SamplePlayerNodeTestApp::draw()
{
	gl::clear();

	if( mTestSelector.currentSection() == "recorder" ) {
		audio::BufferRef recordedBuffer = mRecorder->getRecordedCopy();
		drawAudioBuffer( *recordedBuffer, getWindowBounds() );
	}
	else {
		auto bufferPlayer = dynamic_pointer_cast<audio::BufferPlayerNode>( mSamplePlayerNode );
		if( bufferPlayer )
			mWaveformPlot.draw();
		else if( mMonitor && mMonitor->isInitialized() )
			drawAudioBuffer( mMonitor->getBuffer(), getWindowBounds() );

		float readPos = (float)getWindowWidth() * mSamplePlayerNode->getReadPosition() / mSamplePlayerNode->getNumFrames();
		gl::color( ColorA( 0, 1, 0, 0.7f ) );
		gl::drawSolidRect( Rectf( readPos - 2, 0, readPos + 2, (float)getWindowHeight() ) );
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

	drawWidgets( mWidgets );
}

void SamplePlayerNodeTestApp::testConverter()
{
	audio::BufferRef audioBuffer = mSourceFile->loadBuffer();

	size_t destSampleRate = 48000;
	size_t destChannels = 1;
	size_t sourceMaxFramesPerBlock = 512;
	auto converter = audio::dsp::Converter::create( mSourceFile->getSampleRate(), destSampleRate, mSourceFile->getNumChannels(), destChannels, sourceMaxFramesPerBlock );

	CI_LOG_V( "FROM samplerate: " << converter->getSourceSampleRate() << ", channels: " << converter->getSourceNumChannels() << ", frames per block: " << converter->getSourceMaxFramesPerBlock() );
	CI_LOG_V( "TO samplerate: " << converter->getDestSampleRate() << ", channels: " << converter->getDestNumChannels() << ", frames per block: " << converter->getDestMaxFramesPerBlock() );

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

	CI_LOG_V( "seconds: " << timer.getSeconds() );
}

void SamplePlayerNodeTestApp::testWrite()
{
	audio::BufferRef audioBuffer = mSourceFile->loadBuffer();

	try {
		const string fileName = "out.wav";
		audio::TargetFileRef target = audio::TargetFile::create( fileName, mSourceFile->getSampleRate(), mSourceFile->getNumChannels() ); // INT_16
		//	audio::TargetFileRef target = audio::TargetFile::create( fileName, mSourceFile->getSampleRate(), mSourceFile->getNumChannels(), audio::SampleType::FLOAT_32 );

		CI_LOG_V( "writing " << audioBuffer->getNumFrames() << " frames at samplerate: " << mSourceFile->getSampleRate() << ", num channels: " << mSourceFile->getNumChannels() );
		target->write( audioBuffer.get() );
		CI_LOG_V( "...complete." );

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

CINDER_APP( SamplePlayerNodeTestApp, RendererGl, []( App::Settings *settings ) {
	settings->setWindowSize( 1000, 500 );
} )
