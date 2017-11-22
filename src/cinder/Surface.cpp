/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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

#include "cinder/Surface.h"

#if defined( CINDER_UWP )
	#include <ppltasks.h>
	#include "cinder/winrt/WinRTUtils.h"
	#include "cinder/Utilities.h"
	#include "cinder/msw/CinderMsw.h"
	using namespace Windows::Storage;
	using namespace Concurrency;
	#undef max
	using namespace Windows::Storage;
	using namespace Concurrency;
#endif

#include "cinder/ChanTraits.h"
#include "cinder/ImageIo.h"
#include "cinder/ip/Fill.h"

#include <type_traits>

namespace cinder {

template<typename T>
class ImageTargetSurface : public ImageTarget {
  public:
	static std::shared_ptr<ImageTargetSurface<T> > createRef( SurfaceT<T> *surface ) { return std::shared_ptr<ImageTargetSurface<T> >( new ImageTargetSurface<T>( surface ) ); }

	virtual bool hasAlpha() const;
	
	virtual void*	getRowPointer( int32_t row );
	
  protected:
	ImageTargetSurface( SurfaceT<T> *surface );
	
	SurfaceT<T>		*mSurface;
};

class ImageSourceSurface : public ImageSource {
  public:
	template<typename T>
	ImageSourceSurface( const SurfaceT<T> &surface )
		: ImageSource()
	{
		mWidth = surface.getWidth();
		mHeight = surface.getHeight();
		setColorModel( ImageIo::CM_RGB );
		setChannelOrder( ImageIo::ChannelOrder( surface.getChannelOrder().getImageIoChannelOrder() ) );
		if( std::is_same<T,uint8_t>::value ) {
			setDataType( ImageIo::UINT8 );
		}
		else if( std::is_same<T,uint16_t>::value ) {
			setDataType( ImageIo::UINT16 );
		}
		else if( std::is_same<T,float>::value ) {
			setDataType( ImageIo::FLOAT32 );
		}
		else
			throw; // this surface seems to be a type we've never met
		mRowBytes = surface.getRowBytes();
		mData = reinterpret_cast<const uint8_t*>( surface.getData() );
		mDataStoreRef = surface.getDataStore();
	}

	void load( ImageTargetRef target ) {
		// get a pointer to the ImageSource function appropriate for handling our data configuration
		ImageSource::RowFunc func = setupRowFunc( target );

		const uint8_t *data = mData;
		for( int32_t row = 0; row < mHeight; ++row ) {
			((*this).*func)( target, row, data );
			data += mRowBytes;
		}
	}

