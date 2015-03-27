///////////////////////////////////////////////////////////////////////////////////
/// OpenGL Mathematics (glm.g-truc.net)
///
/// Copyright (c) 2005 - 2015 G-Truc Creation (www.g-truc.net)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// Restrictions:
///		By making use of the Software for military purposes, you choose to make
///		a Bunny unhappy.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///
/// @ref gtx_transform2
/// @file glm/gtx/transform2.inl
/// @date 2005-12-21 / 2011-06-07
/// @author Christophe Riccio
///////////////////////////////////////////////////////////////////////////////////

namespace glm
{
	template <typename T, precision P>
	GLM_FUNC_DECL tmat3x3<T, P> translate2D(
		tvec2<T, P> const & v)
	{
		return translate2D(
			tmat3x3<T, P>(1), v);
	}

	template <typename T, precision P>
	GLM_FUNC_DECL tmat3x3<T, P> translate2D(
		tmat3x3<T, P> const & m,
		tvec2<T, P> const & v)
	{
		tmat3x3<T, P> Result(m);
		Result[2] = m[0] * v[0] + m[1] * v[1] + m[2];
		return Result;
	}

	template <typename T, precision P>
	GLM_FUNC_DECL tmat3x3<T, P> scale2D(
		tvec2<T, P> const & v)
	{
		return scale2D(
			tmat3x3<T, P>(1), v);
	}

	template <typename T, precision P>
	GLM_FUNC_DECL tmat3x3<T, P> scale2D(
		tmat3x3<T, P> const & m,
		tvec2<T, P> const & v)
	{
		tmat3x3<T, P> Result(uninitialize);
		Result[0] = m[0] * v[0];
		Result[1] = m[1] * v[1];
		Result[2] = m[2];
		return Result;
	}

	template <typename T, precision P>
	GLM_FUNC_DECL tmat3x3<T, P> rotate2D(
		T angle)
	{
		return rotate2D(
			tmat3x3<T, P>(1), angle);
	}
	
	template <typename T, precision P>
	GLM_FUNC_DECL tmat3x3<T, P> rotate2D(
		tmat3x3<T, P> const & m,
		T angle)
	{
		T const c = cos(angle);
		T const s = sin(angle);

		tmat3x3<T, P> Result(uninitialize);
		Result[0][0] = m[0][0] * c + m[1][0] * s;
		Result[0][1] = m[0][1] * c + m[1][1] * s;
		Result[0][2] = m[0][2] * c + m[1][2] * s;

		Result[1][0] = m[0][0] * -s + m[1][0] * c;
		Result[1][1] = m[0][1] * -s + m[1][1] * c;
		Result[1][2] = m[0][2] * -s + m[1][2] * c;

		Result[2][0] = m[2][0];
		Result[2][1] = m[2][1];
		Result[2][2] = m[2][2];

		return Result;
	}

	template <typename T, precision P>
	GLM_FUNC_QUALIFIER tmat3x3<T, P> shearX2D(
		const tmat3x3<T, P>& m, 
		T s)
	{
		tmat3x3<T, P> r(1);
		r[0][1] = s;
		return m * r;
	}

	template <typename T, precision P>
	GLM_FUNC_QUALIFIER tmat3x3<T, P> shearY2D(
		const tmat3x3<T, P>& m, 
		T s)
	{
		tmat3x3<T, P> r(1);
		r[1][0] = s;
		return m * r;
	}

	template <typename T, precision P>
	GLM_FUNC_QUALIFIER tmat4x4<T, P> shearX3D(
		const tmat4x4<T, P>& m, 
		T s, 
		T t)
	{
		tmat4x4<T, P> r(1);
		r[1][0] = s;
		r[2][0] = t;
		return m * r;
	}

	template <typename T, precision P>
	GLM_FUNC_QUALIFIER tmat4x4<T, P> shearY3D(
		const tmat4x4<T, P>& m, 
		T s, 
		T t)
	{
		tmat4x4<T, P> r(1);
		r[0][1] = s;
		r[2][1] = t;
		return m * r;
	}

	template <typename T, precision P>
	GLM_FUNC_QUALIFIER tmat4x4<T, P> shearZ3D(
		const tmat4x4<T, P>& m, 
		T s, 
		T t)
	{
		tmat4x4<T, P> r(1);
		r[0][2] = s;
		r[1][2] = t;
		return m * r;
	}

	template <typename T, precision P>
	GLM_FUNC_QUALIFIER tmat3x3<T, P> reflect2D(
		const tmat3x3<T, P>& m, 
		const tvec3<T, P>& normal)
	{
		tmat3x3<T, P> r(1);
		r[0][0] = 1 - 2 * normal.x * normal.x;
		r[0][1] = -2 * normal.x * normal.y;
		r[1][0] = -2 * normal.x * normal.y;
		r[1][1] = 1 - 2 * normal.y * normal.y;
		return m * r;
	}

	template <typename T, precision P>
	GLM_FUNC_QUALIFIER tmat4x4<T, P> reflect3D(
		const tmat4x4<T, P>& m, 
		const tvec3<T, P>& normal)
	{
		tmat4x4<T, P> r(1);
		r[0][0] = 1 - 2 * normal.x * normal.x;
		r[0][1] = -2 * normal.x * normal.y;
		r[0][2] = -2 * normal.x * normal.z;

		r[1][0] = -2 * normal.x * normal.y;
		r[1][1] = 1 - 2 * normal.y * normal.y;
		r[1][2] = -2 * normal.y * normal.z;

		r[2][0] = -2 * normal.x * normal.z;
		r[2][1] = -2 * normal.y * normal.z;
		r[2][2] = 1 - 2 * normal.z * normal.z;
		return m * r;
	}

	template <typename T, precision P>
	GLM_FUNC_QUALIFIER tmat3x3<T, P> proj2D(
		const tmat3x3<T, P>& m, 
		const tvec3<T, P>& normal)
	{
		tmat3x3<T, P> r(1);
		r[0][0] = 1 - normal.x * normal.x;
		r[0][1] = - normal.x * normal.y;
		r[1][0] = - normal.x * normal.y;
		r[1][1] = 1 - normal.y * normal.y;
		return m * r;
	}

	template <typename T, precision P>
	GLM_FUNC_QUALIFIER tmat4x4<T, P> proj3D(
		const tmat4x4<T, P>& m, 
		const tvec3<T, P>& normal)
	{
		tmat4x4<T, P> r(1);
		r[0][0] = 1 - normal.x * normal.x;
		r[0][1] = - normal.x * normal.y;
		r[0][2] = - normal.x * normal.z;
		r[1][0] = - normal.x * normal.y;
		r[1][1] = 1 - normal.y * normal.y;
		r[1][2] = - normal.y * normal.z;
		r[2][0] = - normal.x * normal.z;
		r[2][1] = - normal.y * normal.z;
		r[2][2] = 1 - normal.z * normal.z;
		return m * r;
	}

	template <typename T, precision P>
	GLM_FUNC_QUALIFIER tmat4x4<T, P> scaleBias(
		T scale, 
		T bias)
	{
		tmat4x4<T, P> result;
		result[3] = tvec4<T, P>(tvec3<T, P>(bias), T(1));
		result[0][0] = scale;
		result[1][1] = scale;
		result[2][2] = scale;
		return result;
	}

	template <typename T, precision P> 
	GLM_FUNC_QUALIFIER tmat4x4<T, P> scaleBias(
		const tmat4x4<T, P>& m, 
		T scale, 
		T bias)
	{
		return m * scaleBias(scale, bias);
	}
}//namespace glm

