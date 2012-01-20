/*
 Copyright (c) 2011, The Cinder Project, All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Portions Copyright (c) 2004, Laminar Research.

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

#include "cinder/CinderMath.h"

namespace cinder {


/////////////////////////////////////////////////////////////////////////////////////////////////
// solveCubic
template<typename T>
int solveCubic( T a, T b, T c, T d, T result[3] )
{
	if( a == 0 )
		return solveQuadratic( b, c, d, result );

	T f = ((3 * c / a) - ((b * b) / (a * a))) / 3;
	T g = ((2 * b * b * b) / (a * a * a) - (9 * b * c) / (a * a) + (27 * d) / (a)) / 27;
	T h = g * g / 4 + f * f * f / 27;

	if( f == 0 && g == 0 && h == 0 ) {
		result[0] = -math<T>::cbrt( d / a );
		return 1;
	}
	else if( h > 0 ) {
		// 1 root
		T r = -( g / 2 ) + math<T>::sqrt( h );
		T s = math<T>::cbrt( r );
		T t = -(g / 2) - math<T>::sqrt( h );
		T u = math<T>::cbrt( t );

		result[0] = (s + u) - (b / (3 * a));
		return 1;
	}
	else { // 3 roots
		T i = math<T>::sqrt( (g * g / 4) - h );
		T j = math<T>::cbrt( i );
		T k = math<T>::acos( -(g / (2 * i)) );
		T l = -j;
		T m = math<T>::cos( k / 3 );
		T n = math<T>::sqrt(3) * math<T>::sin( k / 3 );
		T p = -b / (3 * a);
		result[0] = 2 * j * math<T>::cos(k / 3) - (b / (3 * a));
		result[1] = l * (m + n) + p;
		result[2] = l * (m - n) + p;
		return 3;
	}
}
template int solveCubic( float a, float b, float c, float d, float result[3] );
template int solveCubic( double a, double b, double c, double d, double result[3] );


} // namespace cinder