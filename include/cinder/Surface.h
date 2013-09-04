/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.
 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

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
#include "cinder/Channel.h"
#include "cinder/ChanTraits.h"
#include "cinder/Color.h"
#include "cinder/FileSystem.h"

#include <boost/logic/tribool.hpp>

namespace cinder {

//! Specifies the in-memory ordering of the channels of a Surface.
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

typedef std::shared_ptr<class ImageSource> ImageSourceRef;
typedef std::shared_ptr<class ImageTarget> ImageTargetRef;

template<typename T>
//! An in-memory representation of an image. \ImplShared
class SurfaceT {
 private:
	/// \cond
	struct Obj {
		Obj( int32_t aWidth, int32_t aHeight, SurfaceChannelOrder aChannelOrder, T *aData, bool aOwnsData, int32_t aRowBytes );
		~Obj();
		
		void		initChannels();
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
	/*! Constructs an empty Surface, which is the equivalent of NULL and should not be used directly. */
	SurfaceT() {}
	/*! \brief Creates a Surface object that is \a height and \a width pixels, an alpha channel based on \a alpha, and the channel order \a channelOrder
	 
	 As an example, the following would create a 640x480 pixel surface with an alpha channel, and an RGBA channel order
	 <tt>Surface32f mySurface( 640, 480, true, SurfaceChannelOrder::RGBA );</tt>
	*/
	SurfaceT( int32_t width, int32_t height, bool alpha, SurfaceChannelOrder channelOrder = SurfaceChannelOrder::UNSPECIFIED );
	SurfaceT( int32_t width, int32_t height, bool alpha, const SurfaceConstraints &constraints );
	//! Constructs a surface from the memory pointed to by \a data. Does not assume ownership of the memory in \a data, which consequently should not be freed while the Surface is still in use.
	SurfaceT( T *data, int32_t width, int32_t height, int32_t rowBytes, SurfaceChannelOrder channelOrder );
	/*! \brief Creates a Surface object from an ImageSource, for instance from the result of a loadImage() call
	 
	 To load an image from a resource, pass the result of the loadImage() call to the Surface constructor as shown below
	 <tt>Surface mySurface = Surface( loadImage( loadResource( RES ) );</tt>
	 */
	SurfaceT( ImageSourceRef imageSource, const SurfaceConstraints &constraints = SurfaceConstraintsDefault(), boost::tribool alpha = boost::logic::indeterminate );

#if defined( CINDER_WINRT )
	/** \brief Constructs asynchronously a Surface from an images located at \a path. The loaded Surface is returned in \a surface.
		If you are creating a Surface from an image that is located outside of the WinRT Windows Store App folder, you must use this method.
	**/
	static void loadImageAsync(const fs::path path, SurfaceT &surface, const SurfaceConstraints &constraints = SurfaceConstraintsDefault(), boost::tribool alpha = boost::logic::indeterminate );
#endif

	operator ImageSourceRef() const;
	operator ImageTargetRef();

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

	//! Retuns the raw data of an image as a pointer to either uin8t_t values in the case of a Surface8u or floats in the case of a Surface32f
	T*					getData() { return mObj->mData; }
	const T*			getData() const { return mObj->mData; }
	T*					getData( const Vec2i &offset ) { return reinterpret_cast<T*>( reinterpret_cast<unsigned char*>( mObj->mData + offset.x * getPixelInc() ) + offset.y * mObj->mRowBytes ); }
	const T*			getData( const Vec2i &offset ) const { return reinterpret_cast<T*>( reinterpret_cast<unsigned char*>( mObj->mData + offset.x * getPixelInc() ) + offset.y * mObj->mRowBytes ); }
	//! Returns a pointer to the red channel data of the pixel located at \a offset. Result is a uint8_t* for Surface8u and a float* for Surface32f.
	T*					getDataRed( const Vec2i &offset ) { return getData( offset ) + getRedOffset(); }
	const T*			getDataRed( const Vec2i &offset ) const { return getData( offset ) + getRedOffset(); }
	//! Returns a pointer to the green channel data of the pixel located at \a offset. Result is a uint8_t* for Surface8u and a float* for Surface32f.
	T*					getDataGreen( const Vec2i &offset ) { return getData( offset ) + getGreenOffset(); }
	const T*			getDataGreen( const Vec2i &offset ) const { return getData( offset ) + getGreenOffset(); }
	//! Returns a pointer to the blue channel data of the pixel located at \a offset. Result is a uint8_t* for Surface8u and a float* for Surface32f.
	T*					getDataBlue( const Vec2i &offset ) { return getData( offset ) + getBlueOffset(); }
	const T*			getDataBlue( const Vec2i &offset ) const { return getData( offset ) + getBlueOffset(); }
	//! Returns a pointer to the alpha channel data of the pixel located at \a offset. Result is a uint8_t* for Surface8u and a float* for Surface32f. Undefined for Surfaces without an alpha channel.
	T*					getDataAlpha( const Vec2i &offset ) { return getData( offset ) + getAlphaOffset(); }
	const T*			getDataAlpha( const Vec2i &offset ) const { return getData( offset ) + getAlphaOffset(); }
	
