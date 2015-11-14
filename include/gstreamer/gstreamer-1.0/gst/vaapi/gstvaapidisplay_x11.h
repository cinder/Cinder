/*
 *  gstvaapidisplay_x11.h - VA/X11 display abstraction
 *
 *  Copyright (C) 2010-2011 Splitted-Desktop Systems
 *    Author: Gwenole Beauchesne <gwenole.beauchesne@splitted-desktop.com>
 *  Copyright (C) 2012-2013 Intel Corporation
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

#ifndef GST_VAAPI_DISPLAY_X11_H
#define GST_VAAPI_DISPLAY_X11_H

#include <va/va_x11.h>
#include <gst/vaapi/gstvaapidisplay.h>

G_BEGIN_DECLS

#define GST_VAAPI_DISPLAY_X11(obj) \
    ((GstVaapiDisplayX11 *)(obj))

typedef struct _GstVaapiDisplayX11              GstVaapiDisplayX11;

GstVaapiDisplay *
gst_vaapi_display_x11_new (const gchar * display_name);

GstVaapiDisplay *
gst_vaapi_display_x11_new_with_display (Display * x11_display);

Display *
gst_vaapi_display_x11_get_display (GstVaapiDisplayX11 * display);

int
gst_vaapi_display_x11_get_screen (GstVaapiDisplayX11 * display);

void
gst_vaapi_display_x11_set_synchronous (GstVaapiDisplayX11 * display,
    gboolean synchronous);

G_END_DECLS

#endif /* GST_VAAPI_DISPLAY_X11_H */
