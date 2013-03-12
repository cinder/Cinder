/*
 *  CVPixelBuffer.h
 *  CoreVideo
 *
 *  Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
 *
 */
 
  /*! @header CVPixelBuffer.h
	@copyright 2004 Apple Computer, Inc. All rights reserved.
	@availability Mac OS X 10.4 or later
    @discussion CVPixelBuffers are CVImageBuffers that hold the pixels in main memory
		   
*/

#if !defined(__COREVIDEO_CVPIXELBUFFER_H__)
#define __COREVIDEO_CVPIXELBUFFER_H__ 1

#include <TargetConditionals.h>

#if TARGET_OS_MAC
#include <QuartzCore/CVImageBuffer.h>
#include <CoreFoundation/CFArray.h>
#else
#pragma warning (disable: 4068)		// ignore unknown pragmas
#include <CVImageBuffer.h>
#include <CFArray.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#pragma mark BufferAttributeKeys
#if TARGET_OS_MAC
CV_EXPORT const CFStringRef kCVPixelBufferPixelFormatTypeKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;		    // A single CFNumber or a CFArray of CFNumbers (OSTypes)
CV_EXPORT const CFStringRef kCVPixelBufferMemoryAllocatorKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;		    // CFAllocatorRef
CV_EXPORT const CFStringRef kCVPixelBufferWidthKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;			    // CFNumber
CV_EXPORT const CFStringRef kCVPixelBufferHeightKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;			    // CFNumber
CV_EXPORT const CFStringRef kCVPixelBufferExtendedPixelsLeftKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;	    // CFNumber
CV_EXPORT const CFStringRef kCVPixelBufferExtendedPixelsTopKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;		    // CFNumber
CV_EXPORT const CFStringRef kCVPixelBufferExtendedPixelsRightKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;	    // CFNumber
CV_EXPORT const CFStringRef kCVPixelBufferExtendedPixelsBottomKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;	    // CFNumber
CV_EXPORT const CFStringRef kCVPixelBufferBytesPerRowAlignmentKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;	    // CFNumber
CV_EXPORT const CFStringRef kCVPixelBufferCGBitmapContextCompatibilityKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;  // CFBoolean
CV_EXPORT const CFStringRef kCVPixelBufferCGImageCompatibilityKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;	    // CFBoolean
CV_EXPORT const CFStringRef kCVPixelBufferOpenGLCompatibilityKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;	    // CFBoolean
#else
#define kCVPixelBufferPixelFormatTypeKey		CFSTR("PixelFormatType")
#define kCVPixelBufferMemoryAllocatorKey		CFSTR("MemoryAllocator")
#define kCVPixelBufferWidthKey				CFSTR("Width")
#define kCVPixelBufferHeightKey			CFSTR("Height")
#define kCVPixelBufferExtendedPixelsLeftKey		CFSTR("ExtendedPixelsLeft")
#define kCVPixelBufferExtendedPixelsTopKey		CFSTR("ExtendedPixelsTop")
#define kCVPixelBufferExtendedPixelsRightKey		CFSTR("ExtendedPixelsRight")
#define kCVPixelBufferExtendedPixelsBottomKey		CFSTR("ExtendedPixelsBottom")
#define kCVPixelBufferBytesPerRowAlignmentKey		CFSTR("BytesPerRowAlignment")
#define kCVPixelBufferCGBitmapContextCompatibilityKey  CFSTR("CGBitmapContextCompatibility")
#define kCVPixelBufferCGImageCompatibilityKey		CFSTR("CGImageCompatibility")
#define kCVPixelBufferOpenGLCompatibilityKey		CFSTR("OpenGLCompatibility")
#endif
/*!
    @typedef	CVPixelBufferRef
    @abstract   Based on the image buffer type. The pixel buffer implements the memory storage for an image buffer.

*/
typedef CVImageBufferRef CVPixelBufferRef;

CV_EXPORT CFTypeID CVPixelBufferGetTypeID() AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferRetain
    @abstract   Retains a CVPixelBuffer object
    @discussion Equivalent to CFRetain, but NULL safe
    @param      buffer A CVPixelBuffer object that you want to retain.
    @result     A CVPixelBuffer object that is the same as the passed in buffer.
*/
CV_EXPORT CVPixelBufferRef CVPixelBufferRetain( CVPixelBufferRef texture ) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferRelease
    @abstract   Releases a CVPixelBuffer object
    @discussion Equivalent to CFRelease, but NULL safe
    @param      buffer A CVPixelBuffer object that you want to release.
