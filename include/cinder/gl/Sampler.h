/*
 Copyright (c) 2017, The Cinder Project, All rights reserved.

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
#include "cinder/Color.h"
#include <array>

#if defined( CINDER_GL_HAS_SAMPLERS )

namespace cinder { namespace gl {

class Sampler;
typedef std::shared_ptr<Sampler>	SamplerRef;

//! A Sampler represents an OpenGL object that stores the sampling parameters for Texture access from within a shader
class CI_API Sampler {
  public:
	
	struct CI_API Format {
		Format();
		Format( const Format &rhs ) { copy( rhs ); }
		Format& operator=(const Format &rhs ) { copy( rhs ); return *this; }
	
		//! Sets the S and T wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
		Format&	wrap( GLenum wrapS, GLenum wrapT ) { setWrapS( wrapS ); setWrapT( wrapT ); return *this; }
		//! Sets the wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
		void	setWrap( GLenum wrapS, GLenum wrapT ) { setWrapS( wrapS ); setWrapT( wrapT ); }
        //! Sets the wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
        void	setWrap( GLenum wrapS, GLenum wrapT, GLenum wrapR ) { setWrapS( wrapS ); setWrapT( wrapT ); setWrapR( wrapR ); }
		//! Sets the S, T, and R wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
		Format&	wrap( GLenum wrapS, GLenum wrapT, GLenum wrapR ) { setWrapS( wrapS ); setWrapT( wrapT ); setWrapR( wrapR ); return *this; }
		//! Sets the horizontal wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
		void	setWrapS( GLenum wrapS ) { mWrapS = wrapS; }
		//! Sets the vertical wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
		void	setWrapT( GLenum wrapT ) { mWrapT = wrapT; }
		//! Sets the depth wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
		void	setWrapR( GLenum wrapR ) { mWrapR = wrapR; }
		//! Returns the horizontal wrapping behavior for the texture coordinates.
		GLenum	getWrapS() const { return mWrapS; }
		//! Returns the vertical wrapping behavior for the texture coordinates.
		GLenum	getWrapT() const { return mWrapT; }
		//! Returns the depth wrapping behavior for the texture coordinates.
		GLenum	getWrapR() const { return mWrapR; }

		//! Sets the filtering behavior when a texture is displayed at a lower resolution than its native resolution. Default is \c GL_LINEAR.
		Format& minFilter( GLenum minFilter ) { setMinFilter( minFilter ); return *this; }
		//! Sets the filtering behavior when a texture is displayed at a higher resolution than its native resolution. Default is \c GL_LINEAR.
		Format& magFilter( GLenum magFilter ) { setMagFilter( magFilter ); return *this; }
		//! Sets the filtering behavior when a texture is displayed at a lower resolution than its native resolution. Default is \c GL_LINEAR
		void	setMinFilter( GLenum minFilter ) { mMinFilter = minFilter; }
		//! Sets the filtering behavior when a texture is displayed at a higher resolution than its native resolution. Default is \c GL_LINEAR.
		void	setMagFilter( GLenum magFilter ) { mMagFilter = magFilter; }
		//! Returns the texture minifying function, which is used whenever the pixel being textured maps to an area greater than one texture element.
		GLenum	getMinFilter() const { return mMinFilter; }
		//! Returns the texture magnifying function, which is used whenever the pixel being textured maps to an area less than or equal to one texture element.
		GLenum	getMagFilter() const { return mMagFilter; }

		// Specifies the texture comparison mode for currently bound depth textures. Default is \c GL_NONE.
		void	setCompareMode( GLenum compareMode ) { mCompareMode = compareMode; }
		// Specifies the comparison operator used when \c GL_TEXTURE_COMPARE_MODE is set to \c GL_COMPARE_REF_TO_TEXTURE. Default is GL_LEQUAL.
		void	setCompareFunc( GLenum compareFunc ) { mCompareFunc = compareFunc; }
		//! Returns the texture comparison mode for currently bound depth texture.
		GLenum	getCompareMode() const { return mCompareMode; }
		//! Returns the comparison operator used when \c GL_TEXTURE_COMPARE_MODE is set to \c GL_COMPARE_REF_TO_TEXTURE.
		GLenum	getCompareFunc() const { return mCompareFunc; }

#if ! defined( CINDER_GL_ES )
		//! Sets the texture's border color.
		void			setBorderColor( const ColorA &color ) { mBorderColor = color; }
		//! Returns the texture's border color
		const ColorA&	getBorderColor() const { return mBorderColor; }
		
		//! Limits the selection of highest resolution mipmap (lowest mipmap level). Default is \c -1000
		void			setMinLod( float minLod ) { mMinLod = minLod; }
		//! Returns the highest resolution mipmap (lowest mipmap level) sampling will select. Default is \c -1000
		float			getMinLod() const { return mMinLod; }
		//! Limits the selection of lowest resolution mipmap (highest mipmap level). Default is \c 1000
		void			setMaxLod( float maxLod ) { mMaxLod = maxLod; }
		//! Returns the highest resolution mipmap (lowest mipmap level) sampling will select. Default is \c 1000
		float			getMaxLod() const { return mMaxLod; }
#endif

		//! Sets the anisotropic filter amount. A value greater than 1.0 "enables" anisotropic filtering. Maximum of Texture::getMaxAnisotropyMax();
		void    setMaxAnisotropy( float maxAnisotropy ) { mMaxAnisotropy = maxAnisotropy; }
		//! Gets the anisotropic filter amount.
		float	getMaxAnisotropy() const { return mMaxAnisotropy; } 
		Format&	maxAnisotropy( float maxAnisotropy ) { mMaxAnisotropy = maxAnisotropy; return *this; } 
		
		//! Returns the debugging label associated with the Texture.
		const std::string&	getLabel() const { return mLabel; }
		//! Sets the debugging label associated with the Texture. Calls glObjectLabel() when available.
		void				setLabel( const std::string &label ) { mLabel = label; }
		//! Sets the debugging label associated with the Texture. Calls glObjectLabel() when available.
		Format&				label( const std::string &label ) { setLabel( label ); return *this; }
		
	protected:	
		void	copy( const Format &rhs );

		GLenum					mWrapS, mWrapT, mWrapR;
		GLenum					mMinFilter, mMagFilter;
		GLint					mCompareMode, mCompareFunc;		
#if ! defined( CINDER_GL_ES )
		ColorA					mBorderColor;
		float					mMinLod, mMaxLod;
#endif
		float					mMaxAnisotropy;
		std::string				mLabel; // debug label

		friend class Sampler;
	};

	//! Default-constructs an uninitialized Sampler
	Sampler();
	Sampler( const Format &format );
	Sampler( Sampler &&rhs );
	Sampler( const Sampler &rhs ) = delete;
	~Sampler();
	
	Sampler& operator=( Sampler &&rhs );
	Sampler& operator=( const Sampler &rhs ) = delete;
	
	static SamplerRef		create() { return std::make_shared<Sampler>(); }
	static SamplerRef		create( const Format &format ) { return std::make_shared<Sampler>( format ); }
	
	
	const Format&	getFormat() const { return mFormat; }	

	//! Sets the wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
	void	setWrap( GLenum wrapS, GLenum wrapT );
	//! Sets the wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
	void	setWrap( GLenum wrapS, GLenum wrapT, GLenum wrapR );
	//! Sets the horizontal wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
	void	setWrapS( GLenum wrapS );
	//! Sets the vertical wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
	void	setWrapT( GLenum wrapT );
	//! Sets the depth wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
	void	setWrapR( GLenum wrapR );
	//! Returns the horizontal wrapping behavior for the texture coordinates.
	GLenum	getWrapS() const { return mFormat.mWrapS; }
	//! Returns the vertical wrapping behavior for the texture coordinates.
	GLenum	getWrapT() const { return mFormat.mWrapT; }
	//! Returns the depth wrapping behavior for the texture coordinates.
	GLenum	getWrapR() const { return mFormat.mWrapR; }

	//! Sets the filtering behavior when a texture is displayed at a lower resolution than its native resolution. Default is \c GL_LINEAR_MIPMAP_LINEAR
	void	setMinFilter( GLenum minFilter );
	//! Sets the filtering behavior when a texture is displayed at a higher resolution than its native resolution. Default is \c GL_LINEAR.
	void	setMagFilter( GLenum magFilter );
	//! Returns the texture minifying function, which is used whenever the pixel being textured maps to an area greater than one texture element.
	GLenum	getMinFilter() const { return mFormat.mMinFilter; }
	//! Returns the texture magnifying function, which is used whenever the pixel being textured maps to an area less than or equal to one texture element.
	GLenum	getMagFilter() const { return mFormat.mMagFilter; }

	// Specifies the texture comparison mode for currently bound depth textures. Default is \c GL_NONE.
	void	setCompareMode( GLenum compareMode );
	// Specifies the comparison operator used when \c GL_TEXTURE_COMPARE_MODE is set to \c GL_COMPARE_REF_TO_TEXTURE. Default is GL_LEQUAL.
	void	setCompareFunc( GLenum compareFunc );
	//! Returns the texture comparison mode for currently bound depth texture.
	GLenum	getCompareMode() const { return mFormat.mCompareMode; }
	//! Returns the comparison operator used when \c GL_TEXTURE_COMPARE_MODE is set to \c GL_COMPARE_REF_TO_TEXTURE.
	GLenum	getCompareFunc() const { return mFormat.mCompareFunc; }

#if ! defined( CINDER_GL_ES )
	//! Sets the texture's border color.
	void			setBorderColor( const ColorA &color );
	//! Returns the texture's border color
	const ColorA&	getBorderColor() const { return mFormat.mBorderColor; }
	
	//! Limits the selection of highest resolution mipmap (lowest mipmap level). Default is \c -1000
	void			setMinLod( float minLod );
	//! Returns the highest resolution mipmap (lowest mipmap level) sampling will select. Default is \c -1000
	float			getMinLod() const { return mFormat.mMinLod; }
	//! Limits the selection of lowest resolution mipmap (highest mipmap level). Default is \c 1000
	void			setMaxLod( float maxLod );
	//! Returns the highest resolution mipmap (lowest mipmap level) sampling will select. Default is \c 1000
	float			getMaxLod() const { return mFormat.mMaxLod; }
#endif
	
	//! Sets the anisotropic filter amount. A value greater than 1.0 "enables" anisotropic filtering. Maximum of Texture::getMaxAnisotropyMax();
	void    setMaxAnisotropy( float maxAnisotropy );
	//! Gets the anisotropic filter amount.
	float	getMaxAnisotropy() const { return mFormat.mMaxAnisotropy; } 
	
	//! Returns the debugging label associated with the Texture.
	const std::string&	getLabel() const { return mFormat.mLabel; }
	//! Sets the debugging label associated with the Texture. Calls glObjectLabel() when available.
	void				setLabel( const std::string &label );
	
	//! Returns the Sampler object ID
	GLuint			getId() const { return mId; }
	
	//!	Binds the Sampler in the multitexturing unit \c GL_TEXTURE0 + \a textureUnit
	void 			bind( uint8_t textureUnit = 0 ) const;
	//!	Unbinds the Sampler currently bound in the multitexturing unit \c GL_TEXTURE0 + \a textureUnit
	void 			unbind( uint8_t textureUnit = 0 ) const;
	
  protected:
	void	initParams();

	Format		mFormat;
	GLuint		mId;
};

} } // namespace cinder::gl

#endif // defined( CINDER_GL_HAS_SAMPLERS )
