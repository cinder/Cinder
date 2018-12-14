/*
 Copyright (c) 2014, The Cinder Project: http://libcinder.org
 All rights reserved.

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


#include "cinder/gl/ConstantConversions.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#if ! defined( CINDER_GL_ES )
	#define CINDER_GL_DESKTOP
#endif

#include <sstream>
#include <unordered_map>


namespace cinder { namespace gl {

//! Returns a string representation for a subset of the GL constants. Returns empty string if unknown.
std::string	constantToString( GLenum constant )
{
	static bool initialized = false;
	static std::unordered_map<GLenum,std::string> sSymbols;
	if( ! initialized ) {
		// Types
		sSymbols[GL_BYTE] = "BYTE";
		sSymbols[GL_UNSIGNED_BYTE] = "UNSIGNED_BYTE";
		sSymbols[GL_SHORT] = "SHORT";
		sSymbols[GL_UNSIGNED_SHORT] = "UNSIGNED_SHORT";
		sSymbols[GL_INT] = "INT";
		sSymbols[GL_UNSIGNED_INT] = "UNSIGNED_INT";
		sSymbols[GL_FIXED] = "FIXED";
		sSymbols[GL_FLOAT] = "FLOAT";
		sSymbols[GL_FLOAT_VEC2] = "FLOAT_VEC2";
		sSymbols[GL_FLOAT_VEC3] = "FLOAT_VEC3";
		sSymbols[GL_FLOAT_VEC4] = "FLOAT_VEC4";
#if ! defined( CINDER_GL_ES_2 )
		sSymbols[GL_UNSIGNED_INT_VEC2] = "UNSIGNED_INT_VEC2";
		sSymbols[GL_UNSIGNED_INT_VEC3] = "UNSIGNED_INT_VEC3";
		sSymbols[GL_UNSIGNED_INT_VEC4] = "UNSIGNED_INT_VEC4";
		sSymbols[GL_INT_SAMPLER_2D] = "INT_SAMPLER_2D";
		sSymbols[GL_INT_SAMPLER_3D] = "INT_SAMPLER_3D";
		sSymbols[GL_INT_SAMPLER_CUBE] = "INT_SAMPLER_CUBE";
		sSymbols[GL_INT_SAMPLER_2D_ARRAY] = "GL_INT_SAMPLER_2D_ARRAY";
		sSymbols[GL_SAMPLER_3D] = "SAMPLER_3D";
		sSymbols[GL_UNSIGNED_INT_SAMPLER_2D] = "UNSIGNED_INT_SAMPLER_2D";
		sSymbols[GL_UNSIGNED_INT_SAMPLER_3D] = "UNSIGNED_INT_SAMPLER_3D";
		sSymbols[GL_UNSIGNED_INT_SAMPLER_2D_ARRAY] = "UNSIGNED_INT_SAMPLER_2D_ARRAY";
		sSymbols[GL_UNSIGNED_INT_SAMPLER_CUBE] = "UNSIGNED_INT_SAMPLER_CUBE";		
		sSymbols[GL_SAMPLER_2D_SHADOW] = "SAMPLER_2D_SHADOW";
		sSymbols[GL_SAMPLER_2D_ARRAY_SHADOW] = "SAMPLER_2D_ARRAY_SHADOW";
		sSymbols[GL_SAMPLER_CUBE_SHADOW] = "GL_SAMPLER_CUBE_SHADOW";
#endif
#if defined( CINDER_ANDROID )
		sSymbols[GL_SAMPLER_EXTERNAL_OES] = "GL_SAMPLER_EXTERNAL_OES";
#endif
		sSymbols[GL_INT_VEC2] = "INT_VEC2";
		sSymbols[GL_INT_VEC3] = "INT_VEC3";
		sSymbols[GL_INT_VEC4] = "INT_VEC4";
		sSymbols[GL_BOOL] = "BOOL";
		sSymbols[GL_BOOL_VEC2] = "BOOL_VEC2";
		sSymbols[GL_BOOL_VEC3] = "BOOL_VEC3";
		sSymbols[GL_BOOL_VEC4] = "BOOL_VEC4";
		sSymbols[GL_FLOAT_MAT2] = "FLOAT_MAT2";
		sSymbols[GL_FLOAT_MAT3] = "FLOAT_MAT3";
		sSymbols[GL_FLOAT_MAT4] = "FLOAT_MAT4";
		sSymbols[GL_SAMPLER_2D] = "SAMPLER_2D";
		sSymbols[GL_SAMPLER_CUBE] = "SAMPLER_CUBE";


		sSymbols[GL_HALF_FLOAT] = "HALF_FLOAT";
#if ! defined( CINDER_GL_ES )
		sSymbols[GL_SAMPLER_BUFFER] = "SAMPLER_BUFFER";
		sSymbols[GL_UNSIGNED_INT_SAMPLER_BUFFER] = "UNSIGNED_INT_SAMPLER_BUFFER";
		sSymbols[GL_SAMPLER_1D] = "SAMPLER_1D";
		sSymbols[GL_INT_SAMPLER_2D_RECT] = "INT_SAMPLER_2D_RECT";
		sSymbols[GL_UNSIGNED_INT_SAMPLER_2D_RECT] = "UNSIGNED_INT_SAMPLER_2D_RECT";
		sSymbols[GL_SAMPLER_2D_RECT] = "SAMPLER_2D_RECT";
		sSymbols[GL_SAMPLER_1D_SHADOW] = "SAMPLER_1D_SHADOW";
		sSymbols[GL_DOUBLE] = "DOUBLE";
		sSymbols[GL_INT_2_10_10_10_REV] = "INT_2_10_10_10_REV";
		sSymbols[GL_UNSIGNED_INT_2_10_10_10_REV] = "UNSIGNED_INT_2_10_10_10_REV";
#endif
		// Buffer bindings
		sSymbols[GL_ARRAY_BUFFER] = "GL_ARRAY_BUFFER";
		sSymbols[GL_ELEMENT_ARRAY_BUFFER] = "GL_ELEMENT_ARRAY_BUFFER";
		
#if ! defined( CINDER_GL_ES_2 )
		sSymbols[GL_PIXEL_PACK_BUFFER] = "GL_PIXEL_PACK_BUFFER";
		sSymbols[GL_PIXEL_UNPACK_BUFFER] = "GL_PIXEL_UNPACK_BUFFER";
		sSymbols[GL_COPY_READ_BUFFER] = "GL_COPY_READ_BUFFER";
		sSymbols[GL_COPY_WRITE_BUFFER] = "GL_COPY_WRITE_BUFFER";
		sSymbols[GL_TRANSFORM_FEEDBACK_BUFFER] = "GL_TRANSFORM_FEEDBACK_BUFFER";
		sSymbols[GL_UNIFORM_BUFFER] = "GL_UNIFORM_BUFFER";
#endif
#if ! defined( CINDER_GL_ES )
		sSymbols[GL_ATOMIC_COUNTER_BUFFER] = "GL_ATOMIC_COUNTER_BUFFER";
		sSymbols[GL_DRAW_INDIRECT_BUFFER] = "GL_DRAW_INDIRECT_BUFFER";
		sSymbols[GL_DISPATCH_INDIRECT_BUFFER] = "GL_DISPATCH_INDIRECT_BUFFER";
		sSymbols[GL_QUERY_BUFFER] = "GL_QUERY_BUFFER";
		sSymbols[GL_SHADER_STORAGE_BUFFER] = "GL_SHADER_STORAGE_BUFFER";
		sSymbols[GL_TEXTURE_BUFFER] = "GL_TEXTURE_BUFFER";
#endif
		// Buffer usage
		sSymbols[GL_STREAM_DRAW] = "GL_STREAM_DRAW";
		sSymbols[GL_STATIC_DRAW] = "GL_STATIC_DRAW";
		sSymbols[GL_DYNAMIC_DRAW] = "GL_DYNAMIC_DRAW";
#if ! defined( CINDER_GL_ES_2 )
		sSymbols[GL_STREAM_READ] = "GL_STREAM_READ";
		sSymbols[GL_STREAM_COPY] = "GL_STREAM_COPY";
		sSymbols[GL_STATIC_READ] = "GL_STATIC_READ";
		sSymbols[GL_STATIC_COPY] = "GL_STATIC_COPY";
		sSymbols[GL_DYNAMIC_READ] = "GL_DYNAMIC_READ";
		sSymbols[GL_DYNAMIC_COPY] = "GL_DYNAMIC_COPY";
#endif
		// Texture targets
		sSymbols[GL_TEXTURE_2D] = "GL_TEXTURE_2D";
		sSymbols[GL_TEXTURE_CUBE_MAP] = "GL_TEXTURE_CUBE_MAP";
#if ! defined( CINDER_GL_ES )
		sSymbols[GL_TEXTURE_1D] = "GL_TEXTURE_1D";
		sSymbols[GL_TEXTURE_3D] = "GL_TEXTURE_3D";
#endif
		// Texture internal formats
		sSymbols[GL_RGB] = "GL_RGB";
		sSymbols[GL_RGBA] = "GL_RGBA";
		sSymbols[GL_ALPHA] = "GL_ALPHA";

#if defined( CINDER_GL_ES )
		sSymbols[GL_LUMINANCE] = "GL_LUMINANCE";
		sSymbols[GL_LUMINANCE_ALPHA] = "GL_LUMINANCE_ALPHA";
#endif		

		sSymbols[GL_RED] = "GL_RED";
		sSymbols[GL_RG] = "GL_RG";
		sSymbols[GL_R8] = "GL_R8";
		sSymbols[GL_RG8] = "GL_RG8";
		sSymbols[GL_RGB8] = "GL_RGB8";
		sSymbols[GL_RGBA4] = "GL_RGBA4";
		sSymbols[GL_RGB5_A1] = "GL_RGB5_A1";
		sSymbols[GL_RGBA8] = "GL_RGBA8";
		sSymbols[GL_R16F] = "GL_R16F";
		sSymbols[GL_RG16F] = "GL_RG16F";
		sSymbols[GL_RGB16F] = "GL_RGB16F";
		sSymbols[GL_RGBA16F] = "GL_RGBA16F";
		sSymbols[GL_R32F] = "GL_R32F";
		sSymbols[GL_RG32F] = "GL_RG32F";
		sSymbols[GL_RGB32F] = "GL_RGB32F";
		sSymbols[GL_RGBA32F] = "GL_RGBA32F";
		
		sSymbols[GL_R11F_G11F_B10F] = "GL_R11F_G11F_B10F";
		sSymbols[GL_RGB9_E5] = "GL_RGB9_E5";
		
#if ! defined( CINDER_GL_ES_2 )
		sSymbols[GL_R8I] = "GL_R8I";
		sSymbols[GL_R8UI] = "GL_R8UI";
		sSymbols[GL_R16I] = "GL_R16I";
		sSymbols[GL_R16UI] = "GL_R16UI";
		sSymbols[GL_R32I] = "GL_R32I";
		sSymbols[GL_R32UI] = "GL_R32UI";
		sSymbols[GL_RG8I] = "GL_RG8I";
		sSymbols[GL_RG8UI] = "GL_RG8UI";
		sSymbols[GL_RG16I] = "GL_RG16I";
		sSymbols[GL_RG16UI] = "GL_RG16UI";
		sSymbols[GL_RG32I] = "GL_RG32I";
		sSymbols[GL_RG32UI] = "GL_RG32UI";
		sSymbols[GL_RGB8I] = "GL_RGB8I";
		sSymbols[GL_RGB8UI] = "GL_RGB8UI";
		sSymbols[GL_RGB16I] = "GL_RGB16I";
		sSymbols[GL_RGB16UI] = "GL_RGB16UI";
		sSymbols[GL_RGB32I] = "GL_RGB32I";
		sSymbols[GL_RGB32UI] = "GL_RGB32UI";
		sSymbols[GL_RGBA8I] = "GL_RGBA8I";
		sSymbols[GL_RGBA8UI] = "GL_RGBA8UI";
		sSymbols[GL_RGBA16I] = "GL_RGBA16I";
		sSymbols[GL_RGBA16UI] = "GL_RGBA16UI";
		sSymbols[GL_RGBA32I] = "GL_RGBA32I";
		sSymbols[GL_RGBA32UI] = "GL_RGBA32UI";
#endif // ! defined( CINDER_GL_ES_2 )

#if defined( CINDER_HAS_GL_SRGB )
		sSymbols[GL_SRGB8] = "GL_SRGB8";
		sSymbols[GL_SRGB8_ALPHA8] = "GL_SRGB8_ALPHA8";
#endif // defined( CINDER_HAS_GL_SRGB )

#if defined( CINDER_GL_HAS_REQUIRED_INTERNALFORMAT )
	#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX ) || defined( CINDER_EMSCRIPTEN )
		sSymbols[GL_RGB10] = "GL_RGB10";
	#endif
		sSymbols[GL_RGB10_A2] = "GL_RGB10_A2";

	#if ( ! defined( CINDER_GL_ES ) ) || ( defined( CINDER_GL_ES ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 ) )
		sSymbols[GL_RGB10_A2UI] = "GL_RGB10_A2UI";
	#endif		
#endif

#if defined( CINDER_GL_HAS_TEXTURE_NORM16 )
		sSymbols[GL_R16] = "GL_R16";
		sSymbols[GL_RG16] = "GL_RG16";
		sSymbols[GL_RGB16] = "GL_RGB16";
		sSymbols[GL_RGBA16] = "GL_RGBA16";
#endif

#if ! defined( CINDER_GL_ES )
		sSymbols[GL_R3_G3_B2] = "GL_R3_G3_B2";
		sSymbols[GL_RGB4] = "GL_RGB4";
		sSymbols[GL_RGB5] = "GL_RGB5";
		sSymbols[GL_RGB12] = "GL_RGB12";
#endif

#if defined( CINDER_GL_HAS_RENDER_SNORM )
		sSymbols[GL_R8_SNORM] = "GL_R8_SNORM";
		sSymbols[GL_RG8_SNORM] = "GL_RG8_SNORM";

	#if defined( CINDER_GL_DESKTOP ) || ( defined( CINDER_GL_ES ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 ) )
		sSymbols[GL_RGB8_SNORM] = "GL_RGB8_SNORM";
	#endif

		sSymbols[GL_RGBA8_SNORM] = "GL_RGBA8_SNORM";
  #if ! ( defined( CINDER_COCOA_TOUCH ) || defined( CINDER_GL_ANGLE ) )
		// iOS and ANGLE doesn't have these
		sSymbols[GL_R16_SNORM] = "GL_R16_SNORM";
		sSymbols[GL_RG16_SNORM] = "GL_RG16_SNORM";
		sSymbols[GL_RGB16_SNORM] = "GL_RGB16_SNORM";
  #endif
#endif
		
#if ! defined( CINDER_GL_ES )
		sSymbols[GL_RGBA2] = "GL_RGBA2";
		sSymbols[GL_RGBA12] = "GL_RGBA12";
		sSymbols[GL_RGBA16] = "GL_RGBA16";
#endif

		sSymbols[GL_DEPTH_COMPONENT] = "GL_DEPTH_COMPONENT";
		sSymbols[GL_DEPTH_COMPONENT16] = "GL_DEPTH_COMPONENT16";
		sSymbols[GL_DEPTH_COMPONENT24] = "GL_DEPTH_COMPONENT24";

		sSymbols[GL_FRAMEBUFFER] = "GL_FRAMEBUFFER";
		sSymbols[GL_DRAW_FRAMEBUFFER] = "GL_DRAW_FRAMEBUFFER";

		// framebuffer attachments
		sSymbols[GL_STENCIL_ATTACHMENT] = "GL_STENCIL_ATTACHMENT";
		sSymbols[GL_DEPTH_ATTACHMENT]   = "GL_DEPTH_ATTACHMENT";
		sSymbols[GL_COLOR_ATTACHMENT0]  = "GL_COLOR_ATTACHMENT0";
#if ! defined( CINDER_GL_ES_2 )
		sSymbols[GL_DEPTH_STENCIL_ATTACHMENT] = "GL_DEPTH_STENCIL_ATTACHMENT";
		sSymbols[GL_COLOR_ATTACHMENT1] = "GL_COLOR_ATTACHMENT1";
		sSymbols[GL_COLOR_ATTACHMENT2] = "GL_COLOR_ATTACHMENT2";
		sSymbols[GL_COLOR_ATTACHMENT3] = "GL_COLOR_ATTACHMENT3";
		sSymbols[GL_COLOR_ATTACHMENT4] = "GL_COLOR_ATTACHMENT4";
		sSymbols[GL_COLOR_ATTACHMENT5] = "GL_COLOR_ATTACHMENT5";
		sSymbols[GL_COLOR_ATTACHMENT6] = "GL_COLOR_ATTACHMENT6";
		sSymbols[GL_COLOR_ATTACHMENT7] = "GL_COLOR_ATTACHMENT7";
		sSymbols[GL_COLOR_ATTACHMENT8] = "GL_COLOR_ATTACHMENT8";
		sSymbols[GL_COLOR_ATTACHMENT9] = "GL_COLOR_ATTACHMENT9";
#endif // ! defined( CINDER_GL_ES_2 )
		
		initialized = true;
	}

/*	
	static bool initialized = false;
	static std::unordered_map<GLenum,std::string> sSymbols;
	if( ! initialized ) {
		// Types
		sSymbols[GL_BYTE] = "BYTE";
		sSymbols[GL_UNSIGNED_BYTE] = "UNSIGNED_BYTE";
		sSymbols[GL_SHORT] = "SHORT";
		sSymbols[GL_UNSIGNED_SHORT] = "UNSIGNED_SHORT";
		sSymbols[GL_INT] = "INT";
		sSymbols[GL_UNSIGNED_INT] = "UNSIGNED_INT";
		sSymbols[GL_FIXED] = "FIXED";
		sSymbols[GL_FLOAT] = "FLOAT";
		sSymbols[GL_FLOAT_VEC2] = "FLOAT_VEC2";
		sSymbols[GL_FLOAT_VEC3] = "FLOAT_VEC3";
		sSymbols[GL_FLOAT_VEC4] = "FLOAT_VEC4";
#if ! defined( CINDER_GL_ES_2 )
		sSymbols[GL_UNSIGNED_INT_VEC2] = "UNSIGNED_INT_VEC2";
		sSymbols[GL_UNSIGNED_INT_VEC3] = "UNSIGNED_INT_VEC3";
		sSymbols[GL_UNSIGNED_INT_VEC4] = "UNSIGNED_INT_VEC4";
		sSymbols[GL_INT_SAMPLER_2D] = "INT_SAMPLER_2D";
		sSymbols[GL_INT_SAMPLER_3D] = "INT_SAMPLER_3D";
		sSymbols[GL_INT_SAMPLER_CUBE] = "INT_SAMPLER_CUBE";
		sSymbols[GL_INT_SAMPLER_2D_ARRAY] = "GL_INT_SAMPLER_2D_ARRAY";
		sSymbols[GL_SAMPLER_3D] = "SAMPLER_3D";
		sSymbols[GL_UNSIGNED_INT_SAMPLER_2D] = "UNSIGNED_INT_SAMPLER_2D";
		sSymbols[GL_UNSIGNED_INT_SAMPLER_3D] = "UNSIGNED_INT_SAMPLER_3D";
		sSymbols[GL_UNSIGNED_INT_SAMPLER_2D_ARRAY] = "UNSIGNED_INT_SAMPLER_2D_ARRAY";
		sSymbols[GL_UNSIGNED_INT_SAMPLER_CUBE] = "UNSIGNED_INT_SAMPLER_CUBE";		
		sSymbols[GL_SAMPLER_2D_SHADOW] = "SAMPLER_2D_SHADOW";
		sSymbols[GL_SAMPLER_2D_ARRAY_SHADOW] = "SAMPLER_2D_ARRAY_SHADOW";
		sSymbols[GL_SAMPLER_CUBE_SHADOW] = "GL_SAMPLER_CUBE_SHADOW";
#endif
#if defined( CINDER_ANDROID )
		sSymbols[GL_SAMPLER_EXTERNAL_OES] = "GL_SAMPLER_EXTERNAL_OES";
#endif
		sSymbols[GL_INT_VEC2] = "INT_VEC2";
		sSymbols[GL_INT_VEC3] = "INT_VEC3";
		sSymbols[GL_INT_VEC4] = "INT_VEC4";
		sSymbols[GL_BOOL] = "BOOL";
		sSymbols[GL_BOOL_VEC2] = "BOOL_VEC2";
		sSymbols[GL_BOOL_VEC3] = "BOOL_VEC3";
		sSymbols[GL_BOOL_VEC4] = "BOOL_VEC4";
		sSymbols[GL_FLOAT_MAT2] = "FLOAT_MAT2";
		sSymbols[GL_FLOAT_MAT3] = "FLOAT_MAT3";
		sSymbols[GL_FLOAT_MAT4] = "FLOAT_MAT4";
		sSymbols[GL_SAMPLER_2D] = "SAMPLER_2D";
		sSymbols[GL_SAMPLER_CUBE] = "SAMPLER_CUBE";
#if ! defined( CINDER_GL_ES )
		sSymbols[GL_SAMPLER_BUFFER] = "SAMPLER_BUFFER";
		sSymbols[GL_UNSIGNED_INT_SAMPLER_BUFFER] = "UNSIGNED_INT_SAMPLER_BUFFER";
		sSymbols[GL_SAMPLER_1D] = "SAMPLER_1D";
		sSymbols[GL_INT_SAMPLER_2D_RECT] = "INT_SAMPLER_2D_RECT";
		sSymbols[GL_UNSIGNED_INT_SAMPLER_2D_RECT] = "UNSIGNED_INT_SAMPLER_2D_RECT";
		sSymbols[GL_SAMPLER_2D_RECT] = "SAMPLER_2D_RECT";
		sSymbols[GL_SAMPLER_1D_SHADOW] = "SAMPLER_1D_SHADOW";
		sSymbols[GL_HALF_FLOAT] = "HALF_FLOAT";
		sSymbols[GL_DOUBLE] = "DOUBLE";
		sSymbols[GL_INT_2_10_10_10_REV] = "INT_2_10_10_10_REV";
		sSymbols[GL_UNSIGNED_INT_2_10_10_10_REV] = "UNSIGNED_INT_2_10_10_10_REV";
#endif
		// Buffer bindings
		sSymbols[GL_ARRAY_BUFFER] = "GL_ARRAY_BUFFER";
		sSymbols[GL_ELEMENT_ARRAY_BUFFER] = "GL_ELEMENT_ARRAY_BUFFER";
#if ! defined( CINDER_GL_ES_2 )
		sSymbols[GL_PIXEL_PACK_BUFFER] = "GL_PIXEL_PACK_BUFFER";
		sSymbols[GL_PIXEL_UNPACK_BUFFER] = "GL_PIXEL_UNPACK_BUFFER";
		sSymbols[GL_COPY_READ_BUFFER] = "GL_COPY_READ_BUFFER";
		sSymbols[GL_COPY_WRITE_BUFFER] = "GL_COPY_WRITE_BUFFER";
		sSymbols[GL_TRANSFORM_FEEDBACK_BUFFER] = "GL_TRANSFORM_FEEDBACK_BUFFER";
		sSymbols[GL_UNIFORM_BUFFER] = "GL_UNIFORM_BUFFER";
#endif
#if ! defined( CINDER_GL_ES )
		sSymbols[GL_ATOMIC_COUNTER_BUFFER] = "GL_ATOMIC_COUNTER_BUFFER";
		sSymbols[GL_DRAW_INDIRECT_BUFFER] = "GL_DRAW_INDIRECT_BUFFER";
		sSymbols[GL_DISPATCH_INDIRECT_BUFFER] = "GL_DISPATCH_INDIRECT_BUFFER";
		sSymbols[GL_QUERY_BUFFER] = "GL_QUERY_BUFFER";
		sSymbols[GL_SHADER_STORAGE_BUFFER] = "GL_SHADER_STORAGE_BUFFER";
		sSymbols[GL_TEXTURE_BUFFER] = "GL_TEXTURE_BUFFER";
#endif
		// Buffer usage
		sSymbols[GL_STREAM_DRAW] = "GL_STREAM_DRAW";
		sSymbols[GL_STATIC_DRAW] = "GL_STATIC_DRAW";
		sSymbols[GL_DYNAMIC_DRAW] = "GL_DYNAMIC_DRAW";
#if ! defined( CINDER_GL_ES_2 )
		sSymbols[GL_STREAM_READ] = "GL_STREAM_READ";
		sSymbols[GL_STREAM_COPY] = "GL_STREAM_COPY";
		sSymbols[GL_STATIC_READ] = "GL_STATIC_READ";
		sSymbols[GL_STATIC_COPY] = "GL_STATIC_COPY";
		sSymbols[GL_DYNAMIC_READ] = "GL_DYNAMIC_READ";
		sSymbols[GL_DYNAMIC_COPY] = "GL_DYNAMIC_COPY";
#endif
		// Texture targets
		sSymbols[GL_TEXTURE_2D] = "GL_TEXTURE_2D";
		sSymbols[GL_TEXTURE_CUBE_MAP] = "GL_TEXTURE_CUBE_MAP";
#if ! defined( CINDER_GL_ES )
		sSymbols[GL_TEXTURE_1D] = "GL_TEXTURE_1D";
		sSymbols[GL_TEXTURE_3D] = "GL_TEXTURE_3D";
#endif
		// Texture internal formats
		sSymbols[GL_RGB] = "GL_RGB";
		sSymbols[GL_RGBA] = "GL_RGBA";
		sSymbols[GL_ALPHA] = "GL_ALPHA";
#if defined( CINDER_GL_ES ) && ( CINDER_GL_ES_VERSION <= CINDER_GL_ES_VERSION_2 )
		sSymbols[GL_LUMINANCE] = "GL_LUMINANCE";
		sSymbols[GL_LUMINANCE_ALPHA] = "GL_LUMINANCE_ALPHA";
#else
		sSymbols[GL_RED] = "GL_RED";
		sSymbols[GL_RG] = "GL_RG";
		sSymbols[GL_R8] = "GL_R8";
		sSymbols[GL_R8_SNORM] = "GL_R8_SNORM";
		sSymbols[GL_R16] = "GL_R16";
		sSymbols[GL_R16_SNORM] = "GL_R16_SNORM";
		sSymbols[GL_RG8] = "GL_RG8";
		sSymbols[GL_RG8_SNORM] = "GL_RG8_SNORM";
		sSymbols[GL_RG16] = "GL_RG16";
		sSymbols[GL_RG16_SNORM] = "GL_RG16_SNORM";
		sSymbols[GL_R3_G3_B2] = "GL_R3_G3_B2";
		sSymbols[GL_RGB4] = "GL_RGB4";
		sSymbols[GL_RGB5] = "GL_RGB5";
		sSymbols[GL_RGB8] = "GL_RGB8";
		sSymbols[GL_RGB8_SNORM] = "GL_RGB8_SNORM";
		sSymbols[GL_RGB10] = "GL_RGB10";
		sSymbols[GL_RGB12] = "GL_RGB12";
		sSymbols[GL_RGB16_SNORM] = "GL_RGB16_SNORM";
		sSymbols[GL_RGBA2] = "GL_RGBA2";
		sSymbols[GL_RGBA4] = "GL_RGBA4";
		sSymbols[GL_RGB5_A1] = "GL_RGB5_A1";
		sSymbols[GL_RGBA8] = "GL_RGBA8";
		sSymbols[GL_RGBA8_SNORM] = "GL_RGBA8_SNORM";
		sSymbols[GL_RGB10_A2] = "GL_RGB10_A2";
		sSymbols[GL_RGB10_A2UI] = "GL_RGB10_A2UI";
		sSymbols[GL_RGBA12] = "GL_RGBA12";
		sSymbols[GL_RGBA16] = "GL_RGBA16";
		sSymbols[GL_SRGB8] = "GL_SRGB8";
		sSymbols[GL_SRGB8_ALPHA8] = "GL_SRGB8_ALPHA8";
		sSymbols[GL_R16F] = "GL_R16F";
		sSymbols[GL_RG16F] = "GL_RG16F";
		sSymbols[GL_RGB16F] = "GL_RGB16F";
		sSymbols[GL_RGBA16F] = "GL_RGBA16F";
		sSymbols[GL_R32F] = "GL_R32F";
		sSymbols[GL_RG32F] = "GL_RG32F";
		sSymbols[GL_RGB32F] = "GL_RGB32F";
		sSymbols[GL_RGBA32F] = "GL_RGBA32F";
		sSymbols[GL_R11F_G11F_B10F] = "GL_R11F_G11F_B10F";
		sSymbols[GL_RGB9_E5] = "GL_RGB9_E5";
		sSymbols[GL_R8I] = "GL_R8I";
		sSymbols[GL_R8UI] = "GL_R8UI";
		sSymbols[GL_R16I] = "GL_R16I";
		sSymbols[GL_R16UI] = "GL_R16UI";
		sSymbols[GL_R32I] = "GL_R32I";
		sSymbols[GL_R32UI] = "GL_R32UI";
		sSymbols[GL_RG8I] = "GL_RG8I";
		sSymbols[GL_RG8UI] = "GL_RG8UI";
		sSymbols[GL_RG16I] = "GL_RG16I";
		sSymbols[GL_RG16UI] = "GL_RG16UI";
		sSymbols[GL_RG32I] = "GL_RG32I";
		sSymbols[GL_RG32UI] = "GL_RG32UI";
		sSymbols[GL_RGB8I] = "GL_RGB8I";
		sSymbols[GL_RGB8UI] = "GL_RGB8UI";
		sSymbols[GL_RGB16I] = "GL_RGB16I";
		sSymbols[GL_RGB16UI] = "GL_RGB16UI";
		sSymbols[GL_RGB32I] = "GL_RGB32I";
		sSymbols[GL_RGB32UI] = "GL_RGB32UI";
		sSymbols[GL_RGBA8I] = "GL_RGBA8I";
		sSymbols[GL_RGBA8UI] = "GL_RGBA8UI";
		sSymbols[GL_RGBA16I] = "GL_RGBA16I";
		sSymbols[GL_RGBA16UI] = "GL_RGBA16UI";
		sSymbols[GL_RGBA32I] = "GL_RGBA32I";
		sSymbols[GL_RGBA32UI] = "GL_RGBA32UI";
#endif // ! defined( CINDER_GL_ES )
		// framebuffer attachments
		sSymbols[GL_STENCIL_ATTACHMENT] = "GL_STENCIL_ATTACHMENT";
		sSymbols[GL_DEPTH_ATTACHMENT] = "GL_DEPTH_ATTACHMENT";
		sSymbols[GL_COLOR_ATTACHMENT0] = "GL_COLOR_ATTACHMENT0";
#if ! defined( CINDER_GL_ES_2 )
		sSymbols[GL_DEPTH_STENCIL_ATTACHMENT] = "GL_DEPTH_STENCIL_ATTACHMENT";
		sSymbols[GL_COLOR_ATTACHMENT1] = "GL_COLOR_ATTACHMENT1";
		sSymbols[GL_COLOR_ATTACHMENT2] = "GL_COLOR_ATTACHMENT2";
		sSymbols[GL_COLOR_ATTACHMENT3] = "GL_COLOR_ATTACHMENT3";
		sSymbols[GL_COLOR_ATTACHMENT4] = "GL_COLOR_ATTACHMENT4";
		sSymbols[GL_COLOR_ATTACHMENT5] = "GL_COLOR_ATTACHMENT5";
		sSymbols[GL_COLOR_ATTACHMENT6] = "GL_COLOR_ATTACHMENT6";
		sSymbols[GL_COLOR_ATTACHMENT7] = "GL_COLOR_ATTACHMENT7";
		sSymbols[GL_COLOR_ATTACHMENT8] = "GL_COLOR_ATTACHMENT8";
		sSymbols[GL_COLOR_ATTACHMENT9] = "GL_COLOR_ATTACHMENT9";
#endif // ! defined( CINDER_GL_ES_2 )
		
		initialized = true;
	}
*/	
	
	auto it = sSymbols.find( constant );
	if( it != sSymbols.end() )
		return it->second;
	else {
		// return constant as a hex integer
		std::stringstream ss;
		ss << "0x" << std::hex << constant;
		return ss.str();
	}
}
	
