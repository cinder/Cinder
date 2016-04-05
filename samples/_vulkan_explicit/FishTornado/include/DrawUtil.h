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

#pragma once

#include "cinder/vk/vk.h"

class Billboard;
class SolidRect;
using BillboardRef = std::shared_ptr<Billboard>;
using SolidRectRef = std::shared_ptr<SolidRect>;

class SolidRect {
public:

	SolidRect() {}
	SolidRect( const ci::vk::ShaderProgRef& shaderProg );
	virtual ~SolidRect();

	void							set( const ci::Rectf& rect );
	void							draw( const ci::vk::CommandBufferRef& commandBuffer );
	const ci::vk::UniformSetRef&	getUniformSet() const { return mUniformSet; }

protected:
	ci::vk::VertexBufferRef							mVertexBuffer;
	ci::vk::UniformSetRef							mUniformSet;
	ci::vk::DescriptorSetViewRef					mDescriptorSetView;
	//ci::vk::DescriptorSetLayoutRef				mDescriptorSetLayout;
	//ci::vk::DescriptorPoolRef						mDescriptorPool;
	//ci::vk::DescriptorSetRef						mDescriptorSet;
	ci::vk::PipelineLayoutRef						mPipelineLayout;
	std::vector<VkVertexInputBindingDescription>	mVertexBindings;
	std::vector<VkVertexInputAttributeDescription>	mVertexAttributes;
	ci::vk::ShaderProgRef							mShader;
	ci::Rectf										mPosRect;
	ci::Rectf										mTexCoordRect;
	std::vector<float>								mData;
	void initialize();
};

class Billboard : public SolidRect {
public:

	Billboard() {}
	Billboard( const ci::vk::ShaderProgRef& shaderProg );
	virtual ~Billboard() {}

	void	set( const ci::vec3 &pos, const ci::vec2 &scale, float rotationRadians, const ci::vec3 &bbRight, const ci::vec3 &bbUp, const ci::Rectf &texCoords = ci::Rectf( 0, 0, 1, 1 ) );
};