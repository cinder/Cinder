/*
 * GStreamer
 * Copyright (C) 2007 David Schleef <ds@schleef.org>
 * Copyright (C) 2008 Julien Isorce <julien.isorce@gmail.com>
 * Copyright (C) 2008 Filippo Argiolas <filippo.argiolas@gmail.com>
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

#ifndef _GST_GL_BASE_FILTER_H_
#define _GST_GL_BASE_FILTER_H_

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include <gst/video/video.h>

#include <gst/gl/gl.h>

G_BEGIN_DECLS

GType gst_gl_base_filter_get_type(void);
#define GST_TYPE_GL_BASE_FILTER            (gst_gl_base_filter_get_type())
#define GST_GL_BASE_FILTER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_GL_BASE_FILTER,GstGLBaseFilter))
#define GST_IS_GL_BASE_FILTER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GL_BASE_FILTER))
#define GST_GL_BASE_FILTER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass) ,GST_TYPE_GL_BASE_FILTER,GstGLBaseFilterClass))
#define GST_IS_GL_BASE_FILTER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass) ,GST_TYPE_GL_BASE_FILTER))
#define GST_GL_BASE_FILTER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj) ,GST_TYPE_GL_BASE_FILTER,GstGLBaseFilterClass))

/**
 * GstGLBaseFilter:
 * @base_transform: parent #GstBaseTransform
 * @display: the currently configured #GstGLDisplay
 * @context: the currently configured #GstGLContext
 *
 * #GstGLBaseFilter is a base class that provides the logic of getting the
 * GL context from the pipeline.
 */
struct _GstGLBaseFilter
{
  GstBaseTransform   parent;

  GstGLDisplay      *display;
  GstGLContext      *context;

  /* <private> */
  gpointer _padding[GST_PADDING];

  GstGLBaseFilterPrivate *priv;
};

/**
 * GstGLBaseFilterClass:
 * @base_transform_class: parent class
 * @gl_start: called in the GL thread to setup the element GL state.
 * @gl_stop: called in the GL thread to setup the element GL state.
 */
struct _GstGLBaseFilterClass
{
  GstBaseTransformClass parent_class;
  GstGLAPI supported_gl_api;

  gboolean (*gl_start)          (GstGLBaseFilter *filter);
  void     (*gl_stop)           (GstGLBaseFilter *filter);

  gpointer _padding[GST_PADDING];
};

G_END_DECLS

#endif /* _GST_GL_BASE_FILTER_H_ */
