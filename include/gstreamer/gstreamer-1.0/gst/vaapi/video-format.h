/*
 *  video-format.h - Video format helpers for VA-API
 *
 *  Copyright (C) 2010-2011 Splitted-Desktop Systems
 *    Author: Gwenole Beauchesne <gwenole.beauchesne@splitted-desktop.com>
 *  Copyright (C) 2011-2013 Intel Corporation
 *    Author: Gwenole Beauchesne <gwenole.beauchesne@intel.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 */

#ifndef GST_VAAPI_VIDEO_FORMAT_H
#define GST_VAAPI_VIDEO_FORMAT_H

#include <gst/video/video.h>

G_BEGIN_DECLS

const gchar *
gst_vaapi_video_format_to_string (GstVideoFormat format);

gboolean
gst_vaapi_video_format_is_rgb (GstVideoFormat format);

gboolean
gst_vaapi_video_format_is_yuv (GstVideoFormat format);

GstVideoFormat
gst_vaapi_video_format_from_va_fourcc (guint32 fourcc);

GstVideoFormat
gst_vaapi_video_format_from_va_format (const VAImageFormat * va_format);

const VAImageFormat *
gst_vaapi_video_format_to_va_format (GstVideoFormat format);

guint
gst_vaapi_video_format_get_chroma_type (GstVideoFormat format);

guint
gst_vaapi_video_format_get_score (GstVideoFormat format);

G_END_DECLS

#endif /* GST_VAAPI_VIDEO_FORMAT_H */
