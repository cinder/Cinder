/*
 *  gstvaapiencoder_mpeg2.h - MPEG-2 encoder
 *
 *  Copyright (C) 2011-2014 Intel Corporation
 *    Author: Guangxin Xu <guangxin.xu@intel.com>
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

#ifndef GST_VAAPI_ENCODER_MPEG2_H
#define GST_VAAPI_ENCODER_MPEG2_H

#include <gst/vaapi/gstvaapiencoder.h>

G_BEGIN_DECLS

#define GST_VAAPI_ENCODER_MPEG2(encoder) \
  ((GstVaapiEncoderMpeg2 *) (encoder))

typedef struct _GstVaapiEncoderMpeg2 GstVaapiEncoderMpeg2;

/**
 * GstVaapiEncoderMpeg2Prop:
 * @GST_VAAPI_ENCODER_MPEG2_PROP_QUANTIZER: Constant quantizer value (uint).
 * @GST_VAAPI_ENCODER_MPEG2_PROP_MAX_BFRAMES: Number of B-frames between I
 *   and P (uint).
 *
 * The set of MPEG-2 encoder specific configurable properties.
 */
typedef enum {
  GST_VAAPI_ENCODER_MPEG2_PROP_QUANTIZER = -1,
  GST_VAAPI_ENCODER_MPEG2_PROP_MAX_BFRAMES = -2,
} GstVaapiEncoderMpeg2Prop;

GstVaapiEncoder *
gst_vaapi_encoder_mpeg2_new (GstVaapiDisplay * display);

GPtrArray *
gst_vaapi_encoder_mpeg2_get_default_properties (void);

G_END_DECLS

#endif /* GST_VAAPI_ENCODER_MPEG2_H */
