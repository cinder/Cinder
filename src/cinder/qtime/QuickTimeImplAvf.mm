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

#include "cinder/Cinder.h"
#include <AvailabilityMacros.h>

// This path is used on iOS or Mac OS X 10.8+
#if defined( CINDER_COCOA_TOUCH ) || ( defined( CINDER_MAC ) && ( MAC_OS_X_VERSION_MIN_REQUIRED >= 1080 ) )

#include "cinder/gl/platform.h"
#include "cinder/app/AppBase.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Url.h"

#if defined( CINDER_COCOA )
	#import <AVFoundation/AVFoundation.h>
	#if defined( CINDER_COCOA_TOUCH )
		#import <CoreVideo/CoreVideo.h>
	#else
		#import <CoreVideo/CVDisplayLink.h>
	#endif
#endif

#include "cinder/qtime/QuickTimeImplAvf.h"
#include "cinder/qtime/AvfUtils.h"

////////////////////////////////////////////////////////////////////////
//
// TODO: use global time from the system clock
// TODO: setup CADisplayLink for iOS, remove CVDisplayLink callback on OSX
// TODO: test operations for thread-safety -- add/remove locks as necessary
//
////////////////////////////////////////////////////////////////////////

static void* AVPlayerItemStatusContext = &AVPlayerItemStatusContext;

@interface MovieDelegate : NSObject<AVPlayerItemOutputPullDelegate> {
	cinder::qtime::MovieResponder* responder;
}

- (id)initWithResponder:(cinder::qtime::MovieResponder*)player;
- (void)playerReady;
- (void)playerItemDidReachEndCallback;
- (void)playerItemDidNotReachEndCallback;
- (void)playerItemTimeJumpedCallback;
#if defined( CINDER_COCOA_TOUCH )
- (void)displayLinkCallback:(CADisplayLink*)sender;
#elif defined( CINDER_COCOA )
- (void)displayLinkCallback:(CVDisplayLinkRef*)sender;
#endif
- (void)outputSequenceWasFlushed:(AVPlayerItemOutput *)output;

@end


@implementation MovieDelegate

- (void)dealloc
{
	[super dealloc];
}

- (id)init
{
	self = [super init];
	self->responder = nil;
	return self;
}

- (id)initWithResponder:(cinder::qtime::MovieResponder*)player
{
	self = [super init];
	self->responder = player;
	return self;
}

- (void)playerReady
{
	self->responder->playerReadyCallback();
}

- (void)playerItemDidReachEndCallback
{
	self->responder->playerItemDidReachEndCallback();
}

- (void)playerItemDidNotReachEndCallback
{
	self->responder->playerItemDidNotReachEndCallback();
}

- (void)playerItemTimeJumpedCallback
{
	self->responder->playerItemTimeJumpedCallback();
}

- (void)observeValueForKeyPath:(NSString*)keyPath ofObject:(id)object change:(NSDictionary*)change context:(void*)context
{
	if (context == AVPlayerItemStatusContext) {
		AVPlayerItem* player_item = (AVPlayerItem*)object;
		AVPlayerItemStatus status = [player_item status];
		switch (status) {
			case AVPlayerItemStatusUnknown:
				//ci::app::console() << "AVPlayerItemStatusUnknown" << std::endl;
				break;
			case AVPlayerItemStatusReadyToPlay:
				[self playerReady];
				break;
			case AVPlayerItemStatusFailed:
				//ci::app::console() << "AVPlayerItemStatusFailed" << std::endl;
				break;
		}
	}
	else {
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
	}
}

#pragma mark - CADisplayLink Callback

#if defined( CINDER_COCOA_TOUCH )
- (void)displayLinkCallback:(CADisplayLink*)sender
#elif defined( CINDER_COCOA )
- (void)displayLinkCallback:(CVDisplayLinkRef*)sender
#endif
{
	ci::app::console() << "displayLinkCallback" << std::endl;
	
	/*
	 CMTime outputItemTime = kCMTimeInvalid;
	 
	 // Calculate the nextVsync time which is when the screen will be refreshed next.
	 CFTimeInterval nextVSync = ([sender timestamp] + [sender duration]);
	 
	 outputItemTime = [[self videoOutput] itemTimeForHostTime:nextVSync];
	 
	 if ([[self videoOutput] hasNewPixelBufferForItemTime:outputItemTime]) {
	 CVPixelBufferRef pixelBuffer = NULL;
	 pixelBuffer = [[self videoOutput] copyPixelBufferForItemTime:outputItemTime itemTimeForDisplay:NULL];
	 
	 [[self playerView] displayPixelBuffer:pixelBuffer];
	 }
	 */
}

