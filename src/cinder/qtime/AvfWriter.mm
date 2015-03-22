/*
 Copyright (c) 2014, The Cinder Project, All rights reserved.
 
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

#include "cinder/Utilities.h"
#include "cinder/Log.h"
#include "cinder/qtime/AvfUtils.h"
#include "cinder/qtime/AvfWriter.h"
#include "cinder/cocoa/CinderCocoa.h"
#include "cinder/ip/Fill.h"

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

namespace cinder { namespace qtime {

const float PLATFORM_DEFAULT_GAMMA = 2.2f;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MovieWriter::Format

MovieWriter::Format::Format()
	: mCodec( H264 )
{
	initDefaults();
}

MovieWriter::Format::Format( Codec codec, float quality )
	: mCodec( codec )
{
	initDefaults();
	setQuality( quality );
}

MovieWriter::Format::Format( Codec codec, float quality, float frameRate, bool enableMultiPass )
	:	mCodec( codec ), mEnableMultiPass( enableMultiPass )
{
	setQuality( quality );
	setTimeScale( (long)(frameRate * 100) );
	setDefaultDuration( 1.0f / frameRate );
	setGamma( PLATFORM_DEFAULT_GAMMA );
}

MovieWriter::Format::Format( const Format &format )
:	mCodec( format.mCodec ), mTimeBase( format.mTimeBase ), mDefaultTime( format.mDefaultTime ), 
	mGamma( format.mGamma ), mEnableMultiPass( format.mEnableMultiPass ), mQualityFloat( format.mQualityFloat )
{
	//  TODO: ???
}

MovieWriter::Format::~Format()
{
}

void MovieWriter::Format::initDefaults()
{
	mTimeBase = 600;
	mDefaultTime = 1 / 30.0f;
	mGamma = PLATFORM_DEFAULT_GAMMA;
	mEnableMultiPass = false;

	enableTemporal( true );
	enableReordering( true );
	enableFrameTimeChanges( true );
	setQuality( 0.99f );
}

MovieWriter::Format& MovieWriter::Format::setQuality( float quality )
{
	/* TODO: RE-IMPLEMENT
	mQualityFloat = constrain<float>( quality, 0, 1 );
	CodecQ compressionQuality = CodecQ(0x00000400 * mQualityFloat);
	OSStatus err = ICMCompressionSessionOptionsSetProperty( mOptions,
                                kQTPropertyClass_ICMCompressionSessionOptions,
                                kICMCompressionSessionOptionsPropertyID_Quality,
                                sizeof(compressionQuality),
                                &compressionQuality );	
	*/
	return *this;
}

bool MovieWriter::Format::isTemporal() const
{
	// TODO: RE-IMPLEMENT
	return false;
}

MovieWriter::Format& MovieWriter::Format::enableTemporal( bool enable )
{
	// TODO: RE-IMPLEMENT
	return *this;
}

bool MovieWriter::Format::isReordering() const
{
	// TODO: RE-IMPLEMENT
	return false;
}

MovieWriter::Format& MovieWriter::Format::enableReordering( bool enable )
{
	// TODO: RE-IMPLEMENT
	return *this;
}

bool MovieWriter::Format::isFrameTimeChanges() const
{
	// TODO: RE-IMPLEMENT
	return false;
}

MovieWriter::Format& MovieWriter::Format::enableFrameTimeChanges( bool enable )
{
	// TODO: RE-IMPLEMENT
	return *this;
}

int32_t MovieWriter::Format::getMaxKeyFrameRate() const
{
	// TODO: RE-IMPLEMENT
	return 0;
}

MovieWriter::Format& MovieWriter::Format::setMaxKeyFrameRate( int32_t rate )
{
	// TODO: RE-IMPLEMENT
	return *this;
}

const MovieWriter::Format& MovieWriter::Format::operator=( const Format &format )
{
	// TODO: UPDATE
	mCodec = format.mCodec;
	mTimeBase = format.mTimeBase;
	mDefaultTime = format.mDefaultTime;
	mGamma = format.mGamma;
	mEnableMultiPass = format.mEnableMultiPass;

	return *this;
}