	/** Sets the deallocator, an optional callback which will fire upon the Surface::Obj's destruction. This is useful when a Surface is wrapping another API's image data structure whose lifetime is tied to the Surface's. **/
	void				setDeallocator( void(*aDeallocatorFunc)( void * ), void *aDeallocatorRefcon );
	
	//! Returns the channel order of the Surface, the in-memory ordering of the channels of each pixel
	const SurfaceChannelOrder&	getChannelOrder() const { return mObj->mChannelOrder; }
	//! Returns the in-memory offset relative to a pixel for the red data. For example, for RGBA, returns 0.
	uint8_t						getRedOffset() const { return mObj->mChannelOrder.getRedOffset(); }
	//! Returns the in-memory offset relative to a pixel for the red data. For example, for RGBA, returns 1.
	uint8_t						getGreenOffset() const { return mObj->mChannelOrder.getGreenOffset(); }
	//! Returns the in-memory offset relative to a pixel for the red data. For example, for RGBA, returns 2.
	uint8_t						getBlueOffset() const { return mObj->mChannelOrder.getBlueOffset(); }
	//! Returns the in-memory offset relative to a pixel for the red data. For example, for RGBA, returns 3. Returns SurfaceChannelOrder::INVALID in the absence of an alpha channel.
	uint8_t						getAlphaOffset() const { return mObj->mChannelOrder.getAlphaOffset(); }
	//! Sets the channel order of the Surface, the in-memory ordering of the channels of each pixel. Call does not modify any pixel data, but does modify its interpretation. 
	void						setChannelOrder( const SurfaceChannelOrder &aChannelOrder );

	//! Returns a reference to a Channel \a channelIndex indexed according to how the channels are arranged per the SurfaceChannelOrder.
	ChannelT<T>&				getChannel( uint8_t channelIndex ) { return mObj->mChannels[channelIndex]; }
	//! Returns a const reference to a Channel \a channelIndex indexed  according to how the channels are arranged per the SurfaceChannelOrder.
	const ChannelT<T>&			getChannel( uint8_t channelIndex ) const { return mObj->mChannels[channelIndex]; }
	
	/*! Returns a reference to the red Channel of the Surface */
	ChannelT<T>&		getChannelRed() { return mObj->mChannels[SurfaceChannelOrder::CHAN_RED]; }
	/*! Returns a reference to the green Channel of the Surface */
	ChannelT<T>&		getChannelGreen() { return mObj->mChannels[SurfaceChannelOrder::CHAN_GREEN]; }
	/*! Returns a reference to the blue Channel of the Surface */
	ChannelT<T>&		getChannelBlue() { return mObj->mChannels[SurfaceChannelOrder::CHAN_BLUE]; }
	/*! Returns a reference to the alpha Channel of the Surface. Undefined in the absence of an alpha channel. */
	ChannelT<T>&		getChannelAlpha() { return mObj->mChannels[SurfaceChannelOrder::CHAN_ALPHA]; }