	// not ideal, but these are used to register a reference to the surface we were constructed with
	const uint8_t			*mData;
	ptrdiff_t				mRowBytes;
	std::shared_ptr<void>	mDataStoreRef;
};

SurfaceChannelOrder::SurfaceChannelOrder( int aCode )
	: mCode( aCode )
{
	switch( mCode ) {
		case RGBA:
			set( 0, 1, 2, 3, 4 );
		break;
		case BGRA:
			set( 2, 1, 0, 3, 4 );
		break;
		case ARGB:
			set( 1, 2, 3, 0, 4 );
		break;
		case ABGR:
			set( 3, 2, 1, 0, 4 );
		break;
		case RGBX:
			set( 0, 1, 2, INVALID, 4 );
		break;
		case BGRX:
			set( 2, 1, 0, INVALID, 4 );
		break;
		case XRGB:
			set( 1, 2, 3, INVALID, 4 );
		break;
		case XBGR:
			set( 3, 2, 1, INVALID, 4 );
		break;
		case RGB:
			set( 0, 1, 2, INVALID, 3 );
		break;
		case BGR:
			set( 2, 1, 0, INVALID, 3 );
		break;
		case UNSPECIFIED:
			set( INVALID, INVALID, INVALID, INVALID, INVALID );
		break;
		default:
			mCode = UNSPECIFIED;
			set( INVALID, INVALID, INVALID, INVALID, INVALID );
		break;
	}
}

SurfaceChannelOrder::SurfaceChannelOrder( const SurfaceChannelOrder &aOrder )
	: mRed( aOrder.mRed ), mGreen( aOrder.mGreen ), mBlue( aOrder.mBlue ), mAlpha( aOrder.mAlpha ), mPixelInc( aOrder.mPixelInc ), mCode( aOrder.mCode )
{
}

void SurfaceChannelOrder::set( uint8_t aRed, uint8_t aGreen, uint8_t aBlue, uint8_t aAlpha, uint8_t aPixelInc )
{
	mRed = aRed;
	mGreen = aGreen;
	mBlue = aBlue;
	mAlpha = aAlpha;
	mPixelInc = aPixelInc;
}

//! Translates a SurfaceChannelOrder into an ImageIo::ChannelOrder
int	SurfaceChannelOrder::getImageIoChannelOrder() const
{
	switch( mCode ) {
		case RGBA: return ImageIo::RGBA;
		case BGRA: return ImageIo::BGRA;
		case ARGB: return ImageIo::ARGB;
		case ABGR: return ImageIo::ABGR;
		case RGBX: return ImageIo::RGBX;
		case BGRX: return ImageIo::BGRX;
		case XRGB: return ImageIo::XRGB;
		case XBGR: return ImageIo::XBGR;
		case RGB: return ImageIo::RGB;
		case BGR: return ImageIo::BGR;
		case UNSPECIFIED:
		default:
			return ImageIo::CUSTOM;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceT
template<typename T>
void SurfaceT<T>::initChannels()
{
	mChannels[SurfaceChannelOrder::CHAN_RED] = ChannelT<T>( mWidth, mHeight, mRowBytes, mChannelOrder.getPixelInc(), mData + mChannelOrder.getRedOffset(), mDataStore );
	mChannels[SurfaceChannelOrder::CHAN_GREEN] = ChannelT<T>( mWidth, mHeight, mRowBytes, mChannelOrder.getPixelInc(), mData + mChannelOrder.getGreenOffset(), mDataStore );
	mChannels[SurfaceChannelOrder::CHAN_BLUE] = ChannelT<T>( mWidth, mHeight, mRowBytes, mChannelOrder.getPixelInc(), mData + mChannelOrder.getBlueOffset(), mDataStore );
	if( mChannelOrder.hasAlpha() )
		mChannels[SurfaceChannelOrder::CHAN_ALPHA] = ChannelT<T>( mWidth, mHeight, mRowBytes, mChannelOrder.getPixelInc(), mData + mChannelOrder.getAlphaOffset(), mDataStore );
}

template<typename T>
void SurfaceT<T>::setChannelOrder( const SurfaceChannelOrder &aChannelOrder )
{
	mChannelOrder = aChannelOrder;
	initChannels();
}

template<typename T>
SurfaceT<T>::SurfaceT()
	: mWidth( 0 ), mHeight( 0 ), mChannelOrder( SurfaceChannelOrder::UNSPECIFIED ), mRowBytes( 0 ), mData( nullptr ), mPremultiplied( false )
{
}

template<typename T>
SurfaceT<T>::SurfaceT( const SurfaceT<T> &rhs )
	: mWidth( rhs.mWidth ), mHeight( rhs.mHeight ), mChannelOrder( rhs.mChannelOrder ), mRowBytes( rhs.mRowBytes ), mPremultiplied( rhs.mPremultiplied )
{
	mDataStore = std::shared_ptr<T>( new T[mHeight * mRowBytes], std::default_delete<T[]>() );
	mData = mDataStore.get();
	initChannels();
	copyFrom( rhs, Area( 0, 0, mWidth, mHeight ) );
}

template<typename T>
SurfaceT<T>::SurfaceT( SurfaceT<T> &&rhs )
	: mWidth( rhs.mWidth ), mHeight( rhs.mHeight ), mChannelOrder( rhs.mChannelOrder ), mRowBytes( rhs.mRowBytes ), mPremultiplied( rhs.mPremultiplied )
{
	mDataStore = rhs.mDataStore;
	rhs.mDataStore = nullptr;
	mData = mDataStore.get();
	initChannels();
}

template<typename T>
SurfaceT<T>::SurfaceT( int32_t width, int32_t height, bool alpha, SurfaceChannelOrder channelOrder )
	: mWidth( width ), mHeight( height ), mChannelOrder( channelOrder )
{
	if( mChannelOrder == SurfaceChannelOrder::UNSPECIFIED )
		mChannelOrder = ( alpha ) ? SurfaceChannelOrder::RGBA : SurfaceChannelOrder::RGB;
	mPremultiplied = false;
	mRowBytes = width * sizeof(T) * mChannelOrder.getPixelInc();
	mDataStore = std::shared_ptr<T>( new T[height * mRowBytes], std::default_delete<T[]>() );
	mData = mDataStore.get();
	initChannels();
}

template<typename T>
SurfaceT<T>::SurfaceT( int32_t width, int32_t height, bool alpha, const SurfaceConstraints &constraints )
	: mWidth( width ), mHeight( height )
{
	mChannelOrder = constraints.getChannelOrder( alpha );
	mPremultiplied = false;
	mRowBytes = constraints.getRowBytes( width, mChannelOrder, sizeof(T) );
	mDataStore = std::shared_ptr<T>( new T[height * mRowBytes], std::default_delete<T[]>() );
	mData = mDataStore.get();
	initChannels();
}

template<typename T>
SurfaceT<T>::SurfaceT( T *data, int32_t width, int32_t height, ptrdiff_t rowBytes, SurfaceChannelOrder channelOrder )
	: mData( data ), mWidth( width ), mHeight( height ), mRowBytes( rowBytes ), mChannelOrder( channelOrder )
{
	mPremultiplied = false;
	initChannels();
}

template<typename T>
SurfaceT<T>::SurfaceT( ImageSourceRef imageSource, const SurfaceConstraints &constraints )
{
	init( imageSource, constraints, imageSource->hasAlpha() );
}

template<typename T>
SurfaceT<T>::SurfaceT( ImageSourceRef imageSource, const SurfaceConstraints &constraints, bool alpha )
{
	init( imageSource, constraints, alpha );
}

#if defined( CINDER_UWP )
template<typename T>
void SurfaceT<T>::loadImageAsync(const fs::path path, SurfaceT &surface, const SurfaceConstraints &constraints )
{
	loadImageAsync( path, surface, constraints, surface.hasAlpha() );
}

template<typename T>
void SurfaceT<T>::loadImageAsync(const fs::path path, SurfaceT &surface, const SurfaceConstraints &constraints, bool alpha )
{
	auto loadImageTask = create_task([path]() -> ImageSourceRef
	{
		return loadImage(path);
	});

	  // Task-based continuation.
    auto c2 = loadImageTask.then([path, &surface,constraints,alpha](task<ImageSourceRef> previousTask)
    {
        // We do expect to get here because task-based continuations 
        // are scheduled even when the antecedent task throws. 
        try
        {
			surface = SurfaceT(previousTask.get(),constraints, alpha);
        }
        catch (const ImageIoExceptionFailedLoad&)
        {
			auto copyTask = winrt::copyFileToTempDirAsync(path);
			copyTask.then([path, &surface,constraints,alpha](StorageFile^ file) 
			{
				fs::path temp = fs::path( msw::toUtf8String( file->Path->Data() ) );
				surface = SurfaceT(loadImage(fs::path(temp)),constraints, alpha);
				winrt::deleteFileAsync(temp);
			});
        }
    });
}
#endif

template<typename T>
SurfaceT<T>& SurfaceT<T>::operator=( const SurfaceT<T> &rhs )
{
	mWidth = rhs.mWidth;
	mHeight = rhs.mHeight;
	mChannelOrder = rhs.mChannelOrder;
	mRowBytes = rhs.mRowBytes;
	mPremultiplied = rhs.mPremultiplied;
	mDataStore = std::shared_ptr<T>( new T[mHeight * mRowBytes], std::default_delete<T[]>() );
	
	mData = mDataStore.get();
	initChannels();
	
	copyFrom( rhs, Area( 0, 0, mWidth, mHeight ) );
	
	return *this;
}

template<typename T>
SurfaceT<T>& SurfaceT<T>::operator=( SurfaceT<T> &&rhs )
{
	mWidth = rhs.mWidth;
	mHeight = rhs.mHeight;
	mChannelOrder = rhs.mChannelOrder;
	mRowBytes = rhs.mRowBytes;
	mPremultiplied = rhs.mPremultiplied;
	mDataStore = rhs.mDataStore;	
	mData = rhs.mData;
	rhs.mDataStore = nullptr;
	rhs.mData = nullptr;
	
	initChannels();
	
	return *this;
}

template<typename T>
SurfaceT<T>::operator ImageSourceRef() const
{
	return std::shared_ptr<ImageSource>( new ImageSourceSurface( *this ) );
}

template<typename T>
SurfaceT<T>::operator ImageTargetRef()
{
	return ImageTargetSurface<T>::createRef( this );
}

template<typename T>
SurfaceT<T> SurfaceT<T>::clone( bool copyPixels ) const
{
	SurfaceT result( getWidth(), getHeight(), hasAlpha(), getChannelOrder() );
	if( copyPixels )
		result.copyFrom( *this, getBounds() );
	
	return result;
}

template<typename T>
SurfaceT<T> SurfaceT<T>::clone( const Area &area, bool copyPixels ) const
{
	SurfaceT result( area.getWidth(), area.getHeight(), hasAlpha(), getChannelOrder() );
	if( copyPixels )
		result.copyFrom( *this, area, -area.getUL() );
	
	return result;
}

template<typename T>
void SurfaceT<T>::init( ImageSourceRef imageSource, const SurfaceConstraints &constraints, bool alpha )
{
	mWidth = imageSource->getWidth();
	mHeight = imageSource->getHeight();

	mChannelOrder = constraints.getChannelOrder( alpha );
	mRowBytes = constraints.getRowBytes( mWidth, mChannelOrder, sizeof(T) );
	
	mDataStore = std::shared_ptr<T>( new T[mHeight * mRowBytes], std::default_delete<T[]>() );
	mData = mDataStore.get();

	mPremultiplied = imageSource->isPremultiplied();
	
	std::shared_ptr<ImageTargetSurface<T> > target = ImageTargetSurface<T>::createRef( this );
	imageSource->load( target );
	
	initChannels();
	// if the image doesn't have alpha but we do, set the alpha to 1.0
	if( alpha && ( ! imageSource->hasAlpha() ) )
		ip::fill( &getChannelAlpha(), CHANTRAIT<T>::max() );
}

template<typename T>
void SurfaceT<T>::copyFrom( const SurfaceT<T> &srcSurface, const Area &srcArea, const ivec2 &relativeOffset )
{
	std::pair<Area,ivec2> srcDst = clippedSrcDst( srcSurface.getBounds(), srcArea, getBounds(), srcArea.getUL() + relativeOffset );
	
	if( getChannelOrder() == srcSurface.getChannelOrder() )
		copyRawSameChannelOrder( srcSurface, srcDst.first, srcDst.second );
	else if( hasAlpha() && srcSurface.hasAlpha() )
		copyRawRgba( srcSurface, srcDst.first, srcDst.second );
	else if( hasAlpha() && ( ! srcSurface.hasAlpha() ) )
		copyRawRgbFullAlpha( srcSurface, srcDst.first, srcDst.second );
	else
		copyRawRgb( srcSurface, srcDst.first, srcDst.second );
}

template<typename T>
void SurfaceT<T>::copyRawSameChannelOrder( const SurfaceT<T> &srcSurface, const Area &srcArea, const ivec2 &absoluteOffset )
{
	ptrdiff_t srcRowBytes = srcSurface.getRowBytes();
	uint8_t srcPixelInc = srcSurface.getPixelInc();
	uint8_t dstPixelInc = getPixelInc();
	size_t copyBytes = srcArea.getWidth() * srcPixelInc * sizeof(T);
	for( int32_t y = 0; y < srcArea.getHeight(); ++y ) {
		const T *srcPtr = reinterpret_cast<const T*>( reinterpret_cast<const uint8_t*>( srcSurface.getData() + srcArea.x1 * srcPixelInc ) + ( srcArea.y1 + y ) * srcRowBytes );
		T *dstPtr = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( getData() + absoluteOffset.x * dstPixelInc ) + ( y + absoluteOffset.y ) * getRowBytes() );
		memcpy( dstPtr, srcPtr, copyBytes );
	}
}

template<typename T>
void SurfaceT<T>::copyRawRgba( const SurfaceT<T> &srcSurface, const Area &srcArea, const ivec2 &absoluteOffset )
{
	const ptrdiff_t srcRowBytes = srcSurface.getRowBytes();
	uint8_t srcRed = srcSurface.getChannelOrder().getRedOffset();
	uint8_t srcGreen = srcSurface.getChannelOrder().getGreenOffset();
	uint8_t srcBlue = srcSurface.getChannelOrder().getBlueOffset();
	uint8_t srcAlpha = srcSurface.getChannelOrder().getAlphaOffset();

	uint8_t dstRed = getChannelOrder().getRedOffset();
	uint8_t dstGreen = getChannelOrder().getGreenOffset();
	uint8_t dstBlue = getChannelOrder().getBlueOffset();
	uint8_t dstAlpha = getChannelOrder().getAlphaOffset();

	int32_t width = srcArea.getWidth();

	for( int32_t y = 0; y < srcArea.getHeight(); ++y ) {
		const T *src = reinterpret_cast<const T*>( reinterpret_cast<const uint8_t*>( srcSurface.getData() + srcArea.x1 * 4 ) + ( srcArea.y1 + y ) * srcRowBytes );
		T *dst = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( getData() + absoluteOffset.x * 4 ) + ( y + absoluteOffset.y ) * getRowBytes() );
		for( int x = 0; x < width; ++x ) {
			dst[dstRed] = src[srcRed];
			dst[dstGreen] = src[srcGreen];
			dst[dstBlue] = src[srcBlue];
			dst[dstAlpha] = src[srcAlpha];
			src += 4;
			dst += 4;
		}
	}
}

template<typename T>
void SurfaceT<T>::copyRawRgbFullAlpha( const SurfaceT<T> &srcSurface, const Area &srcArea, const ivec2 &absoluteOffset )
{
	const ptrdiff_t srcRowBytes = srcSurface.getRowBytes();
	const uint8_t srcPixelInc = srcSurface.getPixelInc();
	uint8_t srcRed = srcSurface.getChannelOrder().getRedOffset();
	uint8_t srcGreen = srcSurface.getChannelOrder().getGreenOffset();
	uint8_t srcBlue = srcSurface.getChannelOrder().getBlueOffset();
	const T fullAlpha = CHANTRAIT<T>::max();

	uint8_t dstRed = getChannelOrder().getRedOffset();
	uint8_t dstGreen = getChannelOrder().getGreenOffset();
	uint8_t dstBlue = getChannelOrder().getBlueOffset();
	uint8_t dstAlpha = getChannelOrder().getAlphaOffset();

	int32_t width = srcArea.getWidth();

	for( int32_t y = 0; y < srcArea.getHeight(); ++y ) {
		const T *src = reinterpret_cast<const T*>( reinterpret_cast<const uint8_t*>( srcSurface.getData() + srcArea.x1 * 4 ) + ( srcArea.y1 + y ) * srcRowBytes );
		T *dst = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( getData() + absoluteOffset.x * 4 ) + ( y + absoluteOffset.y ) * getRowBytes() );
		for( int x = 0; x < width; ++x ) {
			dst[dstRed] = src[srcRed];
			dst[dstGreen] = src[srcGreen];
			dst[dstBlue] = src[srcBlue];
			dst[dstAlpha] = fullAlpha;
			src += srcPixelInc;
			dst += 4;
		}
	}
}

