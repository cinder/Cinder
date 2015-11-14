/* GStreamer
 * Copyright (C) <2015> YouView TV Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GST_PROTECTION_H__
#define __GST_PROTECTION_H__

#include <gst/gst.h>

G_BEGIN_DECLS

/**
 * GST_PROTECTION_SYSTEM_ID_CAPS_FIELD:
 *
 * The field name in a GstCaps that is used to signal the UUID of the protection
 * system.
 *
 * Since: 1.6
 */
#define GST_PROTECTION_SYSTEM_ID_CAPS_FIELD "protection-system"

typedef struct _GstProtectionMeta GstProtectionMeta;
/**
 * GstProtectionMeta:
 * @meta: the parent #GstMeta.
 * @info: the cryptographic information needed to decrypt the sample.
 *
 * Metadata type that holds information about a sample from a protection-protected
 * track, including the information needed to decrypt it (if it is encrypted).
 *
 * Since: 1.6
 */
struct _GstProtectionMeta
{
  GstMeta meta;

  GstStructure *info;
};

GType gst_protection_meta_api_get_type (void);
#define GST_PROTECTION_META_API_TYPE (gst_protection_meta_api_get_type())

#define gst_buffer_get_protection_meta(b) \
    ((GstProtectionMeta*)gst_buffer_get_meta ((b), GST_PROTECTION_META_API_TYPE))

#define GST_PROTECTION_META_INFO (gst_protection_meta_get_info())

const GstMetaInfo *gst_protection_meta_get_info (void);

GstProtectionMeta *gst_buffer_add_protection_meta (GstBuffer * buffer,
    GstStructure * info);

const gchar *gst_protection_select_system (const gchar ** system_identifiers);

G_END_DECLS
#endif /* __GST_PROTECTION_META_H__ */