uint8_t typeToDimension( GLenum type )
{
	switch (type) {
		case GL_UNSIGNED_INT:
		case GL_INT:
		case GL_FLOAT:
		case GL_BOOL:
			return 1;
		break;
#if ! defined( CINDER_GL_ES_2 )
		case GL_UNSIGNED_INT_VEC2:
#endif
		case GL_INT_VEC2:
		case GL_FLOAT_VEC2:
		case GL_BOOL_VEC2:
			return 2;
		break;
#if ! defined( CINDER_GL_ES_2 )
		case GL_UNSIGNED_INT_VEC3:
#endif
		case GL_INT_VEC3:
		case GL_FLOAT_VEC3:
		case GL_BOOL_VEC3:
			return 3;
		break;
#if ! defined( CINDER_GL_ES_2 )
		case GL_UNSIGNED_INT_VEC4:
#endif
		case GL_INT_VEC4:
		case GL_FLOAT_VEC4:
		case GL_BOOL_VEC4:
		case GL_FLOAT_MAT2:
			return 4;
#if ! defined( CINDER_GL_ES_2 )
		case GL_FLOAT_MAT2x3:
		case GL_FLOAT_MAT3x2:
			return 6;
		case GL_FLOAT_MAT4x2:
		case GL_FLOAT_MAT2x4:
			return 8;
#endif
		case GL_FLOAT_MAT3:
			return 9;
#if ! defined( CINDER_GL_ES_2 )
		case GL_FLOAT_MAT3x4:
		case GL_FLOAT_MAT4x3:
			return 12;
#endif
		case GL_FLOAT_MAT4:
			return 16;
		default:
			return 0;
		break;
	}
}
	
