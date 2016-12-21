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
#include "cinder/vk/glsl_types.h"

#include "cinder/GeomIo.h"
#include "cinder/Matrix.h"
#include "cinder/Vector.h"

#include <map>

namespace cinder { namespace vk {

class CommandBuffer;
class Context;
class Device;
class TextureBase;
class UniformBuffer;
class UniformSet;
using CommandBufferRef = std::shared_ptr<CommandBuffer>;
using TextureBaseRef = std::shared_ptr<TextureBase>;
using UniformBufferRef = std::shared_ptr<UniformBuffer>;
using UniformSetRef = std::shared_ptr<UniformSet>;

// Uniform semantics
enum UniformSemantic {
	UNIFORM_MODEL_MATRIX,
	UNIFORM_MODEL_MATRIX_INVERSE,
	UNIFORM_MODEL_MATRIX_INVERSE_TRANSPOSE,
	UNIFORM_VIEW_MATRIX,
	UNIFORM_VIEW_MATRIX_INVERSE,
	UNIFORM_MODEL_VIEW,
	UNIFORM_MODEL_VIEW_INVERSE,
	UNIFORM_MODEL_VIEW_INVERSE_TRANSPOSE,
	UNIFORM_MODEL_VIEW_PROJECTION,
	UNIFORM_MODEL_VIEW_PROJECTION_INVERSE,
	UNIFORM_PROJECTION_MATRIX,
	UNIFORM_PROJECTION_MATRIX_INVERSE,
	UNIFORM_VIEW_PROJECTION,
	UNIFORM_NORMAL_MATRIX,
	UNIFORM_VIEWPORT_MATRIX,
	UNIFORM_WINDOW_SIZE,
	UNIFORM_ELAPSED_SECONDS,
	UNIFORM_USER_DEFINED
};

enum {
	INVALID_BINDING	= 0xFFFFFFFF,
	DEFAULT_SET		= 0x0,
};

enum {
	CHANGES_DONTCARE	= 0x00000000, // Don't care how often writes happen
	CHANGES_SELDOM		= 0x00000001, // Once or twice after initial write
	CHANGES_REGULAR		= 0x00000002, // On state changes
	CHANGES_FREQUENT	= 0x00000004, // Every frame or every few frames
};

//! \class UniformLayout
//!
//!
class UniformLayout {
public:

	union Value {
		float		f;
		int32_t		i;
		uint32_t	u = 0;
	};

	using ValueStore = std::vector<std::vector<Value>>;

	//! \class Uniform
	//
	//
	class Uniform {
	public:

		Uniform() {}
		Uniform( const std::string& name, GlslUniformDataType dataType );
		virtual ~Uniform() {}

		const std::string&					getName() const { return mName; }
		GlslUniformDataType					getDataType() const { return mDataType; }
		size_t								getOffset() const { return mOffset; }
		uint32_t							getArraySize() const  { return mArraySize; }
		UniformSemantic						getUniformSemantic() const { return mSemantic; }
	private:
		std::string							mName;
		GlslUniformDataType					mDataType	= glsl_unknown;
		uint32_t							mOffset		= 0;
		uint32_t							mArraySize	= 1;
		UniformSemantic						mSemantic	= UniformSemantic::UNIFORM_USER_DEFINED;

		void setOffset( uint32_t offset ) { mOffset = offset; }
		void setArraySize( uint32_t arraySize ) { mArraySize = std::max<uint32_t>( 1, arraySize ); }
		friend class UniformLayout;
	};

	// ---------------------------------------------------------------------------------------------

	class PushConstant : public vk::UniformLayout::Uniform {
	public:

		PushConstant() {}
		PushConstant( VkShaderStageFlags shaderStages, const vk::UniformLayout::Uniform& uniform )
			: vk::UniformLayout::Uniform( uniform ), mShaderStages( shaderStages ) {}
		virtual ~PushConstant() {}

		VkShaderStageFlags					getShaderStages() const { return mShaderStages; }
		uint32_t							getSize() const;
	private:
		VkShaderStageFlags					mShaderStages = 0;
	};

	// ---------------------------------------------------------------------------------------------
	
