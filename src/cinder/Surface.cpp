/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

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
#include "cinder/ImageIo.h"
#include "cinder/ip/Fill.h"

#include <boost/type_traits/is_same.hpp>
using boost::tribool;

namespace cinder {

template<typename T>
class ImageTargetSurface : public ImageTarget {
  public:
	static shared_ptr<ImageTargetSurface<T> > createRef( SurfaceT<T> *surface ) { return shared_ptr<ImageTargetSurface<T> >( new ImageTargetSurface<T>( surface ) ); }

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
		if( boost::is_same<T,uint8_t>::value )
			setDataType( ImageIo::UINT8 );
		else if( boost::is_same<T,float>::value )
			setDataType( ImageIo::FLOAT32 );
		else
			throw; // this surface seems to be a type we've never met
		mRowBytes = surface.getRowBytes();
		mData = reinterpret_cast<const uint8_t*>( surface.getData() );
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
	
	const uint8_t		*mData;
	int32_t				mRowBytes;
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
// SurfaceT::Obj
template<typename T>
SurfaceT<T>::Obj::Obj( int32_t aWidth, int32_t aHeight, SurfaceChannelOrder aChannelOrder, T *aData, bool aOwnsData, int32_t aRowBytes )
	: mWidth( aWidth ), mHeight( aHeight ), mChannelOrder( aChannelOrder ), mData( aData ), mOwnsData( aOwnsData ), mRowBytes( aRowBytes ), mIsPremultiplied( false )
{
	mDeallocatorFunc = NULL;
	initChannels();
}

template<typename T>
SurfaceT<T>::Obj::~Obj()
{
	if( mDeallocatorFunc )
		(*mDeallocatorFunc)( mDeallocatorRefcon );

	if( mOwnsData )
		delete [] mData;
}

template<typename T>
void SurfaceT<T>::Obj::initChannels()
{
	mChannels[SurfaceChannelOrder::CHAN_RED] = ChannelT<T>( mWidth, mHeight, mRowBytes, mChannelOrder.getPixelInc(), mData + mChannelOrder.getRedOffset() );
	mChannels[SurfaceChannelOrder::CHAN_GREEN] = ChannelT<T>( mWidth, mHeight, mRowBytes, mChannelOrder.getPixelInc(), mData + mChannelOrder.getGreenOffset() );
	mChannels[SurfaceChannelOrder::CHAN_BLUE] = ChannelT<T>( mWidth, mHeight, mRowBytes, mChannelOrder.getPixelInc(), mData + mChannelOrder.getBlueOffset() );
	if( mChannelOrder.hasAlpha() )
		mChannels[SurfaceChannelOrder::CHAN_ALPHA] = ChannelT<T>( mWidth, mHeight, mRowBytes, mChannelOrder.getPixelInc(), mData + mChannelOrder.getAlphaOffset() );
}

template<typename T>
void SurfaceT<T>::Obj::setData( T *aData, int32_t aWidth, int32_t aHeight, int32_t aRowBytes )
{
	if( mOwnsData )
		delete [] mData;

	mData = aData;
	mOwnsData = false;
	mWidth = aWidth;
	mHeight = aHeight;
	mRowBytes = aRowBytes;
	
	initChannels();
}

template<typename T>
void SurfaceT<T>::Obj::setDeallocator( void(*aDeallocatorFunc)( void * ), void *aDeallocatorRefcon )
{
	mDeallocatorFunc = aDeallocatorFunc;
	mDeallocatorRefcon = aDeallocatorRefcon;
}

template<typename T>
void SurfaceT<T>::Obj::setChannelOrder( const SurfaceChannelOrder &aChannelOrder )
{
	mChannelOrder = aChannelOrder;
	initChannels();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SurfaceT

template<typename T>
SurfaceT<T>::SurfaceT( int32_t aWidth, int32_t aHeight, bool alpha, SurfaceChannelOrder aChannelOrder )
{
	SurfaceChannelOrder channelOrder = aChannelOrder;
	if( channelOrder == SurfaceChannelOrder::UNSPECIFIED )
		channelOrder = ( alpha ) ? SurfaceChannelOrder::RGBA : SurfaceChannelOrder::RGB;
	int32_t rowBytes = aWidth * sizeof(T) * channelOrder.getPixelInc();
	T *data = new T[aHeight * rowBytes];
	mObj = shared_ptr<Obj>( new Obj( aWidth, aHeight, channelOrder, data, true, rowBytes ) );
}

template<typename T>
SurfaceT<T>::SurfaceT( int32_t aWidth, int32_t aHeight, bool alpha, const SurfaceConstraints &constraints )
{
	SurfaceChannelOrder channelOrder = constraints.getChannelOrder( alpha );
	int32_t rowBytes = constraints.getRowBytes( aWidth, channelOrder, sizeof(T) );
	T *data = new T[aHeight * rowBytes];
	mObj = shared_ptr<Obj>( new Obj( aWidth, aHeight, channelOrder, data, true, rowBytes ) );
}

template<typename T>
SurfaceT<T>::SurfaceT( T *aData, int32_t aWidth, int32_t aHeight, int32_t aRowBytes, SurfaceChannelOrder aChannelOrder )
{
	mObj = shared_ptr<Obj>( new Obj( aWidth, aHeight, aChannelOrder, aData, false, aRowBytes ) );
}

template<typename T>
SurfaceT<T>::SurfaceT( ImageSourceRef imageSource, const SurfaceConstraints &constraints, boost::tribool alpha )
{
	init( imageSource, constraints, alpha );
}

template<typename T>
SurfaceT<T>::operator ImageSourceRef() const
{
	return shared_ptr<ImageSource>( new ImageSourceSurface( *this ) );
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
void SurfaceT<T>::setData( T *aData, int32_t aWidth, int32_t aHeight, int32_t aRowBytes )
{
	mObj->setData( aData, aWidth, aHeight, aRowBytes );
}

template<typename T>
void SurfaceT<T>::setDeallocator( void(*aDeallocatorFunc)( void * ), void *aDeallocatorRefcon )
{
	mObj->setDeallocator( aDeallocatorFunc, aDeallocatorRefcon );
}

template<typename T>
void SurfaceT<T>::init( ImageSourceRef imageSource, const SurfaceConstraints &constraints, boost::tribool alpha )
{
	int32_t width = imageSource->getWidth();
	int32_t height = imageSource->getHeight();
	bool hasAlpha;
	if( alpha )
		hasAlpha = true;
	else if( ! alpha )
		hasAlpha = false;
	else
		hasAlpha = imageSource->hasAlpha();

	SurfaceChannelOrder channelOrder = constraints.getChannelOrder( hasAlpha );
	int32_t rowBytes = constraints.getRowBytes( width, channelOrder, sizeof(T) );
	
	T *data = new T[height * rowBytes];

	mObj = shared_ptr<Obj>( new Obj( width, height, channelOrder, data, true, rowBytes ) );
	mObj->mIsPremultiplied = imageSource->isPremultiplied();
	
	shared_ptr<ImageTargetSurface<T> > target = ImageTargetSurface<T>::createRef( this );
	imageSource->load( target );
	
	// if the image doesn't have alpha but we do, set ourselves to be full alpha
	if( hasAlpha && ( ! imageSource->hasAlpha() ) ) {
		ip::fill( getChannelAlpha(), CHANTRAIT<T>::max() );
	}	
}

template<typename T>
void SurfaceT<T>::setChannelOrder( const SurfaceChannelOrder &aChannelOrder )
{
	mObj->setChannelOrder( aChannelOrder );
}

template<typename T>
void SurfaceT<T>::copyFrom( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &relativeOffset )
{
	std::pair<Area,Vec2i> srcDst = clippedSrcDst( srcSurface.getBounds(), srcArea, getBounds(), srcArea.getUL() + relativeOffset );
	
	if( getChannelOrder() == srcSurface.getChannelOrder() )
		copyRawSameChannelOrder( srcSurface, srcDst.first, srcDst.second );
	else if( hasAlpha() && srcSurface.hasAlpha() )
		copyRawRgba( srcSurface, srcDst.first, srcDst.second );
	else
		copyRawRgb( srcSurface, srcDst.first, srcDst.second );
}

template<typename T>
void SurfaceT<T>::copyRawSameChannelOrder( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &absoluteOffset )
{
	int32_t srcRowBytes = srcSurface.getRowBytes();
	int32_t srcPixelInc = srcSurface.getPixelInc();
	int32_t dstPixelInc = getPixelInc();
	size_t copyBytes = srcArea.getWidth() * srcPixelInc * sizeof(T);
	for( int32_t y = 0; y < srcArea.getHeight(); ++y ) {
		const T *srcPtr = reinterpret_cast<const T*>( reinterpret_cast<const uint8_t*>( srcSurface.getData() + srcArea.x1 * srcPixelInc ) + ( srcArea.y1 + y ) * srcRowBytes );
		T *dstPtr = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( getData() + absoluteOffset.x * dstPixelInc ) + ( y + absoluteOffset.y ) * getRowBytes() );
		memcpy( dstPtr, srcPtr, copyBytes );
	}
}

template<typename T>
void SurfaceT<T>::copyRawRgba( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &absoluteOffset )
{
	const int32_t srcRowBytes = srcSurface.getRowBytes();
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
void SurfaceT<T>::copyRawRgb( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &absoluteOffset )
{
	const int32_t srcRowBytes = srcSurface.getRowBytes();
	const int8_t srcPixelInc = srcSurface.getPixelInc();
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
	if( boost::is_same<T,float>::value )
		setDataType( ImageIo::FLOAT32 );
	else if( boost::is_same<T,uint8_t>::value )
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
	return reinterpret_cast<void*>( mSurface->getData( Vec2i( 0, row ) ) );
}

#define SURFACE_PROTOTYPES(r,data,T)\
	template class SurfaceT<T>;

BOOST_PP_SEQ_FOR_EACH( SURFACE_PROTOTYPES, ~, CHANNEL_TYPES )

} // namespace dt