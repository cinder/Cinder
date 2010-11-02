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

// None of this works in 64 bit on the mac or Windows. We'll need to move to QTKit on the mac.
#if ! defined( __LP64__ )

#include "cinder/qtime/QuickTime.h"
#include "cinder/qtime/QuickTimeUtils.h"
#include "cinder/Cinder.h"
#include "cinder/Utilities.h"

#include <sstream>

#if defined( CINDER_MAC )
	#include "cinder/app/App.h"
	#include <QTKit/QTKit.h>
	#include <QTKit/QTMovie.h>
	#include <CoreVideo/CoreVideo.h>
	#include <CoreVideo/CVBase.h>
#else
	#pragma push_macro( "__STDC_CONSTANT_MACROS" )
	#pragma push_macro( "_STDINT_H" )
		#undef __STDC_CONSTANT_MACROS
		#if _MSC_VER >= 1600 // VC10 or greater
			#define _STDINT_H
		#endif
		#include <QTML.h>
		#include <CVPixelBuffer.h>
		#include <ImageCompression.h>
		#include <Movies.h>
		#include <GXMath.h>
	#pragma pop_macro( "_STDINT_H" )
	#pragma pop_macro( "__STDC_CONSTANT_MACROS" )
	// this call is improperly defined as Mac-only in the headers
	extern "C" {
	EXTERN_API_C( OSStatus ) QTPixelBufferContextCreate( CFAllocatorRef, CFDictionaryRef, QTVisualContextRef* );
	}
#endif

using std::string;

