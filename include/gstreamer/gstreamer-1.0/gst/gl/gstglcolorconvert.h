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

#ifndef __GST_GL_COLOR_CONVERT_H__
#define __GST_GL_COLOR_CONVERT_H__

#include <gst/video/video.h>
#include <gst/gstmemory.h>

#include <gst/gl/gstgl_fwd.h>

G_BEGIN_DECLS

GType gst_gl_color_convert_get_type (void);
#define GST_TYPE_GL_COLOR_CONVERT (gst_gl_color_convert_get_type())
#define GST_GL_COLOR_CONVERT(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_GL_COLOR_CONVERT,GstGLColorConvert))
#define GST_GL_COLOR_CONVERT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_GL_DISPLAY,GstGLColorConvertClass))
#define GST_IS_GL_COLOR_CONVERT(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GL_COLOR_CONVERT))
#define GST_IS_GL_COLOR_CONVERT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_GL_COLOR_CONVERT))
#define GST_GL_COLOR_CONVERT_CAST(obj) ((GstGLColorConvert*)(obj))

/**
 * GstGLColorConvert
 *
 * Opaque #GstGLColorConvert object
 */
struct _GstGLColorConvert
{
  /* <private> */
  GstObject        parent;

  GstGLContext    *context;

  /* input data */
  GstVideoInfo     in_info;
  GstVideoInfo     out_info;

  gboolean         initted;
  gboolean         passthrough;

  GstBuffer *    inbuf;
  GstBuffer *    outbuf;

  /* used for the conversion */
  GLuint           fbo;
  GLuint           depth_buffer;
  GstGLShader     *shader;

  /* <private> */
  GstGLColorConvertPrivate *priv;

  gpointer _reserved[GST_PADDING];
};

/**
 * GstGLColorConvertClass:
 *
 * The #GstGLColorConvertClass struct only contains private data
 */
struct _GstGLColorConvertClass
{
  GstObjectClass object_class;
};

/**
 * GST_GL_COLOR_CONVERT_FORMATS:
 *
 * The currently supported formats that can be converted
 */
#define GST_GL_COLOR_CONVERT_FORMATS "{ RGB, RGBx, RGBA, BGR, BGRx, BGRA, xRGB, " \
                               "xBGR, ARGB, ABGR, Y444, I420, YV12, Y42B, " \
                               "Y41B, NV12, NV21, YUY2, UYVY, AYUV, " \
                               "GRAY8, GRAY16_LE, GRAY16_BE, RGB16, BGR16 }"

/**
 * GST_GL_COLOR_CONVERT_VIDEO_CAPS:
 *
 * The currently supported #GstCaps that can be converted
 */
#define GST_GL_COLOR_CONVERT_VIDEO_CAPS \
    GST_VIDEO_CAPS_MAKE_WITH_FEATURES (GST_CAPS_FEATURE_MEMORY_GL_MEMORY, \
        GST_GL_COLOR_CONVERT_FORMATS)

#define GST_GL_COLOR_CONVERT_VIDEO_OVERLAY_COMPOSITION_CAPS \
    GST_VIDEO_CAPS_MAKE_WITH_FEATURES (GST_CAPS_FEATURE_MEMORY_GL_MEMORY \
        "," GST_CAPS_FEATURE_META_GST_VIDEO_OVERLAY_COMPOSITION, \
        GST_GL_COLOR_CONVERT_FORMATS)

GstGLColorConvert * gst_gl_color_convert_new (GstGLContext * context);

GstCaps *   gst_gl_color_convert_transform_caps (GstGLContext * convert,
                                                 GstPadDirection direction,
                                                 GstCaps * caps,
                                                 GstCaps * filter);
gboolean    gst_gl_color_convert_set_caps    (GstGLColorConvert * convert,
                                              GstCaps           * in_caps,
                                              GstCaps           * out_caps);

GstBuffer * gst_gl_color_convert_perform    (GstGLColorConvert * convert, GstBuffer * inbuf);

G_END_DECLS

#endif /* __GST_GL_COLOR_CONVERT_H__ */
