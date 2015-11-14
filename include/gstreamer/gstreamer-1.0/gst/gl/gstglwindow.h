/*
 * GStreamer
 * Copyright (C) 2008 Julien Isorce <julien.isorce@gmail.com>
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

#ifndef __GST_GL_WINDOW_H__
#define __GST_GL_WINDOW_H__

#include <gst/gst.h>

#include <gst/gl/gstgl_fwd.h>
#include <gst/gl/gstglcontext.h>
#include <gst/gl/gstgldisplay.h>

G_BEGIN_DECLS

#define GST_GL_TYPE_WINDOW         (gst_gl_window_get_type())
#define GST_GL_WINDOW(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), GST_GL_TYPE_WINDOW, GstGLWindow))
#define GST_GL_WINDOW_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), GST_GL_TYPE_WINDOW, GstGLWindowClass))
#define GST_GL_IS_WINDOW(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), GST_GL_TYPE_WINDOW))
#define GST_GL_IS_WINDOW_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), GST_GL_TYPE_WINDOW))
#define GST_GL_WINDOW_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), GST_GL_TYPE_WINDOW, GstGLWindowClass))

#define GST_GL_WINDOW_LOCK(w) g_mutex_lock(&GST_GL_WINDOW(w)->lock)
#define GST_GL_WINDOW_UNLOCK(w) g_mutex_unlock(&GST_GL_WINDOW(w)->lock)
#define GST_GL_WINDOW_GET_LOCK(w) (&GST_GL_WINDOW(w)->lock)

#define GST_GL_WINDOW_ERROR (gst_gl_window_error_quark ())

typedef enum
{
  GST_GL_WINDOW_ERROR_FAILED,
  GST_GL_WINDOW_ERROR_OLD_LIBS,
  GST_GL_WINDOW_ERROR_RESOURCE_UNAVAILABLE,
} GstGLWindowError;

typedef void (*GstGLWindowCB) (gpointer data);
typedef void (*GstGLWindowResizeCB) (gpointer data, guint width, guint height);

#define	GST_GL_WINDOW_CB(f)			 ((GstGLWindowCB) (f))
#define	GST_GL_WINDOW_RESIZE_CB(f)		 ((GstGLWindowResizeCB) (f))

/**
 * GstGLWindow:
 *
 * #GstGLWindow is an opaque struct and should only be accessed through the
 * provided api.
 */
struct _GstGLWindow {
  /*< private >*/
  GstObject parent;

  GMutex        lock;
  GMutex        nav_lock;
  GCond         nav_create_cond;
  GCond         nav_destroy_cond;
  gboolean      nav_created;
  gboolean      nav_alive;

  GstGLDisplay *display;
  GWeakRef      context_ref;

  guintptr      external_gl_context;

  gboolean      is_drawing;

  GstGLWindowCB         draw;
  gpointer              draw_data;
  GDestroyNotify        draw_notify;
  GstGLWindowCB         close;
  gpointer              close_data;
  GDestroyNotify        close_notify;
  GstGLWindowResizeCB   resize;
  gpointer              resize_data;
  GDestroyNotify        resize_notify;

  gboolean              queue_resize;

  /*< private >*/
  GMainContext *navigation_context;
  GMainLoop *navigation_loop;

  GstGLWindowPrivate *priv;

  gpointer _reserved[GST_PADDING];
};

/**
 * GstGLWindowClass:
 * @parent_class: Parent class
 * @get_display: Gets the current windowing system display connection
 * @set_window_handle: Set a window to render into
 * @get_window_handle: Gets the current window that this #GstGLWindow is
 *                     rendering into
 * @draw_unlocked: redraw the window with the specified dimensions
 * @draw: redraw the window with the specified dimensions
 * @run: run the mainloop
 * @quit: send a quit to the mainloop
 * @send_message: invoke a function on the window thread.  Required to be reentrant.
 * @send_message_async: invoke a function on the window thread. @run may or may
 *                      not have been called.  Required to be reentrant.
 * @open: open the connection to the display
 * @close: close the connection to the display
 * @get_surface_dimensions: get the width and height of the surface we are
 *                          rendering into.
 * @handle_events: whether to handle 'extra' events from the windowing system.
 *                 Basic events like surface moves and resizes are still valid
 *                 things to listen for.
 * @set_preferred_size: request that the window change surface size.  The
 *                      implementation is free to ignore this information.
 */
struct _GstGLWindowClass {
  GstObjectClass parent_class;

