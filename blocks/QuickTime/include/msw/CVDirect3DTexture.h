/*
 *  CVDirect3DTexture.h
 *  CoreVideo
 *
 *  Copyright (c) 2004 Apple Computer, Inc. All rights reserved.
 *
 */
 
 /*! @header CVDirect3DTexture.h
	@copyright 2004 Apple Computer, Inc. All rights reserved.
    @discussion A CoreVideo Texture derives from an ImageBuffer, and is used for supplying source image data to Direct3D.
    		   
*/

#if !defined(__COREVIDEO_CVDIRECT3DTEXTURE_H__)
#define __COREVIDEO_CVDIRECT3DTEXTURE_H__ 1

#include <CVBase.h>
#include <CVReturn.h>
#include <CVImageBuffer.h>

#if defined(__cplusplus)
extern "C" {
#endif

#pragma mark CVDirect3DTexture

/*!
    @typedef	CVDirect3DTextureRef
    @abstract   Direct3D texture based image buffer

*/
typedef CVImageBufferRef CVDirect3DTextureRef;

CV_EXPORT CFTypeID CVDirect3DTextureGetTypeID();

/*!
    @function   CVDirect3DTextureRetain
    @abstract   Retains a CVDirect3DTexture object
    @discussion Equivalent to CFRetain, but NULL safe
    @param      buffer A CVDirect3DTexture object that you want to retain.
    @result     A CVDirect3DTexture object that is the same as the passed in buffer.
*/
CV_EXPORT CVDirect3DTextureRef CVDirect3DTextureRetain( CVDirect3DTextureRef texture );

/*!
    @function   CVDirect3DTextureRelease
    @abstract   Releases a CVDirect3DTexture object
    @discussion Equivalent to CFRelease, but NULL safe
    @param      buffer A CVDirect3DTexture object that you want to release.
*/
CV_EXPORT void CVDirect3DTextureRelease( CVDirect3DTextureRef texture );

/*!
    @function   CVDirect3DTextureGetName
    @abstract   Returns the raw texture associated with the CVDirect3DTexture
    @param      image Target CVDirect3DTexture
    @result     an LPDIRECT3DTEXTURE9 pointing to the texture
*/
CV_EXPORT void* CVDirect3DTextureGetName( CVDirect3DTextureRef image);

/*!
    @function   CVDirect3DTextureIsFlipped
    @abstract   Returns whether the image is flipped vertically or not.
    @param      image Target CVDirect3DTexture
    @result     True if 0,0 in the texture is upper left, false if 0,0 is lower left
*/
CV_EXPORT Boolean CVDirect3DTextureIsFlipped( CVDirect3DTextureRef image);

/*!
    @function   CVDirect3DTextureGetCleanTexCoords 
    @abstract   Returns convenient texture coordinates for the part of the image that should be displayed
    @discussion This function automatically takes into account whether or not the texture is flipped.  It returns
				texture coordinate values from 0.0 to 1.0 ready for use in a Direct3D VertexBuffer.
    @param      image Target CVDirect3DTexture
    @param      lowerLeft  - array of two floats where the s and t texture coordinates of the lower left corner of the image will be stored
    @param      lowerRight - array of two floats where the s and t texture coordinates of the lower right corner of the image will be stored
    @param      upperRight - array of two floats where the s and t texture coordinates of the upper right corner of the image will be stored
    @param      upperLeft  - array of two floats where the s and t texture coordinates of the upper right corner of the image will be stored
*/
CV_EXPORT void CVDirect3DTextureGetCleanTexCoords( CVDirect3DTextureRef image, 
                                                 float lowerLeft[2], 
						 float lowerRight[2], 
						 float upperRight[2],
						 float upperLeft[2]);

#if defined(__cplusplus)
}
#endif

#endif
