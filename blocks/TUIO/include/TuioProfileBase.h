/*
 Copyright (c) 2010, The Cinder Project: http://libcinder.org
 All rights reserved.

 Portions Copyright (c) 2010, Hector Sanchez-Pajares
 Aer Studio http://www.aerstudio.com
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

namespace cinder { namespace tuio {
	
// Base class from which the Object and Cursor profiles are derived
class ProfileBase {
  public:
	ProfileBase() : mSessionId( -1 ), mSource("Unknown") {}

	ProfileBase( std::string source, int32_t sessionId ) 
		: mSessionId( sessionId ), mSource(source)
	{
	}
		
	int32_t	getSessionId() const { return mSessionId; }
	const std::string & getSource() const { return mSource; }

	//! Translates into a ci::TouchEvent::Touch
	app::TouchEvent::Touch getTouch( double time, const vec2 &posScale ) const {
		return app::TouchEvent::Touch( getPos() * posScale, getPrevPos() * posScale, (uint32_t)getSessionId(), time, NULL );
	}

	virtual vec2	getPos() const = 0;
	virtual vec2	getPrevPos() const = 0;

  protected:
	int32_t		mSessionId;
	std::string mSource;
};

} } // namespace cinder::tuio
