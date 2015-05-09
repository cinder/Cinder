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

#include "cinder/Url.h"
#include "cinder/DataSource.h"
#include "cinder/Utilities.h"
#include "cinder/Unicode.h"
#if defined( CINDER_MSW )
	#include <Shlwapi.h>
	#include "cinder/UrlImplWinInet.h"
	typedef cinder::IStreamUrlImplWinInet	IStreamUrlPlatformImpl;
#elif defined( CINDER_COCOA )
	#include "cinder/cocoa/CinderCocoa.h"
	#include "cinder/UrlImplCocoa.h"
	typedef cinder::IStreamUrlImplCocoa		IStreamUrlPlatformImpl;
#elif defined( CINDER_WINRT )
	#include "cinder/WinRTUtils.h"
	#include "cinder/msw/CinderMsw.h"
	#include <wrl/client.h>
	#include <agile.h>
	using namespace Windows::Storage;
	using namespace Windows::System;
#else
	#include "cinder/UrlImplCurl.h"
	typedef cinder::IStreamUrlImplCurl		IStreamUrlPlatformImpl;
#endif

namespace cinder {

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Url
Url::Url( const std::string &urlString, bool isEscaped )
	: mStr( isEscaped ? 
		urlString : encode( urlString ) )
{
}

std::string Url::encode( const std::string &unescaped )
{
#if defined( CINDER_COCOA )
	cocoa::SafeCfString unescapedStr = cocoa::createSafeCfString( unescaped );
	CFStringRef escaped = ::CFURLCreateStringByAddingPercentEscapes( kCFAllocatorDefault, unescapedStr.get(), NULL, NULL, kCFStringEncodingUTF8 );
	std::string result = cocoa::convertCfString( escaped );
	::CFRelease( escaped );
	return result;
#elif defined( CINDER_MSW )
	char16_t buffer[4096];
	DWORD bufferSize = 4096;
	std::u16string wideUnescaped = toUtf16( unescaped );
	UrlEscape( (wchar_t*)wideUnescaped.c_str(), (wchar_t*)buffer, &bufferSize, 0 );
	return toUtf8( buffer );
#elif defined( CINDER_WINRT )
	std::wstring urlStr = msw::toWideString( unescaped );
	auto uri = ref new Windows::Foundation::Uri(ref new Platform::String(urlStr.c_str()));
	return msw::toUtf8String( std::wstring( uri->AbsoluteCanonicalUri->Data()));
#endif	
}

#if !defined( CINDER_WINRT)

//////////////////////////////////////////////////////////////////////////////////////////////////////
// IStreamUrl
IStreamUrlRef IStreamUrl::create( const Url &url, const std::string &user, const std::string &password, const UrlOptions &options )
{
	return IStreamUrlRef( new IStreamUrl( url.str(), user, password, options ) );
}

IStreamUrl::IStreamUrl( const std::string &url, const std::string &user, const std::string &password, const UrlOptions &options )
	: IStreamCinder()
{
	setFileName( url );
	mImpl = std::shared_ptr<IStreamUrlImpl>( new IStreamUrlPlatformImpl( url, user, password, options ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// loadUrl
IStreamUrlRef loadUrlStream( const Url &url, const UrlOptions &options )
{
	return IStreamUrl::create( url, "", "", options );
}

IStreamUrlRef loadUrlStream( const std::string &url, const std::string &user, const std::string &password, const UrlOptions &options )
{
	return IStreamUrl::create( Url( url ), user, password, options );
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
// UrlLoadExc
UrlLoadExc::UrlLoadExc( int code, const std::string &message )
	: mMessage( message ), mStatusCode( code )
{
}

} // namespace cinder