namespace {
const NSString * codecToAvVideoCodecKey( MovieWriter::Codec codec )
{
	switch( codec ) {
		case MovieWriter::Codec::H264:
			return AVVideoCodecH264;
		case MovieWriter::Codec::JPEG:
			return AVVideoCodecJPEG;
		case MovieWriter::Codec::PRO_RES_4444:
			return AVVideoCodecAppleProRes4444;
		case MovieWriter::Codec::PRO_RES_422:
			return AVVideoCodecAppleProRes422;
		default:
			return nil;
	}
}
} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MovieWriter
MovieWriter::MovieWriter( const fs::path &path, int32_t width, int32_t height, const Format &format )
	: mPath( path ), mWidth( width ), mHeight( height ), mFormat( format ), mFinished( false ), mNumFrames(0)
{
//	AVFileTypeQuickTimeMovie
//	AVFileTypeMPEG4
//	AVFileTypeAppleM4V
	// AVFoundation will fail if the path already exists.
	if( fs::exists( path ) )
		fs::remove( path );
	
	NSURL* localOutputURL = [NSURL fileURLWithPath:[NSString stringWithCString:mPath.c_str() encoding:[NSString defaultCStringEncoding]]];
	NSError* error = nil;
	mWriter = [[AVAssetWriter alloc] initWithURL:localOutputURL fileType:AVFileTypeQuickTimeMovie error:&error];
	
	NSMutableDictionary* videoSettings = [NSMutableDictionary dictionaryWithObjectsAndKeys:
										  codecToAvVideoCodecKey( format.getCodec() ), AVVideoCodecKey,
										  [NSNumber numberWithInteger:mWidth], AVVideoWidthKey,
										  [NSNumber numberWithInteger:mHeight], AVVideoHeightKey,
										  nil];
	mWriterSink = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo outputSettings:videoSettings];
	[mWriterSink setExpectsMediaDataInRealTime:true];

	// These are documented as optimal for their respective platforms. AVVideoCodecAppleProRes4444 can support an alpha channel, but it automatically infers its necessity from the image content it seems.
	NSDictionary *compressionSettings = [NSMutableDictionary dictionaryWithObjectsAndKeys:
#if defined( CINDER_COCOA_TOUCH )
										[NSNumber numberWithInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey,
#else
										[NSNumber numberWithInt:kCVPixelFormatType_32ARGB], kCVPixelBufferPixelFormatTypeKey,
#endif
										nil ];
	
	mSinkAdapater = [AVAssetWriterInputPixelBufferAdaptor assetWriterInputPixelBufferAdaptorWithAssetWriterInput:mWriterSink
																					 sourcePixelBufferAttributes:compressionSettings];
	
	[mSinkAdapater retain];
	[mWriter addInput:mWriterSink];
	mWriter.movieFragmentInterval = CMTimeMakeWithSeconds(1.0, 1000);
	[mWriter startWriting];
	AVAssetWriterStatus status = [mWriter status];
	if( status == AVAssetWriterStatusFailed) {
		error = [mWriter error];
		NSString* str = [error description];
		std::string description = (str? std::string([str UTF8String]): "");
		CI_LOG_E( "Error at startWriting:" << description );
	}
	else {
		[mWriter startSessionAtSourceTime:kCMTimeZero];
	}
}

MovieWriter::~MovieWriter()
{
	if( ! mFinished )
		finish();
}

