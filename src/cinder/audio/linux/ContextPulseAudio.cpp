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
#include "cinder/audio/dsp/RingBuffer.h"
#include "cinder/Log.h"

#include <pulse/pulseaudio.h>

namespace pulse {

struct Context;
struct OutputStream;
struct InputStream;

using AudioExc = cinder::audio::AudioExc;
using AudioContextExc = cinder::audio::AudioContextExc;

//! ScopedLock
//!
//!
class ScopedLock {
public:
	explicit ScopedLock( pa_threaded_mainloop* mainLoop ) : mMainLoop( mainLoop ) { pa_threaded_mainloop_lock( mMainLoop ); }
	virtual ~ScopedLock() { pa_threaded_mainloop_unlock( mMainLoop ); }
private:
	ScopedLock( const ScopedLock& );
	ScopedLock& operator=( const ScopedLock& );
	pa_threaded_mainloop* mMainLoop = nullptr;	
};

//! ScopedPropertyList
//!
//!
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

//! Context
//!
//!
struct Context {
	pa_context*				mPaContext	= nullptr;
	pa_threaded_mainloop*	mPaMainLoop = nullptr;

	Context();
	virtual ~Context();

	static std::unique_ptr<Context>	create();
	static void 					stateCallback( pa_context* context, void* userData );
};

Context::Context()
{
	mPaMainLoop = pa_threaded_mainloop_new();
	if( ! mPaMainLoop ) {
		throw AudioContextExc( "Could not create PulseAudio threaded mainloop" );
	}

#if PA_CHECK_VERSION( 5, 0, 0 )
	pa_threaded_mainloop_set_name( mPaMainLoop, "cinder::audio (PulseAudio)" );
#endif

	if( 0 != pa_threaded_mainloop_start( mPaMainLoop ) ) {
		throw AudioContextExc( "Could not start PulseAudio threaded mainloop" );
	}

	// Block the PulseAudio mainloop thread while finish the setup
	{
		pulse::ScopedLock scopedLock( mPaMainLoop );

		pa_mainloop_api* api = pa_threaded_mainloop_get_api( mPaMainLoop );
		mPaContext = pa_context_new( api, "cinder-audio" );
		if( ! mPaContext ) {
			throw AudioContextExc( "Could not create PulseAudio context" );
		}

		pa_context_set_state_callback( mPaContext, &Context::stateCallback, static_cast<void*>( mPaMainLoop ) );
		pa_context_connect( mPaContext, nullptr, PA_CONTEXT_NOFLAGS, nullptr );

		// Wait until context is ready
		while( true ) {
			pa_context_state_t state = pa_context_get_state( mPaContext );
			
			if( ! PA_CONTEXT_IS_GOOD( state ) ) {
				throw AudioContextExc( "Context connect failed" );
			}	

			if( PA_CONTEXT_READY == state ) {
				break;
			}

			pa_threaded_mainloop_wait( mPaMainLoop );
		}
	}
}

Context::~Context()
{
	CI_ASSERT( mPaMainLoop );
	CI_ASSERT( mPaContext );

	{
		pulse::ScopedLock scopedLock( mPaMainLoop );

		pa_context_set_state_callback( mPaContext, nullptr, nullptr );
		pa_context_disconnect( mPaContext );
		pa_context_unref( mPaContext );
		mPaContext = nullptr;
	}

	pa_threaded_mainloop_stop( mPaMainLoop );
	pa_threaded_mainloop_free( mPaMainLoop );
	mPaMainLoop = nullptr;		
}

std::unique_ptr<Context> Context::create() 
{
	std::unique_ptr<Context> result = std::unique_ptr<Context>( new Context() );
	return result;
}

// TODO: fill out this switch?
// - also I think userData should just be the Context, or even ci::audio::linux::ContextPulseAudio
void Context::stateCallback( pa_context* context, void* userData ) 
{
	pa_threaded_mainloop* mainLoop = static_cast<pa_threaded_mainloop*>( userData );

	pa_context_state_t state = pa_context_get_state( context );
	switch( state ) {
		default:
		break;
	}

	pa_threaded_mainloop_signal( mainLoop, 0 );	
}

//! Stream
struct Stream {
	Context*	mContext 	= nullptr;
	pa_stream*	mPaStream 	= nullptr;

