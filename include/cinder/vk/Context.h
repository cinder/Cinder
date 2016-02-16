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
#include "cinder/Color.h"
#include "cinder/Matrix.h"
#include "cinder/Vector.h"

#include <map>

namespace cinder { namespace app {

class RendererVk;

}} // namespace cinder::app

namespace cinder { namespace vk {

class Buffer;
class CommandBuffer;
class CommandPool;
class DescriptorPool;
class DescriptorSet;
class DescriptorSetLayout;
class Environment;
class Framebuffer;
class Image;
class ImageView;
class IndexBuffer;
class Pipeline;
class PipelineCache;
class PipelineLayout;
class PipelineSelector;
class RenderPass;
class ShaderDef;
class ShaderProg;
class Swapchain;
class Texture2d;
class UniformBuffer;
class VertexBuffer;
using BufferRef = std::shared_ptr<Buffer>;
using CommandBufferRef = std::shared_ptr<CommandBuffer>;
using CommandPoolRef = std::shared_ptr<CommandPool>;
using FramebufferRef = std::shared_ptr<Framebuffer>;
using DescriptorPoolRef = std::shared_ptr<DescriptorPool>;
using DescriptorSetRef = std::shared_ptr<DescriptorSet>;
using DescriptorSetLayoutRef = std::shared_ptr<DescriptorSetLayout>;
using ImageRef = std::shared_ptr<Image>;
using ImageViewRef = std::shared_ptr<ImageView>;
using IndexBufferRef = std::shared_ptr<IndexBuffer>;
using PipelineRef = std::shared_ptr<Pipeline>;
using PipelineCacheRef = std::shared_ptr<PipelineCache>;
using PipelineLayoutRef = std::shared_ptr<PipelineLayout>;
using PipelineSelectorRef = std::shared_ptr<PipelineSelector>;
using RenderPassRef = std::shared_ptr<RenderPass>;
using ShaderProgRef = std::shared_ptr<ShaderProg>;
using SwapchainRef = std::shared_ptr<Swapchain>;
using Texture2dRef = std::shared_ptr<Texture2d>;
using UniformBufferRef = std::shared_ptr<UniformBuffer>;
using VertexBufferRef = std::shared_ptr<VertexBuffer>;

class Context;
using ContextRef = std::shared_ptr<Context>;

//! Cap enums
//!
//!
enum class Cap {
	BLEND,
	DEPTH_BIAS
};

//! \class Context
//! 
//!
class Context {
public:
	enum class Type { UNDEFINED, PRIMARY, SECONDARY };

	Context();
#if defined( CINDER_LINUX )
#elif defined( CINDER_MSW )
	Context( ::HINSTANCE connection, ::HWND window, bool explicitMode, uint32_t workQueueCount, VkPhysicalDevice gpu, Environment *env = nullptr );
#endif
	Context( const Context* existingContext, int queueIndex );
	virtual ~Context();

	static ContextRef						createFromExisting( const vk::Context* existingContext, int queueIndex );

	Environment*							getEnvironment() const { return mEnvironment; }
	Context::Type							getType() const { return mType; }
	bool									isPrimary() const { return Context::Type::PRIMARY == getType(); }
	bool									isSecondary() const { return Context::Type::SECONDARY == getType(); }

	static Context*							getCurrent();
	void									makeCurrent();

	bool									isExplicitMode() const { return mExplicitMode; }
	
	VkPhysicalDevice						getGpu() const { return mGpu; }
	VkDevice								getDevice() const { return mDevice; }
	VkQueue									getQueue() const { return mQueue; }

	uint32_t								getGraphicsQueueFamilyIndex() const { return mGraphicsQueueFamilyIndex; }
	VkPhysicalDeviceProperties				getGpuProperties() const { return mGpuProperties; }
	std::vector<VkQueueFamilyProperties>	getQueueProperties() const { return mQueueFamilyProperties; }
	VkPhysicalDeviceMemoryProperties		getMemoryProperties() const { return mMemoryProperties; }

	uint32_t								getQueueCount() const { return mQueueFamilyPropertyCount; }

