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
#include "cinder/Utilities.h"
#include "cinder/Log.h"
#import <AVFoundation/AVFoundation.h>
#include <set>
#include <sstream>

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

std::vector<Capture::Mode> CaptureImplAvFoundationDevice::getModes() const
{
	std::vector<Capture::Mode> modes;

	if( !mNativeDevice )
		return modes;

	// Define practical frame rates we want to expose
	std::vector<int> practicalFrameRates = { 120, 60, 30, 24, 15, 10, 5 };

	std::set<std::string> seenModes; // To avoid duplicates

	for( AVCaptureDeviceFormat *format in [mNativeDevice formats] ) {
		CMVideoDimensions dimensions = CMVideoFormatDescriptionGetDimensions( format.formatDescription );
		FourCharCode mediaSubType = CMFormatDescriptionGetMediaSubType( format.formatDescription );

		// Map FourCharCode to our PixelFormat enum
		Capture::Mode::PixelFormat pixelFormat = Capture::Mode::PixelFormat::Unknown;
		switch( mediaSubType ) {
			case 'BGRA': pixelFormat = Capture::Mode::PixelFormat::BGRA32; break;
			case '420v': pixelFormat = Capture::Mode::PixelFormat::NV12; break;
			case '420f': pixelFormat = Capture::Mode::PixelFormat::I420; break;
			case 'yuvs': pixelFormat = Capture::Mode::PixelFormat::YUY2; break;
			case '2vuy': pixelFormat = Capture::Mode::PixelFormat::UYVY; break;
			case 'y420': pixelFormat = Capture::Mode::PixelFormat::YUV420P; break;
			default:
				// Skip unknown formats
				continue;
		}

		// Get the supported frame rate range for this format
		AVFrameRateRange *frameRateRange = format.videoSupportedFrameRateRanges.firstObject;
		if( !frameRateRange ) continue;

		int minFps = (int)round( frameRateRange.minFrameRate );
		int maxFps = (int)round( frameRateRange.maxFrameRate );

		std::vector<int> framesToCreate;
		if( minFps == maxFps ) { // Device has fixed frame rate - just use that
			framesToCreate.push_back( minFps );
		} else { // Device supports variable frame rates - try practical rates first
			for( int targetFps : practicalFrameRates )
				if( targetFps >= minFps && targetFps <= maxFps )
					framesToCreate.push_back( targetFps );
			
			// If no practical rates match, fall back to max frame rate
			if( framesToCreate.empty() )
				framesToCreate.push_back( maxFps );
		}

		// Create modes for each frame rate
		for( int targetFps : framesToCreate ) {
			MediaTime frameRate( 1, targetFps ); // Frame duration: 1/fps seconds

			std::string modeKey = std::to_string( dimensions.width ) + "x" + std::to_string( dimensions.height ) +
								 "@" + std::to_string( targetFps ) + "fps_" + std::to_string( (int)pixelFormat );

			// Skip if we've already seen this exact mode
			if( seenModes.find( modeKey ) != seenModes.end() )
				continue;
			seenModes.insert( modeKey );

			// Create description - only show range if min != max
			std::string description = std::to_string( dimensions.width ) + "x" + std::to_string( dimensions.height ) +
									 " @ " + std::to_string( targetFps ) + "fps";

			if( minFps != maxFps ) {
				// Device supports variable frame rates - store the range
				description += " (range: " + std::to_string( minFps ) + "-" + std::to_string( maxFps ) + "fps)";
				modes.emplace_back( Capture::Mode( dimensions.width, dimensions.height, frameRate, Capture::Mode::Codec::Uncompressed, pixelFormat, description ) );
			} else {
				// Device has fixed frame rate - use simple constructor without range
				modes.emplace_back( Capture::Mode( dimensions.width, dimensions.height, frameRate, Capture::Mode::Codec::Uncompressed, pixelFormat, description ) );
			}
		}
	}

	return modes;
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

- (id)initWithDevice:(const cinder::Capture::DeviceRef)device mode:(const cinder::Capture::Mode&)mode
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
		mWidth = mode.getWidth();
		mHeight = mode.getHeight();
		mHasNewFrame = false;
		mExposedFrameBytesPerRow = 0;
		mExposedFrameWidth = 0;
		mExposedFrameHeight = 0;

		// Store the mode for use during capture setup
		mSelectedMode = std::make_unique<cinder::Capture::Mode>(mode);
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

	[mSession beginConfiguration];

	// Configure device format and frame rate if we have a specific mode
	if( mSelectedMode ) {
		// Find the best matching format for the selected mode
		AVCaptureDeviceFormat *bestFormat = nil;
		double targetFrameRate = 1.0 / mSelectedMode->getFrameRate().getSeconds(); // Convert duration to rate

		// Look for exact or best matching format
		for( AVCaptureDeviceFormat *format in [device formats] ) {
			CMFormatDescriptionRef formatDesc = [format formatDescription];
			CMVideoDimensions dimensions = CMVideoFormatDescriptionGetDimensions( formatDesc );

			// Check if resolution matches
			if( dimensions.width == mSelectedMode->getWidth() &&
				dimensions.height == mSelectedMode->getHeight() ) {

				// Check if frame rate is supported
				for( AVFrameRateRange *range in [format videoSupportedFrameRateRanges] ) {
					if( targetFrameRate >= range.minFrameRate && targetFrameRate <= range.maxFrameRate ) {
						bestFormat = format;
						break;
					}
				}
				if( bestFormat )
					break;
			}
		}

		// Configure device with the selected format
		if( bestFormat && [device lockForConfiguration:&error] ) {
			device.activeFormat = bestFormat;

			// Set frame rate
			CMTime frameDuration = CMTimeMake( 1, (int32_t)targetFrameRate );
			device.activeVideoMinFrameDuration = frameDuration;
			device.activeVideoMaxFrameDuration = frameDuration;

			[device unlockForConfiguration];
		}
		else {
			// Use session preset as fallback
			if( cinder::ivec2( mWidth, mHeight ) == cinder::ivec2( 640, 480 ) )
				mSession.sessionPreset = AVCaptureSessionPreset640x480;
			else if( cinder::ivec2( mWidth, mHeight ) == cinder::ivec2( 1280, 720 ) )
				mSession.sessionPreset = AVCaptureSessionPreset1280x720;
			else if( cinder::ivec2( mWidth, mHeight ) == cinder::ivec2( 1920, 1080 ) )
				mSession.sessionPreset = AVCaptureSessionPreset1920x1080;
			else
				mSession.sessionPreset = AVCaptureSessionPresetMedium;
		}
	}
	else {
		// Legacy mode - use session presets
		if( cinder::ivec2( mWidth, mHeight ) == cinder::ivec2( 640, 480 ) )
			mSession.sessionPreset = AVCaptureSessionPreset640x480;
		else if( cinder::ivec2( mWidth, mHeight ) == cinder::ivec2( 1280, 720 ) )
			mSession.sessionPreset = AVCaptureSessionPreset1280x720;
		else if( cinder::ivec2( mWidth, mHeight ) == cinder::ivec2( 1920, 1080 ) )
			mSession.sessionPreset = AVCaptureSessionPreset1920x1080;
		else
			mSession.sessionPreset = AVCaptureSessionPresetMedium;
	}

	// Create a device input with the device and add it to the session.
	AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:device error:&error];
	if( ! input ) {
		[mSession commitConfiguration];
		throw cinder::CaptureExcInitFail();
	}
	[mSession addInput:input];

	// Create a VideoDataOutput and add it to the session
	AVCaptureVideoDataOutput *output = [[[AVCaptureVideoDataOutput alloc] init] autorelease];
	[mSession addOutput:output];

	[mSession commitConfiguration];

	// Configure your output.
	dispatch_queue_t queue = dispatch_queue_create("myQueue", NULL);
	[output setSampleBufferDelegate:self queue:queue];
	dispatch_release(queue);

	// Always request BGRA format to match legacy behavior and avoid YUV conversion issues
	// TODO: Add proper YUV support in the future
	OSType pixelFormatType = kCVPixelFormatType_32BGRA;

	// Specify the pixel format
	NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:
