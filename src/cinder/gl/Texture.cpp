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

#include "cinder/gl/platform.h" // has to be first
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Pbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/TextureFormatParsers.h"
#include "cinder/gl/Environment.h"
#include "cinder/gl/ConstantConversions.h"
#include "cinder/gl/scoped.h"
#include "cinder/ip/Flip.h"
#include "cinder/Log.h"
#include <stdio.h>
#include <algorithm>
#include <memory>
#include <type_traits>

// Figure out if we need std::Log2
#if defined(CINDER_ANDROID ) && ((__GNUC__ == 4) && (__GNUC_MINOR__ <= 9))
    #if defined(__arm__)
        #define _IS_ARM32
    #endif

   	#if defined(__aarch64__)
        #define _IS_ARM64
    #endif

 	#if defined(i386) || defined(__i386) || defined(__i386__)
 		#define _IS_X86_32
 	#endif

    #if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__)
        #define _IS_X86_64
    #endif

 	#if (defined(mips) || defined(_mips) || defined(__mips__) || defined(__mips)) && ! defined(__mips64)
 		#define _IS_MIPS32
 	#endif

 	#if defined(__mips64)
 		#define _IS_MIPS64
 	#endif

    #if defined(_IS_ARM32) || defined(_IS_X86_32) || defined(_IS_MIPS32)
        #define NEEDS_STD_LOG2
    #endif

    #undef _IS_ARM32
 	#undef _IS_ARM64
    #undef _IS_X86_32
    #undef _IS_X86_64
 	#undef _IS_MIPS32
 	#undef _IS_MIPS64
#endif

#if defined(NEEDS_STD_LOG2)
namespace std {

double log2( double x ) {
#if __ANDROID_API__ < 18
    return ::log( x ) / ::log( 2.0 );
#else
    return ::log2( x );
#endif
}

} // namespace std
#endif
#undef NEEDS_STD_LOG2

using namespace std;

namespace cinder { namespace gl {

#if defined( CINDER_GL_ANGLE )
	#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT	GL_COMPRESSED_RGBA_S3TC_DXT3_ANGLE
	#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT	GL_COMPRESSED_RGBA_S3TC_DXT5_ANGLE
#endif

class ImageSourceTexture;
class ImageTargetTexture;

/////////////////////////////////////////////////////////////////////////////////
// ImageTargetGlTexture
template<typename T>
class ImageTargetGlTexture : public ImageTarget {
  public:
	static shared_ptr<ImageTargetGlTexture> create( const Texture *texture, ImageIo::ChannelOrder &channelOrder, bool isGray, bool hasAlpha );
#if ! defined( CINDER_GL_ES )
	// receives a pointer to an intermediate data store, presumably a mapped PBO
	static shared_ptr<ImageTargetGlTexture> create( const Texture *texture, ImageIo::ChannelOrder &channelOrder, bool isGray, bool hasAlpha, void *data );
#endif

	virtual bool	hasAlpha() const { return mHasAlpha; }
	virtual void*	getRowPointer( int32_t row );

	void*			getData() const { return mDataBaseAddress; }

  private:
	ImageTargetGlTexture( const Texture *texture, ImageIo::ChannelOrder &channelOrder, bool isGray, bool hasAlpha, void *intermediateData );

