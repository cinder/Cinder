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

#include "cinder/gl/gl.h" // has to be first
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Pbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/TextureFormatParsers.h"
#include "cinder/gl/Environment.h"
#include "cinder/gl/ConstantStrings.h"
#include "cinder/ip/Flip.h"
#include "cinder/Log.h"
#include <stdio.h>
#include <algorithm>
#include <memory>
#include <type_traits>

#if ! defined( CINDER_GL_ES )
#define GL_LUMINANCE						GL_RED
#define GL_LUMINANCE_ALPHA					GL_RG
#endif

using namespace std;

namespace cinder { namespace gl {

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
	virtual void*	getRowPointer( int32_t row ) { return mData + row * mRowInc; }
	
	const void*		getData() const { return mData; }
	
  private:
	ImageTargetGlTexture( const Texture *texture, ImageIo::ChannelOrder &channelOrder, bool isGray, bool hasAlpha, void *intermediateData );
	
	const Texture		*mTexture;
	bool				mHasAlpha;
	uint8_t				mPixelInc;
	T					*mData;
	unique_ptr<T[]>		mDataStore; // may be NULL
	int					mRowInc;
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

// Expects texture to be bound and mTarget/mTextureId to be valid
void TextureBase::initParams( Format &format, GLint defaultInternalFormat )
{
	// default is GL_REPEAT
	if( format.mWrapS != GL_REPEAT )
		glTexParameteri( mTarget, GL_TEXTURE_WRAP_S, format.mWrapS );
	// default is GL_REPEAT
	if( format.mWrapT != GL_REPEAT )
		glTexParameteri( mTarget, GL_TEXTURE_WRAP_T, format.mWrapT );
#if ! defined( CINDER_GL_ES )
	// default is GL_REPEAT
	if( format.mWrapR != GL_REPEAT )
		glTexParameteri( mTarget, GL_TEXTURE_WRAP_R, format.mWrapR );
#endif // ! defined( CINDER_GL_ES )

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

	if( format.mInternalFormat == -1 )
		mInternalFormat = defaultInternalFormat;
	else
		mInternalFormat = format.mInternalFormat;

#if defined( CINDER_GL_ES )		
	// by default mPixelDataFormat should match mInternalFormat
	if( format.mPixelDataFormat == -1 )
		format.mPixelDataFormat = mInternalFormat;
#else
	if( format.mPixelDataFormat == -1 ) {
		format.mPixelDataFormat = GL_RGB;
	}
#endif

	// Swizzle mask
#if ! defined( CINDER_GL_ES_2 )
	if( supportsHardwareSwizzle() ) {
		if( format.mSwizzleMask[0] != GL_RED || format.mSwizzleMask[1] != GL_GREEN || format.mSwizzleMask[2] != GL_BLUE || format.mSwizzleMask[3] != GL_ALPHA )
			glTexParameteriv( mTarget, GL_TEXTURE_SWIZZLE_RGBA, format.mSwizzleMask.data() );
	}
// Compare Mode and Func for Depth Texture
	if( format.mCompareMode > -1 ) {
		glTexParameteri( mTarget, GL_TEXTURE_COMPARE_MODE, format.mCompareMode );
	}
	if( format.mCompareFunc > -1 ) {
		glTexParameteri( mTarget, GL_TEXTURE_COMPARE_FUNC, format.mCompareFunc );
	}
#else
	if( format.mCompareMode > -1 ) {
		if( supportsShadowSampler() ) {
			glTexParameteri( mTarget, GL_TEXTURE_COMPARE_MODE_EXT, format.mCompareMode );
		}
		else {
			CI_LOG_E("This device doesn't support GL_TEXTURE_COMPARE_MODE from EXT_shadow_samplers");
		}
	}
	if( format.mCompareFunc > -1 ) {
		if( supportsShadowSampler() ) {
			glTexParameteri( mTarget, GL_TEXTURE_COMPARE_FUNC_EXT, format.mCompareFunc );
		}
		else {
			CI_LOG_E("This device doesn't support GL_TEXTURE_COMPARE_FUNC from EXT_shadow_samplers");
		}
	}
#endif
	mSwizzleMask = format.mSwizzleMask;
	
	mMipmapping = format.mMipmapping;

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
	return mInternalFormat;
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
#if ! defined( CINDER_GL_ES )
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexParameteri( mTarget, GL_TEXTURE_COMPARE_MODE, compareMode );
#else
	if( supportsShadowSampler() ) {
		ScopedTextureBind tbs( mTarget, mTextureId );
		glTexParameteri( mTarget, GL_TEXTURE_COMPARE_MODE_EXT, compareMode );
	}
	else {
		CI_LOG_E("This device doesn't support GL_TEXTURE_COMPARE_MODE from EXT_shadow_samplers");
	}
#endif
}
	
void TextureBase::setCompareFunc( GLenum compareFunc )
{
#if ! defined( CINDER_GL_ES )
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexParameteri( mTarget, GL_TEXTURE_COMPARE_FUNC, compareFunc );
#else
	if( supportsShadowSampler() ) {
		ScopedTextureBind tbs( mTarget, mTextureId );
		glTexParameteri( mTarget, GL_TEXTURE_COMPARE_FUNC_EXT, compareFunc );
	}
	else {
		CI_LOG_E("This device doesn't support GL_TEXTURE_COMPARE_FUNC from EXT_shadow_samplers");
	}
#endif
}

bool TextureBase::surfaceRequiresIntermediate( int32_t width, int32_t rowBytes, SurfaceChannelOrder surfaceChannelOrder )
{
	if( width * surfaceChannelOrder.getPixelInc() != rowBytes )
		return true;
	
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

void TextureBase::SurfaceChannelOrderToDataFormatAndType( const SurfaceChannelOrder &sco, GLint *dataFormat, GLenum *type )
{
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

Vec2i TextureBase::calcMipLevelSize( int mipLevel, GLint width, GLint height )
{
	width = max( 1, (int)floor( width >> mipLevel ) );
	height = max( 1, (int)floor( height >> mipLevel ) );
	
	return Vec2i( width, height );
}
	
GLfloat TextureBase::getMaxMaxAnisotropy()
{
	GLfloat maxMaxAnisotropy;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxMaxAnisotropy );
	return maxMaxAnisotropy;
}

bool TextureBase::supportsHardwareSwizzle()
{
	#if defined( CINDER_GL_ES_2 )
		return false;
	#else
		static bool supported = ( ( gl::isExtensionAvailable( "GL_EXT_texture_swizzle" ) || gl::getVersion() >= make_pair( 3, 3 ) ) );
		return supported;
	#endif
}

#if defined( CINDER_GL_ES )
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
// Texture::Format
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
	mMaxMipmapLevel = 1000;
	mInternalFormat = -1;
	mMaxAnisotropy = -1.0f;
	mPixelDataFormat = -1;
	mPixelDataType = GL_UNSIGNED_BYTE;
	mSwizzleMask[0] = GL_RED; mSwizzleMask[1] = GL_GREEN; mSwizzleMask[2] = GL_BLUE; mSwizzleMask[3] = GL_ALPHA;
	mCompareMode = -1;
	mCompareFunc = -1;	
}

void TextureBase::Format::setSwizzleMask( GLint r, GLint g, GLint b, GLint a )
{
	array<GLint,4> swizzleMask;
	swizzleMask[0] = r; swizzleMask[1] = g; swizzleMask[2] = b; swizzleMask[3] = a;
	setSwizzleMask( swizzleMask );
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

/////////////////////////////////////////////////////////////////////////////////
// Texture
TextureRef Texture::create( int width, int height, Format format )
{
	return TextureRef( new Texture( width, height, format ) );
}

TextureRef Texture::create( const unsigned char *data, int dataFormat, int width, int height, Format format )
{
	return TextureRef( new Texture( data, dataFormat, width, height, format ) );
}

TextureRef Texture::create( const Surface8u &surface, Format format )
{
	return TextureRef( new Texture( surface, format ) );
}

TextureRef Texture::create( const Surface32f &surface, Format format )
{
	return TextureRef( new Texture( surface, format ) );
}

TextureRef Texture::create( const Channel8u &channel, Format format )
{
	return TextureRef( new Texture( channel, format ) );
}
	
TextureRef Texture::create( const Channel32f &channel, Format format )
{
	return TextureRef( new Texture( channel, format ) );
}
	
TextureRef Texture::create( ImageSourceRef imageSource, Format format )
{
	return TextureRef( new Texture( imageSource, format ) );
}
	
TextureRef Texture::create( GLenum target, GLuint textureID, int width, int height, bool doNotDispose )
{
	return TextureRef( new Texture( target, textureID, width, height, doNotDispose ) );
}

TextureRef Texture::create( const TextureData &data, const Format &format )
{
	return TextureRef( new Texture( data, format ) );
}

Texture::Texture( int width, int height, Format format )
	: mWidth( width ), mHeight( height ),
	mCleanWidth( width ), mCleanHeight( height ),
	mFlipped( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, GL_RGBA );
	initData( (unsigned char*)0, 0, format.mPixelDataFormat, format.mPixelDataType, format );
}

Texture::Texture( const unsigned char *data, int dataFormat, int width, int height, Format format )
	: mWidth( width ), mHeight( height ),
	mCleanWidth( width ), mCleanHeight( height ),
	mFlipped( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, GL_RGBA );
	initData( data, 0, dataFormat, GL_UNSIGNED_BYTE, format );
}

Texture::Texture( const Surface8u &surface, Format format )
	: mWidth( surface.getWidth() ), mHeight( surface.getHeight() ),
	mCleanWidth( surface.getWidth() ), mCleanHeight( surface.getHeight() ),
	mFlipped( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, surface.hasAlpha() ? GL_RGBA : GL_RGB );
	