template<typename T>
void SurfaceT<T>::copyRawRgb( const SurfaceT<T> &srcSurface, const Area &srcArea, const ivec2 &absoluteOffset )
{
	const ptrdiff_t srcRowBytes = srcSurface.getRowBytes();
	const uint8_t srcPixelInc = srcSurface.getPixelInc();
	const uint8_t srcRed = srcSurface.getChannelOrder().getRedOffset();
	const uint8_t srcGreen = srcSurface.getChannelOrder().getGreenOffset();
	const uint8_t srcBlue = srcSurface.getChannelOrder().getBlueOffset();

	const uint8_t dstPixelInc = getPixelInc();
	const uint8_t dstRed = getChannelOrder().getRedOffset();
	const uint8_t dstGreen = getChannelOrder().getGreenOffset();
	const uint8_t dstBlue = getChannelOrder().getBlueOffset();

	int32_t width = srcArea.getWidth();

	for( int32_t y = 0; y < srcArea.getHeight(); ++y ) {
		const T *src = reinterpret_cast<const T*>( reinterpret_cast<const uint8_t*>( srcSurface.getData() + srcArea.x1 * srcPixelInc ) + ( srcArea.y1 + y ) * srcRowBytes );
		T *dst = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( getData() + absoluteOffset.x * dstPixelInc ) + ( y + absoluteOffset.y ) * getRowBytes() );
		for( int x = 0; x < width; ++x ) {
			dst[dstRed] = src[srcRed];
			dst[dstGreen] = src[srcGreen];
			dst[dstBlue] = src[srcBlue];
			src += srcPixelInc;
			dst += dstPixelInc;
		}
	}
}