  guintptr (*get_display)        (GstGLWindow *window);
  void     (*set_window_handle)  (GstGLWindow *window, guintptr id);
  guintptr (*get_window_handle)  (GstGLWindow *window);
  void     (*draw_unlocked)      (GstGLWindow *window);
  void     (*draw)               (GstGLWindow *window);
  void     (*run)                (GstGLWindow *window);
  void     (*quit)               (GstGLWindow *window);
  void     (*send_message)       (GstGLWindow *window, GstGLWindowCB callback, gpointer data);
  void     (*send_message_async) (GstGLWindow *window, GstGLWindowCB callback, gpointer data, GDestroyNotify destroy);

  gboolean (*open)               (GstGLWindow *window, GError **error);
  void     (*close)              (GstGLWindow *window);
  void     (*handle_events)      (GstGLWindow *window, gboolean handle_events);
  void     (*set_preferred_size) (GstGLWindow *window, gint width, gint height);
  void     (*show)               (GstGLWindow *window);
  gboolean (*set_render_rectangle)(GstGLWindow *window, gint x, gint y, gint width, gint height);
  void     (*queue_resize)       (GstGLWindow *window);

  /*< private >*/
  gpointer _reserved[GST_PADDING];
};

struct key_event
{
  GstGLWindow *window;
  const char *event_type;
  const char *key_str;
};

struct mouse_event
{
  GstGLWindow *window;
  const char *event_type;
  int button;
  double posx;
  double posy;
};

GQuark gst_gl_window_error_quark (void);
GType gst_gl_window_get_type     (void);

GstGLWindow * gst_gl_window_new  (GstGLDisplay *display);

/* callbacks */
void     gst_gl_window_set_draw_callback    (GstGLWindow *window,
                                             GstGLWindowCB callback,
                                             gpointer data,
                                             GDestroyNotify destroy_notify);
void     gst_gl_window_set_resize_callback  (GstGLWindow *window,
                                             GstGLWindowResizeCB callback,
                                             gpointer data,
                                             GDestroyNotify destroy_notify);
void     gst_gl_window_set_close_callback   (GstGLWindow *window,
                                             GstGLWindowCB callback,
                                             gpointer data,
                                             GDestroyNotify destroy_notify);

void     gst_gl_window_set_window_handle    (GstGLWindow *window, guintptr handle);
guintptr gst_gl_window_get_window_handle    (GstGLWindow *window);

/* loop/events */
void     gst_gl_window_run                  (GstGLWindow *window);
void     gst_gl_window_quit                 (GstGLWindow *window);
gboolean gst_gl_window_is_running           (GstGLWindow *window);
void     gst_gl_window_send_message         (GstGLWindow *window,
                                             GstGLWindowCB callback,
                                             gpointer data);
void     gst_gl_window_send_message_async   (GstGLWindow *window,
                                             GstGLWindowCB callback,
                                             gpointer data,
                                             GDestroyNotify destroy);

/* navigation */
void     gst_gl_window_handle_events        (GstGLWindow * window,
                                             gboolean handle_events);
gboolean gst_gl_window_key_event_cb         (gpointer data);
gboolean gst_gl_window_mouse_event_cb       (gpointer data);
void     gst_gl_window_send_key_event       (GstGLWindow * window,
                                             const char * event_type,
                                             const char * key_str);
void     gst_gl_window_send_mouse_event     (GstGLWindow * window,
                                             const char * event_type,
                                             int button,
                                             double posx,
                                             double posy);

/* surfaces/rendering */
void     gst_gl_window_queue_resize         (GstGLWindow *window);
void     gst_gl_window_draw_unlocked        (GstGLWindow *window);
void     gst_gl_window_draw                 (GstGLWindow *window);
void     gst_gl_window_show                 (GstGLWindow *window);
void     gst_gl_window_set_preferred_size   (GstGLWindow * window,
                                             gint width,
                                             gint height);
void     gst_gl_window_get_surface_dimensions (GstGLWindow * window,
                                               guint * width,
                                               guint * height);
gboolean gst_gl_window_set_render_rectangle   (GstGLWindow * window,
                                               gint x,
                                               gint y,
                                               gint width,
                                               gint height);

/* subclass usage only */
void     gst_gl_window_resize               (GstGLWindow *window, guint width, guint height);

GstGLContext * gst_gl_window_get_context    (GstGLWindow *window);
guintptr       gst_gl_window_get_display    (GstGLWindow *window);

GST_DEBUG_CATEGORY_EXTERN (gst_gl_window_debug);

G_END_DECLS

#endif /* __GST_GL_WINDOW_H__ */
