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
    create( aNumCtrlPoints, iDegree, afKnot );
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
// BSpline
template<int D,typename T>
BSpline<D,T>::BSpline( const std::vector<VecT> &points, int degree, bool loop, bool open )
    : mLoop( loop )
{
	assert(points.size() >= 2);
	assert( ( 1 <= degree ) && ( degree <= (int)points.size() - 1 ) );

	mNumCtrlPoints = (int)points.size();
	mReplicate = ( mLoop ? (open ? 1 : degree) : 0);
	createControl( &points[0] );
	mBasis.create( mNumCtrlPoints + mReplicate, degree, open );
}

template<int D,typename T>
BSpline<D,T>::BSpline( int numControlPoints, const typename BSpline<D,T>::VecT *controlPoints, int degree, bool loop, const float *knots )
    : mNumCtrlPoints( numControlPoints ), mLoop( loop )
{
	assert( mNumCtrlPoints >= 2);
	assert( ( 1 <= degree ) && ( degree <= mNumCtrlPoints - 1 ) );

	mReplicate = (mLoop ? 1 : 0);
	createControl( controlPoints );
	mBasis.create( mNumCtrlPoints + mReplicate, degree, knots );
}

template<int D,typename T>
BSpline<D,T>::BSpline( const BSpline &bspline )
	: mCtrlPoints( 0 )
{
	*this = bspline;
}

template<int D,typename T>
BSpline<D,T>& BSpline<D,T>::operator=( const BSpline &bspline )
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


template<int D,typename T>
BSpline<D,T>::~BSpline()
{
	delete [] mCtrlPoints;
}

template<int D,typename T>
void BSpline<D,T>::createControl( const VecT *akCtrlPoint )
{
	int iNewNumCtrlPoints = mNumCtrlPoints + mReplicate;
	mCtrlPoints = new VecT[iNewNumCtrlPoints];
	size_t uiSrcSize = mNumCtrlPoints * sizeof( VecT );
	memcpy( mCtrlPoints, akCtrlPoint, uiSrcSize );
	for( int i = 0; i < mReplicate; i++ ) {
		mCtrlPoints[mNumCtrlPoints+i] = akCtrlPoint[i];
	}
}

template<int D,typename T>
void BSpline<D,T>::setControlPoint( int i, const VecT &rkCtrl )
{
	assert( i >= 0 && i < mNumCtrlPoints );

	// set the control point
	mCtrlPoints[i] = rkCtrl;

	// set the replicated control point
	if( i < mReplicate ) {
		mCtrlPoints[mNumCtrlPoints+i] = rkCtrl;
	}
}

template<int D,typename T>
typename BSpline<D,T>::VecT BSpline<D,T>::getControlPoint( int i ) const
{
	assert( i >= 0 && i < mNumCtrlPoints );

	return mCtrlPoints[i];
}

template<int D,typename T>
void BSpline<D,T>::setKnot( int i, float fKnot )
{
    mBasis.setKnot( i, fKnot );
}

template<int D,typename T>
float BSpline<D,T>::getKnot( int i ) const
{
    return mBasis.getKnot( i );
}

template<int D,typename T>
void BSpline<D,T>::get( float t, VecT *position, VecT *firstDerivative,	VecT *secondDerivative, VecT *thirdDerivative ) const
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
		*position = VecT();
		for( i = iMin; i <= iMax; i++ ) {
			float weight = mBasis.getD0( i );
			*position += mCtrlPoints[i] * weight;
		}
	}

	if( firstDerivative ) {
		*firstDerivative = VecT();
		for( i = iMin; i <= iMax; i++ ) {
			*firstDerivative += mCtrlPoints[i]*mBasis.getD1( i );
		}
	}

	if( secondDerivative ) {
		*secondDerivative = VecT();
		for( i = iMin; i <= iMax; i++ ) {
			*secondDerivative += mCtrlPoints[i]*mBasis.getD2( i );
		}
	}

	if( thirdDerivative ) {
		*thirdDerivative = VecT();
		for (i = iMin; i <= iMax; i++) {
			*thirdDerivative += mCtrlPoints[i]*mBasis.getD3( i );
		}
	}
}

template<int D,typename T>
float BSpline<D,T>::getTime( float length ) const
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

template<int D,typename T>
float BSpline<D,T>::getLength( float fT0, float fT1 ) const
{
	if( fT0 >= fT1 )
		return (float)0.0;

    return rombergIntegral<T,10>( fT0, fT1, std::bind( &BSpline<D,T>::getSpeed, this, std::placeholders::_1 ) );
}

template<int D,typename T>
BSplineBasis& BSpline<D,T>::getBasis()
{
	return mBasis;
}

template<int D,typename T>
typename BSpline<D,T>::VecT BSpline<D,T>::getPosition( float t ) const
{
	VecT pos;
	get( t, &pos, 0, 0, 0 );
	return pos;
}

template<int D,typename T>
typename BSpline<D,T>::VecT BSpline<D,T>::getDerivative( float t ) const
{
	VecT d1;
	get( t, 0, &d1, 0, 0 );
	return d1;
}

template<int D,typename T>
typename BSpline<D,T>::VecT BSpline<D,T>::getSecondDerivative( float t ) const
{
	VecT d2;
	get( t, 0, 0, &d2, 0 );
	return d2;
}

template<int D,typename T>
typename BSpline<D,T>::VecT BSpline<D,T>::getThirdDerivative( float t ) const
{
	VecT d3;
	get( t, 0, 0, 0, &d3 );
	return d3;
}

template<int D,typename T>
T BSpline<D,T>::getSpeed( float t ) const
{
	return length( getDerivative( t ) );
}

// explicit template instantiations
template class CI_API BSpline<2,float>;
template class CI_API BSpline<3,float>;
template class CI_API BSpline<4,float>;

} // namespace cinder