	const Texture		*mTexture;
	bool				mHasAlpha;
	uint8_t				mPixelInc;
	T					*mDataBaseAddress;
	unique_ptr<T[]>		mDataStore; // may be NULL
	int32_t				mRowInc;
};

/////////////////////////////////////////////////////////////////////////////////
// TextureBase
TextureBase::TextureBase()
	: mTarget( 0 ), mTextureId( 0 ), mInternalFormat( -1 ), mDoNotDispose( false ), mMipmapping( false )
{
	gl::context()->textureCreated( this );
}

TextureBase::TextureBase( GLenum target, GLuint textureId, GLint internalFormat )
	: mTarget( target ), mTextureId( textureId ), mInternalFormat( internalFormat ), mDoNotDispose( false ), mMipmapping( false )
{
	gl::context()->textureCreated( this );
}

TextureBase::~TextureBase()
{
	if ( ( mTextureId > 0 ) && ( ! mDoNotDispose ) ) {
		auto ctx = gl::context();
		if( ctx ) {
			ctx->textureDeleted( this );
			glDeleteTextures( 1, &mTextureId );
		}
	}
}

// Expects texture to be bound and mTarget,mTextureId and getWidth(), getHeight() and getDepth() functional
void TextureBase::initParams( Format &format, GLint defaultInternalFormat, GLint defaultDataType )
{
	// default is GL_REPEAT
	if( format.mWrapS != GL_REPEAT )
		glTexParameteri( mTarget, GL_TEXTURE_WRAP_S, format.mWrapS );
	// default is GL_REPEAT
	if( format.mWrapT != GL_REPEAT )
		glTexParameteri( mTarget, GL_TEXTURE_WRAP_T, format.mWrapT );

#if defined( CINDER_GL_HAS_WRAP_R )
	// default is GL_REPEAT
	if( format.mWrapR != GL_REPEAT )
		glTexParameteri( mTarget, GL_TEXTURE_WRAP_R, format.mWrapR );
#endif

	if( format.mMipmapping && ! format.mMinFilterSpecified )
		format.mMinFilter = GL_LINEAR_MIPMAP_LINEAR;
	// default is GL_NEAREST_MIPMAP_LINEAR
	if( format.mMinFilter != GL_NEAREST_MIPMAP_LINEAR )
		glTexParameteri( mTarget, GL_TEXTURE_MIN_FILTER, format.mMinFilter );

	// default is GL_LINEAR
	if( format.mMagFilter != GL_LINEAR )
		glTexParameteri( mTarget, GL_TEXTURE_MAG_FILTER, format.mMagFilter );

	if( format.mMaxAnisotropy > 1.0f )
		glTexParameterf( mTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, format.mMaxAnisotropy );

	if( format.mInternalFormat == -1 ) {
		mInternalFormat = defaultInternalFormat;
	}
	else {
		mInternalFormat = format.mInternalFormat;
	}

#if ! defined( CINDER_GL_ES )
	if( format.mPerGpuStorageSpecifiedNV )
		glTexParameteri( mTarget, GL_PER_GPU_STORAGE_NV, (GLint)format.mPerGpuStorageEnabledNV );
#endif

	//if( ( format.mDataType == -1 ) && ( defaultDataType > 0 ) )
	//	format.mDataType = defaultDataType;

	// Try to find a matching data type based on the internal format, use the
	// defaultDataType if a match isn't found.
	GLenum dataFormatFromInternal = GL_INVALID_ENUM;
	GLenum dataTypeFromInternal = GL_INVALID_ENUM;
	TextureBase::getInternalFormatInfo( mInternalFormat, &dataFormatFromInternal, &dataTypeFromInternal );
	if( -1 == format.mDataType && ( GL_INVALID_ENUM != dataTypeFromInternal ) ) {
		format.mDataType = dataTypeFromInternal;
	}
	else if( ( format.mDataType == -1 ) && ( defaultDataType > 0 ) ) {
		format.mDataType = defaultDataType;
	}

	// Swizzle mask
#if ! defined( CINDER_GL_ES )
	if( supportsHardwareSwizzle() ) {
		if( format.mSwizzleMask[0] != GL_RED || format.mSwizzleMask[1] != GL_GREEN || format.mSwizzleMask[2] != GL_BLUE || format.mSwizzleMask[3] != GL_ALPHA )
			glTexParameteriv( mTarget, GL_TEXTURE_SWIZZLE_RGBA, format.mSwizzleMask.data() );
	}
#elif defined( CINDER_GL_ES_3 )
	if( format.mSwizzleMask[0] != GL_RED )
		glTexParameteri( mTarget, GL_TEXTURE_SWIZZLE_R, format.mSwizzleMask[0] );
	if( format.mSwizzleMask[1] != GL_GREEN )
		glTexParameteri( mTarget, GL_TEXTURE_SWIZZLE_G, format.mSwizzleMask[1] );
	if( format.mSwizzleMask[2] != GL_BLUE )
		glTexParameteri( mTarget, GL_TEXTURE_SWIZZLE_B, format.mSwizzleMask[2] );
	if( format.mSwizzleMask[3] != GL_ALPHA )
		glTexParameteri( mTarget, GL_TEXTURE_SWIZZLE_A, format.mSwizzleMask[3] );
#endif
	mSwizzleMask = format.mSwizzleMask;

	// Compare Mode and Func for Depth Texture
#if defined( CINDER_GL_ES_2 )
  #if defined( CINDER_GL_HAS_SHADOW_SAMPLERS )
	if( format.mCompareMode > -1 ) {
		if( supportsShadowSampler() ) {
			glTexParameteri( mTarget, GL_TEXTURE_COMPARE_MODE, format.mCompareMode );
		}
		else {
			CI_LOG_E("This device doesn't support GL_TEXTURE_COMPARE_MODE from EXT_shadow_samplers");
		}
	}
	if( format.mCompareFunc > -1 ) {
		if( supportsShadowSampler() ) {
			glTexParameteri( mTarget, GL_TEXTURE_COMPARE_FUNC, format.mCompareFunc );
		}
		else {
			CI_LOG_E("This device doesn't support GL_TEXTURE_COMPARE_FUNC from EXT_shadow_samplers");
		}
	}
  #endif
#else
	if( format.mCompareMode > -1 ) {
		glTexParameteri( mTarget, GL_TEXTURE_COMPARE_MODE, format.mCompareMode );
	}
	if( format.mCompareFunc > -1 ) {
		glTexParameteri( mTarget, GL_TEXTURE_COMPARE_FUNC, format.mCompareFunc );
	}
#endif

	mMipmapping = format.mMipmapping;
	if( mMipmapping ) {
		mBaseMipmapLevel = format.mBaseMipmapLevel;
		mMaxMipmapLevel = format.mMaxMipmapLevel;
#if ! defined( CINDER_GL_ES_2 )
		if( mBaseMipmapLevel != 0 )
			glTexParameteri( mTarget, GL_TEXTURE_BASE_LEVEL, mBaseMipmapLevel );
#endif
	}
	else {
		mBaseMipmapLevel = 0;
		mMaxMipmapLevel = 0;
	}

#if ! defined( CINDER_GL_ES )
	if( format.mBorderSpecified ) {
		glTexParameterfv( mTarget, GL_TEXTURE_BORDER_COLOR, format.mBorderColor.data() );
	}
#endif

	if( ! format.mLabel.empty() )
		setLabel( format.mLabel );
}

GLint TextureBase::getInternalFormat() const
{
#if ! defined( CINDER_GL_ES )
	if( mInternalFormat == -1 ) {
		ScopedTextureBind scopedBind( mTarget, mTextureId );
		glGetTexLevelParameteriv( mTarget, 0, GL_TEXTURE_INTERNAL_FORMAT, &mInternalFormat );
	}
#endif
	return mInternalFormat;
}

void TextureBase::getInternalFormatInfo( GLint internalFormat, GLenum *outDataFormat, GLenum *outDataType, bool *outAlpha, bool *outCompressed, bool *outSrgb )
{
	bool sRgb = false;
	GLenum dataFormat;
	GLenum dataType;

	switch( internalFormat ) {
#if defined( CINDER_GL_ES_2 )
		case GL_RGB8_OES:				dataFormat = GL_RGB;				dataType = GL_UNSIGNED_BYTE;			break;
		case GL_RGBA8_OES:				dataFormat = GL_RGBA;				dataType = GL_UNSIGNED_BYTE;			break;
	#if defined( CINDER_GL_HAS_TEXTURE_STORAGE )
		case GL_ALPHA8_EXT:				dataFormat = GL_ALPHA;				dataType = GL_UNSIGNED_BYTE;			break;
		case GL_LUMINANCE8_EXT:			dataFormat = GL_LUMINANCE;			dataType = GL_UNSIGNED_BYTE;			break;
		case GL_LUMINANCE8_ALPHA8_EXT:	dataFormat = GL_LUMINANCE_ALPHA;	dataType = GL_UNSIGNED_BYTE;			break;
		case GL_RGBA32F_EXT:			dataFormat = GL_RGBA;				dataType = GL_FLOAT;					break;
		case GL_RGB32F_EXT:				dataFormat = GL_RGB;				dataType = GL_FLOAT;					break;
		case GL_ALPHA32F_EXT:			dataFormat = GL_ALPHA;				dataType = GL_FLOAT;					break;
		case GL_LUMINANCE32F_EXT:		dataFormat = GL_LUMINANCE;			dataType = GL_FLOAT;					break;
		case GL_LUMINANCE_ALPHA32F_EXT:	dataFormat = GL_LUMINANCE_ALPHA;	dataType = GL_FLOAT;					break;
		case GL_ALPHA16F_EXT:			dataFormat = GL_ALPHA;				dataType = GL_HALF_FLOAT_OES;			break;
		case GL_LUMINANCE16F_EXT:		dataFormat = GL_LUMINANCE;			dataType = GL_HALF_FLOAT_OES;			break;
		case GL_LUMINANCE_ALPHA16F_EXT:	dataFormat = GL_LUMINANCE_ALPHA;	dataType = GL_HALF_FLOAT_OES;			break;
		case GL_BGRA8_EXT:				dataFormat = GL_LUMINANCE_ALPHA;	dataType = GL_HALF_FLOAT_OES;			break;
		case GL_R32F_EXT:				dataFormat = GL_RED;				dataType = GL_FLOAT;					break;
	#endif

	#if defined( CINDER_GL_HAS_COLOR_BUFFER_HALF_FLOAT )
		case GL_RGBA16F_EXT:			dataFormat = GL_RGBA;				dataType = GL_HALF_FLOAT_OES;			break;
		case GL_RGB16F_EXT:				dataFormat = GL_RGB;				dataType = GL_HALF_FLOAT_OES;			break;
		case GL_RG16F_EXT:				dataFormat = GL_RG;					dataType = GL_HALF_FLOAT_OES;			break;
		case GL_R16F_EXT:				dataFormat = GL_RED;				dataType = GL_HALF_FLOAT_OES;			break;
	#endif
#else
		case GL_R8:				dataFormat = GL_RED;			dataType = GL_UNSIGNED_BYTE;					break;
		case GL_R8_SNORM:		dataFormat = GL_RED;			dataType = GL_BYTE;								break;
		case GL_R16F:			dataFormat = GL_RED;			dataType = GL_HALF_FLOAT;						break;
		case GL_R32F:			dataFormat = GL_RED;			dataType = GL_FLOAT;							break;
		case GL_R8UI:			dataFormat = GL_RED_INTEGER;	dataType = GL_UNSIGNED_BYTE;					break;
		case GL_R8I:			dataFormat = GL_RED_INTEGER;	dataType = GL_BYTE;								break;
		case GL_R16UI:			dataFormat = GL_RED_INTEGER;	dataType = GL_UNSIGNED_SHORT;					break;
		case GL_R16I:			dataFormat = GL_RED_INTEGER;	dataType = GL_SHORT;							break;
		case GL_R32UI:			dataFormat = GL_RED_INTEGER;	dataType = GL_UNSIGNED_INT;						break;
		case GL_R32I:			dataFormat = GL_RED_INTEGER;	dataType = GL_INT;								break;
		case GL_RG8:			dataFormat = GL_RG;				dataType = GL_UNSIGNED_BYTE;					break;
		case GL_RG8_SNORM:		dataFormat = GL_RG;				dataType = GL_BYTE;								break;
		case GL_RG16F:			dataFormat = GL_RG;				dataType = GL_HALF_FLOAT;						break;
		case GL_RG32F:			dataFormat = GL_RG;				dataType = GL_FLOAT;							break;
		case GL_RG8UI:			dataFormat = GL_RG_INTEGER;		dataType = GL_UNSIGNED_BYTE;					break;
		case GL_RG8I:			dataFormat = GL_RG_INTEGER;		dataType = GL_BYTE;								break;
		case GL_RG16UI:			dataFormat = GL_RG_INTEGER;		dataType = GL_UNSIGNED_SHORT;					break;
		case GL_RG16I:			dataFormat = GL_RG_INTEGER;		dataType = GL_SHORT;							break;
		case GL_RG32UI:			dataFormat = GL_RG_INTEGER;		dataType = GL_UNSIGNED_INT;						break;
		case GL_RG32I:			dataFormat = GL_RG_INTEGER;		dataType = GL_INT;								break;
		case GL_RGB8:			dataFormat = GL_RGB;			dataType = GL_UNSIGNED_BYTE;					break;
		case GL_SRGB8:			dataFormat = GL_RGB;			dataType = GL_UNSIGNED_BYTE;	sRgb = true;	break;
		case GL_RGB8_SNORM:		dataFormat = GL_RGB;			dataType = GL_BYTE;								break;
		case GL_R11F_G11F_B10F: dataFormat = GL_RGB;			dataType = GL_UNSIGNED_INT_10F_11F_11F_REV;		break;
		case GL_RGB9_E5:		dataFormat = GL_RGB;			dataType = GL_UNSIGNED_INT_5_9_9_9_REV;			break;
		case GL_RGB16F:			dataFormat = GL_RGB;			dataType = GL_HALF_FLOAT;						break;
		case GL_RGB32F:			dataFormat = GL_RGB;			dataType = GL_FLOAT;							break;
		case GL_RGB8UI:			dataFormat = GL_RGB_INTEGER;	dataType = GL_UNSIGNED_BYTE;					break;
		case GL_RGB8I:			dataFormat = GL_RGB_INTEGER;	dataType = GL_BYTE;								break;
		case GL_RGB16UI:		dataFormat = GL_RGB_INTEGER;	dataType = GL_UNSIGNED_SHORT;					break;
		case GL_RGB16I:			dataFormat = GL_RGB_INTEGER;	dataType = GL_SHORT;							break;
		case GL_RGB32UI:		dataFormat = GL_RGB_INTEGER;	dataType = GL_UNSIGNED_INT;						break;
		case GL_RGB32I:			dataFormat = GL_RGB_INTEGER;	dataType = GL_INT;								break;
		case GL_RGBA8:			dataFormat = GL_RGBA;			dataType = GL_UNSIGNED_BYTE;					break;
		case GL_SRGB8_ALPHA8:	dataFormat = GL_RGBA;			dataType = GL_UNSIGNED_BYTE;	sRgb = true;	break;
		case GL_RGBA8_SNORM:	dataFormat = GL_RGBA;			dataType = GL_BYTE;								break;
		case GL_RGB10_A2:		dataFormat = GL_RGBA;			dataType = GL_UNSIGNED_INT_2_10_10_10_REV;		break;
		case GL_RGBA16F:		dataFormat = GL_RGBA;			dataType = GL_HALF_FLOAT;						break;
		case GL_RGBA32F:		dataFormat = GL_RGBA;			dataType = GL_FLOAT;							break;
		case GL_RGBA8UI:		dataFormat = GL_RGBA_INTEGER;	dataType = GL_UNSIGNED_BYTE;					break;
		case GL_RGBA8I:			dataFormat = GL_RGBA_INTEGER;	dataType = GL_BYTE;								break;
		case GL_RGB10_A2UI:		dataFormat = GL_RGBA_INTEGER;	dataType = GL_UNSIGNED_INT_2_10_10_10_REV;		break;
		case GL_RGBA16UI:		dataFormat = GL_RGBA_INTEGER;	dataType = GL_UNSIGNED_SHORT;					break;
		case GL_RGBA16I:		dataFormat = GL_RGBA_INTEGER;	dataType = GL_SHORT;							break;
		case GL_RGBA32I:		dataFormat = GL_RGBA_INTEGER;	dataType = GL_INT;								break;
		case GL_RGBA32UI:		dataFormat = GL_RGBA_INTEGER;	dataType = GL_UNSIGNED_INT;						break;
#endif

		case GL_RGB5_A1:				dataFormat = GL_RGBA;				dataType = GL_UNSIGNED_BYTE;			break;
		case GL_RGBA4:					dataFormat = GL_RGBA;				dataType = GL_UNSIGNED_BYTE;			break;
		case GL_RGB565:					dataFormat = GL_RGB;				dataType = GL_UNSIGNED_BYTE;			break;

		// UNSIZED FORMATS
		case GL_RGB:					dataFormat = GL_RGB;				dataType = GL_UNSIGNED_BYTE;			break;
		case GL_RGBA:					dataFormat = GL_RGBA;				dataType = GL_UNSIGNED_BYTE;			break;

#if defined( CINDER_GL_ES )
		case GL_LUMINANCE_ALPHA:		dataFormat = GL_LUMINANCE_ALPHA;	dataType = GL_UNSIGNED_BYTE;			break;
		case GL_LUMINANCE:				dataFormat = GL_LUMINANCE;			dataType = GL_UNSIGNED_BYTE;			break;
#endif // ! CINDER_GL_ES

		case GL_ALPHA:					dataFormat = GL_ALPHA;				dataType = GL_UNSIGNED_BYTE;			break;

#if ! defined( CINDER_GL_ES )
		case GL_DEPTH_COMPONENT:		dataFormat = GL_DEPTH_COMPONENT;	dataType = GL_UNSIGNED_INT;				break;
		case GL_DEPTH_STENCIL:			dataFormat = GL_DEPTH_STENCIL;		dataType = GL_UNSIGNED_INT_24_8;		break;
		case GL_RED:					dataFormat = GL_RED;				dataType = GL_UNSIGNED_BYTE;			break;
		case GL_RG:						dataFormat = GL_RG;					dataType = GL_UNSIGNED_BYTE;			break;
		case GL_R16:					dataFormat = GL_RED;				dataType = GL_UNSIGNED_SHORT;			break;
		case GL_RG16:					dataFormat = GL_RG;					dataType = GL_UNSIGNED_SHORT;			break;
		case GL_RGB16:					dataFormat = GL_RGB;				dataType = GL_UNSIGNED_SHORT;			break;
		case GL_RGBA16:					dataFormat = GL_RGBA;				dataType = GL_UNSIGNED_SHORT;			break;
		case GL_R16_SNORM:				dataFormat = GL_RED;				dataType = GL_SHORT;					break;
		case GL_RG16_SNORM:				dataFormat = GL_RG;					dataType = GL_SHORT;					break;
#endif // ! CINDER_GL_ES

		// SIZED DEPTH FORMATS
		case GL_DEPTH_COMPONENT16:	dataFormat = GL_DEPTH_COMPONENT; dataType = GL_UNSIGNED_SHORT;				break;
#if ! defined( CINDER_GL_ES_2 )
		case GL_DEPTH_COMPONENT24:	dataFormat = GL_DEPTH_COMPONENT; dataType = GL_UNSIGNED_INT;					break;
		case GL_DEPTH_COMPONENT32F:	dataFormat = GL_DEPTH_COMPONENT; dataType = GL_FLOAT;							break;
		case GL_DEPTH24_STENCIL8:	dataFormat = GL_DEPTH_STENCIL;	dataType = GL_UNSIGNED_INT_24_8;				break;
		case GL_DEPTH32F_STENCIL8:	dataFormat = GL_DEPTH_STENCIL;	dataType = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;	break;
#endif
		// COMPRESSED FORMATS
#if ! defined( CINDER_GL_ES ) || defined( CINDER_GL_ANGLE )
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:				dataFormat = GL_RGB;	dataType = 0;					break;
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:				dataFormat = GL_RGBA;	dataType = 0;					break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:				dataFormat = GL_RGBA;	dataType = 0;					break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:				dataFormat = GL_RGBA;	dataType = 0;					break;
#endif

#if defined( CINDER_GL_ES ) && ! defined( CINDER_GL_ANGLE )
		case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:			dataFormat = GL_RGB;	dataType = 0;					break;
		case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:			dataFormat = GL_RGB;	dataType = 0;					break;
		case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:			dataFormat = GL_RGBA;	dataType = 0;					break;
		case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:			dataFormat = GL_RGBA;	dataType = 0;					break;
#endif

#if ! defined( CINDER_GL_ES_2 )
		case GL_COMPRESSED_RGB8_ETC2:						dataFormat = GL_RGB;	dataType = 0;					break;
		case GL_COMPRESSED_SRGB8_ETC2:						dataFormat = GL_RGB;	dataType = 0; sRgb = true;		break;
		case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:	dataFormat = GL_RGBA;	dataType = 0;					break;
		case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:	dataFormat = GL_RGBA;	dataType = 0; sRgb = true;		break;
		case GL_COMPRESSED_RGBA8_ETC2_EAC:					dataFormat = GL_RGBA;	dataType = 0;					break;
		case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:			dataFormat = GL_RGBA;	dataType = 0; sRgb = true;		break;
		case GL_COMPRESSED_R11_EAC:							dataFormat = GL_RED;	dataType = 0;					break;
		case GL_COMPRESSED_SIGNED_R11_EAC:					dataFormat = GL_RED;	dataType = 0;					break;
		case GL_COMPRESSED_RG11_EAC:						dataFormat = GL_RG;		dataType = 0;					break;
		case GL_COMPRESSED_SIGNED_RG11_EAC:					dataFormat = GL_RG;		dataType = 0;					break;
#endif

		default:
			CI_LOG_W( "Unknown internalFormat:" << gl::constantToString( internalFormat ) );
			// Defaulting to GL_RGBA and GL_UNSIGNED_BYTE can cause the wrong texture storage
			// allocation to happen for the wrong data type. This leads to FBOs having
			// incomplete attachments on Android and Linux.
			dataFormat = GL_INVALID_ENUM; //GL_RGBA;
			dataType = GL_INVALID_ENUM; //GL_UNSIGNED_BYTE;
			break;
	}

	if( outAlpha )
#if defined( CINDER_GL_ES )
		*outAlpha = ( dataFormat == GL_RGBA ) || ( dataFormat == GL_ALPHA ) || ( dataFormat == GL_LUMINANCE_ALPHA );
#else
		*outAlpha = ( dataFormat == GL_RGBA ) || ( dataFormat == GL_ALPHA ) || ( dataFormat == GL_RGBA_INTEGER );
#endif

	if( outCompressed )
		*outCompressed = ( dataType == 0 );
	if( outSrgb )
		*outSrgb = sRgb;
	if( outDataFormat )
		*outDataFormat = dataFormat;
	if( outDataType )
		*outDataType = dataType;
}

void TextureBase::bind( uint8_t textureUnit ) const
{
	ScopedActiveTexture ScopedActiveTexture( textureUnit );
	gl::context()->bindTexture( mTarget, mTextureId );
}

void TextureBase::unbind( uint8_t textureUnit ) const
{
	ScopedActiveTexture ScopedActiveTexture( textureUnit );
	gl::context()->bindTexture( mTarget, 0 );
}

bool TextureBase::hasAlpha() const
{
	GLint internalFormat = getInternalFormat();
	bool resultAlpha;
	getInternalFormatInfo( internalFormat, nullptr, nullptr, &resultAlpha );
	return resultAlpha;
}

// Returns the appropriate parameter to glGetIntegerv() for a specific target; ie GL_TEXTURE_2D -> GL_TEXTURE_BINDING_2D
GLenum TextureBase::getBindingConstantForTarget( GLenum target )
{
	switch( target ) {
		case GL_TEXTURE_2D:
			return GL_TEXTURE_BINDING_2D;
		break;
		case GL_TEXTURE_CUBE_MAP:
			return GL_TEXTURE_BINDING_CUBE_MAP;
		break;
#if ! defined( CINDER_GL_ES )
		case GL_TEXTURE_RECTANGLE: // equivalent to GL_TEXTURE_RECTANGLE_ARB
			return GL_TEXTURE_BINDING_RECTANGLE;
		break;
		case GL_TEXTURE_1D:
			return GL_TEXTURE_BINDING_1D;
		break;
		case GL_TEXTURE_3D:
			return GL_TEXTURE_BINDING_3D;
		break;
		case GL_TEXTURE_2D_ARRAY:
			return GL_TEXTURE_BINDING_2D_ARRAY;
		break;
		case GL_TEXTURE_1D_ARRAY:
			return GL_TEXTURE_BINDING_1D_ARRAY;
		break;
		case GL_TEXTURE_CUBE_MAP_ARRAY:
			return GL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
		break;
		case GL_TEXTURE_BUFFER:
			return GL_TEXTURE_BINDING_BUFFER;
		break;
		case GL_TEXTURE_2D_MULTISAMPLE:
			return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
		break;
		case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			return GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;
		break;
#endif
		default:
			return 0;
	}
}

void TextureBase::setWrapS( GLenum wrapS )
{
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexParameteri( mTarget, GL_TEXTURE_WRAP_S, wrapS );
}

void TextureBase::setWrapT( GLenum wrapT )
{
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexParameteri( mTarget, GL_TEXTURE_WRAP_T, wrapT );
}

#if ! defined( CINDER_GL_ES )
void TextureBase::setWrapR( GLenum wrapR )
{
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexParameteri( mTarget, GL_TEXTURE_WRAP_R, wrapR );
}
#endif

void TextureBase::setMinFilter( GLenum minFilter )
{
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexParameteri( mTarget, GL_TEXTURE_MIN_FILTER, minFilter );
}

void TextureBase::setMagFilter( GLenum magFilter )
{
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexParameteri( mTarget, GL_TEXTURE_MAG_FILTER, magFilter );
}

void TextureBase::setMaxAnisotropy( GLfloat maxAnisotropy )
{
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexParameterf( mTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy );
}

void TextureBase::setCompareMode( GLenum compareMode )
{
#if defined( CINDER_GL_ES_2 )
  #if defined( CINDER_GL_HAS_SHADOW_SAMPLERS )
	if( supportsShadowSampler() ) {
		ScopedTextureBind tbs( mTarget, mTextureId );
		glTexParameteri( mTarget, GL_TEXTURE_COMPARE_MODE, compareMode );
	}
	else {
		CI_LOG_E("This device doesn't support GL_TEXTURE_COMPARE_MODE from EXT_shadow_samplers");
	}
  #endif
#else
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexParameteri( mTarget, GL_TEXTURE_COMPARE_MODE, compareMode );
#endif
}

void TextureBase::setCompareFunc( GLenum compareFunc )
{
#if defined( CINDER_GL_ES_2 )
  #if defined( CINDER_GL_HAS_SHADOW_SAMPLERS )
	if( supportsShadowSampler() ) {
		ScopedTextureBind tbs( mTarget, mTextureId );
		glTexParameteri( mTarget, GL_TEXTURE_COMPARE_FUNC, compareFunc );
	}
	else {
		CI_LOG_E("This device doesn't support GL_TEXTURE_COMPARE_FUNC from EXT_shadow_samplers");
	}
  #endif
#else
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexParameteri( mTarget, GL_TEXTURE_COMPARE_FUNC, compareFunc );
#endif
}

template<typename T>
bool TextureBase::surfaceRequiresIntermediate( int32_t width, uint8_t pixelBytes, ptrdiff_t rowBytes, SurfaceChannelOrder surfaceChannelOrder )
{
	if( width * pixelBytes != rowBytes )
		return true;

	if( std::is_same<T,uint8_t>::value ) { // 8-bit
		switch( surfaceChannelOrder.getCode() ) {
			case SurfaceChannelOrder::RGB:
			case SurfaceChannelOrder::RGBA:
			case SurfaceChannelOrder::RGBX:
			case SurfaceChannelOrder::BGRA:
			case SurfaceChannelOrder::BGRX:
				return false;
			break;
			default:
				return true;
		}
	}
	else { // for 16/32-bit
		switch( surfaceChannelOrder.getCode() ) {
			case SurfaceChannelOrder::RGB:
			case SurfaceChannelOrder::RGBA:
				return false;
			break;
			default:
				return true;
		}
	}
}

template<typename T>
void TextureBase::SurfaceChannelOrderToDataFormatAndType( const SurfaceChannelOrder &sco, GLint *dataFormat, GLenum *type )
{
	if( std::is_same<T,uint8_t>::value ) {
		switch( sco.getCode() ) {
			case SurfaceChannelOrder::RGB:
				*dataFormat = GL_RGB;
				*type = GL_UNSIGNED_BYTE;
			break;
			case SurfaceChannelOrder::RGBA:
			case SurfaceChannelOrder::RGBX:
				*dataFormat = GL_RGBA;
				*type = GL_UNSIGNED_BYTE;
			break;
			case SurfaceChannelOrder::BGRA:
			case SurfaceChannelOrder::BGRX:
#if defined( CINDER_GL_ES )
				*dataFormat = GL_BGRA_EXT;
#else
				*dataFormat = GL_BGRA;
#endif
				*type = GL_UNSIGNED_BYTE;
			break;
			default:
				throw TextureDataExc( "Invalid channel order" ); // this is an unsupported channel order for a texture
			break;
		}
	}
	else if( std::is_same<T,uint16_t>::value ) { // 16-bit
		switch( sco.getCode() ) {
			case SurfaceChannelOrder::RGB:
				*dataFormat = GL_RGB;
				*type = GL_UNSIGNED_SHORT;
			break;
			case SurfaceChannelOrder::RGBA:
				*dataFormat = GL_RGBA;
				*type = GL_UNSIGNED_SHORT;
			break;
			default:
				throw TextureDataExc( "Invalid channel order" ); // this is an unsupported channel order for a texture
			break;
		}
	}
	else if( std::is_same<T,float>::value ) { // 32-bit float
		switch( sco.getCode() ) {
			case SurfaceChannelOrder::RGB:
				*dataFormat = GL_RGB;
				*type = GL_FLOAT;
			break;
			case SurfaceChannelOrder::RGBA:
				*dataFormat = GL_RGBA;
				*type = GL_FLOAT;
			break;
			default:
				throw TextureDataExc( "Invalid channel order" ); // this is an unsupported channel order for a texture
			break;
		}
	}
}

ivec2 TextureBase::calcMipLevelSize( int mipLevel, GLint width, GLint height )
{
	width = std::max<int>( 1, width >> mipLevel );
	height = std::max<int>( 1, height >> mipLevel );

	return ivec2( width, height );
}

int TextureBase::requiredMipLevels( GLint width, GLint height, GLint depth )
{
	int maxDim = std::max( width, std::max( height, depth ) );
	return static_cast<int>( floor( std::log2( maxDim ) ) )+ 1;
}

GLfloat TextureBase::getMaxAnisotropyMax()
{
	GLfloat maxAnisotropyMax;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropyMax );
	return maxAnisotropyMax;
}

bool TextureBase::supportsHardwareSwizzle()
{
#if defined( CINDER_GL_ES)
  #if CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3
	return true;
  #else
	return false;
  #endif
#else
	static bool supported = ( ( gl::isExtensionAvailable( "GL_EXT_texture_swizzle" ) || gl::getVersion() >= make_pair( 3, 3 ) ) );
	return supported;
#endif
}

#if defined( CINDER_GL_ES_2 )
bool TextureBase::supportsShadowSampler()
{
	static bool supported = gl::isExtensionAvailable( "GL_EXT_shadow_samplers" );
	return supported;
}
#endif

void TextureBase::setLabel( const std::string &label )
{
	mLabel = label;
	env()->objectLabel( GL_TEXTURE, mTextureId, (GLsizei)label.size(), label.c_str() );
}

/////////////////////////////////////////////////////////////////////////////////
// TextureBase::Format
TextureBase::Format::Format()
{
	mTarget = GL_TEXTURE_2D;
	mWrapS = GL_CLAMP_TO_EDGE;
	mWrapT = GL_CLAMP_TO_EDGE;
	mWrapR = GL_CLAMP_TO_EDGE;
	mMinFilterSpecified = false;
	mMinFilter = GL_LINEAR;
	mMagFilter = GL_LINEAR;
	mMipmapping = false;
	mMipmappingSpecified = false;
	mBorderSpecified = false;
	mBaseMipmapLevel = 0;
	mMaxMipmapLevel = -1; // auto-calculate
	mImmutableStorage = false;
	mInternalFormat = -1;
	mDataType = -1;
	mMaxAnisotropy = -1.0f;
	mSwizzleSpecified = false;
	mSwizzleMask[0] = GL_RED; mSwizzleMask[1] = GL_GREEN; mSwizzleMask[2] = GL_BLUE; mSwizzleMask[3] = GL_ALPHA;
	mCompareMode = -1;
	mCompareFunc = -1;
#if ! defined( CINDER_GL_ES )
	mPerGpuStorageSpecifiedNV = false;
	mPerGpuStorageEnabledNV = false;
#endif
}

void TextureBase::Format::setSwizzleMask( GLint r, GLint g, GLint b, GLint a )
{
	array<GLint,4> swizzleMask;
	swizzleMask[0] = r; swizzleMask[1] = g; swizzleMask[2] = b; swizzleMask[3] = a;
	setSwizzleMask( swizzleMask );
	mSwizzleSpecified = true;
}

void TextureBase::Format::setBorderColor( const ColorA &color )
{
	array<GLfloat,4> border;
	border[0] = color.r;
	border[1] = color.g;
	border[2] = color.b;
	border[3] = color.a;
	setBorderColor( border );
}

#if ! defined( CINDER_GL_ES )
/////////////////////////////////////////////////////////////////////////////////
// Texture1d
Texture1dRef Texture1d::create( GLint width, const Format &format )
{
	if( format.mDeleter )
		return Texture1dRef( new Texture1d( width, format ), format.mDeleter );
	else
		return Texture1dRef( new Texture1d( width, format ) );
}

Texture1dRef Texture1d::create( const Surface8u &surface, const Format &format )
{
	if( format.mDeleter )
		return Texture1dRef( new Texture1d( surface, format ), format.mDeleter );
	else
		return Texture1dRef( new Texture1d( surface, format ) );
}

Texture1dRef Texture1d::create( const void *data, GLenum dataFormat, int width, const Format &format )
{
	if( format.mDeleter )
		return Texture1dRef( new Texture1d( data, dataFormat, width, format ), format.mDeleter );
	else
		return Texture1dRef( new Texture1d( data, dataFormat, width, format ) );
}

Texture1d::Texture1d( GLint width, Format format )
	: mWidth( width )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	TextureBase::initParams( format, GL_RGB, GL_UNSIGNED_BYTE );

