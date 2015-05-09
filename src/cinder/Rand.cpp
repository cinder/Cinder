/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#include "cinder/Rand.h"
#if defined( CINDER_COCOA )
#	include <mach/mach.h>
#	include <mach/mach_time.h>
#elif (defined( CINDER_MSW ) || defined( CINDER_WINRT ))
#	include <windows.h>
#endif

namespace cinder {
	
std::mt19937 Rand::sBase( 310u );
std::uniform_real_distribution<float> Rand::sFloatGen;

void Rand::randomize()
{
#if defined( CINDER_COCOA )
	sBase = std::mt19937( (uint32_t)( mach_absolute_time() & 0xFFFFFFFF ) );
#elif defined( CINDER_WINRT)
	sBase = std::mt19937( static_cast<unsigned long>(::GetTickCount64()) );
#else
	sBase = std::mt19937( ::GetTickCount() );
#endif
}

void Rand::randSeed( uint32_t seed )
{
	sBase = std::mt19937( seed );
}

void Rand::seed( uint32_t seedValue )
{
	mBase = std::mt19937( seedValue );
}

} // ci