	size_t 		mNumChannels 	= 0;
	size_t 		mSampleRate 	= 0;
	size_t		mFramesPerBlock	= 0;
	std::string	mDeviceName;

	size_t 		mBytesPerSample	= 0;
	size_t 		mBytesPerFrame	= 0;
	size_t		mBytesPerBuffer	= 0;


	Stream( Context* context, size_t numChannels, size_t sampleRate, size_t framesPerBlock, const std::string &deviceName );
	virtual ~Stream();

	static void notifyCallback( pa_stream *stream, void* userData );
	static void successCallback( pa_stream *stream, int success, void* userData );
};

Stream::Stream( Context* context, size_t numChannels, size_t sampleRate, size_t framesPerBlock, const std::string &deviceName )
	: mContext( context ), mNumChannels( numChannels ), mSampleRate( sampleRate ), mFramesPerBlock( framesPerBlock ), mDeviceName( deviceName )
{
}

Stream::~Stream()
{
}

void Stream::notifyCallback( pa_stream *stream, void* userData ) 
{
	Stream* thisObj = static_cast<Stream*>( userData );
	pa_threaded_mainloop_signal( thisObj->mContext->mPaMainLoop, 0 );
}

void Stream::successCallback( pa_stream *stream, int success, void* userData ) 
{
	Stream* thisObj = static_cast<Stream*>( userData );
	pa_threaded_mainloop_signal( thisObj->mContext->mPaMainLoop, 0 );
}

//! OutputStream
struct OutputStream : public Stream {
	std::function<void(size_t, void*)>	mSourceFn;

	OutputStream( Context* context, size_t numChannels, size_t sampleRate, size_t framesPerBlock, const std::string &deviceName );
	virtual ~OutputStream();

	static std::unique_ptr<OutputStream> create( Context* context, size_t numChannels, size_t sampleRate, size_t framesPerBlock );

	void open();
	void close();
	void start( std::function<void(size_t, void*)> sourceFn );
	void stop();

