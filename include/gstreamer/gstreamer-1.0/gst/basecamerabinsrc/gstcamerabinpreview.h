/*
 * GStreamer
 * Copyright (C) 2008 Nokia Corporation <multimedia@maemo.org>
 * Copyright (C) 2010 Thiago Santos <thiago.sousa.santos@collabora.co.uk>
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

#ifndef __CAMERABIN_PREVIEW_H_
#define __CAMERABIN_PREVIEW_H_

#ifndef GST_USE_UNSTABLE_API
#warning "camera bin preview is unstable API and may change in future."
#warning "You can define GST_USE_UNSTABLE_API to avoid this warning."
#endif

#include <gst/gst.h>

typedef struct
{
  GstElement *pipeline;

  GstElement *appsrc;
  GstElement *filter;
  GstElement *appsink;
  GstElement *vscale;

  GstElement *element;

  GstCaps *pending_preview_caps;
  guint processing;
  GMutex processing_lock;
  GCond processing_cond;

} GstCameraBinPreviewPipelineData;

GstCameraBinPreviewPipelineData *gst_camerabin_create_preview_pipeline (GstElement * element, GstElement * filter);
void gst_camerabin_destroy_preview_pipeline (GstCameraBinPreviewPipelineData * preview);
gboolean gst_camerabin_preview_pipeline_post (GstCameraBinPreviewPipelineData * preview, GstSample * sample);
void gst_camerabin_preview_set_caps (GstCameraBinPreviewPipelineData * preview, GstCaps * caps);
gboolean gst_camerabin_preview_set_filter (GstCameraBinPreviewPipelineData * preview, GstElement * filter);

#endif /* #ifndef __CAMERABIN_PREVIEW_H_ */
