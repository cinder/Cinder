/*
 * GStreamer
 * Copyright (C) 2007 David A. Schleef <ds@schleef.org>
 * Copyright (C) 2008 Julien Isorce <julien.isorce@gmail.com>
 * Copyright (C) 2008 Filippo Argiolas <filippo.argiolas@gmail.com>
 * Copyright (C) 2013 Matthew Waters <ystreet00@gmail.com>
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

#ifndef __GST_GL_DISPLAY_H__
#define __GST_GL_DISPLAY_H__

#include <gst/gst.h>

#include <gst/gl/gstgl_fwd.h>

G_BEGIN_DECLS

GType gst_gl_display_get_type (void);

#define GST_TYPE_GL_DISPLAY             (gst_gl_display_get_type())
#define GST_GL_DISPLAY(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_GL_DISPLAY,GstGLDisplay))
#define GST_GL_DISPLAY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_GL_DISPLAY,GstGLDisplayClass))
#define GST_IS_GL_DISPLAY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GL_DISPLAY))
#define GST_IS_GL_DISPLAY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_GL_DISPLAY))
#define GST_GL_DISPLAY_CAST(obj)        ((GstGLDisplay*)(obj))
#define GST_GL_DISPLAY_GET_CLASS(o)     (G_TYPE_INSTANCE_GET_CLASS((o), GST_TYPE_GL_DISPLAY, GstGLDisplayClass))

typedef enum
{
  GST_GL_DISPLAY_TYPE_NONE = 0,
  GST_GL_DISPLAY_TYPE_X11 = (1 << 0),
  GST_GL_DISPLAY_TYPE_WAYLAND = (1 << 1),
  GST_GL_DISPLAY_TYPE_COCOA = (1 << 2),
  GST_GL_DISPLAY_TYPE_WIN32 = (1 << 3),
  GST_GL_DISPLAY_TYPE_DISPMANX = (1 << 4),
  GST_GL_DISPLAY_TYPE_EGL = (1 << 5),

  GST_GL_DISPLAY_TYPE_ANY = G_MAXUINT32
} GstGLDisplayType;

/**
 * GstGLDisplay:
 *
 * The contents of a #GstGLDisplay are private and should only be accessed
 * through the provided API
 */
struct _GstGLDisplay
{
  /* <private> */
  GstObject             object;

  GstGLDisplayType      type;

  GstGLDisplayPrivate  *priv;
};

struct _GstGLDisplayClass
{
  GstObjectClass object_class;

  guintptr (*get_handle)      (GstGLDisplay * display);
};

GstGLDisplay *gst_gl_display_new (void);

#define gst_gl_display_lock(display)        GST_OBJECT_LOCK (display)
#define gst_gl_display_unlock(display)      GST_OBJECT_UNLOCK (display)

guintptr         gst_gl_display_get_handle             (GstGLDisplay * display);
GstGLDisplayType gst_gl_display_get_handle_type        (GstGLDisplay * display);
void             gst_gl_display_filter_gl_api          (GstGLDisplay * display,
                                                        GstGLAPI api);
GstGLAPI         gst_gl_display_get_gl_api             (GstGLDisplay * display);
GstGLAPI         gst_gl_display_get_gl_api_unlocked    (GstGLDisplay * display);

#define GST_GL_DISPLAY_CONTEXT_TYPE "gst.gl.GLDisplay"
void     gst_context_set_gl_display (GstContext * context, GstGLDisplay * display);
gboolean gst_context_get_gl_display (GstContext * context, GstGLDisplay ** display);

gboolean  gst_gl_display_create_context (GstGLDisplay * display,
    GstGLContext * other_context, GstGLContext ** p_context, GError **error);
GstGLContext * gst_gl_display_get_gl_context_for_thread (GstGLDisplay * display,
    GThread * thread);
gboolean gst_gl_display_add_context (GstGLDisplay * display,
    GstGLContext * context);

G_END_DECLS

#endif /* __GST_GL_DISPLAY_H__ */
