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

#include "cinder/vk/ImageView.h"
#include "cinder/vk/ConstantConversion.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Device.h"
#include "cinder/vk/wrapper.h"

#include "cinder/ChanTraits.h"
#include "cinder/Color.h"

#include <algorithm>

namespace cinder { namespace vk {

// -------------------------------------------------------------------------------------------------
// ImageView
// -------------------------------------------------------------------------------------------------
ImageView::ImageView( VkImageViewType viewType, VkImageType imageType, int32_t width, int32_t height, int32_t depth, const Image::Format& imageOptions, VkImage image, vk::Device *device )
	: BaseDeviceObject( device )
{
	initialize( viewType, imageType, width, height, depth, imageOptions, image );
}

ImageView::ImageView( VkImageViewType viewType, VkImageType imageType, int32_t width, int32_t height, int32_t depth, const Image::Format& imageOptions, const vk::ImageRef& premadeImage, vk::Device *device )
	: BaseDeviceObject( device )
{
	initialize( viewType, imageType, width, height, depth, imageOptions, premadeImage );
}

// selfOwned is generally false
ImageView::ImageView( bool selfOwned, vk::Device *device )
	: BaseDeviceObject( device ),
	  mSelfOwned( selfOwned )
{
	// Derived object will handle init sequence
}

ImageView::~ImageView()
{
	if( mSelfOwned ) {
		destroy();
	}
}

void ImageView::initialize( VkImageViewType viewType, VkImageType imageType, int32_t width, int32_t height, int32_t depth, const Image::Format& imageOptions, VkImage image )
{
	if( VK_NULL_HANDLE != mImageView ) {
		return;
	}
	
	mViewType = viewType;
	mImage = vk::Image::create( imageType, width, height, depth, image, imageOptions, mDevice );
	assert( mImage );

    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.pNext							= nullptr;
    imageViewCreateInfo.viewType						= mViewType;
    imageViewCreateInfo.format							= mImage->getFormat();
    imageViewCreateInfo.flags							= 0;
	imageViewCreateInfo.image							= mImage->getImage();
	imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
	imageViewCreateInfo.subresourceRange.levelCount     = mImage->getMipLevels();
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount     = mImage->getArrayLayers();
	imageViewCreateInfo.subresourceRange.aspectMask	    = mImage->getAspectMask();
	assert( 0 != imageViewCreateInfo.subresourceRange.aspectMask );
	if( VK_IMAGE_ASPECT_COLOR_BIT == imageViewCreateInfo.subresourceRange.aspectMask ) {
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	}

	// @TODO: May need to update imageViewCreateInfo.subresourceRange.layerCount to imageOptions.arrayLayers.
	if( ( VK_IMAGE_VIEW_TYPE_CUBE == mViewType ) || ( VK_IMAGE_VIEW_TYPE_CUBE_ARRAY == mViewType ) ) {
		// Do we need anything else for these cases?
	}

	VkResult res = vkCreateImageView( mDevice->getDevice(), &imageViewCreateInfo, nullptr, &mImageView );
	assert(res == VK_SUCCESS);

	if( mSelfOwned ) {
		mDevice->trackedObjectCreated( this );
	}
}

void ImageView::initialize( VkImageViewType viewType, VkImageType imageType, int32_t width, int32_t height, int32_t depth, const Image::Format& imageOptions, const vk::ImageRef& premadeImage )
{
	if( VK_NULL_HANDLE != mImageView ) {
		return;
	}

	mViewType = viewType;
	mImage = premadeImage;

    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.pNext							= nullptr;
    imageViewCreateInfo.viewType						= mViewType;
    imageViewCreateInfo.format							= mImage->getFormat();
    imageViewCreateInfo.flags							= 0;
	imageViewCreateInfo.image							= mImage->getImage();
	imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
	imageViewCreateInfo.subresourceRange.levelCount     = mImage->getMipLevels();
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount     = mImage->getArrayLayers();
	imageViewCreateInfo.subresourceRange.aspectMask	    = mImage->getAspectMask();
	assert( 0 != imageViewCreateInfo.subresourceRange.aspectMask );
	if( VK_IMAGE_ASPECT_COLOR_BIT == imageViewCreateInfo.subresourceRange.aspectMask ) {
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	}

	// @TODO: May need to update imageViewCreateInfo.subresourceRange.layerCount to imageOptions.arrayLayers.
	if( ( VK_IMAGE_VIEW_TYPE_CUBE == mViewType ) || ( VK_IMAGE_VIEW_TYPE_CUBE_ARRAY == mViewType ) ) {
		// Do we need anything else for these cases?
	}

	VkResult res = vkCreateImageView( mDevice->getDevice(), &imageViewCreateInfo, nullptr, &mImageView );
	assert(res == VK_SUCCESS);

	if( mSelfOwned ) {
		mDevice->trackedObjectCreated( this );
	}
}

void ImageView::destroy( bool removeFromTracking )
{
	if( VK_NULL_HANDLE == mImageView ) {
		return;
	}

	vkDestroyImageView( mDevice->getDevice(), mImageView, nullptr );
	mImageView = VK_NULL_HANDLE;

	if( mImage ) {
		mImage->~Image();
		mImage.reset();
	}

	if( removeFromTracking && mSelfOwned ) {
		mDevice->trackedObjectDestroyed( this );
	}
}

ImageViewRef ImageView::create( int32_t width, const Image::Format& imageOptions, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	VkImageViewType viewType = ( imageOptions.getArrayLayers() > 1 ) ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
	VkImage nullImage = VK_NULL_HANDLE;
	ImageViewRef result = ImageViewRef( new ImageView( viewType, VK_IMAGE_TYPE_1D, width, 1, 1, imageOptions, nullImage, device ) );
	return result;
}

ImageViewRef ImageView::create( int32_t width, int32_t height, const Image::Format& imageOptions, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	VkImageViewType viewType = ( imageOptions.getArrayLayers() > 1 ) ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
	VkImage nullImage = VK_NULL_HANDLE;
	ImageViewRef result = ImageViewRef( new ImageView( viewType, VK_IMAGE_TYPE_2D, width, height, 1, imageOptions, nullImage, device ) );
	return result;
}

ImageViewRef ImageView::create( int32_t width, int32_t height, int32_t depth, const Image::Format& imageOptions, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_3D;
	VkImage nullImage = VK_NULL_HANDLE;
	ImageViewRef result = ImageViewRef( new ImageView( viewType, VK_IMAGE_TYPE_3D, width, height, depth, imageOptions, nullImage, device ) );
	return result;
}

ImageViewRef ImageView::create( int32_t width, VkFormat format, VkImage image, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_1D;
	Image::Format imageOptions = Image::Format::createUndefined( format );
	ImageViewRef result = ImageViewRef( new ImageView( viewType, VK_IMAGE_TYPE_1D, width, 1, 1, imageOptions, image, device ) );
	return result;
}

ImageViewRef ImageView::create( int32_t width, int32_t height, VkFormat format, VkImage image,vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
	Image::Format imageOptions = Image::Format::createUndefined( format );
	ImageViewRef result = ImageViewRef( new ImageView( viewType, VK_IMAGE_TYPE_2D, width, height, 1, imageOptions, image, device ) );
	return result;
}

ImageViewRef ImageView::create( int32_t width, int32_t height, int32_t depth, VkFormat format, VkImage image, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_3D;
	Image::Format imageOptions = Image::Format::createUndefined( format );
	ImageViewRef result = ImageViewRef( new ImageView( viewType, VK_IMAGE_TYPE_3D, width, height, depth, imageOptions, image, device ) );
	return result;
}

ImageViewRef ImageView::create( int32_t width, const vk::ImageRef& premadeImage, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_1D;
	ImageViewRef result = ImageViewRef( new ImageView( viewType, VK_IMAGE_TYPE_1D, width, 1, 1, premadeImage->getOptions(), premadeImage, device ) );
	return result;
}

ImageViewRef ImageView::create( int32_t width, int32_t height, const vk::ImageRef& premadeImage, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
	ImageViewRef result = ImageViewRef( new ImageView( viewType, VK_IMAGE_TYPE_2D, width, height, 1, premadeImage->getOptions(), premadeImage, device ) );
	return result;
}

ImageViewRef ImageView::create( int32_t width, int32_t height, int32_t depth, const vk::ImageRef& premadeImage, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_3D;
	ImageViewRef result = ImageViewRef( new ImageView( viewType, VK_IMAGE_TYPE_3D, width, height, depth, premadeImage->getOptions(), premadeImage, device ) );
	return result;
}

ImageViewRef ImageView::createCube( int32_t width, int32_t height, const vk::ImageRef& premadeImage, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	// Image view type of CUBE and CUBE_ARRAY requires at least 6 elements in imageOptions.arrayLayers
	assert( premadeImage && ( premadeImage->getOptions().getArrayLayers() >= 6 ) );	
	ImageViewRef result = ImageViewRef( new ImageView( VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_TYPE_2D, width, height, 1, premadeImage->getOptions(), premadeImage, device ) );
	return result;
}


}} // namespace cinder::vk