- (void)outputSequenceWasFlushed:(AVPlayerItemOutput *)output
{
	self->responder->outputSequenceWasFlushedCallback(output);
}

@end


// this has a conflict with Boost 1.53, so instead just declare the symbol extern
namespace cinder {
	extern void sleep( float milliseconds );
}

namespace cinder { namespace qtime {
	
MovieBase::MovieBase()
:	mPlayer( nil ),
	mPlayerItem( nil ),
	mAsset( nil ),
	mPlayerVideoOutput( nil ),
	mPlayerDelegate( nil ),
	mResponder( nullptr ),
	mAssetLoaded( false )
{
	init();
}

MovieBase::~MovieBase()
{
	// remove all observers
	removeObservers();
	
	// release resources for AVF objects.
	if( mPlayer ) {
		[mPlayer cancelPendingPrerolls];
		[mPlayer release];
	}
	
	if( mAsset ) {
		[mAsset cancelLoading];
		[mAsset release];
	}

	if( mPlayerVideoOutput ) {
		[mPlayerVideoOutput release];
	}
}
	
float MovieBase::getPixelAspectRatio() const
{
	float pixelAspectRatio = 1.0;
	
	if( ! mAsset )
		return pixelAspectRatio;
	
	NSArray* video_tracks = [mAsset tracksWithMediaType:AVMediaTypeVideo];
	if( video_tracks ) {
		CMFormatDescriptionRef format_desc = NULL;
		NSArray* descriptions_arr = [[video_tracks firstObject] formatDescriptions];
		if ([descriptions_arr count] > 0)
			format_desc = (CMFormatDescriptionRef)[descriptions_arr firstObject];
		
		CGSize size;
		if (format_desc)
			size = CMVideoFormatDescriptionGetPresentationDimensions(format_desc, false, false);
		else
			size = [[video_tracks firstObject] naturalSize];
		
		CFDictionaryRef pixelAspectRatioDict = (CFDictionaryRef) CMFormatDescriptionGetExtension(format_desc, kCMFormatDescriptionExtension_PixelAspectRatio);
		if (pixelAspectRatioDict) {
			CFNumberRef horizontal = (CFNumberRef) CFDictionaryGetValue(pixelAspectRatioDict, kCMFormatDescriptionKey_PixelAspectRatioHorizontalSpacing);//, AVVideoPixelAspectRatioHorizontalSpacingKey,
			CFNumberRef vertical = (CFNumberRef) CFDictionaryGetValue(pixelAspectRatioDict, kCMFormatDescriptionKey_PixelAspectRatioVerticalSpacing);//, AVVideoPixelAspectRatioVerticalSpacingKey,
			float x_value, y_value;
			if (horizontal && vertical) {
				if (CFNumberGetValue(horizontal, kCFNumberFloat32Type, &x_value) &&
					CFNumberGetValue(vertical, kCFNumberFloat32Type, &y_value))
				{
					pixelAspectRatio = x_value / y_value;
				}
			}
		}
	}
	
	return pixelAspectRatio;
}

bool MovieBase::checkPlaythroughOk()
{
	mPlayThroughOk = [mPlayerItem isPlaybackLikelyToKeepUp];
	
	return mPlayThroughOk;
}

int32_t MovieBase::getNumFrames()
{
	if( mFrameCount <= 0 )
		mFrameCount = countFrames();
	
	return mFrameCount;
}

bool MovieBase::checkNewFrame()
{
	if( ! mPlayer || ! mPlayerVideoOutput )
		return false;
	
	if( mPlayerVideoOutput )
		return [mPlayerVideoOutput hasNewPixelBufferForItemTime:[mPlayer currentTime]];
	else
		return false;
}

float MovieBase::getCurrentTime() const
{
	if( ! mPlayer )
		return -1.0f;
	
	return CMTimeGetSeconds([mPlayer currentTime]);
}

void MovieBase::seekToTime( float seconds )
{
	if( ! mPlayer )
		return;
	
	CMTime seek_time = CMTimeMakeWithSeconds(seconds, [mPlayer currentTime].timescale);
	[mPlayer seekToTime:seek_time toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero];
}

void MovieBase::seekToFrame( int frame )
{
	if( ! mPlayer )
		return;
	
	CMTime currentTime = [mPlayer currentTime];
	CMTime oneFrame = CMTimeMakeWithSeconds(1.0 / mFrameRate, currentTime.timescale);
	CMTime startTime = kCMTimeZero;
	CMTime addedFrame = CMTimeMultiply(oneFrame, frame);
	CMTime added = CMTimeAdd(startTime, addedFrame);
	
	[mPlayer seekToTime:added toleranceBefore:kCMTimeZero toleranceAfter:kCMTimeZero];
}

void MovieBase::seekToStart()
{
	if( ! mPlayer )
		return;
	
	[mPlayer seekToTime:kCMTimeZero];
}

void MovieBase::seekToEnd()
{
	if( ! mPlayer || ! mPlayerItem )
		return;
	
	if( mPlayingForward ) {
		[mPlayer seekToTime:[mPlayerItem forwardPlaybackEndTime]];
	}
	else {
		[mPlayer seekToTime:[mPlayerItem reversePlaybackEndTime]];
	}
}

void MovieBase::setActiveSegment( float startTime, float duration )
{
	if( ! mPlayer || ! mPlayerItem )
		return;
	
	int32_t scale = [mPlayer currentTime].timescale;
	CMTime cm_start = CMTimeMakeWithSeconds(startTime, scale);
	CMTime cm_duration = CMTimeMakeWithSeconds(startTime + duration, scale);
	
	if (mPlayingForward) {
		[mPlayer seekToTime:cm_start];
		[mPlayerItem setForwardPlaybackEndTime:cm_duration];
	}
	else {
		[mPlayer seekToTime:cm_duration];
		[mPlayerItem setReversePlaybackEndTime:cm_start];
	}
}

void MovieBase::resetActiveSegment()
{
	if( ! mPlayer || ! mPlayerItem )
		return;
	
	if (mPlayingForward) {
		[mPlayer seekToTime:kCMTimeZero];
		[mPlayerItem setForwardPlaybackEndTime:[mPlayerItem duration]];
	}
	else {
		[mPlayer seekToTime:[mPlayerItem duration]];
		[mPlayerItem setReversePlaybackEndTime:kCMTimeZero];
	}
}

void MovieBase::setLoop( bool loop, bool palindrome )
{
	mLoop = loop;
	mPalindrome = (loop? palindrome: false);
}

bool MovieBase::stepForward()
{
	if( ! mPlayerItem )
		return false;
	
	bool can_step_forwards = [mPlayerItem canStepForward];
	if( can_step_forwards ) {
		[mPlayerItem stepByCount:1];
	}
	
	return can_step_forwards;
}

bool MovieBase::stepBackward()
{
	if( ! mPlayerItem)
		return false;
	
	bool can_step_backwards = [mPlayerItem canStepBackward];
	
	if (can_step_backwards) {
		[mPlayerItem stepByCount:-1];
	}
	
	return can_step_backwards;
}

bool MovieBase::setRate( float rate )
{
	if( ! mPlayer || ! mPlayerItem )
		return false;
	
	bool success;
	
	if( rate < -1.0f )
		success = [mPlayerItem canPlayFastReverse];
	else if( rate < 0.0f )
		success = [mPlayerItem canPlaySlowReverse];
	else if( rate > 1.0f )
		success = [mPlayerItem canPlayFastForward];
	else
		success = [mPlayerItem canPlaySlowForward];
	
	[mPlayer setRate:rate];
	
	return success;
}

void MovieBase::setVolume( float volume )
{
	if( ! mPlayer )
		return;
	
#if defined( CINDER_COCOA_TOUCH )
	NSArray* audioTracks = [mAsset tracksWithMediaType:AVMediaTypeAudio];
	NSMutableArray* allAudioParams = [NSMutableArray array];
	for( AVAssetTrack *track in audioTracks ) {
		AVMutableAudioMixInputParameters* audioInputParams =[AVMutableAudioMixInputParameters audioMixInputParameters];
		[audioInputParams setVolume:volume atTime:kCMTimeZero];
		[audioInputParams setTrackID:[track trackID]];
		[allAudioParams addObject:audioInputParams];
	}
	AVMutableAudioMix* volumeMix = [AVMutableAudioMix audioMix];
	[volumeMix setInputParameters:allAudioParams];
	[mPlayerItem setAudioMix:volumeMix];
	
#elif defined( CINDER_COCOA )
	[mPlayer setVolume:volume];
	
#endif
}

float MovieBase::getVolume() const
{
	if (!mPlayer) return -1;
	
#if defined( CINDER_COCOA_TOUCH )
	AVMutableAudioMix* mix = (AVMutableAudioMix*) [mPlayerItem audioMix];
	NSArray* inputParams = [mix inputParameters];
	float startVolume, endVolume;
	bool success = false;
	for( AVAudioMixInputParameters* param in inputParams )
		success = [param getVolumeRampForTime:[mPlayerItem currentTime] startVolume:&startVolume endVolume:&endVolume timeRange:NULL] || success;

	if( ! success )
		return -1;
	else
		return endVolume;
	
#elif defined( CINDER_COCOA )
	return [mPlayer volume];
#endif
}

bool MovieBase::isPlaying() const
{
	if( ! mPlayer )
		return false;
	
	return [mPlayer rate] != 0;
}

bool MovieBase::isDone() const
{
	if( ! mPlayer )
		return false;
	
	CMTime current_time = [mPlayerItem currentTime];
	CMTime end_time = mPlayingForward ? [mPlayerItem duration] : kCMTimeZero;
	return ::CMTimeCompare( current_time, end_time ) >= 0;
}

void MovieBase::play(bool toggle)
{
	if( ! mPlayer ) {
		mPlaying = true;
		return;
	}
	
	if( toggle ) {
		isPlaying()? [mPlayer pause]: [mPlayer play];
	}
	else {
		[mPlayer play];
	}
}

void MovieBase::stop()
{
	mPlaying = false;
	
	if( ! mPlayer )
		return;
	
	[mPlayer pause];
}

void MovieBase::init()
{
	mHasAudio = mHasVideo = false;
	mPlayThroughOk = mPlayable = mProtected = false;
	mPlaying = false;
	mPlayingForward = true;
	mLoop = mPalindrome = false;
	mFrameRate = -1;
	mWidth = -1;
	mHeight = -1;
	mDuration = -1;
	mFrameCount = -1;
}
	
void MovieBase::initFromUrl( const Url& url )
{
	NSURL* asset_url = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
	if( ! asset_url )
		throw AvfUrlInvalidExc();
	
	// Create the AVAsset
	NSDictionary* asset_options = @{(id)AVURLAssetPreferPreciseDurationAndTimingKey: @(YES)};
	mAsset = [[AVURLAsset alloc] initWithURL:asset_url options:asset_options];
	
	mResponder = new MovieResponder( this );
	mPlayerDelegate = [[MovieDelegate alloc] initWithResponder:mResponder];
	
	loadAsset();
}

void MovieBase::initFromPath( const fs::path& filePath )
{
	NSURL* asset_url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:filePath.c_str()]];
	if( ! asset_url )
		throw AvfPathInvalidExc();
	
