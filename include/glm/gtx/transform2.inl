/// @ref gtx_transform2
/// @file glm/gtx/transform2.inl

namespace glm
{
	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<3, 3, T, P> shearX2D(mat<3, 3, T, P> const& m, T s)
	{
		mat<3, 3, T, P> r(1);
		r[1][0] = s;
		return m * r;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<3, 3, T, P> shearY2D(mat<3, 3, T, P> const& m, T s)
	{
		mat<3, 3, T, P> r(1);
		r[0][1] = s;
		return m * r;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 4, T, P> shearX3D(mat<4, 4, T, P> const& m, T s, T t)
	{
		mat<4, 4, T, P> r(1);
		r[0][1] = s;
		r[0][2] = t;
		return m * r;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 4, T, P> shearY3D(mat<4, 4, T, P> const& m, T s, T t)
	{
		mat<4, 4, T, P> r(1);
		r[1][0] = s;
		r[1][2] = t;
		return m * r;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 4, T, P> shearZ3D(mat<4, 4, T, P> const& m, T s, T t)
	{
		mat<4, 4, T, P> r(1);
		r[2][0] = s;
		r[2][1] = t;
		return m * r;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<3, 3, T, P> reflect2D(mat<3, 3, T, P> const& m, vec<3, T, P> const& normal)
	{
		mat<3, 3, T, P> r(static_cast<T>(1));
		r[0][0] = static_cast<T>(1) - static_cast<T>(2) * normal.x * normal.x;
		r[0][1] = -static_cast<T>(2) * normal.x * normal.y;
		r[1][0] = -static_cast<T>(2) * normal.x * normal.y;
		r[1][1] = static_cast<T>(1) - static_cast<T>(2) * normal.y * normal.y;
		return m * r;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 4, T, P> reflect3D(mat<4, 4, T, P> const& m, vec<3, T, P> const& normal)
	{
		mat<4, 4, T, P> r(static_cast<T>(1));
		r[0][0] = static_cast<T>(1) - static_cast<T>(2) * normal.x * normal.x;
		r[0][1] = -static_cast<T>(2) * normal.x * normal.y;
		r[0][2] = -static_cast<T>(2) * normal.x * normal.z;

		r[1][0] = -static_cast<T>(2) * normal.x * normal.y;
		r[1][1] = static_cast<T>(1) - static_cast<T>(2) * normal.y * normal.y;
		r[1][2] = -static_cast<T>(2) * normal.y * normal.z;

		r[2][0] = -static_cast<T>(2) * normal.x * normal.z;
		r[2][1] = -static_cast<T>(2) * normal.y * normal.z;
		r[2][2] = static_cast<T>(1) - static_cast<T>(2) * normal.z * normal.z;
		return m * r;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<3, 3, T, P> proj2D(
		const mat<3, 3, T, P>& m, 
		const vec<3, T, P>& normal)
	{
		mat<3, 3, T, P> r(static_cast<T>(1));
		r[0][0] = static_cast<T>(1) - normal.x * normal.x;
		r[0][1] = - normal.x * normal.y;
		r[1][0] = - normal.x * normal.y;
		r[1][1] = static_cast<T>(1) - normal.y * normal.y;
		return m * r;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 4, T, P> proj3D(
		const mat<4, 4, T, P>& m, 
		const vec<3, T, P>& normal)
	{
		mat<4, 4, T, P> r(static_cast<T>(1));
		r[0][0] = static_cast<T>(1) - normal.x * normal.x;
		r[0][1] = - normal.x * normal.y;
		r[0][2] = - normal.x * normal.z;
		r[1][0] = - normal.x * normal.y;
		r[1][1] = static_cast<T>(1) - normal.y * normal.y;
		r[1][2] = - normal.y * normal.z;
		r[2][0] = - normal.x * normal.z;
		r[2][1] = - normal.y * normal.z;
		r[2][2] = static_cast<T>(1) - normal.z * normal.z;
		return m * r;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 4, T, P> scaleBias(T scale, T bias)
	{
		mat<4, 4, T, P> result;
		result[3] = vec<4, T, P>(vec<3, T, P>(bias), static_cast<T>(1));
		result[0][0] = scale;
		result[1][1] = scale;
		result[2][2] = scale;
		return result;
	}

	template<typename T, precision P> 
	GLM_FUNC_QUALIFIER mat<4, 4, T, P> scaleBias(mat<4, 4, T, P> const& m, T scale, T bias)
	{
		return m * scaleBias(scale, bias);
	}
}//namespace glm

