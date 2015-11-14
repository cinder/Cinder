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

#ifndef __GST_GL_CONTEXT_H__
#define __GST_GL_CONTEXT_H__

#include <gst/gst.h>

#include <gst/gl/gl.h>

G_BEGIN_DECLS

#define GST_GL_TYPE_CONTEXT         (gst_gl_context_get_type())
#define GST_GL_CONTEXT(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), GST_GL_TYPE_CONTEXT, GstGLContext))
#define GST_GL_CONTEXT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), GST_GL_TYPE_CONTEXT, GstGLContextClass))
#define GST_GL_IS_CONTEXT(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), GST_GL_TYPE_CONTEXT))
#define GST_GL_IS_CONTEXT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), GST_GL_TYPE_CONTEXT))
#define GST_GL_CONTEXT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), GST_GL_TYPE_CONTEXT, GstGLContextClass))
GType gst_gl_context_get_type     (void);

#define GST_GL_CONTEXT_ERROR (gst_gl_context_error_quark ())
GQuark gst_gl_context_error_quark (void);

/**
 * GstGLContextThreadFunc:
 * @context: a #GstGLContext
 * @data: user data
 *
 * Represents a function to run in the GL thread with @context and @data
 */
typedef void (*GstGLContextThreadFunc) (GstGLContext * context, gpointer data);

#define GST_GL_CONTEXT_TYPE_CGL "gst.gl.context.CGL"
#define GST_GL_CONTEXT_TYPE_GLX "gst.gl.context.GLX"
#define GST_GL_CONTEXT_TYPE_EGL "gst.gl.context.EGL"
#define GST_GL_CONTEXT_TYPE_WGL "gst.gl.context.WGL"
#define GST_GL_CONTEXT_TYPE_EAGL "gst.gl.context.EAGL"

typedef enum
{
  GST_GL_CONTEXT_ERROR_FAILED,
  GST_GL_CONTEXT_ERROR_WRONG_CONFIG,
  GST_GL_CONTEXT_ERROR_WRONG_API,
  GST_GL_CONTEXT_ERROR_OLD_LIBS,
  GST_GL_CONTEXT_ERROR_CREATE_CONTEXT,
  GST_GL_CONTEXT_ERROR_RESOURCE_UNAVAILABLE,
} GstGLContextError;

/**
 * GstGLContext:
 *
 * Opaque #GstGLContext object
 */
struct _GstGLContext {
  /*< private >*/
  GstObject parent;

  GstGLDisplay *display;
  GstGLWindow  *window;

  GstGLFuncs *gl_vtable;

  gpointer _reserved[GST_PADDING];

  GstGLContextPrivate *priv;
};

/**
 * GstGLContextClass:
 * @get_gl_context: get the backing platform specific OpenGL context
 * @get_gl_api: get the available OpenGL api's that this context can work with
 * @get_proc_address: get an function pointer to an OpenGL function
 * @activate: call eglMakeCurrent or similar
 * @choose_format: choose a format for the framebuffer
 * @create_context: create the OpenGL context
 * @destroy_context: destroy the OpenGL context
 * @swap_buffers: swap the default framebuffer's front/back buffers
 */
struct _GstGLContextClass {
  GstObjectClass parent_class;

  guintptr      (*get_current_context) (void);
  guintptr      (*get_gl_context)     (GstGLContext *context);
  GstGLAPI      (*get_gl_api)         (GstGLContext *context);
  GstGLPlatform (*get_gl_platform)    (GstGLContext *context);
  gpointer      (*get_proc_address)   (GstGLAPI gl_api, const gchar *name);
  gboolean      (*activate)           (GstGLContext *context, gboolean activate);
  gboolean      (*choose_format)      (GstGLContext *context, GError **error);
  gboolean      (*create_context)     (GstGLContext *context, GstGLAPI gl_api,
                                       GstGLContext *other_context, GError ** error);
  void          (*destroy_context)    (GstGLContext *context);
  void          (*swap_buffers)       (GstGLContext *context);
  gboolean      (*check_feature)      (GstGLContext *context, const gchar *feature);

  /*< private >*/
  gpointer _reserved[GST_PADDING];
};

/* methods */

GstGLContext * gst_gl_context_new  (GstGLDisplay *display);
GstGLContext * gst_gl_context_new_wrapped (GstGLDisplay *display,
                                           guintptr handle,
                                           GstGLPlatform context_type,
                                           GstGLAPI available_apis);

gboolean      gst_gl_context_activate         (GstGLContext *context, gboolean activate);
GThread *     gst_gl_context_get_thread       (GstGLContext *context);
GstGLContext * gst_gl_context_get_current     (void);

GstGLDisplay * gst_gl_context_get_display (GstGLContext *context);
gpointer      gst_gl_context_get_proc_address (GstGLContext *context, const gchar *name);
GstGLPlatform gst_gl_context_get_gl_platform  (GstGLContext *context);
GstGLAPI      gst_gl_context_get_gl_api       (GstGLContext *context);
guintptr      gst_gl_context_get_gl_context   (GstGLContext *context);
gboolean      gst_gl_context_can_share        (GstGLContext * context, GstGLContext *other_context);

gboolean      gst_gl_context_create           (GstGLContext *context, GstGLContext *other_context, GError ** error);
void          gst_gl_context_destroy          (GstGLContext *context);

gpointer      gst_gl_context_default_get_proc_address (GstGLAPI gl_api, const gchar *name);
gpointer      gst_gl_context_get_proc_address_with_platform (GstGLPlatform, GstGLAPI gl_api, const gchar *name);

gboolean      gst_gl_context_set_window (GstGLContext *context, GstGLWindow *window);
GstGLWindow * gst_gl_context_get_window (GstGLContext *context);

void          gst_gl_context_get_gl_version (GstGLContext *context, gint *maj, gint *min);
gboolean      gst_gl_context_check_gl_version (GstGLContext * context, GstGLAPI api, gint maj, gint min);
gboolean      gst_gl_context_check_feature (GstGLContext *context, const gchar *feature);

guintptr      gst_gl_context_get_current_gl_context     (GstGLPlatform platform);
GstGLAPI      gst_gl_context_get_current_gl_api         (GstGLPlatform platform, guint *major, guint *minor);

gboolean gst_gl_context_fill_info (GstGLContext * context, GError ** error);

/* FIXME: remove */
void gst_gl_context_thread_add (GstGLContext * context,
    GstGLContextThreadFunc func, gpointer data);

GST_DEBUG_CATEGORY_EXTERN (gst_gl_context_debug);

G_END_DECLS

#endif /* __GST_GL_CONTEXT_H__ */
