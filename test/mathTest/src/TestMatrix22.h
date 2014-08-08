#pragma once

template <typename T> void TestMatrix22( std::ostream& os )
{
	typedef Matrix22<T> MatT;

	// Matrix22();
	{
		bool result = false;
		MatT c0( 1, 0, 0, 1 );
		result = ( c0 == MatT() );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22();" << "\n";
	}


	// explicit Matrix22( T s );
	{
		bool result = false;
		MatT c0( 3, 3, 3, 3 );
		result = ( c0 == MatT( 3 ) );
		os << (result ? "passed" : "FAILED") << " : " << "explicit Matrix22( T s );" << "\n";
	}


	// Matrix22( const T *dt );
	{
		bool result = false;
		T data[4] = { 1, 2, 3, 4 };
		MatT c0( 1, 2, 3, 4 );
		result = ( c0 == MatT( data ) );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22( const T *dt );" << "\n";
	}


	// Matrix22( T d0, T d1, T d2, T d3 );
	{
		bool result = false;
		T data[4] = { 1, 2, 3, 4 };
		MatT c0( 1, 2, 3, 4 );
		result = ( ( data[0] == c0.m[0] ) && 
			       ( data[1] == c0.m[1] ) && 
				   ( data[2] == c0.m[2] ) &&
				   ( data[3] == c0.m[3] ) );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22( T d0, T d1, T d2, T d3 );" << "\n";
	}


	// Matrix22( const Vec2<T> &vx, const Vec2<T> &vy );
	{
		bool result = false;
		MatT c0( 1, 2, 3, 4 );
		result = ( c0 == MatT( Vec2<T>( 1, 2 ), Vec2<T>( 3, 4 ) ) );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22( const Vec2<T> &vx, const Vec2<T> &vy );" << "\n";
	}


	// template< typename FromT > Matrix22( const Matrix22<FromT>& src );
	{
		bool result = false;
		Matrix22<double> dmat0( 1, 2, 3, 4 );
		Matrix22<float>  fmat0( 1, 2, 3, 4 );
		result = ( ( dmat0 == Matrix22<double>( fmat0 ) ) && 
			       ( fmat0 == Matrix22<float>( dmat0 ) ) ); 
		os << (result ? "passed" : "FAILED") << " : " << "template< typename FromT > Matrix22( const Matrix22<FromT>& src );" << "\n";
	}


	// Matrix22( const Matrix22<T>& src );
	{
		bool result = false;
		MatT c0( 1, 2, 3, 4 );
		MatT m0( c0 );
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22( const Matrix22<T>& src );" << "\n";
	}


	// operator T*() { return (T*)m; }
	{
		bool result = false;
		MatT c0( 1, 2, 3, 4 );
		T *data = (T*)c0;
		result = ( ( c0.m[0] == data[0] ) &&
			       ( c0.m[1] == data[1] ) &&
				   ( c0.m[2] == data[2] ) &&
				   ( c0.m[3] == data[3] ) );
		os << (result ? "passed" : "FAILED") << " : " << "operator T*() { return (T*)m; }" << "\n";
	}


	// operator const T*() const { return (const T*)m; }
	{
		bool result = false;
		MatT c0( 1, 2, 3, 4 );
		const T *const_data = (const T *)c0;
		result = ( ( c0.m[0] == const_data[0] ) &&
			       ( c0.m[1] == const_data[1] ) &&
				   ( c0.m[2] == const_data[2] ) &&
				   ( c0.m[3] == const_data[3] ) );
		os << (result ? "passed" : "FAILED") << " : " << "operator const T*() const { return (const T*)m; }" << "\n";
	}


	// Matrix22<T>& operator=( const Matrix22<T>& rhs );
	{
		bool result = false;
		MatT c0( 1, 2, 3, 4 );
		MatT m0 = c0;
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T>& operator=( const Matrix22<T>& rhs );" << "\n";
	}


	// Matrix22<T>& operator=( T rhs );
	{
		bool result = false;
		MatT c0( 4, 4, 4, 4 );
		MatT m0 = (T)4;
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T>& operator=( T rhs );" << "\n";
	}


	// template< typename FromT > Matrix22<T>& operator=( const Matrix22<FromT>& rhs );
	{
		bool result = false;
		Matrix22<double> dmat0( 1, 2, 3, 4 );
		Matrix22<float>  fmat0( 1, 2, 3, 4 );		
		Matrix22<double> dmat1 = fmat0;
		Matrix22<float>  fmat1 = dmat0;
		result = ( ( dmat0 == dmat1 ) && 
			       ( fmat0 == fmat1 ) );
		os << (result ? "passed" : "FAILED") << " : " << "template< typename FromT > Matrix22<T>& operator=( const Matrix22<FromT>& rhs );" << "\n";
	}


	// bool equalCompare( const Matrix22<T>& rhs, T epsilon ) const;
	{
		bool result = false;
		T epsilon = (T)1.0e-05;
		MatT c0( (T)1.0e-04, (T)1.0e-04, (T)1.0e-04, (T)1.0e-04 );
		MatT m0( (T)1.3e-04, (T)1.3e-04, (T)1.3e-04, (T)1.3e-04 );
		MatT m1( (T)1.01e-04, (T)1.01e-04, (T)1.01e-04, (T)1.01e-04 );
		result = ( false == ( m0.equalCompare( c0, epsilon ) ) &&
			       true  == ( m1.equalCompare( c0, epsilon ) ) );
		os << (result ? "passed" : "FAILED") << " : " << "bool equalCompare( const Matrix22<T>& rhs, T epsilon ) const;" << "\n";
	}


	// bool operator==( const Matrix22<T> &rhs ) const;
	{
		bool result = false;
		MatT c0( (T)1.2345, (T)2.3456, (T)3.4567, (T)4.5678 );
		MatT m0( (T)1.2345, (T)2.3456, (T)3.4567, (T)4.5678 );
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "bool operator==( const Matrix22<T> &rhs ) const;" << "\n";
	}


	// bool operator!=( const Matrix22<T> &rhs ) const;
	{
		bool result = false;
		MatT c0( (T)1.2345, (T)2.3456, (T)3.4567, (T)4.5678 );
		MatT m0( (T)1.2345, (T)2.3456, (T)3.4567, (T)4.5678 );
		result = ( false == ( c0 != m0 ) );
		os << (result ? "passed" : "FAILED") << " : " << "bool operator!=( const Matrix22<T> &rhs ) const;" << "\n";
	}


	// Matrix22<T>& operator*=( const Matrix22<T> &rhs );
	{
		bool result = false;
		MatT c0( 23, 34, 31, 46 );
		MatT m0( 1, 2, 3, 4 );
		MatT m1( 5, 6, 7, 8 );
		m0 *= m1;
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T>& operator*=( const Matrix22<T> &rhs );" << "\n";
	}


	// Matrix22<T>& operator+=( const Matrix22<T> &rhs );
	{
		bool result = false;
		MatT c0( 6, 8, 10, 12 );
		MatT m0( 1, 2, 3, 4 );
		MatT m1( 5, 6, 7, 8 );
		m0 += m1;
		result = ( c0 == m0 );		
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T>& operator+=( const Matrix22<T> &rhs );" << "\n";
	}


	// Matrix22<T>& operator-=( const Matrix22<T> &rhs );
	{
		bool result = false;
		MatT c0( (T)-4.2, (T)-4.9, (T)-4.3, (T)-4.1 );
		MatT m0( 1, 2, 3, 4 );
		MatT m1( (T)5.2, (T)6.9, (T)7.3, (T)8.1 );
		m0 -= m1;
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T>& operator-=( const Matrix22<T> &rhs );" << "\n";
	}


	// Matrix22<T>& operator*=( T s );
	{
		bool result = false;
		MatT c0( (T)3.03, (T)6.03, (T)-9.03, (T)12.03 );
		MatT m0( (T)1.01, (T)2.01, (T)-3.01, (T)4.01 );
		m0 *= (T)3;
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T>& operator*=( T s );" << "\n";
	}


	// Matrix22<T>& operator/=( T s );
	{
		bool result = false;
		MatT c0( (T)1.01, (T)2.01, (T)-3.01, (T)4.01 );
		MatT m0( (T)3.03, (T)6.03, (T)-9.03, (T)12.03 );
		m0 /= (T)3;
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T>& operator/=( T s );" << "\n";
	}


	// Matrix22<T>& operator+=( T s );
	{
		bool result = false;
		MatT c0( (T)8.053, (T)11.053, (T)-4.007, (T)17.053 );
		MatT m0( (T)3.03, (T)6.03, (T)-9.03, (T)12.03 );
		m0 += (T)5.023;
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T>& operator+=( T s );" << "\n";
	}


	// Matrix22<T>& operator-=( T s );
	{
		bool result = false;
		MatT c0( (T)-1.993, (T)1.007, (T)-14.053, (T)7.007 );
		MatT m0( (T)3.03, (T)6.03, (T)-9.03, (T)12.03 );
		m0 -= (T)5.023;
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T>& operator-=( T s );" << "\n";
	}


	// const Matrix22<T> operator*( const Matrix22<T> &rhs ) const;
	{
		bool result = false;
		MatT c0( 23, 34, 31, 46 );
		MatT m0( 1, 2, 3, 4 );
		MatT m1( 5, 6, 7, 8 );
		result = ( c0 == ( m0*m1 ) );
		os << (result ? "passed" : "FAILED") << " : " << "const Matrix22<T> operator*( const Matrix22<T> &rhs ) const;" << "\n";
	}


	// const Matrix22<T> operator+( const Matrix22<T> &rhs ) const;
	{
		bool result = false;
		MatT c0( 6, 8, 10, 12 );
		MatT m0( 1, 2, 3, 4 );
		MatT m1( 5, 6, 7, 8 );
		result = ( c0 == ( m0 + m1 ) );	
		os << (result ? "passed" : "FAILED") << " : " << "const Matrix22<T> operator+( const Matrix22<T> &rhs ) const;" << "\n";
	}


	// const Matrix22<T> operator-( const Matrix22<T> &rhs ) const;
	{
		bool result = false;
		MatT c0( (T)-4.2, (T)-4.9, (T)-4.3, (T)-4.1 );
		MatT m0( 1, 2, 3, 4 );
		MatT m1( (T)5.2, (T)6.9, (T)7.3, (T)8.1 );
		result = ( c0 == ( m0 - m1 ) );
		os << (result ? "passed" : "FAILED") << " : " << "const Matrix22<T> operator-( const Matrix22<T> &rhs ) const;" << "\n";
	}


	// const Vec2<T> operator*( const Vec2<T> &rhs ) const;
	{
		bool result = false;
		Vec2<T> cv0( (T)-28.395060, (T)-37.037040);
		MatT m0( 1, 2, 3, 4 );
		Vec2<T> v0( (T)1.23456, (T)-9.87654 );
		Vec2<T> diff = ( cv0 - ( m0*v0 ) );
		result = ( fabs( diff.x ) <= EPSILON && fabs( diff.y ) <= EPSILON );
		os << (result ? "passed" : "FAILED") << " : " << "const Vec2<T> operator*( const Vec2<T> &rhs ) const;" << "\n";
	}


	// const Matrix22<T> operator*( T rhs ) const;
	{
		bool result = false;
		MatT c0( (T)3.03, (T)6.03, (T)-9.03, (T)12.03 );
		MatT m0( (T)1.01, (T)2.01, (T)-3.01, (T)4.01 );
		result = ( c0 == ( m0*(T)3 ) );
		os << (result ? "passed" : "FAILED") << " : " << "const Matrix22<T> operator*( T rhs ) const;" << "\n";
	}


	// const Matrix22<T> operator/( T rhs ) const;
	{
		bool result = false;
		MatT c0( (T)1.01, (T)2.01, (T)-3.01, (T)4.01 );
		MatT m0( (T)3.03, (T)6.03, (T)-9.03, (T)12.03 );
		result = ( c0 == ( m0/(T)3 ) );
		os << (result ? "passed" : "FAILED") << " : " << "const Matrix22<T> operator/( T rhs ) const;" << "\n";
	}


	// const Matrix22<T> operator+( T rhs ) const;
	{
		bool result = false;
		MatT c0( (T)8.053, (T)11.053, (T)-4.007, (T)17.053 );
		MatT m0( (T)3.03, (T)6.03, (T)-9.03, (T)12.03 );
		result = ( c0 == ( m0 + (T)5.023 ) );
		os << (result ? "passed" : "FAILED") << " : " << "const Matrix22<T> operator+( T rhs ) const;" << "\n";
	}


	// const Matrix22<T> operator-( T rhs ) const;
	{
		bool result = false;
		MatT c0( (T)-1.993, (T)1.007, (T)-14.053, (T)7.007 );
		MatT m0( (T)3.03, (T)6.03, (T)-9.03, (T)12.03 );
		result = ( c0 == ( m0 - (T)5.023 ) );
		os << (result ? "passed" : "FAILED") << " : " << "const Matrix22<T> operator-( T rhs ) const;" << "\n";
	}


	// T& at( int row, int col );
	{
		bool result = false;
		MatT c0( 1, 2, 3, 4 );
		MatT m0;
		m0.at( 0, 0 ) = 1;
		m0.at( 1, 0 ) = 2;
		m0.at( 0, 1 ) = 3;
		m0.at( 1, 1 ) = 4;
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "T& at( int row, int col );" << "\n";
	}


	// const T& at( int row, int col ) const;
	{
		bool result = false;
		MatT c0( 1, 2, 3, 4 );
		result = ( ( fabs( 1 - c0.at( 0, 0 ) ) < EPSILON ) &&
			       ( fabs( 2 - c0.at( 1, 0 ) ) < EPSILON ) &&
				   ( fabs( 3 - c0.at( 0, 1 ) ) < EPSILON ) &&
				   ( fabs( 4 - c0.at( 1, 1 ) ) < EPSILON ) );
		os << (result ? "passed" : "FAILED") << " : " << "const T& at( int row, int col ) const;" << "\n";
	}


	// Vec2<T> getColumn( int col ) const;
	{
		bool result = false;
		MatT m0( 1, 2, 3, 4 );
		result = ( ( Vec2<T>( (T)1, (T)2 ) == m0.getColumn( 0 ) ) &&
			       ( Vec2<T>( (T)3, (T)4 ) == m0.getColumn( 1 ) ) );
		os << (result ? "passed" : "FAILED") << " : " << "Vec2<T> getColumn( int col ) const;" << "\n";
	}


	// void setColumn( int col, const Vec2<T> &v );
	{
		bool result = false;
		MatT c0( 1, 2, 3, 4 );
		MatT m0;
		m0.setColumn( 0, Vec2<T>( 1, 2 ) );
		m0.setColumn( 1, Vec2<T>( 3, 4 ) );
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "void setColumn( int col, const Vec2<T> &v );" << "\n";
	}


	// Vec2<T> getRow( int row ) const;
	{
		bool result = false;
		MatT m0( 1, 2, 3, 4 );
		result = ( ( Vec2<T>( (T)1, (T)3 ) == m0.getRow( 0 ) ) &&
			       ( Vec2<T>( (T)2, (T)4 ) == m0.getRow( 1 ) ) );
		os << (result ? "passed" : "FAILED") << " : " << "Vec2<T> getRow( int row ) const;" << "\n";
	}


	// void setRow( int row, const Vec2<T> &v );
	{
		bool result = false;
		MatT c0( 1, 2, 3, 4 );
		MatT m0;
		m0.setRow( 0, Vec2<T>( 1, 3 ) );
		m0.setRow( 1, Vec2<T>( 2, 4 ) );
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "void setRow( int row, const Vec2<T> &v );" << "\n";
	}


	// void getColumns( Vec2<T> *c0, Vec2<T> *c1 ) const;
	{
		bool result = false;
		Vec2<T> cv0( 1, 2);
		Vec2<T> cv1( 3, 4);
		MatT m0( 1, 2, 3, 4 );
		Vec2<T> v0, v1;
		m0.getColumns( &v0, &v1 );
		result = ( ( cv0 == v0 ) && ( cv1 == v1 ) );
		os << (result ? "passed" : "FAILED") << " : " << "void getColumns( Vec2<T> *c0, Vec2<T> *c1 ) const;" << "\n";
	}


	// void setColumns( const Vec2<T> &c0, const Vec2<T> &c1 );
	{
		bool result = false;
		MatT c0( 1, 2, 3, 4 );
		MatT m0;
		m0.setColumn( 0, Vec2<T>( 1, 2 ) );
		m0.setColumn( 1, Vec2<T>( 3, 4 ) );
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "void setColumns( const Vec2<T> &c0, const Vec2<T> &c1 );" << "\n";
	}


	// void getRows( Vec2<T> *r0, Vec2<T> *r1 ) const;
	{
		bool result = false;
		Vec2<T> cv0( 1, 3);
		Vec2<T> cv1( 2, 4);
		MatT m0( 1, 2, 3, 4 );
		Vec2<T> v0, v1;
		m0.getRows( &v0, &v1 );
		result = ( ( cv0 == v0 ) && ( cv1 == v1 ) );
		os << (result ? "passed" : "FAILED") << " : " << "void getRows( Vec2<T> *r0, Vec2<T> *r1 ) const;" << "\n";
	}


	// void setRows( const Vec2<T> &r0, const Vec2<T> &r1 );
	{
		bool result = false;
		MatT c0( 1, 2, 3, 4 );
		MatT m0;
		m0.setRows( Vec2<T>( 1, 3 ), Vec2<T>( 2, 4 ) );
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "void setRows( const Vec2<T> &r0, const Vec2<T> &r1 );" << "\n";
	}


	// void setToNull();
	{
		bool result = false;
		MatT c0( 0, 0, 0, 0 );
		MatT m0( 1, 2, 3, 4 );
		m0.setToNull();
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "void setToNull();" << "\n";
	}


	// void setToIdentity();
	{
		bool result = false;
		MatT c0( 1, 0, 0, 1 );
		MatT m0( 1, 2, 3, 4 );
		m0.setToIdentity();
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "void setToIdentity();" << "\n";
	}


	// T determinant() const;
	{
		bool result = false;
		MatT m0( 1, 2, 3, 4 );
		T det = m0.determinant();
		result = fabs( det - (T)-2.0 ) < EPSILON;
		os << (result ? "passed" : "FAILED") << " : " << "T determinant() const;" << "\n";
	}


	// T trace() const;
	{
		bool result = false;
		MatT m0( 1, 2, 3, 4 );
		T trace = m0.trace();
		result = fabs( trace - (T)5.0 ) < EPSILON;
		os << (result ? "passed" : "FAILED") << " : " << "T trace() const;" << "\n";
	}


	// Matrix22<T> diagonal() const;
	{
		bool result = false;
		MatT c0( 1, 0, 0, 4 );
		MatT m0( 1, 2, 3, 4 );
		result = ( c0 == m0.diagonal() );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T> diagonal() const;" << "\n";
	}


	// Matrix22<T> lowerTriangular() const;
	{
		bool result = false;
		MatT c0( 1, 2, 0, 4 );
		MatT m0( 1, 2, 3, 4 );
		result = ( c0 == m0.lowerTriangular() );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T> lowerTriangular() const;" << "\n";
	}


	// Matrix22<T> upperTriangular() const;
	{
		bool result = false;
		MatT c0( 1, 0, 3, 4 );
		MatT m0( 1, 2, 3, 4 );
		result = ( c0 == m0.upperTriangular() );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T> upperTriangular() const;" << "\n";
	}


	// void transpose();
	{
		bool result = false;
		MatT c0( 1, 3, 2, 4 );
		MatT m0( 1, 2, 3, 4 );
		m0.transpose();
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "void transpose();" << "\n";
	}

	// Matrix22<T> transposed() const;
	{
		bool result = false;
		MatT c0( 1, 3, 2, 4 );
		MatT m0( 1, 2, 3, 4 );
		result = ( c0 == m0.transposed() );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T> transposed() const;" << "\n";
	}


	// void invert (T epsilon = FLT_MIN );
	{
		bool result = false;
		MatT c0( 
			(T)-67114.093959732, (T)0.3355704698, 
			(T)0.5033557047,     (T)-1.68e-8 
		);
		MatT m0( 
			(T)0.0000001, (T)2,
			(T)3,         (T)400000
		);
		m0.invert();
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "void invert (T epsilon = FLT_MIN );" << "\n";
	}


	// Matrix22<T> inverted( T epsilon = FLT_MIN ) const;
	{
		bool result = false;

		MatT c0( 
			(T)-67114.093959732, (T)0.3355704698, 
			(T)0.5033557047,     (T)-1.68e-8 
		);
		MatT m0( 
			(T)0.0000001, (T)2,
			(T)3,         (T)400000
		);

		bool single = ( c0 == m0.inverted() );

		bool multi = true;
		T da = (T)0.0015;
		T eu = 0;

		int iter = 100000;
		for( int i = 0; i < iter; ++i ) {
			MatT m0 = MatT::createRotation( eu );
			MatT m1 = m0.inverted();
			MatT res0 = m0*m1;
			MatT res1 = m1.inverted();
			if( ! ( MatT::identity() == res0 && res1 == m0 ) ) {
				multi = false;
				break;
			}
			eu += da; 
		}

		result = single && multi;

		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T> inverted( T epsilon = FLT_MIN ) const;" << "\n";
	}


	// Vec2<T> preMultiply( const Vec2<T> &v ) const;
	{
		bool result = false;
		Vec2<T> cv0( (T)-18.518520, (T)-35.802480 );
		MatT m0( 1, 2, 3, 4 );
		Vec2<T> v0( (T)1.23456, (T)-9.87654 );
		Vec2<T> diff = ( cv0 - m0.preMultiply( v0 ) );
		result = ( fabs( diff.x ) <= EPSILON && fabs( diff.y ) <= EPSILON );
		os << (result ? "passed" : "FAILED") << " : " << "Vec2<T> preMultiply( const Vec2<T> &v ) const;" << "\n";
	}


	// Vec2<T> postMultiply( const Vec2<T> &v ) const;
	{
		bool result = false;
		Vec2<T> cv0( (T)-28.395060, (T)-37.037040 );
		MatT m0( 1, 2, 3, 4 );
		Vec2<T> v0( (T)1.23456, (T)-9.87654 );
		Vec2<T> diff = ( cv0 - m0.postMultiply( v0 ) );
		result = ( fabs( diff.x ) <= EPSILON && fabs( diff.y ) <= EPSILON );
		os << (result ? "passed" : "FAILED") << " : " << "Vec2<T> postMultiply( const Vec2<T> &v ) const;" << "\n";
	}


	// Vec2<T> transformVec( const Vec2<T> &rhs );
	{
		bool result = false;
		Vec2<T> cv0( (T)-28.395060, (T)-37.037040 );
		MatT m0( 1, 2, 3, 4 );
		Vec2<T> v0( (T)1.23456, (T)-9.87654 );
		Vec2<T> diff = ( cv0 - m0.transformVec( v0 ) );
		result = ( fabs( diff.x ) <= EPSILON && fabs( diff.y ) <= EPSILON );
		os << (result ? "passed" : "FAILED") << " : " << "Vec2<T> transformVec( const Vec2<T> &rhs );" << "\n";
	}


	// void rotate( T radians );
	{
		bool result = false;
		MatT c0( (T) 2.182514, (T)-0.486452, 
			     (T) 4.606950, (T)-1.943200 );
		MatT m0( 1, 2, 3, 4 );
		m0.rotate( toRadians( (T)76 ) );
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "void rotate( T radians );" << "\n";
	}


	// void scale( T s );
	{
		bool result = false;
		MatT c0( 2, 4, 6, 8 );
		MatT m0( 1, 2, 3, 4 );
		m0.scale( 2 );
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "void scale( T s );" << "\n";
	}


	// void scale( const Vec2<T> &v );
	{
		bool result = false;
		MatT c0( 2, 6, 6, 12 );
		MatT m0( 1, 2, 3, 4 );
		m0.scale( Vec2<T>( 2, 3 ) );
		result = ( c0 == m0 );
		os << (result ? "passed" : "FAILED") << " : " << "void scale( const Vec2<T> &v );" << "\n";
	}


	// Matrix22<T> invertTransform() const;
	{
		bool result = false;
		MatT c0( (T)1.02, (T)3.04, (T)2.03, (T)4.05 );
		MatT m0( (T)1.02, (T)2.03, (T)3.04, (T)4.05 );
		result = ( c0 == m0.invertTransform() );		
		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T> invertTransform() const;" << "\n";
	}


	// static Matrix22<T> identity();
	{
		bool result = false;
		MatT c0( 1, 0, 0, 1 );
		result = ( c0 == MatT::identity() );
		os << (result ? "passed" : "FAILED") << " : " << "static Matrix22<T> identity();" << "\n";
	}


	// static Matrix22<T> one();
	{
		bool result = false;
		MatT c0( 1, 1, 1, 1 );
		result = ( c0 == MatT::one() );
		os << (result ? "passed" : "FAILED") << " : " << "static Matrix22<T> one();" << "\n";
	}


	// static Matrix22<T> zero();
	{
		bool result = false;
		MatT c0( 0, 0, 0, 0 );
		result = ( c0 == MatT::zero() );
		os << (result ? "passed" : "FAILED") << " : " << "static Matrix22<T> zero();" << "\n";
	}


	// static Matrix22<T> createRotation( T radians );
	{
		bool result = false;
		MatT c0( (T) 0.241922, (T)-0.970296, 
			     (T) 0.970296, (T) 0.241922 );
		result = ( c0 == MatT::createRotation( toRadians( (T)76 ) ) );
		os << (result ? "passed" : "FAILED") << " : " << "static Matrix22<T> createRotation( T radians );" << "\n";
	}


	// static Matrix22<T> createScale( T s );
	{
		bool result = false;
		MatT c0( 3, 0, 0, 3 );
		result = ( c0 == MatT::createScale( 3 ) );
		os << (result ? "passed" : "FAILED") << " : " << "static Matrix22<T> createScale( T s );" << "\n";
	}


	// static Matrix22<T> createScale( const Vec2<T> &v );
	{
		bool result = false;
		MatT c0( 3, 0, 0, 4 );
		result = ( c0 == MatT::createScale( Vec2<T>( 3, 4 ) ) );
		os << (result ? "passed" : "FAILED") << " : " << "static Matrix22<T> createScale( const Vec2<T> &v );" << "\n";
	}
}
