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

#pragma once

#include "cinder/audio/Context.h"
#include "cinder/audio/dsp/RingBuffer.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/cocoa/CinderCoreAudio.h"

#include <AudioUnit/AudioUnit.h>

namespace cinder { namespace audio { namespace cocoa {

class DeviceAudioUnit;
class ContextAudioUnit;

class NodeAudioUnit {
  public:
	NodeAudioUnit() : mAudioUnit( nullptr ), mOwnsAudioUnit( true )	{}
	virtual ~NodeAudioUnit();

	virtual ::AudioUnit getAudioUnit() const	{ return mAudioUnit; }

  protected:
	void initAu();
	void uninitAu();

	::AudioUnit			mAudioUnit;
	bool				mOwnsAudioUnit;
	Buffer*				mProcessBuffer;

	struct RenderData {
		RenderData()	: node( nullptr ), context( nullptr )	{}
		~RenderData()	{ node = nullptr; context = nullptr; }

		Node*				node;
		ContextAudioUnit*	context;
	} mRenderData;
};

class OutputDeviceNodeAudioUnit : public OutputDeviceNode, public NodeAudioUnit {
  public:
	OutputDeviceNodeAudioUnit( const DeviceRef &device, const Format &format = Format() );
	virtual ~OutputDeviceNodeAudioUnit() = default;

	void enableProcessing()		override;
	void disableProcessing()	override;

  protected:
	void initialize()	override;
	void uninitialize()	override;
	bool supportsProcessInPlace() const	override	{ return false; }

  private:
	static OSStatus renderCallback( void *data, ::AudioUnitRenderActionFlags *flags, const ::AudioTimeStamp *timeStamp, UInt32 busNumber, UInt32 numFrames, ::AudioBufferList *bufferList );

	bool								mSynchronousIO;

	friend class InputDeviceNodeAudioUnit;
};

class InputDeviceNodeAudioUnit : public InputDeviceNode, public NodeAudioUnit {
  public:
	InputDeviceNodeAudioUnit( const DeviceRef &device, const Format &format = Format() );
	virtual ~InputDeviceNodeAudioUnit();

	void enableProcessing()		override;
	void disableProcessing()	override;

  protected:
	void initialize()				override;
	void uninitialize()				override;
	void process( Buffer *buffer )	override;

  private:
	static OSStatus inputCallback( void *data, ::AudioUnitRenderActionFlags *flags, const ::AudioTimeStamp *timeStamp, UInt32 bus, UInt32 numFrames, ::AudioBufferList *bufferList );

	dsp::RingBuffer						mRingBuffer;
	std::unique_ptr<dsp::Converter>		mConverter;
	BufferDynamic						mReadBuffer, mConvertedReadBuffer;
	AudioBufferListPtr					mBufferList;
	bool								mSynchronousIO;
};

// TODO: when stopped / mEnabled = false; kAudioUnitProperty_BypassEffect should be used
class EffectAudioUnitNode : public Node, public NodeAudioUnit {
  public:
	EffectAudioUnitNode( UInt32 subType, const Format &format = Format() );
	virtual ~EffectAudioUnitNode();

	void setParameter( ::AudioUnitParameterID paramId, float val );

  protected:
	void initialize()				override;
	void uninitialize()				override;
	void process( Buffer *buffer )	override;

  private:
	static OSStatus renderCallback( void *data, ::AudioUnitRenderActionFlags *flags, const ::AudioTimeStamp *timeStamp, UInt32 busNumber, UInt32 numFrames, ::AudioBufferList *bufferList );

	UInt32		mEffectSubType;
	AudioBufferListPtr mBufferList;
};

class ContextAudioUnit : public Context {
  public:
	virtual ~ContextAudioUnit();

	OutputDeviceNodeRef		createOutputDeviceNode( const DeviceRef &device, const Node::Format &format = Node::Format() ) override;
	InputDeviceNodeRef		createInputDeviceNode( const DeviceRef &device, const Node::Format &format = Node::Format() ) override;

	//! set by the OutputNode
	void setCurrentTimeStamp( const ::AudioTimeStamp *timeStamp ) { mCurrentTimeStamp = timeStamp; }
	//! all other NodeAudioUnit's need to pass this correctly formatted timestamp to AudioUnitRender
	const ::AudioTimeStamp* getCurrentTimeStamp() { return mCurrentTimeStamp; }

  private:

	const ::AudioTimeStamp *mCurrentTimeStamp;
};

} } } // namespace cinder::audio::cocoa
