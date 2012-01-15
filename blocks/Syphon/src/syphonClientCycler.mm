/*
 syphonClientCycler.mm
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

#include "syphonClientCycler.h"

#import <Syphon/Syphon.h>
#import "SyphonNameboundClient.h"

#include "cinder/app/App.h"

using namespace ci;


syphonClientCycler::syphonClientCycler() : syphonClient()
{
	mCurrentLayer	= -1;
}

void syphonClientCycler::setup()
{
	this->syphonClient::setup();
}

void syphonClientCycler::update()
{
	this->syphonClient::update();
	
	// Get FBO
	mCurrentLayer = ( mCurrentLayer < (CYCLER_MAX_FBOS-1) ? mCurrentLayer+1 : 0 );
	gl::Fbo & fbo = mFbos[mCurrentLayer];
	if ( ! fbo )
		fbo = this->resizeFbo( mCurrentLayer );
	else if ( fbo.getWidth() != this->getWidth() || fbo.getHeight() != this->getHeight() )
		fbo = this->resizeFbo( mCurrentLayer );
	
	// Draw to FBO
	if (fbo)
	{
		fbo.bindFramebuffer();
		gl::disableDepthRead();
		gl::disableDepthWrite();
		gl::disableAlphaBlending();
		gl::setMatricesWindow( fbo.getTexture().getSize() );
		gl::setViewport( fbo.getTexture().getBounds() );
		gl::clear( ColorA::zero() );
		gl::color( ColorA::white() );
		gl::Texture & tex = this->syphonClient::getTexture();
		if ( tex )
			gl::draw( tex, fbo.getBounds() );
		fbo.unbindFramebuffer();
		//printf("CYCLER update layer [%d]\n",mCurrentLayer);
	}
}
gl::Fbo & syphonClientCycler::resizeFbo( int layer)
{
	if ( this->getWidth() && this->getHeight() )
	{
		gl::Fbo::Format format;
		format.enableDepthBuffer( false );
		format.enableMipmapping();
		//format.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		//format.setMagFilter(GL_LINEAR_MIPMAP_LINEAR);
		mFbos[layer] = gl::Fbo( this->getWidth(), this->getHeight(), format );
		mFbos[layer].getTexture().setFlipped();
	}
	else
		mFbos[layer] = gl::Fbo();
	return mFbos[layer];
}


//
// Dont forget to unbind() !!! ... or else the client will remain locked
void syphonClientCycler::bind(int unit, int layer)
{
	this->syphonClient::bind( unit );
}

gl::Texture & syphonClientCycler::getTexture( int layer, int step )
{
	//return this->syphonClient::getTexture();
	
	// out of bounds
	if ( layer < 0 || layer >= CYCLER_MAX_LAYERS || step < 0 || step > CYCLER_MAX_STEPS )
		return mEmptyTexture;
	
	// no FBO
	if ( mCurrentLayer < 0 )
		return mEmptyTexture;
	
	// Current
	if (step == 0)
		return mFbos[mCurrentLayer].getTexture();
	
	// get fbo
	int i = ( mCurrentLayer - (layer * step) );
	if (i < 0)
		i += CYCLER_MAX_FBOS;
	if ( ! mFbos[i] )
		return mEmptyTexture;

	// return texture!
	//printf("CYCLER getTexture   [%d]  + layer [%d] step [%d] = [%d]\n",mCurrentLayer,layer,step,i);
	return mFbos[i].getTexture();
}






