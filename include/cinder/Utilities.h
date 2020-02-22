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
#include "cinder/DataTarget.h"
#include <sstream>

namespace cinder {

//! Returns a canonical version of \a path. Collapses '.', ".." and "//". Converts '~' on Cocoa. Expands environment variables on MSW.
CI_API fs::path expandPath( const fs::path &path );
//! Returns a path to the user's home directory.
CI_API fs::path getHomeDirectory();
//! Returns a path to the user's documents directory.
CI_API fs::path getDocumentsDirectory();

//! Launches a path in a web browser
CI_API void launchWebBrowser( const Url &url );
	
//! Returns a vector of substrings split by the separator \a separator. <tt>split( "one two three", ' ' ) -> [ "one", "two", "three" ]</tt> If \a compress is \c true, it will consider consecutive separators as one.
CI_API std::vector<std::string> split( const std::string &str, char separator, bool compress = true );
//! Returns a vector of substrings split by the characters in \a separators. <tt>split( "one, two, three", " ," ) -> [ "one", "two", "three" ]</tt> If \a compress is \c true, it will consider consecutive separators as one.
CI_API std::vector<std::string> split( const std::string &str, const std::string &separators, bool compress = true );

//! Loads the contents of \a dataSource and returns it as a std::string
CI_API std::string loadString( const DataSourceRef &dataSource );
CI_API void writeString( const fs::path &path, const std::string &str );
CI_API void writeString( const DataTargetRef &dataTarget, const std::string &str );

//! Suspends the execution of the current thread until \a milliseconds have passed. Supports sub-millisecond precision only on Mac OS X.
CI_API void sleep( float milliseconds );

//! Returns the path separator for the host operating system's file system, \c '\' on Windows and \c '/' on Mac OS
#if defined( CINDER_MSW )
CI_API inline char getPathSeparator() { return '\\'; }
#else
inline char getPathSeparator() { return '/'; }
#endif

//! Returns a std::map of the system's environment variables. Empty on WinRT.
CI_API std::map<std::string, std::string> getEnvironmentVariables();

template<typename T>
std::string toString( const T &t ) { std::ostringstream ss; ss << t; return ss.str(); }

template<typename T>
T fromString( const std::string &s ) { std::stringstream ss; ss << s; T temp; ss >> temp; return temp; }

template<>
inline std::string fromString( const std::string &s ) { return s; }

#if defined(CINDER_COCOA_TOUCH)
// Necessary because boost::lexical_cast crashes when trying to convert a string to a double on iOS
template<>
inline double fromString( const std::string &s ) { return atof( s.c_str() ); }
#endif

//! returns \c true if ASCII strings \a a and \a b are case-insensitively equal. Not Unicode-aware.
CI_API bool asciiCaseEqual( const std::string &a, const std::string &b );
//! returns \c true if ASCII strings \a a and \a b are case-insensitively equal. Not Unicode-aware.
CI_API bool asciiCaseEqual( const char *a, const char *b );
//! returns equivalent of strcmp() using ASCII case-insensitive comparison
CI_API int asciiCaseCmp( const char *a, const char *b );

//! returns a copy of \a str with all whitespace (as defined by std::isspace()) removed from beginning and end. Unicode aware.
CI_API std::string trim( const std::string &str );

//! Returns a stack trace (aka backtrace) where \c stackTrace()[0] == caller, \c stackTrace()[1] == caller's parent, etc
CI_API std::vector<std::string> stackTrace();

//! Sets the name of the current thread to \a name
CI_API void setThreadName( const std::string &name );

// ENDIANNESS
CI_API inline int8_t	swapEndian( int8_t val ) { return val; }
CI_API inline uint8_t	swapEndian( uint8_t val ) { return val; }
extern CI_API int16_t	swapEndian( int16_t val );
extern CI_API uint16_t	swapEndian( uint16_t val );
extern CI_API int32_t	swapEndian( int32_t val );
extern CI_API uint32_t	swapEndian( uint32_t val );
extern CI_API int64_t	swapEndian( int64_t val );
extern CI_API uint64_t	swapEndian( uint64_t val );
extern CI_API float		swapEndian( float val );
extern CI_API double	swapEndian( double val );

extern CI_API void swapEndianBlock( uint16_t *blockPtr, size_t blockSizeInBytes );
extern CI_API void swapEndianBlock( float *blockPtr, size_t blockSizeInBytes );

} // namespace cinder