	ScopedTextureBind tbs( mTarget, mTextureId );
	env()->allocateTexStorage1d( mTarget, mMaxMipmapLevel + 1, mInternalFormat, mWidth, format.isImmutableStorage(), format.getDataType() );
}

Texture1d::Texture1d( const Surface8u &surface, Format format )
	: mWidth( surface.getWidth() )
{
	GLint dataFormat;
	GLenum type;
	SurfaceChannelOrderToDataFormatAndType<uint8_t>( surface.getChannelOrder(), &dataFormat, &type );

	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, surface.hasAlpha() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE );

	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexImage1D( mTarget, 0, mInternalFormat, mWidth, 0, dataFormat, GL_UNSIGNED_BYTE, surface.getData() );
}

Texture1d::Texture1d( const void *data, GLenum dataFormat, int width, Format format )
	: mWidth( width )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	TextureBase::initParams( format, GL_RGB, GL_UNSIGNED_BYTE );

	glTexImage1D( mTarget, 0, mInternalFormat, mWidth, 0, dataFormat, format.getDataType(), data );
}

void Texture1d::update( const Surface8u &surface, int mipLevel )
{
	GLint dataFormat;
	GLenum type;
	SurfaceChannelOrderToDataFormatAndType<uint8_t>( surface.getChannelOrder(), &dataFormat, &type );

	ivec2 mipMapSize = calcMipLevelSize( mipLevel, getWidth(), getHeight() );
	if( surface.getSize() != mipMapSize )
		throw TextureResizeExc( "Invalid Texture1d::update() surface dimensions", surface.getSize(), mipMapSize );

	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexSubImage1D( mTarget, mipLevel, 0, // offsets
				mipMapSize.x, dataFormat, type, surface.getData() );
}

