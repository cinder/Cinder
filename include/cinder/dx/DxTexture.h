// The copyright in this software is being made available under the BSD License, included below. 
// This software may be subject to other third party and contributor rights, including patent rights, 
// and no such rights are granted under this license.
//
// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#pragma once

#include "cinder/Cinder.h"
#include "cinder/dx/dx.h"
#include "cinder/Surface.h"
#include "cinder/Rect.h"
#include "cinder/Stream.h"
#include "cinder/FileSystem.h"

#include <vector>
#include <utility>

#include <d3d11.h>

namespace cinder { namespace dx {

enum CinderDxgiChannel {
	CINDER_DXGI_CHAHNNEL_UNKNOWN,
	CINDER_DXGI_CHAHNNEL_8_UNORM,
	CINDER_DXGI_CHAHNNEL_16_FLOAT,
	CINDER_DXGI_CHAHNNEL_32_FLOAT
};


class Texture;
typedef std::shared_ptr<Texture>	TextureRef;

/** \brief Represents an OpenGL Texture. \ImplShared
\see \ref guide_Images "Images in Cinder" */
class Texture {
  public:
	struct Format;

	/** Constructs a texture of size(\a width, \a height) */
	static TextureRef	create( int width, int height, Format format = Format() ) { return TextureRef( new Texture( width, height, format ) ); }
	/** Constructs a texture of size(\a width, \a height). Pixel data is provided by \a data and is expected to be interleaved and in format \a dataFormat */
	static TextureRef	create( const unsigned char *data, DXGI_FORMAT dataFormat, int width, int height, Format format = Format() ) { return TextureRef( new Texture( data, dataFormat, width, height, format ) ); }
	/** Constructs a texture based on the contents of \a surface */
	static TextureRef	create( const Surface8u &surface, Format format = Format() ) { return TextureRef( new Texture( surface, format ) ); }
	/** Constructs a texture based on the contents of \a surface. */
	static TextureRef	create( const Surface32f &surface, Format format = Format() ) { return TextureRef( new Texture( surface, format ) ); }
	//! Constructs a Texture based on the contents of \a channel.
	static TextureRef	create( const Channel8u &channel, Format format = Format() ) { return TextureRef( new Texture( channel, format ) ); }
	//! Constructs a Texture based on the contents of \a channel.
	static TextureRef	create( const Channel32f &channel, Format format = Format() ) { return TextureRef( new Texture( channel, format ) ); }
	/** \brief Constructs a texture based on \a imageSource.  */
	static TextureRef	create( ImageSourceRef imageSource, Format format = Format() ) { return TextureRef( new Texture( imageSource, format ) ); }

	~Texture();

#if defined( CINDER_WINRT )
	/** \brief Constructs asynchronously a texture based on a image located \a path. The loaded texture is returned in \a texture. A default value of -1 for \a internalFormat chooses an appropriate internal format based on the contents of \a imageSource. 
		If you are creating a texture from an image that is located outside of the WinRT Windows Store App folder, you must use this method.
	**/
	static void loadImageAsync(const fs::path path, dx::Texture &texture, const Format format = Format());
#endif

	//! Constructs a Texture based on an externally initialized OpenGL texture. \a aDoNotDispose specifies whether the Texture is responsible for disposing of the associated OpenGL resource.
	//Texture( GLenum aTarget, GLuint aTextureID, int aWidth, int aHeight, bool aDoNotDispose );

