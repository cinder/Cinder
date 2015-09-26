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

#if ! defined( CINDER_GL_PLATFORM )
 	#error "cinder/gl/platform_android.h cannot be included directly, please include cinder/gl/platform.h instead."
#endif

#if defined( CINDER_GL_ES_3 )

#if defined( GL_OES_mapbuffer )
	#define CINDER_GL_HAS_MAP_BUFFER
	extern PFNGLMAPBUFFEROESPROC 				fnptr_ci_glMapBufferOES;
	extern PFNGLUNMAPBUFFEROESPROC 				fnptr_ci_glUnmapBufferOES;
	extern PFNGLGETBUFFERPOINTERVOESPROC 		fnptr_ci_glGetBufferPointervOES;
 	#define glMapBuffer 						fnptr_ci_glMapBufferOES
 	#define glUnmapBuffer 						fnptr_ci_glUnmapBufferOES
 	#define glGetBufferPointerv 				fnptr_ci_glGetBufferPointervOES
#endif

#endif // defined( CINDER_GL_ES_3 )