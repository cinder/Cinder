/// @ref gtc_quaternion
/// @file glm/gtc/quaternion.inl

#include "../trigonometric.hpp"
#include "../geometric.hpp"
#include "../exponential.hpp"
#include <limits>

namespace glm{
namespace detail
{
	template<typename T, precision P, bool Aligned>
	struct compute_dot<tquat<T, P>, T, Aligned>
	{
		static GLM_FUNC_QUALIFIER T call(tquat<T, P> const& a, tquat<T, P> const& b)
		{
			vec<4, T, P> tmp(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
			return (tmp.x + tmp.y) + (tmp.z + tmp.w);
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_quat_add
	{
		static tquat<T, P> call(tquat<T, P> const& q, tquat<T, P> const& p)
		{
			return tquat<T, P>(q.w + p.w, q.x + p.x, q.y + p.y, q.z + p.z);
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_quat_sub
	{
		static tquat<T, P> call(tquat<T, P> const& q, tquat<T, P> const& p)
		{
			return tquat<T, P>(q.w - p.w, q.x - p.x, q.y - p.y, q.z - p.z);
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_quat_mul_scalar
	{
		static tquat<T, P> call(tquat<T, P> const& q, T s)
		{
			return tquat<T, P>(q.w * s, q.x * s, q.y * s, q.z * s);
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_quat_div_scalar
	{
		static tquat<T, P> call(tquat<T, P> const& q, T s)
		{
			return tquat<T, P>(q.w / s, q.x / s, q.y / s, q.z / s);
		}
	};

	template<typename T, precision P, bool Aligned>
	struct compute_quat_mul_vec4
	{
		static vec<4, T, P> call(tquat<T, P> const & q, vec<4, T, P> const & v)
		{
			return vec<4, T, P>(q * vec<3, T, P>(v), v.w);
		}
	};
}//namespace detail

	// -- Component accesses --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER T & tquat<T, P>::operator[](typename tquat<T, P>::length_type i)
	{
		assert(i >= 0 && i < this->length());
		return (&x)[i];
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER T const & tquat<T, P>::operator[](typename tquat<T, P>::length_type i) const
	{
		assert(i >= 0 && i < this->length());
		return (&x)[i];
	}

	// -- Implicit basic constructors --

#	if !GLM_HAS_DEFAULTED_FUNCTIONS || !defined(GLM_FORCE_NO_CTOR_INIT)
		template<typename T, precision P>
		GLM_FUNC_QUALIFIER GLM_CONSTEXPR tquat<T, P>::tquat()
#			ifndef GLM_FORCE_NO_CTOR_INIT
				: x(0), y(0), z(0), w(1)
#			endif
		{}
#	endif

#	if !GLM_HAS_DEFAULTED_FUNCTIONS
		template<typename T, precision P>
		GLM_FUNC_QUALIFIER GLM_CONSTEXPR tquat<T, P>::tquat(tquat<T, P> const & q)
			: x(q.x), y(q.y), z(q.z), w(q.w)
		{}
#	endif//!GLM_HAS_DEFAULTED_FUNCTIONS

	template<typename T, precision P>
	template<precision Q>
	GLM_FUNC_QUALIFIER GLM_CONSTEXPR tquat<T, P>::tquat(tquat<T, Q> const& q)
		: x(q.x), y(q.y), z(q.z), w(q.w)
	{}

	// -- Explicit basic constructors --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER GLM_CONSTEXPR_CTOR tquat<T, P>::tquat(ctor)
	{}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER GLM_CONSTEXPR tquat<T, P>::tquat(T s, vec<3, T, P> const& v)
		: x(v.x), y(v.y), z(v.z), w(s)
	{}

	template <typename T, precision P>
	GLM_FUNC_QUALIFIER GLM_CONSTEXPR tquat<T, P>::tquat(T _w, T _x, T _y, T _z)
		: x(_x), y(_y), z(_z), w(_w)
	{}

	// -- Conversion constructors --

	template<typename T, precision P>
	template<typename U, precision Q>
	GLM_FUNC_QUALIFIER GLM_CONSTEXPR tquat<T, P>::tquat(tquat<U, Q> const& q)
		: x(static_cast<T>(q.x))
		, y(static_cast<T>(q.y))
		, z(static_cast<T>(q.z))
		, w(static_cast<T>(q.w))
	{}

	//template<typename valType> 
	//GLM_FUNC_QUALIFIER tquat<valType>::tquat
	//(
	//	valType const & pitch,
	//	valType const & yaw,
	//	valType const & roll
	//)
	//{
	//	vec<3, valType> eulerAngle(pitch * valType(0.5), yaw * valType(0.5), roll * valType(0.5));
	//	vec<3, valType> c = glm::cos(eulerAngle * valType(0.5));
	//	vec<3, valType> s = glm::sin(eulerAngle * valType(0.5));
	//	
	//	this->w = c.x * c.y * c.z + s.x * s.y * s.z;
	//	this->x = s.x * c.y * c.z - c.x * s.y * s.z;
	//	this->y = c.x * s.y * c.z + s.x * c.y * s.z;
	//	this->z = c.x * c.y * s.z - s.x * s.y * c.z;
	//}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P>::tquat(vec<3, T, P> const& u, vec<3, T, P> const& v)
	{
		vec<3, T, P> const LocalW(cross(u, v));
		T Dot = detail::compute_dot<vec<3, T, P>, T, detail::is_aligned<P>::value>::call(u, v);
		tquat<T, P> q(T(1) + Dot, LocalW.x, LocalW.y, LocalW.z);

		*this = normalize(q);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P>::tquat(vec<3, T, P> const& eulerAngle)
	{
		vec<3, T, P> c = glm::cos(eulerAngle * T(0.5));
		vec<3, T, P> s = glm::sin(eulerAngle * T(0.5));
		
		this->w = c.x * c.y * c.z + s.x * s.y * s.z;
		this->x = s.x * c.y * c.z - c.x * s.y * s.z;
		this->y = c.x * s.y * c.z + s.x * c.y * s.z;
		this->z = c.x * c.y * s.z - s.x * s.y * c.z;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P>::tquat(mat<3, 3, T, P> const& m)
	{
		*this = quat_cast(m);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P>::tquat(mat<4, 4, T, P> const& m)
	{
		*this = quat_cast(m);
	}

#	if GLM_HAS_EXPLICIT_CONVERSION_OPERATORS
	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P>::operator mat<3, 3, T, P>()
	{
		return mat3_cast(*this);
	}
	
	template<typename T, precision P>	
	GLM_FUNC_QUALIFIER tquat<T, P>::operator mat<4, 4, T, P>()
	{
		return mat4_cast(*this);
	}
#	endif//GLM_HAS_EXPLICIT_CONVERSION_OPERATORS

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> conjugate(tquat<T, P> const& q)
	{
		return tquat<T, P>(q.w, -q.x, -q.y, -q.z);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> inverse(tquat<T, P> const & q)
	{
		return conjugate(q) / dot(q, q);
	}

	// -- Unary arithmetic operators --

#	if !GLM_HAS_DEFAULTED_FUNCTIONS
		template<typename T, precision P>
		GLM_FUNC_QUALIFIER tquat<T, P> & tquat<T, P>::operator=(tquat<T, P> const & q)
		{
			this->w = q.w;
			this->x = q.x;
			this->y = q.y;
			this->z = q.z;
			return *this;
		}
#	endif//!GLM_HAS_DEFAULTED_FUNCTIONS

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER tquat<T, P> & tquat<T, P>::operator=(tquat<U, P> const & q)
	{
		this->w = static_cast<T>(q.w);
		this->x = static_cast<T>(q.x);
		this->y = static_cast<T>(q.y);
		this->z = static_cast<T>(q.z);
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER tquat<T, P> & tquat<T, P>::operator+=(tquat<U, P> const& q)
	{
		return (*this = detail::compute_quat_add<T, P, detail::is_aligned<P>::value>::call(*this, tquat<T, P>(q)));
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER tquat<T, P> & tquat<T, P>::operator-=(tquat<U, P> const& q)
	{
		return (*this = detail::compute_quat_sub<T, P, detail::is_aligned<P>::value>::call(*this, tquat<T, P>(q)));
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER tquat<T, P> & tquat<T, P>::operator*=(tquat<U, P> const & r)
	{
		tquat<T, P> const p(*this);
		tquat<T, P> const q(r);

		this->w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
		this->x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
		this->y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
		this->z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER tquat<T, P> & tquat<T, P>::operator*=(U s)
	{
		return (*this = detail::compute_quat_mul_scalar<T, P, detail::is_aligned<P>::value>::call(*this, static_cast<U>(s)));
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER tquat<T, P> & tquat<T, P>::operator/=(U s)
	{
		return (*this = detail::compute_quat_div_scalar<T, P, detail::is_aligned<P>::value>::call(*this, static_cast<U>(s)));
	}

	// -- Unary bit operators --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> operator+(tquat<T, P> const & q)
	{
		return q;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> operator-(tquat<T, P> const & q)
	{
		return tquat<T, P>(-q.w, -q.x, -q.y, -q.z);
	}

	// -- Binary operators --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> operator+(tquat<T, P> const & q,	tquat<T, P> const & p)
	{
		return tquat<T, P>(q) += p;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> operator*(tquat<T, P> const & q,	tquat<T, P> const & p)
	{
		return tquat<T, P>(q) *= p;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<3, T, P> operator*(tquat<T, P> const & q,	vec<3, T, P> const & v)
	{
		vec<3, T, P> const QuatVector(q.x, q.y, q.z);
		vec<3, T, P> const uv(glm::cross(QuatVector, v));
		vec<3, T, P> const uuv(glm::cross(QuatVector, uv));

		return v + ((uv * q.w) + uuv) * static_cast<T>(2);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<3, T, P> operator*(vec<3, T, P> const & v, tquat<T, P> const & q)
	{
		return glm::inverse(q) * v;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<4, T, P> operator*(tquat<T, P> const& q, vec<4, T, P> const& v)
	{
		return detail::compute_quat_mul_vec4<T, P, detail::is_aligned<P>::value>::call(q, v);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<4, T, P> operator*(vec<4, T, P> const & v, tquat<T, P> const & q)
	{
		return glm::inverse(q) * v;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> operator*(tquat<T, P> const & q, T const & s)
	{
		return tquat<T, P>(
			q.w * s, q.x * s, q.y * s, q.z * s);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> operator*(T const & s, tquat<T, P> const & q)
	{
		return q * s;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> operator/(tquat<T, P> const & q, T const & s)
	{
		return tquat<T, P>(
			q.w / s, q.x / s, q.y / s, q.z / s);
	}

	// -- Boolean operators --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER bool operator==(tquat<T, P> const & q1, tquat<T, P> const & q2)
	{
		return (q1.x == q2.x) && (q1.y == q2.y) && (q1.z == q2.z) && (q1.w == q2.w);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER bool operator!=(tquat<T, P> const & q1, tquat<T, P> const & q2)
	{
		return (q1.x != q2.x) || (q1.y != q2.y) || (q1.z != q2.z) || (q1.w != q2.w);
	}

	// -- Operations --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER T length(tquat<T, P> const & q)
	{
		return glm::sqrt(dot(q, q));
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> normalize(tquat<T, P> const & q)
	{
		T len = length(q);
		if(len <= T(0)) // Problem
			return tquat<T, P>(1, 0, 0, 0);
		T oneOverLen = T(1) / len;
		return tquat<T, P>(q.w * oneOverLen, q.x * oneOverLen, q.y * oneOverLen, q.z * oneOverLen);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> cross(tquat<T, P> const & q1, tquat<T, P> const & q2)
	{
		return tquat<T, P>(
			q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
			q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
			q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
			q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x);
	}
/*
	// (x * sin(1 - a) * angle / sin(angle)) + (y * sin(a) * angle / sin(angle))
	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> mix(tquat<T, P> const & x, tquat<T, P> const & y, T const & a)
	{
		if(a <= T(0)) return x;
		if(a >= T(1)) return y;

		float fCos = dot(x, y);
		tquat<T, P> y2(y); //BUG!!! tquat<T, P> y2;
		if(fCos < T(0))
		{
			y2 = -y;
			fCos = -fCos;
		}

		//if(fCos > 1.0f) // problem
		float k0, k1;
		if(fCos > T(0.9999))
		{
			k0 = T(1) - a;
			k1 = T(0) + a; //BUG!!! 1.0f + a;
		}
		else
		{
			T fSin = sqrt(T(1) - fCos * fCos);
			T fAngle = atan(fSin, fCos);
			T fOneOverSin = static_cast<T>(1) / fSin;
			k0 = sin((T(1) - a) * fAngle) * fOneOverSin;
			k1 = sin((T(0) + a) * fAngle) * fOneOverSin;
		}

		return tquat<T, P>(
			k0 * x.w + k1 * y2.w,
			k0 * x.x + k1 * y2.x,
			k0 * x.y + k1 * y2.y,
			k0 * x.z + k1 * y2.z);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> mix2
	(
		tquat<T, P> const & x, 
		tquat<T, P> const & y, 
		T const & a
	)
	{
		bool flip = false;
		if(a <= static_cast<T>(0)) return x;
		if(a >= static_cast<T>(1)) return y;

		T cos_t = dot(x, y);
		if(cos_t < T(0))
		{
			cos_t = -cos_t;
			flip = true;
		}

		T alpha(0), beta(0);

		if(T(1) - cos_t < 1e-7)
			beta = static_cast<T>(1) - alpha;
		else
		{
			T theta = acos(cos_t);
			T sin_t = sin(theta);
			beta = sin(theta * (T(1) - alpha)) / sin_t;
			alpha = sin(alpha * theta) / sin_t;
		}

		if(flip)
			alpha = -alpha;
		
		return normalize(beta * x + alpha * y);
	}
*/

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> mix(tquat<T, P> const & x, tquat<T, P> const & y, T a)
	{
		T cosTheta = dot(x, y);

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if(cosTheta > T(1) - epsilon<T>())
		{
			// Linear interpolation
			return tquat<T, P>(
				mix(x.w, y.w, a),
				mix(x.x, y.x, a),
				mix(x.y, y.y, a),
				mix(x.z, y.z, a));
		}
		else
		{
			// Essential Mathematics, page 467
			T angle = acos(cosTheta);
			return (sin((T(1) - a) * angle) * x + sin(a * angle) * y) / sin(angle);
		}
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> lerp(tquat<T, P> const & x, tquat<T, P> const & y, T a)
	{
		// Lerp is only defined in [0, 1]
		assert(a >= static_cast<T>(0));
		assert(a <= static_cast<T>(1));

		return x * (T(1) - a) + (y * a);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> slerp(tquat<T, P> const & x,	tquat<T, P> const & y, T a)
	{
		tquat<T, P> z = y;

		T cosTheta = dot(x, y);

		// If cosTheta < 0, the interpolation will take the long way around the sphere. 
		// To fix this, one quat must be negated.
		if (cosTheta < T(0))
		{
			z        = -y;
			cosTheta = -cosTheta;
		}

		// Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
		if(cosTheta > T(1) - epsilon<T>())
		{
			// Linear interpolation
			return tquat<T, P>(
				mix(x.w, z.w, a),
				mix(x.x, z.x, a),
				mix(x.y, z.y, a),
				mix(x.z, z.z, a));
		}
		else
		{
			// Essential Mathematics, page 467
			T angle = acos(cosTheta);
			return (sin((T(1) - a) * angle) * x + sin(a * angle) * z) / sin(angle);
		}
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> rotate(tquat<T, P> const & q, T const & angle, vec<3, T, P> const & v)
	{
		vec<3, T, P> Tmp = v;

		// Axis of rotation must be normalised
		T len = glm::length(Tmp);
		if(abs(len - T(1)) > T(0.001))
		{
			T oneOverLen = static_cast<T>(1) / len;
			Tmp.x *= oneOverLen;
			Tmp.y *= oneOverLen;
			Tmp.z *= oneOverLen;
		}

		T const AngleRad(angle);
		T const Sin = sin(AngleRad * T(0.5));

		return q * tquat<T, P>(cos(AngleRad * T(0.5)), Tmp.x * Sin, Tmp.y * Sin, Tmp.z * Sin);
		//return gtc::quaternion::cross(q, tquat<T, P>(cos(AngleRad * T(0.5)), Tmp.x * fSin, Tmp.y * fSin, Tmp.z * fSin));
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<3, T, P> eulerAngles(tquat<T, P> const & x)
	{
		return vec<3, T, P>(pitch(x), yaw(x), roll(x));
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER T roll(tquat<T, P> const & q)
	{
		return T(atan(T(2) * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z));
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER T pitch(tquat<T, P> const & q)
	{
		//return T(atan(T(2) * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z));
		const T y = T(2) * (q.y * q.z + q.w * q.x);
		const T x = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;
        	if(y == T(0) && x == T(0)) //avoid atan2(0,0) - handle singularity - Matiis
            		return T(T(2)*atan(q.x,q.w));
		
		return T(atan(y,x));
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER T yaw(tquat<T, P> const & q)
	{
		return asin(clamp(T(-2) * (q.x * q.z - q.w * q.y), T(-1), T(1)));
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<3, 3, T, P> mat3_cast(tquat<T, P> const & q)
	{
		mat<3, 3, T, P> Result(T(1));
		T qxx(q.x * q.x);
		T qyy(q.y * q.y);
		T qzz(q.z * q.z);
		T qxz(q.x * q.z);
		T qxy(q.x * q.y);
		T qyz(q.y * q.z);
		T qwx(q.w * q.x);
		T qwy(q.w * q.y);
		T qwz(q.w * q.z);

		Result[0][0] = T(1) - T(2) * (qyy +  qzz);
		Result[0][1] = T(2) * (qxy + qwz);
		Result[0][2] = T(2) * (qxz - qwy);

		Result[1][0] = T(2) * (qxy - qwz);
		Result[1][1] = T(1) - T(2) * (qxx +  qzz);
		Result[1][2] = T(2) * (qyz + qwx);

		Result[2][0] = T(2) * (qxz + qwy);
		Result[2][1] = T(2) * (qyz - qwx);
		Result[2][2] = T(1) - T(2) * (qxx +  qyy);
		return Result;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 4, T, P> mat4_cast(tquat<T, P> const & q)
	{
		return mat<4, 4, T, P>(mat3_cast(q));
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> quat_cast(mat<3, 3, T, P> const & m)
	{
		T fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
		T fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
		T fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
		T fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

		int biggestIndex = 0;
		T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
		if(fourXSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourXSquaredMinus1;
			biggestIndex = 1;
		}
		if(fourYSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourYSquaredMinus1;
			biggestIndex = 2;
		}
		if(fourZSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourZSquaredMinus1;
			biggestIndex = 3;
		}

		T biggestVal = sqrt(fourBiggestSquaredMinus1 + T(1)) * T(0.5);
		T mult = static_cast<T>(0.25) / biggestVal;

		tquat<T, P> Result(uninitialize);
		switch(biggestIndex)
		{
		case 0:
			Result.w = biggestVal;
			Result.x = (m[1][2] - m[2][1]) * mult;
			Result.y = (m[2][0] - m[0][2]) * mult;
			Result.z = (m[0][1] - m[1][0]) * mult;
			break;
		case 1:
			Result.w = (m[1][2] - m[2][1]) * mult;
			Result.x = biggestVal;
			Result.y = (m[0][1] + m[1][0]) * mult;
			Result.z = (m[2][0] + m[0][2]) * mult;
			break;
		case 2:
			Result.w = (m[2][0] - m[0][2]) * mult;
			Result.x = (m[0][1] + m[1][0]) * mult;
			Result.y = biggestVal;
			Result.z = (m[1][2] + m[2][1]) * mult;
			break;
		case 3:
			Result.w = (m[0][1] - m[1][0]) * mult;
			Result.x = (m[2][0] + m[0][2]) * mult;
			Result.y = (m[1][2] + m[2][1]) * mult;
			Result.z = biggestVal;
			break;
			
		default:					// Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
			assert(false);
			break;
		}
		return Result;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> quat_cast(mat<4, 4, T, P> const & m4)
	{
		return quat_cast(mat<3, 3, T, P>(m4));
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER T angle(tquat<T, P> const & x)
	{
		return acos(x.w) * T(2);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<3, T, P> axis(tquat<T, P> const & x)
	{
		T tmp1 = static_cast<T>(1) - x.w * x.w;
		if(tmp1 <= static_cast<T>(0))
			return vec<3, T, P>(0, 0, 1);
		T tmp2 = static_cast<T>(1) / sqrt(tmp1);
		return vec<3, T, P>(x.x * tmp2, x.y * tmp2, x.z * tmp2);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER tquat<T, P> angleAxis(T const & angle, vec<3, T, P> const & v)
	{
		tquat<T, P> Result(uninitialize);

		T const a(angle);
		T const s = glm::sin(a * static_cast<T>(0.5));

		Result.w = glm::cos(a * static_cast<T>(0.5));
		Result.x = v.x * s;
		Result.y = v.y * s;
		Result.z = v.z * s;
		return Result;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<4, bool, P> lessThan(tquat<T, P> const & x, tquat<T, P> const & y)
	{
		vec<4, bool, P> Result(uninitialize);
		for(length_t i = 0; i < x.length(); ++i)
			Result[i] = x[i] < y[i];
		return Result;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<4, bool, P> lessThanEqual(tquat<T, P> const & x, tquat<T, P> const & y)
	{
		vec<4, bool, P> Result(uninitialize);
		for(length_t i = 0; i < x.length(); ++i)
			Result[i] = x[i] <= y[i];
		return Result;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<4, bool, P> greaterThan(tquat<T, P> const & x, tquat<T, P> const & y)
	{
		vec<4, bool, P> Result(uninitialize);
		for(length_t i = 0; i < x.length(); ++i)
			Result[i] = x[i] > y[i];
		return Result;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<4, bool, P> greaterThanEqual(tquat<T, P> const & x, tquat<T, P> const & y)
	{
		vec<4, bool, P> Result(uninitialize);
		for(length_t i = 0; i < x.length(); ++i)
			Result[i] = x[i] >= y[i];
		return Result;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<4, bool, P> equal(tquat<T, P> const & x, tquat<T, P> const & y)
	{
		vec<4, bool, P> Result(uninitialize);
		for(length_t i = 0; i < x.length(); ++i)
			Result[i] = x[i] == y[i];
		return Result;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<4, bool, P> notEqual(tquat<T, P> const & x, tquat<T, P> const & y)
	{
		vec<4, bool, P> Result(uninitialize);
		for(length_t i = 0; i < x.length(); ++i)
			Result[i] = x[i] != y[i];
		return Result;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<4, bool, P> isnan(tquat<T, P> const& q)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'isnan' only accept floating-point inputs");

		return vec<4, bool, P>(isnan(q.x), isnan(q.y), isnan(q.z), isnan(q.w));
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER vec<4, bool, P> isinf(tquat<T, P> const& q)
	{
		GLM_STATIC_ASSERT(std::numeric_limits<T>::is_iec559, "'isinf' only accept floating-point inputs");

		return vec<4, bool, P>(isinf(q.x), isinf(q.y), isinf(q.z), isinf(q.w));
	}
}//namespace glm

#if GLM_ARCH != GLM_ARCH_PURE && GLM_HAS_ALIGNED_TYPE
#	include "quaternion_simd.inl"
#endif

