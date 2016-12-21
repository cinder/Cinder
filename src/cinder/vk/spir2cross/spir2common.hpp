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

#ifndef SPIR2COMMON_HPP
#define SPIR2COMMON_HPP

#include <sstream>
#include <stdio.h>
#include <string.h>

namespace spir2cross
{
    class CompilerError : public std::runtime_error
    {
        public:
            CompilerError(const std::string &str) : std::runtime_error(str) {}
    };

    namespace inner
    {
        template<typename T>
        void join_helper(std::ostringstream &stream, T&& t)
        {
            stream << std::forward<T>(t);
        }

        template<typename T, typename... Ts>
        void join_helper(std::ostringstream &stream, T&& t, Ts&&... ts)
        {
            stream << std::forward<T>(t);
            join_helper(stream, std::forward<Ts>(ts)...);
        }
    }

    // Helper template to avoid lots of nasty string temporary munging.
    template<typename... Ts>
    std::string join(Ts&&... ts)
    {
        std::ostringstream stream;
        inner::join_helper(stream, std::forward<Ts>(ts)...);
        return stream.str();
    }

    inline std::string merge(const std::vector<std::string> &list)
    {
        std::string s;
        for (auto &elem : list)
        {
            s += elem;
            if (&elem != &list.back())
                s += ", ";
        }
        return s;
    }

    template<typename T>
    inline std::string convert_to_string(T&& t)
    {
        return std::to_string(std::forward<T>(t));
    }

    inline std::string convert_to_string(float t)
    {
        // std::to_string for floating point values is broken.
        // Fallback to something more sane.
        char buf[64];
        sprintf(buf, "%.32g", t);
        // Ensure that the literal is float.
        if (!strchr(buf, '.') && !strchr(buf, 'e'))
            strcat(buf, ".0");
        return buf;
    }

    inline std::string convert_to_string(double t)
    {
        // std::to_string for floating point values is broken.
        // Fallback to something more sane.
        char buf[64];
        sprintf(buf, "%.32g", t);
        // Ensure that the literal is float.
        if (!strchr(buf, '.') && !strchr(buf, 'e'))
            strcat(buf, ".0");
        return buf;
    }

    struct Instruction
    {
        Instruction(const std::vector<uint32_t> &spirv, uint32_t &index);

        uint16_t op;
        uint16_t count;
        uint32_t offset;
        uint32_t length;
    };

    // Helper for Variant interface.
    struct IVariant
    {
        virtual ~IVariant() = default;
        uint32_t self = 0;
    };

    enum Types
    {
        TypeNone,
        TypeType,
        TypeVariable,
        TypeConstant,
        TypeFunction,
        TypeFunctionPrototype,
        TypePointer,
        TypeBlock,
        TypeExtension,
        TypeExpression,
        TypeUndef
    };

    struct SPIRUndef : IVariant
    {
        enum { type = TypeUndef };
        SPIRUndef(uint32_t basetype) : basetype(basetype) {}
        uint32_t basetype;
    };

    struct SPIRType : IVariant
    {
        enum { type = TypeType };

        enum BaseType
        {
            Unknown,
            Void,
            Bool,
            Int,
            UInt,
            AtomicCounter,
            Float,
            Struct,
            Image,
            SampledImage,
            Sampler
        };

        // Scalar/vector/matrix support.
        BaseType basetype = Unknown;
        uint32_t width = 0;
        uint32_t vecsize = 1;
        uint32_t columns = 1;

        // Arrays, suport array of arrays by having a vector of array sizes.
        std::vector<uint32_t> array;

        // Pointers
        bool pointer = false;
        spv::StorageClass storage = spv::StorageClassGeneric;

        std::vector<uint32_t> member_types;

        struct Image
        {
            uint32_t type;
            spv::Dim dim;
            bool depth;
            bool arrayed;
            bool ms;
            uint32_t sampled;
            spv::ImageFormat format;
        } image;
    };

    struct SPIRExtension : IVariant
    {
        enum { type = TypeExtension };

