#include "DeviceTest.h"

#include "cinder/app/App.h"
#include "cinder/gl/draw.h"
#include "cinder/gl/scoped.h"
#include "cinder/Log.h"
#include "cinder/Timeline.h"
#include "cinder/CinderImGui.h"


#include "cinder/audio/Context.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/GainNode.h"
#include "cinder/audio/ChannelRouterNode.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/SampleRecorderNode.h"
#include "cinder/audio/dsp/Dsp.h"
#include "cinder/audio/Exception.h"

#include "../../../../samples/_audio/common/AudioDrawUtils.h"

const bool SET_FRAMES_PER_BLOCK = false;
const size_t FRAMES_PER_BLOCK = 256;
const double RECORD_SECONDS = 4.0;

using namespace ci;
using namespace std;
namespace im = ImGui;


DeviceTest::DeviceTest()
{
	mSubTests = {
		"sinewave",
		"noise",
		"input (pulled)",
		"I/O (clean)",
		"I/O (processed)",
		"I/O and sine",
		"send",
		"send stereo"
	};

	// TODO: show this in the GUI
	app::console() << audio::Device::printDevicesToString() << endl;

	auto ctx = audio::master();

	mMonitor = ctx->makeNode( new audio::MonitorNode( audio::MonitorNode::Format().windowSize( 480 ) ) );
	mGain = ctx->makeNode( new audio::GainNode() );
	//mGain = ctx->makeNode( new audio::GainNode( audio::Node::Format().channels( 1 ) ) ); // force mix down to mono
	mGain->setValue( 0.4f );

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

	setupSubTest( mSubTests[mCurrentSubTest] );

	//setupInputPulled();
	//setupIOClean();
}

void DeviceTest::setOutputDevice( const audio::DeviceRef &device, size_t numChannels )
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
								//updateUIParams();
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
}

void DeviceTest::setInputDevice( const audio::DeviceRef &device, size_t numChannels  )
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

void DeviceTest::setupMultiChannelDevice( const string &deviceName )
{
	auto dev = audio::Device::findDeviceByName( deviceName );
	CI_ASSERT( dev );

//	setOutputDevice( dev );
//	setInputDevice( dev );

	setOutputDevice( dev, dev->getNumOutputChannels() );
	setInputDevice( dev, dev->getNumInputChannels() );
}

void DeviceTest::setupMultiChannelDeviceWindows(  const string &deviceName )
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

void DeviceTest::setupRolandOctaCaptureInputMonitoring()
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

void DeviceTest::printDeviceDetails( const audio::DeviceRef &device )
{
	app::console() << "\t name: " << device->getName() << endl;
	app::console() << "\t output channels: " << device->getNumOutputChannels() << endl;
	app::console() << "\t input channels: " << device->getNumInputChannels() << endl;
	app::console() << "\t samplerate: " << device->getSampleRate() << endl;
	app::console() << "\t frames per block: " << device->getFramesPerBlock() << endl;

	bool isSyncIO = mInputDeviceNode && mOutputDeviceNode && ( mInputDeviceNode->getDevice() == mOutputDeviceNode->getDevice() && ( mInputDeviceNode->getNumChannels() == mOutputDeviceNode->getNumChannels() ) );

	app::console() << "\t sync IO: " << boolalpha << isSyncIO << dec << endl;
}

void DeviceTest::setupSine()
{
	mGen = audio::master()->makeNode( new audio::GenSineNode() );
	mGen->setFreq( 440 );

	mGen->connect( mGain );
	mGen->enable();
}

void DeviceTest::setupNoise()
{
	mGen = audio::master()->makeNode( new audio::GenNoiseNode() );

	mGen->connect( mGain );
	mGen->enable();
}

void DeviceTest::setupInputPulled()
{
	if( ! mInputDeviceNode ) {
		CI_LOG_E( "no InputDeviceNode" );
		return;
	}

	mOutputDeviceNode->disconnectAllInputs();

	mInputDeviceNode >> mGain >> mMonitor;
	mInputDeviceNode->enable();
}

void DeviceTest::setupIOClean()
{
	if( ! mInputDeviceNode ) {
		CI_LOG_E( "no InputDeviceNode" );
		return;
	}

	mInputDeviceNode->connect( mGain );
	mInputDeviceNode->enable();
}

void DeviceTest::setupIOProcessed()
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

void DeviceTest::setupIOAndSine()
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

void DeviceTest::setupSend()
{
	auto ctx = audio::master();
	ctx->disconnectAllNodes();

	auto router = ctx->makeNode( new audio::ChannelRouterNode( audio::Node::Format().channels( mOutputDeviceNode->getNumChannels()	) ) );

	mGen = audio::master()->makeNode( new audio::GenSineNode( 440 ) );

	auto input = mGen;

	CI_LOG_I( "routing input to channel: " << mSendChannel );

	input >> mGain >> router->route( 0, mSendChannel );
	router >> mMonitor >> ctx->getOutput();

	mGen->enable();
}

