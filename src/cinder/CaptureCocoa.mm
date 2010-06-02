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

#import "cinder/CaptureCocoa.h"
#include "cinder/cocoa/CinderCocoa.h"

static void frameDeallocator( void *refcon );

/*- (void)attributes
{
	NSLog( @"--------Device Attributes--------" );
	for ( id key in [mDevice deviceAttributes] )
	{
		char * readOnly = "Writable";
		if( [mDevice attributeIsReadOnly: key] ) {
			readOnly = "Read Only";
		}
		NSLog( @" %@ : %@ <%s>", key, [[mDevice deviceAttributes] objectForKey:key], readOnly );
	}
	
	
	NSLog( @"--------Device Formats--------" );
	NSArray * deviceFormats = [mDevice formatDescriptions];
	for ( int i = 0; i < [deviceFormats count]; i++  )
	{
		QTFormatDescription * desc = [deviceFormats objectAtIndex: i];
		
		NSLog( @"QuickTime Sample Description: %@", [desc quickTimeSampleDescription] );
		NSLog( @"Media Type: %@", [desc mediaType] );
		NSLog( @"Description: %@", [desc localizedFormatSummary] );
		//NSLog( @"FormatType: %@", [desc formatType] );
		
		
		for ( id key in [desc formatDescriptionAttributes] ) 
		{
			NSLog( @" %@ : %@", key, [desc attributeForKey: key] );
		}
	}
	NSLog(@"------------");
}
*/
@implementation CaptureCocoa

- (void)dealloc 
{
	if( isCapturing ) {
		[self stopCapture];
	}
	
	[mDeviceUniqueId release];
	
	[super dealloc];
}

- (id)initWithDevice:(const cinder::Capture::Device&)device width:(int)width height:(int)height 
{
	self = [super init];

	mDeviceUniqueId = [NSString stringWithUTF8String:device.getUniqueId().c_str()];
	[mDeviceUniqueId retain];
	
	mWidth = width;
	mHeight = height;
	
	return self;
}

- (void)prepareStartCapture
{
	mCaptureSession = [[QTCaptureSession alloc] init];
	
	BOOL success = NO;
	NSError *error;
	
	//if mDevice is NULL, use the default video device to capture from
	QTCaptureDevice *device = nil;
	if( [mDeviceUniqueId length] == 0 ) {
		device = [QTCaptureDevice defaultInputDeviceWithMediaType:QTMediaTypeVideo];
	}
	else {
		device = [QTCaptureDevice deviceWithUniqueID:mDeviceUniqueId];
	}
	
	if( ! device ) {
		throw cinder::CaptureExcInitFail();
	}

	success = [device open:&error];
	if( ! success ) {
		throw cinder::CaptureExcInitFail();
	}
	
	mCaptureDeviceInput = [[QTCaptureDeviceInput alloc] initWithDevice:device];
	success = [mCaptureSession addInput:mCaptureDeviceInput error:&error];
	if( ! success ) {
		throw cinder::CaptureExcInitFail();
	}
	
	// Disable the sound connection
	NSArray *connections = [mCaptureDeviceInput connections];
	for( int i = 0; i < [connections count]; i++ ) {
		QTCaptureConnection *connection = [connections objectAtIndex:i];
		if( [[connection mediaType] isEqualToString:QTMediaTypeSound] ) {
			[connection setEnabled:NO];
		}
	}
	
	//get decompressed video output
	mCaptureDecompressedOutput = [[QTCaptureDecompressedVideoOutput alloc] init];
	success = [mCaptureSession addOutput:mCaptureDecompressedOutput error:&error];
	if( ! success ) {
		throw cinder::CaptureExcInitFail();
	}
	
	[mCaptureDecompressedOutput setDelegate: self];
	
/*	int pixelBufferFormat = cinder::cocoa::getCvPixelFormatTypeFromSurfaceChannelOrder( cinder::SurfaceChannelOrder( mSurfaceChannelOrderCode ) ); 
	if( pixelBufferFormat < 0 ) 
		throw cinder::CaptureExcInvalidChannelOrder(); 	*/
	
	NSDictionary *attributes = [NSDictionary dictionaryWithObjectsAndKeys:
								[NSNumber numberWithDouble:mWidth], (id)kCVPixelBufferWidthKey,
								[NSNumber numberWithDouble:mHeight], (id)kCVPixelBufferHeightKey,
								//10.4: k32ARGBPixelFormat
								//10.5: kCVPixelFormatType_32ARGB
								//[NSNumber numberWithUnsignedInt:pixelBufferFormat], (id)kCVPixelBufferPixelFormatTypeKey,
								[NSNumber numberWithUnsignedInt:kCVPixelFormatType_24RGB], (id)kCVPixelBufferPixelFormatTypeKey,
								nil
								];
	
	[mCaptureDecompressedOutput setPixelBufferAttributes: attributes];
}

