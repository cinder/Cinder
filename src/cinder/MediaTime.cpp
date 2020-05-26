/*
 Copyright (c) 2020, The Cinder Project: http://libcinder.org All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/MediaTime.h"
#include "cinder/CinderAssert.h"
#include <algorithm>
#include <iostream>

namespace cinder {

const int32_t MediaTime::DEFAULT_TIME_BASE;
const int64_t MediaTime::MAX_TIME_BASE;

MediaTime::MediaTime( int64_t value, int32_t base, int64_t epoch )
	: value( value ), base( base ), epoch( epoch )
{
	if( base < 0 ) {
		value = -value;
		base = -base;
	}
	CI_ASSERT( base > 0 );
}

void MediaTime::setBase( int32_t newBase )
{
	CI_ASSERT( newBase > 0 ); value = value * newBase / base; base = newBase;
}

// Divides the value and base by the greatest common common divisor of both. Does not affect epoch
void MediaTime::simplify()
{
	CI_ASSERT( base > 0 );

	if( value == 0 ) {
		base = 1;
		return;
	}

	int64_t u = value, v = base;
	while( v != 0 ) {
		int64_t r = u % v;
		u = v;
		v = r;
	}
	value /= u;
	base /= (int32_t)u;
}

//! returns a new common base, and re-bases 'lhsValue' and 'rhsValue' to this new base. If no legal common base, then uses DEFAULT_TIME_BASE
int32_t MediaTime::simplifyBases( int64_t *lhsValue, int32_t lhsBase, int64_t *rhsValue, int32_t rhsBase )
{
	CI_ASSERT( lhsBase > 0 && rhsBase > 0 );

	// calculate gcdV = gcd( lhsBase, rhsBase )
	int64_t a = (int64_t)lhsBase, b = (int64_t)rhsBase;
	while( b != 0 )  {
		int64_t t = b;
		b = a % b;
		a = t;
	}
	const int64_t gcdV = a;

	// lcm(a,b) = a / gcd(a,b) * b
	int64_t lcmV = (int64_t)lhsBase / gcdV * rhsBase;
	// if lcm is acceptably small, use that
	if( lcmV <= DEFAULT_TIME_BASE ) {
		*lhsValue = (int64_t)(*lhsValue) * lcmV / lhsBase;
		*rhsValue = (int64_t)(*rhsValue) * lcmV / rhsBase;
		return (int32_t)lcmV;
	}
	// we're going to have to rebase using DEFAULT_BASE
	*lhsValue = (int64_t)(*lhsValue) * DEFAULT_TIME_BASE / lhsBase;
	*rhsValue = (int64_t)(*rhsValue) * DEFAULT_TIME_BASE / rhsBase;
	return DEFAULT_TIME_BASE;
}

std::ostream& operator<<( std::ostream &os, const MediaTime &mt )
{
	if( mt.epoch == 0 )
		return os << mt.value << "/" << mt.base;
	else
		return os << mt.value << "/" << mt.base << "[" << mt.epoch << "]";
}

} // namespace cinder