void Texture1d::update( const void *data, GLenum dataFormat, GLenum dataType, int mipLevel, int width, int offset )
{
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexSubImage1D( mTarget, mipLevel, offset, width, dataFormat, dataType, data );
}

void Texture1d::printDims( std::ostream &os ) const
{
	os << mWidth;
}

#endif // ! defined( CINDER_GL_ES )

/////////////////////////////////////////////////////////////////////////////////
// Texture2d
Texture2dRef Texture2d::create( int width, int height, const Format &format )
{
	if( format.mDeleter )
		return TextureRef( new Texture( width, height, format ), format.mDeleter );
	else
		return TextureRef( new Texture( width, height, format ) );
}

Texture2dRef Texture2d::create( const void *data, GLenum dataFormat, int width, int height, const Format &format )
{
	if( format.mDeleter )
		return TextureRef( new Texture( data, dataFormat, width, height, format ), format.mDeleter );
	else
		return TextureRef( new Texture( data, dataFormat, width, height, format ) );
}

Texture2dRef Texture2d::create( const Surface8u &surface, const Format &format )
{
	if( format.mDeleter )
		return TextureRef( new Texture( surface, format ), format.mDeleter );
	else
		return TextureRef( new Texture( surface, format ) );
}

Texture2dRef Texture2d::create( const Surface16u &surface, const Format &format )
{
	if( format.mDeleter )
		return TextureRef( new Texture( surface, format ), format.mDeleter );
	else
		return TextureRef( new Texture( surface, format ) );
}

Texture2dRef Texture2d::create( const Surface32f &surface, const Format &format )
{
	if( format.mDeleter )
		return TextureRef( new Texture( surface, format ), format.mDeleter );
	else
		return TextureRef( new Texture( surface, format ) );
}

Texture2dRef Texture2d::create( const Channel8u &channel, const Format &format )
{
	if( format.mDeleter )
		return TextureRef( new Texture( channel, format ), format.mDeleter );
	else
		return TextureRef( new Texture( channel, format ) );
}

Texture2dRef Texture2d::create( const Channel16u &channel, const Format &format )
{
	if( format.mDeleter )
		return TextureRef( new Texture( channel, format ), format.mDeleter );
	else
		return TextureRef( new Texture( channel, format ) );
}

Texture2dRef Texture2d::create( const Channel32f &channel, const Format &format )
{
	if( format.mDeleter )
		return TextureRef( new Texture( channel, format ), format.mDeleter );
	else
		return TextureRef( new Texture( channel, format ) );
}

Texture2dRef Texture2d::create( ImageSourceRef imageSource, const Format &format )
{
	if( format.mDeleter )
		return TextureRef( new Texture( imageSource, format ), format.mDeleter );
	else
		return TextureRef( new Texture( imageSource, format ) );
}

Texture2dRef Texture2d::create( GLenum target, GLuint textureID, int width, int height, bool doNotDispose, const std::function<void(Texture2d*)> &deleter )
{
	if( deleter )
		return TextureRef( new Texture( target, textureID, width, height, doNotDispose ), deleter );
	else
		return TextureRef( new Texture( target, textureID, width, height, doNotDispose ) );
}

Texture2dRef Texture2d::create( const TextureData &data, const Format &format )
{
	if( format.mDeleter )
		return TextureRef( new Texture( data, format ), format.mDeleter );
	else
		return TextureRef( new Texture( data, format ) );
}

void Texture2d::initMaxMipmapLevel()
{
	if( mMaxMipmapLevel == -1 )
		mMaxMipmapLevel = requiredMipLevels( mActualSize.x, mActualSize.y, 1 ) - 1;
#if ! defined( CINDER_GL_ES_2 )
	glTexParameteri( mTarget, GL_TEXTURE_MAX_LEVEL, mMaxMipmapLevel );
#endif
}

Texture2d::Texture2d( int width, int height, Format format )
	: mActualSize( width, height ),
	mCleanBounds( 0, 0, width, height ),
	mTopDown( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
#if ! defined( CINDER_GL_ES_2 )
	initParams( format, GL_RGBA8, GL_UNSIGNED_BYTE );
#else
	initParams( format, GL_RGBA, GL_UNSIGNED_BYTE );
#endif

	if( mMipmapping )
		initMaxMipmapLevel();

	env()->allocateTexStorage2d( mTarget, mMaxMipmapLevel + 1, mInternalFormat, width, height, format.isImmutableStorage(), format.getDataType() );
}

Texture2d::Texture2d( const void *data, GLenum dataFormat, int width, int height, Format format )
	: mActualSize( width, height ),
	mCleanBounds( 0, 0, width, height ),
	mTopDown( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, GL_RGBA, GL_UNSIGNED_BYTE );

	if( format.mLoadTopDown )
		CI_LOG_W( "Ignoring request for loadTopDown" );

	initData( data, dataFormat, format );
}

Texture2d::Texture2d( const Surface8u &surface, Format format )
	: mActualSize( surface.getSize() ),
	mCleanBounds( 0, 0, surface.getWidth(), surface.getHeight() ),
	mTopDown( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, surface.hasAlpha() ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE );
	setData<uint8_t>( surface, true, 0, ivec2( 0, 0 ) );
}

Texture2d::Texture2d( const Channel8u &channel, Format format )
	: mActualSize( channel.getSize() ),
	mCleanBounds( 0, 0, channel.getWidth(), channel.getHeight() ),
	mTopDown( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
#if defined( CINDER_GL_ES )
	initParams( format, GL_LUMINANCE, GL_UNSIGNED_BYTE );
#else
	if( ! format.mSwizzleSpecified ) {
		std::array<int,4> swizzleMask = { GL_RED, GL_RED, GL_RED, GL_ONE };
		format.setSwizzleMask( swizzleMask );
	}
	initParams( format, GL_RED, GL_UNSIGNED_BYTE );
#endif
	setData<uint8_t>( channel, true, 0, ivec2( 0, 0 ) );
}

Texture2d::Texture2d( const Surface16u &surface, Format format )
	: mActualSize( surface.getSize() ),
	mCleanBounds( 0, 0, surface.getWidth(), surface.getHeight() ),
	mTopDown( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
#if defined( CINDER_GL_ES )
	initParams( format, surface.hasAlpha() ? GL_RGBA : GL_RGB, GL_UNSIGNED_SHORT );
#else
	initParams( format, surface.hasAlpha() ? GL_RGBA : GL_RGB, GL_UNSIGNED_SHORT );
#endif

	setData<uint16_t>( surface, true, 0, ivec2( 0, 0 ) );
}

Texture2d::Texture2d( const Channel16u &channel, Format format )
	: mActualSize( channel.getSize() ),
	mCleanBounds( 0, 0, channel.getWidth(), channel.getHeight() ),
	mTopDown( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
#if defined( CINDER_GL_ES )
	initParams( format, GL_LUMINANCE, GL_UNSIGNED_SHORT );
#else
	if( ! format.mSwizzleSpecified ) {
		std::array<int,4> swizzleMask = { GL_RED, GL_RED, GL_RED, GL_ONE };
		format.setSwizzleMask( swizzleMask );
	}
	initParams( format, GL_RED, GL_UNSIGNED_SHORT );
#endif

	setData<uint16_t>( channel, true, 0, ivec2( 0, 0 ) );
}

Texture2d::Texture2d( const Surface32f &surface, Format format )
	: mActualSize( surface.getSize() ),
	mCleanBounds( 0, 0, surface.getWidth(), surface.getHeight() ),
	mTopDown( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
#if defined( CINDER_GL_ES_2 )
	initParams( format, surface.hasAlpha() ? GL_RGBA : GL_RGB, GL_FLOAT );
#else
	initParams( format, surface.hasAlpha() ? GL_RGBA32F : GL_RGB32F, GL_FLOAT );
#endif

	setData<float>( surface, true, 0, ivec2( 0, 0 ) );
}

Texture2d::Texture2d( const Channel32f &channel, Format format )
	: mActualSize( channel.getSize() ),
	mCleanBounds( 0, 0, channel.getWidth(), channel.getHeight() ),
	mTopDown( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
#if defined( CINDER_GL_ES_2 )
	initParams( format, GL_LUMINANCE, GL_FLOAT );
#else
	if( ! format.mSwizzleSpecified ) {
		std::array<int,4> swizzleMask = { GL_RED, GL_RED, GL_RED, GL_ONE };
		format.setSwizzleMask( swizzleMask );
	}
	initParams( format, GL_RED, GL_FLOAT );
#endif

	setData<float>( channel, true, 0, ivec2( 0, 0 ) );
}

Texture2d::Texture2d( const ImageSourceRef &imageSource, Format format )
	: mActualSize( -1, -1 ), mCleanBounds( 0, 0, -1, -1 ),
	mTopDown( false )
{
	GLint defaultInternalFormat;
	// Set the internal format based on the image's color space
	switch( imageSource->getColorModel() ) {
		case ImageIo::CM_RGB:
#if defined( CINDER_GL_ES_2 )
			defaultInternalFormat = ( imageSource->hasAlpha() ) ? GL_RGBA : GL_RGB;
#else
			switch( imageSource->getDataType() ) {
#if ! defined( CINDER_GL_ES )
				case ImageIo::UINT16:
					defaultInternalFormat = ( imageSource->hasAlpha() ) ? GL_RGBA16 : GL_RGB16;
				break;
#endif
				case ImageIo::FLOAT16:
					defaultInternalFormat = ( imageSource->hasAlpha() ) ? GL_RGBA16F : GL_RGB16F;
				break;
				case ImageIo::FLOAT32:
					defaultInternalFormat = ( imageSource->hasAlpha() ) ? GL_RGBA32F : GL_RGB32F;
				break;
				default:
					defaultInternalFormat = ( imageSource->hasAlpha() ) ? GL_RGBA8 : GL_RGB8;
				break;
			}
#endif
		break;
		case ImageIo::CM_GRAY: {
#if defined( CINDER_GL_ES )
			defaultInternalFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE_ALPHA : GL_LUMINANCE;
#else
			switch( imageSource->getDataType() ) {
				case ImageIo::UINT16:
					defaultInternalFormat = ( imageSource->hasAlpha() ) ? GL_RG16 : GL_R16;
				break;
				case ImageIo::FLOAT16:
					defaultInternalFormat = ( imageSource->hasAlpha() ) ? GL_RG16F : GL_R16F;
				break;
				case ImageIo::FLOAT32:
					defaultInternalFormat = ( imageSource->hasAlpha() ) ? GL_RG32F : GL_R32F;
				break;
				default:
					defaultInternalFormat = ( imageSource->hasAlpha() ) ? GL_RG8 : GL_R8;
				break;
			}

			if( ! format.mSwizzleSpecified ) {
				std::array<int,4> swizzleMask = { GL_RED, GL_RED, GL_RED, GL_ONE };
				if( imageSource->hasAlpha() )
					swizzleMask[3] = GL_GREEN;
				format.setSwizzleMask( swizzleMask );
			}
#endif
		}
		break;
		default:
			throw ImageIoExceptionIllegalColorModel( "Unsupported color model for gl::Texture construction." );
		break;
	}

	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, defaultInternalFormat, 0 /* unused */ );
	initData( imageSource, format );
}

