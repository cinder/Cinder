/*
 Copyright (c) 2010, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#include "cinder/Vector.h"
#include <vector>

namespace cinder {

template<typename T>
class CI_API PolyLineT {
  public:
	PolyLineT() : mClosed( false ) {}
	PolyLineT( const std::vector<T> &aPoints, bool closed = false ) : mPoints( aPoints ), mClosed( closed ) {}
	PolyLineT( std::vector<T> &&aPoints, bool closed = false ) : mPoints( std::move( aPoints ) ), mClosed( closed ) {}

	const std::vector<T>&	getPoints() const { return mPoints; }
	std::vector<T>&			getPoints() { return mPoints; }

	// STL-like convenience functions for iterating points
	typedef typename std::vector<T>::const_iterator	const_iterator;
	typedef typename std::vector<T>::iterator		iterator;

	size_t				size() const { return mPoints.size(); }

	void				push_back( const T &v ) { mPoints.push_back( v ); }
	iterator			begin() { return mPoints.begin(); }
	const_iterator		begin() const { return mPoints.begin(); }
	iterator			end() { return mPoints.end(); }
	const_iterator		end() const { return mPoints.end(); }

	void				setClosed( bool aClosed = true ) { mClosed = aClosed; }
	bool				isClosed() const { return mClosed; }

	//! Returns \c true if PolyLine is clockwise-oriented. If \a isColinear is non-null, it receives \c true if all points are colinear
	bool  isClockwise( bool *isColinear = nullptr ) const;
	//! Returns \c true if PolyLine is counterclockwise-oriented. If \a isColinear is non-null, it receives \c true if all points are colinear
	bool  isCounterclockwise( bool *isColinear = nullptr ) const;

	T			getPosition( float t ) const;
	T			getDerivative( float t ) const;

	void			scale( const T &scaleFactor, T scaleCenter = T() );
	PolyLineT<T>	scaled( const T &scaleFactor, T scaleCenter = T() ) const;
	void			offset( const T &offsetBy );
	PolyLineT<T>	getOffset( const T &offsetBy ) const;
	void			reverse();
	PolyLineT<T>	reversed() const;

	//! Returns whether the point \a pt is contained within the boundaries of the PolyLine
	bool	contains( const vec2 &pt ) const;

	//! Returns the unsigned area of the polygon. Assumes closed and no self-intersections.
	double	calcArea() const;
	//! Returns the centroid or "center of mass" of the polygon. Assumes closed and no self-intersections.
	T		calcCentroid() const;

	friend CI_API std::ostream& operator<<( std::ostream& lhs, const PolyLineT& rhs )
	{
		lhs << "(";
		for( const auto &it : rhs.mPoints )
			lhs << it << " ";
		return lhs << ")";
	}

  private:
	std::vector<T>			mPoints;
	bool					mClosed;
};

typedef PolyLineT<vec2>		PolyLine2;
typedef PolyLineT<vec2>		PolyLine2f;
typedef PolyLineT<dvec2>	PolyLine2d;

} // namespace cinder
