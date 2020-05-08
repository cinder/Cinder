/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.
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
#include "cinder/Surface.h"
#include "cinder/Rect.h"
#include "cinder/Stream.h"
#include "cinder/Exception.h"
#include "cinder/DataSource.h"
#include "cinder/ImageIo.h"

#include <vector>
#include <utility>
#include <array>

#if defined( CINDER_GL_ES )
	#define GL_BLUE		0x1905
	#define GL_GREEN	0x1904
  #if ! defined( GL_RED )
	#define GL_RED		0x1903
  #endif
#endif

namespace cinder { namespace gl {

typedef class Texture2d							Texture;
typedef std::shared_ptr<class TextureBase>		TextureBaseRef;
typedef std::shared_ptr<class Texture2d>		Texture2dRef;
typedef Texture2dRef							TextureRef;
typedef std::shared_ptr<Texture2d>				Texture2dRef;
#if ! defined( CINDER_GL_ES )
	typedef std::shared_ptr<class Texture1d>		Texture1dRef;
#endif
#if ! defined( CINDER_GL_ES_2 )
	typedef std::shared_ptr<class Texture3d>		Texture3dRef;
#endif
typedef std::shared_ptr<class TextureCubeMap>	TextureCubeMapRef;

// Forward-declared from cinder/gl/Pbo.h
class Pbo;
typedef std::shared_ptr<Pbo>					PboRef;

class CI_API TextureBase {
  public:
	virtual ~TextureBase();

	//! Determines whether the Texture will call glDeleteTextures() to free the associated texture objects on destruction
	void			setDoNotDispose( bool aDoNotDispose = true ) { mDoNotDispose = aDoNotDispose; }
	//! the Texture's internal format, which is the format that OpenGL stores the texture data in memory. Common values include \c GL_RGB, \c GL_RGBA and \c GL_LUMINANCE
	GLint			getInternalFormat() const;
	//! the ID number for the texture, appropriate to pass to calls like \c glBindTexture()
	GLuint			getId() const { return mTextureId; }
	//! the target associated with texture. Typical values are \c GL_TEXTURE_2D and \c GL_TEXTURE_RECTANGLE_ARB
	GLenum			getTarget() const { return mTarget; }
	//!	Binds the Texture's texture to its target in the multitexturing unit \c GL_TEXTURE0 + \a textureUnit
	void 			bind( uint8_t textureUnit = 0 ) const;
	//!	Unbinds the Texture currently bound in the Texture's target
	void			unbind( uint8_t textureUnit = 0 ) const;

	//! Returns the width of the texture in pixels, ignoring clean bounds.
	virtual GLint	getWidth() const = 0;
	//! Returns the height of the texture in pixels, ignoring clean bounds.
	virtual GLint	getHeight() const = 0;
	//! Returns the depth of the texture in pixels, ignoring clean bounds.
	virtual GLint	getDepth() const = 0;
	//! Returns the 2D aspect ratio of the texture (width / height), ignoring clean bounds.
	float			getAspectRatio() const { return getWidth() / (float)getHeight(); }
	//! Returns the Area defining the Texture's bounds in pixels, ignoring clean bounds.
	Area			getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }

	//! Returns whether the Texture has an alpha channel based on its internal format
	bool			hasAlpha() const;

	//! Sets the wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
	void			setWrap( GLenum wrapS, GLenum wrapT ) { setWrapS( wrapS ); setWrapT( wrapT ); }
#if ! defined( CINDER_GL_ES )
    //! Sets the wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
    void			setWrap( GLenum wrapS, GLenum wrapT, GLenum wrapR ) { setWrapS( wrapS ); setWrapT( wrapT ); setWrapR( wrapR ); }
#endif
	//! Sets the horizontal wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT and \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
	void			setWrapS( GLenum wrapS );
	//! Sets the vertical wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
	void			setWrapT( GLenum wrapT );
#if ! defined( CINDER_GL_ES )
	//! Sets the depth wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
	void			setWrapR( GLenum wrapR );
#endif	
	/** \brief Sets the filtering behavior when a texture is displayed at a lower resolution than its native resolution.
	 * Possible values are \li \c GL_NEAREST \li \c GL_LINEAR \li \c GL_NEAREST_MIPMAP_NEAREST \li \c GL_LINEAR_MIPMAP_NEAREST \li \c GL_NEAREST_MIPMAP_LINEAR \li \c GL_LINEAR_MIPMAP_LINEAR **/
	void			setMinFilter( GLenum minFilter );
	/** Sets the filtering behavior when a texture is displayed at a higher resolution than its native resolution.
	 * Possible values are \li \c GL_NEAREST \li \c GL_LINEAR **/
	void			setMagFilter( GLenum magFilter );
	//! Sets the anisotropic filtering amount. A value greater than 1.0 "enables" anisotropic filtering. Maximum of getMaxAnisotropyMax();
	void			setMaxAnisotropy( GLfloat maxAnisotropy );
	//! Returns whether the Texture has Mipmapping enabled
	bool			hasMipmapping() const { return mMipmapping; }
	// Specifies the texture comparison mode for currently bound depth textures.
	void			setCompareMode( GLenum compareMode );
	// Specifies the comparison operator used when \c GL_TEXTURE_COMPARE_MODE is set to \c GL_COMPARE_R_TO_TEXTURE
	void			setCompareFunc( GLenum compareFunc );

	//! Returns the appropriate parameter to glGetIntegerv() for a specific target; ie GL_TEXTURE_2D -> GL_TEXTURE_BINDING_2D. Returns 0 on failure.
	static GLenum	getBindingConstantForTarget( GLenum target );
	//! Returns the corresponding legal values for glTexImage*D() calls for dataFormat and dataType based on \a internalFormat, as well as whether the internal format contains an alpha channel, is compressed, and is in the sRGB color space.
	static void		getInternalFormatInfo( GLint internalFormat, GLenum *outDataFormat, GLenum *outDataType, bool *outAlpha = nullptr, bool *outCompressed = nullptr, bool *outSrgb = nullptr );
	//! Returns the number of mip levels necessary to represent a texture of \a width, \a height and \a depth
	static int		requiredMipLevels( GLint width, GLint height, GLint depth );

	//! Returns whether a Surface of \a width, \a rowBytes and \a surfaceChannelOrder would require an intermediate Surface in order to be copied into a GL Texture.
	template<typename T>
	static bool		surfaceRequiresIntermediate( int32_t width, uint8_t pixelBytes, ptrdiff_t rowBytes, SurfaceChannelOrder surfaceChannelOrder );
	//! Converts a SurfaceChannelOrder into an appropriate OpenGL dataFormat and type
	template<typename T>
	static void		SurfaceChannelOrderToDataFormatAndType( const SurfaceChannelOrder &sco, GLint *dataFormat, GLenum *type );
	//! calculate the size of mipMap for the corresponding level
	static ivec2	calcMipLevelSize( int level, GLint width, GLint height );
	//! Returns the maximum anisotropic filtering maximum allowed by the hardware
	static GLfloat	getMaxAnisotropyMax();

