/*
 *  gstvaapiutils_mpeg2.h - MPEG-2 related utilities
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

#ifndef GST_VAAPI_UTILS_MPEG2_H
#define GST_VAAPI_UTILS_MPEG2_H

#include <va/va.h>
#include <gst/vaapi/gstvaapiprofile.h>
#include <gst/vaapi/gstvaapisurface.h>

G_BEGIN_DECLS

/**
 * GstVaapiLevelMPEG2:
 * @GST_VAAPI_LEVEL_MPEG2_LOW: Low level.
 * @GST_VAAPI_LEVEL_MPEG2_MAIN: Main level.
 * @GST_VAAPI_LEVEL_MPEG2_HIGH_1440: High-1440 level.
 * @GST_VAAPI_LEVEL_MPEG2_HIGH: High level.
 *
 * The set of all levels for #GstVaapiLevelMPEG2.
 */
typedef enum {
  GST_VAAPI_LEVEL_MPEG2_LOW = 1,
  GST_VAAPI_LEVEL_MPEG2_MAIN,
  GST_VAAPI_LEVEL_MPEG2_HIGH_1440,
  GST_VAAPI_LEVEL_MPEG2_HIGH,
} GstVaapiLevelMPEG2;

/* Returns a relative score for the supplied GstVaapiProfile */
guint
gst_vaapi_utils_mpeg2_get_profile_score (GstVaapiProfile profile);

/* Returns GstVaapiProfile from a string representation */
GstVaapiProfile
gst_vaapi_utils_mpeg2_get_profile_from_string (const gchar * str);

/* Returns a string representation for the supplied MPEG-2 profile */
const gchar *
gst_vaapi_utils_mpeg2_get_profile_string (GstVaapiProfile profile);

/* Returns GstVaapiLevelMPEG2 from a string representation */
GstVaapiLevelMPEG2
gst_vaapi_utils_mpeg2_get_level_from_string (const gchar * str);

/* Returns a string representation for the supplied MPEG-2 level */
const gchar *
gst_vaapi_utils_mpeg2_get_level_string (GstVaapiLevelMPEG2 level);

G_END_DECLS

#endif /* GST_VAAPI_UTILS_MPEG2_H */