	GLint dataFormat;
	GLenum type;
	SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );
	
	// we need an intermediate format for certain channel orders, and rowBytes != numChannels * width
	if( surfaceRequiresIntermediate( surface.getWidth(), surface.getRowBytes(), surface.getChannelOrder() ) ) {
		CI_LOG_V( "Surface rowBytes or ChannelOrder will prevent full efficiency in gl::Texture upload." );
		Surface8u intermediateSurface( surface.getWidth(), surface.getHeight(), surface.hasAlpha(), surface.hasAlpha() ? SurfaceChannelOrder::RGBA : SurfaceChannelOrder::RGBA );
		intermediateSurface.copyFrom( surface, surface.getBounds() );
		initData( intermediateSurface.getData(), intermediateSurface.getRowBytes() / intermediateSurface.getChannelOrder().getPixelInc(),
			dataFormat, type, format );
	}
	else
		initData( surface.getData(), surface.getRowBytes() / surface.getChannelOrder().getPixelInc(), dataFormat, type, format );
}

Texture::Texture( const Surface32f &surface, Format format )
	: mWidth( surface.getWidth() ), mHeight( surface.getHeight() ),
	mCleanWidth( surface.getWidth() ), mCleanHeight( surface.getHeight() ),
	mFlipped( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
#if defined( CINDER_GL_ES )
	initParams( format, surface.hasAlpha() ? GL_RGBA : GL_RGB );
#else
	initParams( format, surface.hasAlpha() ? GL_RGBA32F : GL_RGB32F );
#endif
	initData( surface.getData(), surface.hasAlpha()?GL_RGBA:GL_RGB, format );
}

Texture::Texture( const Channel8u &channel, Format format )
	: mWidth( channel.getWidth() ), mHeight( channel.getHeight() ),
	mCleanWidth( channel.getWidth() ), mCleanHeight( channel.getHeight() ),
	mFlipped( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, GL_LUMINANCE );
	
	// if the data is not already contiguous, we'll need to create a block of memory that is
	if( ( channel.getIncrement() != 1 ) || ( channel.getRowBytes() != channel.getWidth() * sizeof( uint8_t ) ) ) {
		shared_ptr<uint8_t> data( new uint8_t[ channel.getWidth() * channel.getHeight() ], checked_array_deleter<uint8_t>() );
		uint8_t* dest		= data.get();
		const int8_t inc	= channel.getIncrement();
		const int32_t width = channel.getWidth();
		for ( int y = 0; y < channel.getHeight(); ++y ) {
			const uint8_t* src = channel.getData( 0, y );
			for ( int x = 0; x < width; ++x ) {
				*dest++	= *src;
				src		+= inc;
			}
		}
		initData( data.get(), channel.getRowBytes() / channel.getIncrement(), GL_LUMINANCE, GL_UNSIGNED_BYTE, format );
	} else {
		initData( channel.getData(), channel.getRowBytes() / channel.getIncrement(), GL_LUMINANCE, GL_UNSIGNED_BYTE, format );
	}
}

