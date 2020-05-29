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

#pragma once

#include "cinder/Cinder.h"
#include <iosfwd>

namespace cinder {

//! Does not allow negative or zero base
struct MediaTime {
  public:
	static constexpr int32_t DEFAULT_TIME_BASE = 1000000; // microseconds
	static constexpr int64_t MAX_TIME_BASE = 2147483647;

	MediaTime() : value( 0 ), base( 1 ), epoch( 0 ) {} // zero
	MediaTime( int64_t value, int32_t base, int64_t epoch = 0 );
	
	explicit MediaTime( int value ) : value( value ), base( 1 ), epoch( 0 ) {}
	explicit MediaTime( int64_t value ) : value( value ), base( 1 ), epoch( 0 ) {}
	explicit MediaTime( double seconds )
		: value( (int64_t)( seconds * DEFAULT_TIME_BASE ) ), base( DEFAULT_TIME_BASE ), epoch( 0 )
	{}

	int64_t		getValue() const { return value; }
	int32_t		getBase() const { return base; }
	int64_t		getEpoch() const { return epoch; }
	double		getSeconds() const { return value / (double)base; }

	//! Scale value to 'newBase'. value is set to floor if necessary.
	void		setBase( int32_t newBase );

	void		setEpoch( int64_t newEpoch ) { epoch = newEpoch; }

	//! Returns value as scaled to \a otherBase
	int64_t		getValueAsBase( int32_t otherBase ) const { return ( base == otherBase ) ? value : ( value * otherBase / base ); }

	MediaTime	asBase( int32_t otherBase ) const { auto temp = *this; temp.setBase( otherBase ); return temp; }
	MediaTime	asEpoch( int64_t otherEpoch ) const { return MediaTime( value, base, otherEpoch ); }
	MediaTime	as( int32_t otherBase, int64_t otherEpoch ) const { auto temp = *this; temp.setBase( otherBase ); temp.setEpoch( otherEpoch ); return temp; }

	bool operator<( const MediaTime &rhs ) const {
		if( epoch != rhs.epoch ) return epoch < rhs.epoch;
		if( base == rhs.base ) return value < rhs.value;
		else return value * rhs.base < rhs.value * base;
	}

	bool operator>( const MediaTime &rhs ) const {
		if( epoch != rhs.epoch ) return epoch > rhs.epoch;
		if( base == rhs.base ) return value > rhs.value;
		else return value * rhs.base > rhs.value * base;
	}

	bool operator<=( const MediaTime &rhs ) const {
		if( epoch != rhs.epoch ) return epoch <= rhs.epoch;
		if( base == rhs.base ) return value <= rhs.value;
		else return value * rhs.base <= rhs.value * base;
	}

	bool operator>=( const MediaTime &rhs ) const {
		if( epoch != rhs.epoch ) return epoch >= rhs.epoch;
		if( base == rhs.base ) return value >= rhs.value;
		else return value * rhs.base >= rhs.value * base;
	}

	//! Tests for mathematical equivalence, not bitwise equivalence
	bool operator==( const MediaTime &rhs ) const {
		if( epoch != rhs.epoch ) return false;
		else if( base == rhs.base ) return value == rhs.value;
		else return value * rhs.base == rhs.value * base;
	}

	bool operator!=( const MediaTime &rhs ) const {
		if( epoch != rhs.epoch ) return true;
		else if( base == rhs.base ) return value != rhs.value;
		else return value * rhs.base != rhs.value * base;
	}

	//! Epoch is max of epochs. If base overflows, DEFAULT_TIME_BASE is selected
	MediaTime operator-( const MediaTime &rhs ) const {
		if( rhs.base == base ) return MediaTime( value - rhs.value, base, epoch > rhs.epoch ? epoch : rhs.epoch );
		else {
			int64_t lhsValue = value, rhsValue = rhs.value;
			int32_t newBase = simplifyBases( &lhsValue, base, &rhsValue, rhs.base );
			return MediaTime( lhsValue - rhsValue, newBase, epoch > rhs.epoch ? epoch : rhs.epoch );
		}
	}

	//! Epoch is max of epochs. If base overflows, DEFAULT_TIME_BASE is selected
	MediaTime operator+( const MediaTime &rhs ) const {
		if( rhs.base == base ) return MediaTime( value + rhs.value, base, epoch > rhs.epoch ? epoch : rhs.epoch );
		else {
			int64_t lhsValue = value, rhsValue = rhs.value;
			int32_t newBase = simplifyBases( &lhsValue, base, &rhsValue, rhs.base );
			return MediaTime( lhsValue + rhsValue, newBase, epoch > rhs.epoch ? epoch : rhs.epoch );
		}
	}

