#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/GainNode.h"
#include "cinder/audio/ChannelRouterNode.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/SampleRecorderNode.h"
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/audio/Exception.h"

#include "../../common/AudioTestGui.h"

// TODO: check iOS 6+ interruption handlers via notification

const double RECORD_SECONDS = 2.0;

using namespace ci;
using namespace ci::app;
using namespace std;

class DeviceTestApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;

	void setOutputDevice( const audio::DeviceRef &device, size_t numChannels = 0 );
	void setInputDevice( const audio::DeviceRef &device, size_t numChannels = 0 );
	void setupMultiChannelDevice( const string &deviceName );
	void setupMultiChannelDeviceWindows( const string &deviceName );
	void printDeviceDetails( const audio::DeviceRef &device );
	void startRecording();
	void writeRecordedToFile();

	void setupSine();
	void setupNoise();
	void setupInputPulled();
	void setupIOClean();
	void setupIOProcessed();
	void setupIOAndSine();
	void setupSend();
	void setupSendStereo();

	void setupTest( string test );
	void setupUI();
	void processTap( ivec2 pos );
	void processDrag( ivec2 pos );
	void keyDown( KeyEvent event );

	audio::InputDeviceNodeRef		mInputDeviceNode;
	audio::OutputDeviceNodeRef		mOutputDeviceNode;
	audio::MonitorNodeRef			mMonitor;
	audio::BufferRecorderNodeRef	mRecorder;
	audio::GainNodeRef				mGain;
	audio::GenNodeRef				mGen;

	vector<TestWidget *> mWidgets;
	VSelector mTestSelector, mInputSelector, mOutputSelector;
	Button mPlayButton, mRecordButton;
	HSlider mGainSlider;
	TextInput mSamplerateInput, mFramesPerBlockInput, mNumInChannelsInput, mNumOutChannelsInput, mSendChannelInput;

	Anim<float> mInputDeviceNodeUnderrunFade, mInputDeviceNodeOverrunFade, mOutputDeviceNodeClipFade;
	Anim<float> mViewYOffset; // for iOS keyboard
	Rectf mUnderrunRect, mOverrunRect, mClipRect;
};

void DeviceTestApp::setup()
{
	console() << audio::Device::printDevicesToString();

	auto ctx = audio::master();

	mMonitor = ctx->makeNode( new audio::MonitorNode( audio::MonitorNode::Format().windowSize( 1024 ) ) );
	mGain = ctx->makeNode( new audio::GainNode() );
	mGain->setValue( 0.4f );

	mGain->connect( mMonitor );

	setOutputDevice( audio::Device::getDefaultOutput() );
	setInputDevice( audio::Device::getDefaultInput() );
	//setInputDevice( audio::Device::getDefaultInput(), 1 ); // force mono input

	//setupMultiChannelDevice( "PreSonus FIREPOD (1431)" );
//	setupMultiChannelDeviceWindows( "MOTU Analog (MOTU Audio Wave for 64 bit)" );

	mRecorder = ctx->makeNode( new audio::BufferRecorderNode( RECORD_SECONDS * ctx->getSampleRate() ) );
	mRecorder->setEnabled( false );
	mGain >> mRecorder;

	setupUI();
	setupTest( mTestSelector.currentSection() );

//	setupInputPulled();
//	setupIOClean();

	PRINT_GRAPH( ctx );
	CI_LOG_V( "Context samplerate: " << ctx->getSampleRate() );
}

