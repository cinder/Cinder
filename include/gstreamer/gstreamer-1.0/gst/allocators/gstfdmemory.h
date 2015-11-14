/* GStreamer fd memory
 * Copyright (C) 2013 Linaro SA
 * Author: Benjamin Gaignard <benjamin.gaignard@linaro.org> for Linaro.
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GST_FD_ALLOCATOR_H__
#define __GST_FD_ALLOCATOR_H__

#include <gst/gst.h>

G_BEGIN_DECLS

typedef struct _GstFdAllocator GstFdAllocator;
typedef struct _GstFdAllocatorClass GstFdAllocatorClass;

#define GST_ALLOCATOR_FD "fd"

#define GST_TYPE_FD_ALLOCATOR              (gst_fd_allocator_get_type())
#define GST_IS_FD_ALLOCATOR(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_FD_ALLOCATOR))
#define GST_IS_FD_ALLOCATOR_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_FD_ALLOCATOR))
#define GST_FD_ALLOCATOR_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_FD_ALLOCATOR, GstFdAllocatorClass))
#define GST_FD_ALLOCATOR(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_FD_ALLOCATOR, GstFdAllocator))
#define GST_FD_ALLOCATOR_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_FD_ALLOCATOR, GstFdAllocatorClass))
#define GST_FD_ALLOCATOR_CAST(obj)         ((GstFdAllocator *)(obj))

/**
 * GstFdMemoryFlags:
 * @GST_FD_MEMORY_FLAG_NONE: no flag
 * @GST_FD_MEMORY_FLAG_KEEP_MAPPED: once the memory is mapped,
 *        keep it mapped until the memory is destroyed.
 * @GST_FD_MEMORY_FLAG_MAP_PRIVATE: do a private mapping instead of
 *        the default shared mapping.
 *
 * Various flags to control the operation of the fd backed memory.
 *
 * Since: 1.6
 */
typedef enum {
  GST_FD_MEMORY_FLAG_NONE = 0,
  GST_FD_MEMORY_FLAG_KEEP_MAPPED = (1 << 0),
  GST_FD_MEMORY_FLAG_MAP_PRIVATE = (1 << 1),
} GstFdMemoryFlags;

/**
 * GstFdAllocator:
 *
 * Base class for allocators with fd-backed memory
 *
 * Since: 1.6
 */
struct _GstFdAllocator
{
  GstAllocator parent;
};

struct _GstFdAllocatorClass
{
  GstAllocatorClass parent_class;
};

GType gst_fd_allocator_get_type (void);

GstAllocator *  gst_fd_allocator_new    (void);
GstMemory *     gst_fd_allocator_alloc  (GstAllocator * allocator, gint fd,
                                         gsize size, GstFdMemoryFlags flags);

gboolean        gst_is_fd_memory        (GstMemory *mem);
gint            gst_fd_memory_get_fd    (GstMemory *mem);

G_END_DECLS

#endif /* __GST_FD_ALLOCATOR_H__ */