Texture::Texture( const Channel32f &channel, Format format )
	: mWidth( channel.getWidth() ), mHeight( channel.getHeight() ),
	mCleanWidth( channel.getWidth() ), mCleanHeight( channel.getHeight() ),
	mFlipped( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, GL_LUMINANCE );
	
	// if the data is not already contiguous, we'll need to create a block of memory that is
	if( ( channel.getIncrement() != 1 ) || ( channel.getRowBytes() != channel.getWidth() * sizeof(float) ) ) {
		shared_ptr<float> data( new float[channel.getWidth() * channel.getHeight()], checked_array_deleter<float>() );
		float* dest			= data.get();
		const int8_t inc	= channel.getIncrement();
		const int32_t width = channel.getWidth();
		for( int y = 0; y < channel.getHeight(); ++y ) {
			const float* src = channel.getData( 0, y );
			for( int x = 0; x < width; ++x ) {
				*dest++ = *src;
				src		+= inc;
			}
		}
		
		initData( data.get(), GL_LUMINANCE, format );
	}
	else {
		initData( channel.getData(), GL_LUMINANCE, format );
	}
}

Texture::Texture( const ImageSourceRef &imageSource, Format format )
	: mWidth( -1 ), mHeight( -1 ), mCleanWidth( -1 ), mCleanHeight( -1 ),
	mFlipped( false )
{
	GLint defaultInternalFormat;	
	// Set the internal format based on the image's color space
	switch( imageSource->getColorModel() ) {
		case ImageIo::CM_RGB:
			defaultInternalFormat = ( imageSource->hasAlpha() ) ? GL_RGBA : GL_RGB;
		break;
		case ImageIo::CM_GRAY: {
#if defined( CINDER_GL_ES )
			defaultInternalFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE_ALPHA : GL_LUMINANCE;
#else
			defaultInternalFormat = ( imageSource->hasAlpha() ) ?  GL_RG : GL_RED;
			std::array<int,4> swizzleMask = { GL_RED, GL_RED, GL_RED, GL_GREEN };
			if( defaultInternalFormat == GL_RED )
				swizzleMask[3] = GL_ONE;
			format.setSwizzleMask( swizzleMask );
#endif
		} break;
		default:
			throw ImageIoExceptionIllegalColorModel( "Unsupported color model for gl::Texture construction." );
		break;
	}

	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, defaultInternalFormat );
	initData( imageSource, format );
}

Texture::Texture( GLenum target, GLuint textureId, int width, int height, bool doNotDispose )
	: TextureBase( target, textureId, -1 ), mWidth( width ), mHeight( height ),
	mCleanWidth( width ), mCleanHeight( height ),
	mFlipped( false )
{
	mDoNotDispose = doNotDispose;
	if( mTarget == GL_TEXTURE_2D ) {
		mMaxU = mMaxV = 1.0f;
	}
	else {
		mMaxU = (float)mWidth;
		mMaxV = (float)mHeight;
	}
}

Texture::Texture( const TextureData &data, Format format )
	: mFlipped( false )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	initParams( format, 0 /* unused */ );
	
	replace( data );

	if( format.mMipmapping && data.getNumLevels() <= 1 ) {
#if ! defined( CINDER_GL_ES )	
		glTexParameteri( mTarget, GL_TEXTURE_BASE_LEVEL, format.mBaseMipmapLevel );
		glTexParameteri( mTarget, GL_TEXTURE_MAX_LEVEL, format.mMaxMipmapLevel );
#endif		
		glGenerateMipmap( mTarget );
	}
}
	
void Texture::initData( const unsigned char *data, int unpackRowLength, GLenum dataFormat, GLenum type, const Format &format )
{
	ScopedTextureBind tbs( mTarget, mTextureId );
	
	if( mTarget == GL_TEXTURE_2D ) {
		mMaxU = mMaxV = 1.0f;
	}
	else {
		mMaxU = (float)mWidth;
		mMaxV = (float)mHeight;
	}
	
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexImage2D( mTarget, 0, mInternalFormat, mWidth, mHeight, 0, dataFormat, type, data );
    
	if( format.mMipmapping ) {
#if ! defined( CINDER_GL_ES )
		glTexParameteri( mTarget, GL_TEXTURE_BASE_LEVEL, format.mBaseMipmapLevel );
		glTexParameteri( mTarget, GL_TEXTURE_MAX_LEVEL, format.mMaxMipmapLevel );
#endif
		glGenerateMipmap( mTarget );
	}
}

void Texture::initData( const float *data, GLint dataFormat, const Format &format )
{
	ScopedTextureBind tbs( mTarget, mTextureId );
	
	if( mTarget == GL_TEXTURE_2D ) {
		mMaxU = mMaxV = 1.0f;
	}
	else {
		mMaxU = (float)mWidth;
		mMaxV = (float)mHeight;
	}
	
	if( data ) {
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glTexImage2D( mTarget, 0, mInternalFormat, mWidth, mHeight, 0, dataFormat, GL_FLOAT, data );
	}
	else {
		glTexImage2D( mTarget, 0, mInternalFormat, mWidth, mHeight, 0, GL_LUMINANCE, GL_FLOAT, 0 );  // init to black...
	}
    
	if( format.mMipmapping ) {
#if ! defined( CINDER_GL_ES )
		glTexParameteri( mTarget, GL_TEXTURE_BASE_LEVEL, format.mBaseMipmapLevel );
		glTexParameteri( mTarget, GL_TEXTURE_MAX_LEVEL, format.mMaxMipmapLevel );
#endif
		glGenerateMipmap( mTarget );
	}
}