	//! Returns the Texture's swizzle mask (corresponding to \c GL_TEXTURE_SWIZZLE_RGBA)	
	std::array<GLint,4>	getSwizzleMask() const { return mSwizzleMask; }
	//! Returns whether this hardware supports texture swizzling (via \c GL_TEXTURE_SWIZZLE_RGBA)
	static bool		supportsHardwareSwizzle();
#if defined( CINDER_GL_ES_2 )
	//! Returns whether this hardware supports shadow sampling.
	static bool		supportsShadowSampler();
#endif
	//! Returns the debugging label associated with the Texture.
	const std::string&	getLabel() const { return mLabel; }
	//! Sets the debugging label associated with the Texture. Calls glObjectLabel() when available.
	void				setLabel( const std::string &label );

	struct CI_API Format {
		//! Specifies the texture's target. The default is \c GL_TEXTURE_2D
		void	setTarget( GLenum target ) { mTarget = target; }
		//! Sets the texture's target to be \c GL_TEXTURE_RECTANGLE. Not available in OpenGL ES.
#if ! defined( CINDER_GL_ES )
		void	setTargetRect() { mTarget = GL_TEXTURE_RECTANGLE; }
#endif
		
		//! Enables or disables mipmapping. Default is disabled.
		void	enableMipmapping( bool enableMipmapping = true ) { mMipmapping = enableMipmapping; mMipmappingSpecified = true; }
		//! Specifies the index of the lowest defined mipmap level. Default is \c 0. Ignored on ES 2.
		void	setBaseMipmapLevel( GLuint level ) { mBaseMipmapLevel = level; }
		//! Sets the max mipmap level. Default (expressed as \c -1) is derived from the size of the texture. Ignored on ES 2.
		void	setMaxMipmapLevel( GLint level ) { mMaxMipmapLevel = level; }
		//! Returns the index of the lowest defined mipmap level.
		GLuint	getBaseMipmapLevel() const { return mBaseMipmapLevel; }
		//! Returns the max mipmap level.
		GLuint	getMaxMipmapLevel() const { return mMaxMipmapLevel; }
		
		//! Sets whether the storage for the cannot be changed in the future (making glTexImage*D() calls illegal). More efficient when possible. Default is \c false.
		void	setImmutableStorage( bool immutable = true ) { mImmutableStorage = immutable; }
		//! Returns whether the storage for the cannot be changed in the future (making glTexImage*D() calls illegal). Default is \c false.
		bool	isImmutableStorage() const { return mImmutableStorage; }
		
		//! Sets the Texture's internal format. A value of -1 implies selecting the best format for the context.
		void	setInternalFormat( GLint internalFormat ) { mInternalFormat = internalFormat; }
		//! Sets the Texture's internal format to be automatically selected based on the context.
		void	setAutoInternalFormat() { mInternalFormat = -1; }
		
		//! Sets the data type parameter used by glTexImage2D when glTexStorage2D is unavailable. Defaults to \c -1 which implies automatic determination. Primary use is to pass \c GL_FLOAT or \c GL_HALF_FLOAT to create 32F or 16F textures on ES 2 when OES_texture_float is available.
		void	setDataType( GLint dataType ) { mDataType = dataType; }
		//! Sets the Texture's data type format to be automatically selected based on the context.
		void	setAutoDataType() { mDataType = -1; }
		
		// Specifies the texture comparison mode for currently bound depth textures.
		void	setCompareMode( GLenum compareMode ) { mCompareMode = compareMode; }
		// Specifies the comparison operator used when \c GL_TEXTURE_COMPARE_MODE is set to \c GL_COMPARE_R_TO_TEXTURE
		void	setCompareFunc( GLenum compareFunc ) { mCompareFunc = compareFunc; }
		//! Returns the texture comparison mode for currently bound depth texture.
		GLenum	getCompareMode() const { return mCompareMode; }
		//! Returns the comparison operator used when \c GL_TEXTURE_COMPARE_MODE is set to \c GL_COMPARE_R_TO_TEXTURE
		GLenum	getCompareFunc() const { return mCompareFunc; }

		//! Sets the wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
		void	setWrap( GLenum wrapS, GLenum wrapT ) { setWrapS( wrapS ); setWrapT( wrapT ); }
#if ! defined( CINDER_GL_ES )
        //! Sets the wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
        void	setWrap( GLenum wrapS, GLenum wrapT, GLenum wrapR ) { setWrapS( wrapS ); setWrapT( wrapT ); setWrapR( wrapR ); }
#endif
		//! Sets the horizontal wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
		void	setWrapS( GLenum wrapS ) { mWrapS = wrapS; }
		//! Sets the vertical wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
		void	setWrapT( GLenum wrapT ) { mWrapT = wrapT; }
#if ! defined( CINDER_GL_ES )
		//! Sets the depth wrapping behavior when a texture coordinate falls outside the range of [0,1]. Possible values are \c GL_REPEAT, \c GL_CLAMP_TO_EDGE, etc. Default is \c GL_CLAMP_TO_EDGE.
		void	setWrapR( GLenum wrapR ) { mWrapR = wrapR; }
#endif
		//! Sets the filtering behavior when a texture is displayed at a lower resolution than its native resolution. Default is \c GL_LINEAR unless mipmapping is enabled, in which case \c GL_LINEAR_MIPMAP_LINEAR
		void	setMinFilter( GLenum minFilter ) { mMinFilter = minFilter; mMinFilterSpecified = true; }
		//! Sets the filtering behavior when a texture is displayed at a higher resolution than its native resolution. Default is \c GL_LINEAR.
		void	setMagFilter( GLenum magFilter ) { mMagFilter = magFilter; }
		//! Sets the anisotropic filter amount. A value greater than 1.0 "enables" anisotropic filtering. Maximum of getMaxAnisotropyMax();
		void    setMaxAnisotropy( GLfloat maxAnisotropy ) { mMaxAnisotropy = maxAnisotropy; }
		
		//! Returns the texture's target
		GLenum	getTarget() const { return mTarget; }
		//! Returns whether the texture has mipmapping enabled
		bool	hasMipmapping() const { return mMipmapping; }
		
		//! Returns the Texture's internal format. A value of -1 implies automatic selection of the internal format based on the context.
		GLint	getInternalFormat() const { return mInternalFormat; }
		//! Returns whether the Texture's internal format will be automatically selected based on the context.
		bool	isAutoInternalFormat() const { return mInternalFormat == -1; }
		//! Returns the data type parameter used by glTexImage2D when glTexStorage2D is unavailable. Defaults to \c -1 which implies automatic determination.
		GLint	getDataType() const { return mDataType; }
		//! Returns whether the Texture's data type will be automatically selected based on the context.
		bool	isAutoDataType() const { return mDataType == -1; }
		
