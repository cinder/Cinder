/*
 *  CVDirect3DBufferPool.h
 *  CoreVideo
 *
 *  Created by David Eldred based on CVOpenGLBufferPool.h
 *  Copyright 2004 Apple Computer, Inc. All rights reserved.
 *
 */

 /*! @header CVDirect3DBufferPool.h
	@copyright 2004 Apple Computer, Inc. All rights reserved.
    @discussion CVDirect3DBufferPool is a utility object for managing a set of CVDirect3DBuffer objects that are going to be recycled.
		   
*/

#if !defined(__COREVIDEO__CVDIRECT3DBUFFERPOOL_H__)
#define __COREVIDEO__CVDIRECT3DBUFFERPOOL_H__ 1

#include <CVBase.h>
#include <CVReturn.h>
#include <CVDirect3DBuffer.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct __CVDirect3DBufferPool *CVDirect3DBufferPoolRef;

#define kCVDirect3DBufferPoolMinimumBufferCountKey	CFSTR("MinimumBufferCount")
#define kCVDirect3DBufferPoolMaximumBufferAgeKey		CFSTR("MaximumBufferAge")

CV_EXPORT CFTypeID CVDirect3DBufferPoolGetTypeID();

/*!
    @function   CVDirect3DBufferPoolRetain
    @abstract   Retains a CVDirect3DBufferPoolRef object
    @discussion Equivalent to CFRetain, but NULL safe
    @param      buffer A CVDirect3DBufferPoolRef object that you want to retain.
    @result     A CVDirect3DBufferPoolRef object that is the same as the passed in buffer.
*/
CV_EXPORT CVDirect3DBufferPoolRef CVDirect3DBufferPoolRetain( CVDirect3DBufferPoolRef pixelBufferPool ); // NULL-safe

/*!
    @function   CVDirect3DBufferPoolRelease
    @abstract   Releases a CVDirect3DBufferPoolRef object
    @discussion Equivalent to CFRelease, but NULL safe
    @param      buffer A CVDirect3DBufferPoolRef object that you want to release.
*/
CV_EXPORT void CVDirect3DBufferPoolRelease( CVDirect3DBufferPoolRef pixelBufferPool ); // NULL-safe

/*!
    @function   CVDirect3DBufferPoolCreate
    @abstract   Creates a new Pixel Buffer pool.
    @discussion Equivalent to CFRelease, but NULL safe
    @param      allocator The CFAllocatorRef to use for allocating this buffer pool.  May be NULL.
    @param      poolAttributes   A CFDictionaryRef containing the attributes to be used for the pool itself.
    @param      pixelBufferAttributes   A CFDictionaryRef containing the attributes to be used for creating new D3DBuffers within the pool.
    @param      d3dDevice   the LPDIRECT3DDEVICE9 to be used for allocation of buffers for this pool
    @param      poolOut   The newly created pool will be placed here
    @result     Returns kCVReturnSuccess on success
*/
CV_EXPORT CVReturn CVDirect3DBufferPoolCreate(CFAllocatorRef allocator,
					   CFDictionaryRef poolAttributes,
					   CFDictionaryRef pixelBufferAttributes,
					   void *d3dDevice, 
					   CVDirect3DBufferPoolRef *poolOut);

/*!
    @function   CVDirect3DBufferPoolGetAttributes
    @abstract   Returns the pool attributes dictionary for a CVDirect3DBufferPool
    @param      pool  The CVDirect3DBufferPoolRef to retrieve the attributes from
    @result     Returns the pool attributes dictionary, or NULL on failure.
*/
CV_EXPORT CFDictionaryRef CVDirect3DBufferPoolGetAttributes(CVDirect3DBufferPoolRef pool);

/*!
    @function   CVDirect3DBufferPoolGetDirect3DBufferAttributes
    @abstract   Returns the attributes of pixel buffers that will be created from this pool.
    @discussion This function is provided for those cases where you may need to know some information about the buffers that
                will be created up front.
    @param      pool  The CVDirect3DBufferPoolRef to retrieve the attributes from
    @result     Returns the pixel buffer attributes dictionary, or NULL on failure.
*/
CV_EXPORT CFDictionaryRef CVDirect3DBufferPoolGetDirect3DBufferAttributes(CVDirect3DBufferPoolRef pool);

/*!
    @function   CVDirect3DBufferPoolCreateDirect3DBuffer
    @abstract   Creates a new D3DBuffer object from the pool.
    @discussion The function creates a new (attachment-free) CVDirect3DBuffer using the pixel buffer attributes specifed during pool creation.
    @param      allocator The CFAllocatorRef to use for creating the pixel buffer.  May be NULL.
    @param      pool      The CVDirect3DBufferPool that should create the new CVDirect3DBuffer.
    @param      pixelBufferOut   The newly created pixel buffer will be placed here
    @result     Returns kCVReturnSuccess on success
*/
CV_EXPORT CVReturn CVDirect3DBufferPoolCreateDirect3DBuffer(CFAllocatorRef allocator, 
						         CVDirect3DBufferPoolRef pixelBufferPool,
							 CVDirect3DBufferRef *pixelBufferOut);

#if defined(__cplusplus)
}
#endif

#endif
