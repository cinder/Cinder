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
#include "cinder/Surface.h"
#include "cinder/Shape2d.h"
#include "cinder/Color.h"
#include "cinder/Buffer.h"
#include "cinder/ImageIo.h"
namespace cinder {
	class Url;
	class Font;
}

#if defined( CINDER_COCOA_TOUCH )
	#include <CoreGraphics/CoreGraphics.h>
#else
	#include <ApplicationServices/ApplicationServices.h>
#endif
#include <CoreFoundation/CoreFoundation.h>
#if defined( __OBJC__ )
	@class NSBitmapImageRep;
	@class NSString;
	@class NSData;
#else
	class NSBitmapImageRep;
	class NSString;
	class NSData;	
#endif

namespace cinder { namespace cocoa {

typedef std::shared_ptr<struct __CFString> SafeCfString;

//! Represents an exception-safe Cocoa NSString which behaves like a shared_ptr but can implicitly cast itself to NSString*
class SafeNsString {
  public:
	SafeNsString() {}
	//! Creates a SafeNsString using an existing NSString. This constructor automatically increments the retain count.
	SafeNsString( NSString *str );
	//! Creates a SafeNsString by converting a std::string.
	SafeNsString( const std::string &str );
	
	operator NSString* const() { if( mPtr ) return mPtr.get(); else return 0; }
	operator std::string() const;
	
  private:
	static void safeRelease( NSString *ptr );
	
	std::shared_ptr<NSString>	mPtr;
};

//! Represents an exception-safe Cocoa NSData which behaves like a shared_ptr but can implicitly cast itself to NSData*
class SafeNsData {
  public:
	SafeNsData() {}
	//! Creates a SafeNsData using an existing cinder::Buffer. The SafeNsData retains a copy of the buffer in order to prevent its deletion
	SafeNsData( const Buffer &buffer );
	
	operator NSData* const() { if( mPtr ) return mPtr.get(); else return 0; }
	
  private:
	static void safeRelease( const NSData *ptr );
	
	std::shared_ptr<NSData>	mPtr;
	const Buffer			mBuffer;
};

//! Represents an exception-safe NSAutoreleasePool. Replaces the global NSAutoreleasePool for its lifetime
class SafeNsAutoreleasePool {
  public:
	SafeNsAutoreleasePool();
	~SafeNsAutoreleasePool();
	
