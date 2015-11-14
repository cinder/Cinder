/*
 * GStreamer
 * Copyright (C) 2008 Filippo Argiolas <filippo.argiolas@gmail.com>
 * Copyright (C) 2014 Julien Isorce <julien.isorce@collabora.co.uk>
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

#ifndef __GST_GL_SHADER_H__
#define __GST_GL_SHADER_H__

#include <gst/gl/gstgl_fwd.h>

G_BEGIN_DECLS

#define GST_GL_TYPE_SHADER         (gst_gl_shader_get_type())
#define GST_GL_SHADER(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), GST_GL_TYPE_SHADER, GstGLShader))
#define GST_GL_SHADER_CLASS(k)     (G_TYPE_CHECK_CLASS((k), GST_GL_TYPE_SHADER, GstGLShaderClass))
#define GST_GL_IS_SHADER(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), GST_GL_TYPE_SHADER))
#define GST_GL_IS_SHADER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), GST_GL_TYPE_SHADER))
#define GST_GL_SHADER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), GST_GL_TYPE_SHADER, GstGLShaderClass))

#define GST_GL_SHADER_ERROR (gst_gl_shader_error_quark ())

typedef enum {
  GST_GL_SHADER_ERROR_COMPILE,
  GST_GL_SHADER_ERROR_LINK,
  GST_GL_SHADER_ERROR_PROGRAM
} GstGLShaderError;

typedef enum {
  GST_GL_SHADER_FRAGMENT_SOURCE,
  GST_GL_SHADER_VERTEX_SOURCE
} GstGLShaderSourceType;

struct _GstGLShader {
  /*< private >*/
  GstObject parent;

  GstGLContext *context;

  GstGLShaderPrivate *priv;
};

struct _GstGLShaderClass {
  /*< private >*/
  GstObjectClass parent_class;
};

/* methods */

GQuark gst_gl_shader_error_quark (void);
GType gst_gl_shader_get_type (void);

GstGLShader * gst_gl_shader_new (GstGLContext *context);

int gst_gl_shader_get_program_handle(GstGLShader * shader);

void          gst_gl_shader_set_vertex_source   (GstGLShader *shader, const gchar *src);
void          gst_gl_shader_set_fragment_source (GstGLShader *shader, const gchar *src);
const gchar * gst_gl_shader_get_vertex_source   (GstGLShader *shader);
const gchar * gst_gl_shader_get_fragment_source (GstGLShader *shader);

void     gst_gl_shader_set_active        (GstGLShader *shader, gboolean active);
gboolean gst_gl_shader_is_compiled       (GstGLShader *shader);
gboolean gst_gl_shader_compile           (GstGLShader *shader, GError **error);
gboolean gst_gl_shader_compile_and_check (GstGLShader *shader, const gchar *source, GstGLShaderSourceType type);
gboolean gst_gl_shader_compile_all_with_attribs_and_check (GstGLShader *shader, const gchar *v_src, const gchar *f_src, const gint n_attribs, const gchar *attrib_names[], GLint attrib_locs[]);

gboolean gst_gl_shader_compile_with_default_f_and_check   (GstGLShader *shader, const gchar *v_src, const gint n_attribs, const gchar *attrib_names[], GLint attrib_locs[]);
gboolean gst_gl_shader_compile_with_default_v_and_check   (GstGLShader *shader, const gchar *f_src, GLint *pos_loc, GLint *tex_loc);
gboolean gst_gl_shader_compile_with_default_vf_and_check  (GstGLShader *shader, GLint *pos_loc, GLint *tex_loc);

void gst_gl_shader_release       (GstGLShader *shader);
void gst_gl_shader_use           (GstGLShader *shader);
void gst_gl_context_clear_shader (GstGLContext *context);

void gst_gl_shader_set_uniform_1i           (GstGLShader *shader, const gchar *name, gint value);
void gst_gl_shader_set_uniform_1iv          (GstGLShader *shader, const gchar *name, guint count, gint *value);
void gst_gl_shader_set_uniform_1f           (GstGLShader *shader, const gchar *name, gfloat value);
void gst_gl_shader_set_uniform_1fv          (GstGLShader *shader, const gchar *name, guint count, gfloat *value);
void gst_gl_shader_set_uniform_2i           (GstGLShader *shader, const gchar *name, gint v0,     gint v1);
void gst_gl_shader_set_uniform_2iv          (GstGLShader *shader, const gchar *name, guint count, gint *value);
void gst_gl_shader_set_uniform_2f           (GstGLShader *shader, const gchar *name, gfloat v0,   gfloat v1);
void gst_gl_shader_set_uniform_2fv          (GstGLShader *shader, const gchar *name, guint count, gfloat *value);
void gst_gl_shader_set_uniform_3i           (GstGLShader *shader, const gchar *name, gint v0,     gint v1,       gint v2);
void gst_gl_shader_set_uniform_3iv          (GstGLShader *shader, const gchar *name, guint count, gint * value);
void gst_gl_shader_set_uniform_3f           (GstGLShader *shader, const gchar *name, gfloat v0,   gfloat v1,     gfloat v2);
void gst_gl_shader_set_uniform_3fv          (GstGLShader *shader, const gchar *name, guint count, gfloat *value);
void gst_gl_shader_set_uniform_4i           (GstGLShader *shader, const gchar *name, gint v0,     gint v1,       gint v2,   gint v3);
void gst_gl_shader_set_uniform_4iv          (GstGLShader *shader, const gchar *name, guint count, gint *value);
void gst_gl_shader_set_uniform_4f           (GstGLShader *shader, const gchar *name, gfloat v0,   gfloat v1,     gfloat v2, gfloat v3);
void gst_gl_shader_set_uniform_4fv          (GstGLShader *shader, const gchar *name, guint count, gfloat *value);
void gst_gl_shader_set_uniform_matrix_2fv   (GstGLShader *shader, const gchar *name, gint count, gboolean transpose, const gfloat* value);
void gst_gl_shader_set_uniform_matrix_3fv   (GstGLShader *shader, const gchar *name, gint count, gboolean transpose, const gfloat* value);
void gst_gl_shader_set_uniform_matrix_4fv   (GstGLShader *shader, const gchar *name, gint count, gboolean transpose, const gfloat* value);
#if GST_GL_HAVE_OPENGL
void gst_gl_shader_set_uniform_matrix_2x3fv (GstGLShader *shader, const gchar *name, gint count, gboolean transpose, const gfloat* value);
void gst_gl_shader_set_uniform_matrix_2x4fv (GstGLShader *shader, const gchar *name, gint count, gboolean transpose, const gfloat* value);
void gst_gl_shader_set_uniform_matrix_3x2fv (GstGLShader *shader, const gchar *name, gint count, gboolean transpose, const gfloat* value);
void gst_gl_shader_set_uniform_matrix_3x4fv (GstGLShader *shader, const gchar *name, gint count, gboolean transpose, const gfloat* value);
void gst_gl_shader_set_uniform_matrix_4x2fv (GstGLShader *shader, const gchar *name, gint count, gboolean transpose, const gfloat* value);
void gst_gl_shader_set_uniform_matrix_4x3fv (GstGLShader *shader, const gchar *name, gint count, gboolean transpose, const gfloat* value);
#endif

gint gst_gl_shader_get_attribute_location  (GstGLShader *shader, const gchar *name);
void gst_gl_shader_bind_attribute_location (GstGLShader * shader, guint index, const gchar * name);

G_END_DECLS

#endif /* __GST_GL_SHADER_H__ */
