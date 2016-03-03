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

#include "cinder/vk/Texture.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/CommandPool.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Device.h"
#include "cinder/vk/ImageView.h"
#include "cinder/vk/wrapper.h"

namespace cinder { namespace vk {

// -------------------------------------------------------------------------------------------------
// ImageTargetVkTexture2d
// -------------------------------------------------------------------------------------------------
template<typename T>
class ImageTargetVkTexture2d : public ImageTarget {
  public:
	// Receives a pointer to an intermediate data store, presumably a mapped PBO
	static std::shared_ptr<ImageTargetVkTexture2d> create( const Texture *texture, ImageIo::ChannelOrder &channelOrder, bool isGray, bool hasAlpha, void *data );
	
	virtual bool	hasAlpha() const { return mHasAlpha; }
	virtual void*	getRowPointer( int32_t row );
	
	void*			getData() const { return mDataBaseAddress; }
	
  private:
	ImageTargetVkTexture2d( const Texture *texture, ImageIo::ChannelOrder &channelOrder, bool isGray, bool hasAlpha, void *intermediateData );
	
	const Texture2d			*mTexture = nullptr;
	bool					mHasAlpha = false;;
	uint8_t					mPixelInc = 0;
	T						*mDataBaseAddress = nullptr;
	std::unique_ptr<T[]>	mDataStore; // may be NULL
	int32_t					mRowInc = 0;
};

// -------------------------------------------------------------------------------------------------
// TextureBase
// -------------------------------------------------------------------------------------------------
TextureBase::TextureBase()
{
}

TextureBase::~TextureBase()
{
}

// -------------------------------------------------------------------------------------------------
// Texture2d
// -------------------------------------------------------------------------------------------------
Texture2d::Texture2d()
	: TextureBase()
{
}

Texture2d::Texture2d( int width, int height, const Texture2d::Format &format, vk::Device *device )
	: TextureBase(), mWidth( width ), mHeight( height ), mFormat( format )
{
	initialize( device );
}

Texture2d::Texture2d( const void *data, VkFormat dataFormat, int width, int height, const Texture2d::Format &format, vk::Device *device )
	: TextureBase(), mWidth( width ), mHeight( height ), mFormat( format )
{
}

Texture2d::Texture2d( const Surface8u& surf, const Texture2d::Format &format, vk::Device *device )
	: TextureBase(), mWidth( surf.getWidth() ), mHeight( surf.getHeight() ), mFormat( format )
{
	if( VK_FORMAT_UNDEFINED == mFormat.getInternalFormat() ) {
		// Force a supported format 
		mFormat.setInternalFormat( VK_FORMAT_R8G8B8A8_UNORM );
	}
	initialize( surf.getData(), surf.getRowBytes(), surf.getPixelBytes(), device );
}

Texture2d::Texture2d( const Surface16u& surf, const Texture2d::Format &format, vk::Device *device )
	: TextureBase(), mWidth( surf.getWidth() ), mHeight( surf.getHeight() ), mFormat( format )
{

	if( VK_FORMAT_UNDEFINED == mFormat.getInternalFormat() ) {
		// Force a supported format 
		mFormat.setInternalFormat( VK_FORMAT_R16G16B16A16_UNORM );
	}
	initialize( surf.getData(), surf.getRowBytes(), surf.getPixelBytes(), device );
}

Texture2d::Texture2d( const Surface32f& surf, const Texture2d::Format &format, vk::Device *device )
	: TextureBase() , mWidth( surf.getWidth() ), mHeight( surf.getHeight() ), mFormat( format )
{
	if( VK_FORMAT_UNDEFINED == mFormat.getInternalFormat() ) {
		// Force a supported format 
		mFormat.setInternalFormat( VK_FORMAT_R32G32B32A32_SFLOAT );
	}
	initialize( surf.getData(), surf.getRowBytes(), surf.getPixelBytes(), device );
}

//Texture2d::Texture2d( const ImageSourceRef& imageSource,const Texture2d::Format &format, vk::Device *device )
//	: TextureBase(), mWidth( imageSource->getWidth() ), mHeight( imageSource->getHeight() ), mFormat( format )
//{
//	VkFormat defaultInternalFormat = VK_FORMAT_UNDEFINED;
//	// Set the internal format based on the image's color space
//	switch( imageSource->getColorModel() ) {
//		// Force RGBA format for now, since VUlkan doesn't seem to like creating RGB images.
//		case ImageIo::CM_RGB:
//			switch( imageSource->getDataType() ) {
//				case ImageIo::UINT16:
//					defaultInternalFormat = VK_FORMAT_R16G16B16A16_UNORM; //( imageSource->hasAlpha() ) ? VK_FORMAT_R16G16B16A16_UNORM : VK_FORMAT_R16G16B16_UNORM;
//				break;
//				case ImageIo::FLOAT16:
//					defaultInternalFormat = VK_FORMAT_R16G16B16A16_SFLOAT; //( imageSource->hasAlpha() ) ? VK_FORMAT_R16G16B16A16_SFLOAT : VK_FORMAT_R16G16B16_SFLOAT;
//				break;
//				case ImageIo::FLOAT32:
//					defaultInternalFormat = VK_FORMAT_R32G32B32A32_SFLOAT; //( imageSource->hasAlpha() ) ? VK_FORMAT_R32G32B32A32_SFLOAT : VK_FORMAT_R32G32B32_SFLOAT;
//				break;
//				default:
//					defaultInternalFormat = VK_FORMAT_R8G8B8A8_UNORM; //( imageSource->hasAlpha() ) ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8_UNORM;
//				break;
//			}
//		break;
//		case ImageIo::CM_GRAY: {
//			switch( imageSource->getDataType() ) {
//				case ImageIo::UINT16:
//					defaultInternalFormat = ( imageSource->hasAlpha() ) ? VK_FORMAT_R16G16_UNORM : VK_FORMAT_R16_UNORM;
//				break;
//				case ImageIo::FLOAT16:
//					defaultInternalFormat = ( imageSource->hasAlpha() ) ? VK_FORMAT_R16G16_SFLOAT : VK_FORMAT_R16_SFLOAT;
//				break;
//				case ImageIo::FLOAT32:
//					defaultInternalFormat = ( imageSource->hasAlpha() ) ? VK_FORMAT_R32G32_SFLOAT : VK_FORMAT_R32_SFLOAT;
//				break;
//				default:
//					defaultInternalFormat = ( imageSource->hasAlpha() ) ? VK_FORMAT_R8G8_UNORM : VK_FORMAT_R8_UNORM;
//				break;
//			}
//			
//			//if( ! format.mSwizzleSpecified ) {
//			//	std::array<int,4> swizzleMask = { GL_RED, GL_RED, GL_RED, GL_ONE };
//			//	if( imageSource->hasAlpha() )
//			//		swizzleMask[3] = GL_GREEN;
//			//	format.setSwizzleMask( swizzleMask );
//			//}
//		}
//		break;
//		default:
//			throw ImageIoExceptionIllegalColorModel( "Unsupported color model for vk::Texture2d construction." );
//		break;
//	}
//
//	// Set internal format if one isn't defined
//	if( VK_FORMAT_UNDEFINED == mFormat.getInternalFormat() ) {
//		mFormat.setInternalFormat( defaultInternalFormat );
//	}
//
//	initialize( imageSource, context );
//}

Texture2d::~Texture2d()
{
	destroy();
}

void Texture2d::initializeCommon( vk::Device* device )
{
	mFormatProperties = {};
	vkGetPhysicalDeviceFormatProperties( device->getGpu(), mFormat.getInternalFormat(), &mFormatProperties );

	//mNeedsStaging = ( ! ( mFormatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT ) ) || format.getForceStaging();
	mHostVisible = mFormat.getHostVisible();

	// Calculate mipmap levels
	if( mFormat.getMipmapEnabled() ) {
		const float mipCountBias = 0.0001f;
		const float logWidth = log2( static_cast<float>( mWidth ) );
		const float logHeight = log2( static_cast<float>( mHeight ) );
		mMipLevels = static_cast<int>( std::min( logWidth, logHeight ) + mipCountBias );
		if( mFormat.getMaxMipmapLevels() > 0 ) {
			mMipLevels = std::min<uint32_t>( mMipLevels, mFormat.getMaxMipmapLevels() );
		}
	}
}

void Texture2d::initializeFinal( vk::Device *device )
{
    VkResult U_ASSERT_ONLY res;

	// @TODO: Expose this fields as format parameters
    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType						= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext						= nullptr;
    samplerCreateInfo.magFilter					= mFormat.mMagFilter;
    samplerCreateInfo.minFilter					= mFormat.mMinFilter;
    samplerCreateInfo.mipmapMode				= mFormat.mMipmapMode;
    samplerCreateInfo.addressModeU				= mFormat.mSamplerAddressModeU;
    samplerCreateInfo.addressModeV				= mFormat.mSamplerAddressModeV;
    samplerCreateInfo.addressModeW				= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.mipLodBias				= 0.0f;
	samplerCreateInfo.anisotropyEnable			= VK_FALSE;
    samplerCreateInfo.maxAnisotropy				= 1;
    samplerCreateInfo.compareEnable				= VK_FALSE; //mFormat.mCompareEnabled;
    samplerCreateInfo.compareOp					= mFormat.mCompareOp;
    samplerCreateInfo.minLod					= 0.0f;
    samplerCreateInfo.maxLod					= static_cast<float>( mMipLevels );
    samplerCreateInfo.borderColor				= VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates	= mFormat.mUnnormalizedCoordinates;
	if( mFormat.mUnnormalizedCoordinates ) {
		// Enforce some rules if unnormalizedCoordinates is requested
		samplerCreateInfo.mipmapMode		= VK_SAMPLER_MIPMAP_MODE_NEAREST;
		samplerCreateInfo.minLod			= 0.0f;
		samplerCreateInfo.maxLod			= 0.0f;
		samplerCreateInfo.anisotropyEnable	= VK_FALSE;
		samplerCreateInfo.compareEnable		= VK_FALSE;
	}
    res = vkCreateSampler( device->getDevice(), &samplerCreateInfo, nullptr, &mSampler );
    assert(res == VK_SUCCESS);
	
	// Descriptor
	mImageInfo.imageView   = this->getImageView()->getImageView();
	mImageInfo.sampler     = this->mSampler;
	mImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
}

void Texture2d::initialize( vk::Device *device )
{
	initializeCommon( device );

	vk::Image::Format imageOptions = vk::Image::Format( mFormat.getInternalFormat() );
	imageOptions.setSamples( mFormat.getSamples() );
	if( mHostVisible ) {
		// NOTE: If the device doesn't support linear tiled images with sampling or 
		//       a memory type can't be found for these properties - an error will
		//       occur during the initialization of the image. 		
		imageOptions.setTilingLinear();
		imageOptions.setUsageSampled();
		imageOptions.setMemoryPropertyHostVisible();
	}
	else {
		imageOptions.setTiling( mFormat.getTiling() );
		imageOptions.setUsage( mFormat.getUsage() );
		imageOptions.setMemoryPropertyDeviceLocal();
	}
	vk::ImageRef preMadeImage = vk::Image::create( mWidth, mHeight, imageOptions, device );
	//preMadeImage->setImageLayout( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );		
	//ImageView::initialize( VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_TYPE_2D, mWidth, mHeight, 1, imageOptions, preMadeImage );
	mImageView = vk::ImageView::create( mWidth, mHeight, preMadeImage, device );

	initializeFinal( device );
}

/*
void Texture2d::initialize( const void *data, VkFormat dataFormat, vk::Device *device )
{
	initializeCommon( device );	

	// Create the premade image
	vk::Image::Format imageOptions = vk::Image::Format( mFormat.getInternalFormat() )
		.setSamples( VK_SAMPLE_COUNT_1_BIT )
		.setMipLevels( mMipLevels )
		.setTilingOptimal()
		.setMemoryPropertyDeviceLocal();
	vk::ImageRef premadeImage = vk::Image::create( mWidth, mHeight, imageOptions, device );
	//premadeImage->setImageLayout( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
	mImageView = vk::ImageView::create( mWidth, mHeight, premadeImage, device );
		
	initializeFinal( device );
}
*/

template <typename T>
void Texture2d::initialize( const T* srcData, size_t srcRowBytes, size_t srcPixelBytes, vk::Device *device )
{
	initializeCommon( device );

	size_t numChannels = srcPixelBytes / sizeof( T 	);
	VkFormat srcDataFormat = vk::findBestVkFormat<uint8_t>( numChannels );
	assert( srcDataFormat != VK_FORMAT_UNDEFINED );


	if( mHostVisible ) {
		// NOTE: If the device doesn't support linear tiled images with sampling or 
		//       a memory type can't be found for these properties - an error will
		//       occur during the initialization of the image. 
		vk::Image::Format imageOptions = vk::Image::Format( mFormat.getInternalFormat() )
			.setSamples( VK_SAMPLE_COUNT_1_BIT )
			.setTilingLinear()
			.setUsageSampled()
			.setMemoryPropertyHostVisible()
			.setMipLevels( 1 );
		vk::ImageRef preMadeImage = vk::Image::create( mWidth, mHeight, srcData, srcRowBytes, srcPixelBytes, Area( 0, 0, mWidth, mHeight ), imageOptions, device );
		//ImageView::initialize( VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_TYPE_2D, mWidth, mHeight, 1, preMadeImage->getOptions(), preMadeImage );
		mImageView = vk::ImageView::create( mWidth, mHeight, preMadeImage, device );
	}
	else {
		// Create image for texture
		vk::Image::Format imageOptions = vk::Image::Format( mFormat.getInternalFormat() )
			.setSamples( mFormat.getSamples() )
			.setTilingOptimal()
			.setUsageSampled()
			.setUsageTransferDestination()
			.setMemoryPropertyDeviceLocal()
			.setMipLevels( mMipLevels );
		vk::ImageRef preMadeImage = vk::Image::create( mWidth, mHeight, imageOptions, device );
		//ImageView::initialize( VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_TYPE_2D, mWidth, mHeight, 1, preMadeImage->getOptions(), preMadeImage );
		mImageView = vk::ImageView::create( mWidth, mHeight, preMadeImage, device );
		
		doUpdate( mWidth, mHeight, srcData, srcRowBytes, srcPixelBytes );
	}

	initializeFinal( device );
}

void Texture2d::initialize( const ImageSourceRef& imageSource, vk::Device* device )
{
	initializeCommon( device );

	/*
	// Setup an appropriate dataFormat/ImageTargetTexture based on the image's color space
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
	*/

	ImageIo::ChannelOrder channelOrder =  ImageIo::RGBA;
	bool isGray = false;

	// Create the premade image
	vk::Image::Format imageOptions = vk::Image::Format( mFormat.getInternalFormat() )
		.setMipLevels( mMipLevels )
		.setTilingLinear()
		.setMemoryPropertyHostVisible();
	vk::ImageRef premadeImage = vk::Image::create( mWidth, mHeight, imageOptions, device );
	//premadeImage->setImageLayout( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
	//ImageView::initialize( VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_TYPE_2D, mWidth, mHeight, 1, premadeImage->getOptions(), premadeImage );
	mImageView = vk::ImageView::create( mWidth, mHeight, premadeImage, device );

	initializeFinal( device );

	// Load data
	if( premadeImage ) {
		void *data = premadeImage->map();
		if( nullptr != data ) {
			if( imageSource->getDataType() == ImageIo::UINT8 ) {
				auto target = ImageTargetVkTexture2d<uint8_t>::create( this, channelOrder, isGray, imageSource->hasAlpha(), data );
				imageSource->load( target );	
			}
			else if( imageSource->getDataType() == ImageIo::UINT16 ) {
				auto target = ImageTargetVkTexture2d<uint16_t>::create( this, channelOrder, isGray, imageSource->hasAlpha(), data );
				imageSource->load( target );	
			}
			else if( imageSource->getDataType() == ImageIo::FLOAT16 ) {
				auto target = ImageTargetVkTexture2d<half_float>::create( this, channelOrder, isGray, imageSource->hasAlpha(), data );
				imageSource->load( target );	
			}
			else {
				auto target = ImageTargetVkTexture2d<float>::create( this, channelOrder, isGray, imageSource->hasAlpha(), data );
				imageSource->load( target );	
			}
			premadeImage->unmap();
		}
	}

	if( mMipLevels > 1 ) {
		//generateMipmaps();
	}
}

void Texture2d::destroy()
{
	mImageView->~ImageView();
}

Texture2dRef Texture2d::create( int width, int height, const Texture2d::Format& format, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	Texture2dRef result = Texture2dRef( new Texture2d( width, height, format, device ) );
	return result;
}

Texture2dRef Texture2d::create( const void *data, VkFormat dataFormat, int width, int height, const Texture2d::Format &format, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	Texture2dRef result = Texture2dRef( new Texture2d( data, dataFormat, width, height, format, device ) );
	return result;
}

Texture2dRef Texture2d::create( const Surface8u& surf, const Texture2d::Format& format, vk::Device* device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	Texture2dRef result = Texture2dRef( new Texture2d( surf, format, device ) );
	return result;
}

Texture2dRef Texture2d::create( const Surface16u& surf, const Texture2d::Format& format, vk::Device* device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	Texture2dRef result = Texture2dRef( new Texture2d( surf, format, device ) );
	return result;
}

Texture2dRef Texture2d::create( const Surface32f& surf, const Texture2d::Format& format, vk::Device* device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	Texture2dRef result = Texture2dRef( new Texture2d( surf, format, device ) );
	return result;
}

//Texture2dRef Texture2d::create( ImageSourceRef imageSource, const Format &format, vk::Device *device )
//{
//	context = ( nullptr != context ) ? context : Context::getCurrent();
//	Texture2dRef result = Texture2dRef( new Texture2d( imageSource, format, context ) );
//	return result;
//}

template <typename T>
void Texture2d::doUpdate( int srcWidth, int srcHeight, const T *srcData, size_t srcRowBytes, size_t srcPixelBytes )
{
	auto context = vk::context()->getCurrent();

	// Force a 4-channel format since some devices will not accept a 3 channel format. 
	size_t numChannels = srcPixelBytes / sizeof( T );
	numChannels = ( 3 == numChannels ) ? 4 : numChannels;
	VkFormat format = findBestVkFormat<T>( numChannels );
	// Staging image
	Image::Format stagingOptions = Image::Format( format )
		.setSamples( VK_SAMPLE_COUNT_1_BIT )
		.setTilingLinear()
		.setMemoryPropertyHostVisible()
		.setUsageTransferSource();
	ImageRef stagingImage = Image::create( srcWidth, srcHeight, srcData, srcRowBytes, srcPixelBytes, ci::Area( 0, 0, srcWidth, srcHeight ), stagingOptions, mImageView->getDevice() );

	if( mFormat.isUnnormalizedCoordinates() ) {
		Image::copy( context, stagingImage, 0, 0, mImageView->getImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 0, ivec2( srcWidth, srcHeight ) );
	}
	else {
		// Use blit to generate mipmaps for each level
		ivec2 dstSize = ivec2( this->getWidth(), this->getHeight() );
		for( uint32_t mipLevel = 0; mipLevel < mMipLevels; ++mipLevel ) {
			ci::Area srcArea = ci::Area( 0, 0, srcWidth, srcHeight );
			ci::Area dstArea = ci::Area( 0, 0, dstSize.x, dstSize.y );
			Image::blit( context, stagingImage, 0, 0, srcArea, mImageView->getImage(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevel, 0, dstArea );
			dstSize /= 2;
		}
	}
}

void Texture2d::update( int srcWidth, int srcHeight, const uint8_t* srcData, size_t srcRowBytes, size_t srcPixelBytes )
{
	doUpdate( srcWidth, srcHeight, srcData, srcRowBytes, srcPixelBytes );
}

void Texture2d::update( int srcWidth, int srcHeight, const uint16_t* srcData, size_t srcRowBytes, size_t srcPixelBytes )
{
	doUpdate( srcWidth, srcHeight, srcData, srcRowBytes, srcPixelBytes );
}

void Texture2d::update( int srcWidth, int srcHeight, const float* srcData, size_t srcRowBytes, size_t srcPixelBytes )
{
	doUpdate( srcWidth, srcHeight, srcData, srcRowBytes, srcPixelBytes );
}

void Texture2d::update( const Surface8u& surf )
{
	update( surf.getWidth(), surf.getHeight(), surf.getData(), surf.getRowBytes(), surf.getPixelBytes() );
}

void Texture2d::update( const Surface16u& surf )
{
	update( surf.getWidth(), surf.getHeight(), surf.getData(), surf.getRowBytes(), surf.getPixelBytes() );
}

void Texture2d::update( const Surface32f& surf )
{
	update( surf.getWidth(), surf.getHeight(), surf.getData(), surf.getRowBytes(), surf.getPixelBytes() );
}

// -------------------------------------------------------------------------------------------------
// ImageTargetVkTexture2d
// -------------------------------------------------------------------------------------------------
// Create method receives an existing pointer which presumably is a mapped PBO
template<typename T>
std::shared_ptr<ImageTargetVkTexture2d<T>> ImageTargetVkTexture2d<T>::create( const Texture2d *texture, ImageIo::ChannelOrder &channelOrder, bool isGray, bool hasAlpha, void *intermediateDataStore )
{
	return std::shared_ptr<ImageTargetVkTexture2d<T>>( new ImageTargetVkTexture2d<T>( texture, channelOrder, isGray, hasAlpha, intermediateDataStore ) );
}

template<typename T>
ImageTargetVkTexture2d<T>::ImageTargetVkTexture2d( const Texture2d *texture, ImageIo::ChannelOrder &channelOrder, bool isGray, bool hasAlpha, void *intermediateDataStore )
	: ImageTarget(), mTexture( texture ), mHasAlpha( hasAlpha )
{
	if( isGray ) {
		mPixelInc = mHasAlpha ? 2 : 1;
	}
	else {
		mPixelInc = mHasAlpha ? 4 : 3;
	}
	mRowInc = mTexture->getWidth() * mPixelInc;
	
	// Allocate enough room to hold all these pixels if we haven't been passed a data*
	if( ! intermediateDataStore ) {
		mDataStore = std::unique_ptr<T[]>( new T[mTexture->getHeight() * mRowInc] );
		mDataBaseAddress = mDataStore.get();
	}
	else {
		mDataBaseAddress = reinterpret_cast<T*>( intermediateDataStore );
	}
	
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
void* ImageTargetVkTexture2d<T>::getRowPointer( int32_t row )
{
	return mDataBaseAddress + ( mTexture->getHeight() - 1 - row ) * mRowInc;
}

// -------------------------------------------------------------------------------------------------
// TextureCubeMap
// -------------------------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////////
// TextureCubeMap
namespace {
struct CubeMapFaceRegion {
	Area		mArea;
	ivec2		mOffset;
	bool		mFlip; // horizontal + vertical
};

std::vector<CubeMapFaceRegion> calcCubeMapHorizontalCrossRegions( const int imageWidth, const int imageHeight )
{
	std::vector<CubeMapFaceRegion> result;

	ivec2 faceSize( imageWidth / 4, imageHeight / 3 );
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
	
std::vector<CubeMapFaceRegion> calcCubeMapVerticalCrossRegions( const int imageWidth, const int imageHeight )
{
	std::vector<CubeMapFaceRegion> result;
	
	ivec2 faceSize( imageWidth / 3, imageHeight / 4 );
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
	
std::vector<CubeMapFaceRegion> calcCubeMapHorizontalRegions( const int imageWidth, const int imageHeight )
{
	std::vector<CubeMapFaceRegion> result;
	ivec2 faceSize( imageHeight, imageHeight );

	for( uint8_t index = 0; index < 6; ++index ) {
		Area area( static_cast<int32_t>( index * faceSize.x ), 0, (index + 1) * faceSize.x, faceSize.y );
		ivec2 offset( -index * faceSize.x, 0.0f );
		result.push_back( { area, offset, false } );
	}

	return result;
};
	
std::vector<CubeMapFaceRegion> calcCubeMapVerticalRegions( const int imageWidth, const int imageHeight )
{
	std::vector<CubeMapFaceRegion> result;
	ivec2 faceSize( imageWidth, imageWidth );
	
	for( uint8_t index = 0; index < 6; ++index ) {
		Area area( 0, static_cast<int32_t>( index * faceSize.x ), faceSize.x, (index + 1) * faceSize.y );
		ivec2 offset( 0.0f, -index * faceSize.y );
		result.push_back( { area, offset, false } );
	}

	return result;
};
} // anonymous namespace

TextureCubeMap::TextureCubeMap( int width, int height, TextureCubeMap::Format format, vk::Device *device )
	: TextureBase()
{
	initialize( width, height, static_cast<const uint8_t*>( nullptr ), 0, 0, format, device );
}

TextureCubeMap::TextureCubeMap( const Surface8u& surf, TextureCubeMap::Format format, vk::Device *device )
	: TextureBase()
{
	format.setInternalFormat( VK_FORMAT_R8G8B8A8_UNORM );
	initialize( surf.getWidth(), surf.getHeight(), surf.getData(), surf.getRowBytes(), surf.getPixelBytes(), format, device );
}

TextureCubeMap::TextureCubeMap( const Surface16u& surf, TextureCubeMap::Format format, vk::Device *device )
	: TextureBase()
{
	format.setInternalFormat( VK_FORMAT_R16G16B16A16_UNORM );
	initialize( surf.getWidth(), surf.getHeight(), surf.getData(), surf.getRowBytes(),  surf.getPixelBytes(), format, device );
}

TextureCubeMap::TextureCubeMap( const Surface32f& surf, TextureCubeMap::Format format, vk::Device *device )
	: TextureBase()
{
	format.setInternalFormat( VK_FORMAT_R32G32B32A32_SFLOAT);
	initialize( surf.getWidth(), surf.getHeight(),surf.getData(), surf.getRowBytes(), surf.getPixelBytes(), format, device );
}

//TextureCubeMap::TextureCubeMap( ImageSourceRef imageSource, TextureCubeMap::Format format, vk::Device *device )
//	: TextureBase()
//{
//	initialize( imageSource, format, context );
//}

TextureCubeMap::~TextureCubeMap()
{
	destroy();
}

void TextureCubeMap::initializeCommon( const TextureCubeMap::Format& format, vk::Device *device )
{
	mFormatProperties = {};
	vkGetPhysicalDeviceFormatProperties( device->getGpu(), format.getInternalFormat(), &mFormatProperties );

	// No mipmaps for cube maps
	mHostVisible = false;
	mMipLevels = 1;
}

void TextureCubeMap::initializeFinal( vk::Device *device )
{
    VkResult U_ASSERT_ONLY res;

    VkSamplerCreateInfo samplerCreateInfo = {};
    samplerCreateInfo.sType						= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter					= VK_FILTER_NEAREST;
    samplerCreateInfo.minFilter					= VK_FILTER_NEAREST;
	samplerCreateInfo.mipmapMode				= VK_SAMPLER_MIPMAP_MODE_NEAREST;
    samplerCreateInfo.addressModeU				= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.addressModeV				= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.addressModeW				= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerCreateInfo.mipLodBias				= 0.0f;
    samplerCreateInfo.maxAnisotropy				= 0;
    samplerCreateInfo.compareOp					= VK_COMPARE_OP_NEVER;
    samplerCreateInfo.minLod					= 0.0f;
    samplerCreateInfo.maxLod					= 0.0f;
    samplerCreateInfo.compareEnable				= VK_FALSE;
    samplerCreateInfo.borderColor				= VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	samplerCreateInfo.unnormalizedCoordinates	= VK_FALSE;
    res = vkCreateSampler( device ->getDevice(), &samplerCreateInfo, nullptr, &mSampler );
    assert(res == VK_SUCCESS);
	
	// Descriptor
	mImageInfo.imageView   = this->getImageView()->getImageView();
	mImageInfo.sampler     = this->mSampler;
	mImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

template <typename T>
void TextureCubeMap::initialize( int width, int height, const T* srcData, size_t srcRowBytes, size_t srcPixelBytes, const TextureCubeMap::Format& format, vk::Device *device )
{
	initializeCommon( format, device );

	std::vector<CubeMapFaceRegion> faceRegions;
	// Infer the layout based on image aspect ratio
	ivec2 imageSize( width, height );
	float minDim = (float)std::min( imageSize.x, imageSize.y );
	float maxDim = (float)std::max( imageSize.x, imageSize.y );
	float aspect = minDim / maxDim;
	if( abs( aspect - 1 / 6.0f ) < abs( aspect - 3 / 4.0f ) ) { // closer to 1:6 than to 4:3, so row or column
		faceRegions = ( imageSize.x > imageSize.y ) ? calcCubeMapHorizontalRegions( width, height ) : calcCubeMapVerticalRegions( width, height );
	}
	else { // else, horizontal or vertical cross
		faceRegions = ( imageSize.x > imageSize.y ) ? calcCubeMapHorizontalCrossRegions( width, height ) : calcCubeMapVerticalCrossRegions( width, height );
	}
	Area faceArea = faceRegions.front().mArea;
	ivec2 faceSize = faceArea.getSize();

	// Create the premade image
	vk::Image::Format imageOptions = vk::Image::Format( format.getInternalFormat() )
		.setArrayLayers( 6 )
		.setTilingOptimal()
		.setMemoryPropertyDeviceLocal()
		.setUsageSampled()
		.setUsageTransferDestination()
		.setInitialLayout( VK_IMAGE_LAYOUT_PREINITIALIZED )
		.setCreateFlags( VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT );
	vk::ImageRef premadeImage = vk::Image::create( faceSize.x, faceSize.y, imageOptions, device );
	//premadeImage->setImageLayout( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
	//ImageView::initialize( VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_TYPE_2D, premadeImage->getWidth(), premadeImage->getHeight(), 1, premadeImage->getOptions(), premadeImage );
	mImageView = vk::ImageView::createCube( premadeImage->getWidth(), premadeImage->getHeight(), premadeImage, device );

	Image::Format stagingOptions = Image::Format( format.getInternalFormat() )
		.setSamples( VK_SAMPLE_COUNT_1_BIT )
		.setTilingLinear()
		.setMemoryPropertyHostVisible()
		.setUsageTransferSource();
	ImageRef stagingImage = Image::create( faceSize.x, faceSize.y, stagingOptions, device );

	auto context = vk::Context::getCurrent();
	for( uint32_t face = 0; face < 6; ++face ) {
		stagingImage->copyData( 0, srcData, srcRowBytes, srcPixelBytes, faceRegions[face].mArea );
		Image::copy( context, stagingImage, 0, 0, premadeImage, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, face );
	}

	initializeFinal( device );
}

void TextureCubeMap::initialize( ImageSourceRef imageSource, const TextureCubeMap::Format& format, vk::Device *device )
{
}

void TextureCubeMap::destroy()
{
	mImageView->~ImageView();
}

TextureCubeMapRef TextureCubeMap::create( int width, int height, const TextureCubeMap::Format& format, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	TextureCubeMapRef result = TextureCubeMapRef( new TextureCubeMap( width, height, format, device ) );
	return result;
}

TextureCubeMapRef TextureCubeMap::create( const Surface8u& surf, const TextureCubeMap::Format& format, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	TextureCubeMapRef result = TextureCubeMapRef( new TextureCubeMap( surf, format, device ) );
	return result;
}

TextureCubeMapRef TextureCubeMap::create( const Surface16u& surf, const TextureCubeMap::Format& format, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	TextureCubeMapRef result = TextureCubeMapRef( new TextureCubeMap( surf, format, device ) );
	return result;
}

TextureCubeMapRef TextureCubeMap::create( const Surface32f& surf, const TextureCubeMap::Format& format, vk::Device *device )
{
	device = ( nullptr != device ) ? device : vk::Context::getCurrent()->getDevice();
	TextureCubeMapRef result = TextureCubeMapRef( new TextureCubeMap( surf, format, device ) );
	return result;
}

//TextureCubeMapRef TextureCubeMap::create( ImageSourceRef imageSource, const TextureCubeMap::Format &format, vk::Device *device )
//{
//	context = ( nullptr != context ) ? context : Context::getCurrent();
//	TextureCubeMapRef result = TextureCubeMapRef( new TextureCubeMap( imageSource, format, context ) );
//	return result;
//}

}} // namespace cinder::vk
