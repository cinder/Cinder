/*
 *  CVPixelFormatDescription.h
 *  CoreVideo
 *
 *  Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
 *
 */

#if !defined(__COREVIDEO_CVPIXELFORMATDESCRIPTION_H__)
#define __COREVIDEO_CVPIXELFORMATDESCRIPTION_H__

#include <TargetConditionals.h>

#if TARGET_OS_MAC
#include <QuartzCore/CoreVideo.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFArray.h>
#else
#include <CoreVideo.h>
#include <CFDictionary.h>
#include <CFArray.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/* This document is influenced by Ice Floe #19: http://developer.apple.com/quicktime/icefloe/dispatch019.html */

#if TARGET_OS_MAC
/* The canonical name for the format.  This should bethe same as the codec name you'd use in QT */
CV_EXPORT const CFStringRef kCVPixelFormatName AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* QuickTime/QuickDraw Pixel Format Type constant (OSType) */
CV_EXPORT const CFStringRef kCVPixelFormatConstant AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* This is the codec type constant, i.e. '2vuy' or k422YpCbCr8CodecType */
CV_EXPORT const CFStringRef kCVPixelFormatCodecType AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* This is the equivalent Microsoft FourCC code for this pixel format */
CV_EXPORT const CFStringRef kCVPixelFormatFourCC AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* All buffers have one or more image planes.  Each plane may contain a single or an interleaved set of components */   
/* For simplicity sake, pixel formats that are not planar may place the required format keys at the top
   level dictionary. */
CV_EXPORT const CFStringRef kCVPixelFormatPlanes AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* The following keys describe the requirements/layout of a a single image plane. */

/* Used to assist with allocating memory for pixel formats that don't have an integer value for
   bytes per pixel */
/* Block width is essentially the width in pixels of the smallest "byte addressable" group of pixels */
/* This works in close conjunction with BitsPerBlock */
/* Examples:
   8-bit luminance only, BlockWidth would be 1, BitsPerBlock would be 8
   16-bit 1555 RGB, BlockWidth would be 1, BitsPerBlock would be 16
   32-bit 8888 ARGB, BlockWidth would be 1, BitsPerBlock would be 32
   2vuy (CbYCrY), BlockWidth would be 2, BitsPerBlock would be 32
   1-bit bitmap, BlockWidth would be 8, BitsPerBlock would be 8
   v210, BlockWidth would be 6, BitsPerBlock would be 128 */
/* Values assumed to 1 be one if not present */
CV_EXPORT const CFStringRef kCVPixelFormatBlockWidth AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
CV_EXPORT const CFStringRef kCVPixelFormatBlockHeight AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* This value must be present.  For simple pixel formats this will be equivalent to the traditional 
   bitsPerPixel value. */
CV_EXPORT const CFStringRef kCVPixelFormatBitsPerBlock AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* Used to state requirements on block multiples.  v210 would be '8' here for the horizontal case, 
   to match the standard v210 row alignment value of 48.
   These may be assumed as 1 if not present. */
CV_EXPORT const CFStringRef kCVPixelFormatBlockHorizontalAlignment AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
CV_EXPORT const CFStringRef kCVPixelFormatBlockVerticalAlignment AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* Subsampling information for this plane.  Assumed to be '1' if not present. */
CV_EXPORT const CFStringRef kCVPixelFormatHorizontalSubsampling AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
CV_EXPORT const CFStringRef kCVPixelFormatVerticalSubsampling AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* If present, these two keys describe the OpenGL format and type enums you would use to describe this
   image plane to OpenGL */
CV_EXPORT const CFStringRef kCVPixelFormatOpenGLFormat AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
CV_EXPORT const CFStringRef kCVPixelFormatOpenGLType AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
CV_EXPORT const CFStringRef kCVPixelFormatOpenGLInternalFormat AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* CGBitmapInfo value, if required */
CV_EXPORT const CFStringRef kCVPixelFormatCGBitmapInfo AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* Pixel format compatibility flags */
CV_EXPORT const CFStringRef kCVPixelFormatQDCompatibility AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
CV_EXPORT const CFStringRef kCVPixelFormatCGBitmapContextCompatibility AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
CV_EXPORT const CFStringRef kCVPixelFormatCGImageCompatibility AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
CV_EXPORT const CFStringRef kCVPixelFormatOpenGLCompatibility AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
#endif

