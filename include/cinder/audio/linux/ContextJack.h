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

#pragma once

#include "cinder/audio/Context.h" 

namespace cinder { namespace audio { namespace linux {

struct OutputDeviceNodeJackImpl;
struct InputDeviceNodeJackImpl;
class ContextJack;

class OutputDeviceNodeJack : public OutputDeviceNode {
  public:
	OutputDeviceNodeJack( const DeviceRef &device, const Format &format, const std::shared_ptr<ContextJack> &context );

  protected:
	void initialize()				override;
	void uninitialize()				override;
	void enableProcessing()			override;
	void disableProcessing()		override;
	bool supportsProcessInPlace() const	override	{ return false; }

  private:
	void renderToBufferFromInputs();

	std::unique_ptr<OutputDeviceNodeJackImpl>     mImpl;

	friend struct OutputDeviceNodeJackImpl;
};

class InputDeviceNodeJack : public InputDeviceNode {
  public:
	InputDeviceNodeJack( const DeviceRef &device, const Format &format, const std::shared_ptr<ContextJack> &context );
	virtual ~InputDeviceNodeJack();

  protected:
	void initialize()				override;
	void uninitialize()				override;
	void enableProcessing()			override;
	void disableProcessing()		override;
	void process( Buffer *buffer )	override;

  private:

	std::unique_ptr<InputDeviceNodeJackImpl>   mImpl;

	friend struct InputDeviceNodeJackImpl;
};

class ContextJack : public Context {
  public:
	ContextJack();
	virtual ~ContextJack();

	OutputDeviceNodeRef	createOutputDeviceNode( const DeviceRef &device, const Node::Format &format = Node::Format() ) override;
	InputDeviceNodeRef	createInputDeviceNode( const DeviceRef &device, const Node::Format &format = Node::Format()  ) override;

	//SLObjectItf getSLEngineObject() { return mSLEngineObject; }
	//SLEngineItf getSLEngineEngine() { return mSLEngineEngine; }

  private:
	//SLObjectItf mSLEngineObject = nullptr;
	//SLEngineItf mSLEngineEngine = nullptr;
};	

} } } // namespace cinder::audio::linux