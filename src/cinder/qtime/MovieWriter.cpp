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

#include "cinder/app/App.h"
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
		#include <GXMath.h>
	#pragma pop_macro( "__STDC_CONSTANT_MACROS" )
#endif
#include <QuickTimeComponents.h>

namespace cinder { namespace qtime {

#if defined( CINDER_MSW )
const float PLATFORM_DEFAULT_GAMMA = 2.5f;
#else
const float PLATFORM_DEFAULT_GAMMA = 2.2f;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MovieWriter::Format

MovieWriter::Format::Format()
	: mCodec( 'png ' )
{
	initDefaults();
}

MovieWriter::Format::Format( uint32_t codec, float quality )
	: mCodec( codec )
{
	initDefaults();
	setQuality( quality );
}

MovieWriter::Format::Format( const ICMCompressionSessionOptionsRef options, uint32_t codec, float quality, float frameRate )
	: mCodec( codec )
{
	::ICMCompressionSessionOptionsCreateCopy( NULL, options, &mOptions );
	setQuality( quality );
	setTimeScale( (long)(frameRate * 100) );
	setDefaultDuration( 1.0f / frameRate );
	setGamma( PLATFORM_DEFAULT_GAMMA );
}

MovieWriter::Format::Format( const Format &format )
	: mCodec( format.mCodec ), mTimeBase( format.mTimeBase ), mDefaultTime( format.mDefaultTime ), mGamma( format.mGamma )
{
	::ICMCompressionSessionOptionsCreateCopy( NULL, format.mOptions, &mOptions );
}

MovieWriter::Format::~Format()
{
	::ICMCompressionSessionOptionsRelease( mOptions );
}

void MovieWriter::Format::initDefaults()
{
	OSStatus err = ::ICMCompressionSessionOptionsCreate( NULL, &mOptions );

	mTimeBase = 600;
	mDefaultTime = 1 / 30.0f;
	mGamma = PLATFORM_DEFAULT_GAMMA;

	enableTemporal( true );
	enableReordering( true );
	enableFrameTimeChanges( true );
	setQuality( 0.99f );
}

MovieWriter::Format& MovieWriter::Format::setQuality( float quality )
{
	quality = constrain<float>( quality, 0, 1 );
	CodecQ compressionQuality = CodecQ(0x00000400 * quality);
	OSStatus err = ICMCompressionSessionOptionsSetProperty( mOptions,
                                kQTPropertyClass_ICMCompressionSessionOptions,
                                kICMCompressionSessionOptionsPropertyID_Quality,
                                sizeof(compressionQuality),
                                &compressionQuality );	
	return *this;
}

MovieWriter::Format& MovieWriter::Format::enableTemporal( bool enable )
{
	OSStatus err = ::ICMCompressionSessionOptionsSetAllowTemporalCompression( mOptions, enable );
	return *this;
}

MovieWriter::Format& MovieWriter::Format::enableReordering( bool enable )
{
	OSStatus err = ::ICMCompressionSessionOptionsSetAllowFrameReordering( mOptions, enable );
	return *this;
}

MovieWriter::Format& MovieWriter::Format::enableFrameTimeChanges( bool enable )
{
	OSStatus err = ::ICMCompressionSessionOptionsSetAllowFrameTimeChanges( mOptions, enable );
	return *this;
}

MovieWriter::Format& MovieWriter::Format::setMaxKeyFrameRate( int32_t rate )
{
	OSStatus err = ::ICMCompressionSessionOptionsSetMaxKeyFrameInterval( mOptions, rate );
	return *this;
}

const MovieWriter::Format& MovieWriter::Format::operator=( const Format &format )
{
	if( mOptions != format.mOptions ) {
		::ICMCompressionSessionOptionsRelease( mOptions );
		::ICMCompressionSessionOptionsCreateCopy( NULL, format.mOptions, &mOptions );
	}

	mCodec = format.mCodec;
	mTimeBase = format.mTimeBase;
	mDefaultTime = format.mDefaultTime;
	mGamma = format.mGamma;

	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MovieWriter
MovieWriter::MovieWriter( const std::string &path, int32_t width, int32_t height, const Format &format )
	: mObj( shared_ptr<Obj>( new Obj( path, width, height, format ) ) )
{
}

MovieWriter::Obj::~Obj()
{
	if( ! mFinished )
		finish();
}

MovieWriter::Obj::Obj( const std::string &path, int32_t width, int32_t height, const Format &format )
	: mPath( path ), mWidth( width ), mHeight( height ), mFormat( format ), mFinished( false )
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
	mMedia = ::NewTrackMedia( mTrack, ::VideoMediaType, mFormat.mTimeBase, 0, 0 );
	err = ::GetMoviesError();
	if( err )
		throw MovieWriterExc();

	//Prepare media for editing
	err = ::BeginMediaEdits( mMedia );
	createCompressionSession();
	mCurrentTimeValue = 0;
}
	
void MovieWriter::Obj::addFrame( const ImageSourceRef &imageSource, float duration )
{
	if( mFinished )
		throw MovieWriterExcAlreadyFinished();

	if( duration <= 0 )
		duration = mFormat.mDefaultTime;

	CVPixelBufferRef pixelBuffer = createCvPixelBuffer( imageSource );
	::CFNumberRef gammaLevel = CFNumberCreate( kCFAllocatorDefault, kCFNumberFloatType, &mFormat.mGamma );
	::CVBufferSetAttachment( pixelBuffer, kCVImageBufferGammaLevelKey, gammaLevel, kCVAttachmentMode_ShouldPropagate );
	::CFRelease( gammaLevel );
	::CVBufferSetAttachment( pixelBuffer, kCVImageBufferYCbCrMatrixKey, kCVImageBufferYCbCrMatrix_ITU_R_709_2, kCVAttachmentMode_ShouldPropagate );

	ICMValidTimeFlags validTimeFlags = kICMValidTime_DisplayTimeStampIsValid | kICMValidTime_DisplayDurationIsValid;
	ICMCompressionFrameOptionsRef frameOptions = NULL;
	long timeVal = static_cast<long>( duration * mFormat.mTimeBase );
	OSStatus err = ::ICMCompressionSessionEncodeFrame( mCompressionSession, pixelBuffer,
				mCurrentTimeValue, timeVal, validTimeFlags,
                frameOptions, NULL, NULL );
	mCurrentTimeValue += timeVal;
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

	ImageDescriptionHandle imageDescription = NULL;
	err = ICMCompressionSessionGetImageDescription( session, &imageDescription );
	if( ! err ) {
		Fixed gammaLevel = FloatToFixed( obj->mFormat.mGamma );
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
}

}

void MovieWriter::Obj::createCompressionSession()
{
	OSStatus err = noErr;
	::ICMEncodedFrameOutputRecord encodedFrameOutputRecord = {0};
	::ICMCompressionSessionOptionsRef sessionOptions = NULL;
	
	err = ::ICMCompressionSessionOptionsCreateCopy( NULL, mFormat.mOptions, &sessionOptions );
	if( err )
		goto bail;
	
	// We need durations when we store frames.
	err = ::ICMCompressionSessionOptionsSetDurationsNeeded( sessionOptions, true );
	if( err )
		goto bail;
	
	encodedFrameOutputRecord.encodedFrameOutputCallback = encodedFrameOutputCallback;
	encodedFrameOutputRecord.encodedFrameOutputRefCon = this;
	encodedFrameOutputRecord.frameDataAllocator = NULL;
	err = ::ICMCompressionSessionCreate( NULL, mWidth, mHeight, mFormat.mCodec, mFormat.mTimeBase,
			sessionOptions, NULL, &encodedFrameOutputRecord, &mCompressionSession );
	if( err )
		goto bail;

	::ICMCompressionSessionOptionsRelease( sessionOptions );
	return;

bail:
	if( sessionOptions )
		::ICMCompressionSessionOptionsRelease( sessionOptions );
	throw MovieWriterExc();
}

void MovieWriter::Obj::finish()
{
	if( mFinished )
		return;

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
		err = ::InsertMediaIntoTrack( mTrack, 0, 0, (TimeValue)::GetMediaDisplayDuration( mMedia ), fixed1 );
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

bool MovieWriter::getUserCompressionSettings( Format *result, ImageSourceRef imageSource )
{
	ComponentInstance stdCompression = 0;
	long scPreferences;
	ICMCompressionSessionOptionsRef sessionOptionsRef = NULL;
	ComponentResult err;

	startQuickTime();

	err = ::OpenADefaultComponent( ::StandardCompressionType, ::StandardCompressionSubType, &stdCompression );
	if( err || ( stdCompression == 0 ) )
		return false;

	// Indicates the client is ready to use the ICM compression session API to perform compression operations
	// StdCompression will disable frame reordering and multi pass encoding if this flag not set because the
	// older sequence APIs do not support these capabilities
	scPreferences = scAllowEncodingWithCompressionSession;

	// set the preferences we want
	err = ::SCSetInfo( stdCompression, ::scPreferenceFlagsType, &scPreferences );
	if( err ) {
	    if( stdCompression )
			::CloseComponent( stdCompression );
		return false;
	}

	// build a preview image
	GWorldPtr previewImageGWorld = 0;
	PixMapHandle previewImagePixMap = 0;
	if( imageSource ) {
		previewImageGWorld = qtime::createGWorld( imageSource );
		previewImagePixMap = ::GetGWorldPixMap( previewImageGWorld );
		if( ! ::LockPixels( previewImagePixMap ) ) {
			if( stdCompression )
				::CloseComponent( stdCompression );
			return false;
		}
		::SCSetTestImagePixMap( stdCompression, previewImagePixMap, NULL, scPreferScaling );
	}

	// display the standard compression dialog box
	err = ::SCRequestSequenceSettings( stdCompression );

	// before we do anything else, let's free up our preview image resources
	if( previewImagePixMap )
        if( ::GetPixelsState( previewImagePixMap ) & pixelsLocked )
            ::UnlockPixels( previewImagePixMap );
	if( previewImageGWorld )
		::DisposeGWorld( previewImageGWorld );

	// now process the result
	if( err ) {
	    if( stdCompression )
			::CloseComponent( stdCompression );
		return false;
	}

	// pull out the codec and quality
	::SCSpatialSettings spatialSettings;
	::SCGetInfo( stdCompression, scSpatialSettingsType, &spatialSettings );
	::CodecType codec = spatialSettings.codecType;
	::CodecQ quality = spatialSettings.spatialQuality;

	::SCTemporalSettings temporalSettings;
	::SCGetInfo( stdCompression, scTemporalSettingsType, &temporalSettings );

	// creates a compression session options object based on configured settings
	err = ::SCCopyCompressionSessionOptions( stdCompression, &sessionOptionsRef );
    if( stdCompression )
		::CloseComponent( stdCompression );

	*result = Format( sessionOptionsRef, static_cast<uint32_t>( codec ), quality / (float)codecLosslessQuality, FixedToFloat( temporalSettings.frameRate ) );

	return true;
}

} } // namespace cinder::qtime