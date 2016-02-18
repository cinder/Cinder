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

#include "cinder/vk/ImageView.h"
#include "cinder/Channel.h"
#include "cinder/DataSource.h"
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"

namespace cinder { namespace vk {

class ImageView;
using ImageViewRef = std::shared_ptr<ImageView>;

class Texture2d;
class TextureCubeMap;
using Texture2dRef = std::shared_ptr<Texture2d>;
using TextureCubeMapRef = std::shared_ptr<TextureCubeMap>;

//! \class TextureBase
//!
//!
class TextureBase {
public:

	//! \class Format
	//!
	class Format {
	public:

		Format( VkFormat format = VK_FORMAT_UNDEFINED ) : mInternalFormat( format ) {
			mSwizzle.r = VK_COMPONENT_SWIZZLE_R;
			mSwizzle.g = VK_COMPONENT_SWIZZLE_G;
			mSwizzle.b = VK_COMPONENT_SWIZZLE_B;
			mSwizzle.a = VK_COMPONENT_SWIZZLE_A;
		}

		virtual ~Format() {}

		void					setInternalFormat( VkFormat value ) { mInternalFormat = value; }
		VkFormat				getInternalFormat() const { return mInternalFormat; }

		void					setHostVisible( bool value ) { mHostVisible = value;  }
		bool					getHostVisible() const { return mHostVisible; }

		void					setSamples( VkSampleCountFlagBits value ) { mSamples = value; }
		VkSampleCountFlagBits	getSamples() const { return mSamples; }

		void					setTiling( VkImageTiling value ) { mTiling = value; }
		VkImageTiling			getTiling() const { return mTiling; }
		void					setTilingLinear() { setTiling( VK_IMAGE_TILING_LINEAR ); }
		void					setTilingOptimal() { setTiling( VK_IMAGE_TILING_OPTIMAL ); }
		bool					isTilingLinear() const { return VK_IMAGE_TILING_LINEAR == mTiling; }
		bool					isTilingOptimal() const { return VK_IMAGE_TILING_OPTIMAL == mTiling; }

		Format&					setUsage( VkImageUsageFlags value, bool exclusive = false ) { if( exclusive ) { mUsage = value; } else { mUsage |= value; } }
		VkImageUsageFlags		getUsage() const { return mUsage; }
		void					setUsageTransferSource( bool exclusive = false )         { setUsage( VK_IMAGE_USAGE_TRANSFER_SRC_BIT, exclusive ); }
		void					setUsageTransferDestination( bool exclusive = false )    { setUsage( VK_IMAGE_USAGE_TRANSFER_DST_BIT, exclusive ); }
		void					setUsageSampled( bool exclusive = false )                { setUsage( VK_IMAGE_USAGE_SAMPLED_BIT, exclusive ); }
		void					setUsageStorage( bool exclusive = false )                { setUsage( VK_IMAGE_USAGE_STORAGE_BIT, exclusive ); }
		void					setUsageColorAttachment( bool exclusive = false )        { setUsage( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, exclusive ); }
		void					setUsageDepthStencilAttachment( bool exclusive = false ) { setUsage( VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, exclusive ); }
		void					setUsageTransientAttachment( bool exclusive = false )    { setUsage( VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, exclusive ); }
		void					setUsageInputAttachment( bool exclusive = false )        { setUsage( VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, exclusive ); }
		bool					isUsageTransferSource() const { return VK_IMAGE_USAGE_TRANSFER_SRC_BIT == mUsage; }

		void					setMipmapEnabled( bool value = true ) { mMipmap = value; }
		bool					getMipmapEnabled() const { return mMipmap; }
		void					setMaxMipmapLevels( uint32_t value ) { mMaxMipmapLevels = value; }
		uint32_t				getMaxMipmapLevels() const { return mMaxMipmapLevels; }

		void					setSwizzle( VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a ) { mSwizzle.r = r; mSwizzle.g = g; mSwizzle.b = b; mSwizzle.a = a; }
		void					setSwizzleR( VkComponentSwizzle r ) { mSwizzle.r = r; }
		void					setSwizzleG( VkComponentSwizzle g ) { mSwizzle.g = g; }
		void					setSwizzleB( VkComponentSwizzle b ) { mSwizzle.b = b; }
		void					setSwizzleA( VkComponentSwizzle a ) { mSwizzle.a = a; }

		void					setMagFilter( VkFilter value ) { mMagFilter = value; }
		void					setMinFilter( VkFilter value ) { mMinFilter = value; }

		void					setCompareOp( VkCompareOp value ) { mCompareOp = value; mCompareEnabled = VK_TRUE; }
		void					setCompareMode( VkCompareOp value ) { return setCompareOp( value ); }

	private:
		VkFormat				mInternalFormat = VK_FORMAT_UNDEFINED;
		bool					mHostVisible = false;

		VkSampleCountFlagBits	mSamples = VK_SAMPLE_COUNT_1_BIT;
		VkImageTiling			mTiling  = VK_IMAGE_TILING_OPTIMAL;
		VkImageUsageFlags		mUsage   = VK_IMAGE_USAGE_SAMPLED_BIT;
		VkComponentMapping		mSwizzle;

