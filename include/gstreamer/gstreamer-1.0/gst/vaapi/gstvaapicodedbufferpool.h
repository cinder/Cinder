/*
 *  gstvaapicodedbufferpool.h - VA coded buffer pool
 *
 *  Copyright (C) 2013-2014 Intel Corporation
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

#ifndef GST_VAAPI_CODED_BUFFER_POOL_H
#define GST_VAAPI_CODED_BUFFER_POOL_H

#include <gst/vaapi/gstvaapivideopool.h>
#include <gst/vaapi/gstvaapiencoder.h>

G_BEGIN_DECLS

#define GST_VAAPI_CODED_BUFFER_POOL(obj) \
    ((GstVaapiCodedBufferPool *)(obj))

struct _GstVaapiEncoder;

GstVaapiVideoPool *
gst_vaapi_coded_buffer_pool_new (struct _GstVaapiEncoder * encoder,
    gsize buf_size);

gsize
gst_vaapi_coded_buffer_pool_get_buffer_size (GstVaapiCodedBufferPool * pool);

G_END_DECLS

#endif /* GST_VAAPI_CODED_BUFFER_POOL_H */