	static void writeCallback( pa_stream* stream, size_t requestedBytes, void* userData );
};

OutputStream::OutputStream( Context* context, size_t numChannels, size_t sampleRate, size_t framesPerBlock, const std::string &deviceName )
	: Stream( context, numChannels, sampleRate, framesPerBlock, deviceName )
{
}

OutputStream::~OutputStream()
{	
	stop();
	close();
}

void OutputStream::open()
{
	mBytesPerSample = sizeof( float );
	mBytesPerFrame  = mNumChannels * mBytesPerSample;
	mBytesPerBuffer = mFramesPerBlock * mBytesPerFrame;

	pa_sample_spec sampleSpec;
	sampleSpec.format 	= PA_SAMPLE_FLOAT32LE;
	sampleSpec.rate 	= mSampleRate;
	sampleSpec.channels	= mNumChannels;
	CI_ASSERT( pa_sample_spec_valid( &sampleSpec ) );

	// default channel map for now
	pa_channel_map* channelMap = nullptr;

	{
		pulse::ScopedPropertyList propList;
		//pa_proplist_sets( propList.get(), PA_PROP_APPLICATION_ICON_NAME, "cinder-pulseaudio-output" );

		mPaStream = pa_stream_new_with_proplist( mContext->mPaContext, "Playback", &sampleSpec, channelMap, propList.get() );
		if( ! mPaStream ) {
			int errorNum = pa_context_errno( mContext->mPaContext );
			std::string errorText = pa_strerror( errorNum );
			throw AudioContextExc( "Could not create PulseAudio output stream" + errorText );
		}
		
		pa_stream_set_state_callback( mPaStream, &Stream::notifyCallback, static_cast<void*>( this ) );
	
		// Even though we start the stream corked above, PulseAudio will issue one stream request 
		// after setup. OutputDeviceNodePulseAudioImpl::playerCallback() must fulfill the write.
		pa_stream_set_write_callback( mPaStream, &OutputStream::writeCallback, static_cast<void*>( this ) );

		pa_buffer_attr bufferAttr;
		bufferAttr.maxlength	= uint32_t( mFramesPerBlock * mBytesPerFrame );
		bufferAttr.minreq		= UINT32_MAX;
		bufferAttr.prebuf		= 0;
		bufferAttr.tlength		= bufferAttr.maxlength;
		bufferAttr.fragsize		= UINT32_MAX;

		pa_stream_flags_t streamFlags = pa_stream_flags_t(
				PA_STREAM_START_CORKED |
				PA_STREAM_AUTO_TIMING_UPDATE |
				PA_STREAM_INTERPOLATE_TIMING |
				PA_STREAM_ADJUST_LATENCY );

		int status = pa_stream_connect_playback( mPaStream, mDeviceName.c_str(), &bufferAttr, streamFlags, nullptr, nullptr );
		if( status ) {
			throw AudioContextExc( "Could not connect PulseAudio output stream playback" );	
		}

		// Wait for the stream to be ready.
		while( true ) {
			pa_stream_state_t state = pa_stream_get_state( mPaStream );
			if( ! PA_STREAM_IS_GOOD( state ) ) {
				throw AudioContextExc( "OutputStream connect failed" );
			}

			if( PA_STREAM_READY == state ) {
				break;
			}

			pa_threaded_mainloop_wait( mContext->mPaMainLoop );
		}
	}
}

void OutputStream::close()
{
	pulse::ScopedLock scopedLock( mContext->mPaMainLoop );

	pa_operation* op = pa_stream_flush( mPaStream, &Stream::successCallback, static_cast<void*>( this ) );
	pulse::waitForOperationCompletion( mContext->mPaMainLoop, op );

	pa_stream_disconnect( mPaStream );
	pa_stream_set_write_callback( mPaStream, nullptr, nullptr );
	pa_stream_set_state_callback( mPaStream, nullptr, nullptr );
	pa_stream_unref( mPaStream );
	mPaStream = nullptr;
}

void OutputStream::start( std::function<void(size_t, void*)> sourceFn )
{
	pulse::ScopedLock scopedLock( mContext->mPaMainLoop );
	
	mSourceFn = sourceFn;

	// Ensure the context and stream are ready.
	pa_context_state_t contextState = pa_context_get_state( mContext->mPaContext );
	pa_stream_state_t streamState = pa_stream_get_state( mPaStream );
	if( ( PA_CONTEXT_READY != contextState ) && ( PA_STREAM_READY != streamState ) ) {
		CI_LOG_E( "failed to get context state" );
		// TODO: improve errir reporting here
		return;
	}

	// Uncork (resume) the stream.
	pa_operation* op = pa_stream_cork( mPaStream, 0, &Stream::successCallback, static_cast<void*>( this ) );
	pulse::waitForOperationCompletion( mContext->mPaMainLoop, op );	
}

void OutputStream::stop()
{
	pulse::ScopedLock scopedLock( mContext->mPaMainLoop );

	mSourceFn = nullptr;

	// Flush the stream prior to cork, doing so after will cause hangs. Write callbacks
	// are suspended while inside pa_threaded_mainloop_lock() so this is all thread safe.
	pa_operation* op = pa_stream_flush( mPaStream, &Stream::successCallback, static_cast<void*>( this ) );
	pulse::waitForOperationCompletion( mContext->mPaMainLoop, op );

	op = pa_stream_cork( mPaStream, 1, &Stream::successCallback, static_cast<void*>( this ) );
	pulse::waitForOperationCompletion( mContext->mPaMainLoop, op );
}

void OutputStream::writeCallback( pa_stream* stream, size_t requestedBytes, void* userData )
{
	OutputStream* thisObj = static_cast<OutputStream*>( userData );

	void* buffer = nullptr;
	size_t bytesToFill = requestedBytes;
	int ret = pa_stream_begin_write( thisObj->mPaStream, &buffer, &bytesToFill );

	if( ret >= 0 ) {

		if( thisObj->mSourceFn ) {
			size_t numSamplesToFill = bytesToFill / thisObj->mBytesPerSample;
			thisObj->mSourceFn( numSamplesToFill, buffer );
			ret = pa_stream_write( thisObj->mPaStream, buffer, numSamplesToFill*thisObj->mBytesPerSample, nullptr, 0, PA_SEEK_RELATIVE );
			// @TODO: Handle error with ret
		}
		else {
			std::memset( buffer, 0, bytesToFill );
			ret = pa_stream_write( thisObj->mPaStream, buffer, bytesToFill, nullptr, 0, PA_SEEK_RELATIVE );
		}
	}
}

//! InputStream
//!
//!
struct InputStream : public Stream {

