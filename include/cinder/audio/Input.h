/*
 Copyright (c) 2009, The Barbarian Group
 All rights reserved.

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

#include "cinder/Cinder.h"
#include "cinder/audio/PcmBuffer.h"

namespace cinder { namespace audio {

//! \cond
// This is an abstract base class for implementing Input
class InputImpl {
 public:
	virtual ~InputImpl() {}
	
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual PcmBuffer32fRef getPcmBuffer() = 0;
	virtual bool isCapturing() const = 0;
	
	virtual uint32_t getSampleRate() const = 0;
	virtual uint16_t getChannelCount() const = 0;
 protected:
	InputImpl() {}
};
//! \endcond

class Input {
 public:
	Input() {}
	Input( bool placeHolder );
	~Input() {}
	
	//! Starts capturing audio data from the input
	void start() { mImpl->start(); }
	//! Stops capturing audio data from the input
	void stop() { mImpl->stop(); }
	//! Returns a copy of the buffer of the most recently captured audio data
	PcmBuffer32fRef getPcmBuffer() { return mImpl->getPcmBuffer(); }
	//! Returns whether or not the input is currently capturing audio data
	bool isCapturing() const { return mImpl->isCapturing(); }
	//! Returns the sample rate of the captured audio data
	uint32_t getSampleRate() { return mImpl->getSampleRate(); };
	//! Returns the number of channels of the captured audio data
	uint16_t getChannelCount() { return mImpl->getChannelCount(); };
 private:
	shared_ptr<InputImpl> mImpl;
 public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef shared_ptr<InputImpl> Input::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mImpl.get() == 0 ) ? 0 : &Input::mImpl; }
	void reset() { mImpl.reset(); }
	//@}
};

}} //namespace