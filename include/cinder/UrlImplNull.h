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

namespace cinder {

class IStreamUrlImplNull : public IStreamUrlImpl {
  public:
	IStreamUrlImplNull( const std::string &url, const std::string &user, const std::string &password, const UrlOptions &options ) 
		: IStreamUrlImpl( user, password, options ) {}
	virtual ~IStreamUrlImplNull() {}

	virtual size_t		readDataAvailable( void *dest, size_t maxSize ) { return 0; }
	virtual void		seekAbsolute( off_t absoluteOffset ) {}
	virtual void		seekRelative( off_t relativeOffset ) {}
	virtual off_t		tell() const { return -1; }
	virtual off_t		size() const { return -1; }
	
	virtual bool		isEof() const { return true; }
	virtual void		IORead( void *t, size_t size ) {}

private:
	ci::BufferRef 		mData;
	off_t 				mOffset;	
};

} // namespace cinder
