/*
 Copyright (c) 2010, The Barbarian Group
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

#include "cinder/Cinder.h"
#include "cinder/Stream.h"

typedef void CURL;
typedef void CURLM;

namespace cinder {

class Url {
 public:
	Url() {}
	explicit Url( const std::string &urlString );
	
	std::string	str() const { return mStr; }
	const char*	c_str() const { return mStr.c_str(); }
		
 private:
	std::string		mStr;
};

//! A pointer to an instance of an IStreamUrl. Can be created using IStreamUrl::createRef()
typedef shared_ptr<class IStreamUrl>	IStreamUrlRef;

/** \warning IStreamUrl does not currently support proper random access **/
class IStreamUrl : public IStream {
  public:
	//! Creates a new IStreamUrlRef from the memory pointed to by \a data which is of size \a size bytes.
	static IStreamUrlRef	createRef( const std::string &url, const std::string &user, const std::string &password );
	~IStreamUrl();

	virtual size_t		readDataAvailable( void *dest, size_t maxSize );
	virtual void		seekAbsolute( off_t absoluteOffset );
	virtual void		seekRelative( off_t relativeOffset );
	virtual off_t		tell() const { return mBufferFileOffset + mBufferOffset; }
	virtual off_t		size() const;
	
	virtual bool		isEof() const;

	std::string			getUser() const { return mUser; }
	std::string			getPassword() const { return mPassword; }
	long				getResponseCode() const;
	std::string			getEffectiveUrl() const;
	

  protected:
	IStreamUrl( const std::string &url, const std::string &user, const std::string &password );

	virtual void		IORead( void *t, size_t size );
	virtual void		IOWrite( const void *t, size_t size ) {}

	int					bufferRemaining() const { return mBufferedBytes - mBufferOffset; }
	void				fillBuffer( int wantBytes ) const;
	
	static size_t		writeCallback( char *buffer, size_t size, size_t nitems, void *userp ); 
	
  private:
	CURL					*mCurl;
	CURLM					*mMulti;
	const std::string		mUser, mPassword;
	std::string				mUserColonPassword;
	
	mutable int still_running;				// Is background url fetch still in progress
	mutable bool			mStartedRead;

	mutable off_t			mSize;
	mutable bool			mSizeCached;
	mutable long			mResponseCode;
	mutable char			*mEffectiveUrl;
	
	mutable uint8_t		*mBuffer;
	mutable int			mBufferSize;
	mutable int			mBufferOffset, mBufferedBytes;
	mutable off_t		mBufferFileOffset;	// where in the file the buffer starts
	static const int	DEFAULT_BUFFER_SIZE = 4096;
};

IStreamUrlRef		loadUrlStream( const Url &url );
IStreamUrlRef		loadUrlStream( const std::string &url, const std::string &user = "", const std::string &password = "" );

} // namespace cinder
