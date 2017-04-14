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

#import "cinder/cocoa/CinderCocoa.h"
#include "cinder/Url.h"
#include "cinder/Buffer.h"
#include "cinder/Font.h"

#if defined( CINDER_MAC )
	#import <Cocoa/Cocoa.h>
	#import <CoreVideo/CVPixelBuffer.h>
	#import <AppKit/AppKit.h>
	#import <ApplicationServices/ApplicationServices.h>
	#include <objc/objc-auto.h>
#else
	#import <UIKit/UIKit.h>
	#import <CoreText/CoreText.h>
	#import <CoreVideo/CVPixelBuffer.h>
#endif
#import <Foundation/NSData.h>

using namespace std;

namespace cinder { namespace cocoa {

SafeNsString::SafeNsString( NSString *str )
{
	[str retain];
	mPtr = shared_ptr<NSString>( str, safeRelease );
}

SafeNsString::SafeNsString( const std::string &str )
{
	mPtr = shared_ptr<NSString>( [NSString stringWithUTF8String:str.c_str()], safeRelease );
	[mPtr.get() retain];
}

void SafeNsString::safeRelease( NSString *ptr )
{
	if( ptr )
		[ptr release];
}

SafeNsString::operator std::string() const
{
	if( ! mPtr )
		return std::string();
	else
		return std::string( [mPtr.get() UTF8String] );
}

SafeNsData::SafeNsData( const BufferRef &buffer )
	: mBuffer( buffer )
{
	mPtr = shared_ptr<NSData>( [NSData dataWithBytesNoCopy:const_cast<void*>( buffer->getData() ) length:buffer->getSize() freeWhenDone:NO], safeRelease );
	if( mPtr.get() )
		[mPtr.get() retain];
}

void SafeNsData::safeRelease( const NSData *ptr )
{
	if( ptr )
		[ptr release];
}

SafeNsAutoreleasePool::SafeNsAutoreleasePool()
{
	[NSThread currentThread]; // register this thread with garbage collection
	mPool = [[NSAutoreleasePool alloc] init];
}

SafeNsAutoreleasePool::~SafeNsAutoreleasePool()
{
	[((NSAutoreleasePool*)mPool) drain];
}

void safeCfRelease( const CFTypeRef cfRef )
{
	if( cfRef != NULL )
		::CFRelease( cfRef );
}

void safeCocoaRelease( void *nsObject )
{
	if( nsObject )
		[(NSObject*)nsObject release];
}

CGContextRef createCgBitmapContext( const Surface8u &surface )
{
	// See the enumeration of Supported Pixel Formats in the Quartz 2D Programming Guide
	// http://developer.apple.com/mac/library/documentation/GraphicsImaging/Conceptual/drawingwithquartz2d/dq_context/dq_context.html#//apple_ref/doc/uid/TP30001066-CH203-BCIBHHBB
	// Sadly, unpremultipllied alpha is not amongst them
	CGColorSpaceRef colorSpace = ::CGColorSpaceCreateDeviceRGB();
	CGImageAlphaInfo alphaInfo;
	switch( surface.getChannelOrder().getCode() ) {
		case SurfaceChannelOrder::RGBA:
			alphaInfo = kCGImageAlphaPremultipliedLast;
		break;
		case SurfaceChannelOrder::ARGB:
			alphaInfo = kCGImageAlphaPremultipliedFirst;
		break;
		case SurfaceChannelOrder::RGBX:
			alphaInfo = kCGImageAlphaNoneSkipLast;
		break;
		case SurfaceChannelOrder::XRGB:
			alphaInfo = kCGImageAlphaNoneSkipFirst;
		break;
		// CGBitmapContextCreate cannont handle this option despite the existence of this constant
		/*case SurfaceChannelOrder::RGB:
			alphaInfo = kCGImageAlphaNone;
		break;*/
		default:
			throw;
	}
	CGContextRef context = CGBitmapContextCreate( const_cast<uint8_t*>( surface.getData() ), surface.getWidth(), surface.getHeight(), 8, surface.getRowBytes(), colorSpace, alphaInfo );
	CGColorSpaceRelease( colorSpace );
	return context;
}

CGContextRef getWindowContext()
{
#if defined( CINDER_MAC )
	return (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
#else
	return ::UIGraphicsGetCurrentContext();
#endif
}

#if defined( CINDER_MAC )
Surface8uRef convertNsBitmapDataRep( const NSBitmapImageRep *rep, bool assumeOwnership )
{
	int32_t rowBytes = (int32_t)[rep bytesPerRow];
	int32_t width = (int32_t)[rep pixelsWide];
	int32_t height = (int32_t)[rep pixelsHigh];
	uint8_t *data = [rep bitmapData];
	SurfaceChannelOrder co = SurfaceChannelOrder::RGB;
	if( [rep bitsPerPixel] == 32 )
		co = ( [rep bitmapFormat] & NSAlphaFirstBitmapFormat ) ? SurfaceChannelOrder::ARGB : SurfaceChannelOrder::RGBA;

	// If requested, point the result's deallocator to the appropriate function. This will get called when the Surface::Obj is destroyed
	if( assumeOwnership )
		return Surface8uRef( new Surface8u( data, width, height, rowBytes, co ),
			[=] ( Surface8u *s ) { delete s; [rep release]; } );
	else
		return Surface8u::create( data, width, height, rowBytes, co );
}
#endif // defined( CINDER_MAC )

std::string convertCfString( CFStringRef str )
{
	char buffer[4096];
	Boolean worked = CFStringGetCString( str, buffer, 4095, kCFStringEncodingUTF8 );
	if( worked ) {
		std::string result( buffer );
		return result;
	}
	else
		return std::string();
}

CFStringRef	createCfString( const std::string &str )
{
	CFStringRef result = CFStringCreateWithCString( kCFAllocatorDefault, str.c_str(), kCFStringEncodingUTF8 );
	return result;
}

SafeCfString createSafeCfString( const std::string &str )
{
	CFStringRef result = CFStringCreateWithCString( kCFAllocatorDefault, str.c_str(), kCFStringEncodingUTF8 );
	if( result )
		return SafeCfString( (__CFString*)result, safeCfRelease );
	else
		return SafeCfString();
}

std::string	convertNsString( NSString *str )
{
	return std::string( [str UTF8String] );
}

CFURLRef createCfUrl( const Url &url )
{
	::CFStringRef pathString = createCfString( url.str() );
	::CFURLRef result = ::CFURLCreateWithString( kCFAllocatorDefault, pathString, NULL );
	::CFRelease( pathString );
	return result;
}

CFAttributedStringRef createCfAttributedString( const std::string &str, const Font &font, const ColorA &color )
{
	CGColorRef cgColor = createCgColor( color );
	const CFStringRef keys[] = {
		kCTFontAttributeName,
		kCTForegroundColorAttributeName
	};
	const CFTypeRef values[] = {
		font.getCtFontRef(),
		cgColor
	};
	
	// Create our attributes
	CFDictionaryRef attributes = ::CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys, (const void**)&values, sizeof(keys)/sizeof(keys[0]), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	assert( attributes != NULL );

	CGColorRelease( cgColor );
	
	// Create the attributed string
	CFStringRef strRef = CFStringCreateWithCString( kCFAllocatorDefault, str.c_str(), kCFStringEncodingUTF8 );
	if( ! strRef ) { // failure
		::CFRelease( attributes );
		return NULL;
	}
	CFAttributedStringRef attrString = ::CFAttributedStringCreate( kCFAllocatorDefault, strRef, attributes );
	
	CFRelease( strRef );
	CFRelease( attributes );
	
	return attrString;
}

CFAttributedStringRef createCfAttributedString( const std::string &str, const Font &font, const ColorA &color, bool ligate )
{
	CGColorRef cgColor = createCgColor( color );
	const int ligatures = ( ligate ) ? 1 : 0;
    CFNumberRef ligaturesRef = ::CFNumberCreate( kCFAllocatorDefault, kCFNumberIntType, &ligatures );
	const CFStringRef keys[] = {
		kCTFontAttributeName,
		kCTForegroundColorAttributeName,
		kCTLigatureAttributeName
	};
	const CFTypeRef values[] = {
		font.getCtFontRef(),
		cgColor,
		ligaturesRef
	};
	
	// Create our attributes
	CFDictionaryRef attributes = ::CFDictionaryCreate(kCFAllocatorDefault, (const void**)&keys, (const void**)&values, sizeof(keys)/sizeof(keys[0]), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	assert( attributes != NULL );

	CGColorRelease( cgColor );
	
	// Create the attributed string
	CFStringRef strRef = ::CFStringCreateWithCString( kCFAllocatorDefault, str.c_str(), kCFStringEncodingUTF8 );
	if( ! strRef ) { // failure
		::CFRelease( attributes );
		::CFRelease( ligaturesRef );
		return NULL;
	}
	CFAttributedStringRef attrString = ::CFAttributedStringCreate( kCFAllocatorDefault, strRef, attributes );
	
	::CFRelease( strRef );
	::CFRelease( attributes );
	::CFRelease( ligaturesRef );
	
	return attrString;
}

CGColorRef createCgColor( const Color &color )
{
	shared_ptr<CGColorSpace> safeColor( ::CGColorSpaceCreateDeviceRGB(), ::CGColorSpaceRelease );
	CGFloat components[4] = { color.r, color.g, color.b, 1 };
	return ::CGColorCreate( safeColor.get(), components );
}

CGColorRef createCgColor( const ColorA &color )
{
	shared_ptr<CGColorSpace> safeColor( ::CGColorSpaceCreateDeviceRGB(), ::CGColorSpaceRelease );
	CGFloat components[4] = { color.r, color.g, color.b, color.a };
	return ::CGColorCreate( safeColor.get(), components );
}

CGRect createCgRect( const Area &area )
{
	CGRect result;
	result.size.width = area.getWidth();
	result.size.height = area.getHeight();
	result.origin.x = area.getX1();
	result.origin.y = area.getY1();
	return result;
}

Area CgRectToArea( const CGRect &rect )
{
	return Area( rect.origin.x, rect.origin.y, rect.origin.x + rect.size.width, rect.origin.y + rect.size.height );
}

namespace { // anonymous namespace
extern "C" void cgPathApplierFunc( void *info, const CGPathElement *el )
{
	Shape2d *path = reinterpret_cast<Shape2d*>( info );

    switch( el->type ) {
		case kCGPathElementMoveToPoint:
			path->moveTo( el->points[0].x, el->points[0].y );
		break;
		case kCGPathElementAddLineToPoint:
			path->lineTo( el->points[0].x, el->points[0].y );
		break;
		case kCGPathElementAddQuadCurveToPoint:
			path->quadTo( el->points[0].x, el->points[0].y, el->points[1].x, el->points[1].y );
		break;
		case kCGPathElementAddCurveToPoint:
			path->curveTo( el->points[0].x, el->points[0].y, el->points[1].x, el->points[1].y, el->points[2].x, el->points[2].y );
		break;
		case kCGPathElementCloseSubpath:
			path->close();
		break;
	}
}

extern "C" void cgPathApplierFlippedFunc( void *info, const CGPathElement *el )
{
	Shape2d *path = reinterpret_cast<Shape2d*>( info );

    switch( el->type ) {
		case kCGPathElementMoveToPoint:
			path->moveTo( el->points[0].x, -el->points[0].y );
		break;
		case kCGPathElementAddLineToPoint:
			path->lineTo( el->points[0].x, -el->points[0].y );
		break;
		case kCGPathElementAddQuadCurveToPoint:
			path->quadTo( el->points[0].x, -el->points[0].y, el->points[1].x, -el->points[1].y );
		break;
		case kCGPathElementAddCurveToPoint:
			path->curveTo( el->points[0].x, -el->points[0].y, el->points[1].x, -el->points[1].y, el->points[2].x, -el->points[2].y );
		break;
		case kCGPathElementCloseSubpath:
			path->close();
		break;
	}
}
} // anonymous namespace

void convertCgPath( CGPathRef cgPath, cinder::Shape2d *resultShape, bool flipVertical )
{
	resultShape->clear();
	if( flipVertical )
		CGPathApply( cgPath, resultShape, cgPathApplierFlippedFunc );
	else
		CGPathApply( cgPath, resultShape, cgPathApplierFunc );
}

#if defined( CINDER_MAC )
int getCvPixelFormatTypeFromSurfaceChannelOrder( const SurfaceChannelOrder &sco )
{
	switch( sco.getCode() ) {
		case SurfaceChannelOrder::RGB:
			return kCVPixelFormatType_24RGB;
		break;
		case SurfaceChannelOrder::BGR:
			return kCVPixelFormatType_24BGR;
		break;
		case SurfaceChannelOrder::ARGB:
			return kCVPixelFormatType_32ARGB;
		break;
		case SurfaceChannelOrder::BGRA:
			return kCVPixelFormatType_32BGRA;
		break;
		case SurfaceChannelOrder::ABGR:
			return kCVPixelFormatType_32ABGR;
		break;
		case SurfaceChannelOrder::RGBA:
			return kCVPixelFormatType_32RGBA;
		break;
		default:
			return -1;
	}
}
#endif // defined( CINDER_MAC )

CFDataRef createCfDataRef( const Buffer &buffer )
{
	return ::CFDataCreateWithBytesNoCopy( kCFAllocatorDefault, reinterpret_cast<const UInt8*>( buffer.getData() ), buffer.getSize(), kCFAllocatorNull );
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ImageSourceCgImage
ImageSourceCgImageRef ImageSourceCgImage::createRef( ::CGImageRef imageRef, ImageSource::Options options )
{
	return shared_ptr<ImageSourceCgImage>( new ImageSourceCgImage( imageRef, options ) );
}

ImageSourceCgImage::ImageSourceCgImage( ::CGImageRef imageRef, ImageSource::Options /*options*/ )
	: ImageSource(), mIsIndexed( false ), mIs16BitPacked( false )
{
	::CGImageRetain( imageRef );
	mImageRef = shared_ptr<CGImage>( imageRef, ::CGImageRelease );
	
	setSize( (int32_t)::CGImageGetWidth( mImageRef.get() ), (int32_t)::CGImageGetHeight( mImageRef.get() ) );
	size_t bpc = ::CGImageGetBitsPerComponent( mImageRef.get() );
	size_t bpp = ::CGImageGetBitsPerPixel( mImageRef.get() );

	// translate data types
	::CGBitmapInfo bitmapInfo = ::CGImageGetBitmapInfo( mImageRef.get() );
	bool isFloat = ( bitmapInfo & kCGBitmapFloatComponents ) != 0;
	::CGImageAlphaInfo alphaInfo = ::CGImageGetAlphaInfo( mImageRef.get() );
	if( isFloat )
		setDataType( ImageIo::FLOAT32 );
	else
		setDataType( ( bpc == 16 ) ? ImageIo::UINT16 : ImageIo::UINT8 );
	if( isFloat && ( bpc != 32 ) )
		throw ImageIoExceptionIllegalDataType( "Illegal data type (cannot handle half-precision floats)" ); // we don't know how to handle half-sized floats yet, but Quartz seems to make them 32bit anyway
	bool hasAlpha = ( alphaInfo != kCGImageAlphaNone ) && ( alphaInfo != kCGImageAlphaNoneSkipLast ) && ( alphaInfo != kCGImageAlphaNoneSkipFirst );

	bool swapEndian = false;
	if( bitmapInfo & kCGBitmapByteOrder32Little )
		swapEndian = true;

	if( bpp == 16 && bpc == 5 ) {// 16-bit packed: 5-5-5 format
		mIs16BitPacked = true;
		m16BitPackedRedOffset = 10; m16BitPackedGreenOffset = 5; m16BitPackedBlueOffset = 0;
		if( (alphaInfo == kCGImageAlphaNoneSkipLast) || (alphaInfo == kCGImageAlphaPremultipliedLast) || (alphaInfo == kCGImageAlphaLast) ) {
			m16BitPackedRedOffset++; m16BitPackedGreenOffset++; m16BitPackedBlueOffset++;
		}
		setColorModel( ImageIo::CM_RGB );
		setChannelOrder( ImageIo::RGB );
		setDataType( ImageIo::UINT8 );
	}
	else {
		// translate color space
		::CGColorSpaceRef colorSpace = ::CGImageGetColorSpace( mImageRef.get() );
		switch( ::CGColorSpaceGetModel( colorSpace ) ) {
			case kCGColorSpaceModelMonochrome:
				setColorModel( ImageIo::CM_GRAY );
				setChannelOrder( ( hasAlpha ) ? ImageIo::YA : ImageIo::Y );
			break;
			case kCGColorSpaceModelRGB:
				setColorModel( ImageSource::CM_RGB );
				switch( alphaInfo ) {
					case kCGImageAlphaNone:
						setChannelOrder( (swapEndian) ? ImageIo::BGR : ImageIo::RGB );
					break;
					case kCGImageAlphaPremultipliedLast:
						setChannelOrder( (swapEndian) ? ImageIo::ABGR : ImageIo::RGBA ); setPremultiplied( true );
					break;
					case kCGImageAlphaLast:
						setChannelOrder( (swapEndian) ? ImageIo::ABGR : ImageIo::RGBA );
					break;
					case kCGImageAlphaPremultipliedFirst:
						setChannelOrder( (swapEndian) ? ImageIo::BGRA : ImageIo::ARGB ); setPremultiplied( true );
					break;
					case kCGImageAlphaFirst:
						setChannelOrder( (swapEndian) ? ImageIo::BGRA : ImageIo::ARGB );
					break;
					case kCGImageAlphaNoneSkipFirst:
						setChannelOrder( (swapEndian) ? ImageIo::BGRX : ImageIo::XRGB );
					break;
					case kCGImageAlphaNoneSkipLast:
						setChannelOrder( (swapEndian) ? ImageIo::XBGR : ImageIo::RGBX );
					break;
					case kCGImageAlphaOnly:
						setColorModel( ImageSource::CM_GRAY );
						setChannelOrder( ImageIo::Y );							
					break;
				}
			break;
			case kCGColorSpaceModelIndexed: {
				setColorModel( ImageIo::CM_RGB );
				setChannelOrder( ImageIo::RGB );
				
				mIsIndexed = true;
				size_t clutSize = ::CGColorSpaceGetColorTableCount( colorSpace );
				uint8_t colorTable[256*3];
				::CGColorSpaceGetColorTable( colorSpace, colorTable );
				for( size_t c = 0; c < clutSize; ++c )
					mColorTable[c] = Color8u( colorTable[c*3+0], colorTable[c*3+1], colorTable[c*3+2] );
			}
			break;
			default: // we only support Gray and RGB data for now
				throw ImageIoExceptionIllegalColorModel( "Core Graphics unexpected data type" );
			break;
		}
	}
}

void ImageSourceCgImage::load( ImageTargetRef target )
{
	int32_t rowBytes = (int32_t)::CGImageGetBytesPerRow( mImageRef.get() );
	const std::shared_ptr<__CFData> pixels( (__CFData*)::CGDataProviderCopyData( ::CGImageGetDataProvider( mImageRef.get() ) ), safeCfRelease );
	
	if( ! pixels )
		throw ImageIoExceptionFailedLoad( "Core Graphics failure copying data." );
	
	// get a pointer to the ImageSource function appropriate for handling our data configuration
	ImageSource::RowFunc func = setupRowFunc( target );
	
	unique_ptr<Color8u[]> tempRowBuffer;
	if( mIsIndexed || mIs16BitPacked )
		tempRowBuffer = unique_ptr<Color8u[]>( new Color8u[mWidth] );
	
	const uint8_t *data = ::CFDataGetBytePtr( pixels.get() );
	for( int32_t row = 0; row < mHeight; ++row ) {
		// if this is indexed fill in our temporary row buffer with the colors pulled from the palette
		if( mIsIndexed ) {
			for( int32_t i = 0; i < mWidth; ++i )
				tempRowBuffer.get()[i] = mColorTable[data[i]];
			((*this).*func)( target, row, tempRowBuffer.get() );
		}
		else if( mIs16BitPacked ) {
			const uint16_t *data16 = reinterpret_cast<const uint16_t*>( data );
			for( int32_t i = 0; i < mWidth; ++i ) {
				const uint16_t d = data16[i];
				Color8u *out = &tempRowBuffer.get()[i];
				out->r = (( d & ( 31 << m16BitPackedRedOffset ) ) >> m16BitPackedRedOffset) * 255 / 31;
				out->g = (( d & ( 31 << m16BitPackedGreenOffset ) ) >> m16BitPackedGreenOffset) * 255 / 31;
				out->b = (( d & ( 31 << m16BitPackedBlueOffset ) ) >> m16BitPackedBlueOffset) * 255 / 31;
			}
			((*this).*func)( target, row, tempRowBuffer.get() );
		}
		else
			((*this).*func)( target, row, data );
		data += rowBytes;
	}
}

ImageSourceCgImageRef createImageSource( ::CGImageRef imageRef, ImageSource::Options options )
{
	return ImageSourceCgImage::createRef( imageRef, options );
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ImageTargetCgImage
ImageTargetCgImageRef ImageTargetCgImage::createRef( ImageSourceRef imageSource, ImageTarget::Options options )
{
	return ImageTargetCgImageRef( new ImageTargetCgImage( imageSource, options ) );
}

ImageTargetCgImage::ImageTargetCgImage( ImageSourceRef imageSource, ImageTarget::Options options )
	: ImageTarget(), mImageRef( 0 )
{
	setSize( (int32_t)imageSource->getWidth(), (int32_t)imageSource->getHeight() );
	mBitsPerComponent = 32;
	bool writingAlpha = imageSource->hasAlpha();
	bool isFloat = true;
	switch( imageSource->getDataType() ) {
		case ImageIo::UINT8: mBitsPerComponent = 8; isFloat = false; setDataType( ImageIo::UINT8 ); mBitmapInfo = kCGBitmapByteOrderDefault; break;
		case ImageIo::UINT16: mBitsPerComponent = 16; isFloat = false; setDataType( ImageIo::UINT16 ); mBitmapInfo = kCGBitmapByteOrder16Little; break;
		default: mBitsPerComponent = 32; isFloat = true; mBitmapInfo = kCGBitmapByteOrder32Little | kCGBitmapFloatComponents; setDataType( ImageIo::FLOAT32 );
	}
	
	if( options.isColorModelDefault() )
		setColorModel( ( imageSource->getColorModel() == ImageIo::CM_GRAY ) ? ImageIo::CM_GRAY : ImageIo::CM_RGB );
	else
		setColorModel( ( options.getColorModel() == ImageIo::CM_GRAY ) ? ImageIo::CM_GRAY : ImageIo::CM_RGB );
	
	uint8_t numChannels;
	switch( mColorModel ) {
		case ImageIo::CM_GRAY:
			numChannels = ( writingAlpha ) ? 2 : 1; break;
		default:
			numChannels = ( writingAlpha ) ? 4 : 3;
	}
	mBitsPerPixel = numChannels * mBitsPerComponent;
	mRowBytes = mWidth * ( numChannels * mBitsPerComponent ) / 8;
	
	if( writingAlpha ) {
		mBitmapInfo |= ( imageSource->isPremultiplied() ) ? kCGImageAlphaPremultipliedLast : kCGImageAlphaLast;
		if( mColorModel == CM_GRAY )
			setChannelOrder( ImageIo::YA );
		else
			setChannelOrder( ImageIo::RGBA );
	}
	else {
		if( mColorModel == CM_GRAY )
			setChannelOrder( ImageIo::Y );
		else {
			setChannelOrder( ImageIo::RGB );		
			mBitmapInfo |= kCGImageAlphaNone;
		}
	}
	
	mDataRef = ::CFDataCreateMutable( kCFAllocatorDefault, mHeight * mRowBytes );
	::CFDataIncreaseLength( mDataRef, mHeight * mRowBytes );
	mDataPtr = ::CFDataGetMutableBytePtr( mDataRef );
}

ImageTargetCgImage::~ImageTargetCgImage()
{
	::CFRelease( mDataRef );
	if( mImageRef )
		::CGImageRelease( mImageRef );
}

void* ImageTargetCgImage::getRowPointer( int32_t row )
{
	return &mDataPtr[row * mRowBytes];
}

void ImageTargetCgImage::finalize()
{
	shared_ptr<CGColorSpace> colorSpaceRef( ( mColorModel == ImageIo::CM_GRAY ) ? ::CGColorSpaceCreateDeviceGray() : ::CGColorSpaceCreateDeviceRGB(), ::CGColorSpaceRelease );
	shared_ptr<CGDataProvider> dataProvider( ::CGDataProviderCreateWithCFData( mDataRef ), ::CGDataProviderRelease );

	mImageRef = ::CGImageCreate( mWidth, mHeight, mBitsPerComponent, mBitsPerPixel, mRowBytes,
			colorSpaceRef.get(), mBitmapInfo, dataProvider.get(), NULL, false, kCGRenderingIntentDefault );
}

::CGImageRef createCgImage( ImageSourceRef imageSource, ImageTarget::Options options )
{
	ImageTargetCgImageRef target = ImageTargetCgImage::createRef( imageSource, options );
	imageSource->load( target );
	target->finalize();
	::CGImageRef result( target->getCgImage() );
	::CGImageRetain( result );
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

Surface8uRef convertCVPixelBufferToSurface( CVPixelBufferRef pixelBufferRef )
{
	::CVPixelBufferLockBaseAddress( pixelBufferRef, 0 );
	uint8_t *ptr = reinterpret_cast<uint8_t*>( CVPixelBufferGetBaseAddress( pixelBufferRef ) );
	int32_t rowBytes = (int32_t)::CVPixelBufferGetBytesPerRow( pixelBufferRef );
	OSType type = CVPixelBufferGetPixelFormatType( pixelBufferRef );
	size_t width = CVPixelBufferGetWidth( pixelBufferRef );
	size_t height = CVPixelBufferGetHeight( pixelBufferRef );
	SurfaceChannelOrder sco;
	if( type == kCVPixelFormatType_24RGB )
		sco = SurfaceChannelOrder::RGB;
	else if( type == kCVPixelFormatType_32ARGB )
		sco = SurfaceChannelOrder::ARGB;
	else if( type == kCVPixelFormatType_24BGR )
		sco = SurfaceChannelOrder::BGR;
	else if( type == kCVPixelFormatType_32BGRA )
		sco = SurfaceChannelOrder::BGRA;
	Surface8u *newSurface = new Surface8u( ptr, (int32_t)width, (int32_t)height, rowBytes, sco );
	return Surface8uRef( newSurface, [=] ( Surface8u *s )
		{	::CVPixelBufferUnlockBaseAddress( pixelBufferRef, 0 );
			::CVBufferRelease( pixelBufferRef );
			delete s;
		}
		);
}

} } // namespace cinder::cocoa


namespace cinder {

SurfaceChannelOrder SurfaceConstraintsCgBitmapContext::getChannelOrder( bool alpha ) const
{
	return ( alpha ) ? SurfaceChannelOrder::RGBA : SurfaceChannelOrder::RGBX;
}

ptrdiff_t SurfaceConstraintsCgBitmapContext::getRowBytes( int32_t requestedWidth, const SurfaceChannelOrder &sco, int elementSize ) const
{
	return requestedWidth * elementSize * 4;
}

} // namespace cinder