	//! \class Block
	//
	//
	class Block {
	public:

		using UniformStore = std::pair<Uniform, ValueStore>;

		Block() {}
		virtual ~Block() {}

		const std::vector<UniformStore>&	getUniforms() const { return mUniforms; }
		size_t								getSizeBytes() const { return mSizeBytes; }

		void								sortUniformsByOffset();

	private:
		std::vector<UniformStore>			mUniforms;
		size_t								mSizeBytes = 0;

		UniformStore* findUniformObject( const std::string& name, GlslUniformDataType dataType, bool addIfNotExits = false );
		void setSizeBytes( size_t size ) { mSizeBytes = size; }
		friend class UniformLayout;
	};

	// ---------------------------------------------------------------------------------------------

	//! \class Binding
	//
	//
	class Binding {
	public:

		enum class Type { 
			UNDEFINED		= 0, 
			BLOCK			= 0x000000001, 
			SAMPLER			= 0x000000002,
			STORAGE_IMAGE	= 0x000000004,
			STORAGE_BUFFER	= 0x000000008,
			PUSH_CONSTANTS	= 0x000000010,
			ANY_BLOCK		= BLOCK | PUSH_CONSTANTS,
			ANY_IMAGE		= SAMPLER | STORAGE_IMAGE,
			ANY				= BLOCK | SAMPLER | STORAGE_IMAGE | STORAGE_BUFFER | PUSH_CONSTANTS
		};

		Binding() {}
		Binding( const std::string& name, Binding::Type type, uint32_t set = DEFAULT_SET );
		virtual ~Binding() {}

		bool								isDirty() const { return mDirty; }
		void								setDirty( bool value = true ) { mDirty = value; }
		void								clearDirty() { setDirty( false ); }

		uint32_t							getSet() const { return mSet; }
		uint32_t							getBinding() const { return mBinding; }
		VkShaderStageFlags					getShaderStages() const { return mShaderStages; }
		Binding::Type						getType() const { return mType; }
		const std::string&					getName() const { return mName; } 
		bool								isBlock() const { return Binding::Type::BLOCK == mType; }
		bool								isSampler() const { return Binding::Type::SAMPLER == mType; }
		bool								isStorageImage() const { return Binding::Type::STORAGE_IMAGE == mType; }
		bool								isStorageBuffer() const { return Binding::Type::STORAGE_BUFFER == mType; }
		bool								isPushConstants() const { return Binding::Type::PUSH_CONSTANTS == mType; }

		const Block&						getBlock() const { return mBlock; }
		
		const vk::TextureBaseRef&			getTexture() const { return mTexture; }
		void								setTexture( const vk::TextureBaseRef& texture );

		void								sortByOffset();

		std::vector<UniformLayout::PushConstant>	getPushConstants() const;

	private:
		bool								mDirty = false;

		uint32_t							mSet = DEFAULT_SET;
		uint32_t							mBinding = INVALID_BINDING;
		VkShaderStageFlags					mShaderStages = 0;
		Binding::Type						mType = Binding::Type::UNDEFINED;
		std::string							mName;

		// Uniform data
		Block								mBlock;
		// Sampler data
		vk::TextureBaseRef					mTexture;

		// These functions force their type since in most cases mBinding is set before it's know what the binding is needed.
		void setBinding( uint32_t binding, uint32_t set ) { mBinding = binding; mSet = set; }
		void setBlockSizeBytes( size_t sizeBytes ) { mBlock.setSizeBytes( sizeBytes ); }
		void setShaderStages( VkShaderStageFlags stages, bool exclusive = false ) { if( exclusive ) { mShaderStages = stages; } else { mShaderStages |= stages; } }
		friend class UniformLayout;
	};


	// ---------------------------------------------------------------------------------------------

	//! \class Set
	//
	//
	class Set {
	public:
		Set( uint32_t set, uint32_t changeFrequency )
			: mSet( set ), mChangeFrequency( changeFrequency ) {}
		virtual ~Set() {}

		uint32_t	getSet() const { return mSet; }
		uint32_t	getChangeFrequency() const { return mChangeFrequency; }
	private:
		uint32_t	mSet = DEFAULT_SET;
		// The higher the value the more frequently the descriptor set is expected to change.
		uint32_t	mChangeFrequency = CHANGES_DONTCARE;
		friend class UniformLayout;
	};

