/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.
 
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

#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>

namespace cinder { namespace qtime {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MovieWriter::Format

MovieWriter::Format::Format()
	: mCodec( H264 ), mFileType( QUICK_TIME_MOVIE )
{
	mTimeBase = 600;
	mDefaultFrameDuration = 1 / 30.0f;

	mFrameReorderingEnabled = false;
	jpegQuality( 0.99f );
	
	mH264AverageBitsPerSecondSpecified = false;
	mH264AverageBitsPerSecond = 0;
}

MovieWriter::Format& MovieWriter::Format::jpegQuality( float quality )
{
	mJpegQuality = quality;
	return *this;
}

#if ( defined( CINDER_MAC ) &&  ( MAC_OS_X_VERSION_MIN_REQUIRED >= 101000 ) ) || ( defined( CINDER_COCOA_TOUCH ) && ( __IPHONE_OS_VERSION_MIN_REQUIRED >= 70000 ) )
bool MovieWriter::Format::isFrameReorderingEnabled() const
{
	return mFrameReorderingEnabled;
}

MovieWriter::Format& MovieWriter::Format::enableFrameReordering( bool enable )
{
	mFrameReorderingEnabled = enable;
	return *this;
}
#endif

namespace {
const NSString * codecToAvVideoCodecKey( MovieWriter::Codec codec )
{
	switch( codec ) {
		case MovieWriter::Codec::H264:
			return AVVideoCodecH264;
		case MovieWriter::Codec::JPEG:
			return AVVideoCodecJPEG;
#if ! defined( CINDER_COCOA_TOUCH )
		case MovieWriter::Codec::PRO_RES_4444:
			return AVVideoCodecAppleProRes4444;
		case MovieWriter::Codec::PRO_RES_422:
			return AVVideoCodecAppleProRes422;
#endif
		default:
			return nil;
	}
}

NSString * fileTypeToAvFileType( MovieWriter::FileType fileType )
{
	switch( fileType ) {
		case MovieWriter::FileType::QUICK_TIME_MOVIE:
			return AVFileTypeQuickTimeMovie;
		case MovieWriter::FileType::MPEG4:
			return AVFileTypeMPEG4;
		case MovieWriter::FileType::M4V:
			return AVFileTypeAppleM4V;
		default:
			return nil;
	}
}
} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MovieWriter
MovieWriter::MovieWriter( const fs::path &path, int32_t width, int32_t height, const Format &format )
	: mWidth( width ), mHeight( height ), mFormat( format ), mFinished( false ), mNumFrames( 0 ), mCurrentSeconds( 0 )
{
	// AVFoundation will fail if the path already exists.
	if( fs::exists( path ) )
		fs::remove( path );
	
	NSURL* localOutputURL = [NSURL fileURLWithPath:[NSString stringWithCString:path.c_str() encoding:[NSString defaultCStringEncoding]]];
	NSError* error = nil;
	mWriter = [[AVAssetWriter alloc] initWithURL:localOutputURL fileType:fileTypeToAvFileType( format.getFileType() ) error:&error];

	NSMutableDictionary *compressionProperties = [NSMutableDictionary dictionary];
	if( format.getCodec() == Codec::H264 ) {
// Frame reordering is only enabled in OS X 10.10 / iOS 7.0
#if ( defined( CINDER_MAC ) &&  ( MAC_OS_X_VERSION_MIN_REQUIRED >= 101000 ) ) || ( defined( CINDER_COCOA_TOUCH ) && ( __IPHONE_OS_VERSION_MIN_REQUIRED >= 70000 ) )
		[compressionProperties setValue:[NSNumber numberWithBool:format.isFrameReorderingEnabled()] forKey:AVVideoAllowFrameReorderingKey];
#endif
		if( format.isAverageBitsPerSecondSpecified() )
			[compressionProperties setValue:[NSNumber numberWithFloat:format.getAverageBitsPerSecond()] forKey:AVVideoAverageBitRateKey];
	}
	else if( format.getCodec() == Codec::JPEG ) {
		[compressionProperties setValue:[NSNumber numberWithFloat:format.getJpegQuality()] forKey:AVVideoQualityKey];
	}
	
	NSMutableDictionary* outputSettings = [NSMutableDictionary dictionaryWithObjectsAndKeys:
											codecToAvVideoCodecKey( format.getCodec() ), AVVideoCodecKey,
											[NSNumber numberWithInteger:mWidth], AVVideoWidthKey,
											[NSNumber numberWithInteger:mHeight], AVVideoHeightKey,
											compressionProperties, AVVideoCompressionPropertiesKey,
											nil];
	mWriterSink = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo outputSettings:outputSettings];
	[mWriterSink setExpectsMediaDataInRealTime:true];

