/*
 Copyright (c) 2010, The Cinder Project (http://libcinder.org)
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

#include "cinder/UrlImplWinInet.h"
#include "cinder/Utilities.h"
#include "cinder/Unicode.h"

#include <Windows.h>
#include <Wininet.h>
#include <Strsafe.h>
#pragma comment( lib, "wininet.lib" )

namespace cinder {

namespace { static const WCHAR AGENT_NAME[] = L"libcinder"; }

// Call InternetCloseHandle, testing for NULL. Designed to be the deallocator for shared_ptr's
void safeInternetCloseHandle( HINTERNET hInternet )
{
	if( hInternet )
		::InternetCloseHandle( hInternet );
}

void testAndThrowHttpStatus( HINTERNET hInternet )
{
	DWORD status;
	DWORD statusSize( sizeof(status) );
	::HttpQueryInfo( hInternet, HTTP_QUERY_FLAG_NUMBER|HTTP_QUERY_STATUS_CODE,
					&status, &statusSize, NULL );
	if( status >= 400 ) {
		throw UrlLoadExc( status, "HTTP Server Error" );
	}
}

IStreamUrlImplWinInet::IStreamUrlImplWinInet( const std::string &url, const std::string &user, const std::string &password, const UrlOptions &options )
	: IStreamUrlImpl( user, password, options ), mIsFinished( false ), mBuffer( 0 ), mBufferFileOffset( 0 )
{
	std::u16string wideUrl = toUtf16( url );

	// we need to break the URL up into its constituent parts so we can choose a scheme
	URL_COMPONENTS urlComponents;
	::memset( &urlComponents, 0, sizeof(urlComponents) );
	urlComponents.dwStructSize = sizeof(urlComponents);
	urlComponents.dwSchemeLength = 1;
	urlComponents.dwHostNameLength = 1;
	urlComponents.dwUrlPathLength = 1;
	BOOL success = ::InternetCrackUrl( (wchar_t*)wideUrl.c_str(), 0, 0, &urlComponents );
	if( ! success )
		throw StreamExc();

	// TODO this should be made safe against buffer overflows
	WCHAR host[1024], path[2048];
	memcpy( host, urlComponents.lpszHostName, urlComponents.dwHostNameLength * sizeof(WCHAR) );
	host[urlComponents.dwHostNameLength] = 0;
	memcpy( path, urlComponents.lpszUrlPath, urlComponents.dwUrlPathLength * sizeof(WCHAR) );
	path[urlComponents.dwUrlPathLength] = 0;	

	// make sure this a scheme we know about - HTTP(S) or FTP
	switch( urlComponents.nScheme ) {
		case INTERNET_SCHEME_HTTP: case INTERNET_SCHEME_HTTPS: case INTERNET_SCHEME_FTP: break;
		default: throw StreamExc();
	}

	mSession = std::shared_ptr<void>( ::InternetOpen( AGENT_NAME, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 ), safeInternetCloseHandle );
	if( ! mSession )
		throw StreamExc();

	std::u16string wideUser = toUtf16( user );
	std::u16string widePassword = toUtf16( password );
    
    //check for HTTP and HTTPS here because they both require the same flag in InternetConnect()
	if( ( urlComponents.nScheme == INTERNET_SCHEME_HTTP ) ||
       ( urlComponents.nScheme == INTERNET_SCHEME_HTTPS ) ) {
	mConnection = std::shared_ptr<void>( ::InternetConnect( mSession.get(), host, urlComponents.nPort, (wchar_t*)((wideUser.empty()) ? NULL : wideUser.c_str()), (wchar_t*)((widePassword.empty()) ? NULL : widePassword.c_str()), INTERNET_SERVICE_HTTP, 0, NULL ),
										safeInternetCloseHandle );
    }else{
        //otherwise we just want to take our best shot at the Scheme type.
        mConnection = std::shared_ptr<void>( ::InternetConnect( mSession.get(), host, urlComponents.nPort, (wchar_t*)((wideUser.empty()) ? NULL : wideUser.c_str()), (wchar_t*)((widePassword.empty()) ? NULL : widePassword.c_str()), urlComponents.nScheme, 0, NULL ),
                                            safeInternetCloseHandle );
    }
	if( ! mConnection )
		throw StreamExc();
    //http and https cases broken out incase someone wishes to modify connection based off of type.
    //it is wrong to group http with https.
    
	unsigned long timeoutMillis = static_cast<unsigned long>( options.getTimeout() * 1000 );
	::InternetSetOptionW( mConnection.get(), INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMillis, sizeof(unsigned long) );
	::InternetSetOptionW( mConnection.get(), INTERNET_OPTION_CONNECT_TIMEOUT, &timeoutMillis, sizeof(unsigned long) );
	::InternetSetOptionW( mConnection.get(), INTERNET_OPTION_SEND_TIMEOUT, &timeoutMillis, sizeof(unsigned long) );

    //http
	DWORD flags = INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES;
	if( options.getIgnoreCache() )
		flags |= INTERNET_FLAG_RELOAD;
	if( urlComponents.nScheme == INTERNET_SCHEME_HTTP ) {
		static LPCTSTR lpszAcceptTypes[] = { L"*/*", NULL };

		mRequest = std::shared_ptr<void>( ::HttpOpenRequest( mConnection.get(), L"GET", path, NULL, NULL, lpszAcceptTypes,
                                                            flags, NULL ),
                                         safeInternetCloseHandle );
		if( ! mRequest )
			throw UrlLoadExc( 0, "Unknown URL load error" );
		BOOL success = ::HttpSendRequest( mRequest.get(), NULL, 0, NULL, 0);
		if( ! success )
			throw UrlLoadExc( 0, "Unknown URL load error" );
		testAndThrowHttpStatus( mRequest.get() );
	}
    //https
	else if(urlComponents.nScheme == INTERNET_SCHEME_HTTPS ) {
		static LPCTSTR lpszAcceptTypes[] = { L"*/*", NULL };
		mRequest = std::shared_ptr<void>( ::HttpOpenRequest( mConnection.get(), L"GET", path, NULL, NULL, lpszAcceptTypes,
											flags | INTERNET_FLAG_SECURE, NULL ),
											safeInternetCloseHandle );
		if( ! mRequest )
			throw UrlLoadExc( 0, "Unknown URL load error" );
		BOOL success = ::HttpSendRequest( mRequest.get(), NULL, 0, NULL, 0);
		if( ! success )
			throw UrlLoadExc( 0, "Unknown URL load error" );
		testAndThrowHttpStatus( mRequest.get() );
	}
    //ftp
	else if( urlComponents.nScheme == INTERNET_SCHEME_FTP ) {
		mRequest = std::shared_ptr<void>( ::FtpOpenFile( mConnection.get(), path, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, NULL ),
										safeInternetCloseHandle );
			if( ! mRequest )
				throw StreamExc();
	}

	mBuffer.resize(DEFAULT_BUFFER_SIZE);
	mBufferOffset = 0;
	mBufferedBytes = 0;
	mBufferFileOffset = 0;
}

