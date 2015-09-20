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
 	#error "cinder/gl/platform_es2_features.h cannot be included directly, please include cinder/gl/platform.h instead."
#endif 

#if defined( CINDER_GL_ES_2 )

#if defined( GL_EXT_texture_storage )
	#define CINDER_GL_HAS_TEXTURE_STORAGE
#endif

#if defined( GL_EXT_color_buffer_half_float )
	#define CINDER_GL_HAS_COLOR_BUFFER_HALF_FLOAT
#endif

#if defined( GL_EXT_shadow_samplers )
	#define CINDER_GL_HAS_SHADOW_SAMPLERS
	#define GL_TEXTURE_COMPARE_MODE			GL_TEXTURE_COMPARE_MODE_EXT
	#define GL_TEXTURE_COMPARE_FUNC			GL_TEXTURE_COMPARE_FUNC_EXT
	#define GL_COMPARE_REF_TO_TEXTURE		GL_COMPARE_REF_TO_TEXTURE_EXT
	#define GL_SAMPLER_2D_SHADOW			GL_SAMPLER_2D_SHADOW_EXT
#endif

#if defined( GL_OES_mapbuffer )
	#define CINDER_GL_HAS_MAP_BUFFER
 	#define glMapBuffer 					glMapBufferOES
 	#define glUnmapBuffer 					glUnmapBufferOES
 	#define glGetBufferPointerv 			glGetBufferPointervOES
#endif

#if defined( GL_EXT_map_buffer_range )
	#define CINDER_GL_HAS_MAP_BUFFER_RANGE
	#define GL_MAP_READ_BIT					GL_MAP_READ_BIT_EXT
	#define GL_MAP_WRITE_BIT				GL_MAP_WRITE_BIT_EXT
	#define GL_MAP_INVALIDATE_RANGE_BIT		GL_MAP_INVALIDATE_RANGE_BIT_EXT
	#define GL_MAP_INVALIDATE_BUFFER_BIT	GL_MAP_INVALIDATE_BUFFER_BIT_EXT
	#define GL_MAP_FLUSH_EXPLICIT_BIT		GL_MAP_FLUSH_EXPLICIT_BIT_EXT
	#define GL_MAP_UNSYNCHRONIZED_BIT 		GL_MAP_UNSYNCHRONIZED_BIT_EXT
 	#define glMapBufferRange 				glMapBufferRangeEXT
 	#define glFlushMappedBufferRange 		glFlushMappedBufferRangeEXT
#endif

#if defined( GL_EXT_instanced_arrays )
 	#define CINDER_GL_HAS_INSTANCED_ARRAYS
 	#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR	GL_VERTEX_ATTRIB_ARRAY_DIVISOR_EXT
#endif

#endif // defined( CINDER_GL_ES_2 )