/*
 Copyright (c) 2015, The Cinder Project (http://libcinder.org)
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

#include "cinder/Url.h"
#include "cinder/Buffer.h"
#include <emscripten.h>
#include "cinder/Log.h"

namespace cinder {

/**
 * A UrlImpl implementation for Emscripten. 
 * Uses native emscripten function to fetch content from a remote source. 
 */
class IStreamUrlImplEmscripten : public IStreamUrlImpl 
{
  public:
	IStreamUrlImplEmscripten( const std::string &url, const std::string &user, const std::string &password, const UrlOptions &options )
		: IStreamUrlImpl( user, password, options )
	{
        
		int size,error;
        void *tmpBuffer;
        emscripten_wget_data( url.c_str(), &tmpBuffer, &size, &error );

		if( error == 0 )
		{
			// create buffer
			mData = ci::Buffer::create( size );

			// write to BufferRef
			mData->copyFrom( tmpBuffer,size );

			free( tmpBuffer );
		}
		else {
			//CI_LOG_I("IStreamUrlImplEmscripten constructor called - an error occured");
        }
    }

	virtual ~IStreamUrlImplEmscripten() {}

	size_t readDataAvailable( void *dest, size_t maxSize ) override
	{
		size_t numBytesRead = 0;
		if( mData ) 
		{
			size_t readSize = std::min( mData->getSize(), maxSize );
			if( readSize > 0 ) 
			{
				memcpy( dest, (const void *)mData->getData(), readSize );
				numBytesRead = readSize;
			}
		}
		return numBytesRead;
	}

	void seekAbsolute( off_t absoluteOffset ) override {}
	void seekRelative( off_t relativeOffset ) override {}
	off_t tell() const override { return -1; }
	off_t size() const override
	{
		off_t result = mData ? mData->getSize() : 0;
		return result;
	}

	bool isEof() const override { return true; }
	void IORead( void *t, size_t size ) override {}

  private:
	ci::BufferRef 		mData;
	off_t 				mOffset;
};

} // namespace cinder
