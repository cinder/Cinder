/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Portions Copyright (c) 2010, The Barbarian Group
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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Area.h"

#include <algorithm>

namespace cinder {

typedef std::shared_ptr<class ImageSource> ImageSourceRef;

//! A single channel of image data, either a color channel of a Surface or a grayscale image. \ImplShared
template<typename T>
class ChannelT {	 
 public:
	//! An null Channel.
	ChannelT();
	//! Allocates and owns a contiguous block of memory that is sizeof(T) * width * height
	ChannelT( int32_t width, int32_t height );
	//! Does not allocate or own memory pointed to by \a data
	ChannelT( int32_t width, int32_t height, ptrdiff_t rowBytes, uint8_t increment, T *data );
	//! Does not allocate memory pointed to by \a data but holds a reference to \a dataStore
	ChannelT( int32_t width, int32_t height, ptrdiff_t rowBytes, uint8_t increment, T *data, const std::shared_ptr<T> &dataStore );
	//! Creates a clone of \a rhs. Data is always stored planar regardless of the increment of \a rhs.
	ChannelT( const ChannelT &rhs );
	//! Move constructor. Receives data store of \a rhs.
	ChannelT( ChannelT &&rhs );
	//! Creates a ChannelT by loading from an ImageSource \a imageSource
	ChannelT( const ImageSourceRef &imageSource );

	ChannelT&	operator=( const ChannelT &rhs );
	ChannelT&	operator=( ChannelT &&rhs );	
	operator ImageSourceRef() const;

	//! Allocates and owns a contiguous block of memory that is sizeof(T) * width * height
	static std::shared_ptr<ChannelT<T>> create( int32_t width, int32_t height )
	{ return std::make_shared<ChannelT<T>>( width, height ); }
	
	//! Does not allocate or own memory pointed to by \a data
	static std::shared_ptr<ChannelT<T>> create( int32_t width, int32_t height, ptrdiff_t rowBytes, uint8_t increment, T *data )
	{ return std::make_shared<ChannelT<T>>( width, height, rowBytes, increment, data ); }
	
	//! Does not allocate memory pointed to by \a data but holds a reference to \a dataStore
	static std::shared_ptr<ChannelT<T>> create( int32_t width, int32_t height, ptrdiff_t rowBytes, uint8_t increment, T *data, const std::shared_ptr<T> &dataStore )
	{ return std::make_shared<ChannelT<T>>( width, height, rowBytes, increment, data, dataStore ); }
	
	//! Creates a clone of \a rhs. Data is always stored planar regardless of the increment of \a rhs.
	static std::shared_ptr<ChannelT<T>> create( const ChannelT &rhs )
	{ return std::make_shared<ChannelT<T>>( rhs ); }
	
	//! Creates a ChannelT by loading from an ImageSource \a imageSource
	static std::shared_ptr<ChannelT<T>> create( const ImageSourceRef &imageSource )
	{ return std::make_shared<ChannelT<T>>( imageSource ); }
	

	//! Returns a new Channel which is a duplicate. If \a copyPixels the pixel values are copied, otherwise the clone's pixels remain uninitialized.
	ChannelT	clone( bool copyPixels = true ) const;
	//! Returns a new Channel which is a duplicate of an Area \a area. If \a copyPixels the pixel values are copied, otherwise the clone's pixels remain uninitialized.
	ChannelT	clone( const Area &area, bool copyPixels = true ) const;
	
	//! Returns the width of the Channel in pixels
	int32_t		getWidth() const { return mWidth; }
	//! Returns the height of the Channel in pixels
	int32_t		getHeight() const { return mHeight; }
	//! Returns the size of the Channel in pixels
	ivec2		getSize() const { return ivec2( mWidth, mHeight ); }
	//! Returns the Channel aspect ratio, which is its width / height
	float		getAspectRatio() const { return mWidth / (float)mHeight; }
	//! Returns the bounding Area of the Channel in pixels: [0,0]-(width,height)
	Area		getBounds() const { return Area( 0, 0, mWidth, mHeight ); }
	//! Returns the width of a row of the Channel measured in bytes, which is not necessarily getWidth() * getPixelInc()
	ptrdiff_t	getRowBytes() const { return mRowBytes; }
	//! Returns the amount to increment a T* to increment by a pixel. For a planar channel this is \c 1, but for a Channel of a Surface this might be \c 3 or \c 4
	uint8_t		getIncrement() const { return mIncrement; }
	//! Returns whether the Channel represents a tightly packed array of values. This will be \c false if the Channel is a member of a Surface. Analogous to <tt>getIncrement() == 1</tt>
	bool		isPlanar() const { return mIncrement == 1; }