- (void)startCapture 
{
	if( isCapturing )
		return; 

	@synchronized( self ) {
		[self prepareStartCapture];
		
		mWorkingPixelBuffer = 0;
		mHasNewFrame = FALSE;
		
		isCapturing = TRUE;
		[mCaptureSession startRunning];
	}
}

- (void)stopCapture
{
	if( ! isCapturing )
		return;

	@synchronized( self ) {
		[mCaptureSession stopRunning];
		[[mCaptureDeviceInput device] close];
		
		[mCaptureSession release];
		[mCaptureDeviceInput release];

		isCapturing = FALSE;
		mHasNewFrame = FALSE;
		
		mCurrentFrame.reset();
		
		if( mWorkingPixelBuffer ) {
			CVBufferRelease( mWorkingPixelBuffer );
			mWorkingPixelBuffer = 0;
		}		
	}
}

- (BOOL)isCapturing
{
	return isCapturing;
}

- (cinder::Surface8u)getCurrentFrame
{
	if( ( ! isCapturing ) || ( ! mWorkingPixelBuffer ) ) {
		return mCurrentFrame;
	}
	
	@synchronized (self) {
		CVPixelBufferLockBaseAddress( mWorkingPixelBuffer, 0 );
		
		uint8_t *data = (uint8_t *)CVPixelBufferGetBaseAddress( mWorkingPixelBuffer );
		mExposedFrameBytesPerRow = CVPixelBufferGetBytesPerRow( mWorkingPixelBuffer );
		mExposedFrameWidth = CVPixelBufferGetWidth( mWorkingPixelBuffer );
		mExposedFrameHeight = CVPixelBufferGetHeight( mWorkingPixelBuffer );

		mCurrentFrame = cinder::Surface8u( data, mExposedFrameWidth, mExposedFrameHeight, mExposedFrameBytesPerRow, cinder::SurfaceChannelOrder::RGB );
		mCurrentFrame.setDeallocator( frameDeallocator, mWorkingPixelBuffer );
		
		// mark the working pixel buffer as empty since we have wrapped it in the current frame
		mWorkingPixelBuffer = 0;
	}
	
	return mCurrentFrame;
}

- (BOOL)checkNewFrame
{
	BOOL result;
	@synchronized (self) {
		result = mHasNewFrame;
		mHasNewFrame = FALSE;
	}
	return result;
}

void frameDeallocator( void *refcon )
{
	CVPixelBufferRef pixelBuffer = reinterpret_cast<CVPixelBufferRef>( refcon );
	CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
	CVBufferRelease( pixelBuffer );
}

- (size_t)getCurrentFrameBytesPerRow
{
	return mExposedFrameBytesPerRow;
}

- (size_t)getCurrentFrameWidth
{
	return mExposedFrameWidth;
}

- (size_t)getCurrentFrameHeight
{
	return mExposedFrameHeight;
}

- (void)captureOutput:(QTCaptureOutput *)captureOutput didOutputVideoFrame:(CVImageBufferRef)videoFrame withSampleBuffer:(QTSampleBuffer *)sampleBuffer fromConnection:(QTCaptureConnection *)connection
{
	@synchronized( self ) {
		if( isCapturing ) {
			// if the last pixel buffer went unclaimed, we'll need to release it
			if( mWorkingPixelBuffer ) {
				CVBufferRelease( mWorkingPixelBuffer );
			}
			
			CVBufferRetain( videoFrame );
		
			mWorkingPixelBuffer = (CVPixelBufferRef)videoFrame;
			mHasNewFrame = TRUE;			
		}
	}	
}

@end
