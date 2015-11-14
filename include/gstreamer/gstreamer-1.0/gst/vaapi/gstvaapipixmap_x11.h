/*
 *  gstvaapipixmap_x11.h - X11 pixmap abstraction
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

#ifndef GST_VAAPI_PIXMAP_X11_H
#define GST_VAAPI_PIXMAP_X11_H

#include <X11/Xlib.h>
#include <gst/vaapi/gstvaapidisplay.h>
#include <gst/vaapi/gstvaapipixmap.h>

G_BEGIN_DECLS

#define GST_VAAPI_PIXMAP_X11(obj) \
    ((GstVaapiPixmapX11 *)(obj))

/**
 * GST_VAAPI_PIXMAP_XPIXMAP:
 * @pixmap: a #GstVaapiPixmap
 *
 * Macro that evaluates to the underlying X11 #Pixmap of @pixmap
 */
#define GST_VAAPI_PIXMAP_XPIXMAP(pixmap) \
    gst_vaapi_pixmap_x11_get_xid(GST_VAAPI_PIXMAP_X11(pixmap))

typedef struct _GstVaapiPixmapX11               GstVaapiPixmapX11;

GstVaapiPixmap *
gst_vaapi_pixmap_x11_new(GstVaapiDisplay *display, GstVideoFormat format,
    guint width, guint height);

GstVaapiPixmap *
gst_vaapi_pixmap_x11_new_with_xid(GstVaapiDisplay *display, Pixmap xid);

Pixmap
gst_vaapi_pixmap_x11_get_xid(GstVaapiPixmapX11 *pixmap);

gboolean
gst_vaapi_pixmap_x11_is_foreign_xid(GstVaapiPixmapX11 *pixmap);

G_END_DECLS

#endif /* GST_VAAPI_PIXMAP_X11_H */
