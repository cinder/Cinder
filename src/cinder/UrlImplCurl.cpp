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

#include "cinder/UrlImplCurl.h"
#include <curl/curl.h>

#include <cstring>

namespace cinder {

// -------------------------------------------------------------------------------------------------
// CurlLib
// -------------------------------------------------------------------------------------------------
class CurlLib {
  public:
	CurlLib();
	~CurlLib();

	static CurlLib*		instance();

  private:
	CurlLib( const CurlLib& );
	CurlLib& operator=( const CurlLib& );

	static std::unique_ptr<CurlLib> sInstance;
};

std::unique_ptr<CurlLib> CurlLib::sInstance;

CurlLib::CurlLib()
{
	curl_global_init( CURL_GLOBAL_NOTHING );
}

CurlLib::~CurlLib()
{
	curl_global_cleanup();	
}

CurlLib* CurlLib::instance()
{
	if( ! sInstance ) {
		sInstance.reset( new CurlLib() );
	}

	return sInstance.get();
}


// -------------------------------------------------------------------------------------------------
// IStreamUrlImplCurl
// -------------------------------------------------------------------------------------------------
IStreamUrlImplCurl::IStreamUrlImplCurl( const std::string &url, const std::string &user, const std::string &password, const UrlOptions &options )
	: IStreamUrlImpl( user, password, options ), still_running( 1 ), mSizeCached( false ), mBufferFileOffset( 0 ), mStartedRead( false ),
	mEffectiveUrl( 0 ), mResponseCode( 0 )
{	
	if( ! CurlLib::instance() ) {
		throw StreamExc(); // for some reason the CurlLib isn't initialized
	}
	
	mMulti = curl_multi_init();

	mCurl = curl_easy_init();
	curl_easy_setopt( mCurl, CURLOPT_URL, url.c_str() );
	curl_easy_setopt( mCurl, CURLOPT_WRITEDATA, this );
	curl_easy_setopt( mCurl, CURLOPT_VERBOSE, 0L );
	curl_easy_setopt( mCurl, CURLOPT_FOLLOWLOCATION, 1L );
	curl_easy_setopt( mCurl, CURLOPT_WRITEFUNCTION, IStreamUrlImplCurl::writeCallback );

	if( ( ! mUser.empty() ) || ( ! mPassword.empty() ) ) {
		mUserColonPassword = mUser + ":" + mPassword;
		curl_easy_setopt( mCurl, CURLOPT_USERPWD, mUserColonPassword.c_str() );
		curl_easy_setopt( mCurl, CURLOPT_HTTPAUTH, CURLAUTH_ANY );
	}
		
	curl_multi_add_handle( mMulti, mCurl );

	// we fill the buffer just to get things rolling
	mBufferSize = DEFAULT_BUFFER_SIZE;
	mBuffer = (uint8_t*)malloc( mBufferSize );
	mBufferOffset = 0;
	mBufferedBytes = 0;
	mBufferFileOffset = 0;
//	fillBuffer( Stream::MINIMUM_BUFFER_SIZE );
}

extern "C" {

size_t IStreamUrlImplCurl::writeCallback( char *buffer, size_t size, size_t nitems, void *userp )
{
	cinder::IStreamUrlImplCurl *stream = (cinder::IStreamUrlImplCurl*)userp;
	size *= nitems;

	int roomInBuffer = stream->mBufferSize - stream->mBufferedBytes;

	if( (off_t)size > roomInBuffer ) {
		// not enough space in buffer
		int oldBufferSize = stream->mBufferSize;
		while( stream->mBufferSize - stream->mBufferedBytes <= (off_t)size )
			stream->mBufferSize *= 2;
		uint8_t *newBuff = reinterpret_cast<uint8_t*>( realloc( stream->mBuffer, stream->mBufferSize ) );
		if( ! newBuff ) { // allocation failed - just copy the bytes we can fit
			size = stream->bufferRemaining();
			stream->mBufferSize = oldBufferSize;
		}
		else {
			// realloc suceeded increase buffer size
			stream->mBuffer = newBuff;
		}
	}

	memcpy( &stream->mBuffer[stream->mBufferedBytes], buffer, size );
	stream->mBufferedBytes += size;

	return size;
}

} // extern "C"

IStreamUrlImplCurl::~IStreamUrlImplCurl()
{
	if( mBuffer )
		free( mBuffer );

	curl_multi_remove_handle( mMulti, mCurl );
	curl_easy_cleanup( mCurl );

	if( mMulti ) {
		curl_multi_cleanup( mMulti );
	}
}

bool IStreamUrlImplCurl::isEof() const
{
	return ( mBufferedBytes - mBufferOffset == 0 ) && ( ! still_running );
}

void IStreamUrlImplCurl::seekRelative( off_t relativeOffset )
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

void IStreamUrlImplCurl::seekAbsolute( off_t absoluteOffset )
{
	seekRelative( absoluteOffset - ( mBufferFileOffset + mBufferOffset ) );
}

off_t IStreamUrlImplCurl::tell() const
{
	return mBufferFileOffset + mBufferOffset;
}

off_t IStreamUrlImplCurl::size() const
{
	if( ! mStartedRead )
		fillBuffer( 1 );

	if( mSizeCached )
		return mSize;
	else {
		if( still_running ) {			
			double tempSize = 0;
			CURLcode result = curl_easy_getinfo( mCurl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &tempSize );
			if( ( result == CURLE_OK ) && ( tempSize > 0 ) ) {
				mSize = (off_t)tempSize;
				mSizeCached = true;
			}
			else
				mSize = 0;
		}
		else { // transfer is done, so use CURLINFO_SIZE_DOWNLOAD 
			double tempSize = 0;
			CURLcode result = curl_easy_getinfo( mCurl, CURLINFO_SIZE_DOWNLOAD, &tempSize );
			if( ( result == CURLE_OK ) && ( tempSize > 0 ) ) {
				mSize = (off_t)tempSize;
				mSizeCached = true;
			}
			else
				mSize = 0;			
		}
		return mSize;
	}
}

void IStreamUrlImplCurl::fillBuffer( int wantBytes ) const
{
	// first make sure we've started reading, and do so if not
	if( ! mStartedRead ) {
		while( curl_multi_perform( mMulti, &still_running ) == CURLM_CALL_MULTI_PERFORM );
		if( ( bufferRemaining() == 0 ) && ( ! still_running ) ) {			
			throw StreamExc();
		}
		
		mStartedRead = true;
	}
	
    // only attempt to fill buffer if transactions still running and buffer
    // doesnt exceed required size already
    if( ( ! still_running ) || ( bufferRemaining() >= wantBytes ) )
        return;

	// if we want more bytes than will fit in the rest of the buffer, let's make some room
	if( ( mBufferSize - mBufferedBytes ) < wantBytes ) {
		int bytesCulled = mBufferOffset;
		std::memmove( mBuffer, &mBuffer[mBufferOffset], mBufferedBytes - bytesCulled );
		mBufferedBytes -= bytesCulled;
		mBufferOffset = 0;
		mBufferFileOffset += bytesCulled;
	}

    // attempt to fill buffer
    do {
		fd_set fdread;
		fd_set fdwrite;
		fd_set fdexcep;
		FD_ZERO( &fdread );
		FD_ZERO( &fdwrite );
		FD_ZERO( &fdexcep );

		struct timeval timeout;
		// set a suitable timeout to fail on
		timeout.tv_sec  = 60; // 1 minute
		timeout.tv_usec = 0;

		long curl_timeout = -1;
		curl_multi_timeout( mMulti, &curl_timeout );
		if( curl_timeout >= 0 ) {
			timeout.tv_sec = curl_timeout / 1000;
			if( timeout.tv_sec > 1 ) {
				timeout.tv_sec = 1;
			}
			else {
				timeout.tv_usec = ( curl_timeout % 1000 ) * 1000;
			}
		}

		// get file descriptors from the transfers
		int maxfd = -1;
		CURLMcode error = curl_multi_fdset( mMulti, &fdread, &fdwrite, &fdexcep, &maxfd );
		if( CURLM_OK != error ) {
			const char* errorStr = curl_multi_strerror( error );
			throw StreamExc( std::string( errorStr ) );
		}

		int rc; 
		if( -1 == maxfd ) {
			struct timeval wait = { 0, 100 * 1000 };
			rc = select( 0, nullptr, nullptr, nullptr, &wait );
		}
		else {
			rc = select( maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout );
		}
		
		switch( rc ) {
			// error
			case -1:
				throw StreamExc();
			break;

			// timeout
			case 0:
			// action
			default:
				// timeout or readable/writable sockets
				// note we *could* be more efficient and not wait for
				// CURLM_CALL_MULTI_PERFORM to clear here and check it on re-entry
				// but that gets messy
				while( CURLM_CALL_MULTI_PERFORM == curl_multi_perform( mMulti, &still_running ) );
			break;
		}
    } while( still_running && ( bufferRemaining() < wantBytes ) );
}

void IStreamUrlImplCurl::IORead( void *dest, size_t size )
{
	fillBuffer( size );
	
	// check if theres data in the buffer - if not fillBuffer() either errored or EOF
	if( bufferRemaining() < (off_t)size )
		throw StreamExc();

	memcpy( dest, mBuffer + mBufferOffset, size );
	mBufferOffset += size;
}

size_t IStreamUrlImplCurl::readDataAvailable( void *dest, size_t maxSize )
{
	fillBuffer( maxSize );
	
	if( bufferRemaining() < (off_t)maxSize )
		maxSize = bufferRemaining();
		
	memcpy( dest, mBuffer + mBufferOffset, maxSize );
	
	mBufferOffset += maxSize;
	return maxSize;
}

/*long IStreamUrlImplCurl::getResponseCode() const
{
	if( ! mStartedRead )
		fillBuffer( 1 );

	if( ! mResponseCode ) {
		curl_easy_getinfo( mCurl, CURLINFO_RESPONSE_CODE, &mResponseCode );
	}
	
	return mResponseCode;
}

std::string	IStreamUrlImplCurl::getEffectiveUrl() const
{
	if( ! mStartedRead )
		fillBuffer( 1 );

	if( ! mEffectiveUrl ) {
		curl_easy_getinfo( mCurl, CURLINFO_EFFECTIVE_URL, &mEffectiveUrl ); 
	}
	
	return std::string( mEffectiveUrl );
}*/

} // namespace cinder