void DeviceTestApp::setOutputDevice( const audio::DeviceRef &device, size_t numChannels )
{
	if( ! device ) {
		CI_LOG_E( "Empty DeviceRef" );
		return;
	}

	auto ctx = audio::master();

	ctx->uninitializeAllNodes();

	audio::Node::Format format;
	if( numChannels )
		format.channels( numChannels );

	mOutputDeviceNode = ctx->createOutputDeviceNode( device, format );

	mOutputDeviceNode->getDevice()->getSignalParamsDidChange().connect(
							[this] {
								CI_LOG_V( "OutputDeviceNode params changed:" );
								printDeviceDetails( mOutputDeviceNode->getDevice() );
								PRINT_GRAPH( audio::master() );
							} );


	// TODO: if this call is moved to after the mMonitor->connect(), there is a chance that initialization can
	// take place with samplerate / frames-per-block derived from the default OutputNode (ses default Device)
	// Double check this doesn't effect anyone, if it does then setOutput may need to do more work to update Nodes.
	ctx->setOutput( mOutputDeviceNode );

	mMonitor->connect( mOutputDeviceNode );

	ctx->initializeAllNodes();

	CI_LOG_V( "OutputDeviceNode device properties: " );
	printDeviceDetails( device );

	// TODO: considering doing this automatically in Context::setOutput, but then also have to worry about initialize()
	// - also may do a ScopedEnable that handles Context as well.
	if( mPlayButton.mEnabled )
		mOutputDeviceNode->enable();
}

void DeviceTestApp::setInputDevice( const audio::DeviceRef &device, size_t numChannels  )
{
	if( ! device ) {
		CI_LOG_E( "Empty DeviceRef" );
		return;
	}

	audio::ScopedEnableNode enableNodeScope( mInputDeviceNode, false );

	if( mInputDeviceNode )
		mInputDeviceNode->disconnectAllOutputs();

	auto format = audio::Node::Format().autoEnable();
	if( numChannels )
		format.channels( numChannels );

	mInputDeviceNode = audio::master()->createInputDeviceNode( device, format );

	CI_LOG_V( "InputDeviceNode device properties: " );
	printDeviceDetails( device );
}

void DeviceTestApp::setupMultiChannelDevice( const string &deviceName )
{
	auto dev = audio::Device::findDeviceByName( deviceName );
	CI_ASSERT( dev );

//	setOutputDevice( dev );
//	setInputDevice( dev );

	setOutputDevice( dev, dev->getNumOutputChannels() );
	setInputDevice( dev, dev->getNumInputChannels() );

}

void DeviceTestApp::setupMultiChannelDeviceWindows(  const string &deviceName )
{
	audio::DeviceRef inputDev, outputDev;

	for( auto &dev : audio::Device::getDevices() ) {
		if( dev->getName() == deviceName ) {
			if( dev->getNumOutputChannels() > 2 )
				outputDev = dev;
			if( dev->getNumInputChannels() > 2 )
				inputDev = dev;
		}
	}

	if( outputDev )
		setOutputDevice( outputDev, outputDev->getNumOutputChannels() );
	else
		CI_LOG_E( "could not find output device with channels > 2 named: " << deviceName );

	if( inputDev )
		setInputDevice( inputDev, inputDev->getNumInputChannels() );
	else
		CI_LOG_E( "could not find input device with channels > 2 named: " << deviceName );
}

void DeviceTestApp::printDeviceDetails( const audio::DeviceRef &device )
{
	console() << "\t name: " << device->getName() << endl;
	console() << "\t output channels: " << device->getNumOutputChannels() << endl;
	console() << "\t input channels: " << device->getNumInputChannels() << endl;
	console() << "\t samplerate: " << device->getSampleRate() << endl;
	console() << "\t block size: " << device->getFramesPerBlock() << endl;

	bool isSyncIO = mInputDeviceNode && mOutputDeviceNode && ( mInputDeviceNode->getDevice() == mOutputDeviceNode->getDevice() && ( mInputDeviceNode->getNumChannels() == mOutputDeviceNode->getNumChannels() ) );

	console() << "\t sync IO: " << boolalpha << isSyncIO << dec << endl;
}

void DeviceTestApp::setupSine()
{
	mGen = audio::master()->makeNode( new audio::GenSineNode() );
	mGen->setFreq( 440 );

	mGen->connect( mGain );
	mGen->enable();
}

