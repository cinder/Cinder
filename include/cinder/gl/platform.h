/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

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

#include "cinder/Cinder.h"

#define CINDER_GL_ES_VERSION_2		200
#define CINDER_GL_ES_VERSION_3		300
#define CINDER_GL_ES_VERSION_3_1	310
#define CINDER_GL_ES_VERSION_3_2	320

#if defined( CINDER_GL_ANGLE ) || defined( CINDER_WINRT )
	#define GL_GLEXT_PROTOTYPES
	#define CINDER_GL_ES
	// the default for ANGLE is GL ES 3, but can be overridden with CINDER_GL_ES_2
	#if defined( CINDER_GL_ES_2 )
		#include "GLES2/gl2.h"
		#include "GLES2/gl2ext.h"
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_2
	#else
		#include "GLES3/gl3.h"
		#include "GLES3/gl3ext.h"
		#include "GLES2/gl2ext.h"
		#define CINDER_GL_ES_3
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3
	#endif
	#pragma comment( lib, "libEGL.lib" )
	#pragma comment( lib, "libGLESv2.lib" )
#elif defined( CINDER_ANDROID )
 	// Default is OpenGL ES 3
	#define CINDER_GL_ES
 	#include "EGL/egl.h" 
 	#if defined( CINDER_GL_ES_2 )
 		#define GL_GLEXT_PROTOTYPES
		#include "cinder/linux/GLES2/gl2.h"
		#include "cinder/linux/GLES2/gl2ext.h"
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_2
	#elif defined( CINDER_GL_ES_3_1 )
		#define GL_GLEXT_PROTOTYPES
		#include "cinder/linux/GLES3/gl31.h"
		#include "cinder/linux/GLES2/gl2ext.h"
		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3_1
	#elif defined( CINDER_GL_ES_3_2 )
		#define GL_GLEXT_PROTOTYPES
		#include "cinder/linux/GLES3/gl32.h"
		#include "cinder/linux/GLES2/gl2ext.h"  
		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3_2
 	#else
 		#define GL_GLEXT_PROTOTYPES
		#include "cinder/linux/GLES3/gl3.h"
		#include "cinder/linux/GLES2/gl2ext.h"
		#define CINDER_GL_ES_3
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3
 	#endif
#elif defined( CINDER_LINUX )
	// Default is Desktop
 	#if defined( CINDER_GL_ES_2 )
 		#define GL_GLEXT_PROTOTYPES
 		#include "EGL/egl.h" 
		#include "cinder/linux/GLES2/gl2.h"
		#include "cinder/linux/GLES2/gl2ext.h"
 		#define CINDER_GL_ES
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_2
 	#elif defined( CINDER_GL_ES_3 )
 		#define GL_GLEXT_PROTOTYPES
 		#include "EGL/egl.h"
		#include "cinder/linux/GLES3/gl3.h"
		#include "cinder/linux/GLES2/gl2ext.h"
 		#define CINDER_GL_ES
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3
	#elif defined( CINDER_GL_ES_3_1 )
		#define GL_GLEXT_PROTOTYPES
 		#include "EGL/egl.h" 
		#include "cinder/linux/GLES3/gl31.h"
		#include "cinder/linux/GLES2/gl2ext.h"
 		#define CINDER_GL_ES
		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3_1
	#elif defined( CINDER_GL_ES_3_2 )
		#define GL_GLEXT_PROTOTYPES
 		#include "EGL/egl.h" 
		#include "cinder/linux/GLES3/gl32.h"
		#include "cinder/linux/GLES2/gl2ext.h" 
 		#define CINDER_GL_ES
		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3_2
 	#else
 		#include "glload/gl_core.h"
 	#endif
#elif ! defined( CINDER_COCOA_TOUCH ) // OS X
	#if defined( __clang__ )
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wtypedef-redefinition"
	#endif
	#include "glload/gl_core.h"
	#if defined( __clang__ )
		#pragma clang diagnostic pop
	#endif
#else // iOS
	#define CINDER_GL_ES
	// the default for iOS is GL ES 2, but can be overridden with CINDER_GL_ES_3
	#if defined( CINDER_GL_ES_3 )
		#include <OpenGLES/ES3/gl.h>
		#include <OpenGLES/ES3/glext.h>
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3
	#else
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
		#define CINDER_GL_ES_2
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_2		
	#endif
#endif

