/*
 syphonClient.h
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
#pragma once

#include "cinder/gl/Texture.h"

class syphonClient {
	public:
	syphonClient();
	~syphonClient();
	
	virtual void setup();
	virtual void update();
	
	void setApplicationName(std::string appName);
    void setServerName(std::string serverName);
	
    void bind(int unit=0);
    void unbind(int unit=0);
    
    void draw(float x, float y)						{ this->draw( ci::Vec2f(x,y) ); }
    void draw(float x, float y, float w, float h)	{ this->draw( ci::Rectf(x,y,x+w,y+h) ); }
	void draw( const ci::Vec2f & p, ci::Vec2f &sz)	{ this->draw( ci::Rectf(p.x, p.y, p.x+sz.x, p.y+sz.y) ); }
	void draw( const ci::Vec2f & pos );
	void draw( const ci::Rectf & rect );
	void draw( const ci::Area & srcArea, const ci::Rectf & destRect );

	ci::gl::Texture & getTexture()	{ return mTex; }
	int getWidth()					{ return ( mTex ? mTex.getWidth() : 0 ); }
	int getHeight()					{ return ( mTex ? mTex.getHeight() : 0 ); }
	ci::Vec2i getSize()				{ return ( mTex ? mTex.getSize() : ci::Vec2i::zero() ); }
	ci::Rectf getBounds()			{ return ( mTex ? mTex.getBounds() : ci::Rectf(0,0,0,0) ); }
	bool isConnected()				{ return ( mTex ? true : false ); }
	bool hasNewFrame();
	unsigned int getCurrentFrame();
	float getCurrentFrameRate();
	
protected:

	void* mClient;
	std::string name;
	ci::gl::Texture mTex;
	GLboolean mOldTargetBinding;
	int width, height;
	bool bSetup;
	bool bHasNewFrame;
	uint32_t refreshFrame;
	unsigned int mCurrentFrame;
	
	void refresh();
};
