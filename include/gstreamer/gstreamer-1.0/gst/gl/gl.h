/*
 * GStreamer
 * Copyright (C) 2013 Julien Isorce <julien.isorce@gmail.com>
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

#ifndef __GST_GL_H__
#define __GST_GL_H__

#ifndef GST_USE_UNSTABLE_API
#warning "The GL library from gst-plugins-bad is unstable API and may change in future."
#warning "You can define GST_USE_UNSTABLE_API to avoid this warning."
#endif

#include <gst/gl/gstgl_fwd.h>
#include <gst/gl/gstglconfig.h>
#include <gst/gl/gstglapi.h>
#include <gst/gl/gstgldisplay.h>
#include <gst/gl/gstglcontext.h>
#include <gst/gl/gstglfeature.h>
#include <gst/gl/gstglutils.h>
#include <gst/gl/gstglwindow.h>
#include <gst/gl/gstglshader.h>
#include <gst/gl/gstglcolorconvert.h>
#include <gst/gl/gstglupload.h>
#include <gst/gl/gstgluploadmeta.h>
#include <gst/gl/gstgldownload.h>
#include <gst/gl/gstglbasebuffer.h>
#include <gst/gl/gstglmemory.h>
#include <gst/gl/gstglbufferpool.h>
#include <gst/gl/gstglframebuffer.h>
#include <gst/gl/gstglbasefilter.h>
#include <gst/gl/gstglviewconvert.h>
#include <gst/gl/gstglfilter.h>
#include <gst/gl/gstglshadervariables.h>
#include <gst/gl/gstglsyncmeta.h>
#include <gst/gl/gstgloverlaycompositor.h>

#endif /* __GST_GL_H__ */