	InputStream( Context* context, size_t numChannels, size_t sampleRate, size_t framesPerBlock, const std::string &deviceName );
	virtual ~InputStream();

	static std::unique_ptr<InputStream> create( Context* context, size_t numChannels, size_t sampleRate, size_t framesPerBlock );

	void open();
	void close();
	void start();
	void stop();
	void enqueueSamples( pa_stream *s, size_t nbytes );

	static void readCallback( pa_stream *stream, size_t nbytes, void *userData );

	ci::audio::BufferDynamic                    mReadBuffer;
	std::vector<ci::audio::dsp::RingBuffer>     mRingBuffers;
	size_t								        mNumFramesBuffered = 0;
	std::function<void ()>                      mMarkOverrunFn;
};

InputStream::InputStream( Context* context, size_t numChannels, size_t sampleRate, size_t framesPerBlock, const std::string &deviceName )
	: Stream( context, numChannels, sampleRate, framesPerBlock, deviceName )
{
	mBytesPerSample = sizeof( float );
	mBytesPerFrame  = mNumChannels * mBytesPerSample;
	mBytesPerBuffer = mFramesPerBlock * mBytesPerFrame;

	// Allocate a big enough buffer size that will accomodate most hardware.
	// TODO: allocate this dynamically based on current latency settings
	const size_t kBufferSizeBytes = 32768;
	size_t numFrames = ( kBufferSizeBytes / mBytesPerSample ) + mFramesPerBlock + 1;
	const size_t ringBufferSize = numFrames * mNumChannels;
	mRingBuffers.resize( numChannels );
	for( auto &ringBuffer : mRingBuffers ) {
		ringBuffer.resize( ringBufferSize );
	}

	mReadBuffer.setSize( framesPerBlock, numChannels );
}

InputStream::~InputStream()
{
	stop();
	close();
}

void InputStream::open()
{
	pa_sample_spec sampleSpec;
	sampleSpec.format 	= PA_SAMPLE_FLOAT32LE;
	sampleSpec.rate 	= mSampleRate;
	sampleSpec.channels	= mNumChannels;
	CI_ASSERT( pa_sample_spec_valid( &sampleSpec ) );

	// default channel map for now
	pa_channel_map* channelMap = nullptr;

	{
		pulse::ScopedPropertyList propList;
		//pa_proplist_sets( propList.get(), PA_PROP_APPLICATION_ICON_NAME, "cinder-pulseaudio-input" );

		mPaStream = pa_stream_new_with_proplist( mContext->mPaContext, "Record", &sampleSpec, channelMap, propList.get() );
		if( ! mPaStream ) {
			int errorNum = pa_context_errno( mContext->mPaContext );
			std::string errorText = pa_strerror( errorNum );
			throw AudioContextExc( "Could not create PulseAudio input stream" + errorText );
		}

		pa_stream_set_state_callback( mPaStream, &Stream::notifyCallback, static_cast<void*>( this ) );
		pa_stream_set_read_callback( mPaStream, &InputStream::readCallback, static_cast<void*>( this ) );

		pa_buffer_attr bufferAttr;
		bufferAttr.maxlength	= UINT32_MAX;
		bufferAttr.minreq		= UINT32_MAX;
		bufferAttr.prebuf		= 0;
		bufferAttr.tlength		= UINT32_MAX;
		bufferAttr.fragsize		= uint32_t( mFramesPerBlock * mBytesPerFrame );

		pa_stream_flags_t streamFlags = pa_stream_flags_t(
				PA_STREAM_START_CORKED |
				PA_STREAM_AUTO_TIMING_UPDATE |
				PA_STREAM_INTERPOLATE_TIMING |
				PA_STREAM_ADJUST_LATENCY );

		int status = pa_stream_connect_record( mPaStream, mDeviceName.c_str(), &bufferAttr, streamFlags );
		if( status ) {
			throw AudioContextExc( "Could not connect PulseAudio output stream playback" );
		}

		// Wait for the stream to be ready.
		while( true ) {
			pa_stream_state_t state = pa_stream_get_state( mPaStream );
			if( ! PA_STREAM_IS_GOOD( state ) ) {
				throw AudioContextExc( "InputStream connect failed" );
			}

			if( PA_STREAM_READY == state ) {
				break;
			}

			pa_threaded_mainloop_wait( mContext->mPaMainLoop );
		}
	}
}

void InputStream::close()
{
	pulse::ScopedLock scopedLock( mContext->mPaMainLoop );

	pa_operation* op = pa_stream_flush( mPaStream, &Stream::successCallback, static_cast<void*>( this ) );
	pulse::waitForOperationCompletion( mContext->mPaMainLoop, op );

	pa_stream_disconnect( mPaStream );
	pa_stream_set_read_callback( mPaStream, nullptr, nullptr );
	pa_stream_set_state_callback( mPaStream, nullptr, nullptr );
	pa_stream_unref( mPaStream );
	mPaStream = nullptr;
}

void InputStream::start()
{
	pulse::ScopedLock scopedLock( mContext->mPaMainLoop );

	//mSourceFn = sourceFn;

	// Ensure the context and stream are ready.
	pa_context_state_t contextState = pa_context_get_state( mContext->mPaContext );
	pa_stream_state_t streamState = pa_stream_get_state( mPaStream );
	if( ( PA_CONTEXT_READY != contextState ) && ( PA_STREAM_READY != streamState ) ) {
		CI_LOG_E( "failed to get context state" );
		// TODO: improve errir reporting here
		return;
	}

	// Uncork (resume) the stream.
	pa_operation* op = pa_stream_cork( mPaStream, 0, &Stream::successCallback, this );
	pulse::waitForOperationCompletion( mContext->mPaMainLoop, op );
}

void InputStream::stop()
{
	pulse::ScopedLock scopedLock( mContext->mPaMainLoop );

	// Flush the stream prior to cork, doing so after will cause hangs. Write callbacks
	// are suspended while inside pa_threaded_mainloop_lock() so this is all thread safe.
	pa_operation* op = pa_stream_flush( mPaStream, &Stream::successCallback, static_cast<void*>( this ) );
	pulse::waitForOperationCompletion( mContext->mPaMainLoop, op );

	op = pa_stream_cork( mPaStream, 1, &Stream::successCallback, static_cast<void*>( this ) );
	pulse::waitForOperationCompletion( mContext->mPaMainLoop, op );
}

// static
void InputStream::readCallback( pa_stream *s, size_t nbytes, void *userData )
{
	InputStream *inputStream = (InputStream *)userData;
	inputStream->enqueueSamples( s, nbytes );
}

void InputStream::enqueueSamples( pa_stream *s, size_t nbytes )
{
	CI_ASSERT( nbytes % mBytesPerFrame == 0 );

	// TODO: use the result of pa_stream_readable_size here?
	// - can use it to check if there is enough room in the ringbuffer left to write to
	while( pa_stream_readable_size( s ) > 0 ) {
		const void *data;

		if( pa_stream_peek( s, &data, &nbytes ) < 0 ) {
			auto errorStr = pa_strerror( pa_context_errno( mContext->mPaContext ) );
			CI_LOG_E( "pa_stream_peek() failed: " << errorStr );
			CI_ASSERT_NOT_REACHABLE();
			return;
		}

		CI_ASSERT( nbytes > 0 );

		const size_t numFramesRead = nbytes / mBytesPerFrame;

		// resize mReadBuffer if necessary
		mReadBuffer.setSize( numFramesRead, mNumChannels );

		if( data ) {
			// de-interleave and write data to a ringbuffer that will be consumed in InpudeDeviceNode's process method
			ci::audio::dsp::deinterleave( (const float *)data, mReadBuffer.getData(), numFramesRead, mNumChannels, numFramesRead );
		}
		else {
			// there is a hole in the stream, generate silence
			mReadBuffer.zero();
		}

		size_t framesBuffered = 0;
		for( size_t ch = 0; ch < mRingBuffers.size(); ch++ ) {
			if( mRingBuffers[ch].write( mReadBuffer.getChannel( ch ), numFramesRead ) ) {
				// only set this to non-zero if we succeeded at writing to the ringbuffer
				framesBuffered = numFramesRead;
			}
			else {
				//CI_LOG_W( "RingBuffer full for channel: " << ch );
				if( mMarkOverrunFn )
					mMarkOverrunFn();
			}
		}

		mNumFramesBuffered += framesBuffered;
		pa_stream_drop( s );
	}

}

} // namespace pulse

