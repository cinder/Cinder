// The copyright in this software is being made available under the BSD License, included below. 
// This software may be subject to other third party and contributor rights, including patent rights, 
// and no such rights are granted under this license.
//
// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#include "cinder/dx/dx.h" // has to be first
#include "cinder/ImageIo.h"
#include "cinder/dx/DxTexture.h"
#include <stdio.h>
#include "cinder/app/AppImplMswRendererDx.h"
#include "cinder/dx/DDSTextureLoader.h"

#if defined( CINDER_WINRT )
	#include <ppltasks.h>
	#include "cinder/WinRTUtils.h"
	#include "cinder/Utilities.h"
	using namespace Windows::Storage;
	using namespace Concurrency;
#endif

using namespace std;

namespace cinder {
namespace dx {

const char* kErrorInsufficientNonFloatChannels = "Non-float textures need to have all four color channels (RGBA) defined";

class ImageSourceTexture;
class ImageTargetTexture;

TextureDataExc::TextureDataExc( const std::string &log ) throw()
{ strncpy_s( mMessage, log.c_str(), 16000 ); }

/////////////////////////////////////////////////////////////////////////////////
// ImageTargetGLTexture
template<typename T>
class ImageTargetGLTexture : public ImageTarget {
 public:
	static shared_ptr<ImageTargetGLTexture> createRef( const Texture *aTexture, ImageIo::ChannelOrder &aChannelOrder, bool aIsGray, bool aHasAlpha );
	~ImageTargetGLTexture();

	virtual bool	hasAlpha() const { return mHasAlpha; }
	virtual void*	getRowPointer( int32_t row ) { return mData + row * mRowInc; }

	const void*		getData() const { return mData; }
	