Texture2d::Texture2d( GLenum target, GLuint textureId, int width, int height, bool doNotDispose )
	: TextureBase( target, textureId, -1 ), mActualSize( width, height ),
	mCleanBounds( 0, 0, width, height ), mTopDown( false )
{
	mDoNotDispose = doNotDispose;
}

Texture2d::Texture2d( const TextureData &data, Format format )
	: mTopDown( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, 0 /* unused */, 0 /* unused */ );

	replace( data );

	if( mMipmapping && data.getNumLevels() <= 1 ) {
		glGenerateMipmap( mTarget );
	}
}

template<typename T>
void Texture2d::setData( const SurfaceT<T> &original, bool createStorage, int mipLevel, const ivec2 &destOffset )
{
	SurfaceT<T> intermediate;
	bool useIntermediate = false;

	// we need an intermediate format for not top-down, certain channel orders, and rowBytes != numChannels * width
	if( ( ! mTopDown ) || surfaceRequiresIntermediate<T>( original.getWidth(), original.getPixelBytes(), original.getRowBytes(), original.getChannelOrder() ) ) {
		intermediate = SurfaceT<T>( original.getWidth(), original.getHeight(), original.hasAlpha(), original.hasAlpha() ? SurfaceChannelOrder::RGBA : SurfaceChannelOrder::RGB );
		if( mTopDown )
			intermediate.copyFrom( original, original.getBounds() );
		else
			ip::flipVertical( original, &intermediate );
		useIntermediate = true;
	}

	const SurfaceT<T> &source = ( useIntermediate ) ? intermediate : original;

	GLint dataFormat;
	GLenum type;
	SurfaceChannelOrderToDataFormatAndType<T>( source.getChannelOrder(), &dataFormat, &type );

	ScopedTextureBind tbs( mTarget, mTextureId );

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	if( createStorage )
		glTexImage2D( mTarget, 0, mInternalFormat, source.getWidth(), source.getHeight(), 0, dataFormat, type, source.getData() );
    else
		glTexSubImage2D( mTarget, mipLevel, destOffset.x, destOffset.y, source.getWidth(), source.getHeight(), dataFormat, type, source.getData() );

	if( mMipmapping && mipLevel == 0 )
		glGenerateMipmap( mTarget );
}

template<typename T>
void Texture2d::setData( const ChannelT<T> &original, bool createStorage, int mipLevel, const ivec2 &destOffset )
{
	ChannelT<T> intermediate;
	bool useIntermediate = false;

	// we need an intermediate format for not top-down or non-planar
	if( ( ! mTopDown ) || ( ! original.isPlanar() ) ) {
		intermediate = ChannelT<T>( original.getWidth(), original.getHeight() );
		if( mTopDown )
			intermediate.copyFrom( original, original.getBounds() );
		else
			ip::flipVertical( original, &intermediate );
		useIntermediate = true;
	}

	const ChannelT<T> &source = ( useIntermediate ) ? intermediate : original;

	GLenum dataFormat;
	getInternalFormatInfo( mInternalFormat, &dataFormat, nullptr, nullptr, nullptr, nullptr );

	GLenum type = GL_UNSIGNED_BYTE;
	if( std::is_same<uint16_t,T>::value )
		type = GL_UNSIGNED_SHORT;
	else if( std::is_same<float,T>::value )
		type = GL_FLOAT;

	ScopedTextureBind tbs( mTarget, mTextureId );

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	if( createStorage )
		glTexImage2D( mTarget, 0, mInternalFormat, source.getWidth(), source.getHeight(), 0, dataFormat, type, source.getData() );
    else
		glTexSubImage2D( mTarget, mipLevel, destOffset.x, destOffset.y, source.getWidth(), source.getHeight(), dataFormat, type, source.getData() );

	if( mMipmapping && mipLevel == 0 )
		glGenerateMipmap( mTarget );
}

void Texture2d::initData( const void *data, GLenum dataFormat, const Format &format )
{
	ScopedTextureBind tbs( mTarget, mTextureId );

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexImage2D( mTarget, 0, mInternalFormat, mActualSize.x, mActualSize.y, 0, dataFormat, format.getDataType(), data );

	if( mMipmapping ) {
		initMaxMipmapLevel();
		glGenerateMipmap( mTarget );
	}
}

#if ! defined( CINDER_GL_ES )
// Called by initData( ImageSourceRef ) when the user has supplied an intermediate PBO via Format
// We map the PBO after resizing it if necessary, and then use that as a data store for the ImageTargetGlTexture
void Texture2d::initDataImageSourceWithPboImpl( const ImageSourceRef &imageSource, const Format & /*format*/, GLint dataFormat, GLint dataType, ImageIo::ChannelOrder channelOrder, bool isGray, const PboRef &pbo )
{
	auto ctx = gl::context();

	ctx->pushBufferBinding( GL_PIXEL_UNPACK_BUFFER, pbo->getId() );
	// resize the PBO if necessary
	if( pbo->getSize() < imageSource->getRowBytes() * imageSource->getHeight() )
		pbo->bufferData( imageSource->getRowBytes() * imageSource->getHeight(), nullptr, GL_STREAM_DRAW );
	void *pboData = pbo->map( GL_WRITE_ONLY );

	if( imageSource->getDataType() == ImageIo::UINT8 ) {
		auto target = ImageTargetGlTexture<uint8_t>::create( this, channelOrder, isGray, imageSource->hasAlpha(), pboData );
		imageSource->load( target );
		pbo->unmap();
		glTexImage2D( mTarget, 0, mInternalFormat, mActualSize.x, mActualSize.y, 0, dataFormat, dataType, nullptr );
	}
	else if( imageSource->getDataType() == ImageIo::UINT16 ) {
		auto target = ImageTargetGlTexture<uint16_t>::create( this, channelOrder, isGray, imageSource->hasAlpha(), pboData );
		imageSource->load( target );
		pbo->unmap();
		glTexImage2D( mTarget, 0, mInternalFormat, mActualSize.x, mActualSize.y, 0, dataFormat, dataType, nullptr );
	}
	else if( imageSource->getDataType() == ImageIo::FLOAT16 ) {
		auto target = ImageTargetGlTexture<half_float>::create( this, channelOrder, isGray, imageSource->hasAlpha(), pboData );
		imageSource->load( target );
		pbo->unmap();
		glTexImage2D( mTarget, 0, mInternalFormat, mActualSize.x, mActualSize.y, 0, dataFormat, dataType, nullptr );
	}
	else {
		auto target = ImageTargetGlTexture<float>::create( this, channelOrder, isGray, imageSource->hasAlpha(), pboData );
		imageSource->load( target );
		pbo->unmap();
		glTexImage2D( mTarget, 0, mInternalFormat, mActualSize.x, mActualSize.y, 0, dataFormat, dataType, nullptr );
	}

	ctx->popBufferBinding( GL_PIXEL_UNPACK_BUFFER );
}
#endif