	/*! Returns a const reference to the red Channel of the Surface */
	const ChannelT<T>&	getChannelRed() const { return mObj->mChannels[SurfaceChannelOrder::CHAN_RED]; }
	/*! Returns a const reference to the green Channel of the Surface */	
	const ChannelT<T>&	getChannelGreen() const { return mObj->mChannels[SurfaceChannelOrder::CHAN_GREEN]; }
	/*! Returns a const reference to the blue Channel of the Surface */	
	const ChannelT<T>&	getChannelBlue() const { return mObj->mChannels[SurfaceChannelOrder::CHAN_BLUE]; }
	/*! Returns a const reference to the alpha Channel of the Surface. Undefined in the absence of an alpha channel. */	
	const ChannelT<T>&	getChannelAlpha() const { return mObj->mChannels[SurfaceChannelOrder::CHAN_ALPHA]; }

	//! Convenience method for getting a single pixel. For performance-sensitive code consider \ref SurfaceT::Iter "Surface::Iter" instead.
	ColorAT<T>	getPixel( Vec2i pos ) const { pos.x = constrain<int32_t>( pos.x, 0, mObj->mWidth - 1); pos.y = constrain<int32_t>( pos.y, 0, mObj->mHeight - 1 ); const T *p = getData( pos ); return ColorAT<T>( p[getRedOffset()], p[getGreenOffset()], p[getBlueOffset()], ( hasAlpha() ) ? p[getAlphaOffset()] : CHANTRAIT<T>::max() ); }
	//! Convenience method for setting a single pixel. For performance-sensitive code consider \ref SurfaceT::Iter "Surface::Iter" instead.
	void	setPixel( Vec2i pos, const ColorT<T> &c ) { pos.x = constrain<int32_t>( pos.x, 0, mObj->mWidth - 1); pos.y = constrain<int32_t>( pos.y, 0, mObj->mHeight - 1 ); T *p = getData( pos ); p[getRedOffset()] = c.r; p[getGreenOffset()] = c.g; p[getBlueOffset()] = c.b; }
	//! Convenience method for setting a single pixel. For performance-sensitive code consider \ref SurfaceT::Iter "Surface::Iter" instead.
	void	setPixel( Vec2i pos, const ColorAT<T> &c ) { pos.x = constrain<int32_t>( pos.x, 0, mObj->mWidth - 1); pos.y = constrain<int32_t>( pos.y, 0, mObj->mHeight - 1 ); T *p = getData( pos ); p[getRedOffset()] = c.r; p[getGreenOffset()] = c.g; p[getBlueOffset()] = c.b; if( hasAlpha() ) p[getAlphaOffset()] = c.a; }

	//! Copies the Area \a srcArea of the Surface \a srcSurface to \a this Surface. The destination Area is \a srcArea offset by \a relativeOffset.
	void	copyFrom( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &relativeOffset = Vec2i::zero() );

	//! Returns an averaged color for the Area defined by \a area
	ColorT<T>	areaAverage( const Area &area ) const;

	/// \cond
	typedef std::shared_ptr<Obj> SurfaceT::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &SurfaceT::mObj; }
	void reset() { mObj.reset(); }
	/// \endcond

 private:
	std::shared_ptr<Obj>		mObj;

	void init( ImageSourceRef imageSource, const SurfaceConstraints &constraints = SurfaceConstraintsDefault(), boost::tribool alpha = boost::logic::indeterminate );

	void	copyRawSameChannelOrder( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &absoluteOffset );
	void	copyRawRgba( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &absoluteOffset );
	void 	copyRawRgbFullAlpha( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &absoluteOffset );
	void	copyRawRgb( const SurfaceT<T> &srcSurface, const Area &srcArea, const Vec2i &absoluteOffset );
 
 public:
	
