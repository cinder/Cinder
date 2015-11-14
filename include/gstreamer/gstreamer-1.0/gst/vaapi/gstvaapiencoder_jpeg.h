/*
 *  gstvaapiencoder_jpeg.h JPEGG encoder
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

#ifndef GST_VAAPI_ENCODER_JPEG_H
#define GST_VAAPI_ENCODER_JPEG_H

#include <gst/vaapi/gstvaapiencoder.h>

G_BEGIN_DECLS

#define GST_VAAPI_ENCODER_JPEG(encoder) \
  ((GstVaapiEncoderJpeg *) (encoder))

typedef struct _GstVaapiEncoderJpeg GstVaapiEncoderJpeg;

/**
 * GstVaapiEncoderJpegProp:
 * @GST_VAAPI_ENCODER_JPEG_PROP_QUALITY: Quality Factor value (uint).
 *
 * The set of JPEG encoder specific configurable properties.
 */
typedef enum {
  GST_VAAPI_ENCODER_JPEG_PROP_QUALITY = -1
} GstVaapiEncoderJpegProp;

GstVaapiEncoder *
gst_vaapi_encoder_jpeg_new (GstVaapiDisplay * display);

GPtrArray *
gst_vaapi_encoder_jpeg_get_default_properties (void);

G_END_DECLS
#endif /*GST_VAAPI_ENCODER_JPEG_H */
