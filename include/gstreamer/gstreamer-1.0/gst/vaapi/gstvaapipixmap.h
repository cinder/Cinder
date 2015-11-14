/*
 *  gstvaapipixmap.h - Pixmap abstraction
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

#ifndef GST_VAAPI_PIXMAP_H
#define GST_VAAPI_PIXMAP_H

#include <gst/vaapi/gstvaapitypes.h>
#include <gst/vaapi/gstvaapiobject.h>
#include <gst/vaapi/gstvaapidisplay.h>
#include <gst/vaapi/gstvaapisurface.h>
#include <gst/vaapi/video-format.h>

G_BEGIN_DECLS

#define GST_VAAPI_PIXMAP(obj) \
    ((GstVaapiPixmap *)(obj))

typedef struct _GstVaapiPixmap                  GstVaapiPixmap;
typedef struct _GstVaapiPixmapClass             GstVaapiPixmapClass;

/**
 * GST_VAAPI_PIXMAP_FORMAT:
 * @pixmap: a #GstVaapiPixmap
 *
 * Macro that evaluates to the format in pixels of the @pixmap.
 */
#define GST_VAAPI_PIXMAP_FORMAT(pixmap) \
    gst_vaapi_pixmap_get_format(GST_VAAPI_PIXMAP(pixmap))

/**
 * GST_VAAPI_PIXMAP_WIDTH:
 * @pixmap: a #GstVaapiPixmap
 *
 * Macro that evaluates to the width in pixels of the @pixmap.
 */
#define GST_VAAPI_PIXMAP_WIDTH(pixmap) \
    gst_vaapi_pixmap_get_width(GST_VAAPI_PIXMAP(pixmap))

/**
 * GST_VAAPI_PIXMAP_HEIGHT:
 * @pixmap: a #GstVaapiPixmap
 *
 * Macro that evaluates to the height in pixels of the @pixmap.
 */
#define GST_VAAPI_PIXMAP_HEIGHT(pixmap) \
    gst_vaapi_pixmap_get_height(GST_VAAPI_PIXMAP(pixmap))

GstVaapiPixmap *
gst_vaapi_pixmap_ref(GstVaapiPixmap *pixmap);

void
gst_vaapi_pixmap_unref(GstVaapiPixmap *pixmap);

void
gst_vaapi_pixmap_replace(GstVaapiPixmap **old_pixmap_ptr,
    GstVaapiPixmap *new_pixmap);

GstVaapiDisplay *
gst_vaapi_pixmap_get_display(GstVaapiPixmap *pixmap);

GstVideoFormat
gst_vaapi_pixmap_get_format(GstVaapiPixmap *pixmap);

guint
gst_vaapi_pixmap_get_width(GstVaapiPixmap *pixmap);

guint
gst_vaapi_pixmap_get_height(GstVaapiPixmap *pixmap);

void
gst_vaapi_pixmap_get_size(GstVaapiPixmap *pixmap, guint *width, guint *height);

gboolean
gst_vaapi_pixmap_put_surface(GstVaapiPixmap *pixmap, GstVaapiSurface *surface,
    const GstVaapiRectangle *crop_rect, guint flags);

G_END_DECLS

#endif /* GST_VAAPI_PIXMAP_H */
