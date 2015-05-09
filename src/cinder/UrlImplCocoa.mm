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

#include "cinder/UrlImplCocoa.h"

#if defined( CINDER_COCOA_TOUCH )
	#import <UIKit/UIKit.h>
#else
	#import <Cocoa/Cocoa.h>
#endif

@interface IStreamUrlImplCocoaDelegate : NSObject {
	ci::IStreamUrlImplCocoa		*mImpl;
	ci::Url						mUrl;
	std::string					mUser, mPassword;
	
	uint8_t				*mBuffer;
	off_t				mBufferSize;
	off_t				mBufferOffset, mBufferedBytes;
	off_t				mBufferFileOffset;	// where in the file the buffer starts
	off_t				mSize;
	BOOL				mStillConnected;
	BOOL				mResponseReceived;
	BOOL				mDidFail;
	std::string			mErrorString;
	long				mStatusCode;
}

- (id)initWithImpl:(ci::IStreamUrlImplCocoa*)impl url:(ci::Url)url user:(std::string)user password:(std::string)password;

- (void)threadEntry:(id)arg;

// delegate methods for NSURLConnection
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response;
-(void)connection:(NSURLConnection *)connection
		didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *)challenge;
- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data;
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error;
- (void)connectionDidFinishLoading:(NSURLConnection *)connection;

- (off_t)seekRelative:(off_t)relativeOffset;
- (void)seekAbsolute:(off_t)absoluteOffset;
- (off_t)tell;
- (off_t)bufferRemaining;
- (bool)isEof;
- (off_t)getSize;

- (off_t)IoRead:(void*)dest withSize:(size_t)size;
- (void)fillBuffer:(off_t)wantBytes;
- (size_t)readDataAvailable:(void*)dest withSize:(size_t)maxSize;

@end

@implementation IStreamUrlImplCocoaDelegate

- (id)initWithImpl:(ci::IStreamUrlImplCocoa*)impl url:(ci::Url)url user:(std::string)user password:(std::string)password
{
	self = [super init];

	mImpl = impl;
	mUrl = url;
	mUser = user;
	mPassword = password;
	mBufferSize = 4096;
	mBuffer = (uint8_t*)malloc( (size_t)mBufferSize );
	mBufferOffset = 0;
	mBufferedBytes = 0;
	mBufferFileOffset = 0;
	mStillConnected = YES;
	mResponseReceived = NO;
	mDidFail = NO;
	mStatusCode = 0;
	mSize = 0;

	return self;
}

- (void)cleanup
{
	if( mBuffer )
		free( mBuffer );
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
	@synchronized( self ) {
		// we're ignoring this now because sometimes it's incorrect
/*		long long expectedSize = [response expectedContentLength];
		if( expectedSize == NSURLResponseUnknownLength )
			mSize = 0;
		else
			mSize = expectedSize;*/
		
		mSize = 0;
		mResponseReceived = YES;

		if( [response isKindOfClass:[NSHTTPURLResponse class]] ) {
			NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
			mStatusCode = [httpResponse statusCode];
			if( mStatusCode >= 400) {
				mErrorString = "HTTP error: ";
				mErrorString += [[NSHTTPURLResponse localizedStringForStatusCode:mStatusCode] cStringUsingEncoding:NSUTF8StringEncoding];
				mDidFail = YES;
			}
		}
	}
}

- (void)threadEntry:(id)arg
{
	ci::IStreamUrlImplCocoa *impl = ((IStreamUrlImplCocoaDelegate*)self)->mImpl;

	@autoreleasepool {
		NSURLRequestCachePolicy cachePolicy = (impl->getOptions().getIgnoreCache())? NSURLRequestReloadIgnoringLocalCacheData : NSURLRequestUseProtocolCachePolicy;
		NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:[NSString stringWithUTF8String:mUrl.c_str()]]
															   cachePolicy:cachePolicy
														   timeoutInterval:impl->getOptions().getTimeout()];

		NSURLConnection *connection = [[NSURLConnection alloc] initWithRequest:request delegate:self];
		if( connection ) {
			while( mStillConnected ) {
				[[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
			}

/*
			if( ( ! mUser.empty() ) || ( ! mPassword.empty() ) ) {
//				mUserColonPassword = mUser + ":" + mPassword;
			}

			// we fill the buffer just to get things rolling*/
		}
		[connection release];
	}
}

-(void)connection:(NSURLConnection *)connection
		didReceiveAuthenticationChallenge:(NSURLAuthenticationChallenge *)challenge
{
	if( [challenge previousFailureCount] == 0 ) {
		NSURLCredential *newCredential;
		newCredential = [NSURLCredential credentialWithUser:[NSString stringWithUTF8String:mUser.c_str()]
									password:[NSString stringWithUTF8String:mPassword.c_str()]
									persistence:NSURLCredentialPersistenceNone];
		[[challenge sender] useCredential:newCredential forAuthenticationChallenge:challenge];
	}
	else {
		[[challenge sender] cancelAuthenticationChallenge:challenge];
	}
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
	@synchronized( self ) {
		off_t roomInBuffer = mBufferSize - mBufferedBytes;
		off_t size = [data length];	
		if( (off_t)size > roomInBuffer ) {
			// not enough space in buffer
			off_t oldBufferSize = mBufferSize;
			while( mBufferSize - mBufferedBytes <= (off_t)size )
				mBufferSize *= 2;
			uint8_t *newBuff = reinterpret_cast<uint8_t*>( realloc( mBuffer, (size_t)mBufferSize ) );
			if( ! newBuff ) { // allocation failed - just copy the bytes we can fit
				size = [self bufferRemaining];
				mBufferSize = oldBufferSize;
			}
			else {
				// realloc suceeded increase buffer size
				mBuffer = newBuff;
			}
		}

		memcpy( &mBuffer[mBufferedBytes], [data bytes], (size_t)size );
		mBufferedBytes += size;
	}
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	mErrorString = std::string( [[error localizedDescription] cStringUsingEncoding:NSUTF8StringEncoding] );
	mStatusCode = [error code];
	mDidFail = YES;
	mStillConnected = NO;
	mResponseReceived = YES;
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	mStillConnected = NO;
}