namespace cinder { namespace qtime {

static GWorldPtr sDefaultGWorld;

::Movie openMovieFromUrl( const	Url &url );
::Movie openMovieFromPath( const std::string &path );

void startQuickTime()
{
	static bool initialized = false;
	
	if( initialized )
		return;
		
#if defined( CINDER_MSW )	
	::InitializeQTML( 0L );
#endif	
	::EnterMovies();
	initialized = true;

	// create a default GWorld which we will set in order to prevent NewMovieFromDataRef() from balking
	::Rect boundsRect;
	boundsRect.left = boundsRect.top = 0;
	boundsRect.right = boundsRect.bottom = 4;
	::QTNewGWorld( &sDefaultGWorld, k24BGRPixelFormat, &boundsRect, NULL, NULL, 0 );
}

float MovieBase::getPixelAspectRatio() const
{
	float aspectRatio = 1.0f;

	Track track = ::GetMovieIndTrackType( getObj()->mMovie, 1, VideoMediaType, movieTrackMediaType );
	if( track != NULL ) {
		Media media = ::GetTrackMedia(track);
		if( media ) {
			ImageDescriptionHandle idh;
			SampleDescriptionHandle sdh = (SampleDescriptionHandle)::NewHandle( 0 );			
			::GetMediaSampleDescription( media, 1, sdh );
			idh = (ImageDescriptionHandle)sdh;
			long count = 0;
			::CountImageDescriptionExtensionType( idh, 'pasp', &count );
			if( count >= 1 ) {
				PixelAspectRatioImageDescriptionExtension ext;
				::GetImageDescriptionExtension( idh, NULL, 'pasp', 1 );
				::ICMImageDescriptionGetProperty( idh, kQTPropertyClass_ImageDescription, kICMImageDescriptionPropertyID_PixelAspectRatio, sizeof(ext), &ext, NULL );
			
				aspectRatio = ext.hSpacing / (float)ext.vSpacing;
			}

			::DisposeHandle( (Handle)sdh );
		}
	}

	return aspectRatio;
}

	
// We shouldn't call an abstract virtual from the constructor (specifically getObj()), so we have a two-phase construction, an empty constructor and this
void MovieBase::initFromLoader( const MovieLoader &loader )
{
	startQuickTime();
	
	loader.waitForLoaded();
	getObj()->mMovie = loader.transferMovieHandle();

	init();
}

// We shouldn't call an abstract virtual from the constructor (specifically getObj()), so we have a two-phase construction, an empty constructor and this
void MovieBase::initFromPath( const string &path )
{
	startQuickTime();
	getObj()->mMovie = openMovieFromPath( path );
	init();
}

void MovieBase::initFromMemory( const void *data, size_t dataSize, const string &fileNameHint, const std::string &mimeTypeHint )
{
	startQuickTime();
	getObj()->mMovie = openMovieFromMemory( data, dataSize, fileNameHint, mimeTypeHint );
	init();
}

void MovieBase::initFromDataSource( DataSourceRef dataSourceRef, const std::string &mimeTypeHint )
{
	startQuickTime();
	if( dataSourceRef->isFilePath() ) { // try to use quicktime's native file handling if possible
		getObj()->mMovie = openMovieFromPath( dataSourceRef->getFilePath() );
		// no need to retain the data source
	}
	else if( dataSourceRef->isUrl() ) { // try to use quicktime's native Url handling if possible
		// Create a loader for this Url and then wait on it
		MovieLoader loader( dataSourceRef->getUrl() );
		loader.waitForLoaded();
		getObj()->mMovie = loader.transferMovieHandle();
		// no need to retain the data source
	}
	else { // we'll need to load from memory; and we'll rer to the data source to make sure it doesn't go away before the movie does
		Buffer buffer( dataSourceRef->getBuffer() );
		getObj()->mMovie = openMovieFromMemory( buffer.getData(), buffer.getDataSize(), dataSourceRef->getFilePathHint(), mimeTypeHint );	
		getObj()->mDataSource = dataSourceRef; // retain a reference to the dataSource so that it doesn't go away before we do
	}
	init();
}

void MovieBase::init()
{	
	getObj()->mLoaded = false;
	getObj()->mPlayable = false;
	getObj()->mFrameCount = -1;
	getObj()->mPlayingForward = true;
	getObj()->mLoop = false;
	getObj()->mPalindrome = false;
	getObj()->mFFTData = NULL;
	getObj()->mFFTNumChannels = getObj()->mFFTNumBandLevels = 0;
	getObj()->mVisualContext = 0;

	::Rect bounds;	
	::GetMovieNaturalBoundsRect( getObj()->mMovie, &bounds );
	getObj()->mWidth = bounds.right - bounds.left;
	getObj()->mHeight = bounds.bottom - bounds.top;

	getObj()->mDuration = ::GetMovieDuration( getObj()->mMovie ) / (float)::GetMovieTimeScale( getObj()->mMovie );

	//::SetMovieTimeValue( getObj()->mMovie, getStartTimeOfFirstSample() );
	::GoToBeginningOfMovie( getObj()->mMovie );
}

MovieBase::Obj::Obj()
{
	mVisualContext = 0;
	mNewFrameCallback = 0;
	mVisualContext = 0;
	mMovie = 0;
}

MovieBase::Obj::~Obj()
{
	lock();
		if( mVisualContext ) {
			::SetMovieVisualContext( mMovie, NULL );
			::QTVisualContextRelease( (QTVisualContextRef)mVisualContext );
		}

		if( mMovie )
			::DisposeMovie( mMovie );
	unlock();
}

void MovieBase::Obj::prepareForDestruction()
{
}

// checks movie's current loadstate and updates mLoaded and mPlayable accordingly
void MovieBase::updateLoadState()
{
	::MoviesTask( getObj()->mMovie, 0 );
	long state;
	state = ::GetMovieLoadState( getObj()->mMovie );
	if( state == kMovieLoadStateError )
		throw QuickTimeErrorLoadingExc();
	if( ( ! getObj()->mLoaded ) && ( state >= kMovieLoadStateLoaded ) ) {
		getObj()->mLoaded = true;
	}
	if( state >= kMovieLoadStatePlaythroughOK ) {
		getObj()->mPlayable = true;
	}
}

//! \return The movie has loaded and buffered enough to playback without interruption
bool MovieBase::checkPlayable()
{
	if( getObj()->mPlayable ) return true;
	updateLoadState();
	return getObj()->mPlayable;
}

//! \see http://developer.apple.com/mac/library/qa/qa2001/qa1262.html Technical Q&A QA1262	
float MovieBase::getFramerate() const
{
	int32_t ct = getNumFrames();
	if( getObj()->mDuration )
		return ct / getObj()->mDuration;
	else
		return 0;
}

int32_t	MovieBase::getNumFrames() const
{
	if( getObj()->mFrameCount < 0 ) {
		getObj()->mFrameCount = countFrames( getObj()->mMovie );
	}
	
	return getObj()->mFrameCount;
}

//! \see http://developer.apple.com/qa/qtmtb/qtmtb17.html Technical Q&A QTMTB17
bool MovieBase::hasAlpha() const
{
	for( long trackIdx = 1; trackIdx <= ::GetMovieTrackCount( getObj()->mMovie ); ++trackIdx ) {
		Track track = ::GetMovieIndTrack( getObj()->mMovie, trackIdx );
		Media media = ::GetTrackMedia( track );
		OSType      dwType;
		::GetMediaHandlerDescription( media, &dwType, 0, 0 );
		if( ( dwType == VideoMediaType ) || ( dwType == MPEGMediaType ) ) {
			ImageDescriptionHandle imageDescH = (ImageDescriptionHandle)::NewHandleClear(sizeof(ImageDescription));
			::GetMediaSampleDescription( media, 1, (SampleDescriptionHandle)imageDescH );
			OSErr err = ::GetMoviesError();
			if( err != noErr ) {
				DisposeHandle( (Handle)imageDescH );
				continue;
			}
			if( imageDescH ) {
				bool result = (*imageDescH)->depth == 32;
				::DisposeHandle( (Handle)imageDescH );
				return result;
			}
		}
	}
	
	return false;
}

bool MovieBase::hasVisuals() const
{
	Track theTrack = ::GetMovieIndTrackType( getObj()->mMovie, 1, VisualMediaCharacteristic, movieTrackCharacteristic );
	return theTrack != 0;
}

bool MovieBase::hasAudio() const
{
	Track theTrack = ::GetMovieIndTrackType( getObj()->mMovie, 1, AudioMediaCharacteristic, movieTrackCharacteristic );
	return theTrack != 0;
}

float MovieBase::getCurrentTime() const
{
	return ::GetMovieTime( getObj()->mMovie, NULL ) / (float)::GetMovieTimeScale( getObj()->mMovie );
}

void MovieBase::seekToTime( float seconds )
{
	::SetMovieTimeValue( getObj()->mMovie, ::TimeValue( seconds * ::GetMovieTimeScale( getObj()->mMovie ) ) );
}

void MovieBase::seekToFrame( int frame )
{
	long        frameCount = 0;
	TimeValue   curMovieTime = 0, lastGoodTime = 0;
	
	OSType types[] = { VisualMediaCharacteristic };
	while( ( frameCount < frame ) && ( curMovieTime >= 0 ) ) {
		::GetMovieNextInterestingTime( getObj()->mMovie, nextTimeStep, 1, types, curMovieTime, fixed1, &curMovieTime, NULL );
		frameCount++;
		if( curMovieTime >= 0 )
			lastGoodTime = curMovieTime;
	}
	::SetMovieTimeValue( getObj()->mMovie, lastGoodTime );
	::MoviesTask( getObj()->mMovie, 0 );
}

void MovieBase::seekToStart()
{
	::GoToBeginningOfMovie( getObj()->mMovie );
}

void MovieBase::seekToEnd()
{
	::GoToEndOfMovie( getObj()->mMovie );
}

void MovieBase::setActiveSegment( float startTime, float duration )
{
	getObj()->lock();
		//qtime can blow up if we don't kill the looping before modifying the active segment
		::TimeBase timeBase;
		long flags = 0L, oldFlags;
		timeBase = ::GetMovieTimeBase( getObj()->mMovie );
		oldFlags = flags = ::GetTimeBaseFlags( timeBase );
		flags &= ~loopTimeBase;
		flags &= ~palindromeLoopTimeBase;		
		::SetTimeBaseFlags( timeBase, flags );
		
		::SetMovieActiveSegment( getObj()->mMovie, ::TimeValue( startTime * ::GetMovieTimeScale( getObj()->mMovie ) ), ::TimeValue( duration * ::GetMovieTimeScale( getObj()->mMovie ) ) );
		::SetMovieTimeValue( getObj()->mMovie, ::TimeValue( startTime * ::GetMovieTimeScale( getObj()->mMovie ) ) );

		::SetTimeBaseFlags( timeBase, oldFlags );

	getObj()->unlock();
}

void MovieBase::resetActiveSegment()
{
	getObj()->lock();
		::SetMovieActiveSegment( getObj()->mMovie, -1, -1 );
	getObj()->unlock();		
}
	
int32_t MovieBase::countFrames( ::Movie theMovie ) 
{  
	long        frameCount = 0;
	TimeValue   curMovieTime = 0;

	OSType types[] = { VisualMediaCharacteristic }; 
	
	while( curMovieTime >= 0 ) {
		::GetMovieNextInterestingTime( theMovie, nextTimeStep, 1, types, curMovieTime, fixed1, &curMovieTime, NULL );
		frameCount++;
	}

	return frameCount - 1; // there's an extra time step at the end of the movie
}

void MovieBase::setLoop( bool aLoop, bool aPalindrome )
{
	getObj()->mLoop = aLoop;
	if( ! aLoop )
		getObj()->mPalindrome = false;
	getObj()->mPalindrome = aPalindrome;

	::TimeBase timeBase;
	long flags = 0L;
	timeBase = ::GetMovieTimeBase( getObj()->mMovie );
	flags = ::GetTimeBaseFlags( timeBase );
	if( getObj()->mLoop ) {
		if( getObj()->mPalindrome ) {
			flags |= loopTimeBase;
			flags |= palindromeLoopTimeBase;		
			::SetMoviePlayHints( getObj()->mMovie, hintsLoop, hintsLoop );
			::SetMoviePlayHints( getObj()->mMovie, hintsPalindrome, hintsPalindrome );
		}
		else {
			flags |= loopTimeBase;
			flags &= ~palindromeLoopTimeBase;		
			::SetMoviePlayHints( getObj()->mMovie, hintsLoop, hintsLoop );
			::SetMoviePlayHints( getObj()->mMovie, 0, hintsPalindrome );
		}
	}
	else {
		flags &= ~loopTimeBase;
		flags &= ~palindromeLoopTimeBase;		
		::SetMoviePlayHints( getObj()->mMovie, 0, hintsLoop | hintsPalindrome );
	}

	::SetTimeBaseFlags( timeBase, flags );
}

TimeValue MovieBase::getStartTimeOfFirstSample() const
{
	OSType     types[] = { VisualMediaCharacteristic, AudioMediaCharacteristic };

	TimeValue theTime;
	::GetMovieNextInterestingTime( getObj()->mMovie, nextTimeStep + nextTimeEdgeOK, 2, types, (TimeValue)0, fixed1, &theTime, NULL );
	return theTime;
}

void MovieBase::stepForward()
{
	TimeValue curMovieTime;
	
	TimeValue oldTime = ::GetMovieTime( getObj()->mMovie, NULL );
	OSType types[] = { VisualMediaCharacteristic/*, AudioMediaCharacteristic*/ };
	::GetMovieNextInterestingTime( getObj()->mMovie, nextTimeStep, 1, types, oldTime, fixed1, &curMovieTime, NULL );
	if( curMovieTime != -1 ) { // hit the end
		::SetMovieTimeValue( getObj()->mMovie, curMovieTime );
	}
	
	::MoviesTask( getObj()->mMovie, 0 );
}

void MovieBase::stepBackward()
{
	TimeValue curMovieTime;
	
	TimeValue oldTime = ::GetMovieTime( getObj()->mMovie, NULL );
	OSType types[] = { VisualMediaCharacteristic/*, AudioMediaCharacteristic*/ };
	::GetMovieNextInterestingTime( getObj()->mMovie, nextTimeStep, 1, types, oldTime, FixMul(Long2Fix(-1), fixed1), &curMovieTime, NULL );
	if( curMovieTime != -1 ) { // hit the end
		::SetMovieTimeValue( getObj()->mMovie, curMovieTime );
	}
	
	::MoviesTask( getObj()->mMovie, 0 );
}

void MovieBase::setRate( float rate )
{
	::SetMovieRate( getObj()->mMovie, FloatToFixed( rate ) );
}

void MovieBase::updateFrame()
{
	getObj()->lock();

	::MoviesTask( getObj()->mMovie, 0 );
	if( (QTVisualContextRef)getObj()->mVisualContext ) {
		::QTVisualContextTask( (QTVisualContextRef)getObj()->mVisualContext );
		if( ::QTVisualContextIsNewImageAvailable( (QTVisualContextRef)getObj()->mVisualContext, nil ) ) {		
			getObj()->releaseFrame();
			
			CVImageBufferRef newImageRef = NULL;
			long tv = ::GetMovieTime( getObj()->mMovie, NULL );
			OSStatus err = ::QTVisualContextCopyImageForTime( (QTVisualContextRef)getObj()->mVisualContext, kCFAllocatorDefault, NULL, &newImageRef );
			if( ( err == noErr ) && newImageRef )
				getObj()->newFrame( newImageRef );
			if( getObj()->mNewFrameCallback && newImageRef ) {
				
				/*CVAttachmentMode mode;
				CFDictionaryRef attachments = CVBufferGetAttachments( newImageRef, kCVAttachmentMode_ShouldPropagate );
				NSLog( @"attach: %@", attachments );
				CFDictionaryRef timedict = (CFDictionaryRef)::CVBufferGetAttachment( newImageRef, kCVBufferMovieTimeKey, &mode );
				long tv;
				if( timedict ) {
					CFNumberRef time = (CFNumberRef)::CFDictionaryGetValue( timedict, kCVBufferTimeValueKey );
					::CFNumberGetValue( time, kCFNumberLongType, &tv );
				}
				else
					tv = 0;*/
				(*getObj()->mNewFrameCallback)( tv, getObj()->mNewFrameCallbackRefcon );
			}
		}
	}

	getObj()->unlock();
}

void MovieBase::setVolume( float volume )
{
	float clamped = math<float>::clamp( volume, 0, 1 );
	short s = static_cast<short>( clamped * 255 );
	::SetMovieVolume( getObj()->mMovie, s );
}

float MovieBase::getVolume() const
{
	return ::GetMovieVolume( getObj()->mMovie ) / 255.0f;
}

void MovieBase::setupFft( FourCharCode code, uint32_t bandNum, uint8_t channelNum )
{
	getObj()->mFFTFourCharCode = code;

	// free an existing FFTData
	if( getObj()->mFFTData )
		free( (void*)getObj()->mFFTData );
	
	// these values all imply failure
	getObj()->mFFTData = NULL;
	getObj()->mFFTNumBandLevels = 0;
	getObj()->mFFTNumChannels = 0;

	// call this to establish metering
	UInt32 bandLevels = bandNum;
	OSStatus err = ::SetMovieAudioFrequencyMeteringNumBands( getObj()->mMovie, getObj()->mFFTFourCharCode, &bandLevels );
	if( err ) {
		throw QuickTimeExcFft();
	}
	else {
		getObj()->mFFTData = (QTAudioFrequencyLevels*) malloc( sizeof(QTAudioFrequencyLevels) + sizeof(Float32) * ( channelNum * bandNum - 1 ) );
		
		if( getObj()->mFFTData == NULL ) {
			throw QuickTimeExcFft();
		}

		getObj()->mFFTNumBandLevels = bandNum;
		getObj()->mFFTNumChannels = channelNum;		
		getObj()->mFFTData->numChannels = getObj()->mFFTNumChannels;
		getObj()->mFFTData->numFrequencyBands = getObj()->mFFTNumBandLevels;
	}
}

void MovieBase::setupMonoFft( uint32_t bandNum )
{
	setupFft( kQTAudioMeter_MonoMix, bandNum, 1 );
}

void MovieBase::setupStereoFft( uint32_t bandNum )
{
	setupFft( kQTAudioMeter_StereoMix, bandNum, 2 );
}

void MovieBase::setupMultiChannelFft( uint32_t bandNum )
{
	// first we have to figure out how many channels there are
	AudioChannelLayout *layout = NULL;
    UInt32 size = 0;
    UInt32 numChannels = 0;
    OSStatus err;
	
	err = QTGetMoviePropertyInfo( getObj()->mMovie, kQTPropertyClass_Audio, kQTAudioPropertyID_DeviceChannelLayout, NULL, &size, NULL );

    throw QuickTimeExcFft();

    // allocate memory for the device layout
    layout = (AudioChannelLayout*)calloc(1, size);
    if( NULL == layout ) {
	    throw QuickTimeExcFft();
    } 

    // get the device layout from the movie
    err = QTGetMovieProperty( getObj()->mMovie, kQTPropertyClass_Audio, kQTAudioPropertyID_DeviceChannelLayout, size, layout, NULL );
    if( err )
		throw QuickTimeExcFft();

    // now get the number of channels
	numChannels = (layout->mChannelLayoutTag ==
                   kAudioChannelLayoutTag_UseChannelDescriptions) ?
                   layout->mNumberChannelDescriptions :
                   AudioChannelLayoutTag_GetNumberOfChannels(layout->mChannelLayoutTag);
    free( layout );	

	setupFft( kQTAudioMeter_DeviceMix, bandNum, (uint8_t)numChannels );
}

uint32_t MovieBase::getNumFftBands() const
{
	return getObj()->mFFTNumBandLevels;
}

uint32_t MovieBase::getNumFftChannels() const
{
	return getObj()->mFFTNumChannels;
}
	
float* MovieBase::getFftData() const
{
	if( getObj()->mFFTData != NULL ) {
		OSStatus err = GetMovieAudioFrequencyLevels( getObj()->mMovie, getObj()->mFFTFourCharCode, getObj()->mFFTData );
		if( err ) {
			throw QuickTimeExcFft();
		}
		
		return getObj()->mFFTData->level;
	}
	else {
		throw QuickTimeExcFft();
	}
}

bool MovieBase::isPlaying() const
{
	return ::GetMovieRate( getObj()->mMovie ) != 0;
}

bool MovieBase::isDone() const
{
	return ::IsMovieDone( getObj()->mMovie ) != 0;
}

void MovieBase::play()
{
	::StartMovie( getObj()->mMovie );
}

void MovieBase::stop()
{
	::StopMovie( getObj()->mMovie );
}

void MovieBase::setNewFrameCallback( void(*aNewFrameCallback)( long, void * ), void *aNewFrameCallbackRefcon )
{
	getObj()->mNewFrameCallback = aNewFrameCallback;
	getObj()->mNewFrameCallbackRefcon = aNewFrameCallbackRefcon;
}

/////////////////////////////////////////////////////////////////////////////////
// MovieSurface

MovieSurface::Obj::~Obj()
{
	// see note on prepareForDestruction()
	prepareForDestruction();
}

MovieSurface::MovieSurface( const std::string &path )
	: MovieBase(), mObj( new Obj() )
{
	MovieBase::initFromPath( path );
	allocateVisualContext();
}

MovieSurface::MovieSurface( const MovieLoader &loader )
	: MovieBase(), mObj( new Obj() )
{
	MovieBase::initFromLoader( loader );
	allocateVisualContext();
}

MovieSurface::MovieSurface( const void *data, size_t dataSize, const std::string &fileNameHint, const std::string &mimeTypeHint )
	: MovieBase(), mObj( new Obj() )
{
	MovieBase::initFromMemory( data, dataSize, fileNameHint, mimeTypeHint );
	allocateVisualContext();
}

MovieSurface::MovieSurface( DataSourceRef dataSource, const std::string mimeTypeHint )
	: MovieBase(), mObj( new Obj() )
{
	MovieBase::initFromDataSource( dataSource, mimeTypeHint );
	allocateVisualContext();
}

void MovieSurface::allocateVisualContext()
{
	if( ! hasVisuals() )
		return;

	CFMutableDictionaryRef visualContextOptions = initQTVisualContextOptions( getObj()->mWidth, getObj()->mHeight, hasAlpha() );
	OSStatus status = ::QTPixelBufferContextCreate( kCFAllocatorDefault, visualContextOptions, &(getObj()->mVisualContext) );

	if( status == noErr )
		::SetMovieVisualContext( getObj()->mMovie, getObj()->mVisualContext );
	else
		getObj()->mVisualContext = 0;
}

void MovieSurface::Obj::newFrame( CVImageBufferRef cvImage )
{
	CVPixelBufferRef imgRef = reinterpret_cast<CVPixelBufferRef>( cvImage );
	if( imgRef ) {
		mSurface = convertCVPixelBufferToSurface( imgRef );
	}
	else
		mSurface.reset();
}

void MovieSurface::Obj::releaseFrame()
{
	mSurface.reset();
}

Surface MovieSurface::getSurface()
{
	updateFrame();
	
	mObj->lock();
		Surface result = mObj->mSurface;
	mObj->unlock();
	
	return result;
}

/////////////////////////////////////////////////////////////////////////////////
// MovieGl
MovieGl::Obj::Obj()
	: MovieBase::Obj()
{
}

MovieGl::Obj::~Obj()
{
	// see note on prepareForDestruction()
	prepareForDestruction();
}

MovieGl::MovieGl( const MovieLoader &loader )
	: MovieBase(), mObj( new Obj() )
{
	MovieBase::initFromLoader( loader );
	allocateVisualContext();
}

MovieGl::MovieGl( const std::string &path )
	: MovieBase(), mObj( new Obj() )
{
	MovieBase::initFromPath( path );
	allocateVisualContext();
}

MovieGl::MovieGl( const void *data, size_t dataSize, const std::string &fileNameHint, const std::string &mimeTypeHint )
	: MovieBase(), mObj( new Obj() )
{
	MovieBase::initFromMemory( data, dataSize, fileNameHint, mimeTypeHint );
	allocateVisualContext();	
}

MovieGl::MovieGl( DataSourceRef dataSource, const std::string mimeTypeHint )
	: MovieBase(), mObj( new Obj() )
{
	MovieBase::initFromDataSource( dataSource, mimeTypeHint );
	allocateVisualContext();
}

void MovieGl::allocateVisualContext()
{
#if defined( CINDER_MAC )
	CGLContextObj cglContext = app::App::get()->getRenderer()->getCglContext();
	CGLPixelFormatObj cglPixelFormat = ::CGLGetPixelFormat( cglContext );

	// Creates a new OpenGL texture context for a specified OpenGL context and pixel format
	::QTOpenGLTextureContextCreate( kCFAllocatorDefault, cglContext, cglPixelFormat, NULL, (QTVisualContextRef*)&getObj()->mVisualContext );
	::SetMovieVisualContext( getObj()->mMovie, (QTVisualContextRef)getObj()->mVisualContext );
#else
	CFMutableDictionaryRef visualContextOptions = initQTVisualContextOptions( getObj()->mWidth, getObj()->mHeight, hasAlpha() );
	::QTPixelBufferContextCreate( kCFAllocatorDefault, visualContextOptions, &(getObj()->mVisualContext) );

	::SetMovieVisualContext( getObj()->mMovie, getObj()->mVisualContext );
#endif
}

#if defined( CINDER_MAC )
static void CVOpenGLTextureDealloc( void *refcon )
{
	CVOpenGLTextureRelease( (CVImageBufferRef)(refcon) );
}

#endif // defined( CINDER_MAC )

void MovieGl::Obj::releaseFrame()
{
	mTexture.reset();
}

void MovieGl::Obj::newFrame( CVImageBufferRef cvImage )
{
#if defined( CINDER_MAC )
	CVOpenGLTextureRef imgRef = reinterpret_cast<CVOpenGLTextureRef>( cvImage );
	GLenum target = CVOpenGLTextureGetTarget( imgRef );
	GLuint name = CVOpenGLTextureGetName( imgRef );
	bool flipped = ! CVOpenGLTextureIsFlipped( imgRef );
	mTexture = gl::Texture( target, name, mWidth, mHeight, true );
	Vec2f t0, lowerRight, t2, upperLeft;
	CVOpenGLTextureGetCleanTexCoords( imgRef, t0, lowerRight, t2, upperLeft );
	mTexture.setCleanTexCoords( std::max( upperLeft.x, lowerRight.x ), std::max( upperLeft.y, lowerRight.y ) );
	mTexture.setFlipped( flipped );
	mTexture.setDeallocator( CVOpenGLTextureDealloc, imgRef );
#else
	// on Windows this is actually a CVPixelBufferRef, which we will convert into a texture
	CVPixelBufferRef imgRef = reinterpret_cast<CVPixelBufferRef>( cvImage );
	CVPixelBufferLockBaseAddress( imgRef, 0 );
	uint8_t *ptr = reinterpret_cast<uint8_t*>( CVPixelBufferGetBaseAddress( imgRef ) );
	int32_t rowBytes = CVPixelBufferGetBytesPerRow( imgRef );
	OSType type = CVPixelBufferGetPixelFormatType( imgRef );
	size_t width = CVPixelBufferGetWidth( imgRef );
	size_t height = CVPixelBufferGetHeight( imgRef );
	SurfaceChannelOrder sco = SurfaceChannelOrder::BGRA;
	if( type == k24RGBPixelFormat )
		sco = SurfaceChannelOrder::RGB;
	else if( type == k32ARGBPixelFormat )
		sco = SurfaceChannelOrder::ARGB;
	else if( type == k24BGRPixelFormat )
		sco = SurfaceChannelOrder::BGR;
	else if( type == k32BGRAPixelFormat )
		sco = SurfaceChannelOrder::BGRA;
	
	if( ! mTextureCache ) {
		gl::Texture::Format format;
		format.setTargetRect();
		mTextureCache = gl::TextureCache( Surface8u( ptr, width, height, rowBytes, sco ), format );
	}
	mTexture = mTextureCache.cache( Surface8u( ptr, width, height, rowBytes, sco ) );
	
	::CVBufferRelease( imgRef );
#endif
}

const gl::Texture MovieGl::getTexture()
{
	updateFrame();

	mObj->lock();
		gl::Texture result = mObj->mTexture;
	mObj->unlock();

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MovieLoader
MovieLoader::MovieLoader( const Url &url )
	: mObj( new Obj( url ) )
{
}

bool MovieLoader::checkLoaded() const
{
	if( ! mObj->mLoaded )
		updateLoadState();
	return mObj->mLoaded;
}

bool MovieLoader::checkPlayable() const
{
	if( ! mObj->mPlayable )
		updateLoadState();
	return mObj->mPlayable;
}

bool MovieLoader::checkPlaythroughOk() const
{
	if( ! mObj->mPlaythroughOK )
		updateLoadState();
	return mObj->mPlaythroughOK;
}

void MovieLoader::waitForLoaded() const
{
	while( ! mObj->mLoaded ) {
		cinder::sleep( 250 ); // its documentation says not to call GetMovieLoadState more often than once a quarter-second
		updateLoadState();
	}
}

void MovieLoader::waitForPlayable() const
{
	while( ! mObj->mPlayable ) {
		cinder::sleep( 250 ); // its documentation says not to call GetMovieLoadState more often than once a quarter-second
		updateLoadState();
	}
}

void MovieLoader::waitForPlaythroughOk() const
{
	while( ! mObj->mPlaythroughOK ) {
		cinder::sleep( 250 ); // its documentation says not to call GetMovieLoadState more often than once a quarter-second
		updateLoadState();
	}
}

void MovieLoader::updateLoadState() const
{
	::MoviesTask( mObj->mMovie, 0 );
	long state = ::GetMovieLoadState( mObj->mMovie );
	if( state == kMovieLoadStateError )
		throw QuickTimeErrorLoadingExc();
	mObj->mLoaded = ( state >= kMovieLoadStateLoaded );
	mObj->mPlayable = ( state >= kMovieLoadStatePlayable );
	mObj->mPlaythroughOK = ( state >= kMovieLoadStatePlaythroughOK );
}
	
MovieLoader::Obj::Obj( const Url &url )
	: mUrl( url )
{
	startQuickTime();

	mMovie = openMovieFromUrl( url );
	
	mLoaded = mPlayable = mPlaythroughOK = false;
	mOwnsMovie = true;
};

MovieLoader::Obj::~Obj()
{
	if( mOwnsMovie )
		::DisposeMovie( mMovie );
}

int32_t getQuickTimeVersion()
{
	SInt32 result = 0;
	
#if defined( CINDER_MSW )
	if( InitializeQTML( 0L ) == noErr ) {
#endif
		OSErr error;

		error = ::Gestalt( gestaltQuickTime, &result );
		if( error != noErr )
			return 0;
#if defined( CINDER_MSW )				
		TerminateQTML();
	}
	else // error means QuickTime is unavailable
		return 0;
#endif
	return static_cast<int32_t>( result );
}

std::string getQuickTimeVersionString()
{
	int32_t v = getQuickTimeVersion();
	if( ! v ) // 0 implies not installed, so return empty string
		return std::string();
	
	// 0x05020000 -> 5.0.2   5: bits 31 to 24    0: bits 23 to 20    2: 19 to 16
	int a = ( v & 0xFF000000 ) >> 24;
	int b = ( ( v & 0x00F00000 ) >> 20 );
	int c = ( ( v & 0x000F0000 ) >> 16 );
	std::stringstream ss;
	ss << a << "." << b;
	if( c ) 
		ss << "." << c;
	return ss.str();
}

void quickTimeTask()
{
#if defined( CINDER_MSW )
	::MoviesTask( 0, 0 );
#endif
}

} /* namespace qtime */ } /* namespace cinder */

#endif // ! defined( __LP64__ )