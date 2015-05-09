#include "CustomTremoloNode.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace std;

void CustomTremoloNode::initialize()
{
	app::console() << "initializeI() called." << endl;

	// We don't really have any major work to do in this sample's custom Node class, so we just init our parameters.
	mRate = 1.8f;
	mDepth = 0.5f;
	mPhase = 0;
}

void CustomTremoloNode::process( ci::audio::Buffer *buffer )
{
	// calculate the phase increase based on the current samplerate (which is ultimately governed by the audio::Context).
	const float phaseIncr = ( mRate / (float)getSampleRate() ) * 2 * (float)M_PI;

	// The number of frames is the same as getFramesPerBlock() (though it is slightly faster to retrieve that number this way).
	// This is the number of samples per channel in the buffer.
	const size_t numFrames = buffer->getNumFrames();

	// The buffer data is stored as a contigious float array of all channels, meaning that channel 1 begins right after the last sample of channel 0.
	float *data = buffer->getData();

	// In order to account for an arbitrary number of channels, we first loop over frames so that each channel sees the same value for mPhase.
	for( size_t i = 0; i < numFrames; i++ ) {
		// calculate the new phase once per frame, regardless of number of channels.
		mPhase = fmodf( mPhase + phaseIncr, 2 * M_PI );

		// calculate amplitude adjustment so that maximum is 1, minimum is 1 - depth.
		for( size_t ch = 0; ch < buffer->getNumChannels(); ch++ )
			data[ch * numFrames + i] *= 1.0f - ( 1.0f - std::sin( mPhase ) ) * mDepth * 0.5f;
	}
}
