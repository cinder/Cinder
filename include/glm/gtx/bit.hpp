/// @ref gtx_bit
/// @file glm/gtx/bit.hpp
///
/// @see core (dependence)
///
/// @defgroup gtx_bit GLM_GTX_bit
/// @ingroup gtx
/// 
/// @brief Allow to perform bit operations on integer values
/// 
/// <glm/gtx/bit.hpp> need to be included to use these functionalities.

#pragma once

// Dependencies
#include "../gtc/bitfield.hpp"

#ifndef GLM_ENABLE_EXPERIMENTAL
#	error "GLM: GLM_GTX_bit is an experimental extension and may change in the future. Use #define GLM_ENABLE_EXPERIMENTAL before including it, if you really want to use it."
#endif

#if GLM_MESSAGES == GLM_MESSAGES_ENABLED && !defined(GLM_EXT_INCLUDED)
#	pragma message("GLM: GLM_GTX_bit extension is deprecated, include GLM_GTC_bitfield and GLM_GTC_integer instead")
#endif

namespace glm
{
	/// @addtogroup gtx_bit
	/// @{

	/// @see gtx_bit
	template<typename genIUType>
	GLM_FUNC_DECL genIUType highestBitValue(genIUType Value);

	/// @see gtx_bit
	template<typename genIUType>
	GLM_FUNC_DECL genIUType lowestBitValue(genIUType Value);

	/// Find the highest bit set to 1 in a integer variable and return its value.
	///
	/// @see gtx_bit
	template<length_t L, typename T, precision P, template<length_t, typename, precision> class vecType>
	GLM_FUNC_DECL vecType<L, T, P> highestBitValue(vecType<L, T, P> const & value);

	/// Return the power of two number which value is just higher the input value.
	/// Deprecated, use ceilPowerOfTwo from GTC_round instead
	///
	/// @see gtc_round
	/// @see gtx_bit
	template<typename genIUType>
	GLM_DEPRECATED GLM_FUNC_DECL genIUType powerOfTwoAbove(genIUType Value);

	/// Return the power of two number which value is just higher the input value.
	/// Deprecated, use ceilPowerOfTwo from GTC_round instead
	///
	/// @see gtc_round
	/// @see gtx_bit
	template<length_t L, typename T, precision P, template<length_t, typename, precision> class vecType>
	GLM_DEPRECATED GLM_FUNC_DECL vecType<L, T, P> powerOfTwoAbove(vecType<L, T, P> const & value);

	/// Return the power of two number which value is just lower the input value.
	/// Deprecated, use floorPowerOfTwo from GTC_round instead
	///
	/// @see gtc_round
	/// @see gtx_bit
	template<typename genIUType>
	GLM_DEPRECATED GLM_FUNC_DECL genIUType powerOfTwoBelow(genIUType Value);

	/// Return the power of two number which value is just lower the input value.
	/// Deprecated, use floorPowerOfTwo from GTC_round instead
	///
	/// @see gtc_round
	/// @see gtx_bit
	template<length_t L, typename T, precision P, template<length_t, typename, precision> class vecType>
	GLM_DEPRECATED GLM_FUNC_DECL vecType<L, T, P> powerOfTwoBelow(vecType<L, T, P> const & value);

	/// Return the power of two number which value is the closet to the input value.
	/// Deprecated, use roundPowerOfTwo from GTC_round instead
	///
	/// @see gtc_round
	/// @see gtx_bit
	template<typename genIUType>
	GLM_DEPRECATED GLM_FUNC_DECL genIUType powerOfTwoNearest(genIUType Value);

	/// Return the power of two number which value is the closet to the input value.
	/// Deprecated, use roundPowerOfTwo from GTC_round instead
	///
	/// @see gtc_round
	/// @see gtx_bit
	template<length_t L, typename T, precision P, template<length_t, typename, precision> class vecType>
	GLM_DEPRECATED GLM_FUNC_DECL vecType<L, T, P> powerOfTwoNearest(vecType<L, T, P> const & value);

	/// @}
} //namespace glm


#include "bit.inl"