		bool					mMipmap = false;
		VkSamplerMipmapMode		mMipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		uint32_t				mMaxMipmapLevels = 0;

		VkFilter				mMagFilter = VK_FILTER_LINEAR;
		VkFilter				mMinFilter = VK_FILTER_LINEAR;

		VkBool32				mCompareEnabled = VK_FALSE;
		VkCompareOp				mCompareOp = VK_COMPARE_OP_NEVER;

		friend class TextureBase;
		friend class Texture2d;
	};

	// ---------------------------------------------------------------------------------------------

	TextureBase();
	virtual ~TextureBase();

	const vk::ImageViewRef&			getImageView() const { return mImageView; }
	const VkDescriptorImageInfo&	getImageInfo() const { return mImageInfo; }

protected:
	vk::ImageViewRef				mImageView;

	VkFormatProperties				mFormatProperties;
	bool							mHostVisible = false;

	VkSampler						mSampler = 0;
	VkDescriptorImageInfo			mImageInfo;
	uint32_t						mMipLevels = 1;
};

//! \class Texture2d
//!
//!
class Texture2d : public TextureBase {
public:

	class Format : public TextureBase::Format {
	public:

		Format( VkFormat format = VK_FORMAT_UNDEFINED ) : TextureBase::Format( format ) {
			mSwizzle.r = VK_COMPONENT_SWIZZLE_R;
			mSwizzle.g = VK_COMPONENT_SWIZZLE_G;
			mSwizzle.b = VK_COMPONENT_SWIZZLE_B;
			mSwizzle.a = VK_COMPONENT_SWIZZLE_A;
		}

		virtual ~Format() {}

		Texture2d::Format&			samples( VkSampleCountFlagBits value ) { TextureBase::Format::setSamples( value ); return *this; }

		Texture2d::Format&			mipmap( bool value = true ) { TextureBase::Format::setMipmapEnabled( value ); return *this; }

		Texture2d::Format&			setSampleAddressModeU( VkSamplerAddressMode uValue, VkSamplerAddressMode vValues ) { mSamplerAddressModeU = uValue; mSamplerAddressModeV = vValues; return *this; }
		Texture2d::Format&			setSampleAddressModeU( VkSamplerAddressMode value ) { mSamplerAddressModeU = value; return *this; }
		Texture2d::Format&			setSampleAddressModeV( VkSamplerAddressMode value ) { mSamplerAddressModeV = value; return *this; }
		Texture2d::Format&			setWrap( VkSamplerAddressMode uValue, VkSamplerAddressMode vValue ) { return setSampleAddressModeU( uValue, vValue ); }
		Texture2d::Format&			setWrapU( VkSamplerAddressMode value ) { return setSampleAddressModeU( value ); }
		Texture2d::Format&			setWrapV( VkSamplerAddressMode value ) { return setSampleAddressModeV( value ); }

		Texture2d::Format&			setUnnormalizedCoordinates( VkBool32 value = VK_TRUE ) { mUnnormalizedCoordinates = value; return *this; }
		VkBool32					isUnnormalizedCoordinates() const { return mUnnormalizedCoordinates; }

		Texture2d::Format&			setSwizzle( const VkComponentMapping& value ) { mSwizzle = value; return *this; }
		Texture2d::Format&			setSwizzle( VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a ) { mSwizzle.r = r; mSwizzle.g = g; mSwizzle.b = b; mSwizzle.a = a; return *this; }
		Texture2d::Format&			setSwizzleR( VkComponentSwizzle r ) { mSwizzle.r = r; return *this; }
		Texture2d::Format&			setSwizzleG( VkComponentSwizzle g ) { mSwizzle.g = g; return *this; }
		Texture2d::Format&			setSwizzleB( VkComponentSwizzle b ) { mSwizzle.b = b; return *this; }
		Texture2d::Format&			setSwizzleA( VkComponentSwizzle a ) { mSwizzle.a = a; return *this; }

	private:
		VkSamplerAddressMode		mSamplerAddressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		VkSamplerAddressMode		mSamplerAddressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

		// @TODO: requirements for unnormalizedCoordinates
		//  - minFilter and magFilter must be equal.
		//  – mipmapMode must be VK_SAMPLER_MIPMAP_MODE_NEAREST.
		//  – minLod and maxLod must be zero.
		//  – addressModeU and addressModeV must each be either VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE or VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER.
		//  – anisotropyEnable must be VK_FALSE.
		//  – compareEnable must be VK_FALSE.
		//  – The functions must not use projection.
		//  – The functions must not use offsets.
		VkBool32					mUnnormalizedCoordinates = VK_FALSE;

		VkComponentMapping			mSwizzle;

		friend class Texture2d;
	};


	// ---------------------------------------------------------------------------------------------