// Called by initData( ImageSourceRef ) when the user has NOT supplied an intermediate PBO
void Texture2d::initDataImageSourceImpl( const ImageSourceRef &imageSource, const Format & /*format*/, GLint dataFormat, GLint dataType, ImageIo::ChannelOrder channelOrder, bool isGray )
{
	if( imageSource->getDataType() == ImageIo::UINT8 ) {
		auto target = ImageTargetGlTexture<uint8_t>::create( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		glTexImage2D( mTarget, 0, mInternalFormat, mActualSize.x, mActualSize.y, 0, dataFormat, dataType, target->getData() );
	}
	else if( imageSource->getDataType() == ImageIo::UINT16 ) {
		auto target = ImageTargetGlTexture<uint16_t>::create( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		glTexImage2D( mTarget, 0, mInternalFormat, mActualSize.x, mActualSize.y, 0, dataFormat, dataType, target->getData() );

	}
	else if( imageSource->getDataType() == ImageIo::FLOAT16 ) {
		auto target = ImageTargetGlTexture<half_float>::create( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
#if defined( CINDER_GL_ES_2 )
		glTexImage2D( mTarget, 0, mInternalFormat, mActualSize.x, mActualSize.y, 0, dataFormat, dataType, target->getData() );
#else
		glTexImage2D( mTarget, 0, mInternalFormat, mActualSize.x, mActualSize.y, 0, dataFormat, dataType, target->getData() );
#endif

	}
	else {
		auto target = ImageTargetGlTexture<float>::create( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		glTexImage2D( mTarget, 0, mInternalFormat, mActualSize.x, mActualSize.y, 0, dataFormat, dataType, target->getData() );
	}
}

void Texture2d::initData( const ImageSourceRef &imageSource, const Format &format )
{
	mActualSize = ivec2( imageSource->getWidth(), imageSource->getHeight() );
	mCleanBounds = Area( 0, 0, mActualSize.x, mActualSize.y );

	// setup an appropriate dataFormat/ImageTargetTexture based on the image's color space
	ImageIo::ChannelOrder channelOrder;
	bool isGray = false;
	switch( imageSource->getColorModel() ) {
		case ImageSource::CM_RGB:
			channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::RGBA : ImageIo::RGB;
		break;
		case ImageSource::CM_GRAY:
			channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::YA : ImageIo::Y;
			isGray = true;
		break;
		default: // if this is some other color space, we'll have to punt and go w/ RGB
			channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::RGBA : ImageIo::RGB;
		break;
	}

	GLenum dataFormat, dataType;
	getInternalFormatInfo( mInternalFormat, &dataFormat, &dataType, nullptr, nullptr, nullptr );
	if( ! format.isAutoDataType() )
		dataType = format.getDataType();

	ScopedTextureBind tbs( mTarget, mTextureId );

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
#if ! defined( CINDER_GL_ES )
	auto pbo = format.getIntermediatePbo();
	if( pbo )
		initDataImageSourceWithPboImpl( imageSource, format, dataFormat, dataType, channelOrder, isGray, pbo );
	else {
		initDataImageSourceImpl( imageSource, format, dataFormat, dataType, channelOrder, isGray );
	}
#else
	initDataImageSourceImpl( imageSource, format, dataFormat, dataType, channelOrder, isGray );
#endif

	if( mMipmapping ) {
		initMaxMipmapLevel();
		glGenerateMipmap( mTarget );
	}
}

void Texture2d::update( const void *data, GLenum dataFormat, GLenum dataType, int mipLevel, int width, int height, const ivec2 &destLowerLeftOffset )
{
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexSubImage2D( mTarget, mipLevel, destLowerLeftOffset.x, destLowerLeftOffset.y, width, height, dataFormat, dataType, data );
}

void Texture2d::update( const Surface8u &surface, int mipLevel, const ivec2 &destLowerLeftOffset )
{
	setData( surface, false, mipLevel, destLowerLeftOffset );
}

void Texture2d::update( const Channel8u &channel, int mipLevel, const ivec2 &destLowerLeftOffset )
{
	setData( channel, false, mipLevel, destLowerLeftOffset );
}

void Texture2d::update( const Surface16u &surface, int mipLevel, const ivec2 &destLowerLeftOffset )
{
	setData<uint16_t>( surface, false, mipLevel, destLowerLeftOffset );
}

void Texture2d::update( const Channel16u &channel, int mipLevel, const ivec2 &destLowerLeftOffset )
{
	setData<uint16_t>( channel, false, mipLevel, destLowerLeftOffset );
}

void Texture2d::update( const Surface32f &surface, int mipLevel, const ivec2 &destLowerLeftOffset )
{
	setData<float>( surface, false, mipLevel, destLowerLeftOffset );
}

void Texture2d::update( const Channel32f &channel, int mipLevel, const ivec2 &destLowerLeftOffset )
{
	setData<float>( channel, false, mipLevel, destLowerLeftOffset );
}

#if ! defined( CINDER_GL_ES )
void Texture2d::update( const PboRef &pbo, GLenum format, GLenum type, int mipLevel, size_t pboByteOffset )
{
	update( pbo, format, type, getBounds(), mipLevel, pboByteOffset );
}

void Texture2d::update( const PboRef &pbo, GLenum format, GLenum type, const Area &destArea, int mipLevel, size_t pboByteOffset )
{
	// TODO: warn if PBO's target is wrong
	/*
	CI_ASSERT_ERROR( pbo->getTarget() == GL_PIXEL_UNPACK_BUFFER )
	*/

	ScopedBuffer bufScp( (BufferObjRef)( pbo ) );
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexSubImage2D( mTarget, mipLevel, destArea.getX1(), mActualSize.y - destArea.getY2(), destArea.getWidth(), destArea.getHeight(), format, type, reinterpret_cast<const GLvoid*>( pboByteOffset ) );
	if( mMipmapping && mipLevel == 0 )
		glGenerateMipmap( mTarget );
}
#endif

void Texture2d::setCleanBounds( const Area &cleanBounds )
{
	mCleanBounds = cleanBounds;
}

Rectf Texture2d::getAreaTexCoords( const Area &area ) const
{
	Rectf result;

	if( mTarget == GL_TEXTURE_2D
#if ! defined( CINDER_GL_ES )
		|| mTarget == GL_TEXTURE_2D_MULTISAMPLE
#elif defined( CINDER_ANDROID )
		|| mTarget == GL_TEXTURE_EXTERNAL_OES
#endif
	   ) { // normalized 0-1.0 coordinates
		result.x1 = (mCleanBounds.x1 + area.x1) / (float)mActualSize.x;
		result.y1 = (mCleanBounds.y1 + area.y1) / (float)mActualSize.y;
		result.x2 = (mCleanBounds.x1 + area.x2) / (float)mActualSize.x;
		result.y2 = (mCleanBounds.y1 + area.y2) / (float)mActualSize.y;

		if( ! mTopDown ) {
			result.y1 = 1 - result.y1;
			result.y2 = 1 - result.y2;
		}
	}
	else { // rectangular (pixel) coordinates
		result = Rectf( area ) + mCleanBounds.getUL();

		if( ! mTopDown ) {
			result.y1 = mActualSize.y - result.y1;
			result.y2 = mActualSize.y - result.y2;
		}
	}

	return result;
}

std::ostream& operator<<( std::ostream &os, const TextureBase &rhs )
{
	ScopedTextureBind bind( rhs.getTarget(), rhs.getId() );

	os << "Target: " << constantToString( rhs.getTarget() ) << "  ID: " << rhs.mTextureId << std::endl;
	if( ! rhs.mLabel.empty() )
	os << "       Label: " << rhs.mLabel << std::endl;
	os << "  Intrnl Fmt: " << constantToString( rhs.getInternalFormat() );
	os << "    Dims: ";
	rhs.printDims( os );
#if ! defined( CINDER_GL_ES )
	GLint compressed;
	glGetTexLevelParameteriv( rhs.getTarget(), 0, GL_TEXTURE_COMPRESSED, &compressed );
	if( compressed ) {
		GLint compressedSize;
		glGetTexLevelParameteriv( rhs.getTarget(), 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSize );
		os << std::endl;
		os << "  Level 0 Size: " << compressedSize << " bytes (Compressed)";
	}
#endif

	return os;
}

void Texture2d::printDims( std::ostream &os ) const
{
	os << getWidth() << " x " << getHeight();
}

void Texture2d::initParams( Format &format, GLint defaultInternalFormat, GLint defaultDataType )
{
	mTopDown = format.mLoadTopDown;
	TextureBase::initParams( format, defaultInternalFormat, defaultDataType );
}

/////////////////////////////////////////////////////////////////////////////////
// Texture2dCache

Texture2dCacheRef Texture2dCache::create()
{
	return Texture2dCacheRef( new Texture2dCache() );
}

Texture2dCacheRef Texture2dCache::create( const Surface8u &prototypeSurface, const Texture2d::Format &format )
{
	return Texture2dCacheRef( new Texture2dCache( prototypeSurface, format ) );
}

Texture2dCache::Texture2dCache()
{
}

Texture2dCache::Texture2dCache( const Surface8u &prototypeSurface, const Texture2d::Format &format )
	: mWidth( prototypeSurface.getWidth() ), mHeight( prototypeSurface.getHeight() ),
	mFormat( format ), mNextId( 0 )
{
	if( mWidth * prototypeSurface.getChannelOrder().getPixelInc() != prototypeSurface.getRowBytes() ) {
		CI_LOG_V( "Surface rowBytes will prevent full efficiency in gl::Texture upload." );
		mIntermediateSurface = Surface8u::create( prototypeSurface.getWidth(), prototypeSurface.getHeight(),
				prototypeSurface.hasAlpha(), prototypeSurface.getChannelOrder() );
	}
}

gl::TextureRef Texture2dCache::cache( const Surface8u &originalData )
{
	const Surface8u &surfaceData = ( mIntermediateSurface ) ? *mIntermediateSurface : originalData;
	// If mIntermediateSurface isn't null then we need to use that instead.
	// This is to accommodate rowBytes values which aren't the same as width * bytesPerPixel
	if( mIntermediateSurface )
		mIntermediateSurface->copyFrom( originalData, originalData.getBounds() );

	// find an available slot and update that if possible
	for( vector<pair<int,TextureRef>>::iterator texIt = mTextures.begin(); texIt != mTextures.end(); ++texIt ) {
		if( texIt->first == -1 ) { // this texture is available, let's use it!
			texIt->second->update( surfaceData );
			texIt->first = mNextId++;
			// normally this would be very wrong, but when the result TextureRef is destroyed, it calls markTextureAsFree rather than deleting the master texture
			return TextureRef( texIt->second.get(), std::bind( &Texture2dCache::markTextureAsFree, shared_from_this(), texIt->first ) );
		}
	}

	// we didn't find an available slot, so let's make a new texture
	TextureRef masterTex( new Texture( surfaceData, mFormat ) );
	mTextures.push_back( make_pair( mNextId++, masterTex ) );
	// normally this would be very wrong, but when the result TextureRef is destroyed, it calls markTextureAsFree rather than deleting the master texture
	return TextureRef( mTextures.back().second.get(), std::bind( &Texture2dCache::markTextureAsFree, shared_from_this(), mTextures.back().first ) );
}

void Texture2dCache::markTextureAsFree( int id )
{
	for( vector<pair<int,TextureRef> >::iterator texIt = mTextures.begin(); texIt != mTextures.end(); ++texIt ) {
		if( texIt->first == id ) { // this texture is available now, let's mark it as usable
			texIt->first = -1;
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////
// ImageSourceTexture
class ImageSourceTexture : public ImageSource {
  public:
	ImageSourceTexture( Texture &texture )
		: ImageSource()
	{
		mWidth = texture.getWidth();
		mHeight = texture.getHeight();


		GLenum format;
#if ! defined( CINDER_GL_ES )
		GLint internalFormat = texture.getInternalFormat();
		switch( internalFormat ) {
			case GL_RGB: setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); format = GL_RGB; break;
			case GL_RGBA: setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); format = GL_RGBA; break;
			case GL_RGBA8: setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); format = GL_RGBA; break;
			case GL_RGB8: setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); format = GL_RGB; break;
			case GL_BGR: setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); format = GL_RGB; break;
			case GL_DEPTH_COMPONENT16: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT16 ); format = GL_DEPTH_COMPONENT; break;
			case GL_DEPTH_COMPONENT24: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_DEPTH_COMPONENT; break;
			case GL_DEPTH_COMPONENT32: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_DEPTH_COMPONENT; break;
			case GL_RGBA32F: setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); format = GL_RGBA; break;
			case GL_RGB32F: setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); format = GL_RGB; break;
			case GL_R32F: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_RED; break;
			case GL_RG32F: setChannelOrder( ImageIo::YA ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_RG; break;
			case GL_RGBA16F: setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT16 ); format = GL_RGBA; break;
			case GL_RGB16F: setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT16 ); format = GL_RGB; break;
			case GL_R16F: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT16 ); format = GL_RED; break;
			case GL_RG16F: setChannelOrder( ImageIo::YA ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT16 ); format = GL_RG; break;
			default:
				setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); format = GL_RGBA; break;
		}
#else
		// at least on iOS, non-RGBA appears to fail on glReadPixels, so we force RGBA
		setChannelOrder( ImageIo::RGBA );
		setColorModel( ImageIo::CM_RGB );
		setDataType( ImageIo::UINT8 );
		format = GL_RGBA;
#endif

		GLenum dataType = GL_UNSIGNED_BYTE;
		int dataSize = 1;
		if( mDataType == ImageIo::UINT16 ) {
			dataType = GL_UNSIGNED_SHORT;
			dataSize = 2;
		}
		else if( mDataType == ImageIo::FLOAT16 ) {
#if defined( CINDER_GL_ES_2 )
			dataType = GL_HALF_FLOAT_OES;
#else
			dataType = GL_HALF_FLOAT;
#endif
			dataSize = 2;
		}
		else if( mDataType == ImageIo::FLOAT32 ) {
			dataType = GL_FLOAT;
			dataSize = 4;
		}

		mRowBytes = mWidth * ImageIo::channelOrderNumChannels( mChannelOrder ) * dataSize;
		mData = unique_ptr<uint8_t[]>( new uint8_t[mRowBytes * mHeight] );

#if defined( CINDER_GL_ES )
		// This line is not too awesome, however we need a TextureRef, not a Texture, for an FBO attachment. So this creates a shared_ptr with a no-op deleter
		// that won't destroy our original texture.
		TextureRef tempSharedPtr( &texture, []( const Texture* ){} );
		// The theory here is we need to build an FBO, attach the Texture to it, issue a glReadPixels against it, and the put it away
		FboRef fbo = Fbo::create( mWidth, mHeight, gl::Fbo::Format().attachment( GL_COLOR_ATTACHMENT0, tempSharedPtr ).disableDepth() );
		ScopedFramebuffer fbScp( fbo );
		glReadPixels( 0, 0, mWidth, mHeight, format, dataType, mData.get() );
#else
		gl::ScopedTextureBind tbScope( texture.getTarget(), texture.getId() );
		glPixelStorei( GL_PACK_ALIGNMENT, 1 );
		glGetTexImage( texture.getTarget(), 0, format, dataType, mData.get() );
#endif

		// invert rowbytes to accommodate a top-down texture as necessary
		if( ! texture.isTopDown() )
			mRowBytes = -mRowBytes;
	}

	void load( ImageTargetRef target ) {
		// get a pointer to the ImageSource function appropriate for handling our data configuration
		ImageSource::RowFunc func = setupRowFunc( target );

		const uint8_t *data = mData.get();
		if( mRowBytes < 0 )
			data = data + ( mHeight - 1 ) * (-mRowBytes);
		for( int32_t row = 0; row < mHeight; ++row ) {
			((*this).*func)( target, row, data );
			data += mRowBytes;
		}
	}

	std::unique_ptr<uint8_t[]>	mData;
	int32_t						mRowBytes;
};

void Texture2d::regenerateMipmap()
{
	if( mMipmapping ) {
		ScopedTextureBind tbs( mTarget, mTextureId );
		glGenerateMipmap( mTarget );
	}
}

ImageSourceRef Texture2d::createSource()
{
	return ImageSourceRef( new ImageSourceTexture( *this ) );
}

#if ! defined( CINDER_GL_ES_2 )
/////////////////////////////////////////////////////////////////////////////////
// Texture3d
Texture3dRef Texture3d::create( GLint width, GLint height, GLint depth, const Format &format )
{
	if( format.mDeleter )
		return Texture3dRef( new Texture3d( width, height, depth, format ), format.mDeleter );
	else
		return Texture3dRef( new Texture3d( width, height, depth, format ) );
}

Texture3dRef Texture3d::create( const void *data, GLenum dataFormat, int width, int height, int depth, const Format &format )
{
	if( format.mDeleter )
		return Texture3dRef( new Texture3d( data, dataFormat, width, height, depth, format ), format.mDeleter );
	else
		return Texture3dRef( new Texture3d( data, dataFormat, width, height, depth, format ) );
}

Texture3d::Texture3d( GLint width, GLint height, GLint depth, Format format )
	: mWidth( width ), mHeight( height ), mDepth( depth )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	TextureBase::initParams( format, GL_RGB, GL_UNSIGNED_BYTE );

	ScopedTextureBind tbs( mTarget, mTextureId );
	env()->allocateTexStorage3d( mTarget, mMaxMipmapLevel + 1, mInternalFormat, mWidth, mHeight, mDepth, format.isImmutableStorage() );
}

Texture3d::Texture3d( const void *data, GLenum dataFormat, int width, int height, int depth, Format format )
	: mWidth( width ), mHeight( height ), mDepth( depth )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	TextureBase::initParams( format, GL_RGB, GL_UNSIGNED_BYTE );

	glTexImage3D( mTarget, 0, mInternalFormat, mWidth, mHeight, mDepth, 0, dataFormat, format.getDataType(), data );
}

