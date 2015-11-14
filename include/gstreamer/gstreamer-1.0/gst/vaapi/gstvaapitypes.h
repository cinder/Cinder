/*
 *  gstvaapitypes.h - Basic types
 *
 *  Copyright (C) 2010-2011 Splitted-Desktop Systems
 *    Author: Gwenole Beauchesne <gwenole.beauchesne@splitted-desktop.com>
 *  Copyright (C) 2012-2014 Intel Corporation
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

#ifndef GST_VAAPI_TYPES_H
#define GST_VAAPI_TYPES_H

#include <glib.h>

G_BEGIN_DECLS

/**
 * GstVaapiID:
 *
 * An integer large enough to hold a generic VA id or a pointer
 * wherever necessary.
 */
typedef gsize GstVaapiID;

/**
 * GST_VAAPI_ID:
 * @id: an arbitrary integer value
 *
 * Macro that creates a #GstVaapiID from @id.
 */
#define GST_VAAPI_ID(id) ((GstVaapiID)(id))

/**
 * GST_VAAPI_ID_INVALID:
 *
 * Macro that evaluates to an invalid #GstVaapiID value.
 */
#define GST_VAAPI_ID_INVALID GST_VAAPI_ID((gssize)(gint32)-1)

/**
 * GST_VAAPI_ID_FORMAT:
 *
 * Can be used together with #GST_VAAPI_ID_ARGS to properly output an
 * integer value in a printf()-style text message.
 * <informalexample>
 * <programlisting>
 * printf("id: %" GST_VAAPI_ID_FORMAT "\n", GST_VAAPI_ID_ARGS(id));
 * </programlisting>
 * </informalexample>
 */
#define GST_VAAPI_ID_FORMAT "p"

/**
 * GST_VAAPI_ID_ARGS:
 * @id: a #GstVaapiID
 *
 * Can be used together with #GST_VAAPI_ID_FORMAT to properly output
 * an integer value in a printf()-style text message.
 */
#define GST_VAAPI_ID_ARGS(id) GSIZE_TO_POINTER(id)

/**
 * GstVaapiPoint:
 * @x: X coordinate
 * @y: Y coordinate
 *
 * A location within a surface.
 */
typedef struct _GstVaapiPoint GstVaapiPoint;
struct _GstVaapiPoint {
    guint32 x;
    guint32 y;
};

/**
 * GstVaapiRectangle:
 * @x: X coordinate
 * @y: Y coordinate
 * @width: region width
 * @height: region height
 *
 * A rectangle region within a surface.
 */
typedef struct _GstVaapiRectangle GstVaapiRectangle;
struct _GstVaapiRectangle {
    guint32 x;
    guint32 y;
    guint32 width;
    guint32 height;
};

/**
 * GstVaapiRenderMode:
 * @GST_VAAPI_RENDER_MODE_OVERLAY: in this mode, the VA display
 *   backend renders surfaces with an overlay engine. This means that
 *   the surface that is currently displayed shall not be re-used
 *   right away for decoding. i.e. it needs to be retained further,
 *   until the next surface is to be displayed.
 * @GST_VAAPI_RENDER_MODE_TEXTURE: in this modem the VA display
 *   backend renders surfaces with a textured blit (GPU/3D engine).
 *   This means that the surface is copied to some intermediate
 *   backing store, or back buffer of a frame buffer, and is free to
 *   be re-used right away for decoding.
 */
typedef enum {
    GST_VAAPI_RENDER_MODE_OVERLAY = 1,
    GST_VAAPI_RENDER_MODE_TEXTURE
} GstVaapiRenderMode;

/**
 * GstVaapiRotation:
 * @GST_VAAPI_ROTATION_0: the VA display is not rotated.
 * @GST_VAAPI_ROTATION_90: the VA display is rotated by 90°, clockwise.
 * @GST_VAAPI_ROTATION_180: the VA display is rotated by 180°, clockwise.
 * @GST_VAAPI_ROTATION_270: the VA display is rotated by 270°, clockwise.
 */
typedef enum {
    GST_VAAPI_ROTATION_0   = 0,
    GST_VAAPI_ROTATION_90  = 90,
    GST_VAAPI_ROTATION_180 = 180,
    GST_VAAPI_ROTATION_270 = 270,
} GstVaapiRotation;

/**
 * GstVaapiRateControl:
 * @GST_VAAPI_RATECONTROL_NONE: No rate control performed by the
 *   underlying driver
 * @GST_VAAPI_RATECONTROL_CQP: Constant QP
 * @GST_VAAPI_RATECONTROL_CBR: Constant bitrate
 * @GST_VAAPI_RATECONTROL_VCM: Video conference mode
 * @GST_VAAPI_RATECONTROL_VBR: Variable bitrate
 * @GST_VAAPI_RATECONTROL_VBR_CONSTRAINED: Variable bitrate with peak
 *   rate higher than average bitrate
 *
 * The set of allowed rate control values for #GstVaapiRateControl.
 * Note: this is only valid for encoders.
 */
typedef enum {
    GST_VAAPI_RATECONTROL_NONE = 0,
    GST_VAAPI_RATECONTROL_CQP,
    GST_VAAPI_RATECONTROL_CBR,
    GST_VAAPI_RATECONTROL_VCM,
    GST_VAAPI_RATECONTROL_VBR,
    GST_VAAPI_RATECONTROL_VBR_CONSTRAINED,
} GstVaapiRateControl;

/* Define a mask for GstVaapiRateControl */
#define GST_VAAPI_RATECONTROL_MASK(RC) \
    (1 << G_PASTE(GST_VAAPI_RATECONTROL_,RC))

G_END_DECLS

#endif /* GST_VAAPI_TYPES_H */
