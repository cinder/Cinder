/// @ref gtx_matrix_transform_2d
/// @file glm/gtx/matrix_transform_2d.hpp
/// @author Miguel Ángel Pérez Martínez
///
/// @see core (dependence)
///
/// @defgroup gtx_matrix_transform_2d GLM_GTX_matrix_transform_2d
/// @ingroup gtx
///
/// @brief Defines functions that generate common 2d transformation matrices.
///
/// <glm/gtx/matrix_transform_2d.hpp> need to be included to use these functionalities.

#pragma once

// Dependency:
#include "../mat3x3.hpp"
#include "../vec2.hpp"

#ifndef GLM_ENABLE_EXPERIMENTAL
#	error "GLM: GLM_GTX_matrix_transform_2d is an experimental extension and may change in the future. Use #define GLM_ENABLE_EXPERIMENTAL before including it, if you really want to use it."
#endif

#if GLM_MESSAGES == GLM_MESSAGES_ENABLED && !defined(GLM_EXT_INCLUDED)
#	pragma message("GLM: GLM_GTX_matrix_transform_2d extension included")
#endif

namespace glm
{
	/// @addtogroup gtx_matrix_transform_2d
	/// @{
	
	/// Builds a translation 3 * 3 matrix created from a vector of 2 components.
	///
	/// @param m Input matrix multiplied by this translation matrix.
	/// @param v Coordinates of a translation vector.		
	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<3, 3, T, P> translate(
		mat<3, 3, T, P> const& m,
		vec<2, T, P> const & v);

	/// Builds a rotation 3 * 3 matrix created from an angle. 
	///
	/// @param m Input matrix multiplied by this translation matrix.
	/// @param angle Rotation angle expressed in radians if GLM_FORCE_RADIANS is defined or degrees otherwise.
	template<typename T, precision P> 
	GLM_FUNC_QUALIFIER mat<3, 3, T, P> rotate(
		mat<3, 3, T, P> const& m,
		T angle);

	/// Builds a scale 3 * 3 matrix created from a vector of 2 components.
	///
	/// @param m Input matrix multiplied by this translation matrix.
	/// @param v Coordinates of a scale vector.		
	template<typename T, precision P> 
	GLM_FUNC_QUALIFIER mat<3, 3, T, P> scale(
		mat<3, 3, T, P> const& m,
		vec<2, T, P> const & v);

	/// Builds an horizontal (parallel to the x axis) shear 3 * 3 matrix. 
	///
	/// @param m Input matrix multiplied by this translation matrix.
	/// @param y Shear factor.
	template<typename T, precision P> 
	GLM_FUNC_QUALIFIER mat<3, 3, T, P> shearX(
		mat<3, 3, T, P> const& m,
		T y);

	/// Builds a vertical (parallel to the y axis) shear 3 * 3 matrix. 
	///
	/// @param m Input matrix multiplied by this translation matrix.
	/// @param x Shear factor.
	template<typename T, precision P> 
	GLM_FUNC_QUALIFIER mat<3, 3, T, P> shearY(
		mat<3, 3, T, P> const& m,
		T x);

	/// @}
}//namespace glm

#include "matrix_transform_2d.inl"