#if ! defined( CINDER_GL_ES )
// Called by initData( ImageSourceRef ) when the user has supplied an intermediate PBO via Format
// We map the PBO after resizing it if necessary, and then use that as a data store for the ImageTargetGlTexture
void Texture::initDataImageSourceWithPboImpl( const ImageSourceRef &imageSource, const Format &format, GLint dataFormat, ImageIo::ChannelOrder channelOrder, bool isGray, const PboRef &pbo )
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
		glTexImage2D( mTarget, 0, mInternalFormat, mWidth, mHeight, 0, dataFormat, GL_UNSIGNED_BYTE, nullptr );
	}
	else if( imageSource->getDataType() == ImageIo::UINT16 ) {
		auto target = ImageTargetGlTexture<uint16_t>::create( this, channelOrder, isGray, imageSource->hasAlpha(), pboData );
		imageSource->load( target );
		pbo->unmap();
		glTexImage2D( mTarget, 0, mInternalFormat, mWidth, mHeight, 0, dataFormat, GL_UNSIGNED_SHORT, nullptr );
		
	}
	else {
		auto target = ImageTargetGlTexture<float>::create( this, channelOrder, isGray, imageSource->hasAlpha(), pboData );
		imageSource->load( target );
		pbo->unmap();		
		glTexImage2D( mTarget, 0, mInternalFormat, mWidth, mHeight, 0, dataFormat, GL_FLOAT, nullptr );
	}
	
	ctx->popBufferBinding( GL_PIXEL_UNPACK_BUFFER );
}
#endif

// Called by initData( ImageSourceRef ) when the user has NOT supplied an intermediate PBO
void Texture::initDataImageSourceImpl( const ImageSourceRef &imageSource, const Format &format, GLint dataFormat, ImageIo::ChannelOrder channelOrder, bool isGray )
{
	if( imageSource->getDataType() == ImageIo::UINT8 ) {
		auto target = ImageTargetGlTexture<uint8_t>::create( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		glTexImage2D( mTarget, 0, mInternalFormat, mWidth, mHeight, 0, dataFormat, GL_UNSIGNED_BYTE, target->getData() );
	}
	else if( imageSource->getDataType() == ImageIo::UINT16 ) {
		auto target = ImageTargetGlTexture<uint16_t>::create( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		glTexImage2D( mTarget, 0, mInternalFormat, mWidth, mHeight, 0, dataFormat, GL_UNSIGNED_SHORT, target->getData() );
		
	}
	else {
		auto target = ImageTargetGlTexture<float>::create( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		glTexImage2D( mTarget, 0, mInternalFormat, mWidth, mHeight, 0, dataFormat, GL_FLOAT, target->getData() );
	}
}

void Texture::initData( const ImageSourceRef &imageSource, const Format &format )
{
	mWidth = mCleanWidth = imageSource->getWidth();
	mHeight = mCleanHeight = imageSource->getHeight();
	
	// setup an appropriate dataFormat/ImageTargetTexture based on the image's color space
	GLint dataFormat;
	ImageIo::ChannelOrder channelOrder;
	bool isGray = false;
	switch( imageSource->getColorModel() ) {
		case ImageSource::CM_RGB:
			dataFormat = ( imageSource->hasAlpha() ) ? GL_RGBA : GL_RGB;
			channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::RGBA : ImageIo::RGB;
		break;
		case ImageSource::CM_GRAY:
			dataFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE_ALPHA : GL_LUMINANCE;
			channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::YA : ImageIo::Y;
			isGray = true;
		break;
		default: // if this is some other color space, we'll have to punt and go w/ RGB
			dataFormat = ( imageSource->hasAlpha() ) ? GL_RGBA : GL_RGB;
			channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::RGBA : ImageIo::RGB;
		break;
	}
	
	ScopedTextureBind tbs( mTarget, mTextureId );	
	if( mTarget == GL_TEXTURE_2D ) {
		mMaxU = mMaxV = 1.0f;
	}
	else {
		mMaxU = (float)mWidth;
		mMaxV = (float)mHeight;
	}
	
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
#if ! defined( CINDER_GL_ES )
	auto pbo = format.getIntermediatePbo();
	if( pbo )
		initDataImageSourceWithPboImpl( imageSource, format, dataFormat, channelOrder, isGray, pbo );
	else {
		initDataImageSourceImpl( imageSource, format, dataFormat, channelOrder, isGray );
	}	
#else
	initDataImageSourceImpl( imageSource, format, dataFormat, channelOrder, isGray );
#endif	
	if( format.mMipmapping ) {
#if ! defined( CINDER_GL_ES )
		glTexParameteri( mTarget, GL_TEXTURE_BASE_LEVEL, format.mBaseMipmapLevel );
		glTexParameteri( mTarget, GL_TEXTURE_MAX_LEVEL, format.mMaxMipmapLevel );
#endif
		glGenerateMipmap( mTarget );
	}
}

void Texture::update( const Surface &surface, int mipLevel )
{
	GLint dataFormat;
	GLenum type;
	if( mipLevel == 0 ) {
		// we need an intermediate format for certain channel orders, and rowBytes != numChannels * width
		Surface8u sourceSurface;
		if( surfaceRequiresIntermediate( surface.getWidth(), surface.getRowBytes(), surface.getChannelOrder() ) 
			|| ( surface.getWidth() != getWidth() ) || ( surface.getHeight() != getHeight() ) )
		{
			CI_LOG_V( "Surface size, rowBytes or ChannelOrder will prevent full efficiency in gl::Texture upload." );
			sourceSurface = Surface8u( getWidth(), getHeight(), surface.hasAlpha(), surface.hasAlpha() ? SurfaceChannelOrder::RGBA : SurfaceChannelOrder::RGBA );
			sourceSurface.copyFrom( surface, sourceSurface.getBounds() );
		}
		else
			sourceSurface = surface;

		SurfaceChannelOrderToDataFormatAndType( sourceSurface.getChannelOrder(), &dataFormat, &type );

		ScopedTextureBind tbs( mTarget, mTextureId );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glTexSubImage2D( mTarget, mipLevel, 0, 0, getWidth(), getHeight(), dataFormat, type, sourceSurface.getData() );
	}
	else {
		SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );
		
		Vec2i mipMapSize = calcMipLevelSize( mipLevel, getWidth(), getHeight() );
		
		ScopedTextureBind tbs( mTarget, mTextureId );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glTexSubImage2D( mTarget, mipLevel, 0, 0, mipMapSize.x, mipMapSize.y, dataFormat, type, surface.getData() );
	}
}

void Texture::update( const Surface32f &surface, int mipLevel )
{
	GLint dataFormat;
	GLenum type;
	if( mipLevel == 0 ) {
		SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );
		if( ( surface.getWidth() != getWidth() ) || ( surface.getHeight() != getHeight() ) )
			throw TextureResizeExc( "Invalid Texture::update() surface dimensions", surface.getSize(), getSize() );

		ScopedTextureBind tbs( mTarget, mTextureId );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		// @TODO: type does not seem to be pulling out the right value..
		glTexImage2D( mTarget, mipLevel, getInternalFormat(), getWidth(), getHeight(), 0, dataFormat, GL_FLOAT, surface.getData() );
	}
	else {
		SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );
		
		Vec2i mipMapSize = calcMipLevelSize( mipLevel, getWidth(), getHeight() );
		
		ScopedTextureBind tbs( mTarget, mTextureId );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		// @TODO: type does not seem to be pulling out the right value..
		glTexImage2D( mTarget, mipLevel, getInternalFormat(), mipMapSize.x, mipMapSize.y, 0, dataFormat, GL_FLOAT, surface.getData() );
	}
}