	//! Epoch is max of epochs. If base overflows, DEFAULT_TIME_BASE is selected
	MediaTime operator*( const MediaTime &rhs ) const {
		if( base * (int64_t)rhs.base <= (int64_t)DEFAULT_TIME_BASE ) return MediaTime( value * rhs.value, base * rhs.base, epoch > rhs.epoch ? epoch : rhs.epoch );
		else {
			int64_t lhsValue = value * DEFAULT_TIME_BASE / base, rhsValue = rhs.value * DEFAULT_TIME_BASE / rhs.base;
			return MediaTime( lhsValue * rhsValue / DEFAULT_TIME_BASE, DEFAULT_TIME_BASE, epoch > rhs.epoch ? epoch : rhs.epoch );
		}
	}

	//! Epoch is max of epochs. If base overflows, DEFAULT_TIME_BASE is selected
	MediaTime operator/( const MediaTime &rhs ) const {
		// exchanges 'value' and 'base' of 'rhs' and multiplies
		if( base * (int64_t)rhs.value <= (int64_t)DEFAULT_TIME_BASE ) return MediaTime( value * rhs.base, (int32_t)(base * rhs.value), epoch > rhs.epoch ? epoch : rhs.epoch );
		else {
			int64_t lhsValue = value * DEFAULT_TIME_BASE / base, rhsValue = (int64_t)rhs.base * DEFAULT_TIME_BASE / rhs.value;
			return MediaTime( lhsValue * rhsValue / DEFAULT_TIME_BASE, DEFAULT_TIME_BASE, epoch > rhs.epoch ? epoch : rhs.epoch );
		}
	}

	//! Does not affect epoch. If base overflows, DEFAULT_TIME_BASE is selected
	MediaTime& operator-=( const MediaTime &rhs ) {
		MediaTime newTime = *this - rhs;
		value = newTime.value;
		base = newTime.base;
		return *this;
	}

	//! Does not affect epoch. If base overflows, DEFAULT_TIME_BASE is selected
	MediaTime& operator+=( const MediaTime &rhs ) {
		MediaTime newTime = *this + rhs;
		value = newTime.value;
		base = newTime.base;
		return *this;
	}

	//! Does not affect epoch. If base overflows, DEFAULT_TIME_BASE is selected
	MediaTime& operator*=( const MediaTime &rhs ) {
		MediaTime newTime = *this * rhs;
		value = newTime.value;
		base = newTime.base;
		return *this;
	}

	//! Does not affect epoch. If base overflows, DEFAULT_TIME_BASE is selected
	MediaTime& operator/=( const MediaTime &rhs ) {
		MediaTime newTime = *this / MediaTime( rhs.value, rhs.base, rhs.epoch );
		value = newTime.value;
		base = newTime.base;
		return *this;
	}

	MediaTime operator-() const {
		return MediaTime( -value, base, epoch );
	}

	friend std::ostream& operator<<( std::ostream &os, const MediaTime &mt );

	//! Divides the value and base by the greatest common common divisor of both. Does not affect epoch
	void simplify();

	//! returns a new common base, and re-bases \a lhsValue and \a rhsValue to this new base. If no legal common base, then uses DEFAULT_TIME_BASE
	static int32_t simplifyBases( int64_t *lhsValue, int32_t lhsBase, int64_t *rhsValue, int32_t rhsBase );

	int64_t		value;
	int32_t		base;
	int64_t		epoch;
};

inline MediaTime operator*( int lhs, const MediaTime &rhs ) { return MediaTime( rhs.value * lhs, rhs.base, rhs.epoch ); }
inline MediaTime operator*( const MediaTime &lhs, int rhs ) { return MediaTime( lhs.value * rhs, lhs.base, lhs.epoch ); }
inline MediaTime operator*( double lhs, const MediaTime &rhs ) { return MediaTime( lhs ) * rhs; }
inline MediaTime operator*( const MediaTime &lhs, double rhs ) { return lhs * MediaTime( rhs ); }
inline MediaTime operator*( float lhs, const MediaTime &rhs ) { return MediaTime( lhs ) * rhs; }
inline MediaTime operator*( const MediaTime &lhs, float rhs ) { return lhs * MediaTime( rhs ); }

inline MediaTime operator/( int lhs, const MediaTime &rhs ) { return MediaTime( lhs ) / rhs; }
inline MediaTime operator/( const MediaTime &lhs, int rhs ) { return lhs / MediaTime( rhs ); }
inline MediaTime operator/( double lhs, const MediaTime &rhs ) { return MediaTime( lhs ) / rhs; }
inline MediaTime operator/( const MediaTime &lhs, double rhs ) { return lhs / MediaTime( rhs ); }
inline MediaTime operator/( float lhs, const MediaTime &rhs ) { return MediaTime( lhs ) / rhs; }
inline MediaTime operator/( const MediaTime &lhs, float rhs ) { return lhs / MediaTime( rhs ); }

inline MediaTime operator"" _sec( long double seconds )
{
	return MediaTime( (int64_t)(seconds * MediaTime::DEFAULT_TIME_BASE ), MediaTime::DEFAULT_TIME_BASE );
}

inline MediaTime operator"" _sec( unsigned long long seconds )
{
	return MediaTime( (int64_t)(seconds * MediaTime::DEFAULT_TIME_BASE ), MediaTime::DEFAULT_TIME_BASE );
}

} // namespace cinder
