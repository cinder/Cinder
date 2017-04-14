/// @ref gtx_transform
/// @file glm/gtx/transform.hpp
///
/// @see core (dependence)
/// @see gtc_matrix_transform (dependence)
/// @see gtx_transform
/// @see gtx_transform2
///
/// @defgroup gtx_transform GLM_GTX_transform
/// @ingroup gtx
///
/// @brief Add transformation matrices
///
/// <glm/gtx/transform.hpp> need to be included to use these functionalities.

#pragma once

// Dependency:
#include "../glm.hpp"
#include "../gtc/matrix_transform.hpp"

#ifndef GLM_ENABLE_EXPERIMENTAL
#	error "GLM: GLM_GTX_transform is an experimental extension and may change in the future. Use #define GLM_ENABLE_EXPERIMENTAL before including it, if you really want to use it."
#endif

#if GLM_MESSAGES == GLM_MESSAGES_ENABLED && !defined(GLM_EXT_INCLUDED)
#	pragma message("GLM: GLM_GTX_transform extension included")
#endif

namespace glm
{
	/// @addtogroup gtx_transform
	/// @{

	/// Transforms a matrix with a translation 4 * 4 matrix created from 3 scalars.
	/// @see gtc_matrix_transform
	/// @see gtx_transform
	template<typename T, precision P>
	GLM_FUNC_DECL mat<4, 4, T, P> translate(
		vec<3, T, P> const & v);

	/// Builds a rotation 4 * 4 matrix created from an axis of 3 scalars and an angle expressed in radians. 
	/// @see gtc_matrix_transform
	/// @see gtx_transform
	template<typename T, precision P>
	GLM_FUNC_DECL mat<4, 4, T, P> rotate(
		T angle, 
		vec<3, T, P> const & v);

	/// Transforms a matrix with a scale 4 * 4 matrix created from a vector of 3 components.
	/// @see gtc_matrix_transform
	/// @see gtx_transform
	template<typename T, precision P>
	GLM_FUNC_DECL mat<4, 4, T, P> scale(
		vec<3, T, P> const & v);

	/// @}
}// namespace glm

#include "transform.inl"
