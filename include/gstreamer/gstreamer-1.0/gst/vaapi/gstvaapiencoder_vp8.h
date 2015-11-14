/*
 *  gstvaapiencoder_vp8.h VP8G encoder
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

#ifndef GST_VAAPI_ENCODER_VP8_H
#define GST_VAAPI_ENCODER_VP8_H

#include <gst/vaapi/gstvaapiencoder.h>

G_BEGIN_DECLS

#define GST_VAAPI_ENCODER_VP8(encoder) \
  ((GstVaapiEncoderVP8 *) (encoder))

typedef struct _GstVaapiEncoderVP8 GstVaapiEncoderVP8;

/**
 * GstVaapiEncoderVP8Prop:
 * @GST_VAAPI_ENCODER_VP8_PROP_LOOP_FILTER_LEVEL: Loop Filter Level(uint).
 * @GST_VAAPI_ENCODER_VP8_PROP_LOOP_SHARPNESS_LEVEL: Sharpness Level(uint).
 * @GST_VAAPI_ENCODER_VP8_PROP_YAC_Q_INDEX: Quantization table index for luma AC(uint).
 *
 * The set of VP8 encoder specific configurable properties.
 */
typedef enum {
  GST_VAAPI_ENCODER_VP8_PROP_LOOP_FILTER_LEVEL = -1,
  GST_VAAPI_ENCODER_VP8_PROP_SHARPNESS_LEVEL = -2,
  GST_VAAPI_ENCODER_VP8_PROP_YAC_Q_INDEX = -3
} GstVaapiEncoderVP8Prop;

GstVaapiEncoder *
gst_vaapi_encoder_vp8_new (GstVaapiDisplay * display);

GPtrArray *
gst_vaapi_encoder_vp8_get_default_properties (void);

G_END_DECLS
#endif /*GST_VAAPI_ENCODER_VP8_H */
