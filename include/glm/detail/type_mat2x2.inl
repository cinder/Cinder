/// @ref core
/// @file glm/detail/type_mat2x2.inl

#include "func_matrix.hpp"

namespace glm
{
	// -- Constructors --

#	if !GLM_HAS_DEFAULTED_FUNCTIONS || !defined(GLM_FORCE_NO_CTOR_INIT)
		template<typename T, precision P>
		GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat()
		{
#			ifndef GLM_FORCE_NO_CTOR_INIT 
				this->value[0] = col_type(1, 0);
				this->value[1] = col_type(0, 1);
#			endif
		}
#	endif

#	if !GLM_HAS_DEFAULTED_FUNCTIONS
		template<typename T, precision P>
		GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(mat<2, 2, T, P> const& m)
		{
			this->value[0] = m.value[0];
			this->value[1] = m.value[1];
		}
#	endif//!GLM_HAS_DEFAULTED_FUNCTIONS

	template<typename T, precision P>
	template<precision Q>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(mat<2, 2, T, Q> const& m)
	{
		this->value[0] = m.value[0];
		this->value[1] = m.value[1];
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER GLM_CONSTEXPR_CTOR mat<2, 2, T, P>::mat(ctor)
	{}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(T scalar)
	{
		this->value[0] = col_type(scalar, 0);
		this->value[1] = col_type(0, scalar);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat 
	(
		T const & x0, T const & y0,
		T const & x1, T const & y1
	)
	{
		this->value[0] = col_type(x0, y0);
		this->value[1] = col_type(x1, y1);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(col_type const& v0, col_type const& v1)
	{
		this->value[0] = v0;
		this->value[1] = v1;
	}

	// -- Conversion constructors --

	template<typename T, precision P>
	template<typename X1, typename Y1, typename X2, typename Y2>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat 
	(
		X1 const & x1, Y1 const & y1,
		X2 const & x2, Y2 const & y2
	)
	{
		this->value[0] = col_type(static_cast<T>(x1), value_type(y1));
		this->value[1] = col_type(static_cast<T>(x2), value_type(y2));
	}
	
	template<typename T, precision P>
	template<typename V1, typename V2>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(vec<2, V1, P> const& v1, vec<2, V2, P> const& v2)
	{
		this->value[0] = col_type(v1);
		this->value[1] = col_type(v2);
	}

	// -- mat2x2 matrix conversions --

	template<typename T, precision P>
	template<typename U, precision Q>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(mat<2, 2, U, Q> const& m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(mat<3, 3, T, P> const& m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(mat<4, 4, T, P> const& m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
	}

	template<typename T, precision P> 
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(mat<2, 3, T, P> const& m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(mat<3, 2, T, P> const& m)
	{
		this->value[0] = m[0];
		this->value[1] = m[1];
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(mat<2, 4, T, P> const& m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(mat<4, 2, T, P> const& m)
	{
		this->value[0] = m[0];
		this->value[1] = m[1];
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(mat<3, 4, T, P> const& m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>::mat(mat<4, 3, T, P> const& m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
	}

	// -- Accesses --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER typename mat<2, 2, T, P>::col_type& mat<2, 2, T, P>::operator[](typename mat<2, 2, T, P>::length_type i)
	{
		assert(i < this->length());
		return this->value[i];
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER typename mat<2, 2, T, P>::col_type const& mat<2, 2, T, P>::operator[](typename mat<2, 2, T, P>::length_type i) const
	{
		assert(i < this->length());
		return this->value[i];
	}

	// -- Unary updatable operators --

#	if !GLM_HAS_DEFAULTED_FUNCTIONS
		template<typename T, precision P>
		GLM_FUNC_QUALIFIER mat<2, 2, T, P>& mat<2, 2, T, P>::operator=(mat<2, 2, T, P> const & m)
		{
			this->value[0] = m[0];
			this->value[1] = m[1];
			return *this;
		}
#	endif//!GLM_HAS_DEFAULTED_FUNCTIONS

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>& mat<2, 2, T, P>::operator=(mat<2, 2, U, P> const & m)
	{
		this->value[0] = m[0];
		this->value[1] = m[1];
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>& mat<2, 2, T, P>::operator+=(U scalar)
	{
		this->value[0] += scalar;
		this->value[1] += scalar;
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>& mat<2, 2, T, P>::operator+=(mat<2, 2, U, P> const & m)
	{
		this->value[0] += m[0];
		this->value[1] += m[1];
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>& mat<2, 2, T, P>::operator-=(U scalar)
	{
		this->value[0] -= scalar;
		this->value[1] -= scalar;
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>& mat<2, 2, T, P>::operator-=(mat<2, 2, U, P> const & m)
	{
		this->value[0] -= m[0];
		this->value[1] -= m[1];
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>& mat<2, 2, T, P>::operator*=(U scalar)
	{
		this->value[0] *= scalar;
		this->value[1] *= scalar;
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>& mat<2, 2, T, P>::operator*=(mat<2, 2, U, P> const & m)
	{
		return (*this = *this * m);
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>& mat<2, 2, T, P>::operator/=(U scalar)
	{
		this->value[0] /= scalar;
		this->value[1] /= scalar;
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>& mat<2, 2, T, P>::operator/=(mat<2, 2, U, P> const & m)
	{
		return *this *= inverse(m);
	}

	// -- Increment and decrement operators --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>& mat<2, 2, T, P>::operator++()
	{
		++this->value[0];
		++this->value[1];
		return *this;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P>& mat<2, 2, T, P>::operator--()
	{
		--this->value[0];
		--this->value[1];
		return *this;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> mat<2, 2, T, P>::operator++(int)
	{
		mat<2, 2, T, P> Result(*this);
		++*this;
		return Result;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> mat<2, 2, T, P>::operator--(int)
	{
		mat<2, 2, T, P> Result(*this);
		--*this;
		return Result;
	}

	// -- Unary arithmetic operators --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator+(mat<2, 2, T, P> const & m)
	{
		return m;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator-(mat<2, 2, T, P> const & m)
	{
		return mat<2, 2, T, P>(
			-m[0], 
			-m[1]);
	}

	// -- Binary arithmetic operators --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator+(mat<2, 2, T, P> const & m, T scalar)
	{
		return mat<2, 2, T, P>(
			m[0] + scalar,
			m[1] + scalar);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator+(T scalar, mat<2, 2, T, P> const & m)
	{
		return mat<2, 2, T, P>(
			m[0] + scalar,
			m[1] + scalar);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator+(mat<2, 2, T, P> const & m1, mat<2, 2, T, P> const & m2)
	{
		return mat<2, 2, T, P>(
			m1[0] + m2[0],
			m1[1] + m2[1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator-(mat<2, 2, T, P> const & m, T scalar)
	{
		return mat<2, 2, T, P>(
			m[0] - scalar,
			m[1] - scalar);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator-(T scalar, mat<2, 2, T, P> const & m)
	{
		return mat<2, 2, T, P>(
			scalar - m[0],
			scalar - m[1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator-(mat<2, 2, T, P> const & m1, mat<2, 2, T, P> const & m2)
	{
		return mat<2, 2, T, P>(
			m1[0] - m2[0],
			m1[1] - m2[1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator*(mat<2, 2, T, P> const & m, T scalar)
	{
		return mat<2, 2, T, P>(
			m[0] * scalar,
			m[1] * scalar);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator*(T scalar, mat<2, 2, T, P> const & m)
	{
		return mat<2, 2, T, P>(
			m[0] * scalar,
			m[1] * scalar);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER typename mat<2, 2, T, P>::col_type operator*
	(
		mat<2, 2, T, P> const& m,
		typename mat<2, 2, T, P>::row_type const & v
	)
	{
		return vec<2, T, P>(
			m[0][0] * v.x + m[1][0] * v.y,
			m[0][1] * v.x + m[1][1] * v.y);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER typename mat<2, 2, T, P>::row_type operator*
	(
		typename mat<2, 2, T, P>::col_type const & v,
		mat<2, 2, T, P> const& m
	)
	{
		return vec<2, T, P>(
			v.x * m[0][0] + v.y * m[0][1],
			v.x * m[1][0] + v.y * m[1][1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator*(mat<2, 2, T, P> const & m1, mat<2, 2, T, P> const & m2)
	{
		return mat<2, 2, T, P>(
			m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
			m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
			m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
			m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<3, 2, T, P> operator*(mat<2, 2, T, P> const & m1, mat<3, 2, T, P> const & m2)
	{
		return mat<3, 2, T, P>(
			m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
			m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
			m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
			m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
			m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
			m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> operator*(mat<2, 2, T, P> const & m1, mat<4, 2, T, P> const & m2)
	{
		return mat<4, 2, T, P>(
			m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
			m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
			m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
			m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1],
			m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1],
			m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1],
			m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1],
			m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1]);
	}

	template<typename T, precision P> 
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator/(mat<2, 2, T, P> const & m, T scalar)
	{
		return mat<2, 2, T, P>(
			m[0] / scalar,
			m[1] / scalar);
	}

	template<typename T, precision P> 
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator/(T scalar, mat<2, 2, T, P> const & m)
	{
		return mat<2, 2, T, P>(
			scalar / m[0],
			scalar / m[1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER typename mat<2, 2, T, P>::col_type operator/(mat<2, 2, T, P> const & m, typename mat<2, 2, T, P>::row_type const & v)
	{
		return inverse(m) * v;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER typename mat<2, 2, T, P>::row_type operator/(typename mat<2, 2, T, P>::col_type const & v, mat<2, 2, T, P> const & m)
	{
		return v *  inverse(m);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator/(mat<2, 2, T, P> const & m1, mat<2, 2, T, P> const & m2)
	{	
		mat<2, 2, T, P> m1_copy(m1);
		return m1_copy /= m2;
	}

	// -- Boolean operators --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER bool operator==(mat<2, 2, T, P> const & m1, mat<2, 2, T, P> const & m2)
	{
		return (m1[0] == m2[0]) && (m1[1] == m2[1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER bool operator!=(mat<2, 2, T, P> const & m1, mat<2, 2, T, P> const & m2)
	{
		return (m1[0] != m2[0]) || (m1[1] != m2[1]);
	}
} //namespace glm
