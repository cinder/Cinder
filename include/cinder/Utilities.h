/*
 Copyright (c) 2010, The Barbarian Group
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

#include <string>
#include <vector>
#include "cinder/Cinder.h"
#include <boost/lexical_cast.hpp>

namespace cinder {

//! Returns a canonical version of \a path by expanding a "~" and symlinks on the Mac "..", "." and "//"
std::string expandPath( const std::string &path );
//! Returns a path to the user's home directory.
std::string getHomeDirectory();
//! Returns a path to the user's documents directory.
std::string getDocumentsDirectory();
//! Returns a path to the user's temporary directory.
std::string getTemporaryDirectory();
//! Returns a path that is gauranteed to be unique and is suitable for creating a temporary file. An optional \a prefix parameters allows specification of a file name prefix, some portion of which will be incorporated into the result. Note a race condition that can exist between the uniqueness of the path and the creation of the file.
std::string getTemporaryFilePath( const std::string &prefix = "" );
//! Returns the directory portion of file path \a path, the last component of which must be a file name or a terminating path separator. 
std::string getPathDirectory( const std::string &path );
//! Returns the file name portion of file path \a path. For example \c "C:\Images\Beyonce.jpg" returns \c "Beyonce.jpg".
std::string getPathFileName( const std::string &path );
//! Returns the file extension of the file located at \a path
std::string getPathExtension( const std::string &path );
//! Creates a directory at \a path and optionally creates any missing parent directories when \a createParents is \c true. Returns \c true upon success.
bool createDirectories( const std::string &path, bool createParents = true );

//! Delete the file at \a path. Fails quietly if the path does not exist.
void deleteFile( const std::string &path );

//! Returns a utf-16 encoded std::wstring by converting the utf-8 encoded string \a utf8
std::wstring toUtf16( const std::string &utf8 );
//! Returns a utf-8 encoded std::string by converting the utf-16 encoded string \a utf16
std::string toUtf8( const std::wstring &utf16 );

//! Suspends the execution of the current thread until \a milliseconds have passed. Supports sub-millisecond precision only on Mac OS X.
void sleep( float milliseconds );

//! Returns the path separator for the host operating system's file system, \c '\' on Windows and \c '/' on Mac OS
#if defined( CINDER_MSW )
inline char getPathSeparator() { return '\\'; }
#else
inline char getPathSeparator() { return '/'; }
#endif

template<typename T>
inline std::string toString( const T &t ) { return boost::lexical_cast<std::string>( t ); }
template<typename T>
inline T fromString( const std::string &s ) { return boost::lexical_cast<T>( s ); }

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
