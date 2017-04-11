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

/*
 Geometric Tools, LLC
 Portions Copyright (c) 1998-2010
 Distributed under the Boost Software License, Version 1.0.
 http://www.boost.org/LICENSE_1_0.txt
 http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
*/

#include "cinder/BandedMatrix.h"

#include <assert.h>
#include <memory.h>

namespace cinder {

template<typename T>
BandedMatrix<T>::BandedMatrix( int iSize, int iLBands, int iUBands )
{
	assert(iSize > 0 && iLBands >= 0 && iUBands >= 0);
	assert(iLBands < iSize && iUBands < iSize);

	m_iSize = iSize;
	m_iLBands = iLBands;
	m_iUBands = iUBands;
	allocate();
}

template<typename T>
BandedMatrix<T>::BandedMatrix( const BandedMatrix& rkM )
{
	m_afDBand = 0;
	m_aafLBand = 0;
	m_aafUBand = 0;
	*this = rkM;
}

template<typename T>
BandedMatrix<T>::~BandedMatrix()
{
	deallocate();
}

template<typename T>
BandedMatrix<T>& BandedMatrix<T>::operator=( const BandedMatrix& rkM)
{
	deallocate();
	m_iSize = rkM.m_iSize;
	m_iLBands = rkM.m_iLBands;
	m_iUBands = rkM.m_iUBands;
	allocate();

	size_t uiSize = m_iSize*sizeof(T);
	memcpy( m_afDBand, rkM.m_afDBand, uiSize );

	int i;
	for (i = 0; i < m_iLBands; i++) {
		uiSize = (m_iSize-1-i)*sizeof(T);
		memcpy( m_aafLBand[i], rkM.m_aafLBand[i], uiSize );
	}

	for (i = 0; i < m_iUBands; i++) {
		uiSize = (m_iSize-1-i)*sizeof(T);
		memcpy( m_aafUBand[i], rkM.m_aafUBand[i], uiSize );
	}

	return *this;
}

template<typename T>
int BandedMatrix<T>::getSize() const
{
    return m_iSize;
}

template<typename T>
int BandedMatrix<T>::getLBands() const
{
    return m_iLBands;
}

template<typename T>
int BandedMatrix<T>::getUBands() const
{
    return m_iUBands;
}

template<typename T>
T* BandedMatrix<T>::getDBand()
{
    return m_afDBand;
}

template<typename T>
const T* BandedMatrix<T>::getDBand() const
{
    return m_afDBand;
}

template<typename T>
int BandedMatrix<T>::getLBandMax( int i ) const
{
    assert(0 <= i && i < m_iLBands);
    return m_iSize-1-i;
}

template<typename T>
T* BandedMatrix<T>::getLBand( int i )
{
    if ( m_aafLBand )
    {
        assert(0 <= i && i < m_iLBands);
        return m_aafLBand[i];
    }
    return 0;
}

template<typename T>
const T* BandedMatrix<T>::getLBand( int i ) const
{
    if (m_aafLBand)
    {
        assert(0 <= i && i < m_iLBands);
        return m_aafLBand[i];
    }
    return 0;
}

template<typename T>
int BandedMatrix<T>::getUBandMax( int i ) const
{
    assert(0 <= i && i < m_iUBands);
    return m_iSize-1-i;
}

template<typename T>
T* BandedMatrix<T>::getUBand( int i )
{
    if (m_aafUBand)
    {
        assert(0 <= i && i < m_iUBands);
        return m_aafUBand[i];
    }
    return 0;
}

template<typename T>
const T* BandedMatrix<T>::getUBand( int i ) const
{
    if (m_aafUBand)
    {
        assert(0 <= i && i < m_iUBands);
        return m_aafUBand[i];
    }
    return 0;
}

template<typename T>
T& BandedMatrix<T>::operator()( int iRow, int iCol )
{
    assert(0 <= iRow && iRow < m_iSize && 0 <= iCol && iCol < m_iSize);

    int iBand = iCol - iRow;
    if (iBand > 0)
    {
        if (--iBand < m_iUBands && iRow < m_iSize-1-iBand)
        {
            return m_aafUBand[iBand][iRow];
        }
    }
    else if ( iBand < 0 )
    {
        iBand = -iBand;
        if (--iBand < m_iLBands && iCol < m_iSize-1-iBand)
        {
            return m_aafLBand[iBand][iCol];
        }
    }
    else
    {
        return m_afDBand[iRow];
    }

    static T s_fDummy = (T)0.0;
    return s_fDummy;
}

template<typename T>
T BandedMatrix<T>::operator()( int iRow, int iCol ) const
{
    assert(0 <= iRow && iRow < m_iSize && 0 <= iCol && iCol < m_iSize);

    int iBand = iCol - iRow;
    if (iBand > 0)
    {
        if (--iBand < m_iUBands && iRow < m_iSize-1-iBand)
        {
            return m_aafUBand[iBand][iRow];
        }
    }
    else if ( iBand < 0 )
    {
        iBand = -iBand;
        if (--iBand < m_iLBands && iCol < m_iSize-1-iBand)
        {
            return m_aafLBand[iBand][iCol];
        }
    }
    else
    {
        return m_afDBand[iRow];
    }

    return (T)0.0;
}

template<typename T>
void BandedMatrix<T>::setZero()
{
    assert(m_iSize > 0);

    memset(m_afDBand,0,m_iSize*sizeof(T));

    int i;
    for (i = 0; i < m_iLBands; i++)
    {
        memset(m_aafLBand[i],0,(m_iSize-1-i)*sizeof(T));
    }

    for (i = 0; i < m_iUBands; i++)
    {
        memset(m_aafUBand[i],0,(m_iSize-1-i)*sizeof(T));
    }
}

template<typename T>
void BandedMatrix<T>::setIdentity()
{
    assert(m_iSize > 0);

    int i;
    for (i = 0; i < m_iSize; i++)
    {
        m_afDBand[i] = (T)1.0;
    }

    for (i = 0; i < m_iLBands; i++)
    {
        memset(m_aafLBand[i],0,(m_iSize-1-i)*sizeof(T));
    }

    for (i = 0; i < m_iUBands; i++)
    {
        memset(m_aafUBand[i],0,(m_iSize-1-i)*sizeof(T));
    }
}

template<typename T>
void BandedMatrix<T>::allocate()
{
    // assert:  m_iSize, m_iLBands, m_iRBandQuantity already set
    // assert:  m_afDBand, m_aafLBand, m_aafUBand all null

    m_afDBand = new T[m_iSize];
    memset( m_afDBand,0,m_iSize*sizeof(T) );

    if( m_iLBands > 0 )
    {
        m_aafLBand = new T*[m_iLBands];
    }
    else
    {
        m_aafLBand = 0;
    }

    if (m_iUBands > 0)
    {
        m_aafUBand = new T*[m_iUBands];
    }
    else
    {
        m_aafUBand = 0;
    }

    int i;
    for( i = 0; i < m_iLBands; i++ ) {
        m_aafLBand[i] = new T[m_iSize-1-i];
        memset(m_aafLBand[i],0,(m_iSize-1-i)*sizeof(T));
    }

    for (i = 0; i < m_iUBands; i++) {
        m_aafUBand[i] = new T[m_iSize-1-i];
        memset( m_aafUBand[i],0,(m_iSize-1-i)*sizeof(T) );
    }
}

template<typename T>
void BandedMatrix<T>::deallocate()
{
    delete [] m_afDBand;

    int i;

    if( m_aafLBand ) {
        for (i = 0; i < m_iLBands; i++)
        {
            delete [] m_aafLBand[i];
        }

        delete [] m_aafLBand;
        m_aafLBand = 0;
    }

    if( m_aafUBand ) {
        for (i = 0; i < m_iUBands; i++) {
            delete [] m_aafUBand[i];
        }

        delete [] m_aafUBand;
        m_aafUBand = 0;
    }
}

template class CI_API BandedMatrix<float>;
template class CI_API BandedMatrix<double>;

} // namespace cinder
