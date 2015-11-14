/*
 *  gstvaapisurfaceproxy.h - VA surface proxy
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

#ifndef GST_VAAPI_SURFACE_PROXY_H
#define GST_VAAPI_SURFACE_PROXY_H

#include <gst/vaapi/gstvaapisurface.h>
#include <gst/vaapi/gstvaapisurfacepool.h>

G_BEGIN_DECLS

/**
 * GstVaapiSurfaceProxyFlags:
 * @GST_VAAPI_SURFACE_PROXY_FLAG_INTERLACED: interlaced frame
 * @GST_VAAPI_SURFACE_PROXY_FLAG_TFF: top-field-first
 * @GST_VAAPI_SURFACE_PROXY_FLAG_RFF: repeat-first-field
 * @GST_VAAPI_SURFACE_PROXY_FLAG_ONEFIELD: only one field is available
 * @GST_VAAPI_SURFACE_PROXY_FLAG_FFB: first frame in bundle, e.g. the first
 *   view component of a MultiView Coded (MVC) frame
 * @GST_VAAPI_SURFACE_PROXY_FLAG_LAST: first flag that can be used by subclasses
 *
 * Flags for #GstVaapiDecoderFrame.
 */
typedef enum
{
  GST_VAAPI_SURFACE_PROXY_FLAG_INTERLACED = (1 << 0),
  GST_VAAPI_SURFACE_PROXY_FLAG_TFF = (1 << 1),
  GST_VAAPI_SURFACE_PROXY_FLAG_RFF = (1 << 2),
  GST_VAAPI_SURFACE_PROXY_FLAG_ONEFIELD = (1 << 3),
  GST_VAAPI_SURFACE_PROXY_FLAG_FFB = (1 << 4),
  GST_VAAPI_SURFACE_PROXY_FLAG_LAST = (1 << 8)
} GstVaapiSurfaceProxyFlags;

/**
 * GST_VAAPI_SURFACE_PROXY_SURFACE:
 * @proxy: a #GstVaapiSurfaceProxy
 *
 * Macro that evaluates to the #GstVaapiSurface of @proxy.
 */
#define GST_VAAPI_SURFACE_PROXY_SURFACE(proxy) \
  gst_vaapi_surface_proxy_get_surface (proxy)

/**
 * GST_VAAPI_SURFACE_PROXY_SURFACE_ID:
 * @proxy: a #GstVaapiSurfaceProxy
 *
 * Macro that evaluates to the VA surface ID of the underlying @proxy
 * surface.
 */
#define GST_VAAPI_SURFACE_PROXY_SURFACE_ID(proxy) \
  gst_vaapi_surface_proxy_get_surface_id (proxy)

/**
 * GST_VAAPI_SURFACE_PROXY_VIEW_ID:
 * @proxy: a #GstVaapiSurfaceProxy
 *
 * Macro that evaluates to the decoded view ID of the underlying @proxy
 * surface.
 */
#define GST_VAAPI_SURFACE_PROXY_VIEW_ID(proxy) \
  gst_vaapi_surface_proxy_get_view_id (proxy)

/**
 * GST_VAAPI_SURFACE_PROXY_TIMESTAMP:
 * @proxy: a #GstVaapiSurfaceProxy
 *
 * Macro that evaluates to the presentation timestamp of the
 * underlying @proxy surface.
 */
#define GST_VAAPI_SURFACE_PROXY_TIMESTAMP(proxy) \
  gst_vaapi_surface_proxy_get_timestamp (proxy)

/**
 * GST_VAAPI_SURFACE_PROXY_DURATION:
 * @proxy: a #GstVaapiSurfaceProxy
 *
 * Macro that evaluates to the presentation duration of the
 * underlying @proxy surface.
 */
#define GST_VAAPI_SURFACE_PROXY_DURATION(proxy) \
  gst_vaapi_surface_proxy_get_duration (proxy)

GstVaapiSurfaceProxy *
gst_vaapi_surface_proxy_new (GstVaapiSurface * surface);

GstVaapiSurfaceProxy *
gst_vaapi_surface_proxy_new_from_pool (GstVaapiSurfacePool * pool);

GstVaapiSurfaceProxy *
gst_vaapi_surface_proxy_copy (GstVaapiSurfaceProxy * proxy);

GstVaapiSurfaceProxy *
gst_vaapi_surface_proxy_ref (GstVaapiSurfaceProxy * proxy);

void
gst_vaapi_surface_proxy_unref (GstVaapiSurfaceProxy * proxy);

void
gst_vaapi_surface_proxy_replace (GstVaapiSurfaceProxy ** old_proxy_ptr,
    GstVaapiSurfaceProxy * new_proxy);

guint
gst_vaapi_surface_proxy_get_flags (GstVaapiSurfaceProxy * proxy);

GstVaapiSurface *
gst_vaapi_surface_proxy_get_surface (GstVaapiSurfaceProxy * proxy);

GstVaapiID
gst_vaapi_surface_proxy_get_surface_id (GstVaapiSurfaceProxy * proxy);

guintptr
gst_vaapi_surface_proxy_get_view_id (GstVaapiSurfaceProxy * proxy);

GstClockTime
gst_vaapi_surface_proxy_get_timestamp (GstVaapiSurfaceProxy * proxy);

GstClockTime
gst_vaapi_surface_proxy_get_duration (GstVaapiSurfaceProxy * proxy);

void
gst_vaapi_surface_proxy_set_destroy_notify (GstVaapiSurfaceProxy * proxy,
    GDestroyNotify destroy_func, gpointer user_data);

const GstVaapiRectangle *
gst_vaapi_surface_proxy_get_crop_rect (GstVaapiSurfaceProxy * proxy);

void
gst_vaapi_surface_proxy_set_crop_rect (GstVaapiSurfaceProxy * proxy,
    const GstVaapiRectangle * crop_rect);

G_END_DECLS

#endif /* GST_VAAPI_SURFACE_PROXY_H */
