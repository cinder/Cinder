/*
 *  gstvaapidecoder.h - VA decoder abstraction
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

#ifndef GST_VAAPI_DECODER_H
#define GST_VAAPI_DECODER_H

#include <gst/gstbuffer.h>
#include <gst/base/gstadapter.h>
#include <gst/vaapi/gstvaapisurfaceproxy.h>
#include <gst/video/gstvideoutils.h>

G_BEGIN_DECLS

#define GST_VAAPI_DECODER(obj) \
    ((GstVaapiDecoder *)(obj))

typedef struct _GstVaapiDecoder GstVaapiDecoder;
typedef void (*GstVaapiDecoderStateChangedFunc) (GstVaapiDecoder * decoder,
    const GstVideoCodecState * codec_state, gpointer user_data);

/**
 * GstVaapiDecoderStatus:
 * @GST_VAAPI_DECODER_STATUS_SUCCESS: Success.
 * @GST_VAAPI_DECODER_STATUS_END_OF_STREAM: End-Of-Stream.
 * @GST_VAAPI_DECODER_STATUS_ERROR_ALLOCATION_FAILED: No memory left.
 * @GST_VAAPI_DECODER_STATUS_ERROR_INIT_FAILED: Decoder initialization failure.
 * @GST_VAAPI_DECODER_STATUS_ERROR_UNSUPPORTED_CODEC: Unsupported codec.
 * @GST_VAAPI_DECODER_STATUS_ERROR_NO_DATA: Not enough input data to decode.
 * @GST_VAAPI_DECODER_STATUS_ERROR_NO_SURFACE: No surface left to hold the decoded picture.
 * @GST_VAAPI_DECODER_STATUS_ERROR_INVALID_SURFACE: Invalid surface.
 * @GST_VAAPI_DECODER_STATUS_ERROR_BITSTREAM_PARSER: Invalid or unsupported bitstream data.
 * @GST_VAAPI_DECODER_STATUS_ERROR_UNSUPPORTED_PROFILE: Unsupported codec profile.
 * @GST_VAAPI_DECODER_STATUS_ERROR_UNSUPPORTED_CHROMA_FORMAT: Unsupported chroma format.
 * @GST_VAAPI_DECODER_STATUS_ERROR_INVALID_PARAMETER: Unsupported parameter.
 * @GST_VAAPI_DECODER_STATUS_ERROR_UNKNOWN: Unknown error.
 *
 * Decoder status for gst_vaapi_decoder_get_surface().
 */
typedef enum {
  GST_VAAPI_DECODER_STATUS_SUCCESS = 0,
  GST_VAAPI_DECODER_STATUS_END_OF_STREAM,
  GST_VAAPI_DECODER_STATUS_ERROR_ALLOCATION_FAILED,
  GST_VAAPI_DECODER_STATUS_ERROR_INIT_FAILED,
  GST_VAAPI_DECODER_STATUS_ERROR_UNSUPPORTED_CODEC,
  GST_VAAPI_DECODER_STATUS_ERROR_NO_DATA,
  GST_VAAPI_DECODER_STATUS_ERROR_NO_SURFACE,
  GST_VAAPI_DECODER_STATUS_ERROR_INVALID_SURFACE,
  GST_VAAPI_DECODER_STATUS_ERROR_BITSTREAM_PARSER,
  GST_VAAPI_DECODER_STATUS_ERROR_UNSUPPORTED_PROFILE,
  GST_VAAPI_DECODER_STATUS_ERROR_UNSUPPORTED_CHROMA_FORMAT,
  GST_VAAPI_DECODER_STATUS_ERROR_INVALID_PARAMETER,
  GST_VAAPI_DECODER_STATUS_ERROR_UNKNOWN = -1
} GstVaapiDecoderStatus;

GstVaapiDecoder *
gst_vaapi_decoder_ref (GstVaapiDecoder * decoder);

void
gst_vaapi_decoder_unref (GstVaapiDecoder * decoder);

void
gst_vaapi_decoder_replace (GstVaapiDecoder ** old_decoder_ptr,
    GstVaapiDecoder * new_decoder);

gpointer
gst_vaapi_decoder_get_user_data (GstVaapiDecoder * decoder);

void
gst_vaapi_decoder_set_user_data (GstVaapiDecoder * decoder, gpointer user_data);

GstVaapiCodec
gst_vaapi_decoder_get_codec (GstVaapiDecoder * decoder);

GstVideoCodecState *
gst_vaapi_decoder_get_codec_state (GstVaapiDecoder * decoder);

void
gst_vaapi_decoder_set_codec_state_changed_func (GstVaapiDecoder * decoder,
    GstVaapiDecoderStateChangedFunc func, gpointer user_data);

GstCaps *
gst_vaapi_decoder_get_caps (GstVaapiDecoder * decoder);

gboolean
gst_vaapi_decoder_put_buffer (GstVaapiDecoder * decoder, GstBuffer * buf);

GstVaapiDecoderStatus
gst_vaapi_decoder_get_surface (GstVaapiDecoder * decoder,
    GstVaapiSurfaceProxy ** out_proxy_ptr);

GstVaapiDecoderStatus
gst_vaapi_decoder_get_frame (GstVaapiDecoder * decoder,
    GstVideoCodecFrame ** out_frame_ptr);

GstVaapiDecoderStatus
gst_vaapi_decoder_get_frame_with_timeout (GstVaapiDecoder * decoder,
    GstVideoCodecFrame ** out_frame_ptr, guint64 timeout);

GstVaapiDecoderStatus
gst_vaapi_decoder_parse (GstVaapiDecoder * decoder,
    GstVideoCodecFrame * frame, GstAdapter * adapter, gboolean at_eos,
    guint * got_unit_size_ptr, gboolean * got_frame_ptr);

GstVaapiDecoderStatus
gst_vaapi_decoder_decode (GstVaapiDecoder * decoder,
    GstVideoCodecFrame * frame);

GstVaapiDecoderStatus
gst_vaapi_decoder_flush (GstVaapiDecoder * decoder);

GstVaapiDecoderStatus
gst_vaapi_decoder_check_status (GstVaapiDecoder * decoder);

G_END_DECLS

#endif /* GST_VAAPI_DECODER_H */
