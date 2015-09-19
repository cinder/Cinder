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
	#if ( __ANDROID_API__ >= 21 )
		#include "GLES2/gl2ext.h"
	#else
 		// GLES/gl2ext.h collides with GLES3/gl3.h so certain constants
 		// have to be defined manually.

 		// GL_APPLE_texture_format_BGRA8888
		#ifndef GL_APPLE_texture_format_BGRA8888
			#define GL_APPLE_texture_format_BGRA8888 			1
			#define GL_BGRA_EXT                       			0x80E1
			#define GL_BGRA8_EXT                      			0x93A1
		#endif

 		// GL_EXT_texture_filter_anisotropic
		#ifndef GL_EXT_texture_filter_anisotropic
			#define GL_EXT_texture_filter_anisotropic 			1
			#define GL_TEXTURE_MAX_ANISOTROPY_EXT     			0x84FE
			#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 			0x84FF
		#endif

 		// GL_EXT_debug_label
 		#ifndef GL_EXT_debug_label
 			#define GL_EXT_debug_label 							1
	 		#define GL_BUFFER_OBJECT_EXT 						0x9151
 			#define GL_PROGRAM_OBJECT_EXT             			0x8B40
 			#define GL_VERTEX_ARRAY_OBJECT_EXT        			0x9154
 		#endif

		// GL_OES_EGL_image_external
		#ifndef GL_OES_EGL_image_external
			#define GL_OES_EGL_image_external 					1
			#define GL_TEXTURE_EXTERNAL_OES           			0x8D65
			#define GL_TEXTURE_BINDING_EXTERNAL_OES   			0x8D67
			#define GL_REQUIRED_TEXTURE_IMAGE_UNITS_OES 		0x8D68
			#define GL_SAMPLER_EXTERNAL_OES          	 		0x8D66
		#endif 

 		// GL_IMG_texture_compression_pvrtc
		#ifndef GL_IMG_texture_compression_pvrtc
			#define GL_IMG_texture_compression_pvrtc 			1
			#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 			0x8C00
			#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 			0x8C01
			#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 		0x8C02
			#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 		0x8C03
		#endif
	#endif


 	#define CINDER_GL_HAS_DRAW_INSTANCED
#endif

// Map these symbols since they're not defined on OpenGL ES2 on ARM32 and ARM64.
#if defined( CINDER_GL_ES_2 ) && (defined(__arm__) || defined(__aarch64__))
	#if ( __ANDROID_API__ < 21 )
    	#define PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMG
  	#endif

	extern PFNGLGENVERTEXARRAYSOESPROC    				glGenVertexArraysOESEXT;
	extern PFNGLBINDVERTEXARRAYOESPROC    				glBindVertexArrayOESEXT;
	extern PFNGLDELETEVERTEXARRAYSOESPROC 				glDeleteVertexArraysOESEXT;
	extern PFNGLISVERTEXARRAYOESPROC					glIsVertexArrayOESEXT;
	extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC	glRenderbufferStorageMultisampleIMGEXT;

	#define glGenVertexArraysOES 						glGenVertexArraysOESEXT
	#define glBindVertexArrayOES 						glBindVertexArrayOESEXT
	#define glDeleteVertexArraysOES 					glDeleteVertexArraysOESEXT
	#define glIsVertexArrayOES							glIsVertexArrayOESEXT
	#define glRenderbufferStorageMultisampleIMG			glRenderbufferStorageMultisampleIMGEXT
#endif
