/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

/* Significant portions of this code come from the public domain libb64 project. For details, see http://sourceforge.net/projects/libb64 */

#include "cinder/Base64.h"

namespace {

typedef enum { step_a, step_b, step_c, step_d } base64_decodestep;

typedef struct {
	base64_decodestep step;
	char plainchar;
} base64_decodestate;

int base64_decode_value( char value_in ) {
	static const char decoding[] = {62,static_cast<char>(-1),static_cast<char>(-1),static_cast<char>(-1),63,52,53,54,55,56,57,58,59,60,61,static_cast<char>(-1),static_cast<char>(-1),static_cast<char>(-1),static_cast<char>(-2),static_cast<char>(-1),static_cast<char>(-1),static_cast<char>(-1),0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,static_cast<char>(-1),static_cast<char>(-1),static_cast<char>(-1),static_cast<char>(-1),static_cast<char>(-1),static_cast<char>(-1),26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
	static const char decoding_size = sizeof(decoding);
	value_in -= 43;
	if (value_in < 0 || value_in > decoding_size) return -1;
	return decoding[(int)value_in];
}

void base64_init_decodestate( base64_decodestate* state_in )
{
	state_in->step = step_a;
	state_in->plainchar = 0;
}

ptrdiff_t base64_decode_block( const char* code_in, const size_t length_in, char* plaintext_out, base64_decodestate* state_in )
{
	const char* codechar = code_in;
	char* plainchar = plaintext_out;
	char fragment;
	
	*plainchar = state_in->plainchar;
	
	switch (state_in->step)
	{
		while (1)
		{
	case step_a:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_a;
					state_in->plainchar = 0;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar    = (fragment & 0x03f) << 2;
	case step_b:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_b;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar++ |= (fragment & 0x030) >> 4;
			*plainchar    = (fragment & 0x00f) << 4;
	case step_c:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_c;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar++ |= (fragment & 0x03c) >> 2;
			*plainchar    = (fragment & 0x003) << 6;
	case step_d:
			do {
				if (codechar == code_in+length_in)
				{
					state_in->step = step_d;
					state_in->plainchar = *plainchar;
					return plainchar - plaintext_out;
				}
				fragment = (char)base64_decode_value(*codechar++);
			} while (fragment < 0);
			*plainchar++   |= (fragment & 0x03f);
		}
	}
	/* control should not reach here */
	return plainchar - plaintext_out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// encode
typedef enum { step_A, step_B, step_C } base64_encodestep;

typedef struct {
	base64_encodestep step;
	char result;
	int stepcount;
} base64_encodestate;

void base64_init_encodestate( base64_encodestate* state_in )
{
	state_in->step = step_A;
	state_in->result = 0;
	state_in->stepcount = 0;
}

char base64_encode_value(char value_in)
{
	static const char* encoding = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	if (value_in > 63) return '=';
	return encoding[(int)value_in];
}

ptrdiff_t base64_encode_block(const char* plaintext_in, size_t length_in, char* code_out, base64_encodestate* state_in, int charsPerLine )
{
	const char* plainchar = plaintext_in;
	const char* const plaintextend = plaintext_in + length_in;
	char* codechar = code_out;
	char result;
	char fragment;
	
	result = state_in->result;
	
	switch (state_in->step)
	{
		while (1)
		{
	case step_A:
			if (plainchar == plaintextend)
			{
				state_in->result = result;
				state_in->step = step_A;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result = (fragment & 0x0fc) >> 2;
			*codechar++ = base64_encode_value(result);
			result = (fragment & 0x003) << 4;
	case step_B:
			if (plainchar == plaintextend)
			{
				state_in->result = result;
				state_in->step = step_B;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result |= (fragment & 0x0f0) >> 4;
			*codechar++ = base64_encode_value(result);
			result = (fragment & 0x00f) << 2;
	case step_C:
			if (plainchar == plaintextend)
			{
				state_in->result = result;
				state_in->step = step_C;
				return codechar - code_out;
			}
			fragment = *plainchar++;
			result |= (fragment & 0x0c0) >> 6;
			*codechar++ = base64_encode_value(result);
			result  = (fragment & 0x03f) >> 0;
			*codechar++ = base64_encode_value(result);
			
			++(state_in->stepcount);
			if ( ( charsPerLine != 0 ) && ( state_in->stepcount == charsPerLine/4 ) )
			{
				*codechar++ = '\n';
				state_in->stepcount = 0;
			}
		}
	}
	/* control should not reach here */
	return codechar - code_out;
}

ptrdiff_t base64_encode_blockend( char* code_out, base64_encodestate* state_in )
{
	char* codechar = code_out;
	
	switch (state_in->step) {
	case step_B:
		*codechar++ = base64_encode_value(state_in->result);
		*codechar++ = '=';
		*codechar++ = '=';
		break;
	case step_C:
		*codechar++ = base64_encode_value(state_in->result);
		*codechar++ = '=';
		break;
	case step_A:
		break;
	}
	
	return codechar - code_out;
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace cinder {

std::string toBase64( const std::string &input, int charsPerLine )
{
	return toBase64( input.c_str(), input.size(), charsPerLine );
}

std::string toBase64( const Buffer &input, int charsPerLine )
{
	return toBase64( input.getData(), input.getSize(), charsPerLine );
}

std::string toBase64( const void *input, size_t inputSize, int charsPerLine )
{
	if( inputSize == 0 ) return std::string();

	if( charsPerLine != 0 )
		charsPerLine -= charsPerLine % 4;
	size_t lines = ( charsPerLine == 0 ) ? 0 : ( inputSize * 4 / 3 / charsPerLine + 1 ); // account for inserted carriage returns
	char *buffer = new char[inputSize * 4 / 3 + 4 + lines];

	base64_encodestate encs;
	base64_init_encodestate( &encs );
	ptrdiff_t resultSize = base64_encode_block( reinterpret_cast<const char*>(input), inputSize, &buffer[0], &encs, charsPerLine );
	resultSize += base64_encode_blockend( &buffer[resultSize], &encs );
	buffer[resultSize] = 0; // null terminate the string
	
	std::string result( buffer );
	delete [] buffer;
	return result;
}

Buffer fromBase64( const std::string &input )
{
	return fromBase64( input.c_str(), input.size() );
}

Buffer fromBase64( const Buffer &input )
{
	return fromBase64( input.getData(), input.getSize() );
}

Buffer fromBase64( const void *input, size_t inputSize )
{
	size_t outputSize = inputSize / 4 * 3;
	Buffer result( outputSize );
	if( inputSize >= 4 ) {
		base64_decodestate decs;
		base64_init_decodestate( &decs );
		ptrdiff_t actualSize = base64_decode_block( reinterpret_cast<const char*>(input), inputSize, (char*)result.getData(), &decs );
		result.setSize( actualSize );
	}
	return result;
}


} // namespace cinder
