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

typedef std::shared_ptr<class Pan2dNode>		Pan2dNodeRef;

//! Simple stereo panning using an equal power cross-fade. The panning position is specified by a single position between the left and right speakers.
class CI_API Pan2dNode : public Node {
  public:
	//! Constructs a Pan2dNode. \note Format::channel() and Format::channelMode() are ignored and number of output channels is always 2.
	Pan2dNode( const Format &format = Format() );
	virtual ~Pan2dNode() {}

	//! Sets the panning position in range of [0:1]: 0 = left, 1 = right, and 0.5 = center.
	void setPos( float pos );
	//! Gets the current panning position in range of [0:1]: 0 = left, 1 = right, and 0.5 = center.
	float getPos() const	{ return mPos.getValue(); }
	//! Returns a pointer to the Param that controls the position.
	Param* getParamPos()			{ return &mPos; }

	//! \brif Sets whether 'stereo input mode' is enabled or not (it is disabled by default)
	//!
	//! This allows one to pan its stereo input without just silenecing one channel or the other (the farther away channel 'leaks' into the one you are panning to).
	//! It is a bit of a farce to stereo pan an already stereo signal, but this allows you to do it anyway and works reasonably well.
	void setStereoInputModeEnabled( bool enable = true )	{ mStereoInputMode = enable; }
	//! Returns whether 'stereo input mode' is enabled or not (disabled by default).
	bool isStereoInputModeEnabled() const					{ return mStereoInputMode; }

protected:
	void process( Buffer *buffer ) override;

  private:
	void processRegularMode( Buffer *buffer );
	void processStereoInputMode( Buffer *buffer );

	Param	mPos;
	bool	mStereoInputMode;
};

} } // namespace cinder::audio
