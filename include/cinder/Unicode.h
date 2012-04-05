#pragma once

#include "cinder/Cinder.h"
#include "cinder/Function.h"

namespace cinder {

//! Returns the number of characters (not bytes) in the the UTF-8 string \a str. Optimize operation by supplying a non-default \a lengthInBytes of \a str.
size_t		stringLengthUtf8( const char *str, size_t lengthInBytes = 0 );
//!  Returns the UTF-32 code point of the next character in \a str, relative to the byte \a inOutByte. Increments \a inOutByte to be the first byte of the next character. Optimize operation by supplying a non-default \a lengthInBytes of \a str.
uint32_t	nextCharUtf8( const char *str, size_t *inOutByte, size_t lengthInBytes = 0 );
//! Returns the index in bytes of the next character in \a str, advanced by \a numChars characters. Optimize operation by supplying a non-default \a lengthInBytes of \a str.
size_t		advanceCharUtf8( const char *str, size_t numChars, size_t lengthInBytes = 0 );

void		lineBreakUtf8( const char *str, const std::function<bool(const char *, size_t)> &measureFn, std::function<void(const char *,size_t)> lineProcessFn );

}