IStreamUrlImplWinInet::~IStreamUrlImplWinInet()
{
}

bool IStreamUrlImplWinInet::isEof() const
{
	return ( mBufferedBytes - mBufferOffset == 0 ) && ( mIsFinished );
}

void IStreamUrlImplWinInet::seekRelative( off_t relativeOffset )
{
	// if this move stays inside the current buffer, we're good
	if( ( mBufferOffset + relativeOffset >= 0 ) && ( mBufferOffset + relativeOffset < mBufferedBytes ) ) {
		mBufferOffset += relativeOffset;
		return;
	}
	else if( relativeOffset < 0 ) {	// if we're moving backwards out of the buffer, we have to reset
		throw StreamExc(); // need to implement this
	}
	else { // moving forward off the end of the buffer - keep buffering til we're in range
		throw StreamExc(); // need to implement this		
	}
}

void IStreamUrlImplWinInet::seekAbsolute( off_t absoluteOffset )
{
	seekRelative( absoluteOffset - ( mBufferFileOffset + mBufferOffset ) );
}

off_t IStreamUrlImplWinInet::tell() const
{
	return mBufferFileOffset + mBufferOffset;
}

off_t IStreamUrlImplWinInet::size() const
{
	return 0; // we don't know the size
}

void IStreamUrlImplWinInet::fillBuffer( int wantBytes ) const
{
	// we've already got all the data we need
	if( bufferDataRemaining() > wantBytes )
		return;
	
	// if we want more bytes than will fit in the rest of the buffer, let's make some room
	if( wantBytes > (int)mBuffer.size() - mBufferedBytes ) {
		memmove( mBuffer.data(), mBuffer.data() + mBufferOffset, mBufferedBytes - mBufferOffset );
		mBufferedBytes -= mBufferOffset;
		mBufferFileOffset += mBufferOffset;
		mBufferOffset = 0;
	}
	
	// now if we've made all the room there is to make, and we still aren't big enough, reallocate
	if( wantBytes > (int)mBuffer.size() - mBufferedBytes ) {
		mBuffer.resize( mBufferedBytes + wantBytes );
	}
	
	do {
		::DWORD bytesAvailable, bytesRead;
		if( ! ::InternetQueryDataAvailable( mRequest.get(), &bytesAvailable, 0, 0 ) )
			throw StreamExc();
		
		if( bytesAvailable == 0 ) {
			mIsFinished = true;
			break;
		}
		
		::DWORD bytesToRead = std::min<int>( bytesAvailable, wantBytes );
		if( ! ::InternetReadFile( mRequest.get(), mBuffer.data() + mBufferedBytes, bytesToRead, &bytesRead ) )
			throw StreamExc();
		mBufferedBytes += bytesRead;
		wantBytes -= bytesRead;
		if( wantBytes < 0 )
			wantBytes = 0;
	} while( wantBytes );
}

void IStreamUrlImplWinInet::IORead( void *dest, size_t size )
{
	fillBuffer( static_cast<int>( size ) );

	// check if theres data in the buffer - if not fillBuffer() either errored or EOF
	if( bufferDataRemaining() < (off_t)size )
		throw StreamExc();

	memcpy( dest, mBuffer.data() + mBufferOffset, size );
	mBufferOffset += static_cast<int>( size );
}

size_t IStreamUrlImplWinInet::readDataAvailable( void *dest, size_t maxSize )
{
	fillBuffer( static_cast<int>( maxSize ) );

	if( bufferDataRemaining() < (off_t)maxSize )
		maxSize = bufferDataRemaining();

	memcpy( dest, mBuffer.data() + mBufferOffset, maxSize );

	mBufferOffset += static_cast<int>( maxSize );
	return maxSize;
}

} // namespace cinder