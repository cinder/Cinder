#pragma once

#include "cinder/vk/vk.h"
using namespace ci;

class Test {
public:
	virtual void	setup() = 0;
	virtual void	update() = 0;
	virtual void	draw() = 0;

	void			disable() { mEnabled = false; }
	
protected:
	bool						mEnabled = true;

	vk::TextureRef				mTexture;
	vk::VertexBufferRef			mVertexBuffer;
	vk::GlslProgRef				mShader;
	vk::UniformSetRef			mUniformSet;

	VkDescriptorSetLayout		mDescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool			mDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet				mDescriptorSet = VK_NULL_HANDLE;
	vk::PipelineLayoutRef		mPipelineLayout;
	VkPipeline					mPipeline = VK_NULL_HANDLE;
};