*/
CV_EXPORT void CVPixelBufferRelease( CVPixelBufferRef texture ) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferCreateResolvedAttributesDictionary
    @abstract   Takes a CFArray of CFDictionary objects describing various pixel buffer attributes and tries to resolve them into a
                single dictionary.
    @discussion This is useful when you need to resolve multiple requirements between different potential clients of a buffer.
    @param      attributes CFArray of CFDictionaries containing kCVPixelBuffer key/value pairs.
    @param      resolvedDictionaryOut The resulting dictionary will be placed here.
    @result     Return value that may be useful in discovering why resolution failed.
*/
CV_EXPORT CVReturn CVPixelBufferCreateResolvedAttributesDictionary(CFAllocatorRef allocator, CFArrayRef attributes, CFDictionaryRef *resolvedDictionaryOut) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;


/*!
    @function   CVPixelBufferCreate
    @abstract   Call to create a single PixelBuffer for a given size and pixelFormatType.
    @discussion Creates a single PixelBuffer for a given size and pixelFormatType. It allocates the necessary memory based on the pixel dimensions, pixelFormatType and extended pixels described in the pixelBufferAttributes. Not all parameters of the pixelBufferAttributes will be used here.
    @param      width   Width of the PixelBuffer in pixels.
    @param      height  Height of the PixelBuffer in pixels.
    @param	pixelFormatType		Pixel format indentified by its respective OSType.
    @param	pixelBufferAttributes      A dictionary with additonal attributes for a a pixel buffer. This parameter is optional. See PixelBufferAttributes for more details.
    @param      pixelBufferOut          The new pixel buffer will be returned here
    @result	returns kCVReturnSuccess on success.
*/    
CV_EXPORT CVReturn CVPixelBufferCreate(CFAllocatorRef allocator,
                                      size_t width,
                                      size_t height,
                                      OSType pixelFormatType,
                                      CFDictionaryRef pixelBufferAttributes,
                                      CVPixelBufferRef *pixelBufferOut) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

typedef void (*CVPixelBufferReleaseBytesCallback)( void *releaseRefCon, const void *baseAddress );

/*!
    @function   CVPixelBufferCreateWithBytes
    @abstract   Call to create a single PixelBuffer for a given size and pixelFormatType based on a passed in piece of memory.
    @discussion Creates a single PixelBuffer for a given size and pixelFormatType. Not all parameters of the pixelBufferAttributes will be used here. It requires a release callback function that will be called, when the PixelBuffer gets destroyed so that the owner of the pixels can free the memory.
    @param      width   Width of the PixelBuffer in pixels
    @param      height  Height of the PixelBuffer in pixels
    @param      pixelFormatType		Pixel format indentified by its respective OSType.
    @param      baseAddress		Address of the memory storing the pixels.
    @param      bytesPerRow		Row bytes of the pixel storage memory.
    @param      releaseCallback         CVPixelBufferReleaseBytePointerCallback function that gets called when the PixelBuffer gets destroyed.
    @param      releaseRefCon           User data identifying the PixelBuffer for the release callback.
    @param      pixelBufferAttributes      A dictionary with additonal attributes for a a pixel buffer. This parameter is optional. See PixelBufferAttributes for more details.
    @param      pixelBufferOut          The new pixel buffer will be returned here
    @result	returns kCVReturnSuccess on success.
*/
CV_EXPORT CVReturn CVPixelBufferCreateWithBytes(CFAllocatorRef allocator,
					       size_t width,
					       size_t height,
					       OSType pixelFormatType,
					       void *baseAddress,
					       size_t bytesPerRow,
					       CVPixelBufferReleaseBytesCallback releaseCallback,
					       void *releaseRefCon,
					       CFDictionaryRef pixelBufferAttributes,
				               CVPixelBufferRef *pixelBufferOut) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

typedef void (*CVPixelBufferReleasePlanarBytesCallback)( void *releaseRefCon, const void *dataPtr, size_t dataSize, size_t numberOfPlanes, const void *planeAddresses[] );

