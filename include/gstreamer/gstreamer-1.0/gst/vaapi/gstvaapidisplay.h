/*
 *  gstvaapidisplay.h - VA display abstraction
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

#ifndef GST_VAAPI_DISPLAY_H
#define GST_VAAPI_DISPLAY_H

#include <va/va.h>
#include <gst/gst.h>
#include <gst/vaapi/gstvaapitypes.h>
#include <gst/vaapi/gstvaapiprofile.h>
#include <gst/vaapi/video-format.h>

G_BEGIN_DECLS

#define GST_VAAPI_DISPLAY(obj) \
    ((GstVaapiDisplay *)(obj))

/**
 * GST_VAAPI_DISPLAY_GET_CLASS_TYPE:
 * @display: a #GstVaapiDisplay
 *
 * Returns the #display class type
 */
#define GST_VAAPI_DISPLAY_GET_CLASS_TYPE(display) \
    gst_vaapi_display_get_class_type (GST_VAAPI_DISPLAY (display))

/**
 * GST_VAAPI_DISPLAY_VADISPLAY_TYPE:
 * @display: a #GstVaapiDisplay
 *
 * Returns the underlying VADisplay @display type.
 */
#define GST_VAAPI_DISPLAY_VADISPLAY_TYPE(display) \
  gst_vaapi_display_get_display_type (GST_VAAPI_DISPLAY (display))

/**
 * GST_VAAPI_DISPLAY_VADISPLAY:
 * @display_: a #GstVaapiDisplay
 *
 * Macro that evaluates to the #VADisplay of @display.
 */
#define GST_VAAPI_DISPLAY_VADISPLAY(display) \
  gst_vaapi_display_get_display (GST_VAAPI_DISPLAY (display))

/**
 * GST_VAAPI_DISPLAY_LOCK:
 * @display: a #GstVaapiDisplay
 *
 * Locks @display
 */
#define GST_VAAPI_DISPLAY_LOCK(display) \
  gst_vaapi_display_lock (GST_VAAPI_DISPLAY (display))

/**
 * GST_VAAPI_DISPLAY_UNLOCK:
 * @display: a #GstVaapiDisplay
 *
 * Unlocks @display
 */
#define GST_VAAPI_DISPLAY_UNLOCK(display) \
  gst_vaapi_display_unlock (GST_VAAPI_DISPLAY (display))

typedef struct _GstVaapiDisplayInfo             GstVaapiDisplayInfo;
typedef struct _GstVaapiDisplay                 GstVaapiDisplay;

/**
 * GstVaapiDisplayType:
 * @GST_VAAPI_DISPLAY_TYPE_ANY: Automatic detection of the display type.
 * @GST_VAAPI_DISPLAY_TYPE_X11: VA/X11 display.
 * @GST_VAAPI_DISPLAY_TYPE_GLX: VA/GLX display.
 * @GST_VAAPI_DISPLAY_TYPE_WAYLAND: VA/Wayland display.
 * @GST_VAAPI_DISPLAY_TYPE_DRM: VA/DRM display.
 * @GST_VAAPI_DISPLAY_TYPE_EGL: VA/EGL display.
 */
typedef enum
{
  GST_VAAPI_DISPLAY_TYPE_ANY = 0,
  GST_VAAPI_DISPLAY_TYPE_X11,
  GST_VAAPI_DISPLAY_TYPE_GLX,
  GST_VAAPI_DISPLAY_TYPE_WAYLAND,
  GST_VAAPI_DISPLAY_TYPE_DRM,
  GST_VAAPI_DISPLAY_TYPE_EGL,
} GstVaapiDisplayType;

#define GST_VAAPI_TYPE_DISPLAY_TYPE \
    (gst_vaapi_display_type_get_type())

GType
gst_vaapi_display_type_get_type (void) G_GNUC_CONST;

gboolean
gst_vaapi_display_type_is_compatible (GstVaapiDisplayType type1,
    GstVaapiDisplayType type2);

/**
 * GstVaapiDisplayInfo:
 *
 * Generic class to retrieve VA display info
 */
struct _GstVaapiDisplayInfo
{
  GstVaapiDisplay *display;
  GstVaapiDisplayType display_type;
  gchar *display_name;
  VADisplay va_display;
  gpointer native_display;
};

