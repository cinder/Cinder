/// @ref core
/// @file glm/detail/type_mat4x2.inl

namespace glm
{
	// -- Constructors --

#	if !GLM_HAS_DEFAULTED_FUNCTIONS || !defined(GLM_FORCE_NO_CTOR_INIT)
		template<typename T, precision P> 
		GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat()
		{
#			ifndef GLM_FORCE_NO_CTOR_INIT 
				this->value[0] = col_type(1, 0);
				this->value[1] = col_type(0, 1);
				this->value[2] = col_type(0, 0);
				this->value[3] = col_type(0, 0);
#			endif
		}
#	endif

#	if !GLM_HAS_DEFAULTED_FUNCTIONS
		template<typename T, precision P>
		GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat(mat<4, 2, T, P> const & m)
		{
			this->value[0] = m.value[0];
			this->value[1] = m.value[1];
			this->value[2] = m.value[2];
			this->value[3] = m.value[3];
		}
#	endif//!GLM_HAS_DEFAULTED_FUNCTIONS

	template<typename T, precision P>
	template<precision Q>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat(mat<4, 2, T, Q> const & m)
	{
		this->value[0] = m.value[0];
		this->value[1] = m.value[1];
		this->value[2] = m.value[2];
		this->value[3] = m.value[3];
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER GLM_CONSTEXPR_CTOR mat<4, 2, T, P>::mat(ctor)
	{}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat(T scalar)
	{
		this->value[0] = col_type(scalar, 0);
		this->value[1] = col_type(0, scalar);
		this->value[2] = col_type(0, 0);
		this->value[3] = col_type(0, 0);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat
	(
		T x0, T y0,
		T x1, T y1,
		T x2, T y2,
		T x3, T y3
	)
	{
		this->value[0] = col_type(x0, y0);
		this->value[1] = col_type(x1, y1);
		this->value[2] = col_type(x2, y2);
		this->value[3] = col_type(x3, y3);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat
	(
		col_type const & v0,
		col_type const & v1,
		col_type const & v2,
		col_type const & v3
	)
	{
		this->value[0] = v0;
		this->value[1] = v1;
		this->value[2] = v2;
		this->value[3] = v3;
	}

	// -- Conversion constructors --

	template<typename T, precision P>
	template<
		typename X1, typename Y1,
		typename X2, typename Y2,
		typename X3, typename Y3,
		typename X4, typename Y4>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat
	(
		X1 x1, Y1 y1,
		X2 x2, Y2 y2,
		X3 x3, Y3 y3,
		X4 x4, Y4 y4
	)
	{
		this->value[0] = col_type(static_cast<T>(x1), value_type(y1));
		this->value[1] = col_type(static_cast<T>(x2), value_type(y2));
		this->value[2] = col_type(static_cast<T>(x3), value_type(y3));
		this->value[3] = col_type(static_cast<T>(x4), value_type(y4));
	}
	
	template<typename T, precision P>
	template<typename V1, typename V2, typename V3, typename V4>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat
	(
		vec<2, V1, P> const & v1,
		vec<2, V2, P> const & v2,
		vec<2, V3, P> const & v3,
		vec<2, V4, P> const & v4
	)
	{
		this->value[0] = col_type(v1);
		this->value[1] = col_type(v2);
		this->value[2] = col_type(v3);
		this->value[3] = col_type(v4);
	}

	// -- Conversion --

	template<typename T, precision P>
	template<typename U, precision Q>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat(mat<4, 2, U, Q> const & m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
		this->value[2] = col_type(m[2]);
		this->value[3] = col_type(m[3]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat(mat<2, 2, T, P> const & m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
		this->value[2] = col_type(0);
		this->value[3] = col_type(0);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat(mat<3, 3, T, P> const & m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
		this->value[2] = col_type(m[2]);
		this->value[3] = col_type(0);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat(mat<4, 4, T, P> const & m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
		this->value[2] = col_type(m[2]);
		this->value[3] = col_type(m[3]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat(mat<2, 3, T, P> const & m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
		this->value[2] = col_type(0);
		this->value[3] = col_type(0);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat(mat<3, 2, T, P> const & m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
		this->value[2] = col_type(m[2]);
		this->value[3] = col_type(0);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat(mat<2, 4, T, P> const & m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
		this->value[2] = col_type(0);
		this->value[3] = col_type(0);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat(mat<4, 3, T, P> const & m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
		this->value[2] = col_type(m[2]);
		this->value[3] = col_type(m[3]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>::mat(mat<3, 4, T, P> const & m)
	{
		this->value[0] = col_type(m[0]);
		this->value[1] = col_type(m[1]);
		this->value[2] = col_type(m[2]);
		this->value[3] = col_type(0);
	}

	// -- Accesses --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER typename mat<4, 2, T, P>::col_type & mat<4, 2, T, P>::operator[](typename mat<4, 2, T, P>::length_type i)
	{
		assert(i < this->length());
		return this->value[i];
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER typename mat<4, 2, T, P>::col_type const & mat<4, 2, T, P>::operator[](typename mat<4, 2, T, P>::length_type i) const
	{
		assert(i < this->length());
		return this->value[i];
	}

	// -- Unary updatable operators --

#	if !GLM_HAS_DEFAULTED_FUNCTIONS
		template<typename T, precision P>
		GLM_FUNC_QUALIFIER mat<4, 2, T, P>& mat<4, 2, T, P>::operator=(mat<4, 2, T, P> const & m)
		{
			this->value[0] = m[0];
			this->value[1] = m[1];
			this->value[2] = m[2];
			this->value[3] = m[3];
			return *this;
		}
#	endif//!GLM_HAS_DEFAULTED_FUNCTIONS

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P>& mat<4, 2, T, P>::operator=(mat<4, 2, U, P> const & m)
	{
		this->value[0] = m[0];
		this->value[1] = m[1];
		this->value[2] = m[2];
		this->value[3] = m[3];
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> & mat<4, 2, T, P>::operator+=(U s)
	{
		this->value[0] += s;
		this->value[1] += s;
		this->value[2] += s;
		this->value[3] += s;
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> & mat<4, 2, T, P>::operator+=(mat<4, 2, U, P> const & m)
	{
		this->value[0] += m[0];
		this->value[1] += m[1];
		this->value[2] += m[2];
		this->value[3] += m[3];
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> & mat<4, 2, T, P>::operator-=(U s)
	{
		this->value[0] -= s;
		this->value[1] -= s;
		this->value[2] -= s;
		this->value[3] -= s;
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> & mat<4, 2, T, P>::operator-=(mat<4, 2, U, P> const & m)
	{
		this->value[0] -= m[0];
		this->value[1] -= m[1];
		this->value[2] -= m[2];
		this->value[3] -= m[3];
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> & mat<4, 2, T, P>::operator*=(U s)
	{
		this->value[0] *= s;
		this->value[1] *= s;
		this->value[2] *= s;
		this->value[3] *= s;
		return *this;
	}

	template<typename T, precision P>
	template<typename U>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> & mat<4, 2, T, P>::operator/=(U s)
	{
		this->value[0] /= s;
		this->value[1] /= s;
		this->value[2] /= s;
		this->value[3] /= s;
		return *this;
	}

	// -- Increment and decrement operators --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> & mat<4, 2, T, P>::operator++()
	{
		++this->value[0];
		++this->value[1];
		++this->value[2];
		++this->value[3];
		return *this;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> & mat<4, 2, T, P>::operator--()
	{
		--this->value[0];
		--this->value[1];
		--this->value[2];
		--this->value[3];
		return *this;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> mat<4, 2, T, P>::operator++(int)
	{
		mat<4, 2, T, P> Result(*this);
		++*this;
		return Result;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> mat<4, 2, T, P>::operator--(int)
	{
		mat<4, 2, T, P> Result(*this);
		--*this;
		return Result;
	}

	// -- Unary arithmetic operators --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> operator+(mat<4, 2, T, P> const & m)
	{
		return m;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> operator-(mat<4, 2, T, P> const & m)
	{
		return mat<4, 2, T, P>(
			-m[0],
			-m[1],
			-m[2],
			-m[3]);
	}

	// -- Binary arithmetic operators --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> operator+(mat<4, 2, T, P> const & m, T scalar)
	{
		return mat<4, 2, T, P>(
			m[0] + scalar,
			m[1] + scalar,
			m[2] + scalar,
			m[3] + scalar);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> operator+(mat<4, 2, T, P> const & m1, mat<4, 2, T, P> const & m2)
	{
		return mat<4, 2, T, P>(
			m1[0] + m2[0],
			m1[1] + m2[1],
			m1[2] + m2[2],
			m1[3] + m2[3]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> operator-(mat<4, 2, T, P> const & m, T scalar)
	{
		return mat<4, 2, T, P>(
			m[0] - scalar,
			m[1] - scalar,
			m[2] - scalar,
			m[3] - scalar);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> operator-(mat<4, 2, T, P> const & m1, mat<4, 2, T, P> const & m2)
	{
		return mat<4, 2, T, P>(
			m1[0] - m2[0],
			m1[1] - m2[1],
			m1[2] - m2[2],
			m1[3] - m2[3]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> operator*(mat<4, 2, T, P> const & m, T scalar)
	{
		return mat<4, 2, T, P>(
			m[0] * scalar,
			m[1] * scalar,
			m[2] * scalar,
			m[3] * scalar);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> operator*(T scalar, mat<4, 2, T, P> const & m)
	{
		return mat<4, 2, T, P>(
			m[0] * scalar,
			m[1] * scalar,
			m[2] * scalar,
			m[3] * scalar);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER typename mat<4, 2, T, P>::col_type operator*(mat<4, 2, T, P> const & m, typename mat<4, 2, T, P>::row_type const & v)
	{
		return typename mat<4, 2, T, P>::col_type(
			m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
			m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER typename mat<4, 2, T, P>::row_type operator*(typename mat<4, 2, T, P>::col_type const & v, mat<4, 2, T, P> const & m)
	{
		return typename mat<4, 2, T, P>::row_type(
			v.x * m[0][0] + v.y * m[0][1],
			v.x * m[1][0] + v.y * m[1][1],
			v.x * m[2][0] + v.y * m[2][1],
			v.x * m[3][0] + v.y * m[3][1]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<2, 2, T, P> operator*(mat<4, 2, T, P> const & m1, mat<2, 4, T, P> const & m2)
	{
		T const SrcA00 = m1[0][0];
		T const SrcA01 = m1[0][1];
		T const SrcA10 = m1[1][0];
		T const SrcA11 = m1[1][1];
		T const SrcA20 = m1[2][0];
		T const SrcA21 = m1[2][1];
		T const SrcA30 = m1[3][0];
		T const SrcA31 = m1[3][1];

		T const SrcB00 = m2[0][0];
		T const SrcB01 = m2[0][1];
		T const SrcB02 = m2[0][2];
		T const SrcB03 = m2[0][3];
		T const SrcB10 = m2[1][0];
		T const SrcB11 = m2[1][1];
		T const SrcB12 = m2[1][2];
		T const SrcB13 = m2[1][3];

		mat<2, 2, T, P> Result(uninitialize);
		Result[0][0] = SrcA00 * SrcB00 + SrcA10 * SrcB01 + SrcA20 * SrcB02 + SrcA30 * SrcB03;
		Result[0][1] = SrcA01 * SrcB00 + SrcA11 * SrcB01 + SrcA21 * SrcB02 + SrcA31 * SrcB03;
		Result[1][0] = SrcA00 * SrcB10 + SrcA10 * SrcB11 + SrcA20 * SrcB12 + SrcA30 * SrcB13;
		Result[1][1] = SrcA01 * SrcB10 + SrcA11 * SrcB11 + SrcA21 * SrcB12 + SrcA31 * SrcB13;
		return Result;
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<3, 2, T, P> operator*(mat<4, 2, T, P> const & m1, mat<3, 4, T, P> const & m2)
	{
		return mat<3, 2, T, P>(
			m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
			m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
			m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
			m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
			m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
			m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> operator*(mat<4, 2, T, P> const & m1, mat<4, 4, T, P> const & m2)
	{
		return mat<4, 2, T, P>(
			m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3],
			m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3],
			m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3],
			m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3],
			m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3],
			m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3],
			m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3],
			m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> operator/(mat<4, 2, T, P> const & m, T scalar)
	{
		return mat<4, 2, T, P>(
			m[0] / scalar,
			m[1] / scalar,
			m[2] / scalar,
			m[3] / scalar);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER mat<4, 2, T, P> operator/(T scalar, mat<4, 2, T, P> const & m)
	{
		return mat<4, 2, T, P>(
			scalar / m[0],
			scalar / m[1],
			scalar / m[2],
			scalar / m[3]);
	}

	// -- Boolean operators --

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER bool operator==(mat<4, 2, T, P> const & m1, mat<4, 2, T, P> const & m2)
	{
		return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]) && (m1[3] == m2[3]);
	}

	template<typename T, precision P>
	GLM_FUNC_QUALIFIER bool operator!=(mat<4, 2, T, P> const & m1, mat<4, 2, T, P> const & m2)
	{
		return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]) || (m1[3] != m2[3]);
	}
} //namespace glm
