/*
 Copyright (c) 2013, The Cinder Project
 All rights reserved.
 
 This code is designed for use with the Cinder C++ library, http://libcinder.org

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

#include <string>
#include <functional>
#include <vector>

namespace cinder {

CI_API std::u16string	toUtf16( const char *utf8Str, size_t lengthInBytes = 0 );
CI_API std::u16string	toUtf16( const std::string &utf8Str );
CI_API std::u32string	toUtf32( const char *utf8Str, size_t lengthInBytes = 0 );
CI_API std::u32string	toUtf32( const std::string &utf8Str );

CI_API std::string		toUtf8( const char16_t *utf16Str, size_t lengthInBytes = 0 );
CI_API std::string		toUtf8( const std::u16string &utf16Str );

CI_API std::string		toUtf8( const char32_t *utf32str, size_t lengthInBytes = 0 );
CI_API std::string		toUtf8( const std::u32string &utf32Str );

CI_API std::u16string	toUtf16( const std::u32string &utf32str );
CI_API std::u32string	toUtf32( const std::u16string &utf16str );

//! Returns the number of characters (not bytes) in the the UTF-8 string \a str. Optimize operation by supplying a non-default \a lengthInBytes of \a str.
CI_API size_t	stringLengthUtf8( const char *str, size_t lengthInBytes = 0 );
//!  Returns the UTF-32 code point of the next character in \a str, relative to the byte \a inOutByte. Increments \a inOutByte to be the first byte of the next character. Optimize operation by supplying a non-default \a lengthInBytes of \a str.
CI_API uint32_t	nextCharUtf8( const char *str, size_t *inOutByte, size_t lengthInBytes = 0 );
//! Returns the index in bytes of the next character in \a str, advanced by \a numChars characters. Optimize operation by supplying a non-default \a lengthInBytes of \a str.
CI_API size_t	advanceCharUtf8( const char *str, size_t numChars, size_t lengthInBytes = 0 );

CI_API void		lineBreakUtf8( const char *str, const std::function<bool(const char *, size_t)> &measureFn, const std::function<void(const char *,size_t)> &lineProcessFn );

//! Values returned by calcBreaksUtf8 and calcBreaksUtf16
enum UnicodeBreaks { UNICODE_MUST_BREAK, UNICODE_ALLOW_BREAK, UNICODE_NO_BREAK, UNICODE_INSIDE_CHAR };

//! Sets \a resultBreaks to be of the same length as the null-terminated UTF-8 string \a str with the values enumerated by UnicodeBreaks
CI_API void		calcLinebreaksUtf8( const char *str, std::vector<uint8_t> *resultBreaks );

//! Sets \a resultBreaks to be of the same length as the UTF-8 string \a str with the values enumerated by UnicodeBreaks
CI_API void		calcLinebreaksUtf8( const char *str, size_t strLength, std::vector<uint8_t> *resultBreaks );

//! Sets \a resultBreaks to be of the same length as the null-terminated UTF-16 string \a str with the values enumerated by UnicodeBreaks
CI_API void		calcLinebreaksUtf16( const uint16_t *str, std::vector<uint8_t> *resultBreaks );

//! Sets \a resultBreaks to be of the same length as the UTF-16 string \a str with the values enumerated by UnicodeBreaks
CI_API void		calcLinebreaksUtf16( const uint16_t *str, size_t strLength, std::vector<uint8_t> *resultBreaks );

}