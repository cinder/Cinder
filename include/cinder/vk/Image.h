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

#include "cinder/vk/BaseVkObject.h"
#include "cinder/Area.h"

namespace cinder { namespace vk {

class Image;
using ImageRef = std::shared_ptr<Image>;

//! \class Image
//!
//!
class Image : public BaseVkObject {
public:

	//! \class Options
	//!
	//!
	class Format {
	public:
		Format() {} 
		Format( VkFormat format, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT, VkImageTiling tiling = VK_IMAGE_TILING_LINEAR, VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT ) 
			: mInternalFormat( format ), mSamples( samples ), mTiling( tiling ), mUsage( usage ) {}
		virtual ~Format() {}

		static Format			createUndefined( VkFormat format = VK_FORMAT_UNDEFINED ) { return Format( format, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_LINEAR, 0 ); }

		Format&					setInternalFormat( VkFormat value ) { mInternalFormat = value; return *this; }
		VkFormat				getInternalFormat() const { return mInternalFormat; }
		bool					isInternalFormatUndefined() const { return VK_FORMAT_UNDEFINED == mInternalFormat;; }

		Format&					setSamples( VkSampleCountFlagBits value ) { mSamples = value; if( mSamples > VK_SAMPLE_COUNT_1_BIT ) { mTiling = VK_IMAGE_TILING_OPTIMAL; } return *this; }
		VkSampleCountFlagBits	getSamples() const { return mSamples; }

		Format&					setTiling( VkImageTiling value ) { mTiling = value; return *this; }
		VkImageTiling			getTiling() const { return mTiling; }
		Format&					setTilingLinear() { setTiling( VK_IMAGE_TILING_LINEAR ); return *this; }
		Format&					setTilingOptimal() { setTiling( VK_IMAGE_TILING_OPTIMAL ); return *this; }
		bool					isTilingLinear() const { return VK_IMAGE_TILING_LINEAR == mTiling; }
		bool					isTilingOptimal() const { return VK_IMAGE_TILING_OPTIMAL == mTiling; }

		Format&					setUsage( VkImageUsageFlags value, bool exclusive = false ) { if( exclusive ) { mUsage = value; } else { mUsage |= value; } return *this; }
		VkImageUsageFlags		getUsage() const { return mUsage; }
		Format&					setUsageTransferSource( bool exclusive = false )         { setUsage( VK_IMAGE_USAGE_TRANSFER_SRC_BIT, exclusive ); return *this; }
		Format&					setUsageTransferDestination( bool exclusive = false )    { setUsage( VK_IMAGE_USAGE_TRANSFER_DST_BIT, exclusive ); return *this; }
		Format&					setUsageSampled( bool exclusive = false )                { setUsage( VK_IMAGE_USAGE_SAMPLED_BIT, exclusive ); return *this; }
		Format&					setUsageStorage( bool exclusive = false )                { setUsage( VK_IMAGE_USAGE_STORAGE_BIT, exclusive ); return *this; }
		Format&					setUsageColorAttachment( bool exclusive = false )        { setUsage( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, exclusive ); setTilingOptimal(); return *this; }
		Format&					setUsageDepthStencilAttachment( bool exclusive = false ) { setUsage( VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, exclusive ); setTilingOptimal(); return *this; }
		Format&					setUsageTransientAttachment( bool exclusive = false )    { setUsage( VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, exclusive ); return *this; }
		Format&					setUsageInputAttachment( bool exclusive = false )        { setUsage( VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, exclusive ); return *this; }
		bool					isUsageTransferSource() const { return VK_IMAGE_USAGE_TRANSFER_SRC_BIT == mUsage; }

