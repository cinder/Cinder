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

#include "cinder/Url.h"
#include "cinder/DataSource.h"
#if defined( CINDER_MSW )
	#include "cinder/UrlImplWinInet.h"
	typedef cinder::IStreamUrlImplWinInet	IStreamUrlPlatformImpl;
#elif defined( CINDER_COCOA )
	#include "cinder/UrlImplCocoa.h"
	typedef cinder::IStreamUrlImplCocoa		IStreamUrlPlatformImpl;
#else
	#include "cinder/UrlImplCurl.h"
	typedef cinder::IStreamUrlImplCurl		IStreamUrlPlatformImpl;
#endif

namespace cinder {

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Url
Url::Url( const std::string &urlString )
	: mStr( urlString )
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// IStreamUrl
IStreamUrlRef IStreamUrl::createRef( const std::string &url, const std::string &user, const std::string &password )
{
	return IStreamUrlRef( new IStreamUrl( url, user, password ) );
}

IStreamUrl::IStreamUrl( const std::string &url, const std::string &user, const std::string &password )
	: IStream()
{
	setFileName( url );
	mImpl = shared_ptr<IStreamUrlImpl>( new IStreamUrlPlatformImpl( url, user, password ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// loadUrl
IStreamUrlRef loadUrlStream( const Url &url )
{
	try {
		IStreamUrlRef result = IStreamUrl::createRef( url.str(), "", "" );
		return result;
	}
	catch( ... ) {
		return IStreamUrlRef();
	}
}

IStreamUrlRef loadUrlStream( const std::string &url, const std::string &user, const std::string &password )
{
	try {
		IStreamUrlRef result = IStreamUrl::createRef( url, user, password );
		return result;
	}
	catch( ... ) {
		return IStreamUrlRef();
	}
}

} // namespace cinder
