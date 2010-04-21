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

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Area.h"

namespace cinder {

typedef shared_ptr<class ImageSource> ImageSourceRef;

template<typename T>
class ChannelT {
 protected:
 	struct Obj {
		Obj( int32_t width, int32_t height );
		Obj( int32_t aWidth, int32_t aHeight, int32_t aRowBytes, uint8_t aIncrement, bool aOwnsData, T *aData );
		~Obj();
			
		int32_t						mWidth, mHeight, mRowBytes;
		T							*mData;
		uint8_t						mIncrement;
		bool						mOwnsData;
		
		void						(*mDeallocatorFunc)(void *refcon);
		void						*mDeallocatorRefcon;
	};

 public:
	//! default constructor, creates an invalid ChannelT
	ChannelT() {}
	//! Allocates and owns a contiguous block of memory that is sizeof(T) * width * height
	ChannelT( int32_t width, int32_t height );
	//! Does not allocate or own memory pointed to by \a data
	ChannelT( int32_t width, int32_t height, int32_t rowBytes, uint8_t increment, T *data );
	//! Creates a ChannelT by loading from an ImageSource \a imageSource
	ChannelT( ImageSourceRef imageSource );

	operator ImageSourceRef() const;

	//! Returns a new Channel which is a duplicate. If \a copyPixels the pixel values are copied, otherwise the clone's pixels remain uninitialized.
	ChannelT			clone( bool copyPixels = true ) const;
	//! Returns a new Channel which is a duplicate of an Area \a area. If \a copyPixels the pixel values are copied, otherwise the clone's pixels remain uninitialized.
	ChannelT			clone( const Area &area, bool copyPixels = true ) const;
	
	//! Returns the width of the Channel in pixels
	int32_t		getWidth() const { return mObj->mWidth; }
	//! Returns the height of the Channel in pixels
	int32_t		getHeight() const { return mObj->mHeight; }
	//! Returns the size of the Channel in pixels
	Vec2i		getSize() const { return Vec2i( mObj->mWidth, mObj->mHeight ); }
	//! Returns the Channel aspect ratio, which is its width / height
	float		getAspectRatio() const { return mObj->mWidth / (float)mObj->mHeight; }
	//! Returns the bounding Area of the Channel in pixels: [0,0]-(width,height)
	Area		getBounds() const { return Area( 0, 0, mObj->mWidth, mObj->mHeight ); }
	//! Returns the width of a row of the Channel measured in bytes, which is not necessarily getWidth() * getPixelInc()
	int32_t		getRowBytes() const { return mObj->mRowBytes; }
	//! Returns the amount to increment a T* to increment by a pixel. For a planar channel this is \c 1, but for a Channel of a Surface this might be \c 3 or \c 4
	uint8_t		getIncrement() const { return mObj->mIncrement; }
	//! Returns whether the Channel represents a tightly packed array of values. This will be \c false if the Channel is a member of a Surface. Analogous to <tt>getIncrement() == 1</tt>
	bool		isPlanar() const { return mObj->mIncrement == 1; }

	T*			getData() { return mObj->mData; }
	const T*	getData() const { return mObj->mData; }
	T*			getData( const Vec2i &offset ) { return reinterpret_cast<T*>( reinterpret_cast<unsigned char*>( mObj->mData + offset.x * mObj->mIncrement ) + offset.y * mObj->mRowBytes ); }
	const T*	getData( const Vec2i &offset ) const { return reinterpret_cast<T*>( reinterpret_cast<unsigned char*>( mObj->mData + offset.x * mObj->mIncrement ) + offset.y * mObj->mRowBytes ); }
	T*			getData( int32_t x, int32_t y ) { return reinterpret_cast<T*>( reinterpret_cast<unsigned char*>( mObj->mData + x * mObj->mIncrement ) + y * mObj->mRowBytes ); }
	const T*	getData( int32_t x, int32_t y ) const { return reinterpret_cast<T*>( reinterpret_cast<unsigned char*>( mObj->mData + x * mObj->mIncrement ) + y * mObj->mRowBytes ); }