namespace cinder { namespace audio { namespace linux {

// ----------------------------------------------------------------------------------------------------
// OutputDeviceNodePulseAudioImpl (private)
// ----------------------------------------------------------------------------------------------------
struct OutputDeviceNodePulseAudioImpl {

	OutputDeviceNodePulseAudioImpl( OutputDeviceNodePulseAudio* parent, const std::shared_ptr<ContextPulseAudio>& context )
		: mParent( parent ), mCinderContext( context ), mPulseContext( context->getPulseContext() )
	{
	}	

	void initPlayer( size_t numChannels, size_t sampleRate, size_t framesPerBlock )
	{
		mPulseStream = std::unique_ptr<pulse::OutputStream>( new pulse::OutputStream( mPulseContext, numChannels, sampleRate, framesPerBlock, mParent->getDevice()->getName() ) );
		mPulseStream->open();

		// Allocate a big enough buffer size that will accomodate most hardware. This is a workaround
		// since pa_stream_writable_size always seems to return 0.
		const size_t kBufferSizeBytes = 32768;
		size_t numFrames = ( kBufferSizeBytes / mPulseStream->mBytesPerSample ) + mPulseStream->mFramesPerBlock + 1;
		const size_t ringBufferSize = numFrames * mPulseStream->mNumChannels;		
		mRingBuffer.reset( new dsp::RingBuffer( ringBufferSize ) );		
	}

