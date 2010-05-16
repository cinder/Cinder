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

#include <Windows.h>
#include <Wininet.h>
#include <Strsafe.h>
#pragma comment( lib, "wininet.lib" )

namespace cinder {

namespace { static const WCHAR AGENT_NAME[] = L"libcinder.org"; }

IStreamUrlImplWinInet::IStreamUrlImplWinInet( const std::string &url, const std::string &user, const std::string &password )
	: IStreamUrlImpl( user, password ), mIsFinished( false ), mBuffer( 0 ), mBufferFileOffset( 0 )
{
	mSession = shared_ptr<void>( ::InternetOpen( AGENT_NAME, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 ), ::InternetCloseHandle );
	if( ! mSession )
		throw StreamExc();

	// We don't currently handle user/pass because it will require going through InternetConnect instead of InternetOpenUrl.
	
/*	if( ! user.empty() ) {
		std::wstring wideUser = toUtf16( user );
		if( ! ::InternetSetOption( mSession.get(), INTERNET_OPTION_USERNAME, (void*)wideUser.c_str(), wideUser.length()+1 ) ) {
			DWORD err = GetLastError();	
			throw StreamExc();
		}
	}

	if( ! password.empty() ) {
		std::wstring widePassword = toUtf16( password );
		if( ! ::InternetSetOption( mSession.get(), INTERNET_OPTION_PASSWORD, (void*)widePassword.c_str(), widePassword.length()+1 ) )
			throw StreamExc();
	}*/

	std::wstring wideUrl = toUtf16( url );
	mConnection = shared_ptr<void>( ::InternetOpenUrl( mSession.get(), wideUrl.c_str(), NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_RELOAD, reinterpret_cast<DWORD_PTR>( this ) ), ::InternetCloseHandle );
	if( ! mConnection )
		throw StreamExc();

	mBufferSize = DEFAULT_BUFFER_SIZE;
	mBuffer = (uint8_t*)malloc( mBufferSize );
	mBufferOffset = 0;
	mBufferedBytes = 0;
	mBufferFileOffset = 0;
}

IStreamUrlImplWinInet::~IStreamUrlImplWinInet()
{
	if( mBuffer )
		free( mBuffer );
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
	return 0;
}

void IStreamUrlImplWinInet::fillBuffer( int wantBytes ) const
{
	// we've already got all the data we need
	if( bufferDataRemaining() > wantBytes )
		return;
	
	// if we want more bytes than will fit in the rest of the buffer, let's make some room
	if( mBufferSize - mBufferedBytes < wantBytes ) {
		int bytesCulled = mBufferOffset;
		memmove( mBuffer, &mBuffer[mBufferOffset], mBufferedBytes - bytesCulled );
		mBufferedBytes -= bytesCulled;
		mBufferOffset = 0;
		mBufferFileOffset += bytesCulled;
	}
	
	// now if we've made all the room there is to make, and we still aren't big enough, reallocate
	if( wantBytes > mBufferSize - mBufferedBytes ) {
		// not enough space in buffer
		int oldBufferSize = mBufferSize;
		while( mBufferSize - mBufferedBytes < wantBytes )
			mBufferSize *= 2;
		uint8_t *newBuff = reinterpret_cast<uint8_t*>( realloc( mBuffer, mBufferSize ) );
		if( ! newBuff ) {
			throw StreamExc();
		}
		else {
			// realloc suceeded increase buffer size
			mBuffer = newBuff;
		}
	}
	
	do {
		::DWORD bytesAvailable, bytesToRead, bytesRead;
		if( ! ::InternetQueryDataAvailable( mConnection.get(), &bytesAvailable, 0, 0 ) )
			throw StreamExc();
		
		if( bytesAvailable == 0 ) {
			mIsFinished = true;
			break;
		}
		
		bytesToRead = std::min<int>( bytesAvailable, wantBytes );
		if( ! ::InternetReadFile( mConnection.get(), mBuffer + mBufferedBytes, bytesToRead, &bytesRead ) )
			throw StreamExc();
		mBufferedBytes += bytesRead;
		wantBytes -= bytesRead;
		if( wantBytes < 0 )
			wantBytes = 0;
	} while( wantBytes );
}

void IStreamUrlImplWinInet::IORead( void *dest, size_t size )
{
	fillBuffer( size );
	
	// check if theres data in the buffer - if not fillBuffer() either errored or EOF
	if( bufferDataRemaining() < (off_t)size )
		throw StreamExc();

	memcpy( dest, mBuffer + mBufferOffset, size );
	mBufferOffset += size;
}

size_t IStreamUrlImplWinInet::readDataAvailable( void *dest, size_t maxSize )
{
	fillBuffer( maxSize );
	
	if( bufferDataRemaining() < (off_t)maxSize )
		maxSize = bufferDataRemaining();
		
	memcpy( dest, mBuffer + mBufferOffset, maxSize );
	
	mBufferOffset += maxSize;
	return maxSize;
}

} // namespace cinder