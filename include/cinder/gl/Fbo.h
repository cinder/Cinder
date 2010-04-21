/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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

#include "cinder/Cinder.h"
#include "cinder/Exception.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

namespace cinder { namespace gl {

//! Represents a reference-counted OpenGL Framebuffer Object
class Fbo {
 public:
	struct Format;

	//! Creates a NULL FBO
	Fbo() {}
	//! Creates an FBO \a width pixels wide and \a height pixels high, using Fbo::Format \a format
	Fbo( int width, int height, Format format = Format() );
	//! Creates an FBO \a width pixels wide and \a height pixels high, with an optional alpha channel, color buffer and depth buffer
	Fbo( int width, int height, bool alpha, bool color = true, bool depth = true );

	//! Returns the width of the FBO in pixels
	int				getWidth() const { return mObj->mWidth; }
	//! Returns the height of the FBO in pixels
	int				getHeight() const { return mObj->mHeight; }
	//! Returns the size of the FBO in pixels
	Vec2i			getSize() const { return Vec2i( mObj->mWidth, mObj->mHeight ); }
	//! Returns the bounding area of the FBO in pixels
	Area			getBounds() const { return Area( 0, 0, mObj->mWidth, mObj->mHeight ); }
	//! Returns the aspect ratio of the FBO
	float			getAspectRatio() const { return mObj->mWidth / (float)mObj->mHeight; }
	//! Returns the Fbo::Format of this FBO
	const Format&	getFormat() const { return mObj->mFormat; }
	//! Returns the texture target for this FBO. Typically \c GL_TEXTURE_2D or \c GL_TEXTURE_RECTANGLE_ARB
	GLenum			getTarget() const { return mObj->mFormat.mTarget; }

	//! Returns a reference to the color texture of the FBO
	Texture&		getTexture();
	//! Returns a reference to the depth texture of the FBO. Depth textures are not created when multisampling is used.
	Texture&		getDepthTexture();	
	
	//! Binds the color texture associated with an Fbo to its target. Optionally binds to a multitexturing unit when \a textureUnit is non-zero
	void 			bindTexture( int textureUnit = 0 );
	//! Unbinds the texture associated with an Fbo's target
	void			unbindTexture();
	//! Binds the depth texture associated with an Fbo to its target. Optionally binds to a multitexturing unit when \a textureUnit is non-zero
	void 			bindDepthTexture( int textureUnit = 0 );
	//! Binds the Fbo as the currently active framebuffer, meaning it will receive the results of all subsequent rendering until it is unbound
	void 			bindFramebuffer();
	//! Unbinds the Fbo as the currently active framebuffer, restoring the primary context as the target for all subsequent rendering
	static void 	unbindFramebuffer();

	//! Returns the maximum number of samples the graphics card is capable of using per pixel in MSAA for an Fbo
	static GLint	getMaxSamples();
	
	struct Format {
	  public:
		//! Default constructor, sets the target to \c GL_TEXTURE_2D with an 8-bit color+alpha, a 24-bit depth texture, and no multisampling or mipmapping
		Format();

		//! Set the texture target associated with the FBO. Defaults to \c GL_TEXTURE_2D, \c GL_TEXTURE_RECTANGLE_ARB is a common option as well
		void	setTarget( GLenum target ) { mTarget = target; }
		//! Sets the GL internal format for the color buffer. Defaults to \c GL_RGBA8. Common options also include <tt>GL_RGB8 and GL_RGBA32F</tt>
		void	setColorInternalFormat( GLenum colorInternalFormat ) { mColorInternalFormat = colorInternalFormat; }
		//! Sets the GL internal format for the depth buffer. Defaults to \c GL_DEPTH_COMPONENT24. Common options also include <tt>GL_DEPTH_COMPONENT16 and GL_DEPTH_COMPONENT32</tt>
		void	setDepthInternalFormat( GLenum depthInternalFormat ) { mDepthInternalFormat = depthInternalFormat; }
		//! Sets the number of samples used in MSAA-style antialiasing. Defaults to none, disabling multisampling. Note that not all implementations support multisampling.
		void	setSamples( int samples ) { mSamples = samples; }
		//! Sets the number of coverage samples used in CSAA-style antialiasing. Defaults to none. Note that not all implementations support CSAA, and is currenlty Windows-only Nvidia.
		void	setCoverageSamples( int coverageSamples ) { mCoverageSamples = coverageSamples; }
		//! Enables or disables the creation of a color buffer for the FBO.
		void	enableColorBuffer( bool colorBuffer = true ) { mColorBuffer = colorBuffer; }
		//! Enables or disables the creation of a depth buffer for the FBO.
		void	enableDepthBuffer( bool depthBuffer = true ) { mDepthBuffer = depthBuffer; }
//		void	enableStencilBuffer( bool stencilBuffer = true ) { mStencilBuffer = stencilBuffer; }
		//! Enables or disables mip-mapping for the FBO's textures
		void	enableMipmapping( bool enableMipmapping = true ) { mMipmapping = enableMipmapping; }