/**
 * GstVaapiDisplayProperties:
 * @GST_VAAPI_DISPLAY_PROP_RENDER_MODE: rendering mode (#GstVaapiRenderMode).
 * @GST_VAAPI_DISPLAY_PROP_ROTATION: rotation angle (#GstVaapiRotation).
 * @GST_VAAPI_DISPLAY_PROP_HUE: hue (float: [-180 ; 180], default: 0).
 * @GST_VAAPI_DISPLAY_PROP_SATURATION: saturation (float: [0 ; 2], default: 1).
 * @GST_VAAPI_DISPLAY_PROP_BRIGHTNESS: brightness (float: [-1 ; 1], default: 0).
 * @GST_VAAPI_DISPLAY_PROP_CONTRAST: contrast (float: [0 ; 2], default: 1).
 */
#define GST_VAAPI_DISPLAY_PROP_RENDER_MODE      "render-mode"
#define GST_VAAPI_DISPLAY_PROP_ROTATION         "rotation"
#define GST_VAAPI_DISPLAY_PROP_HUE              "hue"
#define GST_VAAPI_DISPLAY_PROP_SATURATION       "saturation"
#define GST_VAAPI_DISPLAY_PROP_BRIGHTNESS       "brightness"
#define GST_VAAPI_DISPLAY_PROP_CONTRAST         "contrast"

GstVaapiDisplay *
gst_vaapi_display_new_with_display (VADisplay va_display);

GstVaapiDisplay *
gst_vaapi_display_ref (GstVaapiDisplay * display);

void
gst_vaapi_display_unref (GstVaapiDisplay * display);

void
gst_vaapi_display_replace (GstVaapiDisplay ** old_display_ptr,
    GstVaapiDisplay * new_display);

void
gst_vaapi_display_lock (GstVaapiDisplay * display);

void
gst_vaapi_display_unlock (GstVaapiDisplay * display);

void
gst_vaapi_display_sync (GstVaapiDisplay * display);

void
gst_vaapi_display_flush (GstVaapiDisplay * display);

GstVaapiDisplayType
gst_vaapi_display_get_class_type (GstVaapiDisplay * display);

GstVaapiDisplayType
gst_vaapi_display_get_display_type (GstVaapiDisplay * display);

const gchar *
gst_vaapi_display_get_display_name (GstVaapiDisplay * display);

VADisplay
gst_vaapi_display_get_display (GstVaapiDisplay * display);

guint
gst_vaapi_display_get_width (GstVaapiDisplay * display);

guint
gst_vaapi_display_get_height (GstVaapiDisplay * display);

void
gst_vaapi_display_get_size (GstVaapiDisplay * display, guint * pwidth,
    guint * pheight);

void
gst_vaapi_display_get_pixel_aspect_ratio (GstVaapiDisplay * display,
    guint * par_n, guint * par_d);

gboolean
gst_vaapi_display_has_video_processing (GstVaapiDisplay * display);

GArray *
gst_vaapi_display_get_decode_profiles (GstVaapiDisplay * display);

gboolean
gst_vaapi_display_has_decoder (GstVaapiDisplay * display,
    GstVaapiProfile profile, GstVaapiEntrypoint entrypoint);

GArray *
gst_vaapi_display_get_encode_profiles (GstVaapiDisplay * display);

gboolean
gst_vaapi_display_has_encoder (GstVaapiDisplay * display,
    GstVaapiProfile profile, GstVaapiEntrypoint entrypoint);

GArray *
gst_vaapi_display_get_image_formats (GstVaapiDisplay * display);

gboolean
gst_vaapi_display_has_image_format (GstVaapiDisplay * display,
    GstVideoFormat format);

GArray *
gst_vaapi_display_get_subpicture_formats (GstVaapiDisplay * display);

gboolean
gst_vaapi_display_has_subpicture_format (GstVaapiDisplay * display,
    GstVideoFormat format, guint * flags_ptr);

gboolean
gst_vaapi_display_has_property (GstVaapiDisplay * display, const gchar * name);

gboolean
gst_vaapi_display_get_property (GstVaapiDisplay * display, const gchar * name,
    GValue * out_value);

gboolean
gst_vaapi_display_set_property (GstVaapiDisplay * display, const gchar * name,
    const GValue * value);

gboolean
gst_vaapi_display_get_render_mode (GstVaapiDisplay * display,
    GstVaapiRenderMode * pmode);

gboolean
gst_vaapi_display_set_render_mode (GstVaapiDisplay * display,
    GstVaapiRenderMode mode);

GstVaapiRotation
gst_vaapi_display_get_rotation (GstVaapiDisplay * display);

gboolean
gst_vaapi_display_set_rotation (GstVaapiDisplay * display,
    GstVaapiRotation rotation);

const gchar *
gst_vaapi_display_get_vendor_string (GstVaapiDisplay * display);

gboolean
gst_vaapi_display_has_opengl (GstVaapiDisplay * display);

G_END_DECLS

#endif /* GST_VAAPI_DISPLAY_H */
