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
*/

#include "DrawUtil.h"

SolidRect::SolidRect( const ci::vk::ShaderProgRef& shaderProg )
	: mShader( shaderProg ), mPosRect( 0, 0, 0, 0 ), mTexCoordRect( 0, 0, 1, 1 )
{
	initialize();
}

SolidRect::~SolidRect()
{
	if( mVertexBuffer ) {
		mVertexBuffer->~VertexBuffer();
		mVertexBuffer.reset();
	}

	if( mUniformSet ) {
		mUniformSet->~UniformSet();
		mUniformSet.reset();
	}

	if( mDescriptorSetLayout ) {
		mDescriptorSetLayout->~DescriptorSetLayout();
		mDescriptorSetLayout.reset();
	}

	if( mDescriptorSet ) {
		mDescriptorSet->~DescriptorSet();
		mDescriptorSet.reset();
	}

	if( mDescriptorPool ) {
		mDescriptorPool->~DescriptorPool();
		mDescriptorPool.reset();
	}

	if( mPipelineLayout ) {
		mPipelineLayout->~PipelineLayout();
		mPipelineLayout.reset();
	}
}

void SolidRect::initialize()
{
	// Triangle strip
	mData = {
		mPosRect.x1, mPosRect.y1, 0.0f, 1.0f, mTexCoordRect.x1, mTexCoordRect.y1,
		mPosRect.x1, mPosRect.y2, 0.0f, 1.0f, mTexCoordRect.x1, mTexCoordRect.y2,
		mPosRect.x2, mPosRect.y1, 0.0f, 1.0f, mTexCoordRect.x2, mTexCoordRect.y1,
		mPosRect.x2, mPosRect.y2, 0.0f, 1.0f, mTexCoordRect.x2, mTexCoordRect.y2
	};

	// Vertex buffer
	mVertexBuffer = ci::vk::VertexBuffer::create( static_cast<const void*>( mData.data() ), mData.size()*sizeof( float ) );

	// Uniform layout, uniform set
	const ci::vk::UniformLayout& uniformLayout = mShader->getUniformLayout();
	mUniformSet = ci::vk::UniformSet::create( uniformLayout );

	// Descriptor layout, pool, set
	mDescriptorSetLayout = ci::vk::DescriptorSetLayout::create( *mUniformSet );
	mDescriptorPool = ci::vk::DescriptorPool::create( mDescriptorSetLayout->getLayoutBindings() );

	// Pipeline layout
	mPipelineLayout = ci::vk::PipelineLayout::create( mDescriptorSetLayout );

	// Look for the semantics that we want
	int32_t posLoc = -1;
	int32_t texLoc = -1;
	const auto& attribs = mShader->getActiveAttributes();
	for( const auto& attrib : attribs ) {
		switch( attrib.getSemantic() ) {
			case ci::geom::Attrib::POSITION:
				posLoc = attrib.getLocation();
				break;
			case ci::geom::Attrib::TEX_COORD_0:
				texLoc = attrib.getLocation();
				break;
		}
	}

	// Vertex attributes
	if( -1 != posLoc ) {
		VkVertexInputAttributeDescription viad = {};
		viad.binding	= 0;
		viad.format		= VK_FORMAT_R32G32B32A32_SFLOAT;
		viad.location	= posLoc;
		viad.offset		= 0;
		mVertexAttributes.push_back( viad );
	}
	//
	if( -1 != texLoc ) {
		VkVertexInputAttributeDescription viad = {};
		viad.binding	= 0;
		viad.format		= VK_FORMAT_R32G32_SFLOAT;
		viad.location	= texLoc;
		viad.offset		= 4*sizeof( float );
		mVertexAttributes.push_back( viad );
	}

	// Vertex binding
	VkVertexInputBindingDescription vibd = {};
	vibd.binding	= 0;
	vibd.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
	vibd.stride		= 6*sizeof( float );
	mVertexBindings.push_back( vibd );		
}

void SolidRect::set( const ci::Rectf& rect )
{
	// Update
	mPosRect = rect;

	// Triangle strip
	mData = {
		mPosRect.x1, mPosRect.y1, 0.0f, 1.0f, mTexCoordRect.x1, mTexCoordRect.y1,
		mPosRect.x1, mPosRect.y2, 0.0f, 1.0f, mTexCoordRect.x1, mTexCoordRect.y2,
		mPosRect.x2, mPosRect.y1, 0.0f, 1.0f, mTexCoordRect.x2, mTexCoordRect.y1,
		mPosRect.x2, mPosRect.y2, 0.0f, 1.0f, mTexCoordRect.x2, mTexCoordRect.y2
	};

	// Buffer data
	mVertexBuffer->bufferData( mData.size()*sizeof( float ), static_cast<const void*>( mData.data() ) );
}

