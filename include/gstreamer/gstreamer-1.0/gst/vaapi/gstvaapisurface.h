/*
 *  gstvaapisurface.h - VA surface abstraction
 *
 *  Copyright (C) 2010-2011 Splitted-Desktop Systems
 *    Author: Gwenole Beauchesne <gwenole.beauchesne@splitted-desktop.com>
 *  Copyright (C) 2011-2014 Intel Corporation
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

#ifndef GST_VAAPI_SURFACE_H
#define GST_VAAPI_SURFACE_H

#include <gst/vaapi/gstvaapiobject.h>
#include <gst/vaapi/gstvaapidisplay.h>
#include <gst/vaapi/gstvaapiimage.h>
#include <gst/vaapi/gstvaapisubpicture.h>
#include <gst/vaapi/gstvaapibufferproxy.h>
#include <gst/video/video.h>
#include <gst/video/video-overlay-composition.h>

G_BEGIN_DECLS

/**
 * GST_VAAPI_SURFACE_CAPS_NAME:
 *
 * Generic caps type for VA surfaces.
 */
#define GST_VAAPI_SURFACE_CAPS_NAME "video/x-surface"

/**
 * GST_VAAPI_SURFACE_CAPS:
 *
 * Generic caps for VA surfaces.
 */
#define GST_VAAPI_SURFACE_CAPS                  \
    GST_VAAPI_SURFACE_CAPS_NAME ", "            \
    "type = vaapi, "                            \
    "opengl = (boolean) { true, false }, "      \
    "width  = (int) [ 1, MAX ], "               \
    "height = (int) [ 1, MAX ], "               \
    "framerate = (fraction) [ 0, MAX ]"

/**
 * GstVaapiChromaType:
 * @GST_VAAPI_CHROMA_TYPE_YUV420: YUV 4:2:0 chroma format
 * @GST_VAAPI_CHROMA_TYPE_YUV422: YUV 4:2:2 chroma format
 * @GST_VAAPI_CHROMA_TYPE_YUV444: YUV 4:4:4 chroma format
 * @GST_VAAPI_CHROMA_TYPE_YUV411: YUV 4:1:1 chroma format
 * @GST_VAAPI_CHROMA_TYPE_YUV410: YUV 4:1:0 chroma format
 * @GST_VAAPI_CHROMA_TYPE_YUV400: YUV 4:0:0 chroma format (grayscale)
 * @GST_VAAPI_CHROMA_TYPE_RGB32: 32-bit RGB chroma format
 * @GST_VAAPI_CHROMA_TYPE_RGB16: 16-bit RGB chroma format
 *
 * The set of all chroma types for #GstVaapiSurface.
 */
typedef enum
{
  GST_VAAPI_CHROMA_TYPE_YUV420 = 1,
  GST_VAAPI_CHROMA_TYPE_YUV422,
  GST_VAAPI_CHROMA_TYPE_YUV444,
  GST_VAAPI_CHROMA_TYPE_YUV411,
  GST_VAAPI_CHROMA_TYPE_YUV410,
  GST_VAAPI_CHROMA_TYPE_YUV400,
  GST_VAAPI_CHROMA_TYPE_RGB32,
  GST_VAAPI_CHROMA_TYPE_RGB16
} GstVaapiChromaType;

/**
 * GstVaapiSurfaceStatus:
 * @GST_VAAPI_SURFACE_STATUS_IDLE:
 *   the surface is not being rendered or displayed
 * @GST_VAAPI_SURFACE_STATUS_RENDERING:
 *   the surface is used for rendering (decoding to the surface in progress)
 * @GST_VAAPI_SURFACE_STATUS_DISPLAYING:
 *   the surface is being displayed to screen
 * @GST_VAAPI_SURFACE_STATUS_SKIPPED:
 *   indicates a skipped frame during encode
 *
 * The set of all surface status for #GstVaapiSurface.
 */
typedef enum
{
  GST_VAAPI_SURFACE_STATUS_IDLE = 1 << 0,
  GST_VAAPI_SURFACE_STATUS_RENDERING = 1 << 1,
  GST_VAAPI_SURFACE_STATUS_DISPLAYING = 1 << 2,
  GST_VAAPI_SURFACE_STATUS_SKIPPED = 1 << 3
} GstVaapiSurfaceStatus;

/**
 * GstVaapiSurfaceRenderFlags
 * @GST_VAAPI_PICTURE_STRUCTURE_TOP_FIELD:
 *   selects the top field of the surface
 * @GST_VAAPI_PICTURE_STRUCTURE_BOTTOM_FIELD:
 *   selects the bottom field of the surface
 * @GST_VAAPI_PICTURE_STRUCTURE_FRAME:
 *   selects the entire surface
 * @GST_VAAPI_COLOR_STANDARD_ITUR_BT_601:
 *   uses ITU-R BT.601 standard for color space conversion
 * @GST_VAAPI_COLOR_STANDARD_ITUR_BT_709:
 *   uses ITU-R BT.709 standard for color space conversion
 * @GST_VAAPI_COLOR_STANDARD_ITUR_BT_470M:
 *   uses ITU-R BT.470-2 System M standard for color space conversion
 * @GST_VAAPI_COLOR_STANDARD_ITUR_BT_470BG:
 *   uses ITU-R BT.470-2 System B, G standard for color space conversion
 * @GST_VAAPI_COLOR_STANDARD_SMPTE_170M:
 *   uses SMPTE-170M standard for color space conversion
 * @GST_VAAPI_COLOR_STANDARD_SMPTE_240M:
 *   uses SMPTE-240M standard for color space conversion
 *
 * The set of all render flags for gst_vaapi_window_put_surface().
 */