#if ! defined( CINDER_COCOA_TOUCH )
								[NSNumber numberWithDouble:mWidth], (id)kCVPixelBufferWidthKey,
								[NSNumber numberWithDouble:mHeight], (id)kCVPixelBufferHeightKey,
#endif
								[NSNumber numberWithUnsignedInt:pixelFormatType], (id)kCVPixelBufferPixelFormatTypeKey,
								nil];
	output.videoSettings = options;

	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(avCaptureInputPortFormatDescriptionDidChange:) name:AVCaptureInputPortFormatDescriptionDidChangeNotification object:nil];

	// Add session runtime error notifications to detect device disconnections
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionRuntimeError:) name:AVCaptureSessionRuntimeErrorNotification object:mSession];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionWasInterrupted:) name:AVCaptureSessionWasInterruptedNotification object:mSession];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sessionInterruptionEnded:) name:AVCaptureSessionInterruptionEndedNotification object:mSession];

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
	// Check not only our flag, but also the actual session state
	// This will detect device disconnections that don't trigger stopCapture
	bool sessionRunning = mSession && mSession.isRunning;
	bool hasInputs = mSession && mSession.inputs.count > 0;
	bool result = mIsCapturing && sessionRunning && hasInputs;

	// Log when state doesn't match expectations
	if( mIsCapturing && (!sessionRunning || !hasInputs) ) {
		CI_LOG_W( "Camera disconnection detected: mIsCapturing=" << mIsCapturing
			<< " sessionRunning=" << sessionRunning
			<< " hasInputs=" << hasInputs
			<< " inputCount=" << (mSession ? mSession.inputs.count : 0) );
	}

	return result;
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

// Session notification handlers for device disconnection detection
- (void)sessionRuntimeError:(NSNotification *)notification
{
	NSError *error = notification.userInfo[AVCaptureSessionErrorKey];
	CI_LOG_E( "AVCapture session runtime error: " << [error.localizedDescription UTF8String]
		<< " (code: " << error.code << ")" );

	// Common error codes that indicate device disconnection:
	// -11814: Device disconnected
	// -11819: Media services were reset
	// -11808: Recording stopped (can be disconnection)
	// Check if the device is still connected
	if( error.code == -11814 || error.code == -11819 || error.code == -11808 ) {
		// Mark session as not capturing on these critical errors
		mIsCapturing = false;
		CI_LOG_W( "Device likely disconnected - marking capture as stopped" );
	}
}

- (void)sessionWasInterrupted:(NSNotification *)notification
{
	CI_LOG_W( "AVCapture session was interrupted (possible device disconnection)" );
}

- (void)sessionInterruptionEnded:(NSNotification *)notification
{
	CI_LOG_I( "AVCapture session interruption ended" );
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
