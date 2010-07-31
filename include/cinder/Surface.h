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
#include "cinder/Channel.h"
#include "cinder/ChanTraits.h"
#include "cinder/Color.h"

#include <boost/logic/tribool.hpp>

namespace cinder {

class SurfaceChannelOrder {
  public:
	SurfaceChannelOrder() : mCode( UNSPECIFIED ), mRed( INVALID ), mGreen( INVALID ), mBlue( INVALID ), mAlpha( INVALID ), mPixelInc( INVALID ) {}
	SurfaceChannelOrder( int aCode );
	SurfaceChannelOrder( const SurfaceChannelOrder &aOrder );
		
//	static ChannelOrder	GetPlatformNativeChannelOrder( bool includeAlpha );
	
	uint8_t		getRedOffset() const { return mRed; }
	uint8_t		getGreenOffset() const { return mGreen; }
	uint8_t		getBlueOffset() const { return mBlue; }
	uint8_t		getAlphaOffset() const { return mAlpha; }
	bool		hasAlpha() const { return ( mAlpha != INVALID ) ? true : false; }
	uint8_t		getPixelInc() const { return mPixelInc; }
	int			getCode() const { return mCode; }

	bool operator==( const SurfaceChannelOrder& sco ) const 
	{
		return mCode == sco.mCode;
	}

	enum { CHAN_RED, CHAN_GREEN, CHAN_BLUE, CHAN_ALPHA, INVALID = 255 };	
	enum { RGBA, BGRA, ARGB, ABGR, RGBX, BGRX, XRGB, XBGR, RGB, BGR, UNSPECIFIED }; // Codes

	int			getImageIoChannelOrder() const;

  private:
	void		set( uint8_t aRed, uint8_t aGreen, uint8_t aBlue, uint8_t aAlpha, uint8_t aPixelInc );
	int			mCode; // the enum 
	uint8_t		mRed, mGreen, mBlue, mAlpha, mPixelInc;
	
};

//! Base class for defining the properties of a Surface necessary to be interoperable with different APIs
class SurfaceConstraints {
 public:
	virtual ~SurfaceConstraints() {}
 
	virtual SurfaceChannelOrder getChannelOrder( bool alpha ) const { return ( alpha ) ? SurfaceChannelOrder::RGBA : SurfaceChannelOrder::RGB; }
	virtual int32_t				getRowBytes( int requestedWidth, const SurfaceChannelOrder &sco, int elementSize ) const { return requestedWidth * elementSize * sco.getPixelInc(); }
};

class SurfaceConstraintsDefault : public SurfaceConstraints {
};

typedef shared_ptr<class ImageSource> ImageSourceRef;

template<typename T>
class SurfaceT {
 private:
	/// \cond
	struct Obj {
		Obj( int32_t aWidth, int32_t aHeight, SurfaceChannelOrder aChannelOrder, T *aData, bool aOwnsData, int32_t aRowBytes );
		~Obj();
		
		void		initChannels();
		void		setData( T *aData, int32_t aWidth, int32_t aHeight, int32_t aRowBytes );
		void		setChannelOrder( const SurfaceChannelOrder &aChannelOrder );
		void		setDeallocator( void(*aDeallocatorFunc)( void * ), void *aDeallocatorRefcon );
	
		int32_t						mWidth, mHeight, mRowBytes;
		bool						mIsPremultiplied;
		T							*mData;
		bool						mOwnsData;
		SurfaceChannelOrder			mChannelOrder;
		ChannelT<T>					mChannels[4];
		
		void						(*mDeallocatorFunc)(void *refcon);
		void						*mDeallocatorRefcon;
	};
	/// \endcond

 public:
	SurfaceT() {}
	SurfaceT( int32_t aWidth, int32_t aHeight, bool alpha, SurfaceChannelOrder aChannelOrder = SurfaceChannelOrder::UNSPECIFIED );
	SurfaceT( int32_t aWidth, int32_t aHeight, bool alpha, const SurfaceConstraints &constraints );
	SurfaceT( T *aData, int32_t aWidth, int32_t aHeight, int32_t aRowBytes, SurfaceChannelOrder aChannelOrder );
	SurfaceT( shared_ptr<class ImageSource> imageSource, const SurfaceConstraints &constraints = SurfaceConstraintsDefault(), boost::tribool alpha = boost::logic::indeterminate );

