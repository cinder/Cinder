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

#include "cinder/vk/Image.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/CommandPool.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/wrapper.h"

namespace cinder { namespace vk {

// -------------------------------------------------------------------------------------------------
// Image
// -------------------------------------------------------------------------------------------------
Image::Image()
	: BaseVkObject()
{
}

Image::Image( VkImageType imageType, int32_t width, int32_t height, int32_t depth, VkImage image, const Image::Format& options, Context *context )
	: BaseVkObject( context ),
	  mImageType( imageType ),
	  mExtent( { width, height, depth } ),
	  mOptions( options ),
	  mImage( image )
{
	initialize();
}

Image::Image( int32_t width, int32_t height, const uint8_t *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion, const Image::Format& options, Context *context )
	: BaseVkObject( context ),
	  mImageType( VK_IMAGE_TYPE_2D ),
	  mExtent( { width, height, 1 } ),
	  mOptions( options ),
	  mImage( VK_NULL_HANDLE )
{
	initialize();
	if( nullptr != srcData ) {
		copyData( 0, srcData, srcRowBytes, srcPixelBytes, srcRegion );
	}
}

Image::Image( int32_t width, int32_t height, const uint16_t *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion, const Image::Format& options, Context *context )
	: BaseVkObject( context ),
	  mImageType( VK_IMAGE_TYPE_2D ),
	  mExtent( { width, height, 1 } ),
	  mOptions( options ),
	  mImage( VK_NULL_HANDLE )
{
	initialize();
	if( nullptr != srcData ) {
		copyData( 0, srcData, srcRowBytes, srcPixelBytes, srcRegion );
	}
}

Image::Image( int32_t width, int32_t height, const float *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion, const Image::Format& options, Context *context )
	: BaseVkObject( context ),
	  mImageType( VK_IMAGE_TYPE_2D ),
	  mExtent( { width, height, 1 } ),
	  mOptions( options ),
	  mImage( VK_NULL_HANDLE )
{
	initialize();
	if( nullptr != srcData ) {
		copyData( 0, srcData, srcRowBytes, srcPixelBytes, srcRegion );
	}
}

Image::Image( int32_t width, int32_t height, const Image::Format& format, Context *context )
	: Image( width, height, static_cast<const uint8_t*>( nullptr ), 0, 0, ci::Area(), format, context )
{
}

Image::~Image()
{
	destroy();
}


VkImageUsageFlags determineAttachmentUsage( VkFormat format )
{
	VkImageUsageFlags result = 0;

	switch( format ) {
		// Depth/stencil
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_X8_D24_UNORM_PACK32:
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
		case VK_FORMAT_S8_UINT:
		{
			result = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		break;

		// Assume everything else is Color
		default:
		{
			result = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}
		break;
	}

	return result;
}


void Image::initialize()
{
	if( VK_NULL_HANDLE != mImage ) {
		return;
	}

	// Set these here so we don't forget
	mAspectMask = vk::determineAspectMask( mOptions.mInternalFormat );

	if( VK_NULL_HANDLE == mImage ) {
		VkResult res = VK_NOT_READY;

		// Get format properties
		mFormatProperties = {};
		vkGetPhysicalDeviceFormatProperties( mContext->getGpu(), mOptions.mInternalFormat, &mFormatProperties );

		// If this assert fires, the device doesn't support an image of the requested format in mOptions.mFormat.
		// This should probably be an exception, assert seems to break at the wrong place.
		assert( ( mFormatProperties.linearTilingFeatures > 0 ) || ( mFormatProperties.optimalTilingFeatures > 0 ) );

		if( mOptions.mMipLevels > 1 ) {
			// @TODO: Add some checks here to make sure the options are compatible with mipmaps.
			if( ! mOptions.isTilingOptimal() ) {
				// @TODO: Throw exception?
			}
		}

		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType 					= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext 					= nullptr;
		imageCreateInfo.imageType 				= mImageType;
		imageCreateInfo.format 					= mOptions.mInternalFormat;
		imageCreateInfo.extent 					= mExtent;
		imageCreateInfo.mipLevels 				= mOptions.mMipLevels;
		imageCreateInfo.arrayLayers				= mOptions.mArrayLayers;
		imageCreateInfo.samples 				= mOptions.mSamples;
		imageCreateInfo.tiling 					= mOptions.mTiling;
		imageCreateInfo.usage 					= mOptions.mUsage;
		imageCreateInfo.flags 					= mOptions.mCreateFlags;
		imageCreateInfo.sharingMode 			= VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount 	= 0;
		imageCreateInfo.pQueueFamilyIndices		= nullptr;
		imageCreateInfo.initialLayout			= mOptions.mImageLayout;// VK_IMAGE_LAYOUT_UNDEFINED;
		res = vkCreateImage( mContext->getDevice(), &imageCreateInfo, nullptr, &mImage );
		assert(res == VK_SUCCESS);

		// Get memory requirements
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements( mContext->getDevice(), mImage, &memoryRequirements );

		// Find the memory type index that fits memory requirements
		uint32_t memoryTypeIndex = 0;
		bool foundMemory = mContext->findMemoryType( memoryRequirements.memoryTypeBits, mOptions.mMemoryProperty, &memoryTypeIndex );
		assert( foundMemory );

		// Allocate the memory
		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext           = nullptr;
		allocInfo.allocationSize  = memoryRequirements.size;
		allocInfo.memoryTypeIndex = memoryTypeIndex;
		res = vkAllocateMemory( mContext->getDevice(), &allocInfo, nullptr, &mMemory );
		assert(res == VK_SUCCESS);

		// Bind memory
		res = vkBindImageMemory( mContext->getDevice(), mImage, mMemory, 0 );
		assert(res == VK_SUCCESS);

		mOwnsImage = true;
		mAllocationSize = allocInfo.allocationSize;
	}

	mContext->trackedObjectCreated( this );
}

void Image::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mImage ) {
		return;
	}

