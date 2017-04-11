
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

#include "cinder/Exception.h"

#include <string>

namespace cinder { namespace audio {

//! General audio exception.
class CI_API AudioExc : public Exception {
  public:
	AudioExc( const std::string &description, int32_t errorCode = 0 ) : Exception( description ), mErrorCode( errorCode ) {}

	//! Returns a platform-specific error code. Could return 0 (meaning none was available).
	int32_t getCode() const						{ return mErrorCode; }

  protected:
	int32_t		mErrorCode;
};

//! Audio exception related to Device management.
class CI_API AudioDeviceExc : public AudioExc {
  public:
	AudioDeviceExc( const std::string &description ) : AudioExc( description )	{}
};

//! Audio exception originating from within the Context.
class CI_API AudioContextExc : public AudioExc {
  public:
	AudioContextExc( const std::string &description ) : AudioExc( description )	{}
};

//! Audio exception that occurs when the format of a Node leads to an irrecoverable error.
class CI_API AudioFormatExc : public AudioExc {
  public:
	AudioFormatExc( const std::string &description ) : AudioExc( description )	{}
};

//! Audio exception related to file i/o.
class CI_API AudioFileExc : public AudioExc {
  public:
	AudioFileExc( const std::string &description, int32_t errorCode = 0 ) : AudioExc( description, errorCode )	{}
};

} } // namespace cinder::audio
