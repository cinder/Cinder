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
	explicit ScopedLock( pa_threaded_mainloop* mainLoop ) : mMainLoop( mainLoop ) { pa_threaded_mainloop_lock( mMainLoop ); }
	virtual ~ScopedLock() { pa_threaded_mainloop_unlock( mMainLoop ); }
private:
	ScopedLock( const ScopedLock& );
	ScopedLock& operator=( const ScopedLock& );
	pa_threaded_mainloop* mMainLoop = nullptr;	
};

class ScopedPropertyList {
public:
	ScopedPropertyList() : mPropertyList( pa_proplist_new() ) {}
	virtual ~ScopedPropertyList() { pa_proplist_free( mPropertyList ); }
	pa_proplist* get() const { return mPropertyList; }
private:
	ScopedPropertyList( const ScopedPropertyList& );
	ScopedPropertyList& operator=( const ScopedPropertyList& );	
	pa_proplist* mPropertyList = nullptr;
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
std::cout << "OutputDeviceNodePulseAudioImpl::initPlayer" << std::endl;		
		auto context = mContextPulseAudio.lock();

		auto paMainLoop = context->getPulseAudioMainLoop();
		pulse::ScopedLock scopedLock( paMainLoop );

		pa_sample_spec sampleSpec;
		sampleSpec.format 	= PA_SAMPLE_FLOAT32LE;
		sampleSpec.rate 	= sampleRate;
		sampleSpec.channels	= numChannels;

		// nullptr for now
		pa_channel_map* map = nullptr;

		{
			pulse::ScopedPropertyList propList;
			pa_proplist_sets( propList.get(), PA_PROP_APPLICATION_ICON_NAME, "cinder-pulseaudio-output" );

			auto paContext = context->getPulseAudioContext();
			mStream = pa_stream_new_with_proplist( paContext, "Playback", &sampleSpec, map, propList.get() );
			if( 0 != mStream ) {
				throw AudioContextExc( "Could not create PulseAudio output stream" );
			}

			pa_stream_set_state_callback( mStream, &OutputDeviceNodePulseAudioImpl::streamNotifyCallback, static_cast<void*>( this ) );

			// Even though we start the stream corked above, PulseAudio will issue one stream request 
			// after setup. OutputDeviceNodePulseAudioImpl::playerCallback() must fulfill the write.
			pa_stream_set_write_callback( mStream, &OutputDeviceNodePulseAudioImpl::playerCallback, static_cast<void*>( this ) );


			const size_t kBytesPerSample = sizeof(float);
			size_t bytesPerBuffer = 512*numChannels*kBytesPerSample;
			pa_buffer_attr bufferAttr;
			bufferAttr.maxlength 	= static_cast<uint32_t>(-1);
			bufferAttr.minreq		= bytesPerBuffer / 2;
			bufferAttr.prebuf		= static_cast<uint32_t>(-1);
			bufferAttr.tlength		= bytesPerBuffer * 3;
			bufferAttr.fragsize		= static_cast<uint32_t>(-1);

			pa_stream_flags_t streamFlags = PA_STREAM_START_CORKED;
			if( 0 != pa_stream_connect_playback( mStream, nullptr, &bufferAttr, streamFlags, nullptr, nullptr ) ) {
				throw AudioContextExc( "Could not connect PulseAudio output stream playback" );	
			}

			// Wait for the stream to be ready.
			while( true ) {
				pa_stream_state_t state = pa_stream_get_state( mStream );
				if( ! PA_STREAM_IS_GOOD( state ) ) {
					throw AudioContextExc( "Stream connect failed" );	
				}

				if( PA_STREAM_READY == state ) {
					break;
				}

				pa_threaded_mainloop_wait( paMainLoop );
			}
		}
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
std::cout << "OutputDeviceNodePulseAudioImpl::playerCallback: requestedBytes=" << requestedBytes << std::endl;
	}

	static void streamNotifyCallback( pa_stream* stream, void* userData )
	{
		OutputDeviceNodePulseAudioImpl* thisObj = static_cast<OutputDeviceNodePulseAudioImpl*>( userData );

		// Forward unexpected failures to the AudioSourceCallback if available.  All
		// these variables are only modified under pa_threaded_mainloop_lock() so this
		// should be thread safe.
		/*
		if( stream && stream->source_callback_ && ( PA_STREAM_FAILED == pa_stream_get_state( stream ) ) {
			stream->source_callback_->OnError(stream);
		}
		*/

		auto context = thisObj->mContextPulseAudio.lock();

		auto mainloop = context->getPulseAudioMainLoop();
		pa_threaded_mainloop_signal( mainloop, 0 );
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
std::cout << "OutputDeviceNodePulseAudio::OutputDeviceNodePulseAudio" << std::endl;		
}

void OutputDeviceNodePulseAudio::initialize()
{
std::cout << "OutputDeviceNodePulseAudio::initialize" << std::endl;		
	const size_t sampleRate = getOutputSampleRate();
	const size_t framesPerBlock = getOutputFramesPerBlock();
	const size_t numChannels = getNumChannels();

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
std::cout << "ContextPulseAudio::ContextPulseAudio ENTER" << std::endl;

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

std::cout << "ContextPulseAudio::ContextPulseAudio EXIT" << std::endl;
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
std::cout << "ContextPulseAudio::createOutputDeviceNode" << std::endl;	
	//auto thisRef = std::dynamic_pointer_cast<ContextPulseAudio>( shared_from_this() );
	//return makeNode( new OutputDeviceNodePulseAudio( device, format, thisRef ) );

OutputDeviceNodeRef result;
return result;
}

InputDeviceNodeRef ContextPulseAudio::createInputDeviceNode( const DeviceRef &device, const Node::Format &format  )
{
	//auto thisRef = std::dynamic_pointer_cast<ContextPulseAudio>( shared_from_this() );
	//return makeNode( new InputDeviceNodePulseAudio( device, format, thisRef ) );

	InputDeviceNodeRef result;
	return result;
}

} } } // namespace cinder::audio::linux