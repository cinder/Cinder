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

#include "cinder/audio/android/ContextOpenSl.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/dsp/RingBuffer.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

//#include <cinder/audio/android/DeviceManagerOpenSl.h>

#include "cinder/app/App.h"

using namespace std;

namespace cinder { namespace audio { namespace android {

// ----------------------------------------------------------------------------------------------------
// OutputDeviceNodeOpenSlImpl (private)
// ----------------------------------------------------------------------------------------------------

struct OutputDeviceNodeOpenSlImpl {
    OutputDeviceNodeOpenSlImpl( OutputDeviceNodeOpenSl *parent, const shared_ptr<ContextOpenSl> &context )
		    : mParent( parent ), mContextOpenSl( context )
    {
    }

	void initPlayer( size_t numChannels, size_t sampleRate, size_t framesPerBlock )
	{
		auto context = mContextOpenSl.lock();
		SLEngineItf engine = context->getSLEngineEngine();

		mNumSamplesBuffered = 0;
		mSamplesPerProcessingBlock = framesPerBlock * numChannels;

		// TODO: size our buffer to be twice as large as the Context's block size, then pass to opensl the exact number of samples it wants each callback
//		auto manager = dynamic_cast<DeviceManagerOpenSl *>( Context::deviceManager() );
//		mFramesPerBlockHardware = manager->getFramesPerBlockHardware( mParent->getDevice() );

		mSampleBuffer.resize( framesPerBlock * numChannels );

		SLuint32 channelMask = SL_SPEAKER_FRONT_CENTER;
		if( numChannels == 2 ) {
			channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
		}

		SLDataFormat_PCM sourceFormat = { 0 };
		sourceFormat.formatType     = SL_DATAFORMAT_PCM;
		sourceFormat.numChannels    = numChannels;
		sourceFormat.samplesPerSec  = sampleRate * 1000; // despite the variable name, the samplerate must be specified in milliHertz.
		sourceFormat.bitsPerSample  = SL_PCMSAMPLEFORMAT_FIXED_16;
		sourceFormat.containerSize  = 16,
		sourceFormat.channelMask    = channelMask;
		sourceFormat.endianness     = SL_BYTEORDER_LITTLEENDIAN;

		SLDataLocator_AndroidSimpleBufferQueue simpleBufferQueue = { 0 };
		simpleBufferQueue.locatorType   = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
		simpleBufferQueue.numBuffers    = 1;

		SLDataSource audioSource = { 0 };
		audioSource.pLocator = &simpleBufferQueue;
		audioSource.pFormat = &sourceFormat;

		SLresult result = (*engine )->CreateOutputMix( engine, &mOutputMixObject, 0, nullptr, nullptr );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		result = (*mOutputMixObject)->Realize( mOutputMixObject, SL_BOOLEAN_FALSE );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		SLDataLocator_OutputMix locatorOutputMix = { 0 };
		locatorOutputMix.locatorType = SL_DATALOCATOR_OUTPUTMIX;
		locatorOutputMix.outputMix = mOutputMixObject;

		SLDataSink audioSink = { 0 };
		audioSink.pLocator = &locatorOutputMix;
		audioSink.pFormat = nullptr; // parameter is ignored for SLDataLocator_OutputMix

		// create audio player
		const SLInterfaceID playerInterfaceIds[] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
		const SLboolean playerInterfacesRequired[] = { SL_BOOLEAN_TRUE };

		result = (*engine )->CreateAudioPlayer( engine, &mPlayerObject, &audioSource, &audioSink, 1, playerInterfaceIds, playerInterfacesRequired );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		result = (*mPlayerObject)->Realize( mPlayerObject, SL_BOOLEAN_FALSE );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		result = (*mPlayerObject)->GetInterface( mPlayerObject, SL_IID_PLAY, &mPlayerPlay );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		result = (*mPlayerObject)->GetInterface( mPlayerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &mPlayerBufferQueue );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		result = (*mPlayerBufferQueue)->RegisterCallback( mPlayerBufferQueue, &OutputDeviceNodeOpenSlImpl::playerCallback, this );
		CI_VERIFY( result == SL_RESULT_SUCCESS );
	}

