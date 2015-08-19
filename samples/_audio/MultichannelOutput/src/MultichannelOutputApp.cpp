#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"

#include "cinder/audio/GenNode.h"
#include "cinder/audio/GainNode.h"
#include "cinder/audio/ChannelRouterNode.h"
#include "cinder/audio/MonitorNode.h"

#include "../../common/AudioDrawUtils.h"

// NOTE: this sample is meant to exemplify how to address multi-channel audio hardware devices, in the
// sense that they contain more than stereo channels of output. However, regardless of the actual number
// of output channels, it will pick the audio::Device that has the most output channels on your system and use that.

using namespace ci;
using namespace ci::app;
using namespace std;

const float MAX_GAIN		= 0.4f;
const float RAMP_SECONDS	= 1.0f;

class MultichannelOutputApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;

	void setupMultichannelDevice();
	void rampGain();
	void shiftRouteChannel();

	audio::GenNodeRef				mGen;			// Gen's generate audio signals
	audio::GainNodeRef				mGain;			// GainNode modifies the volume of the signal
	audio::MonitorNodeRef			mMonitor;			// Scope lets you retrieve audio samples in a thread-safe manner
	audio::ChannelRouterNodeRef	mChannelRouterNode;	// Scope lets you retrieve audio samples in a thread-safe manner

	size_t mCurrentChannel;
};

void MultichannelOutputApp::setup()
{
	mCurrentChannel = 0;

	setupMultichannelDevice();

	auto ctx = audio::Context::master();

	// for information on creating Node's, see NodeBasic sample.
	mGen = ctx->makeNode( new audio::GenSineNode( 440, audio::Node::Format().autoEnable() ) );
	mGain = ctx->makeNode( new audio::GainNode( 0 ) );

	// make a ChannelRouterNode with the same number of channels as the Context's output, which is the multichannel output device.
	mChannelRouterNode = ctx->makeNode( new audio::ChannelRouterNode( audio::Node::Format().channels( ctx->getOutput()->getNumChannels() ) ) );

	// make a Scope to visualize the channels.  You don't need to specify the number of channels here because its default ChannelMode is MATCHES_INPUT.
	mMonitor = ctx->makeNode( new audio::MonitorNode );

	// connect up our Node's, routing mGain to the output channel specified by mCurrentChannel. Send the result through the Scope, onto the output.
	mGen >> mGain >> mChannelRouterNode->route( 0, mCurrentChannel ) >> mMonitor >> ctx->getOutput();

	ctx->enable();

	// Start the gain ramping. When this finishes, the route channel will be shifted and a new gain ramp will be applied.
	rampGain();
}

void MultichannelOutputApp::setupMultichannelDevice()
{
	// debug print all devices to console
	console() << audio::Device::printDevicesToString() << endl;

	audio::DeviceRef deviceWithMaxOutputs;

	for( const auto &dev : audio::Device::getDevices() ) {
		if( ! deviceWithMaxOutputs || deviceWithMaxOutputs->getNumOutputChannels() < dev->getNumOutputChannels() )
			deviceWithMaxOutputs = dev;
	}

	console() << endl << "max output channels: " << deviceWithMaxOutputs->getNumOutputChannels() << endl;
	getWindow()->setTitle( deviceWithMaxOutputs->getName() );

	auto ctx = audio::master();
	audio::OutputDeviceNodeRef multichannelOutputDeviceNode = ctx->createOutputDeviceNode( deviceWithMaxOutputs, audio::Node::Format().channels( deviceWithMaxOutputs->getNumOutputChannels() ) );
	ctx->setOutput( multichannelOutputDeviceNode );
}

void MultichannelOutputApp::rampGain()
{
	mGain->getParam()->applyRamp( 0, MAX_GAIN, RAMP_SECONDS / 2 );
	mGain->getParam()->appendRamp( 0, RAMP_SECONDS / 2 );
}

void MultichannelOutputApp::shiftRouteChannel()
{
	mCurrentChannel = ( mCurrentChannel + 1 ) % mChannelRouterNode->getNumChannels();

	mChannelRouterNode->disconnectAllInputs();
	mGain->disconnectAllOutputs();
	
	mGain >> mChannelRouterNode->route( 0, mCurrentChannel );
}

void MultichannelOutputApp::update()
{
	if( mGain->getParam()->getNumEvents() == 0 ) {
		shiftRouteChannel();
		rampGain();
	}
}

void MultichannelOutputApp::draw()
{
	gl::clear();

	// Draw the Scope's recorded Buffer, all channels as separate line arrays.
	if( mMonitor && mMonitor->isEnabled() ) {
		Rectf scopeRect( 10, 10, (float)getWindowWidth() - 10, (float)getWindowHeight() - 10 );
		drawAudioBuffer( mMonitor->getBuffer(), scopeRect, true );
	}
}

CINDER_APP( MultichannelOutputApp, RendererGl( RendererGl::Options().msaa( 8 ) ) )