void SolidRect::draw( const ci::vk::CommandBufferRef& commandBuffer )
{

	if( ! mDescriptorSet ) {
		mDescriptorSet = ci::vk::DescriptorSet::create( mDescriptorPool->getDescriptorPool(), *mUniformSet, mDescriptorSetLayout );
	}

	for( auto& binding : mUniformSet->getBindings() ) {
		if( ! binding.isBlock() ) {
			continue;
		}
		ci::vk::context()->setDefaultUniformVars( binding.getUniformBuffer() );
	}
	mUniformSet->bufferPending();

	auto cmdBuf = commandBuffer->getCommandBuffer();

	std::vector<VkBuffer> vertexBuffers = { mVertexBuffer->getBuffer() };
	std::vector<VkDeviceSize> offsets = { 0 };
	vkCmdBindVertexBuffers( cmdBuf, 0, static_cast<uint32_t>( vertexBuffers.size() ), vertexBuffers.data(), offsets.data() );
	
	// Update descriptor set
	mDescriptorSet->update( *mUniformSet );

	std::vector<VkDescriptorSet> descSets = { mDescriptorSet->getDescriptorSet() };
	vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getPipelineLayout(), 0, static_cast<uint32_t>( descSets.size() ), descSets.data(), 0, nullptr );

	auto ctx = ci::vk::context();
	auto& pipelineSelector = ctx->getDevice()->getPipelineSelector();
	pipelineSelector->setTopology( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP );
	pipelineSelector->setVertexInputBindingDescriptions( mVertexBindings );
	pipelineSelector->setVertexInputAttributeDescriptions( mVertexAttributes );
	pipelineSelector->setCullMode( ctx->getCullMode() );
	pipelineSelector->setFrontFace( ctx->getFrontFace() );
	pipelineSelector->setDepthTest( ctx->getDepthTest() );
	pipelineSelector->setDepthWrite( ctx->getDepthWrite() );
	pipelineSelector->setColorBlendAttachments( ctx->getColorBlendAttachments() );
	pipelineSelector->setShaderStages( mShader->getShaderStages() );
	pipelineSelector->setRenderPass( ctx->getRenderPass()->getRenderPass() );
	pipelineSelector->setSubPass( ctx->getSubpass() );
	pipelineSelector->setPipelineLayout( mPipelineLayout->getPipelineLayout() );
	auto pipeline = pipelineSelector->getSelectedPipeline();
	vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );

	uint32_t vertexCount = 4;
	vkCmdDraw( cmdBuf, vertexCount, 1, 0, 0 );
}

Billboard::Billboard( const ci::vk::ShaderProgRef& shaderProg )
	: SolidRect( shaderProg )
{

}

void Billboard::set( const ci::vec3 &pos, const ci::vec2 &scale, float rotationRadians, const ci::vec3 &bbRight, const ci::vec3 &bbUp, const ci::Rectf &texCoords )
{
	float data[12+8]; // both verts and texCoords
	ci::vec3 *verts = (ci::vec3*)data;
	float *texCoordsOut = data + 12;

	float sinA = ci::math<float>::sin( rotationRadians );
	float cosA =ci:: math<float>::cos( rotationRadians );

	verts[0] = pos + bbRight * ( -0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	texCoordsOut[0*2+0] = texCoords.getX1(); texCoordsOut[0*2+1] = texCoords.getY1();
	verts[1] = pos + bbRight * ( -0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
	texCoordsOut[1*2+0] = texCoords.getX1(); texCoordsOut[1*2+1] = texCoords.getY2();
	verts[2] = pos + bbRight * ( 0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	texCoordsOut[2*2+0] = texCoords.getX2(); texCoordsOut[2*2+1] = texCoords.getY1();
	verts[3] = pos + bbRight * ( 0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
	texCoordsOut[3*2+0] = texCoords.getX2(); texCoordsOut[3*2+1] = texCoords.getY2();

	// Triangle strip
	mData = {
		verts[0].x, verts[0].y, verts[0].z, 1.0f, texCoords.x1, texCoords.y1,
		verts[1].x, verts[1].y, verts[1].z, 1.0f, texCoords.x1, texCoords.y2,
		verts[2].x, verts[2].y, verts[2].z, 1.0f, texCoords.x2, texCoords.y1,
		verts[3].x, verts[3].y, verts[3].z, 1.0f, texCoords.x2, texCoords.y2
	};

	// Buffer data
	mVertexBuffer->bufferData( mData.size()*sizeof( float ), static_cast<const void*>( mData.data() ) );
}
