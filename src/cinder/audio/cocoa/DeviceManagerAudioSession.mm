/*
 Copyright (c) 2014, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/audio/cocoa/DeviceManagerAudioSession.h"
#include "cinder/audio/Exception.h"
#include "cinder/app/App.h"
#include "cinder/CinderAssert.h"

#include "cinder/cocoa/CinderCocoa.h"

#include <cmath>

#import <AVFoundation/AVAudioSession.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIApplication.h>

@interface AudioSessionNotificationHandlerImpl : NSObject {
  @public
	ci::audio::cocoa::DeviceManagerAudioSession *mDeviceManager;
}

- (void)handleInterruption:(NSNotification *)notification;

@end


using namespace std;

namespace cinder { namespace audio { namespace cocoa {


namespace {

const string REMOTE_IO_KEY = "iOS-RemoteIO";

} // anonymous namespace


// ----------------------------------------------------------------------------------------------------
// MARK: - DeviceManagerAudioSession
// ----------------------------------------------------------------------------------------------------

DeviceManagerAudioSession::DeviceManagerAudioSession()
: DeviceManager(), mSessionIsActive( false ), mInputEnabled( false ), mInterruptionHasEnded( false ), mSessionNotificationHandler( nullptr )
{
	activateSession();
}

DeviceManagerAudioSession::~DeviceManagerAudioSession()
{
	if( mSessionNotificationHandler ) {
		[[NSNotificationCenter defaultCenter] removeObserver:mSessionNotificationHandler];
		[mSessionNotificationHandler release];
		mSessionNotificationHandler = nullptr;
	}
}

DeviceRef DeviceManagerAudioSession::getDefaultOutput()
{
	return getRemoteIODevice();
}

DeviceRef DeviceManagerAudioSession::getDefaultInput()
{
	setInputEnabled( true ); // in order for any input's to be available, the AVAudioSessionCategory needs to be updated from its default.
	return getRemoteIODevice();
}

DeviceRef DeviceManagerAudioSession::findDeviceByName( const string &name, bool supportsOutput, bool supportsInput )
{
	return getRemoteIODevice();
}

DeviceRef DeviceManagerAudioSession::findDeviceByKey( const string &key )
{
	return getRemoteIODevice();
}

const vector<DeviceRef>& DeviceManagerAudioSession::getDevices()
{
	if( mDevices.empty() )
		getRemoteIODevice(); // this will call addDevice() with the default device.
	
	return mDevices;
}

void DeviceManagerAudioSession::setInputEnabled( bool enable )
{
	if( mInputEnabled == enable )
		return;
	
	NSString *category = ( enable ? AVAudioSessionCategoryPlayAndRecord : AVAudioSessionCategoryAmbient );

	NSError *error = nil;
	[[AVAudioSession sharedInstance] setCategory:category error:&error];
	CI_ASSERT( ! error );

	mInputEnabled = enable;
}

string DeviceManagerAudioSession::getName( const DeviceRef &device )
{
	return REMOTE_IO_KEY;
}

size_t DeviceManagerAudioSession::getNumInputChannels( const DeviceRef &device )
{
	if( ! isInputEnabled() )
		return 0;

#if( __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_6_0 )
	// TODO: AVAudioSession's maximumOutputNumberOfChannels will say 2 here if headphones are plugged in - need to expose that as an option
	NSInteger result = [[AVAudioSession sharedInstance] inputNumberOfChannels];
#else
	NSInteger result = [[AVAudioSession sharedInstance] currentHardwareInputNumberOfChannels];
#endif
	return static_cast<size_t>( result );
}

size_t DeviceManagerAudioSession::getNumOutputChannels( const DeviceRef &device )
{
#if( __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_6_0 )
	NSInteger result = [[AVAudioSession sharedInstance] outputNumberOfChannels];
#else
	NSInteger result = [[AVAudioSession sharedInstance] currentHardwareOutputNumberOfChannels];
#endif

	return static_cast<size_t>( result );
}

size_t DeviceManagerAudioSession::getSampleRate( const DeviceRef &device )
{
#if( __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_6_0 )
	double result = [[AVAudioSession sharedInstance] sampleRate];
#else
	double result = [[AVAudioSession sharedInstance] currentHardwareSampleRate];
#endif

	return static_cast<size_t>( result );
}

size_t DeviceManagerAudioSession::getFramesPerBlock( const DeviceRef &device )
{
#if( __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_6_0 )
	double durationSeconds = [[AVAudioSession sharedInstance] IOBufferDuration];
#else
	Float32 durationSeconds;
	UInt32 durationSecondsSize = sizeof( durationSeconds );
	OSStatus status = ::AudioSessionGetProperty( kAudioSessionProperty_CurrentHardwareIOBufferDuration, &durationSecondsSize, &durationSeconds );
	CI_VERIFY( status == noErr );
#endif

	return lround( static_cast<Float32>( getSampleRate( device ) ) * durationSeconds );
}

void DeviceManagerAudioSession::setSampleRate( const DeviceRef &device, size_t sampleRate )
{
	NSError *error = nil;
#if( __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_6_0 )
	BOOL didUpdate = [[AVAudioSession sharedInstance] setPreferredSampleRate:sampleRate error:&error];
#else
	BOOL didUpdate = [[AVAudioSession sharedInstance] setPreferredHardwareSampleRate:sampleRate error:&error];
#endif
	CI_ASSERT( ! error );

	if( ! didUpdate || getSampleRate( device ) != sampleRate )
		throw AudioDeviceExc( "Failed to update samplerate to the requested value." );
}

void DeviceManagerAudioSession::setFramesPerBlock( const DeviceRef &device, size_t framesPerBlock )
{
	NSTimeInterval bufferDuration = (NSTimeInterval)framesPerBlock / (NSTimeInterval)getSampleRate( device );

	NSError *error = nil;
	BOOL didUpdate = [[AVAudioSession sharedInstance] setPreferredIOBufferDuration:bufferDuration error:&error];
	CI_ASSERT( ! error );

	if( ! didUpdate || getFramesPerBlock( device ) != framesPerBlock )
		throw AudioDeviceExc( "Failed to update frames per block to the requested value." );
}

void DeviceManagerAudioSession::privateBeginInterruption()
{
	mSessionIsActive = false;

	mSignalInterruptionBegan.emit();
}

void DeviceManagerAudioSession::privateEndInterruption( bool resumeImmediately )
{
	if( resumeImmediately ) {
		mSessionIsActive = true;
		mInterruptionHasEnded = false;

		mSignalInterruptionEnded.emit();
	}
	else {
		mInterruptionHasEnded = true;
	}
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Private
// ----------------------------------------------------------------------------------------------------

const DeviceRef& DeviceManagerAudioSession::getRemoteIODevice()
{
	if( ! mRemoteIODevice )
		mRemoteIODevice = addDevice( REMOTE_IO_KEY );

	return mRemoteIODevice;
}

void DeviceManagerAudioSession::activateSession()
{
	if( ! mSessionNotificationHandler ) {
		mSessionNotificationHandler = [AudioSessionNotificationHandlerImpl new];
		mSessionNotificationHandler->mDeviceManager = this;

		[[NSNotificationCenter defaultCenter] addObserver:mSessionNotificationHandler selector:@selector( handleInterruption: ) name:AVAudioSessionInterruptionNotification object:nil];
	}

	AVAudioSession *globalSession = [AVAudioSession sharedInstance];

	NSError *error = nil;
	bool didActivate = [globalSession setActive:YES error:&error];
	CI_ASSERT( ! error );

	if( ! didActivate )
		throw AudioDeviceExc( "Failed to activate global AVAudioSession." );

	auto app = app::App::get();
	if( app ) {
		mAppDidBecomeActiveConn = app->getSignalDidBecomeActive().connect( [this] {
			if( mInterruptionHasEnded ) {
				privateEndInterruption( true );
			}
		} );
	}

	mSessionIsActive = true;
}

string DeviceManagerAudioSession::getSessionCategory()
{
	NSString *category = [[AVAudioSession sharedInstance] category];
	return ci::cocoa::convertNsString( category );
}


} } } // namespace cinder::audio::cocoa


// ----------------------------------------------------------------------------------------------------
// MARK: - AudioSessionNotificationHandlerImpl
// ----------------------------------------------------------------------------------------------------

@implementation AudioSessionNotificationHandlerImpl

- (void)handleInterruption:(NSNotification *)notification
{
	NSUInteger interruptionType = [[notification.userInfo objectForKey:AVAudioSessionInterruptionTypeKey] unsignedIntValue];

	if( interruptionType == AVAudioSessionInterruptionTypeBegan ) {
		mDeviceManager->privateBeginInterruption();
	}
	else if( interruptionType == AVAudioSessionInterruptionTypeEnded ) {
		UIApplication *app = [UIApplication sharedApplication];
		AVAudioSession *session = [AVAudioSession sharedInstance];

		bool resumeImmediately = app.applicationState == UIApplicationStateActive ||
			[session.category isEqualToString:AVAudioSessionCategoryPlayAndRecord] ||
			[session.category isEqualToString:AVAudioSessionCategoryPlayback] ||
			[session.category isEqualToString:AVAudioSessionCategoryRecord];

		mDeviceManager->privateEndInterruption( resumeImmediately );
	}
}

@end