	//! Determines whether the Texture will call glDeleteTextures() to free the associated texture objects on destruction
	void			setDoNotDispose( bool aDoNotDispose = true ) { mDoNotDispose = aDoNotDispose; }
	//! Sets the wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c D3D11_TEXTURE_ADDRESS_WRAP, \c D3D11_TEXTURE_ADDRESS_MIRROR, \c D3D11_TEXTURE_ADDRESS_CLAMP, \c D3D11_TEXTURE_ADDRESS_BORDER and \c D3D11_TEXTURE_ADDRESS_MIRROR_ONCE.
	void			setWrap( D3D11_TEXTURE_ADDRESS_MODE wrapS, D3D11_TEXTURE_ADDRESS_MODE wrapT ) { setWrapS( wrapS ); setWrapT( wrapT ); }
	/** \brief Sets the horizontal wrapping behavior when a texture coordinate falls outside the range of [0,1].
		Possible values are \c D3D11_TEXTURE_ADDRESS_WRAP, \c D3D11_TEXTURE_ADDRESS_MIRROR, \c D3D11_TEXTURE_ADDRESS_CLAMP, \c D3D11_TEXTURE_ADDRESS_BORDER and \c D3D11_TEXTURE_ADDRESS_MIRROR_ONCE. **/
	void			setWrapS( D3D11_TEXTURE_ADDRESS_MODE wrapS );
	/** \brief Sets the vertical wrapping behavior when a texture coordinate falls outside the range of [0,1].
		Possible values are \c D3D11_TEXTURE_ADDRESS_WRAP, \c D3D11_TEXTURE_ADDRESS_MIRROR, \c D3D11_TEXTURE_ADDRESS_CLAMP, \c D3D11_TEXTURE_ADDRESS_BORDER and \c D3D11_TEXTURE_ADDRESS_MIRROR_ONCE. **/
	void			setWrapT( D3D11_TEXTURE_ADDRESS_MODE wrapT );
	/** \brief Sets the filtering behavior when a texture is displayed at a different resolution than its native resolution.
	 * Way too many to list. Look it up on the MSDN. **/
	void			setFilter( D3D11_FILTER filter );

	/** Designed to accommodate texture where not all pixels are "clean", meaning the maximum texture coordinate value may not be 1.0 (or the texture's width in \c GL_TEXTURE_RECTANGLE_ARB) **/
	void			setCleanTexCoords( float maxU, float maxV );

	//! Replaces the pixels of a texture with contents of \a surface. Expects \a surface's size to match the Texture's.
	void			update( const Surface &surface );
	//! Replaces the pixels of a texture with contents of \a surface. Expects \a surface's size to match the Texture's.
	void			update( const Surface32f &surface );
	/** \brief Replaces the pixels of a texture with contents of \a surface. Expects \a area's size to match the Texture's.
		\todo Method for updating a subrectangle with an offset into the source **/
	void			update( const Surface &surface, const Area &area );
	//! Replaces the pixels of a texture with contents of \a channel. Expects \a channel's size to match the Texture's.
	void			update( const Channel &channel );
	//! Replaces the pixels of a texture with contents of \a channel. Expects \a channel's size to match the Texture's.
	void			update( const Channel32f &channel );
	//! Replaces the pixels of a texture with contents of \a channel. Expects \a area's size to match the Texture's.
	void			update( const Channel8u &channel, const Area &area );
	
	//! the width of the texture in pixels
	UINT			getWidth() const;
	//! the height of the texture in pixels
	UINT			getHeight() const;
	//! the width of the texture in pixels accounting for its "clean" area - \sa getCleanBounds()
	UINT			getCleanWidth() const;
	//! the height of the texture in pixels accounting for its "clean" area - \sa getCleanBounds()
	UINT			getCleanHeight() const;
	//! the size of the texture in pixels
	Vec2i			getSize() const { return Vec2i( getWidth(), getHeight() ); }	
	//! the aspect ratio of the texture (width / height)
	float			getAspectRatio() const { return getWidth() / (float)getHeight(); }
	//! the Area defining the Texture's bounds in pixels: [0,0]-[width,height]
	Area			getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }
	//! the Area defining the Texture's clean pixel bounds in pixels: [0,0]-[width*maxU,height*maxV]
	Area			getCleanBounds() const { return Area( 0, 0, getCleanWidth(), getCleanHeight() ); }
	//! whether the texture has an alpha channel
	bool			hasAlpha() const;
	//!	These return the right thing even when the texture coordinate space is flipped
	float			getLeft() const;
	float			getRight() const;	
	float			getTop() const;		
	float			getBottom() const;	
	//!	These do not correspond to "top" and "right" when the texture is flipped
	float			getMaxU() const;
	float			getMaxV() const;
	//! Returns the UV coordinates which correspond to the pixels contained in \a area. Does not compensate for clean coordinates. Does compensate for flipping.
	Rectf			getAreaTexCoords( const Area &area ) const;
	//! the Texture's internal format, which is the format that OpenGL stores the texture data in memory. Common values include \c GL_RGB, \c GL_RGBA and \c GL_LUMINANCE
	DXGI_FORMAT		getInternalFormat() const;
	//! the target associated with texture. Typical values are \c GL_TEXTURE_2D and \c GL_TEXTURE_RECTANGLE_ARB
	//GLenum			getTarget() const { return mTarget; }
	//!	whether the texture is flipped vertically
	bool			isFlipped() const { return mFlipped; }
	//!	Marks the texture as being flipped vertically or not
	void			setFlipped( bool aFlipped = true ) { mFlipped = aFlipped; }

