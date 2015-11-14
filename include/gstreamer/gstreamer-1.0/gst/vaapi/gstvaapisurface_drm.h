/*
 *  gstvaapisurface_drm.h - VA surface abstraction (DRM interop)
 *
 *  Copyright (C) 2014 Intel Corporation
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

#ifndef GST_VAAPI_SURFACE_DRM_H
#define GST_VAAPI_SURFACE_DRM_H

#include <gst/vaapi/gstvaapisurface.h>
#include <gst/vaapi/gstvaapibufferproxy.h>

G_BEGIN_DECLS

GstVaapiBufferProxy *
gst_vaapi_surface_get_dma_buf_handle (GstVaapiSurface * surface);

GstVaapiBufferProxy *
gst_vaapi_surface_get_gem_buf_handle (GstVaapiSurface * surface);

GstVaapiSurface *
gst_vaapi_surface_new_with_dma_buf_handle (GstVaapiDisplay * display,
    gint fd, guint size, GstVideoFormat format, guint width, guint height,
    gsize offset[GST_VIDEO_MAX_PLANES], gint stride[GST_VIDEO_MAX_PLANES]);

GstVaapiSurface *
gst_vaapi_surface_new_with_gem_buf_handle (GstVaapiDisplay * display,
    guint32 name, guint size, GstVideoFormat format, guint width, guint height,
    gsize offset[GST_VIDEO_MAX_PLANES], gint stride[GST_VIDEO_MAX_PLANES]);

G_END_DECLS

#endif /* GST_VAAPI_SURFACE_DRM_H */
