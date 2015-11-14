/*
 * Copyright (c) 2007-2009 Intel Corporation. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL INTEL AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _VA_TPI_H_
#define _VA_TPI_H_

#include <va/va.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    VAExternalMemoryNULL, /* it is not external buffer, but requires the implementation allocates
                           * the surface with the input attribute
                           */
    VAExternalMemoryV4L2Buffer,
    VAExternalMemoryCIFrame, /* the memory is from camera frames and buffers points the frame ID list */
    VAExternalMemoryUserPointer, /* the memory is malloc-ed and buffers points to the buffers */
    VAExternalMemoryKernelDRMBufffer, /* the memory is from kernel DRM buffers and buffers points the
                                       * DRM buffer handle list
                                       */
    VAExternalMemoryAndroidGrallocBuffer, /* the memory is from Android Gralloc memory, and buffers points
                                           * the gralloc native_handle_t list
                                           */
} VASurfaceMemoryType;

typedef struct _VASurfaceAttributeTPI {
    VASurfaceMemoryType type;
    unsigned int width;
    unsigned int height;
    unsigned int size;
    unsigned int pixel_format; /* buffer format */
    unsigned int tiling; /* the memory is tiling or not */
    unsigned int luma_stride; /* luma stride, could be width aligned with a special value */
    unsigned int chroma_u_stride; /* chroma stride */
    unsigned int chroma_v_stride;
    unsigned int luma_offset; /* could be 0 */
    unsigned int chroma_u_offset; /* U offset from the beginning of the memory */
    unsigned int chroma_v_offset; /* V offset from the beginning of the memory */
    unsigned int count; /* buffer count for surface creation */
    unsigned int *buffers; /* buffer handles or user pointers */
    unsigned int reserved[4]; /* used to pass additional information, like
                               * Android native window pointer
                               */
} VASurfaceAttributeTPI;


VAStatus vaPutSurfaceBuf (
    VADisplay dpy,
    VASurfaceID surface,
    unsigned char* data,
    int* data_len,
    short srcx,
    short srcy,
    unsigned short srcw,
    unsigned short srch,
    short destx,
    short desty,
    unsigned short destw,
    unsigned short desth,
    VARectangle *cliprects, /* client supplied clip list */
    unsigned int number_cliprects, /* number of clip rects in the clip list */
    unsigned int flags /* de-interlacing flags */
);


VAStatus vaCreateSurfacesWithAttribute (
    VADisplay dpy,
    int width,
    int height,
    int format,
    int num_surfaces,
    VASurfaceID *surfaces,       /* out */
    VASurfaceAttributeTPI *attribute_tpi
);

#ifdef __cplusplus
}
#endif

#endif 