	void destroyPlayer()
	{
		if( mPlayerObject ) {
			(*mPlayerObject)->Destroy( mPlayerObject );
			mPlayerObject = nullptr;
		}
		if( mOutputMixObject ) {
			(*mOutputMixObject)->Destroy( mOutputMixObject );
			mOutputMixObject = nullptr;
		}
	}

	void play()
	{
		SLresult result = (*mPlayerPlay)->SetPlayState( mPlayerPlay, SL_PLAYSTATE_PLAYING );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		// need to fill the BufferQueue initially with samples, playerCallback() will be called to do this when needed afterwards
		enqueueSamples( mPlayerBufferQueue );
	}

	void stop()
	{
		SLresult result = (*mPlayerPlay)->SetPlayState( mPlayerPlay, SL_PLAYSTATE_STOPPED );
		CI_VERIFY( result == SL_RESULT_SUCCESS );
	}


//	int mNumCallbackCalls = 0;
//	string mDebugMessage;

	void enqueueSamples( SLAndroidSimpleBufferQueueItf bufferQueue )
	{
//		if( mNumCallbackCalls < 50 ) {
//			mNumCallbackCalls++;
//			auto numProcessedFrames = master()->getNumProcessedFrames();
//			auto currTime = app::getElapsedSeconds();
//			mDebugMessage += "[" + to_string( numProcessedFrames ) + "] " + to_string( currTime ) + ", ";
//		}
//		else if( mNumCallbackCalls == 50 ) {
//			mNumCallbackCalls++;
//			CI_LOG_I( mDebugMessage );
//		}

		while( mSampleBuffer.size() - mNumSamplesBuffered >= mSamplesPerProcessingBlock ) {
			mParent->renderToBufferFromInputs();
		}

		SLresult result = (*bufferQueue)->Enqueue( bufferQueue, mSampleBuffer.data(), mNumSamplesBuffered * sizeof( int16_t ) );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		mNumSamplesBuffered = 0;
	}

	static void playerCallback( SLAndroidSimpleBufferQueueItf bufferQueue, void *context )
	{
		OutputDeviceNodeOpenSlImpl *impl = (OutputDeviceNodeOpenSlImpl *)context;
		impl->enqueueSamples( bufferQueue );
	}

	OutputDeviceNodeOpenSl*    mParent = nullptr;
	weak_ptr<ContextOpenSl>    mContextOpenSl;


	SLObjectItf mOutputMixObject = nullptr;
	SLObjectItf mPlayerObject = nullptr;
	SLPlayItf   mPlayerPlay = nullptr;
	SLAndroidSimpleBufferQueueItf mPlayerBufferQueue = nullptr;

	std::vector<int16_t>    mSampleBuffer;
	size_t	                mNumSamplesBuffered = 0;
	size_t                  mSamplesPerProcessingBlock = 0;
};

// ----------------------------------------------------------------------------------------------------
// OutputDeviceNodeOpenSl
// ----------------------------------------------------------------------------------------------------

OutputDeviceNodeOpenSl::OutputDeviceNodeOpenSl( const DeviceRef &device, const Node::Format &format, const shared_ptr<ContextOpenSl> &context )
	: OutputDeviceNode( device, format ), mImpl( new OutputDeviceNodeOpenSlImpl( this, context ) )
{
}

void OutputDeviceNodeOpenSl::initialize()
{
	const size_t sampleRate = getOutputSampleRate();
	const size_t framesPerBlock = getOutputFramesPerBlock();
	const size_t numChannels = getNumChannels();

	if( numChannels > 2 ) {
		throw AudioFormatExc( string( "Device can not accommodate " ) + to_string( numChannels ) + " output channels, only one or two are supported." );
	}

	mImpl->initPlayer( numChannels, sampleRate, framesPerBlock );
}

void OutputDeviceNodeOpenSl::uninitialize()
{
	mImpl->destroyPlayer();
}

void OutputDeviceNodeOpenSl::enableProcessing()
{
	mImpl->play();
}

void OutputDeviceNodeOpenSl::disableProcessing()
{
	mImpl->stop();
}

void OutputDeviceNodeOpenSl::renderToBufferFromInputs()
{
//	CI_LOG_I( "begin" );
	auto ctx = getContext();
	if( ! ctx )
		return;

	lock_guard<mutex> lock( ctx->getMutex() );

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

	// copy samples to opensl output buffer.
	int16_t *destInt16Samples = &mImpl->mSampleBuffer[mImpl->mNumSamplesBuffered];
	dsp::interleave( internalBuffer->getData(), destInt16Samples, internalBuffer->getNumFrames(), internalBuffer->getNumChannels(), internalBuffer->getNumFrames() );


	mImpl->mNumSamplesBuffered += internalBuffer->getSize();

	ctx->postProcess();
}

// ----------------------------------------------------------------------------------------------------
// InputDeviceNodeOpenSlImpl (private)
// ----------------------------------------------------------------------------------------------------

const size_t MIN_RINGBUFFER_FRAMES = 4096;

struct InputDeviceNodeOpenSlImpl {
	InputDeviceNodeOpenSlImpl( InputDeviceNodeOpenSl *parent, const shared_ptr<ContextOpenSl> &context )
		: mParent( parent ), mContextOpenSl( context )
	{
	}