void Texture::update( const Surface &surface, const Area &area, int mipLevel )
{
	GLint dataFormat;
	GLenum type;

	if( mipLevel == 0 ) {
		SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );
		
		ScopedTextureBind tbs( mTarget, mTextureId );
		glTexSubImage2D( mTarget, mipLevel, area.getX1(), area.getY1(), area.getWidth(), area.getHeight(), dataFormat, type, surface.getData( area.getUL() ) );
	}
	else {
		SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );
		
		Vec2i mipMapSize = calcMipLevelSize( mipLevel, area.getWidth(), area.getHeight() );
		
		ScopedTextureBind tbs( mTarget, mTextureId );
		glTexSubImage2D( mTarget, mipLevel, area.getX1(), area.getY1(), mipMapSize.x, mipMapSize.y, dataFormat, type, surface.getData( area.getUL() ) );
	}
}

void Texture::update( const Channel32f &channel, int mipLevel )
{

	if( mipLevel == 0 ) {
		if( ( channel.getWidth() != getWidth() ) || ( channel.getHeight() != getHeight() ) )
			throw TextureResizeExc( "Invalid Texture::update() channel dimensions", channel.getSize(), getSize() );

		ScopedTextureBind tbs( mTarget, mTextureId );
		glTexSubImage2D( mTarget, mipLevel, 0, 0, getWidth(), getHeight(), GL_LUMINANCE, GL_FLOAT, channel.getData() );
	}
	else {
		
		Vec2i mipMapSize = calcMipLevelSize( mipLevel, getWidth(), getHeight() );
		
		ScopedTextureBind tbs( mTarget, mTextureId );
		glTexSubImage2D( mTarget, mipLevel, 0, 0, mipMapSize.x, mipMapSize.y, GL_LUMINANCE, GL_FLOAT, channel.getData() );
	}
}

void Texture::update( const Channel8u &channel, const Area &area, int mipLevel )
{
	ScopedTextureBind tbs( mTarget, mTextureId );
	if( mipLevel == 0 ) {
		// if the data is not already contiguous, we'll need to create a block of memory that is
		if( ( channel.getIncrement() != 1 ) || ( channel.getRowBytes() != channel.getWidth() * sizeof(uint8_t) ) ) {
			shared_ptr<uint8_t> data( new uint8_t[area.getWidth() * area.getHeight()], checked_array_deleter<uint8_t>() );
			uint8_t* dest		= data.get();
			const int8_t inc	= channel.getIncrement();
			const int32_t width	= area.getWidth();
			for ( int y = 0; y < area.getHeight(); ++y ) {
				const uint8_t *src = channel.getData( area.getX1(), area.getY1() + y );
				for( int x = 0; x < width; ++x ) {
					*dest++	= *src;
					src		+= inc;
				}
			}
			
			glTexSubImage2D( mTarget, mipLevel, area.getX1(), area.getY1(), area.getWidth(), area.getHeight(), GL_LUMINANCE, GL_UNSIGNED_BYTE, data.get() );
		}
		else {
			glTexSubImage2D( mTarget, mipLevel, area.getX1(), area.getY1(), area.getWidth(), area.getHeight(), GL_LUMINANCE, GL_UNSIGNED_BYTE, channel.getData( area.getUL() ) );
		}
	}
	else {
		Vec2i mipMapSize = calcMipLevelSize( mipLevel, area.getWidth(), area.getHeight() );
		
		// if the data is not already contiguous, we'll need to create a block of memory that is
		if( ( channel.getIncrement() != 1 ) || ( channel.getRowBytes() != channel.getWidth() * sizeof(uint8_t) ) ) {
			shared_ptr<uint8_t> data( new uint8_t[area.getWidth() * area.getHeight()], checked_array_deleter<uint8_t>() );
			uint8_t* dest		= data.get();
			const int8_t inc	= channel.getIncrement();
			const int32_t width	= area.getWidth();
			for ( int y = 0; y < area.getHeight(); ++y ) {
				const uint8_t *src = channel.getData( area.getX1(), area.getY1() + y );
				for( int x = 0; x < width; ++x ) {
					*dest++	= *src;
					src		+= inc;
				}
			}
			
			glTexSubImage2D( mTarget, mipLevel, area.getX1(), area.getY1(), mipMapSize.x, mipMapSize.y, GL_LUMINANCE, GL_UNSIGNED_BYTE, data.get() );
		}
		else {
			glTexSubImage2D( mTarget, mipLevel, area.getX1(), area.getY1(), mipMapSize.x, mipMapSize.y, GL_LUMINANCE, GL_UNSIGNED_BYTE, channel.getData( area.getUL() ) );
		}
	}
}

#if ! defined( CINDER_GL_ES )
void Texture::update( const PboRef &pbo, GLenum format, GLenum type, int mipLevel, size_t pboByteOffset )
{
	update( pbo, format, type, getBounds(), mipLevel, pboByteOffset );
}

void Texture::update( const PboRef &pbo, GLenum format, GLenum type, const Area &destArea, int mipLevel, size_t pboByteOffset )
{
	// TODO: warn if PBO's target is wrong
	/*
	CI_ASSERT_ERROR( pbo->getTarget() == GL_PIXEL_UNPACK_BUFFER )
	*/
	
	ScopedBuffer bufScp( (BufferObjRef)( pbo ) );
	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexSubImage2D( mTarget, mipLevel, destArea.getX1(), mHeight - destArea.getY2(), destArea.getWidth(), destArea.getHeight(), format, type, reinterpret_cast<const GLvoid*>( pboByteOffset ) );
}
#endif

