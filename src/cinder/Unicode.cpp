#include "cinder/Unicode.h"
#include <cstring>

extern "C" {
#include "linebreak.h"
#include "linebreakdef.h"
}

using namespace std;

namespace cinder {

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

namespace {
bool shouldBreak( char code )
{
	return ( code == LINEBREAK_ALLOWBREAK ) || ( code == LINEBREAK_MUSTBREAK );
}
} // anonymous namespace

void lineBreakUtf8( const char *line, const std::function<bool(const char *, size_t)> &measureFn, std::function<void(const char *,size_t)> lineProcessFn )
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
		int minChar = 0, maxChar = charLen - lineStartChar /*[minChar,maxChar)*/, curChar = 0;
		
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