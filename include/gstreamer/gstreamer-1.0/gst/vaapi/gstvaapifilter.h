/*
 *  gstvaapifilter.h - Video processing abstraction
 *
 *  Copyright (C) 2013 Intel Corporation
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

#ifndef GST_VAAPI_FILTER_H
#define GST_VAAPI_FILTER_H

#include <gst/vaapi/gstvaapisurface.h>
#include <gst/vaapi/video-format.h>

G_BEGIN_DECLS

typedef struct _GstVaapiFilter                  GstVaapiFilter;
typedef struct _GstVaapiFilterOpInfo            GstVaapiFilterOpInfo;

/**
 * @GST_VAAPI_FILTER_OP_FORMAT: Force output pixel format (#GstVideoFormat).
 * @GST_VAAPI_FILTER_OP_CROP: Crop source surface (#GstVaapiRectangle).
 * @GST_VAAPI_FILTER_OP_DENOISE: Noise reduction (float).
 * @GST_VAAPI_FILTER_OP_SHARPEN: Sharpening (float).
 * @GST_VAAPI_FILTER_OP_HUE: Change color hue (float).
 * @GST_VAAPI_FILTER_OP_SATURATION: Change saturation (float).
 * @GST_VAAPI_FILTER_OP_BRIGHTNESS: Change brightness (float).
 * @GST_VAAPI_FILTER_OP_CONTRAST: Change contrast (float).
 * @GST_VAAPI_FILTER_OP_SCALING: Change scaling method (#GstVaapiScaleMethod).
 * @GST_VAAPI_FILTER_OP_SKINTONE: Skin tone enhancement (bool).
 *
 * The set of operations that could be applied to the filter.
 */
typedef enum {
  GST_VAAPI_FILTER_OP_FORMAT = 1,
  GST_VAAPI_FILTER_OP_CROP,
  GST_VAAPI_FILTER_OP_DENOISE,
  GST_VAAPI_FILTER_OP_SHARPEN,
  GST_VAAPI_FILTER_OP_HUE,
  GST_VAAPI_FILTER_OP_SATURATION,
  GST_VAAPI_FILTER_OP_BRIGHTNESS,
  GST_VAAPI_FILTER_OP_CONTRAST,
  GST_VAAPI_FILTER_OP_DEINTERLACING,
  GST_VAAPI_FILTER_OP_SCALING,
  GST_VAAPI_FILTER_OP_SKINTONE,
} GstVaapiFilterOp;

/**
 * GstVaapiFilterOpInfo:
 * @operation: the #GstVaapiFilterOp
 * @pspec: the #GParamSpec describing the associated configurable value
 *
 * A #GstVaapiFilterOp descriptor.
 */
struct _GstVaapiFilterOpInfo
{
  const GstVaapiFilterOp op;
  GParamSpec *const pspec;
};

/**
 * GstVaapiFilterStatus:
 * @GST_VAAPI_FILTER_STATUS_SUCCESS: Success.
 * @GST_VAAPI_FILTER_STATUS_ERROR_ALLOCATION_FAILED: No memory left.
 * @GST_VAAPI_FILTER_STATUS_ERROR_OPERATION_FAILED: Operation failed.
 * @GST_VAAPI_FILTER_STATUS_ERROR_INVALID_PARAMETER: Invalid parameter.
 * @GST_VAAPI_FILTER_STATUS_ERROR_UNSUPPORTED_OPERATION: Unsupported operation.
 * @GST_VAAPI_FILTER_STATUS_ERROR_UNSUPPORTED_FORMAT: Unsupported target format.
 *
 * Video processing status for gst_vaapi_filter_process().
 */
typedef enum {
  GST_VAAPI_FILTER_STATUS_SUCCESS = 0,
  GST_VAAPI_FILTER_STATUS_ERROR_ALLOCATION_FAILED,
  GST_VAAPI_FILTER_STATUS_ERROR_OPERATION_FAILED,
  GST_VAAPI_FILTER_STATUS_ERROR_INVALID_PARAMETER,
  GST_VAAPI_FILTER_STATUS_ERROR_UNSUPPORTED_OPERATION,
  GST_VAAPI_FILTER_STATUS_ERROR_UNSUPPORTED_FORMAT,
} GstVaapiFilterStatus;

/**
 * GstVaapiScaleMethod:
 * @GST_VAAPI_SCALE_METHOD_DEFAULT: Default scaling mode.
 * @GST_VAAPI_SCALE_METHOD_FAST: Fast scaling mode, at the expense of quality.
 * @GST_VAAPI_SCALE_METHOD_HQ: High quality scaling mode, at the
 *   expense of speed.
 *
 * Scaling algorithms.
 */
typedef enum {
  GST_VAAPI_SCALE_METHOD_DEFAULT,
  GST_VAAPI_SCALE_METHOD_FAST,
  GST_VAAPI_SCALE_METHOD_HQ,
} GstVaapiScaleMethod;

/**
 * GstVaapiDeinterlaceMethod:
 * @GST_VAAPI_DEINTERLACE_METHOD_NONE: No deinterlacing.
 * @GST_VAAPI_DEINTERLACE_METHOD_BOB: Basic bob deinterlacing algorithm.
 * @GST_VAAPI_DEINTERLACE_METHOD_WEAVE: Weave deinterlacing algorithm.
 * @GST_VAAPI_DEINTERLACE_METHOD_MOTION_ADAPTIVE: Motion adaptive
 *   deinterlacing algorithm.
 * @GST_VAAPI_DEINTERLACE_METHOD_MOTION_COMPENSATED: Motion compensated
 *   deinterlacing algorithm.
 *
 * Deinterlacing algorithms.
 */
