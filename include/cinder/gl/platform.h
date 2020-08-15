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

#if ! defined( CINDER_GL_ANGLE ) && defined( CINDER_UWP )
	#define CINDER_GL_ANGLE
#endif

#if defined( CINDER_GL_ANGLE )
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
		#include "glad/glad_es.h"
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_2
	#elif defined( CINDER_GL_ES_3_1 )
		#include "glad/glad_es.h"
		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3_1
	#elif defined( CINDER_GL_ES_3_2 )
		#include "glad/glad_es.h"
		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3_2
 	#else
		#include "glad/glad_es.h"
		#define CINDER_GL_ES_3
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3
 	#endif
#elif defined( CINDER_LINUX )
	// Default is Desktop
 	#if defined( CINDER_GL_ES_2 )
 		#include "EGL/egl.h" 
		#include "glad/glad_es.h"
 		#define CINDER_GL_ES
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_2
 	#elif defined( CINDER_GL_ES_3 )
 		#include "EGL/egl.h"
		#include "glad/glad_es.h"
 		#define CINDER_GL_ES
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3
	#elif defined( CINDER_GL_ES_3_1 )
 		#include "EGL/egl.h" 
		#include "glad/glad_es.h"
 		#define CINDER_GL_ES
		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3_1
	#elif defined( CINDER_GL_ES_3_2 )
 		#include "EGL/egl.h" 
		#include "glad/glad_es.h"
 		#define CINDER_GL_ES
		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3_2
	#else
		#include "glad/glad.h"
	#endif
#elif ! defined( CINDER_COCOA_TOUCH ) // OS X
	#if defined( __clang__ )
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wtypedef-redefinition"
	#endif
	#include "glad/glad.h"
	#if defined( __clang__ )
		#pragma clang diagnostic pop
	#endif
#else // iOS
	#define CINDER_GL_ES
	// the default for iOS is GL ES 3, but can be overridden with CINDER_GL_ES_2
	#if ! defined( CINDER_GL_ES_2 )
		#include <OpenGLES/ES3/gl.h>
		#include <OpenGLES/ES3/glext.h>
		#define CINDER_GL_ES_3
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_3
	#else
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
 		#define CINDER_GL_ES_VERSION CINDER_GL_ES_VERSION_2		
	#endif
#endif

// OpenGL ES
#if defined( CINDER_GL_ES )
	#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX ) 	
		#define GL_ES_EXT_VERSION_2_0
	#endif
	#if ! defined( CINDER_COCOA_TOUCH ) && ! defined( CINDER_GL_ANGLE )
		#include "glad/glad_es.h"
	#endif

	// Android and Linux
	#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
		#if ! defined( CINDER_GL_ES_3_RPI )
		    #define CINDER_GL_HAS_DRAW_INSTANCED
			#define CINDER_GL_HAS_TEXTURE_NORM16
		#endif

		// Requires: GL_ANDROID_extension_pack_es31a
		#if defined( CINDER_ANDROID ) && ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_3_1 )
			#define CINDER_GL_HAS_GEOM_SHADER
			#define CINDER_GL_HAS_TESS_SHADER 
		#endif
	#endif

	// Android, iOS, and Linux
	#if ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 )
 		// OpenGL ES 3+
		#define CINDER_GL_HAS_DRAW_INSTANCED
		#define CINDER_GL_HAS_FBO_MULTISAMPLING
		#define CINDER_GL_HAS_UNIFORM_BLOCKS
		#define CINDER_GL_HAS_MAP_BUFFER_RANGE
		#define CINDER_GL_HAS_TRANSFORM_FEEDBACK 
		#define CINDER_GL_HAS_RENDER_SNORM
		#define CINDER_GL_HAS_REQUIRED_INTERNALFORMAT
		#define CINDER_GL_HAS_SAMPLERS
	#else 
		// OpenGL ES 2
		#if ! defined( CINDER_GL_ES_3_RPI )
		    #define CINDER_GL_HAS_DRAW_INSTANCED
		    #define CINDER_GL_HAS_FBO_MULTISAMPLING
		    #define CINDER_GL_HAS_MAP_BUFFER_RANGE
		#endif
		#define CINDER_GL_HAS_MAP_BUFFER
		#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )
			#define CINDER_GL_HAS_RENDER_SNORM
			#define CINDER_GL_HAS_REQUIRED_INTERNALFORMAT
		#endif
	#endif

 	// Android, Linux
	#if ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_1 )
 		#define CINDER_GL_HAS_COMPUTE_SHADER
		#define CINDER_GL_HAS_DRAW_INDIRECT
 	#endif

#else // OpenGL Desktop
	#define CINDER_GL_HAS_UNIFORM_BLOCKS
	#define CINDER_GL_HAS_DRAW_INSTANCED
	#define CINDER_GL_HAS_MULTI_DRAW
	#define CINDER_GL_HAS_DRAW_INDIRECT
	#define CINDER_GL_HAS_FBO_MULTISAMPLING
	#define CINDER_GL_HAS_TRANSFORM_FEEDBACK
	#define CINDER_GL_HAS_WRAP_R
	#define CINDER_GL_HAS_SHADOW_SAMPLERS
 	#define CINDER_GL_HAS_MAP_BUFFER
 	#define CINDER_GL_HAS_MAP_BUFFER_RANGE
 	#define CINDER_GL_HAS_INSTANCED_ARRAYS
	#define CINDER_GL_HAS_GEOM_SHADER
	#define CINDER_GL_HAS_TESS_SHADER
	#define CINDER_GL_HAS_SAMPLERS
	
	#define CINDER_GL_HAS_RENDER_SNORM
	#define CINDER_GL_HAS_REQUIRED_INTERNALFORMAT
	#define CINDER_GL_HAS_TEXTURE_NORM16
	
	#if defined( CINDER_LINUX ) || defined( CINDER_MSW )
		#define CINDER_GL_HAS_COMPUTE_SHADER
		#define CINDER_GL_HAS_DEBUG_OUTPUT
		#define CINDER_GL_HAS_MULTI_DRAW_INDIRECT
	#endif
#endif

#if defined( CINDER_MSW_DESKTOP )
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
#endif // defined( CINDER_MSW_DESKTOP )

#if defined( GL_EXT_debug_label )
	#define CINDER_GL_HAS_DEBUG_LABEL 
#endif

#if defined( CINDER_GL_ES )
	#if defined( GL_KHR_debug ) && ( CINDER_GL_ES_VERSION <= CINDER_GL_ES_VERSION_3_1 ) && ( ! defined( CINDER_GL_ANGLE ) )
		#define CINDER_GL_HAS_KHR_DEBUG
		#if ! defined( CINDER_GL_ANGLE )
			#define GL_BUFFER 			GL_BUFFER_KHR
			#define GL_SHADER 			GL_SHADER_KHR
			#define GL_PROGRAM 			GL_PROGRAM_KHR
			#define GL_DEBUG_SOURCE_APPLICATION	GL_DEBUG_SOURCE_APPLICATION_KHR
			#define glPushDebugGroup		glPushDebugGroupKHR
			#define glPopDebugGroup			glPopDebugGroupKHR
		#endif
	#endif
#else
	#define CINDER_GL_HAS_KHR_DEBUG
#endif