void DeviceTestApp::setupNoise()
{
	mGen = audio::master()->makeNode( new audio::GenNoiseNode() );

	mGen->connect( mGain );
	mGen->enable();
}

void DeviceTestApp::setupInputPulled()
{
	mOutputDeviceNode->disconnectAllInputs();

	mInputDeviceNode >> mGain >> mMonitor;
	mInputDeviceNode->enable();
}

void DeviceTestApp::setupIOClean()
{
	mInputDeviceNode->connect( mGain );
	mInputDeviceNode->enable();
}

void DeviceTestApp::setupIOProcessed()
{
	auto ctx = audio::master();
	auto mod = ctx->makeNode( new audio::GenSineNode( audio::Node::Format().autoEnable() ) );
	mod->setFreq( 200 );

	auto ringMod = audio::master()->makeNode( new audio::GainNode );
	ringMod->setName( "RingModGain" );
	ringMod->getParam()->setProcessor( mod );

	mGain->disconnectAllInputs();
	mInputDeviceNode >> ringMod >> mGain;

	mInputDeviceNode->enable();
}

void DeviceTestApp::setupIOAndSine()
{
	mGen = audio::master()->makeNode( new audio::GenSineNode() );
	mGen->setFreq( 440 );

	mGen->connect( mGain );
	mGen->enable();

	mInputDeviceNode->connect( mGain );
	mInputDeviceNode->enable();
}

void DeviceTestApp::setupSend()
{
	auto ctx = audio::master();
	ctx->disconnectAllNodes();

	auto router = ctx->makeNode( new audio::ChannelRouterNode( audio::Node::Format().channels( mOutputDeviceNode->getNumChannels()	) ) );

	mGen = audio::master()->makeNode( new audio::GenSineNode( 440 ) );

	auto input = mGen;

	int channelIndex = mSendChannelInput.getValue();
	CI_LOG_V( "routing input to channel: " << channelIndex );

	input >> mGain >> router->route( 0, channelIndex );
	router >> mMonitor >> ctx->getOutput();

	mGen->enable();
}

void DeviceTestApp::setupSendStereo()
{
	auto ctx = audio::master();
	ctx->disconnectAllNodes();

	auto router = ctx->makeNode( new audio::ChannelRouterNode( audio::Node::Format().channels( mOutputDeviceNode->getNumChannels()	) ) );
	auto upmix = ctx->makeNode( new audio::Node( audio::Node::Format().channels( 2 ) ) );

	int channelIndex = mSendChannelInput.getValue();
	CI_LOG_V( "routing input to channel: " << channelIndex );

	mGen >> upmix >> mGain >> router->route( 0, channelIndex );
	router >> mMonitor >> ctx->getOutput();

	mGen->enable();
}

void DeviceTestApp::startRecording()
{
	CI_LOG_I( "begin record..." );

	mRecorder->start();
	timeline().add( [this] {
		writeRecordedToFile();
		mRecorder->disable();
	}, timeline().getCurrentTime() + RECORD_SECONDS );
}

void DeviceTestApp::writeRecordedToFile()
{
	const string fileName = "recorder_out.wav";
	CI_LOG_V( "writing to: " << fileName );

	mRecorder->writeToFile( fileName );

	CI_LOG_V( "...complete." );
}