void Texture3d::update( const Surface8u &surface, int depth, int mipLevel )
{
	GLint dataFormat;
	GLenum dataType;
	SurfaceChannelOrderToDataFormatAndType<uint8_t>( surface.getChannelOrder(), &dataFormat, &dataType );

	ivec2 mipMapSize = calcMipLevelSize( mipLevel, getWidth(), getHeight() );
	if( surface.getSize() != mipMapSize )
		throw TextureResizeExc( "Invalid Texture3d::update() surface dimensions", surface.getSize(), mipMapSize );

	update( (void*)surface.getData(), dataFormat, dataType, mipLevel, mipMapSize.x, mipMapSize.y, 1, 0, 0, depth );
}

void Texture3d::update( const void *data, GLenum dataFormat, GLenum dataType, int mipLevel, int width, int height, int depth, int xOffset, int yOffset, int zOffset )
{
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexSubImage3D( mTarget, mipLevel, xOffset, yOffset, zOffset, width, height, depth, dataFormat, dataType, data );
}

void Texture3d::printDims( std::ostream &os ) const
{
	os << mWidth << " x " << mHeight << " x " << mDepth;
}

#endif // ! defined( CINDER_GL_ES_2 )

/////////////////////////////////////////////////////////////////////////////////
// TextureCubeMap
namespace {
struct CubeMapFaceRegion {
	Area		mArea;
	ivec2		mOffset;
	bool		mFlip; // horizontal + vertical
};

std::vector<CubeMapFaceRegion> calcCubeMapHorizontalCrossRegions( const ImageSourceRef &imageSource )
{
	std::vector<CubeMapFaceRegion> result;

	ivec2 faceSize( imageSource->getWidth() / 4, imageSource->getHeight() / 3 );
	Area faceArea( 0, 0, faceSize.x, faceSize.y );

	Area area;
	ivec2 offset;

	// GL_TEXTURE_CUBE_MAP_POSITIVE_X
	area = faceArea + ivec2( faceSize.x * 2, faceSize.y * 1 );
	offset = -ivec2( faceSize.x * 2, faceSize.y * 1 );
	result.push_back( { area, offset, false } );
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	area = faceArea + ivec2( faceSize.x * 0, faceSize.y * 1 );
	offset = -ivec2( faceSize.x * 0, faceSize.y * 1 );
	result.push_back( { area, offset, false } );
	// GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	area = faceArea + ivec2( faceSize.x * 1, faceSize.y * 0 );
	offset = -ivec2( faceSize.x * 1, faceSize.y * 0 );
	result.push_back( { area, offset, false } );
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	area = faceArea + ivec2( faceSize.x * 1, faceSize.y * 2 );
	offset = -ivec2( faceSize.x * 1, faceSize.y * 2 );
	result.push_back( { area, offset, false } );
	// GL_TEXTURE_CUBE_MAP_POSITIVE_Z
	area = faceArea + ivec2( faceSize.x * 1, faceSize.y * 1 );
	offset = -ivec2( faceSize.x * 1, faceSize.y * 1 );
	result.push_back( { area, offset, false } );
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	area = faceArea + ivec2( faceSize.x * 3, faceSize.y * 1 );
	offset = -ivec2( faceSize.x * 3, faceSize.y * 1 );
	result.push_back( { area, offset, false } );

	return result;
};

std::vector<CubeMapFaceRegion> calcCubeMapVerticalCrossRegions( const ImageSourceRef &imageSource )
{
	std::vector<CubeMapFaceRegion> result;

	ivec2 faceSize( imageSource->getWidth() / 3, imageSource->getHeight() / 4 );
	Area faceArea( 0, 0, faceSize.x, faceSize.y );

	Area area;
	ivec2 offset;

	// GL_TEXTURE_CUBE_MAP_POSITIVE_X
	area = faceArea + ivec2( faceSize.x * 2, faceSize.y * 1 );
	offset = -ivec2( faceSize.x * 2, faceSize.y * 1 );
	result.push_back( { area, offset, false } );
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	area = faceArea + ivec2( faceSize.x * 0, faceSize.y * 1 );
	offset = -ivec2( faceSize.x * 0, faceSize.y * 1 );
	result.push_back( { area, offset, false } );
	// GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	area = faceArea + ivec2( faceSize.x * 1, faceSize.y * 0 );
	offset = -ivec2( faceSize.x * 1, faceSize.y * 0 );
	result.push_back( { area, offset, false } );
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	area = faceArea + ivec2( faceSize.x * 1, faceSize.y * 2 );
	offset = -ivec2( faceSize.x * 1, faceSize.y * 2 );
	result.push_back( { area, offset, false } );
	// GL_TEXTURE_CUBE_MAP_POSITIVE_Z
	area = faceArea + ivec2( faceSize.x * 1, faceSize.y * 1 );
	offset = -ivec2( faceSize.x * 1, faceSize.y * 1 );
	result.push_back( { area, offset, false } );
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	area = faceArea + ivec2( faceSize.x * 1, faceSize.y * 3 );
	offset = -ivec2( faceSize.x * 1, faceSize.y * 3 );
	result.push_back( { area, offset, true } );

	return result;
};

std::vector<CubeMapFaceRegion> calcCubeMapHorizontalRegions( const ImageSourceRef &imageSource )
{
	std::vector<CubeMapFaceRegion> result;
	ivec2 faceSize( imageSource->getHeight(), imageSource->getHeight() );

	for( uint8_t index = 0; index < 6; ++index ) {
		Area area( index * faceSize.x, 0, (index + 1) * faceSize.x, faceSize.y );
		ivec2 offset( -index * faceSize.x, 0 );
		result.push_back( { area, offset, false } );
	}

	return result;
};

std::vector<CubeMapFaceRegion> calcCubeMapVerticalRegions( const ImageSourceRef &imageSource )
{
	std::vector<CubeMapFaceRegion> result;
	ivec2 faceSize( imageSource->getWidth(), imageSource->getWidth() );

	for( uint8_t index = 0; index < 6; ++index ) {
		Area area( 0, index * faceSize.x, faceSize.x, (index + 1) * faceSize.y );
		ivec2 offset( 0, -index * faceSize.y );
		result.push_back( { area, offset, false } );
	}

	return result;
};
} // anonymous namespace

TextureCubeMap::Format::Format()
	: TextureBase::Format()
{
	mTarget = GL_TEXTURE_CUBE_MAP;
	mMinFilter = GL_NEAREST;
	mMagFilter = GL_NEAREST;
}

TextureCubeMapRef TextureCubeMap::create( int32_t width, int32_t height, const Format &format )
{
	if( format.mDeleter )
		return TextureCubeMapRef( new TextureCubeMap( width, height, format ), format.mDeleter );
	else
		return TextureCubeMapRef( new TextureCubeMap( width, height, format ) );
}

TextureCubeMapRef TextureCubeMap::create( const ImageSourceRef &imageSource, const Format &format )
{
	if ( imageSource->getDataType() == ImageIo::UINT8 )
		return createTextureCubeMapImpl<uint8_t>( imageSource, format );
	else
		return createTextureCubeMapImpl<float>( imageSource, format );
}

TextureCubeMapRef TextureCubeMap::create( const TextureData &data, const Format &format )
{
	if( format.mDeleter )
		return TextureCubeMapRef( new TextureCubeMap( data, format ), format.mDeleter );
	else
		return TextureCubeMapRef( new TextureCubeMap( data, format ) );
}

template<typename T>
TextureCubeMapRef TextureCubeMap::createTextureCubeMapImpl( const ImageSourceRef &imageSource, const Format &format )
{
	std::vector<CubeMapFaceRegion> faceRegions;

	// Infer the layout based on image aspect ratio
	ivec2 imageSize( imageSource->getWidth(), imageSource->getHeight() );
	float minDim = (float)std::min( imageSize.x, imageSize.y );
	float maxDim = (float)std::max( imageSize.x, imageSize.y );
	float aspect = minDim / maxDim;
	if( abs( aspect - 1 / 6.0f ) < abs( aspect - 3 / 4.0f ) ) { // closer to 1:6 than to 4:3, so row or column
		faceRegions = ( imageSize.x > imageSize.y ) ? calcCubeMapHorizontalRegions( imageSource ) : calcCubeMapVerticalRegions( imageSource );
	}
	else { // else, horizontal or vertical cross
		faceRegions = ( imageSize.x > imageSize.y ) ? calcCubeMapHorizontalCrossRegions( imageSource ) : calcCubeMapVerticalCrossRegions( imageSource );
	}

	Area faceArea = faceRegions.front().mArea;
	ivec2 faceSize = faceArea.getSize();

	SurfaceT<T> masterSurface( imageSource, SurfaceConstraintsDefault() );
	SurfaceT<T> images[6];

	for( uint8_t f = 0; f < 6; ++f ) {
		images[f] = SurfaceT<T>( faceSize.x, faceSize.y, masterSurface.hasAlpha(), SurfaceConstraints() );
		images[f].copyFrom( masterSurface, faceRegions[f].mArea, faceRegions[f].mOffset );
		if( faceRegions[f].mFlip ) {
			ip::flipVertical( &images[f] );
			ip::flipHorizontal( &images[f] );
		}
	}

	if( format.mDeleter )
		return TextureCubeMapRef( new TextureCubeMap( images, format ), format.mDeleter );
	else
		return TextureCubeMapRef( new TextureCubeMap( images, format ) );
}

TextureCubeMapRef TextureCubeMap::create( const ImageSourceRef images[6], const Format &format )
{
	if( images[0]->getDataType() == ImageIo::UINT8 ) {
		Surface8u surfaces[6];
		for( size_t i = 0; i < 6; ++i )
			surfaces[i] = Surface8u( images[i] );

		if( format.mDeleter )
			return TextureCubeMapRef( new TextureCubeMap( surfaces, format ), format.mDeleter );
		else
			return TextureCubeMapRef( new TextureCubeMap( surfaces, format ) );
	}
	else {
		Surface32f surfaces[6];
		for( size_t i = 0; i < 6; ++i )
			surfaces[i] = Surface32f( images[i] );

		if( format.mDeleter )
			return TextureCubeMapRef( new TextureCubeMap( surfaces, format ), format.mDeleter );
		else
			return TextureCubeMapRef( new TextureCubeMap( surfaces, format ) );
	}
}

TextureCubeMapRef TextureCubeMap::createFromKtx( const DataSourceRef &dataSource, const Format &format )
{
#if ! defined( CINDER_GL_ES )
	TextureData textureData( format.getIntermediatePbo() );
#else
	TextureData textureData;
#endif

	parseKtx( dataSource, &textureData );
	return TextureCubeMap::create( textureData, format );
}

#if ! defined( CINDER_GL_ES ) || defined( CINDER_GL_ANGLE )
TextureCubeMapRef TextureCubeMap::createFromDds( const DataSourceRef &dataSource, const Format &format )
{
#if ! defined( CINDER_GL_ES )
	TextureData textureData( format.getIntermediatePbo() );
#else
	TextureData textureData;
#endif

	parseDds( dataSource, &textureData );
	return TextureCubeMap::create( textureData, format );
}
#endif

TextureCubeMap::TextureCubeMap( int32_t width, int32_t height, Format format )
	: mWidth( width ), mHeight( height )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	TextureBase::initParams( format, GL_RGB, GL_UNSIGNED_BYTE );

	env()->allocateTexStorageCubeMap( mMaxMipmapLevel + 1, mInternalFormat, width, height, format.isImmutableStorage() );
}

template<typename T>
TextureCubeMap::TextureCubeMap( const SurfaceT<T> images[6], Format format )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );

	GLenum dataType = GL_UNSIGNED_BYTE;
	if( std::is_same<T,float>::value )
		dataType = GL_FLOAT;

	TextureBase::initParams( format, ( images[0].hasAlpha() ) ? GL_RGBA : GL_RGB, dataType );

	mWidth = images[0].getWidth();
	mHeight = images[0].getHeight();

	for( GLenum target = 0; target < 6; ++target )
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + target, 0, mInternalFormat, images[target].getWidth(), images[target].getHeight(), 0,
			( images[target].hasAlpha() ) ? GL_RGBA : GL_RGB, format.getDataType(), images[target].getData() );

	if( format.mMipmapping ) {
#if ! defined( CINDER_GL_ES_2 )
		if( mMaxMipmapLevel == -1 )
			mMaxMipmapLevel = requiredMipLevels( mWidth, mHeight, 1 ) - 1;
		glTexParameteri( mTarget, GL_TEXTURE_MAX_LEVEL, mMaxMipmapLevel );
#endif
		glGenerateMipmap( mTarget );
	}
}

TextureCubeMap::TextureCubeMap( const TextureData &data, Format format )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, 0 /* unused */, 0 /* unused */ );

	replace( data );

	if( mMipmapping && data.getNumLevels() <= 1 ) {
		glGenerateMipmap( mTarget );
	}
}

