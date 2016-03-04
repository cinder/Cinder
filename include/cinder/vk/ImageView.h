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

#include "cinder/vk/Image.h"
#include "cinder/Area.h"

namespace cinder { namespace vk {

class Context;
class Swapchain;

class Image;
class ImageView;
using ImageRef = std::shared_ptr<Image>;
using ImageViewRef = std::shared_ptr<ImageView>;

//! \class ImageView
//!
//!
class ImageView : public BaseDeviceObject {
public:

	//! \class Options
	//!
	//!
	class Options {
	public:

		Options( const Image::Format& imageFormat = Image::Format() ) : mImageFormat( imageFormat ) {
			mSwizzle.r = VK_COMPONENT_SWIZZLE_R;
			mSwizzle.g = VK_COMPONENT_SWIZZLE_G;
			mSwizzle.b = VK_COMPONENT_SWIZZLE_B;
			mSwizzle.a = VK_COMPONENT_SWIZZLE_A;
		}

		Options( VkImageViewType viewType, const Image::Format& imageFormat = Image::Format() ) : mViewType( viewType ), mImageFormat( imageFormat ) {
			mSwizzle.r = VK_COMPONENT_SWIZZLE_R;
			mSwizzle.g = VK_COMPONENT_SWIZZLE_G;
			mSwizzle.b = VK_COMPONENT_SWIZZLE_B;
			mSwizzle.a = VK_COMPONENT_SWIZZLE_A;
		}

		virtual ~Options() {}

		Options&					setViewType( VkImageViewType value ) { mViewType = value; return *this; }

		Options&					setImageFormat( const Image::Format& value ) { mImageFormat = value; return *this; }
		
		Options&					setSwizzle( const VkComponentMapping& value ) { mSwizzle = value; return *this; }
		Options&					setSwizzle( VkComponentSwizzle r, VkComponentSwizzle g, VkComponentSwizzle b, VkComponentSwizzle a ) { mSwizzle.r = r; mSwizzle.g = g; mSwizzle.b = b; mSwizzle.a = a; return *this; }
		Options&					setSwizzleR( VkComponentSwizzle r ) { mSwizzle.r = r; return *this; }
		Options&					setSwizzleG( VkComponentSwizzle g ) { mSwizzle.g = g; return *this; }
		Options&					setSwizzleB( VkComponentSwizzle b ) { mSwizzle.b = b; return *this; }
		Options&					setSwizzleA( VkComponentSwizzle a ) { mSwizzle.a = a; return *this; }

	private:
		VkImageViewType			mViewType = VK_IMAGE_VIEW_TYPE_2D;
		Image::Format			mImageFormat;
		VkComponentMapping		mSwizzle;
	};

	// --------------------------------------------------------------------------------------------

	ImageView();
	explicit ImageView( VkImageViewType viewType, VkImageType imageType, int32_t width, int32_t height, int32_t depth, const Image::Format& imageOptions, VkImage image, vk::Device *device );
	explicit ImageView( VkImageViewType viewType, VkImageType imageType, int32_t width, int32_t height, int32_t depth, const Image::Format& imageOptions, const vk::ImageRef& premadeImage, vk::Device *device );
	virtual ~ImageView();

	//! Creates an image view and allocates an image that fits the requested specifications
	static ImageViewRef		create( int32_t width, const Image::Format& imageOptions = Image::Format(), vk::Device *device = nullptr );
	static ImageViewRef		create( int32_t width, int32_t height, const Image::Format& imageOptions = Image::Format(), vk::Device *device = nullptr );
	static ImageViewRef		create( int32_t width, int32_t height, int32_t depth, const Image::Format& imageOptions = Image::Format(), vk::Device *device = nullptr );
	//! Creates an image view and uses and existing Vulkan image
	static ImageViewRef		create( int32_t width, VkFormat format, VkImage image, vk::Device *device = nullptr );
	static ImageViewRef		create( int32_t width, int32_t height, VkFormat format, VkImage image, vk::Device *device = nullptr );
	static ImageViewRef		create( int32_t width, int32_t height, int32_t depth, VkFormat format, VkImage image, vk::Device *device = nullptr );
	//! Creates an image view and references and a premade image
	static ImageViewRef		create( int32_t width, const vk::ImageRef& premadeImage, vk::Device *device = nullptr );
	static ImageViewRef		create( int32_t width, int32_t height, const vk::ImageRef& premadeImage, vk::Device *device = nullptr );
	static ImageViewRef		create( int32_t width, int32_t height, int32_t depth, const vk::ImageRef& premadeImage,vk::Device *device = nullptr );
	//! Creates an 2D image view using a specific viewType
	static ImageViewRef		createCube( int32_t width, int32_t height, const vk::ImageRef& premadeImage, vk::Device *device = nullptr );

	VkImageView				getImageView() const { return mImageView; }

	const vk::ImageRef&		getImage() const { return mImage; }

	int32_t					getWidth()  const { return mImage->getWidth(); }
	int32_t					getHeight() const { return mImage->getHeight(); }
	int32_t					getDepth()  const { return mImage->getDepth(); }

	VkFormat				getFormat() const { return mImage->getFormat(); }
	VkImageAspectFlags		getAspectMask() const { return mImage->getAspectMask(); }

	VkImageLayout			getInitialLayout() const { return mImage->getInitialLayout(); }

protected:
	// Derived objects should use this c'tor
	explicit ImageView( bool selfOwned, Device *device );

	bool					mSelfOwned = true;

	VkImageViewType			mViewType = VK_IMAGE_VIEW_TYPE_2D;
	VkImageView 			mImageView = VK_NULL_HANDLE;

	vk::ImageRef			mImage;

	void initialize( VkImageViewType viewType, VkImageType imageType, int32_t width, int32_t height, int32_t depth, const Image::Format& imageOptions, VkImage image );
	void initialize( VkImageViewType viewType, VkImageType imageType, int32_t width, int32_t height, int32_t depth, const Image::Format& imageOptions, const vk::ImageRef& premadeImage );
	void destroy( bool removeFromTracking = true );
	void destroySelf();
	friend class vk::Device;
	friend class vk::Swapchain;
};

}} // namespace cinder::vk