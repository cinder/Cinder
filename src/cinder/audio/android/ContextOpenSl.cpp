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

//#include "cinder/app/App.h"

using namespace std;

namespace cinder { namespace audio { namespace android {

struct OutputDeviceNodeOpenSlImpl {
    OutputDeviceNodeOpenSlImpl( OutputDeviceNodeOpenSl *parent )
		    : mParent( parent )
    {
		initEngine();
    }

	~OutputDeviceNodeOpenSlImpl()
	{
		destroyEngine();
	}

    void initEngine()
    {
	    SLresult result = slCreateEngine( &mSLObject, 0, NULL, 0, NULL, NULL );
	    CI_VERIFY( result == SL_RESULT_SUCCESS );

	    SLboolean realizeAsync = SL_BOOLEAN_FALSE;
	    result = (*mSLObject)->Realize( mSLObject, realizeAsync );
	    CI_VERIFY( result == SL_RESULT_SUCCESS );

	    result = (*mSLObject)->GetInterface( mSLObject, SL_IID_ENGINE, &mSLEngine );
	    CI_VERIFY( result == SL_RESULT_SUCCESS );

	    CI_LOG_I( "complete" );
    }

	void destroyEngine()
	{
		if( mSLObject ) {
			(*mSLObject)->Destroy( mSLObject );
			mSLEngine = nullptr;
		}
	}

	void initPlayer( size_t numChannels, size_t sampleRate, size_t framesPerBlock )
	{
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

		SLresult result = (*mSLEngine)->CreateOutputMix( mSLEngine, &mOutputMixObject, 0, nullptr, nullptr );
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
		const SLInterfaceID playIds[] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
		const SLboolean playRec[] = { SL_BOOLEAN_TRUE };

		result = (*mSLEngine)->CreateAudioPlayer( mSLEngine, &mPlayerObject, &audioSource, &audioSink, 1, playIds, playRec );
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

		// TODO: check if all sl objects for player have been destroyed
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

	OutputDeviceNodeOpenSl* mParent = nullptr;

	SLObjectItf mSLObject = nullptr;
	SLEngineItf mSLEngine = nullptr;
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

OutputDeviceNodeOpenSl::OutputDeviceNodeOpenSl( const DeviceRef &device, const Node::Format &format )
	: OutputDeviceNode( device, format ), mImpl( new OutputDeviceNodeOpenSlImpl( this ) )
{
}

void OutputDeviceNodeOpenSl::initialize()
{
	size_t sampleRate = getOutputSampleRate();
	size_t framesPerBlock = getOutputFramesPerBlock();

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
// InputDeviceNodeOpenSl
// ----------------------------------------------------------------------------------------------------

#if 0
InputDeviceNodeOpenSl::InputDeviceNodeOpenSl( const DeviceRef &device, const Node::Format &format )
	: InputDeviceNode( device, format )
{
}

InputDeviceNodeOpenSl::~InputDeviceNodeOpenSl()
{
}

void InputDeviceNodeOpenSl::initialize()
{
}

void InputDeviceNodeOpenSl::uninitialize()
{
}

void InputDeviceNodeOpenSl::enableProcessing()
{
}

void InputDeviceNodeOpenSl::disableProcessing()
{
}

void InputDeviceNodeOpenSl::process( Buffer *buffer )
{
}
#endif

// ----------------------------------------------------------------------------------------------------
// ContextOpenSl
// ----------------------------------------------------------------------------------------------------

OutputDeviceNodeRef ContextOpenSl::createOutputDeviceNode( const DeviceRef &device, const Node::Format &format )
{
	return makeNode( new OutputDeviceNodeOpenSl( device, format ) );
}

InputDeviceNodeRef ContextOpenSl::createInputDeviceNode( const DeviceRef &device, const Node::Format &format )
{
	CI_ASSERT( 0 && "not yet implemented" );

//	return makeNode( new InputDeviceNodeOpenSl( device, format ) );
	return nullptr;
}

} } } // namespace cinder::audio::android