//! Replaces the pixels (and data store) of a Texture with contents of \a textureData.
void TextureCubeMap::replace( const TextureData &textureData )
{
	mInternalFormat = textureData.getInternalFormat();

	mWidth = textureData.getWidth();
	mHeight = textureData.getHeight();

	ScopedTextureBind bindScope( mTarget, mTextureId );
	if( textureData.getUnpackAlignment() != 0 )
		glPixelStorei( GL_UNPACK_ALIGNMENT, textureData.getUnpackAlignment() );

	if( textureData.getNumFaces() != 6 ) {
		throw TextureDataExc( "TextureCubeMap requires 6 faces" );
	}

	int curLevel = 0, curFaceIdx;
	for( const auto &textureDataLevel : textureData.getLevels() ) {
		curFaceIdx = 0;
		for( const auto &textureDataFace : textureDataLevel.getFaces() ) {
			if( ! textureData.isCompressed() )
				glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + curFaceIdx, curLevel, mInternalFormat, textureDataLevel.width, textureDataLevel.height, 0, textureData.getDataFormat(), textureData.getDataType(), textureData.getDataStorePtr( textureDataFace.offset ) );
			else
				glCompressedTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + curFaceIdx, curLevel, textureData.getInternalFormat(), textureDataLevel.width, textureDataLevel.height, 0, textureDataFace.dataSize, textureData.getDataStorePtr( textureDataFace.offset ) );
			++curFaceIdx;
		}
		++curLevel;
	}
	if( textureData.getUnpackAlignment() != 0 )
		glPixelStorei( GL_UNPACK_ALIGNMENT, textureData.getUnpackAlignment() );

#if ! defined( CINDER_GL_ES_2 )
	mMaxMipmapLevel = (int32_t)textureData.getNumLevels() - 1;
	glTexParameteri( mTarget, GL_TEXTURE_MAX_LEVEL, mMaxMipmapLevel );
#endif
}

void TextureCubeMap::regenerateMipmap()
{
	if( mMipmapping ) {
		ScopedTextureBind tbs( mTarget, mTextureId );
		glGenerateMipmap( mTarget );
	}
}

void TextureCubeMap::printDims( std::ostream &os ) const
{
	os << mWidth << " x " << mHeight;
}

/////////////////////////////////////////////////////////////////////////////////
// ImageTargetGlTexture
template<typename T>
shared_ptr<ImageTargetGlTexture<T>> ImageTargetGlTexture<T>::create( const Texture *texture, ImageIo::ChannelOrder &channelOrder, bool isGray, bool hasAlpha )
{
	return shared_ptr<ImageTargetGlTexture<T>>( new ImageTargetGlTexture<T>( texture, channelOrder, isGray, hasAlpha, nullptr ) );
}

#if ! defined( CINDER_GL_ES )
// create method receives an existing pointer which presumably is a mapped PBO
template<typename T>
shared_ptr<ImageTargetGlTexture<T>> ImageTargetGlTexture<T>::create( const Texture *texture, ImageIo::ChannelOrder &channelOrder, bool isGray, bool hasAlpha, void *intermediateDataStore )
{
	return shared_ptr<ImageTargetGlTexture<T>>( new ImageTargetGlTexture<T>( texture, channelOrder, isGray, hasAlpha, intermediateDataStore ) );
}
#endif

template<typename T>
ImageTargetGlTexture<T>::ImageTargetGlTexture( const Texture *texture, ImageIo::ChannelOrder &channelOrder, bool isGray, bool hasAlpha, void *intermediateDataStore )
	: ImageTarget(), mTexture( texture ), mHasAlpha( hasAlpha )
{
	if( isGray ) {
		mPixelInc = mHasAlpha ? 2 : 1;
	}
	else {
		mPixelInc = mHasAlpha ? 4 : 3;
	}
	mRowInc = mTexture->getWidth() * mPixelInc;

	// allocate enough room to hold all these pixels if we haven't been passed a data*
	if( ! intermediateDataStore ) {
		mDataStore = std::unique_ptr<T[]>( new T[mTexture->getHeight() * mRowInc] );
		mDataBaseAddress = mDataStore.get();
	}
	else
		mDataBaseAddress = reinterpret_cast<T*>( intermediateDataStore );

	if( std::is_same<T,uint8_t>::value ) {
		setDataType( ImageIo::UINT8 );
	}
	else if( std::is_same<T,uint16_t>::value ) {
		setDataType( ImageIo::UINT16 );
	}
	else if( std::is_same<T,half_float>::value ) {
		setDataType( ImageIo::FLOAT16 );
	}
	else if( std::is_same<T,float>::value ) {
		setDataType( ImageIo::FLOAT32 );
	}

	setChannelOrder( channelOrder );
	setColorModel( isGray ? ImageIo::CM_GRAY : ImageIo::CM_RGB );
}

template<typename T>
void* ImageTargetGlTexture<T>::getRowPointer( int32_t row )
{
	if( mTexture->isTopDown() )
		return mDataBaseAddress + row * mRowInc;
	else
		return mDataBaseAddress + ( mTexture->getHeight() - 1 - row ) * mRowInc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TextureData
TextureData::TextureData()
{
	init();
}

#if ! defined( CINDER_GL_ES )
TextureData::TextureData( const PboRef &pbo )
	: mPbo( pbo )
{
	init();
	if( mPbo ) {
		gl::context()->pushBufferBinding( GL_PIXEL_UNPACK_BUFFER, pbo->getId() );
		mPboMappedPtr = nullptr;
	}
}
#endif

void TextureData::init()
{
	mWidth = mHeight = mDepth = mNumFaces = 0;
	mInternalFormat = 0;
	mDataFormat = mDataType = 0;
	mUnpackAlignment = 0;
	mSwizzleMask[0] = GL_RED;
	mSwizzleMask[1] = GL_GREEN;
	mSwizzleMask[2] = GL_BLUE;
	mSwizzleMask[3] = GL_ALPHA;
}

TextureData::~TextureData()
{
#if ! defined( CINDER_GL_ES )
	if( mPbo )
		gl::context()->popBufferBinding( GL_PIXEL_UNPACK_BUFFER );
#endif
}

void TextureData::allocateDataStore( size_t requireBytes )
{
#if defined( CINDER_GL_ES )
	mDataStoreMem = unique_ptr<uint8_t[]>( new uint8_t[requireBytes] );
#else
	if( mPbo ) {
		if( mPbo->getSize() < requireBytes )
			mPbo->bufferData( requireBytes, nullptr, GL_STREAM_DRAW );
	}
	else {
		mDataStoreMem = unique_ptr<uint8_t[]>( new uint8_t[requireBytes] );
	}
#endif
	mDataStoreSize = requireBytes;
}

void TextureData::mapDataStore()
{
#if ! defined( CINDER_GL_ES )
	if( mPbo ) {
		mPboMappedPtr = mPbo->map( GL_WRITE_ONLY );
		if( ! mPboMappedPtr ) {
			CI_LOG_W( "Failed to map PBO for TextureData of size " << mDataStoreSize << " bytes. Using CPU heap instead." );
			// a failure to map the data store means we need to resort to memory as a backup
			if( ! mDataStoreMem )
				mDataStoreMem = unique_ptr<uint8_t[]>( new uint8_t[mDataStoreSize] );
		}
	}
#endif
}

void TextureData::unmapDataStore()
{
#if ! defined( CINDER_GL_ES )
	if( mPbo )
		mPbo->unmap();
	mPboMappedPtr = nullptr;
#endif
}

void* TextureData::getDataStorePtr( size_t offset ) const
{
#if ! defined( CINDER_GL_ES )
	if( mPbo && mPboMappedPtr ) {
		return ((uint8_t*)mPboMappedPtr) + offset;
	}
	else
#endif
		return mDataStoreMem.get() + offset;
}

Texture2dRef Texture2d::createFromKtx( const DataSourceRef &dataSource, const Format &format )
{
#if ! defined( CINDER_GL_ES )
	TextureData textureData( format.getIntermediatePbo() );
#else
	TextureData textureData;
#endif

	parseKtx( dataSource, &textureData );
	return Texture2d::create( textureData, format );
}

#if defined( CINDER_GL_ES )
void Texture2d::updateFromKtx( const DataSourceRef &dataSource )
#else
void Texture2d::updateFromKtx( const DataSourceRef &dataSource, const PboRef &intermediatePbo )
#endif
{
#if defined( CINDER_GL_ES )
	TextureData textureData;
#else
	TextureData textureData( intermediatePbo );
#endif
	parseKtx( dataSource, &textureData );
	update( textureData );
}

void Texture2d::update( const TextureData &textureData )
{
	if( textureData.getWidth() != mActualSize.x || textureData.getHeight() != mActualSize.y )
		replace( textureData );
	else {
		ScopedTextureBind bindScope( mTarget, mTextureId );
		if( textureData.getUnpackAlignment() != 0 )
			glPixelStorei( GL_UNPACK_ALIGNMENT, textureData.getUnpackAlignment() );

		int curLevel = 0;
		for( const auto &textureDataLevel : textureData.getLevels() ) {
			const TextureData::Face& textureDataFace = textureDataLevel.getFaces()[0];
			if( ! textureData.isCompressed() )
				glTexSubImage2D( mTarget, curLevel, 0, 0, textureDataLevel.width, textureDataLevel.height, textureData.getDataFormat(), textureData.getDataType(), textureData.getDataStorePtr( textureDataFace.offset ) );
			else
				glCompressedTexSubImage2D( mTarget, curLevel, 0, 0, textureDataLevel.width, textureDataLevel.height, textureData.getInternalFormat(), textureDataFace.dataSize, textureData.getDataStorePtr( textureDataFace.offset ) );
			++curLevel;
		}
		if( textureData.getUnpackAlignment() != 0 )
			glPixelStorei( GL_UNPACK_ALIGNMENT, textureData.getUnpackAlignment() );
	}
}

void Texture2d::replace( const TextureData &textureData )
{
	mActualSize = ivec2( textureData.getWidth(), textureData.getHeight() );
	mCleanBounds = Area( 0, 0, mActualSize.x, mActualSize.y );
	mInternalFormat = textureData.getInternalFormat();

	ScopedTextureBind bindScope( mTarget, mTextureId );
	if( textureData.getUnpackAlignment() != 0 )
		glPixelStorei( GL_UNPACK_ALIGNMENT, textureData.getUnpackAlignment() );

	int curLevel = 0;
	for( const auto &textureDataLevel : textureData.getLevels() ) {
		const TextureData::Face& textureDataFace = textureDataLevel.getFaces()[0];
		if( ! textureData.isCompressed() )
			glTexImage2D( mTarget, curLevel, textureData.getInternalFormat(), textureDataLevel.width, textureDataLevel.height, 0, textureData.getDataFormat(), textureData.getDataType(), textureData.getDataStorePtr( textureDataFace.offset ) );
		else
			glCompressedTexImage2D( mTarget, curLevel, textureData.getInternalFormat(), textureDataLevel.width, textureDataLevel.height, 0, textureDataFace.dataSize, textureData.getDataStorePtr( textureDataFace.offset ) );
		++curLevel;
	}
	if( textureData.getUnpackAlignment() != 0 )
		glPixelStorei( GL_UNPACK_ALIGNMENT, textureData.getUnpackAlignment() );
}

#if ! defined( CINDER_GL_ES ) || defined( CINDER_GL_ANGLE )
TextureRef Texture2d::createFromDds( const DataSourceRef &dataSource, const Format &format )
{
#if ! defined( CINDER_GL_ES )
	TextureData textureData( format.getIntermediatePbo() );
#else
	TextureData textureData;
#endif

	parseDds( dataSource, &textureData );
	return Texture2d::create( textureData, format );
}

#if defined( CINDER_GL_ANGLE )
void Texture2d::updateFromDds( const DataSourceRef &dataSource )
#else
void Texture2d::updateFromDds( const DataSourceRef &dataSource, const PboRef &intermediatePbo )
#endif
{
#if defined( CINDER_GL_ES )
	TextureData textureData;
#else
	TextureData textureData( intermediatePbo );
#endif

	parseDds( dataSource, &textureData );
	update( textureData );
}
#endif // ! defined( CINDER_GL_ES ) || defined( CINDER_GL_ANGLE )

TextureResizeExc::TextureResizeExc( const string &description, const ivec2 &updateSize, const ivec2 &textureSize )
	 : TextureDataExc( "" )
{
	stringstream ss;
	ss << description << ", update size: " << updateSize << ", texture size: " << textureSize << ")";
	setDescription( ss.str() );
}

} } // namespace cinder::gl
