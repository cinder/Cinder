/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/CinderMath.h"
#include "cinder/Vector.h"
#include "cinder/Matrix.h"

namespace cinder {

template<typename T, typename Y>
struct QUATCONV {
	typedef typename T::TYPE F;
	static F	getW( const Y &v ) { return static_cast<F>( v.w ); }
	static F	getX( const Y &v ) { return static_cast<F>( v.x ); }
	static F	getY( const Y &v ) { return static_cast<F>( v.y ); }
	static F	getZ( const Y &v ) { return static_cast<F>( v.z ); }
};

template<typename T>
class Quaternion
{
 public:
	typedef T TYPE;
 
	Vec3<T>		v; // axisOfRotation.normalized() * sin( angleOfRotation / 2 )
	T			w; // cos( angleOfRotation / 2 )

	Quaternion(): w( 1 ), v( 0, 0, 0 ){} // default constructor is identity quat
	template<typename FromT>
	Quaternion( const Quaternion<FromT>& q ) : w( static_cast<T>( q.w ) ), v( q.v ) {}

	Quaternion( T aW, T x, T y, T z ): w( aW ), v( x, y, z ) {}	
	// construct from axis-angle
	Quaternion( const Vec3<T> &axis, T radians ) { set( axis, radians ); } 
	Quaternion( const Vec3<T> &from, const Vec3<T> &to ) { set( from, to ); }
	// create from euler angles in radians expressed in ZYX rotation order
	Quaternion( T xRotation, T yRotation, T zRotation ) { set( xRotation, yRotation, zRotation ); }
	Quaternion( const Matrix44<T> &m ) { set( m ); }
	template<typename Y>
	explicit Quaternion( const Y &v )
		: w( QUATCONV<Quaternion<typename Quaternion::TYPE>,Y>::getW( v ) ), v( QUATCONV<typename Quaternion::TYPE,Y>::getX( v ), QUATCONV<typename Quaternion::TYPE,Y>::getY( v ), QUATCONV<typename Quaternion::TYPE,Y>::getZ( v ) )
	{}
	
	// get axis-angle representation's axis
	Vec3<T> getAxis() const
	{
		T cos_angle = w;
		T invLen = static_cast<T>( 1.0 ) / math<T>::sqrt( static_cast<T>( 1.0 ) - cos_angle * cos_angle );

		return v * invLen;
	}
	
	// get axis-angle representation's angle in radians
	T getAngle() const
	{
		T cos_angle = w;
		return math<T>::acos( cos_angle ) * 2;
	}

	T getPitch() const
	{
		return math<T>::atan2( (T)2 * ( v.y * v.z + w * v.x ), w * w - v.x * v.x - v.y * v.y + v.z * v.z );
	}

	T getYaw() const
	{
		return math<T>::sin( (T)-2 * ( v.x * v.z - w * v.y ) );
	}

	T getRoll() const
	{
		return math<T>::atan2( (T)2 * ( v.x * v.y + w * v.z), w * w + v.x * v.x - v.y * v.y - v.z * v.z );
	}
	
	T dot( const Quaternion<T> &quat ) const
	{
		return w * quat.w + v.dot( quat.v );
	}
	
	T length() const
	{
		return (T)std::sqrt( w*w + v.lengthSquared() );	
	}

	T lengthSquared() const
	{
		return w * w + v.lengthSquared();	
	}

	Quaternion<T> inverse() const
	{
		T norm = w * w + v.x * v.x + v.y * v.y + v.z * v.z;
		// if we're the zero quaternion, just return identity
		/*if( ! ( math<T>::abs( norm ) < EPSILON_VALUE ) ) {
			return identity();
		}*/

		T normRecip = static_cast<T>( 1.0f ) / norm;
		return Quaternion<T>( normRecip * w, -normRecip * v.x, -normRecip * v.y, -normRecip * v.z );
	}

	void normalize()
	{
		if( T len = length() ) {
			w /= len;
			v /= len;
		}
		else {
			w = static_cast<T>( 1.0 );
			v = Vec3<T>( 0, 0, 0 );
		}
	}

	Quaternion<T> normalized() const
	{
		Quaternion<T> result = *this;
	
		if( T len = length() ) {
			result.w /= len;
			result.v /= len;
		}
		else {
			result.w = static_cast<T>( 1.0 );
			result.v = Vec3<T>( 0, 0, 0 );
		}
		
		return result;
	}