	//!	Binds the Texture's texture to its target in the multitexturing unit \c GL_TEXTURE0 + \a textureUnit
	void 			bind( UINT textureUnit = 0 ) const;
	//!	Unbinds the Texture currently bound in the Texture's target
	void			unbind( UINT textureUnit = 0 ) const;

	//!	Creates a new Texture from raw DirectDraw Stream data
	static TextureRef	loadDds( IStreamRef ddsStream, Format format );

	//! Converts a SurfaceChannelOrder into an appropriate DXGI dataFormat and type
	//static void		SurfaceChannelOrderToDataFormatAndType( const SurfaceChannelOrder &sco, GLint *dataFormat, GLenum *type );
	static void		SurfaceChannelOrderToDataFormatAndType( const SurfaceChannelOrder &sco, DXGI_FORMAT *dataFormat, CinderDxgiChannel* type, bool isSurface32f = false );

	//! Returns whether a given DXGI dataFormat contains an alpha channel
	//static bool		dataFormatHasAlpha( GLint dataFormat );
	static bool		dataFormatHasAlpha( DXGI_FORMAT dataFormat );
	//! Returns whether a give OpenGL dataFormat contains color channels
	//static bool		dataFormatHasColor( GLint dataFormat );
	static bool		dataFormatHasColor( DXGI_FORMAT dataFormat );
	//! Returns the number of channels for the given DXGI dataFormat
	static uint32_t	dataFormatNumChannels( DXGI_FORMAT dataFormat );

	ID3D11Texture2D*			getDxTexture() { return mDxTexture; }
	ID3D11ShaderResourceView*	getDxShaderResourceView() { return mSRV; }

#if ! defined( CINDER_GLES )
	//! Returns an ImageSource pointing to this Texture
	operator	ImageSourceRef() const;
#endif
	
	struct Format {
		//! Default constructor, sets the target to \c GL_TEXTURE_2D, wrap to \c GL_CLAMP, disables mipmapping, the internal format to "automatic"
		Format();
		
		//! Specifies the texture's target. The default is \c GL_TEXTURE_2D
		//void	setTarget( GLenum target ) { mTarget = target; }
		//! Sets the texture's target to be \c GL_TEXTURE_RECTANGLE_ARB. Not available in OpenGL ES.
#ifndef CINDER_GLES
		//void	setTargetRect() { mTarget = GL_TEXTURE_RECTANGLE_ARB; }
#endif		

		//! Enables or disables mipmapping. Default is disabled.
		void	enableMipmapping( bool enableMipmapping = true ) { mMipmapping = enableMipmapping; }

		//! Sets the Texture's internal format. A value of -1 implies selecting the best format for the context. 
		void	setInternalFormat( DXGI_FORMAT internalFormat ) { mInternalFormat = internalFormat; }
		//! Sets the Texture's internal format to be automatically selected based on the context.
		void	setAutoInternalFormat() { mInternalFormat = (DXGI_FORMAT)-1; }		
		
