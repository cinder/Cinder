/*
 *  CVDirect3DBuffer.h
 *  CoreVideo
 *
 *  Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
 *
 */
 
 /*! @header CVDirect3DBuffer.h
	@copyright 2004 Apple Computer, Inc. All rights reserved.
    @discussion A CoreVideo buffer derives from a generic buffer and can be an ImageBuffer or PixelBuffer. 
		   
*/

#if !defined(__COREVIDEO_CVDIRECT3DBUFFER_H__)
#define __COREVIDEO_CVDIRECT3DBUFFER_H__ 1

#include <CVImageBuffer.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define kCVDirect3DBufferWidth				CFSTR("Width")
#define kCVDirect3DBufferHeight				CFSTR("Height")
#define kCVDirect3DBufferTarget				CFSTR("Direct3DTarget")
#define kCVDirect3DBufferInternalFormat		CFSTR("Direct3DInternalFormat")
#define kCVDirect3DBufferMaximumMipmapLevel	CFSTR("MaximumMipmapLevel")

typedef CVImageBufferRef CVDirect3DBufferRef;

CV_EXPORT CFTypeID CVDirect3DBufferGetTypeID();

/*!
    @function   CVDirect3DBufferRetain
    @abstract   Retains a CVDirect3DBuffer object
    @discussion Equivalent to CFRetain, but NULL safe
    @param      buffer A CVDirect3DBuffer object that you want to retain.
    @result     A CVDirect3DBuffer object that is the same as the passed in buffer.
*/
CV_EXPORT CVDirect3DBufferRef CVDirect3DBufferRetain( CVDirect3DBufferRef texture );

/*!
    @function   CVDirect3DBufferRelease
    @abstract   Releases a CVDirect3DBuffer object
    @discussion Equivalent to CFRelease, but NULL safe
    @param      buffer A CVDirect3DBuffer object that you want to release.
*/
CV_EXPORT void CVDirect3DBufferRelease( CVDirect3DBufferRef texture );

/*!
    @function   CVDirect3DBufferCreate
    @abstract   Create a new CVDirect3DBuffer that may be used for D3D rendering purposes
    @param      width The width of the buffer in pixels
    @param      height The height of the buffer in pixels
    @param      attributes A CFDictionaryRef containing other desired attributes of the buffer (texture format, max mipmap level, etc.).
					May be NULL. 
    @param      bufferOut    The newly created buffer will be placed here.
    @result     kCVReturnSuccess if the attachment succeeded
*/
CV_EXPORT CVReturn CVDirect3DBufferCreate(CFAllocatorRef allocator, size_t width, size_t height, void *d3dDevice, CFDictionaryRef attributes, CVDirect3DBufferRef *bufferOut);

/*!
    @function   CVDirect3DBufferGetAttributes
    @param      openGLBuffer Target D3D Buffer.
    @result     CVDirect3DBuffer attributes dictionary, NULL if not set.
*/
CV_EXPORT CFDictionaryRef CVDirect3DBufferGetAttributes(CVDirect3DBufferRef d3DBuffer);


#if defined(__cplusplus)
}
#endif

#endif
