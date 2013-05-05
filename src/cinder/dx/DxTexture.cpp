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
	mInternalFormat = (DXGI_FORMAT)-1;
}

/////////////////////////////////////////////////////////////////////////////////
// Texture::Obj
Texture::~Texture()
{
	if( mDxTexture && ( ! mDoNotDispose ) ) {
		mDxTexture->Release();
	}
	if( mSamplerState ) mSamplerState->Release();
	if( mSRV ) mSRV->Release();
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
	if( format.mInternalFormat == -1 )
		format.mInternalFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	mInternalFormat = format.mInternalFormat;
	//mTarget = format.mTarget;
	init( (unsigned char*)0, DXGI_FORMAT_R8G8B8A8_UNORM, format );
}

Texture::Texture( const unsigned char *data, DXGI_FORMAT dataFormat, int width, int height, Format format )
{
	init( width, height );
	if( format.mInternalFormat == -1 )
		format.mInternalFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	mInternalFormat = format.mInternalFormat;
	//mTarget = format.mTarget;
	init( data, dataFormat, format );
}	

Texture::Texture( const Surface8u &surface, Format format )
{
	init( surface.getWidth(), surface.getHeight() );
	if( format.mInternalFormat < 0 )
		format.mInternalFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	mInternalFormat = format.mInternalFormat;
	//mTarget = format.mTarget;

	GLint dataFormat;
	GLenum type;
	SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );

	init( surface.getData(), DXGI_FORMAT_R8G8B8A8_UNORM, format );	
}

Texture::Texture( const Surface32f &surface, Format format )
{
	init( surface.getWidth(), surface.getHeight() );
#if (defined( CINDER_MSW ) || defined( CINDER_WINRT ))
	bool supportsTextureFloat = true;//GLEE_ARB_texture_float != 0;
#endif

	if( format.mInternalFormat < 0 ) {
#if ! defined( CINDER_GLES )
		if( supportsTextureFloat )
			format.mInternalFormat = surface.hasAlpha() ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R32G32B32_FLOAT;
		else
			format.mInternalFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
#else
		format.mInternalFormat = surface.hasAlpha() ? GL_RGBA : GL_RGB;
#endif	
	}
	mInternalFormat = format.mInternalFormat;
	//mTarget = format.mTarget;

	init( surface.getData(), surface.hasAlpha()?DXGI_FORMAT_R32G32B32A32_FLOAT:DXGI_FORMAT_R32G32B32_FLOAT, format );	
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
	mDxTexture = NULL;
	mSamplerState = NULL;
	mSRV = NULL;
}

void Texture::init( const unsigned char *data, DXGI_FORMAT dataFormat, const Format &format )
{
	mDoNotDispose = false;

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
	if(hr != S_OK)
		__debugbreak();

	D3D11_SUBRESOURCE_DATA subData;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = dataFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	subData.pSysMem = data;
	subData.SysMemPitch = mWidth * 4;
	subData.SysMemSlicePitch = subData.SysMemPitch * mHeight;
	hr = getDxRenderer()->md3dDevice->CreateTexture2D(&desc, &subData, &mDxTexture);
	if(hr != S_OK)
		__debugbreak();

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = -1;
	getDxRenderer()->md3dDevice->CreateShaderResourceView(mDxTexture, &SRVDesc, &mSRV);
}

