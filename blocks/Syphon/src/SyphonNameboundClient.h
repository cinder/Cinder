/*
    SyphonNameboundClient.h
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
 
/*
	SyphonNameboundClient

	For a given name and hostname, keeps alive any matching client across server arrival and retiral.
	If name and hostname are both nil or empty strings, no server will be matched.
	If name or hostname or both are set, they will be used to determine server match.

	This class is intended for situations where parameters must be observed but cannot be set, such as FreeFrame or Quartz Composer
	plugins. In these cases a server changing its name results in us ditching the client as it no longer meets the criteria set by our
	parameters.

	An object similar to this, which instantiates clients as and when they become available for a given host/name combination might
	be useful, but it should be implemented to adapt to a server changing its name by keeping the existing client alive and reflecting
	the change in its own properties (and on to the object model and UI).
*/

#import <Cocoa/Cocoa.h>
#import <libkern/OSAtomic.h>
#import <Syphon/Syphon.h>


@interface SyphonNameboundClient : NSObject {
@private
	NSString *_name;
	OSSpinLock _lock;
	NSString *_appname;
	SyphonClient *_client;
	SyphonClient *_lockedClient;
	BOOL _searchPending;
	// New in Cinder
	unsigned int currentFrame;
	unsigned int fpsCount;
	float currentFrameRate;
}
@property (readwrite, retain) NSString *name;
@property (readwrite, retain) NSString *appName;
- (void)lockClient;
- (void)unlockClient;
/*
 @property (readonly) SyphonClient *client

	Only use this property (and the client it returns) between lock/unlockClient calls.
 */
// New in Cinder
@property (readonly) SyphonClient *client;
@property (readonly) unsigned int currentFrame;
@property (readonly) float currentFrameRate;
- (void)handleNewFrame;
- (void)updateFrameRate;
@end
