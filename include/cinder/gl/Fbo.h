/*
 Copyright (c) 2013, The Cinder Project
 All rights reserved.
 
 This code is designed for use with the Cinder C++ library, http://libcinder.org

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
#include "cinder/gl/Texture.h"
#include "cinder/Matrix44.h"

#include <map>
#include <iosfwd>

namespace cinder { namespace gl {

class Renderbuffer;
typedef std::shared_ptr<Renderbuffer>	RenderbufferRef;
class Fbo;
typedef std::shared_ptr<Fbo>			FboRef;
class FboCubeMap;
typedef std::shared_ptr<FboCubeMap>		FboCubeMapRef;

//! Represents an OpenGL Renderbuffer, used primarily in conjunction with FBOs. Supported on OpenGL ES but multisampling is currently ignored. \ImplShared
class CI_API Renderbuffer {
  public:
	//! Create a Renderbuffer \a width pixels wide and \a heigh pixels high, with an internal format of \a internalFormat, defaulting to GL_RGBA8, MSAA samples \a msaaSamples, and CSAA samples \a coverageSamples
#if defined( CINDER_GL_ES_2 )
	static RenderbufferRef create( int width, int height, GLenum internalFormat = GL_RGBA8_OES, int msaaSamples = 0, int coverageSamples = 0 );
#else
	static RenderbufferRef create( int width, int height, GLenum internalFormat = GL_RGBA8, int msaaSamples = 0, int coverageSamples = 0 );
#endif

	~Renderbuffer();

	//! Returns the width of the Renderbuffer in pixels
	int		getWidth() const { return mWidth; }
	//! Returns the height of the Renderbuffer in pixels
	int		getHeight() const { return mHeight; }
	//! Returns the size of the Renderbuffer in pixels
	ivec2	getSize() const { return ivec2( mWidth, mHeight ); }
	//! Returns the bounding area of the Renderbuffer in pixels
	Area	getBounds() const { return Area( 0, 0, mWidth, mHeight ); }
	//! Returns the aspect ratio of the Renderbuffer
	float	getAspectRatio() const { return mWidth / (float)mHeight; }

	//! Returns the ID of the Renderbuffer
	GLuint	getId() const { return mId; }
	//! Returns the internal format of the Renderbuffer
	GLenum	getInternalFormat() const { return mInternalFormat; }
	//! Returns the number of samples used in MSAA-style antialiasing. Defaults to none, disabling multisampling
	int		getSamples() const { return mSamples; }
	//! Returns the number of coverage samples used in CSAA-style antialiasing. Defaults to none.
	int		getCoverageSamples() const { return mCoverageSamples; }

	//! Returns the debugging label associated with the Renderbuffer.
	const std::string&	getLabel() const { return mLabel; }
	//! Sets the debugging label associated with the Renderbuffer. Calls glObjectLabel() when available.
	void				setLabel( const std::string &label );

  private:
	//! Create a Renderbuffer \a width pixels wide and \a heigh pixels high, with an internal format of \a internalFormat, MSAA samples \a msaaSamples, and CSAA samples \a coverageSamples
	Renderbuffer( int width, int height, GLenum internalFormat, int msaaSamples, int coverageSamples );  
  
	void	init( int aWidth, int aHeight, GLenum internalFormat, int msaaSamples, int coverageSamples );
  
	int					mWidth, mHeight;
	GLuint				mId;
	GLenum				mInternalFormat;
	int					mSamples, mCoverageSamples;
	std::string			mLabel; // debug label
	
	friend CI_API std::ostream& operator<<( std::ostream &os, const Renderbuffer &rhs );
};

CI_API std::ostream& operator<<( std::ostream &os, const Renderbuffer &rhs );

//! Represents an OpenGL Framebuffer Object.
class CI_API Fbo : public std::enable_shared_from_this<Fbo> {
  public:
	struct Format;

	//! Creates an FBO \a width pixels wide and \a height pixels high, using Fbo::Format \a format
	static FboRef create( int width, int height, const Format &format = Format() );
	//! Creates an FBO \a width pixels wide and \a height pixels high, a color texture (with optional \a alpha channel), and optionally a \a depth buffer and \a stencil buffer
	static FboRef create( int width, int height, bool alpha, bool depth = true, bool stencil = false );
	~Fbo();

	//! Returns the width of the FBO in pixels
	int				getWidth() const { return mWidth; }
	//! Returns the height of the FBO in pixels
	int				getHeight() const { return mHeight; }
	//! Returns the size of the FBO in pixels
	ivec2			getSize() const { return ivec2( mWidth, mHeight ); }
	//! Returns the bounding area of the FBO in pixels
	Area			getBounds() const { return Area( 0, 0, mWidth, mHeight ); }
	//! Returns the aspect ratio of the FBO
	float			getAspectRatio() const { return mWidth / (float)mHeight; }
	//! Returns the Fbo::Format of this FBO
	const Format&	getFormat() const { return mFormat; }
	//! Returns the Fbo::Format of this FBO
	Format			getFormat() { return mFormat; }

	//! Returns a reference to the color Texture2d of the FBO (at \c GL_COLOR_ATTACHMENT0). Resolves multisampling and renders mipmaps if necessary. Returns an empty Ref if there is no Texture2d attached at \c GL_COLOR_ATTACHMENT0
	Texture2dRef	getColorTexture();	
	//! Returns a reference to the depth Texture2d of the FBO. Resolves multisampling and renders mipmaps if necessary. Returns an empty Ref if there is no Texture2d as a depth attachment.
	Texture2dRef	getDepthTexture();
	//! Returns a Texture2dRef attached at \a attachment (such as \c GL_COLOR_ATTACHMENT0). Resolves multisampling and renders mipmaps if necessary. Returns NULL if a Texture2d is not bound at \a attachment.
	Texture2dRef	getTexture2d( GLenum attachment );
	//! Returns a TextureBaseRef attached at \a attachment (such as \c GL_COLOR_ATTACHMENT0). Resolves multisampling and renders mipmaps if necessary. Returns NULL if a Texture is not bound at \a attachment.
	TextureBaseRef	getTextureBase( GLenum attachment );
	
	//! Binds the color texture associated with an Fbo to its target. Optionally binds to a multitexturing unit when \a textureUnit is non-zero. Optionally binds to a multitexturing unit when \a textureUnit is non-zero. \a attachment specifies which color buffer in the case of multiple attachments.
	void 			bindTexture( int textureUnit = 0, GLenum attachment = GL_COLOR_ATTACHMENT0 );
	//! Unbinds the texture associated with an Fbo attachment
	void			unbindTexture( int textureUnit = 0, GLenum attachment = GL_COLOR_ATTACHMENT0 );
	//! Binds the Fbo as the currently active framebuffer, meaning it will receive the results of all subsequent rendering until it is unbound
	void 			bindFramebuffer( GLenum target = GL_FRAMEBUFFER );
	//! Unbinds the Fbo as the currently active framebuffer, restoring the primary context as the target for all subsequent rendering
	static void 	unbindFramebuffer();
	//! Resolves internal Multisample FBO to attached Textures. Only necessary when not using getColorTexture() or getTexture(), which resolve automatically.
	void			resolveTextures() const;

	//! Returns the ID of the framebuffer. For antialiased FBOs this is the ID of the output multisampled FBO
	GLuint		getId() const { if( mMultisampleFramebufferId ) return mMultisampleFramebufferId; else return mId; }

	//! For antialiased FBOs this returns the ID of the mirror FBO designed for multisampled writing. Returns 0 otherwise.
	GLuint		getMultisampleId() const { return mMultisampleFramebufferId; }
	//! Returns the resolve FBO, which is the same value as getId() without multisampling
	GLuint		getResolveId() const { return mId; }

	//! Marks multisampling framebuffer and mipmaps as needing updates. Not generally necessary to call directly.
	void		markAsDirty();

#if ! defined( CINDER_GL_ES_2 )
	//! Copies to FBO \a dst from \a srcArea to \a dstArea using filter \a filter. \a mask allows specification of color (\c GL_COLOR_BUFFER_BIT) and/or depth(\c GL_DEPTH_BUFFER_BIT). Calls glBlitFramebufferEXT() and is subject to its constraints and coordinate system.
	void		blitTo( const FboRef &dst, const Area &srcArea, const Area &dstArea, GLenum filter = GL_NEAREST, GLbitfield mask = GL_COLOR_BUFFER_BIT ) const;
	//! Copies to the screen from Area \a srcArea to \a dstArea using filter \a filter. \a mask allows specification of color (\c GL_COLOR_BUFFER_BIT) and/or depth(\c GL_DEPTH_BUFFER_BIT). Calls glBlitFramebufferEXT() and is subject to its constraints and coordinate system.
	void		blitToScreen( const Area &srcArea, const Area &dstArea, GLenum filter = GL_NEAREST, GLbitfield mask = GL_COLOR_BUFFER_BIT ) const;
	//! Copies from the screen from Area \a srcArea to \a dstArea using filter \a filter. \a mask allows specification of color (\c GL_COLOR_BUFFER_BIT) and/or depth(\c GL_DEPTH_BUFFER_BIT). Calls glBlitFramebufferEXT() and is subject to its constraints and coordinate system.
	void		blitFromScreen( const Area &srcArea, const Area &dstArea, GLenum filter = GL_NEAREST, GLbitfield mask = GL_COLOR_BUFFER_BIT );
#endif

	//! Returns the maximum number of samples the graphics card is capable of using per pixel in MSAA for an Fbo
	static GLint	getMaxSamples();
	//! Returns the maximum number of color attachments the graphics card is capable of using for an Fbo
	static GLint	getMaxAttachments();
	
	//! Returns the debugging label associated with the Fbo.
	const std::string&	getLabel() const { return mLabel; }
	//! Sets the debugging label associated with the Fbo. Calls glObjectLabel() when available.
	void				setLabel( const std::string &label );
	
	//! Returns a copy of the pixels in \a attachment within \a area (cropped to the bounding rectangle of the attachment) as a Surface8u. \a attachment ignored on ES 2.
	Surface8u		readPixels8u( const Area &area, GLenum attachment = GL_COLOR_ATTACHMENT0 ) const;
	//! Returns a copy of the pixels in \a attachment within \a area (cropped to the bounding rectangle of the attachment) as a Surface32f. \a attachment ignored on ES 2.
	Surface32f		readPixels32f( const Area &area, GLenum attachment = GL_COLOR_ATTACHMENT0 ) const;
	
	//! \brief Defines the Format of the Fbo, which is passed in via create().
	//!
	//! The default provides an 8-bit RGBA color texture attachment and a 24-bit depth renderbuffer attachment, multi-sampling and stencil disabled.
	struct CI_API Format {
	  public:
		//! Default constructor, sets the target to \c GL_TEXTURE_2D with an 8-bit color+alpha, a 24-bit depth texture, and no multisampling or mipmapping
		Format();

		//! Enables a color texture at \c GL_COLOR_ATTACHMENT0 with a Texture::Format of \a textureFormat, which defaults to 8-bit RGBA with no mipmapping. Disables a color renderbuffer.
		Format&	colorTexture( const Texture::Format &textureFormat = getDefaultColorTextureFormat( true ) ) { mColorTexture = true; mColorTextureFormat = textureFormat; return *this; }
		//! Disables both a color Texture and a color Buffer
		Format&	disableColor() { mColorTexture = false; return *this; }
		
		//! Enables a depth renderbuffer with an internal format of \a internalFormat, which defaults to \c GL_DEPTH_COMPONENT24. Disables a depth texture.
		Format&	depthBuffer( GLenum internalFormat = getDefaultDepthInternalFormat() ) { mDepthTexture = false; mDepthBuffer = true; mDepthBufferInternalFormat = internalFormat; return *this; }
		//! Enables a depth texture with a format of \a textureFormat, which defaults to \c GL_DEPTH_COMPONENT24. Disables a depth renderbuffer.
		Format&	depthTexture( const Texture::Format &textureFormat = getDefaultDepthTextureFormat()) { mDepthTexture = true; mDepthBuffer = false; mDepthTextureFormat = textureFormat; return *this; }
		//! Disables both a depth Texture and a depth Buffer
		Format&	disableDepth() { mDepthBuffer = false; return *this; }
		
		//! Sets the number of MSAA samples. Defaults to none.
		Format& samples( int samples ) { mSamples = samples; return *this; }
		//! Sets the number of CSAA samples. Defaults to none.
		Format& coverageSamples( int coverageSamples ) { mCoverageSamples = coverageSamples; return *this; }
		//! Enables a stencil buffer. Defaults to false.
		Format& stencilBuffer( bool stencilBuffer = true ) { mStencilBuffer = stencilBuffer; return *this; }

		//! Adds a Renderbuffer attachment \a buffer at \a attachmentPoint (such as \c GL_COLOR_ATTACHMENT0). Replaces any existing attachment at the same attachment point.
		Format&	attachment( GLenum attachmentPoint, const RenderbufferRef &buffer, RenderbufferRef multisampleBuffer = RenderbufferRef() );
		//! Adds a Texture attachment \a texture at \a attachmentPoint (such as \c GL_COLOR_ATTACHMENT0). Replaces any existing attachment at the same attachment point.
		Format&	attachment( GLenum attachmentPoint, const TextureBaseRef &texture, RenderbufferRef multisampleBuffer = RenderbufferRef() );
		
		//! Sets the internal format for the depth buffer. Defaults to \c GL_DEPTH_COMPONENT24. Common options also include \c GL_DEPTH_COMPONENT16 and \c GL_DEPTH_COMPONENT32
		void	setDepthBufferInternalFormat( GLint depthInternalFormat ) { mDepthBufferInternalFormat = depthInternalFormat; }
		//! Sets the number of samples used in MSAA-style antialiasing. Defaults to none, disabling multisampling. Note that not all implementations support multisampling.
		void	setSamples( int samples ) { mSamples = samples; }
		//! Sets the number of coverage samples used in CSAA-style antialiasing. Defaults to none. Note that not all implementations support CSAA, and is currenlty Windows-only Nvidia. Ignored on OpenGL ES.
		void	setCoverageSamples( int coverageSamples ) { mCoverageSamples = coverageSamples; }
		//! Sets the Color Texture::Format for use in the creation of the color texture.
		void	setColorTextureFormat( const Texture::Format &format ) { mColorTextureFormat = format; }
		//! Enables or disables the creation of a depth buffer for the FBO.
		void	enableDepthBuffer( bool depthBuffer = true ) { mDepthBuffer = depthBuffer; }
		//! Enables or disables the creation of a stencil buffer.
		void	enableStencilBuffer( bool stencilBuffer = true ) { mStencilBuffer = stencilBuffer; }
		//! Removes a buffer or texture attached at \a attachmentPoint
		void	removeAttachment( GLenum attachmentPoint );

		//! Returns the GL internal format for the depth buffer. Defaults to \c GL_DEPTH_COMPONENT24.
		GLint	getDepthBufferInternalFormat() const { return mDepthBufferInternalFormat; }
		//! Returns the Texture::Format for the default color texture at GL_COLOR_ATTACHMENT0.
		const Texture::Format&	getColorTextureFormat() const { return mColorTextureFormat; }
		//! Returns the Texture::Format for the depth texture.
		const Texture::Format&	getDepthTextureFormat() const { return mDepthTextureFormat; }
		//! Returns the number of samples used in MSAA-style antialiasing. Defaults to none, disabling multisampling.
		int		getSamples() const { return mSamples; }
		//! Returns the number of coverage samples used in CSAA-style antialiasing. Defaults to none. MSW only.
		int		getCoverageSamples() const { return mCoverageSamples; }
		//! Returns whether the FBO contains a Texture at GL_COLOR_ATTACHMENT0
		bool	hasColorTexture() const { return mColorTexture; }
		//! Returns whether the FBO has a Renderbuffer as a depth attachment.
		bool	hasDepthBuffer() const { return mDepthBuffer; }
		//! Returns whether the FBO has a Renderbuffer as a stencil attachment.
		bool	hasStencilBuffer() const { return mStencilBuffer; }
		
		//! Returns the default color Texture::Format for this platform
		static Texture::Format	getDefaultColorTextureFormat( bool alpha = true );
		//! Returns the default depth Texture::Format for this platform
		static Texture::Format	getDefaultDepthTextureFormat();
		//! Returns the default internalFormat for a color Renderbuffer for this platform
		static GLint			getDefaultColorInternalFormat( bool alpha = true );
		//! Returns the default internalFormat for a depth Renderbuffer for this platform
		static GLint			getDefaultDepthInternalFormat();
		// Returns the +stencil complement of a given internalFormat; ie GL_DEPTH_COMPONENT24 -> GL_DEPTH24_STENCIL8, as well as appropriate pixelDataType for glTexImage2D
		static void				getDepthStencilFormats( GLint depthInternalFormat, GLint *resultInternalFormat, GLenum *resultPixelDataType );

		//! Returns the debugging label associated with the Fbo.
		const std::string&	getLabel() const { return mLabel; }
		//! Sets the debugging label associated with the Fbo. Calls glObjectLabel() when available.
		void				setLabel( const std::string &label ) { mLabel = label; }
		//! Sets the debugging label associated with the Fbo. Calls glObjectLabel() when available.
		Format&				label( const std::string &label ) { setLabel( label ); return *this; }
		
	  protected:
		GLenum			mDepthBufferInternalFormat;
		int				mSamples, mCoverageSamples;
		bool			mColorTexture, mDepthTexture;
		bool			mDepthBuffer;
		bool			mStencilBuffer;
		Texture::Format	mColorTextureFormat, mDepthTextureFormat;
		std::string		mLabel; // debug label

		
		std::map<GLenum,RenderbufferRef>	mAttachmentsBuffer;
		std::map<GLenum,RenderbufferRef>	mAttachmentsMultisampleBuffer;
		std::map<GLenum,TextureBaseRef>		mAttachmentsTexture;

		friend class Fbo;
	};

 protected:
	Fbo( int width, int height, const Format &format );
 
	void		init();
	void		initMultisamplingSettings( bool *useMsaa, bool *useCsaa, Format *format );
	void		prepareAttachments( const Format &format, bool multisampling );
	void		attachAttachments();
	void		initMultisample( const Format &format );
	void		updateMipmaps( GLenum attachment ) const;
	bool		checkStatus( class FboExceptionInvalidSpecification *resultExc );
	void		setDrawBuffers( GLuint fbId, const std::map<GLenum,RenderbufferRef> &attachmentsBuffer, const std::map<GLenum,TextureBaseRef> &attachmentsTexture );
	//! Helper function for readPixels8u() / readPixels32f(), returns read area
	Area		prepareReadPixels( const Area &area, GLenum attachment ) const;

	int					mWidth, mHeight;
	Format				mFormat;
	GLuint				mId;
	GLuint				mMultisampleFramebufferId;
	
	std::map<GLenum,RenderbufferRef>	mAttachmentsBuffer; // map from attachment ID to Renderbuffer
	std::map<GLenum,RenderbufferRef>	mAttachmentsMultisampleBuffer; // map from attachment ID to Renderbuffer	
	std::map<GLenum,TextureBaseRef>		mAttachmentsTexture; // map from attachment ID to Texture

	std::string			mLabel; // debugging label

	mutable bool		mNeedsResolve, mNeedsMipmapUpdate;
	
	friend CI_API std::ostream& operator<<( std::ostream &os, const Fbo &rhs );
};

CI_API std::ostream& operator<<( std::ostream &os, const Fbo &rhs );

//! Helper class for implementing dynamic cube mapping
class CI_API FboCubeMap : public Fbo {
  public:
	struct CI_API Format : private Fbo::Format {
		// Default constructor. Enables a depth RenderBuffer and a color CubeMap
		Format();
		
		//! Sets the TextureCubeMap format for the default CubeMap.
		Format&							textureCubeMapFormat( const TextureCubeMap::Format &format )	{ mTextureCubeMapFormat = format; return *this; }
		//! Returns the TextureCubeMap format for the default CubeMap.
		const TextureCubeMap::Format&	getTextureCubeMapFormat() const { return mTextureCubeMapFormat; }
		
		//! Disables a depth Buffer
		Format&	disableDepth() { mDepthBuffer = false; return *this; }

		//! Sets the debugging label associated with the Fbo. Calls glObjectLabel() when available.
		Format&	label( const std::string &label ) { setLabel( label ); return *this; }
		
	  protected:
		gl::TextureCubeMap::Format	mTextureCubeMapFormat;
		
		friend class FboCubeMap;
	};
  
	static FboCubeMapRef	create( int32_t faceWidth, int32_t faceHeight, const Format &format = Format() );
	
	//! Binds a face of the Fbo as the currently active framebuffer. \a faceTarget expects values in the \c GL_TEXTURE_CUBE_MAP_POSITIVE_X family.
	void 	bindFramebufferFace( GLenum faceTarget, GLint level = 0, GLenum target = GL_FRAMEBUFFER, GLenum attachment = GL_COLOR_ATTACHMENT0 );
	//! Returns the view matrix appropriate for a given face (in the \c GL_TEXTURE_CUBE_MAP_POSITIVE_X family) looking from the position \a eyePos
	mat4	calcViewMatrix( GLenum face, const vec3 &eyePos );

	//! Returns a TextureCubeMapRef attached at \a attachment (default \c GL_COLOR_ATTACHMENT0). Resolves multisampling and renders mipmaps if necessary. Returns NULL if a TextureCubeMap is not bound at \a attachment.
	TextureCubeMapRef	getTextureCubeMap( GLenum attachment = GL_COLOR_ATTACHMENT0 );
	
  protected:
	FboCubeMap( int32_t faceWidth, int32_t faceHeight, const Format &format, const TextureCubeMapRef &textureCubeMap );
  
	TextureCubeMapRef		mTextureCubeMap;
};

class CI_API FboException : public Exception {
  public:
	FboException()	{}
	FboException( const std::string &description ) : Exception( description )	{}
};

class CI_API FboExceptionInvalidSpecification : public FboException {
  public:
	FboExceptionInvalidSpecification()	{}
	FboExceptionInvalidSpecification( const std::string &description ) : FboException( description )	{}
};

} } // namespace cinder::gl