		//! Sets the wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_CLAMP, \c GL_REPEAT and \c GL_CLAMP_TO_EDGE. The default is \c GL_CLAMP
		void	setWrap( D3D11_TEXTURE_ADDRESS_MODE wrapS, D3D11_TEXTURE_ADDRESS_MODE wrapT ) { setWrapS( wrapS ); setWrapT( wrapT ); }
		/** \brief Sets the horizontal wrapping behavior when a texture coordinate falls outside the range of [0,1].
			Possible values are \c GL_CLAMP, \c GL_REPEAT and \c GL_CLAMP_TO_EDGE. The default is \c GL_CLAMP_TO_EDGE **/
		void	setWrapS( D3D11_TEXTURE_ADDRESS_MODE wrapS ) { mWrapS = wrapS; }
		/** \brief Sets the vertical wrapping behavior when a texture coordinate falls outside the range of [0,1].
			Possible values are \c GL_CLAMP, \c GL_REPEAT and \c GL_CLAMP_TO_EDGE. The default is \c GL_CLAMP_TO_EDGE. **/
		void	setWrapT( D3D11_TEXTURE_ADDRESS_MODE wrapT ) { mWrapT = wrapT; }
		/** \brief Sets the filtering behavior when a texture is displayed at a lower resolution than its native resolution. Default is \c GL_LINEAR
		 * Possible values are \li \c GL_NEAREST \li \c GL_LINEAR \li \c GL_NEAREST_MIPMAP_NEAREST \li \c GL_LINEAR_MIPMAP_NEAREST \li \c GL_NEAREST_MIPMAP_LINEAR \li \c GL_LINEAR_MIPMAP_LINEAR **/
		void	setFilter( D3D11_FILTER filter ) { mFilter = filter; }
				
		//! Returns the texture's target
		//GLenum	getTarget() const { return mTarget; }
		//! Returns whether the texture has mipmapping enabled
		bool	hasMipmapping() const { return mMipmapping; }

		//! Returns the Texture's internal format. A value of -1 implies automatic selection of the internal format based on the context.
		DXGI_FORMAT	getInternalFormat() const { return mInternalFormat; }
		//! Returns whether the Texture's internal format will be automatically selected based on the context.
		bool	isAutoInternalFormat() const { return mInternalFormat == -1; }
		
		//! Returns the horizontal wrapping behavior for the texture coordinates.
		D3D11_TEXTURE_ADDRESS_MODE	getWrapS() const { return mWrapS; }
		//! Returns the vertical wrapping behavior for the texture coordinates.
		D3D11_TEXTURE_ADDRESS_MODE	getWrapT() const { return mWrapT; }
		//! Returns the texture minifying function, which is used whenever the pixel being textured maps to an area greater than one texture element.
		D3D11_FILTER				getFilter() const { return mFilter; }
		
		//! Returns true if render target flag is set
		bool	isRenderTarget() const { return mRenderTarget; }
		//! Enables or disables render target. Default is disabled.
		void	enableRenderTarget( bool bEnableRenderTarget = true ) { mRenderTarget = bEnableRenderTarget; }

	  protected:
		//GLenum						mTarget;
		D3D11_TEXTURE_ADDRESS_MODE	mWrapS, mWrapT;
		D3D11_FILTER				mFilter;
		bool						mMipmapping;
		DXGI_FORMAT					mInternalFormat;
		bool						mRenderTarget;
		
		friend class Texture;
	};

