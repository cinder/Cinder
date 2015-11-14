/*
 * mpegts.h -
 * Copyright (C) 2013 Edward Hervey
 *
 * Authors:
 *   Edward Hervey <edward@collabora.com>
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

#ifndef _GST_MPEGTS_H_
#define _GST_MPEGTS_H_

#ifndef GST_USE_UNSTABLE_API
#warning "The GStreamer mpegts library is unstable API and may change in future."
#warning "You can define GST_USE_UNSTABLE_API to avoid this warning."
#endif

#include <gst/mpegts/gstmpegtsdescriptor.h>
#include <gst/mpegts/gst-dvb-descriptor.h>
#include <gst/mpegts/gstmpegtssection.h>
#include <gst/mpegts/gst-atsc-section.h>
#include <gst/mpegts/gst-dvb-section.h>
#include <gst/mpegts/gst-scte-section.h>
#include <gst/mpegts/gstmpegts-enumtypes.h>

G_BEGIN_DECLS

void gst_mpegts_initialize (void);

G_END_DECLS

#endif