		//! Returns the horizontal wrapping behavior for the texture coordinates.
		GLenum	getWrapS() const { return mWrapS; }
		//! Returns the vertical wrapping behavior for the texture coordinates.
		GLenum	getWrapT() const { return mWrapT; }
#if ! defined( CINDER_GL_ES )
		//! Returns the depth wrapping behavior for the texture coordinates.
		GLenum	getWrapR() const { return mWrapR; }
#endif
		//! Returns the texture minifying function, which is used whenever the pixel being textured maps to an area greater than one texture element.
		GLenum	getMinFilter() const { return mMinFilter; }
		//! Returns the texture magnifying function, which is used whenever the pixel being textured maps to an area less than or equal to one texture element.
		GLenum	getMagFilter() const { return mMagFilter; }
		//! Returns the texture anisotropic filtering amount
		GLfloat getMaxAnisotropy() const { return mMaxAnisotropy; }

#if ! defined( CINDER_GL_ES )
		//! Supplies an intermediate PBO that Texture constructors optionally make use of. A PBO of an inadequate size may result in an exception.
		void			setIntermediatePbo( const PboRef &intermediatePbo ) { mIntermediatePbo = intermediatePbo; }
		//! Returns the optional intermediate PBO that Texture constructors may make use of.
		const PboRef&	getIntermediatePbo() const { return mIntermediatePbo; }
#endif
		//! Sets the texture's border color. Ignored in OpenGL ES.
		void			setBorderColor( const std::array<GLfloat, 4> &border ) { mBorderColor = border; mBorderSpecified = true; }
		//! Sets the texture's border color. Ignored in OpenGL ES.
		void			setBorderColor( const ColorA &color );

		//! Sets the swizzle mask corresponding to \c GL_TEXTURE_SWIZZLE_RGBA. Expects \c GL_RED through \c GL_ALPHA, or \c GL_ONE or \c GL_ZERO
		void	setSwizzleMask( const std::array<GLint,4> &swizzleMask ) { mSwizzleMask = swizzleMask; mSwizzleSpecified = true; }
		//! Sets the swizzle mask corresponding to \c GL_TEXTURE_SWIZZLE_RGBA. Expects \c GL_RED through \c GL_ALPHA, or \c GL_ONE or \c GL_ZERO
		void	setSwizzleMask( GLint r, GLint g, GLint b, GLint a );
		//! Returns the swizzle mask corresponding to \c GL_TEXTURE_SWIZZLE_RGBA.
		const std::array<GLint,4>&	getSwizzleMask() const { return mSwizzleMask; }
		
		//! Returns the debugging label associated with the Texture.
		const std::string&	getLabel() const { return mLabel; }
		//! Sets the debugging label associated with the Texture. Calls glObjectLabel() when available.
		void				setLabel( const std::string &label ) { mLabel = label; }
		//! Sets the debugging label associated with the Texture. Calls glObjectLabel() when available.
		Format&				label( const std::string &label ) { setLabel( label ); return *this; }
#if ! defined( CINDER_GL_ES )
		//! (NVIDIA only) Returns whether per-gpu storage is enabled.
		bool				isPerGpuStorageNV() const { return mPerGpuStorageSpecifiedNV && mPerGpuStorageEnabledNV; }
		//! (NVIDIA only) Enables per-gpu storage for multi-gpu multicast.
		void				setPerGpuStorageNV( bool enable ) { mPerGpuStorageSpecifiedNV = true; mPerGpuStorageEnabledNV = enable; }
		//! (NVIDIA only) Enables per-gpu storage for multi-gpu multicast.
		Format&				perGpuStorageNV( bool enable = true ) { setPerGpuStorageNV( enable ); return *this; }
#endif
	protected:
		Format();
	
		GLenum				mTarget;
		GLenum				mWrapS, mWrapT, mWrapR;
		GLenum				mMinFilter, mMagFilter;
		GLint				mCompareMode, mCompareFunc;		
		bool				mMipmapping, mMipmappingSpecified;
		bool				mMinFilterSpecified;
		GLuint				mBaseMipmapLevel;
		GLint				mMaxMipmapLevel;
		bool				mImmutableStorage;
		GLfloat				mMaxAnisotropy;
		GLint				mInternalFormat, mDataType;
		bool				mSwizzleSpecified;
		std::array<GLint,4>	mSwizzleMask;
		bool				mBorderSpecified;
		std::array<GLfloat,4>	mBorderColor;
		std::string			mLabel; // debug label

#if ! defined( CINDER_GL_ES )
		bool				mPerGpuStorageSpecifiedNV;
		bool				mPerGpuStorageEnabledNV;
		PboRef				mIntermediatePbo;
#endif
		friend class TextureBase;
	};

  protected:
	TextureBase();
	TextureBase( GLenum target, GLuint textureId, GLint internalFormat );
	
	void			initParams( Format &format, GLint defaultInternalFormat, GLint defaultDataType );

	virtual void	printDims( std::ostream &os ) const = 0;
	
	GLenum				mTarget;
	GLuint				mTextureId;
	mutable GLint		mInternalFormat;
	bool				mMipmapping;
	GLint				mBaseMipmapLevel, mMaxMipmapLevel;
	bool				mDoNotDispose;
	std::array<GLint,4>	mSwizzleMask;
	std::string			mLabel; // debugging label
	
	friend CI_API std::ostream& operator<<( std::ostream &os, const TextureBase &rhs );
};

CI_API std::ostream& operator<<( std::ostream &os, const TextureBase &rhs );

class CI_API TextureData {
  public:
	//! Represents a face of a texture; typically 1 Face per Level; CubeMaps have 6.
	struct CI_API Face {
  		GLsizei						dataSize;
		size_t						offset;
//		std::shared_ptr<uint8_t>	dataStore;
	};

	//! Represents a single mip-level, composed of 1 or more Faces
	struct CI_API Level {
		GLsizei						width, height, depth;

		size_t						getNumFaces() const { return mFaces.size(); }
		const Face&					getFace( size_t index ) const { return mFaces.at( index ); }
		const std::vector<Face>&	getFaces() const { return mFaces; }
		std::vector<Face>&			getFaces() { return mFaces; }
		Face&						back() { return mFaces.back(); }
		void						push_back( const Face &face ) { mFaces.push_back( face ); }
		
		std::vector<Face>			mFaces;
	};

	TextureData();
#if ! defined( CINDER_GL_ES )
	//! Binds the pbo if it's not nullptr
	TextureData( const PboRef &pbo );
#endif
	//! Unbinds the pbo if it is not nullptr
	~TextureData();

	GLint				getWidth() const { return mWidth; }
	void				setWidth( GLint width ) { mWidth = width; }
	GLint				getHeight() const { return mHeight; }
	void				setHeight( GLint height ) { mHeight = height; }
	GLint				getDepth() const { return mDepth; }
	void				setDepth( GLint depth ) { mDepth = depth; }
	GLint				getNumFaces() const { return mNumFaces; }
	void				setNumFaces( GLint numFaces ) { mNumFaces = numFaces; }	
	