	// For unit quaternion, from Advanced Animation and 
	// Rendering Techniques by Watt and Watt, Page 366:
	Quaternion<T> log() const
	{
		T theta = math<T>::acos( std::min( w, (T) 1.0 ) );

		if( theta == 0 )
			return Quaternion<T>( v, 0 );
	    
		T sintheta = math<T>::sin( theta );
	    
		T k;
		if ( abs( sintheta ) < 1 && abs( theta ) >= 3.402823466e+38F * abs( sintheta ) )
			k = 1;
		else
			k = theta / sintheta;

		return Quaternion<T>( (T)0, v.x * k, v.y * k, v.z * k );
	}

	// For pure quaternion (zero scalar part):
	// from Advanced Animation and Rendering
	// Techniques by Watt and Watt, Page 366:
	Quaternion<T> exp() const
	{
		T theta = v.length();
		T sintheta = math<T>::sin( theta );
	    
		T k;
		if( abs( theta ) < 1 && abs( sintheta ) >= 3.402823466e+38F * abs( theta ) )
			k = 1;
		else
			k = sintheta / theta;

		T costheta = math<T>::cos( theta );

		return Quaternion<T>( costheta, v.x * k, v.y * k, v.z * k );
	}

	Quaternion<T> inverted() const
	{
	    T qdot = this->dot( *this );
		return Quaternion( -v / qdot, w / qdot );
	}

	void invert()
	{
	    T qdot = this->dot( *this );
		set( -v / qdot, w / qdot );		
	}

	void set( T aW, T x, T y, T z )
	{
		w = aW;
		v = Vec3<T>( x, y, z );
	}

#if 1
	void set( const Vec3<T> &from, const Vec3<T> &to )
	{
		Vec3<T> axis = from.cross( to );
		
		set( from.dot( to ), axis.x, axis.y, axis.z );
		normalize();

		w += static_cast<T>( 1.0 );

		if( w <= EPSILON ) {
			if ( from.z * from.z > from.x * from.x ) {
				set( static_cast<T>( 0.0 ), static_cast<T>( 0.0 ), from.z, -from.y );
			}
			else {
				set( static_cast<T>( 0.0 ), from.y, -from.x, static_cast<T>( 0.0 ) );
			}
		}
		
		normalize();
	}
#else
	void set( const Vec3<T> &from, const Vec3<T> &to )
	{
	    Vec3<T> f0 = from.normalized();
		Vec3<T> t0 = to.normalized();

		if( f0.dot( t0 ) >= 0 ) { // The rotation angle is less than or equal to pi/2.
			setRotationInternal (f0, t0, *this);
		}
		else {
			// The angle is greater than pi/2.  After computing h0,
			// which is halfway between f0 and t0, we rotate first
			// from f0 to h0, then from h0 to t0.

			Vec3<T> h0 = (f0 + t0).normalized();

			if( h0.dot( h0 ) != 0 ) {
				setRotationInternal( f0, h0, *this );
				Quaternion<T> q;
				setRotationInternal( h0, t0, q );
				
				set( q.w*w - q.v.x*v.x - q.v.y*v.y - q.v.z*v.z,
					q.w*v.x + q.v.x*w + q.v.y*v.z - q.v.z*v.y,
					q.w*v.y + q.v.y*w + q.v.z*v.x - q.v.x*v.z,
					q.w*v.z + q.v.z*w + q.v.x*v.y - q.v.y*v.x );
				
				//*this *= q;
			}
			else {
				// f0 and t0 point in exactly opposite directions.
				// Pick an arbitrary axis that is orthogonal to f0,
				// and rotate by pi.

				w = T( 0 );

				Vec3<T> f02 = f0 * f0;

				if( ( f02.x <= f02.y ) && ( f02.x <= f02.z ) )
					v = f0.cross( Vec3<T>( 1, 0, 0 ) ).normalized();
				else if( f02.y <= f02.z )
					v = f0.cross( Vec3<T>( 0, 1, 0 ) ).normalized();
				else
					v = f0.cross( Vec3<T>( 0, 0, 1 ) ).normalized();
			}
		}
	}

