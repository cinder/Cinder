/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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
#include "cinder/Vector.h"
#import <AVFoundation/AVFoundation.h>

namespace cinder {

CaptureImplAvFoundationDevice::CaptureImplAvFoundationDevice( AVCaptureDevice *device )
	: Capture::Device()
{
	mUniqueId = cocoa::convertNsString( [device uniqueID] );
	mName = cocoa::convertNsString( [device localizedName] );
	mNativeDevice = [device retain];
	mFrontFacing = device.position == AVCaptureDevicePositionFront;
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
	::CVPixelBufferRef pixelBuffer = reinterpret_cast<CVPixelBufferRef>( refcon );
	::CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
	::CVBufferRelease( pixelBuffer );
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
	
	NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
	for( AVCaptureDevice *device in devices ) {
		sDevices.push_back( cinder::Capture::DeviceRef( new cinder::CaptureImplAvFoundationDevice( device ) ) );
	}
	sDevicesEnumerated = true;
	return sDevices;
}

- (id)initWithDevice:(const cinder::Capture::DeviceRef)device width:(int)width height:(int)height
{
	if( ( self = [super init] ) ) {

		mDevice = device;
		if( ! mDevice ) {
			if( [CaptureImplAvFoundation getDevices:NO].empty() )
				throw cinder::CaptureExcInitFail();
			mDevice = [CaptureImplAvFoundation getDevices:NO][0];
		}
		
		mDeviceUniqueId = [NSString stringWithUTF8String:mDevice->getUniqueId().c_str()];
		[mDeviceUniqueId retain];
		
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
	NSError *error = nil;

	mSession = [[AVCaptureSession alloc] init];

	// Find a suitable AVCaptureDevice
	AVCaptureDevice *device = nil;
	if( ! mDeviceUniqueId ) {
		device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
	}
	else {
		device = [AVCaptureDevice deviceWithUniqueID:mDeviceUniqueId];
	}
	
	if( ! device ) {
		throw cinder::CaptureExcInitFail();
	}

	// Create a device input with the device and add it to the session.
	AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];
	if( ! input ) {
		throw cinder::CaptureExcInitFail();
	}
	[mSession addInput:input];

	// Create a VideoDataOutput and add it to the session
	AVCaptureVideoDataOutput *output = [[[AVCaptureVideoDataOutput alloc] init] autorelease];

	[mSession addOutput:output];

	[mSession beginConfiguration];
	if( cinder::ivec2( mWidth, mHeight ) == cinder::ivec2( 640, 480 ) )
		mSession.sessionPreset = AVCaptureSessionPreset640x480;
	else if( cinder::ivec2( mWidth, mHeight ) == cinder::ivec2( 1280, 720 ) )
		mSession.sessionPreset = AVCaptureSessionPreset1280x720;
	else
		mSession.sessionPreset = AVCaptureSessionPresetMedium;
	[mSession commitConfiguration];
	
	//adjust connection settings
	/*
	//Testing indicates that at least the 3GS doesn't support video orientation changes
	NSArray * connections = output.connections;
	for( AVCaptureConnection *connection in connections ) {
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
	NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:
#if ! defined( CINDER_COCOA_TOUCH )
								[NSNumber numberWithDouble:mWidth], (id)kCVPixelBufferWidthKey,
								[NSNumber numberWithDouble:mHeight], (id)kCVPixelBufferHeightKey,
#endif
								[NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA], (id)kCVPixelBufferPixelFormatTypeKey,
								nil];
	output.videoSettings = options;

	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(avCaptureInputPortFormatDescriptionDidChange:) name:AVCaptureInputPortFormatDescriptionDidChangeNotification object:nil];

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

		if( mWorkingPixelBuffer ) {
			::CVBufferRelease( mWorkingPixelBuffer );
			mWorkingPixelBuffer = nullptr;
		}
		
		[mSession release];
		mSession = nil;

		mIsCapturing = false;
		mHasNewFrame = false;
		
		mCurrentFrame.reset();		
	}
}

- (bool)isCapturing
{
	return mIsCapturing;
}

// Called initially when the camera is instantiated and then again (hypothetically) if the resolution ever changes
- (void)avCaptureInputPortFormatDescriptionDidChange:(NSNotification *)notification
{
	AVCaptureInput *input = [mSession.inputs objectAtIndex:0];
	AVCaptureInputPort *port = [input.ports objectAtIndex:0];
	CMFormatDescriptionRef formatDescription = port.formatDescription;
	if( formatDescription ) {
		CMVideoDimensions dimensions = ::CMVideoFormatDescriptionGetDimensions( formatDescription );
		if( (dimensions.width != 0) && (dimensions.height != 0) ) {
			mWidth = dimensions.width;
			mHeight = dimensions.height;
		}
	}
}

// Delegate routine that is called when a sample buffer was written
- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{ 
	@synchronized( self ) {
		if( mIsCapturing ) {
			// if the last pixel buffer went unclaimed, we'll need to release it
			if( mWorkingPixelBuffer ) {
				::CVBufferRelease( mWorkingPixelBuffer );
				mWorkingPixelBuffer = nullptr;
			}
			
			mWorkingPixelBuffer = ::CMSampleBufferGetImageBuffer(sampleBuffer);
			::CVBufferRetain( mWorkingPixelBuffer );
			mHasNewFrame = true;			
		}
	}	
}

- (cinder::Surface8uRef)getCurrentFrame
{
	if( ( ! mIsCapturing ) || ( ! mWorkingPixelBuffer ) ) {
		return mCurrentFrame;
	}
	
	@synchronized( self ) {
		::CVPixelBufferLockBaseAddress( mWorkingPixelBuffer, 0 );
		
		uint8_t *data = (uint8_t *)::CVPixelBufferGetBaseAddress( mWorkingPixelBuffer );
		mExposedFrameBytesPerRow = (int32_t)::CVPixelBufferGetBytesPerRow( mWorkingPixelBuffer );
		mExposedFrameWidth = (int32_t)::CVPixelBufferGetWidth( mWorkingPixelBuffer );
		mExposedFrameHeight = (int32_t)::CVPixelBufferGetHeight( mWorkingPixelBuffer );

		auto captureWorkingPixelBuffer = mWorkingPixelBuffer;
		mCurrentFrame = std::shared_ptr<cinder::Surface8u>( new cinder::Surface8u( data, mExposedFrameWidth, mExposedFrameHeight, mExposedFrameBytesPerRow, cinder::SurfaceChannelOrder::BGRA ),
				[captureWorkingPixelBuffer]( cinder::Surface8u* s ) {
					delete s;
					frameDeallocator( captureWorkingPixelBuffer );
				} );
		
		// mark the working pixel buffer as empty since we have wrapped it in the current frame
		mWorkingPixelBuffer = nullptr;
	}
	
	return mCurrentFrame;
}

- (bool)checkNewFrame
{
	bool result;
	@synchronized( self ) {
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