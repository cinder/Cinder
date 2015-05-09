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

#include "cinder/CinderAssert.h"

#include <iostream>
#include <csignal>

namespace cinder { namespace detail {

void assertionFailedBreak( char const *expr, char const *function, char const *file, long line )
{
	std::cerr << "*** Assertion Failed (break) *** | expression: ( " << expr << " ), location: " << file << "[" << line << "], " << function << std::endl;

#if defined( CINDER_MSW )
	__debugbreak();
#else
	std::raise( SIGINT );
#endif
}

void assertionFailedMessageBreak( char const *expr, char const *msg, char const *function, char const *file, long line )
{
	std::cerr << "*** Assertion Failed (break) *** | expression: ( " << expr << " ), location: " << file << "[" << line << "], " << function << "\n\tmessage: " << msg << std::endl;

#if defined( CINDER_MSW )
	__debugbreak();
#else
	std::raise( SIGINT );
#endif
}

void assertionFailedMessageAbort( char const *expr, char const *msg, char const *function, char const *file, long line )
{
	std::cerr << "*** Assertion Failed *** | expression: ( " << expr << " ), location: " << file << "[" << line << "], " << function << "\n\tmessage: " << msg << std::endl;
	std::abort();
}

} } // namespace cinder::detail