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

#pragma once

#include <string>
#include <vector>
#include <map>

#include "cinder/Cinder.h"
#include "cinder/Url.h"
#include "cinder/DataSource.h"
#undef check
#include <boost/lexical_cast.hpp>

namespace cinder {

//! Returns a canonical version of \a path. Collapses '.', ".." and "//". Converts '~' on Cocoa. Expands environment variables on MSW.
fs::path expandPath( const fs::path &path );
//! Returns a path to the user's home directory.
fs::path getHomeDirectory();
//! Returns a path to the user's documents directory.
fs::path getDocumentsDirectory();

//! Launches a path in a web browser
void launchWebBrowser( const Url &url );
	
//! Returns a vector of substrings split by the separator \a separator. <tt>split( "one two three", ' ' ) -> [ "one", "two", "three" ]</tt> If \a compress is TRUE, it will consider consecutive separators as one.
std::vector<std::string> split( const std::string &str, char separator, bool compress = true );
//! Returns a vector of substrings split by the characters in \a separators. <tt>split( "one, two, three", " ," ) -> [ "one", "two", "three" ]</tt> If \a compress is TRUE, it will consider consecutive separators as one.
std::vector<std::string> split( const std::string &str, const std::string &separators, bool compress = true );

//! Loads the contents of \a dataSource and returns it as a std::string
std::string loadString( const DataSourceRef &dataSource );

//! Suspends the execution of the current thread until \a milliseconds have passed. Supports sub-millisecond precision only on Mac OS X.
void sleep( float milliseconds );

//! Returns the path separator for the host operating system's file system, \c '\' on Windows and \c '/' on Mac OS
#if defined( CINDER_MSW )
inline char getPathSeparator() { return '\\'; }
#else
inline char getPathSeparator() { return '/'; }
#endif

//! Returns a std::map of the system's environment variables. Empty on WinRT.
std::map<std::string, std::string> getEnvironmentVariables();

template<typename T>
inline std::string toString( const T &t ) { return boost::lexical_cast<std::string>( t ); }
template<typename T>
inline T fromString( const std::string &s ) { return boost::lexical_cast<T>( s ); }
// This specialization seems to only be necessary with more recent versions of Boost
template<>
inline Url fromString( const std::string &s ) { return Url( s ); }
#if defined(CINDER_COCOA_TOUCH)
// Necessary because boost::lexical_cast crashes when trying to convert a string to a double on iOS
template<>
inline double fromString( const std::string &s ) { return atof( s.c_str() ); }
#endif

//! Returns a stack trace (aka backtrace) where \c stackTrace()[0] == caller, \c stackTrace()[1] == caller's parent, etc
std::vector<std::string> stackTrace();

// ENDIANNESS
inline int8_t	swapEndian( int8_t val ) { return val; }
inline uint8_t	swapEndian( uint8_t val ) { return val; }
extern int16_t	swapEndian( int16_t val );
extern uint16_t	swapEndian( uint16_t val );
extern int32_t	swapEndian( int32_t val );
extern uint32_t swapEndian( uint32_t val );
extern float	swapEndian( float val );
extern double	swapEndian( double val );

extern void swapEndianBlock( uint16_t *blockPtr, size_t blockSizeInBytes );
extern void swapEndianBlock( float *blockPtr, size_t blockSizeInBytes );

} // namespace cinder
