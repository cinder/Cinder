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

#include "cinder/vk/Pipeline.h"

namespace cinder { namespace vk {

namespace util_hash {

uint32_t calculateHash( const char* bytes, size_t size );

template <typename T> uint32_t	calculateHash( const T& data ) {
	return calculateHash( reinterpret_cast<const char*>( &data ), sizeof( T ) );
}

template <typename T> uint32_t	calculateHash( const std::vector<T>& data ) {
	uint32_t result = 0;
	if( ! data.empty() ) {
		result = calculateHash( reinterpret_cast<const char*>( data.data() ), data.size()*sizeof( T ) );
	}
	return result;
}

} // namespace hash


class Context;
class PipelineCache;
class PipelineSelector;
using PipelineCacheRef = std::shared_ptr<PipelineCache>;
using PipelineSelectorRef = std::shared_ptr<PipelineSelector>;

//! \class PipelineSelector
//!
//!
class PipelineSelector {
public:

/*
	class StatesHash {
	public:
		StatesHash() {}
		virtual ~StatesHash() {};
		uint64_t	getHash() const { return mHash; }
	protected:
		bool		mDirty = false;
		uint64_t	mHash = 0;
	};

	template <uint32_t NumVertexBindingDescriptions, uint32_t NumVertexAttributeDescriptions, uint32_t NumSubPassColorBlends>
	class BasicStatesHash : public StatesHash {
	public:

	private:
		struct HashCriteria {
			bool								mDirty = false;
			VkRenderPass						mRenderPass = nullptr;
			uint32_t							mSubPass = 0;
			VkPipelineLayout					mPipelineLayout = nullptr;
			uint32_t							mVertexBindingDescriptionCount = 0;
			VkVertexInputBindingDescription		mVertexBindingDescriptions[NumVertexBindingDescriptions];
			uint32_t							mVertexAttributeDescriptionCount = 0;
			VkVertexInputAttributeDescription	mVertexAttributeDescriptions[NumVertexAttributeDescriptions];
			VkPrimitiveTopology					mTopolgy = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			VkCullModeFlagBits					mCullMode = VK_CULL_MODE_NONE;
			VkFrontFace							mFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			VkBool32							mDepthBiasEnable = VK_FALSE;
			float								mDepthBiasConstantFactor = 0.0f;
			float								mDepthBiasClamp = 0.0f;
			float								mDepthBiasSlopeFactor = 0.0f;
			float								mLineWidth = 1.0f;
			VkBool32							mDepthTest = VK_TRUE;
			VkBool32							mDepthWrite = VK_TRUE;
			uint32_t							mSubPassColorBlendsCount = 0;
			VkPipelineColorBlendAttachmentState	mSubPassColorBlends[NumSubPassColorBlends];
		};
	
		HashCriteria	mCriteria;
	};
*/

	// ---------------------------------------------------------------------------------------------

	PipelineSelector( const vk::PipelineCacheRef& pipelineCacheRef, Device *device );
	virtual ~PipelineSelector();

	static PipelineSelectorRef	create( const vk::PipelineCacheRef& pipelineCacheRef, Device *device = nullptr );

	void						setRenderPass( const VkRenderPass& renderPass );
	void						setSubPass( const uint32_t subPass );
	void						setPipelineLayout( const VkPipelineLayout& layout );

	void						setVertexInputBindingDescriptions( const std::vector<VkVertexInputBindingDescription>& vertexBindingDescriptions );
	void						setVertexInputAttributeDescriptions( const std::vector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions );

	void						setTopology( VkPrimitiveTopology topology );

	void						setCullMode( VkCullModeFlagBits cullMode );
	void						setCullModeNone() { setCullMode( VK_CULL_MODE_NONE ); }
	void						setCullModeFront() { setCullMode( VK_CULL_MODE_FRONT_BIT ); }
	void						setCullModeBack() { setCullMode( VK_CULL_MODE_BACK_BIT ); }
	void						setCullModeFrontAndBack() { setCullMode( VK_CULL_MODE_FRONT_AND_BACK ); }

	void						setFrontFace( VkFrontFace frontFace );
	void						setFrontFaceClockWise() { setFrontFace( VK_FRONT_FACE_CLOCKWISE ); }
	void						setFrontFaceCounterClockWise() { setFrontFace( VK_FRONT_FACE_COUNTER_CLOCKWISE ); }

	void						setDepthBias( VkBool32 enable, float slopeFactor, float constantFactor, float clamp );

	void						setRasterizationSamples( VkSampleCountFlagBits samples );

	void						setDepthTest( VkBool32 depthTestEnable );
	void						setDepthWrite( VkBool32 depthWriteEnable );

