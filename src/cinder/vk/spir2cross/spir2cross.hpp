/*
 * Copyright 2015-2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SPIR2CROSS_HPP
#define SPIR2CROSS_HPP

#include "spirv.hpp"
#include <vector>
#include <string>
#include <stdexcept>
#include <memory>
#include <utility>
#include <unordered_set>
#include <unordered_map>

#include "spir2common.hpp"

namespace spir2cross
{
    struct Resource
    {
        // Resources are identified with their SPIR-V ID.
        // This is the ID of the OpVariable.
        uint32_t id;

        // The type of the declared resource.
        uint32_t type_id;

        // The declared name (OpName) of the resource.
        // For Buffer blocks, the name actually reflects the externally
        // visible Block name.
        //
        // This name can be retrieved again by using either
        // get_name(id) or get_name(type_id) depending if it's a buffer block or not.
        //
        // This name can be an empty string in which case get_fallback_name(id) can be
        // used which obtains a suitable fallback identifier for an ID.
        std::string name;
    };

    struct ShaderResources
    {
        std::vector<Resource> uniform_buffers;
        std::vector<Resource> storage_buffers;
        std::vector<Resource> stage_inputs;
        std::vector<Resource> stage_outputs;
        std::vector<Resource> subpass_inputs;
        std::vector<Resource> storage_images;
        std::vector<Resource> sampled_images;
        std::vector<Resource> atomic_counters;

        // There can only be one push constant block,
        // but keep the vector in case this restriction is lifted in the future.
        std::vector<Resource> push_constant_buffers;
    };

    struct BufferRange
    {
        unsigned index;
        size_t offset;
        size_t range;
    };

    class Compiler
    {
        public:
            // The constructor takes a buffer of SPIR-V words and parses it.
            Compiler(std::vector<uint32_t> ir);

            // After parsing, API users can modify the SPIR-V via reflection and call this
            // to disassemble the SPIR-V into the desired langauage.
            // Sub-classes actually implement this.
            virtual std::string compile();

            // Gets the identifier (OpName) of an ID. If not defined, an empty string will be returned.
            const std::string& get_name(uint32_t id) const;

            // Applies a decoration to an ID. Effectively injects OpDecorate.
            void set_decoration(uint32_t id, spv::Decoration decoration, uint32_t argument = 0);

            // Overrides the identifier OpName of an ID.
            // Identifiers beginning with underscores or identifiers which contain double underscores
            // are reserved by the implementation.
            void set_name(uint32_t id, const std::string& name);

            // Gets a bitmask for the decorations which are applied to ID.
            // I.e. (1ull << spv::DecorationFoo) | (1ull << spv::DecorationBar)
            uint64_t get_decoration_mask(uint32_t id) const;

            // Gets the value for decorations which take arguments.
            // If decoration doesn't exist or decoration is not recognized,
            // 0 will be returned.
            uint32_t get_decoration(uint32_t id, spv::Decoration decoration) const;

            // Removes the decoration for a an ID.
            void unset_decoration(uint32_t id, spv::Decoration decoration);

            // Gets the SPIR-V associated with ID.
            // Mostly used with Resource::type_id to parse the underlying type of a resource.
            const SPIRType& get_type(uint32_t id) const;

            // Gets the underlying storage class for an OpVariable.
            spv::StorageClass get_storage_class(uint32_t id) const;

            // If get_name() is an empty string, get the fallback name which will be used
            // instead in the disassembled source.
            virtual const std::string get_fallback_name(uint32_t id) const
            {
                return join("_", id);
            }

			// Gets the member count associated with an OpTypeStruct in id, e.g. the 
			// fields of a uniform block.
			uint32_t get_member_count(uint32_t id) const;

            // Given an OpTypeStruct in ID, obtain the identifier for member number "index".
            // This may be an empty string.
            const std::string& get_member_name(uint32_t id, uint32_t index) const;

            // Given an OpTypeStruct in ID, obtain the OpMemberDecoration for member number "index".
            uint32_t get_member_decoration(uint32_t id, uint32_t index, spv::Decoration decoration) const;

            // Sets the member identifier for OpTypeStruct ID, member number "index".
            void set_member_name(uint32_t id, uint32_t index, const std::string& name);

            // Gets the decoration mask for a member of a struct, similar to get_decoration_mask.
            uint64_t get_member_decoration_mask(uint32_t id, uint32_t index) const;

            // Similar to set_decoration, but for struct members.
            void set_member_decoration(uint32_t id, uint32_t index,
                    spv::Decoration decoration, uint32_t argument = 0);

            // Unsets a member decoration, similar to unset_decoration.
            void unset_member_decoration(uint32_t id, uint32_t index, spv::Decoration decoration);

            // Gets the fallback name for a member, similar to get_fallback_name.
            virtual const std::string get_fallback_member_name(uint32_t index) const
            {
                return join("_", index);
            }

            // Returns a vector of which members of a struct are potentially in use by a
            // SPIR-V shader. The granularity of this analysis is per-member of a struct.
            // This can be used for Buffer (UBO), BufferBlock (SSBO) and PushConstant blocks.
            // ID is the Resource::id obtained from get_shader_resources().
            std::vector<BufferRange> get_active_buffer_ranges(unsigned id) const;

            // Returns the effective size of a buffer block.
            size_t get_declared_struct_size(const SPIRType &struct_type) const;

            // Legacy GLSL compatibility method.
            // Takes a variable with a block interface and flattens it into a T array[N]; array instead.
            // For this to work, all types in the block must not themselves be composites
            // (except vectors and matrices), and all types must be the same.
            // The name of the uniform will be the same as the interface block name.
            void flatten_interface_block(uint32_t id);

            // Query shader resources, use ids with reflection interface to modify or query binding points, etc.
            ShaderResources get_shader_resources() const;

        protected:
            const uint32_t* stream(uint32_t offset) const
            {
                if (offset > spirv.size())
                    throw CompilerError("Compiler::stream() out of range.");
                return &spirv[offset];
            }
            std::vector<uint32_t> spirv;

            std::vector<Instruction> inst;
            std::vector<Variant> ids;
            std::vector<Meta> meta;

            SPIRFunction *function = nullptr;
            SPIRBlock *block = nullptr;
            std::vector<uint32_t> global_variables;
            std::vector<uint32_t> aliased_variables;

            // If our IDs are out of range here as part of opcodes, throw instead of
            // undefined behavior.
            template<typename T, typename... P>
            T& set(uint32_t id, P&&... args)
            {
                auto &var = variant_set<T>(ids.at(id), std::forward<P>(args)...);
                var.self = id;
                return var;
            }

            template<typename T>
            T& get(uint32_t id)
            {
                return variant_get<T>(ids.at(id));
            }

            template<typename T>
            T* maybe_get(uint32_t id)
            {
                if (ids.at(id).get_type() == T::type)
                    return &get<T>(id);
                else
                    return nullptr;
            }

            template<typename T>
            const T& get(uint32_t id) const
            {
                return variant_get<T>(ids.at(id));
            }

            template<typename T>
            const T* maybe_get(uint32_t id) const
            {
                if (ids.at(id).get_type() == T::type)
                    return &get<T>(id);
                else
                    return nullptr;
            }

            struct execution
            {
                uint64_t flags = 0;
                spv::ExecutionModel model;
                uint32_t entry_point = 0;
                struct
                {
                    uint32_t x = 0, y = 0, z = 0;
                } workgroup_size;
                uint32_t invocations = 0;
                uint32_t output_vertices = 0;
				execution() {}
            } execution;

            struct source
            {
                uint32_t version = 0;
                bool es = false;
                bool known = false;
				source() {}
            } source;

            std::unordered_set<uint32_t> loop_block;
            std::unordered_set<uint32_t> continue_block;
            std::unordered_set<uint32_t> loop_merge_target;
            std::unordered_set<uint32_t> selection_merge_target;
            std::unordered_set<uint32_t> multiselect_merge_target;

            std::string to_name(uint32_t id);
            bool is_builtin_variable(const SPIRVariable &var) const;
            bool is_immutable(uint32_t id) const;
            bool is_member_builtin(const SPIRType &type, uint32_t index, spv::BuiltIn *builtin) const;
            const SPIRType& expression_type(uint32_t id) const;
            bool expression_is_lvalue(uint32_t id) const;
            bool variable_storage_is_aliased(const SPIRVariable &var);
            SPIRVariable* maybe_get_backing_variable(uint32_t chain);

            void register_read(uint32_t expr, uint32_t chain, bool forwarded);
            void register_write(uint32_t chain);

            inline bool is_continue(uint32_t next) const
            {
                return continue_block.find(next) != end(continue_block);
            }

            inline bool is_break(uint32_t next) const
            {
                return loop_merge_target.find(next) != end(loop_merge_target) ||
                    multiselect_merge_target.find(next) != end(multiselect_merge_target);
            }

            inline bool is_conditional(uint32_t next) const
            {
                return selection_merge_target.find(next) != end(selection_merge_target) &&
                    multiselect_merge_target.find(next) == end(multiselect_merge_target);
            }

            // Dependency tracking for temporaries read from variables.
            void flush_dependees(SPIRVariable &var);
            void flush_all_active_variables();
            void flush_all_atomic_capable_variables();
            void flush_all_aliased_variables();
            void register_global_read_dependencies(const SPIRBlock &func, uint32_t id);
            void register_global_read_dependencies(const SPIRFunction &func, uint32_t id);
            std::unordered_set<uint32_t> invalid_expressions;

            void update_name_cache(std::unordered_set<std::string> &cache, std::string &name);
            std::unordered_set<std::string> global_struct_cache;

            bool function_is_pure(const SPIRFunction &func);
            bool block_is_pure(const SPIRBlock &block);
            bool block_is_outside_flow_control_from_block(const SPIRBlock &from, const SPIRBlock &to);

            bool execution_is_branchless(const SPIRBlock &from, const SPIRBlock &to) const;
            bool execution_is_noop(const SPIRBlock &from, const SPIRBlock &to) const;
            SPIRBlock::ContinueBlockType continue_block_type(const SPIRBlock &continue_block) const;

            bool force_recompile = false;

            uint32_t type_struct_member_offset(const SPIRType &type, uint32_t index) const;
            uint32_t type_struct_member_array_stride(const SPIRType &type, uint32_t index) const;

            bool block_is_loop_candidate(const SPIRBlock &block, SPIRBlock::Method method) const;

        private:
            void parse();
            void parse(const Instruction &i);

            // Used internally to implement various traversals for queries.
            struct OpcodeHandler
            {
                virtual ~OpcodeHandler() = default;

                // Return true if traversal should continue.
                // If false, traversal will end immediately.
                virtual bool handle(spv::Op opcode, const uint32_t *args, uint32_t length) = 0;
            };

            struct BufferAccessHandler : OpcodeHandler
            {
                BufferAccessHandler(const Compiler &compiler, std::vector<BufferRange> &ranges, unsigned id)
                    : compiler(compiler), ranges(ranges), id(id) {}

                bool handle(spv::Op opcode, const uint32_t *args, uint32_t length) override;

                const Compiler &compiler;
                std::vector<BufferRange> &ranges;
                uint32_t id;

                std::unordered_set<uint32_t> seen;
            };

            bool traverse_all_reachable_opcodes(const SPIRBlock &block, OpcodeHandler &handler) const;
            bool traverse_all_reachable_opcodes(const SPIRFunction &block, OpcodeHandler &handler) const;

            size_t get_declared_struct_member_size(const SPIRType &struct_type, uint32_t index) const;
    };
}

#endif
