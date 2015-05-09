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

#include "cinder/audio/NodeMath.h"

namespace cinder { namespace audio {

typedef std::shared_ptr<class GainNode>			GainNodeRef;

//! \brief Node for changing the gain, or amplitude, of a signal.
//!
//! Controlling the gain is a fundamental operation in audio signal processing, useful in controlling loudness,
//! adding dynamics, and many other effects. The operation itself is simply a vector-based multiply and GainNode obtains
//! by subclassing MultipleNode.  The only difference between the two is that GainNode's default value is 1, meaning no gain.
class GainNode : public MultiplyNode {
  public:
	//! Constructs a GainNode with an initial gain of 1 and optional \a format.
	GainNode( const Format &format = Format() )
		: MultiplyNode( 1, format )
	{}
	//! Constructs a GainNode with an initial gain of \a initialValue and optional \a format.
	GainNode( float initialValue, const Format &format = Format() )
		: MultiplyNode( initialValue, format )
	{}
};

} } // namespace cinder::audio