		Format&					setMemoryProperty( VkMemoryPropertyFlags value, bool exclusive = false ) { if( exclusive ) { mMemoryProperty = value; } else { mMemoryProperty |= value; } return *this; }
		VkMemoryPropertyFlags	getMemoryProperty() const { return mMemoryProperty; }
		Format&					setMemoryPropertyDeviceLocal( bool exclusive = false )     { setMemoryProperty( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, exclusive ); return *this; }
		Format&					setMemoryPropertyHostVisible( bool exclusive = false )     { setMemoryProperty( VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, exclusive ); return *this; }
		Format&					setMemoryPropertyHostCoherent( bool exclusive = false )    { setMemoryProperty( VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, exclusive ); return *this; }
		Format&					setMemoryPropertyHostCached( bool exclusive = false )      { setMemoryProperty( VK_MEMORY_PROPERTY_HOST_CACHED_BIT, exclusive ); return *this; }
		Format&					setMemoryPropertyLazilyAllocated( bool exclusive = false ) { setMemoryProperty( VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT, exclusive ); return *this; }

		Format&					setMipLevels( uint32_t value ) { mMipLevels = value; if( mMipLevels > 1 ) { mTiling = VK_IMAGE_TILING_OPTIMAL; } return *this; }
		uint32_t				setMipLevels() const { return mMipLevels; }

		Format&					setArrayLayers( uint32_t value ) { mArrayLayers = value; return *this; }
		uint32_t				getArrayLayers() const { return mArrayLayers; }

		Format&					setImageLayout( VkImageLayout imageLayout ) { mImageLayout  = imageLayout; return *this; }
		VkImageLayout			getImageLayout() const { return mImageLayout; }

		Format&					setCreateFlags( VkImageCreateFlagBits value ) { mCreateFlags |= value; return *this; }
		VkImageCreateFlags		getCreateFlags() const { return mCreateFlags; }

	private:
		VkFormat				mInternalFormat  = VK_FORMAT_UNDEFINED;
		VkSampleCountFlagBits	mSamples = VK_SAMPLE_COUNT_1_BIT;
		VkImageTiling			mTiling  = VK_IMAGE_TILING_LINEAR;
		VkImageUsageFlags		mUsage   = VK_IMAGE_USAGE_SAMPLED_BIT;
		VkMemoryPropertyFlags	mMemoryProperty = 0;
		VkImageLayout			mImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		uint32_t				mMipLevels = 1;
		uint32_t				mArrayLayers = 1;

		VkImageCreateFlags		mCreateFlags = 0;

		friend class Image;
	};

	Image();
	Image( VkImageType imageType, uint32_t width, uint32_t height, uint32_t depth, VkImage image, const Image::Format& options, Context *context );
	Image( uint32_t width, uint32_t height, const uint8_t  *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion, const Image::Format& options, Context *context );
	Image( uint32_t width, uint32_t height, const uint16_t *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion, const Image::Format& options, Context *context );
	Image( uint32_t width, uint32_t height, const float    *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion, const Image::Format& options, Context *context );
	Image( uint32_t width, uint32_t height, const Image::Format& options, Context *context );
	virtual ~Image();

	static ImageRef			create( uint32_t width, const Image::Format& options = Image::Format(), Context *context = nullptr );
	static ImageRef			create( uint32_t width, uint32_t height, const Image::Format& options = Image::Format(), Context *context = nullptr );
	static ImageRef			create( uint32_t width, uint32_t height, uint32_t depth, const Image::Format& options = Image::Format(), Context *context = nullptr );

	// Defaults:
	//    - format is derived from the source data
	//    - samples is 1
	//    - tiling is linear
	//    - usage is sampled image if the devices supports it, otherwise transfer source
	//    - memory is mappable (i.e. host visible)
	//    - mipLevels is 1
	//    - arrayLayers is 1
	//
	// Only format, samples, tiling, and usage (transfer source) have effect from initialOptions, other properties are ignored. 
	// If samples > 1, tiling will be forced to optimal the resulting image is not mappable (i.e. not host visible).
	// If tiling is optimal, the resulting image is not mappable (i.e. not host visible).
	// If usage is transfer source, resulting image will be samples = 1, tilingLinear, and mappable (i.e. host visible).
	static ImageRef			create( int32_t width, int32_t height, const uint8_t  *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion, const Image::Format& initialOptions = Image::Format(), Context *context = nullptr );
	static ImageRef			create( int32_t width, int32_t height, const uint16_t *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion, const Image::Format& initialOptions = Image::Format(), Context *context = nullptr );
	static ImageRef			create( int32_t width, int32_t height, const float    *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion, const Image::Format& initialOptions = Image::Format(), Context *context = nullptr );