void DeviceTestApp::setupUI()
{
	mInputDeviceNodeUnderrunFade = mInputDeviceNodeOverrunFade = mOutputDeviceNodeClipFade = 0;
	mViewYOffset = 0;

	mPlayButton = Button( true, "stopped", "playing" );
	mWidgets.push_back( &mPlayButton );

	mRecordButton = Button( false, "record" );
	mWidgets.push_back( &mRecordButton );

	mTestSelector.mSegments.push_back( "sinewave" );
	mTestSelector.mSegments.push_back( "noise" );
	mTestSelector.mSegments.push_back( "input (pulled)" );
	mTestSelector.mSegments.push_back( "I/O (clean)" );
	mTestSelector.mSegments.push_back( "I/O (processed)" );
	mTestSelector.mSegments.push_back( "I/O and sine" );
	mTestSelector.mSegments.push_back( "send" );
	mTestSelector.mSegments.push_back( "send stereo" );
	mWidgets.push_back( &mTestSelector );

#if defined( CINDER_COCOA_TOUCH )
	mPlayButton.mBounds = Rectf( 0, 0, 120, 60 );
	mRecordButton.mBounds = Rectf( 130, 0, 190, 34 );
	mTestSelector.mBounds = Rectf( getWindowWidth() - 190, 0, getWindowWidth(), 180 );
#else
	mPlayButton.mBounds = Rectf( 0, 0, 200, 60 );
	mRecordButton.mBounds = Rectf( 210, 0, 310, 40 );
	mTestSelector.mBounds = Rectf( getWindowCenter().x + 110, 0, (float)getWindowWidth(), 180 );
#endif

	mGainSlider.mBounds = Rectf( mTestSelector.mBounds.x1, mTestSelector.mBounds.y2 + 10, mTestSelector.mBounds.x2, mTestSelector.mBounds.y2 + 50 );
	mGainSlider.mTitle = "GainNode";
	mGainSlider.set( mGain->getValue() );
	mWidgets.push_back( &mGainSlider );

	mOutputSelector.mTitle = "Output Devices";
	mOutputSelector.mBounds = Rectf( mTestSelector.mBounds.x1, getWindowCenter().y + 40, (float)getWindowWidth(), (float)getWindowHeight() );
	if( mOutputDeviceNode ) {
		for( const auto &dev : audio::Device::getOutputDevices() ) {
			if( dev == mOutputDeviceNode->getDevice() )
				mOutputSelector.mCurrentSectionIndex = mOutputSelector.mSegments.size();
			mOutputSelector.mSegments.push_back( dev->getName() );
		}
	}
	mWidgets.push_back( &mOutputSelector );

	mInputSelector.mTitle = "Input Devices";
	mInputSelector.mBounds = mOutputSelector.mBounds - vec2( mOutputSelector.mBounds.getWidth() + 10, 0 );
	if( mOutputDeviceNode ) {
		for( const auto &dev : audio::Device::getInputDevices() ) {
			if( dev == mInputDeviceNode->getDevice() )
				mInputSelector.mCurrentSectionIndex = mInputSelector.mSegments.size();
			mInputSelector.mSegments.push_back( dev->getName() );
		}
	}
	mWidgets.push_back( &mInputSelector );

	Rectf textInputBounds( 0, getWindowCenter().y + 40, 200, getWindowCenter().y + 70  );
	mSamplerateInput.mBounds = textInputBounds;
	mSamplerateInput.mTitle = "samplerate";
	mSamplerateInput.setValue( audio::master()->getSampleRate() );
	mWidgets.push_back( &mSamplerateInput );

	textInputBounds += vec2( 0, textInputBounds.getHeight() + 24 );
	mFramesPerBlockInput.mBounds = textInputBounds;
	mFramesPerBlockInput.mTitle = "frames per block";
	mFramesPerBlockInput.setValue( audio::master()->getFramesPerBlock() );
	mWidgets.push_back( &mFramesPerBlockInput );

	textInputBounds += vec2( 0, textInputBounds.getHeight() + 24 );
	mNumInChannelsInput.mBounds = textInputBounds;
	mNumInChannelsInput.mTitle = "num inputs";
	if( mInputDeviceNode )
		mNumInChannelsInput.setValue( mInputDeviceNode->getNumChannels() );
	mWidgets.push_back( &mNumInChannelsInput );

	textInputBounds += vec2( 0, textInputBounds.getHeight() + 24 );
	mNumOutChannelsInput.mBounds = textInputBounds;
	mNumOutChannelsInput.mTitle = "num outputs";
	if( mOutputDeviceNode )
		mNumOutChannelsInput.setValue( mOutputDeviceNode->getNumChannels() );
	mWidgets.push_back( &mNumOutChannelsInput );

	textInputBounds += vec2( 0, textInputBounds.getHeight() + 24 );
	mSendChannelInput.mBounds = textInputBounds;
	mSendChannelInput.mTitle = "send channel";
	mSendChannelInput.setValue( 2 );
	mWidgets.push_back( &mSendChannelInput );

	vec2 xrunSize( 80, 26 );
	mUnderrunRect = Rectf( 0, mPlayButton.mBounds.y2 + 10, xrunSize.x, mPlayButton.mBounds.y2 + xrunSize.y + 10 );
	mOverrunRect = mUnderrunRect + vec2( xrunSize.x + 10, 0 );
	mClipRect = mOverrunRect + vec2( xrunSize.x + 10, 0 );

	getWindow()->getSignalMouseDown().connect( [this] ( MouseEvent &event ) { processTap( event.getPos() ); } );
	getWindow()->getSignalMouseDrag().connect( [this] ( MouseEvent &event ) { processDrag( event.getPos() ); } );
	getWindow()->getSignalTouchesBegan().connect( [this] ( TouchEvent &event ) { processTap( event.getTouches().front().getPos() ); } );
	getWindow()->getSignalTouchesMoved().connect( [this] ( TouchEvent &event ) {
		for( const TouchEvent::Touch &touch : getActiveTouches() )
			processDrag( touch.getPos() );
	} );

#if defined( CINDER_COCOA_TOUCH )
	getSignalKeyboardWillShow().connect( [this] { timeline().apply( &mViewYOffset, -100.0f, 0.3f, EaseInOutCubic() );	} );
	getSignalKeyboardWillHide().connect( [this] { timeline().apply( &mViewYOffset, 0.0f, 0.3f, EaseInOutCubic() ); } );
#endif

	gl::enableAlphaBlending();
}

