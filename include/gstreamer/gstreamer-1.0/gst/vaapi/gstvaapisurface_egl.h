/*
 *  gstvaapisurface_egl.h - VA surface abstraction (EGL interop)
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

#ifndef GST_VAAPI_SURFACE_EGL_H
#define GST_VAAPI_SURFACE_EGL_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <gst/vaapi/gstvaapisurface.h>

G_BEGIN_DECLS

GstVaapiSurface *
gst_vaapi_surface_new_from_egl_image (GstVaapiDisplay * display,
    const GstVideoInfo * vip, EGLImageKHR image, GstVideoFormat format,
    guint width, guint height, guint flags);

GstVaapiSurface *
gst_vaapi_surface_new_with_egl_image (GstVaapiDisplay * display,
    EGLImageKHR image, GstVideoFormat format, guint width, guint height);

G_END_DECLS

#endif /* GST_VAAPI_SURFACE_EGL_H */