void Texture::init( const float *data, DXGI_FORMAT dataFormat, const Format &format )
{
	mDoNotDispose = false;

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
	if(hr != S_OK)
		__debugbreak();

	D3D11_SUBRESOURCE_DATA subData;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.Format = dataFormat;
	switch(dataFormat)
	{
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			subData.SysMemPitch = 4 * 4 * mWidth;
			break;

		case DXGI_FORMAT_R32G32B32_FLOAT:
			subData.SysMemPitch = 3 * 4 * mWidth;
			break;

		case DXGI_FORMAT_R32_FLOAT:
			desc.Format = DXGI_FORMAT_R32_FLOAT;
			subData.SysMemPitch = 4 * mWidth;
			break;
	}
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	subData.pSysMem = data;
	subData.SysMemSlicePitch = subData.SysMemPitch * mHeight;
	getDxRenderer()->md3dDevice->CreateTexture2D(&desc, &subData, &mDxTexture);
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
#if ! defined( CINDER_GLES )
			case ImageIo::CM_RGB:
				if( imageSource->getDataType() == ImageIo::UINT8 )
					mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R8G8B8A8_UNORM : throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
				else if( imageSource->getDataType() == ImageIo::UINT16 )
					mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R16G16B16A16_UNORM : throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
				else if( imageSource->getDataType() == ImageIo::FLOAT32 && supportsTextureFloat )
					mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R32G32B32A32_FLOAT : DXGI_FORMAT_R32G32B32_FLOAT;
				else
					mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R8G8B8A8_UNORM : throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
			break;
			case ImageIo::CM_GRAY:
				throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
				//if( imageSource->getDataType() == ImageIo::UINT8 )
				//	mInternalFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE8_ALPHA8 : GL_LUMINANCE8;
				//else if( imageSource->getDataType() == ImageIo::UINT16 )
				//	mInternalFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE16_ALPHA16 : GL_LUMINANCE16;
				//else if( imageSource->getDataType() == ImageIo::FLOAT32 && supportsTextureFloat )
				//	mInternalFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE_ALPHA32F_ARB : GL_LUMINANCE32F_ARB;
				//else
				//	mInternalFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE_ALPHA : GL_LUMINANCE;
			break;
#else
			case ImageIo::CM_RGB:
				mInternalFormat = ( imageSource->hasAlpha() ) ? DXGI_FORMAT_R8G8B8A8_UNORM : throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
			break;
			case ImageIo::CM_GRAY:
				throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
				//mInternalFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE_ALPHA : GL_LUMINANCE;
			break;
			
#endif
			default:
				throw ImageIoExceptionIllegalColorModel();
			break;
		}
	}
	else {
		mInternalFormat = format.mInternalFormat;
	}

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
			throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
			//dataFormat = ( imageSource->hasAlpha() ) ? GL_LUMINANCE_ALPHA : GL_LUMINANCE;
			//channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::YA : ImageIo::Y;
			//isGray = true;
		break;
		default: // if this is some other color space, we'll have to punt and go w/ RGB
			dataFormat = ( imageSource->hasAlpha() ) ? GL_RGBA : throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
			channelOrder = ( imageSource->hasAlpha() ) ? ImageIo::RGBA : ImageIo::RGB;
		break;
	}

	//if( mTarget == GL_TEXTURE_2D ) {
		mMaxU = mMaxV = 1.0f;
	//}
	//else {
	//	mMaxU = (float)mWidth;
	//	mMaxV = (float)mHeight;
	//}
		
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
	if(hr != S_OK)
		__debugbreak();

	D3D11_SUBRESOURCE_DATA subData;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	if( imageSource->getDataType() == ImageIo::UINT8 ) {
		shared_ptr<ImageTargetGLTexture<uint8_t> > target = ImageTargetGLTexture<uint8_t>::createRef( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		subData.pSysMem = target->getData();
		subData.SysMemPitch = 4 * mWidth;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		subData.SysMemSlicePitch = 0;//subData.SysMemPitch * mHeight;
		getDxRenderer()->md3dDevice->CreateTexture2D(&desc, &subData, &mDxTexture);
	}
	else if( imageSource->getDataType() == ImageIo::UINT16 ) {
		shared_ptr<ImageTargetGLTexture<uint16_t> > target = ImageTargetGLTexture<uint16_t>::createRef( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		subData.pSysMem = target->getData();
		subData.SysMemPitch = 4 * 2 * mWidth;
		desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
		subData.SysMemSlicePitch = subData.SysMemPitch * mHeight;
		getDxRenderer()->md3dDevice->CreateTexture2D(&desc, &subData, &mDxTexture);
	}
	else {
		shared_ptr<ImageTargetGLTexture<float> > target = ImageTargetGLTexture<float>::createRef( this, channelOrder, isGray, imageSource->hasAlpha() );
		imageSource->load( target );
		subData.pSysMem = target->getData();
		if(dataFormat == GL_RGBA)
		{
			desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			subData.SysMemPitch = 4 * 4 * mWidth;
		}
		else
		{
			desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			subData.SysMemPitch = 3 * 4 * mWidth;
		}
		subData.SysMemSlicePitch = subData.SysMemPitch * mHeight;
		getDxRenderer()->md3dDevice->CreateTexture2D(&desc, &subData, &mDxTexture);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = -1;
	getDxRenderer()->md3dDevice->CreateShaderResourceView(mDxTexture, &SRVDesc, &mSRV);
}

void Texture::update( const Surface &surface )
{
	GLint dataFormat;
	GLenum type;
	SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );
	if( ( surface.getWidth() != getWidth() ) || ( surface.getHeight() != getHeight() ) )
		throw TextureDataExc( "Invalid Texture::update() surface dimensions" );
	auto dx = getDxRenderer();
	HRESULT hr;
	mDxTexture->Release();
	mSRV->Release();
	D3D11_SUBRESOURCE_DATA subData;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	switch(dataFormat)
	{
		case GL_RGBA:
			switch(type)
			{
				case GL_UNSIGNED_BYTE:
					desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					subData.SysMemPitch = 4 * mWidth;
					break;

				case GL_FLOAT:
					desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					subData.SysMemPitch = 4 * 4 * mWidth;
					break;
			}
			break;

		case GL_RGB:
			switch(type)
			{
				case GL_UNSIGNED_BYTE:
					desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					subData.SysMemPitch = 4 * mWidth;
					break;

				case GL_FLOAT:
					desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					subData.SysMemPitch = 3 * 4 * mWidth;
					break;
			}
			break;

		case GL_LUMINANCE:
			switch(type)
			{
				case GL_UNSIGNED_BYTE:
					desc.Format = DXGI_FORMAT_R8_UNORM;
					subData.SysMemPitch = mWidth;
					break;

				case GL_FLOAT:
					desc.Format = DXGI_FORMAT_R32_FLOAT;
					subData.SysMemPitch = 4 * mWidth;
					break;
			}
			break;
	}
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	subData.pSysMem = surface.getData();
	subData.SysMemSlicePitch = subData.SysMemPitch * mHeight;
	if(type != GL_UNSIGNED_BYTE || dataFormat != GL_RGB)
		hr = dx->md3dDevice->CreateTexture2D(&desc, &subData, &mDxTexture);
	else
	{
		hr = dx->md3dDevice->CreateTexture2D(&desc, 0, &mDxTexture);
		for(int y = 0; y < mHeight; ++y)
			for(int x = 0; x < mWidth; ++x)
			{
				unsigned char color[4] = {255, 255, 255, 255};
				for(int i = 0; i < 3; ++i)
					color[i] = surface.getData()[y * 3 * mWidth + x * 3 + i];
				D3D11_BOX box;
				box.left = x;
				box.right = x + 1;
				box.top = y;
				box.bottom = y + 1;
				box.front = 0;
				box.back = 1;
				dx->mDeviceContext->UpdateSubresource(mDxTexture, 0, &box, color, 4 * mWidth, 0);
			}
	}
	if(hr != S_OK)
		__debugbreak();

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = -1;
	dx->md3dDevice->CreateShaderResourceView(mDxTexture, &SRVDesc, &mSRV);
}

void Texture::update( const Surface32f &surface )
{
	GLint dataFormat;
	GLenum type;
	SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );
	if( ( surface.getWidth() != getWidth() ) || ( surface.getHeight() != getHeight() ) )
		throw TextureDataExc( "Invalid Texture::update() surface dimensions" );

	mDxTexture->Release();
	mSRV->Release();
	D3D11_SUBRESOURCE_DATA subData;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	switch(dataFormat)
	{
		case GL_RGBA:
			desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			subData.SysMemPitch = 4 * 4 * mWidth;
			break;

		case GL_RGB:
			desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			subData.SysMemPitch = 3 * 4 * mWidth;
			break;

		case GL_LUMINANCE:
			desc.Format = DXGI_FORMAT_R32_FLOAT;
			subData.SysMemPitch = 4 * mWidth;
			break;
	}
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	subData.pSysMem = surface.getData();
	subData.SysMemSlicePitch = subData.SysMemPitch * mHeight;
	getDxRenderer()->md3dDevice->CreateTexture2D(&desc, &subData, &mDxTexture);

}