        enum Extension
        {
            GLSL
        };

        SPIRExtension(Extension ext)
            : ext(ext) {}

        Extension ext;    
    };

    struct SPIRExpression : IVariant
    {
        enum { type = TypeExpression };

        // Only created by the backend target to avoid creating tons of temporaries.
        SPIRExpression(std::string expr, uint32_t expression_type, bool immutable)
            : expression(move(expr)), expression_type(expression_type), immutable(immutable) {}

        // If non-zero, prepend expression with to_expression(base_expression).
        // Used in amortizing multiple calls to to_expression()
        // where in certain cases that would quickly force a temporary when not needed.
        uint32_t base_expression = 0;

        std::string expression;
        uint32_t expression_type = 0;

        // If this expression is a forwarded load,
        // allow us to reference the original variable.
        uint32_t loaded_from = 0;

        // If this expression will never change, we can avoid lots of temporaries
        // in high level source.
        bool immutable = false;

        // If this expression has been used while invalidated.
        bool used_while_invalidated = false;

        // A list of a variables for which this expression was invalidated by.
        std::vector<uint32_t> invalidated_by;
    };

    struct SPIRFunctionPrototype : IVariant
    {
        enum { type = TypeFunctionPrototype };

        SPIRFunctionPrototype(uint32_t return_type)
            : return_type(return_type) {}

        uint32_t return_type;
        std::vector<uint32_t> parameter_types;
    };

    struct SPIRBlock : IVariant
    {
        enum { type = TypeBlock };

        enum Terminator
        {
            Unknown,
            Direct, // Emit next block directly without a particular condition.

            Select, // Block ends with an if/else block.
            MultiSelect, // Block ends with switch statement.
            Loop, // Block ends with a loop.

            Return, // Block ends with return.
            Unreachable, // Noop
            Kill // Discard
        };

        enum Merge
        {
            MergeNone,
            MergeLoop,
            MergeSelection
        };

        enum Method
        {
            MergeToSelectForLoop,
            MergeToDirectForLoop
        };

        enum ContinueBlockType
        {
            ContinueNone,

            // Continue block is branchless and has at least one instruction.
            ForLoop,

            // Noop continue block.
            WhileLoop,

            // Continue block is conditional.
            DoWhileLoop,

            // Highly unlikely that anything will use this,
            // since it is really awkward/impossible to express in GLSL.
            ComplexLoop
        };

        enum { NoDominator = 0xffffffffu };

        Terminator terminator = Unknown;
        Merge merge = MergeNone;
        uint32_t next_block = 0;
        uint32_t merge_block = 0;
        uint32_t continue_block = 0;

        uint32_t return_value = 0; // If 0, return nothing (void).
        uint32_t condition = 0;
        uint32_t true_block = 0;
        uint32_t false_block = 0;
        uint32_t default_block = 0;

        std::vector<Instruction> ops;

        struct Phi
        {
            uint32_t local_variable; // flush local variable ...
            uint32_t parent; // If we're in from_block and want to branch into this block ...
            uint32_t function_variable; // to this function-global "phi" variable first.
        };

        // Before entering this block flush out local variables to magical "phi" variables.
        std::vector<Phi> phi_variables;

        // Declare these temporaries before beginning the block.
        // Used for handling complex continue blocks which have side effects.
        std::vector<std::pair<uint32_t, uint32_t>> declare_temporary;

        struct Case
        {
            uint32_t value;
            uint32_t block;
        };
        std::vector<Case> cases;

        // If we have tried to optimize code for this block but failed,
        // keep track of this.
        bool disable_block_optimization = false;

        // If the continue block is complex, fallback to "dumb" for loops.
        bool complex_continue = false;

        // The dominating block which this block might be within.
        // Used in continue; blocks to determine if we really need to write continue.
        uint32_t loop_dominator = 0;
    };

    struct SPIRFunction : IVariant
    {
        enum { type = TypeFunction };

