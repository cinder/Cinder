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

#include "cinder/vk/draw.h"
#include "cinder/vk/CommandPool.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Descriptor.h"
#include "cinder/vk/PipelineSelector.h"
#include "cinder/vk/RenderPass.h"
#include "cinder/vk/ShaderProg.h"
#include "cinder/vk/Texture.h"
#include "cinder/vk/UniformBuffer.h"
#include "cinder/vk/UniformLayout.h"
#include "cinder/vk/VertexBuffer.h"

namespace cinder { namespace vk {

void draw( const Texture2dRef &texture, const Rectf &dstRect )
{
	vec2 uv0 = vec2( 0.0f, 0.0f );
	vec2 uv1 = texture->getFormat().isUnnormalizedCoordinates() ? texture->getSize() : vec2( 1.0f, 1.0f );

	// Triangle strip
	std::vector<float> data = {
		dstRect.x1, dstRect.y1, 0.0f, 1.0f, uv0.x, uv0.y,
		dstRect.x1, dstRect.y2, 0.0f, 1.0f, uv0.x, uv1.y,
		dstRect.x2, dstRect.y1, 0.0f, 1.0f, uv1.x, uv0.y,
		dstRect.x2, dstRect.y2, 0.0f, 1.0f, uv1.x, uv1.y
	};

	vk::VertexBufferRef vertexBuffer = vk::VertexBuffer::create( static_cast<const void*>( data.data() ), data.size()*sizeof( float ) );
	vk::context()->addTransient( vertexBuffer );

	auto shader = vk::context()->hasShaderProg() ? vk::context()->getShaderProg() : vk::context()->getStockShader( vk::ShaderDef().texture() );

	// Uniform layout, uniform set
	const vk::UniformLayout& uniformLayout = shader->getUniformLayout();
	vk::UniformSetRef uniformSet = vk::UniformSet::create( uniformLayout );

	// Set the texture here so we can create the descriptor set without any grief
	uniformSet->uniform( "uTex0", texture );

/*
	// Descriptor layout, pool, set
	vk::DescriptorSetLayoutRef descriptorSetLayout = vk::DescriptorSetLayout::create( *uniformSet );
	vk::DescriptorPoolRef descriptorPool = vk::DescriptorPool::create( descriptorSetLayout->getLayoutBindings() );
	vk::DescriptorSetRef descriptorSet = vk::DescriptorSet::create( descriptorPool->getDescriptorPool(), *uniformSet, descriptorSetLayout );
	vk::context()->addTransient( descriptorSetLayout );
	vk::context()->addTransient( descriptorPool );
	vk::context()->addTransient( descriptorSet );
	// Pipeline layout
	vk::PipelineLayoutRef pipelineLayout = vk::PipelineLayout::create( descriptorSetLayout );
	vk::context()->addTransient( pipelineLayout );
*/

	// Descriptor layout, pool, set
	const auto& descLayoutBindings = uniformSet->getDescriptorSetlayoutBindings();
	VkDescriptorSetLayout descriptorSetLayout = vk::context()->getDescriptorSetLayoutSelector()->getSelectedLayout( descLayoutBindings );
	vk::DescriptorPoolRef descriptorPool = vk::DescriptorPool::create( descLayoutBindings );
	vk::DescriptorSetRef descriptorSet = vk::DescriptorSet::create( descriptorPool->getDescriptorPool(), *uniformSet, descriptorSetLayout );
	vk::context()->addTransient( descriptorPool );
	vk::context()->addTransient( descriptorSet );
	// Pipeline layout
	VkPipelineLayout pipelineLayout = vk::context()->getPipelineLayoutSelector()->getSelectedLayout( { descriptorSetLayout } );
/*
	// Pipeline
	vk::PipelineRef pipeline;
	{
		vk::Pipeline::Options pipelineOptions;
		pipelineOptions.setTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
		pipelineOptions.setPipelineLayout( pipelineLayout );
		pipelineOptions.setRenderPass( vk::context()->getRenderPass() );
		pipelineOptions.setShaderProg( shader );

		// Vertex input attribute description
		// Position
		VkVertexInputAttributeDescription viad = {};
		viad.binding	= 0;
		viad.format		= VK_FORMAT_R32G32B32A32_SFLOAT;
		viad.location	= 0;
		viad.offset		= 0;
		pipelineOptions.addVertexAtrribute( viad );
		// UV
		viad = {};
		viad.binding	= 0;
		viad.format		= VK_FORMAT_R32G32_SFLOAT;
		viad.location	= 1;
		viad.offset		= 4*sizeof( float );
		pipelineOptions.addVertexAtrribute( viad );

		// Vertex input binding description
		VkVertexInputBindingDescription vibd = {};
		vibd.binding	= 0;
		vibd.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
		vibd.stride		= 6*sizeof( float );
		pipelineOptions.addVertexBinding( vibd );

		pipeline = vk::Pipeline::create( pipelineOptions, nullptr );
		vk::context()->addTransient( pipeline );
	}
/*/

	// Pipeline
	VkPipeline pipeline = VK_NULL_HANDLE;
	{
		// Vertex input attribute description
		// Position
		VkVertexInputAttributeDescription viad0 = {};
		viad0.binding	= 0;
		viad0.format	= VK_FORMAT_R32G32B32A32_SFLOAT;
		viad0.location	= 0;
		viad0.offset	= 0;
		// UV
		VkVertexInputAttributeDescription viad1 = {};
		viad1.binding	= 0;
		viad1.format	= VK_FORMAT_R32G32_SFLOAT;
		viad1.location	= 1;
		viad1.offset	= 4*sizeof(float);

		// Vertex input binding description
		VkVertexInputBindingDescription vibd = {};
		vibd.binding	= 0;
		vibd.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
		vibd.stride		= 2*sizeof(float) + 4*sizeof(float);

		auto ctx = vk::context();
		auto& pipelineSelector = ctx->getPipelineSelector();
		pipelineSelector->setTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
		pipelineSelector->setVertexInputAttributeDescriptions( { viad0, viad1 } );
		pipelineSelector->setVertexInputBindingDescriptions( { vibd }  );
		pipelineSelector->setCullMode( ctx->getCullMode() );
		pipelineSelector->setFrontFace( ctx->getFrontFace() );
		pipelineSelector->setDepthBias( ctx->getDepthBiasEnable(), ctx->getDepthBiasSlopeFactor(), ctx->getDepthBiasConstantFactor(), ctx->getDepthBiasClamp() );
		pipelineSelector->setRasterizationSamples( ctx->getRenderPass()->getSubPassSampleCount( ctx->getSubPass() ) );
		pipelineSelector->setDepthTest( ctx->getDepthTest() );
		pipelineSelector->setDepthWrite( ctx->getDepthWrite() );
		pipelineSelector->setColorBlendAttachments( ctx->getColorBlendAttachments() );
		pipelineSelector->setShaderStages( shader->getShaderStages() );
		pipelineSelector->setRenderPass( ctx->getRenderPass()->getRenderPass() );
		pipelineSelector->setSubPass( ctx->getSubPass() );
		//pipelineSelector->setPipelineLayout( pipelineLayout->getPipelineLayout() );
		pipelineSelector->setPipelineLayout( pipelineLayout );
		pipeline = pipelineSelector->getSelectedPipeline();
	}
//*/

	// This is the separation between the setup and the draw sequence. 

	// Fill out uniform vars
	for( auto& binding : uniformSet->getBindings() ) {
		if( ! binding.isBlock() ) {
			continue;
		}
		vk::context()->setDefaultUniformVars( binding.getUniformBuffer() );
	}
	uniformSet->bufferPending();

	// Get current command buffer
	auto cmdBuf = vk::context()->getCommandBuffer()->getCommandBuffer();

	// Bind vertex buffer
	std::vector<VkBuffer> vertexBuffers = { vertexBuffer->getBuffer() };
	std::vector<VkDeviceSize> offsets = { 0 };
	vkCmdBindVertexBuffers( cmdBuf, 0, static_cast<uint32_t>( vertexBuffers.size() ), vertexBuffers.data(), offsets.data() );
	// Bind pipeline
	vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );

	std::vector<VkDescriptorSet> descSets = { descriptorSet->getDescriptorSet() };
	//vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout->getPipelineLayout(), 0, static_cast<uint32_t>( descSets.size() ), descSets.data(), 0, nullptr );
	vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, static_cast<uint32_t>( descSets.size() ), descSets.data(), 0, nullptr );

	uint32_t numVertices = 4;
	vkCmdDraw( cmdBuf, numVertices, 1, 0, 0 );
}

void drawSolidRect( const Rectf &r, const vec2 &upperLeftTexCoord, const vec2 &lowerRightTexCoord )
{
	const ColorAf& color = vk::context()->getCurrentColor();
	vec2 uv0 = upperLeftTexCoord;
	vec2 uv1 = lowerRightTexCoord;

	// Triangle strip
	std::vector<float> data = {
		r.x1, r.y1, 0.0f, 1.0f, uv0.x, uv0.y, color.r, color.g, color.b, color.a,
		r.x1, r.y2, 0.0f, 1.0f, uv0.x, uv1.y, color.r, color.g, color.b, color.a,
		r.x2, r.y1, 0.0f, 1.0f, uv1.x, uv0.y, color.r, color.g, color.b, color.a,
		r.x2, r.y2, 0.0f, 1.0f, uv1.x, uv1.y, color.r, color.g, color.b, color.a
	};

	vk::VertexBufferRef vertexBuffer = vk::VertexBuffer::create( static_cast<const void*>( data.data() ), data.size()*sizeof( float ) );
	vk::context()->addTransient( vertexBuffer );

	auto shader = vk::context()->hasShaderProg() ? vk::context()->getShaderProg() : vk::context()->getStockShader( vk::ShaderDef().color() );

	// Uniform layout, uniform set
	const vk::UniformLayout& uniformLayout = shader->getUniformLayout();
	vk::UniformSetRef uniformSet = vk::UniformSet::create( uniformLayout );

	// Descriptor layout, pool, set
	const auto& descLayoutBindings = uniformSet->getDescriptorSetlayoutBindings();
	VkDescriptorSetLayout descriptorSetLayout = vk::context()->getDescriptorSetLayoutSelector()->getSelectedLayout( descLayoutBindings );
	vk::DescriptorPoolRef descriptorPool = vk::DescriptorPool::create( descLayoutBindings );
	vk::DescriptorSetRef descriptorSet = vk::DescriptorSet::create( descriptorPool->getDescriptorPool(), *uniformSet, descriptorSetLayout );
	vk::context()->addTransient( descriptorPool );
	vk::context()->addTransient( descriptorSet );
	// Pipeline layout
	VkPipelineLayout pipelineLayout = vk::context()->getPipelineLayoutSelector()->getSelectedLayout( { descriptorSetLayout } );

	// Pipeline
	VkPipeline pipeline = VK_NULL_HANDLE;
	{
		// Vertex input attribute description
		// Position
		VkVertexInputAttributeDescription viad0 = {};
		viad0.binding	= 0;
		viad0.format	= VK_FORMAT_R32G32B32A32_SFLOAT;
		viad0.location	= 0;
		viad0.offset	= 0;
		// UV
		VkVertexInputAttributeDescription viad1 = {};
		viad1.binding	= 0;
		viad1.format	= VK_FORMAT_R32G32_SFLOAT;
		viad1.location	= 1;
		viad1.offset	= 4*sizeof(float);
		// Color
		VkVertexInputAttributeDescription viad2 = {};
		viad2.binding	= 0;
		viad2.format	= VK_FORMAT_R32G32B32A32_SFLOAT;
		viad2.location	= 2;
		viad2.offset	= 2*sizeof(float) + 4*sizeof(float);

		// Vertex input binding description
		VkVertexInputBindingDescription vibd = {};
		vibd.binding	= 0;
		vibd.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
		vibd.stride		= 4*sizeof(float) + 2*sizeof(float) + 4*sizeof(float);

		auto ctx = vk::context();
		auto& pipelineSelector = ctx->getPipelineSelector();
		pipelineSelector->setTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
		pipelineSelector->setVertexInputAttributeDescriptions( { viad0, viad1, viad2 } );
		pipelineSelector->setVertexInputBindingDescriptions( { vibd }  );
		pipelineSelector->setCullMode( ctx->getCullMode() );
		pipelineSelector->setFrontFace( ctx->getFrontFace() );
		pipelineSelector->setDepthBias( ctx->getDepthBiasEnable(), ctx->getDepthBiasSlopeFactor(), ctx->getDepthBiasConstantFactor(), ctx->getDepthBiasClamp() );
		pipelineSelector->setRasterizationSamples( ctx->getRenderPass()->getSubPassSampleCount( ctx->getSubPass() ) );
		pipelineSelector->setDepthTest( ctx->getDepthTest() );
		pipelineSelector->setDepthWrite( ctx->getDepthWrite() );
		pipelineSelector->setColorBlendAttachments( ctx->getColorBlendAttachments() );
		pipelineSelector->setShaderStages( shader->getShaderStages() );
		pipelineSelector->setRenderPass( ctx->getRenderPass()->getRenderPass() );
		pipelineSelector->setSubPass( ctx->getSubPass() );
		//pipelineSelector->setPipelineLayout( pipelineLayout->getPipelineLayout() );
		pipelineSelector->setPipelineLayout( pipelineLayout );
		pipeline = pipelineSelector->getSelectedPipeline();
	}

	// This is the separation between the setup and the draw sequence. 

	// Fill out uniform vars
	for( auto& binding : uniformSet->getBindings() ) {
		if( ! binding.isBlock() ) {
			continue;
		}
		vk::context()->setDefaultUniformVars( binding.getUniformBuffer() );
	}
	uniformSet->bufferPending();

	// Get current command buffer
	auto cmdBuf = vk::context()->getCommandBuffer()->getCommandBuffer();

	// Bind vertex buffer
	std::vector<VkBuffer> vertexBuffers = { vertexBuffer->getBuffer() };
	std::vector<VkDeviceSize> offsets = { 0 };
	vkCmdBindVertexBuffers( cmdBuf, 0, static_cast<uint32_t>( vertexBuffers.size() ), vertexBuffers.data(), offsets.data() );
	// Bind pipeline
	vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );

	std::vector<VkDescriptorSet> descSets = { descriptorSet->getDescriptorSet() };
	vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, static_cast<uint32_t>( descSets.size() ), descSets.data(), 0, nullptr );

	uint32_t numVertices = 4;
	vkCmdDraw( cmdBuf, numVertices, 1, 0, 0 );
}

}} // namespace cinder::vk