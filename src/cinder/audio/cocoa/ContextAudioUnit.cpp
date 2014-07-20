/*
 Copyright (c) 2014, The Cinder Project

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

#include "cinder/audio/cocoa/ContextAudioUnit.h"
#include "cinder/audio/cocoa/CinderCoreAudio.h"
#include "cinder/CinderAssert.h"
#include "cinder/audio/Debug.h"

#include "cinder/Utilities.h"

#if defined( CINDER_MAC )
	#include "cinder/audio/cocoa/DeviceManagerCoreAudio.h"
#else
	#include "cinder/audio/cocoa/DeviceManagerAudioSession.h"
#endif

using namespace std;

namespace cinder { namespace audio { namespace cocoa {

enum DeviceBus { OUTPUT = 0, INPUT = 1 };

// ----------------------------------------------------------------------------------------------------
// MARK: - NodeAudioUnit
// ----------------------------------------------------------------------------------------------------

NodeAudioUnit::~NodeAudioUnit()
{
	if( mAudioUnit && mOwnsAudioUnit ) {
		OSStatus status = ::AudioComponentInstanceDispose( mAudioUnit );
		CI_VERIFY( status == noErr );
	}
}

void NodeAudioUnit::initAu()
{
	OSStatus status = ::AudioUnitInitialize( mAudioUnit );
	CI_VERIFY( status == noErr );
}

void NodeAudioUnit::uninitAu()
{
	OSStatus status = ::AudioUnitUninitialize( mAudioUnit );
	CI_VERIFY( status == noErr );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - OutputDeviceNodeAudioUnit
// ----------------------------------------------------------------------------------------------------

OutputDeviceNodeAudioUnit::OutputDeviceNodeAudioUnit( const DeviceRef &device, const Format &format )
	: OutputDeviceNode( device, format ), mSynchronousIO( false )
{
	findAndCreateAudioComponent( getOutputAudioUnitDesc(), &mAudioUnit );
}

void OutputDeviceNodeAudioUnit::initialize()
{
	mRenderData.node = this;
	mRenderData.context = dynamic_cast<ContextAudioUnit *>( getContext().get() );

	::AudioStreamBasicDescription asbd = createFloatAsbd( getOutputSampleRate(), getNumChannels() );
	setAudioUnitProperty( mAudioUnit, kAudioUnitProperty_StreamFormat, asbd, kAudioUnitScope_Input, DeviceBus::OUTPUT );

	UInt32 enableOutput = 1;
	setAudioUnitProperty( mAudioUnit, kAudioOutputUnitProperty_EnableIO, enableOutput, kAudioUnitScope_Output, DeviceBus::OUTPUT );

	UInt32 enableInput = mSynchronousIO;
	setAudioUnitProperty( mAudioUnit, kAudioOutputUnitProperty_EnableIO, enableInput, kAudioUnitScope_Input, DeviceBus::INPUT );

	if( enableInput )
		setAudioUnitProperty( mAudioUnit, kAudioUnitProperty_StreamFormat, asbd, kAudioUnitScope_Output, DeviceBus::INPUT );


	::AURenderCallbackStruct callbackStruct { OutputDeviceNodeAudioUnit::renderCallback, &mRenderData };
	setAudioUnitProperty( mAudioUnit, kAudioUnitProperty_SetRenderCallback, callbackStruct, kAudioUnitScope_Input, DeviceBus::OUTPUT );

#if defined( CINDER_MAC )
	auto manager = dynamic_cast<DeviceManagerCoreAudio *>( Context::deviceManager() );
	CI_ASSERT( manager );

	manager->setCurrentOutputDevice( mDevice, mAudioUnit );
#endif

	initAu();
}

void OutputDeviceNodeAudioUnit::uninitialize()
{
	uninitAu();
}

void OutputDeviceNodeAudioUnit::enableProcessing()
{
	OSStatus status = ::AudioOutputUnitStart( mAudioUnit );
	CI_VERIFY( status == noErr );
}

void OutputDeviceNodeAudioUnit::disableProcessing()
{
	OSStatus status = ::AudioOutputUnitStop( mAudioUnit );
	CI_VERIFY( status == noErr );
}

OSStatus OutputDeviceNodeAudioUnit::renderCallback( void *data, ::AudioUnitRenderActionFlags *flags, const ::AudioTimeStamp *timeStamp, UInt32 busNumber, UInt32 numFrames, ::AudioBufferList *bufferList )
{
	RenderData *renderData = static_cast<NodeAudioUnit::RenderData *>( data );

	auto ctx = renderData->node->getContext();
	if( ! ctx ) {
		zeroBufferList( bufferList );
		return noErr;
	}

	lock_guard<mutex> lock( ctx->getMutex() );

	// verify associated context still exists, which may not be true if we blocked in ~Context() and were then deallocated.
	ctx = renderData->node->getContext();
	if( ! ctx ) {
		zeroBufferList( bufferList );
		return noErr;
	}

	OutputDeviceNodeAudioUnit *lineOut = static_cast<OutputDeviceNodeAudioUnit *>( renderData->node );
	Buffer *internalBuffer = lineOut->getInternalBuffer();
	internalBuffer->zero();

	renderData->context->setCurrentTimeStamp( timeStamp );
	lineOut->pullInputs( internalBuffer );

	// if clip detection is enabled and buffer clipped, silence it
	if( lineOut->checkNotClipping() )
		zeroBufferList( bufferList );
	else
		copyToBufferList( bufferList, internalBuffer );

	ctx->postProcess();

	return noErr;
}

// ----------------------------------------------------------------------------------------------------
// MARK: - InputDeviceNodeAudioUnit
// ----------------------------------------------------------------------------------------------------

InputDeviceNodeAudioUnit::InputDeviceNodeAudioUnit( const DeviceRef &device, const Format &format )
	: InputDeviceNode( device, format ), mSynchronousIO( false ), mRingBufferPaddingFactor( 2 )
{
}

InputDeviceNodeAudioUnit::~InputDeviceNodeAudioUnit()
{
}

void InputDeviceNodeAudioUnit::initialize()
{
	mRenderData.node = this;
	mRenderData.context = dynamic_cast<ContextAudioUnit *>( getContext().get() );
	auto device = getDevice();

	// see if synchronous I/O is possible by looking at the OutputDeviceNode
	auto lineOutAu = dynamic_pointer_cast<OutputDeviceNodeAudioUnit>( getContext()->getOutput() );
	CI_ASSERT( lineOutAu );

	if( device == lineOutAu->getDevice() && getNumChannels() == lineOutAu->getNumChannels() ) {
		mSynchronousIO = true;
		mAudioUnit = lineOutAu->getAudioUnit();
		mOwnsAudioUnit = false;
	}
	else {
		// make our own AudioUnit, if we don't already have one (from a previous initialize())
		findAndCreateAudioComponent( getOutputAudioUnitDesc(), &mAudioUnit );

		mSynchronousIO = false;
		mOwnsAudioUnit = true;
	}

	size_t framesPerBlock = lineOutAu->getOutputFramesPerBlock();
	size_t sampleRate = lineOutAu->getOutputSampleRate();
	::AudioStreamBasicDescription asbd = createFloatAsbd( sampleRate, getNumChannels() );

	if( mSynchronousIO ) {
		// OutputDeviceNodeAudioUnit is expected to initialize the AudioUnit, since it is pulling to here. But make sure input is enabled.
		// TODO: this path can surely be optimized to not require line out being initialized twice
		lineOutAu->mSynchronousIO = true;
		bool lineOutWasInitialized = lineOutAu->isInitialized();
		bool lineOutWasEnabled = lineOutAu->isEnabled();
		if( lineOutWasInitialized ) {
			lineOutAu->disable();
			lineOutAu->uninitialize();
		}

		mBufferList = createNonInterleavedBufferList( framesPerBlock, getNumChannels() );

		if( lineOutWasInitialized )
			lineOutAu->initialize();

		if( lineOutWasEnabled )
			lineOutAu->setEnabled();
	}
	else {
		if( device->getSampleRate() != sampleRate || device->getFramesPerBlock() != framesPerBlock )
			device->updateFormat( Device::Format().sampleRate( sampleRate ).framesPerBlock( framesPerBlock ) );

		mRingBuffer.resize( framesPerBlock * getNumChannels() * mRingBufferPaddingFactor );
		mBufferList = createNonInterleavedBufferList( framesPerBlock, getNumChannels() );

		::AURenderCallbackStruct callbackStruct = { InputDeviceNodeAudioUnit::inputCallback, &mRenderData };
		setAudioUnitProperty( mAudioUnit, kAudioOutputUnitProperty_SetInputCallback, callbackStruct, kAudioUnitScope_Global, DeviceBus::INPUT );

		UInt32 enableInput = 1;
		setAudioUnitProperty( mAudioUnit, kAudioOutputUnitProperty_EnableIO, enableInput, kAudioUnitScope_Input, DeviceBus::INPUT );

		UInt32 enableOutput = 0;
		setAudioUnitProperty( mAudioUnit, kAudioOutputUnitProperty_EnableIO, enableOutput, kAudioUnitScope_Output, DeviceBus::OUTPUT );

		setAudioUnitProperty( mAudioUnit, kAudioUnitProperty_StreamFormat, asbd, kAudioUnitScope_Output, DeviceBus::INPUT );

#if defined( CINDER_MAC )
		auto manager = dynamic_cast<DeviceManagerCoreAudio *>( Context::deviceManager() );
		CI_ASSERT( manager );

		manager->setCurrentInputDevice( device, mAudioUnit );
#endif
		initAu();
	}
}

// TODO: what about when synchronous IO and this guy is requested to uninit, does associated OutputDeviceNodeAudioUnit need to be uninitialized too?
// - line out should notify line in we're going out
void InputDeviceNodeAudioUnit::uninitialize()
{
	if( ! mSynchronousIO ) {
		uninitAu();
	}
}

void InputDeviceNodeAudioUnit::enableProcessing()
{
	if( ! mSynchronousIO ) {
		OSStatus status = ::AudioOutputUnitStart( mAudioUnit );
		CI_VERIFY( status == noErr );
	}
}

void InputDeviceNodeAudioUnit::disableProcessing()
{
	if( ! mSynchronousIO ) {
		OSStatus status = ::AudioOutputUnitStop( mAudioUnit );
		CI_VERIFY( status == noErr );
	}
}

void InputDeviceNodeAudioUnit::process( Buffer *buffer )
{
	if( mSynchronousIO ) {
		mProcessBuffer = buffer;
		::AudioUnitRenderActionFlags flags = 0;
		const ::AudioTimeStamp *timeStamp = mRenderData.context->getCurrentTimeStamp();
		OSStatus status = ::AudioUnitRender( mAudioUnit, &flags, timeStamp, DeviceBus::INPUT, (UInt32)buffer->getNumFrames(), mBufferList.get() );
		CI_VERIFY( status == noErr );

		copyFromBufferList( buffer, mBufferList.get() );
	}
	else {
		// copy from ringbuffer. If not possible, store the timestamp of the underrun
		if( ! mRingBuffer.read( buffer->getData(), buffer->getSize() ) )
			markUnderrun();
	}
}

// note: Not all AudioUnitRender status errors are fatal here. For instance, if samplerate just changed we may not be able to pull input just yet, but we will next frame.
OSStatus InputDeviceNodeAudioUnit::inputCallback( void *data, ::AudioUnitRenderActionFlags *flags, const ::AudioTimeStamp *timeStamp, UInt32 bus, UInt32 numFrames, ::AudioBufferList *bufferList )
{
	RenderData *renderData = static_cast<NodeAudioUnit::RenderData *>( data );
	InputDeviceNodeAudioUnit *lineIn = static_cast<InputDeviceNodeAudioUnit *>( renderData->node );

	// this method is called async by Core Audio, so first check that our Context hasn't already been destroyed.
	if( ! lineIn->getContext() )
		return noErr;

	::AudioBufferList *nodeBufferList = lineIn->mBufferList.get();
	OSStatus status = ::AudioUnitRender( lineIn->getAudioUnit(), flags, timeStamp, DeviceBus::INPUT, numFrames, nodeBufferList );
	if( status != noErr )
		return status;

	if( lineIn->mRingBuffer.getAvailableWrite() >= nodeBufferList->mNumberBuffers * numFrames ) {
		for( size_t ch = 0; ch < nodeBufferList->mNumberBuffers; ch++ ) {
			float *channel = static_cast<float *>( nodeBufferList->mBuffers[ch].mData );
			lineIn->mRingBuffer.write( channel, numFrames );
		}
	}
	else
		lineIn->markOverrun();

	return noErr;
}

// ----------------------------------------------------------------------------------------------------
// MARK: - EffectAudioUnitNode
// ----------------------------------------------------------------------------------------------------

EffectAudioUnitNode::EffectAudioUnitNode( UInt32 effectSubType, const Format &format )
	: Node( format ), mEffectSubType( effectSubType )
{
}

EffectAudioUnitNode::~EffectAudioUnitNode()
{
}

void EffectAudioUnitNode::initialize()
{
	mRenderData.node = this;
	mRenderData.context = dynamic_cast<ContextAudioUnit *>( getContext().get() );

	::AudioComponentDescription comp{ 0 };
	comp.componentType = kAudioUnitType_Effect;
	comp.componentSubType = mEffectSubType;
	comp.componentManufacturer = kAudioUnitManufacturer_Apple;
	findAndCreateAudioComponent( comp, &mAudioUnit );

	mBufferList = createNonInterleavedBufferList( getFramesPerBlock(), getNumChannels() );

	::AudioStreamBasicDescription asbd = createFloatAsbd( getSampleRate(), getNumChannels() );
	setAudioUnitProperty( mAudioUnit, kAudioUnitProperty_StreamFormat, asbd, kAudioUnitScope_Input, 0 );
	setAudioUnitProperty( mAudioUnit, kAudioUnitProperty_StreamFormat, asbd, kAudioUnitScope_Output, 0 );

	::AURenderCallbackStruct callbackStruct = { EffectAudioUnitNode::renderCallback, &mRenderData };
	setAudioUnitProperty( mAudioUnit, kAudioUnitProperty_SetRenderCallback, callbackStruct, kAudioUnitScope_Input, 0 );

	initAu();
}

void EffectAudioUnitNode::uninitialize()
{
	uninitAu();
}

// TODO: try pointing buffer list at processBuffer instead of copying
void EffectAudioUnitNode::process( Buffer *buffer )
{
	mProcessBuffer = buffer;

	::AudioUnitRenderActionFlags flags = 0;
	const ::AudioTimeStamp *timeStamp = mRenderData.context->getCurrentTimeStamp();
	OSStatus status = ::AudioUnitRender( mAudioUnit, &flags, timeStamp, 0, (UInt32)buffer->getNumFrames(), mBufferList.get() );
	CI_VERIFY( status == noErr );

	copyFromBufferList( buffer, mBufferList.get() );
}

OSStatus EffectAudioUnitNode::renderCallback( void *data, ::AudioUnitRenderActionFlags *flags, const ::AudioTimeStamp *timeStamp, UInt32 busNumber, UInt32 numFrames, ::AudioBufferList *bufferList )
{
	RenderData *renderData = static_cast<NodeAudioUnit::RenderData *>( data );
	EffectAudioUnitNode *effectNode = static_cast<EffectAudioUnitNode *>( renderData->node );

	copyToBufferList( bufferList, effectNode->mProcessBuffer );
	return noErr;
}

void EffectAudioUnitNode::setParameter( ::AudioUnitParameterID paramId, float val )
{
	setAudioUnitParam( mAudioUnit, paramId, val, kAudioUnitScope_Global, 0 );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - ContextAudioUnit
// ----------------------------------------------------------------------------------------------------

OutputDeviceNodeRef ContextAudioUnit::createOutputDeviceNode( const DeviceRef &device, const OutputNode::Format &format )
{
	return makeNode( new OutputDeviceNodeAudioUnit( device, format ) );
}

InputDeviceNodeRef ContextAudioUnit::createInputDeviceNode( const DeviceRef &device, const Node::Format &format )
{
	return makeNode( new InputDeviceNodeAudioUnit( device, format ) );
}

ContextAudioUnit::~ContextAudioUnit()
{
}

} } } // namespace cinder::audio::cocoa