	bool				isCompressed() const { return mDataType == 0; }
	GLint				getInternalFormat() const { return mInternalFormat; }
	void				setInternalFormat( GLint internalFormat ) { mInternalFormat = internalFormat; }
	GLenum				getDataFormat() const { return mDataFormat; }
	void				setDataFormat( GLenum dataFormat ) { mDataFormat = dataFormat; }
	GLenum				getDataType() const { return mDataType; }
	void				setDataType( GLenum dataType ) { mDataType = dataType; }
	GLint				getUnpackAlignment() const { return mUnpackAlignment; }
	void				setUnpackAlignment( GLint unpackAlignment ) { mUnpackAlignment = unpackAlignment; }

	const std::array<GLint,4>&	getSwizzleMask() const { return mSwizzleMask; }
	void						setSwizzleMask( const std::array<GLint,4> &swizzleMask ) { mSwizzleMask = swizzleMask; }

	size_t						getNumLevels() const { return mLevels.size(); }
	const std::vector<Level>&	getLevels() const { return mLevels; }
	std::vector<Level>&			getLevels() { return mLevels; }
	Level&						back() { return mLevels.back(); }
	void						push_back( const Level &level ) { mLevels.push_back( level ); }
	void						clear() { mLevels.clear(); }

	void	allocateDataStore( size_t requireBytes );
	size_t	getDataStoreSize() const { return mDataStoreSize; }
	void*	getDataStorePtr( size_t offset ) const;
	void	mapDataStore();
	void	unmapDataStore();

  private:
	void		init();
	
	GLint				mWidth, mHeight, mDepth, mNumFaces;
	GLint				mInternalFormat;
	GLenum				mDataFormat, mDataType;
	GLint				mUnpackAlignment;
	std::array<GLint,4>	mSwizzleMask;

	std::vector<Level>	mLevels; // mip-levels
	
  #if ! defined( CINDER_GL_ES )
	PboRef						mPbo;
	void*						mPboMappedPtr;
  #endif
	std::unique_ptr<uint8_t[]>	mDataStoreMem;
	size_t						mDataStoreSize;
};

#if ! defined( CINDER_GL_ES )
class CI_API Texture1d : public TextureBase {
  public:
	struct CI_API Format : public TextureBase::Format {
		//! Default constructor, sets the target to \c GL_TEXTURE_1D, wrap to \c GL_CLAMP, disables mipmapping, the internal format to "automatic"
		Format() : TextureBase::Format() { mTarget = GL_TEXTURE_1D; }

		//! Chaining functions for Format class.
		//! Sets the target, defaults to \c GL_TEXTURE_1D
		Format& target( GLenum target ) { mTarget = target; return *this; }
		Format& mipmap( bool enableMipmapping = true ) { mMipmapping = enableMipmapping; return *this; }
		//! Sets the maximum amount of anisotropic filtering. A value greater than 1.0 "enables" anisotropic filtering. Maximum of getMaxAnisotropyMax();
		Format& maxAnisotropy( float maxAnisotropy ) { mMaxAnisotropy = maxAnisotropy; return *this; }
		Format& internalFormat( GLint internalFormat ) { mInternalFormat = internalFormat; return *this; }
		Format& wrap( GLenum wrap ) { mWrapS = wrap; return *this; }
		Format& wrapS( GLenum wrapS ) { mWrapS = wrapS; return *this; }
		Format& minFilter( GLenum minFilter ) { setMinFilter( minFilter ); return *this; }
		Format& magFilter( GLenum magFilter ) { setMagFilter( magFilter ); return *this; }
		//! Sets whether the storage for the cannot be changed in the future (making glTexImage1D() calls illegal). More efficient when possible. Default is \c false.
		Format& immutableStorage( bool immutable = true ) { setImmutableStorage( immutable ); return *this; }
		//! Sets the debugging label associated with the Texture. Calls glObjectLabel() when available.
		Format&	label( const std::string &label ) { setLabel( label ); return *this; }
		//! Sets a custom deleter for destruction of the shared_ptr<Texture1d>
		Format&	deleter( const std::function<void(Texture1d*)> &sharedPtrDeleter ) { mDeleter = sharedPtrDeleter; return *this; }
#if ! defined( CINDER_GL_ES )
		//! (NVIDIA only) Enables per-gpu storage for multi-gpu multicast.
		Format& perGpuStorageNV( bool enable = true ) { setPerGpuStorageNV( enable ); return *this; }
#endif
	  protected:
		std::function<void(Texture1d*)>		mDeleter;
		
		friend Texture1d;
	};

	static Texture1dRef create( GLint width, const Format &format = Format() );
	//! Constructs a Texture1d using the top row of \a surface
	static Texture1dRef create( const Surface8u &surface, const Format &format = Format() );
	//! Constructs a Texture1d using the data pointed to by \a data, in format \a dataFormat. For a dataType other than \c GL_UNSIGNED_CHAR use \a format.setDataType()
	static Texture1dRef	create( const void *data, GLenum dataFormat, int width, const Format &format = Format() );
	
	//! Updates the Texture1d using the top row of \a surface.
	void	update( const Surface8u &surface, int mipLevel = 0 );
	//! Updates the pixels of a Texture1d with the data pointed to by \a data, of format \a dataFormat (Ex: GL_RGB), and type \a dataType (Ex: GL_UNSIGNED_BYTE) of size \a width.
	void	update( const void *data, GLenum dataFormat, GLenum dataType, int mipLevel, int width, int offset = 0 );
	
	//! Returns the width of the texture in pixels
	GLint			getWidth() const override { return mWidth; }
	//! Returns the height of the texture in pixels, which is always \c 1
	GLint			getHeight() const override { return 1; }
	//! Returns the depth of the texture, which is always \c 1
	GLint			getDepth() const override { return 1; }

  protected:
  	Texture1d( GLint width, Format format );
	Texture1d( const Surface8u &surface, Format format );
	Texture1d( const void *data, GLenum dataFormat, int width, Format format );

	void	printDims( std::ostream &os ) const override;

	GLint		mWidth;
};

#endif // ! defined( CINDER_GL_ES )

class CI_API Texture2d : public TextureBase {
  public:
	struct CI_API Format : public TextureBase::Format {
		//! Default constructor, sets the target to \c GL_TEXTURE_2D, wrap to \c GL_CLAMP, disables mipmapping, the internal format to "automatic"
		Format() : TextureBase::Format(), mLoadTopDown( false ) {}