	// Create the AVAsset
	NSDictionary* asset_options = @{(id)AVURLAssetPreferPreciseDurationAndTimingKey: @(YES)};
	mAsset = [[AVURLAsset alloc] initWithURL:asset_url options:asset_options];
	
	mResponder = new MovieResponder(this);
	mPlayerDelegate = [[MovieDelegate alloc] initWithResponder:mResponder];
	
	loadAsset();
	
	// spin-wait until asset loading is completed
	while( ! mAssetLoaded ) {
	}
}

void MovieBase::initFromLoader( const MovieLoader& loader )
{
	if( ! loader.ownsMovie() )
		return;
	
	loader.waitForLoaded();
	mPlayer = loader.transferMovieHandle();
	mPlayerItem = [mPlayer currentItem];
	mAsset = reinterpret_cast<AVURLAsset*>([mPlayerItem asset]);
	
	mResponder = new MovieResponder( this );
	mPlayerDelegate = [[MovieDelegate alloc] initWithResponder:mResponder];

	// process asset and prepare for playback...
	processAssetTracks( mAsset );
	
	// collect asset information
	mLoaded = true;
	mDuration = (float) CMTimeGetSeconds([mAsset duration]);
	mPlayable = [mAsset isPlayable];
	mProtected = [mAsset hasProtectedContent];
	mPlayThroughOk = [mPlayerItem isPlaybackLikelyToKeepUp];
	
	// setup PlayerItemVideoOutput --from which we will obtain direct texture access
	createPlayerItemOutput( mPlayerItem );
	
	// without this the player continues to move the playhead past the asset duration time...
	[mPlayer setActionAtItemEnd:AVPlayerActionAtItemEndPause];
	
	addObservers();
	
	allocateVisualContext();
}

