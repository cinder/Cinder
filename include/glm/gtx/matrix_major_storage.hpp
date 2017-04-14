/// @ref gtx_matrix_major_storage
/// @file glm/gtx/matrix_major_storage.hpp
///
/// @see core (dependence)
/// @see gtx_extented_min_max (dependence)
///
/// @defgroup gtx_matrix_major_storage GLM_GTX_matrix_major_storage
/// @ingroup gtx
///
/// @brief Build matrices with specific matrix order, row or column
///
/// <glm/gtx/matrix_major_storage.hpp> need to be included to use these functionalities.

#pragma once

// Dependency:
#include "../glm.hpp"

#ifndef GLM_ENABLE_EXPERIMENTAL
#	error "GLM: GLM_GTX_matrix_major_storage is an experimental extension and may change in the future. Use #define GLM_ENABLE_EXPERIMENTAL before including it, if you really want to use it."
#endif

#if GLM_MESSAGES == GLM_MESSAGES_ENABLED && !defined(GLM_EXT_INCLUDED)
#	pragma message("GLM: GLM_GTX_matrix_major_storage extension included")
#endif

namespace glm
{
	/// @addtogroup gtx_matrix_major_storage
	/// @{

	//! Build a row major matrix from row vectors.
	//! From GLM_GTX_matrix_major_storage extension.
	template<typename T, precision P>
	GLM_FUNC_DECL mat<2, 2, T, P> rowMajor2(
		vec<2, T, P> const & v1, 
		vec<2, T, P> const & v2);
		
	//! Build a row major matrix from other matrix.
	//! From GLM_GTX_matrix_major_storage extension.
	template<typename T, precision P>
	GLM_FUNC_DECL mat<2, 2, T, P> rowMajor2(
		mat<2, 2, T, P> const& m);

	//! Build a row major matrix from row vectors.
	//! From GLM_GTX_matrix_major_storage extension.
	template<typename T, precision P>
	GLM_FUNC_DECL mat<3, 3, T, P> rowMajor3(
		vec<3, T, P> const & v1, 
		vec<3, T, P> const & v2, 
		vec<3, T, P> const & v3);

	//! Build a row major matrix from other matrix.
	//! From GLM_GTX_matrix_major_storage extension.
	template<typename T, precision P>
	GLM_FUNC_DECL mat<3, 3, T, P> rowMajor3(
		mat<3, 3, T, P> const& m);

	//! Build a row major matrix from row vectors.
	//! From GLM_GTX_matrix_major_storage extension.
	template<typename T, precision P>
	GLM_FUNC_DECL mat<4, 4, T, P> rowMajor4(
		vec<4, T, P> const & v1, 
		vec<4, T, P> const & v2,
		vec<4, T, P> const & v3, 
		vec<4, T, P> const & v4);

	//! Build a row major matrix from other matrix.
	//! From GLM_GTX_matrix_major_storage extension.
	template<typename T, precision P>
	GLM_FUNC_DECL mat<4, 4, T, P> rowMajor4(
		mat<4, 4, T, P> const& m);

	//! Build a column major matrix from column vectors.
	//! From GLM_GTX_matrix_major_storage extension.
	template<typename T, precision P>
	GLM_FUNC_DECL mat<2, 2, T, P> colMajor2(
		vec<2, T, P> const & v1, 
		vec<2, T, P> const & v2);
		
	//! Build a column major matrix from other matrix.
	//! From GLM_GTX_matrix_major_storage extension.
	template<typename T, precision P>
	GLM_FUNC_DECL mat<2, 2, T, P> colMajor2(
		mat<2, 2, T, P> const& m);

	//! Build a column major matrix from column vectors.
	//! From GLM_GTX_matrix_major_storage extension.
	template<typename T, precision P>
	GLM_FUNC_DECL mat<3, 3, T, P> colMajor3(
		vec<3, T, P> const & v1, 
		vec<3, T, P> const & v2, 
		vec<3, T, P> const & v3);
		
	//! Build a column major matrix from other matrix.
	//! From GLM_GTX_matrix_major_storage extension.
	template<typename T, precision P>
	GLM_FUNC_DECL mat<3, 3, T, P> colMajor3(
		mat<3, 3, T, P> const& m);
		
	//! Build a column major matrix from column vectors.
	//! From GLM_GTX_matrix_major_storage extension.
	template<typename T, precision P>
	GLM_FUNC_DECL mat<4, 4, T, P> colMajor4(
		vec<4, T, P> const & v1, 
		vec<4, T, P> const & v2, 
		vec<4, T, P> const & v3, 
		vec<4, T, P> const & v4);
				
	//! Build a column major matrix from other matrix.
	//! From GLM_GTX_matrix_major_storage extension.
	template<typename T, precision P> 
	GLM_FUNC_DECL mat<4, 4, T, P> colMajor4(
		mat<4, 4, T, P> const& m);

	/// @}
}//namespace glm

#include "matrix_major_storage.inl"