// OpenGL ES
#if defined( CINDER_GL_ES )
	// iOS
	#if defined( CINDER_COCOA_TOUCH )
		#if defined( CINDER_GL_ES_2 )
			// GL_OES_texture_half_float
			#define GL_HALF_FLOAT								GL_HALF_FLOAT_OES

			// GL_EXT_texture_rg
			#define GL_RED										GL_RED_EXT
			#define GL_RG										GL_RG_EXT
			#define GL_R8										GL_R8_EXT
			#define GL_RG8										GL_RG8_EXT

			// GL_OES_rgb8_rgba8
			#define GL_RGB8										GL_RGB8_OES
			#define GL_RGBA8									GL_RGBA8_OES

			// GL_EXT_color_buffer_half_float
			#define GL_RGBA16F									GL_RGBA16F_EXT
			#define GL_RGB16F									GL_RGB16F_EXT
			#define GL_RG16F									GL_RG16F_EXT
			#define GL_R16F										GL_R16F_EXT
			#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE	GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT
			#define GL_UNSIGNED_NORMALIZED						GL_UNSIGNED_NORMALIZED_EXT

			// GL_EXT_texture_storage
			#define GL_RGBA32F									GL_RGBA32F_EXT
			#define GL_RGB32F									GL_RGB32F_EXT
			#define GL_RG32F									GL_RG32F_EXT
			#define GL_R32F										GL_R32F_EXT

			// GL_APPLE_texture_packed_float
			#define GL_UNSIGNED_INT_10F_11F_11F_REV 			GL_UNSIGNED_INT_10F_11F_11F_REV_APPLE
			#define GL_UNSIGNED_INT_5_9_9_9_REV 				GL_UNSIGNED_INT_5_9_9_9_REV_APPLE
			#define GL_R11F_G11F_B10F           				GL_R11F_G11F_B10F_APPLE
			#define GL_RGB9_E5                  				GL_RGB9_E5_APPLE

			// GL_EXT_sRGB
			#define GL_SRGB										GL_SRGB_EXT
			#define GL_SRGB_ALPHA								GL_SRGB_ALPHA_EXT
			#define GL_SRGB8_ALPHA8								GL_SRGB8_ALPHA8_EXT
			#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING	GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING_EXT

			// GL_OES_depth24
			#define GL_DEPTH_COMPONENT24						GL_DEPTH_COMPONENT24_OES

			#define GL_WRITE_ONLY								GL_WRITE_ONLY_OES
			#define GL_BUFFER_ACCESS							GL_BUFFER_ACCESS_OES
			#define GL_BUFFER_MAPPED							GL_BUFFER_MAPPED_OES
			#define GL_BUFFER_MAP_POINTER						GL_BUFFER_MAP_POINTER_OES

			#define GL_MAP_READ_BIT								GL_MAP_READ_BIT_EXT
			#define GL_MAP_WRITE_BIT							GL_MAP_WRITE_BIT_EXT
			#define GL_MAP_INVALIDATE_RANGE_BIT					GL_MAP_INVALIDATE_RANGE_BIT_EXT
			#define GL_MAP_INVALIDATE_BUFFER_BIT				GL_MAP_INVALIDATE_BUFFER_BIT_EXT
			#define GL_MAP_FLUSH_EXPLICIT_BIT					GL_MAP_FLUSH_EXPLICIT_BIT_EXT
			#define GL_MAP_UNSYNCHRONIZED_BIT					GL_MAP_UNSYNCHRONIZED_BIT_EXT

			#define GL_READ_FRAMEBUFFER							GL_READ_FRAMEBUFFER_APPLE
			#define GL_DRAW_FRAMEBUFFER							GL_DRAW_FRAMEBUFFER_APPLE
			#define GL_READ_FRAMEBUFFER_BINDING					GL_READ_FRAMEBUFFER_BINDING_APPLE
			#define GL_DRAW_FRAMEBUFFER_BINDING					GL_DRAW_FRAMEBUFFER_BINDING_APPLE

			#define glBindVertexArray							glBindVertexArrayOES
			#define glDeleteVertexArrays						glDeleteVertexArraysOES
			#define glGenVertexArrays							glGenVertexArraysOES
			#define glIsVertexArray								glIsVertexArrayOES

			#define glMapBuffer									glMapBufferOES
			#define glUnmapBuffer								glUnmapBufferOES

			#define glMapBufferRange							glMapBufferRangeEXT

			#define glRenderbufferStorageMultisample			glRenderbufferStorageMultisampleAPPLE
			#define glResolveMultisampleFramebuffer				glResolveMultisampleFramebufferAPPLE

 			#define glVertexAttribDivisor 						glVertexAttribDivisorEXT
		#endif

	// Android and Linux
	#elif defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
		#define GL_ES_EXT_VERSION_2_0
		#include "cinder/linux/gl_es_load.h"

		#define CINDER_GL_HAS_DRAW_INSTANCED
		#define CINDER_GL_HAS_TEXTURE_NORM16

		// Requires: GL_ANDROID_extension_pack_es31a
		#if defined( CINDER_ANDROID ) && ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_3_1 )
			#define CINDER_GL_HAS_GEOM_SHADER
			#define CINDER_GL_HAS_TESS_SHADER 
		#endif
	#endif

	// Android, iOS, and Linux
	#if ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 )
		#define CINDER_GL_HAS_DRAW_INSTANCED
		#define CINDER_GL_HAS_FBO_MULTISAMPLING
		#define CINDER_GL_HAS_UNIFORM_BLOCKS
		#define CINDER_GL_HAS_MAP_BUFFER_RANGE
		#define CINDER_GL_HAS_TRANSFORM_FEEDBACK 
		#define CINDER_GL_HAS_RENDER_SNORM
		#define CINDER_GL_HAS_REQUIRED_INTERNALFORMAT
	#else // OpenGL ES 2
		#define CINDER_GL_HAS_DRAW_INSTANCED
		#define CINDER_GL_HAS_FBO_MULTISAMPLING
		#define CINDER_GL_HAS_MAP_BUFFER
		#define CINDER_GL_HAS_MAP_BUFFER_RANGE
		#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
			#define CINDER_GL_HAS_RENDER_SNORM
			#define CINDER_GL_HAS_REQUIRED_INTERNALFORMAT
		#endif
	#endif

