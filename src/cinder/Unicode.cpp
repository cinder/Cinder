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

void findBreaksUtf8( const std::string &line, std::vector<size_t> *must, std::vector<size_t> *allow )
{
	const size_t length = line.size();
	shared_ptr<char> brks = shared_ptr<char>( (char*)malloc( length ), free );

	set_linebreaks_utf8( (const uint8_t*) line.c_str(), length, NULL, brks.get() );

	//
	must->clear();
	allow->clear();

	//
	size_t byte = 0;
	while( byte < length ) {
		if( brks.get()[byte] == LINEBREAK_ALLOWBREAK )
			allow->push_back( byte );
		else if( brks.get()[byte] == LINEBREAK_MUSTBREAK ) {
			must->push_back( byte );
			allow->push_back( byte );
		}

		byte++;
	}
}

void findBreaksUtf16( const std::wstring &line, std::vector<size_t> *must, std::vector<size_t> *allow )
{
	const size_t length = line.size();
	shared_ptr<char> brks = shared_ptr<char>( (char*)malloc( length ), free );

	set_linebreaks_utf16( (const uint16_t*) line.c_str(), length, NULL, brks.get() );

	//
	must->clear();
	allow->clear();

	size_t byte = 0;
	while( byte < length ) {
		if( brks.get()[byte] == LINEBREAK_ALLOWBREAK )
			allow->push_back( byte );
		else if( brks.get()[byte] == LINEBREAK_MUSTBREAK ) {
			must->push_back( byte );
			allow->push_back( byte );
		}

		byte++;
	}
} 

bool isWhitespaceUtf8( const char ch )
{ 
	return isWhitespaceUtf16( (short) ch ); 
}

bool isWhitespaceUtf16( const short ch )
{
	// see: http://en.wikipedia.org/wiki/Whitespace_character
	static const short unicode_whitespace[] = {
		0x0009, 0x000A, 0x000B, 0x000C, 0x000D,
		0x0020, 0x0085, 0x00A0, 0x1680, 0x180E,
		0x2000, 0x2001, 0x2002, 0x2003, 0x2004,
		0x2005, 0x2006, 0x2007, 0x2008, 0x2009,
		0x200A, 0x2028, 0x2029, 0x202F, 0x205F, 0x3000
	};

	for(size_t i=0;i<26;++i) {
		if( unicode_whitespace[i] == ch )
			return true;
	}

	return false;
}

} // namespace ci