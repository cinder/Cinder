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

#include "cinder/vk/Batch.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/Context.h"
#include "cinder/vk/Descriptor.h"
#include "cinder/vk/Device.h"
#include "cinder/vk/Pipeline.h"
#include "cinder/vk/PipelineSelector.h"
#include "cinder/vk/RenderPass.h"
#include "cinder/vk/ShaderProg.h"
#include "cinder/vk/UniformBuffer.h"
#include "cinder/vk/wrapper.h"

namespace cinder { namespace vk { 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Batch
BatchRef Batch::create( const VboMeshRef &vboMesh, const vk::ShaderProgRef &shader, const AttributeMapping &attributeMapping )
{
	return BatchRef( new Batch( vboMesh, shader, attributeMapping ) );
}

BatchRef Batch::create( const geom::Source &source, const vk::ShaderProgRef &shader, const AttributeMapping &attributeMapping )
{
	return BatchRef( new Batch( source, shader, attributeMapping ) );
}

Batch::Batch( const VboMeshRef &vboMesh, const vk::ShaderProgRef &shader, const AttributeMapping &attributeMapping )
	: mShader( shader ), mVboMesh( vboMesh )
{
	initPipeline( attributeMapping );
}

Batch::Batch( const geom::Source &source, const vk::ShaderProgRef &shader, const AttributeMapping &attributeMapping )
	: mShader( shader )
{
	VertexBufferMesh::Layout vertexLayout;
	// include all the attributes in the custom attributeMapping
	for( const auto &attrib : attributeMapping ) {
		if( source.getAttribDims( attrib.first ) ) {
			geom::Attrib semantic = attrib.first;
			uint8_t dim = source.getAttribDims( semantic );
			vertexLayout.attrib( semantic, dim );
		}
	}
	// and then the attributes references by the GLSL
	for( const auto &attrib : shader->getActiveAttributes() ) {
		geom::Attrib semantic = attrib.getSemantic();
		uint8_t dim = glslAttributeTypeDim( attrib.getType() );
		if( dim ) {
			vertexLayout.attrib( semantic, dim );
		}
	}

	std::vector<VertexBufferMesh::Layout> vertexLayouts = { vertexLayout };
	mVboMesh = vk::VertexBufferMesh::create( source, vertexLayouts );
	initPipeline( attributeMapping );
}

void Batch::initPipeline( const AttributeMapping &attributeMapping )
{
	mVertexInputDescription = mVboMesh->getVertexInputDescription();

	if( ! attributeMapping.empty() ) {
		for( const auto& semantic : mVertexInputDescription.getSemanticAttributes() ) {
			auto it = mAttribMapping.find( semantic );
			if( mAttribMapping.end() == it ) {
				continue;
			}
			auto attr = mVertexInputDescription.getAttribute( semantic );
			attr->location = it->second.first;
			attr->binding  = it->second.second;
		}
	}
	else {
		mVertexInputDescription.setAttributeLocationsAndBindings( mShader );
	}

	// Uniform buffer
	mUniformSet = vk::UniformSet::create( mShader->getUniformLayout() );

	// Descriptor layout, pool, set
	mDescriptorSetLayout = vk::DescriptorSetLayout::create( *mUniformSet );
	mDescriptorPool = vk::DescriptorPool::create( mDescriptorSetLayout->getLayoutBindings() );
	//mDescriptorSet = vk::DescriptorSet::create( mDescriptorPool->getDescriptorPool(), mUniformLayout, mDescriptorSetLayout );

	// Pipeline layout
	mPipelineLayout = vk::PipelineLayout::create( mDescriptorSetLayout );

/*
	// Pipeline
	vk::Pipeline::Options pipelineOptions;
	pipelineOptions.setTopology( mVboMesh->getPrimitive() );
	pipelineOptions.setPipelineLayout( mPipelineLayout );
	pipelineOptions.setRenderPass( vk::context()->getRenderPass() );
	pipelineOptions.setShaderProg( mShader );
	{
		auto bindings = mVertexInputDescription.getBindings();
		for( const auto& binding : bindings ) {
			pipelineOptions.addVertexBinding( binding );
		}
		//
		auto attributes = mVertexInputDescription.getAttributes();
		for( const auto& attr : attributes ) {
			pipelineOptions.addVertexAtrribute( attr );
		}
	}	
	// Cull none	
	pipelineOptions.setCullModeNone();
	mPipelines[VK_CULL_MODE_NONE] = vk::Pipeline::create( pipelineOptions, nullptr );
	// Cull front
	pipelineOptions.setCullModeFront();
	mPipelines[VK_CULL_MODE_FRONT_BIT] = vk::Pipeline::create( pipelineOptions, nullptr );
	// Cull back
	pipelineOptions.setCullModeBack();
	mPipelines[VK_CULL_MODE_BACK_BIT] = vk::Pipeline::create( pipelineOptions, nullptr );
	// Cull front and back
	pipelineOptions.setCullModeFrontAndBack();
	mPipelines[VK_CULL_MODE_FRONT_AND_BACK] = vk::Pipeline::create( pipelineOptions, nullptr );
*/
}

void Batch::destroyPipeline()
{
	//if( mUniformLayout ) {
	//	mUniformLayout->~UniformBuffer();
	//	mUniformLayout.reset();
	//}

	//if( mPipeline ) {
	//	mPipeline->~Pipeline();
	//	mPipeline.reset();
	//}
	//for( auto& it : mPipelines ) {
	//	auto& pipeline = it.second;
	//	pipeline->~Pipeline();
	//	pipeline.reset();		
	//}

	if( mPipelineLayout ) {
		mPipelineLayout->~PipelineLayout();
		mPipelineLayout.reset();
	}

	if( mDescriptorSet ) {
		mDescriptorSet->~DescriptorSet();
		mDescriptorSet.reset();
	}

	if( mDescriptorPool ) {
		mDescriptorPool->~DescriptorPool();
		mDescriptorPool.reset();
	}

	if( mDescriptorSetLayout ) {
		mDescriptorSetLayout->~DescriptorSetLayout();
		mDescriptorSetLayout.reset();
	}
}

void Batch::replaceGlslProg( const vk::ShaderProgRef& shader )
{
	mShader = shader;
	destroyPipeline();
	initPipeline( mAttribMapping );
}

void Batch::replaceVboMesh( const VboMeshRef &vboMesh )
{
	mVboMesh = vboMesh;
	destroyPipeline();
	initPipeline( mAttribMapping );
}

void Batch::uniform( const std::string& name, const float value )
{
	mUniformSet->uniform( name, value );
}

void Batch::uniform( const std::string& name, const int32_t value )
{
	mUniformSet->uniform( name, value );
}

void Batch::uniform( const std::string& name, const uint32_t value )
{
	mUniformSet->uniform( name, value );
}

void Batch::uniform( const std::string& name, const bool value )
{
	mUniformSet->uniform( name, value );
}

void Batch::uniform( const std::string& name, const vec2& value )
{
	mUniformSet->uniform( name, value );
}

void Batch::uniform( const std::string& name, const vec3& value )
{
	mUniformSet->uniform( name, value );
}

void Batch::uniform( const std::string& name, const vec4& value )
{
	mUniformSet->uniform( name, value );
}

void Batch::uniform( const std::string& name, const mat2& value )
{
	mUniformSet->uniform( name, value );
}

void Batch::uniform( const std::string& name, const mat3& value )
{
	mUniformSet->uniform( name, value );
}

void Batch::uniform( const std::string& name, const mat4& value )
{
	mUniformSet->uniform( name, value );
}

void Batch::uniform( const std::string& name, const TextureBaseRef& texture )
{
	mUniformSet->uniform( name, texture );
}

/*
void Batch::sampler2D( const std::string& name, const TextureBaseRef& texture )
{
	mUniformSet->sampler2D( name, texture );
}

void Batch::sampler2DRect( const std::string& name, const TextureBaseRef& texture )
{
	mUniformSet->sampler2DRect( name, texture );
}

void Batch::samplerCube( const std::string& name, const TextureBaseRef& texture )
{
	mUniformSet->samplerCube( name, texture );
}
*/

void Batch::draw( int32_t first, int32_t count )
{
	if( ! mDescriptorSet ) {
		// The descriptor set is created here to give users a chance to set uniforms in 
		// mUniformSet before the first draw call.
		mDescriptorSet = vk::DescriptorSet::create( mDescriptorPool->getDescriptorPool(), *mUniformSet, mDescriptorSetLayout );
	}

	for( auto& binding : mUniformSet->getBindings() ) {
		if( ! binding.isBlock() ) {
			continue;
		}
		vk::context()->setDefaultUniformVars( binding.getUniformBuffer() );
	}
	mUniformSet->bufferPending();

	auto cmdBuf = vk::context()->getCommandBuffer()->getCommandBuffer();

	bool useIndexBuffer = mVboMesh->getIndexVbo() ? true : false;
	if(  useIndexBuffer) {
		auto indexBuffer = mVboMesh->getIndexVbo()->getBuffer();
		auto indexType = mVboMesh->getIndexVbo()->getIndexType();
		vkCmdBindIndexBuffer( cmdBuf, indexBuffer, 0,indexType );
	}

	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> offsets;
	for( const auto& vb : mVboMesh->getVertexArrayVbos() ) {
		vertexBuffers.push_back( vb->getBuffer() );
		offsets.push_back( 0 );
	}
	vkCmdBindVertexBuffers( cmdBuf, 0, static_cast<uint32_t>( vertexBuffers.size() ), vertexBuffers.data(), offsets.data() );
	
	//auto& pipeline = mPipelines[mPipelineSelection];
	//vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline() );

	// Update descriptor set
	mDescriptorSet->update( *mUniformSet );

	std::vector<VkDescriptorSet> descSets = { mDescriptorSet->getDescriptorSet() };
	vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getPipelineLayout(), 0, static_cast<uint32_t>( descSets.size() ), descSets.data(), 0, nullptr );

