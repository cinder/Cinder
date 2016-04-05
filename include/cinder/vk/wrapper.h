/*
 Copyright 2016 Google Inc.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.


 Copyright (c) 2016, The Cinder Project, All rights reserved.

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

#include "cinder/vk/platform.h"
#include "cinder/GeomIo.h"

namespace cinder { namespace vk {

class ShaderProg;
using ShaderProgRef = std::shared_ptr<ShaderProg>;

enum class DataType {
	UNDEFINED = 0,
	INT8,
	UINT8,
	INT16,
	UINT16,
	INT32,
	UINT32,
	FLOAT16,
	FLOAT32,
	FLOAT64,
};

//! Converts a geom::Primitive to an OpenGL primitive mode( VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, etc )
VkPrimitiveTopology	toVk( geom::Primitive prim );
//! Converts an Vulkan primitive mode( VK_PRIMITIVE_TOPOLOGY_LINE_LIST, VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, etc ) to a geom::Primitive
geom::Primitive		toGeomPrimitive( VkPrimitiveTopology prim );
//! Converts info into a matching VKFormat
VkFormat			toVkFormat( const geom::AttribInfo& info );

VkImageAspectFlags	determineAspectMask( VkFormat format );
VkFormat			determineCompressedFormat( int glFormatConstant );
bool				isCompressedFormat( VkFormat format );

size_t				dataTypeSizeBytes( vk::DataType dataType );
size_t				formatNumChannels( VkFormat format );
vk::DataType		formatDataType( VkFormat format );
size_t				formatSizeBytes( VkFormat format );

template <typename T>
VkFormat findBestVkFormat( size_t numComponents ) 
{ 
	return VK_FORMAT_UNDEFINED; 
}

template <>
inline VkFormat findBestVkFormat<uint8_t>( size_t numComponents )
{
	VkFormat result = VK_FORMAT_UNDEFINED;
	switch( numComponents ) {
		case 1: result = VK_FORMAT_R8_UNORM; break;
		case 2: result = VK_FORMAT_R8G8_UNORM; break;
		case 3: result = VK_FORMAT_R8G8B8_UNORM; break;
		case 4: result = VK_FORMAT_R8G8B8A8_UNORM; break;
	}
	return result;
}

template <>
inline VkFormat findBestVkFormat<uint16_t>( size_t numComponents )
{
	VkFormat result = VK_FORMAT_UNDEFINED;
	switch( numComponents ) {
		case 1: result = VK_FORMAT_R16_UNORM; break;
		case 2: result = VK_FORMAT_R16G16_UNORM; break;
		case 3: result = VK_FORMAT_R16G16B16_UNORM; break;
		case 4: result = VK_FORMAT_R16G16B16A16_UNORM; break;
	}
	return result;
}

template <>
inline VkFormat findBestVkFormat<float>( size_t numComponents )
{
	VkFormat result = VK_FORMAT_UNDEFINED;
	switch( numComponents ) {
		case 1: result = VK_FORMAT_R32_SFLOAT; break;
		case 2: result = VK_FORMAT_R32G32_SFLOAT; break;
		case 3: result = VK_FORMAT_R32G32B32_SFLOAT; break;
		case 4: result = VK_FORMAT_R32G32B32A32_SFLOAT; break;
	}
	return result;
}


//! Context
class Context* context();

ShaderProgRef& getStockShader( const class ShaderDef &shader );

void clear( bool color = true, bool depthStencil = true );

//! Viewport
std::pair<ivec2, ivec2> getViewport();
void viewport( const std::pair<ivec2, ivec2> positionAndSize );
inline void viewport( int x, int y, int width, int height ) { viewport( std::pair<ivec2, ivec2>( ivec2( x, y ), ivec2( width, height ) ) ); }
inline void viewport( const ivec2 &position, const ivec2 &size ) { viewport( std::pair<ivec2, ivec2>( position, size ) ); }
inline void viewport( const ivec2 &size ) { viewport( ivec2(), size ); }
void pushViewport( const std::pair<ivec2, ivec2> positionAndSize );
inline void pushViewport() { pushViewport( getViewport() ); }
inline void pushViewport( int x, int y, int width, int height ) { pushViewport( std::pair<ivec2, ivec2>( ivec2( x, y ), ivec2( width, height ) ) ); }
inline void pushViewport( const ivec2 &position, const ivec2 &size ) { pushViewport( std::pair<ivec2, ivec2>( position, size ) ); }
inline void pushViewport( const ivec2 &size ) { pushViewport( ivec2(), size ); }
void popViewport();

//! Enables or disables blending state as governed by \c GL_BLEND but does not modify blend function.
void enableBlending( bool enable = false );
//! Disables blending state via \c GL_BLEND, but does not modify blend function
inline void disableBlending() { enableBlending( false ); }
//! Enables blending via \c GL_BLEND and sets the blend function to unpremultiplied alpha blending when \p enable is \c true; otherwise disables blending without modifying the blend function.
void enableAlphaBlending( bool enable = true );
//! Enables blending via \c GL_BLEND and sets the blend function to premultiplied alpha blending
void enableAlphaBlendingPremult();
//! Disables blending state as governed by \c GL_BLEND but does not modify blend function.. Deprecated; prefer disableBlending()
inline void disableAlphaBlending() { disableBlending(); }
//! Enables \c GL_BLEND and sets the blend function to additive blending
void enableAdditiveBlending();

//! Specifies whether front or back-facing polygons are culled (as specified by \a face) when polygon culling is enabled. Valid values are \c GL_BACK and \c GL_FRONT.
void cullMode( VkCullModeFlagBits mode );

//! Disables reading / testing from the depth buffer. Disables \c GL_DEPTH_TEST
void disableDepthRead();
//! Disables writing to depth buffer; analogous to calling glDepthMask( GL_FALSE );
void disableDepthWrite();
//! Enables or disables reading / testing from depth buffer; analogous to setting \c GL_DEPTH_TEST to \p enable
void enableDepthRead( bool enable = true );
//! Enables or disables writing to depth buffer; analogous to calling glDepthMask( \p enable ); Note that reading must also be enabled for writing to have any effect.
void enableDepthWrite( bool enable = true );
//! Enables or disables writing to and reading / testing from depth buffer
inline void enableDepth( bool enable = true ) { enableDepthRead( enable ); enableDepthWrite( enable ); }

void enableDepthBias( bool enable = true );
inline void disableDepthBias() { enableDepthBias( false ); }
void depthBias( float slopeFactor, float constantFactor, float clamp = 0.0f );
inline void enablePolygonOffsetFill( bool enable = true ) { enableDepthBias( enable ); }
inline void disablePolygonOffsetFill() { disableDepthBias(); }
inline void polygonOffset( float factor, float units, float clamp = 0.0f ) { depthBias( factor, units, clamp ); }

//! Sets the View and Projection matrices based on a Camera
void setMatrices( const ci::Camera &cam );
void setModelMatrix( const ci::mat4 &m );
void setViewMatrix( const ci::mat4 &m );
void setProjectionMatrix( const ci::mat4 &m );
void pushModelMatrix();
void popModelMatrix();
void pushViewMatrix();
void popViewMatrix();
void pushProjectionMatrix();
void popProjectionMatrix();
//! Pushes Model and View matrices
void pushModelView();
//! Pops Model and View matrices
void popModelView();
//! Pushes Model, View and Projection matrices
void pushMatrices();
//! Pops Model, View and Projection matrices
void popMatrices();
void multModelMatrix( const ci::mat4 &mtx );
void multViewMatrix( const ci::mat4 &mtx );
void multProjectionMatrix( const ci::mat4 &mtx );

mat4 getModelMatrix();
mat4 getViewMatrix();
mat4 getProjectionMatrix();
mat4 getModelView();
mat4 getModelViewProjection();
mat4 calcViewMatrixInverse();

// NOTE: Unlike the OpenGL version, the Vulkan version of these two functions return a mat4. 
//       This is due to an alignment/padding issue in the current shader compiler that
//       doesn't like 3x3 matrices in uniforms.
mat4 calcModelMatrixInverseTranspose();
mat3 calcNormalMatrix();

mat4 calcViewportMatrix();

// NOTE: Vulkan coordinate system has Positive-Y as down. Pass in false to originUpperLeft 
//       to emulate OpenGL's coordinate system.
void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true );
void setMatricesWindowPersp( const ci::ivec2 &screenSize, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true );
void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft = true );
void setMatricesWindow( const ci::ivec2 &screenSize, bool originUpperLeft = true );

void rotate( const quat &quat );
//! Rotates the Model matrix by \a angleRadians around the \a axis
void rotate( float angleRadians, const ci::vec3 &axis );
//! Rotates the Model matrix by \a angleRadians around the axis (\a x,\a y,\a z)
inline void rotate( float angleRadians, float xAxis, float yAxis, float zAxis ) { rotate( angleRadians, ci::vec3(xAxis, yAxis, zAxis) ); }
//! Rotates the Model matrix by \a zRadians around the z-axis
inline void rotate( float zRadians ) { rotate( zRadians, vec3( 0, 0, 1 ) ); }

//! Scales the Model matrix by \a v
void scale( const ci::vec3 &v );
//! Scales the Model matrix by (\a x,\a y, \a z)
inline void scale( float x, float y, float z ) { scale( vec3( x, y, z ) ); }
//! Scales the Model matrix by \a v
inline void scale( const ci::vec2 &v ) { scale( vec3( v.x, v.y, 1 ) ); }
//! Scales the Model matrix by (\a x,\a y, 1)
inline void scale( float x, float y ) { scale( vec3( x, y, 1 ) ); }

//! Translates the Model matrix by \a v
void translate( const ci::vec3 &v );
//! Translates the Model matrix by (\a x,\a y,\a z )
inline void translate( float x, float y, float z ) { translate( vec3( x, y, z ) ); }
//! Translates the Model matrix by \a v
inline void translate( const ci::vec2 &v ) { translate( vec3( v, 0 ) ); }
//! Translates the Model matrix by (\a x,\a y)
inline void translate( float x, float y ) { translate( vec3( x, y, 0 ) ); }

}} // namespace cinder::vk