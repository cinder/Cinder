/*
 * GStreamer
 * Copyright (C) 2010 Texas Instruments, Inc
 * Copyright (C) 2011 Thiago Santos <thiago.sousa.santos@collabora.com>
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


#ifndef __GST_BASE_CAMERA_SRC_H__
#define __GST_BASE_CAMERA_SRC_H__

#ifndef GST_USE_UNSTABLE_API
#warning "GstBaseCameraSrc is unstable API and may change in future."
#warning "You can define GST_USE_UNSTABLE_API to avoid this warning."
#endif

#include <gst/gst.h>
#include <gst/gstbin.h>
#include "gstcamerabin-enum.h"
#include "gstcamerabinpreview.h"

G_BEGIN_DECLS

#define GST_TYPE_BASE_CAMERA_SRC \
  (gst_base_camera_src_get_type())
#define GST_BASE_CAMERA_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_BASE_CAMERA_SRC,GstBaseCameraSrc))
#define GST_BASE_CAMERA_SRC_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_BASE_CAMERA_SRC, GstBaseCameraSrcClass))
#define GST_BASE_CAMERA_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_BASE_CAMERA_SRC,GstBaseCameraSrcClass))
#define GST_IS_BASE_CAMERA_SRC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_BASE_CAMERA_SRC))
#define GST_IS_BASE_CAMERA_SRC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_BASE_CAMERA_SRC))
#define GST_BASE_CAMERA_SRC_CAST(obj) \
  ((GstBaseCameraSrc *) (obj))
GType gst_base_camera_src_get_type (void);

typedef struct _GstBaseCameraSrc GstBaseCameraSrc;
typedef struct _GstBaseCameraSrcClass GstBaseCameraSrcClass;

#define GST_BASE_CAMERA_SRC_VIEWFINDER_PAD_NAME "vfsrc"
#define GST_BASE_CAMERA_SRC_IMAGE_PAD_NAME "imgsrc"
#define GST_BASE_CAMERA_SRC_VIDEO_PAD_NAME "vidsrc"

#define GST_BASE_CAMERA_SRC_PREVIEW_MESSAGE_NAME "preview-image"

/**
 * GstBaseCameraSrc:
 */
struct _GstBaseCameraSrc
{
  GstBin parent;

  GstCameraBinMode mode;

  gboolean auto_start;
  gboolean capturing;
  GMutex capturing_mutex;

  /* Preview convert pipeline */
  GstCaps *preview_caps;
  gboolean post_preview;
  GstElement *preview_filter;
  GstCameraBinPreviewPipelineData *preview_pipeline;

  /* Resolution of the buffers configured to camerabin */
  gint width;
  gint height;

  gfloat zoom;
  gfloat max_zoom;

  gpointer _gst_reserved[GST_PADDING_LARGE];
};


/**
 * GstBaseCameraSrcClass:
 * @construct_pipeline: construct pipeline
 * @setup_pipeline: configure pipeline for the chosen settings
 * @set_zoom: set the zoom
 * @set_mode: set the mode
 */
struct _GstBaseCameraSrcClass
{
  GstBinClass parent;

  /* Construct pipeline. (called in GST_STATE_CHANGE_NULL_TO_READY) Optional. */
  gboolean    (*construct_pipeline)  (GstBaseCameraSrc *self);

  /* (called in GST_STATE_CHANGE_READY_TO_PAUSED). Optional. */
  gboolean    (*setup_pipeline)      (GstBaseCameraSrc *self);

  /* Set the zoom. If set, called when changing 'zoom' property. Optional. */
  void        (*set_zoom)            (GstBaseCameraSrc *self, gfloat zoom);

  /* Set the mode. If set, called when changing 'mode' property. Optional. */
  gboolean    (*set_mode)            (GstBaseCameraSrc *self,
                                      GstCameraBinMode mode);

  /* Set preview caps. If set, called called when setting new 'preview-caps'. Optional. */
  gboolean    (*set_preview)         (GstBaseCameraSrc *self,
                                      GstCaps *preview_caps);

  /* Called by the handler for 'start-capture'. Mandatory. */
  gboolean (*start_capture) (GstBaseCameraSrc * src);

  /* Called by the handler for 'stop-capture'. Mandatory. */
  void (*stop_capture) (GstBaseCameraSrc * src);

  gpointer _gst_reserved[GST_PADDING_LARGE];
};


#define MIN_ZOOM 1.0f
#define MAX_ZOOM 10.0f
#define ZOOM_1X MIN_ZOOM

gboolean gst_base_camera_src_set_mode (GstBaseCameraSrc *self, GstCameraBinMode mode);
void gst_base_camera_src_setup_zoom (GstBaseCameraSrc * self);
void gst_base_camera_src_setup_preview (GstBaseCameraSrc * self, GstCaps * preview_caps);
void gst_base_camera_src_finish_capture (GstBaseCameraSrc *self);


void gst_base_camera_src_post_preview (GstBaseCameraSrc *self, GstSample * sample);
// XXX add methods to get/set img capture and vid capture caps..

G_END_DECLS

#endif /* __GST_BASE_CAMERA_SRC_H__ */