	bool									findMemoryType( uint32_t typeBits, VkFlags requirementsMask, uint32_t *typeIndex ) const;

	const vk::CommandPoolRef&				getDefaultCommandPool() const { return mDefaultCommandPool; }
	const vk::CommandBufferRef&				getDefaultCommandBuffer() const { return mDefaultCommandBuffer; }

	const vk::PipelineCacheRef&				getPipelineCache() const { return mPipelineCache; }
	const vk::PipelineSelectorRef&			getPipelineSelector() const { return mPipelineSelector; }

	VkSurfaceKHR							getPresentSurface() const { return mPresentSurface; }
	VkFormat								getPresentColorFormat() const { return mPresentColorFormat; }
	VkFormat								getPresentDepthStencilFormat() const { return mPresentDepthStencilFormat; }

	void									acquireNextPresentImage( VkFence fence = VK_NULL_HANDLE );
	void									beginPresentRender();
	void									endPresentRender();
	void									submitPresentRender();
	void									setPresentCommandBuffer( const vk::CommandBufferRef& cmdBuf );
	const vk::RenderPassRef&				getPresentRenderPass() const { return mPresentRender->mRenderPass; }

	VkResult	vkCreateSwapchainKHR( const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain );
	void		vkDestroySwapchainKHR( VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator );
	VkResult	vkGetSwapchainImagesKHR( VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages );
	VkResult	vkAcquireNextImageKHR( VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex );
	//VkResult	vkAcquireNextImageKHR( VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence );
	VkResult	vkQueuePresentKHR( const VkPresentInfoKHR* pPresentInfo );

	void						pushRenderPass( const vk::RenderPassRef& renderPass );
	void						popRenderPass();
	const vk::RenderPassRef&	getRenderPass() const;

	void						pushSubPass( uint32_t subPass );
	void						popSubPass();
	void						setSubPass( uint32_t subPass );
	uint32_t					getSubPass() const;

	void						pushFramebuffer( const vk::FramebufferRef& framebuffer );
	void						popFramebuffer();
	const vk::FramebufferRef&	getFramebuffer() const;

	void						pushImage( const vk::ImageRef& image );
	void						popImage();
	const vk::ImageRef&			getImage() const;

	void						pushCommandBuffer( const vk::CommandBufferRef& cmdBuf );
	void						popCommandBuffer();
	const vk::CommandBufferRef&	getCommandBuffer() const;
	bool						hasCommandBuffer() const { return mCommandBufferStack.empty() ? false : true; }

	void						pushShaderProg( const vk::ShaderProgRef& shaderProg );
	void						popShaderProg();
	const vk::ShaderProgRef&	getShaderProg() const;
	bool						hasShaderProg() const {  return mShaderProgStack.empty() ? false : true; }

	//! Returns a reference to the stack of Model matrices
	std::vector<mat4>&			getModelMatrixStack() { return mModelMatrixStack; }
	//! Returns a const reference to the stack of Model matrices
	const std::vector<mat4>&	getModelMatrixStack() const { return mModelMatrixStack; }
	//! Returns a reference to the stack of View matrices
	std::vector<mat4>&			getViewMatrixStack() { return mViewMatrixStack; }
	//! Returns a const reference to the stack of Model matrices
	const std::vector<mat4>&	getViewMatrixStack() const { return mViewMatrixStack; }
	//! Returns a reference to the stack of Projection matrices
	std::vector<mat4>&			getProjectionMatrixStack() { return mProjectionMatrixStack; }
	//! Returns a const reference to the stack of Projection matrices
	const std::vector<mat4>&	getProjectionMatrixStack() const { return mProjectionMatrixStack; }

	//! Analogous to glViewport(). Sets the viewport based on a pair<ivec2,ivec2> representing the position of the lower-left corner and the size, respectively
	void						viewport( const std::pair<ivec2, ivec2> &viewport );
	//! Pushes the viewport based on a pair<ivec2,ivec2> representing the position of the lower-left corner and the size, respectively
	void						pushViewport( const std::pair<ivec2, ivec2> &viewport );
	//! Duplicates and pushes the top of the Viewport stack
	void						pushViewport();
	//! Pops the viewport. If \a forceRestore then redundancy checks are skipped and the hardware state is always set.
	void						popViewport( bool forceRestore = false );
	//! Returns a pair<ivec2,ivec2> representing the position of the lower-left corner and the size, respectively of the viewport
	std::pair<ivec2,ivec2>		getViewport();