	void initRecorder( size_t numChannels, size_t sampleRate, size_t framesPerBlock )
	{
		mNumFramesEnqueued = 0;
		mNumFramesBuffered = 0;
		mEnqueueBuffer.resize( framesPerBlock * numChannels );
		mConvertBuffer.setSize( framesPerBlock, numChannels );
		mRingBuffers.resize( numChannels );

		size_t numRingBufferFrames = max( framesPerBlock * 2, MIN_RINGBUFFER_FRAMES );
		CI_LOG_I( "framesPerBlock: " << framesPerBlock << ", numChannels: " << numChannels << ", numRingBufferFrames: " << numRingBufferFrames );

		for( auto &ringBuffer : mRingBuffers ) {
			ringBuffer.resize( numRingBufferFrames );
		}

		auto context = mContextOpenSl.lock();
		SLEngineItf engine = context->getSLEngineEngine();

		SLDataLocator_IODevice locatorIoDevice = { 0 };
		locatorIoDevice.locatorType  = SL_DATALOCATOR_IODEVICE;
		locatorIoDevice.deviceType   = SL_IODEVICE_AUDIOINPUT;
		locatorIoDevice.deviceID     = SL_DEFAULTDEVICEID_AUDIOINPUT;
		locatorIoDevice.device       = nullptr;

		SLDataSource audioSource = { 0 };
		audioSource.pLocator = &locatorIoDevice;
		audioSource.pFormat = nullptr;

		SLuint32 channelMask = SL_SPEAKER_FRONT_CENTER;
		if( numChannels == 2 ) {
			channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
		}

		SLDataFormat_PCM sinkFormat = { 0 };
		sinkFormat.formatType     = SL_DATAFORMAT_PCM;
		sinkFormat.numChannels    = numChannels;
		sinkFormat.samplesPerSec  = sampleRate * 1000; // despite the variable name, the samplerate must be specified in milliHertz.
		sinkFormat.bitsPerSample  = SL_PCMSAMPLEFORMAT_FIXED_16;
		sinkFormat.containerSize  = 16,
		sinkFormat.channelMask    = channelMask;
		sinkFormat.endianness     = SL_BYTEORDER_LITTLEENDIAN;

		SLDataLocator_AndroidSimpleBufferQueue simpleBufferQueue = { 0 };
		simpleBufferQueue.locatorType   = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
		simpleBufferQueue.numBuffers    = 1;

		SLDataSink audioSink = { 0 };
		audioSink.pLocator = &simpleBufferQueue;
		audioSink.pFormat = &sinkFormat;

		const SLInterfaceID recorderInterfaceIds[1] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
		const SLboolean recorderInterfacesRequired[1] = { SL_BOOLEAN_TRUE };

		SLresult result = (*engine)->CreateAudioRecorder( engine, &mRecorderObject, &audioSource, &audioSink, 1, recorderInterfaceIds, recorderInterfacesRequired );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		// realize the audio recorder
		result = (*mRecorderObject)->Realize( mRecorderObject, SL_BOOLEAN_FALSE );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		// get the record interface
		result = (*mRecorderObject)->GetInterface( mRecorderObject, SL_IID_RECORD, &mRecorderRecord );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		// get the buffer queue interface
		result = (*mRecorderObject)->GetInterface( mRecorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &mBufferQueue );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		// register callback on the buffer queue
		result = (*mBufferQueue)->RegisterCallback( mBufferQueue, &InputDeviceNodeOpenSlImpl::recorderCallback, this );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		CI_LOG_I( "complete" );
	}