template<typename T>
ColorT<T> SurfaceT<T>::areaAverage( const Area &area ) const
{
	typename CHANTRAIT<T>::Sum redSum = 0, greenSum = 0, blueSum = 0;
	const Area clipped( area.getClipBy( getBounds() ) );
	
	if( ( clipped.getWidth() <= 0 ) || ( clipped.getHeight() <= 0 ) )
		return ColorT<T>( 0, 0, 0 );

	uint8_t red = getRedOffset(), green = getGreenOffset(), blue = getBlueOffset();
	uint8_t inc = getPixelInc();
	const T *line = getData( clipped.getUL() );
	for( int32_t y = clipped.y1; y < clipped.y2; ++y ) {
		const T *d = line;
		for( int32_t x = clipped.x1; x < clipped.x2; ++x ) {
			redSum += d[red];
			greenSum += d[green];
			blueSum += d[blue];						
			d += inc;
		}
		
		line = reinterpret_cast<const T*>( reinterpret_cast<const uint8_t*>( line ) + getRowBytes() );
	} 
	
	return ColorT<T>( (T)(redSum / ( clipped.getWidth() * clipped.getHeight() )), (T)(greenSum / ( clipped.getWidth() * clipped.getHeight() )), (T)(blueSum / ( clipped.getWidth() * clipped.getHeight() )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ImageTargetSurface
template<typename T>
ImageTargetSurface<T>::ImageTargetSurface( SurfaceT<T> *aSurface )
	: ImageTarget(), mSurface( aSurface )
{
	if( std::is_same<T,float>::value )
		setDataType( ImageIo::FLOAT32 );
	else if( std::is_same<T,uint16_t>::value )
		setDataType( ImageIo::UINT16 );
	else if( std::is_same<T,uint8_t>::value )
		setDataType( ImageIo::UINT8 );
	else 
		throw; // what is this?

	setColorModel( ImageIo::CM_RGB );
	
	// set the target's ChannelT order based on the SurfaceT's
	switch ( mSurface->getChannelOrder().getCode() ) {
		case SurfaceChannelOrder::RGBA:
			setChannelOrder( ImageIo::RGBA );
		break;
		case SurfaceChannelOrder::BGRA:
			setChannelOrder( ImageIo::BGRA );
		break;
		case SurfaceChannelOrder::ARGB:
			setChannelOrder( ImageIo::ARGB );
		break;
		case SurfaceChannelOrder::ABGR:
			setChannelOrder( ImageIo::ABGR );
		break;
		case SurfaceChannelOrder::RGBX:
			setChannelOrder( ImageIo::RGBX );
		break;
		case SurfaceChannelOrder::BGRX:
			setChannelOrder( ImageIo::BGRX );
		break;
		case SurfaceChannelOrder::XRGB:
			setChannelOrder( ImageIo::XRGB );
		break;
		case SurfaceChannelOrder::XBGR:
			setChannelOrder( ImageIo::XBGR );
		break;
		case SurfaceChannelOrder::RGB:
			setChannelOrder( ImageIo::RGB );
		break;
		case SurfaceChannelOrder::BGR:
			setChannelOrder( ImageIo::BGR );
		break;
		default:
			setChannelOrder( ImageIo::CUSTOM ); // in case we don't find a match
		break;
	}
}

template<typename T>
bool ImageTargetSurface<T>::hasAlpha() const
{
	return mSurface->hasAlpha();
}

template<typename T>
void* ImageTargetSurface<T>::getRowPointer( int32_t row )
{
	return reinterpret_cast<void*>( mSurface->getData( ivec2( 0, row ) ) );
}

template class CI_API SurfaceT<uint8_t>;
template class CI_API SurfaceT<uint16_t>;
template class CI_API SurfaceT<float>;

} // namespace cinder
