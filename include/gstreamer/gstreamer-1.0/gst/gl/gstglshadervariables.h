/*
 * GStreamer
 * Copyright (C) 2009 Luc Deschenaux <luc.deschenaux@freesurf.ch>
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

#ifndef GST_GLSHADER_VARIABLES
#define GST_GLSHADER_VARIABLES

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <gst/gl/gstglshader.h>

#define VALID_VARNAME_CHARS "abcdefghijklmnopqurstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"

struct gst_gl_shadervariable_desc;

int gst_gl_shadervariables_parse (GstGLShader * shader, char *variables,
    int (*_setvariable) (GstGLShader * shader,
        struct gst_gl_shadervariable_desc * v));
int gst_gl_shadervariable_set (GstGLShader * shader,
    struct gst_gl_shadervariable_desc *ret);

#endif