void Texture::update( const Surface &surface, const Area &area )
{
	GLint dataFormat;
	GLenum type;
	SurfaceChannelOrderToDataFormatAndType( surface.getChannelOrder(), &dataFormat, &type );
	auto dx = getDxRenderer();
	if(type == GL_FLOAT)
	{
		D3D11_BOX box;
		box.left = area.getX1();
		box.right = area.getX2();
		box.top = area.getY1();
		box.bottom = area.getY2();
		box.front = 0;
		box.back = 1;
		if(dataFormat == GL_RGB)
			dx->mDeviceContext->UpdateSubresource(mDxTexture, 0, &box, surface.getData( area.getUL() ), 12 * mWidth, 0);
		else if(dataFormat == GL_RGBA)
			dx->mDeviceContext->UpdateSubresource(mDxTexture, 0, &box, surface.getData( area.getUL() ), 16 * mWidth, 0);
		else
			dx->mDeviceContext->UpdateSubresource(mDxTexture, 0, &box, surface.getData( area.getUL() ), 4 * mWidth, 0);
	}
	else
	{
		int width = 1;
		if(dataFormat == GL_RGB)
			width = 3;
		else if(dataFormat == GL_RGBA)
			width = 4;
		if(type == GL_UNSIGNED_SHORT)
			width *= 2;
		for(int y = 0; y < mHeight; ++y)
			for(int x = 0; x < mWidth; ++x)
			{
				unsigned char color[4] = {0, 0, 0, 255};
				for(int i = 0; i < width; ++i)
					color[i] = surface.getData()[y * width * mWidth + x * width + i];
				D3D11_BOX box;
				box.left = x;
				box.right = x + 1;
				box.top = y;
				box.bottom = y + 1;
				box.front = 0;
				box.back = 1;
				dx->mDeviceContext->UpdateSubresource(mDxTexture, 0, &box, color, 4 * mWidth, 0);
			}
	}
}

