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
#include "cinder/audio/dsp/Converter.h"

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
std::cout << "OutputDeviceNodePulseAudioImpl::initPlayer: framesPerBlock=" << framesPerBlock << std::endl;		
		auto context = mContextPulseAudio.lock();

		auto paMainLoop = context->getPulseAudioMainLoop();
		pulse::ScopedLock scopedLock( paMainLoop );

		//mBytesPerBuffer = 512*numChannels*sizeof(float);
		mBytesPerSample = sizeof(float);
		mBytesPerFrame  = numChannels*mBytesPerSample;
		mBytesPerBuffer = framesPerBlock*mBytesPerFrame;

		pa_sample_spec sampleSpec;
		sampleSpec.format 	= PA_SAMPLE_FLOAT32LE;
		sampleSpec.rate 	= sampleRate;
		sampleSpec.channels	= numChannels;

		// nullptr for now
		pa_channel_map* map = nullptr;

		{
			pulse::ScopedPropertyList propList;
			//pa_proplist_sets( propList.get(), PA_PROP_APPLICATION_ICON_NAME, "cinder-pulseaudio-output" );

			auto paContext = context->getPulseAudioContext();
			mStream = pa_stream_new_with_proplist( paContext, "Playback", &sampleSpec, map, propList.get() );
			if( ! mStream ) {
				int errorNum = pa_context_errno( paContext );
				std::string errorText = pa_strerror( errorNum );
				throw AudioContextExc( "Could not create PulseAudio output stream" + errorText );
			}

			pa_stream_set_state_callback( mStream, &OutputDeviceNodePulseAudioImpl::streamNotifyCallback, static_cast<void*>( this ) );

			// Even though we start the stream corked above, PulseAudio will issue one stream request 
			// after setup. OutputDeviceNodePulseAudioImpl::playerCallback() must fulfill the write.
			pa_stream_set_write_callback( mStream, &OutputDeviceNodePulseAudioImpl::playerCallback, static_cast<void*>( this ) );

			pa_buffer_attr bufferAttr;
			bufferAttr.maxlength	= static_cast<uint32_t>(-1);
			bufferAttr.minreq		= mBytesPerBuffer / 2;
			bufferAttr.prebuf		= static_cast<uint32_t>(-1);
			bufferAttr.tlength		= mBytesPerBuffer * 3;
			bufferAttr.fragsize		= static_cast<uint32_t>(-1);

			pa_stream_flags_t streamFlags = static_cast<pa_stream_flags_t>( PA_STREAM_START_CORKED | PA_STREAM_AUTO_TIMING_UPDATE | PA_STREAM_INTERPOLATE_TIMING );
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
		auto context = mContextPulseAudio.lock();
		auto paMainLoop = context->getPulseAudioMainLoop();
		auto paContext = context->getPulseAudioContext();

		{		
			pulse::ScopedLock scopedLock( paMainLoop );

			// Ensure the context and stream are ready.
			pa_context_state_t contextState = pa_context_get_state( paContext );
			pa_stream_state_t streamState = pa_stream_get_state( mStream );
			if( ( PA_CONTEXT_READY != contextState ) && ( PA_STREAM_READY != streamState ) ) {
				return;
			}

			 // Uncork (resume) the stream.
			pa_operation* op = pa_stream_cork( mStream, 0, &pulse::streamSuccessCallback, paMainLoop );
			pulse::waitForOperationCompletion( paMainLoop, op );

			mPlaying = true;
		}
	}

	void stop()
	{
		auto context = mContextPulseAudio.lock();
		auto paMainLoop = context->getPulseAudioMainLoop();

		{		
			pulse::ScopedLock scopedLock( paMainLoop );
			
			mPlaying = false;

			// Flush the stream prior to cork, doing so after will cause hangs.  Write
			// callbacks are suspended while inside pa_threaded_mainloop_lock() so this
			// is all thread safe.
			pa_operation* op = pa_stream_flush( mStream, &pulse::streamSuccessCallback, paMainLoop );
			pulse::waitForOperationCompletion( paMainLoop, op );

			op = pa_stream_cork( mStream, 1, &pulse::streamSuccessCallback, paMainLoop );
			pulse::waitForOperationCompletion( paMainLoop, op );
		}		
	}

	void enqueueSamples( size_t requestedBytes )
	{
		mBuffer = nullptr;
		size_t bytesToFill = requestedBytes;
		int ret = pa_stream_begin_write( mStream, &mBuffer, &bytesToFill );

		if( ret >= 0 ) {
			mNumSamplesRequested = bytesToFill / mBytesPerSample;

//std::cout << requestedBytes << " | " << ( mNumSamplesRequested * mBytesPerSample ) << std::endl;

			size_t filledBytes = mNumSamplesRequested * mBytesPerSample;
			if( requestedBytes != filledBytes ) {
				std::cout << "Misaligned: " << requestedBytes << "|" << filledBytes << std::endl;
			}
			

			if( mPlaying ) {
				while( mNumSamplesBuffered < mNumSamplesRequested ) {
					mParent->renderInputs();
				}
				mNumSamplesBuffered = mNumSamplesRequested;
			}
			else {
				std::memset( mBuffer, 0, mNumSamplesRequested*mBytesPerSample );
				mNumSamplesBuffered = mNumSamplesRequested;
			}

			ret = pa_stream_write( mStream, mBuffer, mNumSamplesBuffered*mBytesPerSample, nullptr, 0, PA_SEEK_RELATIVE );
		}

		mNumSamplesRequested = 0;
		mNumSamplesBuffered = 0;
	}

	static void playerCallback( pa_stream* stream, size_t requestedBytes, void* userData )
	{		
		OutputDeviceNodePulseAudioImpl* thisObj = static_cast<OutputDeviceNodePulseAudioImpl*>( userData );
		thisObj->enqueueSamples( requestedBytes );
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
	void*								mBuffer = nullptr;
	size_t								mNumSamplesRequested = 0;

	size_t 								mBytesPerSample = 0;
	size_t 								mBytesPerFrame = 0;
	size_t								mBytesPerBuffer = 0;

	size_t								mNumSamplesBuffered = 0;

	bool 								mPlaying = false;
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
	const size_t sampleRate = getOutputSampleRate();
	const size_t framesPerBlock = getOutputFramesPerBlock();
	const size_t numChannels = getNumChannels();

	mImpl->initPlayer( numChannels, sampleRate, framesPerBlock );

	mInterleavedBuffer = BufferInterleaved( framesPerBlock, numChannels );
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

void OutputDeviceNodePulseAudio::renderInputs()
{
	auto ctx = getContext();
	if( ! ctx )
		return;

	std::lock_guard<std::mutex> lock( ctx->getMutex() );

	// verify context still exists, since its destructor may have been holding the lock
	ctx = getContext();
	if( ! ctx )
		return;

	ctx->preProcess();

	auto internalBuffer = getInternalBuffer();
	internalBuffer->zero();
	pullInputs( internalBuffer );

	if( checkNotClipping() )
		internalBuffer->zero();

	dsp::interleaveBuffer( internalBuffer, &mInterleavedBuffer );	

std::cout << mInterleavedBuffer.getSize() << std::endl;

/*
	size_t numSamplesAvailable =  mInterleavedBuffer.getSize() / mImpl->mBytesPerSample;
	numSamplesAvailable = std::min( numSamplesAvailable, mImpl->mNumSamplesRequested );
	
	uint8_t *dst = (uint8_t*)(mImpl->mBuffer) + (mImpl->mNumSamplesBuffered * mImpl->mBytesPerSample);
	const uint8_t *src = reinterpret_cast<const uint8_t*>( mInterleavedBuffer.getData() );
	size_t nbytes = numSamplesAvailable * mImpl->mBytesPerSample;
	std::memcpy( dst, src, nbytes );

	mImpl->mNumSamplesBuffered += numSamplesAvailable;
*/

/*
	size_t nbytes = mImpl->mNumSamplesRequested*mImpl->mBytesPerSample;
	nbytes = std::min( nbytes, mInterleavedBuffer.getSize() );

	void *dst = mImpl->mBuffer + (mImpl->mNumSamplesBuffered * mImpl->mBytesPerSample);
	std::memcpy( mImpl->mBuffer, mInterleavedBuffer.getData(), nbytes );

	mImpl->mNumSamplesBuffered += mImpl->mNumSamplesRequested;
*/

	ctx->postProcess();
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
	auto thisRef = std::dynamic_pointer_cast<ContextPulseAudio>( shared_from_this() );
	return makeNode( new OutputDeviceNodePulseAudio( device, format, thisRef ) );
}

InputDeviceNodeRef ContextPulseAudio::createInputDeviceNode( const DeviceRef &device, const Node::Format &format  )
{
	//auto thisRef = std::dynamic_pointer_cast<ContextPulseAudio>( shared_from_this() );
	//return makeNode( new InputDeviceNodePulseAudio( device, format, thisRef ) );

	InputDeviceNodeRef result;
	return result;
}

} } } // namespace cinder::audio::linux