	if( mOwnsImage ) {
		if( VK_NULL_HANDLE != mImage ) {
			vkDestroyImage( mContext->getDevice(), mImage, nullptr );
			mImage = VK_NULL_HANDLE;
		}

		if( VK_NULL_HANDLE != mMemory ) {
			vkFreeMemory( mContext->getDevice(), mMemory, nullptr );
			mMemory = VK_NULL_HANDLE;
		}

		mOwnsImage = false;
	}

	if( removeFromTracking ) {
		mContext->trackedObjectDestroyed( this );
	}
}

ImageRef Image::create( int32_t width, const Image::Format& options, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	ImageRef result = ImageRef( new Image( VK_IMAGE_TYPE_1D, width, 1, 1, VK_NULL_HANDLE, options, context ) );
	return result;
}

ImageRef Image::create( int32_t width, int32_t height, const Image::Format& options, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	ImageRef result = ImageRef( new Image( VK_IMAGE_TYPE_2D, width, height, 1, VK_NULL_HANDLE, options, context ) );
	return result;
}

ImageRef Image::create( int32_t width, int32_t height, int32_t depth, const Image::Format& options, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	ImageRef result = ImageRef( new Image( VK_IMAGE_TYPE_3D, width, height, depth, VK_NULL_HANDLE, options, context ) );
	return result;
}

bool hasLinearTilingSampledImage( vk::Context* context, VkFormat format )
{
	VkFormatProperties formatProperties = {};
	vkGetPhysicalDeviceFormatProperties( context->getGpu(), format, &formatProperties );
	return  0 != ( formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT );
}

ImageRef Image::create( int32_t width, int32_t height, const uint8_t *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion, const Image::Format& initialOptions, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();

	ImageRef result;
	if( initialOptions.isUsageTransferSource() ) {
		Image::Format options = Image::Format( initialOptions.getInternalFormat() )
			.setSamples( VK_SAMPLE_COUNT_1_BIT )
			.setTilingLinear()
			.setMemoryPropertyHostVisible();
		result = ImageRef( new Image( width, height, srcData, srcRowBytes, srcPixelBytes, srcRegion, options, context ) );
	}
	else {
		// Using a staging image to the source data into the final image
		if( initialOptions.isTilingOptimal() || ( VK_SAMPLE_COUNT_1_BIT != initialOptions.getSamples() ) ) {
			Image::Format stagingOptions = Image::Format( initialOptions.getInternalFormat() )
				.setSamples( VK_SAMPLE_COUNT_1_BIT )
				.setTilingLinear()
				.setMemoryPropertyHostVisible();
			ImageRef stagingImage = ImageRef( new Image( width, height, srcData, srcRowBytes,  srcPixelBytes, srcRegion, stagingOptions, context ) );

			Image::Format resultOptions = Image::Format( initialOptions.getInternalFormat() )
				.setSamples( initialOptions.getSamples() )
				.setTiling( initialOptions.getTiling() )
				.setUsageSampled()
				.setUsageTransferDestination()
				.setMemoryPropertyDeviceLocal( true );
			result = ImageRef( new Image( VK_IMAGE_TYPE_2D, width, height, 1, VK_NULL_HANDLE, resultOptions, context ) );

			Image::blit( context, stagingImage, 0, 0, result, 0, 0 );
		}
		else {
			Image::Format resultOptions = Image::Format( initialOptions.getInternalFormat() )
				.setSamples( VK_SAMPLE_COUNT_1_BIT )
				.setTilingLinear()
				.setMemoryPropertyHostVisible()
				.setUsage( hasLinearTilingSampledImage( context, initialOptions.getInternalFormat() ) ? VK_IMAGE_USAGE_SAMPLED_BIT : VK_IMAGE_USAGE_TRANSFER_SRC_BIT );
			result = ImageRef( new Image( width, height, srcData, srcRowBytes, srcPixelBytes, srcRegion, resultOptions, context ) );
		}
	}

	return result;
}

ImageRef Image::create( int32_t width, int32_t height, const uint16_t *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion, const Image::Format& initialOptions, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();

	Image::Format options = Image::Format( initialOptions.getInternalFormat() )
		.setSamples( initialOptions.getSamples() )
		.setTiling( initialOptions.getTiling() );

	ImageRef result;
	if( options.isTilingOptimal() || ( VK_SAMPLE_COUNT_1_BIT != options.getSamples() ) ) {
	}
	else {
		Image::Format options = Image::Format( options.getInternalFormat() )
			.setMemoryPropertyHostVisible()
			.setUsage( hasLinearTilingSampledImage( context, options.getInternalFormat() ) ? VK_IMAGE_USAGE_SAMPLED_BIT : VK_IMAGE_USAGE_TRANSFER_SRC_BIT );
		result = ImageRef( new Image( width, height, srcData, srcRowBytes,srcRowBytes, srcRegion, options, context ) );
	}

	return result;
}

ImageRef Image::create( int32_t width, int32_t height, const float *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion, const Image::Format& initialOptions, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();

	Image::Format options = Image::Format( initialOptions.getInternalFormat() )
		.setSamples( initialOptions.getSamples() )
		.setTiling( initialOptions.getTiling() );

	ImageRef result;
	if( options.isTilingOptimal() || ( VK_SAMPLE_COUNT_1_BIT != options.getSamples() ) ) {
	}
	else {
		Image::Format options = Image::Format( options.getInternalFormat() )
			.setMemoryPropertyHostVisible()
			.setUsage( hasLinearTilingSampledImage( context, options.getInternalFormat() ) ? VK_IMAGE_USAGE_SAMPLED_BIT : VK_IMAGE_USAGE_TRANSFER_SRC_BIT );
		result = ImageRef( new Image( width, height, srcData, srcRowBytes,srcRowBytes, srcRegion, options, context ) );
	}

	return result;
}

ImageRef Image::create( VkImageType imageType, int32_t width, int32_t height, int32_t depth, VkImage image, const Image::Format& options, Context *context )
{
	context = ( nullptr != context ) ? context : Context::getCurrent();
	ImageRef result = ImageRef( new Image( imageType, width, height, depth, image, options, context ) );
	return result;
}

void* Image::map( VkDeviceSize offset )
{
	if( isMappable() && ( nullptr == mMappedAddress ) ) {
		VkMemoryMapFlags flags = 0;
		VkResult result = vkMapMemory( mContext->getDevice(), mMemory, offset, mAllocationSize, flags, &mMappedAddress );
		if( VK_SUCCESS != result ) {
			mMappedAddress = nullptr;
		}
	}
	return mMappedAddress;
}

void Image::unmap()
{
	if( nullptr != mMappedAddress ) {
		vkUnmapMemory( mContext->getDevice(), mMemory );
		mMappedAddress = nullptr;
	}
}

template <typename SrcT, typename DstT>
void copyPixels( const SrcT *srcData, size_t srcRowBytes, size_t srcPixelBytes, DstT *dstData, size_t dstRowBytes, size_t dstPixelBytes, const ci::Area& region )
{
	const int32_t width = region.getWidth();
	const int32_t height = region.getHeight();

	const size_t srcNumChannels = srcPixelBytes / sizeof( SrcT );
	const size_t dstNumChannels = dstPixelBytes / sizeof( DstT );
	
	// Keep these as bytes so we can increment by rowbytes
	size_t startingOffset = region.y1*srcRowBytes + region.x1*srcPixelBytes;
	const uint8_t* srcRow = reinterpret_cast<const uint8_t*>( srcData + startingOffset );
	uint8_t* dstRow = reinterpret_cast<uint8_t*>( dstData );
	for( int32_t j = 0; j < height; ++j ) {
		const uint8_t* srcPixel = srcRow;
		uint8_t* dstPixel = dstRow;
		for( int32_t i = 0; i < width; ++i ) {
			const ColorAT<SrcT>* srcColor = reinterpret_cast<const ColorAT<SrcT>*>( srcPixel );
			ColorAT<DstT>* dstColor = reinterpret_cast<ColorAT<DstT>*>( dstPixel );
			switch( srcNumChannels ) {
				case 1: {
					dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
					dstColor->g = CHANTRAIT<DstT>::convert( srcColor->r );	
					dstColor->b = CHANTRAIT<DstT>::convert( srcColor->r );	
					dstColor->a = CHANTRAIT<DstT>::convert( srcColor->r );	
				}
				break;

				case 2: {
					if( 1 == dstNumChannels ) {
						dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
					}
					else if( 2 == dstNumChannels ) {
						dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
						dstColor->g = CHANTRAIT<DstT>::convert( srcColor->g );	
					}
					else if( 3 == dstNumChannels ) {
						dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
						dstColor->g = CHANTRAIT<DstT>::convert( srcColor->g );	
						dstColor->b = static_cast<DstT>( 0 );	
					}
					else if( 4 == dstNumChannels ) {
						dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
						dstColor->g = CHANTRAIT<DstT>::convert( srcColor->g );	
						dstColor->b = CHANTRAIT<DstT>::convert( srcColor->r );	
						dstColor->a = CHANTRAIT<DstT>::convert( srcColor->g );	
					}
				}
				break;

				case 3: {
					if( 1 == dstNumChannels ) {
						dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
					}
					else if( 2 == dstNumChannels ) {
						dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
						dstColor->g = CHANTRAIT<DstT>::convert( srcColor->g );	
					}
					else if( 3 == dstNumChannels ) {
						dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
						dstColor->g = CHANTRAIT<DstT>::convert( srcColor->g );	
						dstColor->b = CHANTRAIT<DstT>::convert( srcColor->b );	
					}
					else if( 4 == dstNumChannels ) {
						dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
						dstColor->g = CHANTRAIT<DstT>::convert( srcColor->g );	
						dstColor->b = CHANTRAIT<DstT>::convert( srcColor->b );	
						dstColor->a = CHANTRAIT<DstT>::max();	
					}
				}
				break;

				case 4: {
					if( 1 == dstNumChannels ) {
						dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
					}
					else if( 2 == dstNumChannels ) {
						dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
						dstColor->g = CHANTRAIT<DstT>::convert( srcColor->g );	
					}
					else if( 3 == dstNumChannels ) {
						dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
						dstColor->g = CHANTRAIT<DstT>::convert( srcColor->g );	
						dstColor->b = CHANTRAIT<DstT>::convert( srcColor->b );	
					}
					else if( 4 == dstNumChannels ) {
						dstColor->r = CHANTRAIT<DstT>::convert( srcColor->r );	
						dstColor->g = CHANTRAIT<DstT>::convert( srcColor->g );	
						dstColor->b = CHANTRAIT<DstT>::convert( srcColor->b );	
						dstColor->a = CHANTRAIT<DstT>::convert( srcColor->a );	
					}
				}
				break;
			}
			srcPixel += srcPixelBytes;
			dstPixel += dstPixelBytes;					
		}
		srcRow += srcRowBytes;
		dstRow += dstRowBytes;
	}
}

void copyPixelsSelect( VkFormat srcFormat, const void *srcData, size_t srcRowBytes, size_t srcPixelBytes, VkFormat dstFormat, void *dstData, size_t dstRowBytes, size_t dstPixelBytes, const ci::Area& region )
{
	switch( srcFormat ) {
		case VK_FORMAT_R8_UNORM:	
		case VK_FORMAT_R8G8_UNORM:	
		case VK_FORMAT_R8G8B8_UNORM:	
		case VK_FORMAT_R8G8B8A8_UNORM: {
			switch( dstFormat ) {
				case VK_FORMAT_R8_UNORM:
				case VK_FORMAT_R8G8_UNORM:
				case VK_FORMAT_R8G8B8_UNORM:
				case VK_FORMAT_R8G8B8A8_UNORM: {
					copyPixels( static_cast<const uint8_t*>( srcData ), srcRowBytes, srcPixelBytes, static_cast<uint8_t*>( dstData ), dstRowBytes, dstPixelBytes, region );
				}
				break;

				case VK_FORMAT_R16_UNORM:
				case VK_FORMAT_R16G16_UNORM:
				case VK_FORMAT_R16G16B16_UNORM:
				case VK_FORMAT_R16G16B16A16_UNORM: {
					copyPixels( static_cast<const uint8_t*>( srcData ), srcRowBytes, srcPixelBytes, static_cast<uint16_t*>( dstData ), dstRowBytes, dstPixelBytes, region );
				}
				break;

				case VK_FORMAT_R32_SFLOAT:
				case VK_FORMAT_R32G32_SFLOAT:
				case VK_FORMAT_R32G32B32_SFLOAT:
				case VK_FORMAT_R32G32B32A32_SFLOAT: {
					copyPixels( static_cast<const uint8_t*>( srcData ), srcRowBytes, srcPixelBytes, static_cast<float*>( dstData ), dstRowBytes, dstPixelBytes, region );
				}
				break;
			}
		}
		break;

		case VK_FORMAT_R16_UNORM:
		case VK_FORMAT_R16G16_UNORM:
		case VK_FORMAT_R16G16B16_UNORM:
		case VK_FORMAT_R16G16B16A16_UNORM: {
			switch( dstFormat ) {
				case VK_FORMAT_R8_UNORM:	
				case VK_FORMAT_R8G8_UNORM:	
				case VK_FORMAT_R8G8B8_UNORM:	
				case VK_FORMAT_R8G8B8A8_UNORM: {
					copyPixels( static_cast<const uint16_t*>( srcData ), srcRowBytes, srcPixelBytes, static_cast<uint8_t*>( dstData ), dstRowBytes, dstPixelBytes, region );
				}
				break;

				case VK_FORMAT_R16_UNORM:
				case VK_FORMAT_R16G16_UNORM:
				case VK_FORMAT_R16G16B16_UNORM:
				case VK_FORMAT_R16G16B16A16_UNORM: {
					copyPixels( static_cast<const uint16_t*>( srcData ), srcRowBytes, srcPixelBytes, static_cast<uint16_t*>( dstData ), dstRowBytes, dstPixelBytes, region );
				}
				break;

				case VK_FORMAT_R32_SFLOAT:
				case VK_FORMAT_R32G32_SFLOAT:
				case VK_FORMAT_R32G32B32_SFLOAT:
				case VK_FORMAT_R32G32B32A32_SFLOAT: {
					copyPixels( static_cast<const uint16_t*>( srcData ), srcRowBytes, srcPixelBytes, static_cast<float*>( dstData ), dstRowBytes, dstPixelBytes, region );
				}
				break;
			}
		}
		break;

		case VK_FORMAT_R32_SFLOAT:
		case VK_FORMAT_R32G32_SFLOAT:
		case VK_FORMAT_R32G32B32_SFLOAT:
		case VK_FORMAT_R32G32B32A32_SFLOAT: {
			switch( dstFormat ) {
				case VK_FORMAT_R8_UNORM:	
				case VK_FORMAT_R8G8_UNORM:	
				case VK_FORMAT_R8G8B8_UNORM:	
				case VK_FORMAT_R8G8B8A8_UNORM: {
					copyPixels( static_cast<const float*>( srcData ), srcRowBytes, srcPixelBytes, static_cast<uint8_t*>( dstData ), dstRowBytes, dstPixelBytes, region );
				}
				break;

				case VK_FORMAT_R16_UNORM:
				case VK_FORMAT_R16G16_UNORM:
				case VK_FORMAT_R16G16B16_UNORM:
				case VK_FORMAT_R16G16B16A16_UNORM: {
					copyPixels( static_cast<const float*>( srcData ), srcRowBytes, srcPixelBytes, static_cast<uint16_t*>( dstData ), dstRowBytes, dstPixelBytes, region );
				}
				break;

				case VK_FORMAT_R32_SFLOAT:
				case VK_FORMAT_R32G32_SFLOAT:
				case VK_FORMAT_R32G32B32_SFLOAT:
				case VK_FORMAT_R32G32B32A32_SFLOAT: {
					copyPixels( static_cast<const float*>( srcData ), srcRowBytes, srcPixelBytes, static_cast<float*>( dstData ), dstRowBytes, dstPixelBytes, region );
				}
				break;
			}
		}
		break;
	}
}

void Image::copyData( uint32_t dstLayer, const uint8_t *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion )
{
	int width = srcRegion.getWidth();
	int height = srcRegion.getHeight();
	void *dst = map();
	if( ( nullptr != dst ) && ( getWidth() == width ) && ( getHeight() == height ) ) {
		const VkImageSubresource subRes = { VK_IMAGE_ASPECT_COLOR_BIT, 0, dstLayer };
		VkSubresourceLayout layout = {};		
		vkGetImageSubresourceLayout( mContext->getDevice(), mImage, &subRes, &layout );
		size_t dstOffset = dstLayer*layout.depthPitch + layout.offset;
		uint8_t* dstData = static_cast<uint8_t*>( dst ) + dstOffset;
		size_t dstRowBytes = static_cast<size_t>( layout.rowPitch );

		VkFormat srcFormat = vk::findBestVkFormat<uint8_t>( static_cast<size_t>( srcPixelBytes / sizeof( uint8_t ) ) );
		VkFormat dstFormat = mOptions.mInternalFormat;
		size_t dstPixelBytes = formatSizeBytes( dstFormat );
		copyPixelsSelect( srcFormat, srcData, srcRowBytes, srcPixelBytes, dstFormat, dstData, dstRowBytes, dstPixelBytes, srcRegion );
		unmap();
	}
}

void Image::copyData( uint32_t dstLayer, const uint16_t *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion )
{
	int width = srcRegion.getWidth();
	int height = srcRegion.getHeight();
	void *dst = map();
	if( ( nullptr != dst ) && ( getWidth() == width ) && ( getHeight() == height ) ) {
		const VkImageSubresource subRes = { VK_IMAGE_ASPECT_COLOR_BIT, 0, dstLayer };
		VkSubresourceLayout layout = {};		
		vkGetImageSubresourceLayout( mContext->getDevice(), mImage, &subRes, &layout );
		size_t dstOffset = dstLayer*layout.depthPitch + layout.offset;
		uint16_t* dstData = static_cast<uint16_t*>( dst ) + dstOffset;
		size_t dstRowBytes = static_cast<size_t>( layout.rowPitch );

		VkFormat srcFormat = vk::findBestVkFormat<uint16_t>( static_cast<size_t>( srcPixelBytes / sizeof( uint16_t ) ) );
		VkFormat dstFormat = mOptions.mInternalFormat;
		//size_t srcPixelBytes = formatSizeBytes( srcFormat );
		size_t dstPixelBytes = formatSizeBytes( dstFormat );
		copyPixelsSelect( srcFormat, srcData, srcRowBytes, srcPixelBytes, dstFormat, dstData, dstRowBytes, dstPixelBytes, srcRegion );
		unmap();
	}
}

void Image::copyData( uint32_t dstLayer, const float *srcData, size_t srcRowBytes, size_t srcPixelBytes, const ci::Area& srcRegion )
{
	int width = srcRegion.getWidth();
	int height = srcRegion.getHeight();
	void *dst = map();
	if( ( nullptr != dst ) && ( getWidth() == width ) && ( getHeight() == height ) ) {
		const VkImageSubresource subRes = { VK_IMAGE_ASPECT_COLOR_BIT, 0, dstLayer };
		VkSubresourceLayout layout = {};		
		vkGetImageSubresourceLayout( mContext->getDevice(), mImage, &subRes, &layout );
		size_t dstOffset = dstLayer*layout.depthPitch + layout.offset;
		float* dstData = static_cast<float*>( dst ) + dstOffset;
		size_t dstRowBytes = static_cast<size_t>( layout.rowPitch );

		VkFormat srcFormat = vk::findBestVkFormat<float>( static_cast<size_t>( srcPixelBytes / sizeof( float ) ) );
		VkFormat dstFormat = mOptions.mInternalFormat;
		size_t dstPixelBytes = formatSizeBytes( dstFormat );
		copyPixelsSelect( srcFormat, srcData, srcRowBytes, srcPixelBytes, dstFormat, dstData, dstRowBytes, dstPixelBytes, srcRegion );
		unmap();
	}
}

void Image::copy(  Context* context, const vk::ImageRef& srcImage, uint32_t srcMipLevel, uint32_t srcLayer, const ivec2& srcOffset, const vk::ImageRef& dstImage, uint32_t dstMipLevel, uint32_t dstLayer, const ivec2& dstOffset, const ivec2& size )
{
	vk::CommandPoolRef cmdPool = vk::CommandPool::create( context );
	vk::CommandBufferRef cmdBuf = vk::CommandBuffer::create( cmdPool->getCommandPool(), context );

	cmdBuf->begin();
	{
		cmdBuf->pipelineBarrierImageMemory( srcImage, VK_IMAGE_LAYOUT_UNDEFINED, srcImage->getImageLayout() );
		cmdBuf->pipelineBarrierImageMemory( dstImage, VK_IMAGE_LAYOUT_UNDEFINED, dstImage->getImageLayout() );

		cmdBuf->pipelineBarrierImageMemory( srcImage, srcImage->getImageLayout(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL );
		cmdBuf->pipelineBarrierImageMemory( dstImage, dstImage->getImageLayout(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

		VkImageCopy region;
		region.srcSubresource.aspectMask		= srcImage->getAspectMask();
		region.srcSubresource.mipLevel			= srcMipLevel;
		region.srcSubresource.baseArrayLayer	= srcLayer;
		region.srcSubresource.layerCount		= 1;
		region.srcOffset						= { srcOffset.x, srcOffset.y, 0 };
		region.dstSubresource.aspectMask		= srcImage->getAspectMask();
		region.dstSubresource.mipLevel			= dstMipLevel;
		region.dstSubresource.baseArrayLayer	= dstLayer;
		region.dstSubresource.layerCount		= 1;
		region.dstOffset						= { dstOffset.x, dstOffset.y, 0 };
		region.extent							= { srcImage->getWidth(), srcImage->getHeight(), 1 };

		cmdBuf->copyImage( srcImage->getImage(), srcImage->getImageLayout(), dstImage->getImage(), dstImage->getImageLayout(), 1, &region );

		cmdBuf->pipelineBarrierImageMemory( dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dstImage->getImageLayout(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
	}
	cmdBuf->end();

	const VkCommandBuffer cmdBufs[] = { cmdBuf->getCommandBuffer() };
	VkSubmitInfo submitInfo[1] = {};
	submitInfo[0].pNext					= NULL;
	submitInfo[0].sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo[0].waitSemaphoreCount	= 0;
	submitInfo[0].pWaitSemaphores		= NULL;
	submitInfo[0].commandBufferCount	= 1;
	submitInfo[0].pCommandBuffers		= cmdBufs;
	submitInfo[0].signalSemaphoreCount	= 0;
	submitInfo[0].pSignalSemaphores		= NULL;
	VkFence nullFence = VK_NULL_HANDLE;
	VkResult res = vkQueueSubmit( context->getQueue(), 1, submitInfo, nullFence );
	assert( VK_SUCCESS == res );

	res = vkQueueWaitIdle( context->getQueue() );
	assert( VK_SUCCESS == res );
}

void Image::copy(  Context* context, const vk::ImageRef& srcImage, uint32_t srcMipLevel, uint32_t srcLayer, const vk::ImageRef& dstImage, uint32_t dstMipLevel, uint32_t dstLayer, const ivec2& size )
{
	Image::copy( context, srcImage, srcMipLevel, srcLayer, ivec2( 0, 0 ), dstImage, dstMipLevel, dstLayer, ivec2( 0, 0 ), size );
}

void Image::copy(  Context* context, const vk::ImageRef& srcImage, uint32_t srcMipLevel, uint32_t srcLayer, const vk::ImageRef& dstImage, uint32_t dstMipLevel, uint32_t dstLayer )
{
	if( ( srcImage->getWidth() != dstImage->getWidth() ) || ( srcImage->getHeight() != dstImage->getHeight() ) ) {
		return;
	}

	Image::copy( context, srcImage, srcMipLevel, srcLayer, dstImage, dstMipLevel, dstLayer, ivec2( srcImage->getWidth(), srcImage->getHeight() ) );
}

void Image::blit( Context* context, const vk::ImageRef& srcImage, uint32_t srcMipLevel, uint32_t srcLayer, const ci::Area& srcArea, const vk::ImageRef& dstImage, uint32_t dstMipLevel, uint32_t dstLayer, const ci::Area& dstArea )
{
	VkCommandPool cmdPool = context->getDefaultCommandPool()->getCommandPool();
	vk::CommandBufferRef cmdBuf = vk::CommandBuffer::create( cmdPool, context );

	cmdBuf->begin();
	{
		cmdBuf->pipelineBarrierImageMemory( srcImage, VK_IMAGE_LAYOUT_UNDEFINED, srcImage->getImageLayout() );
		cmdBuf->pipelineBarrierImageMemory( dstImage, VK_IMAGE_LAYOUT_UNDEFINED, dstImage->getImageLayout() );

		cmdBuf->pipelineBarrierImageMemory( srcImage, srcImage->getImageLayout(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL );
		cmdBuf->pipelineBarrierImageMemory( dstImage, dstImage->getImageLayout(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

		VkImageBlit region;
		region.srcSubresource.aspectMask		= srcImage->getAspectMask();
		region.srcSubresource.mipLevel			= srcMipLevel;
		region.srcSubresource.baseArrayLayer	= srcLayer;
		region.srcSubresource.layerCount		= 1;
		region.srcOffsets[0]					= { srcArea.x1, srcArea.y1, 0 };
		region.srcOffsets[1]					= { srcArea.x2, srcArea.y2, 1 };
		region.dstSubresource.aspectMask		= srcImage->getAspectMask();
		region.dstSubresource.mipLevel			= dstMipLevel;
		region.dstSubresource.baseArrayLayer	= dstLayer;
		region.dstSubresource.layerCount		= 1;
		region.dstOffsets[0]					= { dstArea.x1, dstArea.y1, 0 };
		region.dstOffsets[1]					= { dstArea.x2, dstArea.y2, 1 };

		cmdBuf->blitImage( srcImage->getImage(), srcImage->getImageLayout(), dstImage->getImage(), dstImage->getImageLayout(), 1, &region, VK_FILTER_LINEAR );

		cmdBuf->pipelineBarrierImageMemory( dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dstImage->getImageLayout(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
	}
	cmdBuf->end();

	const VkCommandBuffer cmdBufs[] = { cmdBuf->getCommandBuffer() };
	VkSubmitInfo submitInfo[1] = {};
	submitInfo[0].pNext					= NULL;
	submitInfo[0].sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo[0].waitSemaphoreCount	= 0;
	submitInfo[0].pWaitSemaphores		= NULL;
	submitInfo[0].commandBufferCount	= 1;
	submitInfo[0].pCommandBuffers		= cmdBufs;
	submitInfo[0].signalSemaphoreCount	= 0;
	submitInfo[0].pSignalSemaphores		= NULL;
	VkFence nullFence = VK_NULL_HANDLE;
	VkResult res = vkQueueSubmit( context->getQueue(), 1, submitInfo, nullFence );
	assert( VK_SUCCESS == res );

	res = vkQueueWaitIdle( context->getQueue() );
	assert( VK_SUCCESS == res );
}

void Image::blit( Context* context, const vk::ImageRef& srcImage, uint32_t srcMipLevel, uint32_t srcLayer, const vk::ImageRef& dstImage, uint32_t dstMipLevel, uint32_t dstLayer )
{
	ci::Area srcArea = ci::Area( 0, 0, srcImage->getWidth(), srcImage->getHeight() );
	ci::Area dstArea = ci::Area( 0, 0, dstImage->getWidth(), dstImage->getHeight() );
	Image::blit( context, srcImage, srcMipLevel, srcLayer, srcArea, dstImage, dstMipLevel, dstLayer, dstArea );
}

}} // namespace cinder::vk