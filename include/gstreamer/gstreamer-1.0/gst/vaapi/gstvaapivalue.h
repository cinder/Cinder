/*
 *  gstvaapivalue.h - GValue implementations specific to VA-API
 *
 *  Copyright (C) 2010-2011 Splitted-Desktop Systems
 *    Author: Gwenole Beauchesne <gwenole.beauchesne@splitted-desktop.com>
 *  Copyright (C) 2012-2014 Intel Corporation
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

#ifndef GST_VAAPI_VALUE_H
#define GST_VAAPI_VALUE_H

#include <glib-object.h>
#include <gst/vaapi/gstvaapitypes.h>

G_BEGIN_DECLS

/**
 * GST_VAAPI_TYPE_POINT:
 *
 * A #GstVaapiPoint type that represents a 2D point coordinates.
 *
 * Return value: the GType of #GstVaapiPoint
 */
#define GST_VAAPI_TYPE_POINT gst_vaapi_point_get_type()

/**
 * GST_VAAPI_TYPE_RECTANGLE:
 *
 * A #GstVaapiRectangle type that represents a 2D rectangle position
 * and size.
 *
 * Return value: the GType of #GstVaapiRectangle
 */
#define GST_VAAPI_TYPE_RECTANGLE gst_vaapi_rectangle_get_type()

/**
 * GST_VAAPI_TYPE_RENDER_MODE:
 *
 * A #GstVaapiRenderMode type that represents the VA display backend
 * rendering mode: overlay (2D engine) or textured-blit (3D engine).
 *
 * Return value: the #GType of GstVaapiRenderMode
 */
#define GST_VAAPI_TYPE_RENDER_MODE gst_vaapi_render_mode_get_type()

/**
 * GST_VAAPI_TYPE_ROTATION:
 *
 * A type that represents the VA display rotation.
 *
 * Return value: the #GType of GstVaapiRotation
 */
#define GST_VAAPI_TYPE_ROTATION gst_vaapi_rotation_get_type()

/**
 * GST_VAAPI_TYPE_RATE_CONTROL:
 *
 * A type that represents the VA rate control.
 *
 * Return value: the #GType of GstVaapiRateControl
 */
#define GST_VAAPI_TYPE_RATE_CONTROL gst_vaapi_rate_control_get_type()

GType
gst_vaapi_point_get_type(void) G_GNUC_CONST;

GType
gst_vaapi_rectangle_get_type(void) G_GNUC_CONST;

GType
gst_vaapi_render_mode_get_type(void) G_GNUC_CONST;

GType
gst_vaapi_rotation_get_type(void) G_GNUC_CONST;

GType
gst_vaapi_rate_control_get_type(void) G_GNUC_CONST;

/**
 * GST_VAAPI_POPCOUNT32:
 * @x: the value from which to compute population count
 *
 * Computes the number of bits set in the supplied 32-bit value @x.
 *
 * Return value: the number of bits set in @x
 */
#define GST_VAAPI_POPCOUNT32(x) \
    GST_VAAPI_POPCOUNT32_0(x)
#define GST_VAAPI_POPCOUNT32_0(x) \
    GST_VAAPI_POPCOUNT32_1((x) - (((x) >> 1) & 0x55555555))
#define GST_VAAPI_POPCOUNT32_1(x) \
    GST_VAAPI_POPCOUNT32_2(((x) & 0x33333333) + (((x) >> 2) & 0x33333333))
#define GST_VAAPI_POPCOUNT32_2(x) \
    GST_VAAPI_POPCOUNT32_3((x) + ((x) >> 4))
#define GST_VAAPI_POPCOUNT32_3(x) \
    GST_VAAPI_POPCOUNT32_4((x) & 0x0f0f0f0f)
#define GST_VAAPI_POPCOUNT32_4(x) \
    (((x) * 0x01010101) >> 24)

/* --- GstVaapiEnumSubset --- */

/**
 * GstVaapiEnumSubset:
 * @name: name of the enum subset
 * @parent_type: parent enum type
 * @type: registered #GType
 * @type_info: #GTypeInfo used to build the @type
 * @values: pointer to a static array of #GEnumValue elements
 * @num_values: number of elements in the @values array, including the
 *   terminator
 *
 * Structure that holds the required information to build a GEnum
 * subset from the supplied @parent_type, i.e. a subset of its values.
 */
typedef struct {
    GType parent_type;
    GType type;
    GTypeInfo type_info;
    const gchar *type_name;
    GEnumValue *values;
    guint num_values;
} GstVaapiEnumSubset;

G_GNUC_INTERNAL
GType
gst_vaapi_type_define_enum_subset_from_mask(GstVaapiEnumSubset *subset,
    guint32 mask);

#define GST_VAAPI_TYPE_DEFINE_ENUM_SUBSET_FROM_MASK(NAME, name, TYPE, MASK) \
static GType                                                            \
G_PASTE(name,_get_type)(void)                                           \
{                                                                       \
    static GEnumValue enum_values[GST_VAAPI_POPCOUNT32(MASK) + 1];      \
    static GstVaapiEnumSubset subset = {                                \
        .type_name = G_STRINGIFY(NAME),                                 \
        .values = enum_values,                                          \
        .num_values = G_N_ELEMENTS(enum_values),                        \
    };                                                                  \
    if (g_once_init_enter(&subset.parent_type))                         \
        g_once_init_leave(&subset.parent_type, TYPE);                   \
    return gst_vaapi_type_define_enum_subset_from_mask(&subset, MASK);  \
}

G_END_DECLS

#endif /* GST_VAAPI_VALUE_H */
