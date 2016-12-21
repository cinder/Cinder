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
#include "cinder/vk/Device.h"
#include "cinder/vk/PipelineSelector.h"
#include "cinder/vk/RenderPass.h"
#include "cinder/vk/ShaderProg.h"
#include "cinder/vk/Texture.h"
#include "cinder/vk/UniformBuffer.h"
#include "cinder/vk/UniformLayout.h"
#include "cinder/vk/VertexBuffer.h"
#include "cinder/vk/wrapper.h"

namespace cinder { namespace vk {

void draw( const Texture2dRef &texture, const Rectf &dstRect, const std::string& uniformName )
{
	vec2 uv0 = vec2( 0.0f, 0.0f );
	vec2 uv1 = texture->getFormat().isUnnormalizedCoordinates() ? vec2( texture->getSize() ) : vec2( 1.0f, 1.0f );

	// Triangle strip
	std::vector<float> data = {
		dstRect.x1, dstRect.y1, 0.0f, 1.0f, uv0.x, uv0.y,
		dstRect.x1, dstRect.y2, 0.0f, 1.0f, uv0.x, uv1.y,
		dstRect.x2, dstRect.y1, 0.0f, 1.0f, uv1.x, uv0.y,
		dstRect.x2, dstRect.y2, 0.0f, 1.0f, uv1.x, uv1.y
	};

	vk::VertexBufferRef vertexBuffer = vk::VertexBuffer::create( static_cast<const void*>( data.data() ), data.size()*sizeof( float ), vk::VertexBuffer::Format().setTransientAllocation() );
	vk::context()->addTransient( vertexBuffer );

	auto shader = vk::context()->hasShaderProg() ? vk::context()->getShaderProg() : vk::context()->getStockShader( vk::ShaderDef().texture() );

	// Uniform layout, uniform set
	const vk::UniformLayout& uniformLayout = shader->getUniformLayout();
	vk::UniformSet::Options uniformSetOptions = vk::UniformSet::Options().setTransientAllocation();
	vk::UniformSetRef transientUniformSet = vk::UniformSet::create( uniformLayout, uniformSetOptions );
	vk::context()->addTransient( transientUniformSet );

	// Descriptor view
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts = vk::context()->getDevice()->getDescriptorSetLayoutSelector()->getSelectedLayout( transientUniformSet->getCachedDescriptorSetLayoutBindings() );
	vk::DescriptorSetViewRef transientDescriptorView = vk::DescriptorSetView::create( transientUniformSet );
	transientDescriptorView->allocateDescriptorSets();
	vk::context()->addTransient( transientDescriptorView );

	// Pipeline layout
	VkPipelineLayout pipelineLayout = vk::context()->getDevice()->getPipelineLayoutSelector()->getSelectedLayout( descriptorSetLayouts );

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
		auto& pipelineSelector = ctx->getDevice()->getPipelineSelector();
		pipelineSelector->setTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
		pipelineSelector->setVertexInputAttributeDescriptions( { viad0, viad1 } );
		pipelineSelector->setVertexInputBindingDescriptions( { vibd }  );
		pipelineSelector->setCullMode( ctx->getCullMode() );
		pipelineSelector->setFrontFace( ctx->getFrontFace() );
		pipelineSelector->setDepthBias( ctx->getDepthBiasEnable(), ctx->getDepthBiasSlopeFactor(), ctx->getDepthBiasConstantFactor(), ctx->getDepthBiasClamp() );
		pipelineSelector->setRasterizationSamples( ctx->getRenderPass()->getSubpassSampleCount( ctx->getSubpass() ) );
		pipelineSelector->setDepthTest( ctx->getDepthTest() );
		pipelineSelector->setDepthWrite( ctx->getDepthWrite() );
		pipelineSelector->setColorBlendAttachments( ctx->getColorBlendAttachments() );
		pipelineSelector->setShaderStages( shader->getShaderStages() );
		pipelineSelector->setRenderPass( ctx->getRenderPass()->getRenderPass() );
		pipelineSelector->setSubPass( ctx->getSubpass() );
		pipelineSelector->setPipelineLayout( pipelineLayout );
		pipeline = pipelineSelector->getSelectedPipeline();
	}

	// This is the separation between the setup and the draw sequence. 

	// Get current command buffer
	auto cmdBufRef = vk::context()->getCommandBuffer();
	auto cmdBuf = cmdBufRef->getCommandBuffer();

	// Fill out uniform vars
	transientUniformSet->uniform( uniformName, texture );
	transientUniformSet->setDefaultUniformVars( vk::context() );
	transientUniformSet->bufferPending( cmdBufRef, VK_ACCESS_HOST_WRITE_BIT , VK_ACCESS_UNIFORM_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );

	// Update descriptor set
	transientDescriptorView->updateDescriptorSets();

	// Bind vertex buffer
	std::vector<VkBuffer> vertexBuffers = { vertexBuffer->getBuffer() };
	std::vector<VkDeviceSize> offsets = { 0 };
	vkCmdBindVertexBuffers( cmdBuf, 0, static_cast<uint32_t>( vertexBuffers.size() ), vertexBuffers.data(), offsets.data() );

