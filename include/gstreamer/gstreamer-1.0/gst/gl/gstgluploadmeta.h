/*
 * GStreamer
 * Copyright (C) 2012 Matthew Waters <ystree00@gmail.com>
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

#ifndef __GST_GL_UPLOAD_META_H__
#define __GST_GL_UPLOAD_META_H__

#include <gst/video/video.h>
#include <gst/gstmemory.h>

#include <gst/gl/gstgl_fwd.h>

G_BEGIN_DECLS

GType gst_gl_upload_meta_get_type (void);
#define GST_TYPE_GL_UPLOAD_META (gst_gl_upload_meta_get_type())
#define GST_GL_UPLOAD_META(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_GL_UPLOAD_META,GstGLUploadMeta))
#define GST_GL_UPLOAD_META_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_GL_UPLOAD_META,GstGLUploadMetaClass))
#define GST_IS_GL_UPLOAD_META(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GL_UPLOAD_META))
#define GST_IS_GL_UPLOAD_META_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_GL_UPLOAD_META))
#define GST_GL_UPLOAD_META_CAST(obj) ((GstGLUploadMeta*)(obj))

/**
 * GstGLUploadMeta
 *
 * Opaque #GstGLUploadMeta object
 */
struct _GstGLUploadMeta
{
  /* <private> */
  GstObject        parent;

  GstGLContext    *context;

  /* input data */
  GstVideoInfo     info;

  /* <private> */
  GstGLUploadMetaPrivate *priv;

  gpointer _reserved[GST_PADDING];
};

/**
 * GstGLUploadMetaClass:
 *
 * The #GstGLUploadMetaClass struct only contains private data
 */
struct _GstGLUploadMetaClass
{
  GstObjectClass object_class;
};

GstGLUploadMeta * gst_gl_upload_meta_new            (GstGLContext * context);

void           gst_gl_upload_meta_set_format    (GstGLUploadMeta * upload, GstVideoInfo * info);
GstVideoInfo * gst_gl_upload_meta_get_format    (GstGLUploadMeta * upload);

gboolean       gst_gl_upload_meta_add_to_buffer (GstGLUploadMeta * upload, GstBuffer * buffer);

G_END_DECLS

#endif /* __GST_GL_UPLOAD_META_H__ */
