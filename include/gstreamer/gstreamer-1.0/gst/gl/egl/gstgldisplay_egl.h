/*
 * GStreamer
 * Copyright (C) 2014 Matthew Waters <ystreet00@gmail.com>
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

#ifndef __GST_GL_DISPLAY_EGL_H__
#define __GST_GL_DISPLAY_EGL_H__

#include <gst/gst.h>
#include <gst/gl/gstgldisplay.h>
#include <gst/gl/egl/gstegl.h>

G_BEGIN_DECLS

GType gst_gl_display_egl_get_type (void);

#define GST_TYPE_GL_DISPLAY_EGL             (gst_gl_display_egl_get_type())
#define GST_GL_DISPLAY_EGL(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_GL_DISPLAY_EGL,GstGLDisplayEGL))
#define GST_GL_DISPLAY_EGL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_GL_DISPLAY_EGL,GstGLDisplayEGLClass))
#define GST_IS_GL_DISPLAY_EGL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GL_DISPLAY_EGL))
#define GST_IS_GL_DISPLAY_EGL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_GL_DISPLAY_EGL))
#define GST_GL_DISPLAY_EGL_CAST(obj)        ((GstGLDisplayEGL*)(obj))

typedef struct _GstGLDisplayEGL GstGLDisplayEGL;
typedef struct _GstGLDisplayEGLClass GstGLDisplayEGLClass;

/**
 * GstGLDisplayEGL:
 *
 * the contents of a #GstGLDisplayEGL are private and should only be accessed
 * through the provided API
 */
struct _GstGLDisplayEGL
{
  GstGLDisplay          parent;

  /* <private> */
  EGLDisplay display;

  gboolean foreign_display;
};

struct _GstGLDisplayEGLClass
{
  GstGLDisplayClass object_class;
};

GstGLDisplayEGL *gst_gl_display_egl_new (void);
GstGLDisplayEGL *gst_gl_display_egl_new_with_egl_display (EGLDisplay display);

G_END_DECLS

#endif /* __GST_GL_DISPLAY_EGL_H__ */