  private:
	void			*mPool;
};

//! Safely release a CoreFoundation object, testing for null before calling CFRelease. Designed to be used as the deleter of a shared_ptr.
void safeCfRelease( const CFTypeRef cfRef );

//! Safely release a Cocoa object, testing for null before calling [nsObject release]. Designed to be used as the deleter of a shared_ptr.
void safeCocoaRelease( void *nsObject );

/** \brief Creates a CGBitmapContext that represents a cinder::Surface8u. Users must call CGContextRelease() to free the result.
	\note CGBitmapContexts only support premultiplied alpha **/
CGContextRef createCgBitmapContext( const Surface8u &surface );

//! Returns the current CoreGraphics context for the active window. Requires the current Renderer to be a Renderer2d. Does not need to be released.
CGContextRef getWindowContext();

#if defined( CINDER_MAC )
/** \brief Converts an NSBitmapImageRep into a cinder::Surface8u
	If \a assumeOwnership the result will take ownership of \a rep and will destroy it upon the Surface's own destruction automatically.
	\return a Surface8u which wraps the NSBitmapImageRep but does not own the data. **/
Surface8u convertNsBitmapDataRep( const NSBitmapImageRep *rep, bool assumeOwnership = false );
#endif

//! Converts a CFStringRef into std::string with UTF8 encoding.
std::string convertCfString( CFStringRef str );
//! Converts a std::string into a CFStringRef. Assumes UTF8 encoding. User must call CFRelease() to free the result.
CFStringRef	createCfString( const std::string &str );
//! Converts a std::string into an exception-safe CFString pointer. Assumes UTF8 encoding. The deleter is set to free the string when appropriate.
SafeCfString createSafeCfString( const std::string &str );
//! Converts a NSString into a std::string with UTF8 encoding.
std::string	convertNsString( NSString *str );
//! Converts a cinder::URL into a CFURLRef. User must call CFRelease() to free the result.
CFURLRef createCfUrl( const cinder::Url &url );

//! Converts a std::string to a CFAttributedStringRef with attributes set for \a font and \a color. Assumes UTF8 encoding. User must call CFRelease() to free the result. Returns NULL on failure.
CFAttributedStringRef createCfAttributedString( const std::string &str, const cinder::Font &font, const ColorA &color );
//! Converts a std::string to a CFAttributedStringRef with attributes set for \a font and \a color. If \a ligate then ligatures will be used. Assumes UTF8 encoding. User must call CFRelease() to free the result. Returns NULL on failure.
CFAttributedStringRef createCfAttributedString( const std::string &str, const cinder::Font &font, const ColorA &color, bool ligate );

//! Converts a cinder::Color to CGColor. User must call CGColorRelease() to free the result.
CGColorRef createCgColor( const Color &color );
//! Converts a cinder::ColorA to CGColor. User must call CGColorRelease() to free the result.
CGColorRef createCgColor( const ColorA &color );

//! Converts a cinder::Area to a CGRect.
CGRect createCgRect( const Area &area );
//! Converts a CGRect to a cinder::Area
Area CgRectToArea( const CGRect &rect );

//! Creates a Cocoa CGSize from a cinder::Vec2i
inline CGSize createCgSize( const Vec2i &s ) { CGSize result; result.width = s.x; result.height = s.y; return result; }
//! Creates a Cocoa CGSize from a cinder::Vec2f
inline CGSize createCgSize( const Vec2f &s ) { CGSize result; result.width = s.x; result.height = s.y; return result; }

//! Converts a CGPathRef to a cinder::Shape2d. If \a flipVertical then the path will be flipped vertically.
void convertCgPath( CGPathRef cgPath, Shape2d *resultShape, bool flipVertical = true );

#if defined( CINDER_MAC )
/** \brief Translates a cinder::SurfaceChannelOrder into the pixel formats of a CoreVideo buffer
	\return -1 on failure, else a constant as defined in <CoreVideo/CVPixelBuffer.h> **/
int getCvPixelFormatTypeFromSurfaceChannelOrder( const SurfaceChannelOrder &sco );
#endif

//! Creates a CFDataRef from a cinder::Buffer \a buffer. The result does not assume ownership of the data and should be freed using CFRelease().
CFDataRef createCfDataRef( const cinder::Buffer &buffer );


typedef std::shared_ptr<class ImageSourceCgImage> ImageSourceCgImageRef;

class ImageSourceCgImage : public ImageSource {
  public:
	//! Retains (and later releases) \a imageRef
	static ImageSourceCgImageRef	createRef( ::CGImageRef imageRef, ImageSource::Options options = ImageSource::Options() );
	virtual ~ImageSourceCgImage() {}

	virtual void	load( ImageTargetRef target );

  protected:
	//! Retains (and later releases) \a imageRef
	ImageSourceCgImage( ::CGImageRef imageRef, ImageSource::Options options );

	bool						mIsIndexed, mIs16BitPacked;
	Color8u						mColorTable[256];
	std::shared_ptr<CGImage>	mImageRef;

	uint16_t					m16BitPackedRedOffset, m16BitPackedGreenOffset, m16BitPackedBlueOffset;
};

ImageSourceCgImageRef createImageSource( ::CGImageRef imageRef, ImageSource::Options = ImageSource::Options() );


typedef std::shared_ptr<class ImageTargetCgImage> ImageTargetCgImageRef;

class ImageTargetCgImage : public ImageTarget {
  public:
	static ImageTargetCgImageRef createRef( ImageSourceRef imageSource, ImageTarget::Options options );
	~ImageTargetCgImage();

	virtual void*	getRowPointer( int32_t row );
	virtual void	finalize();

	::CGImageRef	getCgImage() const { return mImageRef; }

  protected:
	ImageTargetCgImage( ImageSourceRef imageSource, ImageTarget::Options options );

	::CGImageRef		mImageRef;
	size_t				mBitsPerComponent, mBitsPerPixel, mRowBytes;
	::CGBitmapInfo		mBitmapInfo;
	::CFMutableDataRef	mDataRef;
	uint8_t				*mDataPtr;
};

//! Loads an ImageSource into a new CGImageRef. Release the result with ::CGImageRelease.
::CGImageRef createCgImage( ImageSourceRef imageSource, ImageTarget::Options = ImageTarget::Options() );


} } // namespace cinder::cocoa

namespace cinder {

//! \cond
class SurfaceConstraintsCgBitmapContext : public cinder::SurfaceConstraints {
 public:
	virtual SurfaceChannelOrder getChannelOrder( bool alpha ) const;
	virtual int32_t				getRowBytes( int requestedWidth, const SurfaceChannelOrder &sco, int elementSize ) const;
};
//! \endcond

} // namespace cinder
