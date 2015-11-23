/*
 * GStreamer
 * Copyright (C) 2013 Sebastian Dröge <slomo@circular-chaos.org>
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

#ifndef __GST_GL_EGL_H__
#define __GST_GL_EGL_H__

#include <gst/gst.h>
#include <gst/gl/gl.h>
#include <gst/gl/egl/gstegl.h>

G_BEGIN_DECLS

typedef struct _GstGLContextEGL GstGLContextEGL;
typedef struct _GstGLContextEGLClass GstGLContextEGLClass;

GType gst_gl_context_egl_get_type     (void);
#define GST_GL_TYPE_CONTEXT_EGL         (gst_gl_context_egl_get_type())
#define GST_GL_CONTEXT_EGL(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), GST_GL_TYPE_CONTEXT_EGL, GstGLContextEGL))
#define GST_GL_CONTEXT_EGL_CLASS(k)     (G_TYPE_CHECK_CLASS((k), GST_GL_TYPE_CONTEXT_EGL, GstGLContextEGLClass))
#define GST_GL_IS_CONTEXT_EGL(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), GST_GL_TYPE_CONTEXT_EGL))
#define GST_GL_IS_CONTEXT_EGL_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), GST_GL_TYPE_CONTEXT_EGL))
#define GST_GL_CONTEXT_EGL_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), GST_GL_TYPE_CONTEXT_EGL, GstGLContextEGLClass))

struct _GstGLContextEGL {
  GstGLContext context;

  EGLContext egl_context;
  EGLDisplay egl_display;
  EGLSurface egl_surface;
  EGLConfig  egl_config;

  GstGLAPI gl_api;

  EGLImageKHR (*eglCreateImage) (EGLDisplay dpy, EGLContext ctx, EGLenum target,
      EGLClientBuffer buffer, const EGLint *attrib_list);
  EGLBoolean (*eglDestroyImage) (EGLDisplay dpy, EGLImageKHR image);

  /* Cached handle */
  EGLNativeWindowType window_handle;
};

struct _GstGLContextEGLClass {
  GstGLContextClass parent;
};

GstGLContextEGL *   gst_gl_context_egl_new                  (GstGLDisplay * display);
guintptr            gst_gl_context_egl_get_current_context  (void);
gpointer            gst_gl_context_egl_get_proc_address     (GstGLAPI gl_api, const gchar * name);

/* TODO:
 * add support for EGL_NO_CONTEXT
 * add gst_gl_context_egl_new_gl_no_context that only manages the display
 * add gst_gl_context_egl_is_gl_no_context () */

G_END_DECLS

#endif /* __GST_GL_EGL_H__ */
