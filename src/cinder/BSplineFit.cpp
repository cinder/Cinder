/*
 Portions Copyright (c) 2010, The Barbarian Group
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

/*
 Geometric Tools, LLC
 Portions Copyright (c) 1998-2010
 Distributed under the Boost Software License, Version 1.0.
 http://www.boost.org/LICENSE_1_0.txt
 http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
*/


#include "cinder/BSplineFit.h"
#include "cinder/CinderMath.h"
#include "cinder/Vector.h"
#include "cinder/BSpline.h"

#include <string.h>
#include <assert.h>

using std::vector;

namespace cinder {

template<typename T>
class BSplineFitBasis {
 public: 
	// Construction and destruction.  This class is only for open uniform
	// B-spline basis functions.  The input is the number of control points
	// for a B-spline curve using this basis and the degree of that curve.
	BSplineFitBasis( int iQuantity, int iDegree );
	~BSplineFitBasis();

	// Data member access.
	int getQuantity() const;
	int getDegree() const;

	// Evaluate the basis functions.  This function fills in the values
	// returned by GetValue(i) for 0 <= i <= degree.  The return indices iMin
	// and iMax are relative to the array of control points.  The GetValue(i)
	// are the coefficients for the control points ctrl[iMin] throught
	// ctrl[iMax] in the curve evaluation (i.e. the curve has local control).
	void compute( T fT, int &iMin, int &iMax ) const;
	T getValue( int i ) const;

 private:
	// The number of control points and degree for the curve.
	int m_iQuantity, m_iDegree;

	// The storage for knots and basis evaluation.
	mutable T* m_afValue;  // m_afValue[0..degree]
	mutable T* m_afKnot;   // m_afKnot[2*degree]
};

template<typename T>
class BSplineFit {
 public:
	// Construction and destruction.  The preconditions for calling the
	// constructor are
	//   1 <= iDegree && iDegree < iControlQuantity <= iSampleQuantity
	// The samples point are contiguous blocks of iDimension real value
	// stored in afSampleData.
	BSplineFit( int iDimension, int iSampleQuantity, const T* afSampleData, int iDegree, int iControlQuantity );
	~BSplineFit();

	// Access to input sample information.
	int getDimension() const;
	int getSampleQuantity() const;
	const T* getSampleData() const;

	// Access to output control point and curve information.
	int getDegree() const;
	int getControlQuantity() const;
	const T* getControlData() const;
	const BSplineFitBasis<T>& getBasis() const;

	// Evaluation of the B-spline curve.  It is defined for 0 <= t <= 1.  If
	// a t-value is outside [0,1], an open spline clamps it to [0,1].  The
	// caller must ensure that afPosition[] has (at least) 'dimension'
	// elements.
	void getPosition( T fT, T *afPosition ) const;

 private:
	// The matric inversion calculations are performed with double-precision,
	// even when the type T is 'float'.
	bool choleskyFactor( BandedMatrixd &rkMatrix ) const;
	bool solveLower( BandedMatrixd &rkMatrix, double* adControlData ) const;
	bool solveUpper( BandedMatrixd &rkMatrix, double* adControlData ) const;

	// Input sample information.
	int m_iDimension;
	int m_iSampleQuantity;
	const T* m_afSampleData;