	Texture2d();
	Texture2d( int width, int height, const Texture2d::Format &format, vk::Context* context );
	Texture2d( const void *data, VkFormat dataFormat, int width, int height, const Texture2d::Format &format, vk::Context* context );
	Texture2d( const Surface8u& surf, const Texture2d::Format &format, vk::Context* context );
	Texture2d( const Surface16u& surf, const Texture2d::Format &format, vk::Context* context );
	Texture2d( const Surface32f& surf, const Texture2d::Format &format, vk::Context* context );
	//Texture2d( const ImageSourceRef& imageSource, const Texture2d::Format &format, vk::Context* context );
	virtual ~Texture2d();

	// Parameter 'format' will override the format in options
	static Texture2dRef		create( int width, int height, const Texture2d::Format& format = Texture2d::Format(), vk::Context* context = nullptr );
	static Texture2dRef		create( const void *data, VkFormat dataFormat, int width, int height, const Texture2d::Format &format = Texture2d::Format(), vk::Context* context = nullptr );
	static Texture2dRef		create( const Surface8u& surf, const Texture2d::Format& format = Texture2d::Format(), vk::Context* context = nullptr );
	static Texture2dRef		create( const Surface16u& surf, const Texture2d::Format& format = Texture2d::Format(), vk::Context* context = nullptr );
	static Texture2dRef		create( const Surface32f& surf, const Texture2d::Format& format = Texture2d::Format(), vk::Context* context = nullptr );
	//static Texture2dRef		create( ImageSourceRef imageSource, const Format &format = Format(), vk::Context* context = nullptr );

	uint32_t					getWidth() const { return mImageView->getWidth(); }
	uint32_t					getHeight() const { return mImageView->getHeight(); }
	ivec2						getSize() const { return ivec2( mWidth, mHeight ); }
	Area						getBounds() const { return Area( 0, 0, getWidth(), getHeight() ); }
	const Texture2d::Format&	getFormat() const { return mFormat; }

	void					update( int srcWidth, int srcHeight, const uint8_t* srcData, size_t srcRowBytes, size_t srcPixelBytes );
	void					update( int srcWidth, int srcHeight, const uint16_t* srcData, size_t srcRowBytes, size_t srcPixelBytes );
	void					update( int srcWidth, int srcHeight, const float* srcData, size_t srcRowBytes, size_t srcPixelBytes );
	void					update( const Surface8u& surf );
	void					update( const Surface16u& surf );
	void					update( const Surface32f& surf );

private:
	uint32_t				mWidth = 0;
	uint32_t				mHeight = 0;
	Texture2d::Format		mFormat;

	void						initializeCommon( vk::Context* context );
	void						initializeFinal( vk::Context* context );
	void						initialize( vk::Context* context );
	void						initialize( const void *data, VkFormat dataFormat, vk::Context* context );
	template <typename T> void	initialize( const T* srcData, size_t srcRowBytes, size_t srcPixelBytes, vk::Context* context );
	void						initialize( const ImageSourceRef& imageSource, vk::Context* context );
	void						destroy();
	friend class Context;

	template <typename T>
	void doUpdate( int srcWidth, int srcHeight, const T *srcData, size_t srcRowBytes, size_t srcPixelBytes );
};

//! \class Texture2d
//!
//!
class TextureCubeMap : public TextureBase {
public:
	virtual ~TextureCubeMap();

	static TextureCubeMapRef	create( int width, int height, const TextureCubeMap::Format& format = TextureCubeMap::Format(), vk::Context* context = nullptr );
	static TextureCubeMapRef	create( const Surface8u& surf, const TextureCubeMap::Format& format = TextureCubeMap::Format(), vk::Context* context = nullptr );
	static TextureCubeMapRef	create( const Surface16u& surf, const TextureCubeMap::Format& format = TextureCubeMap::Format(), vk::Context* context = nullptr );
	static TextureCubeMapRef	create( const Surface32f& surf, const TextureCubeMap::Format& format = TextureCubeMap::Format(), vk::Context* context = nullptr );
	//static TextureCubeMapRef	create( ImageSourceRef imageSource, const TextureCubeMap::Format &format = TextureCubeMap::Format(), vk::Context* context = nullptr );

private:
	TextureCubeMap( int width, int height, TextureCubeMap::Format format, vk::Context* context );
	TextureCubeMap( const Surface8u& surf, TextureCubeMap::Format format, vk::Context* context );
	TextureCubeMap( const Surface16u& surf, TextureCubeMap::Format format, vk::Context* context );
	TextureCubeMap( const Surface32f& surf, TextureCubeMap::Format format, vk::Context* context );
	//TextureCubeMap( ImageSourceRef imageSource, TextureCubeMap::Format format, vk::Context* context );

	void						initializeCommon( const TextureCubeMap::Format& format, vk::Context* context  );
	void						initializeFinal( vk::Context* context );
	template <typename T> void	initialize( int width, int height, const T* srcData, size_t srcRowBytes, size_t srcPixelBytes, const TextureCubeMap::Format& format, vk::Context* context );
	void						initialize( ImageSourceRef imageSource, const TextureCubeMap::Format& format, vk::Context* context );
    void						destroy();
	friend class Context;
};

// -------------------------------------------------------------------------------------------------
// Mapped type names
// -------------------------------------------------------------------------------------------------
using Texture = Texture2d;
using TextureRef = Texture2dRef;

}} // namespace cinder::vk