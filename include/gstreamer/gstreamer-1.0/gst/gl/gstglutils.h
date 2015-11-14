/*
 * GStreamer
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

#ifndef __GST_GL_UTILS_H__
#define __GST_GL_UTILS_H__

#include <gst/video/video.h>

#include <gst/gl/gstgl_fwd.h>

G_BEGIN_DECLS

/**
 * GstGLContextProjection:
 *
 * %GST_GL_DISPLAY_PROJECTION_ORTHO2D: Orthogonal projection
 * %GST_GL_DISPLAY_CONVERSION_MATRIX: Perspective projection 
 */
typedef enum
{
  GST_GL_DISPLAY_PROJECTION_ORTHO2D,
  GST_GL_DISPLAY_PROJECTION_PERSPECTIVE
} GstGLDisplayProjection;

/**
 * CDCB:
 * @texture: texture to draw
 * @width: new width
 * @height: new height:
 * @data: user data
 *
 * client draw callback
 */
typedef gboolean (*CDCB) (GLuint texture, GLuint width, GLuint height, gpointer data);
/**
 * GLCB:
 * @width: the width
 * @height: the height
 * @texture: texture
 * @stuff: user data
 *
 * callback definition for operating on textures
 */
typedef void (*GLCB) (gint, gint, guint, gpointer stuff);
/**
 * GLCB_V2:
 * @stuff: user data
 *
 * callback definition for operating through a Framebuffer object
 */
typedef void (*GLCB_V2) (gpointer stuff);

/* deprecated. replaced by GstGLMemory */
void gst_gl_context_gen_texture (GstGLContext * context, GLuint * pTexture,
    GstVideoFormat v_format, GLint width, GLint height);
/* deprecated. replaced by GstGLMemory */
void gst_gl_context_del_texture (GstGLContext * context, GLuint * pTexture);

/* deprecated. replaced by GstGLMemory */
void gst_gl_generate_texture_full (GstGLContext * context, const GstVideoInfo * info,
    const guint comp, gint stride[], gsize offset[], gsize size[], GLuint * pTexture);

gboolean gst_gl_context_gen_fbo (GstGLContext * context, gint width, gint height,
    GLuint * fbo, GLuint * depthbuffer);
gboolean gst_gl_context_use_fbo_v2 (GstGLContext * context, gint texture_fbo_width,
    gint texture_fbo_height, GLuint fbo, GLuint depth_buffer,
    GLuint texture_fbo, GLCB_V2 cb, gpointer stuff);
void gst_gl_context_del_fbo (GstGLContext * context, GLuint fbo,
    GLuint depth_buffer);

gboolean gst_gl_context_gen_shader (GstGLContext * context,
    const gchar * shader_vertex_source,
    const gchar * shader_fragment_source, GstGLShader ** shader);
void gst_gl_context_del_shader (GstGLContext * context, GstGLShader * shader);

gboolean gst_gl_context_check_framebuffer_status (GstGLContext * context);

void gst_gl_context_set_error (GstGLContext * context, const char * format, ...);
gchar *gst_gl_context_get_error (void);

gboolean gst_gl_ensure_element_data (gpointer element,
    GstGLDisplay **display_ptr, GstGLContext ** other_context_ptr);
gboolean gst_gl_handle_set_context (GstElement * element, GstContext * context, 
    GstGLDisplay ** display, GstGLContext ** other_context);
gboolean gst_gl_handle_context_query (GstElement * element, GstQuery * query, 
    GstGLDisplay ** display, GstGLContext ** other_context);

gboolean gst_gl_run_query (GstElement * element,
    GstQuery * query, GstPadDirection direction);
gsize gst_gl_get_plane_data_size (GstVideoInfo * info, GstVideoAlignment * align,
    guint plane);
GstCaps * gst_gl_caps_replace_all_caps_features (const GstCaps * caps,
    const gchar * feature_name);

G_END_DECLS

#endif /* __GST_GL_UTILS_H__ */
