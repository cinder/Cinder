/*
 *  gstvaapicodedbufferproxy_priv.h - VA coded buffer proxy
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

#ifndef GST_VAAPI_CODED_BUFFER_PROXY_H
#define GST_VAAPI_CODED_BUFFER_PROXY_H

#include <gst/vaapi/gstvaapicodedbuffer.h>
#include <gst/vaapi/gstvaapicodedbufferpool.h>

G_BEGIN_DECLS

/**
 * GST_VAAPI_CODED_BUFFER_PROXY_BUFFER:
 * @proxy: a #GstVaapiCodedBufferProxy
 *
 * Macro that evaluated to the underlying #GstVaapiCodedBuffer of @proxy.
 */
#define GST_VAAPI_CODED_BUFFER_PROXY_BUFFER(proxy) \
  gst_vaapi_coded_buffer_proxy_get_buffer(proxy)

/**
 * GST_VAAPI_CODED_BUFFER_PROXY_BUFFER_SIZE:
 * @proxy: a #GstVaapiCodedBufferProxy
 *
 * Macro that evaluated to the underlying #GstVaapiCodedBuffer size of
 * @proxy.
 */
#define GST_VAAPI_CODED_BUFFER_PROXY_BUFFER_SIZE(proxy) \
  gst_vaapi_coded_buffer_proxy_get_buffer_size(proxy)

GstVaapiCodedBufferProxy *
gst_vaapi_coded_buffer_proxy_new_from_pool (GstVaapiCodedBufferPool * pool);

GstVaapiCodedBufferProxy *
gst_vaapi_coded_buffer_proxy_ref (GstVaapiCodedBufferProxy * proxy);

void
gst_vaapi_coded_buffer_proxy_unref (GstVaapiCodedBufferProxy * proxy);

void
gst_vaapi_coded_buffer_proxy_replace (GstVaapiCodedBufferProxy ** old_proxy_ptr,
    GstVaapiCodedBufferProxy * new_proxy);

GstVaapiCodedBuffer *
gst_vaapi_coded_buffer_proxy_get_buffer (GstVaapiCodedBufferProxy * proxy);

gssize
gst_vaapi_coded_buffer_proxy_get_buffer_size (GstVaapiCodedBufferProxy * proxy);

void
gst_vaapi_coded_buffer_proxy_set_destroy_notify (GstVaapiCodedBufferProxy *
    proxy, GDestroyNotify destroy_func, gpointer user_data);

gpointer
gst_vaapi_coded_buffer_proxy_get_user_data (GstVaapiCodedBufferProxy * proxy);

void
gst_vaapi_coded_buffer_proxy_set_user_data (GstVaapiCodedBufferProxy * proxy,
    gpointer user_data, GDestroyNotify destroy_func);

G_END_DECLS

#endif /* GST_VAAPI_CODED_BUFFER_PROXY_H */