	// ---------------------------------------------------------------------------------------------

	UniformLayout();
	virtual ~UniformLayout();


	void								addBinding( vk::UniformLayout::Binding::Type bindingType, const std::string& bindingName, uint32_t bindingNumber, VkShaderStageFlags bindingStages, uint32_t setNumber );
	UniformLayout&						setBinding( const std::string& bindingName, uint32_t bindingNumber, VkShaderStageFlags bindingStages, uint32_t setNumber );
	const std::vector<Binding>&			getBindings() const { return mBindings; }

	std::vector<UniformLayout::PushConstant>	getPushConstants() const;

	void								addSet( uint32_t setNumber, uint32_t changeFrequency );
	UniformLayout&						setSet( uint32_t setNumber, uint32_t changeFrequency );
	const std::vector<Set>&				getSets() const { return mSets; }

	UniformLayout&						addUniform( const std::string& name, GlslUniformDataType dataType, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const float& value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const vec2&  value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const vec3&  value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const vec4&  value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const mat2&  value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const mat3&  value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const mat4&  value, uint32_t offset, uint32_t arraySize = 1 );
	UniformLayout&						addUniform( const std::string& name, const vk::TextureBaseRef& texture );

	void								setBlockSizeBytes( const std::string& name, size_t sizeBytes );
	void								sortByOffset();

	// Update uniform variables
	void								uniform( const std::string& name, const float    value );
	void								uniform( const std::string& name, const int32_t  value );
	void								uniform( const std::string& name, const uint32_t value );
	void								uniform( const std::string& name, const bool     value );
	void								uniform( const std::string& name, const vec2&    value );
	void								uniform( const std::string& name, const vec3&    value );
	void								uniform( const std::string& name, const vec4&    value );
	void								uniform( const std::string& name, const ivec2&   value );
	void								uniform( const std::string& name, const ivec3&   value );
	void								uniform( const std::string& name, const ivec4&   value );
	void								uniform( const std::string& name, const uvec2&   value );
	void								uniform( const std::string& name, const uvec3&   value );
	void								uniform( const std::string& name, const uvec4&   value );
	void								uniform( const std::string& name, const mat2&    value );
	void								uniform( const std::string& name, const mat3&    value );
	void								uniform( const std::string& name, const mat4&    value );
	void								uniform( const std::string& name, const vk::TextureBaseRef& texture );

private:
	std::vector<Binding>				mBindings;
	std::vector<Set>					mSets;

	Binding*							findBindingObject( const std::string& name, Binding::Type bindingType, bool addIfNotExits = false );
	void								addUniformImpl( GlslUniformDataType dataType, const std::string& name, uint32_t offset, uint32_t arraySize );

	template <typename T>
	void setUniformValue( GlslUniformDataType dataType, const std::string& name, const std::vector<T>& values );
	friend class ShaderProg;
};

//! \class UniformSet
//!
//!
class UniformSet { 
public:

	//! \class Binding
	//!
	//!
	class Binding : public UniformLayout::Binding {
	public:
		Binding() {}
		Binding( const UniformLayout::Binding& binding )
			: UniformLayout::Binding( binding ) {}
		virtual ~Binding() {}

		const UniformBufferRef&		getUniformBuffer() const { return mUniformBuffer; }
		void						setUniformBuffer( const UniformBufferRef& buffer );

	private:
		UniformBufferRef			mUniformBuffer; 
	};

	//! \class Set
	//!
	//!
	class Set {
	public:

		Set( uint32_t set, uint32_t changeFrequency ) : mSet( set ), mChangeFrequency( changeFrequency ) {}
		virtual ~Set() {}

		uint32_t											getSet() const { return mSet; }
		uint32_t											getChangeFrequency() const { return mChangeFrequency; }
		const std::vector<Binding>&							getBindings() const { return mBindings; }
		const std::vector<VkDescriptorSetLayoutBinding>&	getDescriptorSetlayoutBindings() const { return mDescriptorSetLayoutBindings; } 