void MovieBase::loadAsset()
{
	NSArray* keyArray = [NSArray arrayWithObjects:@"tracks", @"duration", @"playable", @"hasProtectedContent", nil];
	[mAsset loadValuesAsynchronouslyForKeys:keyArray completionHandler:^{
		NSError* error = nil;
		AVKeyValueStatus status = [mAsset statusOfValueForKey:@"tracks" error:&error];
		if( status == AVKeyValueStatusLoaded && ! error ) {
			processAssetTracks( mAsset );
		}
		
		error = nil;
		status = [mAsset statusOfValueForKey:@"duration" error:&error];
		if( status == AVKeyValueStatusLoaded && ! error ) {
			mDuration = (float) CMTimeGetSeconds([mAsset duration]);
		}
		
		error = nil;
		status = [mAsset statusOfValueForKey:@"playable" error:&error];
		if( status == AVKeyValueStatusLoaded && ! error ) {
			mPlayable = [mAsset isPlayable];
		}
		
		error = nil;
		status = [mAsset statusOfValueForKey:@"hasProtectedContent" error:&error];
		if( status == AVKeyValueStatusLoaded && ! error ) {
			mProtected = [mAsset hasProtectedContent];
		}
		
		// Create a new AVPlayerItem and make it our player's current item.
		mPlayer = [[AVPlayer alloc] init];
		mPlayerItem = [AVPlayerItem playerItemWithAsset:mAsset];
		[mPlayer replaceCurrentItemWithPlayerItem:mPlayerItem];
		
		// setup PlayerItemVideoOutput --from which we will obtain direct texture access
		createPlayerItemOutput( mPlayerItem );
		
		// without this the player continues to move the playhead past the asset duration time...
		[mPlayer setActionAtItemEnd:AVPlayerActionAtItemEndPause];
		
		addObservers();
		
		allocateVisualContext();
	
		mAssetLoaded = true;
	}];
}

