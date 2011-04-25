/*
 Copyright (c) 2010, The Cinder Project: http://libcinder.org
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

#include "cinder/Matrix22.h"
#include "cinder/Matrix33.h"
#include "cinder/Matrix44.h"

#if defined( CINDER_MSW ) || defined( CINDER_MAC )
#   include <emmintrin.h>
#   include <xmmintrin.h>
#endif

namespace cinder {

#if defined( CINDER_MSW ) || defined( CINDER_MAC )

#if defined( CINDER_MSW )
	#define CINDER_ALIGN16_MATRIX44F( VAR ) \
		__declspec( align( 16 ) ) Matrix44f VAR
#elif defined( CINDER_MAC )
	#define CINDER_ALIGN16_MATRIX44F( VAR ) \
		Matrix44f VAR __attribute__ ( ( aligned ( 16 ) ) )
#endif

inline Matrix44f SseMultiply( const Matrix44f& mat0, const Matrix44f& mat1 ) 
{
	CINDER_ALIGN16_MATRIX44F( ret );

	const float* a = mat0.m;
	const float* b = mat1.m;
	float* res = ret.m;

	__m128 b_line, r_line, ab;
	__m128 a_line_0, a_line_4, a_line_8, a_line_12;

	// NOTE: Keep the usage of i in. Removed it and
	//       used constants and it ran slower. I'm not
	//       sure why - so I'm just going with it at 
	//       the moment.
	int i;

	// Load in the rows for b
	a_line_0  = _mm_load_ps( a +  0 );
	a_line_4  = _mm_load_ps( a +  4 );
	a_line_8  = _mm_load_ps( a +  8 );
	a_line_12 = _mm_load_ps( a + 12 );

	i = 0;
	// 
	b_line = _mm_set1_ps( *(b + i + 0) );
	r_line = _mm_mul_ps( b_line, a_line_0 );
	//
	b_line = _mm_set1_ps( *(b + i + 1) );
	ab     = _mm_mul_ps( b_line, a_line_4 );
	r_line = _mm_add_ps( ab, r_line );
	//
	b_line = _mm_set1_ps( *(b + i + 2) );
	ab     = _mm_mul_ps( b_line, a_line_8 );
	r_line = _mm_add_ps( ab, r_line );
	//
	b_line = _mm_set1_ps( *(b + i + 3) );
	ab     = _mm_mul_ps( b_line, a_line_12 );
	r_line = _mm_add_ps( ab, r_line );
	//
	_mm_store_ps( res + i, r_line );

	i = 4;
	// 
	b_line = _mm_set1_ps( *(b + i + 0) );
	r_line = _mm_mul_ps( b_line, a_line_0 );
	//
	b_line = _mm_set1_ps( *(b + i + 1) );
	ab     = _mm_mul_ps( b_line, a_line_4 );
	r_line = _mm_add_ps( ab, r_line );
	//
	b_line = _mm_set1_ps( *(b + i + 2) );
	ab     = _mm_mul_ps( b_line, a_line_8 );
	r_line = _mm_add_ps( ab, r_line );
	//
	b_line = _mm_set1_ps( *(b + i + 3) );
	ab     = _mm_mul_ps( b_line, a_line_12 );
	r_line = _mm_add_ps( ab, r_line );
	//
	_mm_store_ps( res + i, r_line );

	i = 8;
	// 
	b_line = _mm_set1_ps( *(b + i + 0) );
	r_line = _mm_mul_ps( b_line, a_line_0 );
	//
	b_line = _mm_set1_ps( *(b + i + 1) );
	ab     = _mm_mul_ps( b_line, a_line_4 );
	r_line = _mm_add_ps( ab, r_line );
	//
	b_line = _mm_set1_ps( *(b + i + 2) );
	ab     = _mm_mul_ps( b_line, a_line_8 );
	r_line = _mm_add_ps( ab, r_line );
	//
	b_line = _mm_set1_ps( *(b + i + 3) );
	ab     = _mm_mul_ps( b_line, a_line_12 );
	r_line = _mm_add_ps( ab, r_line );
	//
	_mm_store_ps( res + i, r_line );

	i = 12;
	// 
	b_line = _mm_set1_ps( *(b + i + 0) );
	r_line = _mm_mul_ps( b_line, a_line_0 );
	//
	b_line = _mm_set1_ps( *(b + i + 1) );
	ab     = _mm_mul_ps( b_line, a_line_4 );
	r_line = _mm_add_ps( ab, r_line );
	//
	b_line = _mm_set1_ps( *(b + i + 2) );
	ab     = _mm_mul_ps( b_line, a_line_8 );
	r_line = _mm_add_ps( ab, r_line );
	//
	b_line = _mm_set1_ps( *(b + i + 3) );
	ab     = _mm_mul_ps( b_line, a_line_12 );
	r_line = _mm_add_ps( ab, r_line );
	//
	_mm_store_ps( res + i, r_line );

	return ret;
}

#if defined( CINDER_MSW )
#   pragma runtime_checks( "", off )
#   pragma warning( push )
#   pragma warning( disable:4700 )
#endif 

inline Matrix44f SseInvert( const Matrix44f& mat )
{
	CINDER_ALIGN16_MATRIX44F( ret );

	const float* src = mat.m;
	float* dst = ret.m;

	__m128 minor0, minor1, minor2, minor3;
	__m128 row0, row1, row2, row3;
	__m128 det, tmp1;
	tmp1   = _mm_loadh_pi( _mm_loadl_pi( tmp1, ( __m64* )( src ) ), ( __m64* )( src + 4 ) );
	row1   = _mm_loadh_pi( _mm_loadl_pi( row1, ( __m64* )( src + 8 ) ), ( __m64* )( src + 12 ) );
	row0   = _mm_shuffle_ps( tmp1, row1, 0x88 );
	row1   = _mm_shuffle_ps( row1, tmp1, 0xDD );
	tmp1   = _mm_loadh_pi( _mm_loadl_pi( tmp1, ( __m64* )( src + 2 ) ), ( __m64* )( src + 6 ) );
	row3   = _mm_loadh_pi( _mm_loadl_pi( row3, ( __m64* )( src + 10 ) ), ( __m64* )( src + 14 ) );
	row2   = _mm_shuffle_ps( tmp1, row3, 0x88 );
	row3   = _mm_shuffle_ps( row3, tmp1, 0xDD );
	// ----------------------------------------------- 
	tmp1   = _mm_mul_ps( row2, row3 );
	tmp1   = _mm_shuffle_ps( tmp1, tmp1, 0xB1 );
	minor0 = _mm_mul_ps( row1, tmp1 );
	minor1 = _mm_mul_ps( row0, tmp1 );
	tmp1   = _mm_shuffle_ps( tmp1, tmp1, 0x4E );
	minor0 = _mm_sub_ps( _mm_mul_ps( row1, tmp1 ), minor0 );
	minor1 = _mm_sub_ps( _mm_mul_ps( row0, tmp1 ), minor1 );
	minor1 = _mm_shuffle_ps( minor1, minor1, 0x4E );
	// -----------------------------------------------
	tmp1   = _mm_mul_ps( row1, row2 );
	tmp1   = _mm_shuffle_ps( tmp1, tmp1, 0xB1 );
	minor0 = _mm_add_ps( _mm_mul_ps( row3, tmp1 ), minor0 );
	minor3 = _mm_mul_ps( row0, tmp1 );
	tmp1   = _mm_shuffle_ps( tmp1, tmp1, 0x4E );
	minor0 = _mm_sub_ps( minor0, _mm_mul_ps( row3, tmp1 ) );
	minor3 = _mm_sub_ps( _mm_mul_ps( row0, tmp1 ), minor3 );
	minor3 = _mm_shuffle_ps( minor3, minor3, 0x4E );
	// -----------------------------------------------
	tmp1   = _mm_mul_ps( _mm_shuffle_ps( row1, row1, 0x4E ), row3 );
	tmp1   = _mm_shuffle_ps( tmp1, tmp1, 0xB1 );
	row2   = _mm_shuffle_ps( row2, row2, 0x4E );
	minor0 = _mm_add_ps( _mm_mul_ps( row2, tmp1 ), minor0 );
	minor2 = _mm_mul_ps( row0, tmp1 );
	tmp1   = _mm_shuffle_ps( tmp1, tmp1, 0x4E );
	minor0 = _mm_sub_ps( minor0, _mm_mul_ps( row2, tmp1 ) );
	minor2 = _mm_sub_ps( _mm_mul_ps( row0, tmp1 ), minor2 );
	minor2 = _mm_shuffle_ps( minor2, minor2, 0x4E );
	// -----------------------------------------------
	tmp1   = _mm_mul_ps( row0, row1 );
	tmp1   = _mm_shuffle_ps( tmp1, tmp1, 0xB1 );
	minor2 = _mm_add_ps( _mm_mul_ps( row3, tmp1 ), minor2 );
	minor3 = _mm_sub_ps( _mm_mul_ps( row2, tmp1 ), minor3 );
	tmp1   = _mm_shuffle_ps( tmp1, tmp1, 0x4E );
	minor2 = _mm_sub_ps( _mm_mul_ps( row3, tmp1 ), minor2 );
	minor3 = _mm_sub_ps( minor3, _mm_mul_ps( row2, tmp1 ) );
	// -----------------------------------------------
	tmp1   = _mm_mul_ps( row0, row3 );
	tmp1   = _mm_shuffle_ps( tmp1, tmp1, 0xB1 );
	minor1 = _mm_sub_ps( minor1, _mm_mul_ps( row2, tmp1 ) );
	minor2 = _mm_add_ps( _mm_mul_ps( row1, tmp1 ), minor2 );
	tmp1   = _mm_shuffle_ps( tmp1, tmp1, 0x4E );
	minor1 = _mm_add_ps( _mm_mul_ps( row2, tmp1 ), minor1 );
	minor2 = _mm_sub_ps( minor2, _mm_mul_ps( row1, tmp1 ) );
	// -----------------------------------------------
	tmp1   = _mm_mul_ps( row0, row2 );
	tmp1   = _mm_shuffle_ps( tmp1, tmp1, 0xB1 );
	minor1 = _mm_add_ps( _mm_mul_ps( row3, tmp1 ), minor1 );
	minor3 = _mm_sub_ps( minor3, _mm_mul_ps( row1, tmp1 ) );
	tmp1   = _mm_shuffle_ps( tmp1, tmp1, 0x4E );
	minor1 = _mm_sub_ps( minor1, _mm_mul_ps( row3, tmp1 ) );
	minor3 = _mm_add_ps( _mm_mul_ps( row1, tmp1 ), minor3 );
	// -----------------------------------------------
	det  = _mm_mul_ps( row0, minor0 );
	det  = _mm_add_ps( _mm_shuffle_ps( det, det, 0x4E ), det );
	det  = _mm_add_ss( _mm_shuffle_ps( det, det, 0xB1 ), det );
	tmp1 = _mm_rcp_ss( det );
	det  = _mm_sub_ss( _mm_add_ss( tmp1, tmp1 ), _mm_mul_ss( det, _mm_mul_ss( tmp1, tmp1 ) ) );
	det  = _mm_shuffle_ps( det, det, 0x00 );

	minor0 = _mm_mul_ps( det, minor0 );
	_mm_storel_pi( ( __m64* )( dst ), minor0 );
	_mm_storeh_pi( ( __m64* )( dst + 2 ), minor0 );

	minor1 = _mm_mul_ps( det, minor1 );
	_mm_storel_pi( ( __m64* )( dst + 4 ), minor1 );
	_mm_storeh_pi( ( __m64* )( dst + 6 ), minor1 );

	minor2 = _mm_mul_ps( det, minor2 );
	_mm_storel_pi( ( __m64* )( dst +  8 ), minor2 );
	_mm_storeh_pi( ( __m64* )( dst + 10 ), minor2 );

	minor3 = _mm_mul_ps( det, minor3 );
	_mm_storel_pi( ( __m64* )( dst + 12 ), minor3 );
	_mm_storeh_pi( ( __m64* )( dst + 14 ), minor3 );	

	return ret;
}

#if defined( CINDER_MSW )
#   pragma warning( pop )
#   pragma runtime_checks( "", restore )
#endif

#endif // #if defined( CINDER_MSW ) || defined( CINDER_MAC )

} // namespace cinder
