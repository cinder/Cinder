/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/Clipboard.h"
#include "cinder/Unicode.h"
#include "cinder/Log.h"
#if defined( CINDER_MAC )
	#include "cinder/cocoa/CinderCocoa.h"
	#import <Cocoa/Cocoa.h>
	#import <AppKit/NSPasteboard.h>
	#import <AppKit/NSImage.h>
#elif defined( CINDER_COCOA_TOUCH )	
	#include "cinder/cocoa/CinderCocoa.h"
	#include "cinder/cocoa/CinderCocoaTouch.h"
	#import <UIKit/UIPasteboard.h>	
#elif defined( CINDER_MSW )
	#include <windows.h>
	#define max(a, b) (((a) > (b)) ? (a) : (b))
	#define min(a, b) (((a) < (b)) ? (a) : (b))
	#include <gdiplus.h>
	#undef min
	#undef max
	#include "cinder/msw/CinderMsw.h"
	#include "cinder/msw/CinderMswGdiPlus.h"
	#include "cinder/Utilities.h"
	#include "cinder/ip/Fill.h"
	#include "cinder/ip/Blend.h"
	#include <set>
#elif defined( CINDER_LINUX )
	#include "cinder/app/App.h"
	#include "cinder/app/RendererGl.h"
	#include "glad/glad.h"
	#include "glfw/glfw3.h"
#endif