	//! Convenience method for getting a single value. For performance-sensitive code consider Channel::Iter instead. Exhibits clamping behavior when outside Channel boundaries.
	T		getValue ( Vec2i pos ) const { pos.x = constrain<int32_t>( pos.x, 0, mObj->mWidth - 1); pos.y = constrain<int32_t>( pos.y, 0, mObj->mHeight - 1 ); return *getData( pos ); }
	//! Convenience method for setting a single value. For performance-sensitive code consider Channel::Iter instead. Exhibits clamping behavior when outside Channel boundaries.
	void	setValue( Vec2i pos, T v ) { pos.x = constrain<int32_t>( pos.x, 0, mObj->mWidth - 1); pos.y = constrain<int32_t>( pos.y, 0, mObj->mHeight - 1 ); *getData( pos ) = v; }

	void		copyFrom( const ChannelT<T> &srcChannel, const Area &srcArea, const Vec2i &relativeOffset = Vec2i::zero() );

	T			areaAverage( const Area &area ) const;

	void		setDeallocator( void(*aDeallocatorFunc)( void * ), void *aDeallocatorRefcon );
	
	//@{
	//! Emulates shared_ptr-like behavior
	typedef typename shared_ptr<Obj>::unspecified_bool_type unspecified_bool_type;
	operator unspecified_bool_type() const { return static_cast<typename shared_ptr<Obj>::unspecified_bool_type>( mObj ); }
	void reset() { mObj.reset(); }
	//@}

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
		
		T&			v() const { return *mPtr; }
		T&			v( int32_t xOff, int32_t yOff ) const { return mPtr[xOff * mInc + yOff * mRowInc]; }
		T&			vClamped ( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return mPtr[xOff * mInc + yOff * mRowInc]; }

		const int32_t	x() const { return mX; }
		const int32_t	y() const { return mY; }
		Vec2i			getPos() const { return Vec2i( mX, mY ); }

		bool pixel() {
			++mX;
			mPtr += mInc;
			return mX < mEndX;
		}
		
		bool line() {
			++mY;
			mLinePtr += mRowInc;
			mPtr = reinterpret_cast<T*>( mLinePtr );
			// in order to be at the right place after an initial call to pixel(), we need to back up one pixel
			mPtr -= mInc;
			mX = mStartX - 1;
			return mY < mEndY;
		}
		
		int32_t		getWidth() { return mWidth; }
		int32_t		getHeight() { return mHeight; }

		uint8_t				mInc;
		uint8_t				*mLinePtr;
		T					*mPtr;
		int32_t				mRowInc, mWidth, mHeight;
		int32_t				mX, mY, mStartX, mStartY, mEndX, mEndY;
	};

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
		
		const T&	v() const { return *mPtr; }
		const T&	v( int32_t xOff, int32_t yOff ) const { return mPtr[xOff * mInc + yOff * mRowInc]; }
		const T&	vClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return mPtr[xOff * mInc + yOff * mRowInc]; }

		const int32_t	x() const { return mX; }
		const int32_t	y() const { return mY; }
		Vec2i			getPos() const { return Vec2i( mX, mY ); }

		bool pixel() {
			++mX;
			mPtr += mInc;
			return mX < mEndX;
		}
		
		bool line() {
			++mY;
			mLinePtr += mRowInc;
			mPtr = reinterpret_cast<const T*>( mLinePtr );
			// in order to be at the right place after an initial call to pixel(), we need to back up one pixel
			mPtr -= mInc;
			mX = mStartX - 1;
			return mY < mEndY;
		}
		
		int32_t		getWidth() { return mWidth; }
		int32_t		getHeight() { return mHeight; }

		uint8_t				mInc;
		const uint8_t		*mLinePtr;
		const T				*mPtr;
		int32_t				mRowInc, mWidth, mHeight;
		int32_t				mX, mY, mStartX, mStartY, mEndX, mEndY;
	};

	Iter		getIter() { return Iter( *this, this->getBounds() ); }
	Iter		getIter( const Area &area ) { return Iter( *this, area ); }
	ConstIter	getIter() const { return ConstIter( *this, this->getBounds() ); }
	ConstIter	getIter( const Area &area ) const { return ConstIter( *this, area ); }
	
 protected:
 	shared_ptr<Obj>		mObj;
};


typedef ChannelT<uint8_t>	Channel;
typedef ChannelT<uint8_t>	Channel8u;
typedef ChannelT<float>		Channel32f;

} // namespace cinder
