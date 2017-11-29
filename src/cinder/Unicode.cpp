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

/*
 * Copyright 2001-2004 Unicode, Inc.
 * 
 * Disclaimer
 * 
 * This source code is provided as is by Unicode, Inc. No claims are
 * made as to fitness for any particular purpose. No warranties of any
 * kind are expressed or implied. The recipient agrees to determine
 * applicability of information provided. If this file has been
 * purchased on magnetic or optical media from Unicode, Inc., the
 * sole remedy for any claim will be exchange of defective media
 * within 90 days of receipt.
 * 
 * Limitations on Rights to Redistribute This Code
 * 
 * Unicode, Inc. hereby grants the right to freely use the information
 * supplied in this file in the creation of products supporting the
 * Unicode Standard, and to make copies of this file in any form
 * for internal or external distribution as long as this notice
 * remains attached.
 */

#include "cinder/Unicode.h"
#include <cstring>
#include <string>

#if defined( CINDER_ANDROID)
#include <stdlib.h>
#endif

#include "utf8cpp/checked.h"
extern "C" {
#include "linebreak.h"
#include "linebreakdef.h"
}

using namespace std;

namespace cinder {

static const char32_t halfBase = 0x0010000UL;
static const char32_t halfMask = 0x3FFUL;
static const int halfShift = 10; // shift by 10 bits

#define UNI_SUR_HIGH_START		(char32_t)0xD800
#define UNI_SUR_HIGH_END		(char32_t)0xDBFF
#define UNI_SUR_LOW_START		(char32_t)0xDC00
#define UNI_SUR_LOW_END			(char32_t)0xDFFF
#define UNI_REPLACEMENT_CHAR	(char32_t)0x0000FFFD
#define UNI_MAX_BMP				(char32_t)0x0000FFFF
#define UNI_MAX_UTF16			(char32_t)0x0010FFFF
#define UNI_MAX_UTF32			(char32_t)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32		(char32_t)0x0010FFFF

std::u16string toUtf16( const char *utf8Str, size_t lengthInBytes )
{
	if( lengthInBytes == 0 )
		lengthInBytes = strlen( utf8Str );
	
	std::u16string result;
	utf8::utf8to16( utf8Str, utf8Str + lengthInBytes, back_inserter( result ));
	return result;
}

std::u16string toUtf16( const std::string &utf8Str )
{
	std::u16string result;
	utf8::utf8to16( utf8Str.begin(), utf8Str.end(), back_inserter( result ));
	return result;
}

std::u32string toUtf32( const char *utf8Str, size_t lengthInBytes )
{
	if( lengthInBytes == 0 )
		lengthInBytes = strlen( utf8Str );
	
	std::u32string result;
	utf8::utf8to32( utf8Str, utf8Str + lengthInBytes, back_inserter( result ));
	return result;
}

std::u32string toUtf32( const std::string &utf8Str )
{
	std::u32string result;
	utf8::utf8to32( utf8Str.begin(), utf8Str.end(), back_inserter( result ));
	return result;
}

std::string toUtf8( const char16_t *utf16Str, size_t lengthInBytes )
{
	if( lengthInBytes == 0 )
		while( utf16Str[lengthInBytes] )
			++lengthInBytes;
	else
		lengthInBytes /= 2;

	std::string result;
	utf8::utf16to8( utf16Str, utf16Str + lengthInBytes, back_inserter( result ));
	return result;	
}

std::string	toUtf8( const std::u16string &utf16Str )
{
	std::string result;
	utf8::utf16to8( utf16Str.begin(), utf16Str.end(), back_inserter( result ));
	return result;
}

std::string toUtf8( const char32_t *utf32Str, size_t lengthInBytes )
{
	if( lengthInBytes == 0 )
		while( utf32Str[lengthInBytes] )
			++lengthInBytes;
	else
		lengthInBytes /= 4;

	std::string result;
	utf8::utf32to8( utf32Str, utf32Str + lengthInBytes, back_inserter( result ));
	return result;
}

std::string	toUtf8( const std::u32string &utf32Str )
{
	std::string result;
	utf8::utf32to8( utf32Str.begin(), utf32Str.end(), back_inserter( result ));
	return result;
}

size_t stringLengthUtf8( const char *str, size_t lengthInBytes )
{
	size_t result = 0;
	size_t nextByte = 0;
	if( lengthInBytes == 0 )
		lengthInBytes = strlen( str );
	while( nextCharUtf8( (const char*)str, &nextByte, lengthInBytes ) != 0xFFFF )
		++result;
	return result;	
}

uint32_t nextCharUtf8( const char *str, size_t *inOutByte, size_t lengthInBytes )
{
	if( lengthInBytes == 0 )
		lengthInBytes = strlen( str );
	return lb_get_next_char_utf8( (const utf8_t*)str, lengthInBytes, inOutByte );
}

size_t advanceCharUtf8( const char *str, size_t numChars, size_t lengthInBytes )
{
	if( lengthInBytes == 0 )
		lengthInBytes = strlen( str );
	size_t nextByte = 0;
	for( size_t curChar = 0; curChar < numChars; ++curChar ) {
		if( lb_get_next_char_utf8( (const utf8_t*)str, lengthInBytes, &nextByte ) == 0xFFFF )
			break;
	}
	
	return nextByte;
}

std::u16string toUtf16( const std::u32string &utf32str )
{
	std::u16string result;
	auto sourceIt = utf32str.cbegin();
	while( sourceIt != utf32str.cend() ) {
		char32_t ch = *sourceIt++;
		if( ch <= UNI_MAX_BMP ) { // Target is a character <= 0xFFFF
			// UTF-16 surrogate values are illegal in UTF-32; 0xffff or 0xfffe are both reserved values
			if( ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END )
				result.push_back( UNI_REPLACEMENT_CHAR );
			else
				result.push_back( static_cast<char16_t>( ch ) );
		}
		else if( ch > UNI_MAX_LEGAL_UTF32 ) {
			result.push_back( UNI_REPLACEMENT_CHAR );
		}
		else { // target is a character in range 0xFFFF - 0x10FFFF.
			ch -= halfBase;
			result.push_back( (char16_t)((ch >> halfShift) + UNI_SUR_HIGH_START) );
			result.push_back( (char16_t)((ch & halfMask) + UNI_SUR_LOW_START) );
		}
	}

    return result;
}

std::u32string toUtf32( const std::u16string &utf16str )
{
	std::u32string result;
	auto sourceIt = utf16str.cbegin();
	while( sourceIt != utf16str.cend() ) {
		char32_t ch = *sourceIt++;
		if( ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END ) {
			// If we have a surrogate pair, convert to utf32 first
			if( sourceIt != utf16str.cend() ) {
				char32_t ch2 = *sourceIt;
				if( ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END ) {
                    ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
                    ++sourceIt;
				}
				else // We don't have the 16 bits following the high surrogate.
					break;
			}
		}
		result.push_back( ch );
	}

	return result;
}

namespace {
bool shouldBreak( char code )
{
	return ( code == LINEBREAK_ALLOWBREAK ) || ( code == LINEBREAK_MUSTBREAK );
}
} // anonymous namespace

void lineBreakUtf8( const char *line, const std::function<bool(const char *, size_t)> &measureFn, const std::function<void(const char *,size_t)> &lineProcessFn )
{
	const size_t lengthInBytes = strlen( line );
	shared_ptr<char> brks = shared_ptr<char>( (char*)malloc( lengthInBytes ), free );
	
	set_linebreaks_utf8( (const uint8_t*)line, lengthInBytes, NULL, brks.get() );

	// Byte-suffixed variables correspond to a byte in the UTF8 string, as opposed to the character
	// binary search for the threshold where measureFn() returns false; emerges as curChar
	size_t charLen = stringLengthUtf8( line, lengthInBytes );
	size_t lineStartByte = 0, lineEndByte = 0;
	size_t lineStartChar = 0;
	while( lineStartChar < charLen ) {
		int minChar = 0;
		int maxChar = (int)charLen - (int)lineStartChar;
		int curChar = 0;
		
		// test to see if we're already on a mustbreak
		if( brks.get()[lineStartByte] != 0 ) {
			// update our maxChar to reflect any MUSTBREAKS
			int maxCharWithMustBreaks = minChar;
			size_t maxCharByte = lineStartByte;

			while( maxCharWithMustBreaks < maxChar ) {
				nextCharUtf8( line, &maxCharByte, lengthInBytes );
				if( brks.get()[maxCharByte] == 0 ) {
					maxCharWithMustBreaks++;
					break;
				}
				else
					++maxCharWithMustBreaks;
			}
			maxChar = maxCharWithMustBreaks + 1;
		
			while( minChar < maxChar ) {
				curChar = minChar + (maxChar-minChar+1)/2;
				size_t newByte = advanceCharUtf8( line + lineStartByte, curChar, lengthInBytes );
				if( ! measureFn( line + lineStartByte, newByte ) )
					maxChar = curChar - 1;
				else
					minChar = curChar;
			}
			curChar = minChar;
		}
		else { // we started at a mustbreak
			curChar = 1;
		}

		// find ideal place to perform the break, either at curChar or before depending on breaks
		size_t lineEndByteAfterBreaking = lineEndByte = advanceCharUtf8( line + lineStartByte, curChar ) + lineStartByte;
		if( ( lineEndByteAfterBreaking < lengthInBytes ) /*&& ( ! shouldBreak( brks.get()[lineEndByteAfterBreaking] ) )*/ ) {
			while( (lineEndByteAfterBreaking > lineStartByte) && ( ! shouldBreak( brks.get()[lineEndByteAfterBreaking-1] ) ) )
				lineEndByteAfterBreaking--;
			if( lineEndByteAfterBreaking == lineStartByte ) // there's no good breakpoint; just break where we would have
				lineEndByteAfterBreaking = lineEndByte;
		}
		lineProcessFn( line + lineStartByte, lineEndByteAfterBreaking - lineStartByte );
		lineStartChar += stringLengthUtf8( line + lineStartByte, lineEndByteAfterBreaking - lineStartByte );
		lineStartByte = lineEndByteAfterBreaking;
		// eat any spaces we'd start on on the next line
		size_t tempByte = lineStartByte;
		while( nextCharUtf8( line, &tempByte, lengthInBytes ) == (uint32_t)' ') {
			lineStartByte = tempByte;
			++lineStartChar;
		}
	}
}

void calcLinebreaksUtf8( const char *str, std::vector<uint8_t> *resultBreaks )
{
	calcLinebreaksUtf8( str, strlen( str ), resultBreaks );
}

void calcLinebreaksUtf8( const char *str, size_t strLength, std::vector<uint8_t> *resultBreaks )
{
	resultBreaks->resize( strLength, 0 );
	
	// our UnicodeBreaks enum lines up with the liblinebreak definitions
	set_linebreaks_utf8( (const uint8_t*)str, strLength, NULL, (char*)&(*resultBreaks)[0] );
}

void calcLinebreaksUtf16( const uint16_t *str, std::vector<uint8_t> *resultBreaks )
{
	size_t strLength = 0;
	while( str[strLength] )
		++strLength;

	calcLinebreaksUtf16( str, strLength, resultBreaks );
}

void calcLinebreaksUtf16( const uint16_t *str, size_t strLength, std::vector<uint8_t> *resultBreaks )
{
	resultBreaks->resize( strLength, 0 );
	
	// our UnicodeBreaks enum lines up with the liblinebreak definitions
	set_linebreaks_utf16( (const unsigned short*)str, strLength, NULL, (char*)&(*resultBreaks)[0] );	
}

}