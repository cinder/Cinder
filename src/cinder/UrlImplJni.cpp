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

#include "cinder/UrlImplJni.h"
#include "cinder/android/net/UrlLoader.h"

namespace cinder {

IStreamUrlImplJni::IStreamUrlImplJni( const std::string &url, const std::string &user, const std::string &password, const UrlOptions &options )
	: IStreamUrlImpl( user, password, options )
{
	ci::android::net::UrlLoader urlLoader = ci::android::net::UrlLoader();
	mData = urlLoader.loadUrl( url );
}

IStreamUrlImplJni::~IStreamUrlImplJni()
{
}

size_t IStreamUrlImplJni::readDataAvailable( void *dest, size_t maxSize )
{
	size_t numBytesRead = 0;
	if( mData ) {
		size_t readSize = std::min( mData->getSize(), maxSize );
		if( readSize > 0 ) {
			memcpy( dest, (const void *)mData->getData(), readSize );
			numBytesRead = readSize;
		}
	}
	return numBytesRead;
}

void IStreamUrlImplJni::seekAbsolute( off_t absoluteOffset )
{
	mOffset = absoluteOffset;
}

void IStreamUrlImplJni::seekRelative( off_t relativeOffset )
{
	mOffset += relativeOffset;
	mOffset = std::min( mOffset, (off_t)size() );
}

off_t IStreamUrlImplJni::tell() const
{
	return mOffset;
}

off_t IStreamUrlImplJni::size() const
{
	off_t result = mData ? mData->getSize() : 0;
	return result;
}

bool IStreamUrlImplJni::isEof() const
{
	bool result = ( mOffset >= size() );
	return 0;
}

void IStreamUrlImplJni::IORead( void *t, size_t size )
{
	if( ! mData ) {
		return;
	}

	int64_t dataSize = mData->getSize();
	int64_t remaining = dataSize - (int64_t)size;
	remaining = std::max( (int64_t)0, remaining );

	size_t readSize = std::min( size, (size_t)remaining );
	memcpy( t, (const void *)mData->getData(), readSize );
}

} // namespace cinder