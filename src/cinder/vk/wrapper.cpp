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

#include "cinder/vk/wrapper.h"
#include "cinder/vk/Context.h"
#include "cinder/GeomIo.h"

namespace cinder { namespace vk {

#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT			0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT			0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT			0x83F3
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT				0x83F0

#define GL_COMPRESSED_SRGB_EXT						0x8C48
#define GL_COMPRESSED_SRGB_ALPHA_EXT				0x8C49
#define GL_COMPRESSED_SLUMINANCE_EXT				0x8C4A
#define GL_COMPRESSED_SLUMINANCE_ALPHA_EXT			0x8C4B
#define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT			0x8C4C   
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT		0x8C4D
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT		0x8C4E
#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT		0x8C4F

#define GL_COMPRESSED_RED_RGTC1						0x8DBB
#define GL_COMPRESSED_RG_RGTC2						0x8DBD
#define GL_COMPRESSED_SIGNED_RED_RGTC1				0x8DBC
#define GL_COMPRESSED_SIGNED_RG_RGTC2				0x8DBE     

#define GL_COMPRESSED_RGBA_BPTC_UNORM_ARB			0x8E8C
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB		0x8E8D  
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB		0x8E8E
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB	0x8E8F

#define GL_COMPRESSED_RGBA_ASTC_4x4_KHR				0x93B0
#define GL_COMPRESSED_RGBA_ASTC_5x4_KHR				0x93B1
#define GL_COMPRESSED_RGBA_ASTC_5x5_KHR				0x93B2
#define GL_COMPRESSED_RGBA_ASTC_6x5_KHR				0x93B3
#define GL_COMPRESSED_RGBA_ASTC_6x6_KHR				0x93B4
#define GL_COMPRESSED_RGBA_ASTC_8x5_KHR				0x93B5
#define GL_COMPRESSED_RGBA_ASTC_8x6_KHR				0x93B6
#define GL_COMPRESSED_RGBA_ASTC_8x8_KHR				0x93B7
#define GL_COMPRESSED_RGBA_ASTC_10x5_KHR			0x93B8
#define GL_COMPRESSED_RGBA_ASTC_10x6_KHR			0x93B9
#define GL_COMPRESSED_RGBA_ASTC_10x8_KHR			0x93BA
#define GL_COMPRESSED_RGBA_ASTC_10x10_KHR			0x93BB
#define GL_COMPRESSED_RGBA_ASTC_12x10_KHR			0x93BC
#define GL_COMPRESSED_RGBA_ASTC_12x12_KHR			0x93BD

#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR		0x93D0
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR		0x93D1
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR		0x93D2
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR		0x93D3
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR		0x93D4
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR		0x93D5
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR		0x93D6
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR		0x93D
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR	0x93D8
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR	0x93D9
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR	0x93DA
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR	0x93DB
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR	0x93DC
#define GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR	0x93DD


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Context
class Context* context()
{
	return Context::getCurrent();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// toVk conversion functions
VkPrimitiveTopology toVk( geom::Primitive prim )
{
	switch( prim ) {
		case geom::Primitive::LINES:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		break;
		case geom::Primitive::LINE_STRIP:
			return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		break;
		case geom::Primitive::TRIANGLES:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;
		case geom::Primitive::TRIANGLE_STRIP:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		break;
		case geom::Primitive::TRIANGLE_FAN:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		default:
			return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM; // no clear right choice here
	}
}

geom::Primitive toGeomPrimitive( VkPrimitiveTopology prim )
{
	switch( prim ) {
		case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
			return geom::Primitive::LINES;
		break;
		case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
			return geom::Primitive::LINE_STRIP;
		break;
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
			return geom::Primitive::TRIANGLES;
		break;
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
			return geom::Primitive::TRIANGLE_STRIP;
		break;
		case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
			return geom::Primitive::TRIANGLE_FAN;
		default:
			return geom::Primitive( 65535 ); // no clear right choice here
	}
}

VkFormat toVkFormat( const geom::AttribInfo& info )
{
	VkFormat result = VK_FORMAT_UNDEFINED;

	switch( info.getDataType() ) {
		case geom::DataType::INTEGER: {
			switch( info.getDims() ) {
				case 1: result = VK_FORMAT_R32_UINT; break;
				case 2: result = VK_FORMAT_R32G32_UINT; break;
				case 3: result = VK_FORMAT_R32G32B32_UINT; break;
				case 4: result = VK_FORMAT_R32G32B32A32_UINT; break;
			}
		}
		break;

		case geom::DataType::FLOAT: {
			switch( info.getDims() ) {
				case 1: result = VK_FORMAT_R32_SFLOAT; break;
				case 2: result = VK_FORMAT_R32G32_SFLOAT; break;
				case 3: result = VK_FORMAT_R32G32B32_SFLOAT; break;
				case 4: result = VK_FORMAT_R32G32B32A32_SFLOAT; break;
			}
		}
		break;
	}

	return result;
}

VkImageAspectFlags determineAspectMask( VkFormat format )
{
	VkImageAspectFlags result = 0;

	switch( format ) {
		// Depth
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_X8_D24_UNORM_PACK32:
		case VK_FORMAT_D32_SFLOAT:
		{
			result = VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		break;

		// Stencil
		case VK_FORMAT_S8_UINT:
		{
			result = VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		break;

		// Depth/Stencil
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
		{
			result = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		break;

		// Assume everything else is Color
		default:
		{
			result = VK_IMAGE_ASPECT_COLOR_BIT;
		}
		break;
	}

	return result;
}

VkFormat determineCompressedFormat( int glFormatConstant )
{
	VkFormat result = VK_FORMAT_UNDEFINED;
	switch( glFormatConstant ) {
		case  GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			break;
		case  GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			break;
		case  GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			result = VK_FORMAT_BC3_UNORM_BLOCK;
			break;
		case  GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			break;
		case  GL_COMPRESSED_SRGB_EXT:
			break;
		case  GL_COMPRESSED_SRGB_ALPHA_EXT:
			break;
		case  GL_COMPRESSED_SLUMINANCE_EXT:
			break;
		case  GL_COMPRESSED_SLUMINANCE_ALPHA_EXT:
			break;
		case  GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
			break;
		case  GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
			break;
		case  GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
			break;
		case  GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
			break;
		case  GL_COMPRESSED_RED_RGTC1:
			break;
		case  GL_COMPRESSED_RG_RGTC2:
			break;
		case  GL_COMPRESSED_SIGNED_RED_RGTC1:
			break;
		case  GL_COMPRESSED_SIGNED_RG_RGTC2:
			break;
		case  GL_COMPRESSED_RGBA_BPTC_UNORM_ARB:
			break;
		case  GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB:
			break;
		case  GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB:
			break;
		case  GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
			break;
		case  GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
			break;
		case  GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
			break;
	}

	return result;
}

bool isCompressedFormat( VkFormat format )
{
	bool result = false;
	switch( format ) {
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
		case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
		case VK_FORMAT_BC2_UNORM_BLOCK:
		case VK_FORMAT_BC2_SRGB_BLOCK:
		case VK_FORMAT_BC3_UNORM_BLOCK:
		case VK_FORMAT_BC3_SRGB_BLOCK:
		case VK_FORMAT_BC4_UNORM_BLOCK:
		case VK_FORMAT_BC4_SNORM_BLOCK:
		case VK_FORMAT_BC5_UNORM_BLOCK:
		case VK_FORMAT_BC5_SNORM_BLOCK:
		case VK_FORMAT_BC6H_UFLOAT_BLOCK:
		case VK_FORMAT_BC6H_SFLOAT_BLOCK:
		case VK_FORMAT_BC7_UNORM_BLOCK:
		case VK_FORMAT_BC7_SRGB_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
		case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
		case VK_FORMAT_EAC_R11_UNORM_BLOCK:
		case VK_FORMAT_EAC_R11_SNORM_BLOCK:
		case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
		case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
		case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
		case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
		case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
		case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
		case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
		case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
		case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
		case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
		case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
		case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
		case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
		case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
		case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
		case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
		case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
		case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
		case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
		case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
		case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
		case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
		case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
		case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
			result = true;
			break;
	}
	return result;
}

size_t dataTypeSizeBytes( vk::DataType dataType )
{
	size_t result = 0;
	switch( dataType ) {
		case vk::DataType::INT8    : result = 1;  break;
		case vk::DataType::UINT8   : result = 1;  break;
		case vk::DataType::INT16   : result = 2;  break;
		case vk::DataType::UINT16  : result = 2;  break;
		case vk::DataType::INT32   : result = 4;  break;
		case vk::DataType::UINT32  : result = 4;  break;
		case vk::DataType::FLOAT16 : result = 2;  break;
		case vk::DataType::FLOAT32 : result = 4;  break;
		case vk::DataType::FLOAT64 : result = 8;  break;
	}
	return result;
}

size_t formatNumChannels( VkFormat format )
{
	size_t result = 0;
	switch( format ) {
		case VK_FORMAT_R8_UNORM:	
		case VK_FORMAT_R16_UNORM:
		case VK_FORMAT_R16_SFLOAT:
		case VK_FORMAT_R32_SFLOAT:
			result = 1;
			break;

		case VK_FORMAT_R8G8_UNORM:	
		case VK_FORMAT_R16G16_UNORM:
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_R32G32_SFLOAT:
			result = 2;
			break;

		case VK_FORMAT_R8G8B8_UNORM:	
		case VK_FORMAT_R16G16B16_UNORM:
		case VK_FORMAT_R16G16B16_SFLOAT:
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			result = 3;
			break;

		case VK_FORMAT_R8G8B8A8_UNORM:
		case VK_FORMAT_R16G16B16A16_UNORM:
		case VK_FORMAT_R32G32B32_SFLOAT:
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			result = 4;
			break;
	}
	return result;
}

vk::DataType formatDataType( VkFormat format )
{
	vk::DataType result = vk::DataType::UNDEFINED;
	switch( format ) {
		case VK_FORMAT_R8_UNORM:	
		case VK_FORMAT_R8G8_UNORM:	
		case VK_FORMAT_R8G8B8_UNORM:	
		case VK_FORMAT_R8G8B8A8_UNORM:
			result = vk::DataType::UINT8;
			break;

		case VK_FORMAT_R16_UNORM:
		case VK_FORMAT_R16G16_UNORM:
		case VK_FORMAT_R16G16B16_UNORM:
		case VK_FORMAT_R16G16B16A16_UNORM:
			result = vk::DataType::UINT16;
			break;

		case VK_FORMAT_R16_SFLOAT:
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_R16G16B16_SFLOAT:
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			result = vk::DataType::FLOAT16;

			break;

		case VK_FORMAT_R32_SFLOAT:
		case VK_FORMAT_R32G32_SFLOAT:
		case VK_FORMAT_R32G32B32_SFLOAT:
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			result = vk::DataType::FLOAT32;
			break;
	}
	return result;
}

size_t formatSizeBytes( VkFormat format )
{
	return dataTypeSizeBytes( formatDataType( format ) ) * formatNumChannels( format );
}

std::pair<ivec2, ivec2> getViewport()
{
	auto ctx = vk::context();
	auto view = ctx->getViewport();
	return view;
}

ShaderProgRef& getStockShader( const class ShaderDef &shader )
{
	return vk::context()->getStockShader( shader );
}

void clear( bool color, bool depthStencil )
{
	vk::context()->clearAttachments( color, depthStencil );
}

void viewport( const std::pair<ivec2, ivec2> positionAndSize )
{
	auto ctx = vk::context();
	ctx->viewport( positionAndSize );
}

void pushViewport( const std::pair<ivec2, ivec2> positionAndSize )
{
	auto ctx = vk::context();
	ctx->pushViewport( positionAndSize );
}

void popViewport()
{
	auto ctx = vk::context();
	ctx->popViewport();
}

void enableBlending( bool enable )
{
	auto ctx = vk::context();
	VkPipelineColorBlendAttachmentState colorBlend = ctx->getCachedColorAttachmentBlend();
	colorBlend.blendEnable = enable ? VK_TRUE : VK_FALSE;
	ctx->setCachedColorAttachmentBlend( colorBlend );	
}

void enableAlphaBlending( bool enable )
{
	auto ctx = vk::context();
	//ctx->enable( GL_BLEND );
	//if( enable ) {
	//	ctx->blendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	//}

	VkPipelineColorBlendAttachmentState colorBlend = {};
	colorBlend.blendEnable			= enable ? VK_TRUE : VK_FALSE;
	colorBlend.srcColorBlendFactor	= VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlend.dstColorBlendFactor	= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlend.colorBlendOp			= VK_BLEND_OP_ADD;
	colorBlend.srcAlphaBlendFactor	= VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlend.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlend.alphaBlendOp			= VK_BLEND_OP_ADD;
	colorBlend.colorWriteMask		= 0xf;
	ctx->setCachedColorAttachmentBlend( colorBlend );	
}

void enableAlphaBlendingPremult()
{
	auto ctx = vk::context();
	//ctx->enable( GL_BLEND );
	//ctx->blendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

	VkPipelineColorBlendAttachmentState colorBlend = {};
	colorBlend.blendEnable			= VK_TRUE;
	colorBlend.srcColorBlendFactor	= VK_BLEND_FACTOR_ONE;
	colorBlend.dstColorBlendFactor	= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlend.colorBlendOp			= VK_BLEND_OP_ADD;
	colorBlend.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE;
	colorBlend.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlend.alphaBlendOp			= VK_BLEND_OP_ADD;
	colorBlend.colorWriteMask		= 0xf;
	ctx->setCachedColorAttachmentBlend( colorBlend );	
}

void enableAdditiveBlending()
{
	auto ctx = vk::context();
	//ctx->enable( GL_BLEND );
	//ctx->blendFunc( GL_SRC_ALPHA, GL_ONE );

	VkPipelineColorBlendAttachmentState colorBlend = {};
	colorBlend.blendEnable			= VK_TRUE;
	colorBlend.srcColorBlendFactor	= VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlend.dstColorBlendFactor	= VK_BLEND_FACTOR_ONE;
	colorBlend.colorBlendOp			= VK_BLEND_OP_ADD;
	colorBlend.srcAlphaBlendFactor	= VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlend.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ONE;
	colorBlend.alphaBlendOp			= VK_BLEND_OP_ADD;
	colorBlend.colorWriteMask		= 0xf;
	ctx->setCachedColorAttachmentBlend( colorBlend );	
}

void cullMode( VkCullModeFlagBits mode )
{
	vk::context()->cullMode( mode );
}

void disableDepthRead()
{
	auto ctx = vk::context();
	ctx->setDepthTest( VK_FALSE );
}

void enableDepthRead( bool enable )
{
	auto ctx = vk::context();
	ctx->setDepthTest( enable ? VK_TRUE : VK_FALSE );
}

void enableDepthWrite( bool enable )
{
	auto ctx = vk::context();
	ctx->setDepthWrite( enable ? VK_TRUE : VK_FALSE );
}

void disableDepthWrite()
{
	auto ctx = vk::context();
	ctx->setDepthWrite( VK_FALSE );
}

void enableDepthBias( bool enable )
{
}

void depthBias( float slopeFactor, float constantFactor, float clamp )
{
}

void setMatrices( const ci::Camera& cam )
{
	const mat4 flipY = mat4( 1, 0, 0, 0,  0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 );
	auto ctx = context();
	ctx->getViewMatrixStack().back() = cam.getViewMatrix();
	ctx->getProjectionMatrixStack().back() = flipY*cam.getProjectionMatrix();
	ctx->getModelMatrixStack().back() = mat4();
}

void setModelMatrix( const ci::mat4 &m )
{
	auto ctx = context();
	ctx->getModelMatrixStack().back() = m;
}

void setViewMatrix( const ci::mat4 &m )
{
	auto ctx = context();
	ctx->getViewMatrixStack().back() = m;
}

void setProjectionMatrix( const ci::mat4 &m )
{
	const mat4 flipY = mat4( 1, 0, 0, 0,  0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 );
	auto ctx = context();
	ctx->getProjectionMatrixStack().back() = flipY*m;
}

void pushModelMatrix()
{
	auto ctx = context();
	ctx->getModelMatrixStack().push_back( ctx->getModelMatrixStack().back() );
}

void popModelMatrix()
{
	auto ctx = context();
	ctx->getModelMatrixStack().pop_back();
}

void pushViewMatrix()
{
	auto ctx = context();
	ctx->getViewMatrixStack().push_back( ctx->getViewMatrixStack().back() );
}

void popViewMatrix()
{
	auto ctx = context();
	ctx->getViewMatrixStack().pop_back();
}

void pushProjectionMatrix()
{
	auto ctx = context();
	ctx->getProjectionMatrixStack().push_back( ctx->getProjectionMatrixStack().back() );
}

void popProjectionMatrix()
{
	auto ctx = context();
	ctx->getProjectionMatrixStack().pop_back();
}

void pushModelView()
{
	auto ctx = context();
	ctx->getModelMatrixStack().push_back( ctx->getModelMatrixStack().back() );
	ctx->getViewMatrixStack().push_back( ctx->getViewMatrixStack().back() );
}

void popModelView()
{
	auto ctx = context();
	ctx->getModelMatrixStack().pop_back();
	ctx->getViewMatrixStack().pop_back();
}

void pushMatrices()
{
	auto ctx = context();
	ctx->getModelMatrixStack().push_back( ctx->getModelMatrixStack().back() );
	ctx->getViewMatrixStack().push_back( ctx->getViewMatrixStack().back() );
	ctx->getProjectionMatrixStack().push_back( ctx->getProjectionMatrixStack().back() );
}

void popMatrices()
{
	auto ctx = context();
	ctx->getModelMatrixStack().pop_back();
	ctx->getViewMatrixStack().pop_back();
	ctx->getProjectionMatrixStack().pop_back();
}

void multModelMatrix( const ci::mat4& mtx )
{
	auto ctx = vk::context();
	ctx->getModelMatrixStack().back() *= mtx;
}

void multViewMatrix( const ci::mat4& mtx )
{
	auto ctx = vk::context();
	ctx->getViewMatrixStack().back() *= mtx;
}

void multProjectionMatrix( const ci::mat4& mtx )
{
	auto ctx = vk::context();
	ctx->getProjectionMatrixStack().back() *= mtx;
}

mat4 getModelMatrix()
{
	auto ctx = vk::context();
	return ctx->getModelMatrixStack().back();
}

mat4 getViewMatrix()
{
	auto ctx = vk::context();
	return ctx->getViewMatrixStack().back();
}

mat4 getProjectionMatrix()
{
	auto ctx = vk::context();
	return ctx->getProjectionMatrixStack().back();
}

mat4 getModelView()
{
	auto ctx = context();
	return ctx->getViewMatrixStack().back() * ctx->getModelMatrixStack().back();
}

mat4 getModelViewProjection()
{
	auto ctx = context();
	return ctx->getProjectionMatrixStack().back() * ctx->getViewMatrixStack().back() * ctx->getModelMatrixStack().back();
}

mat4 calcViewMatrixInverse()
{
	return glm::inverse( getViewMatrix() );
}

mat3 calcNormalMatrix()
{
	//return glm::transpose( glm::inverse( getModelView() ) ); //glm::inverseTranspose( getModelView() );
	return glm::inverseTranspose( glm::mat3( getModelView() ) );
}
	
mat4 calcModelMatrixInverseTranspose()
{
	return glm::inverseTranspose( getModelMatrix() );
}
	
mat4 calcViewportMatrix()
{
	auto curViewport = getViewport();
	
	const float a = ( curViewport.second.x - curViewport.first.x ) / 2.0f;
	const float b = ( curViewport.second.y - curViewport.first.y ) / 2.0f;
	const float c = 1.0f / 2.0f;
	
	const float tx = ( curViewport.second.x + curViewport.first.x ) / 2.0f;
	const float ty = ( curViewport.second.y + curViewport.second.y ) / 2.0f;
	const float tz = 1.0f / 2.0f;
	
	return mat4(
		a, 0, 0, 0,
		0, b, 0, 0,
		0, 0, c, 0,
		tx, ty, tz, 1
	);
}

void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
{
	auto ctx = vk::context();

	CameraPersp cam( screenWidth, screenHeight, fovDegrees, nearPlane, farPlane );
	const mat4 flipY = mat4( 1, 0, 0, 0,  0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 );
	ctx->getModelMatrixStack().back() = mat4();
	ctx->getProjectionMatrixStack().back() = flipY*cam.getProjectionMatrix();
	ctx->getViewMatrixStack().back() = cam.getViewMatrix();
	if( ! originUpperLeft ) {
		ctx->getViewMatrixStack().back() *= glm::scale( vec3( 1, -1, 1 ) );								// invert Y axis so increasing Y goes down.
		ctx->getViewMatrixStack().back() *= glm::translate( vec3( 0, (float) - screenHeight, 0 ) );		// shift origin up to upper-left corner.
	}
}

void setMatricesWindowPersp( const ci::ivec2& screenSize, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
{
	setMatricesWindowPersp( screenSize.x, screenSize.y, fovDegrees, nearPlane, farPlane, originUpperLeft );
}

void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft )
{
	auto ctx = vk::context();
	ctx->getModelMatrixStack().back() = mat4();
	ctx->getViewMatrixStack().back() = mat4();

	float sx = 2.0f / (float)screenWidth;
	float sy = 2.0f / (float)screenHeight;
	float ty = -1;

	if( ! originUpperLeft ) {
		sy *= -1;
		ty *= -1;
	}

	mat4 &m = ctx->getProjectionMatrixStack().back();
	m = mat4( sx, 0,  0, 0,
			  0, sy,  0, 0,
			  0,  0, -1, 0,
			 -1, ty,  0, 1 );
}

void setMatricesWindow( const ci::ivec2& screenSize, bool originUpperLeft )
{
	setMatricesWindow( screenSize.x, screenSize.y, originUpperLeft );
}

void rotate( const quat &quat )
{
	auto ctx = vk::context();
	ctx->getModelMatrixStack().back() *= toMat4( quat );
}

void rotate( float angleRadians, const vec3 &axis )
{
	if( math<float>::abs( angleRadians ) > EPSILON_VALUE ) {
		auto ctx = vk::context();
		ctx->getModelMatrixStack().back() *= glm::rotate( angleRadians, axis );
	}
}

void scale( const ci::vec3& v )
{
	auto ctx = vk::context();
	ctx->getModelMatrixStack().back() *= glm::scale( v );
}

void translate( const ci::vec3& v )
{
	auto ctx = vk::context();
	ctx->getModelMatrixStack().back() *= glm::translate( v );
}

}} // namespace cinder::vk