	operator ImageSourceRef() const;

	//! Returns the width of the Surface in pixels
	int32_t			getWidth() const { return mObj->mWidth; }
	//! Returns the height of the Surface in pixels
	int32_t			getHeight() const { return mObj->mHeight; }
	//! Returns the size of the Surface in pixels
	Vec2i			getSize() const { return Vec2i( mObj->mWidth, mObj->mHeight ); }
	//! Returns the Surface aspect ratio, which is its width / height
	float			getAspectRatio() const { return mObj->mWidth / (float)mObj->mHeight; }
	//! Returns the bounding Area of the Surface in pixels: [0,0]-(width,height)
	Area			getBounds() const { return Area( 0, 0, mObj->mWidth, mObj->mHeight ); }
	//! Returns whether the Surface contains an alpha channel
	bool			hasAlpha() const { return mObj->mChannelOrder.hasAlpha(); }
	//! Returns whether the Surface color data is premultiplied by its alpha channel or not
	bool			isPremultiplied() const { return mObj->mIsPremultiplied; }
	//! Sets whether the Surface color data should be interpreted as being premultiplied by its alpha channel or not
	bool			setPremultiplied( bool premult = true ) const { return mObj->mIsPremultiplied = premult; }
	//! Returns the width of a row of the Surface measured in bytes, which is not necessarily getWidth() * getPixelInc()
	int32_t			getRowBytes() const { return mObj->mRowBytes; }
	//! Returns the amount to increment a T* to increment by a pixel. Analogous to the number of channels, which is either 3 or 4
	uint8_t			getPixelInc() const { return mObj->mChannelOrder.getPixelInc(); }

	//! Returns a new Surface which is a duplicate. If \a copyPixels the pixel values are copied, otherwise the clone's pixels remain uninitialized
	SurfaceT			clone( bool copyPixels = true ) const;
	//! Returns a new Surface which is a duplicate of an Area \a area. If \a copyPixels the pixel values are copied, otherwise the clone's pixels remain uninitialized
	SurfaceT			clone( const Area &area, bool copyPixels = true ) const;

	T*					getData() { return mObj->mData; }
	const T*			getData() const { return mObj->mData; }
	T*					getData( const Vec2i &offset ) { return reinterpret_cast<T*>( reinterpret_cast<unsigned char*>( mObj->mData + offset.x * getPixelInc() ) + offset.y * mObj->mRowBytes ); }
	const T*			getData( const Vec2i &offset ) const { return reinterpret_cast<T*>( reinterpret_cast<unsigned char*>( mObj->mData + offset.x * getPixelInc() ) + offset.y * mObj->mRowBytes ); }
	T*					getDataRed( const Vec2i &offset ) { return getData( offset ) + getRedOffset(); }
	const T*			getDataRed( const Vec2i &offset ) const { return getData( offset ) + getRedOffset(); }
	T*					getDataGreen( const Vec2i &offset ) { return getData( offset ) + getGreenOffset(); }
	const T*			getDataGreen( const Vec2i &offset ) const { return getData( offset ) + getGreenOffset(); }
	T*					getDataBlue( const Vec2i &offset ) { return getData( offset ) + getBlueOffset(); }
	const T*			getDataBlue( const Vec2i &offset ) const { return getData( offset ) + getBlueOffset(); }
	T*					getDataAlpha( const Vec2i &offset ) { return getData( offset ) + getAlphaOffset(); }
	const T*			getDataAlpha( const Vec2i &offset ) const { return getData( offset ) + getAlphaOffset(); }
	
	void				setData( T *aData, int32_t aWidth, int32_t aHeight, int32_t aRowBytes );
	/** Set's the deallocator, an optional callback which will fire upon the Surface::Obj's destruction. This is useful when a Surface is wrapping another API's image data structure whose lifetime is tied to the Surface's. **/
	void				setDeallocator( void(*aDeallocatorFunc)( void * ), void *aDeallocatorRefcon );
	
