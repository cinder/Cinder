/*
 syphonClient.mm
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

#include "syphonClient.h"

#import <Syphon/Syphon.h>
#import "SyphonNameboundClient.h"

#include "cinder/app/App.h"

using namespace cinder;


syphonClient::syphonClient()
{
	refreshFrame = -1;
	bSetup = false;
	bHasNewFrame = false;
}

syphonClient::~syphonClient()
{
    if(bSetup)
    {
		NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		
		[(SyphonNameboundClient*)mClient release];
		mClient = nil;
		
		[pool drain];
	}
}

void syphonClient::setup()
{
    // Need pool
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	mClient = [[SyphonNameboundClient alloc] init]; 
	
	bSetup = true;
    
    [pool drain];
}

void syphonClient::setApplicationName(std::string appName)
{
    if(bSetup)
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        
        NSString *name = [NSString stringWithCString:appName.c_str() encoding:[NSString defaultCStringEncoding]];
        
        [(SyphonNameboundClient*)mClient setAppName:name];
		
        [pool drain];
    }
    
}
void syphonClient::setServerName(std::string serverName)
{
    if(bSetup)
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
        
        NSString *name = [NSString stringWithCString:serverName.c_str() encoding:[NSString defaultCStringEncoding]];
		
        if([name length] == 0)
            name = nil;
        
        [(SyphonNameboundClient*)mClient setName:name];
		
        [pool drain];
    }    
}

//
// Dont forget to unbind() !!!!!
// ... or else the client will remain locked
void syphonClient::bind(int unit)
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
    if(bSetup)
    {
     	[(SyphonNameboundClient*)mClient lockClient];
		
		this->refresh();

		if (mTex)
		{
			// Save old GL_TEXTURE_RECTANGLE_ARB binding or else we can mess GL_TEXTURE_2D used after
			glGetBooleanv( GL_TEXTURE_RECTANGLE_ARB, &mOldTargetBinding );
			
			mTex.enableAndBind(unit);
		}
    }
    else
		std::cout<<"syphonClient is not setup, or is not properly connected to server.  Cannot bind.\n";
    
    [pool drain];
}

void syphonClient::unbind(int unit)
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    
    if(bSetup)
    {
		if (mTex)
		{
			mTex.unbind(unit);
			
			if( mOldTargetBinding )
				glEnable( GL_TEXTURE_RECTANGLE_ARB );
			else
				glDisable( GL_TEXTURE_RECTANGLE_ARB );
		}
		
        [(SyphonNameboundClient*)mClient unlockClient];
    }
    else
		std::cout<<"syphonClient is not setup, or is not properly connected to server.  Cannot unbind.\n";
	
	[pool drain];
}

//
// Draw Syphon texture
void syphonClient::draw( const Vec2f & pos )
{
	if(bSetup){
		this->bind();
		if (mTex)
			gl::draw( mTex, pos );
		this->unbind();
	}
}
void syphonClient::draw( const Rectf & rect )
{
	if(bSetup){
		this->bind();
		if (mTex)
			gl::draw( mTex, rect );
		this->unbind();
	}
}
void syphonClient::draw( const Area & srcArea, const Rectf & destRect )
{
	if(bSetup){
		this->bind();
		if (mTex)
			gl::draw( mTex, srcArea, destRect );
		this->unbind();
	}
}


//
// Update client with latest data
// Call this from inside your app to get always the freshest data from this Client
void syphonClient::update()
{
    if(bSetup)
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		[(SyphonNameboundClient*)mClient lockClient];
		this->refresh();
		[(SyphonNameboundClient*)mClient unlockClient];
		[pool drain];
    }    
    else
		std::cout<<"syphonClient is not setup, or is not properly connected to server.  Cannot bind.\n";
}

//
// (PRIVATE)
// Refresh current texture from Syphon Client
// Sets all data for this client, like
// Can be called by all methods, will be updated just once per frame
void syphonClient::refresh()
{
	// refresh once per frame
	if ( refreshFrame == app::getElapsedFrames() )
		return;
	refreshFrame = app::getElapsedFrames();
	
	// Get Client
	SyphonClient *client = [(SyphonNameboundClient*)mClient client];
	mCurrentFrame = [(SyphonNameboundClient*)mClient currentFrame];
	bHasNewFrame = [client hasNewFrame];
	
	// Grab Frame
	SyphonImage* latestImage = [client newFrameImageForContext:CGLGetCurrentContext()];
	//printf("SyphonImage [%d] current [%d] new [%d]\n",(int)latestImage,(int)mCurrentFrame,bHasNewFrame);
	if (latestImage)
	{
		NSSize texSize = [latestImage textureSize];
		GLuint m_id = [latestImage textureName];
		[latestImage release];
		
		bool newTex = false;
		if ( !mTex )
			newTex = true;
		else if ( texSize.width != mTex.getWidth() || texSize.height != mTex.getHeight() || m_id != mTex.getId() )
			newTex = true;
		
		if (newTex)
		{
			mTex = gl::Texture(GL_TEXTURE_RECTANGLE_ARB, m_id, texSize.width, texSize.height, true);
			mTex.setFlipped();
			//printf("Syphon NEW TEX id[%d] w[%d] h[%d]\n",(int)m_id,(int)texSize.width,(int)texSize.height);
		}
	}
	else if ( mTex )
		mTex = gl::Texture();
}

//
// Has a new frame since last update()
bool syphonClient::hasNewFrame() 
{
    if(bSetup)
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		[(SyphonNameboundClient*)mClient lockClient];
		this->refresh();
		[(SyphonNameboundClient*)mClient unlockClient];
		[pool drain];
		return bHasNewFrame;
    }
	else 
		return false;
};

//
// Current Frame number
unsigned int syphonClient::getCurrentFrame() 
{
    if(bSetup)
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		[(SyphonNameboundClient*)mClient lockClient];
		this->refresh();
		[(SyphonNameboundClient*)mClient unlockClient];
		[pool drain];
		return mCurrentFrame;
    }
	else 
		return 0;
};

//
// This client's FPS
// The FPS calculation is asynchronous and updated inside SyphonNameboundClient
// Even if my app is on a slower framerate, this will be accurate
float syphonClient::getCurrentFrameRate() 
{
	return ( mClient ? [(SyphonNameboundClient*)mClient currentFrameRate] : 0.0 );
};

