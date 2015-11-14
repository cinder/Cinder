/*
 *  gstvaapidisplay_glx.h - VA/GLX display abstraction
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

#ifndef GST_VAAPI_DISPLAY_GLX_H
#define GST_VAAPI_DISPLAY_GLX_H

#include <GL/gl.h>
#include <GL/glx.h>
#include <gst/vaapi/gstvaapidisplay_x11.h>

G_BEGIN_DECLS

#define GST_VAAPI_DISPLAY_GLX(obj) \
    ((GstVaapiDisplayGLX *)(obj))

typedef struct _GstVaapiDisplayGLX              GstVaapiDisplayGLX;

GstVaapiDisplay *
gst_vaapi_display_glx_new (const gchar * display_name);

GstVaapiDisplay *
gst_vaapi_display_glx_new_with_display (Display * x11_display);

G_END_DECLS

#endif /* GST_VAAPI_DISPLAY_GLX_H */