	void destroyRecorder()
	{
		if( mRecorderObject ) {
			(*mRecorderObject)->Destroy( mRecorderObject );
			mRecorderObject = nullptr;
		}
	}

	void beginRecording()
	{
		SLresult result = (*mRecorderRecord)->SetRecordState( mRecorderRecord, SL_RECORDSTATE_RECORDING );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		enqueueSamples( mBufferQueue );
	}

	void stopRecording()
	{
		SLresult result = (*mRecorderRecord)->SetRecordState( mRecorderRecord, SL_RECORDSTATE_STOPPED );
		CI_VERIFY( result == SL_RESULT_SUCCESS );
	}

	// TODO: On the device I tested, it seems like ringbuffers always have about 2000 frames in them,
	// it might be possible to reduce latency by throwing some of these away (overwriting in this method)
	void enqueueSamples( SLAndroidSimpleBufferQueueItf bufferQueue )
	{
		// convert the enqueued samples into de-interleaved floating point, push those into the ringbuffer
//		size_t currentBlock = master()->getNumProcessedFrames() / master()->getFramesPerBlock();
//		CI_LOG_I( "currentBlock: " << currentBlock << " - " << mDebugString );

		dsp::deinterleave( mEnqueueBuffer.data(), mConvertBuffer.getData(), mConvertBuffer.getNumFrames(), mConvertBuffer.getNumChannels(), mNumFramesEnqueued );

		size_t framesBuffered = 0;
		for( size_t ch = 0; ch < mRingBuffers.size(); ch++ ) {
			if( mRingBuffers[ch].write( mConvertBuffer.getChannel( ch ), mNumFramesEnqueued ) ) {
				// only set this to non-zero if we succeeded at writing to the ringbuffer
				framesBuffered = mNumFramesEnqueued;
			}
			else {
//				CI_LOG_W( "RingBuffer full for channel: " << ch );
				mParent->markOverrun();
			}
		}

		mNumFramesBuffered += framesBuffered;
//		CI_LOG_I( "frames buffered: " << mNumFramesBuffered );

		SLresult result = (*bufferQueue)->Enqueue( bufferQueue, mEnqueueBuffer.data(), mEnqueueBuffer.size() * sizeof( int16_t ) );
		CI_VERIFY( result == SL_RESULT_SUCCESS );

		mNumFramesEnqueued = mEnqueueBuffer.size();
	}

	static void recorderCallback( SLAndroidSimpleBufferQueueItf bufferQueue, void *context )
	{
		InputDeviceNodeOpenSlImpl *impl = (InputDeviceNodeOpenSlImpl *)context;
		impl->enqueueSamples( bufferQueue );
	}


	InputDeviceNodeOpenSl*          mParent = nullptr;
	std::weak_ptr<ContextOpenSl>    mContextOpenSl;

	SLObjectItf mRecorderObject = nullptr;
	SLRecordItf mRecorderRecord = nullptr;
	SLAndroidSimpleBufferQueueItf mBufferQueue = nullptr;

	std::vector<int16_t>            mEnqueueBuffer;
	audio::BufferDynamic            mConvertBuffer;
	std::vector<dsp::RingBuffer>    mRingBuffers;