void Texture::update( const Channel32f &channel )
{
	if( ( channel.getWidth() != getWidth() ) || ( channel.getHeight() != getHeight() ) )
		throw TextureDataExc( "Invalid Texture::update() channel dimensions" );

	D3D11_BOX box;
	box.left = 0;
	box.right = getWidth();
	box.top = 0;
	box.bottom = getHeight();
	box.front = 0;
	box.back = 1;
	getDxRenderer()->mDeviceContext->UpdateSubresource(mDxTexture, 0, &box, channel.getData(), getWidth() * 4, 0);
}

void Texture::update( const Channel8u &channel, const Area &area )
{
	D3D11_BOX box;
	box.left = area.getX1();
	box.right = area.getX2();
	box.top = area.getY1();
	box.bottom = area.getY2();
	box.front = 0;
	box.back = 1;
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
		getDxRenderer()->mDeviceContext->UpdateSubresource(mDxTexture, 0, &box, data.get(), area.getWidth(), 0);
	}
	else
		getDxRenderer()->mDeviceContext->UpdateSubresource(mDxTexture, 0, &box, channel.getData( area.getUL() ), area.getWidth(), 0);
}

void Texture::SurfaceChannelOrderToDataFormatAndType( const SurfaceChannelOrder &sco, GLint *dataFormat, GLenum *type )
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
			*dataFormat = GL_BGRA;
			*type = GL_UNSIGNED_BYTE;
		break;
#if ! defined( CINDER_GLES )
		case SurfaceChannelOrder::BGR:
			*dataFormat = GL_BGR;
			*type = GL_UNSIGNED_BYTE;
		break;
		case SurfaceChannelOrder::ARGB:
			*dataFormat = GL_BGRA;
			*type = GL_UNSIGNED_INT_8_8_8_8;
		break;
#endif // ! defined( CINDER_GLES )
		default:
			throw TextureDataExc( "Invalid channel order" ); // this is an unsupported channel order for a texture
		break;
	}
}

bool Texture::dataFormatHasAlpha( GLint dataFormat )
{
	switch( dataFormat ) {
		case GL_RGBA:
		case GL_ALPHA:
		case GL_LUMINANCE_ALPHA:
#if ! defined( CINDER_GLES )
		case GL_BGRA:
#endif
			return true;
		break;
		default:
			return false;
	}
}

bool Texture::dataFormatHasColor( GLint dataFormat )
{
	switch( dataFormat ) {
		case GL_ALPHA:
		case GL_LUMINANCE:
		case GL_LUMINANCE_ALPHA:
			return false;
		break;
	}
	
	return true;
}

TextureRef Texture::loadDds( IStreamRef ddsStream, Format format )
{
	TextureRef texture( new Texture() );
	app::AppImplMswRendererDx *dxRenderer = reinterpret_cast<app::AppImplMswRendererDx*>(reinterpret_cast<app::RendererDx*>(&*app::App::get()->getRenderer())->mImpl);
	uint8_t *data = (uint8_t*)malloc(ddsStream->size());
	if(!data)
		throw TextureDataExc("Not enough memory to load DDS");
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
	switch( mInternalFormat ) {
#if ! defined( CINDER_GLES )
		case GL_RGBA8:
		case GL_RGBA16:
		case GL_RGBA32F_ARB:
		case GL_LUMINANCE8_ALPHA8:
		case GL_LUMINANCE16_ALPHA16:
		case GL_LUMINANCE_ALPHA32F_ARB:
#endif
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

DXGI_FORMAT Texture::getInternalFormat() const
{
	D3D11_TEXTURE2D_DESC desc;
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
		throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");//mPixelInc = ( mHasAlpha ) ? 2 : 1;
	else
		mPixelInc = ( mHasAlpha ) ? 4 : /*3*/throw TextureDataExc("Non-float textures need to have all four color channels (RGBA) defined");
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
