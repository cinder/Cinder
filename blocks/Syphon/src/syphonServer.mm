/*
 syphonServer.mm
 Cinder Syphon Implementation
 
 Created by astellato on 2/6/11
 
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

#include "syphonServer.h"

#import <Syphon/Syphon.h>

syphonServer::syphonServer()
{
	mSyphon = nil;
}

syphonServer::~syphonServer()
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
    [(SyphonServer *)mSyphon stop];
    [(SyphonServer *)mSyphon release];
    
    [pool drain];
}


void syphonServer::setName(std::string n)
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    
	NSString *title = [NSString stringWithCString:n.c_str() 
										 encoding:[NSString defaultCStringEncoding]];
	
	if (!mSyphon)
	{
		mSyphon = [[SyphonServer alloc] initWithName:title context:CGLGetCurrentContext() options:nil];
	}
	else
	{
		[(SyphonServer *)mSyphon setName:title];
	}
    
    [pool drain];
}

std::string syphonServer::getName()
{
	std::string name;
	if (mSyphon)
	{
		NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		
		name = [[(SyphonServer *)mSyphon name] cStringUsingEncoding:[NSString defaultCStringEncoding]];
		
		[pool drain];
	}
	else
	{
		name = "Untitled";
	}
	return name;
}

void syphonServer::publishScreen()
{
	ci::gl::Texture mTex =  ci::gl::Texture(ci::app::copyWindowSurface());
	this->publishTexture( mTex );
}


void syphonServer::publishTexture( ci::gl::Texture & inputTexture )
{
	if(inputTexture){
		NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		GLuint texID = inputTexture.getId();
		if (!mSyphon)
		{
			mSyphon = [[SyphonServer alloc] initWithName:@"Untitled" context:CGLGetCurrentContext() options:nil];
		}
		[(SyphonServer *)mSyphon publishFrameTexture:texID 
									   textureTarget:GL_TEXTURE_2D 
										 imageRegion:NSMakeRect(0, 0, inputTexture.getWidth(), inputTexture.getHeight())
								   textureDimensions:NSMakeSize(inputTexture.getWidth(), inputTexture.getHeight()) 
											 flipped:!inputTexture.isFlipped()];
		[pool drain];
	} else {
		ci::app::console()<<"syphonServer is not setup, or texture is not properly backed.  Cannot draw.\n";
	}
}