	const SurfaceChannelOrder&	getChannelOrder() const { return mObj->mChannelOrder; }
	uint8_t						getRedOffset() const { return mObj->mChannelOrder.getRedOffset(); }
	uint8_t						getGreenOffset() const { return mObj->mChannelOrder.getGreenOffset(); }
	uint8_t						getBlueOffset() const { return mObj->mChannelOrder.getBlueOffset(); }
	uint8_t						getAlphaOffset() const { return mObj->mChannelOrder.getAlphaOffset(); }
	void						setChannelOrder( const SurfaceChannelOrder &aChannelOrder );

	ChannelT<T>*				getChannel( uint8_t ChannelT ) { return &mObj->mChannels[ChannelT]; }
	ChannelT<T>*				getChannelRed() { return &mObj->mChannels[SurfaceChannelOrder::CHAN_RED]; }
	ChannelT<T>*				getChannelGreen() { return &mObj->mChannels[SurfaceChannelOrder::CHAN_GREEN]; }
	ChannelT<T>*				getChannelBlue() { return &mObj->mChannels[SurfaceChannelOrder::CHAN_BLUE]; }
	ChannelT<T>*				getChannelAlpha() { return &mObj->mChannels[SurfaceChannelOrder::CHAN_ALPHA]; }

	const ChannelT<T>*			getChannel( uint8_t ChannelT ) const { return &mObj->mChannels[ChannelT]; }
	const ChannelT<T>*			getChannelRed() const { return &mObj->mChannels[SurfaceChannelOrder::CHAN_RED]; }
	const ChannelT<T>*			getChannelGreen() const { return &mObj->mChannels[SurfaceChannelOrder::CHAN_GREEN]; }
	const ChannelT<T>*			getChannelBlue() const { return &mObj->mChannels[SurfaceChannelOrder::CHAN_BLUE]; }
	const ChannelT<T>*			getChannelAlpha() const { return &mObj->mChannels[SurfaceChannelOrder::CHAN_ALPHA]; }

	//! Convenience method for getting a single pixel. For performance-sensitive code consider Surface::Iter instead.
	ColorAT<T>	getPixel( Vec2i pos ) const { pos.x = constrain<int32_t>( pos.x, 0, mObj->mWidth - 1); pos.y = constrain<int32_t>( pos.y, 0, mObj->mHeight - 1 ); const T *p = getData( pos ); return ColorAT<T>( p[getRedOffset()], p[getGreenOffset()], p[getBlueOffset()], ( hasAlpha() ) ? p[getAlphaOffset()] : CHANTRAIT<T>::max() ); }
	//! Convenience method for setting a single pixel. For performance-sensitive code consider SurfaceT<T>::Iter instead.
	void	setPixel( Vec2i pos, const ColorT<T> &c ) { pos.x = constrain<int32_t>( pos.x, 0, mObj->mWidth - 1); pos.y = constrain<int32_t>( pos.y, 0, mObj->mHeight - 1 ); T *p = getData( pos ); p[getRedOffset()] = c.r; p[getGreenOffset()] = c.g; p[getBlueOffset()] = c.b; }
	//! Convenience method for setting a single pixel. For performance-sensitive code consider SurfaceT<T>::Iter instead.
	void	setPixel( Vec2i pos, const ColorAT<T> &c ) { pos.x = constrain<int32_t>( pos.x, 0, mObj->mWidth - 1); pos.y = constrain<int32_t>( pos.y, 0, mObj->mHeight - 1 ); T *p = getData( pos ); p[getRedOffset()] = c.r; p[getGreenOffset()] = c.g; p[getBlueOffset()] = c.b; if( hasAlpha() ) p[getAlphaOffset()] = c.a; }

	void				copyFrom( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &relativeOffset = Vec2i::zero() );
//	template<typename T2>
//	void				copy( const SurfaceT<T2> &srcSurface, const Area &srcArea, const Offset &dstOffset = Offset::zero() );	