        SPIRFunction(uint32_t return_type, uint32_t function_type)
            : return_type(return_type), function_type(function_type)
        {}

        struct Parameter
        {
            uint32_t type;
            uint32_t id;
            uint32_t read_count;
            uint32_t write_count;
        };

        uint32_t return_type;
        uint32_t function_type;
        std::vector<Parameter> arguments;
        std::vector<uint32_t> local_variables;
        uint32_t entry_block = 0;
        std::vector<uint32_t> blocks;

        void add_local_variable(uint32_t id)
        {
            local_variables.push_back(id);
        }

        void add_parameter(uint32_t type, uint32_t id)
        {
            // Arguments are read-only until proven otherwise.
            arguments.push_back({ type, id, 0u, 0u });
        }

        bool active = false;
        bool flush_undeclared = true;
    };

    struct SPIRVariable : IVariant
    {
        enum { type = TypeVariable };

        SPIRVariable() = default;
        SPIRVariable(uint32_t basetype, spv::StorageClass storage, uint32_t initializer = 0)
            : basetype(basetype), storage(storage), initializer(initializer)
        {}

        uint32_t basetype = 0;
        spv::StorageClass storage = spv::StorageClassGeneric;
        uint32_t decoration = 0;
        uint32_t initializer = 0;

        std::vector<uint32_t> dereference_chain;
        bool compat_builtin = false;

        // If a variable is shadowed, we only statically assign to it
        // and never actually emit a statement for it.
        // When we read the variable as an expression, just forward
        // shadowed_id as the expression.
        bool statically_assigned = false;
        uint32_t static_expression = 0;

        // Temporaries which can remain forwarded as long as this variable is not modified.
        std::vector<uint32_t> dependees;
        bool forwardable = true;

        bool deferred_declaration = false;
        bool phi_variable = false;
        bool remapped_variable = false;

        SPIRFunction::Parameter *parameter = nullptr;
    };

    struct SPIRConstant : IVariant
    {
        enum { type = TypeConstant };

        union Constant
        {
            uint32_t u32;
            int32_t i32;
            float f32;
        };

        struct ConstantVector
        {
            Constant r[4];
            uint32_t vecsize;
        };

        struct ConstantMatrix
        {
            ConstantVector c[4];
            uint32_t columns;
        };

        inline uint32_t scalar(uint32_t col = 0, uint32_t row = 0) const
        {
            return m.c[col].r[row].u32;
        }

        inline float scalar_f32(uint32_t col = 0, uint32_t row = 0) const
        {
            return m.c[col].r[row].f32;
        }

        inline int scalar_i32(uint32_t col = 0, uint32_t row = 0) const
        {
            return m.c[col].r[row].i32;
        }

        inline const ConstantVector& vector() const { return m.c[0]; }
        inline uint32_t vector_size() const { return m.c[0].vecsize; }
        inline uint32_t columns() const { return m.columns; }

		SPIRConstant(uint32_t constant_type, const uint32_t *elements, uint32_t num_elements) :
            constant_type(constant_type)
        {
            subconstants.insert(end(subconstants), elements, elements + num_elements);
        }

        SPIRConstant(uint32_t constant_type, uint32_t v0) :
            constant_type(constant_type)
        {
            m.c[0].r[0].u32 = v0;
            m.c[0].vecsize = 1;
            m.columns = 1;
        }

        SPIRConstant(uint32_t constant_type, uint32_t v0, uint32_t v1) :
            constant_type(constant_type)
        {
            m.c[0].r[0].u32 = v0;
            m.c[0].r[1].u32 = v1;
            m.c[0].vecsize = 2;
            m.columns = 1;
        }

        SPIRConstant(uint32_t constant_type, uint32_t v0, uint32_t v1, uint32_t v2) :
            constant_type(constant_type)
        {
            m.c[0].r[0].u32 = v0;
            m.c[0].r[1].u32 = v1;
            m.c[0].r[2].u32 = v2;
            m.c[0].vecsize = 3;
            m.columns = 1;
        }

