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

#import "cinder/CaptureImplAvFoundation.h"
#include "cinder/cocoa/CinderCocoa.h"
#include <dlfcn.h>

namespace cinder {

CaptureImplAvFoundationDevice::CaptureImplAvFoundationDevice( AVCaptureDevice * device )
	: Capture::Device()
{
	mUniqueId = cocoa::convertNsString( [device uniqueID] );
	mName = cocoa::convertNsString( [device localizedName] );
	mNativeDevice = [device retain];
}

CaptureImplAvFoundationDevice::~CaptureImplAvFoundationDevice()
{
	[mNativeDevice release];
}

bool CaptureImplAvFoundationDevice::checkAvailable() const
{
	return mNativeDevice.connected;
}

bool CaptureImplAvFoundationDevice::isConnected() const
{
	return mNativeDevice.connected;
}

} //namespace

void frameDeallocator( void *refcon )
{
	CVPixelBufferRef pixelBuffer = reinterpret_cast<CVPixelBufferRef>( refcon );
	CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
	CVBufferRelease( pixelBuffer );
}


static std::vector<cinder::Capture::DeviceRef> sDevices;
static BOOL sDevicesEnumerated = false;

@implementation CaptureImplAvFoundation

+ (const std::vector<cinder::Capture::DeviceRef>&)getDevices:(BOOL)forceRefresh
{
	if( sDevicesEnumerated && ( ! forceRefresh ) ) {
		return sDevices;
	}

	sDevices.clear();
	
	Class clsAVCaptureDevice = NSClassFromString(@"AVCaptureDevice");
	if( clsAVCaptureDevice != nil ) {
		NSArray * devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
		for( int i = 0; i < [devices count]; i++ ) {
			AVCaptureDevice * device = [devices objectAtIndex:i];
			sDevices.push_back( cinder::Capture::DeviceRef( new cinder::CaptureImplAvFoundationDevice( device ) ) );
		}
	}
	sDevicesEnumerated = true;
	return sDevices;
}

- (id)initWithDevice:(const cinder::Capture::DeviceRef)device width:(int)width height:(int)height
{
	if( ( self = [super init] ) ) {

		mDevice = device;
		if( mDevice ) {
			mDeviceUniqueId = [NSString stringWithUTF8String:device->getUniqueId().c_str()];
			[mDeviceUniqueId retain];
		}
		
		mIsCapturing = false;
		mWidth = width;
		mHeight = height;
		mHasNewFrame = false;
		mExposedFrameBytesPerRow = 0;
		mExposedFrameWidth = 0;
		mExposedFrameHeight = 0;
	}
	return self;
}

- (void)dealloc
{
	if( mIsCapturing ) {
		[self stopCapture];
	}
	
	[mDeviceUniqueId release];
	
	[super dealloc];
}

- (bool)prepareStartCapture 
{
	// AVFramework is weak-linked to maintain support for iOS 3.2, 
	// so if these symbols don't exist, don't start the capture
	Class clsAVCaptureSession = NSClassFromString(@"AVCaptureSession");
	Class clsAVCaptureDevice = NSClassFromString(@"AVCaptureDevice");
	Class clsAVCaptureDeviceInput = NSClassFromString(@"AVCaptureDeviceInput");
	Class clsAVCaptureVideoDataOutput = NSClassFromString(@"AVCaptureVideoDataOutput");
	Class clsAVCaptureConnection = NSClassFromString(@"AVCaptureConnection");
	if( clsAVCaptureSession == nil || 
		clsAVCaptureDevice == nil || 
		clsAVCaptureDeviceInput == nil || 
		clsAVCaptureVideoDataOutput == nil ||
		clsAVCaptureConnection == nil ) 
	{
		return false;
	}

    NSError * error = nil;

    mSession = [[clsAVCaptureSession alloc] init];

    // Configure the session to produce lower resolution video frames, if your 
    // processing algorithm can cope. We'll specify medium quality for the
    // chosen device.
	mSession.sessionPreset = AVCaptureSessionPresetMedium;

    // Find a suitable AVCaptureDevice
    AVCaptureDevice * device = nil;
	if( ! mDeviceUniqueId ) {
		device = [clsAVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
	} else {
		device = [clsAVCaptureDevice deviceWithUniqueID:mDeviceUniqueId];
	}
	
	if( ! device ) {
		throw cinder::CaptureExcInitFail();
	}

    // Create a device input with the device and add it to the session.
    AVCaptureDeviceInput * input = [clsAVCaptureDeviceInput deviceInputWithDevice:device error:&error];
    if( ! input ) {
        throw cinder::CaptureExcInitFail();
    }
    [mSession addInput:input];

    // Create a VideoDataOutput and add it to the session
    AVCaptureVideoDataOutput * output = [[[clsAVCaptureVideoDataOutput alloc] init] autorelease];
    [mSession addOutput:output];
	
	//adjust connection settings
	/*
	//Testing indicates that at least the 3GS doesn't support video orientation changes
	NSArray * connections = output.connections;
	for( int i = 0; i < [connections count]; i++ ) {
		AVCaptureConnection * connection = [connections objectAtIndex:i];
		
		if( connection.supportsVideoOrientation ) {
			connection.videoOrientation = AVCaptureVideoOrientationPortrait;
		}
	}*/
	
    // Configure your output.
    dispatch_queue_t queue = dispatch_queue_create("myQueue", NULL);
    [output setSampleBufferDelegate:self queue:queue];
    dispatch_release(queue);

    // Specify the pixel format
    output.videoSettings = [NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_32BGRA] forKey:(id)kCVPixelBufferPixelFormatTypeKey];


    // If you wish to cap the frame rate to a known value, such as 15 fps, set 
    // minFrameDuration.
    // output.minFrameDuration = CMTimeMake(1, 15);
	return true;
}