	void destroyPlayer()
	{
		mPulseStream.reset();
	}

	void play()
	{
		CI_ASSERT( mPulseStream );
		if( ! mPulseStream ) {
			return;
		}

		auto sourceFn = std::bind( &OutputDeviceNodePulseAudioImpl::enqueueSamples, this, std::placeholders::_1, std::placeholders::_2 );
		mPulseStream->start( sourceFn );
	}

	void stop()
	{
		CI_ASSERT( mPulseStream );
		if( ! mPulseStream ) {
			return;
		}

		mPulseStream->stop();
	}

	void enqueueSamples( size_t numSamplesToFill, void* buffer )
	{
		// Change the logic to use frames here instead of samples
		size_t numFramesToFill = numSamplesToFill / mPulseStream->mNumChannels;

		while( mNumFramesBuffered < numFramesToFill ) {
			mParent->renderInputs();
		}

		// Now change the logic back to samples
		bool readSuccess = mRingBuffer->read( (float*)buffer, numSamplesToFill );
		CI_ASSERT( readSuccess ); // since this is sync read / write, the read should always succeed.

		mNumFramesBuffered -= numFramesToFill;
	}

	OutputDeviceNodePulseAudio*    			mParent = nullptr;
	std::weak_ptr<ContextPulseAudio>		mCinderContext;

	pulse::Context*			                mPulseContext = nullptr;
	std::unique_ptr<pulse::OutputStream>	mPulseStream;

	std::unique_ptr<dsp::RingBuffer>		mRingBuffer;
	size_t									mNumFramesBuffered = 0;
};

// ----------------------------------------------------------------------------------------------------
// InputDeviceNodePulseAudioImpl (private)
// ----------------------------------------------------------------------------------------------------
struct InputDeviceNodePulseAudioImpl {

	InputDeviceNodePulseAudioImpl( InputDeviceNodePulseAudio* parent, const std::shared_ptr<ContextPulseAudio>& context )
			: mParent( parent ), mCinderContext( context ), mPulseContext( context->getPulseContext() )
	{
	}

	void initStream( size_t numChannels, size_t sampleRate, size_t framesPerBlock )
	{
		mPulseStream = std::unique_ptr<pulse::InputStream>( new pulse::InputStream( mPulseContext, numChannels, sampleRate, framesPerBlock, mParent->getDevice()->getName() ) );
		mPulseStream->open();
	}

	void destroyPlayer()
	{
		mPulseStream.reset();
	}

	void play()
	{
		CI_ASSERT( mPulseStream );
		if( ! mPulseStream ) {
			return;
		}

		mPulseStream->start();
	}

