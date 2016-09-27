/*
 Copyright (c) 2014, Paul Houx - All rights reserved.
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

#pragma once

#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"

class SMAA {
  public:
	SMAA();

	void draw( const ci::gl::Texture2dRef &source, const ci::Area &bounds );
	void apply( const ci::gl::FboRef &destination, const ci::gl::FboRef &source );

	ci::gl::Texture2dRef  getEdgePass();
	ci::gl::Texture2dRef  getBlendPass();

  private:
	ci::gl::Fbo::Format   mFboFormat;
	ci::gl::FboRef        mFboEdgePass;
	ci::gl::FboRef        mFboBlendPass;

	ci::gl::BatchRef	mBatchFirstPass;	// edge detection
	ci::gl::BatchRef	mBatchSecondPass;	// blending weight calculation
	ci::gl::BatchRef	mBatchThirdPass;	// neighborhood blending

	// These textures contain look-up tables that speed up the SMAA process.
	ci::gl::Texture2dRef  mAreaTex;
	ci::gl::Texture2dRef  mSearchTex;

	// Size of our buffers, to be passed to the shaders.
	ci::vec4              mMetrics;

	void                  createBuffers( int width, int height );

	void                  doEdgePass( const ci::gl::Texture2dRef &source );
	void                  doBlendPass();
};