/*!
    @function   CVPixelBufferCreateWithPlanarBytes
    @abstract   Call to create a single PixelBuffer in planar format for a given size and pixelFormatType based on a passed in piece of memory.
    @discussion Creates a single PixelBuffer for a given size and pixelFormatType. Not all parameters of the pixelBufferAttributes will be used here. It requires a release callback function that will be called, when the PixelBuffer gets destroyed so that the owner of the pixels can free the memory.
    @param      width			Width of the PixelBuffer in pixels
    @param      height			Height of the PixelBuffer in pixels
    @param      pixelFormatType		Pixel format indentified by its respective OSType.
    @param	dataPtr			Pass a pointer to a plane descriptor block, or NULL.
    @param	dataSize		pass size if planes are contiguous, NULL if not.
    @param	numberOfPlanes		Number of planes.
    @param	planeBaseAddress	Array of base addresses for the planes.
    @param	planeWidth		Array of plane widths.
    @param	planeHeight		Array of plane heights.
    @param	planeBytesPerRow	Array of plane bytesPerRow values.
    @param	releaseCallback		CVPixelBufferReleaseBytePointerCallback function that gets called when the PixelBuffer gets destroyed.
    @param	releaseRefCon		User data identifying the PixelBuffer for the release callback.
    @param	pixelBufferAttributes      A dictionary with additonal attributes for a a pixel buffer. This parameter is optional. See PixelBufferAttributes for more details.
    @param      pixelBufferOut          The new pixel buffer will be returned here
    @result	returns kCVReturnSuccess on success.
*/
CV_EXPORT CVReturn CVPixelBufferCreateWithPlanarBytes(CFAllocatorRef allocator,
							 size_t width,
							 size_t height,
							 OSType pixelFormatType,
							 void *dataPtr, // pass a pointer to a plane descriptor block, or NULL
							 size_t dataSize, // pass size if planes are contiguous, NULL if not
							 size_t numberOfPlanes,
							 void *planeBaseAddress[],
							 size_t planeWidth[],
							 size_t planeHeight[],
							 size_t planeBytesPerRow[],
							 CVPixelBufferReleasePlanarBytesCallback releaseCallback,
							 void *releaseRefCon,
							 CFDictionaryRef pixelBufferAttributes,
							 CVPixelBufferRef *pixelBufferOut) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;


/*!
    @function   CVPixelBufferLockBaseAddress
    @abstract   Description Locks the BaseAddress of the PixelBuffer to ensure that the is available.
    @param      pixelBuffer Target PixelBuffer.
    @param      lockFlags No options currently defined, pass 0.
    @result     kCVReturnSuccess if the lock succeeded, or error code on failure
*/
CV_EXPORT CVReturn CVPixelBufferLockBaseAddress(CVPixelBufferRef pixelBuffer, CVOptionFlags lockFlags) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferUnlockBaseAddress
    @abstract   Description Unlocks the BaseAddress of the PixelBuffer.
    @param      pixelBuffer Target PixelBuffer.
    @param      unlockFlags No options currently defined, pass 0.
    @result     kCVReturnSuccess if the unlock succeeded, or error code on failure
*/
CV_EXPORT CVReturn CVPixelBufferUnlockBaseAddress(CVPixelBufferRef pixelBuffer, CVOptionFlags unlockFlags) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferGetWidth
    @abstract   Returns the width of the PixelBuffer.
    @param      pixelBuffer Target PixelBuffer.
    @result     Width in pixels.
