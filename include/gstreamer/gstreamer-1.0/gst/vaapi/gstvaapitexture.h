/*
 *  gstvaapitexture.h - VA texture abstraction
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

#ifndef GST_VAAPI_TEXTURE_H
#define GST_VAAPI_TEXTURE_H

#include <gst/vaapi/gstvaapitypes.h>
#include <gst/vaapi/gstvaapisurface.h>

G_BEGIN_DECLS

#define GST_VAAPI_TEXTURE(obj) \
  ((GstVaapiTexture *)(obj))

/**
 * GST_VAAPI_TEXTURE_ID:
 * @texture: a #GstVaapiTexture
 *
 * Macro that evaluates to the GL texture id associated with the @texture
 */
#define GST_VAAPI_TEXTURE_ID(texture) \
  gst_vaapi_texture_get_id (GST_VAAPI_TEXTURE (texture))

/**
 * GST_VAAPI_TEXTURE_TARGET:
 * @texture: a #GstVaapiTexture
 *
 * Macro that evaluates to the GL texture target associated with the @texture
 */
#define GST_VAAPI_TEXTURE_TARGET(texture) \
  gst_vaapi_texture_get_target (GST_VAAPI_TEXTURE (texture))

/**
 * GST_VAAPI_TEXTURE_FORMAT:
 * @texture: a #GstVaapiTexture
 *
 * Macro that evaluates to the GL texture format associated with the @texture
 */
#define GST_VAAPI_TEXTURE_FORMAT(texture) \
  gst_vaapi_texture_get_format (GST_VAAPI_TEXTURE (texture))

/**
 * GST_VAAPI_TEXTURE_WIDTH:
 * @texture: a #GstVaapiTexture
 *
 * Macro that evaluates to the GL texture width associated with the @texture
 */
#define GST_VAAPI_TEXTURE_WIDTH(texture) \
  gst_vaapi_texture_get_width (GST_VAAPI_TEXTURE (texture))

/**
 * GST_VAAPI_TEXTURE_HEIGHT:
 * @texture: a #GstVaapiTexture
 *
 * Macro that evaluates to the GL texture height associated with the @texture
 */
#define GST_VAAPI_TEXTURE_HEIGHT(texture) \
  gst_vaapi_texture_get_height (GST_VAAPI_TEXTURE (texture))

typedef struct _GstVaapiTexture GstVaapiTexture;

/**
 * GstVaapiTextureOrientationFlags:
 * @GST_VAAPI_TEXTURE_ORIENTATION_FLAG_X_INVERTED: indicates whether
 *   the right row comes first in memory.
 * @GST_VAAPI_TEXTURE_ORIENTATION_FLAG_Y_INVERTED: indicates whether
 *   the bottom line comes first in memory.
 *
 * Additional flags to indicate whether the texture data is organized
 * in memory with the X or Y, or both, axis inverted. e.g. if only
 * @GST_VAAPI_TEXTURE_ORIENTATION_FLAG_Y_INVERTED is set, this means
 * that the bottom line comes first in memory, with pixels laid out
 * from the left to the right.
 */
typedef enum {
  GST_VAAPI_TEXTURE_ORIENTATION_FLAG_X_INVERTED = 1 << 31,
  GST_VAAPI_TEXTURE_ORIENTATION_FLAG_Y_INVERTED = 1 << 30,
} GstVaapiTextureOrientationFlags;

GstVaapiTexture *
gst_vaapi_texture_new (GstVaapiDisplay * display, guint target, guint format,
    guint width, guint height);

GstVaapiTexture *
gst_vaapi_texture_new_wrapped (GstVaapiDisplay * display, guint id,
    guint target, guint format, guint width, guint height);

GstVaapiTexture *
gst_vaapi_texture_ref (GstVaapiTexture * texture);

void
gst_vaapi_texture_unref (GstVaapiTexture * texture);

void
gst_vaapi_texture_replace (GstVaapiTexture ** old_texture_ptr,
    GstVaapiTexture * new_texture);

guint
gst_vaapi_texture_get_id (GstVaapiTexture * texture);

guint
gst_vaapi_texture_get_target (GstVaapiTexture * texture);

guint
gst_vaapi_texture_get_format (GstVaapiTexture * texture);

guint
gst_vaapi_texture_get_width (GstVaapiTexture * texture);

guint
gst_vaapi_texture_get_height (GstVaapiTexture * texture);

void
gst_vaapi_texture_get_size (GstVaapiTexture * texture, guint * width_ptr,
    guint * height_ptr);

guint
gst_vaapi_texture_get_orientation_flags (GstVaapiTexture * texture);

void
gst_vaapi_texture_set_orientation_flags (GstVaapiTexture * texture,
    guint flags);

gboolean
gst_vaapi_texture_put_surface (GstVaapiTexture * texture,
    GstVaapiSurface * surface, const GstVaapiRectangle * crop_rect,
    guint flags);

G_END_DECLS

#endif /* GST_VAAPI_TEXTURE_H */
