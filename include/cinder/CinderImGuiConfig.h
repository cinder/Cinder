/*
 Copyright (c) 2010, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/Export.h"
#include "cinder/Color.h"
#include "cinder/Vector.h"
#include "cinder/CinderAssert.h"
#include "cinder/gl/platform.h"

#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM "cinder/gl/platform.h"
#if defined( CINDER_GL_ES_3 )
#define IMGUI_IMPL_OPENGL_ES3
#endif

//---- Define assertion handler. Defaults to calling assert().
// If your macro uses multiple statements, make sure is enclosed in a 'do { .. } while (0)' block so it can be used as a single statement.
#define IM_ASSERT(_EXPR)  CI_ASSERT(_EXPR)
//#define IM_ASSERT(_EXPR)  ((void)(_EXPR))     // Disable asserts

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows
// Using dear imgui via a shared library is not recommended, because of function call overhead and because we don't guarantee backward nor forward ABI compatibility.
#define IMGUI_API CI_API

// Custom implicit cast operators	
// Custom implicit cast operators
#ifndef CINDER_IMGUI_NO_IMPLICIT_CASTS
#define IM_VEC2_CLASS_EXTRA                                            \
ImVec2(const ci::vec2& f) { x = f.x; y = f.y; }                        \
operator ci::vec2() const { return ci::vec2(x,y); }                    \
ImVec2(const ci::ivec2& f) { x = static_cast<float>( f.x ); y = static_cast<float>( f.y ); }  \
operator ci::ivec2() const { return ci::ivec2(x,y); }

#define IM_VEC4_CLASS_EXTRA                                            \
ImVec4(const glm::vec4& f) { x = f.x; y = f.y; z = f.z; w = f.w; }     \
operator glm::vec4() const { return ci::vec4(x,y,z,w); }               \
ImVec4(const ci::ColorA& f) { x = f.r; y = f.g; z = f.b; w = f.a; }    \
operator ci::ColorA() const { return ci::ColorA(x,y,z,w); }            \
ImVec4(const ci::Color& f) { x = f.r; y = f.g; z = f.b; w = 1.0f; }    \
operator ci::Color() const { return ci::Color(x,y,z); }
#endif