void MovieBase::updateFrame()
{
	if( mPlayerVideoOutput && mPlayerItem ) {
		CMTime vTime = [mPlayer currentTime];
		if( [mPlayerVideoOutput hasNewPixelBufferForItemTime:vTime] ) {
			releaseFrame();
			
			CVImageBufferRef buffer = nil;
			buffer = [mPlayerVideoOutput copyPixelBufferForItemTime:vTime itemTimeForDisplay:nil];
			if( buffer ) {
				newFrame( buffer );
				mSignalNewFrame.emit();
			}
		}
	}
}

uint32_t MovieBase::countFrames() const
{
	if( ! mAsset )
		return 0;
	
	CMTime dur = [mAsset duration];
	CMTime one_frame = ::CMTimeMakeWithSeconds( 1.0 / mFrameRate, dur.timescale );
	double dur_seconds = ::CMTimeGetSeconds( dur );
	double one_frame_seconds = ::CMTimeGetSeconds( one_frame );
	return static_cast<uint32_t>(dur_seconds / one_frame_seconds);
}

void MovieBase::processAssetTracks( AVAsset* asset )
{
	// process video tracks
	NSArray* videoTracks = [asset tracksWithMediaType:AVMediaTypeVideo];
	mHasVideo = [videoTracks count] > 0;
	if( mHasVideo ) {
		AVAssetTrack* videoTrack = [videoTracks firstObject];
		if( videoTrack ) {
			// Grab track dimensions from format description
			CGSize size = [videoTrack naturalSize];
			CGAffineTransform trans = [videoTrack preferredTransform];
			size = CGSizeApplyAffineTransform(size, trans);
			mHeight = static_cast<int32_t>(size.height);
			mWidth = static_cast<int32_t>(size.width);
			mFrameRate = [videoTrack nominalFrameRate];
		}
		else
			throw AvfFileInvalidExc();
	}
	
	// process audio tracks
	NSArray* audioTracks = [asset tracksWithMediaType:AVMediaTypeAudio];
	mHasAudio = [audioTracks count] > 0;
#if defined( CINDER_COCOA_TOUCH )
	if( mHasAudio ) {
		setAudioSessionModes();
	}
#elif defined( CINDER_COCOA )
	// No need for changes on OSX
	
#endif
}