	//! Convenience class for iterating the pixels of a Surface.
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
		/*! Returns a reference to the red value of the pixel that the Iter currently points to */
		T&			r() const { return mPtr[mRedOff]; }
		/*! Returns a reference to the green value of the pixel that the Iter currently points to */
		T&			g() const { return mPtr[mGreenOff]; }
		/*! Returns a reference to the blue value of the pixel that the Iter currently points to */
		T&			b() const { return mPtr[mBlueOff]; }
		/*! Returns a reference to the alpha value of the pixel that the Iter currently points to. Undefined in the absence of an alpha channel. */
		T&			a() const { return mPtr[mAlphaOff]; }

		//! Returns a reference to the red value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels
		T&			r( int32_t xOff, int32_t yOff ) const { return mPtr[mRedOff + xOff * mInc + yOff * mRowInc]; }
		//! Returns a reference to the green value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels
		T&			g( int32_t xOff, int32_t yOff ) const { return mPtr[mGreenOff + xOff * mInc + yOff * mRowInc]; }
		//! Returns a reference to the blue value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels
		T&			b( int32_t xOff, int32_t yOff ) const { return mPtr[mBlueOff + xOff * mInc + yOff * mRowInc]; }	
		//! Returns a reference to the alpha value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels. Undefined in the absence of an alpha channel.
		T&			a( int32_t xOff, int32_t yOff ) const { return mPtr[mAlphaOff + xOff * mInc + yOff * mRowInc]; }

