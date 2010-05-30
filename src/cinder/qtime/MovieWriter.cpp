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

#if defined( CINDER_COCOA ) && ( ! defined( __OBJC__ ) )
#	error "This file must be compiled as Objective-C++ on the Mac"
#endif

#include "cinder/qtime/MovieWriter.h"
#include "cinder/qtime/QuickTimeUtils.h"

#if defined( CINDER_MAC )
	#include <QTKit/QTKit.h>
	#include "cinder/cocoa/CinderCocoa.h"
#else
	#pragma push_macro( "__STDC_CONSTANT_MACROS" )
		#undef __STDC_CONSTANT_MACROS
		#include <QTML.h>
		#include <CVPixelBuffer.h>
		#include <ImageCompression.h>
		#include <Movies.h>
		#include <CoreFoundation/CFBase.h>
		#include <CoreFoundation/CFNumber.h>
	#pragma pop_macro( "__STDC_CONSTANT_MACROS" )
#endif


namespace cinder { namespace qtime {

MovieWriter::MovieWriter( const std::string &outPath, int32_t width, int32_t height, MovieWriterCodecType codec, MovieWriterQuality quality )
	: mObj( shared_ptr<Obj>( new Obj( outPath, width, height, codec, quality ) ) )
{
}

MovieWriter::Obj::~Obj()
{
	if( ! mFinished )
		finish();
}

MovieWriter::Obj::Obj( const std::string &path, int32_t width, int32_t height, MovieWriterCodecType codec, MovieWriterQuality quality )
	: mPath( path ), mQuality( quality ), mWidth( width ), mHeight( height ), mFinished( false )
{	
    OSErr       err = noErr;
    Handle      dataRef;
    OSType      dataRefType;
 
	startQuickTime();

    //Create movie file
	CFStringRef strDestMoviePath = ::CFStringCreateWithCString( kCFAllocatorDefault, path.c_str(), kCFStringEncodingUTF8 );
	err = ::QTNewDataReferenceFromFullPathCFString( strDestMoviePath, kQTNativeDefaultPathStyle, 0, &dataRef, &dataRefType );
	::CFRelease( strDestMoviePath );
	if( err )
        throw MovieWriterExcInvalidPath();

	// Create a movie for this file (data ref)
    err = ::CreateMovieStorage( dataRef, dataRefType, 'TVOD', smCurrentScript, createMovieFileDeleteCurFile | createMovieFileDontCreateResFile, &mDataHandler, &mMovie );
	::DisposeHandle( dataRef );
    if( err )
        throw MovieWriterExc();

	mTrack = ::NewMovieTrack( mMovie, width << 16, height << 16, 0 );
	err = ::GetMoviesError();
	if( err )
		throw MovieWriterExc();
        
	//Create track media
	::TimeScale timescale = 1000;
	mMedia = ::NewTrackMedia( mTrack, ::VideoMediaType, timescale, 0, 0 );
	err = GetMoviesError();
	if( err )
		throw MovieWriterExc();

	//Prepare media for editing
	err = ::BeginMediaEdits( mMedia );
mCodec = 'png ';
	createCompressionSession();
	mCurrentTimeValue = 0;
}
	
void MovieWriter::Obj::addFrame( const ImageSourceRef &imageSource )
{
	CVPixelBufferRef pixelBuffer = createCvPixelBuffer( imageSource );
	const float gamma = 2.5f;
	::CFNumberRef gammaLevel = CFNumberCreate( kCFAllocatorDefault, kCFNumberFloatType, &gamma );
	::CVBufferSetAttachment( pixelBuffer, kCVImageBufferGammaLevelKey, gammaLevel, kCVAttachmentMode_ShouldPropagate );

	ICMValidTimeFlags validTimeFlags = kICMValidTime_DisplayTimeStampIsValid | kICMValidTime_DisplayDurationIsValid;
	ICMCompressionFrameOptionsRef frameOptions = NULL;
	OSStatus err = ::ICMCompressionSessionEncodeFrame( mCompressionSession, pixelBuffer,
				mCurrentTimeValue, 60, validTimeFlags,
                frameOptions, NULL, NULL );
	mCurrentTimeValue += 60;
	if( err )
		MovieWriterExcFrameEncode();
}

extern "C" {
OSStatus MovieWriter::Obj::encodedFrameOutputCallback( void *refCon, 
                   ICMCompressionSessionRef session, 
                   OSStatus err,
                   ICMEncodedFrameRef encodedFrame,
                   void *reserved )
{
	MovieWriter::Obj *obj = reinterpret_cast<MovieWriter::Obj*>( refCon );
	OSStatus result = ::AddMediaSampleFromEncodedFrame( obj->mMedia, encodedFrame, NULL );
	return result;
}

}

void MovieWriter::Obj::createCompressionSession()
{
	OSStatus err = noErr;
	::ICMEncodedFrameOutputRecord encodedFrameOutputRecord = {0};
	::ICMCompressionSessionOptionsRef sessionOptions = NULL;
	
	err = ::ICMCompressionSessionOptionsCreate( NULL, &sessionOptions );
	if( err )
		goto bail;
	
	// We must set this flag to enable P or B frames.
	err = ::ICMCompressionSessionOptionsSetAllowTemporalCompression( sessionOptions, true );
	if( err )
		goto bail;
	
	// We must set this flag to enable B frames.
	err = ::ICMCompressionSessionOptionsSetAllowFrameReordering( sessionOptions, true );
	if( err )
		goto bail;
	
	// Set the maximum key frame interval, also known as the key frame rate.
	err = ::ICMCompressionSessionOptionsSetMaxKeyFrameInterval( sessionOptions, 30 );
	if( err )
		goto bail;

	// This allows the compressor more flexibility (ie, dropping and coalescing frames).
	err = ::ICMCompressionSessionOptionsSetAllowFrameTimeChanges( sessionOptions, true );
	if( err )
		goto bail;
	
	// We need durations when we store frames.
	err = ::ICMCompressionSessionOptionsSetDurationsNeeded( sessionOptions, true );
	if( err )
		goto bail;
	
	// Set the average data rate.
	/*err = ICMCompressionSessionOptionsSetProperty( sessionOptions, 
				kQTPropertyClass_ICMCompressionSessionOptions,
				kICMCompressionSessionOptionsPropertyID_AverageDataRate,
				sizeof( averageDataRate ),
				&averageDataRate );*/
	if( err )
		goto bail;
	
	encodedFrameOutputRecord.encodedFrameOutputCallback = encodedFrameOutputCallback;
	encodedFrameOutputRecord.encodedFrameOutputRefCon = this;
	encodedFrameOutputRecord.frameDataAllocator = NULL;
	
	::TimeScale timeScale = 1000;
	err = ::ICMCompressionSessionCreate( NULL, mWidth, mHeight, mCodec, timeScale,
			sessionOptions, NULL, &encodedFrameOutputRecord, &mCompressionSession );
	if( err )
		goto bail;
	return;

bail:
	::ICMCompressionSessionOptionsRelease( sessionOptions );
	throw MovieWriterExc();
}

void MovieWriter::Obj::finish()
{
	mFinished = true; // set this in case of throw

	OSErr err;
	if( mMedia )  {
		err = ::EndMediaEdits( mMedia );
		if( err )
			throw MovieWriterExc();
            
		err = ::ExtendMediaDecodeDurationToDisplayEndTime( mMedia, NULL );
		if( err )
			throw MovieWriterExc();
            
		//Add media to track
		err = ::InsertMediaIntoTrack( mTrack, 0, 0, ::GetMediaDisplayDuration( mMedia ), fixed1 );
		if( err )
			throw MovieWriterExc();
            
		//Write movie
		err = ::AddMovieToStorage( mMovie, mDataHandler );
		if( err )
			throw MovieWriterExc();
	}
        
	//Close movie file
	if( mDataHandler )
		::CloseMovieStorage( mDataHandler );

	if( mMovie )
		::DisposeMovie( mMovie );
}

} } // namespace cinder::qtime