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

#include "cinder/vk/Buffer.h"
#include "cinder/vk/UniformLayout.h"
#include "cinder/vk/wrapper.h"

namespace cinder { namespace vk {

class UniformBuffer;
using UniformBufferRef = std::shared_ptr<UniformBuffer>;

//! \class UniformBuffer
//!
//!
class UniformBuffer : public Buffer {
public:

	class Format : public vk::Buffer::Format {
	public:
		Format( VkMemoryPropertyFlags memoryProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT )
			: vk::Buffer::Format( VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, memoryProperty ) {}
		virtual ~Format() {}

	private:
		friend class VertexBuffer;
	};

	// ---------------------------------------------------------------------------------------------

	//! \class Uniform
	class Uniform : public UniformLayout::Uniform {
	public:

		Uniform() {}
		Uniform( const UniformLayout::Uniform& uniform )
			: UniformLayout::Uniform( uniform ) {}
		virtual ~Uniform() {}

		bool isDirty() const { return mDirty; }
	private:
		bool mDirty = false;
		friend class UniformBuffer;		
	};

	// ---------------------------------------------------------------------------------------------

	UniformBuffer();
	UniformBuffer( const UniformLayout::Block& block, const vk::UniformBuffer::Format& format, vk::Device *device );
	virtual ~UniformBuffer();

	//! Returns a single uniform buffer corresponding to a block
	static UniformBufferRef		create( const UniformLayout::Block& block, const vk::UniformBuffer::Format& format, vk::Device *device = nullptr );

	void						uniform( const std::string& name, const float    value );
	void						uniform( const std::string& name, const int32_t  value );
	void						uniform( const std::string& name, const uint32_t value );
	void						uniform( const std::string& name, const bool     value );
	void						uniform( const std::string& name, const vec2&    value );
	void						uniform( const std::string& name, const vec3&    value );
	void						uniform( const std::string& name, const vec4&    value );
	void						uniform( const std::string& name, const ivec2&   value );
	void						uniform( const std::string& name, const ivec3&   value );
	void						uniform( const std::string& name, const ivec4&   value );
	void						uniform( const std::string& name, const uvec2&   value );
	void						uniform( const std::string& name, const uvec3&   value );
	void						uniform( const std::string& name, const uvec4&   value );
	void						uniform( const std::string& name, const mat2&    value );
	void						uniform( const std::string& name, const mat3&    value );
	void						uniform( const std::string& name, const mat4&    value );

	const std::vector<UniformBuffer::Uniform>&	getActiveUniforms() const { return mUniforms; }
	
	void						bufferPending();

	void						echoValues( std::ostream& os );

private:
	bool								mDirty = false;
	std::vector<UniformBuffer::Uniform>	mUniforms;
	std::vector<uint8_t>				mValues;
	std::map<uint32_t, std::pair<UniformBuffer::Uniform*, size_t>>	mHashLookup;

	template <typename T> 
	void setValue( const std::string& name, const T& value );
	
	void initialize( const UniformLayout::Block& block );
	void destroy( bool removeFromTracking = true );
	friend class Device;
};

}} // namespace cinder::vk