int Texture::getNumMipLevels() const
{
	return floor( std::log( std::max( mWidth, mHeight ) ) / std::log(2) ) + 1;
}
	
void Texture::setCleanTexCoords( float maxU, float maxV )
{
	mMaxU = maxU;
	mMaxV = maxV;
	
	if ( mTarget == GL_TEXTURE_2D ) {
		mCleanWidth	= getWidth() * maxU;
		mCleanHeight	= getHeight() * maxV;
	} else {
		mCleanWidth	= (int32_t)maxU;
		mCleanHeight	= (int32_t)maxV;
	}
}

bool Texture::hasAlpha() const
{
	switch( mInternalFormat ) {
		case GL_RGBA:
		case GL_LUMINANCE_ALPHA:
			return true;
		break;
		default:
			return false;
		break;
	}
}

float Texture::getLeft() const
{
	return 0.0f;
}

float Texture::getRight() const
{
	return mMaxU;
}

float Texture::getTop() const
{
	return ( mFlipped ) ? getMaxV() : 0.0f;
}

GLint Texture::getWidth() const
{
	return mWidth;
}

GLint Texture::getHeight() const
{
	return mHeight;
}

GLint Texture::getCleanWidth() const
{
	return mCleanWidth;
}

GLint Texture::getCleanHeight() const
{
	return mCleanHeight;
}

Rectf Texture::getAreaTexCoords( const Area &area ) const
{
	Rectf result;
	
	if ( mTarget == GL_TEXTURE_2D ) {
		result.x1 = area.x1 / (float)getWidth();
		result.x2 = area.x2 / (float)getWidth();
		result.y1 = area.y1 / (float)getHeight();
		result.y2 = area.y2 / (float)getHeight();
	} else {
		result = Rectf( area );
	}
	
	if ( mFlipped ) {
		std::swap( result.y1, result.y2 );
	}
	
	return result;
}

float Texture::getBottom() const
{
	return ( mFlipped ) ? 0.0f : getMaxV();
}

float Texture::getMaxU() const
{
	return mMaxU;
}

float Texture::getMaxV() const
{
	return mMaxV;
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

void Texture::printDims( std::ostream &os ) const
{
	os << mWidth << " x " << mHeight;
}

/////////////////////////////////////////////////////////////////////////////////
// TextureCache

TextureCacheRef TextureCache::create()
{
	return TextureCacheRef( new TextureCache() );
}
	
TextureCacheRef TextureCache::create( const Surface8u &prototypeSurface, const Texture::Format &format )
{
	return TextureCacheRef( new TextureCache( prototypeSurface, format ) );
}
	
TextureCache::TextureCache()
{
}
	
TextureCache::TextureCache( const Surface8u &prototypeSurface, const Texture::Format &format )
	: mWidth( prototypeSurface.getWidth() ), mHeight( prototypeSurface.getHeight() ),
	mFormat( format ), mNextId( 0 )
{
	if( mWidth * prototypeSurface.getChannelOrder().getPixelInc() != prototypeSurface.getRowBytes() ) {
		CI_LOG_V( "Surface rowBytes will prevent full efficiency in gl::Texture upload." );
		mIntermediateSurface = Surface8u( prototypeSurface.getWidth(), prototypeSurface.getHeight(), 
				prototypeSurface.hasAlpha(), prototypeSurface.getChannelOrder() );
	}
}

gl::TextureRef TextureCache::cache( const Surface8u &originalData )
{
	Surface8u surfaceData = originalData;
	// If mIntermediateSurface isn't null then we need to use that instead.
	// This is to accommodate rowBytes values which aren't the same as width * bytesPerPixel
	if( mIntermediateSurface ) {
		mIntermediateSurface.copyFrom( originalData, originalData.getBounds() );
		surfaceData = mIntermediateSurface;
	}

	// find an available slot and update that if possible
	for( vector<pair<int,TextureRef>>::iterator texIt = mTextures.begin(); texIt != mTextures.end(); ++texIt ) {
		if( texIt->first == -1 ) { // this texture is available, let's use it!
			texIt->second->update( surfaceData );
			texIt->first = mNextId++;
			// normally this would be very wrong, but when the result TextureRef is destroyed, it calls markTextureAsFree rather than deleting the master texture
			return TextureRef( texIt->second.get(), std::bind( &TextureCache::markTextureAsFree, shared_from_this(), texIt->first ) );
		}
	}
	
	// we didn't find an available slot, so let's make a new texture
	TextureRef masterTex( new Texture( surfaceData, mFormat ) );
	mTextures.push_back( make_pair( mNextId++, masterTex ) );
	// normally this would be very wrong, but when the result TextureRef is destroyed, it calls markTextureAsFree rather than deleting the master texture
	return TextureRef( mTextures.back().second.get(), std::bind( &TextureCache::markTextureAsFree, shared_from_this(), mTextures.back().first ) );
}

void TextureCache::markTextureAsFree( int id )
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
			case GL_LUMINANCE: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT8 ); format = GL_LUMINANCE; break;
			case GL_LUMINANCE_ALPHA: setChannelOrder( ImageIo::YA ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT8 ); format = GL_LUMINANCE_ALPHA; break;
			case GL_RGBA8: setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); format = GL_RGBA; break; 
			case GL_RGB8: setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); format = GL_RGB; break;
			case GL_BGR: setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); format = GL_RGB; break;
			case 0x8040/*GL_LUMINANCE8 in Legacy*/: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT8 ); format = GL_LUMINANCE; break;
			case 0x8045/*GL_LUMINANCE8_ALPHA8 in Legacy*/: setChannelOrder( ImageIo::YA ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT8 ); format = GL_LUMINANCE_ALPHA; break; 
			case GL_DEPTH_COMPONENT16: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT16 ); format = GL_DEPTH_COMPONENT; break;
			case GL_DEPTH_COMPONENT24: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_DEPTH_COMPONENT; break;
			case GL_DEPTH_COMPONENT32: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_DEPTH_COMPONENT; break;
			case GL_RGBA32F_ARB: setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); format = GL_RGBA; break; 
			case GL_RGB32F_ARB: setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); format = GL_RGB; break;
			case GL_R32F: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_RED; break;
			case GL_LUMINANCE32F_ARB: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_LUMINANCE; break;
			case GL_LUMINANCE_ALPHA32F_ARB: setChannelOrder( ImageIo::YA ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_LUMINANCE_ALPHA; break;
			default: setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); format = GL_RGBA; break;
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
	}

	void load( ImageTargetRef target ) {
		// get a pointer to the ImageSource function appropriate for handling our data configuration
		ImageSource::RowFunc func = setupRowFunc( target );
		
		const uint8_t *data = mData.get();
		for( int32_t row = 0; row < mHeight; ++row ) {
			((*this).*func)( target, row, data );
			data += mRowBytes;
		}
	}
	
	std::unique_ptr<uint8_t[]>	mData;
	int32_t						mRowBytes;
};

