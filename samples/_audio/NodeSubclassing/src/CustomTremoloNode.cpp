#include "CustomTremoloNode.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace std;

void CustomTremoloNode::initialize()
{
	app::console() << "initializeI() called." << endl;

	// We don't really have any major work to do in this sample's custom Node class, so we just init our parameters.
	mRate = 0.2f;
	mDepth = 0.3f;
}

void CustomTremoloNode::process( ci::audio::Buffer *buffer )
{
	// calculate the phase increase based on the current samplerate (which is ultimately governed by the audio::Context).
	float phaseIncr = ( mRate / (float)getSampleRate() ) * 2 * (float)M_PI;

	// FIXME: this doesn't work for stereo
	for( size_t ch = 0; ch < buffer->getNumChannels(); ch++ ) {
		float *channel = buffer->getChannel( ch );

		for( size_t i = 0; i < buffer->getNumFrames(); i++ )	{
			mPhase = fmodf( mPhase + phaseIncr, 2 * M_PI );
			channel[i] *= 1.0f - ( 1.0f - std::sin( mPhase ) ) * mDepth * 0.5f;
		}
	}
}