	static ImageRef			create( VkImageType imageType, int32_t width, int32_t height, int32_t depth, VkImage image, const Image::Format& options, Context *context = nullptr );

	VkImage					getImage() const { return mImage; }

	int32_t					getWidth()  const { return mExtent.width; }
	int32_t					getHeight() const { return mExtent.height; }
	int32_t					getDepth()  const { return mExtent.depth; }
	const VkExtent3D&		getExtent() const { return mExtent; }

	const Image::Format&	getOptions() const { return mOptions; }
	VkFormat				getFormat() const  { return mOptions.mInternalFormat; }
	VkImageAspectFlags		getAspectMask() const { return mAspectMask; }

	uint32_t				getMipLevels() const { return mOptions.mMipLevels; }
	uint32_t				getArrayLayers() const { return mOptions.mArrayLayers; }

	VkImageLayout			getImageLayout() const { return mOptions.mImageLayout; }
	void					setImageLayout( const VkImageLayout targetLayout ) { mOptions.mImageLayout = targetLayout; }

	bool					isMappable() const { return 0 != ( mOptions.mMemoryProperty & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ); }
	void*					map( VkDeviceSize offset = 0 );
	void					unmap();

	void					copyData( uint32_t dstLayer, const uint8_t  *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion );
	void					copyData( uint32_t dstLayer, const uint16_t *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion );
	void					copyData( uint32_t dstLayer, const float    *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion );

	static void				copy( Context* context, const vk::ImageRef& srcImage, uint32_t srcMipLevel, uint32_t srcLayer, const ivec2& srcOffset, const vk::ImageRef& dstImage, uint32_t dstMipLevel, uint32_t dstLayer, const ivec2& dstOffset, const ivec2& size );
	static void				copy( Context* context, const vk::ImageRef& srcImage, uint32_t srcMipLevel, uint32_t srcLayer, const vk::ImageRef& dstImage, uint32_t dstMipLevel, uint32_t dstLayer, const ivec2& size );
	static void				copy( Context* context, const vk::ImageRef& srcImage, uint32_t srcMipLevel, uint32_t srcLayer, const vk::ImageRef& dstImage, uint32_t dstMipLevel, uint32_t dstLayer );

	static void				blit( Context* context, const vk::ImageRef& srcImage, uint32_t srcMipLevel, uint32_t srcLayer, const ci::Area& srcArea, const vk::ImageRef& dstImage, uint32_t dstMipLevel, uint32_t dstLayer, const ci::Area& dstArea );
	static void				blit( Context* context, const vk::ImageRef& srcImage, uint32_t srcMipLevel, uint32_t srcLayer, const vk::ImageRef& dstImage, uint32_t dstMipLevel, uint32_t dstLayer );

private:
	VkImageType				mImageType;
	VkExtent3D				mExtent;
	Image::Format			mOptions;
	VkFormatProperties		mFormatProperties;
	VkImageAspectFlags		mAspectMask = 0;

	bool					mOwnsImage = false;
	VkImage					mImage = VK_NULL_HANDLE;
	VkDeviceMemory			mMemory = VK_NULL_HANDLE;
	VkDeviceSize			mAllocationSize = 0;
	void*					mMappedAddress = nullptr;

	void initialize();
	void destroy( bool removeFromTracking = true );
	friend class Context;
};

}} // namespace cinder::vk