	//auto& pipeline = mPipelines[mPipelineSelection];
	//vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline() );
	auto ctx = vk::context();
	auto& pipelineSelector = ctx->getDevice()->getPipelineSelector();
	pipelineSelector->setTopology( mVboMesh->getPrimitive() );
	pipelineSelector->setVertexInputBindingDescriptions( mVertexInputDescription.getBindings() );
	pipelineSelector->setVertexInputAttributeDescriptions( mVertexInputDescription.getAttributes() );
	pipelineSelector->setCullMode( ctx->getCullMode() );
	pipelineSelector->setFrontFace( ctx->getFrontFace() );
	pipelineSelector->setDepthBias( ctx->getDepthBiasEnable(), ctx->getDepthBiasSlopeFactor(), ctx->getDepthBiasConstantFactor(), ctx->getDepthBiasClamp() );
	pipelineSelector->setRasterizationSamples( ctx->getRenderPass()->getSubPassSampleCount( ctx->getSubPass() ) );
	pipelineSelector->setDepthTest( ctx->getDepthTest() );
	pipelineSelector->setDepthWrite( ctx->getDepthWrite() );
	pipelineSelector->setColorBlendAttachments( ctx->getColorBlendAttachments() );
	pipelineSelector->setShaderStages( mShader->getShaderStages() );
	pipelineSelector->setRenderPass( ctx->getRenderPass()->getRenderPass() );
	pipelineSelector->setSubPass( ctx->getSubPass() );
	pipelineSelector->setPipelineLayout( mPipelineLayout->getPipelineLayout() );
	auto pipeline = pipelineSelector->getSelectedPipeline();
	vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );

