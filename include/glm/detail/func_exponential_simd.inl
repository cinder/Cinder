/// @ref core
/// @file glm/detail/func_exponential_simd.inl

#include "../simd/exponential.h"

#if GLM_ARCH & GLM_ARCH_SSE2_BIT

namespace glm{
namespace detail
{
	template<precision P>
	struct compute_sqrt<4, float, P, true>
	{
		GLM_FUNC_QUALIFIER static vec<4, float, P> call(vec<4, float, P> const & v)
		{
			vec<4, float, P> result(uninitialize);
			result.data = _mm_sqrt_ps(v.data);
			return result;
		}
	};

	template<>
	struct compute_sqrt<4, float, aligned_lowp, true>
	{
		GLM_FUNC_QUALIFIER static vec<4, float, aligned_lowp> call(vec<4, float, aligned_lowp> const & v)
		{
			vec<4, float, aligned_lowp> result(uninitialize);
			result.data = glm_vec4_sqrt_lowp(v.data);
			return result;
		}
	};
}//namespace detail
}//namespace glm

#endif//GLM_ARCH & GLM_ARCH_SSE2_BIT
