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

#if defined( CINDER_MSW )
#include "cinder/audio/msw/ContextWasapi.h"
#endif

#include "../../common/AudioTestGui.h"
#include "../../../../samples/_audio/common/AudioDrawUtils.h"

const bool USE_SECONDARY_SCREEN = 1;
const bool WASAPI_EXCLUSIVE_MODE = 0;
const bool SET_FRAMES_PER_BLOCK = 0;
const size_t FRAMES_PER_BLOCK = 256;
const double RECORD_SECONDS = 4.0;

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
	void setupRolandOctaCaptureInputMonitoring();
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
	void updateUIParams();

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
	console() << audio::Device::printDevicesToString() << endl;

	auto ctx = audio::master();

#if defined( CINDER_MSW )
	if( WASAPI_EXCLUSIVE_MODE ) {
		CI_LOG_I( "enabling WASAPI exlusive mode" );
		dynamic_cast<audio::msw::ContextWasapi *>( ctx )->setExclusiveModeEnabled();
	}
#endif

	mMonitor = ctx->makeNode( new audio::MonitorNode( audio::MonitorNode::Format().windowSize( 480 ) ) );
	mGain = ctx->makeNode( new audio::GainNode() );
	//mGain = ctx->makeNode( new audio::GainNode( audio::Node::Format().channels( 1 ) ) ); // force mix down to mono
	mGain->setValue( 0.8f );

	mGain->connect( mMonitor );

	setOutputDevice( audio::Device::getDefaultOutput() );
//	setOutputDevice( audio::Device::getDefaultOutput(), 1 );
	//setOutputDevice( audio::Device::findOutputByName( "1-2 (OCTA-CAPTURE)" ) );

	setInputDevice( audio::Device::getDefaultInput() );
	//setInputDevice( audio::Device::getDefaultInput(), 1 );
	//setupRolandOctaCaptureInputMonitoring();

	//setupMultiChannelDevice( "PreSonus FIREPOD (1431)" );
	//setupMultiChannelDeviceWindows( "MOTU Analog (MOTU Audio Wave for 64 bit)" );

	mRecorder = ctx->makeNode( new audio::BufferRecorderNode( RECORD_SECONDS * ctx->getSampleRate() ) );
	mRecorder->setEnabled( false );
	mGain >> mRecorder;

	setupUI();
	setupTest( mTestSelector.currentSection() );

	//setupInputPulled();
	//setupIOClean();

	PRINT_GRAPH( ctx );
	CI_LOG_I( "Context samplerate: " << ctx->getSampleRate() << ", frames per block: " << ctx->getFramesPerBlock() );
}

void DeviceTestApp::setOutputDevice( const audio::DeviceRef &device, size_t numChannels )
{
	if( ! device ) {
		CI_LOG_E( "Empty DeviceRef" );
		return;
	}

	if( SET_FRAMES_PER_BLOCK ) {
		device->updateFormat( audio::Device::Format().framesPerBlock( FRAMES_PER_BLOCK ) );
	}

	auto ctx = audio::master();

	ctx->uninitializeAllNodes();

	audio::Node::Format format;
	if( numChannels )
		format.channels( numChannels );

	mOutputDeviceNode = ctx->createOutputDeviceNode( device, format );

	mOutputDeviceNode->getDevice()->getSignalParamsDidChange().connect( -1,
							[this] {
								CI_LOG_I( "OutputDeviceNode params changed:" );
								printDeviceDetails( mOutputDeviceNode->getDevice() );
								updateUIParams();
								//PRINT_GRAPH( audio::master() );
							} );


	// TODO: if this call is moved to after the mMonitor->connect(), there is a chance that initialization can
	// take place with samplerate / frames-per-block derived from the default OutputNode (ses default Device)
	// Double check this doesn't effect anyone, if it does then setOutput may need to do more work to update Nodes.
	ctx->setOutput( mOutputDeviceNode );

	mMonitor->connect( mOutputDeviceNode );

	ctx->initializeAllNodes();

	CI_LOG_I( "OutputDeviceNode device properties: " );
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

	if( SET_FRAMES_PER_BLOCK ) {
		device->updateFormat( audio::Device::Format().framesPerBlock( FRAMES_PER_BLOCK ) );
	}

	if( mInputDeviceNode )
		mInputDeviceNode->disconnectAllOutputs();

	auto format = audio::Node::Format().autoEnable();
	if( numChannels )
		format.channels( numChannels );

	mInputDeviceNode = audio::master()->createInputDeviceNode( device, format );

	CI_LOG_I( "InputDeviceNode device properties: " );
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
	auto outputDev = audio::Device::findOutputByName( deviceName );
	if( outputDev )
		setOutputDevice( outputDev, outputDev->getNumOutputChannels() );
	else
		CI_LOG_E( "could not find output device named: " << deviceName );

	auto inputDev = audio::Device::findInputByName( deviceName );
	if( inputDev )
		setInputDevice( inputDev, inputDev->getNumInputChannels() );
	else
		CI_LOG_E( "could not find input device named: " << deviceName );
}