	if( useIndexBuffer ) {
		int32_t indexCount = mVboMesh->getNumIndices();
		vkCmdDrawIndexed( cmdBuf, count < 0 ? indexCount : count, 1, 0, 0, 0 );	
	}
	else {
		uint32_t vertexCount = mVboMesh->getNumVertices();
		vkCmdDraw( cmdBuf, vertexCount, 1, 0, 0 );
	}
}

void Batch::drawInstanced( uint32_t instanceCount )
{
	if( ! mDescriptorSet ) {
		mDescriptorSet = vk::DescriptorSet::create( mDescriptorPool->getDescriptorPool(), *mUniformSet, mDescriptorSetLayout );
	}

	for( auto& binding : mUniformSet->getBindings() ) {
		if( ! binding.isBlock() ) {
			continue;
		}
		vk::context()->setDefaultUniformVars( binding.getUniformBuffer() );
	}
	mUniformSet->bufferPending();

	auto cmdBuf = vk::context()->getCommandBuffer()->getCommandBuffer();

	bool useIndexBuffer = mVboMesh->getIndexVbo() ? true : false;
	if(  useIndexBuffer) {
		auto indexBuffer = mVboMesh->getIndexVbo()->getBuffer();
		auto indexType = mVboMesh->getIndexVbo()->getIndexType();
		vkCmdBindIndexBuffer( cmdBuf, indexBuffer, 0,indexType );
	}

	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceSize> offsets;
	for( const auto& vb : mVboMesh->getVertexArrayVbos() ) {
		vertexBuffers.push_back( vb->getBuffer() );
		offsets.push_back( 0 );
	}
	vkCmdBindVertexBuffers( cmdBuf, 0, static_cast<uint32_t>( vertexBuffers.size() ), vertexBuffers.data(), offsets.data() );
	
	//auto& pipeline = mPipelines[mPipelineSelection];
	//vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline() );

	// Update descriptor set
	mDescriptorSet->update( *mUniformSet );

	std::vector<VkDescriptorSet> descSets = { mDescriptorSet->getDescriptorSet() };
	vkCmdBindDescriptorSets( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout->getPipelineLayout(), 0, static_cast<uint32_t>( descSets.size() ), descSets.data(), 0, nullptr );

	//auto& pipeline = mPipelines[mPipelineSelection];
	//vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getPipeline() );
	auto& pipelineSelector = vk::context()->getDevice()->getPipelineSelector();
	pipelineSelector->setVertexInputBindingDescriptions( mVertexInputDescription.getBindings() );
	pipelineSelector->setVertexInputAttributeDescriptions( mVertexInputDescription.getAttributes() );
	pipelineSelector->setShaderStages( mShader->getShaderStages() );
	pipelineSelector->setRenderPass( vk::context()->getRenderPass()->getRenderPass() );
	pipelineSelector->setSubPass( 0 );
	pipelineSelector->setPipelineLayout( mPipelineLayout->getPipelineLayout() );
	auto pipeline = pipelineSelector->getSelectedPipeline();
	vkCmdBindPipeline( cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );

	if( useIndexBuffer ) {
		int32_t indexCount = mVboMesh->getNumIndices();
		vkCmdDrawIndexed( cmdBuf, indexCount, instanceCount, 0, 0, 0 );	
	}
	else {
		uint32_t vertexCount = mVboMesh->getNumVertices();
		vkCmdDraw( cmdBuf, vertexCount, instanceCount, 0, 0 );
	}
}

