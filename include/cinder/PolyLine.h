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

#include "cinder/Vector.h"
#include <vector>

namespace cinder {

template<typename T>
class PolyLine {
  public:
	PolyLine() : mClosed( false ) {}
	PolyLine( const std::vector<T> &aPoints ) : mPoints( aPoints ), mClosed( false ) {}
	
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

	T			getPosition( float t ) const;
	T			getDerivative( float t ) const;

	void		scale( const T &scaleFactor, T scaleCenter = T::zero() );
	void		offset( const T &offsetBy );

	//! Returns whether the point \a pt is contained within the boundaries of the PolyLine
	bool	contains( const Vec2f &pt ) const;

	//! Calculates the boolean union of \a a and \a b. Assumes the first PolyLine in the vector is the outermost and the (optional) others are holes.
	static std::vector<PolyLine> 	calcUnion( const std::vector<PolyLine> &a, std::vector<PolyLine> &b );
	//! Calculates the boolean intersection of \a a and \a b. Assumes the first PolyLine in the vector is the outermost and the (optional) others are holes.
	static std::vector<PolyLine> 	calcIntersection( const std::vector<PolyLine> &a, std::vector<PolyLine> &b );
	//! Calculates the boolean XOR (symmetric difference) of \a a and \a b. Assumes the first PolyLine in the vector is the outermost and the (optional) others are holes.
	static std::vector<PolyLine> 	calcXor( const std::vector<PolyLine> &a, std::vector<PolyLine> &b );
	//! Calculates the boolean difference of \a a and \a b. Assumes the first PolyLine in the vector is the outermost and the (optional) others are holes.
	static std::vector<PolyLine> 	calcDifference( const std::vector<PolyLine> &a, std::vector<PolyLine> &b );		
	
  private:
	std::vector<T>			mPoints;
	bool					mClosed;
};

typedef PolyLine<Vec2f> PolyLine2f;
typedef PolyLine<Vec2d> PolyLine2d;

} // namespace cinder
