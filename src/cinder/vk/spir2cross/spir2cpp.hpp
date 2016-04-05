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

#ifndef SPIR2CPP
#define SPIR2CPP

#include "spir2glsl.hpp"
#include <vector>

namespace spir2cross
{
    class CompilerCPP : public CompilerGLSL
    {
        public:
            CompilerCPP(std::vector<uint32_t> spirv) : CompilerGLSL(move(spirv)) {}
            std::string compile() override;

        private:
            void emit_header() override;
            void emit_c_linkage();
            void emit_function_prototype(SPIRFunction &func, uint64_t return_flags) override;

            void emit_resources();
            void emit_buffer_block(const SPIRVariable &type);
            void emit_push_constant_block(const SPIRVariable &var);
            void emit_interface_block(const SPIRVariable &type);
            void emit_block_chain(SPIRBlock &block);
            void emit_uniform(const SPIRVariable &var);
            void emit_shared(const SPIRVariable &var);

            std::string argument_decl(const SPIRFunction::Parameter &arg);

            std::vector<std::string> resource_registrations;
            std::string impl_type;
            std::string resource_type;
            uint32_t shared_counter = 0;
    };
}

#endif