		//! Sets the wrapping behavior for the FBO's textures. Possible values are \c GL_CLAMP, \c GL_REPEAT and \c GL_CLAMP_TO_EDGE. Default is \c GL_CLAMP_TO_EDGE.
		void	setWrap( GLenum wrapS, GLenum wrapT ) { setWrapS( wrapS ); setWrapT( wrapT ); }
		/** \brief Sets the horizontal wrapping behavior for the FBO's textures. Default is \c GL_CLAMP_TO_EDGE.
			Possible values are \c GL_CLAMP, \c GL_REPEAT and \c GL_CLAMP_TO_EDGE. **/
		void	setWrapS( GLenum wrapS ) { mWrapS = wrapS; }
		/** \brief Sets the vertical wrapping behavior for the FBO's textures. Default is \c GL_CLAMP_TO_EDGE.
			Possible values are \c GL_CLAMP, \c GL_REPEAT and \c GL_CLAMP_TO_EDGE. **/
		void	setWrapT( GLenum wrapT ) { mWrapT = wrapT; }
		/** \brief Sets the minification filtering behavior for the FBO's textures. Default is \c GL_LINEAR:
		 * Possible values are \li \c GL_NEAREST \li \c GL_LINEAR \li \c GL_NEAREST_MIPMAP_NEAREST \li \c GL_LINEAR_MIPMAP_NEAREST \li \c GL_NEAREST_MIPMAP_LINEAR \li \c GL_LINEAR_MIPMAP_LINEAR **/
		void	setMinFilter( GLenum minFilter ) { mMinFilter = minFilter; }
		/** Sets the magnification filtering behavior for the FBO's textures. Default is \c GL_LINEAR:
		 * Possible values are \li \c GL_NEAREST \li \c GL_LINEAR \li \c GL_NEAREST_MIPMAP_NEAREST \li \c GL_LINEAR_MIPMAP_NEAREST \li \c GL_NEAREST_MIPMAP_LINEAR \li \c GL_LINEAR_MIPMAP_LINEAR **/
		void	setMagFilter( GLenum magFilter ) { mMagFilter = magFilter; }

		
		//! Returns the texture target associated with the FBO.
		GLenum	getTarget() const { return mTarget; }
		//! Returns the GL internal format for the color buffer. Defaults to \c GL_RGBA8.
		GLenum	getColorInternalFormat() const { return mColorInternalFormat; }
		//! Returns the GL internal format for the depth buffer. Defaults to \c GL_DEPTH_COMPONENT24
		GLenum	getDepthInternalFormat() const { return mDepthInternalFormat; }
		//! Returns the number of samples used in MSAA-style antialiasing. Defaults to none, disabling multisampling
		int		getSamples() const { return mSamples; }
		//! Returns the number of coverage samples used in CSAA-style antialiasing. Defaults to none.
		int		getCoverageSamples() const { return mCoverageSamples; }
		//! Returns whether the FBO contains a color buffer
		bool	hasColorBuffer() const { return mColorBuffer; }
		//! Returns whether the FBO contains a depth buffer
		bool	hasDepthBuffer() const { return mDepthBuffer; }
//		bool	hasStencilBuffer() const { return mStencilBuffer; }
		//! Returns whether the contents of the FBO textures are mip-mapped.
		bool	hasMipMapping() const { return mMipmapping; }
		
	  protected:
		GLenum		mTarget;
		GLenum		mColorInternalFormat, mDepthInternalFormat;
		int			mSamples;
		int			mCoverageSamples;
		bool		mMipmapping;
		bool		mColorBuffer, mDepthBuffer, mStencilBuffer;
		GLenum		mWrapS, mWrapT;
		GLenum		mMinFilter, mMagFilter;
		
		friend class Fbo;
	};

 protected:
	void		init();
	bool		initMultisample( bool csaa );
	void		resolveTexture() const;
	void		updateMipmaps( bool bindFirst ) const;
	bool		checkStatus( class FboExceptionInvalidSpecification *resultExc );

	struct Obj {
		Obj();
		Obj( int aWidth, int aHeight );
		~Obj();

		int					mWidth, mHeight;
		Format				mFormat;
		GLuint				mId, mColorTextureId, mDepthTextureId;
		GLuint				mColorRenderBufferId, mDepthRenderBufferId, mResolveFramebufferId;
		Texture				mColorTexture, mDepthTexture;
		mutable bool		mNeedsResolve, mNeedsMipmapUpdate;
	};
 
	shared_ptr<Obj>		mObj;
	
	static GLint		sMaxSamples;
	
  public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef shared_ptr<Obj> Fbo::*unspecified_bool_type;
	operator unspecified_bool_type() { return ( mObj.get() == 0 ) ? 0 : &Fbo::mObj; }
	void reset() { mObj.reset(); }
	//@}  	
};

class FboException : public Exception {
};

class FboExceptionInvalidSpecification : public FboException {
  public:
	FboExceptionInvalidSpecification() : FboException() { mMessage[0] = 0; }
	FboExceptionInvalidSpecification( const std::string &message ) throw();
	
	virtual const char * what() const throw() { return mMessage; }
	
  private:	
	char	mMessage[256];
};

} // namespace gl
} // namespace cinder
