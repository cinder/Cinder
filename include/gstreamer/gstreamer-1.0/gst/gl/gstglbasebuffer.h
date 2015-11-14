/*
 * GStreamer
 * Copyright (C) 2015 Matthew Waters <matthew@centricular.com>
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

#ifndef _GST_GL_BASE_BUFFER_H_
#define _GST_GL_BASE_BUFFER_H_

#include <gst/gst.h>
#include <gst/gstallocator.h>
#include <gst/gstmemory.h>

#include <gst/gl/gstgl_fwd.h>

G_BEGIN_DECLS

#define GST_TYPE_GL_BASE_BUFFER_ALLOCATOR (gst_gl_base_buffer_allocator_get_type())
GType gst_gl_base_buffer_allocator_get_type(void);

#define GST_IS_GL_BASE_BUFFER_ALLOCATOR(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_GL_ALLOCATOR))
#define GST_IS_GL_BASE_BUFFER_ALLOCATOR_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_GL_BASE_BUFFER_ALLOCATOR))
#define GST_GL_BASE_BUFFER_ALLOCATOR_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_GL_BASE_BUFFER_ALLOCATOR, GstGLBaseBufferAllocatorClass))
#define GST_GL_BASE_BUFFER_ALLOCATOR(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_GL_BASE_BUFFER_ALLOCATOR, GstGLBaseBufferAllocator))
#define GST_GL_BASE_BUFFER_ALLOCATOR_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_GL_BASE_BUFFER_ALLOCATOR, GstGLBaseBufferAllocatorClass))
#define GST_GL_BASE_BUFFER_ALLOCATOR_CAST(obj)            ((GstGLBaseBufferAllocator *)(obj))

GQuark gst_gl_base_buffer_error_quark (void);
#define GST_GL_BASE_BUFFER_ERROR (gst_gl_base_buffer_error_quark ())

typedef enum
{
  GST_GL_BASE_BUFFER_ERROR_FAILED,
  GST_GL_BASE_BUFFER_ERROR_OLD_LIBS,
  GST_GL_BASE_BUFFER_ERROR_RESOURCE_UNAVAILABLE,
} GstGLBufferError;

typedef enum
{
  GST_GL_BASE_BUFFER_FLAG_NEED_DOWNLOAD   = (GST_MEMORY_FLAG_LAST << 0),
  GST_GL_BASE_BUFFER_FLAG_NEED_UPLOAD     = (GST_MEMORY_FLAG_LAST << 1)
} GstGLBaseBufferFlags;

/**
 * GST_MAP_GL:
 *
 * Flag indicating that we should map the GL object instead of to system memory.
 *
 * Combining #GST_MAP_GL with #GST_MAP_WRITE has the same semantics as though
 * you are writing to OpenGL. Conversely, combining #GST_MAP_GL with
 * #GST_MAP_READ has the same semantics as though you are reading from OpenGL.
 */
#define GST_MAP_GL (GST_MAP_FLAG_LAST << 1)

/**
 * GstGLBaseBuffer:
 * @mem: the parent object
 * @context: the #GstGLContext to use for GL operations
 * @id: the buffer id for this memory
 * @target: the GL target of this texture for binding purposes
 *
 * Represents information about a GL buffer
 */
struct _GstGLBaseBuffer
{
  GstMemory             mem;

  GstGLContext         *context;
  guint                 id;
  guint                 target;

  /* <protected> */
  GMutex                lock;

  GstMapFlags           map_flags;       /* cumulative map flags */
  gint                  map_count;
  gint                  gl_map_count;

  gpointer              data;
  /* <private> */
  gpointer              alloc_data;
};

typedef gboolean          (*GstGLBaseBufferAllocatorCreateFunction)      (GstGLBaseBuffer * buffer, GError ** error);
typedef gpointer          (*GstGLBaseBufferAllocatorMapBufferFunction)   (GstGLBaseBuffer * buffer, GstMapInfo * info, gsize maxsize);
typedef void              (*GstGLBaseBufferAllocatorUnmapBufferFunction) (GstGLBaseBuffer * buffer, GstMapInfo * info);
typedef GstGLBaseBuffer * (*GstGLBaseBufferAllocatorCopyFunction)        (GstGLBaseBuffer * buffer, gssize offset, gssize size);
typedef void              (*GstGLBaseBufferAllocatorDestroyFunction)     (GstGLBaseBuffer * buffer);

/**
 * GstGLBaseBufferAllocator
 *
 * Opaque #GstGLAllocator struct
 */
struct _GstGLBaseBufferAllocator
{
  GstAllocator parent;
};

/**
 * GstGLBaseBufferAllocatorClass:
 *
 * The #GstGLBaseBufferAllocatorClass only contains private data
 */
struct _GstGLBaseBufferAllocatorClass
{
  GstAllocatorClass parent_class;

  GstGLBaseBufferAllocatorCreateFunction      create;
  GstGLBaseBufferAllocatorMapBufferFunction   map_buffer;
  GstGLBaseBufferAllocatorUnmapBufferFunction unmap_buffer;
  GstGLBaseBufferAllocatorCopyFunction        copy;
  GstGLBaseBufferAllocatorDestroyFunction     destroy;
};

#include <gst/gl/gl.h>

#define GST_CAPS_FEATURE_MEMORY_GL_BUFFER "memory:GLBuffer"

/**
 * GST_GL_BASE_BUFFER_ALLOCATOR_NAME:
 *
 * The name of the GL buffer allocator
 */
#define GST_GL_BASE_BUFFER_ALLOCATOR_NAME   "GLBuffer"

void          gst_gl_base_buffer_init_once (void);
gboolean      gst_is_gl_base_buffer        (GstMemory * mem);

void          gst_gl_base_buffer_init      (GstGLBaseBuffer * mem,
                                            GstAllocator * allocator,
                                            GstMemory * parent,
                                            GstGLContext * context,
                                            GstAllocationParams * params,
                                            gsize maxsize);

GstGLBaseBuffer * gst_gl_base_buffer_alloc_data  (GstGLBaseBuffer * gl_mem);

gboolean gst_gl_base_buffer_copy_buffer_sub_data (GstGLBaseBuffer * src,
                                                  GstGLBaseBuffer * dest,
                                                  gssize offset,
                                                  gssize size);
gboolean gst_gl_base_buffer_memcpy               (GstGLBaseBuffer * src,
                                                  GstGLBaseBuffer * dest,
                                                  gssize offset,
                                                  gssize size);

G_END_DECLS

#endif /* _GST_GL_BUFFER_H_ */