	NSDictionary *sourcePixelBufferAttributes = [NSMutableDictionary dictionaryWithObjectsAndKeys:
	// These are documented as optimal for their respective platforms. AVVideoCodecAppleProRes4444 can support an alpha channel, but it automatically infers its necessity from the image content it seems.
#if defined( CINDER_COCOA_TOUCH )
				[NSNumber numberWithInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey,
#else
				[NSNumber numberWithInt:kCVPixelFormatType_32ARGB], kCVPixelBufferPixelFormatTypeKey,
#endif
				nil ];
	
	mSinkAdapater = [AVAssetWriterInputPixelBufferAdaptor assetWriterInputPixelBufferAdaptorWithAssetWriterInput:mWriterSink
																					 sourcePixelBufferAttributes:sourcePixelBufferAttributes];
	
	[mSinkAdapater retain];
	[mWriter addInput:mWriterSink];
	mWriter.movieFragmentInterval = CMTimeMakeWithSeconds( 1.0, 1000 );
	[mWriter startWriting];
	if( [mWriter status] == AVAssetWriterStatusFailed ) {
		NSString* str = [[mWriter error] description];
		std::string description = str ? std::string([str UTF8String]): "";
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
	
	::CVPixelBufferPoolRelease( mSinkAdapater.pixelBufferPool );
	[mSinkAdapater release];
	[mWriter release];
}

void MovieWriter::addFrame( const Surface8u& imageSource, float duration )
{
	if( mFinished )
		throw MovieWriterExcAlreadyFinished();
	
	if( [mWriter status] != AVAssetWriterStatusWriting ) {
		NSString* str = [[mWriter error] description];
		std::string descr = str ? std::string([str UTF8String]): "";
		CI_LOG_E( " Error when trying to start writing: " << descr );
		return;
	}

	::CVPixelBufferPoolRef pixelBufferPool = [mSinkAdapater pixelBufferPool];
	::CVPixelBufferRef pixelBuffer;
	if( ::CVPixelBufferPoolCreatePixelBuffer( kCFAllocatorDefault, pixelBufferPool, &pixelBuffer ) != kCVReturnSuccess ) {
		CI_LOG_E( "Failed to add frame." );
		return;
	}

	// this decrements the retain count on the CVPixelBuffer on destruction
	Surface8uRef destinationSurface = cocoa::convertCVPixelBufferToSurface( pixelBuffer );
	destinationSurface->copyFrom( imageSource, imageSource.getBounds() );

	while( ! mWriterSink.readyForMoreMediaData ) {
		NSDate *future = [NSDate dateWithTimeIntervalSinceNow:0.01];
		[[NSRunLoop currentRunLoop] runUntilDate:future];
	}
	
	[mSinkAdapater appendPixelBuffer:pixelBuffer withPresentationTime:CMTimeMakeWithSeconds( mCurrentSeconds, (int32_t)mFormat.mTimeBase )];
	
	mCurrentSeconds += ( duration <= 0 ) ? mFormat.mDefaultFrameDuration : duration;
	++mNumFrames;
}
	
void MovieWriter::finish()
{
	if( mFinished )
		return;
	
	[mWriterSink markAsFinished];
	
	// finishWriting: only wants to be called from a secondary thread
	__block __typeof__(mWriter) _mWriter = mWriter;
	[mWriter retain]; // released inside block
	dispatch_async( dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_HIGH, 0 ), ^{
		[_mWriter finishWriting];
		[_mWriter release];
	} );
	
	mFinished = true;
}

} } // namespace cinder::qtime