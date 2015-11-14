/*
 * GStreamer
 * Copyright (C) 2014 Matthew Waters <matthew@centricular.com>
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

#ifndef __GST_GL_SYNC_META_H__
#define __GST_GL_SYNC_META_H__

#include <gst/gl/gstgl_fwd.h>

G_BEGIN_DECLS

#define GST_GL_SYNC_META_API_TYPE (gst_gl_sync_meta_api_get_type())
#define GST_GL_SYNC_META_INFO     (gst_gl_sync_meta_get_info())
typedef struct _GstGLSyncMeta GstGLSyncMeta;

#define GST_BUFFER_POOL_OPTION_GL_SYNC_META "GstBufferPoolOptionGLSyncMeta"

struct _GstGLSyncMeta {
  /*< private >*/
  GstMeta parent;

  GstGLContext *context;

  GLsync        glsync;
};

GType gst_gl_sync_meta_api_get_type (void);
const GstMetaInfo * gst_gl_sync_meta_get_info (void);

#define gst_buffer_get_gl_sync_meta(b) ((GstGLSyncMeta*)gst_buffer_get_meta((b),GST_GL_SYNC_META_API_TYPE))

GstGLSyncMeta *     gst_buffer_add_gl_sync_meta         (GstGLContext * context, GstBuffer *buffer);
void                gst_gl_sync_meta_set_sync_point     (GstGLSyncMeta * sync, GstGLContext * context);
void                gst_gl_sync_meta_wait               (GstGLSyncMeta * sync, GstGLContext * context);

G_END_DECLS

#endif /* __GST_GL_SYNC_META_H__ */
