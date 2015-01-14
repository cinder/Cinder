#pragma once

#include "cinder/Rand.h"

template <typename T> void TestMatrix33( std::ostream& os )
{
	typedef Matrix33<T> MatT;

	// Matrix33();
	{
		bool result = false;

		MatT c0( 1, 0, 0, 
			     0, 1, 0, 
				 0, 0, 1  );
		MatT m0;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33();" << "\n";
	}


	// Matrix33( T s );
	{
		bool result = false;

		MatT c0( 3, 3, 3,
			     3, 3, 3,
				 3, 3, 3  );

		MatT m0( 3 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33( T s );" << "\n";
	}


	// Matrix33( const T *dt );
	{
		bool result = false;

		MatT c0( 1, 2, 3, 
			     4, 5, 6, 
				 7, 8, 9  );

		T data[9] = 
		{
			(T)1, (T)2, (T)3,
			(T)4, (T)5, (T)6,
		    (T)7, (T)8, (T)9			
		};

		MatT m0( data );

		result = ( c0 == m0 );	

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33( const T *dt );" << "\n";
	}


	// Matrix33( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8 );
	{
		bool result = false;

		MatT c0;
		c0.m00 = 1; c0.m01 = 4; c0.m02 = 7;
		c0.m10 = 2; c0.m11 = 5; c0.m12 = 8;
		c0.m20 = 3; c0.m21 = 6; c0.m22 = 9;

		MatT m0( 1, 2, 3,
			     4, 5, 6,
				 7, 8, 9  );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33( T d0, T d1, T d2, T d3, T d4, T d5, T d6, T d7, T d8 );" << "\n";
	}


	// Matrix33( const Vec3<T> &vx, const Vec3<T> &vy, const Vec3<T> &vz ); 
	{
		bool result = false;

		MatT c0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );

		MatT m0( Vec3<T>( 1, 2, 3 ), Vec3<T>( 5, 6, 7 ), Vec3<T>( 9, 10, 11 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33( const Vec3<T> &vx, const Vec3<T> &vy, const Vec3<T> &vz ); " << "\n";
	}


	// template< typename FromT > Matrix33( const Matrix33<FromT>& src );
	{
		bool result = false;

		Matrix33<double> dmat0(
			1, 2, 3,
			4, 5, 6,
			7, 8, 9 
		);

		Matrix33<float> fmat0(
			1, 2, 3,
			4, 5, 6,
			7, 8, 9 
		);

		Matrix33<double> dmat1( fmat0 );
		Matrix33<double> fmat1( dmat0 );
		
		result = ( dmat0 == dmat1 ) && ( fmat0 == fmat1 );

		os << (result ? "passed" : "FAILED") << " : " << "template< typename FromT > Matrix33( const Matrix33<FromT>& src );" << "\n";
	}

	// Matrix33( const Matrix22<T>& src );
	{
		bool result = false;

		MatT c0( 1, 2, 0,
			     3, 4, 0,
				 0, 0, 1 );

		Matrix22<T> mat22( 1, 2, 3, 4 );

		MatT m0( mat22 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33( const Matrix22<T>& src );" << "\n";
	}

	// Matrix33( const Matrix33<T>& src );
	{
		bool result = false;

		MatT c0( 1, 2, 3, 
			     4, 5, 6, 
				 7, 8, 9 );

		Matrix33<T> mat33( 
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);

		MatT m0( mat33 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33( const Matrix33<T>& src );" << "\n";
	}

	// operator T*() { return (T*)m; }
	{
		bool result = false;

		MatT c0(  2,  4,  6,
			     10, 12, 14,
				 18, 20, 22 );

		MatT m0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );

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
			(T) 1, (T) 2, (T) 3,
			(T) 5, (T) 6, (T) 7,
			(T) 9, (T)10, (T)11
		};

		MatT m0(  1,  2,  3,
				  5,  6,  7,
			      9, 10, 11 );

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


// Matrix33<T>& operator=( const Matrix33<T>& rhs );
	{
		bool result = false;

		MatT c0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );

		MatT m0 = c0;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T>& operator=( const Matrix33<T>& rhs );" << "\n";
	}


	// Matrix33<T>& operator=( T rhs );
	{
		bool result = false;

		MatT c0( 3, 3, 3,
			     3, 3, 3,
				 3, 3, 3 );

		MatT m0 = 3;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T>& operator=( T rhs );" << "\n";
	}


	// template< typename FromT > Matrix33<T>& operator=( const Matrix33<FromT>& rhs );
	{
		bool result = false;

		Matrix33<double> dmat0(
			 1,  2,  3,
			 5,  6,  7,
			 9, 10, 11
		);

		Matrix33<float> fmat0(
			 1,  2,  3,
			 5,  6,  7,
			 9, 10, 11
		);

		Matrix33<double> dmat1 = fmat0;
		Matrix33<double> fmat1 = dmat0;
		
		result = ( dmat0 == dmat1 ) && ( fmat0 == fmat1 );
		
		os << (result ? "passed" : "FAILED") << " : " << "template< typename FromT > Matrix33<T>& operator=( const Matrix33<FromT>& rhs );" << "\n";
	}


	// Matrix33<T>& operator=( const Matrix22<T>& rhs );
	{
		bool result = false;

		MatT c0( 1, 2, 0, 
			     3, 4, 0, 
				 0, 0, 1 );

		Matrix22<T> mat22( 1, 2, 3, 4 );

		MatT m0;
		m0 = mat22;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T>& operator=( const Matrix22<T>& rhs );" << "\n";
	}


	// Matrix33<T>& operator=( const Matrix33<T>& rhs );
	{
		bool result = false;

		MatT c0( 1, 2, 3,
			     4, 5, 6,
				 7, 8, 9  );

		Matrix33<T> mat33( 
			1, 2, 3,
			4, 5, 6,
			7, 8, 9
		);

		MatT m0;
		m0 = mat33;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T>& operator=( const Matrix33<T>& rhs );" << "\n";
	}


	// bool equalCompare( const Matrix33<T>& rhs, T epsilon ) const;
	{
		bool result = false;

		T epsilon = (T)1.0e-04;

		MatT c0( (T)1.0e-04, (T)1.0e-04, (T)1.0e-04, 
			     (T)1.0e-04, (T)1.0e-04, (T)1.0e-04, 
				 (T)1.0e-04, (T)1.0e-04, (T)1.0e-04 );

		MatT m0( (T)3.0e-04, (T)3.0e-04, (T)3.0e-04,
			     (T)3.0e-04, (T)3.0e-04, (T)3.0e-04,
				 (T)3.0e-04, (T)3.0e-04, (T)3.0e-04 );

		MatT m1( (T)1.5e-04, (T)1.5e-04, (T)1.5e-04,
			     (T)1.5e-04, (T)1.5e-04, (T)1.5e-04,
				 (T)1.5e-04, (T)1.5e-04, (T)1.5e-04 );

	
		result = ( false == c0.equalCompare( m0, epsilon ) ) && ( true == c0.equalCompare( m1, epsilon ) );

		os << (result ? "passed" : "FAILED") << " : " << "bool equalCompare( const Matrix33<T>& rhs, T epsilon ) const;" << "\n";
	}


	// bool operator==( const Matrix33<T> &rhs );
	{
		bool result = false;

		MatT c0( 1, 2, 3,
			     4, 5, 6,
				 7, 8, (T)1.234e-06 );

		MatT m0( 1, 2, 3,
			     4, 5, 6,
				 7, 8, (T)1.235e-06 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "bool operator==( const Matrix33<T> &rhs );" << "\n";
	}


	// bool operator!=( const Matrix33<T> &rhs );
	{
		bool result = false;

		MatT c0( 1, 2, 3,
			     4, 5, 6,
				 7, 8, (T)1.234e-06 );

		MatT m0( 2, 2, 3,
			     4, 5, 6,
				 7, 8, (T)1.235e-06  );

		result = ( c0 != m0 );

		os << (result ? "passed" : "FAILED") << " : " << "bool operator!=( const Matrix33<T> &rhs );" << "\n";
	}


	// Matrix33<T>& operator*=( const Matrix33<T> &rhs );
	{
		bool result = false;
		
		MatT c0(
			20, 24, 28,
			35, 42, 49,
			65, 78, 91
		);

		MatT m0(
			 1,  2,  3,
			 5,  6,  7,
			 9, 10, 11
		);

		MatT m1(
			1, 2, 1,
			2, 3, 2,
			4, 5, 4
		);

		m0 *= m1;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T>& operator*=( const Matrix33<T> &rhs );" << "\n";
	}


	// Matrix33<T>& operator+=( const Matrix33<T> &rhs );
	{
		bool result = false;

		MatT c0(  2,  4,  6,
			     10, 12, 14,
				 18, 20, 22  );

		MatT m0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11  );

		MatT m1(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11  );
		
		m0 += m1;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T>& operator+=( const Matrix33<T> &rhs );" << "\n";
	}


	// Matrix33<T>& operator-=( const Matrix33<T> &rhs );
	{
		bool result = false;

		MatT c0( 0, 0, 0,
			     0, 0, 0,
				 0, 0, 0 );

		MatT m0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11  );

		MatT m1(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );
		
		m0 -= m1;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T>& operator-=( const Matrix33<T> &rhs );" << "\n";
	}


	// Matrix33<T>& operator*=( T rhs );
	{
		bool result = false;

		MatT c0(  2,  4,  6,
			     10, 12, 14,
				 18, 20, 22 );

		MatT m0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11 );

		m0 *= 2;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T>& operator*=( T rhs );" << "\n";
	}


	// Matrix33<T>& operator/=( T rhs );
	{
		bool result = false;

		MatT c0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11 );

		MatT m0(  2,  4,  6,
			     10, 12, 14,
				 18, 20, 22 );

		m0 /= 2;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T>& operator/=( T rhs );" << "\n";
	}


	// Matrix33<T>& operator+=( T rhs );
	{
		bool result = false;

		MatT c0( (T) 1.025, (T) 2.025, (T) 3.025, 
			     (T) 5.025, (T) 6.025, (T) 7.025, 
				 (T) 9.025, (T)10.025, (T)11.025 );

		MatT m0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11 );

		m0 += (T)0.025;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T>& operator+=( T rhs );" << "\n";
	}


	// Matrix33<T>& operator-=( T rhs );
	{
		bool result = false;

		MatT c0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11 );

		MatT m0( (T) 1.025, (T) 2.025, (T) 3.025,
			     (T) 5.025, (T) 6.025, (T) 7.025,
				 (T) 9.025, (T)10.025, (T)11.025 );


		m0 -= (T)0.025;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T>& operator-=( T rhs );" << "\n";
	}


	// const Matrix33<T> operator*( const Matrix33<T> &rhs ) const;
	{
		bool result = false;
		
		MatT c0(
			20, 24, 28,
			35, 42, 49,
			65, 78, 91
			);

		MatT m0(
			1,  2,  3,
			5,  6,  7,
			9, 10, 11
			);

		MatT m1(
			1, 2, 1,
			2, 3, 2,
			4, 5, 4
			);

		result = ( c0 == ( m0*m1 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix33<T> operator*( const Matrix33<T> &rhs ) const;" << "\n";
	}


	// const Matrix33<T> operator+( const Matrix33<T> &rhs ) const;
	{
		bool result = false;

		MatT c0(  2,  4,  6, 
			     10, 12, 14, 
				 18, 20, 22 );

		MatT m0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );

		MatT m1(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );
		
		result = ( c0 == ( m0 + m1 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix33<T> operator+( const Matrix33<T> &rhs ) const;" << "\n";
	}


	// const Matrix33<T> operator-( const Matrix33<T> &rhs ) const;
	{
		bool result = false;

		MatT c0( 0, 0, 0,
			     0, 0, 0,
				 0, 0, 0 );

		MatT m0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );

		MatT m1(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11 );
		
		result = ( c0 == ( m0 - m1 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix33<T> operator-( const Matrix33<T> &rhs ) const;" << "\n";
	}


	// const Vec3<T> operator*( const Vec3<T> &rhs ) const;
	{
		bool result = false;

		Vec3<T> cv0( 1, 2, 3 );

		MatT c0( 1, 0, 0, 
			     0, 1, 0, 
				 0, 0, 1 );

		Vec3<T> diff = (cv0 - c0*cv0 );
		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );

		result = ( diff.x <= EPSILON ) && ( diff.y <= EPSILON ) && ( diff.z <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "const Vec3<T> operator*( const Vec3<T> &rhs ) const;" << "\n";
	}


	// const Vec3<T> operator*( const Vec3<T> &rhs ) const;
	{
		bool result = false;

		Vec3<T> cv0( 1, 2, 3 );

		MatT c0( 1, 0, 0,
			     0, 1, 0,
				 0, 0, 1 );

		Vec3<T> diff = (cv0 - c0*cv0 );
		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );

		result = ( diff.x <= EPSILON ) && 
			     ( diff.y <= EPSILON ) && 
				 ( diff.z <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "const Vec3<T> operator*( const Vec3<T> &rhs ) const;" << "\n";
	}


	// const Matrix33<T> operator*( T rhs ) const;
	{
		bool result = false;

		MatT c0(  2,  4,  6, 
			     10, 12, 14, 
				 18, 20, 22 );

		MatT m0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11 );

		result = ( c0 == ( m0*(T)2 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix33<T> operator*( T rhs ) const;" << "\n";
	}


	// const Matrix33<T> operator/( T rhs ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11 );

		MatT m0(  2,  4,  6,
			     10, 12, 14,
				 18, 20, 22 );

		result = ( c0 == ( m0/(T)2 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix33<T> operator/( T rhs ) const;" << "\n";
	}


	// const Matrix33<T> operator+( T rhs ) const;
	{
		bool result = false;

		MatT c0( (T) 1.025, (T) 2.025, (T) 3.025, 
			     (T) 5.025, (T) 6.025, (T) 7.025, 
				 (T) 9.025, (T)10.025, (T)11.025 );

		MatT m0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11 );

		result = ( c0 == ( m0 + (T)0.025 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix33<T> operator+( T rhs ) const;" << "\n";
	}


	// const Matrix33<T> operator-( T rhs ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );

		MatT m0( (T) 1.025, (T) 2.025, (T) 3.025, 
			     (T) 5.025, (T) 6.025, (T) 7.025, 
				 (T) 9.025, (T)10.025, (T)11.025  );


		result = ( c0 == ( m0 - (T)0.025 ) );

		os << (result ? "passed" : "FAILED") << " : " << "const Matrix33<T> operator-( T rhs ) const;" << "\n";
	}


	// T& at( int row, int col );
	{
		bool result = false;

		MatT c0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11  );

		MatT m0;
		m0.at( 0, 0 ) =  1;
		m0.at( 1, 0 ) =  2;
		m0.at( 2, 0 ) =  3;

		m0.at( 0, 1 ) =  5;
		m0.at( 1, 1 ) =  6;
		m0.at( 2, 1 ) =  7;

		m0.at( 0, 2 ) =  9;
		m0.at( 1, 2 ) = 10;
		m0.at( 2, 2 ) = 11;

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "T& at( int row, int col );" << "\n";
	}


	// const T& at( int row, int col ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );

		MatT m0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );

		bool isOk = true;
		for( int r = 0; r < 3; ++r ) {
			for( int c = 0; c < 3; ++c ) {
				if( c0.at( r, c ) != m0.at( r, c ) ) {
					isOk = false;
					break;
				}
			}
		}

		result = isOk;

		os << (result ? "passed" : "FAILED") << " : " << "const T& at( int row, int col ) const;" << "\n";
	}


	// Vec3<T> getColumn( int col ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );

		result = ( c0.getColumn( 0 ) == Vec3<T>(  1,  2,  3 ) ) &&
				 ( c0.getColumn( 1 ) == Vec3<T>(  5,  6,  7 ) ) &&
				 ( c0.getColumn( 2 ) == Vec3<T>(  9, 10, 11 ) );
		
		os << (result ? "passed" : "FAILED") << " : " << "Vec3<T> getColumn( int col ) const;" << "\n";
	}


	// void setColumn( int col, const Vec3<T> &v );
	{
		bool result = false;

		MatT c0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11 );
		MatT m0;
		m0.setToIdentity();

		m0.setColumn( 0, Vec3<T>(  1,  2,  3 ) );
		m0.setColumn( 1, Vec3<T>(  5,  6,  7 ) );
		m0.setColumn( 2, Vec3<T>(  9, 10, 11 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void setColumn( int col, const Vec3<T> &v );" << "\n";
	}


	// Vec3<T> getRow( int row ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );

		result = ( c0.getRow( 0 ) == Vec3<T>(  1,  5,  9 ) ) &&
				 ( c0.getRow( 1 ) == Vec3<T>(  2,  6, 10 ) ) &&
				 ( c0.getRow( 2 ) == Vec3<T>(  3,  7, 11 ) );

		os << (result ? "passed" : "FAILED") << " : " << "Vec3<T> getRow( int row ) const;" << "\n";
	}


	// void setRow( int row, const Vec3<T> &v );
	{
		bool result = false;

		MatT c0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11 );
		MatT m0;
		m0.setToIdentity();

		m0.setRow( 0, Vec3<T>( 1,  5,  9 ) );
		m0.setRow( 1, Vec3<T>( 2,  6, 10 ) );
		m0.setRow( 2, Vec3<T>( 3,  7, 11 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void setRow( int row, const Vec3<T> &v );" << "\n";
	}


	// void getColumns( Vec3<T> *c0, Vec3<T> *c1, Vec3<T> *c2 ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );

		Vec3<T> v0, v1, v2, v3;
		c0.getColumns( &v0, &v1, &v2 );

		result = ( v0 == Vec3<T>(  1,  2,  3 ) ) &&
				 ( v1 == Vec3<T>(  5,  6,  7 ) ) &&
				 ( v2 == Vec3<T>(  9, 10, 11 ) );

		os << (result ? "passed" : "FAILED") << " : " << "void getColumns( Vec3<T> *c0, Vec3<T> *c1, Vec3<T> *c2 ) const;" << "\n";
	}


	// void setColumns( const Vec3<T> &c0, const Vec3<T> &c1, const Vec3<T> &c2 );
	{
		bool result = false;

		MatT c0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );
		MatT m0;
		m0.setToIdentity();

		Vec3<T> v0(  1,  2,  3 );
		Vec3<T> v1(  5,  6,  7 );
		Vec3<T> v2(  9, 10, 11 );

		m0.setColumns( v0, v1, v2 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void setColumns( const Vec3<T> &c0, const Vec3<T> &c1, const Vec3<T> &c2 );" << "\n";
	}


	// void getRows( Vec3<T> *r0, Vec3<T> *r1, Vec3<T> *r2 ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11 );

		Vec3<T> v0, v1, v2;
		c0.getRows( &v0, &v1, &v2 );

		result = ( v0 == Vec3<T>(  1,  5,  9 ) ) &&
				 ( v1 == Vec3<T>(  2,  6, 10 ) ) &&
				 ( v2 == Vec3<T>(  3,  7, 11 ) );

		os << (result ? "passed" : "FAILED") << " : " << "void getRows( Vec3<T> *r0, Vec3<T> *r1, Vec3<T> *r2 ) const;" << "\n";
	}


	// void setRows( const Vec3<T> &r0, const Vec3<T> &r1, const Vec3<T> &r2 );
	{
		bool result = false;

		MatT c0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11  );
		MatT m0;
		m0.setToIdentity();

		Vec3<T> v0( 1,  5,  9 );
		Vec3<T> v1( 2,  6, 10 );
		Vec3<T> v2( 3,  7, 11 );

		m0.setRows( v0, v1, v2 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void setRows( const Vec3<T> &r0, const Vec3<T> &r1, const Vec3<T> &r2 );" << "\n";
	}


	// Matrix22<T> subMatrix22( int row, int col ) const;
	{
		bool result = false;

		MatT c0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11, true );

		Matrix22<T> m22_00(  1,  2,
			                 5,  6, true );

		Matrix22<T> m22_01(  2,  3,
			                 6,  7, true );

		Matrix22<T> m22_10(  5,  6,
			                 9, 10, true );

		Matrix22<T> m22_11(  6,  7,
			                10, 11, true );

		result = ( m22_00 == c0.subMatrix22( 0, 0 ) ) &&
		         ( m22_01 == c0.subMatrix22( 0, 1 ) ) &&
				 ( m22_10 == c0.subMatrix22( 1, 0 ) ) &&
				 ( m22_11 == c0.subMatrix22( 1, 1 ) );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix22<T> subMatrix22( int row, int col ) const;" << "\n";
	}


	// void setToNull();
	{
		bool result = false;

		MatT c0( 0, 0, 0,
			     0, 0, 0,
				 0, 0, 0 );

		MatT m0;
		m0.setToNull();

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void setToNull();" << "\n";
	}


	// void setToIdentity();
	{
		bool result = false;

		MatT c0( 1, 0, 0, 
			     0, 1, 0, 
				 0, 0, 1  );

		MatT m0;
		m0.setToIdentity();

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void setToIdentity();" << "\n";
	}


	// T determinant() const;
	{
		bool result = false;

		MatT m0(
			(T) 0.009505, (T)0.848196, (T)-0.529596,
			(T)-0.999570, (T)0.022748, (T) 0.018494,
			(T) 0.027734, (T)0.529193, (T) 0.848048, true
		);

		T det = m0.determinant();

		T diff = fabs( (T)0.999999 - det );

		result = ( diff <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "T determinant() const;" << "\n";
	}


	// T trace() const;
	{
		bool result = false;

		MatT m0(
			(T) 0.009505, (T)0.848196, (T)-0.529596, 
			(T)-0.999570, (T)0.022748, (T) 0.018494, 
			(T) 0.027734, (T)0.529193, (T) 0.848048, true
		);

		T trace = m0.trace();

		T diff = fabs( (T)0.880301 - trace );

		result = ( diff <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "T trace() const;" << "\n";
	}


	// Matrix33<T> diagonal() const;
	{
		bool result = false;

		MatT c0(  1,  0,  0,
			      0,  6,  0,
				  0,  0, 11 );

		MatT m0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );

		result = ( c0 == m0.diagonal() );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T> diagonal() const;" << "\n";
	}


	// Matrix33<T> lowerTriangular() const;
	{
		bool result = false;

		MatT c0(  1,  0,  0, 
			      5,  6,  0, 
				  9, 10, 11, true );

		MatT m0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11, true );

		result = ( c0 == m0.lowerTriangular() );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T> lowerTriangular() const;" << "\n";
	}


	// Matrix33<T> upperTriangular() const;
	{
		bool result = false;

		MatT c0(  1,  2,  3,
			      0,  6,  7,
				  0,  0, 11, true );

		MatT m0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11,  true );

		result = ( c0 == m0.upperTriangular() );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T> upperTriangular() const;" << "\n";
	}


	// void transpose();
	{
		bool result = false;

		MatT c0(  1,  5,  9, 
			      2,  6, 10, 
				  3,  7, 11 );

		MatT m0(  1,  2,  3,
			      5,  6,  7,
				  9, 10, 11 );
		
		m0.transpose();

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void transpose();" << "\n";
	}


	// Matrix33<T> transposed() const;
	{
		bool result = false;

		MatT c0(  1,  5,  9,
			      2,  6, 10,
				  3,  7, 11 );

		MatT m0(  1,  2,  3, 
			      5,  6,  7, 
				  9, 10, 11 );

		result = ( c0 == m0.transposed() );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T> transposed() const;" << "\n";
	}


	// void invert (T epsilon = FLT_MIN );
	{
		bool result = false;

		MatT c0(
			(T)0.000000105208,  (T)0.000003012750,  (T)-0.000000298135,
			(T)-0.000000005514, (T)-0.000001024409, (T)1.889669795292,
			(T) 0.000005693105, (T)54.071754880366, (T)-2.324348789360
		);

		MatT m0(
			(T) 0.009505e9, (T) 0.848196, (T)-0.529596,
			(T)-0.999570, (T) 0.022748, (T) 0.018494,
			(T) 0.027734, (T) 0.529193, (T) 0.848048e-8
		);

		m0.invert();

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void invert (T epsilon = FLT_MIN );" << "\n";
	}


	// Matrix33<T> inverted( T epsilon = FLT_MIN ) const;
	{
		bool result = false;

		MatT c0(
			(T)0.000000105208,  (T)0.000003012750,  (T)-0.000000298135,
			(T)-0.000000005514, (T)-0.000001024409, (T)1.889669795292,
			(T) 0.000005693105, (T)54.071754880366, (T)-2.324348789360
		);

		MatT m0(
			(T) 0.009505e9, (T) 0.848196, (T)-0.529596,
			(T)-0.999570, (T) 0.022748, (T) 0.018494,
			(T) 0.027734, (T) 0.529193, (T) 0.848048e-8
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

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T> inverted( T epsilon = FLT_MIN ) const;" << "\n";
	}


	// Vec3<T> preMultiply( const Vec3<T> &v ) const;
	{
		bool result = false;

		Vec3<T> cv0( 1, 2, 3 );

		MatT c0( 1, 0, 0,
			     0, 1, 0,
				 0, 0, 1 );

		Vec3<T> diff = cv0 - c0.preMultiply( cv0 );

		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );

		result = ( diff.x <= EPSILON ) && 
			     ( diff.y <= EPSILON ) && 
				 ( diff.z <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "Vec3<T> preMultiply( const Vec3<T> &v ) const;" << "\n";
	}


	// Vec3<T> preMultiply( const Vec3<T> &v ) const;
	{
		bool result = false;

		Vec3<T> cv0( 1, 2, 3 );

		MatT c0( 1, 0, 0, 
			     0, 1, 0, 
				 0, 0, 1 );

		Vec3<T> diff = cv0 - c0.preMultiply( cv0 );

		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );

		result = ( diff.x <= EPSILON ) && 
			     ( diff.y <= EPSILON ) && 
				 ( diff.z <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "Vec3<T> preMultiply( const Vec3<T> &v ) const;" << "\n";
	}


	// Vec3<T> postMultiply( const Vec3<T> &v ) const;
	{
		bool result = false;

		Vec3<T> cv0( 1, 2, 3 );

		MatT c0( 1, 0, 0,
			     0, 1, 0,
				 0, 0, 1 );

		Vec3<T> diff = cv0 - c0.postMultiply( cv0 );

		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );

		result = ( diff.x <= EPSILON ) && 
			     ( diff.y <= EPSILON ) && 
				 ( diff.z <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "Vec3<T> postMultiply( const Vec3<T> &v ) const;" << "\n";
	}


	// Vec3<T> postMultiply( const Vec3<T> &v ) const;
	{
		bool result = false;

		Vec3<T> cv0( 1, 2, 3 );

		MatT c0( 1, 0, 0, 
			     0, 1, 0, 
				 0, 0, 1 );

		Vec3<T> diff = cv0 - c0.postMultiply( cv0 );

		diff.x = fabs( diff.x );
		diff.y = fabs( diff.y );
		diff.z = fabs( diff.z );

		result = ( diff.x <= EPSILON ) && 
			     ( diff.y <= EPSILON ) && 
				 ( diff.z <= EPSILON );

		os << (result ? "passed" : "FAILED") << " : " << "Vec3<T> postMultiply( const Vec3<T> &v ) const;" << "\n";
	}


	// void affineInvert(){ *this = affineInverted(); } 
	{
		bool result = false;
		os << "notest" << " : " << "void affineInvert(); " << "\n";
		//os << (result ? "passed" : "FAILED") << " : " << "void affineInvert(); " << "\n";
	}


	// Matrix33<T> affineInverted() const;
	{
		bool result = false;
		os << "notest" << " : " << "void affineInverted(); " << "\n";
		//os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T> affineInverted() const;" << "\n";
	}


	// Vec3<T> transformPointAffine( const Vec3<T> &rhs ) const;
	{
		bool result = false;
		os << "notest" << " : " << "Vec3<T> transformPointAffine( const Vec3<T> &rhs ) const;" << "\n";
		//os << (result ? "passed" : "FAILED") << " : " << "Vec3<T> transformPointAffine( const Vec3<T> &rhs ) const;" << "\n";
	}

	// Vec3<T> transformVec( const Vec3<T> &rhs ) const;
	{
		bool result = false;

		MatT c0(
			(T) 0.45399, (T)-0.89101, (T) 0.00000, 
			(T) 0.89101, (T) 0.45399, (T) 0.00000, 
			(T) 0.00000, (T) 0.00000, (T) 1.00000, true
		);

		MatT m0(
			(T) 0.45399, (T)-0.89101, (T) 0.00000,
			(T) 0.89101, (T) 0.45399, (T) 0.00000,
			(T) 0.00000, (T) 0.00000, (T) 1.00000, true
		);

		Vec3<T> v0( (T)1.25, (T)-4.312, (T)5.2112 );

		result = ( c0.transformVec( v0 ) == m0.transformVec( v0 ) );

		os << (result ? "passed" : "FAILED") << " : " << "Vec3<T> transformVec( const Vec3<T> &rhs ) const;" << "\n";
	}


	// void rotate( const Vec3<T> &axis, T radians );
	{
		bool result = false;

		MatT c0( (T) 1.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000,  
				 (T) 0.000000000000000, (T) 0.840963470480837, (T)-0.541091897293636,  
				 (T) 0.000000000000000, (T) 0.541091897293636, (T) 0.840963470480837, true );

		MatT m0;
		m0.setToIdentity();
		m0.rotate( Vec3<T>( 1, 0, 0 ), toRadians( (T)32.758 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void rotate( const Vec3<T> &axis, T radians );" << "\n";
	}


	// void rotate( const Vec3<T> &eulerRadians );
	{
		bool result = false;

		MatT c0(
			(T) 0.091142486334637, (T) 0.428791685462663, (T)-0.898794046299167,
			(T)-0.730490309010667, (T) 0.642199351624628, (T) 0.232301315567534,
			(T) 0.676813826414798, (T) 0.635387821138395, (T) 0.371759816444386
		);

		Vec3<T> eu0;
		eu0.x = toRadians( (T)32 );
		eu0.y = toRadians( (T)64 );
		eu0.z = toRadians( (T)78 );
		MatT m0;
		m0.setToIdentity();
		m0.rotate( eu0 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void rotate( const Vec3<T> &eulerRadians );" << "\n";
		
	}


	// void rotate( const Vec3<T> &from, const Vec3<T> &to, const Vec3<T> &worldUp );
	{
		bool result = false;

		MatT c0( (T) 1.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000, 
			     (T) 0.000000000000000, (T) 0.840963470480837, (T)-0.541091897293636, 
				 (T) 0.000000000000000, (T) 0.541091897293636, (T) 0.840963470480837, true );

		MatT m0;
		m0.setToIdentity();
		m0.rotate( Vec3<T>( 1, 0, 0 ), toRadians( (T)32.758 ) );
		
		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void rotate( const Vec3<T> &from, const Vec3<T> &to, const Vec3<T> &worldUp );" << "\n";
	}


	// void scaleA( T s );
	{
		bool result = false;

		MatT c0( 5, 0, 0, 
			     0, 5, 0, 
				 0, 0, 5 );

		MatT m0( 1, 0, 0, 
			     0, 1, 0, 
				 0, 0, 1 );

//		m0.scale( 5 );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void scale( T s );" << "\n";
	}

	// void scale( const Vec2<T> &v );
	{
		bool result = false;

		MatT c0( 5, 0, 0,  
			     0, 6, 0,  
				 0, 0, 1 );

		MatT m0( 1, 0, 0,  
			     0, 1, 0,  
				 0, 0, 1 );

//		m0.scale( Vec2<T>( 5, 6 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void scale( const Vec2<T> &v );" << "\n";
	}

	// void scale( const Vec3<T> &v );
	{
		bool result = false;

		MatT c0( 5, 0, 0,  
			     0, 6, 0,  
				 0, 0, 7  );

		MatT m0( 1, 0, 0,  
			     0, 1, 0,  
				 0, 0, 1 );

//		m0.scale( Vec3<T>( 5, 6, 7 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "void scale( const Vec3<T> &v );" << "\n";
	}


	// Matrix33<T> invertTransform() const;
	{
		bool result = false;

		MatT c0(
			(T) 0.45399, (T) 0.89101, (T) 0.00000,
			(T)-0.89101, (T) 0.45399, (T) 0.00000,
			(T) 0.00000, (T) 0.00000, (T) 1.00000, true
		);

		MatT m0(
			(T) 0.45399, (T)-0.89101, (T) 0.00000, 
			(T) 0.89101, (T) 0.45399, (T) 0.00000, 
			(T) 0.00000, (T) 0.00000, (T) 1.00000,  true
		);

		m0.invertTransform();

		result = ( c0 == m0.invertTransform() );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33<T> invertTransform() const;" << "\n";
	}


	// static Matrix33<T> identity();
	{
		bool result = false;

		MatT c0( 1, 0, 0, 
			     0, 1, 0, 
				 0, 0, 1 );

		result = ( c0 == MatT::identity() );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix33<T> identity();" << "\n";
	}


	// static Matrix33<T> one();
	{
		bool result = false;

		MatT c0( 1, 1, 1, 
			     1, 1, 1, 
				 1, 1, 1 );

		result = ( c0 == MatT::one() );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix33<T> one();" << "\n";
	}


	// static Matrix33<T> zero();
	{
		bool result = false;

		MatT c0( 0, 0, 0,  
			     0, 0, 0,  
				 0, 0, 0 );

		result = ( c0 == MatT::zero() );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix33<T> zero();" << "\n";
	}

	// static Matrix33<T> createRotation( const Vec3<T> &axis, T radians );
	{
		bool result = false;

		MatT c0( (T) 1.000000000000000, (T) 0.000000000000000, (T) 0.000000000000000,  
			     (T) 0.000000000000000, (T) 0.840963470480837, (T)-0.541091897293636,  
				 (T) 0.000000000000000, (T) 0.541091897293636, (T) 0.840963470480837, true );

		MatT m0 = MatT::createRotation( Vec3<T>( 1, 0, 0 ), toRadians( (T)32.758 ) );
		
		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix33<T> createRotation( const Vec3<T> &axis, T radians );" << "\n";
	}


	// static Matrix33<T> createRotation( const Vec3<T> &from, const Vec3<T> &to, const Vec3<T> &worldUp );
	{
		bool result = false;

		MatT c0( (T) 0.707106781186548, (T) 0.000000000000000, (T)-0.707106781186548,  
			     (T) 0.000000000000000, (T) 1.000000000000000, (T) 0.000000000000000,  
				 (T) 0.707106781186548, (T) 0.000000000000000, (T) 0.707106781186548, true );

		MatT m0 = MatT::createRotation( 
						Vec3<T>( (T)0.00000000000000, (T)0.000000000000000, (T)1.00000000000000 ), 
						Vec3<T>( (T)0.70710678118654, (T)0.000000000000000, (T)0.70710678118654 ), 
						Vec3<T>( (T)0.00000000000000, (T)1.000000000000000, (T)0.00000000000000 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix33<T> createRotation( const Vec3<T> &from, const Vec3<T> &to, const Vec3<T> &worldUp );" << "\n";
	}


	// static Matrix33<T> createRotation( const Vec3<T> &eulerRadians );
	{
		bool result = false;

		MatT c0(
			(T) 0.091142486334637, (T) 0.428791685462663, (T)-0.898794046299167, 
			(T)-0.730490309010667, (T) 0.642199351624628, (T) 0.232301315567534, 
			(T) 0.676813826414798, (T) 0.635387821138395, (T) 0.371759816444386 
		);

		Vec3<T> eu0;
		eu0.x = toRadians( (T)32 );
		eu0.y = toRadians( (T)64 );
		eu0.z = toRadians( (T)78 );
		MatT m0 = MatT::createRotation( Vec3<T>( eu0 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix33<T> createRotation( const Vec3<T> &eulerRadians );" << "\n";
	}


	// static Matrix33<T> createScale( T s );
	{
		bool result = false;

		MatT c0( 6, 0, 0,  
			     0, 6, 0,  
				 0, 0, 6 );

		result = ( c0 == MatT::createScale( (T)6 ) );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix33<T> createScale( T s );" << "\n";
	}


	// static Matrix33<T> createScale( const Vec2<T> &v );
	{
		bool result = false;

		MatT c0( 2, 0, 0,   
			     0, 3, 0,   
				 0, 0, 1 );

		result = ( c0 == MatT::createScale( Vec2<T>( 2, 3 ) ) );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix33<T> createScale( const Vec2<T> &v );" << "\n";
	}


	// static Matrix33<T> createScale( const Vec3<T> &v );
	{
		bool result = false;

		MatT c0( 2, 0, 0,   
			     0, 3, 0,   
				 0, 0, 4  );

		result = ( c0 == MatT::createScale( Vec3<T>( 2, 3, 4 ) ) );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix33<T> createScale( const Vec3<T> &v );" << "\n";
	}
 

	// static Matrix33<T> alignZAxisWithTarget( Vec3<T> targetDir, Vec3<T> upDir );
	{
		bool result = false;

		MatT c0(  1,  0,  0,   
			      0,  1,  0,   
				  0,  0, -1 );

		MatT m0 = MatT::alignZAxisWithTarget( Vec3<T>( 0, 0, -1 ), Vec3<T>( 0, 1, 0 ) );

		result = ( c0 == m0 );

		os << (result ? "passed" : "FAILED") << " : " << "static Matrix33<T> alignZAxisWithTarget( Vec3<T> targetDir, Vec3<T> upDir );" << "\n";
	}

	// Matrix33 to Quaternion/Quaternion to Matrix33 
	{
		bool result = false;

		Matrix33<T> c33(
			(T) 0.091142486334637, (T) 0.428791685462663, (T)-0.898794046299167,
			(T)-0.730490309010667, (T) 0.642199351624628, (T) 0.232301315567534,
			(T) 0.676813826414798, (T) 0.635387821138395, (T) 0.371759816444386
		);



		Matrix44<T> c44(
			(T) 0.091142486334637, (T) 0.428791685462663, (T)-0.898794046299167, 0,
			(T)-0.730490309010667, (T) 0.642199351624628, (T) 0.232301315567534, 0,
			(T) 0.676813826414798, (T) 0.635387821138395, (T) 0.371759816444386, 0,
			    0.000000000000000,     0.000000000000000,     0.000000000000000, 1
		);

		Quaternion<T> q0( c33 );
		Quaternion<T> q1( c44 );

		Matrix33<T> m0 = q1.toMatrix33();
		Matrix44<T> m1 = q1.toMatrix44();

		result = ( m0 == m1.subMatrix33( 0, 0 ) );

		os << (result ? "passed" : "FAILED") << " : " << "Matrix33 to Quaternion/Quaternion to Matrix33" << "\n";
	}


	{
		bool result = true;
		vec3 vec;
		
		// test principal axes
		vec = vec3::yAxis();
		result = result && ( vec.getOrthogonal().dot( vec ) < EPSILON );
		vec = -vec3::yAxis();
		result = result && ( vec.getOrthogonal().dot( vec ) < EPSILON );
		vec = vec3::xAxis();
		result = result && ( vec.getOrthogonal().dot( vec ) < EPSILON );
		vec = -vec3::xAxis();
		result = result && ( vec.getOrthogonal().dot( vec ) < EPSILON );
		vec = vec3::zAxis();
		result = result && ( vec.getOrthogonal().dot( vec ) < EPSILON );
		vec = -vec3::zAxis();
		result = result && ( vec.getOrthogonal().dot( vec ) < EPSILON );
		vec = vec3::zero();
		result = result && ( vec.getOrthogonal().dot( vec ) < EPSILON );
		
		// test some random vectors
		for( int i = 0; i < 100; ++i ) {
			vec = vec3( randFloat(), randFloat(), randFloat() ).safeNormalized();
			result = result && ( vec.getOrthogonal().dot( vec ) < EPSILON );
		}
	
		os << (result ? "passed" : "FAILED") << " : " << "Vec3 getOrthogonal" << "\n";
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