		//! Returns a reference to the red value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels. Clamps offset to the bounds of the Iter.
		T&			rClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return *(T*)((uint8_t*)( mPtr + mRedOff + xOff * mInc ) + yOff * mRowInc); }
		//! Returns a reference to the green value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels. Clamps offset to the bounds of the Iter.
		T&			gClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return *(T*)((uint8_t*)( mPtr + mGreenOff + xOff * mInc ) + yOff * mRowInc); }
		//! Returns a reference to the blue value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels. Clamps offset to the bounds of the Iter.							
		T&			bClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return *(T*)((uint8_t*)( mPtr + mBlueOff + xOff * mInc ) + yOff * mRowInc); }
		//! Returns a reference to the alpha value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels. Clamps offset to the bounds of the Iter. Undefined in the absence of an alpha channel.
		T&			aClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return *(T*)((uint8_t*)( mPtr + mAlphaOff + xOff * mInc ) + yOff * mRowInc); }

		//! Returns the x coordinate of the pixel the Iter currently points to
		const int32_t	x() const { return mX; }
		//! Returns the y coordinate of the pixel the Iter currently points to
		const int32_t	y() const { return mY; }
		//! Returns the coordinate of the pixel the Iter currently points to
		Vec2i			getPos() const { return Vec2i( mX, mY ); }

		//! Increments which pixel of the current row the Iter points to, and returns \c false when no pixels remain in the current row.
		bool pixel() {
			++mX;
			mPtr += mInc;
			return mX < mEndX;
		}
		
		//! Increments which row the Iter points to, and returns \c false when no rows remain in the Surface.
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
		int32_t		getWidth() const { return mWidth; }
		//! Returns the height of the Area the Iter iterates
		int32_t		getHeight() const { return mHeight; }

		/// \cond
		uint8_t				mRedOff, mGreenOff, mBlueOff, mAlphaOff, mInc;
		uint8_t				*mLinePtr;
		T					*mPtr;
		int32_t				mRowInc, mWidth, mHeight;
		int32_t				mX, mY, mStartX, mStartY, mEndX, mEndY;
		/// \endcond
	};

	//! Convenience class for iterating the pixels of a Surface. The iteration is \c const, performing read-only operations on the Surface.
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
		
		//! Returns a reference to the red value of the pixel that the Iter currently points to
		const T&	r() const { return mPtr[mRedOff]; }
		//! Returns a reference to the green value of the pixel that the Iter currently points to
		const T&	g() const { return mPtr[mGreenOff]; }
		//! Returns a reference to the blue value of the pixel that the Iter currently points to		
		const T&	b() const { return mPtr[mBlueOff]; }	
		//! Returns a reference to the alpha value of the pixel that the Iter currently points to. Undefined in the absence of an alpha channel.
		const T&	a() const { return mPtr[mAlphaOff]; }

		//! Returns a reference to the red value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels
		const T&	r( int32_t xOff, int32_t yOff ) const { return mPtr[mRedOff + xOff * mInc + yOff * mRowInc]; }
		//! Returns a reference to the green value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels
		const T&	g( int32_t xOff, int32_t yOff ) const { return mPtr[mGreenOff + xOff * mInc + yOff * mRowInc]; }
		//! Returns a reference to the blue value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels		
		const T&	b( int32_t xOff, int32_t yOff ) const { return mPtr[mBlueOff + xOff * mInc + yOff * mRowInc]; }	
		//! Returns a reference to the alpha value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels		
		const T&	a( int32_t xOff, int32_t yOff ) const { return mPtr[mAlphaOff + xOff * mInc + yOff * mRowInc]; }

		//! Returns the red value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels. Clamps offset to the bounds of the Iter.
		const T&	rClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return *(T*)((uint8_t*)( mPtr + mRedOff + xOff * mInc ) + yOff * mRowInc); }
		//! Returns the green value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels. Clamps offset to the bounds of the Iter.
		const T&	gClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return *(T*)((uint8_t*)( mPtr + mGreenOff + xOff * mInc ) + yOff * mRowInc); }
		//! Returns the blue value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels. Clamps offset to the bounds of the Iter.
		const T&	bClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return *(T*)((uint8_t*)( mPtr + mBlueOff + xOff * mInc ) + yOff * mRowInc); }
		//! Returns the alpha value of the pixel that the Iter currently points to, offset by (\a xOff, \a yOff) pixels. Clamps offset to the bounds of the Iter. Undefined in the absence of an alpha channel.
		const T&	aClamped( int32_t xOff, int32_t yOff ) const
						{	xOff = std::min(std::max(mX + xOff, mStartX),mEndX - 1) - mX; yOff = std::min(std::max( mY + yOff, mStartY ), mEndY - 1) - mY;
							return *(T*)((uint8_t*)( mPtr + mAlphaOff + xOff * mInc ) + yOff * mRowInc); }
		
		//! Returns the x coordinate of the pixel the Iter currently points to
		const int32_t	x() const { return mX; }
			//! Returns the t coordinate of the pixel the Iter currently points to		
		const int32_t	y() const { return mY; }
		//! Returns the coordinate of the pixel the Iter currently points to		
		Vec2i			getPos() const { return Vec2i( mX, mY ); }

		//! Increments which pixel of the current row the Iter points to, and returns \c false when no pixels remain in the current row.
		bool pixel() {
			++mX;
			mPtr += mInc;
			return mX < mEndX;
		}
		
		//! Increments which row the Iter points to, and returns \c false when no rows remain in the Surface.
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
		int32_t		getWidth() const { return mWidth; }
		//! Returns the height of the Area the Iter iterates
		int32_t		getHeight() const { return mHeight; }

		/// \cond
		uint8_t				mRedOff, mGreenOff, mBlueOff, mAlphaOff, mInc;
		const uint8_t		*mLinePtr;
		const T				*mPtr;
		int32_t				mRowInc, mWidth, mHeight;
		int32_t				mX, mY, mStartX, mStartY, mEndX, mEndY;
		/// \endcond
	};

	//! Returns an Iter which iterates the entire Surface.
	Iter		getIter() { return Iter( *this, this->getBounds() ); }
	//! Returns an Iter which iterates the Area \a area.
	Iter		getIter( const Area &area ) { return Iter( *this, area ); }
	//! Returns a ConstIter which iterates the entire Surface.
	ConstIter	getIter() const { return ConstIter( *this, this->getBounds() ); }
	//! Returns a ConstIter which iterates the Area \a area.
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

//! 8-bit image. Synonym for Surface8u.
typedef SurfaceT<uint8_t> Surface;
//! 8-bit image
typedef SurfaceT<uint8_t> Surface8u;	
//! 16-bit image. Suitable as an intermediate representation and ImageIo but not a first-class citizen.
typedef SurfaceT<uint16_t> Surface16u;
//! 32-bit floating point image
typedef SurfaceT<float> Surface32f;

} // namespace cinder