/* This callback routine implements code to handle the functionality of CVPixelBufferFillExtendedPixels.  
   For custom pixel formats where you will never need to use that call, this is not required. */
typedef Boolean (*CVFillExtendedPixelsCallBack)( CVPixelBufferRef pixelBuffer, void *refCon);
typedef struct {
    CFIndex version;
    CVFillExtendedPixelsCallBack    fillCallBack;
    void *refCon;
} CVFillExtendedPixelsCallBackData;

#if TARGET_OS_MAC
/* The value for this key is a CFData containing a CVFillExtendedPixelsCallBackData struct */
CV_EXPORT const CFStringRef kCVPixelFormatFillExtendedPixelsCallback AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
#endif

/* Create a description of a pixel format from a provided OSType */
CV_EXPORT CFDictionaryRef CVPixelFormatDescriptionCreateWithPixelFormatType(CFAllocatorRef allocator, OSType pixelFormat) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* Get an array containing all known pixel format description dictionaries */
CV_EXPORT CFArrayRef CVPixelFormatDescriptionArrayCreateWithAllPixelFormatTypes(CFAllocatorRef allocator) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/* Register a new pixel format with CoreVideo */
CV_EXPORT void CVPixelFormatDescriptionRegisterDescriptionWithPixelFormatType(CFDictionaryRef description, OSType pixelFormat) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

#if TARGET_OS_WIN32
#define kCVPixelFormatName				CFSTR("Name")
#define kCVPixelFormatName422YpCbCr8			CFSTR("Component Y'CbCr 8-bit 4:2:2")
#define kCVPixelFormatName422YpCbCr10			CFSTR("Component Y'CbCr 10-bit 4:2:2")
#define kCVPixelFormatConstant				CFSTR("PixelFormat")
#define kCVPixelFormatCodecType			CFSTR("CodecType")
#define kCVPixelFormatFourCC				CFSTR("FourCC")
#define kCVPixelFormatPlanes				CFSTR("Planes")        
#define kCVPixelFormatComponents			CFSTR("ComponentLayout")
#define kCVPixelFormatBlockWidth			CFSTR("BlockWidth")
#define kCVPixelFormatBlockHeight			CFSTR("BlockHeight")
#define kCVPixelFormatBlockHorizontalAlignment		CFSTR("BlockHorizontalAlignment")
#define kCVPixelFormatBlockVerticalAlignment		CFSTR("BlockVerticalAlignment")
#define kCVPixelFormatBitsPerBlock			CFSTR("BitsPerBlock")
#define kCVPixelFormatHorizontalSubsampling		CFSTR("HorizontalSubsampling")
#define kCVPixelFormatVerticalSubsampling		CFSTR("VerticalSubsampling")
#define kCVPixelFormatOpenGLFormat			CFSTR("OpenGLFormat")
#define kCVPixelFormatOpenGLType			CFSTR("OpenGLType")
#define kCVPixelFormatOpenGLInternalFormat		CFSTR("OpenGLInternalFormat")
#define kCVPixelFormatDirect3DFormat				CFSTR("D3DFormat")
#define kCVPixelFormatDirect3DType				CFSTR("D3DType")
#define kCVPixelFormatDirect3DInternalFormat			CFSTR("D3DInternalFormat")
#define kCVPixelFormatQDCompatibility			CFSTR("QDCompatibility")
#define kCVPixelFormatCGBitmapContextCompatibility	CFSTR("CGBitmapContextCompatibility")
#define kCVPixelFormatCGImageCompatibility		CFSTR("CGImageCompatibility")
#define kCVPixelFormatOpenGLCompatibility		CFSTR("OpenGLCompatibility")
#define kCVPixelFormatDirect3DCompatibility			CFSTR("Direct3DCompatibility")
#define kCVPixelFormatCGBitmapInfo			CFSTR("CGBitmapInfo")
#define kCVPixelFormatFillExtendedPixelsCallback	CFSTR("FillExtendedPixelsCallback")
#endif

#if defined(__cplusplus)
}
#endif

#endif