void MovieWriter::addFrame( const Surface8u& imageSource, float duration )
//void MovieWriter::addFrame( const ImageSourceRef& imageSource, float duration )
{
	/* RE-IMPLEMENT
	if( mFinished )
		throw MovieWriterExcAlreadyFinished();

	if( duration <= 0 )
		duration = mFormat.mDefaultTime;

	::CVPixelBufferRef pixelBuffer = createCvPixelBuffer( imageSource, false );
	::CFNumberRef gammaLevel = CFNumberCreate( kCFAllocatorDefault, kCFNumberFloatType, &mFormat.mGamma );
	::CVBufferSetAttachment( pixelBuffer, kCVImageBufferGammaLevelKey, gammaLevel, kCVAttachmentMode_ShouldPropagate );
	::CFRelease( gammaLevel );

	::ICMValidTimeFlags validTimeFlags = kICMValidTime_DisplayTimeStampIsValid | kICMValidTime_DisplayDurationIsValid;
	::ICMCompressionFrameOptionsRef frameOptions = NULL;
	int64_t durationVal = static_cast<int64_t>( duration * mFormat.mTimeBase );
	OSStatus err = ::ICMCompressionSessionEncodeFrame( mCompressionSession, pixelBuffer,
				mCurrentTimeValue, durationVal, validTimeFlags,
                frameOptions, NULL, NULL );

	mFrameTimes.push_back( std::pair<int64_t,int64_t>( mCurrentTimeValue, durationVal ) );

	if( mDoingMultiPass ) {
		mMultiPassFrameCache->write( (uint32_t)::CVPixelBufferGetWidth( pixelBuffer ) );
		mMultiPassFrameCache->write( (uint32_t)::CVPixelBufferGetHeight( pixelBuffer ) );
		mMultiPassFrameCache->write( (uint32_t)::CVPixelBufferGetPixelFormatType( pixelBuffer ) );
		mMultiPassFrameCache->write( (uint32_t)::CVPixelBufferGetBytesPerRow( pixelBuffer ) );
		::CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
		mMultiPassFrameCache->write( (uint32_t) ::CVPixelBufferGetDataSize( pixelBuffer ) );
		mMultiPassFrameCache->writeData( ::CVPixelBufferGetBaseAddress( pixelBuffer ), ::CVPixelBufferGetDataSize( pixelBuffer ) );
		::CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
	}

	mCurrentTimeValue += durationVal;
	++mNumFrames;

	::CVPixelBufferRelease( pixelBuffer );

	if( err )
		MovieWriterExcFrameEncode();
	*/
		
	if( mFinished )
		throw MovieWriterExcAlreadyFinished();
	
	NSError* error = nil;
	AVAssetWriterStatus status = [mWriter status];
	if( status == AVAssetWriterStatusFailed ) {
		error = [mWriter error];
		NSString* str = [error description];
		std::string descr = (str? std::string([str UTF8String]): "");
		CI_LOG_E( " Error when trying to start writing: " << descr );
		return;
	}
	else if( status != AVAssetWriterStatusWriting ) {
		return;
	}
	
	if( duration <= 0 )
		duration = mFormat.mDefaultTime;
	
	int64_t durationVal = static_cast<int64_t>( duration * mFormat.mTimeBase );

	::CVPixelBufferPoolRef pixelBufferPool = [mSinkAdapater pixelBufferPool];
	::CVPixelBufferRef pixelBuffer;
	if( ::CVPixelBufferPoolCreatePixelBuffer( kCFAllocatorDefault, pixelBufferPool, &pixelBuffer ) != kCVReturnSuccess ) {
		CI_LOG_E( "Failed to add frame." );
		return;
	}

	::CFNumberRef gammaLevel = CFNumberCreate( kCFAllocatorDefault, kCFNumberFloatType, &mFormat.mGamma );
	::CVBufferSetAttachment( pixelBuffer, kCVImageBufferGammaLevelKey, gammaLevel, kCVAttachmentMode_ShouldPropagate );
	::CFRelease( gammaLevel );
	
	// this decrements the retain count on the CVPixelBuffer on destruction
	Surface8uRef destinationSurface = cocoa::convertCVPixelBufferToSurface( pixelBuffer );
	destinationSurface->copyFrom( imageSource, imageSource.getBounds() );
	// THE FOLLOWING TIME VALUE IN SECONDS MUST BE VALID!!!
	static double seconds = 0;
	seconds += 0.016667;
	CMTime currentTime = CMTimeMakeWithSeconds(seconds,120);

//ip::fill( &destinationSurface->getChannelAlpha(), (uint8_t)128 );

	[mSinkAdapater appendPixelBuffer:pixelBuffer withPresentationTime:currentTime];
	mCurrentTimeValue += durationVal;
	++mNumFrames;
}

