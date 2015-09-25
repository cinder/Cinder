/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

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

#include "cinder/gl/platform.h"
#include "cinder/gl/wrapper.h"
#include "cinder/gl/Texture.h"
#include "cinder/Noncopyable.h"

namespace cinder { namespace gl {

class Context;
typedef std::shared_ptr<class Vao>				VaoRef;
typedef std::shared_ptr<class BufferObj>		BufferObjRef;
typedef std::shared_ptr<class Fbo>				FboRef;
typedef std::shared_ptr<class Renderbuffer>		RenderbufferRef;

struct ScopedVao : private Noncopyable {
	ScopedVao( Vao *vao );
	ScopedVao( VaoRef &vao );
	~ScopedVao();

  private:
	Context		*mCtx;
};

struct ScopedBuffer : public Noncopyable {
	ScopedBuffer( const BufferObjRef &bufferObj );
	ScopedBuffer( GLenum target, GLuint id );
	~ScopedBuffer();
	
  private:
	Context		*mCtx;
	GLenum		mTarget;
};

struct ScopedState : private Noncopyable {
	ScopedState( GLenum cap, GLboolean value );
	~ScopedState();

  private:
	Context		*mCtx;
	GLenum		mCap;
};

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
	ScopedBlend( GLboolean enable );
	//! Parallels glBlendFunc(), and implicitly enables blending
	ScopedBlend( GLenum sfactor, GLenum dfactor );
	//! Parallels glBlendFuncSeparate(), and implicitly enables blending
	ScopedBlend( GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha );
	~ScopedBlend();
	
  private:
	Context		*mCtx;
	bool		mSaveFactors; // whether we should also set the blend factors rather than just the blend state
};

//! Enables blending state for the current scope and sets the blending function for standard alpha blending (`sfactor = GL_SRC_ALPHA, dfactor = GL_ONE_MINUS_SRC_ALPHA`).
struct ScopedBlendAlpha : private ScopedBlend {
	ScopedBlendAlpha()
		: ScopedBlend( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA )
	{}
};

//! Enables blending state for the current scope and sets the blending function for premultiplied alpha blending (`sfactor = GL_ONE, dfactor = GL_ONE_MINUS_SRC_ALPHA`).
struct ScopedBlendPremult : private ScopedBlend {
	ScopedBlendPremult()
		: ScopedBlend( GL_ONE, GL_ONE_MINUS_SRC_ALPHA )
	{}
};

//! Enables blending state for the current scope and sets the blending function additive blending (`sfactor = GL_SRC_ALPHA, dfactor = GL_ONE`).
struct ScopedBlendAdditive : public ScopedBlend
{
	ScopedBlendAdditive()
		: ScopedBlend( GL_SRC_ALPHA, GL_ONE )
	{}
};

struct ScopedGlslProg : private Noncopyable {
	ScopedGlslProg( GlslProgRef &prog );
	ScopedGlslProg( const std::shared_ptr<const GlslProg> &prog );
	ScopedGlslProg( const GlslProg *prog );
	~ScopedGlslProg();

  private:
	Context		*mCtx;
};

struct ScopedFramebuffer : private Noncopyable {
	ScopedFramebuffer( const FboRef &fbo, GLenum target = GL_FRAMEBUFFER );
	//! Prefer the FboRef variant when possible. This does not allow gl::Fbo to mark itself as needing multisample resolution.
	ScopedFramebuffer( GLenum target, GLuint framebufferId );
	~ScopedFramebuffer();
	
  private:
	Context		*mCtx;
	GLenum		mTarget;
};

struct ScopedActiveTexture : private Noncopyable {
	//! Sets the currently active texture through glActiveTexture. Expects values relative to \c 0, \em not GL_TEXTURE0
	ScopedActiveTexture( uint8_t textureUnit );
	~ScopedActiveTexture();
	
  private:
	Context		*mCtx;
};

struct ScopedTextureBind : private Noncopyable {
	ScopedTextureBind( GLenum target, GLuint textureId );
	ScopedTextureBind( GLenum target, GLuint textureId, uint8_t textureUnit );
	ScopedTextureBind( const TextureBaseRef &texture );
	ScopedTextureBind( const TextureBaseRef &texture, uint8_t textureUnit );

	//! \cond
	// These overloads are to alleviate a VS2013 bug where it cannot deduce
	// the correct constructor when a TextureBaseRef subclass is passed in
	ScopedTextureBind( const Texture2dRef &texture, uint8_t textureUnit );
#if ! defined( CINDER_GL_ES )
	ScopedTextureBind( const Texture1dRef &texture, uint8_t textureUnit );
#endif // ! defined( CINDER_GL_ES )
#if ! defined( CINDER_GL_ES_2 )
	ScopedTextureBind( const Texture3dRef &texture, uint8_t textureUnit );
#endif // ! defined( CINDER_GL_ES_2 )
	ScopedTextureBind( const TextureCubeMapRef &texture, uint8_t textureUnit );
	//! \endcond

	~ScopedTextureBind();
	