	static void setRotationInternal( const Vec3<T> &f0, const Vec3<T> &t0, Quaternion<T> &q )
	{
		//
		// The following is equivalent to setAxisAngle(n,2*phi),
		// where the rotation axis, is orthogonal to the f0 and
		// t0 vectors, and 2*phi is the angle between f0 and t0.
		//
		// This function is called by setRotation(), above; it assumes
		// that f0 and t0 are normalized and that the angle between
		// them is not much greater than pi/2.  This function becomes
		// numerically inaccurate if f0 and t0 point into nearly
		// opposite directions.
		//

		// Find a normalized vector, h0, that is half way between f0 and t0.
		// The angle between f0 and h0 is phi.
		Vec3<T> h0 = ( f0 + t0 ).normalized();

		// Store the rotation axis and rotation angle.
		q.w = f0.dot( h0 );	//  f0 ^ h0 == cos (phi)
		q.v = f0.cross( h0 );	// (f0 % h0).length() == sin (phi)
	}
#endif

	void set( const Vec3<T> &axis, T radians )
	{
		w = math<T>::cos( radians / 2 );
		v = axis.normalized() * math<T>::sin( radians / 2 );
	}

	// assumes ZYX rotation order and radians
	void set( T xRotation, T yRotation, T zRotation )
	{
		zRotation *= T( 0.5 );
		yRotation *= T( 0.5 );
		xRotation *= T( 0.5 );

		// get sines and cosines of half angles
		T Cx = math<T>::cos( xRotation );
		T Sx = math<T>::sin( xRotation );

		T Cy = math<T>::cos( yRotation );
		T Sy = math<T>::sin( yRotation );

		T Cz = math<T>::cos( zRotation );
		T Sz = math<T>::sin( zRotation );

		// multiply it out
		w = Cx*Cy*Cz - Sx*Sy*Sz;
		v.x = Sx*Cy*Cz + Cx*Sy*Sz;
		v.y = Cx*Sy*Cz - Sx*Cy*Sz;
		v.z = Cx*Cy*Sz + Sx*Sy*Cx;
	}

	void getAxisAngle( Vec3<T> *axis, T *radians ) const
	{
		T cos_angle = w;
		*radians = math<T>::acos( cos_angle ) * 2;
		T invLen = static_cast<T>( 1.0 ) / math<T>::sqrt( static_cast<T>( 1.0 ) - cos_angle * cos_angle );

		axis->x = v.x * invLen;
		axis->y = v.y * invLen;
		axis->z = v.z * invLen;
	}

	Matrix44<T> toMatrix44() const
	{
		Matrix44<T> mV;
		T xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

		xs = v.x + v.x;   
		ys = v.y + v.y;
		zs = v.z + v.z;
		wx = w * xs;
		wy = w * ys;
		wz = w * zs;
		xx = v.x * xs;
		xy = v.x * ys;
		xz = v.x * zs;
		yy = v.y * ys;
		yz = v.y * zs;
		zz = v.z * zs;

		mV[0] = T( 1 ) - ( yy + zz );
		mV[4] = xy - wz;
		mV[8] = xz + wy;
		mV[12] = 0;
	    
		mV[1] = xy + wz;
		mV[5] = T( 1 ) - ( xx + zz );
		mV[9] = yz - wx;
		mV[13] = 0;
	    
		mV[2] = xz - wy;
		mV[6] = yz + wx;
		mV[10] = T( 1 ) - ( xx + yy );
		mV[14] = 0;

		mV[3] = 0;
		mV[7] = 0;
		mV[11] = 0;
		mV[15] = T( 1 );
		
		return mV;
	}

	operator Matrix44<T>() const { return toMatrix44(); }

	Quaternion<T> lerp( T t, const Quaternion<T> &end ) const
	{
		// get cos of "angle" between quaternions
		float cosTheta = dot( end );

		// initialize result
		Quaternion<T> result = t * end;

		// if "angle" between quaternions is less than 90 degrees
		if( cosTheta >= EPSILON ) {
			// use standard interpolation
			result += *this * ( static_cast<T>( 1.0 ) - t );
		}
		else {
			// otherwise, take the shorter path
			result += *this * ( t - static_cast<T>( 1.0 ) );
		}
		
		return result;
	}

