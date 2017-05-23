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

#include "cinder/gl/scoped.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/BufferObj.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Sampler.h"
#include "cinder/CinderAssert.h"

using namespace std;

namespace cinder { namespace gl {

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedVao
ScopedVao::ScopedVao( Vao *vao )
	: mCtx( gl::context() )
{
	mCtx->pushVao( vao );
}

ScopedVao::ScopedVao( VaoRef &vao )
	: mCtx( gl::context() )
{
	mCtx->pushVao( vao );
}

ScopedVao::~ScopedVao()
{
	mCtx->popVao();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedBuffer
ScopedBuffer::ScopedBuffer( const BufferObjRef &bufferObj )
	: mCtx( gl::context() ), mTarget( bufferObj->getTarget() )
{
	mCtx->pushBufferBinding( mTarget, bufferObj->getId() );
}

ScopedBuffer::ScopedBuffer( GLenum target, GLuint id )
		: mCtx( gl::context() ), mTarget( target )
{
	mCtx->pushBufferBinding( target, id );
}

ScopedBuffer::~ScopedBuffer()
{
	mCtx->popBufferBinding( mTarget );
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedState
ScopedState::ScopedState( GLenum cap, GLboolean value )
	: mCtx( gl::context() ), mCap( cap )
{
	mCtx->pushBoolState( cap, value );
}

ScopedState::~ScopedState() {
	mCtx->popBoolState( mCap );
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedColor
ScopedColor::ScopedColor()
	: mCtx( gl::context() )
{
	mColor = mCtx->getCurrentColor();
}

ScopedColor::ScopedColor( const ColorAf &color )
	: mCtx( gl::context() )
{
	mColor = mCtx->getCurrentColor();
	mCtx->setCurrentColor( color );
}

ScopedColor::ScopedColor( float red, float green, float blue, float alpha )
	: mCtx( gl::context() )
{
	mColor = mCtx->getCurrentColor();
	mCtx->setCurrentColor( ColorA( red, green, blue, alpha ) );	
}

ScopedColor::~ScopedColor()
{
	mCtx->setCurrentColor( mColor );
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedBlend
ScopedBlend::ScopedBlend( GLboolean enable )
	: mCtx( gl::context() ), mSaveFactors( false )
{
	mCtx->pushBoolState( GL_BLEND, enable );
}

//! Parallels glBlendFunc(), implicitly enables blending
ScopedBlend::ScopedBlend( GLenum sfactor, GLenum dfactor )
	: mCtx( gl::context() ), mSaveFactors( true )
{
	mCtx->pushBoolState( GL_BLEND, GL_TRUE );
	mCtx->pushBlendFuncSeparate( sfactor, dfactor, sfactor, dfactor );
}

//! Parallels glBlendFuncSeparate(), implicitly enables blending
ScopedBlend::ScopedBlend( GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha )
	: mCtx( gl::context() ), mSaveFactors( true )
{
	mCtx->pushBoolState( GL_BLEND, GL_TRUE );
	mCtx->pushBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha );
}

ScopedBlend::~ScopedBlend()
{
	mCtx->popBoolState( GL_BLEND );
	if( mSaveFactors )
		mCtx->popBlendFuncSeparate();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedGlslProg
ScopedGlslProg::ScopedGlslProg( GlslProgRef& prog )
	: mCtx( gl::context() )
{
	mCtx->pushGlslProg( prog.get() );
}

ScopedGlslProg::ScopedGlslProg( const std::shared_ptr<const GlslProg> &prog )
	: mCtx( gl::context() )
{
	mCtx->pushGlslProg( std::const_pointer_cast<GlslProg>( prog ).get() );
}

ScopedGlslProg::ScopedGlslProg( const GlslProg* prog )
	: mCtx( gl::context() )
{
	mCtx->pushGlslProg( const_cast<GlslProg*>( prog ) );
}

ScopedGlslProg::~ScopedGlslProg()
{
	mCtx->popGlslProg();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedFramebuffer
ScopedFramebuffer::ScopedFramebuffer( const FboRef &fbo, GLenum target )
	: mCtx( gl::context() ), mTarget( target )
{
	mCtx->pushFramebuffer( fbo, target );
}

ScopedFramebuffer::ScopedFramebuffer( GLenum target, GLuint framebufferId )
	: mCtx( gl::context() ), mTarget( target )
{
	mCtx->pushFramebuffer( target, framebufferId );
}

ScopedFramebuffer::~ScopedFramebuffer()
{	
#if ! defined( CINDER_GL_HAS_FBO_MULTISAMPLING )
	mCtx->popFramebuffer( GL_FRAMEBUFFER );
#else
	if( mTarget == GL_FRAMEBUFFER || mTarget == GL_READ_FRAMEBUFFER )
		mCtx->popFramebuffer( GL_READ_FRAMEBUFFER );
	if( mTarget == GL_FRAMEBUFFER || mTarget == GL_DRAW_FRAMEBUFFER )
		mCtx->popFramebuffer( GL_DRAW_FRAMEBUFFER );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedActiveTexture
ScopedActiveTexture::ScopedActiveTexture( uint8_t textureUnit )
	: mCtx( gl::context() )
{
	mCtx->pushActiveTexture( textureUnit );
}
	
ScopedActiveTexture::~ScopedActiveTexture()
{
	mCtx->popActiveTexture();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedTextureBind
ScopedTextureBind::ScopedTextureBind( GLenum target, GLuint textureId )
	: mCtx( gl::context() ), mTarget( target )
{
	mTextureUnit = mCtx->getActiveTexture();
	mCtx->pushTextureBinding( mTarget, textureId, mTextureUnit );
}

ScopedTextureBind::ScopedTextureBind( GLenum target, GLuint textureId, uint8_t textureUnit )
	: mCtx( gl::context() ), mTarget( target ), mTextureUnit( textureUnit )
{
	mCtx->pushTextureBinding( mTarget, textureId, mTextureUnit );
}

ScopedTextureBind::ScopedTextureBind( const TextureBaseRef &texture )
	: mCtx( gl::context() ), mTarget( texture->getTarget() )
{
	mTextureUnit = mCtx->getActiveTexture();
	mCtx->pushTextureBinding( mTarget, texture->getId(), mTextureUnit );
}

ScopedTextureBind::ScopedTextureBind( const TextureBaseRef &texture, uint8_t textureUnit )
	: mCtx( gl::context() ), mTarget( texture->getTarget() ), mTextureUnit( textureUnit )
{
	mCtx->pushTextureBinding( mTarget, texture->getId(), mTextureUnit );
}

// ----------------------------------------------------------------
// These overloads are to alleviate a VS2013 bug where it cannot deduce
// the correct constructor when a TextureBaseRef subclass is passed in
ScopedTextureBind::ScopedTextureBind( const Texture2dRef &texture, uint8_t textureUnit )
	: mCtx( gl::context() ), mTarget( texture->getTarget() ), mTextureUnit( textureUnit )
{
	mCtx->pushTextureBinding( mTarget, texture->getId(), mTextureUnit );
}

#if ! defined( CINDER_GL_ES )
ScopedTextureBind::ScopedTextureBind( const Texture1dRef &texture, uint8_t textureUnit )
: mCtx( gl::context() ), mTarget( texture->getTarget() ), mTextureUnit( textureUnit )
{
	mCtx->pushTextureBinding( mTarget, texture->getId(), mTextureUnit );
}
#endif // ! defined( CINDER_GL_ES )

#if ! defined( CINDER_GL_ES_2 )
ScopedTextureBind::ScopedTextureBind( const Texture3dRef &texture, uint8_t textureUnit )
	: mCtx( gl::context() ), mTarget( texture->getTarget() ), mTextureUnit( textureUnit )
{
	mCtx->pushTextureBinding( mTarget, texture->getId(), mTextureUnit );
}
#endif // ! defined( CINDER_GL_ES_2 )

ScopedTextureBind::ScopedTextureBind( const TextureCubeMapRef &texture, uint8_t textureUnit )
	: mCtx( gl::context() ), mTarget( texture->getTarget() ), mTextureUnit( textureUnit )
{
	mCtx->pushTextureBinding( mTarget, texture->getId(), mTextureUnit );
}
// ----------------------------------------------------------------

ScopedTextureBind::~ScopedTextureBind()
{
	mCtx->popTextureBinding( mTarget, mTextureUnit );
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedSamplerBind
#if defined( CINDER_GL_HAS_SAMPLERS )
ScopedSamplerBind::ScopedSamplerBind( GLuint samplerId, uint8_t textureUnit )
	: mCtx( gl::context() ), mTextureUnit( textureUnit )
{
	mCtx->pushSamplerBinding( textureUnit, samplerId );
}

ScopedSamplerBind::ScopedSamplerBind( const Sampler &sampler, uint8_t textureUnit )
	: mCtx( gl::context() ), mTextureUnit( textureUnit )
{
	mCtx->pushSamplerBinding( textureUnit, sampler.getId() );
}

ScopedSamplerBind::ScopedSamplerBind( const SamplerRef &sampler, uint8_t textureUnit )
	: mCtx( gl::context() ), mTextureUnit( textureUnit )
{
	mCtx->pushSamplerBinding( textureUnit, sampler->getId() );
}

ScopedSamplerBind::~ScopedSamplerBind()
{
	mCtx->popSamplerBinding( mTextureUnit );
}
#endif // defined( CINDER_GL_HAS_SAMPLERS )


///////////////////////////////////////////////////////////////////////////////////////////
// ScopedScissor
ScopedScissor::ScopedScissor( const ivec2 &lowerLeftPosition, const ivec2 &dimension )
	: mCtx( gl::context() )
{
	mCtx->pushBoolState( GL_SCISSOR_TEST, GL_TRUE );
	mCtx->pushScissor( std::pair<ivec2, ivec2>( lowerLeftPosition, dimension ) );
}

ScopedScissor::ScopedScissor( int lowerLeftX, int lowerLeftY, int width, int height )
	: mCtx( gl::context() )
{
	mCtx->pushBoolState( GL_SCISSOR_TEST, GL_TRUE );
	mCtx->pushScissor( std::pair<ivec2, ivec2>( ivec2( lowerLeftX, lowerLeftY ), ivec2( width, height ) ) );		
}
	
ScopedScissor::~ScopedScissor()
{
	mCtx->popBoolState( GL_SCISSOR_TEST );
	mCtx->popScissor();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedFaceCulling
ScopedFaceCulling::ScopedFaceCulling( bool cull )
	: mCtx( gl::context() ), mSaveFace( false )
{
	mCtx->pushBoolState( GL_CULL_FACE, cull );
}

ScopedFaceCulling::ScopedFaceCulling( bool cull, GLenum face )
	: mCtx( gl::context() ), mSaveFace( true )
{
	mCtx->pushBoolState( GL_CULL_FACE, cull );
	mCtx->pushCullFace( face );
}

ScopedFaceCulling::~ScopedFaceCulling()
{
	mCtx->popBoolState( GL_CULL_FACE );
	if( mSaveFace )
		mCtx->popCullFace();
}

#if ! defined( CINDER_GL_ES )

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedLogicOp
ScopedLogicOp::ScopedLogicOp( bool enable )
: mCtx( gl::context() ), mSaveMode( false )
{
	mCtx->pushBoolState( GL_COLOR_LOGIC_OP, enable );
}

ScopedLogicOp::ScopedLogicOp( bool enable, GLenum mode )
: mCtx( gl::context() ), mSaveMode( true )
{
	mCtx->pushBoolState( GL_COLOR_LOGIC_OP, enable );
	mCtx->pushLogicOp( mode );
}

ScopedLogicOp::~ScopedLogicOp()
{
	mCtx->popBoolState( GL_COLOR_LOGIC_OP );
	if( mSaveMode )
		mCtx->popLogicOp();
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedDepth
ScopedDepth::ScopedDepth( bool enableReadAndWrite )
	: mCtx( gl::context() ), mSaveMask( true ), mSaveFunc( false )
{
	mCtx->pushBoolState( GL_DEPTH_TEST, enableReadAndWrite );
	mCtx->pushDepthMask( enableReadAndWrite );
}
	
ScopedDepth::ScopedDepth( bool enableReadAndWrite, GLenum depthFunc )
	: mCtx( gl::context() ), mSaveMask( true ), mSaveFunc( true )
{
	mCtx->pushBoolState( GL_DEPTH_TEST, enableReadAndWrite );
	mCtx->pushDepthMask( enableReadAndWrite );
	mCtx->pushDepthFunc( depthFunc );
}

ScopedDepth::~ScopedDepth()
{
	mCtx->popBoolState( GL_DEPTH_TEST );
	if( mSaveMask )
		mCtx->popDepthMask();
	if( mSaveFunc )
		mCtx->popDepthFunc();
}

ScopedDepthTest::ScopedDepthTest( bool enableTest )
	: mCtx( gl::context() ), mSaveFunc( false )
{
	mCtx->pushBoolState( GL_DEPTH_TEST, enableTest );
}

ScopedDepthTest::ScopedDepthTest( bool enableTest, GLenum depthFunc )
	: mCtx( gl::context() ), mSaveFunc( true )
{
	mCtx->pushBoolState( GL_DEPTH_TEST, enableTest );
	mCtx->pushDepthFunc( depthFunc );
}

ScopedDepthTest::~ScopedDepthTest()
{
	mCtx->popBoolState( GL_DEPTH_TEST );
	if( mSaveFunc )
		mCtx->popDepthFunc();
}

ScopedDepthWrite::ScopedDepthWrite( bool enableWrite )
	: mCtx( gl::context() )
{
	mCtx->pushDepthMask( enableWrite );
}

ScopedDepthWrite::~ScopedDepthWrite()
{
	mCtx->popDepthMask();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedRenderbuffer
ScopedRenderbuffer::ScopedRenderbuffer( const RenderbufferRef &renderBuffer )
	: mCtx( gl::context() )
{
	mCtx->pushRenderbufferBinding( GL_RENDERBUFFER, renderBuffer->getId() );
}

ScopedRenderbuffer::ScopedRenderbuffer( GLenum target, GLuint id )
	: mCtx( gl::context() )
{
	// this is the only legal value currently
	CI_ASSERT( target == GL_RENDERBUFFER );
	mCtx->pushRenderbufferBinding( target, id );
}

ScopedRenderbuffer::~ScopedRenderbuffer()
{
	mCtx->popRenderbufferBinding( GL_RENDERBUFFER );
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedViewport
ScopedViewport::ScopedViewport( const ivec2 &lowerLeftPosition, const ivec2 &dimension )
	: mCtx( gl::context() )
{
	mCtx->pushViewport( { lowerLeftPosition, dimension } );
}

ScopedViewport::ScopedViewport( const ivec2 &size )
	: mCtx( gl::context() )
{
	mCtx->pushViewport( { ivec2( 0 ), size } );
}

ScopedViewport::ScopedViewport( int lowerLeftX, int lowerLeftY, int width, int height )
	: mCtx( gl::context() )
{
	mCtx->pushViewport( { ivec2( lowerLeftX, lowerLeftY ), ivec2( width, height ) } );
}
	
ScopedViewport::~ScopedViewport()
{
	mCtx->popViewport();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedLineWidth
ScopedLineWidth::ScopedLineWidth( float width )
	: mCtx( gl::context() )
{
	mCtx->pushLineWidth( width );
}

ScopedLineWidth::~ScopedLineWidth()
{
	mCtx->popLineWidth();
}

#if ! defined( CINDER_GL_ES )

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedPolygonMode
ScopedPolygonMode::ScopedPolygonMode( GLenum mode )
	: mCtx( gl::context() )
{
	mCtx->pushPolygonMode( GL_FRONT_AND_BACK, mode );
}

ScopedPolygonMode::~ScopedPolygonMode()
{
	mCtx->popPolygonMode( GL_FRONT_AND_BACK );
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedFrontFace
ScopedFrontFace::ScopedFrontFace( GLenum mode )
	: mCtx( gl::context() )
{
	mCtx->pushFrontFace( mode );
}

ScopedFrontFace::~ScopedFrontFace()
{
	mCtx->popFrontFace();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedDebugGroup
#if defined( CINDER_GL_HAS_KHR_DEBUG )
ScopedDebugGroup::ScopedDebugGroup( const std::string &message )
{
	pushDebugGroup( message );
}
ScopedDebugGroup::ScopedDebugGroup( GLuint id, const std::string &message )
{
	pushDebugGroup( id, message );
}
ScopedDebugGroup::ScopedDebugGroup( GLenum source, GLuint id, const std::string &message )
{
	pushDebugGroup( source, id, message ); 
}
ScopedDebugGroup::~ScopedDebugGroup()
{
	popDebugGroup();
}
#endif

} } // namespace cinder::gl