		//! Chaining functions for Format class.
		Format& target( GLenum target ) { mTarget = target; return *this; }
		Format& mipmap( bool enableMipmapping = true ) { mMipmapping = enableMipmapping; return *this; }
		//! Sets the maximum amount of anisotropic filtering. A value greater than 1.0 "enables" anisotropic filtering. Maximum of getMaxAnisotropyMax();
		Format& maxAnisotropy( float maxAnisotropy ) { mMaxAnisotropy = maxAnisotropy; return *this; }
		//! Specifies the internal format for the Texture, used by glTexImage2D or glTexStorage2D when available. Defaults to \c -1 which implies automatic determination.
		Format& internalFormat( GLint internalFormat ) { mInternalFormat = internalFormat; return *this; }
		//! Specifies the data type parameter used by glTexImage2D when glTexStorage2D is unavailable. Defaults to \c -1 which implies automatic determination. Primary use is to pass \c GL_FLOAT or \c GL_HALF_FLOAT to create 32F or 16F textures on ES 2 when OES_texture_float is available.
		Format& dataType( GLint dataType ) { mDataType = dataType; return *this; }
		Format& wrap( GLenum wrap ) { mWrapS = mWrapT = mWrapR = wrap; return *this; }
		Format& wrapS( GLenum wrapS ) { mWrapS = wrapS; return *this; }
		Format& wrapT( GLenum wrapT ) { mWrapT = wrapT; return *this; }
#if ! defined( CINDER_GL_ES )
		Format& wrapR( GLenum wrapR ) { mWrapR = wrapR; return *this; }
#endif
		Format& minFilter( GLenum minFilter ) { setMinFilter( minFilter ); return *this; }
		Format& magFilter( GLenum magFilter ) { setMagFilter( magFilter ); return *this; }
		Format& compareMode( GLenum compareMode ) { mCompareMode = compareMode; return *this; }
		//! Specifies the comparison operator used when \c GL_TEXTURE_COMPARE_MODE is set to \c GL_COMPARE_R_TO_TEXTURE.
		Format& compareFunc( GLenum compareFunc ) { mCompareFunc = compareFunc; return *this; }		Format& swizzleMask( const std::array<GLint,4> &swizzleMask ) { setSwizzleMask( swizzleMask ); return *this; }
		Format& swizzleMask( GLint r, GLint g, GLint b, GLint a ) { setSwizzleMask( r, g, b, a ); return *this; }
		//! Specifies whether the Texture should store scanlines top-down in memory. Default is \c false. Also marks Texture as top-down when \c true.
		Format& loadTopDown( bool loadTopDown = true ) { mLoadTopDown = loadTopDown; return *this; }
		//! Sets whether the storage for the cannot be changed in the future (making glTexImage2D() calls illegal). More efficient when possible. Default is \c false.
		Format& immutableStorage( bool immutable = true ) { setImmutableStorage( immutable ); return *this; }
#if ! defined( CINDER_GL_ES )
		Format& intermediatePbo( const PboRef &intermediatePbo ) { setIntermediatePbo( intermediatePbo ); return *this; }
#endif		
		//! Sets the debugging label associated with the Texture. Calls glObjectLabel() when available.
		Format&	label( const std::string &label ) { setLabel( label ); return *this; }
		//! Sets a custom deleter for destruction of the shared_ptr<Texture2d>
		Format&	deleter( const std::function<void(Texture2d*)> &sharedPtrDeleter ) { mDeleter = sharedPtrDeleter; return *this; }
#if ! defined( CINDER_GL_ES )
		//! (NVIDIA only) Enables per-gpu storage for multi-gpu multicast.
		Format& perGpuStorageNV( bool enable = true ) { setPerGpuStorageNV( enable ); return *this; }
#endif
	  protected:
		bool								mLoadTopDown;
		std::function<void(Texture2d*)>		mDeleter;
		
		friend		Texture2d;
	};
	
	//! Constructs a texture of size(\a width, \a height) and allocates storage.
	static Texture2dRef	create( int width, int height, const Format &format = Format() );
	//! Constructs a texture of size(\a width, \a height). Pixel data is provided by \a data in format \a dataFormat (Ex: \c GL_RGB, \c GL_RGBA). Use \a format.setDataType() to specify a dataType other than \c GL_UNSIGNED_CHAR. Ignores \a format.loadTopDown().
	static Texture2dRef	create( const void *data, GLenum dataFormat, int width, int height, const Format &format = Format() );
	//! Constructs a Texture based on the contents of \a surface.
	static Texture2dRef	create( const Surface8u &surface, const Format &format = Format() );
	//! Constructs a Texture based on the contents of \a channel. Sets swizzle mask to {R,R,R,1} where supported unless otherwise specified in \a format.
	static Texture2dRef	create( const Channel8u &channel, const Format &format = Format() );
	//! Constructs a Texture based on the contents of \a surface.
	static Texture2dRef	create( const Surface16u &surface, const Format &format = Format() );
	//! Constructs a Texture based on the contents of \a channel. Sets swizzle mask to {R,R,R,1} where supported unless otherwise specified in \a format.
	static Texture2dRef	create( const Channel16u &channel, const Format &format = Format() );
	//! Constructs a Texture based on the contents of \a surface.
	static Texture2dRef	create( const Surface32f &surface, const Format &format = Format() );
	/** \brief Constructs a texture based on the contents of \a channel. A default value of -1 for \a internalFormat chooses an appropriate internal format automatically. **/
	static Texture2dRef	create( const Channel32f &channel, const Format &format = Format() );
	//! Constructs a Texture based on \a imageSource. A default value of -1 for \a internalFormat chooses an appropriate internal format based on the contents of \a imageSource. Uses a Format's intermediate PBO when available, which is resized as necessary.
	static Texture2dRef	create( ImageSourceRef imageSource, const Format &format = Format() );
	//! Constructs a Texture based on an externally initialized OpenGL texture. \a doNotDispose specifies whether the Texture is responsible for disposing of the associated OpenGL resource. Supports a custom deleter.
	static Texture2dRef	create( GLenum target, GLuint textureID, int width, int height, bool doNotDispose, const std::function<void(Texture2d*)> &deleter = std::function<void(Texture2d*)>() );
	//! Constructs a Texture based on an instance of TextureData
	static Texture2dRef	create( const TextureData &data, const Format &format );
	//! Constructs a Texture from an optionally compressed KTX file. Enables mipmapping if KTX file contains mipmaps and Format has not specified \c false for mipmapping. Uses Format's intermediate PBO if supplied; requires it to be large enough to hold all MIP levels and throws if it is not. (http://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/)
	static Texture2dRef	createFromKtx( const DataSourceRef &dataSource, const Format &format = Format() );
#if ! defined( CINDER_GL_ES ) || defined( CINDER_GL_ANGLE )
	//! Constructs a Texture from a DDS file. Supports DXT1, DTX3, and DTX5. Supports BC7 in the presence of \c GL_ARB_texture_compression_bptc. Enables mipmapping if DDS contains mipmaps and Format has not specified \c false for mipmapping. ANGLE version requires textures to be a multiple of 4 due to DX limitation.
	static Texture2dRef	createFromDds( const DataSourceRef &dataSource, const Format &format = Format() );
#endif