/*
void Batch::bind()
{
	mGlsl->bind();
	mVao->bind();
}
*/

/*
void Batch::reassignContext( Context *context )
{
	mVao->reassignContext( context );
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// VertBatch
//VertBatch::VertBatch( VkPrimitiveTopology primType, bool useContextDefaultBuffers )
//	: mPrimType( primType )
//{
//	mOwnsBuffers = true;
//
//	/*
//	if( useContextDefaultBuffers ) {
//		auto ctx = gl::context();
//		mVao = ctx->getDefaultVao();
//		mVbo = ctx->getDefaultArrayVbo();
//		mOwnsBuffers = false;
//	}
//	else
//		mOwnsBuffers = true;
//	*/
//}
//
//VertBatchRef VertBatch::create( VkPrimitiveTopology primType, bool useContextDefaultBuffers )
//{
//	return VertBatchRef( new VertBatch( primType, useContextDefaultBuffers ) ); 
//}
//
//void VertBatch::setType( VkPrimitiveTopology primType )
//{
//	mPrimType = primType;
//}
//
//void VertBatch::normal( const vec3 &n )
//{
//	mNormals.push_back( n );
//}
//
//void VertBatch::color( const Colorf &color )
//{
//	mColors.push_back( color );
//}
//
//void VertBatch::color( const ColorAf &color )
//{
//	mColors.push_back( color );
//}
//
//void VertBatch::texCoord0( const vec4 &t )
//{
//	mTexCoords0.push_back( t );
//}
//
//void VertBatch::texCoord1( const vec4 &t )
//{
//	mTexCoords1.push_back( t );
//}
//
//void VertBatch::vertex( const vec4 &v )
//{
//	addVertex( v );
//}
//
//void VertBatch::vertex( const vec4 &v, const ColorAf &c )
//{
//	mColors.push_back( c );
//	addVertex( v );
//}
//
//void VertBatch::addVertex( const vec4 &v )
//{
//	mVertices.push_back( v );
//
//	if( ! mNormals.empty() ) {
//		while( mNormals.size() < mVertices.size() )
//			mNormals.push_back( mNormals.back() );
//	}
//	
//	if( ! mColors.empty() ) {
//		while( mColors.size() < mVertices.size() )
//			mColors.push_back( mColors.back() );	
//	}
//
//	if( ! mTexCoords0.empty() ) {
//		while( mTexCoords0.size() < mVertices.size() )
//			mTexCoords0.push_back( mTexCoords0.back() );
//	}
//
//	if( ! mTexCoords1.empty() ) {
//		while( mTexCoords1.size() < mVertices.size() )
//			mTexCoords1.push_back( mTexCoords1.back() );
//	}
//}
//
//void VertBatch::begin( VkPrimitiveTopology primType )
//{
//	clear();
//	mPrimType = primType;
//}
//
//void VertBatch::clear()
//{
//	mVertices.clear();
//	mNormals.clear();
//	mColors.clear();
//	mTexCoords0.clear();
//	mTexCoords1.clear();
//	mVertexBuffer.reset();
///*
//	mVao = nullptr;
//*/
//}
//
//void VertBatch::draw()
//{
//
//	/*
//	// this pushes the VAO, which needs to be popped
//	setupBuffers();
//	ScopedVao vao( mVao );
//	
//	auto ctx = context();
//	ctx->setDefaultShaderVars();
//	ctx->drawArrays( mPrimType, 0, (GLsizei)mVertices.size() );
//	*/
//}
//
//// Leaves mVAO bound
//void VertBatch::setupBuffers()
//{
///*
//	auto ctx = gl::context();
//*/
//	
///*
//	auto glslProg = ctx->getGlslProg();
//	if( ! glslProg )
//		return;
//*/
//
//	const size_t verticesSizeBytes = mVertices.size() * sizeof(vec4);
//	const size_t normalsSizeBytes = mNormals.size() * sizeof(vec3);
//	const size_t colorsSizeBytes = mColors.size() * sizeof(ColorAf);
//	const size_t texCoords0SizeBytes = mTexCoords0.size() * sizeof(vec4);
//	const size_t texCoords1SizeBytes = mTexCoords1.size() * sizeof(vec4);
//	size_t totalSizeBytes = verticesSizeBytes + normalsSizeBytes + colorsSizeBytes + texCoords0SizeBytes + texCoords1SizeBytes;
//
//	// allocate the VBO if we don't have one yet (which implies we're not using the context defaults)
//	bool forceUpload = false;
//	if( ! mVertexBuffer ) {
//		// allocate the VBO and upload the data
//		mVertexBuffer = vk::VertexBuffer::create( nullptr, totalSizeBytes );
//		//mVbo = gl::Vbo::create( GL_ARRAY_BUFFER, totalSizeBytes );
//		forceUpload = true;
//	}
//	
//	//ScopedBuffer ScopedBuffer( mVbo );
//	// if this VBO was freshly made, or we don't own the buffer because we use the context defaults
//	if( ( forceUpload || ( ! mOwnsBuffers ) ) && ( ! mVertices.empty() ) ) {
//		mVertexBuffer->ensureMinimumSize( totalSizeBytes );
//		
//		// upload positions
//		VkDeviceSize offset = 0;
//		mVertexBuffer->bufferSubData( offset, verticesSizeBytes, &mVertices[0] );
//		offset += verticesSizeBytes;
//		
//		// upload normals
//		if( ! mNormals.empty() ) {
//			mVertexBuffer->bufferSubData( offset, normalsSizeBytes, &mNormals[0] );
//			offset += normalsSizeBytes;
//		}
//
//		// upload colors
//		if( ! mColors.empty() ) {
//			mVertexBuffer->bufferSubData( offset, colorsSizeBytes, &mColors[0] );
//			offset += colorsSizeBytes;
//		}
//
//		// upload texCoords0
//		if( ! mTexCoords0.empty() ) {
//			mVertexBuffer->bufferSubData( offset, texCoords0SizeBytes, &mTexCoords0[0] );
//			offset += texCoords0SizeBytes;
//		}
//
//		// upload texCoords1
//		if( ! mTexCoords1.empty() ) {
//			mVertexBuffer->bufferSubData( offset, texCoords1SizeBytes, &mTexCoords1[0] );
//			offset += texCoords1SizeBytes;
//		}
//	}
//
///*
//	// Setup the VAO
//	ctx->pushVao(); // this will be popped by draw()
//	if( ! mOwnsBuffers ) {
//		mVao->replacementBindBegin();
//	}
//	else {
//		mVaoStorage = gl::Vao::create();
//		mVao = mVaoStorage.get();
//		mVao->bind();
//	}
//*/
//
///*
//	gl::ScopedBuffer vboScope( mVbo );
//	size_t offset = 0;
//	if( glslProg->hasAttribSemantic( geom::Attrib::POSITION ) ) {
//		int loc = glslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
//		ctx->enableVertexAttribArray( loc );
//		ctx->vertexAttribPointer( loc, 4, GL_FLOAT, false, 0, (const GLvoid*)offset );
//		offset += verticesSizeBytes;
//	}
//
//	if( glslProg->hasAttribSemantic( geom::Attrib::NORMAL ) && ( ! mNormals.empty() ) ) {
//		int loc = glslProg->getAttribSemanticLocation( geom::Attrib::NORMAL );
//		ctx->enableVertexAttribArray( loc );
//		ctx->vertexAttribPointer( loc, 3, GL_FLOAT, false, 0, (const GLvoid*)offset );
//		offset += normalsSizeBytes;
//	}
//
//	if( glslProg->hasAttribSemantic( geom::Attrib::COLOR ) && ( ! mColors.empty() ) ) {
//		int loc = glslProg->getAttribSemanticLocation( geom::Attrib::COLOR );
//		ctx->enableVertexAttribArray( loc );
//		ctx->vertexAttribPointer( loc, 4, GL_FLOAT, false, 0, (const GLvoid*)offset );
//		offset += colorsSizeBytes;
//	}
//
//	if( glslProg->hasAttribSemantic( geom::Attrib::TEX_COORD_0 ) && ( ! mTexCoords0.empty() ) ) {
//		int loc = glslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
//		ctx->enableVertexAttribArray( loc );
//		ctx->vertexAttribPointer( loc, 4, GL_FLOAT, false, 0, (const GLvoid*)offset );
//		offset += texCoords0SizeBytes;
//	}
//
//	if( glslProg->hasAttribSemantic( geom::Attrib::TEX_COORD_1 ) && ( ! mTexCoords1.empty() ) ) {
//		int loc = glslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_1 );
//		ctx->enableVertexAttribArray( loc );
//		ctx->vertexAttribPointer( loc, 4, GL_FLOAT, false, 0, (const GLvoid*)offset );
//	}
//*/
//	
///*
//	if( ! mOwnsBuffers ) {
//		mVao->replacementBindEnd();
//	}
//	ctx->popVao();
//*/
//}
//
//size_t VertBatch::getNumVertices() const
//{
//	return mVertices.size();
//}
//
//size_t VertBatch::getNumIndices() const
//{
//	return 0;
//}
//
//geom::Primitive	VertBatch::getPrimitive() const
//{
//	return vk::toGeomPrimitive( mPrimType );
//}
//
//uint8_t	VertBatch::getAttribDims( geom::Attrib attr ) const
//{
//	switch( attr ) {
//		case geom::Attrib::POSITION:
//			return mVertices.empty() ? 0 :4;
//		case geom::Attrib::NORMAL:
//			return mNormals.empty() ? 0 : 3;
//		case geom::Attrib::COLOR:
//			return mColors.empty() ? 0 : 4;
//		case geom::Attrib::TEX_COORD_0:
//			return mTexCoords0.empty() ? 0 : 4;
//		case geom::Attrib::TEX_COORD_1:
//			return mTexCoords1.empty() ? 0 : 4;
//		default:
//			return 0;
//	}
//}
//
//void VertBatch::loadInto( geom::Target *target, const geom::AttribSet &requestedAttribs ) const
//{
//	if( ! mVertices.empty() )
//		target->copyAttrib( geom::Attrib::POSITION, 4, 0, value_ptr( *mVertices.data() ), mVertices.size() );
//	if( ! mNormals.empty() )
//		target->copyAttrib( geom::Attrib::NORMAL, 3, 0, value_ptr( *mNormals.data() ), mNormals.size() );
//	if( ! mColors.empty() )
//		target->copyAttrib( geom::Attrib::COLOR, 4, 0, (const float*)mColors.data(), mColors.size() );
//	if( ! mTexCoords0.empty() )
//		target->copyAttrib( geom::Attrib::TEX_COORD_0, 4, 0, value_ptr( *mTexCoords0.data() ), mTexCoords0.size() );
//	if( ! mTexCoords1.empty() )
//		target->copyAttrib( geom::Attrib::TEX_COORD_1, 4, 0, value_ptr( *mTexCoords1.data() ), mTexCoords1.size() );
//}
//
//geom::AttribSet	VertBatch::getAvailableAttribs() const
//{
//	geom::AttribSet attribs;
//	attribs.insert( geom::Attrib::POSITION );
//	
//	if( ! mNormals.empty() )
//		attribs.insert( geom::Attrib::NORMAL );
//	if( ! mColors.empty() )
//		attribs.insert( geom::Attrib::COLOR );
//	if( ! mTexCoords0.empty() )
//		attribs.insert( geom::Attrib::TEX_COORD_0 );
//	if( ! mTexCoords1.empty() )
//		attribs.insert( geom::Attrib::TEX_COORD_1 );
//	
//	return attribs;
//}

}} // namespace cinder::vk