- (off_t)tell
{
	return mBufferFileOffset + mBufferOffset;
}

- (off_t)bufferRemaining
{
	return mBufferedBytes - mBufferOffset;
}

- (bool)isEof
{
	@synchronized( self ) {
		if( ! mDidFail )
			return ( mBufferedBytes - mBufferOffset == 0 ) && ( ! mStillConnected );

		mStillConnected = NO;
	}
	
	throw cinder::UrlLoadExc( (int)mStatusCode, mErrorString );
}

- (off_t)getSize
{
	if( ! mResponseReceived ) {
		while( ! mResponseReceived )
			usleep( 10000 );
		off_t result;
		@synchronized( self ) {
			result = mSize;
		}
		return result;
	}
	else
		return mSize;
}

// returns 0 on success
- (off_t)seekRelative:(off_t)relativeOffset
{
	@synchronized( self ) {
		// if this move stays inside the current buffer, we're good
		if( ( mBufferOffset + relativeOffset >= 0 ) && ( mBufferOffset + relativeOffset < mBufferedBytes ) ) {
			mBufferOffset += relativeOffset;
		}
		else if( relativeOffset < 0 ) {	// if we're moving backwards out of the buffer, we have to reset
			return -1; // need to implement this
		}
		else { // moving forward off the end of the buffer - keep buffering til we're in range
			return -1; // need to implement this		
		}
		return 0;
	}
}

- (void)seekAbsolute:(off_t)absoluteOffset
{
	@synchronized( self ) {
		[self seekRelative:absoluteOffset - ( mBufferFileOffset + mBufferOffset )];
	}
}

// returns 0 on success
- (off_t)IoRead:(void*)dest withSize:(size_t)size
{
	[self fillBuffer:size];
	
	@synchronized( self ) {		
		// check if theres data in the buffer - if not fillBuffer() either errored or EOF
		if( [self bufferRemaining] < (off_t)size )
			return -1;

		memcpy( dest, mBuffer + mBufferOffset, size );
		mBufferOffset += size;
	}
	
	return 0;
}

- (void)fillBuffer:(off_t)wantBytes
{
	@synchronized( self ) {
		// do we already have the number of bytes we need, or are we disconnected?
		if( ([self bufferRemaining] >= wantBytes) || ( ! mStillConnected ) )
			return;

		// if we want more bytes than will fit in the rest of the buffer, let's make some room
		if( mBufferSize - mBufferedBytes < wantBytes ) {
			off_t bytesCulled = mBufferOffset;
			memmove( mBuffer, &mBuffer[mBufferOffset], (size_t)(mBufferedBytes - bytesCulled) );
			mBufferedBytes -= bytesCulled;
			mBufferOffset = 0;
			mBufferFileOffset += bytesCulled;
		}
	}

	// wait for the buffer to get filled by the secondary thread
	while( ([self bufferRemaining] < wantBytes) && mStillConnected ) {
		usleep( 10000 );
	}
}

- (size_t)readDataAvailable:(void*)dest withSize:(size_t)maxSize
{
	// this MUST be called outside the @synchronized block
	[self fillBuffer:maxSize];

	@synchronized( self ) {	
		off_t remaining = [self bufferRemaining];
		if( remaining < (off_t)maxSize )
			maxSize = (size_t)remaining;
			
		memcpy( dest, mBuffer + mBufferOffset, maxSize );
		
		mBufferOffset += maxSize;
	}
	return maxSize;
}


@end



namespace cinder {

IStreamUrlImplCocoa::IStreamUrlImplCocoa( const std::string &url, const std::string &user, const std::string &password, const UrlOptions &options )
	: IStreamUrlImpl( user, password, options )
{
	mDelegate = [[IStreamUrlImplCocoaDelegate alloc] initWithImpl:this url:Url(url, true) user:user password:password];
	
	mThread = [[NSThread alloc] initWithTarget:mDelegate
									  selector:@selector(threadEntry:)
                                        object:nil];

	[mThread start];
}

IStreamUrlImplCocoa::~IStreamUrlImplCocoa()
{
	while( ! [mThread isFinished] )
		;

	[mThread release];

	[mDelegate cleanup];
	[mDelegate release];	
}

bool IStreamUrlImplCocoa::isEof() const
{
	return [mDelegate isEof];
}

void IStreamUrlImplCocoa::seekRelative( off_t relativeOffset )
{
	if( [mDelegate seekRelative:relativeOffset] < 0 )
		throw StreamExc();
}

void IStreamUrlImplCocoa::seekAbsolute( off_t absoluteOffset )
{
	[mDelegate seekAbsolute:absoluteOffset];
}

off_t IStreamUrlImplCocoa::tell() const
{
	return [mDelegate tell];
}

off_t IStreamUrlImplCocoa::size() const
{
	return [mDelegate getSize];
}

void IStreamUrlImplCocoa::fillBuffer( int wantBytes ) const
{
	[mDelegate fillBuffer:wantBytes];
}

void IStreamUrlImplCocoa::IORead( void *dest, size_t size )
{
	if( [mDelegate IoRead:dest withSize:size] < 0 )
		throw StreamExc();
}

size_t IStreamUrlImplCocoa::readDataAvailable( void *dest, size_t maxSize )
{
	return [mDelegate readDataAvailable:dest withSize:maxSize];
}

} // namespace cinder