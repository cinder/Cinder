/*
 *  CVDirect3DTextureCache.h
 *  CoreVideo
 *
 *  Copyright 2004 Apple Computer, Inc. All rights reserved.
 *
 */

#if !defined(__COREVIDEO__CVDIRECT3DTEXTURECACHE_H__)
#define __COREVIDEO__CVDIRECT3DTEXTURECACHE_H__ 1

#include <CVBase.h>
#include <CVReturn.h>
#include <CVBuffer.h>
#include <CVDirect3DTexture.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*!
    @typedef	CVDirect3DTextureCacheRef
    @abstract   CoreVideo Direct3D Texture Cache

*/
typedef struct __CVDirect3DTextureCache *CVDirect3DTextureCacheRef;

CV_EXPORT CFTypeID CVDirect3DTextureCacheGetTypeID();

/*!
    @function   CVDirect3DTextureCacheRetain
    @abstract   Retains a CVDirect3DTextureCache object
    @discussion Equivalent to CFRetain, but NULL safe
    @param      buffer A CVDirect3DTextureCache object that you want to retain.
    @result     A CVDirect3DTextureCache object that is the same as the passed in buffer.
*/
CV_EXPORT CVDirect3DTextureCacheRef CVDirect3DTextureCacheRetain( CVDirect3DTextureCacheRef textureCache ); // NULL-safe

/*!
    @function   CVDirect3DTextureCacheRelease
    @abstract   Releases a CVDirect3DTextureCache object
    @discussion Equivalent to CFRelease, but NULL safe
    @param      buffer A CVDirect3DTextureCache object that you want to release.
*/
CV_EXPORT void CVDirect3DTextureCacheRelease( CVDirect3DTextureCacheRef textureCache ); // NULL-safe

/*!
    @function   CVDirect3DTextureCacheCreate
    @abstract   Creates a new Texture Cache.
    @param      allocator The CFAllocatorRef to use for allocating the cache.  May be NULL.
    @param      cacheAttributes A CFDictionaryRef containing the attributes of the cache itself.   May be NULL.
    @param      cglContext The D3D context into which the texture objects will be created
    @param      cglPixelFormat The D3D pixel format object used to create the passed in D3D context
    @param      textureAttributes A CFDictionaryRef containing the attributes to be used for creating the CVDirect3DTexture objects.  May be NULL.
    @param      cacheOut   The newly created texture cache will be placed here
    @result     Returns kCVReturnSuccess on success
*/
CV_EXPORT CVReturn CVDirect3DTextureCacheCreate(
					CFAllocatorRef allocator,
					CFDictionaryRef cacheAttributes,
					void *d3dDevice,  /*CVDIRECT3DDEVICE*/
					UInt32 d3dFormat, /*D3DFORMAT*/
					CFDictionaryRef textureAttributes,
					CVDirect3DTextureCacheRef *cacheOut);

/*!
    @function   CVDirect3DTextureCacheCreateTextureFromImage
    @abstract   Creates a CVDirect3DTexture object from an existing CVImageBuffer
    @param      allocator The CFAllocatorRef to use for allocating the CVDirect3DTexture object.  May be NULL.
    @param      sourceImage The CVImageBuffer that you want to create a CVDirect3DTexture from.
    @param      attribuse The desired buffer attributes for the CVDirect3DTexture.
    @param      textureOut The newly created texture object will be placed here.
    @result     Returns kCVReturnSuccess on success
*/
CV_EXPORT CVReturn CVDirect3DTextureCacheCreateTextureFromImage(CFAllocatorRef allocator,
								       CVDirect3DTextureCacheRef textureCache,
								       CVImageBufferRef sourceImage,
								       CFDictionaryRef *attribs,
								       CVDirect3DTextureRef *textureOut);

/*!
    @function   CVDirect3DTextureCacheFlush
    @abstract   Performs internal housekeeping/recycling operations
    @discussion This call must be made periodically to give the texture cache a chance to make D3D calls
                on the Direct3D context used to create it in order to do housekeeping operations.
    @param      textureCache The texture cache object to flush
    @param      options Currently unused, set to 0.
    @result     Returns kCVReturnSuccess on success
*/
CV_EXPORT void CVDirect3DTextureCacheFlush(CVDirect3DTextureCacheRef textureCache, CVOptionFlags options);

#if defined(__cplusplus)
}
#endif

#endif