	//! Allows specification of some Area other than the Texture's full area for cases where not all pixels in the Texture are usable / "clean"; common in video decoding pipelines in particular. Specified in pixels, and relative to upper-left origin coordinate system regardless of whether Textre is top-down or not.
	void			setCleanBounds( const Area &cleanBounds );

	//! Updates the pixels of a Texture with the data pointed to by \a data, of format \a dataFormat (Ex: GL_RGB), and type \a dataType (Ex: GL_UNSIGNED_BYTE) of size (\a width, \a height). \a destLowerLeftOffset specifies a texel offset to copy to within the Texture.
	void			update( const void *data, GLenum dataFormat, GLenum dataType, int mipLevel, int width, int height, const ivec2 &destLowerLeftOffset = ivec2( 0, 0 ) );
	//! Updates the pixels of a Texture with contents of \a surface. Expects \a surface's size to match the Texture's at \a mipLevel. \a destLowerLeftOffset specifies a texel offset to copy to within the Texture.
	void			update( const Surface8u &surface, int mipLevel = 0, const ivec2 &destLowerLeftOffset = ivec2( 0, 0 ) );
	//! Updates the pixels of a Texture with contents of \a channel. Expects \a channel's size to match the Texture's at \a mipLevel. \a destLowerLeftOffset specifies a texel offset to copy to within the Texture.
	void			update( const Channel8u &channel, int mipLevel = 0, const ivec2 &destLowerLeftOffset = ivec2( 0, 0 ) );
	//! Updates the pixels of a Texture with contents of \a surface. Expects \a surface's size to match the Texture's at \a mipLevel. \a destLowerLeftOffset specifies a texel offset to copy to within the Texture.
	void			update( const Surface16u &surface, int mipLevel = 0, const ivec2 &destLowerLeftOffset = ivec2( 0, 0 ) );
	//! Updates the pixels of a Texture with contents of \a channel. Expects \a channel's size to match the Texture's at \a mipLevel. \a destLowerLeftOffset specifies a texel offset to copy to within the Texture.
	void			update( const Channel16u &channel, int mipLevel = 0, const ivec2 &destLowerLeftOffset = ivec2( 0, 0 ) );
	//! Updates the pixels of a Texture with contents of \a surface. Expects \a surface's size to match the Texture's at \a mipLevel. \a destLowerLeftOffset specifies a texel offset to copy to within the Texture.
	void			update( const Surface32f &surface, int mipLevel = 0, const ivec2 &destLowerLeftOffset = ivec2( 0, 0 ) );
	//! Updates the pixels of a Texture with contents of \a channel. Expects \a channel's size to match the Texture's at \a mipLevel. \a destLowerLeftOffset specifies a texel offset to copy to within the Texture.
	void			update( const Channel32f &channel, int mipLevel = 0, const ivec2 &destLowerLeftOffset = ivec2( 0, 0 ) );
	//! Updates the pixels of a Texture with contents of \a textureData. Inefficient if the bounds of \a textureData don't match those of \a this
	void			update( const TextureData &textureData );
#if ! defined( CINDER_GL_ES )
	//! Updates the pixels of a Texture with the contents of a PBO (whose target must be \c GL_PIXEL_UNPACK_BUFFER) at mipmap level \a mipLevel. \a format and \a type correspond to parameters of glTexSubImage2D, and would often be GL_RGB and GL_UNSIGNED_BYTE respectively. Reads from the PBO starting at \a pboByteOffset.
	void			update( const PboRef &pbo, GLenum format, GLenum type, int mipLevel = 0, size_t pboByteOffset = 0 );
	//! Updates a subregion (measured as origin upper-left) of the pixels of a Texture with the contents of a PBO (whose target must be \c GL_PIXEL_UNPACK_BUFFER) at mipmap level \a mipLevel.  \a format and \a type correspond to parameters of glTexSubImage2D, and would often be GL_RGB and GL_UNSIGNED_BYTE respectively. Reads from the PBO starting at \a pboByteOffset.
	void			update( const PboRef &pbo, GLenum format, GLenum type, const Area &destArea, int mipLevel = 0, size_t pboByteOffset = 0 );
	//! Updates a Texture from a KTX file. Uses \a intermediatePbo if supplied; requires it to be large enough to hold all MIP levels and throws if it is not.
	void			updateFromKtx( const DataSourceRef &dataSource, const PboRef &intermediatePbo = PboRef() );
	//! Updates a Texture from a DDS file. Uses \a intermediatePbo if supplied; requires it to be large enough to hold all MIP levels and throws if it is not.
	void			updateFromDds( const DataSourceRef &dataSource, const PboRef &intermediatePbo = PboRef() );
#else
	//! Updates a Texture from a KTX file. Assumes sizes match.
	void			updateFromKtx( const DataSourceRef &dataSource );
  #if defined( CINDER_GL_ANGLE )
	//! Updates a Texture from a KTX file. Assumes sizes match.
	void			updateFromDds( const DataSourceRef &dataSource );
  #endif
#endif
	//! Replaces the pixels (and data store) of a Texture with contents of \a textureData. Use update() instead if the bounds of \a this match those of \a textureData
	void			replace( const TextureData &textureData );

	//! Returns the width of the texture in pixels.
	GLint			getWidth() const override { return mCleanBounds.getWidth(); }
	//! Returns the height of the texture in pixels.
	GLint			getHeight() const override { return mCleanBounds.getHeight(); }
	//! Returns the depth of the texture in pixels.
	GLint			getDepth() const override { return 1; }
	//! Returns the true width of the texture in pixels, which may be larger than it's "clean" area
	GLint			getActualWidth() const { return mActualSize.x; }
	//! Returns the true height of the texture in pixels, which may be larger than it's "clean" area
	GLint			getActualHeight() const { return mActualSize.y; }
	//! Returns size of the texture in pixels, igonring clean bounds
	ivec2			getSize() const { return ivec2( getWidth(), getHeight() ); }
	//! Returns the UV coordinates which correspond to the pixels contained in \a area (as expressed with an upper-left origin, relative to clean bounds). Accounts for top-down storage and target (0-1 for \c GL_TEXTURE_2D and pixels for \c GL_TEXTURE_RECTANGLE)
	Rectf			getAreaTexCoords( const Area &area ) const;
	//! Returns whether the scanlines of the image are stored top-down in memory relative to the base address. Default is \c false.
	bool			isTopDown() const { return mTopDown; }
	//!	Marks whether the scanlines of the image are stored top-down in memory relative to the base address. Default is \c false.
	void			setTopDown( bool topDown = true ) { mTopDown = topDown; }
	//! Explicitly regenerate mipmaps assuming mipmapping is enabled.
	void			regenerateMipmap();
	