	// The fitted B-spline curve, open and with uniform knots.
	int m_iDegree;
	int m_iControlQuantity;
	T* m_afControlData;
	BSplineFitBasis<T> m_kBasis;
};

typedef BSplineFit<float> BSplineFitf;
typedef BSplineFit<double> BSplineFitd;
typedef BSplineFitBasis<float> BSplineFitBasisf;
typedef BSplineFitBasis<double> BSplineFitBasisd;

///////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
BSplineFitBasis<T>::BSplineFitBasis(int iQuantity, int iDegree)
{
    assert(1 <= iDegree && iDegree < iQuantity);
    m_iQuantity = iQuantity;
    m_iDegree = iDegree;

    m_afValue = new T[iDegree+1];
    m_afKnot = new T[2*iDegree];
}

template<typename T>
BSplineFitBasis<T>::~BSplineFitBasis()
{
    delete [] m_afValue;
    delete [] m_afKnot;
}

template<typename T>
int BSplineFitBasis<T>::getQuantity() const
{
    return m_iQuantity;
}

template<typename T>
int BSplineFitBasis<T>::getDegree() const
{
    return m_iDegree;
}

template<typename T>
void BSplineFitBasis<T>::compute( T fTime, int &iMin, int &iMax ) const
{
    assert((T)0.0 <= fTime && fTime <= (T)1.0);

    // Use scaled time and scaled knots so that 1/(Q-D) does not need to
    // be explicitly stored by the class object.  Determine the extreme
    // indices affected by local control.
    T fQmD = (T)(m_iQuantity - m_iDegree);
    T fT;
    if (fTime <= (T)0.0)
    {
        fT = (T)0.0;
        iMin = 0;
        iMax = m_iDegree;
    }
    else if (fTime >= (T)1.0)
    {
        fT = fQmD;
        iMax = m_iQuantity - 1;
        iMin = iMax - m_iDegree;
    }
    else
    {
        fT = fQmD*fTime;
        iMin = (int)fT;
        iMax = iMin + m_iDegree;
    }

    // Precompute the knots.
    for (int i0 = 0, i1 = iMax+1-m_iDegree; i0 < 2*m_iDegree; i0++, i1++)
    {
        if (i1 <= m_iDegree)
        {
            m_afKnot[i0] = (T)0.0f;
        }
        else if (i1 >= m_iQuantity)
        {
            m_afKnot[i0] = fQmD;
        }
        else
        {
            m_afKnot[i0] = (T)(i1 - m_iDegree);
        }
    }

    // Initialize the basis function evaluation table.  The first degree-1
    // entries are zero, but they do not have to be set explicitly.
    m_afValue[m_iDegree] = (T)1.0;

    // Update the basis function evaluation table, each iteration overwriting
    // the results from the previous iteration.
    for (int iRow = m_iDegree-1; iRow >= 0; iRow--)
    {
        int iK0 = m_iDegree, iK1 = iRow;
        T fKnot0 = m_afKnot[iK0], fKnot1 = m_afKnot[iK1];
        T fInvDenom = ((T)1.0)/(fKnot0 - fKnot1);
        T fC1 = (fKnot0 - fT)*fInvDenom, fC0;
        m_afValue[iRow] = fC1*m_afValue[iRow+1];

        for (int iCol = iRow+1; iCol < m_iDegree; iCol++)
        {
            fC0 = (fT - fKnot1)*fInvDenom;
            m_afValue[iCol] *= fC0;

            fKnot0 = m_afKnot[++iK0];
            fKnot1 = m_afKnot[++iK1];
            fInvDenom = ((T)1.0)/(fKnot0 - fKnot1);
            fC1 = (fKnot0 - fT)*fInvDenom;
            m_afValue[iCol] += fC1*m_afValue[iCol+1];
        }

        fC0 = (fT - fKnot1)*fInvDenom;
        m_afValue[m_iDegree] *= fC0;
    }
}

template<typename T>
T BSplineFitBasis<T>::getValue( int i ) const
{
    assert(0 <= i && i <= m_iDegree);
    return m_afValue[i];
}

//----------------------------------------------------------------------------

template<typename T>
BSplineFit<T>::BSplineFit( int iDimension, int iSampleQuantity, const T* afSampleData, int iDegree, int iControlQuantity )
    : m_kBasis( iControlQuantity, iDegree )
{
	if( iControlQuantity <= iDegree + 1 ) iControlQuantity = iDegree + 2;
	if( iControlQuantity > iSampleQuantity ) iControlQuantity = iSampleQuantity;
	iDegree = constrain( iDegree, 1, iControlQuantity - 1 );	

	assert(iDimension >= 1);
	assert(1 <= iDegree && iDegree < iControlQuantity);
	assert(iControlQuantity <= iSampleQuantity);

	m_iDimension = iDimension;
	m_iSampleQuantity = iSampleQuantity;
	m_afSampleData = afSampleData;
	m_iDegree = iDegree;
	m_iControlQuantity = iControlQuantity;
	m_afControlData = new T[m_iDimension*iControlQuantity];

	// Fit the data points with a B-spline curve using a least-squares error
	// metric.  The problem is of the form A^T*A*X = A^T*B.
	BSplineFitBasisd kDBasis(m_iControlQuantity,m_iDegree);
	double dTMultiplier = 1.0/(double)(m_iSampleQuantity - 1);
	double dT;
	int i0, i1, i2, iMin, iMax, j;

	// Construct the matrix A (depends only on the output basis function).
	BandedMatrixd* pkAMat = new BandedMatrixd( m_iControlQuantity, m_iDegree+1, m_iDegree + 1 );

	for (i0 = 0; i0 < m_iControlQuantity; i0++) {
		for ( i1 = 0; i1 < i0; i1++ ) {
			(*pkAMat)(i0,i1) = (*pkAMat)(i1,i0);
		}

		int i1Max = i0 + m_iDegree;
		if (i1Max >= m_iControlQuantity)
		{
			i1Max = m_iControlQuantity - 1;
		}

		for (i1 = i0; i1 <= i1Max; i1++)
		{
			double dValue = 0.0;
			for (i2 = 0; i2 < m_iSampleQuantity; i2++)
			{
				dT = dTMultiplier*(double)i2;
				kDBasis.compute(dT,iMin,iMax);
				if (iMin <= i0 && i0 <= iMax && iMin <= i1 && i1 <= iMax)
				{
					double dB0 = kDBasis.getValue(i0 - iMin);
					double dB1 = kDBasis.getValue(i1 - iMin);
					dValue += dB0*dB1;
				}
			}
			(*pkAMat)(i0,i1) = dValue;
		}
	}

	// Construct the matrix B.
	double** aadBMat;
	aadBMat = new double*[m_iControlQuantity];
	aadBMat[0] = new double[m_iControlQuantity*m_iSampleQuantity];
	for( int iRow = 1; iRow < m_iControlQuantity; iRow++ ) {
		aadBMat[iRow] = &aadBMat[0][m_iSampleQuantity*iRow];
	}

	memset( aadBMat[0],0,m_iControlQuantity*m_iSampleQuantity*sizeof(double) );
	for (i0 = 0; i0 < m_iControlQuantity; i0++)
	{
		for (i1 = 0; i1 < m_iSampleQuantity; i1++)
		{
			dT = dTMultiplier*(double)i1;
			kDBasis.compute(dT,iMin,iMax);
			if (iMin <= i0 && i0 <= iMax)
			{
				aadBMat[i0][i1] = kDBasis.getValue(i0 - iMin);
			}
		}
	}

	// Construct the control points for the least-squares curve.
	double* adControlData = new double[m_iDimension*m_iControlQuantity];
	memset( adControlData,0,m_iDimension*m_iControlQuantity*sizeof(double) );
	double* pdBaseTarget = adControlData;
	for (i0 = 0; i0 < m_iControlQuantity; i0++)
	{
		const T* pfSource = m_afSampleData;
		double* adTarget = pdBaseTarget;
		for (i1 = 0; i1 < m_iSampleQuantity; i1++)
		{
			double dBValue = aadBMat[i0][i1];
			for (j = 0; j < m_iDimension; j++)
			{
				adTarget[j] += dBValue*(double)(*pfSource++);
			}
		}
		pdBaseTarget += m_iDimension;
	}

	// Solve A^T*A*ControlData = A^T*B*SampleData.
	bool bSolved = choleskyFactor(*pkAMat);
	assert(bSolved);
	bSolved = solveLower(*pkAMat,adControlData);
	assert(bSolved);
	bSolved = solveUpper(*pkAMat,adControlData);
	assert(bSolved);

	// Set the B-spline control points.
	T* pfTarget = m_afControlData;
	const double* pdSource = adControlData;
	for (i0 = 0; i0 < m_iDimension*m_iControlQuantity; i0++)
	{
		*pfTarget++ = (T)(*pdSource++);
	}

	// Set the first and last output control points to match the first and
	// last input samples.  This supports the application of fitting keyframe
	// data with B-spline curves.  The user expects that the curve passes
	// through the first and last positions in order to support matching two
	// consecutive keyframe sequences.
	T* pfCEnd0 = m_afControlData;
	const T* pfSEnd0 = m_afSampleData;
	T* pfCEnd1 = &m_afControlData[m_iDimension*(m_iControlQuantity-1)];
	const T* pfSEnd1 = &m_afSampleData[m_iDimension*(m_iSampleQuantity-1)];
	for (j = 0; j < m_iDimension; j++)
	{
		*pfCEnd0++ = *pfSEnd0++;
		*pfCEnd1++ = *pfSEnd1++;
	}

	delete [] adControlData;
	if( aadBMat ) {
        delete [] aadBMat[0];
        delete [] aadBMat;
        aadBMat = 0;
    }
	delete pkAMat;
}

template<typename T>
BSplineFit<T>::~BSplineFit ()
{
    delete [] m_afControlData;
}

template<typename T>
int BSplineFit<T>::getDimension() const
{
    return m_iDimension;
}

template<typename T>
int BSplineFit<T>::getSampleQuantity() const
{
    return m_iSampleQuantity;
}

template<typename T>
const T* BSplineFit<T>::getSampleData() const
{
    return m_afSampleData;
}

template<typename T>
int BSplineFit<T>::getDegree() const
{
    return m_iDegree;
}

template<typename T>
int BSplineFit<T>::getControlQuantity() const
{
    return m_iControlQuantity;
}

template<typename T>
const T* BSplineFit<T>::getControlData() const
{
    return m_afControlData;
}

template<typename T>
const BSplineFitBasis<T>& BSplineFit<T>::getBasis() const
{
    return m_kBasis;
}

template<typename T>
void BSplineFit<T>::getPosition( T fT, T* afPosition ) const
{
    assert(afPosition);

    int iMin, iMax;
    m_kBasis.compute(fT,iMin,iMax);

    T* pfSource = &m_afControlData[m_iDimension*iMin];
    T fBasisValue = m_kBasis.getValue(0);
    int j;
    for (j = 0; j < m_iDimension; j++)
    {
        afPosition[j] = fBasisValue*(*pfSource++);
    }

    for (int i = iMin+1, iIndex = 1; i <= iMax; i++, iIndex++)
    {
        fBasisValue = m_kBasis.getValue(iIndex);
        for (j = 0; j < m_iDimension; j++)
        {
            afPosition[j] += fBasisValue*(*pfSource++);
        }
    }
}

template<typename T>
bool BSplineFit<T>::choleskyFactor( BandedMatrixd &rkMatrix ) const
{
    const int iSize = rkMatrix.getSize(), iSizeM1 = iSize - 1;
    const int iBands = rkMatrix.getLBands();  // == GetUBands()

    int k, kMax;
    for (int i = 0; i < iSize; i++)
    {
        int jMin = i - iBands;
        if (jMin < 0)
        {
            jMin = 0;
        }

        int j;
        for (j = jMin; j < i; j++)
        {
            kMax = j + iBands;
            if (kMax > iSizeM1)
            {
                kMax = iSizeM1;
            }

            for (k = i; k <= kMax; k++)
            {
                rkMatrix(k,i) -= rkMatrix(i,j)*rkMatrix(k,j);
            }
        }

        kMax = j + iBands;
        if (kMax > iSizeM1)
        {
            kMax = iSizeM1;
        }

        for (k = 0; k < i; k++)
        {
            rkMatrix(k,i) = rkMatrix(i,k);
        }

        double dDiagonal = rkMatrix(i,i);
        if (dDiagonal <= 0.0)
        {
            return false;
        }
        double dInvSqrt = 1.0 / math<double>::sqrt( dDiagonal );
        for (k = i; k <= kMax; k++)
        {
            rkMatrix(k,i) *= dInvSqrt;
        }
    }

    return true;
}

template<typename T>
bool BSplineFit<T>::solveLower( BandedMatrixd& rkMatrix, double* adControlData ) const
{
    const int iSize = rkMatrix.getSize();
    double* pdBaseTarget = adControlData;
    for (int iRow = 0; iRow < iSize; iRow++)
    {
        if( math<double>::abs(rkMatrix(iRow,iRow)) < EPSILON_VALUE )
        {
            return false;
        }

        const double* pdBaseSource = adControlData;
        double* adTarget = pdBaseTarget;
        int j;
        for (int iCol = 0; iCol < iRow; iCol++)
        {
            const double* pdSource = pdBaseSource;
            double dMatValue = rkMatrix(iRow,iCol);
            for (j = 0; j < m_iDimension; j++)
            {
                adTarget[j] -= dMatValue*(*pdSource++);
            }
            pdBaseSource += m_iDimension;
        }

        double dInverse = 1.0/rkMatrix(iRow,iRow);
        for (j = 0; j < m_iDimension; j++)
        {
            adTarget[j] *= dInverse;
        }
        pdBaseTarget += m_iDimension;
    }

    return true;
}

template<typename T>
bool BSplineFit<T>::solveUpper( BandedMatrixd& rkMatrix, double *adControlData ) const
{
    const int iSize = rkMatrix.getSize();
    double* pdBaseTarget = &adControlData[m_iDimension*(iSize-1)];
    for (int iRow = iSize - 1; iRow >= 0; iRow--)
    {
        if( math<double>::abs(rkMatrix(iRow,iRow)) < EPSILON_VALUE ) {
            return false;
        }

        const double* pdBaseSource = &adControlData[m_iDimension*(iRow+1)];
        double* adTarget = pdBaseTarget;
        int j;
        for (int iCol = iRow+1; iCol < iSize; iCol++)
        {
            const double* pdSource = pdBaseSource;
            double dMatValue = rkMatrix(iRow,iCol);
            for (j = 0; j < m_iDimension; j++)
            {
                adTarget[j] -= dMatValue*(*pdSource++);
            }
            pdBaseSource += m_iDimension;
        }

        double dInverse = 1.0/rkMatrix(iRow,iRow);
        for (j = 0; j < m_iDimension; j++)
        {
            adTarget[j] *= dInverse;
        }
        pdBaseTarget -= m_iDimension;
    }

    return true;
}

template<int D, typename T>
BSpline<D, T> fitBSpline( const vector<typename BSpline<D, T>::VecT> &samples, int degree, int outputSamples )
{
	typedef typename BSpline<D, T>::VecT VecType;

	BSplineFit<T> fit( D, (int)samples.size(), &(samples[0].x), degree, outputSamples );

	vector<VecType> points;
	for( int c = 0; c < fit.getControlQuantity(); ++c ) {
		const T *vp = &fit.getControlData()[c * D];
		VecType vec;
		for( glm::length_t i = 0; i < D; ++i )
			vec[i] = vp[i];

		points.push_back( vec );
	}
	return BSpline<D, T>( points, fit.getDegree(), false, true );
}

template class BSplineFit<float>;
template class BSplineFit<double>;
template class BSplineFitBasis<float>;
template class BSplineFitBasis<double>;

template CI_API BSpline<2, float> fitBSpline( const std::vector<vec2> &samples, int degree, int outputSamples );
template CI_API BSpline<3, float> fitBSpline( const std::vector<vec3> &samples, int degree, int outputSamples );
template CI_API BSpline<4, float> fitBSpline( const std::vector<vec4> &samples, int degree, int outputSamples );

} // namespace cinder