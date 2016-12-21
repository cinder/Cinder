/*
 Copyright 2016 Google Inc.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.


 Copyright (c) 2016, The Cinder Project, All rights reserved.

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

#include "cinder/vk/BaseVkObject.h"
#include "cinder/vk/wrapper.h"
#include "cinder/Noncopyable.h"

namespace cinder { namespace vk {

class Context;
class ShaderProg;
using ShaderProgRef = std::shared_ptr<ShaderProg>;

struct ScopedColor : private Noncopyable {
	ScopedColor();
	ScopedColor( const ColorAf &color );
	ScopedColor( float red, float green, float blue, float alpha = 1 );
	~ScopedColor();

  private:
	Context		*mCtx;
	ColorAf		mColor;
};

//! Controls the current blend mode for the current scope.
struct ScopedBlend : private Noncopyable {
	//! Enables or disables blending (`GL_BLEND`) state.
	ScopedBlend( VkBool32 enable );
	//! Parallels glBlendFunc(), and implicitly enables blending
	ScopedBlend( VkBlendFactor sfactor, VkBlendFactor dfactor );
	//! Parallels glBlendFuncSeparate(), and implicitly enables blending
	ScopedBlend( VkBlendFactor srcRGB, VkBlendFactor dstRGB, VkBlendFactor srcAlpha, VkBlendFactor dstAlpha );
	~ScopedBlend();
	
  private:
	Context		*mCtx;
	bool		mSaveFactors; // whether we should also set the blend factors rather than just the blend state
};

//! Enables blending state for the current scope and sets the blending function for standard alpha blending (`sfactor = GL_SRC_ALPHA, dfactor = GL_ONE_MINUS_SRC_ALPHA`).
struct ScopedBlendAlpha : private ScopedBlend {
	ScopedBlendAlpha()
		: ScopedBlend( VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA )
	{}
};

//! Enables blending state for the current scope and sets the blending function for premultiplied alpha blending (`sfactor = GL_ONE, dfactor = GL_ONE_MINUS_SRC_ALPHA`).
struct ScopedBlendPremult : private ScopedBlend {
	ScopedBlendPremult()
		: ScopedBlend( VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA )
	{}
};

//! Enables blending state for the current scope and sets the blending function additive blending (`sfactor = GL_SRC_ALPHA, dfactor = GL_ONE`).
struct ScopedBlendAdditive : public ScopedBlend
{
	ScopedBlendAdditive()
		: ScopedBlend( VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE )
	{}
};

struct ScopedShaderProg : private Noncopyable {
	ScopedShaderProg( const ShaderProgRef &prog );
	~ScopedShaderProg();

  private:
	Context		*mCtx;
};

struct ScopedScissor : private Noncopyable {
	//! Implicitly enables scissor test
	ScopedScissor( const ivec2 &lowerLeftPosition, const ivec2 &dimension );
	//! Implicitly enables scissor test	
	ScopedScissor( int lowerLeftX, int lowerLeftY, int width, int height );
	~ScopedScissor();

  private:
	Context					*mCtx;
};

struct ScopedViewport : private Noncopyable {
	ScopedViewport( const ivec2 &lowerLeftPosition, const ivec2 &dimension );
	ScopedViewport( const ivec2 &size );
	ScopedViewport( int lowerLeftX, int lowerLeftY, int width, int height );
	~ScopedViewport();

  private:
	Context					*mCtx;
};

struct ScopedModelMatrix : private Noncopyable {
	ScopedModelMatrix()		{ vk::pushModelMatrix(); }
	~ScopedModelMatrix()	{ vk::popModelMatrix(); }
};

struct ScopedViewMatrix : private Noncopyable {
	ScopedViewMatrix()	{ vk::pushViewMatrix(); }
	~ScopedViewMatrix()	{ vk::popViewMatrix(); }
};

struct ScopedProjectionMatrix : private Noncopyable {
	ScopedProjectionMatrix()	{ vk::pushProjectionMatrix(); }
	~ScopedProjectionMatrix()	{ vk::popProjectionMatrix(); }
};

//! Preserves all matrices
struct ScopedMatrices : private Noncopyable {
	ScopedMatrices()	{ vk::pushMatrices(); }
	~ScopedMatrices()	{ vk::popMatrices(); }
};


}} // namespace cinder::vk