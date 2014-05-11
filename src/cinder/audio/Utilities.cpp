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

#include "cinder/audio/Utilities.h"
#include "cinder/Cinder.h"

#include <cstdlib>
#include <memory>

#if defined( CINDER_COCOA )
	#include <cxxabi.h>
#endif

using namespace std;

namespace cinder { namespace audio {

string demangledTypeName( const char *mangledName )
{
#if defined( CINDER_COCOA )
	int status = 0;

	std::unique_ptr<char, void(*)(void*)> result {
        abi::__cxa_demangle( mangledName, NULL, NULL, &status ),
        std::free
    };

	return ( status == 0 ) ? result.get() : mangledName;
#else
	return mangledName;
#endif
}

bool thresholdBuffer( const Buffer &buffer, float threshold, size_t *recordFrame )
{
	const float *buf = buffer.getData();
	size_t count = buffer.getSize();
	for( size_t t = 0; t < count; t++ ) {
		if( fabs( buf[t] ) > threshold ) {
			if( recordFrame )
				*recordFrame = t % buffer.getNumFrames();
			return true;
		}
	}

	return false;
}


} } // namespace cinder::audio