    // This method does *not* interpolate along the shortest
    // arc between q1 and q2.  If you desire interpolation
    // along the shortest arc, and q1.dot( q2 ) is negative, then
    // consider flipping the second quaternion explicitly.
    //
    // NOTE: the difference between this and slerp isn't clear, but we're using
    // the Don Hatch / ilmbase squad code which explicity requires this impl. of slerp
    // so I'm leaving it for now
	Quaternion<T> slerpShortestUnenforced( T t, const Quaternion<T> &end ) const
	{
	    Quaternion<T> d = *this - end;
		T lengthD = math<T>::sqrt( this->dot( end ) );

		Quaternion<T> st = *this + end;
		T lengthS = math<T>::sqrt( st.dot( st ) );
	
		T a = 2 * math<T>::atan2( lengthD, lengthS );;
		T s = 1 - t;

		Quaternion<T> q( *this * ( sinx_over_x( s * a ) / sinx_over_x( a ) * s ) +
				end * ( sinx_over_x( t * a ) / sinx_over_x( a ) * t ) );

		return q.normalized();
	}

	Quaternion<T> slerp( T t, const Quaternion<T> &end ) const
	{
		// get cosine of "angle" between quaternions
		T cosTheta = this->dot( end );
		T startInterp, endInterp;

		// if "angle" between quaternions is less than 90 degrees
		if( cosTheta >= EPSILON ) {
			// if angle is greater than zero
			if( ( static_cast<T>( 1.0 ) - cosTheta ) > EPSILON ) {
				// use standard slerp
				T theta = math<T>::acos( cosTheta );
				T recipSinTheta = static_cast<T>( 1.0 ) / math<T>::sin( theta );

				startInterp = math<T>::sin( ( static_cast<T>( 1.0 ) - t ) * theta ) * recipSinTheta;
				endInterp = math<T>::sin( t * theta ) * recipSinTheta;
			}
			// angle is close to zero
			else {
				// use linear interpolation
				startInterp = static_cast<T>( 1.0 ) - t;
				endInterp = t;
			}
		}
		// otherwise, take the shorter route
		else {
			// if angle is less than 180 degrees
			if( ( static_cast<T>( 1.0 ) + cosTheta ) > EPSILON ) {
				// use slerp w/negation of start quaternion
				T theta = math<T>::acos( -cosTheta );
				T recipSinTheta = static_cast<T>( 1.0 ) / math<T>::sin( theta );

				startInterp = math<T>::sin( ( t - static_cast<T>( 1.0 ) ) * theta ) * recipSinTheta;
				endInterp = math<T>::sin( t * theta ) * recipSinTheta;
			}
			// angle is close to 180 degrees
			else {
				// use lerp w/negation of start quaternion
				startInterp = t - static_cast<T>( 1.0 );
				endInterp = t;
			}
		}
	    
		return *this * startInterp + end * endInterp;
	}

	// Spherical Quadrangle Interpolation -
	// from Advanced Animation and Rendering
	// Techniques by Watt and Watt, Page 366:
	// It constructs a spherical cubic interpolation as 
	// a series of three spherical linear interpolations 
	// of a quadrangle of unit quaternions. 
	Quaternion<T> squadShortestEnforced( T t, const Quaternion<T> &qa, const Quaternion<T> &qb, const Quaternion<T> &q2 ) const
	{
		Quaternion<T> r1;
		if( this->dot( q2 ) >= 0 )
			r1 = this->slerpShortestUnenforced( t, q2 );
		else
			r1 = this->slerpShortestUnenforced( t, q2.inverted() );
		
		Quaternion<T> r2;
		if( qa.dot( qb ) >= 0 )
			r2 = qa.slerpShortestUnenforced( t, qb );
		else
			r2 = qa.slerpShortestUnenforced( t, qb.inverted() );
		
		if( r1.dot( r2 ) >= 0 )
			return r1.slerpShortestUnenforced( 2 * t * (1-t), r2 );
		else
			return r1.slerpShortestUnenforced( 2 * t * (1-t), r2.inverted() );
	}

	Quaternion<T> squad( T t, const Quaternion<T> &qa, const Quaternion<T> &qb, const Quaternion<T> &q2 ) const
	{
		Quaternion<T> r1 = this->slerp( t, q2 );	
		Quaternion<T> r2 = qa.slerp( t, qb );
		return r1.slerp( 2 * t * (1-t), r2 );
	}

