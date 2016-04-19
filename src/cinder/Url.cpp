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
#if defined( CINDER_MSW_DESKTOP )
	#include <Shlwapi.h>
	#include "cinder/UrlImplWinInet.h"
	typedef cinder::IStreamUrlImplWinInet	IStreamUrlPlatformImpl;
#elif defined( CINDER_COCOA )
	#include <CoreFoundation/CoreFoundation.h>
	#include "cinder/cocoa/CinderCocoa.h"
	#include "cinder/UrlImplCocoa.h"
	typedef cinder::IStreamUrlImplCocoa		IStreamUrlPlatformImpl;
#elif defined( CINDER_UWP )
	#include "cinder/winrt/WinRTUtils.h"
	#include "cinder/msw/CinderMsw.h"
	#include <wrl/client.h>
	#include <agile.h>
	using namespace Windows::Storage;
	using namespace Windows::System;
#elif defined( CINDER_ANDROID ) 
	#include "cinder/UrlImplJni.h"
	typedef cinder::IStreamUrlImplJni		IStreamUrlPlatformImpl;
#elif defined( CINDER_LINUX ) 
	#include "cinder/UrlImplCurl.h"
	typedef cinder::IStreamUrlImplCurl		IStreamUrlPlatformImpl;
#else
	#include "cinder/UrlImpl.h"
	typedef cinder::IStreamUrlImplCurl		IStreamUrlPlatformImpl;
#endif

namespace cinder {

#if defined( CINDER_ANDROID )
std::string char2hex( char dec )
{
	char dig1 = (dec&0xF0)>>4;
	char dig2 = (dec&0x0F);
	if ( 0<= dig1 && dig1<= 9) dig1 += 48;    //0,48 in ascii
	if (10<= dig1 && dig1<=15) dig1 += 65-10; //A,65 in ascii
	if ( 0<= dig2 && dig2<= 9) dig2 += 48;
	if (10<= dig2 && dig2<=15) dig2 += 65-10;

    std::string r;
	r.append( &dig1, 1 );
	r.append( &dig2, 1 );
	return r;
}

std::string urlencode( const std::string &c )
{
    std::string escaped;
	int max = c.length();
	for( int i = 0; i < max; ++i ) {
		if( (48 <= c[i] && c[i] <= 57 ) || //0-9
			(65 <= c[i] && c[i] <= 90 ) || //ABC...XYZ
			(97 <= c[i] && c[i] <= 122) || //abc...xyz
			(c[i]=='~' || c[i]=='-' || c[i]=='_' || c[i]=='.')
		)
		{
std::cout << "Appending: " << std::string( &c[i], 1 ) << std::endl; 			
			escaped.append( &c[i], 1 );
		}
		else
		{
			escaped.append( "%" );
			escaped.append( char2hex( c[i] ) ); //converts char 255 to string "FF"
		}
	}
	return escaped;
}	
#endif	


//////////////////////////////////////////////////////////////////////////////////////////////////////
// Url
Url::Url( const std::string &urlString, bool isEscaped )
	: mStr( isEscaped ? urlString : encode( urlString ) )
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
#elif defined( CINDER_MSW_DESKTOP )
	char16_t buffer[4096];
	DWORD bufferSize = 4096;
	std::u16string wideUnescaped = toUtf16( unescaped );
	UrlEscape( (wchar_t*)wideUnescaped.c_str(), (wchar_t*)buffer, &bufferSize, 0 );
	return toUtf8( buffer );
#elif defined( CINDER_UWP )
	std::wstring urlStr = msw::toWideString( unescaped );
	auto uri = ref new Windows::Foundation::Uri(ref new Platform::String(urlStr.c_str()));
	return msw::toUtf8String( std::wstring( uri->AbsoluteCanonicalUri->Data()));
#elif defined( CINDER_ANDROID )
	std::string result = urlencode( unescaped );
	return result;
#elif defined( CINDER_LINUX )
	// Curl does not seem to agree with encoded URIs.
	return unescaped;
#endif	
}

#if ! defined( CINDER_UWP )

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
	: Exception( message ), mStatusCode( code )
{
}

} // namespace cinder
