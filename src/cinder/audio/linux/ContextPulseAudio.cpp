/*
 Copyright (c) 2015, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "cinder/audio/linux/ContextPulseAudio.h"
#include "cinder/Utilities.h"

#include <iostream>
#include <pulse/pulseaudio.h>

namespace pulse {

class ScopedLock {
public:
	explicit ScopedLock( pa_threaded_mainloop* mainLoop ) : mMainLoop( mainLoop ) {
		pa_threaded_mainloop_lock( mMainLoop );
	}

	virtual ~ScopedLock() {
		pa_threaded_mainloop_unlock( mMainLoop );
	}

private:
	ScopedLock( const ScopedLock& );
	ScopedLock& operator=( const ScopedLock& );
	pa_threaded_mainloop* mMainLoop = nullptr;	
};

void contextStateCallback( pa_context* context, void* userData )
{
	pa_threaded_mainloop *mainLoop = static_cast<pa_threaded_mainloop*>( userData );
	pa_threaded_mainloop_signal( mainLoop, 0 );	
}

void streamSuccessCallback( pa_stream* stream, int error, void* userData )
{
	pa_threaded_mainloop *mainLoop = static_cast<pa_threaded_mainloop*>( userData );
	pa_threaded_mainloop_signal( mainLoop, 0 );		
}


void waitForOperationCompletion( pa_threaded_mainloop* mainLoop, pa_operation* op ) 
{
	if( ! op ) {
		return;
	}

	while( PA_OPERATION_RUNNING == pa_operation_get_state( op ) ) {
		pa_threaded_mainloop_wait( mainLoop );
	}

	pa_operation_unref( op );
}



} // namespace pulse

namespace cinder { namespace audio { namespace linux {

// ----------------------------------------------------------------------------------------------------
// OutputDeviceNodePulseAudioImpl (private)
// ----------------------------------------------------------------------------------------------------
struct OutputDeviceNodePulseAudioImpl {

	OutputDeviceNodePulseAudioImpl( OutputDeviceNodePulseAudio* parent, const std::shared_ptr<ContextPulseAudio>& context )
		: mParent( parent ), mContextPulseAudio( context )
	{
	}	

	void initPlayer( size_t numChannels, size_t sampleRate, size_t framesPerBlock )
	{
		pa_sample_spec sampleSpec;
		sampleSpec.format 	= PA_SAMPLE_FLOAT32LE;
		sampleSpec.rate 	= sampleRate;
		sampleSpec.channels	= numChannels;

  		pa_channel_map* map = nullptr;
	}

	void destroyPlayer()
	{
		if( nullptr == mStream ) {
			return;
		}

		auto context = mContextPulseAudio.lock();
		pa_threaded_mainloop* mainLoop = context->getPulseAudioMainLoop();

		{			
			pulse::ScopedLock scopedLock( mainLoop );

			pa_operation* op = pa_stream_flush( mStream, &pulse::streamSuccessCallback, mainLoop );
			pulse::waitForOperationCompletion( mainLoop, op );

			pa_stream_disconnect( mStream );
			pa_stream_set_write_callback( mStream, nullptr, nullptr );
			pa_stream_set_state_callback( mStream, nullptr, nullptr );
			pa_stream_unref( mStream );
			mStream = nullptr;
		}
	}

	void play()
	{
	}

	void stop()
	{
	}

	void enqueueSamples( size_t requestedBytes )
	{
	}

	static void playerCallback( pa_stream* stream, size_t requestedBytes, void* userData )
	{		
	}

	OutputDeviceNodePulseAudio*    		mParent = nullptr;
	std::weak_ptr<ContextPulseAudio>	mContextPulseAudio;

	pa_stream*							mStream = nullptr;
};

// ----------------------------------------------------------------------------------------------------
// OutputDeviceNodePulseAudio
// ----------------------------------------------------------------------------------------------------
OutputDeviceNodePulseAudio::OutputDeviceNodePulseAudio( const DeviceRef &device, const Format &format, const std::shared_ptr<ContextPulseAudio> &context )
	: OutputDeviceNode( device, format ), mImpl( new OutputDeviceNodePulseAudioImpl( this, context ) )
{
}

void OutputDeviceNodePulseAudio::initialize()
{
	const size_t sampleRate = getOutputSampleRate();
	const size_t framesPerBlock = getOutputFramesPerBlock();
	const size_t numChannels = getNumChannels();

	if( numChannels > 2 ) {
		throw AudioFormatExc( std::string( "Device can not accommodate " ) + std::to_string( numChannels ) + " output channels, only one or two are supported." );
	}

	mImpl->initPlayer( numChannels, sampleRate, framesPerBlock );
}

void OutputDeviceNodePulseAudio::uninitialize()
{
	mImpl->destroyPlayer();
}

void OutputDeviceNodePulseAudio::enableProcessing()
{
	mImpl->play();
}

void OutputDeviceNodePulseAudio::disableProcessing()
{
	mImpl->stop();
}

// ----------------------------------------------------------------------------------------------------
// ContextPulseAudio
// ----------------------------------------------------------------------------------------------------
ContextPulseAudio::ContextPulseAudio()
{
	mMainLoop = pa_threaded_mainloop_new();
	if( ! mMainLoop ) {
		throw AudioContextExc( "Could not create PulseAudio threaded mainloop" );
	}

	if( 0 != pa_threaded_mainloop_start( mMainLoop ) ) {
		throw AudioContextExc( "Could not start PulseAudio threaded mainloop" );
	}

	// Block the PulseAudio mainloop thread while finish the setup
	{
		pulse::ScopedLock scopedLock( mMainLoop );

		pa_mainloop_api* api = pa_threaded_mainloop_get_api( mMainLoop );
		mContext = pa_context_new( api, "cinder-audio" );
		if( ! mContext ) {
			throw AudioContextExc( "Could not create PulseAudio context" );
		}

		pa_context_set_state_callback( mContext, &pulse::contextStateCallback, mMainLoop );
		pa_context_connect( mContext, nullptr, PA_CONTEXT_NOFLAGS, nullptr );

		// Wait until context is ready
		while( true ) {
			pa_context_state_t state = pa_context_get_state( mContext );
			
			if( ! PA_CONTEXT_IS_GOOD( state ) ) {
				throw AudioContextExc( "Context connect failed" );
			}	

			if( PA_CONTEXT_READY == state ) {
				break;
			}

			pa_threaded_mainloop_wait( mMainLoop );
		}
	}
}

ContextPulseAudio::~ContextPulseAudio()
{
	if( nullptr == mMainLoop ) {
		return;
	}

	{
		pulse::ScopedLock scopedLock( mMainLoop );

		if( nullptr != mContext ) {
			pa_context_set_state_callback( mContext, nullptr, nullptr );
			pa_context_disconnect( mContext );
			pa_context_unref( mContext );
			mContext = nullptr;
		}
	}

	pa_threaded_mainloop_stop( mMainLoop );
	pa_threaded_mainloop_free( mMainLoop );
	mMainLoop = nullptr;	
}

OutputDeviceNodeRef	ContextPulseAudio::createOutputDeviceNode( const DeviceRef &device, const Node::Format &format )
{
	auto thisRef = std::dynamic_pointer_cast<ContextPulseAudio>( shared_from_this() );
	return makeNode( new OutputDeviceNodePulseAudio( device, format, thisRef ) );
}

InputDeviceNodeRef ContextPulseAudio::createInputDeviceNode( const DeviceRef &device, const Node::Format &format  )
{
	auto thisRef = std::dynamic_pointer_cast<ContextPulseAudio>( shared_from_this() );
	return makeNode( new InputDeviceNodePulseAudio( device, format, thisRef ) );
}

} } } // namespace cinder::audio::linux