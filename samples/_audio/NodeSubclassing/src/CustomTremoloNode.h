#pragma once

#include "cinder/Cinder.h"
#include "cinder/audio/Node.h"

typedef std::shared_ptr<class CustomTremoloNode>	CustomTremoloNodeRef;

// This exemplary Node subclass provides a tremelo effect, modulating the incoming audio signal's gain
// with a low frequency, typically inbetween 0.1 and 20 hertz.
class CustomTremoloNode : public ci::audio::Node {
  public:
	// Construct a new CustomTremoloNode and pass in optional audio::Format parameters.
	// IMPORTANT: Each Node has a weak reference to a audio::Context, but this is not available from within the Node's constructor.
	//	- instead, you can override initialize() for any setup that requires a valid audio::Context.
	CustomTremoloNode( const Format &format = Format() ) : Node( format )	{}

	// Set the frequency of oscillation, typically under 20 hertz for tremelo.
	void setRate( float rate )		{ mRate = rate; }
	// Set the depth of maximum amplitude decrease (max amp = mag(1 - mDepth))
	void setDepth( float depth )	{ mDepth = depth; }
	
  protected:
	// The base Node class calls a few optional lifecycle methods, initialize() and process() are the most important of these.
	// See Node's header for a complete list of methods available for overriding.

	// Called when it is time to use the Node, typically after a connection. This is a good place to initialize large buffers or other resources.
	// Note that initialize() may be called more than once within the lifetime of a Node, examples include if it is connected later to inputs or outputs
	// with a different number of channels, or if the Context's samplerate or frames per block changes.
	void initialize()							override;

	// Override process() to perform signal processing. The Buffer object passed in will contain processed samples from any
	// connected inputs on the way in, and then the processed buffer is handed to any connected outputs on the way out.
	void process( ci::audio::Buffer *buffer )	override;

  private:
	// in this sample, variables that are modifiable from the UI thread are atomic for thread safety.
	std::atomic<float> mRate;
	std::atomic<float> mDepth;

	// the phase is not exposed in the public API, so it does not need to be stored as atomic.
	float mPhase;
};
