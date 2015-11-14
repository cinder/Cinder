/*
 *  gstvaapiutils_h264.h - H.264 related utilities
 *
 *  Copyright (C) 2011-2014 Intel Corporation
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

#ifndef GST_VAAPI_UTILS_H264_H
#define GST_VAAPI_UTILS_H264_H

#include <va/va.h>
#include <gst/vaapi/gstvaapiprofile.h>
#include <gst/vaapi/gstvaapisurface.h>

G_BEGIN_DECLS

/**
 * GstVaapiLevelH264:
 * @GST_VAAPI_LEVEL_H264_L1: H.264 level 1.
 * @GST_VAAPI_LEVEL_H264_L1_1: H.264 level 1.1.
 * @GST_VAAPI_LEVEL_H264_L1_2: H.264 level 1.2.
 * @GST_VAAPI_LEVEL_H264_L1_3: H.264 level 1.3.
 * @GST_VAAPI_LEVEL_H264_L2: H.264 level 2.
 * @GST_VAAPI_LEVEL_H264_L2_1: H.264 level 2.1.
 * @GST_VAAPI_LEVEL_H264_L2_2: H.264 level 2.2.
 * @GST_VAAPI_LEVEL_H264_L3: H.264 level 3.
 * @GST_VAAPI_LEVEL_H264_L3_1: H.264 level 3.1.
 * @GST_VAAPI_LEVEL_H264_L3_2: H.264 level 3.2.
 * @GST_VAAPI_LEVEL_H264_L4: H.264 level 4.
 * @GST_VAAPI_LEVEL_H264_L4_1: H.264 level 4.1.
 * @GST_VAAPI_LEVEL_H264_L4_2: H.264 level 4.2.
 * @GST_VAAPI_LEVEL_H264_L5: H.264 level 5.
 * @GST_VAAPI_LEVEL_H264_L5_1: H.264 level 5.1.
 * @GST_VAAPI_LEVEL_H264_L5_2: H.264 level 5.2.
 *
 * The set of all levels for #GstVaapiLevelH264.
 */
typedef enum {
  GST_VAAPI_LEVEL_H264_L1 = 1,
  GST_VAAPI_LEVEL_H264_L1b,
  GST_VAAPI_LEVEL_H264_L1_1,
  GST_VAAPI_LEVEL_H264_L1_2,
  GST_VAAPI_LEVEL_H264_L1_3,
  GST_VAAPI_LEVEL_H264_L2,
  GST_VAAPI_LEVEL_H264_L2_1,
  GST_VAAPI_LEVEL_H264_L2_2,
  GST_VAAPI_LEVEL_H264_L3,
  GST_VAAPI_LEVEL_H264_L3_1,
  GST_VAAPI_LEVEL_H264_L3_2,
  GST_VAAPI_LEVEL_H264_L4,
  GST_VAAPI_LEVEL_H264_L4_1,
  GST_VAAPI_LEVEL_H264_L4_2,
  GST_VAAPI_LEVEL_H264_L5,
  GST_VAAPI_LEVEL_H264_L5_1,
  GST_VAAPI_LEVEL_H264_L5_2,
} GstVaapiLevelH264;

/* Returns a relative score for the supplied GstVaapiProfile */
guint
gst_vaapi_utils_h264_get_profile_score (GstVaapiProfile profile);

/* Returns GstVaapiProfile from a string representation */
GstVaapiProfile
gst_vaapi_utils_h264_get_profile_from_string (const gchar * str);

/* Returns a string representation for the supplied H.264 profile */
const gchar *
gst_vaapi_utils_h264_get_profile_string (GstVaapiProfile profile);

/* Returns GstVaapiLevelH264 from a string representation */
GstVaapiLevelH264
gst_vaapi_utils_h264_get_level_from_string (const gchar * str);

/* Returns a string representation for the supplied H.264 level */
const gchar *
gst_vaapi_utils_h264_get_level_string (GstVaapiLevelH264 level);

G_END_DECLS

#endif /* GST_VAAPI_UTILS_H264_H */