 protected:
	Texture();
	/** \brief Constructs a texture of size(\a aWidth, \a aHeight), storing the data in internal format \a aInternalFormat. **/
	Texture( int width, int height, Format format = Format() );
	/** \brief Constructs a texture of size(\a aWidth, \a aHeight), storing the data in internal format \a aInternalFormat. Pixel data is provided by \a data and is expected to be interleaved and in format \a dataFormat, for which \c GL_RGB or \c GL_RGBA would be typical values. **/
	Texture( const unsigned char *data, DXGI_FORMAT dataFormat, int aWidth, int aHeight, Format format = Format() );
	/** \brief Constructs a texture based on the contents of \a surface. A default value of -1 for \a internalFormat chooses an appropriate internal format automatically. **/
	Texture( const Surface8u &surface, Format format = Format() );
	/** \brief Constructs a texture based on the contents of \a surface. A default value of -1 for \a internalFormat chooses an appropriate internal format automatically. **/
	Texture( const Surface32f &surface, Format format = Format() );
	/** \brief Constructs a texture based on the contents of \a channel. A default value of -1 for \a internalFormat chooses an appropriate internal format automatically. **/
	Texture( const Channel8u &channel, Format format = Format() );
	/** \brief Constructs a texture based on the contents of \a channel. A default value of -1 for \a internalFormat chooses an appropriate internal format automatically. **/
	Texture( const Channel32f &channel, Format format = Format() );
	/** \brief Constructs a texture based on \a imageSource. A default value of -1 for \a internalFormat chooses an appropriate internal format based on the contents of \a imageSource. **/
	Texture( ImageSourceRef imageSource, Format format = Format() );

	void	init( int width, int height );
	void	init( const unsigned char *srcData, DXGI_FORMAT srcDataFormat, const Format &format );	
	void	init( const float *srcData, DXGI_FORMAT srcDataFormat, const Format &format );
	void	init( ImageSourceRef imageSource, const Format &format );	
		 	
/*		Obj() : mWidth( -1 ), mHeight( -1 ), mCleanWidth( -1 ), mCleanHeight( -1 ), mInternalFormat( (DXGI_FORMAT)-1 ), mFlipped( false ), mDeallocatorFunc( 0 ), mDxTexture(NULL), mSamplerState(NULL), mSRV(NULL)
		{}
		Obj( int aWidth, int aHeight ) : mInternalFormat( (DXGI_FORMAT)-1 ), mWidth( aWidth ), mHeight( aHeight ), mCleanWidth( aWidth ), mCleanHeight( aHeight ), mFlipped( false ), mDeallocatorFunc( 0 ), mDxTexture(NULL), mSamplerState(NULL), mSRV(NULL)
		{}
		~Obj();*/

	mutable UINT				mWidth, mHeight;
	mutable UINT				mCleanWidth, mCleanHeight;
	float						mMaxU, mMaxV;
	DXGI_FORMAT					mInternalFormat;

	bool						mDoNotDispose;
	bool						mFlipped;	

	ID3D11Texture2D				*mDxTexture;
	D3D11_SAMPLER_DESC			mSamplerDesc;
	ID3D11SamplerState			*mSamplerState;
	ID3D11ShaderResourceView	*mSRV;
};

/*
class TextureCache {
 public:
	TextureCache() {}
	TextureCache( const Surface8u &prototypeSurface, const Texture::Format &format );
	
	dx::Texture		cache( const Surface8u &data );
	
  protected:
 	struct Obj {
		Obj( const Surface8u &prototypeSurface, const Texture::Format &format );

		void		markTextureAsFree( int id );

		int				mWidth, mHeight;
		Texture::Format	mFormat;
		
		int											mNextId;
		std::vector<std::pair<int,dx::Texture> >	mTextures;
		
		static void TextureCacheDeallocator( void *aDeallocatorRefcon );
	};
 
	std::shared_ptr<Obj>		mObj;

  public:
 	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> TextureCache::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &TextureCache::mObj; }
	void reset() { mObj.reset(); }
	//@}	
};
*/

class SurfaceConstraintsGLTexture : public SurfaceConstraints {
 public:
	virtual SurfaceChannelOrder getChannelOrder( bool alpha ) const { return ( alpha ) ? SurfaceChannelOrder::BGRA : SurfaceChannelOrder::BGR; }
	virtual int32_t				getRowBytes( int requestedWidth, const SurfaceChannelOrder &sco, int elementSize ) const { return requestedWidth * elementSize * sco.getPixelInc(); }
};

class TextureDataExc : public std::exception {
public:	
	TextureDataExc( const std::string &log ) throw();
	virtual const char* what() const throw()
	{
		return mMessage;
	}

private:
	char	mMessage[16001];
	GLint	mShaderType;
};


} } // namespace cinder::gl