	//! Sets the scissor box based on a pair<ivec2,ivec2> representing the position of the lower-left corner and the size, respectively	
	void						setScissor( const std::pair<ivec2, ivec2> &scissor );
	//! Pushes the scissor box based on a pair<ivec2,ivec2> representing the position of the lower-left corner and the size, respectively	
	void						pushScissor( const std::pair<ivec2, ivec2> &scissor );
	//! Duplicates and pushes the top of the Scissor box stack
	void						pushScissor();
	//! Pushes the scissor box based on a pair<ivec2,ivec2> representing the position of the lower-left corner and the size, respectively. If \a forceRestore then redundancy checks are skipped and the hardware state is always set.
	void						popScissor( bool forceRestore = false );
	//! Returns a pair<ivec2,ivec2> representing the position of the lower-left corner and the size, respectively of the scissor box
	std::pair<ivec2, ivec2>		getScissor();

	//! Analogous to glCullFace( \a face ). Valid arguments are \c VK_CULL_MODE_NONE, \c VK_CULL_MODE_FRONT_BIT, \c VK_CULL_MODE_BACK_BIT and \c VK_CULL_MODE_FRONT_AND_BACK.
	void						cullMode( VkCullModeFlagBits mode );
	//! Pushes the cull face \a face. Valid arguments are \c VK_CULL_MODE_NONE, \c VK_CULL_MODE_FRONT_BIT, \c VK_CULL_MODE_BACK_BIT and \c VK_CULL_MODE_FRONT_AND_BACK.
	void						pushCullMode( VkCullModeFlagBits mode );
	//! Duplicates and pushes the top of the Cull Face stack.
	void						pushCullMode();
	//! Pops the top of the Cull Face stack. If \a forceRestore then redundancy checks are skipped and the hardware state is always set.
	void						popCullMode( bool forceRestore = false );
	//! Returns a GLenum representing the current cull face. Either \c VK_CULL_MODE_NONE, \c VK_CULL_MODE_FRONT_BIT, \c VK_CULL_MODE_BACK_BIT or \c VK_CULL_MODE_FRONT_AND_BACK.
	VkCullModeFlagBits			getCullMode();

	//! Set the winding order defining front-facing polygons. Valid arguments are \c VK_FRONT_FACE_CLOCKWISE and \c VK_FRONT_FACE_CLOCKWISE. Default is \c VK_FRONT_FACE_COUNTER_CLOCKWISE.
	void						frontFace( VkFrontFace mode );
	//! Push the winding order defining front-facing polygons. Valid arguments are \c VK_FRONT_FACE_CLOCKWISE and \c VK_FRONT_FACE_COUNTER_CLOCKWISE. Default is \c VK_FRONT_FACE_COUNTER_CLOCKWISE.
	void						pushFrontFace( VkFrontFace mode );
	//! Push the winding order defining front-facing polygons.
	void						pushFrontFace();
	//! Pops the winding order defining front-facing polygons. If \a forceRestore then redundancy checks are skipped and the hardware state is always set.
	void						popFrontFace( bool forceRestore = false );
	//! Returns the winding order defining front-facing polygons, either \c VK_FRONT_FACE_CLOCKWISE or \c VK_FRONT_FACE_COUNTER_CLOCKWISE (the default).
	VkFrontFace					getFrontFace();
	
	void						depthBias( bool enable, float slopeFactor, float constantFactor, float clamp );
	void						pushDepthBias( bool enable, float slopeFactor, float constantFactor, float clamp );
	void						pushDepthBias();
	void						popDepthBias( bool forceRestore = false );
	bool						getDepthBiasEnable();
	float						getDepthBiasSlopeFactor();
	float						getDepthBiasConstantFactor();
	float						getDepthBiasClamp();