	// Bind pipeline
	vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );

	// Bind descriptor sets
	const auto& descriptorSets = transientDescriptorView->getDescriptorSets();
	for( uint32_t i = 0; i < descriptorSets.size(); ++i ) {
		const auto& ds = descriptorSets[i];
		std::vector<VkDescriptorSet> descSets = { ds->vkObject() };
		vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, i, static_cast<uint32_t>( descSets.size() ), descSets.data(), 0, nullptr );
	}

	// Draw geometry
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

	vk::VertexBufferRef vertexBuffer = vk::VertexBuffer::create( static_cast<const void*>( data.data() ), data.size()*sizeof( float ), vk::VertexBuffer::Format().setTransientAllocation() );
	vk::context()->addTransient( vertexBuffer );

	auto shader = vk::context()->hasShaderProg() ? vk::context()->getShaderProg() : vk::context()->getStockShader( vk::ShaderDef().color() );

	// Uniform layout, uniform set
	const vk::UniformLayout& uniformLayout = shader->getUniformLayout();
	vk::UniformSet::Options uniformSetOptions = vk::UniformSet::Options().setTransientAllocation();
	vk::UniformSetRef transientUniformSet = vk::UniformSet::create( uniformLayout, uniformSetOptions );
	vk::context()->addTransient( transientUniformSet );

	// Descriptor view
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts = vk::context()->getDevice()->getDescriptorSetLayoutSelector()->getSelectedLayout( transientUniformSet->getCachedDescriptorSetLayoutBindings() );
	vk::DescriptorSetViewRef transientDescriptorView = vk::DescriptorSetView::create( transientUniformSet );
	transientDescriptorView->allocateDescriptorSets();
	vk::context()->addTransient( transientDescriptorView );

	// Pipeline layout
	VkPipelineLayout pipelineLayout = vk::context()->getDevice()->getPipelineLayoutSelector()->getSelectedLayout( descriptorSetLayouts );

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
		auto& pipelineSelector = ctx->getDevice()->getPipelineSelector();
		pipelineSelector->setTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
		pipelineSelector->setVertexInputAttributeDescriptions( { viad0, viad1, viad2 } );
		pipelineSelector->setVertexInputBindingDescriptions( { vibd }  );
		pipelineSelector->setCullMode( ctx->getCullMode() );
		pipelineSelector->setFrontFace( ctx->getFrontFace() );
		pipelineSelector->setDepthBias( ctx->getDepthBiasEnable(), ctx->getDepthBiasSlopeFactor(), ctx->getDepthBiasConstantFactor(), ctx->getDepthBiasClamp() );
		pipelineSelector->setRasterizationSamples( ctx->getRenderPass()->getSubpassSampleCount( ctx->getSubpass() ) );
		pipelineSelector->setDepthTest( ctx->getDepthTest() );
		pipelineSelector->setDepthWrite( ctx->getDepthWrite() );
		pipelineSelector->setColorBlendAttachments( ctx->getColorBlendAttachments() );
		pipelineSelector->setShaderStages( shader->getShaderStages() );
		pipelineSelector->setRenderPass( ctx->getRenderPass()->getRenderPass() );
		pipelineSelector->setSubPass( ctx->getSubpass() );
		pipelineSelector->setPipelineLayout( pipelineLayout );
		pipeline = pipelineSelector->getSelectedPipeline();
	}

	// This is the separation between the setup and the draw sequence. 

	// Get current command buffer
	auto cmdBufRef = vk::context()->getCommandBuffer();
	auto cmdBuf = cmdBufRef->getCommandBuffer();

	// Fill out uniform vars
	transientUniformSet->setDefaultUniformVars( vk::context() );
	transientUniformSet->bufferPending( cmdBufRef, VK_ACCESS_HOST_WRITE_BIT , VK_ACCESS_UNIFORM_READ_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );

	// Update descriptor set
	transientDescriptorView->updateDescriptorSets();

	// Bind vertex buffer
	std::vector<VkBuffer> vertexBuffers = { vertexBuffer->getBuffer() };
	std::vector<VkDeviceSize> offsets = { 0 };
	vkCmdBindVertexBuffers( cmdBuf, 0, static_cast<uint32_t>( vertexBuffers.size() ), vertexBuffers.data(), offsets.data() );

	// Bind pipeline
	vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );

	// Bind descriptor sets
	const auto& descriptorSets = transientDescriptorView->getDescriptorSets();
	for( uint32_t i = 0; i < descriptorSets.size(); ++i ) {
		const auto& ds = descriptorSets[i];
		std::vector<VkDescriptorSet> descSets = { ds->vkObject() };
		vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, i, static_cast<uint32_t>( descSets.size() ), descSets.data(), 0, nullptr );
	}

	// Draw geometry
	uint32_t numVertices = 4;
	vkCmdDraw( cmdBuf, numVertices, 1, 0, 0 );
}

}} // namespace cinder::vk