ImageSourceRef Texture::createSource()
{
	return ImageSourceRef( new ImageSourceTexture( *this ) );
}

#if ! defined( CINDER_GL_ES )
/////////////////////////////////////////////////////////////////////////////////
// Texture3d
Texture3dRef Texture3d::create( GLint width, GLint height, GLint depth, Format format )
{
	return Texture3dRef( new Texture3d( width, height, depth, format ) );
}

Texture3dRef Texture3d::create( GLint width, GLint height, GLint depth, GLenum dataFormat, const uint8_t *data, Format format )
{
	return Texture3dRef( new Texture3d( width, height, depth, dataFormat, data, format ) );
}

Texture3d::Texture3d( GLint width, GLint height, GLint depth, Format format )
	: mWidth( width ), mHeight( height ), mDepth( depth )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	TextureBase::initParams( format, GL_RGB );

	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexImage3D( mTarget, 0, mInternalFormat, mWidth, mHeight, mDepth, 0, format.mPixelDataFormat, format.mPixelDataType, NULL );
}

Texture3d::Texture3d( GLint width, GLint height, GLint depth, GLenum dataFormat, const uint8_t *data, Format format )
	: mWidth( width ), mHeight( height ), mDepth( depth )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );
	TextureBase::initParams( format, GL_RGB );

	glTexImage3D( mTarget, 0, mInternalFormat, mWidth, mHeight, mDepth, 0, dataFormat, GL_UNSIGNED_BYTE, data );
}

void Texture3d::update( const Surface &surface, int depth, int mipLevel )
{
	GLint dataFormat;
	GLenum type;
	SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );
		
	Vec2i mipMapSize = calcMipLevelSize( mipLevel, getWidth(), getHeight() );
	if( surface.getSize() != mipMapSize )
		throw TextureResizeExc( "Invalid Texture::update() surface dimensions", surface.getSize(), mipMapSize );

	ScopedTextureBind tbs( mTarget, mTextureId );
	glTexSubImage3D( mTarget, mipLevel,
		0, 0, depth, // offsets
		mipMapSize.x, mipMapSize.y, 1, dataFormat, type, surface.getData() );
}

void Texture3d::printDims( std::ostream &os ) const
{
	os << mWidth << " x " << mHeight << " x " << mDepth;
}

#endif // ! defined( CINDER_GL_ES )

/////////////////////////////////////////////////////////////////////////////////
// TextureCubeMap
TextureCubeMap::Format::Format()
	: TextureBase::Format()
{
	mTarget = GL_TEXTURE_CUBE_MAP;
	mMinFilter = GL_NEAREST;
	mMagFilter = GL_NEAREST;	
}

TextureCubeMapRef TextureCubeMap::create( int32_t width, int32_t height, const Format &format )
{
	return TextureCubeMapRef( new TextureCubeMap( width, height, format ) );
}

TextureCubeMapRef TextureCubeMap::createHorizontalCross( const ImageSourceRef &imageSource, const Format &format )
{
	Vec2i faceSize( imageSource->getWidth() / 4, imageSource->getHeight() / 3 );
	Area faceArea( 0, 0, faceSize.x, faceSize.y );
	
	Surface8u masterSurface( imageSource, SurfaceConstraintsDefault() );
	
	// allocate the individual face's Surfaces, ensuring rowbytes == faceSize.x * 3 through default SurfaceConstraints
	Surface8u images[6];
	for( uint8_t f = 0; f < 6; ++f )
		images[f] = Surface8u( faceSize.x, faceSize.y, masterSurface.hasAlpha(), SurfaceConstraints() );

	// copy out each individual face
	// GL_TEXTURE_CUBE_MAP_POSITIVE_X
	images[0].copyFrom( masterSurface, faceArea + Vec2i( faceSize.x * 2, faceSize.y * 1 ), -Vec2i( faceSize.x * 2, faceSize.y * 1 ) );
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	images[1].copyFrom( masterSurface, faceArea + Vec2i( faceSize.x * 0, faceSize.y * 1 ), -Vec2i( faceSize.x * 0, faceSize.y * 1 ) );
	// GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	images[2].copyFrom( masterSurface, faceArea + Vec2i( faceSize.x * 1, faceSize.y * 0 ), -Vec2i( faceSize.x * 1, faceSize.y * 0 ) );
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	images[3].copyFrom( masterSurface, faceArea + Vec2i( faceSize.x * 1, faceSize.y * 2 ), -Vec2i( faceSize.x * 1, faceSize.y * 2 ) );
	// GL_TEXTURE_CUBE_MAP_POSITIVE_Z
	images[4].copyFrom( masterSurface, faceArea + Vec2i( faceSize.x * 1, faceSize.y * 1 ), -Vec2i( faceSize.x * 1, faceSize.y * 1 ) );
	// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	images[5].copyFrom( masterSurface, faceArea + Vec2i( faceSize.x * 3, faceSize.y * 1 ), -Vec2i( faceSize.x * 3, faceSize.y * 1 ) );
		
	return TextureCubeMapRef( new TextureCubeMap( images, format ) );
}

TextureCubeMapRef TextureCubeMap::create( const ImageSourceRef images[6], const Format &format )
{
	Surface8u surfaces[6];
	for( size_t i = 0; i < 6; ++i )
		surfaces[i] = Surface8u( images[i] );
	
	return TextureCubeMapRef( new TextureCubeMap( surfaces, format ) );
}