typedef enum
{
  /* Picture structure */
  GST_VAAPI_PICTURE_STRUCTURE_TOP_FIELD         = 0x01 << 0,
  GST_VAAPI_PICTURE_STRUCTURE_BOTTOM_FIELD      = 0x02 << 0,
  GST_VAAPI_PICTURE_STRUCTURE_FRAME             = 0x03 << 0,
  GST_VAAPI_PICTURE_STRUCTURE_MASK              = 0x00000003, /* 2 bits */

  /* Color standard */
  GST_VAAPI_COLOR_STANDARD_ITUR_BT_601          = 0x01 << 2,
  GST_VAAPI_COLOR_STANDARD_ITUR_BT_709          = 0x02 << 2,
  GST_VAAPI_COLOR_STANDARD_ITUR_BT_470M         = 0x03 << 2,
  GST_VAAPI_COLOR_STANDARD_ITUR_BT_470BG        = 0x04 << 2,
  GST_VAAPI_COLOR_STANDARD_SMPTE_170M           = 0x05 << 2,
  GST_VAAPI_COLOR_STANDARD_SMPTE_240M           = 0x06 << 2,
  GST_VAAPI_COLOR_STANDARD_MASK                 = 0x0000003c, /* 4 bits */
} GstVaapiSurfaceRenderFlags;

/**
 * GstVaapiSurfaceAllocFlags:
 * @GST_VAAPI_SURFACE_ALLOC_FLAG_LINEAR_STORAGE: forces allocation
 *   with linear storage. Default behaviour matches native
 *   requirements, and thus could be tiled.
 * @GST_VAAPI_SURFACE_ALLOC_FLAG_FIXED_STRIDES: force allocation with
 *   the supplied strides information from #GstVideoInfo
 * @GST_VAAPI_SURFACE_ALLOC_FLAG_FIXED_OFFSETS: force allocation with
 *   the supplied offsets information from #GstVideoInfo
 *
 * The set of optional allocation flags for gst_vaapi_surface_new_full().
 */
typedef enum
{
  GST_VAAPI_SURFACE_ALLOC_FLAG_LINEAR_STORAGE   = 1 << 0,
  GST_VAAPI_SURFACE_ALLOC_FLAG_FIXED_STRIDES    = 1 << 1,
  GST_VAAPI_SURFACE_ALLOC_FLAG_FIXED_OFFSETS    = 1 << 2,
} GstVaapiSurfaceAllocFlags;

#define GST_VAAPI_SURFACE(obj) \
    ((GstVaapiSurface *)(obj))

typedef struct _GstVaapiSurface                 GstVaapiSurface;
typedef struct _GstVaapiSurfaceProxy            GstVaapiSurfaceProxy;

GstVaapiSurface *
gst_vaapi_surface_new (GstVaapiDisplay * display,
    GstVaapiChromaType chroma_type, guint width, guint height);

GstVaapiSurface *
gst_vaapi_surface_new_full (GstVaapiDisplay * display,
    const GstVideoInfo * vip, guint flags);

GstVaapiSurface *
gst_vaapi_surface_new_with_format (GstVaapiDisplay * display,
    GstVideoFormat format, guint width, guint height);

GstVaapiSurface *
gst_vaapi_surface_new_from_buffer_proxy (GstVaapiDisplay * display,
    GstVaapiBufferProxy * proxy, const GstVideoInfo * vip);

GstVaapiID
gst_vaapi_surface_get_id (GstVaapiSurface * surface);

GstVaapiChromaType
gst_vaapi_surface_get_chroma_type (GstVaapiSurface * surface);

GstVideoFormat
gst_vaapi_surface_get_format (GstVaapiSurface * surface);

guint
gst_vaapi_surface_get_width (GstVaapiSurface * surface);

guint
gst_vaapi_surface_get_height (GstVaapiSurface * surface);

void
gst_vaapi_surface_get_size (GstVaapiSurface * surface, guint * width_ptr,
    guint * height_ptr);

GstVaapiImage *
gst_vaapi_surface_derive_image (GstVaapiSurface * surface);

gboolean
gst_vaapi_surface_get_image (GstVaapiSurface * surface, GstVaapiImage * image);

gboolean
gst_vaapi_surface_put_image (GstVaapiSurface * surface, GstVaapiImage * image);

gboolean
gst_vaapi_surface_associate_subpicture (GstVaapiSurface * surface,
    GstVaapiSubpicture * subpicture, const GstVaapiRectangle * src_rect,
    const GstVaapiRectangle * dst_rect);

gboolean
gst_vaapi_surface_deassociate_subpicture (GstVaapiSurface * surface,
    GstVaapiSubpicture * subpicture);

gboolean
gst_vaapi_surface_sync (GstVaapiSurface * surface);

gboolean
gst_vaapi_surface_query_status (GstVaapiSurface * surface,
    GstVaapiSurfaceStatus * pstatus);

gboolean
gst_vaapi_surface_set_subpictures_from_composition (GstVaapiSurface * surface,
    GstVideoOverlayComposition * composition, gboolean propagate_context);

G_END_DECLS

#endif /* GST_VAAPI_SURFACE_H */
