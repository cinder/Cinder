/*
 * GStreamer
 * Copyright (C) 2013 Julien Isorce <julien.isorce@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GST_GL_FWD_H__
#define __GST_GL_FWD_H__

#include <gst/gst.h>

#include <gst/gl/gstglapi.h>

G_BEGIN_DECLS

typedef struct _GstGLDisplay GstGLDisplay;
typedef struct _GstGLDisplayClass GstGLDisplayClass;
typedef struct _GstGLDisplayPrivate GstGLDisplayPrivate;

typedef struct _GstGLContext GstGLContext;
typedef struct _GstGLContextClass GstGLContextClass;
typedef struct _GstGLContextPrivate GstGLContextPrivate;

typedef struct _GstGLWindow        GstGLWindow;
typedef struct _GstGLWindowPrivate GstGLWindowPrivate;
typedef struct _GstGLWindowClass   GstGLWindowClass;

typedef struct _GstGLBaseBuffer GstGLBaseBuffer;
typedef struct _GstGLBaseBufferAllocator GstGLBaseBufferAllocator;
typedef struct _GstGLBaseBufferAllocatorClass GstGLBaseBufferAllocatorClass;

typedef struct _GstGLMemory GstGLMemory;
typedef struct _GstGLAllocator GstGLAllocator;
typedef struct _GstGLAllocatorClass GstGLAllocatorClass;

typedef struct _GstGLShader        GstGLShader;
typedef struct _GstGLShaderPrivate GstGLShaderPrivate;
typedef struct _GstGLShaderClass   GstGLShaderClass;

typedef struct _GstGLDownload GstGLDownload;
typedef struct _GstGLDownloadClass GstGLDownloadClass;
typedef struct _GstGLDownloadPrivate GstGLDownloadPrivate;

typedef struct _GstGLUpload GstGLUpload;
typedef struct _GstGLUploadClass GstGLUploadClass;
typedef struct _GstGLUploadPrivate GstGLUploadPrivate;

typedef struct _GstGLUploadMeta GstGLUploadMeta;
typedef struct _GstGLUploadMetaClass GstGLUploadMetaClass;
typedef struct _GstGLUploadMetaPrivate GstGLUploadMetaPrivate;

typedef struct _GstGLColorConvert GstGLColorConvert;
typedef struct _GstGLColorConvertClass GstGLColorConvertClass;
typedef struct _GstGLColorConvertPrivate GstGLColorConvertPrivate;

typedef struct _GstGLBaseFilter GstGLBaseFilter;
typedef struct _GstGLBaseFilterClass GstGLBaseFilterClass;
typedef struct _GstGLBaseFilterPrivate GstGLBaseFilterPrivate;

typedef struct _GstGLFilter GstGLFilter;
typedef struct _GstGLFilterClass GstGLFilterClass;

typedef struct _GstGLViewConvert GstGLViewConvert;
typedef struct _GstGLViewConvertClass GstGLViewConvertClass;
typedef struct _GstGLViewConvertPrivate GstGLViewConvertPrivate;

typedef struct _GstGLOverlayCompositor GstGLOverlayCompositor;
typedef struct _GstGLOverlayCompositorClass GstGLOverlayCompositorClass;

G_END_DECLS

#endif /* __GST_GL_FWD_H__ */