	ColorT<T>						areaAverage( const Area &area ) const;

	//@{
	//! Emulates shared_ptr-like behavior
	typedef shared_ptr<Obj> SurfaceT::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &SurfaceT::mObj; }
	void reset() { mObj.reset(); }
	//@}

 private:
	shared_ptr<Obj>		mObj;

	void init( shared_ptr<class ImageSource> imageSource, const SurfaceConstraints &constraints = SurfaceConstraintsDefault(), boost::tribool alpha = boost::logic::indeterminate );

	void	copyRawSameChannelOrder( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &absoluteOffset );
	void	copyRawRgba( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &absoluteOffset );
	void	copyRawRgb( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &absoluteOffset );
 
 public:
	class Iter {
	 public:
		Iter( SurfaceT<T> &SurfaceT, const Area &area ) 
			: mRedOff( SurfaceT.getRedOffset() ), mGreenOff( SurfaceT.getGreenOffset() ), 
				mBlueOff( SurfaceT.getBlueOffset() ), mAlphaOff( SurfaceT.getAlphaOffset() ),
				mInc( SurfaceT.getPixelInc() ), mRowInc( SurfaceT.getRowBytes() )
		{
			Area clippedArea( area.getClipBy( SurfaceT.getBounds() ) );
			mWidth = clippedArea.getWidth();
			mHeight = clippedArea.getHeight();
			mLinePtr = reinterpret_cast<uint8_t*>( SurfaceT.getData( clippedArea.getUL() ) );
			mPtr = reinterpret_cast<T*>( mLinePtr );
			mStartX = mX = clippedArea.getX1();
			mStartY = mY = clippedArea.getY1();
			mEndX = clippedArea.getX2();
			mEndY = clippedArea.getY2();
			// in order to be at the right place after an initial call to line(), we need to back up one line
			mY = clippedArea.getY1() - 1;
			mLinePtr -= mRowInc;
		}
		
		T&			r() const { return mPtr[mRedOff]; }
		T&			g() const { return mPtr[mGreenOff]; }
		T&			b() const { return mPtr[mBlueOff]; }	
		T&			a() const { return mPtr[mAlphaOff]; }

		T&			r( int32_t xOff, int32_t yOff ) const { return mPtr[mRedOff + xOff * mInc + yOff * mRowInc]; }
		T&			g( int32_t xOff, int32_t yOff ) const { return mPtr[mGreenOff + xOff * mInc + yOff * mRowInc]; }
		T&			b( int32_t xOff, int32_t yOff ) const { return mPtr[mBlueOff + xOff * mInc + yOff * mRowInc]; }	
		T&			a( int32_t xOff, int32_t yOff ) const { return mPtr[mAlphaOff + xOff * mInc + yOff * mRowInc]; }

