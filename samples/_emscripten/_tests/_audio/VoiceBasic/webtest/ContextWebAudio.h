/*
 Copyright (c) 2018, The Cinder Project

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
#include <emscripten/bind.h>
#include "cinder/audio/Context.h"
#include "cinder/audio/dsp/Converter.h"
#include "cinder/audio/dsp/RingBuffer.h"
#include "cinder/Log.h"
#include "cinder/emscripten/CinderEmscripten.h"
#include <functional>

using namespace emscripten;

namespace cinder { namespace audio { namespace em {

  class ContextWebAudio;
  struct OutputDeviceNodeWebAudioImpl;

  class OutputDeviceNodeWebAudio : public OutputDeviceNode {
    public:
  	OutputDeviceNodeWebAudio( const DeviceRef &device, const Format &format, const std::shared_ptr<ContextWebAudio> &context );
    protected:
  	void initialize()				override;
  	void uninitialize()				override{}
  	void enableProcessing()			override{}
  	void disableProcessing()		override{}
  	bool supportsProcessInPlace() const	override	{ return false; }
    emscripten::val getRawContext();

    private:
  	void renderInputs(emscripten::val e);

    //! all of the outputs that need to be summed and written out.
    std::vector<NodeRef> mOutputInputs;

  	std::unique_ptr<OutputDeviceNodeWebAudioImpl> mImpl;
  	BufferInterleaved mInterleavedBuffer;

  	friend struct OutputDeviceNodeWebAudioImpl;
  };

  class ContextWebAudio : public Context {

    //! Emscripten value represeting a browser's AudioContext object
    emscripten::val mContext;

    //! Emscripten value representing a browser's ScriptProcessorNode object. This is used
    //! to "render" the internal buffer calculated by the Cinder audio API.
    //! IMPORTANT - note that the bufferSize property needs to match the frame size of the context's internal buffer.
    //! TODO a replacement for this will have to be thought through at some point as this is set to be deprecated at some point.
    emscripten::val mScriptNode;

    //! Emscripten value representing a browser's Oscillator object. This is used to process and playback the ScriptNodeProcessor information
    emscripten::val mOscNode;

    //! Emscripten value that holds the callback function to do the audio processing.
    emscripten::val mCallbackFunction;

    public:
  	ContextWebAudio();
  	virtual ~ContextWebAudio();

    //! Creates an output device for the web and beings setting up everything to get processed.
  	OutputDeviceNodeRef	createOutputDeviceNode( const DeviceRef &device = Device::getDefaultOutput(), const Node::Format &format = Node::Format() ) override;

    // TODO input not implemented yet.
    InputDeviceNodeRef	createInputDeviceNode( const DeviceRef &device = Device::getDefaultInput(), const Node::Format &format = Node::Format()  ) override;

    //! Sets the ScriptProcessorNode's bufferSize property.
    void setOutputBufferSize(int size);

    //! Sets the callback function to use to process audio data.
    void setRenderFunction();

    //! Sets the callback function to use to process audio data.
    void setRenderFunction(std::function<void(emscripten::val e)> functor);

    emscripten::val getRawContext();

    private:
  	  std::vector<std::weak_ptr<Node>>	mDeviceNodes;
  };



}}}
