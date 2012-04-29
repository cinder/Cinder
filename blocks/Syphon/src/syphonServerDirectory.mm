/*
 syphonServerDirectory.mm
 Cinder Syphon Implementation
 
 Created by astellato on 2/6/11
 syphonServerDirectory ny roger sodre 12/8/11

 Copyright 2011 astellato, bangnoise (Tom Butterworth) & vade (Anton Marini).
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

#include "syphonServerDirectory.h"
#import <Syphon/Syphon.h>



//
// Objective-C Local Directory
// 
@interface localServerDirectory : NSObject {
	@private
	NSArray * servers;
	BOOL bChanged;
}
-(id)init;
-(void)update;
@property (readonly) NSArray *servers;
@property (readonly) BOOL bChanged;
@end

@implementation localServerDirectory

@synthesize servers;

- (id)init
{
	NSLog(@">>> INIT localServerDirectory");
    self = [super init];
	if (self)
	{
		[[SyphonServerDirectory sharedDirectory] addObserver:self forKeyPath:@"servers" options:0 context:nil];
		[self update];
	}
	return self;
}

- (void)dealloc
{
	[[SyphonServerDirectory sharedDirectory] removeObserver:self forKeyPath:@"servers"];
    [servers release];
    [super dealloc];
}

// Here we build our UI in response to changing bindings in our syClient, using KVO
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	//	NSLog(@"Changes happened in Syphon Client : %@ change:%@", object, change);
	if([keyPath isEqualToString:@"servers"])
		[self update];
}

- (void)update
{
	servers = [NSArray arrayWithArray:[[SyphonServerDirectory sharedDirectory] servers]];
	bChanged = true;
}

- (BOOL)bChanged
{
	BOOL b = bChanged;
	bChanged = false;
	return b;
}


@end



///////////////////////////////////////////////////////////////////////////////
//
// C++ Directory for Cinder
//
syphonServerDirectory::syphonServerDirectory()
{
	bSetup = false;
}

syphonServerDirectory::~syphonServerDirectory()
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	//[(localServerDirectory*)mLocalDirectory release];
    
    [pool drain];
	
	this->clear();
}

void syphonServerDirectory::setup()
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	mLocalDirectory = (void*) [[localServerDirectory alloc] init];
	
	bSetup = true;
	
	this->update();

    [pool drain];
	
}

//
// Update server vector
void syphonServerDirectory::update()
{
	// empty current list
	this->clear();
	
	// make new list
	for(NSDictionary* serverDescription in [[SyphonServerDirectory sharedDirectory] servers])
	{
		const char* name = [[serverDescription objectForKey:SyphonServerDescriptionNameKey] UTF8String];
		const char* appName = [[serverDescription objectForKey:SyphonServerDescriptionAppNameKey] UTF8String];
		const char *uuid = [[serverDescription objectForKey:SyphonServerDescriptionUUIDKey] UTF8String];
		//NSImage* appImage = [serverDescription objectForKey:SyphonServerDescriptionIconKey];
		
		servers.push_back( new syphonServerDescription( uuid, appName, name) );
	}
	
	// debug
	for (int i = 0 ; i < servers.size() ; i++)
		printf(">> SYPHON SERVERS %d: %s / %s / %s\n",i,servers[i]->getAppName().c_str(),servers[i]->getName().c_str(),servers[i]->getUuid().c_str());
}

//
// Clear server vector
void syphonServerDirectory::clear()
{
	for (int i = 0 ; i < servers.size() ; i++)
		delete servers[i];
	servers.clear();
}

//
// Check if servers list changed and update Cinder vector
// !!! WORKS ONLY ONCE !!!
bool syphonServerDirectory::hasChanged()
{
	if ( ((localServerDirectory*)mLocalDirectory).bChanged )
	{
		this->update();
		return true;
	}
	else
		return false;
}

//
// Returns index of a server by name
// Returns -1 if not found
int syphonServerDirectory::getServerIndexByName( std::string _name )
{
	for (int i = 0 ; i < servers.size() ; i++)
		if ( ! servers[i]->getName().compare(_name) )
			return i;
	return -1;
}





