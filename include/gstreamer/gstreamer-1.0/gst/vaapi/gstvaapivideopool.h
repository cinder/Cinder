/*
 *  gstvaapivideopool.h - Video object pool abstraction
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

#ifndef GST_VAAPI_VIDEO_POOL_H
#define GST_VAAPI_VIDEO_POOL_H

#include <glib.h>
#include <gst/vaapi/gstvaapidisplay.h>

G_BEGIN_DECLS

#define GST_VAAPI_VIDEO_POOL(obj) \
  ((GstVaapiVideoPool *)(obj))

typedef struct _GstVaapiVideoPool GstVaapiVideoPool;

/**
 * GstVaapiVideoPoolObjectType:
 * @GST_VAAPI_VIDEO_POOL_OBJECT_TYPE_IMAGE: #GstVaapiImage objects.
 * @GST_VAAPI_VIDEO_POOL_OBJECT_TYPE_SURFACE: #GstVaapiSurface objects.
 * @GST_VAAPI_VIDEO_POOL_OBJECT_TYPE_CODED_BUFFER: #GstVaapiCodedBuffer objects.
 *
 * The set of all supported #GstVaapiVideoPool object types.
 */
typedef enum
{
  GST_VAAPI_VIDEO_POOL_OBJECT_TYPE_IMAGE = 1,
  GST_VAAPI_VIDEO_POOL_OBJECT_TYPE_SURFACE,
  GST_VAAPI_VIDEO_POOL_OBJECT_TYPE_CODED_BUFFER
} GstVaapiVideoPoolObjectType;

GstVaapiVideoPool *
gst_vaapi_video_pool_ref (GstVaapiVideoPool * pool);

void
gst_vaapi_video_pool_unref (GstVaapiVideoPool * pool);

void
gst_vaapi_video_pool_replace (GstVaapiVideoPool ** old_pool_ptr,
    GstVaapiVideoPool * new_pool);

GstVaapiDisplay *
gst_vaapi_video_pool_get_display (GstVaapiVideoPool * pool);

GstVaapiVideoPoolObjectType
gst_vaapi_video_pool_get_object_type (GstVaapiVideoPool * pool);

gpointer
gst_vaapi_video_pool_get_object (GstVaapiVideoPool * pool);

void
gst_vaapi_video_pool_put_object (GstVaapiVideoPool * pool, gpointer object);

gboolean
gst_vaapi_video_pool_add_object (GstVaapiVideoPool * pool, gpointer object);

gboolean
gst_vaapi_video_pool_add_objects (GstVaapiVideoPool * pool,
    GPtrArray * objects);

guint
gst_vaapi_video_pool_get_size (GstVaapiVideoPool * pool);

gboolean
gst_vaapi_video_pool_reserve (GstVaapiVideoPool * pool, guint n);

guint
gst_vaapi_video_pool_get_capacity (GstVaapiVideoPool * pool);

void
gst_vaapi_video_pool_set_capacity (GstVaapiVideoPool * pool, guint capacity);

G_END_DECLS

#endif /* GST_VAAPI_VIDEO_POOL_H */
