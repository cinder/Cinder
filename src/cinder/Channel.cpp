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

#include <type_traits>

using namespace std;

namespace cinder {

template<typename T>
class ImageTargetChannel : public ImageTarget {
  public:
	static shared_ptr<ImageTargetChannel<T> > createRef( ChannelT<T> *channel ) { return shared_ptr<ImageTargetChannel<T> >( new ImageTargetChannel<T>( channel ) ); }

	virtual bool hasAlpha() const { return false; }
	
	virtual void*	getRowPointer( int32_t row ) { return reinterpret_cast<void*>( mChannel->getData( ivec2( 0, row ) ) ); }
	
  protected:
	ImageTargetChannel( ChannelT<T> *channel )
		: mChannel( channel )
	{
		if( std::is_same<T,float>::value )
			setDataType( ImageIo::FLOAT32 );
		else if( std::is_same<T,uint16_t>::value )
			setDataType( ImageIo::UINT16 );
		else if( std::is_same<T,uint8_t>::value )
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
		if( channel.getIncrement() != 1 )
			setCustomPixelInc( channel.getIncrement() );
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
			throw; // this channel seems to be a type we've never met
		mRowBytes = channel.getRowBytes();
		mDataStore = channel.getDataStore();
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

	// this is used to register a reference to the channel we were constructed with
	shared_ptr<void>	mDataStore;
	const uint8_t		*mData;
	ptrdiff_t			mRowBytes;
};

template<typename T>
ChannelT<T>::ChannelT()
	: mWidth( 0 ), mHeight( 0 ), mRowBytes( 0 ), mIncrement( 1 ), mData( nullptr )
{
}

template<typename T>
ChannelT<T>::ChannelT( int32_t width, int32_t height )
	: mWidth( width ), mHeight( height )
{
	mRowBytes = mWidth * sizeof(T);
	mIncrement = 1;
	
	mDataStore = shared_ptr<T>( new T[mWidth * mHeight], std::default_delete<T[]>() );
	mData = mDataStore.get();
}

template<typename T>
ChannelT<T>::ChannelT( int32_t width, int32_t height, ptrdiff_t rowBytes, uint8_t increment, T *data )
	: mWidth( width ), mHeight( height ), mRowBytes( rowBytes ), mIncrement( increment ), mData( data )
{
	mDataStore = nullptr;
}

template<typename T>
ChannelT<T>::ChannelT( int32_t width, int32_t height, ptrdiff_t rowBytes, uint8_t increment, T *data, const std::shared_ptr<T> &dataStore )
	: mWidth( width ), mHeight( height ), mRowBytes( rowBytes ), mIncrement( increment ), mData( data ), mDataStore( dataStore )
{
}

template<typename T>
ChannelT<T>::ChannelT( const ChannelT &rhs )
	: mWidth( rhs.mWidth ), mHeight( rhs.mHeight ), mRowBytes( mWidth * sizeof(T) ), mIncrement( 1 )
{
	mDataStore = shared_ptr<T>( new T[mWidth * mHeight], std::default_delete<T[]>() );
	mData = mDataStore.get();

	copyFrom( rhs, Area( 0, 0, mWidth, mHeight ) );
}

template<typename T>
ChannelT<T>::ChannelT( ChannelT &&rhs )
	: mWidth( rhs.mWidth ), mHeight( rhs.mHeight ), mRowBytes( rhs.mRowBytes ), mIncrement( rhs.mIncrement ), mDataStore( rhs.mDataStore ), mData( rhs.mData )
{
	rhs.mDataStore.reset();
	rhs.mData = nullptr;
}

template<typename T>
ChannelT<T>::ChannelT( const ImageSourceRef &imageSource )
{
	mWidth = imageSource->getWidth();
	mHeight = imageSource->getHeight();
	mRowBytes = mWidth * sizeof(T);
	mIncrement = 1;

	mDataStore = shared_ptr<T>( new T[mHeight * (mRowBytes/sizeof(T))], std::default_delete<T[]>() );
	mData = mDataStore.get();
	
	shared_ptr<ImageTargetChannel<T>> target = ImageTargetChannel<T>::createRef( this );
	imageSource->load( target );
}

template<typename T>
ChannelT<T>& ChannelT<T>::operator=( const ChannelT &rhs )
{
	mWidth = rhs.mWidth;
	mHeight = rhs.mHeight;
	mRowBytes = mWidth * sizeof(T);
	mIncrement = 1;
	mDataStore = shared_ptr<T>( new T[mHeight * mWidth], std::default_delete<T[]>() );
	mData = mDataStore.get();
	copyFrom( rhs, Area( 0, 0, mWidth, mHeight ) );
	
	return *this;
}

template<typename T>
ChannelT<T>& ChannelT<T>::operator=( ChannelT &&rhs )
{
	mWidth = rhs.mWidth;
	mHeight = rhs.mHeight;
	mRowBytes = rhs.mRowBytes;
	mIncrement = rhs.mIncrement;
	mDataStore = rhs.mDataStore;
	mData = rhs.mData;

	rhs.mDataStore.reset();
	rhs.mData = nullptr;

	return *this;
}

template<typename T>
ChannelT<T>::operator ImageSourceRef() const
{
	return shared_ptr<ImageSource>( new ImageSourceChannel( *this ) );	
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
void ChannelT<T>::copyFrom( const ChannelT<T> &srcChannel, const Area &srcArea, const ivec2 &relativeOffset )
{
	std::pair<Area,ivec2> srcDst = clippedSrcDst( srcChannel.getBounds(), srcArea, getBounds(), srcArea.getUL() + relativeOffset );
	
	ptrdiff_t srcRowBytes = srcChannel.getRowBytes();
	uint8_t srcIncrement = srcChannel.getIncrement();
	uint8_t increment = mIncrement;
	
	int32_t width = srcDst.first.getWidth();
	
	for( int32_t y = 0; y < srcArea.getHeight(); ++y ) {
		const T *src = reinterpret_cast<const T*>( reinterpret_cast<const uint8_t*>( srcChannel.mData + srcArea.x1 * srcIncrement ) + ( srcArea.y1 + y ) * srcRowBytes );
		T *dst = reinterpret_cast<T*>( reinterpret_cast<uint8_t*>( mData + srcDst.second.x * mIncrement ) + ( y + srcDst.second.y ) * mRowBytes );
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

	uint8_t increment = mIncrement;
	ptrdiff_t rowBytes = mRowBytes;
	
	const T *line = reinterpret_cast<const T*>( reinterpret_cast<const uint8_t*>( mData + clipped.x1 * mIncrement ) + clipped.y1 * mRowBytes );
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

template class CI_API ChannelT<uint8_t>;
template class CI_API ChannelT<uint16_t>;
template class CI_API ChannelT<float>;

} // namespace cinder