void DeviceTestApp::setupRolandOctaCaptureInputMonitoring()
{
	vector<string> deviceNames = {
		"1-2 (OCTA-CAPTURE)",
		"3-4 (OCTA-CAPTURE)",
		"5-6 (OCTA-CAPTURE)"//,
		//"7-8 (OCTA-CAPTURE)",
		//"9-10 (OCTA-CAPTURE)"
	};

	size_t numChannels = deviceNames.size() * 2;
	auto channelRouter = audio::master()->makeNode<audio::ChannelRouterNode>( audio::Node::Format().channels( numChannels ) );

	channelRouter >> mGain;

	for( size_t i = 0; i < deviceNames.size(); i++ ) {
		auto devName = deviceNames[i];
		auto inputDev = audio::Device::findInputByName( devName );
		if( inputDev ) {
			if( SET_FRAMES_PER_BLOCK ) {
				inputDev->updateFormat( audio::Device::Format().framesPerBlock( FRAMES_PER_BLOCK ) );
			}

			auto inputNode = audio::master()->createInputDeviceNode( inputDev );
			inputNode >> channelRouter->route( 0, i * 2 );
			inputNode->enable();
		}
		else
			CI_LOG_E( "could not find input device named: " << devName );
	}
}

void DeviceTestApp::printDeviceDetails( const audio::DeviceRef &device )
{
	console() << "\t name: " << device->getName() << endl;
	console() << "\t output channels: " << device->getNumOutputChannels() << endl;
	console() << "\t input channels: " << device->getNumInputChannels() << endl;
	console() << "\t samplerate: " << device->getSampleRate() << endl;
	console() << "\t frames per block: " << device->getFramesPerBlock() << endl;

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
	if( ! mInputDeviceNode ) {
		CI_LOG_E( "no InputDeviceNode" );
		return;
	}

	mOutputDeviceNode->disconnectAllInputs();

	mInputDeviceNode >> mGain >> mMonitor;
	mInputDeviceNode->enable();
}

void DeviceTestApp::setupIOClean()
{
	if( ! mInputDeviceNode ) {
		CI_LOG_E( "no InputDeviceNode" );
		return;
	}

	mInputDeviceNode->connect( mGain );
	mInputDeviceNode->enable();
}

void DeviceTestApp::setupIOProcessed()
{
	if( ! mInputDeviceNode ) {
		CI_LOG_E( "no InputDeviceNode" );
		return;
	}

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

	if( ! mInputDeviceNode ) {
		CI_LOG_E( "no InputDeviceNode" );
		return;
	}

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
	CI_LOG_I( "routing input to channel: " << channelIndex );

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
	CI_LOG_I( "routing input to channel: " << channelIndex );

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
	CI_LOG_I( "writing to: " << fileName );

	mRecorder->writeToFile( fileName );

	CI_LOG_I( "...complete." );
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
	mTestSelector.mCurrentSectionIndex = 0;
	mWidgets.push_back( &mTestSelector );

#if defined( CINDER_COCOA_TOUCH )
	mPlayButton.mBounds = Rectf( 0, 0, 120, 60 );
	mRecordButton.mBounds = Rectf( 130, 0, 190, 34 );
	mTestSelector.mBounds = Rectf( getWindowWidth() - 190, 0, getWindowWidth(), 180 );
#else
	mPlayButton.mBounds = Rectf( 0, 0, 200, 60 );
	mRecordButton.mBounds = Rectf( 210, 0, 310, 40 );
#endif
	mTestSelector.mBounds = Rectf( (float)getWindowWidth() - 210, 0, (float)getWindowWidth(), 180 );

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
	if( mInputDeviceNode ) {
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
		CI_LOG_I( "selected: " << currentTest );

		setupTest( currentTest );
		return;
	}

	size_t currentOutputIndex = mOutputSelector.mCurrentSectionIndex;
	if( mOutputSelector.hitTest( pos ) && currentOutputIndex != mOutputSelector.mCurrentSectionIndex ) {
		auto dev = audio::Device::findOutputByName( mOutputSelector.mSegments[mOutputSelector.mCurrentSectionIndex] );
		CI_LOG_I( "selected output device named: " << dev->getName() << ", key: " << dev->getKey() );

		setOutputDevice( dev );
		// Don't need to reset test as the Device will be updated on the Context's OutputDeviceNode and the change will propagate through the graph
		return;
	}

	size_t currentInputIndex = mInputSelector.mCurrentSectionIndex;
	if( mInputSelector.hitTest( pos ) && currentInputIndex != mInputSelector.mCurrentSectionIndex ) {
		auto dev = audio::Device::findInputByName( mInputSelector.mSegments[mInputSelector.mCurrentSectionIndex] );
		CI_LOG_I( "selected input named: " << dev->getName() << ", key: " << dev->getKey() );

		setInputDevice( dev );
		setupTest( mTestSelector.currentSection() ); // need to reset the test so that we construct the InputDeviceNode with the proper Device
		return;
	}
}

