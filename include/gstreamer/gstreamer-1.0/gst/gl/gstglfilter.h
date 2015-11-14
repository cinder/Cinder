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

#ifndef _GST_GL_FILTER_H_
#define _GST_GL_FILTER_H_

#include <gst/gst.h>
#include <gst/video/video.h>

#include <gst/gl/gl.h>

G_BEGIN_DECLS

GType gst_gl_filter_get_type(void);
#define GST_TYPE_GL_FILTER            (gst_gl_filter_get_type())
#define GST_GL_FILTER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_GL_FILTER,GstGLFilter))
#define GST_IS_GL_FILTER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GL_FILTER))
#define GST_GL_FILTER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass) ,GST_TYPE_GL_FILTER,GstGLFilterClass))
#define GST_IS_GL_FILTER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass) ,GST_TYPE_GL_FILTER))
#define GST_GL_FILTER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj) ,GST_TYPE_GL_FILTER,GstGLFilterClass))

/**
 * GstGLFilter:
 * @base_transform: parent #GstBaseTransform
 * @pool: the currently configured #GstBufferPool
 * @display: the currently configured #GstGLDisplay
 * @in_info: the video info for input buffers
 * @out_info: the video info for output buffers
 * @fbo: GL Framebuffer object used for transformations
 * @depthbuffer: GL renderbuffer attached to @fbo
 * @upload: the object used for uploading data, if needed
 * @download: the object used for downloading data, if needed
 *
 * #GstGLFilter is a base class that provides the logic of getting the GL context
 * from downstream and automatic upload/download for non-#GstGLMemory
 * #GstBuffer<!--  -->s.
 */
struct _GstGLFilter
{
  GstGLBaseFilter    parent;

  GstVideoInfo       in_info;
  GstVideoInfo       out_info;

  GstCaps           *out_caps;

  /* <private> */
  GLuint             fbo;
  GLuint             depthbuffer;

  GLuint             in_tex_id;
  GLuint             out_tex_id;

  GstGLShader       *default_shader;

  GLuint             vao;
  GLuint             vbo_indices;
  GLuint             vertex_buffer;
  GLint              draw_attr_position_loc;
  GLint              draw_attr_texture_loc;
};

/**
 * GstGLFilterClass:
 * @base_transform_class: parent class
 * @set_caps: mirror from #GstBaseTransform
 * @filter: perform operations on the input and output buffers.  In general,
 *          you should avoid using this method if at all possible. One valid
 *          use-case for using this is keeping previous buffers for future calculations.
 *          Note: If @filter exists, then @filter_texture is not run
 * @filter_texture: given @in_tex, transform it into @out_tex.  Not used
 *                  if @filter exists
 * @init_fbo: perform initialization when the Framebuffer object is created
 * @display_init_cb: execute arbitrary gl code on start
 * @display_reset_cb: execute arbitrary gl code at stop
 * @transform_internal_caps: Perform sub-class specific modifications of the
 *   caps to be processed between upload on input and before download for output.
 */
struct _GstGLFilterClass
{
  GstGLBaseFilterClass parent_class;

  gboolean (*set_caps)          (GstGLFilter* filter, GstCaps* incaps, GstCaps* outcaps);
  gboolean (*filter)            (GstGLFilter *filter, GstBuffer *inbuf, GstBuffer *outbuf);
  gboolean (*filter_texture)    (GstGLFilter *filter, guint in_tex, guint out_tex);
  gboolean (*init_fbo)          (GstGLFilter *filter);

  GstCaps *(*transform_internal_caps) (GstGLFilter *filter,
    GstPadDirection direction, GstCaps * caps, GstCaps * filter_caps);

  /* useful to init and cleanup custom gl resources */
  void (*display_init_cb)       (GstGLFilter *filter);
  void (*display_reset_cb)      (GstGLFilter *filter);
};

gboolean gst_gl_filter_filter_texture (GstGLFilter * filter, GstBuffer * inbuf,
                                       GstBuffer * outbuf);

void gst_gl_filter_render_to_target (GstGLFilter *filter, gboolean resize, GLuint input,
                                     GLuint target, GLCB func, gpointer data);

void gst_gl_filter_render_to_target_with_shader (GstGLFilter * filter, gboolean resize,
                                                 GLuint input, GLuint target, GstGLShader *shader);

void gst_gl_filter_draw_texture (GstGLFilter *filter, GLuint texture, guint width, guint height);

G_END_DECLS

#endif /* _GST_GL_FILTER_H_ */
