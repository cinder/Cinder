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

#include "cinder/audio/Node.h"
#include "cinder/audio/Param.h"
#include "cinder/audio/Device.h"

namespace cinder { namespace audio {

typedef std::shared_ptr<class InputNode>				InputNodeRef;
typedef std::shared_ptr<class InputDeviceNode>			InputDeviceNodeRef;
typedef std::shared_ptr<class CallbackProcessorNode>	CallbackProcessorNodeRef;

//! \brief InputNode is the base class for Node's that produce audio. It cannot have any inputs.
//!
//! By default, you must call enable() before a InputNode will process audio. \see Node::Format::autoEnable()
//! The default ChannelMode is set to Node::ChannelMode::MATCHES_OUTPUT, though subclasses may override this to use SPECIFIED instead.
class InputNode : public Node {
  public:
	virtual ~InputNode();

  protected:
	InputNode( const Format &format );
  private:
	// InputNode's cannot have any sources, overridden to assert this method isn't called
	void connectInput( const NodeRef &input ) override;
};

//! \brief Interface representing a Node that communicates with a hardware input device. This is typically a microphone or a 'line-in' on an audio interface.
//!
//! You do not directly construct an InputDeviceNode. Instead, you use the platform-defined method Context::createInputDeviceNode().
//! If number of channels hasn't been specified via Node::Format, defaults to `min( 2, getDevice()->getNumInputChannels() )`.
class InputDeviceNode : public InputNode {
  public:
	virtual ~InputDeviceNode();

	//! Returns the associated \a Device.
	const DeviceRef& getDevice() const		{ return mDevice; }

	//! Returns the frame of the last buffer underrun or 0 if none since the last time this method was called.
	uint64_t getLastUnderrun() const;
	//! Returns the frame of the last buffer overrun or 0 if none since the last time this method was called.
	uint64_t getLastOverrun() const;

  protected:
	InputDeviceNode( const DeviceRef &device, const Format &format );

	//! Should be called by platform-specific implementations that detect an under-run.
	void markUnderrun();
	//! Should be called by platform-specific implementations that detect an over-run.
	void markOverrun();

  private:
	DeviceRef						mDevice;
	mutable std::atomic<uint64_t>	mLastOverrun, mLastUnderrun;
};

//! Callback used to allow simple audio processing without subclassing a Node. First parameter is the Buffer to which to write samples, second parameter is the samplerate.
typedef std::function<void( Buffer *, size_t )> CallbackProcessorFn;

//! InputNode that processes audio with a std::function callback. \see CallbackProcessorFn
class CallbackProcessorNode : public InputNode {
  public:
	CallbackProcessorNode( const CallbackProcessorFn &callbackFn, const Format &format = Format() );
	virtual ~CallbackProcessorNode() {}

  protected:
	void process( Buffer *buffer ) override;

  private:
	CallbackProcessorFn mCallbackFn;
};

} } // namespace cinder::audio
