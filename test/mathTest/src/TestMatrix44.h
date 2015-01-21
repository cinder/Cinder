#pragma once

template <typename T> void TestMatrix44( std::ostream& os )
{
	typedef Matrix44<T> MatT;

	// Matrix44();
	{
		bool result = false;

		MatT c0( 1, 0, 0, 0,
			     0, 1, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1 );
		MatT m0;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44();" << "\n";
	}


	// Matrix44( T s );
	{
		bool result = false;

		MatT c0( 3, 3, 3, 3,
			     3, 3, 3, 3,
				 3, 3, 3, 3,
				 3, 3, 3, 3 );

		MatT m0( 3 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44( T s );" << "\n";
	}


	// Matrix44( const T *dt );
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		T data[16] = 
		{
			(T) 1, (T) 2, (T) 3, (T) 4,
			(T) 5, (T) 6, (T) 7, (T) 8,
		    (T) 9, (T)10, (T)11, (T)12,
			(T)13, (T)14, (T)15, (T)16 
		};

		MatT m0( data );

		result = ( c0 == m0 );	

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44( const T *dt );" << "\n";
	}


	// Matrix44( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8, T d9, T d10, T d11, T d12, T d13, T d14, T d15 );
	{
		bool result = false;

		MatT c0;
		c0.m00 =  1; c0.m01 =  5; c0.m02 =  9; c0.m03 = 13;
		c0.m10 =  2; c0.m11 =  6; c0.m12 = 10; c0.m13 = 14;
		c0.m20 =  3; c0.m21 =  7; c0.m22 = 11; c0.m23 = 15;
		c0.m30 =  4; c0.m31 =  8; c0.m32 = 12; c0.m33 = 16;

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8, T d9, T d10, T d11, T d12, T d13, T d14, T d15 );" << "\n";
	}


	// Matrix44( const Vec3<T> &vx, const Vec3<T> &vy, const Vec3<T> &vz ); 
	{
		bool result = false;

		MatT c0(  1,  2,  3,  0,
			      5,  6,  7,  0,
				  9, 10, 11,  0,
				  0,  0,  0,  1 );

		MatT m0( Vec3<T>( 1, 2, 3 ), Vec3<T>( 5, 6, 7 ), Vec3<T>( 9, 10, 11 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44( const Vec3<T> &vx, const Vec3<T> &vy, const Vec3<T> &vz ); " << "\n";
	}


	// Matrix44( const Vec4<T> &vx, const Vec4<T> &vy, const Vec4<T> &vz, const Vec4<T> &vw = Vec4<T>( 0, 0, 0, 1 ) ); 
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m0( Vec4<T>(  1,  2,  3,  4 ), 
			     Vec4<T>(  5,  6,  7,  8 ), 
			     Vec4<T>(  9, 10, 11, 12 ),
			     Vec4<T>( 13, 14, 15, 16 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44( const Vec4<T> &vx, const Vec4<T> &vy, const Vec4<T> &vz, const Vec4<T> &vw = Vec4<T>( 0, 0, 0, 1 ) ); " << "\n";
	}


	// template< typename FromT > Matrix44( const Matrix44<FromT>& src );
	{
		bool result = false;

		Matrix44<double> dmat0(
			 1,  2,  3,  4,
			 5,  6,  7,  8,
			 9, 10, 11, 12,
			13, 14, 15, 16 
		);

		Matrix44<float> fmat0(
			 1,  2,  3,  4,
			 5,  6,  7,  8,
			 9, 10, 11, 12,
			13, 14, 15, 16 
		);

		Matrix44<double> dmat1( fmat0 );
		Matrix44<double> fmat1( dmat0 );
		
		result = ( dmat0 == dmat1 ) && ( fmat0 == fmat1 );

		os << (result ? "passed" : "FAILED") << " : " << "template< typename FromT > Matrix44( const Matrix44<FromT>& src );" << "\n";
	}

	// Matrix44( const Matrix22<T>& src );
	{
		bool result = false;

		MatT c0( 1, 2, 0, 0,
			     3, 4, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1 );

		Matrix22<T> mat22( 1, 2, 3, 4 );

		MatT m0( mat22 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44( const Matrix22<T>& src );" << "\n";
	}

	// Matrix44( const Matrix33<T>& src );
	{
		bool result = false;

		MatT c0( 1, 2, 3, 0,
			     4, 5, 6, 0,
				 7, 8, 9, 0,
				 0, 0, 0, 1 );

		Matrix33<T> mat33( 
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);

		MatT m0( mat33 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44( const Matrix33<T>& src );" << "\n";
	}

	// Matrix44( const Matrix44<T>& src );
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m0( c0 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44( const Matrix44<T>& src );" << "\n";
	}


	// operator T*() { return (T*)m; }
	{
		bool result = false;

		MatT c0(  2,  4,  6,  8,
			     10, 12, 14, 16,
				 18, 20, 22, 24,
				 26, 28, 30, 32 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		T* data = (T*)m0;
		
		for( int i = 0; i < MatT::DIM_SQ; ++i ) {
			data[i] *= 2;
		}

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "operator T*() { return (T*)m; }" << "\n";
	}


	// operator const T*() const { return (const T*)m; }
	{
		bool result = false;

		T cdata[16] = 
		{
			(T) 1, (T) 2, (T) 3, (T) 4,
			(T) 5, (T) 6, (T) 7, (T) 8,
			(T) 9, (T)10, (T)11, (T)12,
			(T)13, (T)14, (T)15, (T)16 
		};

		MatT m0(  1,  2,  3,  4,
				  5,  6,  7,  8,
			      9, 10, 11, 12,
			     13, 14, 15, 16 );

		const T *data = (const T*)m0;

		bool isOk = true;
		for( int i = 0; i < MatT::DIM_SQ; ++i ) {
			T diff = fabs( data[i] - cdata[i] );
			if( diff >= EPSILON ) {
				isOk = false;
				break;
			}
		}

		result = isOk;

		os << (result ? "passed" : "FAILED") << " : " << "operator const T*() const { return (const T*)m; }" << "\n";
	}


	// Matrix44<T>& operator=( const Matrix44<T>& rhs );
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m0 = c0;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T>& operator=( const Matrix44<T>& rhs );" << "\n";
	}


	// Matrix44<T>& operator=( T rhs );
	{
		bool result = false;

		MatT c0( 3, 3, 3, 3,
			     3, 3, 3, 3,
				 3, 3, 3, 3,
				 3, 3, 3, 3 );

		MatT m0 = 3;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T>& operator=( T rhs );" << "\n";
	}


	// template< typename FromT > Matrix44<T>& operator=( const Matrix44<FromT>& rhs );
	{
		bool result = false;

		Matrix44<double> dmat0(
			 1,  2,  3,  4,
			 5,  6,  7,  8,
			 9, 10, 11, 12,
			13, 14, 15, 16 
		);

		Matrix44<float> fmat0(
			 1,  2,  3,  4,
			 5,  6,  7,  8,
			 9, 10, 11, 12,
			13, 14, 15, 16 
		);

		Matrix44<double> dmat1 = fmat0;
		Matrix44<double> fmat1 = dmat0;
		
		result = ( dmat0 == dmat1 ) && ( fmat0 == fmat1 );
		
		os << (result ? "passed" : "FAILED") << " : " << "template< typename FromT > Matrix44<T>& operator=( const Matrix44<FromT>& rhs );" << "\n";
	}


	// Matrix44<T>& operator=( const Matrix22<T>& rhs );
	{
		bool result = false;

		MatT c0( 1, 2, 0, 0,
			     3, 4, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1 );

		Matrix22<T> mat22( 1, 2, 3, 4 );

		MatT m0;
		m0 = mat22;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T>& operator=( const Matrix22<T>& rhs );" << "\n";
	}


	// Matrix44<T>& operator=( const Matrix33<T>& rhs );
	{
		bool result = false;

		MatT c0( 1, 2, 3, 0,
			     4, 5, 6, 0,
				 7, 8, 9, 0,
				 0, 0, 0, 1 );

		Matrix33<T> mat33( 
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);

		MatT m0;
		m0 = mat33;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T>& operator=( const Matrix33<T>& rhs );" << "\n";
	}


	// bool equalCompare( const Matrix44<T>& rhs, T epsilon ) const;
	{
		bool result = false;

		T epsilon = (T)1.0e-04;

		MatT c0( (T)1.0e-04, (T)1.0e-04, (T)1.0e-04, (T)1.0e-04,
			     (T)1.0e-04, (T)1.0e-04, (T)1.0e-04, (T)1.0e-04,
				 (T)1.0e-04, (T)1.0e-04, (T)1.0e-04, (T)1.0e-04,
				 (T)1.0e-04, (T)1.0e-04, (T)1.0e-04, (T)1.0e-04 );

		MatT m0( (T)3.0e-04, (T)3.0e-04, (T)3.0e-04, (T)3.0e-04,
			     (T)3.0e-04, (T)3.0e-04, (T)3.0e-04, (T)3.0e-04,
				 (T)3.0e-04, (T)3.0e-04, (T)3.0e-04, (T)3.0e-04,
				 (T)3.0e-04, (T)3.0e-04, (T)3.0e-04, (T)3.0e-04 );

		MatT m1( (T)1.5e-04, (T)1.5e-04, (T)1.5e-04, (T)1.5e-04,
			     (T)1.5e-04, (T)1.5e-04, (T)1.5e-04, (T)1.5e-04,
				 (T)1.5e-04, (T)1.5e-04, (T)1.5e-04, (T)1.5e-04,
				 (T)1.5e-04, (T)1.5e-04, (T)1.5e-04, (T)1.5e-04 );

	
		result = ( false == c0.equalCompare( m0, epsilon ) ) && ( true == c0.equalCompare( m1, epsilon ) );

		os << (result ? "passed" : "FAILED") << " : " << "bool equalCompare( const Matrix44<T>& rhs, T epsilon ) const;" << "\n";
	}


	// bool operator==( const Matrix44<T> &rhs );
	{
		bool result = false;

		MatT c0( 1, 2, 3, 0,
			     4, 5, 6, 0,
				 7, 8, 9, 0,
				 0, 0, 0, (T)1.234e-06 );

		MatT m0( 1, 2, 3, 0,
			     4, 5, 6, 0,
				 7, 8, 9, 0,
				 0, 0, 0, (T)1.235e-06  );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "bool operator==( const Matrix44<T> &rhs );" << "\n";
	}


	// bool operator!=( const Matrix44<T> &rhs );
	{
		bool result = false;

		MatT c0( 1, 2, 3, 0,
			     4, 5, 6, 0,
				 7, 8, 9, 0,
				 0, 0, 0, (T)1.234e-06 );

		MatT m0( 2, 2, 3, 0,
			     4, 5, 6, 0,
				 7, 8, 9, 0,
				 0, 0, 0, (T)1.235e-06  );

		result = ( c0 != m0 );

		os << (result ? "passed" : "FAILED") << " : " << "bool operator!=( const Matrix44<T> &rhs );" << "\n";
	}


	// Matrix44<T>& operator*=( const Matrix44<T> &rhs );
	{
		bool result = false;
		
		MatT c0(
			 46,  52,  58,  64,
			 74,  84,  94, 104,
			130, 148, 166, 184,
			158, 180, 202, 224
		);

		MatT m0(
			 1,  2,  3,  4,
			 5,  6,  7,  8,
			 9, 10, 11, 12,
			13, 14, 15, 16
		);

		MatT m1(
			1, 2, 1, 2,
			2, 3, 2, 3,
			4, 5, 4, 5,
			5, 6, 5, 6
		);

		m0 *= m1;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T>& operator*=( const Matrix44<T> &rhs );" << "\n";

		int iter = 10000000;
		os << "   " << "speed test:" << "\n";
		// Multiply
		{
			MatT b; 
			Timer timer;
			timer.start();
			for( int i = 0; i < iter; ++i ) {
				b += c0*(m0*(float)i);
			}
			timer.stop();
			os << "   " << "   " << iter << " operator* calls took " << timer.getSeconds() << " seconds" << "\n";
		}
		{
			CINDER_ALIGN16_MATRIX44F( align_c0 ) = c0;
			CINDER_ALIGN16_MATRIX44F( align_m0 ) = m0;
			CINDER_ALIGN16_MATRIX44F( b ); 

			Timer timer;
			timer.start();
			for( int i = 0; i < iter; ++i ) {
				b += SseMultiply( align_c0, align_m0*(float)i );
			}
			timer.stop();
			os << "   " << "   " << iter << " MatrixAlgo::SseMultiply() calls took " << timer.getSeconds() << " seconds" << "\n";
		}
	}


	// Matrix44<T>& operator+=( const Matrix44<T> &rhs );
	{
		bool result = false;

		MatT c0(  2,  4,  6,  8,
			     10, 12, 14, 16,
				 18, 20, 22, 24,
				 26, 28, 30, 32 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m1(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );
		
		m0 += m1;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T>& operator+=( const Matrix44<T> &rhs );" << "\n";
	}


	// Matrix44<T>& operator-=( const Matrix44<T> &rhs );
	{
		bool result = false;

		MatT c0( 0, 0, 0, 0,
			     0, 0, 0, 0,
				 0, 0, 0, 0,
				 0, 0, 0, 0 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m1(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );
		
		m0 -= m1;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T>& operator-=( const Matrix44<T> &rhs );" << "\n";
	}


	// Matrix44<T>& operator*=( T rhs );
	{
		bool result = false;

		MatT c0(  2,  4,  6,  8,
			     10, 12, 14, 16,
				 18, 20, 22, 24,
				 26, 28, 30, 32 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		m0 *= 2;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T>& operator*=( T rhs );" << "\n";
	}


	// Matrix44<T>& operator/=( T rhs );
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m0(  2,  4,  6,  8,
			     10, 12, 14, 16,
				 18, 20, 22, 24,
				 26, 28, 30, 32 );

		m0 /= 2;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T>& operator/=( T rhs );" << "\n";
	}


	// Matrix44<T>& operator+=( T rhs );
	{
		bool result = false;

		MatT c0( (T) 1.025, (T) 2.025, (T) 3.025, (T) 4.025,
			     (T) 5.025, (T) 6.025, (T) 7.025, (T) 8.025,
				 (T) 9.025, (T)10.025, (T)11.025, (T)12.025,
				 (T)13.025, (T)14.025, (T)15.025, (T)16.025 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		m0 += (T)0.025;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T>& operator+=( T rhs );" << "\n";
	}


	// Matrix44<T>& operator-=( T rhs );
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m0( (T) 1.025, (T) 2.025, (T) 3.025, (T) 4.025,
			     (T) 5.025, (T) 6.025, (T) 7.025, (T) 8.025,
				 (T) 9.025, (T)10.025, (T)11.025, (T)12.025,
				 (T)13.025, (T)14.025, (T)15.025, (T)16.025 );


		m0 -= (T)0.025;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T>& operator-=( T rhs );" << "\n";
	}


	// const Matrix44<T> operator*( const Matrix44<T> &rhs ) const;
	{
		bool result = false;
		
		MatT c0(
			 46,  52,  58,  64,
			 74,  84,  94, 104,
			130, 148, 166, 184,
			158, 180, 202, 224
		);

		MatT m0(
			 1,  2,  3,  4,
			 5,  6,  7,  8,
			 9, 10, 11, 12,
			13, 14, 15, 16
		);

		MatT m1(
			1, 2, 1, 2,
			2, 3, 2, 3,
			4, 5, 4, 5,
			5, 6, 5, 6
		);

		result = ( c0 == ( m0*m1 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix44<T> operator*( const Matrix44<T> &rhs ) const;" << "\n";
	}


	// const Matrix44<T> operator+( const Matrix44<T> &rhs ) const;
	{
		bool result = false;

		MatT c0(  2,  4,  6,  8,
			     10, 12, 14, 16,
				 18, 20, 22, 24,
				 26, 28, 30, 32 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m1(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );
		
		result = ( c0 == ( m0 + m1 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix44<T> operator+( const Matrix44<T> &rhs ) const;" << "\n";
	}


	// const Matrix44<T> operator-( const Matrix44<T> &rhs ) const;
	{
		bool result = false;

		MatT c0( 0, 0, 0, 0,
			     0, 0, 0, 0,
				 0, 0, 0, 0,
				 0, 0, 0, 0 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m1(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );
		
		result = ( c0 == ( m0 - m1 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix44<T> operator-( const Matrix44<T> &rhs ) const;" << "\n";
	}


	// const Vec3<T> operator*( const Vec3<T> &rhs ) const;
	{
		bool result = false;

		Vec3<T> cv0( 1, 2, 3 );

		MatT c0( 1, 0, 0, 0,
			     0, 1, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1 );

		Vec3<T> diff = (cv0 - c0*cv0 );
		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );

		result = ( diff.x <= EPSILON ) && ( diff.y <= EPSILON ) && ( diff.z <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "const Vec3<T> operator*( const Vec3<T> &rhs ) const;" << "\n";
	}


	// const Vec4<T> operator*( const Vec4<T> &rhs ) const;
	{
		bool result = false;

		Vec4<T> cv0( 1, 2, 3, 4 );

		MatT c0( 1, 0, 0, 0,
			     0, 1, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1 );

		Vec4<T> diff = (cv0 - c0*cv0 );
		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );
		diff.z = fabs( diff.w );

		result = ( diff.x <= EPSILON ) && 
			     ( diff.y <= EPSILON ) && 
				 ( diff.z <= EPSILON ) &&
				 ( diff.w <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "const Vec4<T> operator*( const Vec4<T> &rhs ) const;" << "\n";
	}


	// const Matrix44<T> operator*( T rhs ) const;
	{
		bool result = false;

		MatT c0(  2,  4,  6,  8,
			     10, 12, 14, 16,
				 18, 20, 22, 24,
				 26, 28, 30, 32 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		result = ( c0 == ( m0*(T)2 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix44<T> operator*( T rhs ) const;" << "\n";
	}


	// const Matrix44<T> operator/( T rhs ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m0(  2,  4,  6,  8,
			     10, 12, 14, 16,
				 18, 20, 22, 24,
				 26, 28, 30, 32 );

		result = ( c0 == ( m0/(T)2 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix44<T> operator/( T rhs ) const;" << "\n";
	}


	// const Matrix44<T> operator+( T rhs ) const;
	{
		bool result = false;

		MatT c0( (T) 1.025, (T) 2.025, (T) 3.025, (T) 4.025,
			     (T) 5.025, (T) 6.025, (T) 7.025, (T) 8.025,
				 (T) 9.025, (T)10.025, (T)11.025, (T)12.025,
				 (T)13.025, (T)14.025, (T)15.025, (T)16.025 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		result = ( c0 == ( m0 + (T)0.025 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix44<T> operator+( T rhs ) const;" << "\n";
	}


	// const Matrix44<T> operator-( T rhs ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m0( (T) 1.025, (T) 2.025, (T) 3.025, (T) 4.025,
			     (T) 5.025, (T) 6.025, (T) 7.025, (T) 8.025,
				 (T) 9.025, (T)10.025, (T)11.025, (T)12.025,
				 (T)13.025, (T)14.025, (T)15.025, (T)16.025 );


		result = ( c0 == ( m0 - (T)0.025 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix44<T> operator-( T rhs ) const;" << "\n";
	}


	// T& at( int row, int col );
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m0;
		m0.at( 0, 0 ) =  1;
		m0.at( 1, 0 ) =  2;
		m0.at( 2, 0 ) =  3;
		m0.at( 3, 0 ) =  4;

		m0.at( 0, 1 ) =  5;
		m0.at( 1, 1 ) =  6;
		m0.at( 2, 1 ) =  7;
		m0.at( 3, 1 ) =  8;

		m0.at( 0, 2 ) =  9;
		m0.at( 1, 2 ) = 10;
		m0.at( 2, 2 ) = 11;
		m0.at( 3, 2 ) = 12;

		m0.at( 0, 3 ) = 13;
		m0.at( 1, 3 ) = 14;
		m0.at( 2, 3 ) = 15;
		m0.at( 3, 3 ) = 16;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "T& at( int row, int col );" << "\n";
	}


	// const T& at( int row, int col ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		bool isOk = true;
		for( int r = 0; r < 4; ++r ) {
			for( int c = 0; c < 4; ++c ) {
				if( c0.at( r, c ) != m0.at( r, c ) ) {
					isOk = false;
					break;
				}
			}
		}

		result = isOk;

		os << (result ? "passed" : "FAILED") << " : " << "const T& at( int row, int col ) const;" << "\n";
	}


	// Vec4<T> getColumn( int col ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		result = ( c0.getColumn( 0 ) == Vec4<T>(  1,  2,  3,  4 ) ) &&
				 ( c0.getColumn( 1 ) == Vec4<T>(  5,  6,  7,  8 ) ) &&
				 ( c0.getColumn( 2 ) == Vec4<T>(  9, 10, 11, 12 ) ) &&
				 ( c0.getColumn( 3 ) == Vec4<T>( 13, 14, 15, 16  ) );
		
		os << (result ? "passed" : "FAILED") << " : " << "Vec4<T> getColumn( int col ) const;" << "\n";
	}


	// void setColumn( int col, const Vec4<T> &v );
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );
		MatT m0;
		m0.setToIdentity();

		m0.setColumn( 0, Vec4<T>(  1,  2,  3,  4 ) );
		m0.setColumn( 1, Vec4<T>(  5,  6,  7,  8 ) );
		m0.setColumn( 2, Vec4<T>(  9, 10, 11, 12 ) );
		m0.setColumn( 3, Vec4<T>( 13, 14, 15, 16 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void setColumn( int col, const Vec4<T> &v );" << "\n";
	}


	// Vec4<T> getRow( int row ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		result = ( c0.getRow( 0 ) == Vec4<T>(  1,  5,  9, 13 ) ) &&
				 ( c0.getRow( 1 ) == Vec4<T>(  2,  6, 10, 14 ) ) &&
				 ( c0.getRow( 2 ) == Vec4<T>(  3,  7, 11, 15 ) ) &&
				 ( c0.getRow( 3 ) == Vec4<T>(  4,  8, 12, 16 ) );

		os << (result ? "passed" : "FAILED") << " : " << "Vec4<T> getRow( int row ) const;" << "\n";
	}


	// void setRow( int row, const Vec4<T> &v );
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );
		MatT m0;
		m0.setToIdentity();

		m0.setRow( 0, Vec4<T>( 1,  5,  9, 13 ) );
		m0.setRow( 1, Vec4<T>( 2,  6, 10, 14 ) );
		m0.setRow( 2, Vec4<T>( 3,  7, 11, 15 ) );
		m0.setRow( 3, Vec4<T>( 4,  8, 12, 16 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void setRow( int row, const Vec4<T> &v );" << "\n";
	}


	// void getColumns( Vec4<T> *c0, Vec4<T> *c1, Vec4<T> *c2, Vec4<T> *c3 ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		Vec4<T> v0, v1, v2, v3;
		c0.getColumns( &v0, &v1, &v2, &v3 );

		result = ( v0 == Vec4<T>(  1,  2,  3,  4 ) ) &&
				 ( v1 == Vec4<T>(  5,  6,  7,  8 ) ) &&
				 ( v2 == Vec4<T>(  9, 10, 11, 12 ) ) &&
				 ( v3 == Vec4<T>( 13, 14, 15, 16  ) );

		os << (result ? "passed" : "FAILED") << " : " << "void getColumns( Vec4<T> *c0, Vec4<T> *c1, Vec4<T> *c2, Vec4<T> *c3 ) const;" << "\n";
	}


	// void setColumns( const Vec4<T> &c0, const Vec4<T> &c1, const Vec4<T> &c2, const Vec4<T> &c3 );
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );
		MatT m0;
		m0.setToIdentity();

		Vec4<T> v0(  1,  2,  3,  4 );
		Vec4<T> v1(  5,  6,  7,  8 );
		Vec4<T> v2(  9, 10, 11, 12 );
		Vec4<T> v3( 13, 14, 15, 16 );

		m0.setColumns( v0, v1, v2, v3 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void setColumns( const Vec4<T> &c0, const Vec4<T> &c1, const Vec4<T> &c2, const Vec4<T> &c3 );" << "\n";
	}


	// void getRows( Vec4<T> *r0, Vec4<T> *r1, Vec4<T> *r2, Vec4<T> *r3 ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		Vec4<T> v0, v1, v2, v3;
		c0.getRows( &v0, &v1, &v2, &v3 );

		result = ( v0 == Vec4<T>(  1,  5,  9, 13 ) ) &&
				 ( v1 == Vec4<T>(  2,  6, 10, 14 ) ) &&
				 ( v2 == Vec4<T>(  3,  7, 11, 15 ) ) &&
				 ( v3 == Vec4<T>(  4,  8, 12, 16 ) );

		os << (result ? "passed" : "FAILED") << " : " << "void getRows( Vec4<T> *r0, Vec4<T> *r1, Vec4<T> *r2, Vec4<T> *r3 ) const;" << "\n";
	}


	// void setRows( const Vec4<T> &r0, const Vec4<T> &r1, const Vec4<T> &r2, const Vec4<T> &r3 );
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );
		MatT m0;
		m0.setToIdentity();

		Vec4<T> v0( 1,  5,  9, 13 );
		Vec4<T> v1( 2,  6, 10, 14 );
		Vec4<T> v2( 3,  7, 11, 15 );
		Vec4<T> v3( 4,  8, 12, 16 );

		m0.setRows( v0, v1, v2, v3 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void setRows( const Vec4<T> &r0, const Vec4<T> &r1, const Vec4<T> &r2, const Vec4<T> &r3 );" << "\n";
	}


	// Matrix22<T> subMatrix22( int row, int col ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16, true );

		Matrix22<T> m22_00(  1,  2,
			                 5,  6, true );

		Matrix22<T> m22_01(  2,  3,
			                 6,  7, true );

		Matrix22<T> m22_02(  3,  4,
			                 7,  8, true );

		Matrix22<T> m22_10(  5,  6,
			                 9, 10, true );

		Matrix22<T> m22_11(  6,  7,
			                10, 11, true );

		Matrix22<T> m22_12(  7,  8,
			                11, 12, true );

		Matrix22<T> m22_20(  9, 10,
			                13, 14, true );

		Matrix22<T> m22_21( 10, 11,
			                14, 15, true );

		Matrix22<T> m22_22( 11, 12,
			                15, 16, true );

		result = ( m22_00 == c0.subMatrix22( 0, 0 ) ) &&
		         ( m22_01 == c0.subMatrix22( 0, 1 ) ) &&
		         ( m22_02 == c0.subMatrix22( 0, 2 ) ) &&
				 ( m22_10 == c0.subMatrix22( 1, 0 ) ) &&
				 ( m22_11 == c0.subMatrix22( 1, 1 ) ) &&
				 ( m22_12 == c0.subMatrix22( 1, 2 ) ) &&
				 ( m22_20 == c0.subMatrix22( 2, 0 ) ) &&
				 ( m22_21 == c0.subMatrix22( 2, 1 ) ) &&
				 ( m22_22 == c0.subMatrix22( 2, 2 ) );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T> subMatrix22( int row, int col ) const;" << "\n";
	}


	// Matrix33<T> subMatrix33( int row, int col ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16, true );

		Matrix33<T> m33_00(  1,  2,  3,
			                 5,  6,  7,
							 9, 10, 11, true );

		Matrix33<T> m33_01(  2,  3,  4,
			                 6,  7,  8,
							10, 11, 12, true );

		Matrix33<T> m33_10(  5,  6,  7,
			                 9, 10, 11,
							13, 14, 15, true );

		Matrix33<T> m33_11(  6,  7,  8,
			                10, 11, 12,
							14, 15, 16, true );


		result = ( m33_00 == c0.subMatrix33( 0, 0 ) ) &&
		         ( m33_01 == c0.subMatrix33( 0, 1 ) ) &&
				 ( m33_10 == c0.subMatrix33( 1, 0 ) ) &&
				 ( m33_11 == c0.subMatrix33( 1, 1 ) );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T> subMatrix33( int row, int col ) const;" << "\n";
	}


	// void setToNull();
	{
		bool result = false;

		MatT c0( 0, 0, 0, 0,
			     0, 0, 0, 0,
				 0, 0, 0, 0,
				 0, 0, 0, 0 );

		MatT m0;
		m0.setToNull();

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void setToNull();" << "\n";
	}


	// void setToIdentity();
	{
		bool result = false;

		MatT c0( 1, 0, 0, 0,
			     0, 1, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1 );

		MatT m0;
		m0.setToIdentity();

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void setToIdentity();" << "\n";
	}


	// T determinant() const;
	{
		bool result = false;

		MatT m0(
			(T) 0.009505, (T)0.848196, (T)-0.529596, (T)-10.070000,
			(T)-0.999570, (T)0.022748, (T) 0.018494, (T) -3.980000,
			(T) 0.027734, (T)0.529193, (T) 0.848048, (T) 15.230000,
			(T) 1.002352, (T)0.875840, (T) 0.133293, (T)  1.200000
		);

		T det = m0.determinant();

		T diff = fabs( (T)-5.036502 - det );

		result = ( diff <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "T determinant() const;" << "\n";
	}


	// T trace() const;
	{
		bool result = false;

		MatT m0(
			(T) 0.009505, (T)0.848196, (T)-0.529596, (T)-10.070000,
			(T)-0.999570, (T)0.022748, (T) 0.018494, (T) -3.980000,
			(T) 0.027734, (T)0.529193, (T) 0.848048, (T) 15.230000,
			(T) 1.002352, (T)0.875840, (T) 0.133293, (T)  1.200000
		);

		T cs0 = (T)0.009505 + (T)0.022748 + (T)0.848048;
		T cs1 = (T)0.009505 + (T)0.022748 + (T)0.848048 + (T)1.200000;

		T trace0 = m0.trace();
		T trace1 = m0.trace( true );

		float v0 = (T)0.009505 + (T)0.022748 + (T)0.848048;
		float v1 = (T)0.009505 + (T)0.022748 + (T)0.848048 + (T)1.200000;

		T diff0 = fabs( (T)v0 - trace0 );
		T diff1 = fabs( (T)v1 - trace1 );

		result = ( diff0 <= EPSILON ) && ( diff1 <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "T trace() const;" << "\n";
	}


	// Matrix44<T> diagonal() const;
	{
		bool result = false;

		MatT c0(  1,  0,  0,  0,
			      0,  6,  0,  0,
				  0,  0, 11,  0,
				  0,  0, 0, 16 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		result = ( c0 == m0.diagonal() );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T> diagonal() const;" << "\n";
	}


	// Matrix44<T> lowerTriangular() const;
	{
		bool result = false;

		MatT c0(  1,  0,  0,  0,
			      5,  6,  0,  0,
				  9, 10, 11,  0,
				 13, 14, 15, 16, true );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16, true );

		result = ( c0 == m0.lowerTriangular() );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T> lowerTriangular() const;" << "\n";
	}


	// Matrix44<T> upperTriangular() const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      0,  6,  7,  8,
				  0,  0, 11, 12,
				  0,  0,  0, 16, true );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16, true );

		result = ( c0 == m0.upperTriangular() );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T> upperTriangular() const;" << "\n";
	}


	// void transpose();
	{
		bool result = false;

		MatT c0(  1,  5,  9, 13,
			      2,  6, 10, 14,
				  3,  7, 11, 15,
				  4,  8, 12, 16 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16  );
		
		m0.transpose();

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void transpose();" << "\n";
	}


	// Matrix44<T> transposed() const;
	{
		bool result = false;

		MatT c0(  1,  5,  9, 13,
			      2,  6, 10, 14,
				  3,  7, 11, 15,
				  4,  8, 12, 16 );

		MatT m0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16  );

		result = ( c0 == m0.transposed() );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T> transposed() const;" << "\n";
	}


	// void invert (T epsilon = FLT_MIN );
	{
		bool result = false;

		MatT c0(
			(T)0.000000105208,  (T)0.000000030826,    (T)0.000161465821,     (T)-0.000000102688,
			(T)-0.000000157642, (T)-0.345155377696,   (T)-1808.660168116690, (T)1.150725860676,
			(T)0.000000000049,  (T)0.000454767453,    (T)1.197733696053,     (T)-0.000011811630,
			(T)0.000000000000,  (T)-0.000000025323,   (T)-0.000001033201,    (T)0.000000000658
		);

		MatT m0(
			(T)0.009505e9,  (T)0.848196,     (T)-0.529596,  (T)-10.070000,
			(T)-0.999570,   (T)0.022748,     (T)0.018494,   (T)-3.980000e7,
			(T)0.027734e-6, (T)0.529193e-11, (T)0.848048,   (T)15.230000e3,
			(T)1.002352,    (T)0.875840,     (T)0.133293e4, (T)1.200000e7
		);

		m0.invert();

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void invert (T epsilon = FLT_MIN );" << "\n";
	}


	// Matrix44<T> inverted( T epsilon = FLT_MIN ) const;
	{
		bool result = false;

		MatT c0(
			(T)0.000000105208,  (T)0.000000030826,    (T)0.000161465821,     (T)-0.000000102688,
			(T)-0.000000157642, (T)-0.345155377696,   (T)-1808.660168116690, (T)1.150725860676,
			(T)0.000000000049,  (T)0.000454767453,    (T)1.197733696053,     (T)-0.000011811630,
			(T)0.000000000000,  (T)-0.000000025323,   (T)-0.000001033201,    (T)0.000000000658
		);

		MatT m0(
			(T)0.009505e9,  (T)0.848196,     (T)-0.529596,  (T)-10.070000,
			(T)-0.999570,   (T)0.022748,     (T)0.018494,   (T)-3.980000e7,
			(T)0.027734e-6, (T)0.529193e-11, (T)0.848048,   (T)15.230000e3,
			(T)1.002352,    (T)0.875840,     (T)0.133293e4, (T)1.200000e7
		);

		bool single = ( c0 == m0.inverted() );

		bool multi = true;
		Vec3<T> da( (T)0.015, (T)0.034, (T)-0.025 );
		Vec3<T> eu( 0, 0, 0 );

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

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T> inverted( T epsilon = FLT_MIN ) const;" << "\n";

		// Invert comparison test
		iter = 10000;
		int notWithinEpison = 0;
		for( int i = 0; i < iter; ++i ) {
			float angle = (float)i/(float)iter * 360.0f;
			mat4 mat = mat4::createRotation( vec3( angle, angle*0.5f, angle + 0.25f ) );
			CINDER_ALIGN16_MATRIX44F( align_mat ) = mat4::createRotation( vec3( angle, angle*0.5f, angle + 0.25f ) );

			mat.setTranslate( vec3( 1, 2, 3 ) );
			align_mat.setTranslate( vec3( 1, 2, 3 ) );

			mat4 inv_mat = mat.inverted();
			CINDER_ALIGN16_MATRIX44F( inv_align_mat ) = SseInvert( align_mat );

			if( ! inv_mat.equalCompare( inv_align_mat, (float)EPSILON ) ) {
				os << "   " << "angle " << angle << " is not within " << EPSILON << " for invert comparison" << "\n";
				std::cout << mat << "\n" << align_mat << "\n//\n";
				++notWithinEpison;
			}
		}
		os << "   " << "comparison of inverted and MatrixAlgo::SseInvert test: " << notWithinEpison << " angles not within EPSILON";
		if( notWithinEpison > 0 )
			os << " - check source";
		os << "\n";

		// Invert
		os << "   " << "speed test:" << "\n";
		iter = 10000000;
		{
			MatT b; 
			Timer timer;
			timer.start();
			for( int i = 0; i < iter; ++i ) {
				b += m0.inverted();
			}
			timer.stop();
			os << "   " << "   " << iter << " inverted() calls took " << timer.getSeconds() << " seconds" << "\n";
		}
		{
			CINDER_ALIGN16_MATRIX44F( align_m0 ) = m0;
			CINDER_ALIGN16_MATRIX44F( b ); 
			Timer timer;
			timer.start();
			for( int i = 0; i < iter; ++i ) {
				b += SseInvert( align_m0 );
			}
			timer.stop();
			os << "   " << "   " << iter << " MatrixAlgo::SseInvert() calls took " << timer.getSeconds() << " seconds" << "\n";
		}
	}


	// Vec3<T> preMultiply( const Vec3<T> &v ) const;
	{
		bool result = false;

		Vec3<T> cv0( 1, 2, 3 );

		MatT c0( 1, 0, 0, 0,
			     0, 1, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1 );

		Vec3<T> diff = cv0 - c0.preMultiply( cv0 );

		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );

		result = ( diff.x <= EPSILON ) && 
			     ( diff.y <= EPSILON ) && 
				 ( diff.z <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "Vec3<T> preMultiply( const Vec3<T> &v ) const;" << "\n";
	}


	// Vec4<T> preMultiply( const Vec4<T> &v ) const;
	{
		bool result = false;

		Vec4<T> cv0( 1, 2, 3, 4 );

		MatT c0( 1, 0, 0, 0,
			     0, 1, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1 );

		Vec4<T> diff = cv0 - c0.preMultiply( cv0 );

		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );
		diff.w = fabs( diff.w );

		result = ( diff.x <= EPSILON ) && 
			     ( diff.y <= EPSILON ) && 
				 ( diff.z <= EPSILON ) && 
				 ( diff.w <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "Vec4<T> preMultiply( const Vec4<T> &v ) const;" << "\n";
	}


	// Vec3<T> postMultiply( const Vec3<T> &v ) const;
	{
		bool result = false;

		Vec3<T> cv0( 1, 2, 3 );

		MatT c0( 1, 0, 0, 0,
			     0, 1, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1 );

		Vec3<T> diff = cv0 - c0.postMultiply( cv0 );

		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );

		result = ( diff.x <= EPSILON ) && 
			     ( diff.y <= EPSILON ) && 
				 ( diff.z <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "Vec3<T> postMultiply( const Vec3<T> &v ) const;" << "\n";
	}


	// Vec4<T> postMultiply( const Vec4<T> &v ) const;
	{
		bool result = false;

		Vec4<T> cv0( 1, 2, 3, 4 );

		MatT c0( 1, 0, 0, 0,
			     0, 1, 0, 0,
				 0, 0, 1, 0,
				 0, 0, 0, 1 );

		Vec4<T> diff = cv0 - c0.postMultiply( cv0 );

		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );
		diff.w = fabs( diff.w );

		result = ( diff.x <= EPSILON ) && 
			     ( diff.y <= EPSILON ) && 
				 ( diff.z <= EPSILON ) &&
				 ( diff.w <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "Vec4<T> postMultiply( const Vec4<T> &v ) const;" << "\n";
	}


	// void affineInvert(){ *this = affineInverted(); } 
	{
		bool result = false;
		os << "notest" << " : " << "void affineInvert(); " << "\n";
		//os << (result ? "passed" : "FAILED") << " : " << "void affineInvert(); " << "\n";
	}


	// Matrix44<T> affineInverted() const;
	{
		bool result = false;
		os << "notest" << " : " << "void affineInverted(); " << "\n";
		//os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T> affineInverted() const;" << "\n";
	}


	// Vec3<T> transformPoint( const Vec3<T> &rhs ) const;
	{
		bool result = false;

		Vec3<T> cv0(
			(T)-17.780877,
			(T) 21.685958, 
			(T) 50.122100
		);

		MatT c0(
			(T) 0.45399, (T)-0.89101, (T) 0.00000, (T)-22.1904,
			(T) 0.89101, (T) 0.45399, (T) 0.00000, (T) 22.5298,
			(T) 0.00000, (T) 0.00000, (T) 1.00000, (T) 45.0000,
			(T) 0.00000, (T) 0.00000, (T) 0.00000, (T)  1.0000, true
		);

		Vec3<T> p0( (T)1.25, (T)-4.312, (T)5.2112 );
		Vec3<T> diff = (cv0 - c0.transformPoint( p0 ));

		result = ( diff.x < EPSILON ) && 
			     ( diff.y < EPSILON ) &&
				 ( diff.z < EPSILON );
		
		os << (result ? "passed" : "FAILED") << " : " << "Vec3<T> transformPoint( const Vec3<T> &rhs ) const;" << "\n";
	}


	// Vec3<T> transformPointAffine( const Vec3<T> &rhs ) const;
	{
		bool result = false;
		os << "notest" << " : " << "Vec3<T> transformPointAffine( const Vec3<T> &rhs ) const;" << "\n";
		//os << (result ? "passed" : "FAILED") << " : " << "Vec3<T> transformPointAffine( const Vec3<T> &rhs ) const;" << "\n";
	}

	// Vec4<T> transformVec( const Vec3/4<T> &rhs ) const;
	{
		bool result = false;

		MatT c0(
			(T) 0.45399, (T)-0.89101, (T) 0.00000, (T) 0.00000,
			(T) 0.89101, (T) 0.45399, (T) 0.00000, (T) 0.00000,
			(T) 0.00000, (T) 0.00000, (T) 1.00000, (T) 0.00000,
			(T) 0.00000, (T) 0.00000, (T) 0.00000, (T) 1.0000, true
		);

		MatT m0(
			(T) 0.45399, (T)-0.89101, (T) 0.00000, (T)-22.1904,
			(T) 0.89101, (T) 0.45399, (T) 0.00000, (T) 22.5298,
			(T) 0.00000, (T) 0.00000, (T) 1.00000, (T) 45.0000,
			(T) 0.00000, (T) 0.00000, (T) 0.00000, (T)  1.0000, true
		);

		Vec3<T> v0( (T)1.25, (T)-4.312, (T)5.2112 );
		Vec4<T> v1( (T)1.25, (T)-4.312, (T)5.2112 );

		result = ( c0.transformVec( v0 ) == m0.transformVec( v0 ) ) &&
			     ( c0.transformVec( v1 ) == m0.transformVec( v1 ) );

		os << (result ? "passed" : "FAILED") << " : " << "Vec4<T> transformVec( const Vec4<T> &rhs ) const;" << "\n";
	}


	// Vec4<T> getTranslation() const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,  4,
			      5,  6,  7,  8,
				  9, 10, 11, 12,
				 13, 14, 15, 16 );

		Vec4<T> v = c0.getTranslate();

		Vec4<T> diff = ( v - Vec4<T>( 13, 14, 15, 16 ) );

		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );
		diff.w = fabs( diff.w );

		result = ( diff.x <= EPSILON ) && 
			     ( diff.y <= EPSILON ) && 
				 ( diff.z <= EPSILON ) &&
				 ( diff.w <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "Vec4<T> getTranslation() const;" << "\n";
	}


	// void translate( const Vec3/4<T> &tr );
	{
		bool result = false;

		MatT c0(
			(T) 0.45399, (T)-0.89101, (T) 0.00000, (T)-22.1904,
			(T) 0.89101, (T) 0.45399, (T) 0.00000, (T) 22.5298,
			(T) 0.00000, (T) 0.00000, (T) 1.00000, (T) 45.0000,
			(T) 0.00000, (T) 0.00000, (T) 0.00000, (T)  1.0000, true
		);

		MatT m0(
			(T) 0.45399, (T)-0.89101, (T) 0.00000, (T) 0.00000,
			(T) 0.89101, (T) 0.45399, (T) 0.00000, (T) 0.00000,
			(T) 0.00000, (T) 0.00000, (T) 1.00000, (T) 0.00000,
			(T) 0.00000, (T) 0.00000, (T) 0.00000, (T) 1.00000, true
		);

		MatT m1(
			(T) 0.45399, (T)-0.89101, (T) 0.00000, (T) 0.00000,
			(T) 0.89101, (T) 0.45399, (T) 0.00000, (T) 0.00000,
			(T) 0.00000, (T) 0.00000, (T) 1.00000, (T) 0.00000,
			(T) 0.00000, (T) 0.00000, (T) 0.00000, (T) 1.00000, true
		);

		m0.translate( Vec3<T>( (T)10, (T)30, (T)45 ) );
		m1.translate( Vec4<T>( (T)10, (T)30, (T)45, (T)1.0 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void translate( const Vec3/4<T> &tr );" << "\n";
	}


	// void rotate( const Vec3/4<T> &axis, T radians );
	{
		bool result = false;

		MatT c0( (T) 1.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000,  
				 (T) 0.000000000000000, (T) 0.840963470480837, (T)-0.541091897293636, (T) 0.000000000000000,  
				 (T) 0.000000000000000, (T) 0.541091897293636, (T) 0.840963470480837, (T) 0.000000000000000,  
				 (T) 0.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000, (T) 1.000000000000000, true );

		MatT m0;
		m0.setToIdentity();
		m0.rotate( Vec3<T>( 1, 0, 0 ), toRadians( (T)32.758 ) );

		MatT m1;
		m1.setToIdentity();
		m1.rotate( Vec4<T>( 1, 0, 0 ), toRadians( (T)32.758 ) );

		result = ( c0 == m0 ) && ( c0 == m1 );

		os << (result ? "passed" : "FAILED") << " : " << "void rotate( const Vec3/4<T> &axis, T radians );" << "\n";
	}


	// void rotate( const Vec3/4<T> &eulerRadians );
	{
		bool result = false;

		MatT c0(
			(T) 0.091142486334637, (T) 0.428791685462663, (T)-0.898794046299167, (T)0,
			(T)-0.730490309010667, (T) 0.642199351624628, (T) 0.232301315567534, (T)0,
			(T) 0.676813826414798, (T) 0.635387821138395, (T) 0.371759816444386, (T)0,
			(T) 0.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000, (T)1
		);

		Vec3<T> eu0;
		eu0.x = toRadians( (T)32 );
		eu0.y = toRadians( (T)64 );
		eu0.z = toRadians( (T)78 );
		MatT m0;
		m0.setToIdentity();
		m0.rotate( eu0 );

		Vec4<T> eu1;
		eu1.x = toRadians( (T)32 );
		eu1.y = toRadians( (T)64 );
		eu1.z = toRadians( (T)78 );
		MatT m1;
		m1.setToIdentity();
		m1.rotate( eu1 );

		result = ( c0 == m0 ) && ( c0 == m1 );

		os << (result ? "passed" : "FAILED") << " : " << "void rotate( const Vec3/4<T> &eulerRadians );" << "\n";
		
	}


	// void rotate( const Vec3/4<T> &from, const Vec3/4<T> &to, const Vec3/4<T> &worldUp );
	{
		bool result = false;

		MatT c0( (T) 1.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000,  
			     (T) 0.000000000000000, (T) 0.840963470480837, (T)-0.541091897293636, (T) 0.000000000000000,  
				 (T) 0.000000000000000, (T) 0.541091897293636, (T) 0.840963470480837, (T) 0.000000000000000,  
				 (T) 0.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000, (T) 1.000000000000000, true );

		MatT m0;
		m0.setToIdentity();
		m0.rotate( Vec3<T>( 1, 0, 0 ), toRadians( (T)32.758 ) );

		MatT m1;
		m1.setToIdentity();
		m1.rotate( Vec4<T>( 1, 0, 0 ), toRadians( (T)32.758 ) );
		
		result = ( c0 == m0 ) && ( c0 == m1 );

		os << (result ? "passed" : "FAILED") << " : " << "void rotate( const Vec3/4<T> &from, const Vec3/4<T> &to, const Vec3/4<T> &worldUp );" << "\n";
	}


	// void scaleA( T s );
	{
		bool result = false;

		MatT c0( 5, 0, 0, 0, 
			     0, 5, 0, 0, 
				 0, 0, 5, 0, 
				 0, 0, 0, 5 );

		MatT m0( 1, 0, 0, 0, 
			     0, 1, 0, 0, 
				 0, 0, 1, 0, 
				 0, 0, 0, 1 );

		m0.scale( 5 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void scale( T s );" << "\n";
	}

	// void scale( const Vec2<T> &v );
	{
		bool result = false;

		MatT c0( 5, 0, 0, 0, 
			     0, 6, 0, 0, 
				 0, 0, 1, 0, 
				 0, 0, 0, 1 );

		MatT m0( 1, 0, 0, 0, 
			     0, 1, 0, 0, 
				 0, 0, 1, 0, 
				 0, 0, 0, 1 );

		m0.scale( Vec2<T>( 5, 6 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void scale( const Vec2<T> &v );" << "\n";
	}

	// void scale( const Vec3<T> &v );
	{
		bool result = false;

		MatT c0( 5, 0, 0, 0, 
			     0, 6, 0, 0, 
				 0, 0, 7, 0, 
				 0, 0, 0, 1 );

		MatT m0( 1, 0, 0, 0, 
			     0, 1, 0, 0, 
				 0, 0, 1, 0, 
				 0, 0, 0, 1 );

		m0.scale( Vec3<T>( 5, 6, 7 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void scale( const Vec3<T> &v );" << "\n";
	}


	// void scale( const Vec4<T> &v );
	{
		bool result = false;

		MatT c0( 5, 0, 0, 0, 
			     0, 6, 0, 0, 
				 0, 0, 7, 0, 
				 0, 0, 0, 8 );

		MatT m0( 1, 0, 0, 0, 
			     0, 1, 0, 0, 
				 0, 0, 1, 0, 
				 0, 0, 0, 1 );

		m0.scale( Vec4<T>( 5, 6, 7, 8 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void scale( const Vec4<T> &v );" << "\n";
	}


	// Matrix44<T> orthonormalInverted() const;
	{
		bool result = false;

		MatT c0(
			(T) 0.45399, (T) 0.89101, (T) 0.00000, (T) -10.000,
			(T)-0.89101, (T) 0.45399, (T) 0.00000, (T)-30.0000,
			(T) 0.00000, (T) 0.00000, (T) 1.00000, (T)-45.0000,
			(T) 0.00000, (T) 0.00000, (T) 0.00000, (T)  1.0000, true
		);

		MatT m0(
			(T) 0.45399, (T)-0.89101, (T) 0.00000, (T)-22.1904,
			(T) 0.89101, (T) 0.45399, (T) 0.00000, (T) 22.5298,
			(T) 0.00000, (T) 0.00000, (T) 1.00000, (T) 45.0000,
			(T) 0.00000, (T) 0.00000, (T) 0.00000, (T)  1.0000, true
		);

		result = ( c0.equalCompare( m0.orthonormalInverted(), 0.001f ) );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T> orthonormalInverted() const;" << "\n";
	}

	// Matrix44<T> orthonormalInverted() const;
	{
		bool result = false;

		MatT m0(
			(T) 0.45399, (T)-0.89101, (T) 0.00000, (T)-22.1904,
			(T) 0.89101, (T) 0.45399, (T) 0.00000, (T) 22.5298,
			(T) 0.00000, (T) 0.00000, (T) 1.00000, (T) 45.0000,
			(T) 0.00000, (T) 0.00000, (T) 0.00000, (T)  1.0000, true
		);

		result = ( m0.inverted().equalCompare( m0.orthonormalInverted(), 0.001f ) );
		os << (result ? "passed" : "FAILED") << " : " << "Matrix44<T> inverse() == orthonormalInverted() const;" << "\n";
	}

	// static Matrix44<T> identity();
	{
		bool result = false;

		MatT c0( 1, 0, 0, 0, 
			     0, 1, 0, 0, 
				 0, 0, 1, 0, 
				 0, 0, 0, 1 );

		result = ( c0 == MatT::identity() );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> identity();" << "\n";
	}


	// static Matrix44<T> one();
	{
		bool result = false;

		MatT c0( 1, 1, 1, 1, 
			     1, 1, 1, 1, 
				 1, 1, 1, 1, 
				 1, 1, 1, 1 );

		result = ( c0 == MatT::one() );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> one();" << "\n";
	}


	// static Matrix44<T> zero();
	{
		bool result = false;

		MatT c0( 0, 0, 0, 0,  
			     0, 0, 0, 0,  
				 0, 0, 0, 0,  
				 0, 0, 0, 0 );

		result = ( c0 == MatT::zero() );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> zero();" << "\n";
	}


	// static Matrix44<T> createTranslation( const Vec3<T> &v, T w = 1 );
	{
		bool result = false;

		MatT c0( 1, 0, 0, 0,  
			     0, 1, 0, 0,  
				 0, 0, 1, 0,  
				 1, 2, 3, 1 );

		result = ( c0 == MatT::createTranslation( Vec3<T>( 1, 2, 3 ) ) );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> createTranslation( const Vec3<T> &v, T w = 1 );" << "\n";
	}


	// static Matrix44<T> createTranslation( const Vec4<T> &v );
	{
		bool result = false;

		MatT c0( 1, 0, 0, 0,  
			     0, 1, 0, 0,  
				 0, 0, 1, 0,  
				 1, 2, 3, 1 );

		result = ( c0 == MatT::createTranslation( Vec4<T>( 1, 2, 3, 1 ) ) );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> createTranslation( const Vec4<T> &v );" << "\n";
	}


	// static Matrix44<T> createRotation( const Vec3/4<T> &axis, T radians );
	{
		bool result = false;

		MatT c0( (T) 1.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000,  
			     (T) 0.000000000000000, (T) 0.840963470480837, (T)-0.541091897293636, (T) 0.000000000000000,  
				 (T) 0.000000000000000, (T) 0.541091897293636, (T) 0.840963470480837, (T) 0.000000000000000,  
				 (T) 0.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000, (T) 1.000000000000000, true );

		MatT m0 = MatT::createRotation( Vec3<T>( 1, 0, 0 ), toRadians( (T)32.758 ) );
		MatT m1 = MatT::createRotation( Vec4<T>( 1, 0, 0 ), toRadians( (T)32.758 ) );
		
		result = ( c0 == m0 ) && ( c0 == m1 );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> createRotation( const Vec3/4<T> &axis, T radians );" << "\n";
	}


	// static Matrix44<T> createRotation( const Vec3/4<T> &from, const Vec3/4<T> &to, const Vec3/4<T> &worldUp );
	{
		bool result = false;

		MatT c0( (T) 0.707106781186548, (T) 0.000000000000000, (T)-0.707106781186548, (T) 0.000000000000000,  
			     (T) 0.000000000000000, (T) 1.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000,  
				 (T) 0.707106781186548, (T) 0.000000000000000, (T) 0.707106781186548, (T) 0.000000000000000,  
				 (T) 0.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000, (T) 1.000000000000000, true );

		MatT m0 = MatT::createRotation( 
						Vec3<T>( (T)0.00000000000000, (T)0.000000000000000, (T)1.00000000000000 ), 
						Vec3<T>( (T)0.70710678118654, (T)0.000000000000000, (T)0.70710678118654 ), 
						Vec3<T>( (T)0.00000000000000, (T)1.000000000000000, (T)0.00000000000000 ) );

		MatT m1 = MatT::createRotation( 
						Vec4<T>( (T)0.00000000000000, (T)0.000000000000000, (T)1.00000000000000 ), 
						Vec4<T>( (T)0.70710678118654, (T)0.000000000000000, (T)0.70710678118654 ), 
						Vec4<T>( (T)0.00000000000000, (T)1.000000000000000, (T)0.00000000000000 ) );

		result = ( c0 == m0 ) && ( c0 == m1 );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> createRotation( const Vec3/4<T> &from, const Vec3/4<T> &to, const Vec3/4<T> &worldUp );" << "\n";
	}


	// static Matrix44<T> createRotation( const Vec3/4<T> &eulerRadians );
	{
		bool result = false;

		MatT c0(
			(T) 0.091142486334637, (T) 0.428791685462663, (T)-0.898794046299167, (T)0,
			(T)-0.730490309010667, (T) 0.642199351624628, (T) 0.232301315567534, (T)0,
			(T) 0.676813826414798, (T) 0.635387821138395, (T) 0.371759816444386, (T)0,
			(T) 0.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000, (T)1
		);

		Vec3<T> eu0;
		eu0.x = toRadians( (T)32 );
		eu0.y = toRadians( (T)64 );
		eu0.z = toRadians( (T)78 );
		MatT m0 = MatT::createRotation( Vec3<T>( eu0 ) );

		Vec4<T> eu1;
		eu1.x = toRadians( (T)32 );
		eu1.y = toRadians( (T)64 );
		eu1.z = toRadians( (T)78 );
		MatT m1 = MatT::createRotation( Vec4<T>( eu1 ) );

		result = ( c0 == m0 ) && ( c0 == m1 );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> createRotation( const Vec3/4<T> &eulerRadians );" << "\n";
	}

	//static Matrix44<T> createRotationOnb( const Vec3<T>& u, const Vec3<T>& v, const Vec3<T>& w );
	{
		bool result = false;

		MatT c0(
			(T)1, (T)0, (T)0, (T)0,
			(T)0, (T)1, (T)0, (T)0,
			(T)0, (T)0, (T)1, (T)0,
			(T)0, (T)0, (T)0, (T)1
		);

		Vec3<T> u3 = Vec3<T>( 1, 0, 0 );
		Vec3<T> v3 = Vec3<T>( 0, 1, 0 );
		Vec3<T> w3 = Vec3<T>( 0, 0, 1 );
		MatT m0 = MatT::createRotationOnb( u3, v3, w3 );

		Vec3<T> u4 = Vec3<T>( 1, 0, 0 );
		Vec3<T> v4 = Vec3<T>( 0, 1, 0 );
		Vec3<T> w4 = Vec3<T>( 0, 0, 1 );
		MatT m1 = MatT::createRotationOnb( u4, v4, w4 );

		result = ( c0 == m0 ) && ( c0 == m1 );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> createRotationOnb( const Vec3/4<T>& u, const Vec3/4<T>& v, const Vec3/4<T>& w );" << "\n";
	}

	// static Matrix44<T> createScale( T s );
	{
		bool result = false;

		MatT c0( 6, 0, 0, 0,  
			     0, 6, 0, 0,  
				 0, 0, 6, 0,  
				 0, 0, 0, 6 );

		result = ( c0 == MatT::createScale( (T)6 ) );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> createScale( T s );" << "\n";
	}


	// static Matrix44<T> createScale( const Vec2<T> &v );
	{
		bool result = false;

		MatT c0( 2, 0, 0, 0,  
			     0, 3, 0, 0,  
				 0, 0, 1, 0,  
				 0, 0, 0, 1 );

		result = ( c0 == MatT::createScale( Vec2<T>( 2, 3 ) ) );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> createScale( const Vec2<T> &v );" << "\n";
	}


	// static Matrix44<T> createScale( const Vec3<T> &v );
	{
		bool result = false;

		MatT c0( 2, 0, 0, 0,  
			     0, 3, 0, 0,  
				 0, 0, 4, 0,  
				 0, 0, 0, 1 );

		result = ( c0 == MatT::createScale( Vec3<T>( 2, 3, 4 ) ) );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> createScale( const Vec3<T> &v );" << "\n";
	}


	// static Matrix44<T> createScale( const Vec4<T> &v );
	{
		bool result = false;

		MatT c0( 2, 0, 0, 0,  
			     0, 3, 0, 0,  
				 0, 0, 4, 0,  
				 0, 0, 0, 5 );

		result = ( c0 == MatT::createScale( Vec4<T>( 2, 3, 4, 5 ) ) );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> createScale( const Vec4<T> &v );" << "\n";
	}


	// static Matrix44<T> alignZAxisWithTarget( Vec3/4<T> targetDir, Vec3/4<T> upDir );
	{
		bool result = false;

		MatT c0(  1,  0,  0,  0,  
			      0,  1,  0,  0,  
				  0,  0, -1,  0,  
				  0,  0,  0,  1 );

		MatT m0 = MatT::alignZAxisWithTarget( Vec3<T>( 0, 0, -1 ), Vec3<T>( 0, 1, 0 ) );
		MatT m1 = MatT::alignZAxisWithTarget( Vec4<T>( 0, 0, -1 ), Vec4<T>( 0, 1, 0 ) );

		result = ( c0 == m0 ) && ( c0 == m1 );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix44<T> alignZAxisWithTarget( Vec3/4<T> targetDir, Vec3/4<T> upDir );" << "\n";
	}


	{
		/*
		os << "\n";

		MatT m0;

		m0.scale( Vec3<T>( 1, 2, 3 ) );
		m0.translate( Vec3<T>( 10, 30, 45 ) );		
		os << "\n" << m0;

		m0.rotate( Vec3<T>( 0, 0, toRadians( (T)63 ) ) );
		os << "\n" << m0;

		m0.translate( Vec3<T>( 1, 2, 3 ) );		
		os << "\n" << m0;

		m0.rotate( Vec3<T>( 0, toRadians( (T)15 ), 0 ) );
		m0.scale( Vec3<T>( 1, 2, 3 ) );
		os << "\n" << m0;
		*/
	}
}