	// Spherical Cubic Spline Interpolation -
	// from Advanced Animation and Rendering
	// Techniques by Watt and Watt, Page 366:
	// A spherical curve is constructed using three
	// spherical linear interpolations of a quadrangle
	// of unit quaternions: q1, qa, qb, q2.
	// Given a set of quaternion keys: q0, q1, q2, q3,
	// this routine does the interpolation between
	// q1 and q2 by constructing two intermediate
	// quaternions: qa and qb. The qa and qb are 
	// computed by the intermediate function to 
	// guarantee the continuity of tangents across
	// adjacent cubic segments. The qa represents in-tangent
	// for q1 and the qb represents the out-tangent for q2.
	// 
	// The q1 q2 is the cubic segment being interpolated. 
	// The q0 is from the previous adjacent segment and q3 is 
	// from the next adjacent segment. The q0 and q3 are used
	// in computing qa and qb.
	Quaternion<T> spline( T t, const Quaternion<T> &q1,
			   const Quaternion<T> &q2, const Quaternion<T> &q3 ) const
	{
		Quaternion<T> qa = splineIntermediate( *this, q1, q2 );
		Quaternion<T> qb = splineIntermediate( q1, q2, q3 );
		Quaternion<T> result = q1.squadShortestEnforced( t, qa, qb, q2 );

		return result;
	}

	void set( const Matrix44<T> &m )
	{
		T trace =  m.m[0] + m.m[5] + m.m[10];
		if ( trace > (T)0.0 )
		{
			T s = math<T>::sqrt( trace + (T)1.0 );
			w = s * (T)0.5;
			T recip = (T)0.5 / s;
			v.x = ( m.at(2,1) - m.at(1,2) ) * recip;
			v.y = ( m.at(0,2) - m.at(2,0) ) * recip;
			v.z = ( m.at(1,0) - m.at(0,1) ) * recip;
		}
		else
		{
			unsigned int i = 0;
			if( m.at(1,1) > m.at(0,0) )
				i = 1;
			if( m.at(2,2) > m.at(i,i) )
				i = 2;
			unsigned int j = ( i + 1 ) % 3;
			unsigned int k = ( j + 1 ) % 3;
			T s = math<T>::sqrt( m.at(i,i) - m.at(j,j) - m.at(k,k) + (T)1.0 );
			(*this)[i] = (T)0.5 * s;
			T recip = (T)0.5 / s;
			w = ( m.at(k,j) - m.at(j,k) ) * recip;
			(*this)[j] = ( m.at(j,i) + m.at(i,j) ) * recip;
			(*this)[k] = ( m.at(k,i) + m.at(i,k) ) * recip;
		}
	}

	// Operators
	Quaternion<T>& operator=( const Quaternion<T> &rhs )
	{
		v = rhs.v;
		w = rhs.w;
		return *this;
	}

	template<typename FromT>
	Quaternion<T>& operator=( const Quaternion<FromT> &rhs )
	{
		v = rhs.v;
		w = static_cast<T>( rhs.w );
		return *this;
	}

	const Quaternion<T> operator+( const Quaternion<T> &rhs ) const
	{
		const Quaternion<T>& lhs = *this;
		return Quaternion<T>( lhs.w + rhs.w, lhs.v.x + rhs.v.x, lhs.v.y + rhs.v.y, lhs.v.z + rhs.v.z );  
	}

	// post-multiply operator, similar to matrices, but different from Shoemake
	// Concatenates 'rhs' onto 'this'
	const Quaternion<T> operator*( const Quaternion<T> &rhs ) const
	{
		return Quaternion<T>( rhs.w*w - rhs.v.x*v.x - rhs.v.y*v.y - rhs.v.z*v.z,
                   rhs.w*v.x + rhs.v.x*w + rhs.v.y*v.z - rhs.v.z*v.y,
                   rhs.w*v.y + rhs.v.y*w + rhs.v.z*v.x - rhs.v.x*v.z,
                   rhs.w*v.z + rhs.v.z*w + rhs.v.x*v.y - rhs.v.y*v.x );
	}

