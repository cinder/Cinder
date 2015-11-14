/*
 *  gstvaapidisplay_drm.h - VA/DRM display abstraction
 *
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

#ifndef GST_VAAPI_DISPLAY_DRM_H
#define GST_VAAPI_DISPLAY_DRM_H

#include <gst/vaapi/gstvaapidisplay.h>

G_BEGIN_DECLS

#define GST_VAAPI_DISPLAY_DRM(obj) \
    ((GstVaapiDisplayDRM *)(obj))

typedef struct _GstVaapiDisplayDRM              GstVaapiDisplayDRM;

GstVaapiDisplay *
gst_vaapi_display_drm_new (const gchar * device_path);

GstVaapiDisplay *
gst_vaapi_display_drm_new_with_device (gint device);

gint
gst_vaapi_display_drm_get_device (GstVaapiDisplayDRM * display);

const gchar *
gst_vaapi_display_drm_get_device_path (GstVaapiDisplayDRM *
    display);

G_END_DECLS

#endif /* GST_VAAPI_DISPLAY_DRM_H */
