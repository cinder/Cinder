/*
 Copyright (c) 2022, The Cinder Project
 All rights reserved.
 
 This code is designed for use with the Cinder C++ library, http://libcinder.org

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

#include "cinder/Cinder.h"
#include "cinder/Filesystem.h"
#include "cinder/Rect.h"
#include "cinder/Color.h"
#include "cinder/Matrix.h"
#include "cinder/DataSource.h"
#include "glm/gtc/type_ptr.hpp"
#include <nlohmann/json.hpp>

namespace cinder {
	//! A wrapper around JSON for Modern C++: https://github.com/nlohmann/json
	using Json = nlohmann::json;
}

namespace nlohmann {

//! Serializer for tvec2
template <typename T, glm::precision P>
struct adl_serializer<glm::tvec2<T,P>> {
	static void to_json( json& j, const glm::tvec2<T,P> &v ) { const T* ptr = glm::value_ptr( v ); j = std::vector<T>( ptr, ptr + 2 ); }
	static void from_json( const json &j, glm::tvec2<T,P> &v ) { std::vector<T> vec = j; v = glm::make_vec2( vec.data() ); }
};

//! Serializer for tvec3
template <typename T, glm::precision P>
struct adl_serializer<glm::tvec3<T,P>> {
	static void to_json( json& j, const glm::tvec3<T,P> &v ) { const T* ptr = glm::value_ptr( v ); j = std::vector<T>( ptr, ptr + 3 ); }
	static void from_json( const json &j, glm::tvec3<T,P> &v ) { std::vector<T> vec = j; v = glm::make_vec3( vec.data() ); }
};

//! Serializer for tvec4
template <typename T, glm::precision P>
struct adl_serializer<glm::tvec4<T,P>> {
	static void to_json( json& j, const glm::tvec4<T,P> &v ) { const T* ptr = glm::value_ptr( v ); j = std::vector<T>( ptr, ptr + 4 ); }
	static void from_json( const json &j, glm::tvec4<T,P> &v ) { std::vector<T> vec = j; v = glm::make_vec4( vec.data() ); }
};

//! Serializer for tquat
template <typename T, glm::precision P>
struct adl_serializer<glm::tquat<T,P>> {
	static void to_json( json& j, const glm::tquat<T,P> &v ) { const T* ptr = glm::value_ptr( v ); j = std::vector<T>( ptr, ptr + 4 ); }
	static void from_json( const json &j, glm::tquat<T,P> &v ) { std::vector<T> vec = j; v = glm::make_quat( vec.data() ); }
};

//! Serializer for tmat3x3
template <typename T, glm::precision P>
struct adl_serializer<glm::tmat3x3<T,P>> {
	static void to_json( json& j, const glm::tmat3x3<T,P> &m ) { j = std::vector<T>( &m[0][0], &m[0][0] + 3 * 3 ); }
	static void from_json( const json &j, glm::tmat3x3<T,P> &m ) { std::vector<T> vec = j; m = glm::make_mat3x3( vec.data() ); }
};

//! Serializer for tmat4x4
template <typename T, glm::precision P>
struct adl_serializer<glm::tmat4x4<T,P>> {
	static void to_json( json& j, const glm::tmat4x4<T,P> &m ) { j = std::vector<T>( &m[0][0], &m[0][0] + 4 * 4 ); }
	static void from_json( const json &j, glm::tmat4x4<T,P> &m ) { std::vector<T> vec = j; m = glm::make_mat4x4( vec.data() ); }
};

//! Serializer for RectT
template <typename T>
struct adl_serializer<ci::RectT<T>> {
	static void to_json( json& j, const ci::RectT<T> &r ) { j = glm::tvec4<T>( r.getUpperLeft(), r.getLowerRight() ); }
	static void from_json( const json &j, ci::RectT<T> &r ) { glm::tvec4<T> v = j; r = ci::RectT<T>( v.x, v.y, v.z, v.w ); }
};

//! Serializer for Area
template <>
struct adl_serializer<ci::Area> {
	static void to_json( json& j, const ci::Area &a ) { j = glm::ivec4( a.getUL(), a.getLR() );	}
	static void from_json( const json &j, ci::Area &a ) { glm::ivec4 v = j; a = ci::Area( v.x, v.y, v.z, v.w ); }
};

template <typename T>
struct adl_serializer<ci::ColorT<T>> {
	static void to_json( json& j, const ci::ColorT<T>& c ) { j = { c.r, c.g, c.b, }; }
	static void from_json( const json& j, ci::ColorT<T>& c ) { std::vector<T> vec = j; c = ci::ColorT<T>( vec[0], vec[1], vec[2] ); }
};

template <typename T>
struct adl_serializer<ci::ColorAT<T>> {
	static void to_json( json& j, const ci::ColorAT<T>& c ) { j = { c.r, c.g, c.b, c.a }; }
	static void from_json( const json& j, ci::ColorAT<T>& c ) { std::vector<T> vec = j; c = ci::ColorAT<T>( vec[0], vec[1], vec[2], vec[3] ); }
};

}

namespace cinder {

Json loadJson( const cinder::fs::path &path, bool stripComments = false );
Json loadJson( const DataSourceRef &dataSource, bool stripComments = false );
//! Convenience method to write a Json instance \a json to a text file at \a path
void writeJson( const cinder::fs::path &path, const Json &json, int indent = 4 );

} // namespace cinder