  private:
	Context		*mCtx;
	GLenum		mTarget;
	uint8_t		mTextureUnit;
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
	ScopedModelMatrix()		{ gl::pushModelMatrix(); }
	~ScopedModelMatrix()	{ gl::popModelMatrix(); }
};

struct ScopedViewMatrix : private Noncopyable {
	ScopedViewMatrix()	{ gl::pushViewMatrix(); }
	~ScopedViewMatrix()	{ gl::popViewMatrix(); }
};

struct ScopedProjectionMatrix : private Noncopyable {
	ScopedProjectionMatrix()	{ gl::pushProjectionMatrix(); }
	~ScopedProjectionMatrix()	{ gl::popProjectionMatrix(); }
};

//! Preserves all matrices
struct ScopedMatrices : private Noncopyable {
	ScopedMatrices()	{ gl::pushMatrices(); }
	~ScopedMatrices()	{ gl::popMatrices(); }
};

//! Scopes state of face culling.
struct ScopedFaceCulling : private Noncopyable {
	//! Enables or disables polygon culling based on \a cull
	ScopedFaceCulling( bool cull );
	//! Enables or disables polygon culling based on \a cull and specifies a mode, either \c GL_BACK or GL_FRONT
	ScopedFaceCulling( bool cull, GLenum cullFace );
	~ScopedFaceCulling();
	
  private:
	Context		*mCtx;
	bool		mSaveFace;
};

#if ! defined( CINDER_GL_ES )

//! Scopes state of logic op.
struct ScopedLogicOp : private Noncopyable {
	//! Enables or disables logical operation based on \a enable
	ScopedLogicOp( bool enable );
	//! Enables or disables logical operation based on \a enable and specifies a mode, \c GL_CLEAR, \c GL_SET, \c GL_COPY, \c GL_COPY_INVERTED, \c GL_NOOP, \c GL_INVERT, \c GL_AND, \c GL_NAND, \c GL_OR, \c GL_NOR, \c GL_XOR, \c GL_EQUIV, \c GL_AND_REVERSE, \c GL_AND_INVERTED, \c GL_OR_REVERSE, or \c GL_OR_INVERTED.
	ScopedLogicOp( bool enable, GLenum mode );
	~ScopedLogicOp();
	
private:
	Context		*mCtx;
	bool		mSaveMode;
};

#endif

//! Scopes state of depth testing and writing
struct ScopedDepth : private Noncopyable {
	//! Enables or disables both depth testing and writing to the depth buffer
	ScopedDepth( bool enableReadAndWrite );
	//! Enables or disables depth comparisons and writing to the depth buffer, and specifies a depth comparison function, either \c GL_NEVER, \c GL_LESS, \c GL_EQUAL, \c GL_LEQUAL, \c GL_GREATER, \c GL_NOTEQUAL, \c GL_GEQUAL and \c GL_ALWAYS.
	ScopedDepth( bool enableReadAndWrite, GLenum depthFunc );
	~ScopedDepth();
	
  private:
	Context		*mCtx;
	bool		mSaveMask;
	bool		mSaveFunc;
};

//! Scopes state to control the depth testing / reading operation. See information on c\ GL_DEPTH_TEST.
struct ScopedDepthTest : private Noncopyable {
	//! Enables or disables the depth testing / reading operation for the scope of this object.
	ScopedDepthTest( bool enableTest );
	//! Enables or disables the depth test operation, which controls reading and writing to the depth buffer, for the scope of this object. Also specifies a depth comparison function, either \c GL_NEVER, \c GL_LESS, \c GL_EQUAL, \c GL_LEQUAL, \c GL_GREATER, \c GL_NOTEQUAL, \c GL_GEQUAL and \c GL_ALWAYS (see info on `glDepthFunc`).
	ScopedDepthTest( bool enableTest, GLenum depthFunc );
	//! Destructor returns state to how it was before this object was constructed.
	~ScopedDepthTest();

  private:
	Context		*mCtx;
	bool		mSaveFunc;
};

//! Scopes state to control whether successful depth tests write to the depth buffer. See information on `glDepthMask()`. \note You must enable depth testing / reading (`GL_DEPTH_TEST`) for this to take place.
struct ScopedDepthWrite : private Noncopyable {
	//! Enables or disables writing to the depth buffer for the scope of this object.
	ScopedDepthWrite( bool enableWrite );
	//! Destructor returns state to how it was before this object was constructed.
	~ScopedDepthWrite();

  private:
	Context		*mCtx;
};

//! Scopes state of Renderbuffer binding
struct ScopedRenderbuffer : private Noncopyable {
	ScopedRenderbuffer( const RenderbufferRef &renderBuffer );
	ScopedRenderbuffer( GLenum target, GLuint id );
	~ScopedRenderbuffer();
	
  private:
	Context		*mCtx;
};

//! Scopes state of line width
struct ScopedLineWidth : private Noncopyable {
	ScopedLineWidth( float width );
	~ScopedLineWidth();
	
  private:
	Context		*mCtx;
};

#if ! defined( CINDER_GL_ES )

//! Scopes polygon rasterization mode for \c GL_FRONT_AND_BACK
struct ScopedPolygonMode : private Noncopyable {
	//! Values for \a mode may be \c GL_POINT, \c GL_LINE or \c GL_FILL.
	ScopedPolygonMode( GLenum mode );
	~ScopedPolygonMode();
	
  private:
	Context		*mCtx;
};

#endif // ! defined( CINDER_GL_ES )

//! Scopes winding order defining front-facing polygons
struct ScopedFrontFace : private Noncopyable {
	//! Values for \a mode may be \c GL_CW or \c GL_CCW
	ScopedFrontFace( GLenum mode );
	~ScopedFrontFace();
	
  private:
	Context		*mCtx;
};


} } // namespace cinder::gl
