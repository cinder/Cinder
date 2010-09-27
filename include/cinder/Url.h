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
#include <iostream>

namespace cinder {

class Url {
 public:
	Url() {}
	//! Constructs a URL from a string. If \a isEscaped is false, automatically calls Url::encode(). Assumes UTF-8 input.
	explicit Url( const std::string &urlString, bool isEscaped = false );
	
	//! Returns the string representation of the URL as std::string. Encoded as UTF-8.
	std::string	str() const { return mStr; }
	//! Returns the string representation of the URL as char*. Encoded as UTF-8.
	const char*	c_str() const { return mStr.c_str(); }

	//! Replaces illegal URL characters as defined by RFC 2396 with their escaped equivalents and returns a copy of \a unescaped. Assumes UTF-8 input.
	static std::string encode( const std::string &unescaped );
		
 private:
	std::string		mStr;
};

inline std::ostream& operator<<( std::ostream &out, const Url &url )
{
	out << url.str();
	return out;
}

//! \cond
// This is an abstract base class for implementing IStreamUrl
class IStreamUrlImpl {
  protected:
	IStreamUrlImpl( const std::string &user, const std::string &password )
		: mUser( user ), mPassword( password ) {}

  public:
	virtual ~IStreamUrlImpl() {}
  
	std::string			getUser() const { return mUser; }
	std::string			getPassword() const { return mPassword; }

	virtual size_t		readDataAvailable( void *dest, size_t maxSize ) = 0;
	virtual void		seekAbsolute( off_t absoluteOffset ) = 0;
	virtual void		seekRelative( off_t relativeOffset ) = 0;
	virtual off_t		tell() const = 0;
	virtual off_t		size() const = 0;
	
	virtual bool		isEof() const = 0;
	virtual void		IORead( void *t, size_t size ) = 0;
	
  protected:
	const std::string		mUser, mPassword;
};
//! \endcond

//! A pointer to an instance of an IStreamUrl. Can be created using IStreamUrl::createRef()
typedef shared_ptr<class IStreamUrl>	IStreamUrlRef;

/** \warning IStreamUrl does not currently support proper random access **/
class IStreamUrl : public IStream {
  public:
	//! Creates a new IStreamUrlRef from the Url \a url with an optional login and password
	static IStreamUrlRef	createRef( const std::string &url, const std::string &user = "", const std::string &password = "" );

	virtual size_t		readDataAvailable( void *dest, size_t maxSize ) { return mImpl->readDataAvailable( dest, maxSize ); }
	virtual void		seekAbsolute( off_t absoluteOffset ) { return mImpl->seekAbsolute( absoluteOffset ); }
	virtual void		seekRelative( off_t relativeOffset ) { return mImpl->seekRelative( relativeOffset ); }
	virtual off_t		tell() const { return mImpl->tell(); }
	virtual off_t		size() const { return mImpl->size(); }
	
	virtual bool		isEof() const { return mImpl->isEof(); }

	std::string			getUser() const { return mImpl->getUser(); }
	std::string			getPassword() const { return mImpl->getPassword(); }

  protected:
	IStreamUrl( const std::string &url, const std::string &user, const std::string &password );

	virtual void		IORead( void *t, size_t size ) { mImpl->IORead( t, size ); }
	//! IStreamURL does not yet support writing
	virtual void		IOWrite( const void *t, size_t size ) { throw std::exception(); }
	
	shared_ptr<IStreamUrlImpl>	mImpl;
};

IStreamUrlRef		loadUrlStream( const Url &url );
IStreamUrlRef		loadUrlStream( const std::string &url, const std::string &user = "", const std::string &password = "" );

} // namespace cinder