	//! Returns a pointer to the data of the Channel's first pixel. Result is a uint8_t* for Channel8u and a float* for Channel32f.
	T*			getData() { return mData; }
	//! Returns a const pointer to the data of the Channel's first pixel. Result is a uint8_t* for Channel8u and a float* for Channel32f.
	const T*	getData() const { return mData; }
	//! Returns a pointer to the data of the Channel's pixel at \a offset. Result is a uint8_t* for Channel8u and a float* for Channel32f.	
	T*			getData( const ivec2 &offset ) { return reinterpret_cast<T*>( reinterpret_cast<unsigned char*>( mData + offset.x * mIncrement ) + offset.y * mRowBytes ); }
	//! Returns a const pointer to the data of the Channel's pixel at \a offset. Result is a uint8_t* for Channel8u and a float* for Channel32f.	
	const T*	getData( const ivec2 &offset ) const { return reinterpret_cast<T*>( reinterpret_cast<unsigned char*>( mData + offset.x * mIncrement ) + offset.y * mRowBytes ); }
	//! Returns a pointer to the data of the Channel's pixel at (\a x, \a y). Result is a uint8_t* for Channel8u and a float* for Channel32f.		
	T*			getData( int32_t x, int32_t y ) { return reinterpret_cast<T*>( reinterpret_cast<unsigned char*>( mData + x * mIncrement ) + y * mRowBytes ); }
	//! Returns a const pointer to the data of the Channel's pixel at (\a x, \a y). Result is a uint8_t* for Channel8u and a float* for Channel32f.		
	const T*	getData( int32_t x, int32_t y ) const { return reinterpret_cast<T*>( reinterpret_cast<unsigned char*>( mData + x * mIncrement ) + y * mRowBytes ); }

	//! Convenience method for getting a single value at pixel \a pos. For performance-sensitive code consider \ref ChannelT::Iter "Channel::Iter" instead. Exhibits clamping behavior when outside Channel boundaries.
	T		getValue ( ivec2 pos ) const { pos.x = constrain<int32_t>( pos.x, 0, mWidth - 1); pos.y = constrain<int32_t>( pos.y, 0, mHeight - 1 ); return *getData( pos ); }
	//! Convenience method for setting a single value \a v at pixel \a pos. For performance-sensitive code consider \ref ChannelT::Iter "Channel::Iter" instead. Exhibits clamping behavior when outside Channel boundaries.
	void	setValue( ivec2 pos, T v ) { pos.x = constrain<int32_t>( pos.x, 0, mWidth - 1); pos.y = constrain<int32_t>( pos.y, 0, mHeight - 1 ); *getData( pos ) = v; }

	//! Copies the Area \a srcArea of the Channel \a srcChannel to \a this Channel. The destination Area is \a srcArea offset by \a relativeOffset.
	void		copyFrom( const ChannelT<T> &srcChannel, const Area &srcArea, const ivec2 &relativeOffset = ivec2() );

	//! Returns an averaged value for the Area defined by \a area
	T			areaAverage( const Area &area ) const;

	//! Returns the shared_ptr used to store the Channel's data. In general prefer getData() instead.
	std::shared_ptr<T>	getDataStore() const { return mDataStore; }

	/*! Convenience class for iterating the pixels of a Channel. */
	class Iter {
	 public:
		Iter( ChannelT<T> &channelT, const Area &area ) 
			: mInc( channelT.getIncrement() ), mRowInc( channelT.getRowBytes() )
		{
			Area clippedArea( area.getClipBy( channelT.getBounds() ) );
			mWidth = clippedArea.getWidth();
			mHeight = clippedArea.getHeight();
			mLinePtr = reinterpret_cast<uint8_t*>( channelT.getData( clippedArea.getUL() ) );
			mPtr = reinterpret_cast<T*>( mLinePtr );
			mStartX = mX = clippedArea.getX1();
			mStartY = mY = clippedArea.getY1();			
			mEndX = clippedArea.getX2();
			mEndY = clippedArea.getY2();
			// in order to be at the right place after an initial call to line(), we need to back up one line
			mY = clippedArea.getY1() - 1;
			mLinePtr -= mRowInc;
		}

		//! Returns a reference to the value of the pixel that the Iter currently points to
		T&		v() const { return *mPtr; }
		//! Returns a reference to the value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels		
		T&		v( int32_t xOff, int32_t yOff ) const { return mPtr[xOff * mInc + yOff * mRowInc]; }
		//! Returns a reference to the value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels. Clamps offset to the bounds of the Iter.
		T&		vClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return *(T*)((uint8_t*)( mPtr + xOff * mInc ) + yOff * mRowInc); }

		//! Returns the x coordinate of the pixel the Iter currently points to
		const int32_t	x() const { return mX; }
		//! Returns the y coordinate of the pixel the Iter currently points to		
		const int32_t	y() const { return mY; }
		//! Returns the coordinate of the pixel the Iter currently points to		
		ivec2			getPos() const { return ivec2( mX, mY ); }

		//! Increments which pixel of the current row the Iter points to, and returns \c false when no pixels remain in the current row.
		bool pixel() {
			++mX;
			mPtr += mInc;
			return mX < mEndX;
		}
	
		//! Increments which row the Iter points to, and returns \c false when no rows remain in the Channel.
		bool line() {
			++mY;
			mLinePtr += mRowInc;
			mPtr = reinterpret_cast<T*>( mLinePtr );
			// in order to be at the right place after an initial call to pixel(), we need to back up one pixel
			mPtr -= mInc;
			mX = mStartX - 1;
			return mY < mEndY;
		}
		
