/*
 *  gstvaapiobject.h - Base VA object
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

#ifndef GST_VAAPI_OBJECT_H
#define GST_VAAPI_OBJECT_H

#include <gst/vaapi/gstvaapitypes.h>
#include <gst/vaapi/gstvaapidisplay.h>

G_BEGIN_DECLS

#define GST_VAAPI_OBJECT(obj) \
  ((GstVaapiObject *) (obj))

typedef struct _GstVaapiObject GstVaapiObject;

/**
 * GST_VAAPI_OBJECT_DISPLAY:
 * @object: a #GstVaapiObject
 *
 * Macro that evaluates to the #GstVaapiDisplay the @object is bound to.
 */
#define GST_VAAPI_OBJECT_DISPLAY(object) \
  gst_vaapi_object_get_display (GST_VAAPI_OBJECT (object))

/**
 * GST_VAAPI_OBJECT_ID:
 * @object: a #GstVaapiObject
 *
 * Macro that evaluates to the #GstVaapiID contained in @object.
 */
#define GST_VAAPI_OBJECT_ID(object) \
  gst_vaapi_object_get_id (GST_VAAPI_OBJECT (object))

gpointer
gst_vaapi_object_ref (gpointer object);

void
gst_vaapi_object_unref (gpointer object);

void
gst_vaapi_object_replace (gpointer old_object_ptr, gpointer new_object);

GstVaapiDisplay *
gst_vaapi_object_get_display (GstVaapiObject * object);

void
gst_vaapi_object_lock_display (GstVaapiObject * object);

void
gst_vaapi_object_unlock_display (GstVaapiObject * object);

GstVaapiID
gst_vaapi_object_get_id (GstVaapiObject * object);

G_END_DECLS

#endif /* GST_VAAPI_OBJECT_H */
