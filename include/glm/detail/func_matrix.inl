/// @ref core
/// @file glm/detail/func_matrix.inl

#include "../geometric.hpp"
#include <limits>

namespace glm{
namespace detail
{
	template<template<length_t, length_t, typename, precision> class matType, length_t C, length_t R, typename T, precision P, bool Aligned>
	struct compute_matrixCompMult
	{
		GLM_FUNC_QUALIFIER static matType<C, R, T, P> call(matType<C, R, T, P> const& x, matType<C, R, T, P> const& y)
		{
			matType<C, R, T, P> result(uninitialize);
			for(length_t i = 0; i < result.length(); ++i)
				result[i] = x[i] * y[i];
			return result;
		}
	};

	template<template<length_t, length_t, typename, precision> class matType, length_t C, length_t R, typename T, precision P, bool Aligned>
	struct compute_transpose{};

	template<typename T, precision P, bool Aligned>
	struct compute_transpose<mat, 2, 2, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static mat<2, 2, T, P> call(mat<2, 2, T, P> const& m)
		{
			mat<2, 2, T, P> result(uninitialize);
			result[0][0] = m[0][0];
			result[0][1] = m[1][0];
			result[1][0] = m[0][1];
			result[1][1] = m[1][1];
			return result;
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_transpose<mat, 2, 3, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static mat<3, 2, T, P> call(mat<2, 3, T, P> const& m)
		{
			mat<3,2, T, P> result(uninitialize);
			result[0][0] = m[0][0];
			result[0][1] = m[1][0];
			result[1][0] = m[0][1];
			result[1][1] = m[1][1];
			result[2][0] = m[0][2];
			result[2][1] = m[1][2];
			return result;
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_transpose<mat, 2, 4, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static mat<4, 2, T, P> call(mat<2, 4, T, P> const& m)
		{
			mat<4, 2, T, P> result(uninitialize);
			result[0][0] = m[0][0];
			result[0][1] = m[1][0];
			result[1][0] = m[0][1];
			result[1][1] = m[1][1];
			result[2][0] = m[0][2];
			result[2][1] = m[1][2];
			result[3][0] = m[0][3];
			result[3][1] = m[1][3];
			return result;
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_transpose<mat, 3, 2, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static mat<2, 3, T, P> call(mat<3, 2, T, P> const& m)
		{
			mat<2, 3, T, P> result(uninitialize);
			result[0][0] = m[0][0];
			result[0][1] = m[1][0];
			result[0][2] = m[2][0];
			result[1][0] = m[0][1];
			result[1][1] = m[1][1];
			result[1][2] = m[2][1];
			return result;
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_transpose<mat, 3, 3, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static mat<3, 3, T, P> call(mat<3, 3, T, P> const& m)
		{
			mat<3, 3, T, P> result(uninitialize);
			result[0][0] = m[0][0];
			result[0][1] = m[1][0];
			result[0][2] = m[2][0];

			result[1][0] = m[0][1];
			result[1][1] = m[1][1];
			result[1][2] = m[2][1];

			result[2][0] = m[0][2];
			result[2][1] = m[1][2];
			result[2][2] = m[2][2];
			return result;
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_transpose<mat, 3, 4, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static mat<4, 3, T, P> call(mat<3, 4, T, P> const& m)
		{
			mat<4, 3, T, P> result(uninitialize);
			result[0][0] = m[0][0];
			result[0][1] = m[1][0];
			result[0][2] = m[2][0];
			result[1][0] = m[0][1];
			result[1][1] = m[1][1];
			result[1][2] = m[2][1];
			result[2][0] = m[0][2];
			result[2][1] = m[1][2];
			result[2][2] = m[2][2];
			result[3][0] = m[0][3];
			result[3][1] = m[1][3];
			result[3][2] = m[2][3];
			return result;
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_transpose<mat, 4, 2, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static mat<2, 4, T, P> call(mat<4, 2, T, P> const& m)
		{
			mat<2, 4, T, P> result(uninitialize);
			result[0][0] = m[0][0];
			result[0][1] = m[1][0];
			result[0][2] = m[2][0];
			result[0][3] = m[3][0];
			result[1][0] = m[0][1];
			result[1][1] = m[1][1];
			result[1][2] = m[2][1];
			result[1][3] = m[3][1];
			return result;
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_transpose<mat, 4, 3, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static mat<3, 4, T, P> call(mat<4, 3, T, P> const& m)
		{
			mat<3, 4, T, P> result(uninitialize);
			result[0][0] = m[0][0];
			result[0][1] = m[1][0];
			result[0][2] = m[2][0];
			result[0][3] = m[3][0];
			result[1][0] = m[0][1];
			result[1][1] = m[1][1];
			result[1][2] = m[2][1];
			result[1][3] = m[3][1];
			result[2][0] = m[0][2];
			result[2][1] = m[1][2];
			result[2][2] = m[2][2];
			result[2][3] = m[3][2];
			return result;
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_transpose<mat, 4, 4, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static mat<4, 4, T, P> call(mat<4, 4, T, P> const& m)
		{
			mat<4, 4, T, P> result(uninitialize);
			result[0][0] = m[0][0];
			result[0][1] = m[1][0];
			result[0][2] = m[2][0];
			result[0][3] = m[3][0];

			result[1][0] = m[0][1];
			result[1][1] = m[1][1];
			result[1][2] = m[2][1];
			result[1][3] = m[3][1];

			result[2][0] = m[0][2];
			result[2][1] = m[1][2];
			result[2][2] = m[2][2];
			result[2][3] = m[3][2];

			result[3][0] = m[0][3];
			result[3][1] = m[1][3];
			result[3][2] = m[2][3];
			result[3][3] = m[3][3];
			return result;
		}
	};

	template<template<length_t, length_t, typename, precision> class matType, length_t C, length_t R, typename T, precision P, bool Aligned>
	struct compute_determinant{};

	template<typename T, precision P, bool Aligned>
	struct compute_determinant<mat, 2, 2, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static T call(mat<2, 2, T, P> const& m)
		{
			return m[0][0] * m[1][1] - m[1][0] * m[0][1];
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_determinant<mat, 3, 3, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static T call(mat<3, 3, T, P> const& m)
		{
			return
				+ m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
				- m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
				+ m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_determinant<mat, 4, 4, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static T call(mat<4, 4, T, P> const& m)
		{
			T SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
			T SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
			T SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
			T SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
			T SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
			T SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

			vec<4, T, P> DetCof(
				+ (m[1][1] * SubFactor00 - m[1][2] * SubFactor01 + m[1][3] * SubFactor02),
				- (m[1][0] * SubFactor00 - m[1][2] * SubFactor03 + m[1][3] * SubFactor04),
				+ (m[1][0] * SubFactor01 - m[1][1] * SubFactor03 + m[1][3] * SubFactor05),
				- (m[1][0] * SubFactor02 - m[1][1] * SubFactor04 + m[1][2] * SubFactor05));

			return
				m[0][0] * DetCof[0] + m[0][1] * DetCof[1] +
				m[0][2] * DetCof[2] + m[0][3] * DetCof[3];
		}
	};

	template<template<length_t, length_t, typename, precision> class matType, length_t C, length_t R, typename T, precision P, bool Aligned>
	struct compute_inverse{};

	template<typename T, precision P, bool Aligned>
	struct compute_inverse<mat, 2, 2, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static mat<2, 2, T, P> call(mat<2, 2, T, P> const& m)
		{
			T OneOverDeterminant = static_cast<T>(1) / (
				+ m[0][0] * m[1][1]
				- m[1][0] * m[0][1]);

			mat<2, 2, T, P> Inverse(
				+ m[1][1] * OneOverDeterminant,
				- m[0][1] * OneOverDeterminant,
				- m[1][0] * OneOverDeterminant,
				+ m[0][0] * OneOverDeterminant);

			return Inverse;
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_inverse<mat, 3, 3, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static mat<3, 3, T, P> call(mat<3, 3, T, P> const& m)
		{
			T OneOverDeterminant = static_cast<T>(1) / (
				+ m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
				- m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
				+ m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]));

			mat<3, 3, T, P> Inverse(uninitialize);
			Inverse[0][0] = + (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * OneOverDeterminant;
			Inverse[1][0] = - (m[1][0] * m[2][2] - m[2][0] * m[1][2]) * OneOverDeterminant;
			Inverse[2][0] = + (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * OneOverDeterminant;
			Inverse[0][1] = - (m[0][1] * m[2][2] - m[2][1] * m[0][2]) * OneOverDeterminant;
			Inverse[1][1] = + (m[0][0] * m[2][2] - m[2][0] * m[0][2]) * OneOverDeterminant;
			Inverse[2][1] = - (m[0][0] * m[2][1] - m[2][0] * m[0][1]) * OneOverDeterminant;
			Inverse[0][2] = + (m[0][1] * m[1][2] - m[1][1] * m[0][2]) * OneOverDeterminant;
			Inverse[1][2] = - (m[0][0] * m[1][2] - m[1][0] * m[0][2]) * OneOverDeterminant;
			Inverse[2][2] = + (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * OneOverDeterminant;

			return Inverse;
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_inverse<mat, 4, 4, T, P, Aligned>
	{
		GLM_FUNC_QUALIFIER static mat<4, 4, T, P> call(mat<4, 4, T, P> const& m)
		{
			T Coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
			T Coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
			T Coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

			T Coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
			T Coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
			T Coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

			T Coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
			T Coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
			T Coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

			T Coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
			T Coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
			T Coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

			T Coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
			T Coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
			T Coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

			T Coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
			T Coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
			T Coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

			vec<4, T, P> Fac0(Coef00, Coef00, Coef02, Coef03);
			vec<4, T, P> Fac1(Coef04, Coef04, Coef06, Coef07);
			vec<4, T, P> Fac2(Coef08, Coef08, Coef10, Coef11);
			vec<4, T, P> Fac3(Coef12, Coef12, Coef14, Coef15);
			vec<4, T, P> Fac4(Coef16, Coef16, Coef18, Coef19);
			vec<4, T, P> Fac5(Coef20, Coef20, Coef22, Coef23);

			vec<4, T, P> Vec0(m[1][0], m[0][0], m[0][0], m[0][0]);
			vec<4, T, P> Vec1(m[1][1], m[0][1], m[0][1], m[0][1]);
			vec<4, T, P> Vec2(m[1][2], m[0][2], m[0][2], m[0][2]);
			vec<4, T, P> Vec3(m[1][3], m[0][3], m[0][3], m[0][3]);

			vec<4, T, P> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
			vec<4, T, P> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
			vec<4, T, P> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
			vec<4, T, P> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

			vec<4, T, P> SignA(+1, -1, +1, -1);
			vec<4, T, P> SignB(-1, +1, -1, +1);
			mat<4, 4, T, P> Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

			vec<4, T, P> Row0(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

			vec<4, T, P> Dot0(m[0] * Row0);
			T Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

			T OneOverDeterminant = static_cast<T>(1) / Dot1;

			return Inverse * OneOverDeterminant;
		}
	};
}//namespace detail

	template<length_t C, length_t R, typename T, precision P, template<length_t, length_t, typename, precision> class matType>
	GLM_FUNC_QUALIFIER matType<C, R, T, P> matrixCompMult(matType<C, R, T, P> const & x, matType<C, R, T, P> const & y)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559 || GLM_UNRESTRICTED_GENTYPE, "'matrixCompMult' only accept floating-point inputs");
		return detail::compute_matrixCompMult<matType, C, R, T, P, detail::is_aligned<P>::value>::call(x, y);
	}

	template<int DA, int DB, typename T, precision P, template<length_t, typename, precision> class vecTypeA, template<length_t, typename, precision> class vecTypeB>
	GLM_FUNC_QUALIFIER typename detail::outerProduct_trait<DA, DB, T, P, vecTypeA, vecTypeB>::type outerProduct(vecTypeA<DA, T, P> const & c, vecTypeB<DB, T, P> const & r)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559 || GLM_UNRESTRICTED_GENTYPE, "'outerProduct' only accept floating-point inputs");

		typename detail::outerProduct_trait<DA, DB, T, P, vecTypeA, vecTypeB>::type m(uninitialize);
		for(length_t i = 0; i < m.length(); ++i)
			m[i] = c * r[i];
		return m;
	}

	template<length_t C, length_t R, typename T, precision P, template<length_t, length_t, typename, precision> class matType>
	GLM_FUNC_QUALIFIER typename matType<C, R, T, P>::transpose_type transpose(matType<C, R, T, P> const & m)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559 || GLM_UNRESTRICTED_GENTYPE, "'transpose' only accept floating-point inputs");
		return detail::compute_transpose<matType, C, R, T, P, detail::is_aligned<P>::value>::call(m);
	}

	template<length_t C, length_t R, typename T, precision P, template<length_t, length_t, typename, precision> class matType>
	GLM_FUNC_QUALIFIER T determinant(matType<C, R, T, P> const & m)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559 || GLM_UNRESTRICTED_GENTYPE, "'determinant' only accept floating-point inputs");
		return detail::compute_determinant<matType, C, R, T, P, detail::is_aligned<P>::value>::call(m);
	}

	template<length_t C, length_t R, typename T, precision P, template<length_t, length_t, typename, precision> class matType>
	GLM_FUNC_QUALIFIER matType<C, R, T, P> inverse(matType<C, R, T, P> const & m)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559 || GLM_UNRESTRICTED_GENTYPE, "'inverse' only accept floating-point inputs");
		return detail::compute_inverse<matType, C, R, T, P, detail::is_aligned<P>::value>::call(m);
	}
}//namespace glm

#if GLM_ARCH != GLM_ARCH_PURE && GLM_HAS_UNRESTRICTED_UNIONS
#	include "func_matrix_simd.inl"
#endif