	void stop()
	{
		CI_ASSERT( mPulseStream );
		if( ! mPulseStream ) {
			return;
		}

		mPulseStream->stop();
	}

	InputDeviceNodePulseAudio*    			mParent = nullptr;
	std::weak_ptr<ContextPulseAudio>		mCinderContext;

	pulse::Context*			                mPulseContext = nullptr;
	std::unique_ptr<pulse::InputStream>	    mPulseStream;
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
	bool writeSuccess = mImpl->mRingBuffer->write( mInterleavedBuffer.getData(), mInterleavedBuffer.getSize() );
	CI_ASSERT( writeSuccess ); // Since this is sync read / write, the write should always succeed.

	mImpl->mNumFramesBuffered += mInterleavedBuffer.getNumFrames();

	ctx->postProcess();
}

// ----------------------------------------------------------------------------------------------------
// InputDeviceNodePulseAudio
// ----------------------------------------------------------------------------------------------------

InputDeviceNodePulseAudio::InputDeviceNodePulseAudio( const DeviceRef &device, const Format &format, const std::shared_ptr<ContextPulseAudio> &context )
	: InputDeviceNode( device, format ), mImpl( new InputDeviceNodePulseAudioImpl( this, context ) )
{

}

InputDeviceNodePulseAudio::~InputDeviceNodePulseAudio()
{
}

void InputDeviceNodePulseAudio::initialize()
{
	const size_t framesPerBlock = getFramesPerBlock();
	const size_t numChannels = getNumChannels();

	mImpl->initStream( numChannels, getSampleRate(), framesPerBlock );
	mImpl->mPulseStream->mMarkOverrunFn = [this] {
		auto ctx = getContext(); // don't report if the overrun happens when we're shutting down.
		if( ctx )
			markOverrun();
	};
}

void InputDeviceNodePulseAudio::uninitialize()
{
	mImpl->destroyPlayer();
}

void InputDeviceNodePulseAudio::enableProcessing()
{
	mImpl->play();
}

void InputDeviceNodePulseAudio::disableProcessing()
{
	mImpl->stop();
}

void InputDeviceNodePulseAudio::process( Buffer *buffer )
{
	const size_t framesNeeded = buffer->getNumFrames();
	if( mImpl->mPulseStream->mNumFramesBuffered < framesNeeded ) {
//		CI_LOG_W( "not enough frames buffered, need: " << framesNeeded << ", have: " << mImpl->mNumFramesBuffered );
		markUnderrun();
		return;
	}

	for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
		bool readSuccess = mImpl->mPulseStream->mRingBuffers[ch].read( buffer->getChannel( ch ), framesNeeded );
		CI_ASSERT( readSuccess );
	}

	mImpl->mPulseStream->mNumFramesBuffered -= framesNeeded;
}

// ----------------------------------------------------------------------------------------------------
// ContextPulseAudio
// ----------------------------------------------------------------------------------------------------

ContextPulseAudio::ContextPulseAudio()
{
	mPulseContext = pulse::Context::create();
}

ContextPulseAudio::~ContextPulseAudio()
{
	// uninit any device nodes before the pulse context is destroyed
	for( auto& deviceNode : mDeviceNodes ) {
		auto node = deviceNode.lock();
		if( node ) {
			node->disable();
			uninitializeNode( node );
		}
	}
}

OutputDeviceNodeRef	ContextPulseAudio::createOutputDeviceNode( const DeviceRef &device, const Node::Format &format )
{
	auto thisRef = std::dynamic_pointer_cast<ContextPulseAudio>( shared_from_this() );
	auto result = makeNode( new OutputDeviceNodePulseAudio( device, format, thisRef ) );
	mDeviceNodes.push_back( result );
	return result;
}

InputDeviceNodeRef ContextPulseAudio::createInputDeviceNode( const DeviceRef &device, const Node::Format &format  )
{
	auto thisRef = std::dynamic_pointer_cast<ContextPulseAudio>( shared_from_this() );
	auto result = makeNode( new InputDeviceNodePulseAudio( device, format, thisRef ) );
	mDeviceNodes.push_back( result );
	return result;
}

} } } // namespace cinder::audio::linux