void DeviceTestApp::processDrag( ivec2 pos )
{
	if( mGainSlider.hitTest( pos ) )
		mGain->getParam()->applyRamp( mGainSlider.mValueScaled, 0.025f );
}

void DeviceTestApp::processTap( ivec2 pos )
{
//	TextInput *selectedInput = false;
	if( mPlayButton.hitTest( pos ) )
		audio::master()->setEnabled( ! audio::master()->isEnabled() );
	else if( mRecordButton.hitTest( pos ) )
		startRecording();
	else if( mSamplerateInput.hitTest( pos ) ) {
	}
	else if( mFramesPerBlockInput.hitTest( pos ) ) {
	}
	else if( mNumInChannelsInput.hitTest( pos ) ) {
	}
	else if( mNumOutChannelsInput.hitTest( pos ) ) {
	}
	else if( mSendChannelInput.hitTest( pos ) ) {
	}

#if defined( CINDER_COCOA_TOUCH )
	TextInput *currentSelected = TextInput::getCurrentSelected();
	if( currentSelected )
		showKeyboard( KeyboardOptions().type( KeyboardType::NUMERICAL ).initialString( currentSelected->mInputString ) );
#endif

	size_t currentTestIndex = mTestSelector.mCurrentSectionIndex;
	if( mTestSelector.hitTest( pos ) && currentTestIndex != mTestSelector.mCurrentSectionIndex ) {
		string currentTest = mTestSelector.currentSection();
		CI_LOG_V( "selected: " << currentTest );

		setupTest( currentTest );
		return;
	}

	size_t currentOutputIndex = mOutputSelector.mCurrentSectionIndex;
	if( mOutputSelector.hitTest( pos ) && currentOutputIndex != mOutputSelector.mCurrentSectionIndex ) {
		auto dev = audio::Device::findDeviceByName( mOutputSelector.mSegments[mOutputSelector.mCurrentSectionIndex] );
		CI_LOG_V( "selected output device named: " << dev->getName() << ", key: " << dev->getKey() );

		setOutputDevice( dev );
		// Don't need to reset test as the Device will be updated on the Context's OutputDeviceNode and the change will propagate through the graph
		return;
	}

	size_t currentInputIndex = mInputSelector.mCurrentSectionIndex;
	if( mInputSelector.hitTest( pos ) && currentInputIndex != mInputSelector.mCurrentSectionIndex ) {
		auto dev = audio::Device::findDeviceByName( mInputSelector.mSegments[mInputSelector.mCurrentSectionIndex] );
		CI_LOG_V( "selected input named: " << dev->getName() << ", key: " << dev->getKey() );

		setInputDevice( dev );
		setupTest( mTestSelector.currentSection() ); // need to reset the test so that we construct the InputDeviceNode with the proper Device
		return;
	}
}

