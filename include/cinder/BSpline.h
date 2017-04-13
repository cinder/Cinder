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

 Significant Portions Copyright:
 Geometric Tools, LLC
 Copyright (c) 1998-2010
 Distributed under the Boost Software License, Version 1.0.
 http://www.boost.org/LICENSE_1_0.txt
 http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
*/

#pragma once

#include <vector>
#include "cinder/Vector.h"

namespace cinder {

class CI_API BSplineBasis {
 public:
	BSplineBasis();

	// Open uniform or periodic uniform.  The knot array is internally
	// generated with equally spaced elements.
	BSplineBasis( int aNumCtrlPoints, int iDegree, bool bOpen );
	void create( int aNumCtrlPoints, int iDegree, bool bOpen );

	// Open nonuniform.  The knot array must have n-d elements.  The elements
	// must be nondecreasing.  Each element must be in [0,1].  The caller is
	// responsible for deleting afKnot.  An internal copy is made, so to
	// dynamically change knots you must use the setKnot function.
	BSplineBasis( int aNumCtrlPoints, int iDegree, const float* afKnot );
	void create( int aNumCtrlPoints, int iDegree, const float* afKnot );

	BSplineBasis( const BSplineBasis &basis );
	BSplineBasis& operator=( const BSplineBasis &basis );

	~BSplineBasis();

	int getNumControlPoints() const;
	int getDegree() const;
	bool isOpen() const;
	bool isUniform() const;

	// The knot values can be changed only if the basis function is nonuniform
	// and the input index is valid (0 <= i <= n-d-1).  If these conditions
	// are not satisfied, getKnot returns MAX_REAL.
	void setKnot( int i, float fKnot );
	float getKnot( int i ) const;

	// access basis functions and their derivatives
	float getD0( int i ) const;
	float getD1( int i ) const;
	float getD2( int i ) const;
	float getD3( int i ) const;

	// evaluate basis functions and their derivatives
	void compute( float fTime, unsigned int uiOrder, int &riMinIndex, int &riMaxIndex ) const;

 protected:
	int initialize( int iNumCtrlPoints, int iDegree, bool bOpen );
	float** allocate() const;
	void deallocate( float** aafArray );

	// Determine knot index i for which knot[i] <= rfTime < knot[i+1].
	int getKey( float& rfTime ) const;

	int mNumCtrlPoints;    // n+1
	int mDegree;           // d
	float *mKnots;          // knot[n+d+2]
	bool mOpen, mUniform;

	// Storage for the basis functions and their derivatives first three
	// derivatives.  The basis array is always allocated by the constructor
	// calls.  A derivative basis array is allocated on the first call to a
	// derivative member function.
	float **m_aafBD0;             // bd0[d+1][n+d+1]
	mutable float **m_aafBD1;     // bd1[d+1][n+d+1]
	mutable float **m_aafBD2;     // bd2[d+1][n+d+1]
	mutable float **m_aafBD3;     // bd3[d+1][n+d+1]
};

template<int D, typename T>
class CI_API BSpline {
  public:
	typedef typename VECDIM<D, T>::TYPE	VecT;
	// Construction and destruction.  The caller is responsible for deleting
	// the input arrays if they were dynamically allocated.  Internal copies
	// of the arrays are made, so to dynamically change control points or
	// knots you must use the 'setControlPoint', 'getControlPoint', and
	// 'Knot' member functions.

	// Uniform spline.  The number of control points is n+1 >= 2.  The degree
	// of the B-spline is d and must satisfy 1 <= d <= n.  The knots are
	// implicitly calculated in [0,1].  If bOpen is 'true', the spline is
	// open and the knots are
	//   t[i] = 0,               0 <= i <= d
	//          (i-d)/(n+1-d),   d+1 <= i <= n
	//          1,               n+1 <= i <= n+d+1
	// If bOpen is 'false', the spline is periodic and the knots are
	//   t[i] = (i-d)/(n+1-d),   0 <= i <= n+d+1
	// If bLoop is 'true', extra control points are added to generate a closed
	// curve.  For an open spline, the control point array is reallocated and
	// one extra control point is added, set to the first control point
	// C[n+1] = C[0].  For a periodic spline, the control point array is
	// reallocated and the first d points are replicated.  In either case the
	// knot array is calculated accordingly.
	BSpline( const std::vector<VecT> &points, int degree, bool loop, bool open );
	
	// Open, nonuniform spline.  The knot array must have n-d elements.  The
	// elements must be nondecreasing.  Each element must be in [0,1].
	BSpline() : mCtrlPoints( 0 ), mNumCtrlPoints( -1 ) {}
	BSpline( int numControlPoints, const VecT *controlPoints, int degree, bool loop, const float *knots );
	BSpline( const BSpline &bspline );
	BSpline& operator=( const BSpline &bspline );

	~BSpline();

	int getNumControlPoints() const { return mNumCtrlPoints; }
	int getDegree() const { return mBasis.getDegree(); }
	int getNumSpans() const { return mNumCtrlPoints - mBasis.getDegree(); }
	bool isOpen() const { return mBasis.isOpen(); }
	bool isUniform() const { return mBasis.isUniform(); }
	bool isLoop() const { return mLoop; }

	// Control points may be changed at any time.  The input index should be
	// valid (0 <= i <= n).  If it is invalid, getControlPoint returns a
	// vector whose components are all MAX_REAL.
	void setControlPoint( int i, const VecT &rkCtrl );
	VecT getControlPoint( int i ) const;

	// The knot values can be changed only if the basis function is nonuniform
	// and the input index is valid (0 <= i <= n-d-1).  If these conditions
	// are not satisfied, getKnot returns MAX_REAL.
	void setKnot( int i, float fKnot );
	float getKnot( int i ) const;

	// The spline is defined for 0 <= t <= 1.  If a t-value is outside [0,1],
	// an open spline clamps t to [0,1].  That is, if t > 1, t is set to 1;
	// if t < 0, t is set to 0.  A periodic spline wraps to to [0,1].  That
	// is, if t is outside [0,1], then t is set to t-floor(t).
	VecT getPosition( float t ) const;
	VecT getDerivative( float t ) const;
	VecT getSecondDerivative( float t ) const;
	VecT getThirdDerivative( float t ) const;

	T getSpeed( float t ) const;

	float getLength( float fT0, float fT1 ) const;

	// If you need position and derivatives at the same time, it is more
	// efficient to call these functions.  Pass the addresses of those
	// quantities whose values you want.  You may pass 0 in any argument
	// whose value you do not want.
	void get( float t, VecT *position, VecT *firstDerivative = NULL, VecT *secondDerivative = NULL, VecT *thirdDerivative = NULL ) const;
	//! Returns the time associated with an arc length in the range [0,getLength(0,1)]
	float getTime( float length ) const;

	// Access the basis function to compute it without control points.  This
	// is useful for least squares fitting of curves.
	BSplineBasis& getBasis();

 protected:
    // Replicate the necessary number of control points when the create
    // function has bLoop equal to true, in which case the spline curve must
    // be a closed curve.
    void createControl( const VecT *akCtrlPoint );

    int mNumCtrlPoints;
    VecT *mCtrlPoints;  // ctrl[n+1]
    bool mLoop;
    BSplineBasis mBasis;
    int mReplicate;  // the number of replicated control points
};

typedef BSpline<2,float> BSpline2f;
typedef BSpline<3,float> BSpline3f;

} // namespace cinder