extern "C" {
	
void destroyDataArrayU8( void *releaseRefCon, const void *baseAddress )
{
	delete [] (reinterpret_cast<uint8_t*>( const_cast<void*>( baseAddress ) ));
}
/*
OSStatus encodedFrameOutputCallback( void *refCon, ICMCompressionSessionOptionsRef session,
									 OSStatus err, ICMEncodedFrameRef encodedFrame, void *reserved )
{
	// RE-IMPLEMENT
	MovieWriter::Obj *obj = reinterpret_cast<MovieWriter::Obj*>( refCon );

	ImageDescriptionHandle imageDescription = NULL;
	err = ICMCompressionSessionGetImageDescription( session, &imageDescription );
	if( ! err ) {
		Fixed gammaLevel = qtime::floatToFixed( obj->mFormat.mGamma );
		err = ICMImageDescriptionSetProperty(imageDescription,
						kQTPropertyClass_ImageDescription,
						kICMImageDescriptionPropertyID_GammaLevel,
						sizeof(gammaLevel), &gammaLevel);
		if( err != 0 )
			throw;
	}
	else
		throw;

	OSStatus result = ::AddMediaSampleFromEncodedFrame( obj->mMedia, encodedFrame, NULL );
	return result;
	
//	OSStatus result;
//	return result;
}

OSStatus enableMultiPassWithTemporaryFile( ICMCompressionSessionOptionsRef inCompressionSessionOptions, ICMMultiPassStorageRef *outMultiPassStorage )
{
	// RE-IMPLEMENT
	::ICMMultiPassStorageRef multiPassStorage = NULL;
	OSStatus status;
	*outMultiPassStorage = NULL;

	// create storage using a temporary file with a unique file name
	status = ::ICMMultiPassStorageCreateWithTemporaryFile( kCFAllocatorDefault, NULL, NULL, 0, &multiPassStorage );
	if( noErr != status )
		goto bail;

	// enable multi-pass by setting the compression session options
	// note - the compression session options object retains the multi-pass
	// storage object
	status = ::ICMCompressionSessionOptionsSetProperty( inCompressionSessionOptions, kQTPropertyClass_ICMCompressionSessionOptions,
						kICMCompressionSessionOptionsPropertyID_MultiPassStorage, sizeof(ICMMultiPassStorageRef), &multiPassStorage );

 bail:
    if( noErr != status ) {
        // this api is NULL safe so we can just call it
        ICMMultiPassStorageRelease( multiPassStorage );
    }
	else {
        *outMultiPassStorage = multiPassStorage;
    }

    return status;
	
//	OSStatus result;
//	return result;
 }
 */

}
	
void MovieWriter::createCompressionSession()
{
	
	//
	// https://developer.apple.com/library/mac/documentation/AVFoundation/Reference/AVFoundation_Constants/Reference/reference.html#//apple_ref/doc/c_ref/AVVideoMaxKeyFrameIntervalKey
	//
	// apply settings using the following info...
	
	/*
	 // codec options
	AVVideoCodecH264 // @"avc1"
	AVVideoCodecJPEG // @"jpeg"
	AVVideoCodecAppleProRes4444 // @"ap4h"
	AVVideoCodecAppleProRes422   // @"apcn"
	 
	 // compression settings option(s)
	AVVideoCompressionPropertiesKey
	 
	 // bit rate option
	AVVideoAverageBitRateKey
	 
	 // quality option
	AVVideoQualityKey
	 
	 // key frame interval option
	AVVideoMaxKeyFrameIntervalKey
	 
	 // H264 profile level options
	AVVideoProfileLevelKey
	AVVideoProfileLevelH264Baseline30
	AVVideoProfileLevelH264Baseline31
	AVVideoProfileLevelH264Baseline41
	AVVideoProfileLevelH264Main30
	AVVideoProfileLevelH264Main31
	AVVideoProfileLevelH264Main32
	AVVideoProfileLevelH264Main41
	AVVideoProfileLevelH264High40
	AVVideoProfileLevelH264High41
	 
	 // pixel aspect ratio options
	AVVideoPixelAspectRatioKey
	AVVideoPixelAspectRatioHorizontalSpacingKey
	AVVideoPixelAspectRatioVerticalSpacingKey
	 
	 // not sure what "clean aperture" means...
	AVVideoCleanApertureKey
	AVVideoCleanApertureWidthKey
	AVVideoCleanApertureHeightKey
	AVVideoCleanApertureHorizontalOffsetKey
	AVVideoCleanApertureVerticalOffsetKey
	 */
	
}
	
void MovieWriter::finish()
{
	if( mFinished )
		return;
	
	[mWriterSink markAsFinished];
	
	NSError* error = nil;
	bool success = [mWriter finishWriting];
	if( ! success )
		error = [mWriter error];
	
	mFinished = true;
}

bool MovieWriter::getUserCompressionSettings( Format* result, ImageSourceRef imageSource )
{
	
	return false;
}

} } // namespace cinder::qtime