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
#include <functional>
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

//! Removes all files beyond maxFileCount.
CI_API void limitDirectoryFileCount( const fs::path& directoryPath, size_t maxFileCount, std::function<bool(const fs::path&, const fs::path&)> sortFn = []( const fs::path& p1, const fs::path& p2 ) -> bool { return fs::last_write_time( p1 ) > fs::last_write_time( p2 ); } );

//! Searches upwards from \a start (file or directory) for an ancestor directory where \a relativeSearch exists, up to \a maxDepth levels; returns absolute path if found, otherwise empty path.
CI_API fs::path findAncestorDir( const fs::path& start, const fs::path& relativeSearch, int maxDepth = 10 );
//! Searches upwards from \a start (file or directory) for an ancestor file where \a relativeSearch exists, up to \a maxDepth levels; returns absolute path if found, otherwise empty path.
CI_API fs::path findAncestorFile( const fs::path& start, const fs::path& relativeSearch, int maxDepth = 10 );

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

//! Returns a std::map of the system's environment variables.
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
//! returns equivalent of strcmp() using ASCII case-insensitive comparison. Not Unicode-aware.
CI_API int asciiCaseCmp( const char *a, const char *b );

//! removes all whitespace (as defined by std::isspace()) from the beginning of \a str. Unicode aware.
CI_API void trimLeftInPlace( std::string &str );
//! removes all whitespace (as defined by std::isspace()) from the beginning of \a str and returns a copy. Unicode aware.
CI_API std::string trimLeft( std::string str );

//! removes all whitespace (as defined by std::isspace()) from the end of \a str. Unicode aware.
CI_API void trimRightInPlace( std::string &str );
//! removes all whitespace (as defined by std::isspace()) from the end of \a str and returns a copy. Unicode aware.
CI_API std::string trimRight( std::string str );

//! removes all whitespace (as defined by std::isspace()) removed from beginning and end of \a str. Unicode aware.
CI_API void trimInPlace( std::string &str );
//! returns a copy of \a str with all whitespace (as defined by std::isspace()) removed from beginning and end. Unicode aware.
CI_API std::string trim( std::string str );

//! removes all specified \a characters from the beginning of \a str. Not Unicode-aware.
CI_API void trimLeftInPlace( std::string &str, const std::string &characters );
//! removes all specified \a characters from the beginning of \a str. Not Unicode-aware.
CI_API std::string trimLeft( std::string str, const std::string &characters );

//! removes all specified \a characters from the end of \a str. Not Unicode-aware.
CI_API void trimRightInPlace( std::string &str, const std::string &characters );
//! removes all specified \a characters from the end of \a str. Not Unicode-aware.
CI_API std::string trimRight( std::string str, const std::string &characters );

//! removes all specified \a characters from \a str. Not Unicode-aware.
CI_API void trimInPlace( std::string &str, const std::string &characters );
//! removes all specified \a characters from \a str. Not Unicode-aware.
CI_API std::string trim( std::string str, const std::string &characters );

//! removes all occurrences of any of \a characters in \a str. Not Unicode-aware.
CI_API void filterInPlace( std::string &str, const std::string &characters );
//! returns a copy of \a str with all occurrences of any of \a characters removed. Not Unicode-aware.
CI_API std::string filter( std::string str, const std::string &characters );

//! Converts the character \a c to lowercase. Not Unicode-aware.
CI_API char charToLower( const char c );
//! Converts the character \a c to uppercase. Not Unicode-aware.
CI_API char charToUpper( const char c );

//! converts all characters to lowercase (using std::tolower()) in \a str. Not Unicode-aware.
CI_API void toLowerInPlace( std::string &str );
//! returns a copy of \a str with all characters converted to lowercase (using std::tolower()). Not Unicode-aware.
CI_API std::string toLower( std::string str );
//! converts all characters to uppercase (using std::toupper()) in \a str.Not Unicode-aware.
CI_API void toUpperInPlace( std::string &str );
//! returns a copy of \a str with all characters converted to uppercase (using std::toupper()). Not Unicode-aware.
CI_API std::string toUpper( std::string str );

//! replaces all instances of \a find with \a replace in \a str. Unicode-aware.
CI_API void findReplaceInPlace( std::string &str, const std::string &find, const std::string &replace );
//! replaces all instances of \a find with \a replace in \a str and returns a copy. Unicode-aware.
CI_API std::string findReplace( std::string str, const std::string &find, const std::string &replace );

//! returns whether character \a c is considered white space. Not Unicode-aware.
CI_API bool isWhiteSpace( char c );
//! returns whether character \a c is a digit (0-9). Not Unicode-aware.
CI_API bool isDigit( char c );
//! returns whether character \a c is a hexadecimal digit (0-9)+(a-f). Not Unicode-aware.
CI_API bool isHexDigit( char c );
//! returns whether character \a c is alphabetic (a-z). Not Unicode-aware.
CI_API bool isAlpha( char c );
//! returns whether character \a c is numeric (0-9)+(.+-eE). Not Unicode-aware.
CI_API bool isNumeric( char c );

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
