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

#include "cinder/Cinder.h"
#include "cinder/Utilities.h"
#include "cinder/Unicode.h"
#include "cinder/app/Platform.h"

#if defined( CINDER_COCOA )
	#include "cinder/cocoa/CinderCocoa.h"
#endif

#include <vector>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using std::vector;
using std::string;
using std::wstring;

namespace cinder {

fs::path expandPath( const fs::path &path )
{
	return app::Platform::get()->expandPath( path );
}

fs::path getHomeDirectory()
{
	return app::Platform::get()->getHomeDirectory();
}

fs::path getDocumentsDirectory()
{
	return app::Platform::get()->getDocumentsDirectory();
}

void launchWebBrowser( const Url &url )
{
	app::Platform::get()->launchWebBrowser( url );
}

std::map<std::string, std::string> getEnvironmentVariables()
{
	return app::Platform::get()->getEnvironmentVariables();
}

std::vector<std::string> split( const std::string &str, char separator, bool compress )
{
	return split( str, string( 1, separator ), compress );
}

std::vector<std::string> split( const std::string &str, const std::string &separators, bool compress )
{
	vector<string> result;

	boost::algorithm::split( result, str, boost::is_any_of(separators),
		compress ? boost::token_compress_on : boost::token_compress_off );

	return result;
}

string loadString( const DataSourceRef &dataSource )
{
	auto buffer = dataSource->getBuffer();
	const char *data = static_cast<const char *>( buffer->getData() );

	return string( data, data + buffer->getSize() );
}

void writeString( const fs::path &path, const std::string &str )
{
	writeString( (DataTargetRef)writeFile( path ), str );
}

void writeString( const DataTargetRef &dataTarget, const std::string &str )
{
	fs::path outPath = dataTarget->getFilePath();
	if( outPath.empty() ) {
		throw ci::Exception( "writeString can only write to file." );
	}

	std::ofstream ofs( outPath.string(), std::ofstream::binary );
	ofs << str;
	ofs.close();
}

void sleep( float milliseconds )
{
	app::Platform::get()->sleep( milliseconds );
}

vector<string> stackTrace()
{
	return app::Platform::get()->stackTrace();
}

int16_t swapEndian( int16_t val )
{
	return (int16_t) (	(((uint16_t) (val) & (uint16_t) 0x00ffU) << 8) |
						(((uint16_t) (val) & (uint16_t) 0xff00U) >> 8) );
}

uint16_t swapEndian( uint16_t val )
{
	return (uint16_t) (	(((uint16_t) (val) & (uint16_t) 0x00ffU) << 8) |
						(((uint16_t) (val) & (uint16_t) 0xff00U) >> 8) );
}

int32_t swapEndian( int32_t val )
{
	return (int32_t)((((uint32_t) (val) & (uint32_t) 0x000000FFU) << 24) |
					 (((uint32_t) (val) & (uint32_t) 0x0000FF00U) <<  8) |
					 (((uint32_t) (val) & (uint32_t) 0x00FF0000U) >>  8) |
					 (((uint32_t) (val) & (uint32_t) 0xFF000000U) >> 24));
}

uint32_t swapEndian( uint32_t val )
{
	return (uint32_t)((((uint32_t) (val) & (uint32_t) 0x000000FFU) << 24) |
					 (((uint32_t) (val) & (uint32_t) 0x0000FF00U) <<  8) |
					 (((uint32_t) (val) & (uint32_t) 0x00FF0000U) >>  8) |
					 (((uint32_t) (val) & (uint32_t) 0xFF000000U) >> 24));
}

float swapEndian( float val )
{
	uint32_t temp = swapEndian( * reinterpret_cast<uint32_t*>( &val ) );
	return *(reinterpret_cast<float*>( &temp ) );
}

double swapEndian( double val )
{
	union {
		double d;
		struct {
			uint32_t a;
			uint32_t b;
		} i;
	} s1, s2;
	s1.d = val;
	s2.i.a = swapEndian( s1.i.b );
	s2.i.b = swapEndian( s1.i.a	);
	return s2.d;
}

void swapEndianBlock( uint16_t *blockPtr, size_t blockSizeInBytes )
{
	size_t blockSize = blockSizeInBytes / sizeof(uint16_t);

	for( size_t b = 0; b < blockSize; b++ ) {
		*blockPtr = swapEndian( *blockPtr );
		blockPtr++;
	}
}

void swapEndianBlock( float *blockPtr, size_t blockSizeInBytes )
{
	size_t blockSize = blockSizeInBytes / sizeof(float);

	for( size_t b = 0; b < blockSize; b++ ) {
		*(reinterpret_cast<uint32_t*>(blockPtr)) = swapEndian( *(reinterpret_cast<uint32_t*>(blockPtr)) );
		blockPtr++;
	}
}

} // namespace cinder