typedef enum {
  GST_VAAPI_DEINTERLACE_METHOD_NONE,
  GST_VAAPI_DEINTERLACE_METHOD_BOB,
  GST_VAAPI_DEINTERLACE_METHOD_WEAVE,
  GST_VAAPI_DEINTERLACE_METHOD_MOTION_ADAPTIVE,
  GST_VAAPI_DEINTERLACE_METHOD_MOTION_COMPENSATED,
} GstVaapiDeinterlaceMethod;

/**
 * GstVaapiDeinterlaceFlags:
 * @GST_VAAPI_DEINTERLACE_FLAG_TFF: Top-field first. If this flag is
 *   not set, then bottom-field first order is assumed. Note: this
 *   only affects the way reference frames are organized for advanced
 *   deinterlacing modes.
 * @GST_VAAPI_DEINTERLACE_FLAG_ONEFIELD: The input frame represents a
 *   single field. If this flag is not set, then the whole frame holds
 *   two interleaved fields.
 * @GST_VAAPI_DEINTERLACE_FLAG_TOPFIELD: The top field of the input
 *   frame is to be used for deinterlacing. Otherwise, if this flag is
 *   not set, then the bottom field of the input frame will be used
 *   for deinterlacing.
 *
 * The set of gst_vaapi_filter_set_deinterlacing() flags.
 */
typedef enum {
  GST_VAAPI_DEINTERLACE_FLAG_TFF = 1 << 31,
  GST_VAAPI_DEINTERLACE_FLAG_ONEFIELD = 1 << 30,
  GST_VAAPI_DEINTERLACE_FLAG_TOPFIELD = 1 << 29,
} GstVaapiDeinterlaceFlags;

#define GST_VAAPI_TYPE_SCALE_METHOD \
    gst_vaapi_scale_method_get_type()

#define GST_VAAPI_TYPE_DEINTERLACE_METHOD \
    gst_vaapi_deinterlace_method_get_type()

#define GST_VAAPI_TYPE_DEINTERLACE_FLAGS \
    gst_vaapi_deinterlace_flags_get_type()

GType
gst_vaapiscale_method_get_type (void) G_GNUC_CONST;

GType
gst_vaapi_deinterlace_method_get_type (void) G_GNUC_CONST;

GType
gst_vaapi_deinterlace_flags_get_type (void) G_GNUC_CONST;

GstVaapiFilter *
gst_vaapi_filter_new (GstVaapiDisplay * display);

GstVaapiFilter *
gst_vaapi_filter_ref (GstVaapiFilter * filter);

void
gst_vaapi_filter_unref (GstVaapiFilter * filter);

void
gst_vaapi_filter_replace (GstVaapiFilter ** old_filter_ptr,
    GstVaapiFilter * new_filter);

GPtrArray *
gst_vaapi_filter_get_operations (GstVaapiFilter * filter);

gboolean
gst_vaapi_filter_has_operation (GstVaapiFilter * filter, GstVaapiFilterOp op);

gboolean
gst_vaapi_filter_use_operation (GstVaapiFilter * filter, GstVaapiFilterOp op);

gboolean
gst_vaapi_filter_set_operation (GstVaapiFilter * filter, GstVaapiFilterOp op,
    const GValue * value);

GstVaapiFilterStatus
gst_vaapi_filter_process (GstVaapiFilter * filter,
    GstVaapiSurface * src_surface, GstVaapiSurface * dst_surface, guint flags);

GArray *
gst_vaapi_filter_get_formats (GstVaapiFilter * filter);

gboolean
gst_vaapi_filter_set_format (GstVaapiFilter * filter, GstVideoFormat format);

gboolean
gst_vaapi_filter_set_cropping_rectangle (GstVaapiFilter * filter,
    const GstVaapiRectangle * rect);

gboolean
gst_vaapi_filter_set_target_rectangle (GstVaapiFilter * filter,
    const GstVaapiRectangle * rect);

gboolean
gst_vaapi_filter_set_denoising_level (GstVaapiFilter * filter, gfloat level);

gboolean
gst_vaapi_filter_set_sharpening_level (GstVaapiFilter * filter, gfloat level);

gboolean
gst_vaapi_filter_set_hue (GstVaapiFilter * filter, gfloat value);

gboolean
gst_vaapi_filter_set_saturation (GstVaapiFilter * filter, gfloat value);

gboolean
gst_vaapi_filter_set_brightness (GstVaapiFilter * filter, gfloat value);

gboolean
gst_vaapi_filter_set_contrast (GstVaapiFilter * filter, gfloat value);

gboolean
gst_vaapi_filter_set_deinterlacing (GstVaapiFilter * filter,
    GstVaapiDeinterlaceMethod method, guint flags);

gboolean
gst_vaapi_filter_set_deinterlacing_references (GstVaapiFilter * filter,
    GstVaapiSurface ** forward_references, guint num_forward_references,
    GstVaapiSurface ** backward_references, guint num_backward_references);

gboolean
gst_vaapi_filter_set_scaling (GstVaapiFilter * filter,
    GstVaapiScaleMethod method);

gboolean
gst_vaapi_filter_set_skintone (GstVaapiFilter * filter,
    gboolean enhance);

#endif /* GST_VAAPI_FILTER_H */