	//! Analogous to glEnable() or glDisable(). Enables or disables OpenGL capability \a cap
	void						setBoolState( ci::vk::Cap cap, VkBool32 value );
	//! Pushes and sets the state stack for OpenGL capability \a cap to \a value.
	void						pushBoolState( ci::vk::Cap cap, VkBool32 value );
	//! Duplicates and pushes the state stack for OpenGL capability \a cap.
	void						pushBoolState( ci::vk::Cap cap );
	//! Pops the state stack for OpenGL capability \a cap. If \a forceRestore then redundancy checks are skipped and the hardware state is always set.
	void						popBoolState( ci::vk::Cap cap, VkBool32 forceRestore = VK_FALSE );
	//! Synonym for setBoolState(). Enables or disables OpenGL capability \a cap.
	void						enable( ci::vk::Cap cap, VkBool32 value = VK_TRUE );
	//! Analogous to glIsEnabled(). Returns whether a given OpenGL capability is enabled or not
	VkBool32					getBoolState( ci::vk::Cap cap );
	//! Enables or disables OpenGL capability \a cap. Calls \a setter rather than glEnable or glDisable. Not generally necessary to call directly.
	void						setBoolState( ci::vk::Cap cap, VkBool32 value, const std::function<void(VkBool32)> &setter );

	//! Analogous glBlendFunc(). Consider using a ScopedBlend instead.
	void						blendFunc( VkBlendFactor sfactor, VkBlendFactor dfactor );
	//! Analogous to glBlendFuncSeparate(). Consider using a ScopedBlend instead.
	void						blendFuncSeparate( VkBlendFactor srcRGB, VkBlendFactor dstRGB, VkBlendFactor srcAlpha, VkBlendFactor dstAlpha );
	//! Analogous to glBlendFuncSeparate, but pushes values rather than replaces them
	void						pushBlendFuncSeparate( VkBlendFactor srcRGB, VkBlendFactor dstRGB, VkBlendFactor srcAlpha, VkBlendFactor dstAlpha );
	//! Duplicates and pushes the glBlendFunc state stack.
	void						pushBlendFuncSeparate();
	//! Pops the current blend functions. If \a forceRestore then redundancy checks are skipped and the hardware state is always set.
	void						popBlendFuncSeparate( bool forceRestore = false );
	//! Returns the current values for glBendFuncs
	void						getBlendFuncSeparate( VkBlendFactor *resultSrcRGB, VkBlendFactor *resultDstRGB, VkBlendFactor *resultSrcAlpha, VkBlendFactor *resultDstAlpha );

	//! Sets the current line width
	void						lineWidth( float lineWidth );
	//! Pushes and sets the current line width
	void						pushLineWidth( float lineWidth );
	//! Duplicates and pushes the current line width.
	void						pushLineWidth();
	//! Sets the current line width. If \a forceRestore then redundancy checks are skipped and the hardware state is always set.
	void						popLineWidth( bool forceRestore = false );	
	//! Returns the current line width.
	float						getLineWidth();
	

	void						setDefaultUniformVars( const UniformBufferRef& uniformBuffer );

	const ColorAf&				getCurrentColor() const { return mColor; }
	void						setCurrentColor( const ColorAf &color ) { mColor = color; }

	const VkPipelineColorBlendAttachmentState&				getCachedColorAttachmentBlend() const { return mCachedColorAttachmentBlend;}
	void													setCachedColorAttachmentBlend( const VkPipelineColorBlendAttachmentState& cb ) { mCachedColorAttachmentBlend = cb; }
	const std::vector<VkPipelineColorBlendAttachmentState>&	getColorBlendAttachments() const;