namespace cinder {

#if defined( CINDER_MSW )
bool clipboardContainsFormat( const std::set<UINT> &formats )
{
	bool result = false;
	if( ! ::OpenClipboard( NULL ) )
		return false;
	int numFormats = ::CountClipboardFormats();
	for( int f = 0; f < numFormats; ++f ) {
		if( formats.find( ::EnumClipboardFormats( f ) ) != formats.end() ) {
			result = true;
			break;
		}
	}
	::CloseClipboard();
	return result;
}

#endif

bool Clipboard::hasString()
{
#if defined( CINDER_MAC )
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    NSArray *classArray = [NSArray arrayWithObject:[NSString class]];
    NSDictionary *options = [NSDictionary dictionary];
 
	return [pasteboard canReadObjectForClasses:classArray options:options];
#elif defined( CINDER_COCOA_TOUCH )
	UIPasteboard *pasteboard = [UIPasteboard generalPasteboard]; 
	return [pasteboard containsPasteboardTypes:UIPasteboardTypeListString];	
#elif defined( CINDER_MSW )
	std::set<UINT> textFormats;
	textFormats.insert( CF_TEXT ); textFormats.insert( CF_UNICODETEXT ); textFormats.insert( CF_OEMTEXT );
	return clipboardContainsFormat( textFormats );
#elif defined( CINDER_LINUX )
	return glfwGetClipboardString( (GLFWwindow*)app::getWindow()->getNative() ) != nullptr;
#endif
}

bool Clipboard::hasImage()
{
#if defined( CINDER_MAC )
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    NSArray *classArray = [NSArray arrayWithObject:[NSImage class]];
    NSDictionary *options = [NSDictionary dictionary];
 
	return [pasteboard canReadObjectForClasses:classArray options:options];
#elif defined( CINDER_COCOA_TOUCH )
	UIPasteboard *pasteboard = [UIPasteboard generalPasteboard]; 
	return [pasteboard containsPasteboardTypes:UIPasteboardTypeListImage];		
#elif defined( CINDER_MSW )
	std::set<UINT> imageFormats;
	imageFormats.insert( CF_BITMAP ); imageFormats.insert( CF_DIB ); imageFormats.insert( CF_DIBV5 );
	return clipboardContainsFormat( imageFormats );
#elif defined( CINDER_LINUX )
	return false;
#endif
}
	
std::string Clipboard::getString()
{
#if defined( CINDER_MAC )
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    NSArray *classArray = [NSArray arrayWithObject:[NSString class]];
    NSDictionary *options = [NSDictionary dictionary];
 
	if( [pasteboard canReadObjectForClasses:classArray options:options] ) {
		NSArray *objectsToPaste = [pasteboard readObjectsForClasses:classArray options:options];
        NSString *text = [objectsToPaste firstObject];
		if( ! text )
			return std::string();
		else
			return cocoa::convertNsString( text );
	}
	else {
		return std::string();
	}
#elif defined( CINDER_COCOA_TOUCH )
	UIPasteboard *pasteboard = [UIPasteboard generalPasteboard]; 
	NSString *str = pasteboard.string;
	if( str )
		return cocoa::convertNsString( str );
	else
		return std::string();
#elif defined( CINDER_MSW )
	std::string result;
	if( ! ::OpenClipboard( NULL ) )
		return result;
	HANDLE dataH = ::GetClipboardData( CF_UNICODETEXT );
	if( dataH ) {
		wchar_t *pstr = reinterpret_cast<wchar_t*>( ::GlobalLock( dataH ) );
		if( pstr ) {
			std::wstring wstr( pstr );
			result = toUtf8( (char16_t*)wstr.c_str() );
			::GlobalUnlock( dataH );
		}
	}
	::CloseClipboard();
	return result;
#elif defined( CINDER_LINUX )
	return std::string( glfwGetClipboardString( (GLFWwindow*)app::getWindow()->getNative() ) );
#endif
}

ImageSourceRef Clipboard::getImage()
{
#if defined( CINDER_MAC )
	NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    NSBitmapImageRep *rep = (NSBitmapImageRep*)[NSBitmapImageRep imageRepWithPasteboard:pasteboard];
	if( rep )
		return cocoa::ImageSourceCgImage::createRef( [rep CGImage] );
	else
		return ImageSourceRef();
#elif defined( CINDER_COCOA_TOUCH )
	UIPasteboard *pasteboard = [UIPasteboard generalPasteboard]; 
	UIImage *image = pasteboard.image;
	if( image )
		return ImageSourceRef( *cocoa::convertUiImage( image ) );
	else
		return ImageSourceRef();
#elif defined( CINDER_MSW )
	ImageSourceRef result;
	if( ! ::OpenClipboard( NULL ) )
		return ImageSourceRef();
	HANDLE dataH = ::GetClipboardData( CF_DIBV5 );
	if( dataH ) {
		uint8_t *data = reinterpret_cast<uint8_t*>( ::GlobalLock( dataH ) );
		// CF_DIBV5 is composed of a BITMAPV5HEADER + bitmap data
		Gdiplus::Bitmap *bmp = Gdiplus::Bitmap::FromBITMAPINFO( reinterpret_cast<BITMAPINFO*>( data ), data + sizeof(BITMAPV5HEADER) );
		if( bmp ) {
			result = ImageSourceRef( msw::convertGdiplusBitmap( *bmp ) );
			delete bmp;
		}
		::GlobalUnlock( dataH );
	}
	::CloseClipboard();
	return result;
#elif defined( CINDER_LINUX )
	return ImageSourceRef();
#endif
}

void Clipboard::setString( const std::string &str )
{
#if defined( CINDER_MAC )
	[[NSPasteboard generalPasteboard] declareTypes: [NSArray arrayWithObject: NSStringPboardType] owner:nil];
	[[NSPasteboard generalPasteboard] setString:[NSString stringWithUTF8String:str.c_str()] forType: NSStringPboardType];
#elif defined( CINDER_COCOA_TOUCH )
	UIPasteboard *pasteboard = [UIPasteboard generalPasteboard];
	pasteboard.string = [NSString stringWithUTF8String:str.c_str()];
#elif defined( CINDER_MSW )
	if( ! ::OpenClipboard( NULL ) ) {
		CI_LOG_E( "Failed to open clipboard" );
		return;
	}
	::EmptyClipboard();
	std::u16string wstr = toUtf16( str );
	HANDLE hglbCopy = ::GlobalAlloc( GMEM_MOVEABLE, sizeof(uint16_t) * (wstr.length()+1) );
	// Lock the handle and copy the text to the buffer. 
	void *lptstrCopy = ::GlobalLock( hglbCopy ); 
	memcpy( lptstrCopy, &wstr[0], sizeof(uint16_t) * (wstr.length()+1) ); 
	::GlobalUnlock( hglbCopy );
	::SetClipboardData( CF_UNICODETEXT, hglbCopy ); 
	::CloseClipboard();
#elif defined( CINDER_LINUX )
 	glfwSetClipboardString( (GLFWwindow*)app::getWindow()->getNative(), str.c_str() );
#endif
}

void Clipboard::setImage( ImageSourceRef imageSource, ImageTarget::Options options )
{
#if defined( CINDER_MAC )
	cocoa::ImageTargetCgImageRef target = cocoa::ImageTargetCgImage::createRef( imageSource, options );
	imageSource->load( target );
	target->finalize();
	
	NSBitmapImageRep *imageRep = [[[NSBitmapImageRep alloc] initWithCGImage:target->getCgImage()] autorelease];
	NSImage *image = [[NSImage alloc] initWithSize:[imageRep size]];
	[image addRepresentation: imageRep];
	[[NSPasteboard generalPasteboard] declareTypes: [NSArray arrayWithObject: NSTIFFPboardType] owner:nil];
	[[NSPasteboard generalPasteboard] setData:[image TIFFRepresentation] forType:NSTIFFPboardType];	
	[image release];
#elif defined( CINDER_COCOA_TOUCH )
	(void) options;
	UIPasteboard *pasteboard = [UIPasteboard generalPasteboard];
	cocoa::SafeUiImage uiImage = cocoa::createUiImage( imageSource );
	pasteboard.image = (UIImage*)uiImage;
#elif defined( CINDER_MSW )
	(void) options;
	if( ! ::OpenClipboard( NULL ) ) {
		CI_LOG_E( "Failed to open clipboard" );
		return;
	}
	::EmptyClipboard();
	int32_t rowBytes = (( imageSource->getWidth() * 32 + 31 ) / 32 ) * 4;
	size_t dataSize = sizeof(BITMAPV5HEADER) + imageSource->getHeight() * rowBytes; 
	HANDLE hglbCopy = ::GlobalAlloc( GMEM_MOVEABLE, dataSize );
	uint8_t *destData = reinterpret_cast<uint8_t*>( ::GlobalLock( hglbCopy ) ); 
	
	// we create 'tempSurface' convert 'imageSource' into a Surface8u
	// then alpha blend 'tempSurface' into 'destSurface'.
	// Unfortunately alpha on Win32 Clipboard just seems to be a total mess, so we're punting and forcing it to be opaque on black.
	SurfaceChannelOrder sco = SurfaceChannelOrder::BGRA;
	const Surface8u tempSurface( imageSource );
	Surface8u destSurface( destData + sizeof(BITMAPV5HEADER), imageSource->getWidth(), imageSource->getHeight(), rowBytes, sco );
	ip::fill( &destSurface, ColorA8u( 0, 0, 0, 255 ) );
	ip::blend( &destSurface, tempSurface );

	// set the BITMAPV5HEADER based on 'destSurface'
	BITMAPV5HEADER bi = {0};
	bi.bV5Size          = sizeof(BITMAPV5HEADER);
	bi.bV5Width         = destSurface.getWidth();
	bi.bV5Height        = -destSurface.getHeight();
	bi.bV5Planes        = 1;
	bi.bV5BitCount      = 32;
	bi.bV5Compression   = BI_RGB;
	bi.bV5SizeImage     = destSurface.getHeight() * rowBytes;
	bi.bV5XPelsPerMeter = 0;
	bi.bV5YPelsPerMeter = 0;
	bi.bV5ClrUsed       = 0;
	bi.bV5ClrImportant  = 0;
	bi.bV5BlueMask      = 0x000000ff;
	bi.bV5GreenMask     = 0x0000ff00;
	bi.bV5RedMask       = 0x00ff0000;
	bi.bV5AlphaMask     = 0xff000000;
	bi.bV5CSType        = LCS_sRGB;
	bi.bV5Intent        = LCS_GM_IMAGES;
	*(reinterpret_cast<BITMAPV5HEADER*>(destData)) = bi;

	::GlobalUnlock( hglbCopy );
	::SetClipboardData( CF_DIBV5, hglbCopy ); 
	::CloseClipboard();
#elif defined( CINDER_LINUX )
	CI_LOG_E( "Clipboard::setImage() not supported on Linux" );
#endif
}

} // namespace cinder