        SPIRConstant(uint32_t constant_type, uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3) :
            constant_type(constant_type)
        {
            m.c[0].r[0].u32 = v0;
            m.c[0].r[1].u32 = v1;
            m.c[0].r[2].u32 = v2;
            m.c[0].r[3].u32 = v3;
            m.c[0].vecsize = 4;
            m.columns = 1;
        }

        SPIRConstant(uint32_t constant_type,
                const ConstantVector &vec0) :
            constant_type(constant_type)
        {
            m.columns = 1;
            m.c[0] = vec0;
        }

        SPIRConstant(uint32_t constant_type,
                const ConstantVector &vec0, const ConstantVector &vec1) :
            constant_type(constant_type)
        {
            m.columns = 2;
            m.c[0] = vec0;
            m.c[1] = vec1;
        }

        SPIRConstant(uint32_t constant_type,
                const ConstantVector &vec0, const ConstantVector &vec1,
                const ConstantVector &vec2) :
            constant_type(constant_type)
        {
            m.columns = 3;
            m.c[0] = vec0;
            m.c[1] = vec1;
            m.c[2] = vec2;
        }

        SPIRConstant(uint32_t constant_type,
                const ConstantVector &vec0, const ConstantVector &vec1,
                const ConstantVector &vec2, const ConstantVector &vec3) :
            constant_type(constant_type)
        {
            m.columns = 4;
            m.c[0] = vec0;
            m.c[1] = vec1;
            m.c[2] = vec2;
            m.c[3] = vec3;
        }

        uint32_t constant_type;
        ConstantMatrix m;
        bool specialization = false; // If the constant is a specialization constant.

		// For composites which are constant arrays, etc.
		std::vector<uint32_t> subconstants;
    };

    class Variant
    {
        public:
            // MSVC 2013 workaround, we shouldn't need these constructors.
            Variant() = default;
            Variant(Variant&& other) { *this = std::move(other); }
            Variant& operator=(Variant&& other)
            {
                if (this != &other)
                {
                    holder = move(other.holder);
                    type = other.type;
                    other.type = TypeNone;
                }
                return *this;
            }

            void set(std::unique_ptr<IVariant> val, uint32_t type)
            {
                holder = std::move(val);
                if (this->type != TypeNone && this->type != type)
                    throw CompilerError("Overwriting a variant with new type.");
                this->type = type;
            }

            template<typename T>
            T& get()
            {
                if (!holder)
                    throw CompilerError("nullptr");
                if (T::type != type)
                    throw CompilerError("Bad cast");
                return *static_cast<T*>(holder.get());
            }

            template<typename T>
            const T& get() const
            {
                if (!holder)
                    throw CompilerError("nullptr");
                if (T::type != type)
                    throw CompilerError("Bad cast");
                return *static_cast<const T*>(holder.get());
            }

            uint32_t get_type() const { return type; }
            bool empty() const { return !holder; }
            void reset() { holder.reset(); type = TypeNone; }

        private:
            std::unique_ptr<IVariant> holder;
            uint32_t type = TypeNone;
    };

    template<typename T>
    T& variant_get(Variant &var)
    {
        return var.get<T>();
    }

    template<typename T>
    const T& variant_get(const Variant &var)
    {
        return var.get<T>();
    }

    template<typename T, typename... P>
    T& variant_set(Variant &var, P&&... args)
    {
        auto uptr = std::unique_ptr<T>(new T(std::forward<P>(args)...));
        auto ptr = uptr.get();
        var.set(std::move(uptr), T::type);
        return *ptr;
    }

    struct Meta
    {
        struct Decoration
        {
            std::string alias;
            uint64_t decoration_flags = 0;
            spv::BuiltIn builtin_type;
            uint32_t location = 0;
            uint32_t set = 0;
            uint32_t binding = 0;
            uint32_t offset = 0;
            uint32_t array_stride = 0;
            bool builtin = false;
        };

        Decoration decoration;
        std::vector<Decoration> members;
    };
}

#endif