	void						setDepthTest( VkBool32 value ) { mDepthTest = value; }
	VkBool32					getDepthTest() const { return mDepthTest; }
	void						setDepthWrite( VkBool32 value ) { mDepthWrite = value; }
	VkBool32					getDepthWrite() const { return mDepthWrite; }

private:
	std::string					generateVertexShader( const ShaderDef &shader );
	std::string					generateFragmentShader( const ShaderDef &shader );
	ShaderProgRef				buildShader( const ShaderDef &shader );
public:
	ShaderProgRef&				getStockShader( const ShaderDef &shaderDef );


protected:
	//! Returns \c true if \a value is different from the previous top of the stack
	template <typename T>
	bool		pushStackState( std::vector<T> &stack, T value );
	//! Returns \c true if the new top of \a stack is different from the previous top, or the stack is empty
	template <typename T>
	bool		popStackState( std::vector<T> &stack );
	//! Returns \c true if \a value is different from the previous top of the stack
	template <typename T>
	bool		setStackState( std::vector<T> &stack, T value );
	//! Returns \c true if \a result is valid; will return \c false when \a stack was empty
	template <typename T>
	bool		getStackState( std::vector<T> &stack, T *result );

	std::map<ShaderDef,ShaderProgRef>		mStockShaders;

	std::vector<vk::RenderPassRef>			mRenderPassStack;
	std::vector<uint32_t>					mSubPassStack;
	std::vector<vk::FramebufferRef>			mFramebufferStack;
	std::vector<vk::ImageRef>				mImageStack;
	std::vector<vk::CommandBufferRef>		mCommandBufferStack;
	std::vector<vk::ShaderProgRef>			mShaderProgStack;
private:
	Environment								*mEnvironment;
	Context::Type							mType = Context::Type::UNDEFINED;

#if defined( CINDER_LINUX )
#elif defined( CINDER_MSW )
	::HINSTANCE								mConnection = nullptr;
	::HWND									mWindow = nullptr;
#endif

	bool									mExplicitMode = false;

	VkPhysicalDevice						mGpu = nullptr;
	VkDevice								mDevice = nullptr;
	VkQueue									mQueue = nullptr;
    uint32_t								mGraphicsQueueFamilyIndex;
    VkPhysicalDeviceProperties				mGpuProperties;
    std::vector<VkQueueFamilyProperties>	mQueueFamilyProperties;
    VkPhysicalDeviceMemoryProperties		mMemoryProperties;
    uint32_t								mQueueFamilyPropertyCount = 0;
	uint32_t								mQueueIndex = 0;
	uint32_t								mWorkQueueCount = 1;
	
	// Default graphics variables
	std::pair<ivec2,ivec2>					mDefaultViewport;
	float									mDefaultLineWidth = 1.0f;

	// Blend state stacks
	std::vector<VkBlendFactor>				mBlendSrcRgbStack, mBlendDstRgbStack;
	std::vector<VkBlendFactor>				mBlendSrcAlphaStack, mBlendDstAlphaStack;

	std::vector<VkCullModeFlagBits>			mCullFaceStack;
	std::vector<VkFrontFace>				mFrontFaceStack;

	struct DepthBiasInfo {
		bool enable = false;
		float slopeFactor = 0.0f;
		float constantFactor = 0.0f;
		float clamp = 0.0f;
		// These operators may need to change beyond the simple enable/disable states for comparison
		bool operator==( const Context::DepthBiasInfo& rhs ) const {  return enable == rhs.enable; }
		bool operator!=( const Context::DepthBiasInfo& rhs ) const {  return enable != rhs.enable; }
	};
	std::vector<DepthBiasInfo>				mDepthBiasStack;

	std::map<ci::vk::Cap ,std::vector<VkBool32>>	mBoolStateStack;

	std::vector<std::pair<ivec2,ivec2>>		mViewportStack;
	std::vector<std::pair<ivec2,ivec2>>		mScissorStack;

	// Current color
	ci::ColorAf													mColor;	
	// Per subpass attachment color blending states
	VkPipelineColorBlendAttachmentState							mCachedColorAttachmentBlend;
	mutable std::vector<VkPipelineColorBlendAttachmentState>	mColorAttachmentBlends;

	VkBool32								mDepthTest = VK_TRUE;
	VkBool32								mDepthWrite = VK_TRUE;
	
	std::vector<mat4>						mModelMatrixStack;
	std::vector<mat4>						mViewMatrixStack;	
	std::vector<mat4>						mProjectionMatrixStack;
	std::vector<float>						mLineWidthStack;