void DeviceTestApp::setupTest( string test )
{
	if( test.empty() )
		test = "sinewave";

	CI_LOG_V( "test: " << test );

	// FIXME: Switching from 'noise' to 'i/o' on mac is causing a deadlock when initializing InputDeviceNodeAudioUnit.
	//	- it shouldn't have to be stopped, need to check why.
	//  - temp fix: stop / start context around reconfig
	audio::master()->disable();

	mGain->disconnectAllInputs();

	if( test == "sinewave" )
		setupSine();
	else if( test == "noise" )
		setupNoise();
	else if( test == "input (pulled)" )
		setupInputPulled();
	else if( test == "I/O (clean)" )
		setupIOClean();
	else if( test == "I/O (processed)" )
		setupIOProcessed();
	else if( test == "I/O and sine" )
		setupIOAndSine();
	else if( test == "send" )
		setupSend();
	else if( test == "send stereo" )
		setupSendStereo();
	else
		CI_ASSERT_NOT_REACHABLE();

	if( mPlayButton.mEnabled )
		audio::master()->enable();

	PRINT_GRAPH( audio::master() );
}

void DeviceTestApp::keyDown( KeyEvent event )
{
	TextInput *currentSelected = TextInput::getCurrentSelected();
	if( ! currentSelected )
		return;

	if( event.getCode() == KeyEvent::KEY_RETURN ) {
#if defined( CINDER_COCOA_TOUCH )
		hideKeyboard();
#endif

		try {
			if( currentSelected == &mSamplerateInput ) {
				int sr = currentSelected->getValue();
				CI_LOG_V( "updating samplerate from: " << mOutputDeviceNode->getSampleRate() << " to: " << sr );
				mOutputDeviceNode->getDevice()->updateFormat( audio::Device::Format().sampleRate( sr ) );
			}
			else if( currentSelected == &mFramesPerBlockInput ) {
				int frames = currentSelected->getValue();
				CI_LOG_V( "updating frames per block from: " << mOutputDeviceNode->getFramesPerBlock() << " to: " << frames );
				mOutputDeviceNode->getDevice()->updateFormat( audio::Device::Format().framesPerBlock( frames ) );
			}
			else if( currentSelected == &mNumInChannelsInput ) {
				int numChannels = currentSelected->getValue();
				CI_LOG_V( "updating nnm input channels from: " << mInputDeviceNode->getNumChannels() << " to: " << numChannels );
				setInputDevice( mInputDeviceNode->getDevice(), numChannels );
			}
			else if( currentSelected == &mNumOutChannelsInput ) {
				int numChannels = currentSelected->getValue();
				CI_LOG_V( "updating nnm output channels from: " << mOutputDeviceNode->getNumChannels() << " to: " << numChannels );
				setOutputDevice( mOutputDeviceNode->getDevice(), numChannels );
			}
			else if( currentSelected == &mSendChannelInput ) {
				if( mTestSelector.currentSection() == "send" || mTestSelector.currentSection() == "send stereo" )
					setupTest( mTestSelector.currentSection() );
			}
			else
				CI_LOG_E( "unhandled return for string: " << currentSelected->mInputString );
		}
		catch( audio::AudioDeviceExc &exc ) {
			CI_LOG_E( "AudioDeviceExc caught, what: " << exc.what() );
			auto ctx = audio::master();
			mSamplerateInput.setValue( ctx->getSampleRate() );
			mFramesPerBlockInput.setValue( ctx->getFramesPerBlock() );
			return;
		}
	}
	else {
		if( event.getCode() == KeyEvent::KEY_BACKSPACE )
			currentSelected->processBackspace();
		else {
			currentSelected->processChar( event.getChar() );
		}
	}
}