TextureCubeMap::TextureCubeMap( int32_t width, int32_t height, Format format )
	: mWidth( width ), mHeight( height )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );	
	TextureBase::initParams( format, GL_RGB );

	for( GLenum target = 0; target < 6; ++target )
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + target, 0, mInternalFormat, mWidth, mHeight, 0, format.mPixelDataFormat, format.mPixelDataType, NULL );
}

TextureCubeMap::TextureCubeMap( const Surface8u images[6], Format format )
{
	glGenTextures( 1, &mTextureId );
	mTarget = format.getTarget();
	ScopedTextureBind texBindScope( mTarget, mTextureId );	
	TextureBase::initParams( format, ( images[0].hasAlpha() ) ? GL_RGBA : GL_RGB );

	mWidth = images[0].getWidth();
	mHeight = images[0].getHeight();

	for( GLenum target = 0; target < 6; ++target )
		glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + target, 0, mInternalFormat, images[target].getWidth(), images[target].getHeight(), 0,
			( images[target].hasAlpha() ) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, images[target].getData() );
			
	if( format.mMipmapping ) {
#if ! defined( CINDER_GL_ES )
		glTexParameteri( mTarget, GL_TEXTURE_BASE_LEVEL, format.mBaseMipmapLevel );
		glTexParameteri( mTarget, GL_TEXTURE_MAX_LEVEL, format.mMaxMipmapLevel );
#endif
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
		mData = mDataStore.get();
	}
	else
		mData = reinterpret_cast<T*>( intermediateDataStore );
	
	if( std::is_same<T,uint8_t>::value ) {
		setDataType( ImageIo::UINT8 );
	}
	else if( std::is_same<T,uint16_t>::value ) {
		setDataType( ImageIo::UINT16 );
	}
	else if( std::is_same<T,float>::value ) {
		setDataType( ImageIo::FLOAT32 );
	}
	
	setChannelOrder( channelOrder );
	setColorModel( isGray ? ImageIo::CM_GRAY : ImageIo::CM_RGB );
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
	mWidth = mHeight = mDepth = 0;
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

TextureRef Texture::createFromKtx( const DataSourceRef &dataSource, const Format &format )
{
#if ! defined( CINDER_GL_ES )
	TextureData textureData( format.getIntermediatePbo() );
#else
	TextureData textureData;
#endif

	parseKtx( dataSource, &textureData );
	return Texture::create( textureData, format );
}

#if defined( CINDER_GL_ES )
void Texture::updateFromKtx( const DataSourceRef &dataSource )
#else
void Texture::updateFromKtx( const DataSourceRef &dataSource, const PboRef &intermediatePbo )
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

void Texture::update( const TextureData &textureData )
{
	if( textureData.getWidth() != mWidth || textureData.getHeight() != mHeight )
		replace( textureData );
	else {
		ScopedTextureBind bindScope( mTarget, mTextureId );
		if( textureData.getUnpackAlignment() != 0 )
			glPixelStorei( GL_UNPACK_ALIGNMENT, textureData.getUnpackAlignment() );

		int curLevel = 0;
		for( const auto &textureDataLevel : textureData.getLevels() ) {		
			if( textureData.getDataType() != 0 )
				glTexSubImage2D( mTarget, curLevel, 0, 0, textureDataLevel.width, textureDataLevel.height, textureData.getDataFormat(), textureData.getDataType(), textureData.getDataStorePtr( textureDataLevel.offset ) );
			else
				glCompressedTexSubImage2D( mTarget, curLevel, 0, 0, textureDataLevel.width, textureDataLevel.height, textureData.getInternalFormat(), textureDataLevel.dataSize, textureData.getDataStorePtr( textureDataLevel.offset ) );
			++curLevel;
		}
		if( textureData.getUnpackAlignment() != 0 )
			glPixelStorei( GL_UNPACK_ALIGNMENT, textureData.getUnpackAlignment() );
	}
}

void Texture::replace( const TextureData &textureData )
{
	mCleanWidth = mWidth = textureData.getWidth();
	mCleanHeight = mHeight = textureData.getHeight();
	mInternalFormat = textureData.getInternalFormat();

	if( mTarget == GL_TEXTURE_2D ) {
		mMaxU = mMaxV = 1.0f;
	}
	else {
		mMaxU = (float)mWidth;
		mMaxV = (float)mHeight;
	}
	
	ScopedTextureBind bindScope( mTarget, mTextureId );
	if( textureData.getUnpackAlignment() != 0 )
		glPixelStorei( GL_UNPACK_ALIGNMENT, textureData.getUnpackAlignment() );

	int curLevel = 0;
	for( const auto &textureDataLevel : textureData.getLevels() ) {		
		if( textureData.getDataType() != 0 )
			glTexImage2D( mTarget, curLevel, textureData.getInternalFormat(), textureDataLevel.width, textureDataLevel.height, 0, textureData.getDataFormat(), textureData.getDataType(), textureData.getDataStorePtr( textureDataLevel.offset ) );
		else
			glCompressedTexImage2D( mTarget, curLevel, textureData.getInternalFormat(), textureDataLevel.width, textureDataLevel.height, 0, textureDataLevel.dataSize, textureData.getDataStorePtr( textureDataLevel.offset ) );
		++curLevel;
	}
	if( textureData.getUnpackAlignment() != 0 )
		glPixelStorei( GL_UNPACK_ALIGNMENT, textureData.getUnpackAlignment() );
}

#if ! defined( CINDER_GL_ES ) || defined( CINDER_GL_ANGLE )
TextureRef Texture::createFromDds( const DataSourceRef &dataSource, const Format &format )
{
#if ! defined( CINDER_GL_ES )
	TextureData textureData( format.getIntermediatePbo() );
#else
	TextureData textureData;
#endif

	parseDds( dataSource, &textureData );
	return Texture::create( textureData, format );
}

#if defined( CINDER_GL_ANGLE )
void Texture::updateFromDds( const DataSourceRef &dataSource )
#else
void Texture::updateFromDds( const DataSourceRef &dataSource, const PboRef &intermediatePbo )
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

TextureResizeExc::TextureResizeExc( const string &message, const Vec2i &updateSize, const Vec2i &textureSize )
	 : TextureDataExc( "" )
{
	stringstream ss;
	ss << message << ", update size: " << updateSize << ", texture size: " << textureSize << ")";
	mMessage = ss.str();
}

} } // namespace cinder::gl
