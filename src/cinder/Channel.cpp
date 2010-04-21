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

#include "cinder/Channel.h"
#include "cinder/ChanTraits.h"
#include "cinder/ImageIo.h"

#include <boost/type_traits/is_same.hpp>

namespace cinder {

template<typename T>
class ImageTargetChannel : public ImageTarget {
  public:
	static shared_ptr<ImageTargetChannel<T> > createRef( ChannelT<T> *channel ) { return shared_ptr<ImageTargetChannel<T> >( new ImageTargetChannel<T>( channel ) ); }

	virtual bool hasAlpha() const { return false; }
	
	virtual void*	getRowPointer( int32_t row ) { return reinterpret_cast<void*>( mChannel->getData( Vec2i( 0, row ) ) ); }
	
  protected:
	ImageTargetChannel( ChannelT<T> *channel )
		: mChannel( channel )
	{
		if( boost::is_same<T,float>::value )
			setDataType( ImageIo::FLOAT32 );
		else if( boost::is_same<T,uint8_t>::value )
			setDataType( ImageIo::UINT8 );
		else 
			throw; // what is this?

		setColorModel( ImageIo::CM_GRAY );		
		setChannelOrder( ImageIo::Y );
	}
	
	ChannelT<T>		*mChannel;
};

class ImageSourceChannel : public ImageSource {
  public:
	template<typename T>
	ImageSourceChannel( const ChannelT<T> &channel )
		: ImageSource()
	{
		mWidth = channel.getWidth();
		mHeight = channel.getHeight();
		setColorModel( ImageIo::CM_GRAY );
		setChannelOrder( ImageIo::Y );
		if( boost::is_same<T,uint8_t>::value )
			setDataType( ImageIo::UINT8 );
		else if( boost::is_same<T,float>::value )
			setDataType( ImageIo::FLOAT32 );
		else
			throw; // this channel seems to be a type we've never met
		mRowBytes = channel.getRowBytes();
		mData = reinterpret_cast<const uint8_t*>( channel.getData() );
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

template<typename T>
ChannelT<T>::Obj::Obj( int32_t aWidth, int32_t aHeight )
	: mWidth( aWidth ), mHeight( aHeight )
{
	mRowBytes = mWidth * sizeof(T);
	mIncrement = 1;
	
	mOwnsData = true;
	mData = new T[mWidth * mHeight];
	mDeallocatorFunc = 0;
}

template<typename T>
ChannelT<T>::Obj::Obj( int32_t aWidth, int32_t aHeight, int32_t aRowBytes, uint8_t aIncrement, bool aOwnsData, T *aData )
	: mWidth( aWidth ), mHeight( aHeight ), mRowBytes( aRowBytes ), mIncrement( aIncrement ), mOwnsData( aOwnsData ), mData( aData )
{
	mDeallocatorFunc = 0;
}

template<typename T>
ChannelT<T>::Obj::~Obj()
{
	if( mDeallocatorFunc )
		(*mDeallocatorFunc)( mDeallocatorRefcon );
	if( mOwnsData )
		delete [] mData;
}

template<typename T>
ChannelT<T>::ChannelT( int32_t width, int32_t height )
	: mObj( new Obj( width, height ) )
{
}

template<typename T>
ChannelT<T>::ChannelT( int32_t width, int32_t height, int32_t rowBytes, uint8_t increment, T *data )
	: mObj( new Obj( width, height, rowBytes, increment, false, data ) )
{
}

template<typename T>
ChannelT<T>::ChannelT( ImageSourceRef imageSource )
{
	int32_t width = imageSource->getWidth();
	int32_t height = imageSource->getHeight();
	int32_t rowBytes = width * sizeof(T);

	T *data = new T[height * (rowBytes/sizeof(T))];

	mObj = shared_ptr<Obj>( new Obj( width, height, rowBytes, 1, true, data ) );
	mObj->mOwnsData = true;
	
	shared_ptr<ImageTargetChannel<T> > target = ImageTargetChannel<T>::createRef( this );
	imageSource->load( target );	
}

template<typename T>
ChannelT<T>::operator ImageSourceRef() const
{
	return shared_ptr<ImageSource>( new ImageSourceChannel( *this ) );	
}


template<typename T>
void ChannelT<T>::setDeallocator( void(*aDeallocatorFunc)( void * ), void *aDeallocatorRefcon )
{
	mObj->mDeallocatorFunc = aDeallocatorFunc;
	mObj->mDeallocatorRefcon = aDeallocatorRefcon;
}

template<typename T>
ChannelT<T> ChannelT<T>::clone( bool copyPixels ) const
{
	ChannelT result( getWidth(), getHeight() );
	if( copyPixels )
		result.copyFrom( *this, getBounds() );
	
	return result;
}

template<typename T>
ChannelT<T> ChannelT<T>::clone( const Area &area, bool copyPixels ) const
{
	ChannelT result( area.getWidth(), area.getHeight() );
	if( copyPixels )
		result.copyFrom( *this, area, -area.getUL() );
	
	return result;
}


template<typename T>
void ChannelT<T>::copyFrom( const ChannelT<T> &srcChannel, const Area &srcArea, const Vec2i &relativeOffset )
{
	std::pair<Area,Vec2i> srcDst = clippedSrcDst( srcChannel.getBounds(), srcArea, getBounds(), srcArea.getUL() + relativeOffset );
	
	int32_t srcRowBytes = srcChannel.getRowBytes();
	uint8_t srcIncrement = srcChannel.getIncrement();
	uint8_t increment = mObj->mIncrement;
	
	int32_t width = srcDst.first.getWidth();
	
	for( int32_t y = 0; y < srcArea.getHeight(); ++y ) {
		const T *src = reinterpret_cast<const T*>( reinterpret_cast<const uint8_t*>( srcChannel.mObj->mData + srcArea.x1 * srcIncrement ) + ( srcArea.y1 + y ) * srcRowBytes );
		T *dst = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( mObj->mData + srcDst.second.x * mObj->mIncrement ) + ( y + srcDst.second.y ) * mObj->mRowBytes );
		for( int32_t x = 0; x < width; ++x ) {
			*dst = *src;
			src += srcIncrement;
			dst += increment;
		}
	}
}

template<typename T>
T ChannelT<T>::areaAverage( const Area &area ) const
{
	typename CHANTRAIT<T>::Sum sum = 0;
	const Area clipped( area.getClipBy( getBounds() ) );
	
	if( ( clipped.getWidth() <= 0 ) || ( clipped.getHeight() <= 0 ) )
		return 0;

	uint8_t increment = mObj->mIncrement;
	int32_t rowBytes = mObj->mRowBytes;
	
	const T *line = reinterpret_cast<const T*>( reinterpret_cast<const uint8_t*>( mObj->mData + clipped.x1 * mObj->mIncrement ) + clipped.y1 * mObj->mRowBytes );
	for( int32_t y = clipped.y1; y < clipped.y2; ++y ) {
		const T *d = line;
		for( int32_t x = clipped.x1; x < clipped.x2; ++x ) {
			sum += *d;
			d += increment;
		}
		
		line = reinterpret_cast<const T*>( reinterpret_cast<const uint8_t*>( line ) + rowBytes );
	} 
	
	return static_cast<T>( sum / ( clipped.getWidth() * clipped.getHeight() ) );
}

template class ChannelT<uint8_t>;
template class ChannelT<float>;

} // namespace cinder
