/*
 *  CVPixelBufferPool.h
 *  CoreVideo
 *
 *  Copyright 2004 Apple Computer, Inc. All rights reserved.
 *
 */

 /*! @header CVPixelBufferPool.h
	@copyright 2004 Apple Computer, Inc. All rights reserved.
	@availability Mac OS X 10.4 or later
    @discussion CVPixelBufferPool is a utility object for managing a set of CVPixelBuffer objects that are going to be recycled.
		   
*/

#if !defined(__COREVIDEO__CVPIXELBUFFERPOOL_H__)
#define __COREVIDEO__CVPIXELBUFFERPOOL_H__ 1

#include <TargetConditionals.h>

#if TARGET_OS_MAC
#include <QuartzCore/CVBase.h>
#include <QuartzCore/CVReturn.h>
#include <QuartzCore/CVPixelBuffer.h>
#else
#include <CVBase.h>
#include <CVReturn.h>
#include <CVPixelBuffer.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct __CVPixelBufferPool *CVPixelBufferPoolRef;

// By default, buffers will age out after one second.   If required, setting an age of zero will disable
// the age-out mechanism completely.

#if TARGET_OS_MAC
CV_EXPORT const CFStringRef kCVPixelBufferPoolMinimumBufferCountKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
CV_EXPORT const CFStringRef kCVPixelBufferPoolMaximumBufferAgeKey AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;
#else
#define kCVPixelBufferPoolMinimumBufferCountKey	CFSTR("MinimumBufferCount")
#define kCVPixelBufferPoolMaximumBufferAgeKey		CFSTR("MaximumBufferAge")
#endif

CV_EXPORT CFTypeID CVPixelBufferPoolGetTypeID() AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferPoolRetain
    @abstract   Retains a CVPixelBufferPoolRef object
    @discussion Equivalent to CFRetain, but NULL safe
    @param      buffer A CVPixelBufferPoolRef object that you want to retain.
    @result     A CVPixelBufferPoolRef object that is the same as the passed in buffer.
*/
CV_EXPORT CVPixelBufferPoolRef CVPixelBufferPoolRetain( CVPixelBufferPoolRef pixelBufferPool ) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER; // NULL-safe

/*!
    @function   CVPixelBufferPoolRelease
    @abstract   Releases a CVPixelBufferPoolRef object
    @discussion Equivalent to CFRelease, but NULL safe
    @param      buffer A CVPixelBufferPoolRef object that you want to release.
*/
CV_EXPORT void CVPixelBufferPoolRelease( CVPixelBufferPoolRef pixelBufferPool ) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER; // NULL-safe

/*!
    @function   CVPixelBufferPoolCreate
    @abstract   Creates a new Pixel Buffer pool.
    @param      allocator The CFAllocatorRef to use for allocating this buffer pool.  May be NULL.
    @param      attributes   A CFDictionaryRef containing the attributes to be used for creating new PixelBuffers within the pool.
    @param      poolOut   The newly created pool will be placed here
    @result     Returns kCVReturnSuccess on success
*/
CV_EXPORT CVReturn CVPixelBufferPoolCreate(CFAllocatorRef allocator,
					   CFDictionaryRef poolAttributes,
					   CFDictionaryRef pixelBufferAttributes,
					   CVPixelBufferPoolRef *poolOut) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferPoolGetAttributes
    @abstract   Returns the pool attributes dictionary for a CVPixelBufferPool
    @param      pool  The CVPixelBufferPoolRef to retrieve the attributes from
    @result     Returns the pool attributes dictionary, or NULL on failure.
*/
CV_EXPORT CFDictionaryRef CVPixelBufferPoolGetAttributes(CVPixelBufferPoolRef pool) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferPoolGetPixelBufferAttributes
    @abstract   Returns the attributes of pixel buffers that will be created from this pool.
    @discussion This function is provided for those cases where you may need to know some information about the buffers that
                will be created up front.
    @param      pool  The CVPixelBufferPoolRef to retrieve the attributes from
    @result     Returns the pixel buffer attributes dictionary, or NULL on failure.
*/
CV_EXPORT CFDictionaryRef CVPixelBufferPoolGetPixelBufferAttributes(CVPixelBufferPoolRef pool) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

/*!
    @function   CVPixelBufferPoolCreatePixelBuffer
    @abstract   Creates a new PixelBuffer object from the pool.
    @discussion The function creates a new (attachment-free) CVPixelBuffer using the pixel buffer attributes specifed during pool creation.
    @param      allocator The CFAllocatorRef to use for creating the pixel buffer.  May be NULL.
    @param      pool      The CVPixelBufferPool that should create the new CVPixelBuffer.
    @param      pixelBufferOut   The newly created pixel buffer will be placed here
    @result     Returns kCVReturnSuccess on success
*/
CV_EXPORT CVReturn CVPixelBufferPoolCreatePixelBuffer(CFAllocatorRef allocator, 
						         CVPixelBufferPoolRef pixelBufferPool,
							 CVPixelBufferRef *pixelBufferOut) AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER;

#if defined(__cplusplus)
}
#endif

#endif
