//
// Copyright (c) 2013-2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ShaderVars.h:
//  Types to represent GL variables (varyings, uniforms, etc)
//

#ifndef _COMPILER_INTERFACE_VARIABLES_
#define _COMPILER_INTERFACE_VARIABLES_

#include <string>
#include <vector>
#include <algorithm>

// Assume ShaderLang.h is included before ShaderVars.h, for sh::GLenum
// Note: make sure to increment ANGLE_SH_VERSION when changing ShaderVars.h

namespace sh
{

// Varying interpolation qualifier, see section 4.3.9 of the ESSL 3.00.4 spec
enum InterpolationType
{
    INTERPOLATION_SMOOTH,
    INTERPOLATION_CENTROID,
    INTERPOLATION_FLAT
};

// Uniform block layout qualifier, see section 4.3.8.3 of the ESSL 3.00.4 spec
enum BlockLayoutType
{
    BLOCKLAYOUT_STANDARD,
    BLOCKLAYOUT_PACKED,
    BLOCKLAYOUT_SHARED
};

// Base class for all variables defined in shaders, including Varyings, Uniforms, etc
// Note: we must override the copy constructor and assignment operator so we can
// work around excessive GCC binary bloating:
// See https://code.google.com/p/angleproject/issues/detail?id=697
struct COMPILER_EXPORT ShaderVariable
{
    ShaderVariable();
    ShaderVariable(GLenum typeIn, unsigned int arraySizeIn);
    ~ShaderVariable();
    ShaderVariable(const ShaderVariable &other);
    ShaderVariable &operator=(const ShaderVariable &other);

    bool isArray() const { return arraySize > 0; }
    unsigned int elementCount() const { return std::max(1u, arraySize); }
    bool isStruct() const { return !fields.empty(); }

    GLenum type;
    GLenum precision;
    std::string name;
    std::string mappedName;
    unsigned int arraySize;
    bool staticUse;
    std::vector<ShaderVariable> fields;
    std::string structName;
};

struct COMPILER_EXPORT Uniform : public ShaderVariable
{
    Uniform();
    ~Uniform();
    Uniform(const Uniform &other);
    Uniform &operator=(const Uniform &other);
};

struct COMPILER_EXPORT Attribute : public ShaderVariable
{
    Attribute();
    ~Attribute();
    Attribute(const Attribute &other);
    Attribute &operator=(const Attribute &other);

    int location;
};

struct COMPILER_EXPORT InterfaceBlockField : public ShaderVariable
{
    InterfaceBlockField();
    ~InterfaceBlockField();
    InterfaceBlockField(const InterfaceBlockField &other);
    InterfaceBlockField &operator=(const InterfaceBlockField &other);

    bool isRowMajorLayout;
};

struct COMPILER_EXPORT Varying : public ShaderVariable
{
    Varying();
    ~Varying();
    Varying(const Varying &other);
    Varying &operator=(const Varying &other);

    InterpolationType interpolation;
    bool isInvariant;
};

struct COMPILER_EXPORT InterfaceBlock
{
    InterfaceBlock();
    ~InterfaceBlock();
    InterfaceBlock(const InterfaceBlock &other);
    InterfaceBlock &operator=(const InterfaceBlock &other);

    std::string name;
    std::string mappedName;
    std::string instanceName;
    unsigned int arraySize;
    BlockLayoutType layout;
    bool isRowMajorLayout;
    bool staticUse;
    std::vector<InterfaceBlockField> fields;
};

}

#endif // _COMPILER_INTERFACE_VARIABLES_