	vk::CommandPoolRef						mDefaultCommandPool;
	vk::CommandBufferRef					mDefaultCommandBuffer;
	vk::PipelineCacheRef					mPipelineCache;
	vk::PipelineSelectorRef					mPipelineSelector;
	
	VkSurfaceKHR							mPresentSurface = 0;
	VkFormat								mPresentColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
	VkFormat								mPresentDepthStencilFormat = VK_FORMAT_D16_UNORM;
	void									setPresentDepthStencilFormat( VkFormat foramt );

	struct PresentRender {
		VkPresentModeKHR					mPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		ivec2								mWindowSize = ivec2( 0 );
		VkSampleCountFlagBits				mSamples = VK_SAMPLE_COUNT_1_BIT;
		bool								mMultiSample = false;
		vk::ImageViewRef					mMultiSampleAttachment;
		VkRect2D							mRenderAreea;
		vk::SwapchainRef					mSwapchain;
		vk::CommandBufferRef				mCommandBuffer;
		vk::RenderPassRef					mRenderPass;
		std::vector<vk::FramebufferRef>		mFramebuffers;
		uint32_t							mCurrentIamgeIndex = 0;
		VkSemaphore							mSemaphore = 0;
	};
	std::unique_ptr<PresentRender>			mPresentRender;
	void									initializePresentRender( const ivec2& mWindowSize, VkSampleCountFlagBits samples, VkPresentModeKHR presentMode );


	void initialize( const Context* existingContext = nullptr );
	void destroy( bool removeFromTracking = true );
	friend class Environment;

	VkResult	initDevice();
	void		initConnection();
	void		initSwapchainExtension();
	void		initDeviceQueue();
	void		destroyDevice();

	void		setDefaultViewport( const std::pair<ivec2,ivec2>& viewport ) { mDefaultViewport = viewport; }
	friend class ci::app::RendererVk;

public:
    PFN_vkCreateSwapchainKHR	fpCreateSwapchainKHR = nullptr;
    PFN_vkDestroySwapchainKHR	fpDestroySwapchainKHR = nullptr;
    PFN_vkGetSwapchainImagesKHR	fpGetSwapchainImagesKHR = nullptr;
    PFN_vkAcquireNextImageKHR	fpAcquireNextImageKHR = nullptr;
    PFN_vkQueuePresentKHR		fpQueuePresentKHR = nullptr;

private:
	std::vector<CommandPool*>				mTrackedCommandPools;
	std::vector<CommandBuffer*>				mTrackedCommandBuffers;
	std::vector<Image*>						mTrackedImages;
	std::vector<ImageView*>					mTrackedImageViews;
	std::vector<Buffer*>					mTrackedBuffers;
	std::vector<UniformBuffer*>				mTrackedUniformBuffers;
	std::vector<IndexBuffer*>				mTrackedIndexBuffers;
	std::vector<VertexBuffer*>				mTrackedVertexBuffers;
	std::vector<Framebuffer*>				mTrackedFramebuffers;
	std::vector<RenderPass*>				mTrackedRenderPasses;
	std::vector<PipelineLayout*>			mTrackedPipelineLayouts;
	std::vector<PipelineCache*>				mTrackedPipelineCaches;
	std::vector<Pipeline*>					mTrackedPipelines;
	std::vector<DescriptorSetLayout*>		mTrackedDescriptorSetLayouts;
	std::vector<DescriptorPool*>			mTrackedDescriptorPools;
	std::vector<DescriptorSet*>				mTrackedDescriptorSets;
	std::vector<ShaderProg*>				mTrackedShaderProgs;
	std::vector<Swapchain*>					mTrackedSwapchains;

public:
	void trackedObjectCreated(   CommandPool *obj );
	void trackedObjectDestroyed( CommandPool *obj );
	void trackedObjectCreated(   CommandBuffer *obj );
	void trackedObjectDestroyed( CommandBuffer *obj );
	void trackedObjectCreated(   Image *obj );
	void trackedObjectDestroyed( Image *obj );
	void trackedObjectCreated(   ImageView *obj );
	void trackedObjectDestroyed( ImageView *obj );
	void trackedObjectCreated(   Buffer *obj );
	void trackedObjectDestroyed( Buffer *obj );
	void trackedObjectCreated(   UniformBuffer *obj );
	void trackedObjectDestroyed( UniformBuffer *obj );
	void trackedObjectCreated(   IndexBuffer *obj );
	void trackedObjectDestroyed( IndexBuffer *obj );
	void trackedObjectCreated(   VertexBuffer *obj );
	void trackedObjectDestroyed( VertexBuffer *obj );
	void trackedObjectCreated(   Framebuffer *obj );
	void trackedObjectDestroyed( Framebuffer *obj );
	void trackedObjectCreated(   RenderPass *obj );
	void trackedObjectDestroyed( RenderPass *obj );
	void trackedObjectCreated(   PipelineLayout *obj );
	void trackedObjectDestroyed( PipelineLayout *obj );
	void trackedObjectCreated(   PipelineCache *obj );
	void trackedObjectDestroyed( PipelineCache *obj );
	void trackedObjectCreated(   Pipeline *obj );
	void trackedObjectDestroyed( Pipeline *obj );
	void trackedObjectCreated(   DescriptorSetLayout *obj );
	void trackedObjectDestroyed( DescriptorSetLayout *obj );
	void trackedObjectCreated(   DescriptorPool *obj );
	void trackedObjectDestroyed( DescriptorPool *obj );
	void trackedObjectCreated(   DescriptorSet *obj );
	void trackedObjectDestroyed( DescriptorSet *obj );
	void trackedObjectCreated(   ShaderProg *obj );
	void trackedObjectDestroyed( ShaderProg *obj );
	void trackedObjectCreated(   Swapchain *obj );
	void trackedObjectDestroyed( Swapchain *obj );