*/
CV_EXPORT size_t CVPixelBufferGetWidth(CVPixelBufferRef pixelBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferGetHeight
    @abstract   Returns the height of the PixelBuffer.
    @param      pixelBuffer Target PixelBuffer.
    @result     Height in pixels.
*/
CV_EXPORT size_t CVPixelBufferGetHeight(CVPixelBufferRef pixelBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferGetPixelFormatType
    @abstract   Returns the PixelFormatType of the PixelBuffer.
    @param      pixelBuffer Target PixelBuffer.
    @result     OSType identifying the pixel format by its type.
*/
CV_EXPORT OSType CVPixelBufferGetPixelFormatType(CVPixelBufferRef pixelBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferGetBaseAddress
    @abstract   Returns the base address of the PixelBuffer.
    @discussion Retrieving the base address for a PixelBuffer requires that the buffer base address be locked
                via a successful call to CVPixelBufferLockBaseAddress.
    @param      pixelBuffer Target PixelBuffer.
    @result     Base address of the pixels.
		For chunky buffers, this will return a pointer to the pixel at 0,0 in the buffer
		For planar buffers this will return a pointer to a PlanarComponentInfo struct (defined in QuickTime).
*/
CV_EXPORT void *CVPixelBufferGetBaseAddress(CVPixelBufferRef pixelBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferGetBytesPerRow
    @abstract   Returns the rowBytes of the PixelBuffer.
    @param      pixelBuffer Target PixelBuffer.
    @result     Bytes per row of the image data.   For planar buffers this will return a rowBytes value such that bytesPerRow * height
                will cover the entire image including all planes.
*/
CV_EXPORT size_t CVPixelBufferGetBytesPerRow(CVPixelBufferRef pixelBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferGetDataSize
    @abstract   Returns the data size for contigous planes of the PixelBuffer.
    @param      pixelBuffer Target PixelBuffer.
    @result     Data size used in CVPixelBufferCreateWithPlanarBytes.
*/
CV_EXPORT size_t CVPixelBufferGetDataSize(CVPixelBufferRef pixelBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferIsPlanar
    @abstract   Returns if the PixelBuffer is planar.
    @param      pixelBuffer Target PixelBuffer.
    @result     True if the PixelBuffer was created using CVPixelBufferCreateWithPlanarBytes.
*/
CV_EXPORT Boolean CVPixelBufferIsPlanar(CVPixelBufferRef pixelBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferGetPlaneCount
    @abstract   Returns number of planes of the PixelBuffer.
    @param      pixelBuffer Target PixelBuffer.
    @result     Number of planes.  Returns 0 for non-planar CVPixelBufferRefs.
*/
CV_EXPORT size_t CVPixelBufferGetPlaneCount(CVPixelBufferRef pixelBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferGetWidthOfPlane
    @abstract   Returns the width of the plane at planeIndex in the PixelBuffer.
    @param      pixelBuffer Target PixelBuffer.
    @param      planeIndex  Identifying the plane.
    @result     Width in pixels, or 0 for non-planar CVPixelBufferRefs.
*/
CV_EXPORT size_t CVPixelBufferGetWidthOfPlane(CVPixelBufferRef pixelBuffer, size_t planeIndex) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferGetHeightOfPlane
    @abstract   Returns the height of the plane at planeIndex in the PixelBuffer.
    @param      pixelBuffer Target PixelBuffer.
    @param      planeIndex  Identifying the plane.
    @result     Height in pixels, or 0 for non-planar CVPixelBufferRefs.
*/
CV_EXPORT size_t CVPixelBufferGetHeightOfPlane(CVPixelBufferRef pixelBuffer, size_t planeIndex) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferGetBaseAddressOfPlane
    @abstract   Returns the base address of the plane at planeIndex in the PixelBuffer.
    @discussion Retrieving the base address for a PixelBuffer requires that the buffer base address be locked
                via a successful call to CVPixelBufferLockBaseAddress.
    @param      pixelBuffer Target PixelBuffer.
    @param      planeIndex  Identifying the plane.
    @result     Base address of the plane, or NULL for non-planar CVPixelBufferRefs.
*/
CV_EXPORT void *CVPixelBufferGetBaseAddressOfPlane(CVPixelBufferRef pixelBuffer, size_t planeIndex) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferGetBytesPerRowOfPlane
    @abstract   Returns the row bytes of the plane at planeIndex in the PixelBuffer.
    @param      pixelBuffer Target PixelBuffer.
    @param      planeIndex  Identifying the plane.
    @result     Row bytes of the plane, or NULL for non-planar CVPixelBufferRefs.
*/
CV_EXPORT size_t CVPixelBufferGetBytesPerRowOfPlane(CVPixelBufferRef pixelBuffer, size_t planeIndex) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferGetExtendedPixels
    @abstract   Returns the size of extended pixels of the PixelBuffer.
    @param      pixelBuffer Target PixelBuffer.
    @param      extraColumnsOnLeft Returns the pixel row padding to the left.  May be NULL.
    @param      extraRowsOnTop Returns the pixel row padding to the top.  May be NULL. 
    @param      extraColumnsOnRight Returns the pixel row padding to the right. May be NULL.
    @param      extraRowsOnBottom Returns the pixel row padding to the bottom. May be NULL.
*/
CV_EXPORT void CVPixelBufferGetExtendedPixels(CVPixelBufferRef pixelBuffer,
					      size_t *extraColumnsOnLeft,
					      size_t *extraColumnsOnRight,
					      size_t *extraRowsOnTop,
					      size_t *extraRowsOnBottom) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferFillExtendedPixels
    @abstract   Fills the extended pixels of the PixelBuffer with Zero.   This function replicates edge pixels to fill the entire extended region of the image.
    @param      pixelBuffer Target PixelBuffer.
*/
CV_EXPORT CVReturn CVPixelBufferFillExtendedPixels(CVPixelBufferRef pixelBuffer) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

#if defined(__cplusplus)
}
#endif

#endif