	const Quaternion<T> operator*( T rhs ) const 
	{ 
		return Quaternion<T>( w * rhs, v.x * rhs, v.y * rhs, v.z * rhs );
	}

	// transform a vector by the quaternion
	const Vec3<T> operator*( const Vec3<T> &vec )
	{
		T vMult = T( 2 ) * ( v.x * vec.x + v.y * vec.y + v.z * vec.z );
		T crossMult = T( 2 ) * w;
		T pMult = crossMult * w - T( 1 );

		return Vec3<T>( pMult * vec.x + vMult * v.x + crossMult * ( v.y * vec.z - v.z * vec.y ),
						pMult * vec.y + vMult * v.y + crossMult * ( v.z * vec.x - v.x * vec.z ),
						pMult * vec.z + vMult * v.z + crossMult * ( v.x * vec.y - v.y * vec.x ) );
	}

	const Quaternion<T> operator-( const Quaternion<T> &rhs ) const
	{
		const Quaternion<T>& lhs = *this;
		return Quaternion<T>( lhs.w - rhs.w, lhs.v.x - rhs.v.x, lhs.v.y - rhs.v.y, lhs.v.z - rhs.v.z );
	}

	Quaternion<T>& operator+=( const Quaternion<T> &rhs )
	{
		w += rhs.w;
		v += rhs.v;
		return *this;
	}

	Quaternion<T>& operator-=( const Quaternion<T>& rhs )
	{
		w -= rhs.w;
		v -= rhs.v;
		return *this;
	}

	Quaternion<T>& operator*=( const Quaternion<T> &rhs )
	{
		Quaternion q = (*this) * rhs;
		v = q.v;
		w = q.w;
		return *this;
	}

	Quaternion<T>& operator*=( T rhs )
	{
		w *= rhs;
		v *= rhs;
		return *this;
	}

	bool operator==( const Quaternion<T> &rhs ) const
	{
		const Quaternion<T>& lhs = *this;
		return ( std::fabs(lhs.w - rhs.w) < EPSILON ) && lhs.v == rhs.v;
	}

	bool operator!=( const Quaternion<T> &rhs ) const
	{
		return ! (*this == rhs);
	}

	inline T& operator[]( unsigned int i ) { return (&v.x)[i]; }
	inline const T& operator[]( unsigned int i ) const { return (&v.x)[i]; }

	static Quaternion<T> identity()
	{
		return Quaternion();
	}

	// Output
	friend std::ostream& operator <<( std::ostream &oss, const Quaternion<T> &q )
	{
		oss << q.getAxis() << " @ " << q.getAngle() * ( (T)180 / M_PI ) << "deg";
		return oss;
	}

 private:
	// From advanced Animation and Rendering
	// Techniques by Watt and Watt, Page 366:
	// computing the inner quadrangle 
	// points (qa and qb) to guarantee tangent
	// continuity.
	static Quaternion<T> splineIntermediate( const Quaternion<T> &q0, const Quaternion<T> &q1, const Quaternion<T> &q2 )
	{
		Quaternion<T> q1inv = q1.inverted();
		Quaternion<T> c1 = q1inv * q2;
		Quaternion<T> c2 = q1inv * q0;
		Quaternion<T> c3 = ( c2.log() + c1.log() ) * (T)-0.25;
		Quaternion<T> qa = q1 * c3.exp();
		return qa.normalized();
	}
};

template<typename T>
inline Vec3<T> operator*( const Vec3<T> &vec, const Quaternion<T> &q )
{
    T vMult = T( 2 ) * ( q.v.x * vec.x + q.v.y * vec.y + q.v.z * vec.z );
    T crossMult = T( 2 ) * q.w;
    T pMult = crossMult * q.w - T( 1 );

    return Vec3<T>( pMult * vec.x + vMult * q.v.x + crossMult * ( q.v.y * vec.z - q.v.z * vec.y ),
                    pMult * vec.y + vMult * q.v.y + crossMult * ( q.v.z * vec.x - q.v.x * vec.z ),
                    pMult * vec.z + vMult * q.v.z + crossMult * ( q.v.x * vec.y - q.v.y * vec.x ) );
}

typedef Quaternion<float>	Quatf;
typedef Quaternion<double>	Quatd;

} // namespace cinder
