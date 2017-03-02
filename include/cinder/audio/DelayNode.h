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

namespace cinder { namespace audio {

typedef std::shared_ptr<class DelayNode>		DelayNodeRef;

//! \brief General purpose delay line, supporting variable delay with linear interpolation.
//!
//! Enables feedback if connected in a graph cycle.
//! \note Restrictions: The current minimum delay length is one processing block. Restricts channel count to 1 (mixes down inputs).
class CI_API DelayNode : public Node {
  public:
	//! Constructs a DelayNode with an optional \a format.
	DelayNode( const Format &format = Format() );

	//! Sets the maximimum delay in seconds.
	void	setMaxDelaySeconds( float seconds );
	//! Returns the maximum delay in seconds.
	float	getMaxDelaySeconds() const		{ return mMaxDelaySeconds; }

	//! Sets the delay value in seconds. Must be at least at least the length of one processing block.
	void	setDelaySeconds( float seconds );
	//! Returns the delay value in seconds.
	float	getDelaySeconds() const				{ return mParamDelaySeconds.getValue(); }
	//! Returns the Param used to automate the delay seconds. \note Values over max delay seconds will be clipped, causing pops.
	Param*	getParamDelaySeconds()			{ return &mParamDelaySeconds; }

	//! Clears any samples in the delay buffer (sets them to zero).
	void clearBuffer();

  protected:
	void initialize()				override;
	void process( Buffer *buffer )	override;
	bool supportsCycles() const		override	{ return true; }

	size_t			mWriteIndex;
	float			mMaxDelaySeconds, mSampleRate;
	Param			mParamDelaySeconds;
	BufferDynamic	mDelayBuffer;
};

} } // namespace cinder::audio
