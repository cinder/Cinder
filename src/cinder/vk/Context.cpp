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

#include "cinder/vk/Context.h"
#include "cinder/vk/Batch.h"
#include "cinder/vk/CommandBuffer.h"
#include "cinder/vk/CommandPool.h"
#include "cinder/vk/Device.h"
#include "cinder/vk/Pipeline.h"
#include "cinder/vk/Queue.h"
#include "cinder/vk/RenderPass.h"
#include "cinder/vk/ShaderProg.h"
#include "cinder/vk/UniformBuffer.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"

#include <algorithm>

namespace cinder { namespace vk {

#if defined( CINDER_ANDROID )
#elif defined( CINDER_LINUX )
	thread_local Context *sThreadSpecificCurrentVkContext = nullptr;
#elif defined( CINDER_MSW )
	__declspec(thread) Context *sThreadSpecificCurrentVkContext = nullptr;
#endif

Context::Context( const vk::PresenterRef& presenter, vk::Device* device )
	: mType( Context::Type::PRIMARY ), mDevice( device ), mPresenter( presenter )
{
	mGraphicsQueue.index = 0;
	
	initialize();

	mColor = ColorAf::white();
}

Context::Context( const Context* existingContext, const std::map<VkQueueFlagBits, uint32_t> queueIndices )
	: mType( Context::Type::SECONDARY )
{
	// Graphics queue
	{
		auto it = queueIndices.find( VK_QUEUE_GRAPHICS_BIT );
		if( it != queueIndices.end() ) {
			mGraphicsQueue.index = static_cast<int32_t>( it->second );
		}
	}

	// Compute queue
	{
		auto it = queueIndices.find( VK_QUEUE_COMPUTE_BIT );
		if( it != queueIndices.end() ) {
			mComputeQueue.index = static_cast<int32_t>( it->second );
		}
	}

	initialize( existingContext );
	mColor = ColorAf::white();
}

Context::~Context()
{
	destroy();
}

ContextRef Context::create( const vk::PresenterRef& presenter, vk::Device* device )
{
	ContextRef result = ContextRef( new Context( presenter, device ) );
	return result;
}

ContextRef Context::createFromExisting( const vk::Context* existingContext, const std::map<VkQueueFlagBits, uint32_t> queueIndices )
{
	ContextRef result = ContextRef( new Context( existingContext, queueIndices ) );
	return result;
}

void Context::initializeQueues()
{
	// Graphics queue
	if( -1 != mGraphicsQueue.index ) {
		mGraphicsQueue.queue = vk::Queue::create( mDevice->getGraphicsQueueFamilyIndex(), mGraphicsQueue.index, this );
	}

	// Compute queue
	if( -1 != mComputeQueue.index ) {
		mComputeQueue.queue = vk::Queue::create( mDevice->getComputeQueueFamilyIndex(), mComputeQueue.index, this );
	}
}

void Context::initialize( const Context* existingContext )
{
	if( existingContext ) {
		mDevice = existingContext->mDevice;
		mPresenter = existingContext->mPresenter;
	}
	initializeQueues();

    mModelMatrixStack.push_back( mat4() );
    mViewMatrixStack.push_back( mat4() );
	mProjectionMatrixStack.push_back( mat4( 1, 0, 0, 0,  0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 ) );

	mDefaultCommandPool = vk::CommandPool::create( mDevice->getGraphicsQueueFamilyIndex(), this );
	mDefaultCommandBuffer = vk::CommandBuffer::create( mDefaultCommandPool->getCommandPool(), this );

	mCachedColorAttachmentBlend.blendEnable			= VK_FALSE;
	mCachedColorAttachmentBlend.srcColorBlendFactor	= VK_BLEND_FACTOR_SRC_ALPHA;
	mCachedColorAttachmentBlend.dstColorBlendFactor	= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	mCachedColorAttachmentBlend.colorBlendOp		= VK_BLEND_OP_ADD;
	mCachedColorAttachmentBlend.srcAlphaBlendFactor	= VK_BLEND_FACTOR_SRC_ALPHA;
	mCachedColorAttachmentBlend.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	mCachedColorAttachmentBlend.alphaBlendOp		= VK_BLEND_OP_ADD;
	mCachedColorAttachmentBlend.colorWriteMask		= 0xf;

	mDevice->trackedObjectCreated( this );
}

void Context::destroy( bool removeFromTracking )
{
	// Bail if type is UNDEFINED. This usually means the context was
	// not setup properly or has already been destroyed.
	if( vk::Context::Type::UNDEFINED == mType ) {
		return;
	}

	// Destroy tracked objects
	const bool removeTrackedObjects = false;
	mTrackedCommandBuffers.destroyAll( [&removeTrackedObjects]( vk::CommandBuffer *obj ) { obj->destroy( removeTrackedObjects ); } );
	mTrackedCommandPools.destroyAll( [&removeTrackedObjects]( vk::CommandPool *obj ) { obj->destroy( removeTrackedObjects ); } );
	
	mType = vk::Context::Type::UNDEFINED;

	if( removeFromTracking ) {
		mDevice->trackedObjectDestroyed( this );
	}
}

Context* Context::getCurrent()
{
	Context *result = sThreadSpecificCurrentVkContext;
	return result;
}

void Context::makeCurrent()
{
	if( this != sThreadSpecificCurrentVkContext ) {
		sThreadSpecificCurrentVkContext = this;
	}
}

bool Context::isExplicitMode() const
{
	return mDevice->isExplicitMode();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Stack management routines
void Context::pushRenderPass( const vk::RenderPassRef& renderPass )
{
	pushStackState( mRenderPassStack, renderPass );
}

void Context::popRenderPass()
{
	popStackState( mRenderPassStack );
}

const vk::RenderPassRef& Context::getRenderPass() const
{
	return mRenderPassStack.back();
}

void Context::pushSubPass( uint32_t subPass )
{
	pushStackState( mSubPassStack, subPass );
}

void Context::popSubPass()
{
	popStackState( mSubPassStack );
}

void Context::setSubpass( uint32_t subPass )
{
	setStackState( mSubPassStack, subPass );
}

uint32_t Context::getSubpass() const
{
	return mSubPassStack.back();
}

void Context::pushCommandBuffer( const vk::CommandBufferRef& cmdBuf )
{
	pushStackState( mCommandBufferStack, cmdBuf );
}

void Context::popCommandBuffer()
{
	popStackState( mCommandBufferStack );
}

const vk::CommandBufferRef& Context::getCommandBuffer() const
{
	return mCommandBufferStack.back();
}

void Context::pushShaderProg( const vk::ShaderProgRef& shaderProg )
{
	pushStackState( mShaderProgStack, shaderProg );
}

void Context::popShaderProg()
{
	popStackState( mShaderProgStack );
}

const vk::ShaderProgRef& Context::getShaderProg() const
{
	return mShaderProgStack.back();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Template stack management routines
template<typename T>
bool Context::pushStackState( std::vector<T> &stack, T value )
{
	bool needsToBeSet = true;
	if( ( ! stack.empty() ) && ( stack.back() == value ) ) {
		needsToBeSet = false;
	}
	stack.push_back( value );
	return needsToBeSet;
}

template<typename T>
bool Context::popStackState( std::vector<T> &stack )
{
	if( ! stack.empty() ) {
		T prevValue = stack.back();
		stack.pop_back();
		if( ! stack.empty() ) {
			return stack.back() != prevValue;
		}
		else {
			return true;
		}
	}
	else {
		return true;
	}
}

template<typename T>
bool Context::setStackState( std::vector<T> &stack, T value )
{
	bool needsToBeSet = true;
	if( ( ! stack.empty() ) && ( stack.back() == value ) ) {
		needsToBeSet = false;
	}
	else if( stack.empty() ) {
		stack.push_back( value );
	}
	else {
		stack.back() = value;
	}
	return needsToBeSet;
}

template<typename T>
bool Context::getStackState( std::vector<T> &stack, T *result )
{
	if( stack.empty() ) {
		return false;
	}
	else {
		*result = stack.back();
		return true;
	}
}

//////////////////////////////////////////////////////////////////
// Viewport
void Context::viewport( const std::pair<ivec2, ivec2> &viewport )
{
	if( setStackState( mViewportStack, viewport ) ) {
		//glViewport( viewport.first.x, viewport.first.y, viewport.second.x, viewport.second.y );
		if( hasCommandBuffer() ) {
			getCommandBuffer()->setViewport( viewport.first.x, viewport.first.y, viewport.second.x, viewport.second.y );
		}
	}
}

void Context::pushViewport( const std::pair<ivec2, ivec2> &viewport )
{
	if( pushStackState( mViewportStack, viewport ) ) {
		//glViewport( viewport.first.x, viewport.first.y, viewport.second.x, viewport.second.y );
		if( hasCommandBuffer() ) {
			getCommandBuffer()->setViewport( viewport.first.x, viewport.first.y, viewport.second.x, viewport.second.y );
		}
	}
}

void Context::pushViewport()
{
	mViewportStack.push_back( getViewport() );
}

void Context::popViewport( bool forceRestore )
{
	if( mViewportStack.empty() ) {
		CI_LOG_E( "Viewport stack underflow" );
	}
	else if( popStackState( mViewportStack ) || forceRestore ) {
		auto viewport = getViewport();
		//glViewport( viewport.first.x, viewport.first.y, viewport.second.x, viewport.second.y );
		if( hasCommandBuffer() ) {
			getCommandBuffer()->setViewport( viewport.first.x, viewport.first.y, viewport.second.x, viewport.second.y );
		}
	}
}

std::pair<ivec2, ivec2> Context::getViewport()
{
	if( mViewportStack.empty() ) {
		// push twice in anticipation of later pop
		mViewportStack.push_back( mDefaultViewport );
		mViewportStack.push_back( mDefaultViewport );
	}

	return mViewportStack.back();
}
    
//////////////////////////////////////////////////////////////////
// Scissor Test
void Context::setScissor( const std::pair<ivec2, ivec2> &scissor )
{
	if( setStackState( mScissorStack, scissor ) ) {
		//glScissor( scissor.first.x, scissor.first.y, scissor.second.x, scissor.second.y );
		if( hasCommandBuffer() ) {
			getCommandBuffer()->setScissor( scissor.first.x, scissor.first.y, scissor.second.x, scissor.second.y );
		}
	}
}

void Context::pushScissor( const std::pair<ivec2, ivec2> &scissor )
{
	if( pushStackState( mScissorStack, scissor ) ) {
		//glScissor( scissor.first.x, scissor.first.y, scissor.second.x, scissor.second.y );
		if( hasCommandBuffer() ) {
			getCommandBuffer()->setScissor( scissor.first.x, scissor.first.y, scissor.second.x, scissor.second.y );
		}
	}
}

void Context::pushScissor()
{
	mScissorStack.push_back( getScissor() );
}

void Context::popScissor( bool forceRestore )
{
	if( mScissorStack.empty() ) {
		CI_LOG_E( "Scissor stack underflow" );
	}
	else if( popStackState( mScissorStack ) || forceRestore ) { 
		auto scissor = getScissor();
		//glScissor( scissor.first.x, scissor.first.y, scissor.second.x, scissor.second.y );
		if( hasCommandBuffer() ) {
			getCommandBuffer()->setScissor( scissor.first.x, scissor.first.y, scissor.second.x, scissor.second.y );
		}
	}
}

std::pair<ivec2, ivec2> Context::getScissor()
{
	if( mScissorStack.empty() ) {
		return std::make_pair( ivec2( 0 ), ivec2( 0 ) );
	}

	return mScissorStack.back();
}

//////////////////////////////////////////////////////////////////
// Face Culling
void Context::cullMode( VkCullModeFlagBits mode )
{
	setStackState( mCullFaceStack, mode );
}

void Context::pushCullMode( VkCullModeFlagBits mode )
{
	pushStackState( mCullFaceStack, mode );
}

void Context::pushCullMode()
{
	pushCullMode( getCullMode() );
}

void Context::popCullMode( bool forceRestore )
{
	if( mCullFaceStack.empty() ) {
		CI_LOG_E( "Cull face stack underflow" );
	}
	else {
		popStackState( mCullFaceStack );
	}
}

VkCullModeFlagBits Context::getCullMode()
{
	if( mCullFaceStack.empty() ) {
		VkCullModeFlagBits defaultCullMode = VK_CULL_MODE_NONE;
		mCullFaceStack.push_back( defaultCullMode ); // push twice
		mCullFaceStack.push_back( defaultCullMode );
	}
	return mCullFaceStack.back();
}

//////////////////////////////////////////////////////////////////
// FrontFace
void Context::frontFace( VkFrontFace mode )
{
	setStackState( mFrontFaceStack, mode );
}

void Context::pushFrontFace( VkFrontFace mode )
{
	pushStackState( mFrontFaceStack, mode );
}

void Context::pushFrontFace()
{
	pushFrontFace( getFrontFace() );
}

void Context::popFrontFace( bool forceRestore )
{
	if( mFrontFaceStack.empty() ) {
		CI_LOG_E( "Front face stack underflow" );
	}
	else {
		popStackState( mFrontFaceStack );
	}
}

VkFrontFace Context::getFrontFace()
{
	if( mFrontFaceStack.empty() ) {
		VkFrontFace defaultValue = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		mFrontFaceStack.push_back( defaultValue ); // push twice
		mFrontFaceStack.push_back( defaultValue );
	}
	return mFrontFaceStack.back();
}

//////////////////////////////////////////////////////////////////
// Depth Bias
void Context::depthBias( bool enable, float slopeFactor, float constantFactor, float clamp )
{
	Context::DepthBiasInfo dbi;
	dbi.enable = enable;
	dbi.slopeFactor = slopeFactor;
	dbi.constantFactor = constantFactor;
	dbi.clamp = clamp;
	setStackState( mDepthBiasStack, dbi );
}

void Context::pushDepthBias( bool enable, float slopeFactor, float constantFactor, float clamp )
{
	Context::DepthBiasInfo dbi;
	dbi.enable = enable;
	dbi.slopeFactor = slopeFactor;
	dbi.constantFactor = constantFactor;
	dbi.clamp = clamp;
	pushStackState( mDepthBiasStack, dbi );
}

void Context::pushDepthBias()
{
	Context::DepthBiasInfo dbi;
	dbi.enable = getDepthBiasEnable();
	dbi.slopeFactor = getDepthBiasSlopeFactor();
	dbi.constantFactor = getDepthBiasConstantFactor();
	dbi.clamp = getDepthBiasClamp();
	pushStackState( mDepthBiasStack, dbi );
}

void Context::popDepthBias( bool forceRestore  )
{
	if( mDepthBiasStack.empty() ) {
		CI_LOG_E( "Front face stack underflow" );
	}
	else {
		popStackState( mDepthBiasStack );
	}
}

bool Context::getDepthBiasEnable()
{
	if( mDepthBiasStack.empty() ) {
		Context::DepthBiasInfo defaultValue = {};
		mDepthBiasStack.push_back( defaultValue ); // push twice
		mDepthBiasStack.push_back( defaultValue );
	}
	return mDepthBiasStack.back().enable;
}

float Context::getDepthBiasSlopeFactor()
{
	if( mDepthBiasStack.empty() ) {
		Context::DepthBiasInfo defaultValue = {};
		mDepthBiasStack.push_back( defaultValue ); // push twice
		mDepthBiasStack.push_back( defaultValue );
	}
	return mDepthBiasStack.back().slopeFactor;
}

float Context::getDepthBiasConstantFactor()
{
	if( mDepthBiasStack.empty() ) {
		Context::DepthBiasInfo defaultValue = {};
		mDepthBiasStack.push_back( defaultValue ); // push twice
		mDepthBiasStack.push_back( defaultValue );
	}
	return mDepthBiasStack.back().constantFactor;
}

float Context::getDepthBiasClamp()
{
	if( mDepthBiasStack.empty() ) {
		Context::DepthBiasInfo defaultValue = {};
		mDepthBiasStack.push_back( defaultValue ); // push twice
		mDepthBiasStack.push_back( defaultValue );
	}
	return mDepthBiasStack.back().clamp;
}


//////////////////////////////////////////////////////////////////
// States
void Context::setBoolState( ci::vk::Cap cap, VkBool32 value )
{
	bool needsToBeSet = true;
	auto cached = mBoolStateStack.find( cap );
	if( ( cached != mBoolStateStack.end() ) && ( ! cached->second.empty() ) && ( cached->second.back() == value ) ) {
		needsToBeSet = false;
	}
	if( cached == mBoolStateStack.end() ) {
		mBoolStateStack[cap] = std::vector<VkBool32>();
		mBoolStateStack[cap].push_back( value );
	}
	else {
		mBoolStateStack[cap].back() = value;
	}
	//if( needsToBeSet ) {
	//	if( value )
	//		glEnable( cap );
	//	else
	//		glDisable( cap );
	//}
}

void Context::setBoolState( ci::vk::Cap cap, VkBool32 value, const std::function<void(VkBool32)> &setter )
{
	bool needsToBeSet = true;
	auto cached = mBoolStateStack.find( cap );
	if( ( cached != mBoolStateStack.end() ) && ( ! cached->second.empty() ) && ( cached->second.back() == value ) ) {
		needsToBeSet = false;
	}
	if( cached == mBoolStateStack.end() ) {
		mBoolStateStack[cap] = std::vector<VkBool32>();
		mBoolStateStack[cap].push_back( value );
	}
	else {
		mBoolStateStack[cap].back() = value;
	}
	if( needsToBeSet ) {
		setter( value );
	}
}

void Context::pushBoolState( ci::vk::Cap cap, VkBool32 value )
{
	bool needsToBeSet = true;
	auto cached = mBoolStateStack.find( cap );
	if( ( cached != mBoolStateStack.end() ) && ( ! cached->second.empty() ) && ( cached->second.back() == value ) ) {
		needsToBeSet = false;
	}
	else if( cached == mBoolStateStack.end() ) {
		mBoolStateStack[cap] = std::vector<VkBool32>();
		// For now: assume any given state is off by default
		mBoolStateStack[cap].push_back( VK_FALSE );
	}
	mBoolStateStack[cap].push_back( value );
	//if( needsToBeSet ) {
	//	if( value )
	//		glEnable( cap );
	//	else
	//		glDisable( cap );
	//}
}

void Context::pushBoolState( ci::vk::Cap cap )
{
	VkBool32 existingVal = getBoolState( cap );
	mBoolStateStack[cap].push_back( existingVal );
}

void Context::popBoolState( ci::vk::Cap cap, VkBool32 forceRestore )
{
	auto cached = mBoolStateStack.find( cap );
	if( ( cached != mBoolStateStack.end() ) && ( ! cached->second.empty() ) ) {
		VkBool32 prevValue = cached->second.back();
		cached->second.pop_back();
		// Maybe just remove this since Vulkan doesn't have states?
		//if( ! cached->second.empty() ) {
		//	if( forceRestore || ( cached->second.back() != prevValue ) ) {
		//		if( cached->second.back() )
		//			glEnable( cap );
		//		else
		//			glDisable( cap );
		//	}
		//}
	}
	else {
		// @TODO: Fix the constant to string
		//CI_LOG_E( "Boolean state stack underflow: " << constantToString( cap ) );
	}
}

void Context::enable( ci::vk::Cap cap, VkBool32 value )
{
	setBoolState( cap, value );
}

VkBool32 Context::getBoolState( ci::vk::Cap cap )
{
	auto cached = mBoolStateStack.find( cap );
	if( ( cached == mBoolStateStack.end() ) || cached->second.empty() ) {
		// For now: assume any given state is off by default
		VkBool32 result = VK_FALSE;
		if( cached == mBoolStateStack.end() ) {
			mBoolStateStack[cap] = std::vector<VkBool32>();
		}
		// push twice to accommodate later pop
		mBoolStateStack[cap].push_back( result );
		mBoolStateStack[cap].push_back( result );
		return result;
	}
	else {
		return cached->second.back();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// BlendFunc
void Context::blendFunc( VkBlendFactor sfactor, VkBlendFactor dfactor )
{
	blendFuncSeparate( sfactor, dfactor, sfactor, dfactor );
}

void Context::blendFuncSeparate( VkBlendFactor srcRGB, VkBlendFactor dstRGB, VkBlendFactor srcAlpha, VkBlendFactor dstAlpha )
{
	bool needcaching = setStackState<VkBlendFactor>( mBlendSrcRgbStack, srcRGB );
	needcaching = setStackState<VkBlendFactor>( mBlendDstRgbStack, dstRGB ) || needcaching;
	needcaching = setStackState<VkBlendFactor>( mBlendSrcAlphaStack, srcAlpha ) || needcaching;
	needcaching = setStackState<VkBlendFactor>( mBlendDstAlphaStack, dstAlpha ) || needcaching;
	if( needcaching ) {
		mCachedColorAttachmentBlend.blendEnable			= getBoolState( ci::vk::Cap::BLEND );
		mCachedColorAttachmentBlend.srcColorBlendFactor	= srcRGB;
		mCachedColorAttachmentBlend.dstColorBlendFactor	= dstRGB;
		mCachedColorAttachmentBlend.colorBlendOp		= VK_BLEND_OP_ADD;
		mCachedColorAttachmentBlend.srcAlphaBlendFactor	= srcAlpha;
		mCachedColorAttachmentBlend.dstAlphaBlendFactor	= dstAlpha;
		mCachedColorAttachmentBlend.alphaBlendOp		= VK_BLEND_OP_ADD;
		mCachedColorAttachmentBlend.colorWriteMask		= 0xf;
	}
}

void Context::pushBlendFuncSeparate( VkBlendFactor srcRGB, VkBlendFactor dstRGB, VkBlendFactor srcAlpha, VkBlendFactor dstAlpha )
{
	bool needsCaching = pushStackState<VkBlendFactor>( mBlendSrcRgbStack, srcRGB );
	needsCaching = pushStackState<VkBlendFactor>( mBlendDstRgbStack, dstRGB ) || needsCaching;
	needsCaching = pushStackState<VkBlendFactor>( mBlendSrcAlphaStack, srcAlpha ) || needsCaching;
	needsCaching = pushStackState<VkBlendFactor>( mBlendDstAlphaStack, dstAlpha ) || needsCaching;
	if( needsCaching ) {
		mCachedColorAttachmentBlend.blendEnable			= getBoolState( ci::vk::Cap::BLEND );
		mCachedColorAttachmentBlend.srcColorBlendFactor	= srcRGB;
		mCachedColorAttachmentBlend.dstColorBlendFactor	= dstRGB;
		mCachedColorAttachmentBlend.colorBlendOp		= VK_BLEND_OP_ADD;
		mCachedColorAttachmentBlend.srcAlphaBlendFactor	= srcAlpha;
		mCachedColorAttachmentBlend.dstAlphaBlendFactor	= dstAlpha;
		mCachedColorAttachmentBlend.alphaBlendOp		= VK_BLEND_OP_ADD;
		mCachedColorAttachmentBlend.colorWriteMask		= 0xf;
	}
}

void Context::pushBlendFuncSeparate()
{
	VkBlendFactor resultSrcRGB, resultDstRGB, resultSrcAlpha, resultDstAlpha;
	getBlendFuncSeparate( &resultSrcRGB, &resultDstRGB, &resultSrcAlpha, &resultDstAlpha );

	mBlendSrcRgbStack.push_back( resultSrcRGB );
	mBlendDstRgbStack.push_back( resultDstRGB );
	mBlendSrcAlphaStack.push_back( resultSrcAlpha );
	mBlendDstAlphaStack.push_back( resultDstAlpha );
}

void Context::popBlendFuncSeparate( bool forceRestore )
{
	bool needsCaching = popStackState<VkBlendFactor>( mBlendSrcRgbStack );
	needsCaching = popStackState<VkBlendFactor>( mBlendDstRgbStack ) || needsCaching;
	needsCaching = popStackState<VkBlendFactor>( mBlendSrcAlphaStack ) || needsCaching;
	needsCaching = popStackState<VkBlendFactor>( mBlendDstAlphaStack ) || needsCaching;
	needsCaching = forceRestore || needsCaching;
	if( needsCaching && ( ! mBlendSrcRgbStack.empty() ) && ( ! mBlendSrcAlphaStack.empty() ) && ( ! mBlendDstRgbStack.empty() ) && ( ! mBlendDstAlphaStack.empty() ) ) {
		mCachedColorAttachmentBlend.blendEnable			= getBoolState( ci::vk::Cap::BLEND );
		mCachedColorAttachmentBlend.srcColorBlendFactor	= mBlendSrcRgbStack.back();
		mCachedColorAttachmentBlend.dstColorBlendFactor	=  mBlendDstRgbStack.back();
		mCachedColorAttachmentBlend.colorBlendOp		= VK_BLEND_OP_ADD;
		mCachedColorAttachmentBlend.srcAlphaBlendFactor	= mBlendSrcAlphaStack.back();
		mCachedColorAttachmentBlend.dstAlphaBlendFactor	= mBlendDstAlphaStack.back();
		mCachedColorAttachmentBlend.alphaBlendOp		= VK_BLEND_OP_ADD;
		mCachedColorAttachmentBlend.colorWriteMask		= 0xf;
	}
}

void Context::getBlendFuncSeparate( VkBlendFactor *resultSrcRGB, VkBlendFactor *resultDstRGB, VkBlendFactor *resultSrcAlpha, VkBlendFactor *resultDstAlpha )
{
	// push twice on empty to accommodate inevitable push later
	VkBlendFactor defaultValue = VK_BLEND_FACTOR_ZERO;
	if( mBlendSrcRgbStack.empty() ) {
		mBlendSrcRgbStack.push_back( defaultValue ); 
		mBlendSrcRgbStack.push_back( defaultValue );
	}
	if( mBlendDstRgbStack.empty() ) {
		mBlendDstRgbStack.push_back( defaultValue ); 
		mBlendDstRgbStack.push_back( defaultValue );
	}
	if( mBlendSrcAlphaStack.empty() ) {
		mBlendSrcAlphaStack.push_back( defaultValue ); 
		mBlendSrcAlphaStack.push_back( defaultValue );
	}
	if( mBlendDstAlphaStack.empty() ) {
		mBlendDstAlphaStack.push_back( defaultValue ); 
		mBlendDstAlphaStack.push_back( defaultValue );
	}
	
	*resultSrcRGB = mBlendSrcRgbStack.back();
	*resultDstRGB = mBlendDstRgbStack.back();
	*resultSrcAlpha = mBlendSrcAlphaStack.back();
	*resultDstAlpha = mBlendDstAlphaStack.back();
}

///////////////////////////////////////////////////////////////////////////////////////////
// LineWidth
void Context::lineWidth( float lineWidth )
{
	if( setStackState<float>( mLineWidthStack, lineWidth ) ) {
		//glLineWidth( lineWidth );
		if( hasCommandBuffer() ) {
			getCommandBuffer()->setLineWidth( lineWidth );
		}
	}
}

void Context::pushLineWidth( float lineWidth )
{
	if( pushStackState<float>( mLineWidthStack, lineWidth ) ) {
		//glLineWidth( lineWidth );
		if( hasCommandBuffer() ) {
			getCommandBuffer()->setLineWidth( lineWidth );
		}
	}
}

void Context::pushLineWidth()
{
	pushStackState<float>( mLineWidthStack, getLineWidth() );
}

void Context::popLineWidth( bool forceRestore )
{
	if( mLineWidthStack.empty() ) {
		CI_LOG_E( "LineWidth stack underflow" );
	}
	else if( popStackState<float>( mLineWidthStack ) || forceRestore ) {
		//glLineWidth( getLineWidth() );
		if( hasCommandBuffer() ) {
			getCommandBuffer()->setLineWidth( getLineWidth() );
		}
	}
}

float Context::getLineWidth()
{
	if( mLineWidthStack.empty() ) {
		// push twice to account for inevitable pop to follow
		mLineWidthStack.push_back( mDefaultLineWidth );
		mLineWidthStack.push_back( mDefaultLineWidth );
	}

	return mLineWidthStack.back();
}

///////////////////////////////////////////////////////////////////////////////////////////
// Uniform Values
void Context::setDefaultUniformVars( const UniformBufferRef& uniformBuffer )
{
	const auto &uniforms = uniformBuffer->getActiveUniforms();
	for( const auto &uniform : uniforms ) {
		if( uniform.isDirty() ) {
			continue;
		}

		switch( uniform.getUniformSemantic() ) {
			case UNIFORM_MODEL_MATRIX: {
				auto model = vk::getModelMatrix();
				uniformBuffer->uniform( uniform.getName(), model );
			}
			break;
			case UNIFORM_MODEL_MATRIX_INVERSE: {
				auto inverseModel = glm::inverse( vk::getModelMatrix() );
				uniformBuffer->uniform( uniform.getName(), inverseModel );
			}
			break;
			case UNIFORM_MODEL_MATRIX_INVERSE_TRANSPOSE: {
				auto modelInverseTranspose = vk::calcModelMatrixInverseTranspose();
				uniformBuffer->uniform( uniform.getName(), modelInverseTranspose );
			}
			break;
			case UNIFORM_VIEW_MATRIX: {
				auto view = vk::getViewMatrix();
				uniformBuffer->uniform( uniform.getName(), view );
			}
			break;
			case UNIFORM_VIEW_MATRIX_INVERSE: {
				auto viewInverse = vk::calcViewMatrixInverse();
				uniformBuffer->uniform( uniform.getName(), viewInverse );
			}
			break;
			case UNIFORM_MODEL_VIEW: {
				auto modelView = vk::getModelView();
				uniformBuffer->uniform( uniform.getName(), modelView );
			}
			break;
			case UNIFORM_MODEL_VIEW_INVERSE: {
				auto modelViewInverse = glm::inverse( vk::getModelView() );
				uniformBuffer->uniform( uniform.getName(), modelViewInverse );
			}
			break;
			case UNIFORM_MODEL_VIEW_INVERSE_TRANSPOSE: {
				auto normalMatrix = vk::calcNormalMatrix();
				uniformBuffer->uniform( uniform.getName(), normalMatrix );
			}
			break;
			case UNIFORM_MODEL_VIEW_PROJECTION: {
				auto modelViewProjection = vk::getModelViewProjection();
				uniformBuffer->uniform( uniform.getName(), modelViewProjection );
			}
			break;
			case UNIFORM_MODEL_VIEW_PROJECTION_INVERSE: {
				auto modelViewProjectionInverse = glm::inverse( vk::getModelViewProjection() );
				uniformBuffer->uniform( uniform.getName(), modelViewProjectionInverse );
			}
			break;
			case UNIFORM_PROJECTION_MATRIX: {
				auto projection = vk::getProjectionMatrix();
				uniformBuffer->uniform( uniform.getName(), projection );
			}
			break;
			case UNIFORM_PROJECTION_MATRIX_INVERSE: {
				auto projectionInverse = glm::inverse( vk::getProjectionMatrix() );
				uniformBuffer->uniform( uniform.getName(), projectionInverse );
			}
			break;
			case UNIFORM_VIEW_PROJECTION: {
				auto viewProjection = vk::getProjectionMatrix() * vk::getViewMatrix();
				uniformBuffer->uniform( uniform.getName(), viewProjection );
			}
			break;
			case UNIFORM_NORMAL_MATRIX: {
				auto normalMatrix = vk::calcNormalMatrix();
				uniformBuffer->uniform( uniform.getName(), normalMatrix );
			}
			break;
			case UNIFORM_VIEWPORT_MATRIX: {
				auto viewport = vk::calcViewportMatrix();
				uniformBuffer->uniform( uniform.getName(), viewport );
			}
			break;
			case UNIFORM_WINDOW_SIZE: {
				auto windowSize = app::getWindowSize();
				uniformBuffer->uniform( uniform.getName(), windowSize );
			}
			break;
			case UNIFORM_ELAPSED_SECONDS: {
				auto elapsed = float( app::getElapsedSeconds() );
				uniformBuffer->uniform( uniform.getName(), elapsed );
			break;
			}
			default:
			break;
		}

		//const auto &attribs = glslProg->getActiveAttributes();
		//for( const auto &attrib : attribs ) {
		//	switch( attrib.getSemantic() ) {
		//		case geom::Attrib::COLOR: {
		//			ColorA c = ctx->getCurrentColor();
		//			gl::vertexAttrib4f( attrib.getLocation(), c.r, c.g, c.b, c.a );
		//		}
		//		break;
		//		default:
		//			;
		//	}
		//}
	}
}

const std::vector<VkPipelineColorBlendAttachmentState>&	Context::getColorBlendAttachments() const 
{ 
	// If a subpass is depth/stencil only, mColorAttachmentBlends should be empty.
	uint32_t minSize = 0;
	if( this->getRenderPass() ) {
		uint32_t subpass = this->getSubpass();
		uint32_t count = this->getRenderPass()->getSubpassColorAttachmentCount( subpass );
		minSize = std::max( minSize, count );
	}

	if( minSize >0 ) {
		mColorAttachmentBlends.resize( minSize );	
		std::fill( std::begin( mColorAttachmentBlends ), std::end( mColorAttachmentBlends ), mCachedColorAttachmentBlend );
	}
	else {
		mColorAttachmentBlends.clear();
	}

	return mColorAttachmentBlends; 
}

void Context::clearAttachments( bool color, bool depthStencil )
{
	auto& renderPass = this->getRenderPass();
	auto& cmdBuf = this->getCommandBuffer();
	if( renderPass && cmdBuf ) {
		uint32_t subpass = this->getSubpass();
		// Clear attachments
		auto subpassClearAttachs = renderPass->getClearAttachments( subpass );
		// Clear rects
		std::vector<VkClearRect> clearRects;
		for( size_t i = 0; i < subpassClearAttachs.size(); ++i ) {
			// @TODO: Determine if we need to support more than one layer
			auto viewport = this->getViewport();
			VkClearRect cr = {};
			cr.rect.offset		= { viewport.first.x, viewport.first.y };
			cr.rect.extent		= { viewport.second.x, viewport.second.y };
			cr.baseArrayLayer	= 0;
			cr.layerCount		= 1;
			clearRects.push_back( cr );
		}
		// Record to command buffer
		cmdBuf->clearAttachments( static_cast<uint32_t>( subpassClearAttachs.size() ), subpassClearAttachs.data(), static_cast<uint32_t>( clearRects.size() ), clearRects.data() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
// Shaders
std::string	Context::generateVertexShader( const vk::ShaderDef &shader )
{	
	std::string s;
	
	s +=		"#version 150\n"
				"#extension GL_ARB_separate_shader_objects : enable\n"
				"#extension GL_ARB_shading_language_420pack : enable\n"
				"\n"
				"layout(std140, binding=0) uniform ciBlock0T {\n"
				"    uniform mat4 ciModelViewProjection;\n"
				;

	if( shader.mLambert )
		s +=	"    uniform mat4 ciNormalMatrix;\n";

	if( shader.mUniformBasedPosAndTexCoord ) {
		s +=	"    uniform vec2 uPositionOffset;\n";
		s +=	"    uniform vec2 uPositionScale;\n";
		if( shader.mTextureMapping ) {
			s+=	"    uniform vec2 uTexCoordOffset;\n";
			s+=	"    uniform vec2 uTexCoordScale;\n";
		}
	}

	s +=		"} ciBlock0;\n";

	s +=		"\n"
				"layout(location = 0) in vec4 ciPosition;\n"
				;
	
	if( shader.mTextureMapping ) {
		s +=	"layout(location = 1) in vec2 ciTexCoord0;\n"
				"layout(location = 1) out vec2 TexCoord;\n"
				;
	}
	if( shader.mColor ) {
		s +=	"layout(location = 2) in vec4 ciColor;\n"
				"layout(location = 2) out vec4 Color;\n"
				;
	}
	if( shader.mLambert ) {
		s +=	"layout(location = 3) in vec3 ciNormal;\n"
				"layout(location = 3) out vec3 Normal;\n"
				;
	}

	s +=		"void main( void )\n"
				"{\n";
	if( shader.mUniformBasedPosAndTexCoord )
		s +=	"	gl_Position = ciBlock0.ciModelViewProjection * ( vec4( ciBlock0.uPositionOffset, 0, 0 ) + vec4( ciBlock0.uPositionScale, 1, 1 ) * ciPosition );\n";
	else
		s +=	"	gl_Position = ciBlock0.ciModelViewProjection * ciPosition;\n"
				;

	if( shader.mTextureMapping ) {
		if( shader.mUniformBasedPosAndTexCoord )
			s+=	"	TexCoord = ciBlock0.uTexCoordOffset + ciBlock0.uTexCoordScale * ciTexCoord0;\n";
		else
			s+=	"	TexCoord = ciTexCoord0;\n";
				;
	}
	if( shader.mColor ) {
		s +=	"	Color = ciColor;\n"
				;
	}
	if( shader.mLambert ) {
		s +=	"	Normal = mat3(ciBlock0.ciNormalMatrix) * ciNormal;\n"
				;
	}
	
	s +=		"}";
	
//ci::app::console() << "Vertex shader: " << "\n" << s << "\n" << std::endl;
	return s;
}

std::string	Context::generateFragmentShader( const vk::ShaderDef &shader )
{
	std::string s;
	
	s+=			"#version 150\n"
				"#extension GL_ARB_separate_shader_objects : enable\n"
				"#extension GL_ARB_shading_language_420pack : enable\n"
				"\n"
				"layout(location = 0) out vec4 oColor;\n"
				;

	if( shader.mTextureMapping ) {
		if( shader.mTextureUnormalizedCoordinates )
			s +="layout(binding = 1) uniform sampler2DRect uTex0;\n";
		else
			s +="layout(binding = 1) uniform sampler2D uTex0;\n";

		s	+=	"layout(location = 1) in vec2 TexCoord;\n";
				;
	}
	if( shader.mColor ) {
		s +=	"layout(location = 2) in vec4 Color;\n";
	}

	if( shader.mLambert ) {
		s +=	"layout(location = 3) in vec3 Normal;\n";
	}

	s +=		"void main( void )\n"
				"{\n"
				;

	if( shader.mLambert ) {
		s +=	"	const vec3 L = vec3( 0, 0, 1 );\n"
				"	vec3 N = normalize( Normal );\n"
				"	float lambert = max( 0.0, dot( N, L ) );\n"
				;
	}
	
	s += "	oColor = vec4( 1 )";

	if( shader.mTextureMapping ) {
		s +=	" * texture( uTex0, TexCoord.st )";
		//if( ! Texture::supportsHardwareSwizzle() && ! shader.isTextureSwizzleDefault() )
		//	s += "." + shader.getTextureSwizzleString();
	}

	if( shader.mColor )
		s +=	" * Color";

	if( shader.mLambert )
		s +=	" * vec4( vec3( lambert ), 1.0 )";

	s +=	";\n";
	
	//s +=	"oColor = vec4( TexCoord.st, 0, 1 );\n";

	s +=	"}";
	
//ci::app::console() << "Fragment shader: " << "\n" << s << "\n" << std::endl;
	return s;
}


ShaderProgRef Context::buildShader( const vk::ShaderDef &shader )
{
/*
		vk::UniformLayout uniformLayout = vk::UniformLayout();
		uniformLayout.blockBegin( 0, "ciBlock0" );
			uniformLayout.uniform( "ciModelViewProjection", mat4() );
		if( shader.mLambert ) {
			uniformLayout.uniform( "ciNormalMatrix", mat4() );
		}
		if( shader.mTextureMapping ) {
			uniformLayout.sampler2D( 1, "uTex0" );
		}
*/

		vk::ShaderProg::Format format = vk::ShaderProg::Format()
			.vertex( generateVertexShader( shader ) )
			.fragment( generateFragmentShader( shader ) )
			.binding( "ciBlock0", 0 )
			.attribute( geom::Attrib::POSITION, 0, 0, vk::glsl_attr_vec4 );
		if( shader.mTextureMapping ) {
			format.binding( "uTex0", 1 );
			format.attribute( geom::Attrib::TEX_COORD_0, 1, 0, vk::glsl_attr_vec2 );
		}
		if( shader.mColor ) {
			format.attribute( geom::Attrib::COLOR, 2, 0, vk::glsl_attr_vec4 );
		}
		if( shader.mLambert ) {
			format.attribute( geom::Attrib::NORMAL, 3, 0, vk::glsl_attr_vec3 );
		}

		//format.uniformLayout( uniformLayout );

	vk::ShaderProgRef result = vk::GlslProg::create( format );
	return result;
}

ShaderProgRef& Context::getStockShader( const vk::ShaderDef &shaderDef )
{
	auto existing = mStockShaders.find( shaderDef );

	if( existing == mStockShaders.end() ) {
		try {
			auto result = buildShader( shaderDef );
			mStockShaders[shaderDef] = result;
			return mStockShaders[shaderDef];
		}
		catch( const std::exception& e ) {
			ci::app::console() << "Error in getStockShader: " << e.what() << std::endl;
		}
	}

	return existing->second;
}


void Context::addPendingUniformVars( const ci::vk::UniformBufferRef& buffer )
{
	auto it = std::find_if(
		std::begin( mPendingUniformBuffers ),
		std::end( mPendingUniformBuffers ),
		[buffer]( const vk::UniformBufferRef& elem ) -> bool {
			return buffer == elem;
		}
	);

	if( std::end( mPendingUniformBuffers ) == it ) {
		mPendingUniformBuffers.push_back( buffer );
	}
}

void Context::addPendingUniformVars( const vk::UniformSetRef& uniformSetRef )
{
	const auto& sets = uniformSetRef->getSets();
	for( auto& set : sets ) {
		for( auto& binding : set->getBindings() ) {
			if( ! binding.isBlock() ) {
				continue;
			}
			this->addPendingUniformVars( binding.getUniformBuffer() );
		}
	}
}

void Context::addPendingUniformVars( const vk::BatchRef& batch )
{
	this->addPendingUniformVars( batch->getUniformSet() );
}

void Context::transferPendingUniformBuffer( const vk::CommandBufferRef& cmdBuf, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask )
{
	for( const auto& uniformBuffer : mPendingUniformBuffers ) {
		if( ! uniformBuffer->isDirty() ) {
			continue;
		}

		uniformBuffer->transferPending();
		cmdBuf->pipelineBarrierBufferMemory( uniformBuffer, srcAccessMask, dstAccessMask, srcStageMask, dstStageMask );
	}

	mPendingUniformBuffers.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Object tracking routines
static std::mutex sTrackedObjectCreatedMutex;

template <typename T>
void objectCreated( T* obj, std::vector<T*>& tracker )
{
	std::lock_guard<std::mutex> lock( sTrackedObjectCreatedMutex );

	auto it = std::find( std::begin( tracker ), std::end( tracker ), obj );
	if( it == std::end( tracker ) ) {
		tracker.push_back( obj );
	}
}

template <typename T>
void objectDestroyed(  T* obj, std::vector<T*>& tracker )
{
	auto it = std::find( std::begin( tracker ), std::end( tracker ), obj );
	if( it != std::end( tracker ) ) {
		tracker.erase(
			std::remove( std::begin( tracker ), std::end( tracker ), obj ),
			std::end( tracker )
		);
	}
}

void Context::trackedObjectCreated( vk::CommandPool *obj )
{
	mTrackedCommandPools.objectCreated( obj );
}

void Context::trackedObjectDestroyed( vk::CommandPool *obj )
{
	mTrackedCommandPools.objectDestroyed( obj );
}

void Context::trackedObjectCreated( vk::CommandBuffer *obj )
{
	mTrackedCommandBuffers.objectCreated( obj );
}

void Context::trackedObjectDestroyed( vk::CommandBuffer *obj )
{
	mTrackedCommandBuffers.objectDestroyed( obj );
}

//////////////////////////////////////////////////////////////////////////////////////////
// Transient object routines
template <typename T>
void addTransientObject( const T& obj, std::vector<T>& tracker )
{
	auto it = std::find( std::begin( tracker ), std::end( tracker ), obj );
	if( it == std::end( tracker ) ) {
		tracker.push_back( obj );
	}
}

void Context::addTransient( const vk::UniformBufferRef& obj )
{
	addTransientObject( obj, mTransientUniformBuffers );
}

void Context::addTransient( const vk::IndexBufferRef& obj )
{
	addTransientObject( obj, mTransientIndexBuffers );
}

void Context::addTransient( const vk::VertexBufferRef& obj )
{
	addTransientObject( obj, mTransientVertexBuffers );
}

void Context::addTransient( const vk::DescriptorPoolRef& obj )
{
	addTransientObject( obj, mTransientDescriptorPools );
}

void Context::addTransient( const vk::DescriptorSetRef& obj )
{
	addTransientObject( obj, mTransientDescriptorSets );
}

void Context::addTransient( const vk::DescriptorLayoutRef& obj )
{
	addTransientObject( obj, mTransientDescriptorLayouts );
}

void Context::addTransient( const vk::UniformSetRef& obj )
{
	addTransientObject( obj, mTransientUniformSets );
}

void Context::clearTransients()
{
	mTransientUniformBuffers.clear();
	mTransientIndexBuffers.clear();
	mTransientVertexBuffers.clear();

	// Sets need to be destroyed before pools
	mTransientDescriptorSets.clear();
	mTransientDescriptorPools.clear();
	mTransientDescriptorLayouts.clear();

	mTransientUniformSets.clear();
}

}} // namespace cinder::vk