	void transferTrackedObjects( Context* dstCtx );

private:
	//std::vector<vk::CommandPoolRef>			mTransientCommandPools;
	//std::vector<vk::CommandBufferRef>			mTransientCommandBuffers;
	//std::vector<vk::ImageRef>					mTransientImages;
	//std::vector<vk::ImageViewRef>				mTransientImageViews;
	std::vector<vk::BufferRef>					mTransientBuffers;
	std::vector<vk::UniformBufferRef>			mTransientUniformBuffers;
	std::vector<vk::IndexBufferRef>				mTransientIndexBuffers;
	std::vector<vk::VertexBufferRef>			mTransientVertexBuffers;
	//std::vector<vk::FramebufferRef>			mTransientFramebuffers;
	//std::vector<vk::RenderPassRef>			mTransientRenderPasses;
	std::vector<vk::PipelineLayoutRef>			mTransientPipelineLayouts;
	//std::vector<vk::PipelineCacheRef>			mTransientPipelineCaches;
	std::vector<vk::PipelineRef>				mTransientPipelines;
	std::vector<vk::DescriptorSetLayoutRef>		mTransientDescriptorSetLayouts;
	std::vector<vk::DescriptorPoolRef>			mTransientDescriptorPools;
	std::vector<vk::DescriptorSetRef>			mTransientDescriptorSets;
	//std::vector<vk::ShaderProgRef>			mTransientShaderProgs;
	//std::vector<vk::SwapchainRef>				mTransientSwapchains;
	//std::vector<vk::Texture2dRef>				mTransientTexture2ds;

public:
	void	addTransient( const vk::BufferRef& obj );
	void	addTransient( const vk::UniformBufferRef& obj );
	void	addTransient( const vk::IndexBufferRef& obj );
	void	addTransient( const vk::VertexBufferRef& obj );
	void	addTransient( const vk::PipelineLayoutRef& obj );
	void	addTransient( const vk::PipelineRef& obj );
	void	addTransient( const vk::DescriptorSetLayoutRef& obj );
	void	addTransient( const vk::DescriptorPoolRef& obj );
	void	addTransient( const vk::DescriptorSetRef& obj );
	void	clearTransients();
};

}} // namepsace cinder::vk