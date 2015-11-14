/*
 *  gstvaapisubpicture.h - VA subpicture abstraction
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

#ifndef GST_VAAPI_SUBPICTURE_H
#define GST_VAAPI_SUBPICTURE_H

#include <gst/vaapi/gstvaapiobject.h>
#include <gst/vaapi/gstvaapidisplay.h>
#include <gst/vaapi/gstvaapiimage.h>
#include <gst/video/video-overlay-composition.h>

G_BEGIN_DECLS

#define GST_VAAPI_SUBPICTURE(obj) \
    ((GstVaapiSubpicture *)(obj))

typedef struct _GstVaapiSubpicture              GstVaapiSubpicture;

/**
 * GstVaapiSubpictureFlags:
 * @GST_VAAPI_SUBPICTURE_FLAG_PREMULTIPLIED_ALPHA:
 *   subpicture has RGB pixels with pre-multiplied alpha
 * @GST_VAAPI_SUBPICTURE_FLAG_GLOBAL_ALPHA:
 *   subpicture needs to be blended with some global-alpha value at
 *   rendering time
 *
 * The set of all subpicture rendering flags for #GstVaapiSubpicture.
 */
typedef enum {
    GST_VAAPI_SUBPICTURE_FLAG_PREMULTIPLIED_ALPHA    = (1 << 0),
    GST_VAAPI_SUBPICTURE_FLAG_GLOBAL_ALPHA           = (1 << 1),
} GstVaapiSubpictureFlags;

GstVaapiSubpicture *
gst_vaapi_subpicture_new(GstVaapiImage *image, guint flags);

GstVaapiSubpicture *
gst_vaapi_subpicture_new_from_overlay_rectangle(
    GstVaapiDisplay          *display,
    GstVideoOverlayRectangle *rect
);

GstVaapiID
gst_vaapi_subpicture_get_id(GstVaapiSubpicture *subpicture);

guint
gst_vaapi_subpicture_get_flags(GstVaapiSubpicture *subpicture);

GstVaapiImage *
gst_vaapi_subpicture_get_image(GstVaapiSubpicture *subpicture);

gboolean
gst_vaapi_subpicture_set_image(GstVaapiSubpicture *subpicture,
    GstVaapiImage *image);

gfloat
gst_vaapi_subpicture_get_global_alpha(GstVaapiSubpicture *subpicture);

gboolean
gst_vaapi_subpicture_set_global_alpha(GstVaapiSubpicture *subpicture,
    gfloat global_alpha);

G_END_DECLS

#endif /* GST_VAAPI_SUBPICTURE_H */