		std::vector<VkWriteDescriptorSet>					getBindingUpdates( VkDescriptorSet parentDescriptorSet );

	private:
		uint32_t											mSet = DEFAULT_SET;
		uint32_t											mChangeFrequency = CHANGES_DONTCARE;
		std::vector<Binding>								mBindings;
		std::vector<VkDescriptorSetLayoutBinding>			mDescriptorSetLayoutBindings;
		friend class UniformSet;
	};
	
	using SetRef = std::shared_ptr<UniformSet::Set>;

	// ---------------------------------------------------------------------------------------------

	//! \class Binding
	//!
	//!
	class Options {
	public:
		Options() {}
		virtual ~Options() {}
		Options&	setTransientAllocation( bool value = true ) { mTransientAllocation = value; return *this; }
		bool		getTransientAllocation() const { return mTransientAllocation; }
	private:
		bool		mTransientAllocation = false;
		friend class UniformSet;
	};

	// ---------------------------------------------------------------------------------------------

	UniformSet( const UniformLayout& layout, const UniformSet::Options& options, vk::Device *device );
	virtual ~UniformSet();

	static UniformSetRef				create( const UniformLayout& layout, const UniformSet::Options& options = UniformSet::Options(), vk::Device *device = nullptr );

	std::vector<UniformLayout::PushConstant>						getPushConstants() const;

	const std::vector<UniformSet::SetRef>&							getSets() const { return mSets; }
	const std::vector<std::vector<VkDescriptorSetLayoutBinding>>&	getCachedDescriptorSetLayoutBindings() const { return mCachedDescriptorSetLayoutBindings; }

	void							uniform( const std::string& name, const float    value );
	void							uniform( const std::string& name, const int32_t  value );
	void							uniform( const std::string& name, const uint32_t value );
	void							uniform( const std::string& name, const bool     value );
	void							uniform( const std::string& name, const vec2&    value );
	void							uniform( const std::string& name, const vec3&    value );
	void							uniform( const std::string& name, const vec4&    value );
	void							uniform( const std::string& name, const ivec2&   value );
	void							uniform( const std::string& name, const ivec3&   value );
	void							uniform( const std::string& name, const ivec4&   value );
	void							uniform( const std::string& name, const uvec2&   value );
	void							uniform( const std::string& name, const uvec3&   value );
	void							uniform( const std::string& name, const uvec4&   value );
	void							uniform( const std::string& name, const mat2&    value );
	void							uniform( const std::string& name, const mat3&    value );
	void							uniform( const std::string& name, const mat4&    value );
	void							uniform( const std::string& name, const TextureBaseRef& texture );

	void							setDefaultUniformVars( vk::Context *context );
	void							bufferPending( const vk::CommandBufferRef& cmdBuf, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask );

	void							echoValues( std::ostream& os );

private:
	UniformSet::Options				mOptions;

	std::vector<UniformSet::SetRef>							mSets;
	std::vector<std::vector<VkDescriptorSetLayoutBinding>>	mCachedDescriptorSetLayoutBindings;

	Binding*						findBindingObject( const std::string& name, Binding::Type bindingType );

	template <typename T>
	void updateUniform( const std::string& name, const T& value );
};

// -------------------------------------------------------------------------------------------------
// Uniform semantics
// -------------------------------------------------------------------------------------------------
using UniformSemanticMap = std::map<std::string, UniformSemantic>;
using AttribSemanticMap = std::map<std::string, geom::Attrib>;

//! Converts a UniformSemantic to its name
std::string uniformSemanticToString( UniformSemantic uniformSemantic );
//! Default mapping from uniform name to semantic. Can be modified via the reference. Not thread-safe.
const vk::UniformSemanticMap& getDefaultUniformNameToSemanticMap();
//! Returns semantic for default name
UniformSemantic uniformNameToSemantic( const std::string& name );
//! Default mapping from attribute name to semantic. Can be modified via the reference. Not thread-safe.
const vk::AttribSemanticMap& getDefaultAttribNameToSemanticMap();
//! Returns semantic for default name
geom::Attrib attributeNameToSemantic( const std::string& name );

}} // namespace cinder::vk