void DeviceTestApp::setupTest( string test )
{
	if( test.empty() )
		test = "sinewave";

	CI_LOG_I( "test: " << test );

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
				CI_LOG_I( "updating samplerate from: " << mOutputDeviceNode->getSampleRate() << " to: " << sr );
				mOutputDeviceNode->getDevice()->updateFormat( audio::Device::Format().sampleRate( sr ) );
			}
			else if( currentSelected == &mFramesPerBlockInput ) {
				int frames = currentSelected->getValue();
				CI_LOG_I( "updating frames per block from: " << mOutputDeviceNode->getFramesPerBlock() << " to: " << frames );
				mOutputDeviceNode->getDevice()->updateFormat( audio::Device::Format().framesPerBlock( frames ) );
			}
			else if( currentSelected == &mNumInChannelsInput ) {
				int numChannels = currentSelected->getValue();
				CI_LOG_I( "updating nnm input channels from: " << mInputDeviceNode->getNumChannels() << " to: " << numChannels );
				setInputDevice( mInputDeviceNode->getDevice(), numChannels );
			}
			else if( currentSelected == &mNumOutChannelsInput ) {
				int numChannels = currentSelected->getValue();
				CI_LOG_I( "updating nnm output channels from: " << mOutputDeviceNode->getNumChannels() << " to: " << numChannels );
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

	//if( getElapsedFrames() % 20 == 0 ) {
	//	CI_LOG_I( "framerate: " << to_string( getAverageFps() ) );
	//}
}

void DeviceTestApp::updateUIParams()
{
	auto ctx = audio::master();
	
	if( mSamplerateInput.getValue() != ctx->getSampleRate() ) {
		mSamplerateInput.setValue( ctx->getSampleRate() );
	}
	if( mFramesPerBlockInput.getValue() != ctx->getFramesPerBlock() ) {
		mFramesPerBlockInput.setValue( ctx->getFramesPerBlock() );
	}
}

void DeviceTestApp::draw()
{
	gl::clear();
	gl::color( 0, 0.9f, 0 );

	gl::pushMatrices();
	gl::translate( 0, mViewYOffset );

	if( mMonitor && mMonitor->isEnabled() ) {

		Rectf scopeRect( 10, 10, (float)getWindowWidth() - 10, (float)getWindowHeight() - 10 );
		drawAudioBuffer( mMonitor->getBuffer(), scopeRect, true );

		float volume = mMonitor->getVolume();
		const float padding = 20;
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
	bool useSecondaryScreen = ( USE_SECONDARY_SCREEN && Display::getDisplays().size() > 1 );

	if( useSecondaryScreen ) {
		for( const auto &display : Display::getDisplays() ) {
			//CI_LOG_I( "display name: " << display->getName() );
			if( display->getName() == "Color LCD" ) {
				// macbook
				settings->setDisplay( display );
				settings->setWindowSize( 1280, 720 );
			}
			else if( display->getName() == "Generic PnP Monitor" ) {
				// gechic 1303i 13"touch display
				settings->setDisplay( display );
				settings->setFullScreen( true );
			}
		}
	}
	else {
		settings->setWindowPos( 0, 0 );
		settings->setWindowSize( 960, 565 );
	}
} )
