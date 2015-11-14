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

#ifndef __GST_GL_DOWNLOAD_H__
#define __GST_GL_DOWNLOAD_H__

#include <gst/video/video.h>
#include <gst/gstmemory.h>

#include <gst/gl/gl.h>

G_BEGIN_DECLS

GType gst_gl_download_get_type (void);
#define GST_TYPE_GL_DOWNLOAD (gst_gl_download_get_type())
#define GST_GL_DOWNLOAD(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_GL_DOWNLOAD,GstGLDownload))
#define GST_GL_DOWNLOAD_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_GL_DISPLAY,GstGLDownloadClass))
#define GST_IS_GL_DOWNLOAD(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GL_DOWNLOAD))
#define GST_IS_GL_DOWNLOAD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_GL_DOWNLOAD))
#define GST_GL_DOWNLOAD_CAST(obj) ((GstGLDownload*)(obj))

/**
 * GstGLDownload:
 *
 * Opaque #GstGLDownload object
 */
struct _GstGLDownload
{
  /* <private> */
  GstObject        parent;

  GstGLContext     *context;
  GstGLColorConvert *convert;

  /* output data */
  GstVideoInfo     info;

  gboolean         initted;

  GstGLDownloadPrivate *priv;

  gpointer _reserved[GST_PADDING];
};

/**
 * GstGLDownloadClass:
 *
 * The #GstGLDownloadClass struct only contains private data
 */
struct _GstGLDownloadClass
{
  /* <private> */
  GstObjectClass object_class;
};

GstGLDownload * gst_gl_download_new          (GstGLContext * context);

void gst_gl_download_set_format                (GstGLDownload * download, GstVideoInfo * out_info);
GstCaps * gst_gl_download_transform_caps       (GstGLContext * convert,
                                                GstPadDirection direction,
                                                GstCaps * caps,
                                                GstCaps * filter);

gboolean gst_gl_download_perform_with_data          (GstGLDownload * download,
                                                     GLuint texture_id, GLuint texture_target,
                                                     gpointer data[GST_VIDEO_MAX_PLANES]);
gboolean gst_gl_download_perform (GstGLDownload * download,
                                  GstBuffer *inbuf, GstBuffer *outbuf);

G_END_DECLS

#endif /* __GST_GL_DOWNLOAD_H__ */