	//! Returns an ImageSource pointing to this Texture
	ImageSourceRef	createSource();
	
  protected:

	Texture2d( int width, int height, Format format = Format() );
	Texture2d( const void *data, GLenum dataFormat, int width, int height, Format format = Format() );
	Texture2d( const Surface8u &surface, Format format = Format() );
	Texture2d( const Surface16u &surface, Format format = Format() );
	Texture2d( const Surface32f &surface, Format format = Format() );
	Texture2d( const Channel8u &channel, Format format = Format() );
	Texture2d( const Channel16u &channel, Format format = Format() );
	Texture2d( const Channel32f &channel, Format format = Format() );
	Texture2d( const ImageSourceRef &imageSource, Format format = Format() );
	Texture2d( GLenum target, GLuint textureId, int width, int height, bool doNotDispose );
	Texture2d( const TextureData &data, Format format );
	
	void	printDims( std::ostream &os ) const override;
	void	initParams( Format &format, GLint defaultInternalFormat, GLint defaultDataType );
	void	initMaxMipmapLevel();
	template<typename T>
	void	setData( const SurfaceT<T> &surface, bool createStorage, int mipLevel, const ivec2 &offset );
	template<typename T>
	void	setData( const ChannelT<T> &channel, bool createStorage, int mipLevel, const ivec2 &offset );
	void	initData( const void *data, GLenum dataFormat, const Format &format );
	void	initData( const ImageSourceRef &imageSource, const Format &format );
#if ! defined( CINDER_GL_ES )
	void	initDataImageSourceWithPboImpl( const ImageSourceRef &imageSource, const Format &format, GLint dataFormat, GLint dataType, ImageIo::ChannelOrder channelOrder, bool isGray, const PboRef &pbo );
#endif
	void	initDataImageSourceImpl( const ImageSourceRef &imageSource, const Format &format, GLint dataFormat, GLint dataType, ImageIo::ChannelOrder channelOrder, bool isGray );

	ivec2		mActualSize; // true texture size in pixels, as opposed to clean bounds
	Area		mCleanBounds; // relative to upper-left origin regardless of top-down
	bool		mTopDown;
	
	friend class Texture2dCache;
};

#if ! defined( CINDER_GL_ES_2 )
class CI_API Texture3d : public TextureBase {
  public:
	struct CI_API Format : public TextureBase::Format {
		//! Default constructor, sets the target to \c GL_TEXTURE_3D, wrap to \c GL_CLAMP, disables mipmapping, the internal format to "automatic"
		Format() : TextureBase::Format() { mTarget = GL_TEXTURE_3D; }

		//! Chaining functions for Format class.
		//! Sets the target, defaults to \c GL_TEXTURE_3D, also supports \c GL_TEXTURE_2D_ARRAY
		Format& target( GLenum target ) { mTarget = target; return *this; }
		Format& mipmap( bool enableMipmapping = true ) { mMipmapping = enableMipmapping; return *this; }
		//! Sets the maximum amount of anisotropic filtering. A value greater than 1.0 "enables" anisotropic filtering. Maximum of getMaxAnisotropyMax();
		Format& maxAnisotropy( float maxAnisotropy ) { mMaxAnisotropy = maxAnisotropy; return *this; }
		Format& internalFormat( GLint internalFormat ) { mInternalFormat = internalFormat; return *this; }
		Format& wrap( GLenum wrap ) { mWrapS = mWrapT = mWrapR = wrap; return *this; }
		Format& wrapS( GLenum wrapS ) { mWrapS = wrapS; return *this; }
		Format& wrapT( GLenum wrapT ) { mWrapT = wrapT; return *this; }
#if ! defined( CINDER_GL_ES )
		Format& wrapR( GLenum wrapR ) { mWrapR = wrapR; return *this; }
#endif
		Format& minFilter( GLenum minFilter ) { setMinFilter( minFilter ); return *this; }
		Format& magFilter( GLenum magFilter ) { setMagFilter( magFilter ); return *this; }
		//! Sets whether the storage for the cannot be changed in the future (making glTexImage3D() calls illegal). More efficient when possible. Default is \c false.
		Format& immutableStorage( bool immutable = true ) { setImmutableStorage( immutable ); return *this; }
		//! Sets the debugging label associated with the Texture. Calls glObjectLabel() when available.
		Format&	label( const std::string &label ) { setLabel( label ); return *this; }
		//! Sets a custom deleter for destruction of the shared_ptr<Texture3d>
		Format&	deleter( const std::function<void(Texture3d*)> &sharedPtrDeleter ) { mDeleter = sharedPtrDeleter; return *this; }
#if ! defined( CINDER_GL_ES )
		//! (NVIDIA only) Enables per-gpu storage for multi-gpu multicast.
		Format& perGpuStorageNV( bool enable = true ) { setPerGpuStorageNV( enable ); return *this; }
#endif
	  protected:
		std::function<void(Texture3d*)>		mDeleter;
		
		friend Texture3d;
	};

	//! Constructs a texture of size(\a width, \a height, \a depth).
	static Texture3dRef create( GLint width, GLint height, GLint depth, const Format &format = Format() );
	//! Constructs a texture of size(\a width, \a height, \a depth). Pixel data is provided by \a data in format \a dataFormat (Ex: \c GL_RGB, \c GL_RGBA). Use \a format.setDataType() to specify a dataType other than \c GL_UNSIGNED_CHAR.
	static Texture3dRef create( const void *data, GLenum dataFormat, int width, int height, int depth, const Format &format = Format() );
  
	void	update( const Surface &surface, int depth, int mipLevel = 0 );

	void	update( const void *data, GLenum dataFormat, GLenum dataType, int mipLevel, int width, int height, int depth, int xOffset = 0, int yOffset = 0, int zOffset = 0 );
	
	//! Returns the width of the texture in pixels
	GLint			getWidth() const override { return mWidth; }
	//! Returns the height of the texture in pixels
	GLint			getHeight() const override { return mHeight; }
	//! Returns the depth of the texture, which is the number of images in a texture array, or the depth of a 3D texture measured in pixels
	GLint			getDepth() const override { return mDepth; }

  protected:
  	Texture3d( GLint width, GLint height, GLint depth, Format format );
	Texture3d( const void *data, GLenum dataFormat, int width, int height, int depth, Format format );

	void	printDims( std::ostream &os ) const override;

	GLint		mWidth, mHeight, mDepth;
};
#endif // ! defined( CINDER_GL_ES_2 )

class CI_API TextureCubeMap : public TextureBase
{
  public:
  	struct CI_API Format : public TextureBase::Format {
		//! Default constructor, sets the target to \c GL_TEXTURE_CUBE_MAP, wrap to \c GL_CLAMP_TO_EDGE, disables mipmapping, the internal format to "automatic"
		Format();