void DeviceTest::setupSendStereo()
{
	auto ctx = audio::master();
	ctx->disconnectAllNodes();

	auto router = ctx->makeNode( new audio::ChannelRouterNode( audio::Node::Format().channels( mOutputDeviceNode->getNumChannels()	) ) );
	auto upmix = ctx->makeNode( new audio::Node( audio::Node::Format().channels( 2 ) ) );

	CI_LOG_I( "routing input to channel: " << mSendChannel );

	mGen >> upmix >> mGain >> router->route( 0, mSendChannel );
	router >> mMonitor >> ctx->getOutput();

	mGen->enable();
}

void DeviceTest::startRecording()
{
	CI_LOG_I( "begin record..." );

	mRecorder->start();
	app::timeline().add( [this] {
		writeRecordedToFile();
		mRecorder->disable();
	}, app::timeline().getCurrentTime() + RECORD_SECONDS );
}

void DeviceTest::writeRecordedToFile()
{
	const string fileName = "recorder_out.wav";
	CI_LOG_I( "writing to: " << fileName );

	mRecorder->writeToFile( fileName );

	CI_LOG_I( "...complete." );
}

#if 0
void DeviceTest::setupUI()
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

void DeviceTest::processDrag( ivec2 pos )
{
	if( mGainSlider.hitTest( pos ) )
		mGain->getParam()->applyRamp( mGainSlider.mValueScaled, 0.025f );
}

void DeviceTest::processTap( ivec2 pos )
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

#endif

void DeviceTest::setupSubTest( const string &test )
{
	CI_LOG_I( "test: " << test );

	audio::ScopedEnableContext disableContext( audio::master(), false );
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
}

#if 0
void DeviceTest::keyDown( KeyEvent event )
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

#endif

void DeviceTest::update()
{
	const float xrunFadeTime = 1.3f;

	if( mInputDeviceNode ) {
		if( mInputDeviceNode->getLastUnderrun() )
			app::timeline().apply( &mInputDeviceNodeUnderrunFade, 1.0f, 0.0f, xrunFadeTime );
		if( mInputDeviceNode->getLastOverrun() )
			app::timeline().apply( &mInputDeviceNodeOverrunFade, 1.0f, 0.0f, xrunFadeTime );
	}

	if( mOutputDeviceNode ) {
		if( mOutputDeviceNode->getLastClip() )
			app::timeline().apply( &mOutputDeviceNodeClipFade, 1.0f, 0.0f, xrunFadeTime );
	}
}

void DeviceTest::draw()
{
	gl::ScopedMatrices scopedMat;

	gl::pushMatrices();
	gl::translate( 0, mViewYOffset );

	if( mMonitor && mMonitor->isEnabled() ) {

		Rectf scopeRect( 10, 10, (float)app::getWindowWidth() - 10, (float)app::getWindowHeight() - 10 );
		drawAudioBuffer( mMonitor->getBuffer(), scopeRect, true );

		float volume = mMonitor->getVolume();
		const float padding = 20;
		Rectf volumeRect( padding, padding, padding * 2, padding + volume * 200 );
		gl::drawSolidRect( volumeRect );
	}

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


// -----------------------------------------------------------------------
// UI
// -----------------------------------------------------------------------

void DeviceTest::updateUI()
{
	float gain = mGain->getValue();
	if( im::SliderFloat( "gain", &gain, 0, 1 ) ) {
		mGain->setValue( gain );
	}

	if( im::Button( "record" ) ) {
		startRecording();
	}

	if( im::Button( "print devices" ) ) {
		app::console() << audio::Device::printDevicesToString() << endl;
	}

	const int maxVisibleDevices = 8;

	const auto &inputDevices = audio::Device::getInputDevices();
	int currentInputIndex = 0;
	vector<string> inputNames;
	for( int i = 0; i < inputDevices.size(); i++ ) {
		inputNames.push_back( inputDevices[i]->getName() );
		if( mOutputDeviceNode->getDevice()->getName() == inputNames.back() ) {
			currentInputIndex = i;
		}
	}
	if( im::ListBox( "input devices", &currentInputIndex, inputNames, std::min<int>( maxVisibleDevices, inputNames.size() ) ) ) {
		// TODO: handle change
	}

	const auto &outputDevices = audio::Device::getOutputDevices();
	int currentOutputIndex = 0;
	vector<string> outputNames;
	for( int i = 0; i < outputDevices.size(); i++ ) {
		outputNames.push_back( outputDevices[i]->getName() );
		if( mOutputDeviceNode->getDevice()->getName() == outputNames.back() ) {
			currentOutputIndex = i;
		}
	}
	if( im::ListBox( "output devices", &currentOutputIndex, outputNames, std::min<int>( maxVisibleDevices, outputNames.size() ) ) ) {
		// TODO: handle change
	}

	im::Separator();
	if( im::ListBox( "sub-tests", &mCurrentSubTest, mSubTests, mSubTests.size() ) ) {
		setupSubTest( mSubTests[mCurrentSubTest] );
	}
}