void DeviceTestApp::update()
{
	const float xrunFadeTime = 1.3f;

	if( mInputDeviceNode ) {
		if( mInputDeviceNode->getLastUnderrun() )
			timeline().apply( &mInputDeviceNodeUnderrunFade, 1.0f, 0.0f, xrunFadeTime );
		if( mInputDeviceNode->getLastOverrun() )
			timeline().apply( &mInputDeviceNodeOverrunFade, 1.0f, 0.0f, xrunFadeTime );
	}

	if( mOutputDeviceNode ) {
		if( mOutputDeviceNode->getLastClip() )
			timeline().apply( &mOutputDeviceNodeClipFade, 1.0f, 0.0f, xrunFadeTime );
	}
}

void DeviceTestApp::draw()
{
	gl::clear();
	gl::color( 0, 0.9f, 0 );

	gl::pushMatrices();
	gl::translate( 0, mViewYOffset );

	if( mMonitor && mMonitor->isEnabled() ) {
		const audio::Buffer &buffer = mMonitor->getBuffer();

		float padding = 20;
		float waveHeight = ((float)getWindowHeight() - padding * 3.0f ) / (float)buffer.getNumChannels();

		float yOffset = padding;
		float xScale = (float)getWindowWidth() / (float)buffer.getNumFrames();
		for( size_t ch = 0; ch < buffer.getNumChannels(); ch++ ) {
			PolyLine2f waveform;
			const float *channel = buffer.getChannel( ch );
			for( size_t i = 0; i < buffer.getNumFrames(); i++ ) {
				float x = i * xScale;
				float y = ( channel[i] * 0.5f + 0.5f ) * waveHeight + yOffset;
				waveform.push_back( vec2( x, y ) );
			}
			gl::draw( waveform );
			yOffset += waveHeight + padding;
		}

		float volume = mMonitor->getVolume();
		Rectf volumeRect( mGainSlider.mBounds.x1, mGainSlider.mBounds.y2 + padding, mGainSlider.mBounds.x1 + mGainSlider.mBounds.getWidth() * volume, mGainSlider.mBounds.y2 + padding + 20 );
		gl::drawSolidRect( volumeRect );
	}

	drawWidgets( mWidgets );

	if( mInputDeviceNodeUnderrunFade > 0.0001f ) {
		gl::color( ColorA( 0.8f, 0.2f, 0, mInputDeviceNodeUnderrunFade ) );
		gl::drawSolidRect( mUnderrunRect );
		gl::drawStringCentered( "in underrun", mUnderrunRect.getCenter(), Color::black() );
	}
	if( mInputDeviceNodeOverrunFade > 0.0001f ) {
		gl::color( ColorA( 0.8f, 0.2f, 0, mInputDeviceNodeOverrunFade ) );
		gl::drawSolidRect( mOverrunRect );
		gl::drawStringCentered( "in overrun", mOverrunRect.getCenter(), Color::black() );
	}
	if( mOutputDeviceNodeClipFade > 0.0001f ) {
		gl::color( ColorA( 0.8f, 0.2f, 0, mOutputDeviceNodeClipFade ) );
		gl::drawSolidRect( mClipRect );
		gl::drawStringCentered( "out clip", mClipRect.getCenter(), Color::black() );
	}

	gl::popMatrices();
}

CINDER_APP( DeviceTestApp, RendererGl, []( App::Settings *settings ) {
	settings->setWindowSize( 800, 600 );
	settings->setWindowPos( 6, 30 );
} )