void MovieBase::createPlayerItemOutput( const AVPlayerItem* playerItem )
{
	mPlayerVideoOutput = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:avPlayerItemOutputDictionary()];
	dispatch_queue_t outputQueue = dispatch_queue_create("movieVideoOutputQueue", DISPATCH_QUEUE_SERIAL);
	[mPlayerVideoOutput setDelegate:mPlayerDelegate queue:outputQueue];
	mPlayerVideoOutput.suppressesPlayerRendering = YES;
	[playerItem addOutput:mPlayerVideoOutput];
}

void MovieBase::addObservers()
{
	if( mPlayerDelegate && mPlayerItem ) {
		// Determine if this is all we need out of the NotificationCenter
		NSNotificationCenter* notification_center = [NSNotificationCenter defaultCenter];
		[notification_center addObserver:mPlayerDelegate
								selector:@selector(playerItemDidNotReachEndCallback)
									name:AVPlayerItemFailedToPlayToEndTimeNotification
								  object:mPlayerItem];
		
		[notification_center addObserver:mPlayerDelegate
								selector:@selector(playerItemDidReachEndCallback)
									name:AVPlayerItemDidPlayToEndTimeNotification
								  object:mPlayerItem];
		
		[notification_center addObserver:mPlayerDelegate
								selector:@selector(playerItemTimeJumpedCallback)
									name:AVPlayerItemTimeJumpedNotification
								  object:mPlayerItem];
		
		[mPlayerItem addObserver:mPlayerDelegate
					  forKeyPath:@"status"
						 options:(NSKeyValueObservingOptions)0
						 context:AVPlayerItemStatusContext];
	}
}

void MovieBase::removeObservers()
{
	if( mPlayerDelegate && mPlayerItem ) {
		NSNotificationCenter* notify_center = [NSNotificationCenter defaultCenter];
		[notify_center removeObserver:mPlayerDelegate
								 name:AVPlayerItemFailedToPlayToEndTimeNotification
							   object:mPlayerItem];
		
		[notify_center removeObserver:mPlayerDelegate
								 name:AVPlayerItemDidPlayToEndTimeNotification
							   object:mPlayerItem];
		
		[notify_center removeObserver:mPlayerDelegate
								 name:AVPlayerItemTimeJumpedNotification
							   object:mPlayerItem];
		
		[mPlayerItem removeObserver:mPlayerDelegate
						 forKeyPath:@"status"];
	}
}
	
void MovieBase::playerReady()
{
	mSignalReady.emit();
	
	if( mPlaying )
		play();
}
	
void MovieBase::playerItemEnded()
{
	if( mPalindrome ) {
		float rate = -[mPlayer rate];
		mPlayingForward = (rate >= 0);
		this->setRate( rate );
	}
	else if( mLoop ) {
		this->seekToStart();
		this->play();
	}
	
	mSignalEnded.emit();
}
	
void MovieBase::playerItemCancelled()
{
	mSignalCancelled.emit();
}
	
void MovieBase::playerItemJumped()
{
	mSignalJumped.emit();
}

void MovieBase::outputWasFlushed( AVPlayerItemOutput* output )
{
	mSignalOutputWasFlushed.emit();
}

/////////////////////////////////////////////////////////////////////////////////
// MovieSurface
MovieSurface::MovieSurface( const Url& url )
	: MovieBase()
{
	MovieBase::initFromUrl( url );
}

MovieSurface::MovieSurface( const fs::path& path )
	: MovieBase()
{
	MovieBase::initFromPath( path );
}

MovieSurface::MovieSurface( const MovieLoader& loader )
	: MovieBase()
{
	MovieBase::initFromLoader( loader );
}

MovieSurface::~MovieSurface()
{
	deallocateVisualContext();
}

