/*
 *  gstvaapidecoder_h265.h - H.265 decoder
 *
 *  Copyright (C) 2015 Intel Corporation
 *    Author: Sreerenj Balachandran <sreerenj.balachandran@intel.com>
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

#ifndef GST_VAAPI_DECODER_H265_H
#define GST_VAAPI_DECODER_H265_H

#include <gst/vaapi/gstvaapidecoder.h>

G_BEGIN_DECLS

#define GST_VAAPI_DECODER_H265(decoder) \
    ((GstVaapiDecoderH265 *)(decoder))

typedef struct _GstVaapiDecoderH265             GstVaapiDecoderH265;

/**
 * GstVaapiStreamAlignH265:
 * @GST_VAAPI_STREAM_ALIGN_H265_NONE: Generic H.265 stream buffers
 * @GST_VAAPI_STREAM_ALIGN_H265_NALU: H.265 stream buffers aligned NAL
 *   unit boundaries
 * @GST_VAAPI_STREAM_ALIGN_H265_AU: H.265 stream buffers aligned on
 *   access unit boundaries
 *
 * Set of possible buffer alignments for H.265 streams.
 */
typedef enum {
    GST_VAAPI_STREAM_ALIGN_H265_NONE,
    GST_VAAPI_STREAM_ALIGN_H265_NALU,
    GST_VAAPI_STREAM_ALIGN_H265_AU
} GstVaapiStreamAlignH265;

GstVaapiDecoder *
gst_vaapi_decoder_h265_new(GstVaapiDisplay *display, GstCaps *caps);

void
gst_vaapi_decoder_h265_set_alignment(GstVaapiDecoderH265 *decoder,
    GstVaapiStreamAlignH265 alignment);

G_END_DECLS

#endif /* GST_VAAPI_DECODER_H265_H */
