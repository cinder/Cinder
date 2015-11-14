/*
 *  gstvaapiutils_h265.h - H.265 related utilities
 *
 *  Copyright (C) 2015 Intel Corporation
 *    Author: Sreerenj Balachandran <sreerenj.balachandran@intel.com>
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

#ifndef GST_VAAPI_UTILS_H265_H
#define GST_VAAPI_UTILS_H265_H

#include <va/va.h>
#include <gst/vaapi/gstvaapiprofile.h>
#include <gst/vaapi/gstvaapisurface.h>

G_BEGIN_DECLS

/**
 * GstVaapiLevelH265:
 * @GST_VAAPI_LEVEL_H265_L1: H.265 level 1.
 * @GST_VAAPI_LEVEL_H265_L2: H.265 level 2.
 * @GST_VAAPI_LEVEL_H265_L2_1: H.265 level 2.1.
 * @GST_VAAPI_LEVEL_H265_L3: H.265 level 3.
 * @GST_VAAPI_LEVEL_H265_L3_1: H.265 level 3.1.
 * @GST_VAAPI_LEVEL_H265_L4: H.265 level 4.
 * @GST_VAAPI_LEVEL_H265_L4_1: H.265 level 4.1.
 * @GST_VAAPI_LEVEL_H265_L5: H.265 level 5.
 * @GST_VAAPI_LEVEL_H265_L5_1: H.265 level 5.1.
 * @GST_VAAPI_LEVEL_H265_L5_2: H.265 level 5.2.
 * @GST_VAAPI_LEVEL_H265_L6: H.265 level 6.
 * @GST_VAAPI_LEVEL_H265_L6_1: H.265 level 6.1.
 * @GST_VAAPI_LEVEL_H265_L6_2: H.265 level 6.2.
 *
 * The set of all levels for #GstVaapiLevelH265.
 */
typedef enum {
  GST_VAAPI_LEVEL_H265_L1 = 1,
  GST_VAAPI_LEVEL_H265_L2,
  GST_VAAPI_LEVEL_H265_L2_1,
  GST_VAAPI_LEVEL_H265_L3,
  GST_VAAPI_LEVEL_H265_L3_1,
  GST_VAAPI_LEVEL_H265_L4,
  GST_VAAPI_LEVEL_H265_L4_1,
  GST_VAAPI_LEVEL_H265_L5,
  GST_VAAPI_LEVEL_H265_L5_1,
  GST_VAAPI_LEVEL_H265_L5_2,
  GST_VAAPI_LEVEL_H265_L6,
  GST_VAAPI_LEVEL_H265_L6_1,
  GST_VAAPI_LEVEL_H265_L6_2,
} GstVaapiLevelH265;

/**
 * GstVaapiTierH265:
 * GST_VAAPI_TIER_H265_MAIN: H265 Tier 0
 * GST_VAAPI_TIER_H265_HIGH: H265 Tier 1
 * GST_VAAPI_TIER_H265_UNKNOWN: Unknown Tier
 *
 * The set of all Tier for #GstVaapiTierH265.
 */
typedef enum {
  GST_VAAPI_TIER_H265_MAIN,
  GST_VAAPI_TIER_H265_HIGH,
  GST_VAAPI_TIER_H265_UNKNOWN = -1
} GstVaapiTierH265;

/* Returns a relative score for the supplied GstVaapiProfile */
guint
gst_vaapi_utils_h265_get_profile_score (GstVaapiProfile profile);

/* Returns GstVaapiProfile from a string representation */
GstVaapiProfile
gst_vaapi_utils_h265_get_profile_from_string (const gchar * str);

/* Returns a string representation for the supplied H.265 profile */
const gchar *
gst_vaapi_utils_h265_get_profile_string (GstVaapiProfile profile);

/* Returns GstVaapiLevelH265 from a string representation */
GstVaapiLevelH265
gst_vaapi_utils_h265_get_level_from_string (const gchar * str);

/* Returns a string representation for the supplied H.265 level */
const gchar *
gst_vaapi_utils_h265_get_level_string (GstVaapiLevelH265 level);

/* Returns GstVaapiTierH265 from a string representation */
GstVaapiTierH265
gst_vaapi_utils_h265_get_tier_from_string (const gchar * str);

/* Returns a string representation for the supplied H.265 Tier */
const gchar *
gst_vaapi_utils_h265_get_tier_string (GstVaapiTierH265 tier);

G_END_DECLS

#endif /* GST_VAAPI_UTILS_H265_H */