- (void)startCapture 
{
	if( mIsCapturing )
		return; 

	@synchronized( self ) {
		if( [self prepareStartCapture] ) {
			mWorkingPixelBuffer = 0;
			mHasNewFrame = false;
		
			mIsCapturing = true;
			[mSession startRunning];
		}
	}
}

- (void)stopCapture
{
	if( ! mIsCapturing )
		return;

	@synchronized( self ) {
		[mSession stopRunning];
		[mSession release];
		mSession = nil;

		mIsCapturing = false;
		mHasNewFrame = false;
		
		mCurrentFrame.reset();
		
		if( mWorkingPixelBuffer ) {
			CVBufferRelease( mWorkingPixelBuffer );
			mWorkingPixelBuffer = 0;
		}		
	}
}

- (bool)isCapturing
{
	return mIsCapturing;
}

// Delegate routine that is called when a sample buffer was written
- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{ 
    @synchronized( self ) {
		if( mIsCapturing ) {
			// if the last pixel buffer went unclaimed, we'll need to release it
			if( mWorkingPixelBuffer ) {
				CVBufferRelease( mWorkingPixelBuffer );
				mWorkingPixelBuffer = NULL;
			}
			
			CVImageBufferRef videoFrame = CMSampleBufferGetImageBuffer(sampleBuffer);
			// Lock the base address of the pixel buffer
			//CVPixelBufferLockBaseAddress( videoFrame, 0 );
			
			CVBufferRetain( videoFrame );
		
			mWorkingPixelBuffer = (CVPixelBufferRef)videoFrame;
			mHasNewFrame = true;			
		}
	}	
}

- (cinder::Surface8u)getCurrentFrame
{
	if( ( ! mIsCapturing ) || ( ! mWorkingPixelBuffer ) ) {
		return mCurrentFrame;
	}
	
	@synchronized (self) {
		CVPixelBufferLockBaseAddress( mWorkingPixelBuffer, 0 );
		
		uint8_t *data = (uint8_t *)CVPixelBufferGetBaseAddress( mWorkingPixelBuffer );
		mExposedFrameBytesPerRow = CVPixelBufferGetBytesPerRow( mWorkingPixelBuffer );
		mExposedFrameWidth = CVPixelBufferGetWidth( mWorkingPixelBuffer );
		mExposedFrameHeight = CVPixelBufferGetHeight( mWorkingPixelBuffer );

		mCurrentFrame = cinder::Surface8u( data, mExposedFrameWidth, mExposedFrameHeight, mExposedFrameBytesPerRow, cinder::SurfaceChannelOrder::BGRA );
		mCurrentFrame.setDeallocator( frameDeallocator, mWorkingPixelBuffer );
		
		// mark the working pixel buffer as empty since we have wrapped it in the current frame
		mWorkingPixelBuffer = 0;
	}
	
	return mCurrentFrame;
}

- (bool)checkNewFrame
{
	bool result;
	@synchronized (self) {
		result = mHasNewFrame;
		mHasNewFrame = FALSE;
	}
	return result;
}

- (const cinder::Capture::DeviceRef)getDevice {
	return mDevice;
}

- (int32_t)getWidth
{
	return mWidth;
}

- (int32_t)getHeight
{
	return mHeight;
}

- (int32_t)getCurrentFrameBytesPerRow
{
	return mExposedFrameBytesPerRow;
}

- (int32_t)getCurrentFrameWidth
{
	return mExposedFrameWidth;
}

- (int32_t)getCurrentFrameHeight
{
	return mExposedFrameHeight;
}

@end