	void						setColorBlendAttachments( const std::vector<VkPipelineColorBlendAttachmentState>& colorBlends );
	const std::vector<VkPipelineColorBlendAttachmentState>& getColorBlendAttachments() const { return mColorBlendAttachments.getData(); }

	void						setShaderStages( const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages );

	VkPipeline					getSelectedPipeline() const;
	void						invalidate();

private:
	void initializeDefaultValues();

	//! \class HashData
	template <typename T> 
	class HashData {
	public:
		HashData() {}
		virtual ~HashData() {}

		T&			getData() { return mData; }
		const T&	getData() const { return mData; }

		void		setDirty( bool value = true ) { mDirty = value; }
		void		clearDirty() { setDirty( false ); }
		bool		isDirty() const { return mDirty; }

		uint32_t getHash() const { 
			if( mDirty ) {
				mHash = util_hash::calculateHash( mData );
				mDirty = false;
			}
			return mHash; 
		}

	private:
		T					mData;
		mutable bool		mDirty = false;
		mutable uint32_t	mHash = 0;
	};

	//! \class HashSet
	struct HashSet {
		union {
			struct { uint64_t value64[4]; };
			struct { uint32_t value32[8]; };
		};

		HashSet() {
			std::memset( &value32, 0, 8*sizeof( uint32_t ) );
		}

		virtual ~HashSet() {}

		bool isZero() const {
			return	( 0 == value64[0] ) && ( 0 == value64[1] ) && ( 0 == value64[2] ) && ( 0 == value64[3] );
		}

		bool operator==( const HashSet& rhs ) const {
			return	( value64[0] == rhs.value64[0] ) && ( value64[1] == rhs.value64[1] ) && ( value64[2] == rhs.value64[2] ) && ( value64[3] == rhs.value64[3] );
		}

		bool operator!=( const HashSet& rhs ) const {
			return	( value64[0] != rhs.value64[0] ) || ( value64[1] != rhs.value64[1] ) || ( value64[2] != rhs.value64[2] ) || ( value64[3] != rhs.value64[3] );
		}

		//bool operator<( const HashSet& rhs ) const {
		//	return	( value64[0] < rhs.value64[0] ) && ( value64[1] < rhs.value64[1] ) && ( value64[2] < rhs.value64[2] ) && ( value64[3] < rhs.value64[3] );
		//}
	};

	//! \class PipelineParameters
	struct PipelineParameters {
		VkRenderPass								mRenderPass		= VK_NULL_HANDLE;
		uint32_t									mSubPass		= 0;
		VkPipelineLayout							mPipelineLayout	= VK_NULL_HANDLE;
	};

	//! \class PipelineStates
	struct PipelineStates {
		VkPipelineVertexInputStateCreateInfo		mVertexInputState;
		VkPipelineInputAssemblyStateCreateInfo		mInputAssemblyState;
		VkPipelineTessellationStateCreateInfo		mTessellationState;
		VkPipelineViewportStateCreateInfo			mViewportState;
		VkPipelineRasterizationStateCreateInfo		mRasterizationState;
		VkPipelineMultisampleStateCreateInfo		mMultisampleState;
		VkPipelineDepthStencilStateCreateInfo		mDepthStencilState;
		VkPipelineColorBlendStateCreateInfo			mColorBlendState;
		VkPipelineDynamicStateCreateInfo			mDynamicState;
	};


	HashData<PipelineSelector::PipelineParameters>					mPipelineParameters;
	HashData<std::vector<VkVertexInputBindingDescription>>			mVertexBindingDescriptions;
	HashData<std::vector<VkVertexInputAttributeDescription>>		mVertexAttributeDescriptions;
	HashData<PipelineSelector::PipelineStates>						mPipelineStates;
	HashData<std::vector<VkPipelineShaderStageCreateInfo>>			mPipelineShaderStages;
	HashData<std::vector<VkDynamicState>>							mDynamicStates;
	HashData<std::vector<VkPipelineColorBlendAttachmentState>>		mColorBlendAttachments;

	vk::Device*														mDevice = nullptr;
	vk::PipelineCacheRef											mPipelineCacheRef;
	mutable std::mutex												mMutex;
	mutable std::vector<std::pair<HashSet, vk::PipelineRef>>		mPipelines;	
	mutable HashSet													mSelectedPipelineHash;
	mutable VkPipeline												mSelectedPipeline = VK_NULL_HANDLE;

	// Debug
	VkCullModeFlagBits	mCurrentCullMode = VK_CULL_MODE_NONE;
	VkFrontFace			mCurrentFrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
};

}} // namespace cinder::vk