#else // OpenGL Desktop
	#define CINDER_GL_HAS_UNIFORM_BLOCKS
	#define CINDER_GL_HAS_DRAW_INSTANCED
	#define CINDER_GL_HAS_FBO_MULTISAMPLING
	#define CINDER_GL_HAS_TRANSFORM_FEEDBACK
	#define CINDER_GL_HAS_WRAP_R
	#define CINDER_GL_HAS_SHADOW_SAMPLERS
 	#define CINDER_GL_HAS_MAP_BUFFER
 	#define CINDER_GL_HAS_MAP_BUFFER_RANGE
 	#define CINDER_GL_HAS_INSTANCED_ARRAYS
	#define CINDER_GL_HAS_GEOM_SHADER
	#define CINDER_GL_HAS_TESS_SHADER
	
	#define CINDER_GL_HAS_RENDER_SNORM
	#define CINDER_GL_HAS_REQUIRED_INTERNALFORMAT

	#define CINDER_GL_HAS_TEXTURE_NORM16
#endif

#if defined( CINDER_MSW )
	#if ! defined( CINDER_GL_ANGLE ) // MSW Desktop Only
		#define CINDER_GL_HAS_COMPUTE_SHADER
		#define CINDER_GL_HAS_DEBUG_OUTPUT
	#endif
	// both ANGLE and desktop have FBO Multisampling
	#define CINDER_GL_HAS_FBO_MULTISAMPLING
	// platform-specific synonyms
	#if defined( CINDER_GL_ANGLE ) && ! defined( CINDER_GL_ES_3 )
		#define GL_READ_FRAMEBUFFER					GL_READ_FRAMEBUFFER_ANGLE
		#define GL_DRAW_FRAMEBUFFER					GL_DRAW_FRAMEBUFFER_ANGLE
		#define GL_READ_FRAMEBUFFER_BINDING			GL_READ_FRAMEBUFFER_BINDING_ANGLE
		#define GL_DRAW_FRAMEBUFFER_BINDING			GL_DRAW_FRAMEBUFFER_BINDING_ANGLE
		#define glRenderbufferStorageMultisample	glRenderbufferStorageMultisampleANGLE
	#endif
#endif // defined( CINDER_MSW )

#if defined( GL_EXT_debug_label )
	#define CINDER_GL_HAS_DEBUG_LABEL 
#endif

#if defined( CINDER_GL_ES )
	#if defined( GL_KHR_debug ) && ( CINDER_GL_ES_VERSION <= CINDER_GL_ES_VERSION_3_1 )
		#define CINDER_GL_HAS_KHR_DEBUG
		#define GL_BUFFER 		GL_BUFFER_KHR
		#define GL_SHADER 		GL_SHADER_KHR
		#define GL_PROGRAM 		GL_PROGRAM_KHR
	#endif
#else
	#define CINDER_GL_HAS_KHR_DEBUG
#endif