		T&			rClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return mPtr[mRedOff + xOff * mInc + yOff * mRowInc]; }
		T&			gClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return mPtr[mGreenOff + xOff * mInc + yOff * mRowInc]; }
		T&			bClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return mPtr[mBlueOff + xOff * mInc + yOff * mRowInc]; }		
		T&			aClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return mPtr[mAlphaOff + xOff * mInc + yOff * mRowInc]; }
		
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
		
		int32_t		getWidth() const { return mWidth; }
		int32_t		getHeight() const { return mHeight; }

		uint8_t				mRedOff, mGreenOff, mBlueOff, mAlphaOff, mInc;
		uint8_t				*mLinePtr;
		T					*mPtr;
		int32_t				mRowInc, mWidth, mHeight;
		int32_t				mX, mY, mStartX, mStartY, mEndX, mEndY;
	};

	class ConstIter {
	 public:
		ConstIter( const Iter &iter ) {
			mRedOff = iter.mRedOff;
			mGreenOff = iter.mGreenOff;
			mBlueOff = iter.mBlueOff;
			mAlphaOff = iter.mAlphaOff;
			mInc = iter.mInc;		
			mRowInc = iter.mRowInc;
			mWidth = iter.mWidth;
			mHeight = iter.mHeight;
			mLinePtr = iter.mLinePtr;
			mPtr = iter.mPtr;
			mStartX = iter.mStartX;
			mX = iter.mX;			
			mStartY = iter.mStartY;
			mY = iter.mY;
			mEndX = iter.mEndX;
			mEndY = iter.mEndY;
		}
		
		ConstIter( const SurfaceT<T> &SurfaceT, const Area &area ) 
			: mRedOff( SurfaceT.getRedOffset() ), mGreenOff( SurfaceT.getGreenOffset() ), 
				mBlueOff( SurfaceT.getBlueOffset() ), mAlphaOff( SurfaceT.getAlphaOffset() ),
				mInc( SurfaceT.getPixelInc() ), mRowInc( SurfaceT.getRowBytes() )
		{
			Area clippedArea( area.getClipBy( SurfaceT.getBounds() ) );
			mWidth = clippedArea.getWidth();
			mHeight = clippedArea.getHeight();
			mLinePtr = reinterpret_cast<const uint8_t*>( SurfaceT.getData( clippedArea.getUL() ) );
			mPtr = reinterpret_cast<const T*>( mLinePtr );
			mStartX = mX = clippedArea.getX1();
			mStartY = mY = clippedArea.getY1();
			mEndX = clippedArea.getX2();
			mEndY = clippedArea.getY2();
			// in order to be at the right place after an initial call to line(), we need to back up one line
			mY = clippedArea.getY1() - 1;
			mLinePtr -= mRowInc;
		}
		
		const T&	r() const { return mPtr[mRedOff]; }
		const T&	g() const { return mPtr[mGreenOff]; }
		const T&	b() const { return mPtr[mBlueOff]; }	
		const T&	a() const { return mPtr[mAlphaOff]; }

		const T&			r( int32_t xOff, int32_t yOff ) const { return mPtr[mRedOff + xOff * mInc + yOff * mRowInc]; }
		const T&			g( int32_t xOff, int32_t yOff ) const { return mPtr[mGreenOff + xOff * mInc + yOff * mRowInc]; }
		const T&			b( int32_t xOff, int32_t yOff ) const { return mPtr[mBlueOff + xOff * mInc + yOff * mRowInc]; }	
		const T&			a( int32_t xOff, int32_t yOff ) const { return mPtr[mAlphaOff + xOff * mInc + yOff * mRowInc]; }

		const T&	rClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return mPtr[mRedOff + xOff * mInc + yOff * mRowInc]; }
		const T&	gClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return mPtr[mGreenOff + xOff * mInc + yOff * mRowInc]; }
		const T&	bClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return mPtr[mBlueOff + xOff * mInc + yOff * mRowInc]; }		
		const T&	aClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return mPtr[mAlphaOff + xOff * mInc + yOff * mRowInc]; }
		
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
		
		int32_t		getWidth() const { return mWidth; }
		int32_t		getHeight() const { return mHeight; }

		uint8_t				mRedOff, mGreenOff, mBlueOff, mAlphaOff, mInc;
		const uint8_t		*mLinePtr;
		const T				*mPtr;
		int32_t				mRowInc, mWidth, mHeight;
		int32_t				mX, mY, mStartX, mStartY, mEndX, mEndY;
	};

	Iter		getIter() { return Iter( *this, this->getBounds() ); }
	Iter		getIter( const Area &area ) { return Iter( *this, area ); }
	ConstIter	getIter() const { return ConstIter( *this, this->getBounds() ); }
	ConstIter	getIter( const Area &area ) const { return ConstIter( *this, area ); }
};

class SurfaceExc : public std::exception {
	virtual const char* what() const throw() {
		return "Surface exception";
	}
};

class SurfaceConstraintsExc : public SurfaceExc {
	virtual const char* what() const throw() {
		return "Surface exception: does not conform to expected SurfaceConstraints";
	}
};

typedef SurfaceT<uint8_t> Surface;
typedef SurfaceT<uint8_t> Surface8u;
typedef SurfaceT<float> Surface32f;

} // namespace cinder
