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

#include "cinder/BSpline.h"

#include <memory.h>
#include <assert.h>
#include <limits>

#include "cinder/Vector.h"

namespace cinder {

//////////////////////////////////////////////////////////////////////////////////////////////
// BSplineBasis
template <class T>
void allocate2D( int iCols, int iRows, T**& raatArray )
{
    raatArray = new T*[iRows];
    raatArray[0] = new T[iRows*iCols];
    for( int iRow = 1; iRow < iRows; iRow++ ) {
        raatArray[iRow] = &raatArray[0][iCols*iRow];
    }
}

template <class T>
void deallocate2D( T**& raatArray )
{
    if( raatArray ) {
        delete raatArray[0];
        delete raatArray;
        raatArray = 0;
    }
}

BSplineBasis::BSplineBasis()
	: mKnots( 0 ), m_aafBD0( 0 ), m_aafBD1( 0 ), m_aafBD2( 0 ), m_aafBD3( 0 ), mNumCtrlPoints( -1 )
{
}

BSplineBasis::BSplineBasis( int iNumCtrlPoints, int iDegree, bool bOpen )
{
    create( iNumCtrlPoints, iDegree, bOpen );
}

BSplineBasis::BSplineBasis( const BSplineBasis &basis )
	: mKnots( 0 ), m_aafBD0( 0 ), m_aafBD1( 0 ), m_aafBD2( 0 ), m_aafBD3( 0 )
//	: mNumCtrlPoints( basis.mNumCtrlPoints ), mDegree( basis.mDegree ), mOpen( basis.mOpen ), mUniform( basis.mUniform )
{
/*	int numKnots = initialize( mNumCtrlPoints, mDegree, mOpen );
	memcpy( mKnots, basis.mKnots, sizeof(float) * numKnots );*/
	*this = basis;
}

BSplineBasis& BSplineBasis::operator=( const BSplineBasis &basis )
{
    delete [] mKnots;
    deallocate2D( m_aafBD0 );
    deallocate2D( m_aafBD1 );
    deallocate2D( m_aafBD2 );
    deallocate2D( m_aafBD3 );

	mNumCtrlPoints = basis.mNumCtrlPoints;
	mDegree = basis.mDegree;
	mOpen = basis.mOpen;
	mUniform = basis.mUniform;
	
	if( mNumCtrlPoints > 0 ) {
		int numKnots = initialize( mNumCtrlPoints, mDegree, mOpen );
		memcpy( mKnots, basis.mKnots, sizeof(float) * numKnots );	
	}
	else {
		mKnots = 0;
		m_aafBD0 = m_aafBD1 = m_aafBD2 = m_aafBD3 = 0;
	}

	return *this;
}

void BSplineBasis::create( int iNumCtrlPoints, int iDegree, bool bOpen )
{
    mUniform = true;

    int i, iNumKnots = initialize( iNumCtrlPoints, iDegree, bOpen );
    float fFactor = (1.0f)/( iNumCtrlPoints - mDegree );
    if ( mOpen ) {
        for ( i = 0; i <= mDegree; i++ ) {
            mKnots[i] = (float)0.0;
        }

        for (/**/; i < iNumCtrlPoints; i++ ) {
            mKnots[i] = ( i - mDegree ) * fFactor;
        }

        for(/**/; i < iNumKnots; i++) {
            mKnots[i] = (float)1.0;
        }
    }
    else {
        for ( i = 0; i < iNumKnots; i++ ) {
            mKnots[i] = ( i - mDegree ) * fFactor;
        }
    }
}

BSplineBasis::BSplineBasis( int aNumCtrlPoints, int iDegree, const float *afKnot )
{
    create( mNumCtrlPoints, iDegree, afKnot );
}

void BSplineBasis::create( int aNumCtrlPoints, int iDegree, const float *afKnot )
{
    mUniform = false;

	mNumCtrlPoints = aNumCtrlPoints;

    int i, iNumKnots = initialize( mNumCtrlPoints, iDegree, true );
    for( i = 0; i <= mDegree; i++ ) {
        mKnots[i] = (float)0.0;
    }

    for( int j = 0; i < mNumCtrlPoints; i++, j++ ) {
        mKnots[i] = afKnot[j];
    }

    for( /**/; i < iNumKnots; i++ ) {
        mKnots[i] = (float)1.0;
    }
}

BSplineBasis::~BSplineBasis()
{
    delete [] mKnots;
    deallocate2D( m_aafBD0 );
    deallocate2D( m_aafBD1 );
    deallocate2D( m_aafBD2 );
    deallocate2D( m_aafBD3 );
}

int BSplineBasis::getNumControlPoints() const
{
    return mNumCtrlPoints;
}

int BSplineBasis::getDegree() const
{
    return mDegree;
}


bool BSplineBasis::isOpen() const
{
    return mOpen;
}


bool BSplineBasis::isUniform() const
{
    return mUniform;
}


float BSplineBasis::getD0( int i ) const
{
    return m_aafBD0[mDegree][i];
}


float BSplineBasis::getD1( int i ) const
{
    return m_aafBD1[mDegree][i];
}


float BSplineBasis::getD2( int i ) const
{
    return m_aafBD2[mDegree][i];
}


float BSplineBasis::getD3( int i ) const
{
    return m_aafBD3[mDegree][i];
}


float** BSplineBasis::allocate() const
{
    int iRows = mDegree + 1;
    int iCols = mNumCtrlPoints + mDegree;
    float** aafArray;
    allocate2D<float>( iCols, iRows, aafArray );
    memset(aafArray[0],0,iRows*iCols*sizeof(float));
    return aafArray;
}

int BSplineBasis::initialize( int iNumCtrlPoints, int iDegree, bool bOpen )
{
    assert(iNumCtrlPoints >= 2);
    assert(1 <= iDegree && iDegree <= iNumCtrlPoints-1);

    mNumCtrlPoints = iNumCtrlPoints;
    mDegree = iDegree;
    mOpen = bOpen;

    int iNumKnots = mNumCtrlPoints+mDegree+1;
    mKnots = new float[iNumKnots];

    m_aafBD0 = allocate();
    m_aafBD1 = 0;
    m_aafBD2 = 0;
    m_aafBD3 = 0;

    return iNumKnots;
}

void BSplineBasis::setKnot( int i, float fKnot )
{
	if( ! mUniform ) {
		// access only allowed to elements d+1 <= j <= n
		int j = i + mDegree + 1;
		if( mDegree+1 <= j && j <= mNumCtrlPoints - 1 ) {
			mKnots[j] = fKnot;
		}
	}
}

float BSplineBasis::getKnot( int i ) const
{
    if( ! mUniform ) {
        // access only allowed to elements d+1 <= j <= n
        int j = i + mDegree + 1;
        if ( ( mDegree + 1 <= j ) && ( j <= mNumCtrlPoints - 1 ) ) {
            return mKnots[j];
        }
    }

    return std::numeric_limits<float>::max();
}


int BSplineBasis::getKey( float& rfTime ) const
{
	if( mOpen ) {
		// open splines clamp to [0,1]
		if( rfTime <= (float)0.0 ) {
			rfTime = (float)0.0;
			return mDegree;
		}
		else if ( rfTime >= (float)1.0 ) {
			rfTime = (float)1.0;
			return mNumCtrlPoints - 1;
		}
	}
	else {
		// periodic splines wrap to [0,1]
		if (rfTime < (float)0.0 || rfTime >= (float)1.0) {
			rfTime -= floorf( rfTime );
		}
	}


	int i;
	if( mUniform ) {
		i = mDegree + (int)( ( mNumCtrlPoints - mDegree ) * rfTime );
	}
	else {
		for( i = mDegree + 1; i <= mNumCtrlPoints; i++ ) {
			if( rfTime < mKnots[i] ) {
				break;
			}
		}
		i--;
	}

	return i;
}

void BSplineBasis::compute( float fTime, unsigned int uiOrder, int &riMinIndex, int &riMaxIndex ) const
{
    // only derivatives through third order currently supported
    assert(uiOrder <= 3);

    if (uiOrder >= 1) {
        if (!m_aafBD1) {
            m_aafBD1 = allocate();
        }

        if (uiOrder >= 2) {
            if( ! m_aafBD2 ) {
                m_aafBD2 = allocate();
            }

            if( uiOrder >= 3 ) {
                if ( ! m_aafBD3 ) {
                    m_aafBD3 = allocate();
                }
            }
        }
    }

    int i = getKey(fTime);
    m_aafBD0[0][i] = (float)1.0;

    if( uiOrder >= 1 ) {
        m_aafBD1[0][i] = (float)0.0;
        if ( uiOrder >= 2 ) {
            m_aafBD2[0][i] = (float)0.0;
            if ( uiOrder >= 3 ) {
                m_aafBD3[0][i] = (float)0.0;
            }
        }
    }

    float fN0 = fTime-mKnots[i], fN1 = mKnots[i+1] - fTime;
    float fInvD0, fInvD1;
    int j;
    for( j = 1; j <= mDegree; j++ ) {
		fInvD0 = ((float)1.0)/(mKnots[i+j]-mKnots[i]);
		fInvD1 = ((float)1.0)/(mKnots[i+1]-mKnots[i-j+1]);

		m_aafBD0[j][i] = fN0*m_aafBD0[j-1][i]*fInvD0;
		m_aafBD0[j][i-j] = fN1*m_aafBD0[j-1][i-j+1]*fInvD1;

		if( uiOrder >= 1 ) {
			m_aafBD1[j][i] = (fN0*m_aafBD1[j-1][i]+m_aafBD0[j-1][i])*fInvD0;
			m_aafBD1[j][i-j] = (fN1*m_aafBD1[j-1][i-j+1]-m_aafBD0[j-1][i-j+1])
				*fInvD1;

			if( uiOrder >= 2 ) {
				m_aafBD2[j][i] = (fN0*m_aafBD2[j-1][i] +
					((float)2.0)*m_aafBD1[j-1][i])*fInvD0;
				m_aafBD2[j][i-j] = (fN1*m_aafBD2[j-1][i-j+1] -
					((float)2.0)*m_aafBD1[j-1][i-j+1])*fInvD1;

				if ( uiOrder >= 3 ) {
					m_aafBD3[j][i] = (fN0*m_aafBD3[j-1][i] +
						((float)3.0)*m_aafBD2[j-1][i])*fInvD0;
					m_aafBD3[j][i-j] = (fN1*m_aafBD3[j-1][i-j+1] -
						((float)3.0)*m_aafBD2[j-1][i-j+1])*fInvD1;
				}
			}
		}
    }

    for( j = 2; j <= mDegree; j++ ) {
        for( int k = i-j+1; k < i; k++ ) {
            fN0 = fTime-mKnots[k];
            fN1 = mKnots[k+j+1]-fTime;
            fInvD0 = ((float)1.0)/(mKnots[k+j]-mKnots[k]);
            fInvD1 = ((float)1.0)/(mKnots[k+j+1]-mKnots[k+1]);

            m_aafBD0[j][k] = fN0*m_aafBD0[j-1][k]*fInvD0 + fN1*
                m_aafBD0[j-1][k+1]*fInvD1;

            if( uiOrder >= 1 ) {
				m_aafBD1[j][k] = (fN0*m_aafBD1[j-1][k]+m_aafBD0[j-1][k])*
					fInvD0 + (fN1*m_aafBD1[j-1][k+1]-m_aafBD0[j-1][k+1])*
					fInvD1;

				if( uiOrder >= 2 ) {
					m_aafBD2[j][k] = (fN0*m_aafBD2[j-1][k] +
						((float)2.0)*m_aafBD1[j-1][k])*fInvD0 +
						(fN1*m_aafBD2[j-1][k+1]- ((float)2.0)*
						m_aafBD1[j-1][k+1])*fInvD1;

                    if( uiOrder >= 3 ) {
						m_aafBD3[j][k] = (fN0*m_aafBD3[j-1][k] +
							((float)3.0)*m_aafBD2[j-1][k])*fInvD0 +
							(fN1*m_aafBD3[j-1][k+1] - ((float)3.0)*
							m_aafBD2[j-1][k+1])*fInvD1;
                    }
                }
            }
        }
    }

    riMinIndex = i - mDegree;
    riMaxIndex = i;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Integration functions for arc length
template<typename T>
typename T::TYPE getSpeedWithData( float fTime, void* pvData)
{
    return ((BSpline<T>*)pvData)->getSpeed( fTime );
}

template<typename Real>
Real rombergIntegral( int iOrder, Real fA, Real fB, Real (*oF)(Real,void*), void* pvUserData )
{
    assert(iOrder > 0);
    Real** aafRom;
    //Allocate<Real>(iOrder,2,aafRom);
    allocate2D<Real>( iOrder, 2, aafRom );

    Real fH = fB - fA;

    aafRom[0][0] = ((Real)0.5)*fH*(oF(fA,pvUserData)+oF(fB,pvUserData));
    for (int i0=2, iP0=1; i0 <= iOrder; i0++, iP0 *= 2, fH *= (Real)0.5)
    {
        // approximations via the trapezoid rule
        Real fSum = (Real)0.0;
        int i1;
        for (i1 = 1; i1 <= iP0; i1++)
        {
            fSum += oF(fA + fH*(i1-((Real)0.5)),pvUserData);
        }

        // Richardson extrapolation
        aafRom[1][0] = ((Real)0.5)*(aafRom[0][0] + fH*fSum);
        for (int i2 = 1, iP2 = 4; i2 < i0; i2++, iP2 *= 4)
        {
            aafRom[1][i2] = (iP2*aafRom[1][i2-1] - aafRom[0][i2-1])/(iP2-1);
        }

        for (i1 = 0; i1 < i0; i1++)
        {
            aafRom[0][i1] = aafRom[1][i1];
        }
    }

    Real fResult = aafRom[0][iOrder-1];
    deallocate2D<Real>( aafRom );
    return fResult;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// BSpline
template<typename T>
BSpline<T>::BSpline( const std::vector<T> &points, int degree, bool loop, bool open )
    : mLoop( loop )
{
	assert(points.size() >= 2);
	assert( ( 1 <= degree ) && ( degree <= (int)points.size() - 1 ) );

	mNumCtrlPoints = (int)points.size();
	mReplicate = ( mLoop ? (open ? 1 : degree) : 0);
	createControl( &points[0] );
	mBasis.create( mNumCtrlPoints + mReplicate, degree, open );
}

template<typename T>
BSpline<T>::BSpline( int numControlPoints, const T *controlPoints, int degree, bool loop, const float *knots )
    : mNumCtrlPoints( numControlPoints ), mLoop( loop )
{
	assert( mNumCtrlPoints >= 2);
	assert( ( 1 <= degree ) && ( degree <= mNumCtrlPoints - 1 ) );

	mReplicate = (mLoop ? 1 : 0);
	createControl( controlPoints );
	mBasis.create( mNumCtrlPoints + mReplicate, degree, knots );
}

template<typename T>
BSpline<T>::BSpline( const BSpline &bspline )
	: mCtrlPoints( 0 )
{
	*this = bspline;
}

template<typename T>
BSpline<T>& BSpline<T>::operator=( const BSpline &bspline )
{
	delete [] mCtrlPoints;

	mNumCtrlPoints = bspline.mNumCtrlPoints;
	mLoop = bspline.mLoop;
	mBasis = bspline.mBasis;
	mReplicate = bspline.mReplicate;
	
	if( mNumCtrlPoints > 0 )
		createControl( bspline.mCtrlPoints );
	else
		mCtrlPoints = 0;
	
	return *this;
}


template<typename T>
BSpline<T>::~BSpline()
{
	delete [] mCtrlPoints;
}

template<typename T>
void BSpline<T>::createControl( const T* akCtrlPoint )
{
	int iNewNumCtrlPoints = mNumCtrlPoints + mReplicate;
	mCtrlPoints = new T[iNewNumCtrlPoints];
	size_t uiSrcSize = mNumCtrlPoints*sizeof(T);
	memcpy( mCtrlPoints, akCtrlPoint, uiSrcSize );
	for( int i = 0; i < mReplicate; i++ ) {
		mCtrlPoints[mNumCtrlPoints+i] = akCtrlPoint[i];
	}
}

template<typename T>
void BSpline<T>::setControlPoint( int i, const T& rkCtrl )
{
	if( ( 0 <= i ) && ( i < mNumCtrlPoints ) ) {
		// set the control point
		mCtrlPoints[i] = rkCtrl;

		// set the replicated control point
		if( i < mReplicate ) {
			mCtrlPoints[mNumCtrlPoints+i] = rkCtrl;
		}
	}
}

template<typename T>
T BSpline<T>::getControlPoint( int i ) const
{
    if( ( 0 <= i ) && ( i < mNumCtrlPoints) ) {
		return mCtrlPoints[i];
    }

    return T::max();
}

template<typename T>
void BSpline<T>::setKnot( int i, float fKnot )
{
    mBasis.setKnot( i, fKnot );
}

template<typename T>
float BSpline<T>::getKnot( int i ) const
{
    return mBasis.getKnot( i );
}

template<typename T>
void BSpline<T>::get( float t, T *position, T *firstDerivative, T *secondDerivative, T *thirdDerivative ) const
{
	int i, iMin, iMax;
	if( thirdDerivative ) {
		mBasis.compute( t, 3, iMin, iMax );
	}
	else if( secondDerivative ) {
		mBasis.compute( t, 2, iMin, iMax );
	}
	else if( firstDerivative ) {
		mBasis.compute( t, 1, iMin, iMax );
	}
	else {
        mBasis.compute( t, 0, iMin, iMax );
	}

	if( position ) {
		*position = T::zero();
		for( i = iMin; i <= iMax; i++ ) {
			float weight = mBasis.getD0( i );
			*position += mCtrlPoints[i] * weight;
		}
	}

	if( firstDerivative ) {
		*firstDerivative = T::zero();
		for( i = iMin; i <= iMax; i++ ) {
			*firstDerivative += mCtrlPoints[i]*mBasis.getD1( i );
		}
	}

	if( secondDerivative ) {
		*secondDerivative = T::zero();
		for( i = iMin; i <= iMax; i++ ) {
			*secondDerivative += mCtrlPoints[i]*mBasis.getD2( i );
		}
	}

	if( thirdDerivative ) {
		*thirdDerivative = T::zero();
		for (i = iMin; i <= iMax; i++) {
			*thirdDerivative += mCtrlPoints[i]*mBasis.getD3( i );
		}
	}
}

template<typename T>
float BSpline<T>::getTime( float length ) const
{
	const size_t MAX_ITERATIONS = 32;
	const float TOLERANCE = 1.0e-03f;
	// ensure that we remain within valid parameter space
	float totalLength = getLength( 0, 1 );
	if( length >= totalLength )
		return 1;
	if( length <= 0 )
		return 0;

	// initialize bisection endpoints
	float a = 0, b = 1;
	float p = length / totalLength;    // make first guess

	// iterate and look for zeros
	for ( size_t i = 0; i < MAX_ITERATIONS; ++i ) {
		// compute function value and test against zero
		float func = getLength( 0, p ) - length;
		if( math<float>::abs( func ) < TOLERANCE ) {
			return p;
		}

		 // update bisection endpoints
		if( func < 0 ) {
			a = p;
		}
		else {
			b = p;
		}

		// get speed along curve
		float speed = getSpeed( p );

		// if result will lie outside [a,b] 
		if( ((p-a)*speed - func)*((p-b)*speed - func) > -TOLERANCE ) {
			// do bisection
			p = 0.5f*(a+b);
		}    
		else {
			// otherwise Newton-Raphson
			p -= func/speed;
		}
	}
	
	// We failed to converge, but hopefully 'p' is close enough anyway
	return p;
}

template<typename T>
float BSpline<T>::getLength( float fT0, float fT1 ) const
{
	if( fT0 >= fT1 )
		return (float)0.0;

    return rombergIntegral<typename T::TYPE>( 10, fT0, fT1, getSpeedWithData<T>, (void*)this );
}

template<typename T>
BSplineBasis& BSpline<T>::getBasis()
{
	return mBasis;
}

template<typename T>
T BSpline<T>::getPosition( float t ) const
{
	T kPos;
	get( t, &kPos, 0, 0, 0 );
	return kPos;
}

template<typename T>
T BSpline<T>::getDerivative( float t ) const
{
	T kDer1;
	get( t, 0, &kDer1, 0, 0 );
	return kDer1;
}

template<typename T>
T BSpline<T>::getSecondDerivative( float t ) const
{
	T kDer2;
	get( t, 0, 0, &kDer2, 0 );
	return kDer2;
}

template<typename T>
T BSpline<T>::getThirdDerivative( float t ) const
{
	T kDer3;
	get( t, 0, 0, 0, &kDer3 );
	return kDer3;
}

// explicit template instantiations
template class BSpline<Vec2f>;
template class BSpline<Vec3f>;
template class BSpline<Vec4f>;

} // namespace cinder
