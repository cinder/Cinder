/*
 * GStreamer
 * Copyright (C) 2012 Collabora Ltd.
 *   @author: Sebastian Dröge <sebastian.droege@collabora.co.uk>
 * Copyright (C) 2014 Julien Isorce <julien.isorce@gmail.com>
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

#ifndef _GST_EGL_IMAGE_MEMORY_H_
#define _GST_EGL_IMAGE_MEMORY_H_

#include <gst/gst.h>
#include <gst/gstallocator.h>
#include <gst/gstmemory.h>
#include <gst/video/video.h>

#include <gst/gl/gl.h>
#include "gstglcontext_egl.h"

G_BEGIN_DECLS

typedef void (*GstEGLImageDestroyNotify) (GstGLContextEGL * context,
    gpointer data);

typedef struct _GstEGLImageMemory GstEGLImageMemory;

#define GST_EGL_IMAGE_MEMORY_TYPE "EGLImage"

#define GST_CAPS_FEATURE_MEMORY_EGL_IMAGE "memory:EGLImage"

struct _GstEGLImageMemory
{
  GstMemory parent;

  GstGLContextEGL *context;
  EGLImageKHR image;
  GstVideoGLTextureType type;
  GstVideoGLTextureOrientation orientation;

  gpointer user_data;
  GstEGLImageDestroyNotify user_data_destroy;
};

void gst_egl_image_memory_init (void);
gboolean gst_egl_image_memory_setup_buffer (GstGLContext * context, GstVideoInfo * info, GstBuffer * buffer);
gboolean gst_is_egl_image_memory (GstMemory * mem);
EGLImageKHR gst_egl_image_memory_get_image (GstMemory * mem);
EGLDisplay gst_egl_image_memory_get_display (GstMemory * mem);
GstVideoGLTextureOrientation gst_egl_image_memory_get_orientation (GstMemory * mem);
void gst_egl_image_memory_set_orientation (GstMemory * mem,
    GstVideoGLTextureOrientation orientation);

G_END_DECLS

#endif /* _GST_GL_MEMORY_H_ */