NSDictionary* MovieSurface::avPlayerItemOutputDictionary() const
{
	return [NSDictionary dictionaryWithObjectsAndKeys:
				[NSNumber numberWithInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey,
				nil];
}

Surface8uRef MovieSurface::getSurface()
{
	updateFrame();
	
	lock();
	Surface8uRef result = mSurface;
	unlock();
	
	return result;
}

void MovieSurface::newFrame( CVImageBufferRef cvImage )
{
	CVPixelBufferRef imgRef = reinterpret_cast<CVPixelBufferRef>( cvImage );
	if( imgRef )
		mSurface = convertCvPixelBufferToSurface( imgRef );
	else
		mSurface.reset();
}

void MovieSurface::releaseFrame()
{
	mSurface.reset();
}

/////////////////////////////////////////////////////////////////////////////////
// MovieLoader
MovieLoader::MovieLoader( const Url &url )
	:mUrl(url), mBufferFull(false), mBufferEmpty(false), mLoaded(false),
		mPlayable(false), mPlayThroughOK(false), mProtected(false), mOwnsMovie(true)
{
	NSURL* asset_url = [NSURL URLWithString:[NSString stringWithCString:mUrl.c_str() encoding:[NSString defaultCStringEncoding]]];
	if( ! asset_url )
		throw AvfUrlInvalidExc();
	
	AVPlayerItem* playerItem = [[AVPlayerItem alloc] initWithURL:asset_url];
	mPlayer = [[AVPlayer alloc] init];
	[mPlayer replaceCurrentItemWithPlayerItem:playerItem];	// starts the downloading process
	[playerItem release];
}

MovieLoader::~MovieLoader()
{
	if( mOwnsMovie && mPlayer ) {
		[mPlayer release];
	}
}
	
bool MovieLoader::checkLoaded() const
{
	if( ! mLoaded )
		updateLoadState();
	
	return mLoaded;
}

bool MovieLoader::checkPlayable() const
{
	if( ! mPlayable )
		updateLoadState();
	
	return mPlayable;
}

bool MovieLoader::checkPlaythroughOk() const
{
	if( ! mPlayThroughOK )
		updateLoadState();
	
	return mPlayThroughOK;
}

bool MovieLoader::checkProtection() const
{
	updateLoadState();
	
	return mProtected;
}

void MovieLoader::waitForLoaded() const
{
	// Accessing the AVAssets properties (such as duration, tracks, etc) will block the thread until they're available...
	NSArray* video_tracks = [[[mPlayer currentItem] asset] tracksWithMediaType:AVMediaTypeVideo];
	mLoaded = [video_tracks count] > 0;
}

void MovieLoader::waitForPlayable() const
{
	while( ! mPlayable ) {
		cinder::sleep( 250 );
		updateLoadState();
	}
}

void MovieLoader::waitForPlayThroughOk() const
{
	while( ! mPlayThroughOK ) {
		cinder::sleep( 250 );
		updateLoadState();
	}
}

void MovieLoader::updateLoadState() const
{
	AVPlayerItem* playerItem = [mPlayer currentItem];
	mLoaded = mPlayable = [playerItem status] == AVPlayerItemStatusReadyToPlay;
	mPlayThroughOK = [playerItem isPlaybackLikelyToKeepUp];
	mProtected = [[playerItem asset] hasProtectedContent];
	
	//NSArray* loaded = [playerItem seekableTimeRanges];  // this value appears to be garbage
/*	NSArray* loaded = [playerItem loadedTimeRanges];      // this value appears to be garbage
	for( NSValue* value in loaded ) {
		CMTimeRange range = [value CMTimeRangeValue];
		float start = ::CMTimeGetSeconds( range.start );
		float dur = ::CMTimeGetSeconds( range.duration );
		//mLoaded = (CMTimeCompare([playerItem duration], range.duration) >= 0);
	}
	
	AVPlayerItemAccessLog* log = [playerItem accessLog];
	if( log ) {
		NSArray* load_events = [log events];
		for (AVPlayerItemAccessLogEvent* log_event in load_events) {
			int segments = log_event.numberOfSegmentsDownloaded;
			int stalls = log_event.numberOfStalls;							// only accurate if playing!
			double segment_interval = log_event.segmentsDownloadedDuration;	// only accurate if playing!
			double watched_interval = log_event.durationWatched;			// only accurate if playing!
			NSString* str = log_event.serverAddress;
			std::string address = (str? std::string([str UTF8String]): "");
			long long bytes_transfered = log_event.numberOfBytesTransferred;
			double bitrate = log_event.observedBitrate;
			int dropped_frames = log_event.numberOfDroppedVideoFrames;		// only accurate if playing!
		}
	}*/
}

} } // namespace cinder::qtime

#endif // defined( CINDER_COCOA_TOUCH ) || ( defined( CINDER_MAC ) && ( MAC_OS_X_VERSION_MIN_REQUIRED >= 1080 ) )
