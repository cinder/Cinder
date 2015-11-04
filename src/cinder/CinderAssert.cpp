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

#include "cinder/Cinder.h"
#include "cinder/CinderAssert.h"
#include "cinder/Breakpoint.h"

#include <iostream>

#if defined( CINDER_ANDROID )
#include <android/log.h>
#include <sstream>
#endif

namespace cinder { namespace detail {

void assertionFailedBreak( char const *expr, char const *function, char const *file, long line )
{
	std::cerr << "*** Assertion Failed (break) *** | expression: ( " << expr << " ), location: " << file << "[" << line << "], " << function << std::endl;
	CI_BREAKPOINT();
}

void assertionFailedMessageBreak( char const *expr, char const *msg, char const *function, char const *file, long line )
{
	std::cerr << "*** Assertion Failed (break) *** | expression: ( " << expr << " ), location: " << file << "[" << line << "], " << function << "\n\tmessage: " << msg << std::endl;
	CI_BREAKPOINT();
}

void assertionFailedMessageAbort( char const *expr, char const *msg, char const *function, char const *file, long line )
{
#if defined( CINDER_ANDROID )
	std::stringstream ss;
	ss << "*** Assertion Failed *** | expression: ( " << expr << " ), location: " << file << "[" << line << "], " << function << "\n\tmessage: " << msg << std::endl;
	__android_log_print( ANDROID_LOG_FATAL, "cinder", ss.str().c_str() );
	std::terminate();
#else
	std::cerr << "*** Assertion Failed *** | expression: ( " << expr << " ), location: " << file << "[" << line << "], " << function << "\n\tmessage: " << msg << std::endl;
	std::abort();
#endif
}

} } // namespace cinder::detail
