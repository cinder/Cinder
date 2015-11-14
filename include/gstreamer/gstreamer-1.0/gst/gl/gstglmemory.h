/*
 * GStreamer
 * Copyright (C) 2012 Matthew Waters <ystreet00@gmail.com>
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

#ifndef _GST_GL_MEMORY_H_
#define _GST_GL_MEMORY_H_

#include <gst/gst.h>
#include <gst/gstallocator.h>
#include <gst/gstmemory.h>
#include <gst/video/video.h>

#include <gst/gl/gstglbasebuffer.h>
#include <gst/gl/gl.h>

G_BEGIN_DECLS

#define GST_TYPE_GL_ALLOCATOR (gst_gl_allocator_get_type())
GType gst_gl_allocator_get_type(void);

#define GST_IS_GL_ALLOCATOR(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_GL_ALLOCATOR))
#define GST_IS_GL_ALLOCATOR_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_GL_ALLOCATOR))
#define GST_GL_ALLOCATOR_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_GL_ALLOCATOR, GstGLAllocatorClass))
#define GST_GL_ALLOCATOR(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_GL_ALLOCATOR, GstGLAllocator))
#define GST_GL_ALLOCATOR_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_GL_ALLOCATOR, GstGLAllocatorClass))
#define GST_GL_ALLOCATOR_CAST(obj)            ((GstGLAllocator *)(obj))

typedef enum _GstGLMemoryTransfer
{
  /* force a transfer between the texture and the PBO (if available) */
  GST_GL_MEMORY_TRANSFER_NEED_UPLOAD = (1 << 0),
  GST_GL_MEMORY_TRANSFER_NEED_DOWNLOAD = (1 << 1),
} GstGLMemoryTransfer;

#define GST_GL_MEMORY_ADD_TRANSFER(mem,state) ((GstGLMemory *)mem)->transfer_state |= state

/**
 * GstGLMemory:
 * @mem: the parent object
 * @context: the #GstGLContext to use for GL operations
 * @tex_id: the GL texture id for this memory
 * @tex_target: the GL texture target for this memory
 * @tex_type: the texture type
 * @info: the texture's #GstVideoInfo
 * @valign: data alignment for system memory mapping
 * @plane: data plane in @info
 * @tex_scaling: GL shader scaling parameters for @valign and/or width/height
 *
 * Represents information about a GL texture
 */
struct _GstGLMemory
{
  GstGLBaseBuffer       mem;

  guint                 tex_id;
  guint                 tex_target;
  GstVideoGLTextureType tex_type;
  GstVideoInfo          info;
  GstVideoAlignment     valign;
  guint                 plane;
  gfloat                tex_scaling[2];

  /* <private> */
  GstGLMemoryTransfer   transfer_state;
  gboolean              texture_wrapped;
  GDestroyNotify        notify;
  gpointer              user_data;
  guint                 unpack_length;
  guint                 tex_width;
};

#define GST_CAPS_FEATURE_MEMORY_GL_MEMORY "memory:GLMemory"
#define GST_GL_MEMORY_VIDEO_FORMATS_STR \
    "{ RGBA, BGRA, RGBx, BGRx, ARGB, ABGR, xRGB, xBGR, RGB, BGR, RGB16, BGR16, " \
    "AYUV, I420, YV12, NV12, NV21, YUY2, UYVY, Y41B, Y42B, Y444, " \
    "GRAY8, GRAY16_LE, GRAY16_BE }"

/**
 * GST_GL_MEMORY_ALLOCATOR:
 *
 * The name of the GL memory allocator
 */
#define GST_GL_MEMORY_ALLOCATOR   "GLMemory"

void          gst_gl_memory_init (void);
gboolean      gst_is_gl_memory (GstMemory * mem);

GstMemory *   gst_gl_memory_alloc   (GstGLContext * context,
                                     GstAllocationParams *params,
                                     GstVideoInfo * info,
                                     guint plane,
                                     GstVideoAlignment *valign);
GstGLMemory * gst_gl_memory_wrapped (GstGLContext * context,
                                     GstVideoInfo * info,
                                     guint plane,
                                     GstVideoAlignment *valign,
                                     gpointer data,
                                     gpointer user_data,
                                     GDestroyNotify notify);
GstGLMemory * gst_gl_memory_wrapped_texture (GstGLContext * context,
                                             guint texture_id,
                                             guint texture_target,
                                             GstVideoInfo * info,
                                             guint plane,
                                             GstVideoAlignment *valign,
                                             gpointer user_data,
                                             GDestroyNotify notify);

void          gst_gl_memory_download_transfer (GstGLMemory * gl_mem);
void          gst_gl_memory_upload_transfer   (GstGLMemory * gl_mem);

gboolean      gst_gl_memory_copy_into_texture (GstGLMemory *gl_mem,
                                               guint tex_id,
                                               GstVideoGLTextureType tex_type,
                                               gint width,
                                               gint height,
                                               gint stride,
                                               gboolean respecify);

gboolean      gst_gl_memory_setup_buffer  (GstGLContext * context,
                                           GstAllocationParams * params,
                                           GstVideoInfo * info,
                                           GstVideoAlignment *valign,
                                           GstBuffer * buffer);
gboolean      gst_gl_memory_setup_wrapped (GstGLContext * context,
                                           GstVideoInfo * info,
                                           GstVideoAlignment *valign,
                                           gpointer data[GST_VIDEO_MAX_PLANES],
                                           GstGLMemory *textures[GST_VIDEO_MAX_PLANES],
                                           gpointer user_data,
                                           GDestroyNotify notify);

gint          gst_gl_memory_get_texture_width  (GstGLMemory * gl_mem);
gint          gst_gl_memory_get_texture_height (GstGLMemory * gl_mem);

/* utility functions */
GstVideoGLTextureType gst_gl_texture_type_from_format (GstGLContext *context,
                                                       GstVideoFormat v_format,
                                                       guint plane);
guint                 gst_gl_format_from_gl_texture_type (GstVideoGLTextureType tex_format);
guint                 gst_gl_sized_gl_format_from_gl_format_type (GstGLContext * context,
                                                                  guint format,
                                                                  guint type);

/**
 * GstGLAllocator
 *
 * Opaque #GstGLAllocator struct
 */
struct _GstGLAllocator
{
  GstGLBaseBufferAllocator parent;
  GstMemoryCopyFunction fallback_mem_copy;
};

/**
 * GstGLAllocatorClass:
 *
 * The #GstGLAllocatorClass only contains private data
 */
struct _GstGLAllocatorClass
{
  GstGLBaseBufferAllocatorClass parent_class;
};

G_END_DECLS

#endif /* _GST_GL_MEMORY_H_ */