 private:
	ImageTargetGLTexture( const Texture *aTexture, ImageIo::ChannelOrder &aChannelOrder, bool aIsGray, bool aHasAlpha );
	const Texture		*mTexture;
	bool				mIsGray;
	bool				mHasAlpha;
	uint8_t				mPixelInc;
	T					*mData;
	int					mRowInc;
};

/////////////////////////////////////////////////////////////////////////////////
// Texture::Format
Texture::Format::Format()
{
	//mTarget = GL_TEXTURE_2D;
	mWrapS = D3D11_TEXTURE_ADDRESS_CLAMP;
	mWrapT = D3D11_TEXTURE_ADDRESS_CLAMP;
	mFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	mMipmapping = false;
	mInternalFormat = DXGI_FORMAT_UNKNOWN;
	mRenderTarget = false;
}

/////////////////////////////////////////////////////////////////////////////////
// Texture::Obj
Texture::~Texture()
{
	if( mDxTexture && ( ! mDoNotDispose ) ) {
		mDxTexture->Release();
	}

	if( mSamplerState ) {
		mSamplerState->Release();
	}
	
	if( mSRV ) { 
		mSRV->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////////
// Texture
Texture::Texture()
{
	init( -1, -1 );
}
Texture::Texture( int width, int height, Format format )
{
	init( width, height );
	if( DXGI_FORMAT_UNKNOWN == format.mInternalFormat ) {
		format.mInternalFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	}

	mInternalFormat = format.mInternalFormat;
	//mTarget = format.mTarget;
	//init( (unsigned char*)0, DXGI_FORMAT_R8G8B8A8_UNORM, format );
	init( (unsigned char*)nullptr, DXGI_FORMAT_UNKNOWN, format );
}

Texture::Texture( const unsigned char *data, DXGI_FORMAT dataFormat, int width, int height, Format format )
{
	init( width, height );
	if( DXGI_FORMAT_UNKNOWN == format.mInternalFormat ) {
		format.mInternalFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	mInternalFormat = format.mInternalFormat;
	//mTarget = format.mTarget;
	init( data, dataFormat, format );
}	

Texture::Texture( const Surface8u &surface, Format format )
{
	init( surface.getWidth(), surface.getHeight() );
	if( DXGI_FORMAT_UNKNOWN == format.mInternalFormat ) {
		format.mInternalFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	mInternalFormat = format.mInternalFormat;
	//mTarget = format.mTarget;

	//DXGI_FORMAT dataFormat;
	//CinderDxgiChannel type;
	//SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );

	init( surface.getData(), DXGI_FORMAT_R8G8B8A8_UNORM, format );	
}

Texture::Texture( const Surface32f &surface, Format format )
{
	init( surface.getWidth(), surface.getHeight() );

/*
#if (defined( CINDER_MSW ) || defined( CINDER_WINRT ))
	bool supportsTextureFloat = true;//GLEE_ARB_texture_float != 0;
#endif

	if( format.mInternalFormat < 0 ) {
		if( supportsTextureFloat ) {
			format.mInternalFormat = surface.hasAlpha() ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else {
			format.mInternalFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
	}
	mInternalFormat = format.mInternalFormat;
	//mTarget = format.mTarget;
*/

	format.mInternalFormat = surface.hasAlpha() ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R32G32B32_FLOAT;
	mInternalFormat = format.mInternalFormat;

	//DXGI_FORMAT dataFormat;
	//CinderDxgiChannel type;
	//SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );

	init( surface.getData(), surface.hasAlpha() ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R32G32B32_FLOAT, format );	
}

Texture::Texture( const Channel8u &channel, Format format )
{
	init( channel.getWidth(), channel.getHeight() );

	if( DXGI_FORMAT_UNKNOWN == format.mInternalFormat ) {
		format.mInternalFormat = DXGI_FORMAT_R8_UNORM;
	}

	mInternalFormat = format.mInternalFormat;

	// if the data is not already contiguous, we'll need to create a block of memory that is
	if( ( channel.getIncrement() != 1 ) || ( channel.getRowBytes() != channel.getWidth() * sizeof(uint8_t) ) ) {
		shared_ptr<uint8_t> data( new uint8_t[channel.getWidth() * channel.getHeight()], checked_array_deleter<uint8_t>() );
		uint8_t *dest = data.get();
		const int8_t inc = channel.getIncrement();
		const int32_t width = channel.getWidth();
		for( int y = 0; y < channel.getHeight(); ++y ) {
			const uint8_t *src = channel.getData( 0, y );
			for( int x = 0; x < width; ++x ) {
				*dest++ = *src;
				src += inc;
			}
		}

		init( data.get(), DXGI_FORMAT_R8_UNORM, format );
	}
	else {
		init( channel.getData(), DXGI_FORMAT_R8_UNORM, format );
	}
}

Texture::Texture( const Channel32f &channel, Format format )
{
	init( channel.getWidth(), channel.getHeight() );

/*
#if (defined( CINDER_MSW ) || defined( CINDER_WINRT ))
	bool supportsTextureFloat = true;//GLEE_ARB_texture_float != 0;
#endif

	if( format.mInternalFormat < 0 ) {
		if( supportsTextureFloat )
			format.mInternalFormat = DXGI_FORMAT_R32_FLOAT;
		else
			format.mInternalFormat = DXGI_FORMAT_R8_UNORM;
	}

	mInternalFormat = format.mInternalFormat;
*/

	format.mInternalFormat = DXGI_FORMAT_R32_FLOAT;
	mInternalFormat = format.mInternalFormat;

	// if the data is not already contiguous, we'll need to create a block of memory that is
	if( ( channel.getIncrement() != 1 ) || ( channel.getRowBytes() != channel.getWidth() * sizeof(float) ) ) {
		shared_ptr<float> data( new float[channel.getWidth() * channel.getHeight()], checked_array_deleter<float>() );
		float *dest = data.get();
		const int8_t inc = channel.getIncrement();
		const int32_t width = channel.getWidth();
		for( int y = 0; y < channel.getHeight(); ++y ) {
			const float *src = channel.getData( 0, y );
			for( int x = 0; x < width; ++x ) {
				*dest++ = *src;
				src += inc;
			}
		}

		init( data.get(), DXGI_FORMAT_R32_FLOAT, format );
	}
	else {
		init( channel.getData(), DXGI_FORMAT_R32_FLOAT, format );
	}
}

Texture::Texture( ImageSourceRef imageSource, Format format )
{
	init( imageSource, format );
}

#if defined( CINDER_WINRT )
void Texture::loadImageAsync(const fs::path path, dx::Texture &texture, const Format format)
{
	auto loadImageTask = create_task([path, format]() -> ImageSourceRef
	{
		return loadImage(path);
	});

	  // Task-based continuation.
    auto c2 = loadImageTask.then([path, &texture, format](task<ImageSourceRef> previousTask)
    {
        // We do expect to get here because task-based continuations 
        // are scheduled even when the antecedent task throws. 
        try
        {
			texture = Texture(previousTask.get(),format);
        }
        catch (const ImageIoExceptionFailedLoad&)
        {
			auto copyTask = winrt::copyFileToTempDirAsync(path);
			copyTask.then([&texture, format](StorageFile^ file) 
			{
				fs::path temp = fs::path(toUtf8(file->Path->Data()));
				texture = Texture(loadImage(fs::path(temp)), format);
				winrt::deleteFileAsync(temp);
			});
        }
    });
}
#endif

void Texture::init( int width, int height )
{
	mWidth = width;
	mHeight = height;
	mCleanWidth = width;
	mCleanHeight = height;
	mInternalFormat = (DXGI_FORMAT)-1;
	mFlipped = false;

	mDxTexture = nullptr;
	::ZeroMemory( &mSamplerDesc, sizeof(D3D11_SAMPLER_DESC) );
	mSamplerState = nullptr;
	mSRV = nullptr;
}

void Texture::init( const unsigned char *srcData, DXGI_FORMAT srcDataFormat, const Format &format )
{
	mDoNotDispose = false;
	bool bIsDepthStencil = (DXGI_FORMAT_D24_UNORM_S8_UINT == mInternalFormat);

	::ZeroMemory( &mSamplerDesc, sizeof(D3D11_SAMPLER_DESC) );
	mSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	mSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	mSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	mSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	mSamplerDesc.MipLODBias = 0;
	mSamplerDesc.MaxAnisotropy = 1;
	mSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	mSamplerDesc.BorderColor[0] = 0;
	mSamplerDesc.BorderColor[1] = 0;
	mSamplerDesc.BorderColor[2] = 0;
	mSamplerDesc.BorderColor[3] = 0;
	mSamplerDesc.MinLOD = 0;
	mSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = getDxRenderer()->md3dDevice->CreateSamplerState(&mSamplerDesc, &mSamplerState);
	if(hr != S_OK) {
		__debugbreak();
	}

	
	D3D11_TEXTURE2D_DESC texDesc;
	::ZeroMemory( &texDesc, sizeof(D3D11_TEXTURE2D_DESC) );
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = mInternalFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = ( bIsDepthStencil ? D3D11_BIND_DEPTH_STENCIL : D3D11_BIND_SHADER_RESOURCE ); 
	// If we're creating a render target - add bind support for render targets
	if( format.isRenderTarget() && ! bIsDepthStencil ) {
		texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	
	if( ! bIsDepthStencil && nullptr != srcData ) {
		uint32_t numChannels = dataFormatNumChannels( srcDataFormat );
		D3D11_SUBRESOURCE_DATA subData;
		::ZeroMemory( &subData, sizeof(D3D11_SUBRESOURCE_DATA) );
		subData.pSysMem = srcData;
		subData.SysMemPitch = mWidth*sizeof(unsigned char)*numChannels;
		subData.SysMemSlicePitch = subData.SysMemPitch*mHeight;
		hr = getDxRenderer()->md3dDevice->CreateTexture2D( &texDesc, &subData, &mDxTexture );
		if( FAILED( hr ) ) {
			__debugbreak();
		}
	}
	else {
		hr = getDxRenderer()->md3dDevice->CreateTexture2D( &texDesc, nullptr, &mDxTexture );
		if( FAILED( hr ) ) {
			__debugbreak();
		}
	}

	if( ! bIsDepthStencil ) {
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		::ZeroMemory( &srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = ( texDesc.SampleDesc.Count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D );
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;
		hr = getDxRenderer()->md3dDevice->CreateShaderResourceView( mDxTexture, &srvDesc, &mSRV );
		if( FAILED( hr ) ) {
			__debugbreak();
		}
	}
}

void Texture::init( const float *srcData, DXGI_FORMAT srcDataFormat, const Format &format )
{
	mDoNotDispose = false;

	::ZeroMemory( &mSamplerDesc, sizeof(D3D11_SAMPLER_DESC) );
	mSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	mSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	mSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	mSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	mSamplerDesc.MipLODBias = 0;
	mSamplerDesc.MaxAnisotropy = 1;
	mSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	mSamplerDesc.BorderColor[0] = 0;
	mSamplerDesc.BorderColor[1] = 0;
	mSamplerDesc.BorderColor[2] = 0;
	mSamplerDesc.BorderColor[3] = 0;
	mSamplerDesc.MinLOD = 0;
	mSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = getDxRenderer()->md3dDevice->CreateSamplerState(&mSamplerDesc, &mSamplerState);
	if( FAILED( hr ) ) {
		__debugbreak();
	}

	D3D11_TEXTURE2D_DESC desc;
	::ZeroMemory( &desc, sizeof(D3D11_TEXTURE2D_DESC) );
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = mInternalFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	if( nullptr != srcData ) {
		uint32_t numChannels = dataFormatNumChannels( srcDataFormat );
		D3D11_SUBRESOURCE_DATA subData;
		::ZeroMemory( &subData, sizeof(D3D11_SUBRESOURCE_DATA) );
		subData.pSysMem = srcData;
		subData.SysMemPitch = mWidth*sizeof(float)*numChannels;
		subData.SysMemSlicePitch = subData.SysMemPitch*mHeight;
		hr = getDxRenderer()->md3dDevice->CreateTexture2D( &desc, &subData, &mDxTexture );
		if( FAILED( hr ) ) {
			__debugbreak();
		}
	}
	else {
		hr = getDxRenderer()->md3dDevice->CreateTexture2D( &desc, nullptr, &mDxTexture );
		if( FAILED( hr ) ) {
			__debugbreak();
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	::ZeroMemory( &srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	hr = getDxRenderer()->md3dDevice->CreateShaderResourceView(mDxTexture, &srvDesc, &mSRV);
	if( FAILED( hr ) ) {
		__debugbreak();
	}
}

void Texture::init( ImageSourceRef imageSource, const Format &format )
{
	mDoNotDispose = false;
	//mTarget = format.mTarget;
	mWidth = mCleanWidth = imageSource->getWidth();
	mHeight = mCleanHeight = imageSource->getHeight();

	bool supportsTextureFloat = true;//GLEE_ARB_texture_float != 0;
	
	// Set the internal format based on the image's color space
	if( format.isAutoInternalFormat() ) {
		switch( imageSource->getColorModel() ) {
			case ImageIo::CM_RGB:
				if( imageSource->getDataType() == ImageIo::UINT8 ) {
					//mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R8G8B8A8_UNORM : throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
					if( ! imageSource->hasAlpha() ) {
						throw TextureDataExc( kErrorInsufficientNonFloatChannels );
					}
					mInternalFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
				}
				else if( imageSource->getDataType() == ImageIo::UINT16 ) {
					//mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R16G16B16A16_UNORM : throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
					if( ! imageSource->hasAlpha() ) {
						throw TextureDataExc( kErrorInsufficientNonFloatChannels );
					}
					mInternalFormat = DXGI_FORMAT_R16G16B16A16_UNORM;
				}
				else if( imageSource->getDataType() == ImageIo::FLOAT32 && supportsTextureFloat ) {
					mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R32G32B32_FLOAT;
				}
				else {
					//mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R8G8B8A8_UNORM : throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
					if( ! imageSource->hasAlpha() ) {
						throw TextureDataExc( kErrorInsufficientNonFloatChannels );
					}
					mInternalFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
				}
			break;

			case ImageIo::CM_GRAY:
				//@TODO: Add code to expand gray images to all 4 channels
				throw TextureDataExc( kErrorInsufficientNonFloatChannels );

				//if( imageSource->getDataType() == ImageIo::UINT8 )
				//	mInternalFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE8_ALPHA8 : GL_LUMINANCE8;
				//else if( imageSource->getDataType() == ImageIo::UINT16 )
				//	mInternalFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE16_ALPHA16 : GL_LUMINANCE16;
				//else if( imageSource->getDataType() == ImageIo::FLOAT32 && supportsTextureFloat )
				//	mInternalFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE_ALPHA32F_ARB : GL_LUMINANCE32F_ARB;
				//else
				//	mInternalFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE_ALPHA : GL_LUMINANCE;
			break;

			default:
				throw ImageIoExceptionIllegalColorModel();
			break;
		}
	}
	else {
		mInternalFormat = format.mInternalFormat;
	}

	// setup an appropriate dataFormat/ImageTargetTexture based on the image's color space
	//GLint dataFormat;
	DXGI_FORMAT dataFormat = DXGI_FORMAT_UNKNOWN;
	ImageIo::ChannelOrder channelOrder;
	bool isGray = false;
	switch( imageSource->getColorModel() ) {
		case ImageSource::CM_RGB: {
			//dataFormat = ( imageSource->hasAlpha() ) ? GL_RGBA : GL_RGB;
			//channelOrder = ImageIo::RGBA : ImageIo::RGB;

			bool isFloat = ( ImageSource::FLOAT32 == imageSource->getDataType() );
			if( ! isFloat &&  ! imageSource->hasAlpha() ) {
				throw TextureDataExc( kErrorInsufficientNonFloatChannels );
			}

			switch( imageSource->getDataType() ) {
			case ImageSource::UINT8:
				dataFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;

			case ImageSource::UINT16:
				dataFormat = DXGI_FORMAT_R16G16B16A16_UNORM;
			break;

			case ImageSource::FLOAT32:
				dataFormat = imageSource->hasAlpha() ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R32G32B32_FLOAT;
			break;

			default:
			break;
			}

			channelOrder = ImageIo::RGBA;
		}
		break;

		case ImageSource::CM_GRAY:
			throw TextureDataExc( kErrorInsufficientNonFloatChannels );
			//dataFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE_ALPHA : GL_LUMINANCE;
			//channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::YA : ImageIo::Y;
			//isGray = true;
		break;

		// if this is some other color space, we'll have to punt and go w/ RGBA
		default: 
		{			
			//dataFormat = ( imageSource->hasAlpha() ) ? GL_RGBA : throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
			//channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::RGBA : ImageIo::RGB;
			if( ! imageSource->hasAlpha() ) {
				throw TextureDataExc( kErrorInsufficientNonFloatChannels );
			}

			dataFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			channelOrder = ImageIo::RGBA;
		}
		break;
	}

	//if( mTarget == GL_TEXTURE_2D ) {
		mMaxU = mMaxV = 1.0f;
	//}
	//else {
	//	mMaxU = (float)mWidth;
	//	mMaxV = (float)mHeight;
	//}
		
	::ZeroMemory( &mSamplerDesc, sizeof(D3D11_SAMPLER_DESC) );
	mSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	mSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	mSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	mSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	mSamplerDesc.MipLODBias = 0;
	mSamplerDesc.MaxAnisotropy = 1;
	mSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	mSamplerDesc.BorderColor[0] = 0;
	mSamplerDesc.BorderColor[1] = 0;
	mSamplerDesc.BorderColor[2] = 0;
	mSamplerDesc.BorderColor[3] = 0;
	mSamplerDesc.MinLOD = 0;
	mSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = getDxRenderer()->md3dDevice->CreateSamplerState(&mSamplerDesc, &mSamplerState);
	if(hr != S_OK) {
		__debugbreak();
	}

	D3D11_TEXTURE2D_DESC texDesc;
	::ZeroMemory( &texDesc, sizeof(D3D11_TEXTURE2D_DESC) );
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subData;
	::ZeroMemory( &subData, sizeof(D3D11_SUBRESOURCE_DATA) );

	if( ImageIo::UINT8 == imageSource->getDataType() ) {
		const int numChannels = 4;
		shared_ptr<ImageTargetGLTexture<uint8_t>> target = ImageTargetGLTexture<uint8_t>::createRef( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		subData.pSysMem = target->getData();
		subData.SysMemPitch = numChannels*sizeof(uint8_t)*mWidth;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//OLD: - is there a bug or something? 
		// subData.SysMemSlicePitch = 0;//subData.SysMemPitch * mHeight;
		//
		subData.SysMemSlicePitch = subData.SysMemPitch*mHeight;
		getDxRenderer()->md3dDevice->CreateTexture2D(&texDesc, &subData, &mDxTexture);
	}
	else if( ImageIo::UINT16 == imageSource->getDataType() ) {
		const int numChannels = 4;
		shared_ptr<ImageTargetGLTexture<uint16_t>> target = ImageTargetGLTexture<uint16_t>::createRef( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		subData.pSysMem = target->getData();
		subData.SysMemPitch = numChannels*sizeof(uint16_t)*mWidth;
		texDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
		subData.SysMemSlicePitch = subData.SysMemPitch*mHeight;
		getDxRenderer()->md3dDevice->CreateTexture2D(&texDesc, &subData, &mDxTexture);
	}
	else {
		const int numChannels = dataFormatNumChannels( dataFormat );
		shared_ptr<ImageTargetGLTexture<float>> target = ImageTargetGLTexture<float>::createRef( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		subData.pSysMem = target->getData();
		texDesc.Format = dataFormat;
		subData.SysMemPitch = numChannels*sizeof(float)*mWidth;
		//if(dataFormat == GL_RGBA) {
		//	const int numChannels = 4;
		//	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		//	subData.SysMemPitch = numChannels*sizeof(float)*mWidth;
		//}
		//else {
		//	const int numChannels = 3;
		//	texDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		//	subData.SysMemPitch = numChannels*sizeof(float)*mWidth;
		//}
		subData.SysMemSlicePitch = subData.SysMemPitch*mHeight;
		getDxRenderer()->md3dDevice->CreateTexture2D(&texDesc, &subData, &mDxTexture);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	::ZeroMemory( &srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	getDxRenderer()->md3dDevice->CreateShaderResourceView(mDxTexture, &srvDesc, &mSRV);
}

void Texture::update( const Surface &surface )
{
	if( ( surface.getWidth() != getWidth() ) || ( surface.getHeight() != getHeight() ) ) {
		throw TextureDataExc( "Invalid Texture::update() surface dimensions" );
	}

	DXGI_FORMAT dataFormat;
	CinderDxgiChannel type;
	SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );

	D3D11_BOX box;
	box.left	= 0;
	box.right	= surface.getWidth();
	box.top		= 0;
	box.bottom	= surface.getHeight();
	box.front	= 0;
	box.back	= 1;

	auto dx = getDxRenderer();

	UINT srcRowPitch = surface.getWidth()*dataFormatNumChannels( dataFormat )*sizeof(uint8_t);
	UINT srcDepthPitch = 0;
	dx->mDeviceContext->UpdateSubresource( mDxTexture, 0, &box, surface.getData(), srcRowPitch, srcDepthPitch );
}

void Texture::update( const Surface32f &surface )
{
	if( ( surface.getWidth() != getWidth() ) || ( surface.getHeight() != getHeight() ) ) {
		throw TextureDataExc( "Invalid Texture::update() surface dimensions" );
	}

	DXGI_FORMAT dataFormat;
	CinderDxgiChannel type;
	SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );

	D3D11_BOX box;
	box.left	= 0;
	box.right	= surface.getWidth();
	box.top		= 0;
	box.bottom	= surface.getHeight();
	box.front	= 0;
	box.back	= 1;

	auto dx = getDxRenderer();

	UINT srcRowPitch = surface.getWidth()*dataFormatNumChannels( dataFormat )*sizeof(float);
	UINT srcDepthPitch = 0;
	dx->mDeviceContext->UpdateSubresource( mDxTexture, 0, &box, surface.getData(), srcRowPitch, srcDepthPitch );
}

void Texture::update( const Surface &surface, const Area &area )
{
	//GLint dataFormat;
	//GLenum type;
	DXGI_FORMAT dataFormat;
	CinderDxgiChannel type;
	SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );

	D3D11_BOX box;
	box.left	= area.getX1();
	box.right	= area.getX2();
	box.top		= area.getY1();
	box.bottom	= area.getY2();
	box.front	= 0;
	box.back	= 1;

	auto dx = getDxRenderer();

	UINT srcRowPitch = surface.getWidth()*dataFormatNumChannels( dataFormat )*sizeof(uint8_t);
	UINT srcDepthPitch = 0;
	dx->mDeviceContext->UpdateSubresource( mDxTexture, 0, &box, surface.getData( area.getUL() ), srcRowPitch, srcDepthPitch );
}

void Texture::update( const Channel &channel )
{
	if( ( channel.getWidth() != getWidth() ) || ( channel.getHeight() != getHeight() ) ) {
		throw TextureDataExc( "Invalid Texture::update() channel dimensions" );
	}
	
	Area area = channel.getBounds();
	update( channel, area );
}

void Texture::update( const Channel32f &channel )
{
	if( ( channel.getWidth() != getWidth() ) || ( channel.getHeight() != getHeight() ) ) {
		throw TextureDataExc( "Invalid Texture::update() channel dimensions" );
	}

	D3D11_BOX box;
	box.left	= 0;
	box.right	= getWidth();
	box.top		= 0;
	box.bottom	= getHeight();
	box.front	= 0;
	box.back	= 1;

	UINT srcRowPitch = channel.getWidth()*sizeof(float);
	UINT srcDepthPitch = 0;
	getDxRenderer()->mDeviceContext->UpdateSubresource( mDxTexture, 0, &box, channel.getData(), srcRowPitch, srcDepthPitch );
}

void Texture::update( const Channel8u &channel, const Area &area )
{
	D3D11_BOX box;
	box.left	= area.getX1();
	box.right	= area.getX2();
	box.top		= area.getY1();
	box.bottom	= area.getY2();
	box.front	= 0;
	box.back =	 1;

	if( ( channel.getIncrement() != 1 ) || ( channel.getRowBytes() != channel.getWidth() * sizeof(uint8_t) ) ) {
		shared_ptr<uint8_t> data( new uint8_t[area.getWidth() * area.getHeight()], checked_array_deleter<uint8_t>() );
		uint8_t *dest = data.get();
		const int8_t inc = channel.getIncrement();
		const int32_t width = area.getWidth();
		for( int y = 0; y < area.getHeight(); ++y ) {
			const uint8_t *src = channel.getData( area.getX1(), area.getY1() + y );
			for( int x = 0; x < width; ++x ) {
				*dest++ = *src;
				src += inc;
			}
		}
		getDxRenderer()->mDeviceContext->UpdateSubresource( mDxTexture, 0, &box, data.get(), area.getWidth(), 0 );
	}
	else {
		getDxRenderer()->mDeviceContext->UpdateSubresource( mDxTexture, 0, &box, channel.getData( area.getUL() ), area.getWidth(), 0 );
	}
}

//void Texture::SurfaceChannelOrderToDataFormatAndType( const SurfaceChannelOrder &sco, GLint *dataFormat, GLenum *type )
//{
//	switch( sco.getCode() ) {
//		case SurfaceChannelOrder::RGB:
//			*dataFormat = GL_RGB;
//			*type = GL_UNSIGNED_BYTE;
//		break;
//		case SurfaceChannelOrder::RGBA:
//		case SurfaceChannelOrder::RGBX:
//			*dataFormat = GL_RGBA;
//			*type = GL_UNSIGNED_BYTE;
//		break;
//		case SurfaceChannelOrder::BGRA:
//		case SurfaceChannelOrder::BGRX:
//			*dataFormat = GL_BGRA;
//			*type = GL_UNSIGNED_BYTE;
//		break;
//#if ! defined( CINDER_GLES )
//		case SurfaceChannelOrder::BGR:
//			*dataFormat = GL_BGR;
//			*type = GL_UNSIGNED_BYTE;
//		break;
//		case SurfaceChannelOrder::ARGB:
//			*dataFormat = GL_BGRA;
//			*type = GL_UNSIGNED_INT_8_8_8_8;
//		break;
//#endif // ! defined( CINDER_GLES )
//		default:
//			throw TextureDataExc( "Invalid channel order" ); // this is an unsupported channel order for a texture
//		break;
//	}
//}
void Texture::SurfaceChannelOrderToDataFormatAndType( const SurfaceChannelOrder &sco, DXGI_FORMAT *dataFormat, CinderDxgiChannel* type, bool isSurface32f )
{
	switch( sco.getCode() ) {
		case SurfaceChannelOrder::RGB:
			*dataFormat = isSurface32f ? DXGI_FORMAT_R32G32B32_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
			*type = isSurface32f ? CINDER_DXGI_CHAHNNEL_32_FLOAT : CINDER_DXGI_CHAHNNEL_8_UNORM;
		break;

		case SurfaceChannelOrder::RGBA:
		case SurfaceChannelOrder::RGBX:
			*dataFormat = isSurface32f ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
			*type = isSurface32f ? CINDER_DXGI_CHAHNNEL_32_FLOAT : CINDER_DXGI_CHAHNNEL_8_UNORM;
		break;

		case SurfaceChannelOrder::BGRA:
		{
			if( isSurface32f ) {
				throw TextureDataExc( "Invalid channel order" ); // this is an unsupported channel order for a texture
			}
		
			*dataFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
			*type = isSurface32f ? CINDER_DXGI_CHAHNNEL_32_FLOAT : CINDER_DXGI_CHAHNNEL_8_UNORM;
		}
		break;

		case SurfaceChannelOrder::BGRX:
		{
			if( isSurface32f ) {
				throw TextureDataExc( "Invalid channel order" ); // this is an unsupported channel order for a texture
			}
		
			*dataFormat = DXGI_FORMAT_B8G8R8X8_UNORM;
			*type = isSurface32f ? CINDER_DXGI_CHAHNNEL_32_FLOAT : CINDER_DXGI_CHAHNNEL_8_UNORM;
		}
		break;

		default:
			throw TextureDataExc( "Invalid channel order" ); // this is an unsupported channel order for a texture
		break;
	}
}

//bool Texture::dataFormatHasAlpha( GLint dataFormat )
//{
//	switch( dataFormat ) {
//		case GL_RGBA:
//		case GL_ALPHA:
//		case GL_LUMINANCE_ALPHA:
//#if ! defined( CINDER_GLES )
//		case GL_BGRA:
//#endif
//			return true;
//		break;
//		default:
//			return false;
//	}
//}
bool Texture::dataFormatHasAlpha( DXGI_FORMAT dataFormat )
{
	switch( dataFormat ) {
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return true;
	}

	return false;
}

//bool Texture::dataFormatHasColor( GLint dataFormat )
//{
//	switch( dataFormat ) {
//		case GL_ALPHA:
//		case GL_LUMINANCE:
//		case GL_LUMINANCE_ALPHA:
//			return false;
//		break;
//	}
//	
//	return true;
//}
bool Texture::dataFormatHasColor( DXGI_FORMAT dataFormat )
{
	switch( dataFormat ) {
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return true;
	}

	return false;
}

uint32_t Texture::dataFormatNumChannels( DXGI_FORMAT dataFormat ) 
{
	uint32_t result = 0;
	switch( dataFormat ) {
	// 1 Channel
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
		result = 1;
	break;

	// 2 Channels
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R32G32_FLOAT:
		result = 2;
	break;

	// 3 Channels
	case DXGI_FORMAT_R32G32B32_FLOAT: 
		result = 3; 
	break;

	// 4 Channels
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		result = 4;
	break;
	}
	return result;
}

TextureRef Texture::loadDds( IStreamRef ddsStream, Format format )
{
	TextureRef texture( new Texture() );
	app::AppImplMswRendererDx *dxRenderer = reinterpret_cast<app::AppImplMswRendererDx*>(reinterpret_cast<app::RendererDx*>(&*app::App::get()->getRenderer())->mImpl);
	uint8_t *data = (uint8_t*)malloc(ddsStream->size());
	if( ! data ) {
		throw TextureDataExc("Not enough memory to load DDS");
	}
	ddsStream->read(data);
	DirectX::CreateDDSTextureFromMemory(dxRenderer->md3dDevice, data, ddsStream->size(), (ID3D11Resource**)&texture->mDxTexture, &texture->mSRV);
	free(data);
	texture->mDoNotDispose = false;
	texture->mWidth = texture->getWidth();
	texture->mHeight = texture->getHeight();
	return texture;
}

void Texture::setWrapS( D3D11_TEXTURE_ADDRESS_MODE wrapS )
{
	mSamplerState->Release();
	mSamplerDesc.AddressU = wrapS;
	getDxRenderer()->md3dDevice->CreateSamplerState(&mSamplerDesc, &mSamplerState);
}

void Texture::setWrapT( D3D11_TEXTURE_ADDRESS_MODE wrapT )
{
	mSamplerState->Release();
	mSamplerDesc.AddressV = wrapT;
	getDxRenderer()->md3dDevice->CreateSamplerState(&mSamplerDesc, &mSamplerState);
}

void Texture::setFilter( D3D11_FILTER filter )
{
	mSamplerState->Release();
	mSamplerDesc.Filter = filter;
	getDxRenderer()->md3dDevice->CreateSamplerState(&mSamplerDesc, &mSamplerState);
}

void Texture::setCleanTexCoords( float maxU, float maxV )
{
	mMaxU = maxU;
	mMaxV = maxV;
	
	//if( mTarget == GL_TEXTURE_2D ) {
		mCleanWidth = (GLint)(getWidth() * maxU);
		mCleanHeight = (GLint)(getHeight() * maxV);
	//}
	//else {
	//	mCleanWidth = (int32_t)maxU;
	//	mCleanHeight = (int32_t)maxV;
	//}
}

bool Texture::hasAlpha() const
{
	bool result = false;

	switch( mInternalFormat ) {
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		result = true;
	break;
	}

	return result;
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

DXGI_FORMAT Texture::getInternalFormat() const
{
	D3D11_TEXTURE2D_DESC desc;
	::ZeroMemory( &desc, sizeof(D3D11_TEXTURE2D_DESC) );
	mDxTexture->GetDesc(&desc);
	return desc.Format;
}

UINT Texture::getWidth() const
{
	//D3D11_TEXTURE2D_DESC desc;
	//mDxTexture->GetDesc(&desc);
	//return desc.Width;
	return mWidth;
}

UINT Texture::getHeight() const
{
	//D3D11_TEXTURE2D_DESC desc;
	//mDxTexture->GetDesc(&desc);
	//return desc.Height;
	return mHeight;
}

UINT Texture::getCleanWidth() const
{
	if( mCleanWidth == -1 ) {
		mWidth = getWidth();
		mCleanWidth = mWidth;
	}

	return mCleanWidth;
}

UINT Texture::getCleanHeight() const
{
	if( mCleanHeight == -1 ) {
		mHeight = getHeight();
		mCleanHeight = mHeight;		
	}
	return mHeight;
}

Rectf Texture::getAreaTexCoords( const Area &area ) const
{
	Rectf result;

	//if( mTarget == GL_TEXTURE_2D ) {
		result.x1 = area.x1 / (float)getWidth();
		result.x2 = area.x2 / (float)getWidth();
		result.y1 = area.y1 / (float)getHeight();
		result.y2 = area.y2 / (float)getHeight();	
	//}
	//else {
	//	result = Rectf( area );
	//}
	
	if( mFlipped ) {
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

void Texture::bind( UINT textureUnit ) const
{
	getDxRenderer()->mDeviceContext->PSSetShaderResources(textureUnit, 1, &mSRV);
	getDxRenderer()->mDeviceContext->PSSetSamplers(textureUnit, 1, &mSamplerState);
}

void Texture::unbind( UINT textureUnit ) const
{
	ID3D11ShaderResourceView *none = NULL;
	getDxRenderer()->mDeviceContext->PSSetShaderResources(0, 1, &none);
}

/////////////////////////////////////////////////////////////////////////////////
// TextureCache
/*
TextureCache::TextureCache( const Surface8u &prototypeSurface, const Texture::Format &format )
	: mObj( shared_ptr<Obj>( new Obj( prototypeSurface, format ) ) )
{
	
}


dx::Texture	TextureCache::cache( const Surface8u &data )
{
	pair<int,Texture> *resultPair = nullptr;
	
	// find an available slot and update that if possible
	bool found = false;
	for( vector<pair<int,Texture> >::iterator texIt = mTextures.begin(); texIt != mTextures.end(); ++texIt ) {
		if( texIt->first == -1 ) { // this texture is available, let's use it!
			resultPair = &(*texIt);
			resultPair->second.update( data );			
			found = true;
			break;
		}
	}
	
	// we didn't find an available slot, so let's make a new texture
	if( ! found ) {
		mTextures.push_back( make_pair( -1, dx::Texture( data, mFormat ) ) );
		resultPair = &mTextures.back();
	}

	resultPair->first = mNextId++;
	Texture result = resultPair->second.weakClone();
	
	pair<shared_ptr<TextureCache::Obj>,int> *refcon = new pair<shared_ptr<TextureCache::Obj>,int>( mObj, resultPair->first );
	result.setDeallocator( Obj::TextureCacheDeallocator, refcon );
	return result;
}
	
TextureCache::Obj::Obj( const Surface8u &prototypeSurface, const Texture::Format &format )
	: mWidth( prototypeSurface.getWidth() ), mHeight( prototypeSurface.getHeight() ), mFormat( format ), mNextId( 0 )
{
}

void TextureCache::Obj::markTextureAsFree( int id )
{
	for( vector<pair<int,Texture> >::iterator texIt = mTextures.begin(); texIt != mTextures.end(); ++texIt ) {
		if( texIt->first == id ) { // this texture is available, let's use it!
			texIt->first = -1;
			break;
		}
	}
}

void TextureCache::Obj::TextureCacheDeallocator( void *aDeallocatorRefcon )
{
	pair<shared_ptr<TextureCache::Obj>,int> *refconPair = reinterpret_cast<pair<shared_ptr<TextureCache::Obj>,int>* >( aDeallocatorRefcon );
	refconPair->first->markTextureAsFree( refconPair->second );
	delete refconPair;
}
*/

/////////////////////////////////////////////////////////////////////////////////
// ImageTargetGLTexture
template<typename T>
shared_ptr<ImageTargetGLTexture<T> > ImageTargetGLTexture<T>::createRef( const Texture *aTexture, ImageIo::ChannelOrder &aChannelOrder, bool aIsGray, bool aHasAlpha )
{
	return shared_ptr<ImageTargetGLTexture<T> >( new ImageTargetGLTexture<T>( aTexture, aChannelOrder, aIsGray, aHasAlpha ) );
}

template<typename T>
ImageTargetGLTexture<T>::ImageTargetGLTexture( const Texture *aTexture, ImageIo::ChannelOrder &aChannelOrder, bool aIsGray, bool aHasAlpha )
	: ImageTarget(), mTexture( aTexture ), mIsGray( aIsGray ), mHasAlpha( aHasAlpha )
{
	if( mIsGray )
		throw TextureDataExc( kErrorInsufficientNonFloatChannels );//mPixelInc = ( mHasAlpha ) ? 2 : 1;
	else
		mPixelInc = ( mHasAlpha ) ? 4 : /*3*/throw TextureDataExc( kErrorInsufficientNonFloatChannels );
	mRowInc = mTexture->getWidth() * mPixelInc;
	// allocate enough room to hold all these pixels
	mData = new T[mTexture->getHeight() * mRowInc];
	
	if( boost::is_same<T,uint8_t>::value )
		setDataType( ImageIo::UINT8 );
	else if( boost::is_same<T,uint16_t>::value )
		setDataType( ImageIo::UINT16 );
	else if( boost::is_same<T,float>::value )
		setDataType( ImageIo::FLOAT32 );		

	setChannelOrder( aChannelOrder );
	setColorModel( mIsGray ? ImageIo::CM_GRAY : ImageIo::CM_RGB );
}

template<typename T>
ImageTargetGLTexture<T>::~ImageTargetGLTexture()
{
	delete [] mData;
}

/////////////////////////////////////////////////////////////////////////////////
// ImageSourceTexture
#if ! defined( CINDER_GLES )
class ImageSourceTexture : public ImageSource {
  public:
	ImageSourceTexture( const Texture &texture )
		: ImageSource()
	{
		throw TextureDataExc("Haven't implemented ImageSourceTexture::ImageSourceTexture yet");
		mWidth = texture.getWidth();
		mHeight = texture.getHeight();
		
		GLint internalFormat = texture.getInternalFormat();
		GLenum format;
		switch( internalFormat ) {
			case GL_RGB: setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); format = GL_RGB; break;
			case GL_RGBA: setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); format = GL_RGBA; break;
			case GL_LUMINANCE: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_LUMINANCE; break;
			case GL_LUMINANCE_ALPHA: setChannelOrder( ImageIo::YA ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_LUMINANCE_ALPHA; break;
#if ! defined( CINDER_GLES )
			case GL_RGBA8: setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); format = GL_RGBA; break; 
			case GL_RGB8: setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::UINT8 ); format = GL_RGB; break;
			case GL_BGR: setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); format = GL_RGB; break;
			case GL_LUMINANCE8: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT8 ); format = GL_LUMINANCE; break;
			case GL_LUMINANCE8_ALPHA8: setChannelOrder( ImageIo::YA ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT8 ); format = GL_LUMINANCE_ALPHA; break; 
			case GL_DEPTH_COMPONENT16: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::UINT16 ); format = GL_DEPTH_COMPONENT; break;
			case GL_DEPTH_COMPONENT24: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_DEPTH_COMPONENT; break;
			case GL_DEPTH_COMPONENT32: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_DEPTH_COMPONENT; break;
			case GL_RGBA32F_ARB: setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); format = GL_RGBA; break; 
			case GL_RGB32F_ARB: setChannelOrder( ImageIo::RGB ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); format = GL_RGB; break;
			case GL_LUMINANCE32F_ARB: setChannelOrder( ImageIo::Y ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_LUMINANCE; break;
			case GL_LUMINANCE_ALPHA32F_ARB: setChannelOrder( ImageIo::YA ); setColorModel( ImageIo::CM_GRAY ); setDataType( ImageIo::FLOAT32 ); format = GL_LUMINANCE_ALPHA; break;
#endif
			default: setChannelOrder( ImageIo::RGBA ); setColorModel( ImageIo::CM_RGB ); setDataType( ImageIo::FLOAT32 ); break;
		}

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
		mData = shared_ptr<uint8_t>( new uint8_t[mRowBytes * mHeight], boost::checked_array_delete<uint8_t> );
		//gl::SaveTextureBindState( texture.getTarget() );
		texture.bind();
		//glPixelStorei( GL_PACK_ALIGNMENT, 1 );
		//glGetTexImage( texture.getTarget(), 0, format, dataType, mData.get() );
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
	
	shared_ptr<uint8_t>	mData;
	int32_t				mRowBytes;
};

Texture::operator ImageSourceRef() const
{
	return shared_ptr<ImageSource>( new ImageSourceTexture( *this ) );
}

#endif // ! defined( CINDER_GLES )

} } // namespace cinder::gl