		//! Chaining functions for Format class.
		Format& target( GLenum target ) { mTarget = target; return *this; }
		Format& mipmap( bool enableMipmapping = true ) { mMipmapping = enableMipmapping; return *this; }
		//! Sets the maximum amount of anisotropic filtering. A value greater than 1.0 "enables" anisotropic filtering. Maximum of getMaxAnisotropyMax();
		Format& maxAnisotropy( float maxAnisotropy ) { mMaxAnisotropy = maxAnisotropy; return *this; }
		Format& internalFormat( GLint internalFormat ) { mInternalFormat = internalFormat; return *this; }
		Format& wrap( GLenum wrap ) { mWrapS = mWrapT = mWrapR = wrap; return *this; }
		Format& wrapS( GLenum wrapS ) { mWrapS = wrapS; return *this; }
		Format& wrapT( GLenum wrapT ) { mWrapT = wrapT; return *this; }
#if ! defined( CINDER_GL_ES )
		Format& wrapR( GLenum wrapR ) { mWrapR = wrapR; return *this; }		
#endif // ! defined( CINDER_GL_ES )
		Format& minFilter( GLenum minFilter ) { setMinFilter( minFilter ); return *this; }
		Format& magFilter( GLenum magFilter ) { setMagFilter( magFilter ); return *this; }
		//! Sets whether the storage for the cannot be changed in the future (making glTexImage2D() calls illegal). More efficient when possible. Default is \c false.
		Format& immutableStorage( bool immutable = true ) { setImmutableStorage( immutable ); return *this; }
		//! Sets the debugging label associated with the Texture. Calls glObjectLabel() when available.
		Format&	label( const std::string &label ) { setLabel( label ); return *this; }
		//! Sets a custom deleter for destruction of the shared_ptr<TextureCubeMap>
		Format&	deleter( const std::function<void(TextureCubeMap*)> &sharedPtrDeleter ) { mDeleter = sharedPtrDeleter; return *this; }
#if ! defined( CINDER_GL_ES )
		//! (NVIDIA only) Enables per-gpu storage for multi-gpu multicast.
		Format& perGpuStorageNV( bool enable = true ) { setPerGpuStorageNV( enable ); return *this; }
#endif
	  protected:
		std::function<void(TextureCubeMap*)>	mDeleter;
		
		friend TextureCubeMap;
	};
  
	static TextureCubeMapRef	create( int32_t width, int32_t height, const Format &format = Format() );
	//! Automatically infers Horizontal Cross, Vertical Cross, Row, or Column based on image aspect ratio
	static TextureCubeMapRef	create( const ImageSourceRef &imageSource, const Format &format = Format() );
	//! Expects images ordered { +X, -X, +Y, -Y, +Z, -Z }
	static TextureCubeMapRef	create( const ImageSourceRef images[6], const Format &format = Format() );
	//! Constructs a TextureCubeMap based on an instance of TextureData
	static TextureCubeMapRef	create( const TextureData &data, const Format &format );
	//! Constructs a TextureCubeMap from a KTX file. Enables mipmapping if KTX file contains mipmaps and Format has not specified \c false for mipmapping. Uses Format's intermediate PBO if supplied; requires it to be large enough to hold all MIP levels and throws if it is not. (http://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/)
	static TextureCubeMapRef	createFromKtx( const DataSourceRef &dataSource, const Format &format = Format() );
#if ! defined( CINDER_GL_ES ) || defined( CINDER_GL_ANGLE )
	//! Constructs a TextureCubeMap from a DDS file. Supports DXT1, DTX3, and DTX5. Supports BC7 in the presence of \c GL_ARB_texture_compression_bptc. Enables mipmapping if DDS contains mipmaps and Format has not specified \c false for mipmapping. ANGLE version requires textures to be a multiple of 4 due to DX limitation.
	static TextureCubeMapRef	createFromDds( const DataSourceRef &dataSource, const Format &format = Format() );
#endif

	//! Returns the width of the texture in pixels
	GLint			getWidth() const override { return mWidth; }
	//! Returns the height of the texture in pixels
	GLint			getHeight() const override { return mHeight; }
	//! Returns the depth of the texture in pixels (
	GLint			getDepth() const override { return 1; }

	//! Replaces the pixels (and data store) of a Texture with contents of \a textureData.
	void			replace( const TextureData &textureData );
	//! Explicitly regenerate mipmaps assuming mipmapping is enabled.
	void			regenerateMipmap();
  protected:
	TextureCubeMap( int32_t width, int32_t height, Format format );
	template<typename T>
	TextureCubeMap( const SurfaceT<T> images[6], Format format );
	TextureCubeMap( const TextureData &data, Format format );

	template<typename T>
	static TextureCubeMapRef createTextureCubeMapImpl( const ImageSourceRef &imageSource, const Format &format );

	void	printDims( std::ostream &os ) const override;
	
	GLint		mWidth, mHeight;
};

typedef std::shared_ptr<class Texture2dCache> Texture2dCacheRef;

class CI_API Texture2dCache : public std::enable_shared_from_this<Texture2dCache>
{
  public:
	static Texture2dCacheRef create();
	static Texture2dCacheRef create( const Surface8u &prototypeSurface, const Texture2d::Format &format );
	
	Texture2dRef		cache( const Surface8u &data );
  protected:
	Texture2dCache();
	Texture2dCache( const Surface8u &prototypeSurface, const Texture2d::Format &format );
		
	void			markTextureAsFree( int id );
	
	int					mWidth;
	int					mHeight;
	Texture2d::Format	mFormat;
	Surface8uRef		mIntermediateSurface;

	int										mNextId;
	std::vector<std::pair<int,TextureRef>>	mTextures;
};

class CI_API SurfaceConstraintsGLTexture : public SurfaceConstraints {
  public:
	virtual SurfaceChannelOrder getChannelOrder( bool alpha ) const { return ( alpha ) ? SurfaceChannelOrder::BGRA : SurfaceChannelOrder::BGR; }
	virtual ptrdiff_t			getRowBytes( int32_t requestedWidth, const SurfaceChannelOrder &sco, int elementSize ) const { return requestedWidth * elementSize * sco.getPixelInc(); }
};

class CI_API KtxParseExc : public Exception {
  public:	
	KtxParseExc( const std::string &description ) : Exception( description )	{}
};

class CI_API DdsParseExc : public Exception {
  public:	
	DdsParseExc( const std::string &description ) : Exception( description )	{}
};

class CI_API TextureDataExc : public Exception {
  public:	
	TextureDataExc( const std::string &description ) : Exception( description )	{}
};

class CI_API TextureResizeExc : public TextureDataExc {
  public:
	TextureResizeExc( const std::string &description, const ivec2 &updateSize, const ivec2 &textureSize );
};

class CI_API TextureDataStoreTooSmallExc : public Exception {
  public:
	TextureDataStoreTooSmallExc() {}

	virtual const char* what() const throw()	{ return "The data store allocated for a TextureData is too small"; }
};
	
} } // namespace cinder::gl
