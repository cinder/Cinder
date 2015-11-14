/*
 *  gstvaapiencoder_h264.h - H.264 encoder
 *
 *  Copyright (C) 2011-2014 Intel Corporation
 *    Author: Wind Yuan <feng.yuan@intel.com>
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

#ifndef GST_VAAPI_ENCODER_H264_H
#define GST_VAAPI_ENCODER_H264_H

#include <gst/vaapi/gstvaapiencoder.h>
#include <gst/vaapi/gstvaapiutils_h264.h>

G_BEGIN_DECLS

#define GST_VAAPI_ENCODER_H264(encoder) \
  ((GstVaapiEncoderH264 *) (encoder))

typedef struct _GstVaapiEncoderH264 GstVaapiEncoderH264;

/**
 * GstVaapiEncoderH264Prop:
 * @GST_VAAPI_ENCODER_H264_PROP_MAX_BFRAMES: Number of B-frames between I
 *   and P (uint).
 * @GST_VAAPI_ENCODER_H264_PROP_INIT_QP: Initial quantizer value (uint).
 * @GST_VAAPI_ENCODER_H264_PROP_MIN_QP: Minimal quantizer value (uint).
 * @GST_VAAPI_ENCODER_H264_PROP_NUM_SLICES: Number of slices per frame (uint).
 * @GST_VAAPI_ENCODER_H264_PROP_CABAC: Enable CABAC entropy coding mode (bool).
 * @GST_VAAPI_ENCODER_H264_PROP_DCT8X8: Enable adaptive use of 8x8
 *   transforms in I-frames (bool).
 * @GST_VAAPI_ENCODER_H264_PROP_CPB_LENGTH: Length of the CPB buffer
 *   in milliseconds (uint).
 * @GST_VAAPI_ENCODER_H264_PROP_NUM_VIEWS: Number of views per frame.
 * @GST_VAAPI_ENCODER_H264_PROP_VIEW_IDS: View IDs
 *
 * The set of H.264 encoder specific configurable properties.
 */
typedef enum {
  GST_VAAPI_ENCODER_H264_PROP_MAX_BFRAMES = -1,
  GST_VAAPI_ENCODER_H264_PROP_INIT_QP = -2,
  GST_VAAPI_ENCODER_H264_PROP_MIN_QP = -3,
  GST_VAAPI_ENCODER_H264_PROP_NUM_SLICES = -4,
  GST_VAAPI_ENCODER_H264_PROP_CABAC = -5,
  GST_VAAPI_ENCODER_H264_PROP_DCT8X8 = -6,
  GST_VAAPI_ENCODER_H264_PROP_CPB_LENGTH = -7,
  GST_VAAPI_ENCODER_H264_PROP_NUM_VIEWS = -8,
  GST_VAAPI_ENCODER_H264_PROP_VIEW_IDS = -9
} GstVaapiEncoderH264Prop;

GstVaapiEncoder *
gst_vaapi_encoder_h264_new (GstVaapiDisplay * display);

GPtrArray *
gst_vaapi_encoder_h264_get_default_properties (void);

gboolean
gst_vaapi_encoder_h264_set_max_profile (GstVaapiEncoderH264 * encoder,
    GstVaapiProfile profile);

gboolean
gst_vaapi_encoder_h264_get_profile_and_level (GstVaapiEncoderH264 * encoder,
    GstVaapiProfile * out_profile_ptr, GstVaapiLevelH264 * out_level_ptr);

G_END_DECLS

#endif /*GST_VAAPI_ENCODER_H264_H */
