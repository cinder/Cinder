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

#if defined( CINDER_GL_ANGLE ) || defined( CINDER_WINRT )
	#define GL_GLEXT_PROTOTYPES
	#define CINDER_GL_ES
	// the default for ANGLE is GL ES 3, but can be overridden with CINDER_GL_ES_2
	#if defined( CINDER_GL_ES_2 )
		#include "GLES2/gl2.h"
		#include "GLES2/gl2ext.h"
	#else
		#include "GLES3/gl3.h"
		#include "GLES3/gl3ext.h"
		#include "GLES2/gl2ext.h"
		#define CINDER_GL_ES_3
	#endif
	#pragma comment( lib, "libEGL.lib" )
	#pragma comment( lib, "libGLESv2.lib" )
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
	#if ! defined( CINDER_GL_ES_3 )
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
		#define CINDER_GL_ES_2
	#else
		#include <OpenGLES/ES3/gl.h>
		#include <OpenGLES/ES3/glext.h>
	#endif
#endif

// ----------------------------------------------
// define features available
#if ! defined( CINDER_GL_ES_2 ) // ES 3 + Desktop Only
	#define CINDER_GL_HAS_UNIFORM_BLOCKS
	#define CINDER_GL_HAS_DRAW_INSTANCED
	#define CINDER_GL_HAS_FBO_MULTISAMPLING
	#define CINDER_GL_HAS_TRANSFORM_FEEDBACK
	#if ! defined( CINDER_GL_ES_3 ) // Desktop Only
		#define CINDER_GL_HAS_GEOM_SHADER
		#define CINDER_GL_HAS_TESS_SHADER
	#endif // ! defined( CINDER_GL_ES_3 )
#endif // ! defined( CINDER_GL_ES_2 )

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
#endif // defined( CINDER_COCOA )

#if defined( CINDER_COCOA_TOUCH ) // iOS Only
	#define CINDER_GL_HAS_DRAW_INSTANCED
	#define CINDER_GL_HAS_FBO_MULTISAMPLING
	// platform-specific synonyms
	#if ! defined( CINDER_GL_ES_3 )
		#define GL_READ_FRAMEBUFFER					GL_READ_FRAMEBUFFER_APPLE
		#define GL_DRAW_FRAMEBUFFER					GL_DRAW_FRAMEBUFFER_APPLE
		#define GL_READ_FRAMEBUFFER_BINDING			GL_READ_FRAMEBUFFER_BINDING_APPLE
		#define GL_DRAW_FRAMEBUFFER_BINDING			GL_DRAW_FRAMEBUFFER_BINDING_APPLE
		#define glRenderbufferStorageMultisample	glRenderbufferStorageMultisampleAPPLE
		#define glResolveMultisampleFramebuffer		glResolveMultisampleFramebufferAPPLE
	#endif
#endif // defined( CINDER_COCOA )