uint8_t typeToBytes( GLenum type )
{
	switch( type ) {
		case GL_UNSIGNED_INT:		return sizeof(uint32_t); break;
		case GL_INT:				return sizeof(int); break;
		case GL_SAMPLER_2D:			return sizeof(int); break;

#if ! defined( CINDER_GL_ES )
		case GL_SAMPLER_1D:						return sizeof(int); break;
		case GL_SAMPLER_BUFFER_EXT:				return sizeof(int); break;
		case GL_UNSIGNED_INT_SAMPLER_BUFFER:	return sizeof(int); break;
		case GL_SAMPLER_2D_RECT:				return sizeof(int); break;
		case GL_INT_SAMPLER_2D_RECT:			return sizeof(int); break;
		case GL_UNSIGNED_INT_SAMPLER_2D_RECT:	return sizeof(int); break;
#endif

#if ! defined( CINDER_GL_ES_2 )
		case GL_SAMPLER_2D_ARRAY:				return sizeof(int); break;
		case GL_SAMPLER_2D_SHADOW:				return sizeof(int); break;
		case GL_SAMPLER_2D_ARRAY_SHADOW:		return sizeof(int); break;
		case GL_SAMPLER_CUBE_SHADOW:			return sizeof(int); break;						
		case GL_INT_SAMPLER_2D:					return sizeof(int); break;
		case GL_INT_SAMPLER_3D:					return sizeof(int); break;
		case GL_INT_SAMPLER_CUBE:				return sizeof(int); break;
		case GL_INT_SAMPLER_2D_ARRAY:			return sizeof(int); break;		
		case GL_SAMPLER_3D:						return sizeof(int); break;
		case GL_UNSIGNED_INT_SAMPLER_2D:		return sizeof(int); break;
		case GL_UNSIGNED_INT_SAMPLER_3D:		return sizeof(int); break;
		case GL_UNSIGNED_INT_SAMPLER_CUBE:		return sizeof(int); break;		
		case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:	return sizeof(int); break;
#else
	#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX ) || defined( CINDER_EMSCRIPTEN )
	  #if defined( CINDER_GL_HAS_SHADOW_SAMPLERS )
		case GL_SAMPLER_2D_SHADOW_EXT: return sizeof(int); break;
	  #endif
	#else
		case GL_SAMPLER_2D_SHADOW_EXT: return sizeof(int); break;
	#endif
#endif

#if defined( CINDER_ANDROID )
		case GL_SAMPLER_EXTERNAL_OES: return sizeof(int); break;
#endif		

		case GL_SAMPLER_CUBE:		return sizeof(int); break;
		case GL_FLOAT:				return sizeof(float); break;
		case GL_BOOL:				return sizeof(bool); break;

#if ! defined( CINDER_GL_ES_2 )
		case GL_UNSIGNED_INT_VEC2:	return sizeof(glm::uvec2); break;
		case GL_UNSIGNED_INT_VEC3:	return sizeof(glm::uvec3); break;
		case GL_UNSIGNED_INT_VEC4:	return sizeof(glm::uvec4); break;
#endif
		
		case GL_INT_VEC2:			return sizeof(ivec2); break;
		case GL_FLOAT_VEC2:			return sizeof(vec2); break;
		case GL_BOOL_VEC2:			return sizeof(glm::bvec2); break;
		case GL_INT_VEC3:			return sizeof(ivec3); break;
		case GL_FLOAT_VEC3:			return sizeof(vec3); break;
		case GL_BOOL_VEC3:			return sizeof(glm::bvec3); break;
		case GL_INT_VEC4:			return sizeof(ivec4); break;
		case GL_FLOAT_VEC4:			return sizeof(vec4); break;
		case GL_BOOL_VEC4:			return sizeof(glm::bvec4); break;
		case GL_FLOAT_MAT2:			return sizeof(mat2); break;
		case GL_FLOAT_MAT3:			return sizeof(mat3); break;
		case GL_FLOAT_MAT4:			return sizeof(mat4); break;
		default:
			CI_LOG_E("Unknown gl type constant " << constantToString( type ));
			return 0;
		break;
	}
}

} } // namespace cinder::gl

#if defined( CINDER_GL_DESKTOP )
	#undef CINDER_GL_DESKTOP
#endif