		//! Returns the width of the Area the Iter iterates
		int32_t		getWidth() { return mWidth; }
		//! Returns the height of the Area the Iter iterates
		int32_t		getHeight() { return mHeight; }

		/// \cond
		uint8_t				mInc;
		ptrdiff_t			mRowInc;
		uint8_t				*mLinePtr;
		T					*mPtr;
		int32_t				mWidth, mHeight;
		int32_t				mX, mY, mStartX, mStartY, mEndX, mEndY;
		/// \endcond
	};

	//! Convenience class for iterating the pixels of a Channel. The iteration is \c const, performing read-only operations on the Channel.
	class ConstIter {
	 public:
		ConstIter( const ChannelT<T> &channelT, const Area &area ) 
			: mInc( channelT.getIncrement() ), mRowInc( channelT.getRowBytes() )
		{
			Area clippedArea( area.getClipBy( channelT.getBounds() ) );
			mWidth = clippedArea.getWidth();
			mHeight = clippedArea.getHeight();
			mLinePtr = reinterpret_cast<const uint8_t*>( channelT.getData( clippedArea.getUL() ) );
			mPtr = reinterpret_cast<const T*>( mLinePtr );
			mStartX = mX = clippedArea.getX1();
			mStartY = mY = clippedArea.getY1();			
			mEndX = clippedArea.getX2();
			mEndY = clippedArea.getY2();
			// in order to be at the right place after an initial call to line(), we need to back up one line
			mY = clippedArea.getY1() - 1;
			mLinePtr -= mRowInc;
		}

		//! Returns a reference to the value of the pixel that the Iter currently points to
		const T&	v() const { return *mPtr; }
		//! Returns a reference to the value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels
		const T&	v( int32_t xOff, int32_t yOff ) const { return mPtr[xOff * mInc + yOff * mRowInc]; }
		//! Returns a reference to the value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels. Clamps offset to the bounds of the Iter.
		const T&	vClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return *(T*)((uint8_t*)( mPtr + xOff * mInc ) + yOff * mRowInc); }

		//! Returns the x coordinate of the pixel the Iter currently points to
		const int32_t	x() const { return mX; }
		//! Returns the y coordinate of the pixel the Iter currently points to
		const int32_t	y() const { return mY; }
		//! Returns the coordinate of the pixel the Iter currently points to		
		ivec2			getPos() const { return ivec2( mX, mY ); }

		//! Increments which pixel of the current row the Iter points to, and returns \c false when no pixels remain in the current row.
		bool pixel() {
			++mX;
			mPtr += mInc;
			return mX < mEndX;
		}

		//! Increments which row the Iter points to, and returns \c false when no rows remain in the Channel.		
		bool line() {
			++mY;
			mLinePtr += mRowInc;
			mPtr = reinterpret_cast<const T*>( mLinePtr );
			// in order to be at the right place after an initial call to pixel(), we need to back up one pixel
			mPtr -= mInc;
			mX = mStartX - 1;
			return mY < mEndY;
		}

		//! Returns the width of the Area the Iter iterates		
		int32_t		getWidth() { return mWidth; }
		//! Returns the height of the Area the Iter iterates
		int32_t		getHeight() { return mHeight; }

		/// \cond
		uint8_t				mInc;
		ptrdiff_t			mRowInc;
		const uint8_t		*mLinePtr;
		const T				*mPtr;
		int32_t				mWidth, mHeight;
		int32_t				mX, mY, mStartX, mStartY, mEndX, mEndY;
		/// \endcond
	};

	//! Returns an Iter which iterates the entire Channel.
	Iter		getIter() { return Iter( *this, this->getBounds() ); }
	//! Returns an Iter which iterates the Area \a area.
	Iter		getIter( const Area &area ) { return Iter( *this, area ); }
	//! Returns a ConstIter which iterates the entire Channel.
	ConstIter	getIter() const { return ConstIter( *this, this->getBounds() ); }
	//! Returns a ConstIter which iterates the Area \a area.
	ConstIter	getIter( const Area &area ) const { return ConstIter( *this, area ); }

  protected:
	int32_t						mWidth, mHeight;
	uint8_t						mIncrement;
	ptrdiff_t					mRowBytes;
	T							*mData;
	std::shared_ptr<T>			mDataStore;
};


//! 8-bit image channel. Synonym for Channel8u.
typedef ChannelT<uint8_t>			Channel;
//! 8-bit image channel
typedef ChannelT<uint8_t>			Channel8u;
typedef std::shared_ptr<Channel8u>	Channel8uRef;
typedef std::shared_ptr<Channel8u>	ChannelRef;
//! 16-bit image channel. Suitable as an intermediate representation and ImageIo but not a first-class citizen.	
typedef ChannelT<uint16_t>			Channel16u;
typedef std::shared_ptr<Channel16u>	Channel16uRef;
//! 32-bit floating point image channel
typedef ChannelT<float>				Channel32f;
typedef std::shared_ptr<Channel32f>	Channel32fRef;

} // namespace cinder