	size_t	                mNumFramesEnqueued = 0; // amount requested from OpenSL
	size_t	                mNumFramesBuffered = 0; // amount of floating-point frames available to be read in the ringbuffer
};

// ----------------------------------------------------------------------------------------------------
// InputDeviceNodeOpenSl
// ----------------------------------------------------------------------------------------------------

InputDeviceNodeOpenSl::InputDeviceNodeOpenSl( const DeviceRef &device, const Node::Format &format, const shared_ptr<ContextOpenSl> &context )
	: InputDeviceNode( device, format ), mImpl( new InputDeviceNodeOpenSlImpl( this, context ) )
{
}

InputDeviceNodeOpenSl::~InputDeviceNodeOpenSl()
{
}

void InputDeviceNodeOpenSl::initialize()
{
	const size_t sampleRate = getSampleRate();
	const size_t framesPerBlock = getFramesPerBlock();
	const size_t numChannels = getNumChannels();

	if( numChannels > 2 ) {
		throw AudioFormatExc( string( "Device can not accommodate " ) + to_string( numChannels ) + " output channels, only one or two are supported." );
	}

	mImpl->initRecorder( numChannels, sampleRate, framesPerBlock );
}

void InputDeviceNodeOpenSl::uninitialize()
{
	mImpl->destroyRecorder();
}

void InputDeviceNodeOpenSl::enableProcessing()
{
	mImpl->beginRecording();
}

void InputDeviceNodeOpenSl::disableProcessing()
{
	mImpl->stopRecording();
}

void InputDeviceNodeOpenSl::process( Buffer *buffer )
{
//	size_t currentBlock = master()->getNumProcessedFrames() / master()->getFramesPerBlock();
//	CI_LOG_I( "currentBlock: " << currentBlock );

	const size_t framesNeeded = buffer->getNumFrames();
	if( mImpl->mNumFramesBuffered < framesNeeded ) {
//		CI_LOG_W( "not enough frames buffered, need: " << framesNeeded << ", have: " << mImpl->mNumFramesBuffered );
		markUnderrun();
		return;
	}

	for( size_t ch = 0; ch < getNumChannels(); ch++ ) {
		bool readSuccess = mImpl->mRingBuffers[ch].read( buffer->getChannel( ch ), framesNeeded );
		CI_ASSERT( readSuccess );
	}

	mImpl->mNumFramesBuffered -= framesNeeded;
}

// ----------------------------------------------------------------------------------------------------
// ContextOpenSl
// ----------------------------------------------------------------------------------------------------

ContextOpenSl::ContextOpenSl()
{
	SLresult result = slCreateEngine( &mSLEngineObject, 0, NULL, 0, NULL, NULL );
	CI_VERIFY( result == SL_RESULT_SUCCESS );

	SLboolean realizeAsync = SL_BOOLEAN_FALSE;
	result = (*mSLEngineObject)->Realize( mSLEngineObject, realizeAsync );
	CI_VERIFY( result == SL_RESULT_SUCCESS );

	result = (*mSLEngineObject)->GetInterface( mSLEngineObject, SL_IID_ENGINE, &mSLEngineEngine );
	CI_VERIFY( result == SL_RESULT_SUCCESS );

	CI_LOG_I( "complete" );
}

ContextOpenSl::~ContextOpenSl()
{
	if( mSLEngineObject ) {
		(*mSLEngineObject)->Destroy( mSLEngineObject );
	}

	mSLEngineObject = nullptr;
	mSLEngineEngine = nullptr;
}

OutputDeviceNodeRef ContextOpenSl::createOutputDeviceNode( const DeviceRef &device, const Node::Format &format )
{
	auto thisRef = dynamic_pointer_cast<ContextOpenSl>( shared_from_this() );
	return makeNode( new OutputDeviceNodeOpenSl( device, format, thisRef ) );
}

InputDeviceNodeRef ContextOpenSl::createInputDeviceNode( const DeviceRef &device, const Node::Format &format )
{
	auto thisRef = dynamic_pointer_cast<ContextOpenSl>( shared_from_this() );
	return makeNode( new InputDeviceNodeOpenSl( device, format, thisRef ) );
}

} } } // namespace cinder::audio::android
