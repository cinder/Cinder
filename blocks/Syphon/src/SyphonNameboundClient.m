/*
    SyphonNameboundClient.m
	Syphon (Implementations)
	
    Copyright 2011-2011 bangnoise (Tom Butterworth) & vade (Anton Marini).
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#import "SyphonNameboundClient.h"
#include <sys/time.h>

@interface SyphonNameboundClient (Private)
- (void)setClientFromSearchHavingLock:(BOOL)isLocked;
@end
@implementation SyphonNameboundClient

@synthesize currentFrame;

- (id)init
{
    self = [super init];
	if (self)
	{
		_lock = OS_SPINLOCK_INIT;
        _searchPending = YES;
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleServerAnnounce:) name:SyphonServerAnnounceNotification object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleServerUpdate:) name:SyphonServerUpdateNotification object:nil];
	}
	return self;
}

- (void)finalize
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super finalize];
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[_name release];
	[_appname release];
	[_client release];
	[_lockedClient release];
	[super dealloc];
}

- (NSString *)name
{
	NSString *result;
	OSSpinLockLock(&_lock);
	result = [[_name retain] autorelease];
	OSSpinLockUnlock(&_lock);
	return result;
}

- (void)setName:(NSString *)name
{
	OSSpinLockLock(&_lock);
	[name retain];
	[_name release];
	_name = name;
	_searchPending = YES;
	OSSpinLockUnlock(&_lock);
}

- (NSString *)appName
{
	NSString *result;
	OSSpinLockLock(&_lock);
	result = [[_appname retain] autorelease];
	OSSpinLockUnlock(&_lock);
	return result;
}

- (void)setAppName:(NSString *)app
{
	OSSpinLockLock(&_lock);
	[app retain];
	[_appname release];
	_appname = app;
	_searchPending = YES;
	OSSpinLockUnlock(&_lock);
}

- (void)lockClient
{
	OSSpinLockLock(&_lock);
	if (_lockedClient == nil)
	{
		if (_searchPending)
		{
			[self setClientFromSearchHavingLock:YES];
			_searchPending = NO;
		}
		_lockedClient = [_client retain];
	}
	OSSpinLockUnlock(&_lock);
}

- (void)unlockClient
{
	SyphonClient *doneWith;
	OSSpinLockLock(&_lock);
	doneWith = _lockedClient;
	_lockedClient = nil;
	OSSpinLockUnlock(&_lock);
	[doneWith release]; // release outside the lock as it may take time
}

- (SyphonClient *)client
{
	return _lockedClient;
}

- (void)setClient:(SyphonClient *)client havingLock:(BOOL)isLocked
{
	SyphonClient *newClient = [client retain];
	SyphonClient *oldClient;
	
	if (!isLocked) OSSpinLockLock(&_lock);
	oldClient = _client;
	_client = newClient;
	if (!isLocked) OSSpinLockUnlock(&_lock);
	
	// If we were registered for notifications and no longer require them
	// remove ourself from the notification center
	
	if (oldClient == nil && newClient != nil)
	{
		[[NSNotificationCenter defaultCenter] removeObserver:self name:SyphonServerAnnounceNotification object:nil];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleServerRetire:) name:SyphonServerRetireNotification object:nil];
	}
	
	// If we weren't registered already, but need to register now, do so
	
	if (newClient == nil && oldClient != nil)
	{
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleServerAnnounce:) name:SyphonServerAnnounceNotification object:nil];
		[[NSNotificationCenter defaultCenter] removeObserver:self name:SyphonServerRetireNotification object:nil];
	}
	
	// Release the old client
	[oldClient release];
}

- (BOOL)parametersMatchDescription:(NSDictionary *)description
{
	NSString *searchName = self.name;
	NSString *searchApp = self.appName;
	
	if ([searchName length] == 0)
	{
		searchName = nil;
	}
	if ([searchApp length] == 0)
	{
		searchApp = nil;
	}

	if ((!searchName || [[description objectForKey:SyphonServerDescriptionNameKey] isEqualToString:searchName])
		&& (!searchApp || [[description objectForKey:SyphonServerDescriptionAppNameKey] isEqualToString:searchApp]))
	{
			return YES;
	}
	return NO;
}

- (void)setClientFromSearchHavingLock:(BOOL)isLocked
{
	SyphonClient *newClient = nil;
	
	if (!isLocked) OSSpinLockLock(&_lock);
    
    NSArray *matches = [[SyphonServerDirectory sharedDirectory] serversMatchingName:_name appName:_appname];

    if ([matches count] != 0)
    {
        NSString *current = [_client.serverDescription objectForKey:SyphonServerDescriptionUUIDKey];
        NSString *found = [[matches lastObject] objectForKey:SyphonServerDescriptionUUIDKey];
        if (found && [current isEqualToString:found])
        {
            newClient = [_client retain];
        }
        else
        {
            newClient = [[SyphonClient alloc] initWithServerDescription:[matches lastObject] options:nil newFrameHandler:^(SyphonClient *client) {
				[self handleNewFrame];
			}];
        }
    }
	[self setClient:newClient havingLock:YES];
	
    if (!isLocked) OSSpinLockUnlock(&_lock);
	
    [newClient release];
}

- (void)handleServerAnnounce:(NSNotification *)notification
{
	NSDictionary *newInfo = [notification object];
    // If we don't have a client, or our current client doesn't match our parameters any more
	if ((_client == nil || ![self parametersMatchDescription:[_client serverDescription]])
		&& [self parametersMatchDescription:newInfo])
	{
		SyphonClient *newClient = [[SyphonClient alloc] initWithServerDescription:newInfo options:nil newFrameHandler:^(SyphonClient *client) {
			[self handleNewFrame];
		}];
		
		[self setClient:newClient havingLock:NO];
		[newClient release];
	}
}

- (void)handleServerUpdate:(NSNotification *)notification
{
	NSDictionary *newInfo = [notification object];
	NSDictionary *currentServer = [_client serverDescription];
	// It's possible our client hasn't received the update yet, so we can't trust its server description
	// so check if the new update is for our client...
	if ([[currentServer objectForKey:SyphonServerDescriptionUUIDKey] isEqualToString:[newInfo objectForKey:SyphonServerDescriptionUUIDKey]])
	{
		// ...and if so, check to see if our parameters no longer describe the server
		if (![self parametersMatchDescription:newInfo])
		{
			[self setClient:nil havingLock:NO];
		}
		
	}
	// If we don't have a matching client but this client's new details match, then set up a new client
	if (_client == nil && [self parametersMatchDescription:newInfo])
	{
		SyphonClient *newClient = [[SyphonClient alloc] initWithServerDescription:newInfo options:nil newFrameHandler:^(SyphonClient *client) {
			[self handleNewFrame];
		}];
		
		[self setClient:newClient havingLock:NO];
		[newClient release];
	}
}

- (void)handleServerRetire:(NSNotification *)notification
{
	NSString *retiringUUID = [[notification object] objectForKey:SyphonServerDescriptionUUIDKey];
	NSString *ourUUID = [[_client serverDescription] objectForKey:SyphonServerDescriptionUUIDKey];
	
	if ([retiringUUID isEqualToString:ourUUID])
	{
		[self setClientFromSearchHavingLock:NO];
	}
}

- (void)handleNewFrame
{
	currentFrame++;
	fpsCount++;
	
	[self updateFrameRate];
}

- (void)updateFrameRate
{
	static unsigned int millisStart = 0;

    struct timeval now;
	gettimeofday(&now, NULL);
	unsigned int millisNow = (now.tv_sec * 1000 + now.tv_usec / 1000);
	unsigned int elapsed = millisNow - millisStart;
	if (elapsed > 1000)
	{
		currentFrameRate = ((float)fpsCount / (float)elapsed) * 1000.0;
		millisStart = millisNow;
		fpsCount = 0;
	}
}

- (float)currentFrameRate
{
	// Update frame rate even if not receiving frames
	[self updateFrameRate];
	return currentFrameRate;
}



@end
