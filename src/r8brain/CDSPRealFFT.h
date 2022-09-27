//$ nobt
//$ nocpp

/**
 * @file CDSPRealFFT.h
 *
 * @brief Real-valued FFT transform class.
 *
 * This file includes FFT object implementation. All created FFT objects are
 * kept in a global list after use, for a future reusal. Such approach
 * minimizes time necessary to initialize the FFT object of the required
 * length.
 *
 * r8brain-free-src Copyright (c) 2013-2022 Aleksey Vaneev
 * See the "LICENSE" file for license.
 */

#ifndef R8B_CDSPREALFFT_INCLUDED
#define R8B_CDSPREALFFT_INCLUDED

#include "r8bbase.h"

#if !R8B_IPP && !R8B_PFFFT && !R8B_PFFFT_DOUBLE
	#include "fft4g.h"
#endif // !R8B_IPP && !R8B_PFFFT && !R8B_PFFFT_DOUBLE

#if R8B_PFFFT
	#include "pffft.h"
#endif // R8B_PFFFT

#if R8B_PFFFT_DOUBLE
	#include "pffft_double/pffft_double.h"
#endif // R8B_PFFFT_DOUBLE

namespace r8b {

/**
 * @brief Real-valued FFT transform class.
 *
 * Class implements a wrapper for real-valued discrete fast Fourier transform
 * functions. The object of this class can only be obtained via the
 * CDSPRealFFTKeeper class.
 *
 * Uses functions from the FFT package by: Copyright(C) 1996-2001 Takuya OOURA
 * http://www.kurims.kyoto-u.ac.jp/~ooura/fft.html
 *
 * Also uses Intel IPP library functions if available (if the R8B_IPP=1 macro
 * was defined). Note that IPP library's FFT functions are 2-3 times more
 * efficient on the modern Intel Core i7-3770K processor than Ooura's
 * functions. It may be worthwhile investing in IPP. Note, that FFT functions
 * take less than 20% of the overall sample rate conversion time. However,
 * when the "power of 2" resampling is used the performance of FFT functions
 * becomes "everything".
 */

class CDSPRealFFT : public R8B_BASECLASS
{
	R8BNOCTOR( CDSPRealFFT );

	friend class CDSPRealFFTKeeper;

public:
	/**
	 * @return A multiplication constant that should be used after inverse
	 * transform to obtain a correct value scale.
	 */

	double getInvMulConst() const
	{
		return( InvMulConst );
	}

	/**
	 * @return The length (the number of real values in a transform) of *this
	 * FFT object, expressed as Nth power of 2.
	 */

	int getLenBits() const
	{
		return( LenBits );
	}

	/**
	 * @return The length (the number of real values in a transform) of *this
	 * FFT object.
	 */

	int getLen() const
	{
		return( Len );
	}

	/**
	 * Function performs in-place forward FFT.
	 *
	 * @param[in,out] p Pointer to data block to transform, length should be
	 * equal to *this object's getLen().
	 */

	void forward( double* const p ) const
	{
	#if R8B_FLOATFFT

		float* const op = (float*) p;
		int i;

		for( i = 0; i < Len; i++ )
		{
			op[ i ] = (float) p[ i ];
		}

	#endif // R8B_FLOATFFT

	#if R8B_IPP

		ippsFFTFwd_RToPerm_64f( p, p, SPtr, WorkBuffer );

	#elif R8B_PFFFT

		pffft_transform_ordered( setup, op, op, work, PFFFT_FORWARD );

	#elif R8B_PFFFT_DOUBLE

		pffftd_transform_ordered( setup, p, p, work, PFFFT_FORWARD );

	#else // R8B_PFFFT_DOUBLE

		ooura_fft :: rdft( Len, 1, p, wi.getPtr(), wd.getPtr() );

	#endif // R8B_IPP
	}

	/**
	 * Function performs in-place inverse FFT.
	 *
	 * @param[in,out] p Pointer to data block to transform, length should be
	 * equal to *this object's getLen().
	 */

	void inverse( double* const p ) const
	{
	#if R8B_IPP

		ippsFFTInv_PermToR_64f( p, p, SPtr, WorkBuffer );

	#elif R8B_PFFFT

		pffft_transform_ordered( setup, (float*) p, (float*) p, work,
			PFFFT_BACKWARD );

	#elif R8B_PFFFT_DOUBLE

		pffftd_transform_ordered( setup, p, p, work, PFFFT_BACKWARD );

	#else // R8B_PFFFT_DOUBLE

		ooura_fft :: rdft( Len, -1, p, wi.getPtr(), wd.getPtr() );

	#endif // R8B_IPP

	#if R8B_FLOATFFT

		const float* const ip = (const float*) p;
		int i;

		for( i = Len - 1; i >= 0; i-- )
		{
			p[ i ] = ip[ i ];
		}

	#endif // R8B_FLOATFFT
	}

	/**
	 * Function multiplies two complex-valued data blocks and places result in
	 * a new data block. Length of all data blocks should be equal to *this
	 * object's block length. Input blocks should have been produced with the
	 * forward() function of *this object.
	 *
	 * @param aip1 Input data block 1.
	 * @param aip2 Input data block 2.
	 * @param[out] aop Output data block, should not be equal to aip1 nor
	 * aip2.
	 */

	void multiplyBlocks( const double* const aip1, const double* const aip2,
		double* const aop ) const
	{
	#if R8B_FLOATFFT

		const float* const ip1 = (const float*) aip1;
		const float* const ip2 = (const float*) aip2;
		float* const op = (float*) aop;

	#else // R8B_FLOATFFT

		const double* const ip1 = aip1;
		const double* const ip2 = aip2;
		double* const op = aop;

	#endif // R8B_FLOATFFT

	#if R8B_IPP

		ippsMulPerm_64f( (Ipp64f*) ip1, (Ipp64f*) ip2, (Ipp64f*) op, Len );

	#else // R8B_IPP

		op[ 0 ] = ip1[ 0 ] * ip2[ 0 ];
		op[ 1 ] = ip1[ 1 ] * ip2[ 1 ];

		int i = 2;

		while( i < Len )
		{
			op[ i ] = ip1[ i ] * ip2[ i ] - ip1[ i + 1 ] * ip2[ i + 1 ];
			op[ i + 1 ] = ip1[ i ] * ip2[ i + 1 ] + ip1[ i + 1 ] * ip2[ i ];
			i += 2;
		}

	#endif // R8B_IPP
	}

	/**
	 * Function multiplies two complex-valued data blocks in-place. Length of
	 * both data blocks should be equal to *this object's block length. Blocks
	 * should have been produced with the forward() function of *this object.
	 *
	 * @param aip Input data block 1.
	 * @param[in,out] aop Output/input data block 2.
	 */

	void multiplyBlocks( const double* const aip, double* const aop ) const
	{
	#if R8B_FLOATFFT

		const float* const ip = (const float*) aip;
		float* const op = (float*) aop;
		float t;

	#else // R8B_FLOATFFT

		const double* const ip = aip;
		double* const op = aop;

		#if !R8B_IPP
		double t;
		#endif // !R8B_IPP

	#endif // R8B_FLOATFFT

	#if R8B_IPP

		ippsMulPerm_64f( (Ipp64f*) op, (Ipp64f*) ip, (Ipp64f*) op, Len );

	#else // R8B_IPP

		op[ 0 ] *= ip[ 0 ];
		op[ 1 ] *= ip[ 1 ];

		int i = 2;

		while( i < Len )
		{
			t = op[ i ] * ip[ i ] - op[ i + 1 ] * ip[ i + 1 ];
			op[ i + 1 ] = op[ i ] * ip[ i + 1 ] + op[ i + 1 ] * ip[ i ];
			op[ i ] = t;
			i += 2;
		}

	#endif // R8B_IPP
	}

	/**
	 * Function multiplies two complex-valued data blocks in-place,
	 * considering that the "ip" block contains "zero-phase" response. Length
	 * of both data blocks should be equal to *this object's block length.
	 * Blocks should have been produced with the forward() function of *this
	 * object.
	 *
	 * @param aip Input data block 1, "zero-phase" response. This block should
	 * be first transformed via the convertToZP() function.
	 * @param[in,out] aop Output/input data block 2.
	 */

	void multiplyBlocksZP( const double* const aip, double* const aop ) const
	{
	#if R8B_FLOATFFT

		const float* const ip = (const float*) aip;
		float* const op = (float*) aop;

	#else // R8B_FLOATFFT

		const double* ip = aip;
		double* op = aop;

	#endif // R8B_FLOATFFT

	// SIMD implementations assume that pointers are address-aligned.

	#if !R8B_FLOATFFT && defined( R8B_SSE2 )

		int c8 = Len >> 3;

		while( c8 != 0 )
		{
			const __m128d iv1 = _mm_load_pd( ip );
			const __m128d iv2 = _mm_load_pd( ip + 2 );
			const __m128d ov1 = _mm_load_pd( op );
			const __m128d ov2 = _mm_load_pd( op + 2 );
			_mm_store_pd( op, _mm_mul_pd( iv1, ov1 ));
			_mm_store_pd( op + 2, _mm_mul_pd( iv2, ov2 ));

			const __m128d iv3 = _mm_load_pd( ip + 4 );
			const __m128d ov3 = _mm_load_pd( op + 4 );
			const __m128d iv4 = _mm_load_pd( ip + 6 );
			const __m128d ov4 = _mm_load_pd( op + 6 );
			_mm_store_pd( op + 4, _mm_mul_pd( iv3, ov3 ));
			_mm_store_pd( op + 6, _mm_mul_pd( iv4, ov4 ));

			ip += 8;
			op += 8;
			c8--;
		}

		int c = Len & 7;

		while( c != 0 )
		{
			*op *= *ip;
			ip++;
			op++;
			c--;
		}

	#elif !R8B_FLOATFFT && defined( R8B_NEON )

		int c8 = Len >> 3;

		while( c8 != 0 )
		{
			const float64x2_t iv1 = vld1q_f64( ip );
			const float64x2_t iv2 = vld1q_f64( ip + 2 );
			const float64x2_t ov1 = vld1q_f64( op );
			const float64x2_t ov2 = vld1q_f64( op + 2 );
			vst1q_f64( op, vmulq_f64( iv1, ov1 ));
			vst1q_f64( op + 2, vmulq_f64( iv2, ov2 ));

			const float64x2_t iv3 = vld1q_f64( ip + 4 );
			const float64x2_t iv4 = vld1q_f64( ip + 6 );
			const float64x2_t ov3 = vld1q_f64( op + 4 );
			const float64x2_t ov4 = vld1q_f64( op + 6 );
			vst1q_f64( op + 4, vmulq_f64( iv3, ov3 ));
			vst1q_f64( op + 6, vmulq_f64( iv4, ov4 ));

			ip += 8;
			op += 8;
			c8--;
		}

		int c = Len & 7;

		while( c != 0 )
		{
			*op *= *ip;
			ip++;
			op++;
			c--;
		}

	#else // SIMD

		int i;

		for( i = 0; i < Len; i++ )
		{
			op[ i ] *= ip[ i ];
		}

	#endif // SIMD
	}

	/**
	 * Function converts the specified forward-transformed block into
	 * "zero-phase" form suitable for use with the multiplyBlocksZ() function.
	 *
	 * @param[in,out] ap Block to transform.
	 */

	void convertToZP( double* const ap ) const
	{
	#if R8B_FLOATFFT

		float* const p = (float*) ap;

	#else // R8B_FLOATFFT

		double* const p = ap;

	#endif // R8B_FLOATFFT

		int i = 2;

		while( i < Len )
		{
			p[ i + 1 ] = p[ i ];
			i += 2;
		}
	}

private:
	int LenBits; ///< Length of FFT block (expressed as Nth power of 2).
		///<
	int Len; ///< Length of FFT block (number of real values).
		///<
	double InvMulConst; ///< Inverse FFT multiply constant.
		///<
	CDSPRealFFT* Next; ///< Next object in a singly-linked list.
		///<

	#if R8B_IPP
		IppsFFTSpec_R_64f* SPtr; ///< Pointer to initialized data buffer
			///< to be passed to IPP's FFT functions.
			///<
		CFixedBuffer< unsigned char > SpecBuffer; ///< Working buffer.
			///<
		CFixedBuffer< unsigned char > WorkBuffer; ///< Working buffer.
			///<
	#elif R8B_PFFFT
		PFFFT_Setup* setup; ///< PFFFT setup object.
			///<
		CFixedBuffer< float > work; ///< Working buffer.
			///<
	#elif R8B_PFFFT_DOUBLE
		PFFFTD_Setup* setup; ///< PFFFTD setup object.
			///<
		CFixedBuffer< double > work; ///< Working buffer.
			///<
	#else // R8B_PFFFT_DOUBLE
		CFixedBuffer< int > wi; ///< Working buffer (ints).
			///<
		CFixedBuffer< double > wd; ///< Working buffer (doubles).
			///<
	#endif // R8B_IPP

	/**
	 * A simple class that keeps the pointer to the object and deletes it
	 * automatically.
	 */

	class CObjKeeper
	{
		R8BNOCTOR( CObjKeeper );

	public:
		CObjKeeper()
			: Object( NULL )
		{
		}

		~CObjKeeper()
		{
			delete Object;
		}

		CObjKeeper& operator = ( CDSPRealFFT* const aObject )
		{
			Object = aObject;
			return( *this );
		}

		operator CDSPRealFFT* () const
		{
			return( Object );
		}

	private:
		CDSPRealFFT* Object; ///< FFT object being kept.
			///<
	};

	CDSPRealFFT()
	{
	}

	/**
	 * Constructor initializes FFT object.
	 *
	 * @param aLenBits The length of FFT block (Nth power of 2), specifies the
	 * number of real values in a block. Values from 1 to 30 inclusive are
	 * supported.
	 */

	CDSPRealFFT( const int aLenBits )
		: LenBits( aLenBits )
		, Len( 1 << aLenBits )
	#if R8B_IPP
		, InvMulConst( 1.0 / Len )
	#elif R8B_PFFFT
		, InvMulConst( 1.0 / Len )
	#elif R8B_PFFFT_DOUBLE
		, InvMulConst( 1.0 / Len )
	#else // R8B_PFFFT_DOUBLE
		, InvMulConst( 2.0 / Len )
	#endif // R8B_IPP
	{
	#if R8B_IPP

		int SpecSize;
		int SpecBufferSize;
		int BufferSize;

		ippsFFTGetSize_R_64f( LenBits, IPP_FFT_NODIV_BY_ANY,
			ippAlgHintFast, &SpecSize, &SpecBufferSize, &BufferSize );

		CFixedBuffer< unsigned char > InitBuffer( SpecBufferSize );
		SpecBuffer.alloc( SpecSize );
		WorkBuffer.alloc( BufferSize );

		ippsFFTInit_R_64f( &SPtr, LenBits, IPP_FFT_NODIV_BY_ANY,
			ippAlgHintFast, SpecBuffer, InitBuffer );

	#elif R8B_PFFFT

		setup = pffft_new_setup( Len, PFFFT_REAL );
		work.alloc( Len );

	#elif R8B_PFFFT_DOUBLE

		setup = pffftd_new_setup( Len, PFFFT_REAL );
		work.alloc( Len );

	#else // R8B_PFFFT_DOUBLE

		wi.alloc( (int) ceil( 2.0 + sqrt( (double) ( Len >> 1 ))));
		wi[ 0 ] = 0;
		wd.alloc( Len >> 1 );

	#endif // R8B_IPP
	}

	~CDSPRealFFT()
	{
		#if R8B_PFFFT
			pffft_destroy_setup( setup );
		#elif R8B_PFFFT_DOUBLE
			pffftd_destroy_setup( setup );
		#endif // R8B_PFFFT_DOUBLE

		delete Next;
	}
};

/**
 * @brief A "keeper" class for real-valued FFT transform objects.
 *
 * Class implements "keeper" functionality for handling CDSPRealFFT objects.
 * The allocated FFT objects are placed on the global static list of objects
 * for future reuse instead of deallocation.
 */

class CDSPRealFFTKeeper : public R8B_BASECLASS
{
	R8BNOCTOR( CDSPRealFFTKeeper );

public:
	CDSPRealFFTKeeper()
		: Object( NULL )
	{
	}

	/**
	 * Function acquires FFT object with the specified block length.
	 *
	 * @param LenBits The length of FFT block (Nth power of 2), in the range
	 * [1; 30] inclusive, specifies the number of real values in a FFT block.
	 */

	CDSPRealFFTKeeper( const int LenBits )
	{
		Object = acquire( LenBits );
	}

	~CDSPRealFFTKeeper()
	{
		if( Object != NULL )
		{
			release( Object );
		}
	}

	/**
	 * @return Pointer to the acquired FFT object.
	 */

	const CDSPRealFFT* operator -> () const
	{
		R8BASSERT( Object != NULL );

		return( Object );
	}

	/**
	 * Function acquires FFT object with the specified block length. This
	 * function can be called any number of times.
	 *
	 * @param LenBits The length of FFT block (Nth power of 2), in the range
	 * [1; 30] inclusive, specifies the number of real values in a FFT block.
	 */

	void init( const int LenBits )
	{
		if( Object != NULL )
		{
			if( Object -> LenBits == LenBits )
			{
				return;
			}

			release( Object );
		}

		Object = acquire( LenBits );
	}

	/**
	 * Function releases a previously acquired FFT object.
	 */

	void reset()
	{
		if( Object != NULL )
		{
			release( Object );
			Object = NULL;
		}
	}

private:
	CDSPRealFFT* Object; ///< FFT object.
		///<

	static CSyncObject StateSync; ///< FFTObjects synchronizer.
		///<
	static CDSPRealFFT :: CObjKeeper FFTObjects[]; ///< Pool of FFT objects of
		///< various lengths.
		///<

	/**
	 * Function acquires FFT object from the global pool.
	 *
	 * @param LenBits FFT block length (expressed as Nth power of 2).
	 */

	CDSPRealFFT* acquire( const int LenBits )
	{
		R8BASSERT( LenBits > 0 && LenBits <= 30 );

		R8BSYNC( StateSync );

		if( FFTObjects[ LenBits ] == NULL )
		{
			return( new CDSPRealFFT( LenBits ));
		}

		CDSPRealFFT* ffto = FFTObjects[ LenBits ];
		FFTObjects[ LenBits ] = ffto -> Next;

		return( ffto );
	}

	/**
	 * Function releases a previously acquired FFT object.
	 *
	 * @param ffto FFT object to release.
	 */

	void release( CDSPRealFFT* const ffto )
	{
		R8BSYNC( StateSync );

		ffto -> Next = FFTObjects[ ffto -> LenBits ];
		FFTObjects[ ffto -> LenBits ] = ffto;
	}
};

/**
 * Function calculates the minimum-phase transform of the filter kernel, using
 * a discrete Hilbert transform in cepstrum domain.
 *
 * For more details, see part III.B of
 * http://www.hpl.hp.com/personal/Niranjan_Damera-Venkata/files/ComplexMinPhase.pdf
 *
 * @param[in,out] Kernel Filter kernel buffer.
 * @param KernelLen Filter kernel's length, in samples.
 * @param LenMult Kernel length multiplier. Used as a coefficient of the
 * "oversampling" in the frequency domain. Such oversampling is needed to
 * improve the precision of the minimum-phase transform. If the filter's
 * attenuation is high, this multiplier should be increased or otherwise the
 * required attenuation will not be reached due to "smoothing" effect of this
 * transform.
 * @param DoFinalMul "True" if the final multiplication after transform should
 * be performed or not. Such multiplication returns the gain of the signal to
 * its original value. This parameter can be set to "false" if normalization
 * of the resulting filter kernel is planned to be used.
 * @param[out] DCGroupDelay If not NULL, this variable receives group delay
 * at DC offset, in samples (can be a non-integer value).
 */

inline void calcMinPhaseTransform( double* const Kernel, const int KernelLen,
	const int LenMult = 2, const bool DoFinalMul = true,
	double* const DCGroupDelay = NULL )
{
	R8BASSERT( KernelLen > 0 );
	R8BASSERT( LenMult >= 2 );

	const int LenBits = getBitOccupancy(( KernelLen * LenMult ) - 1 );
	const int Len = 1 << LenBits;
	const int Len2 = Len >> 1;
	int i;

	CFixedBuffer< double > ip( Len );
	CFixedBuffer< double > ip2( Len2 + 1 );

	memcpy( &ip[ 0 ], Kernel, KernelLen * sizeof( ip[ 0 ]));
	memset( &ip[ KernelLen ], 0, ( Len - KernelLen ) * sizeof( ip[ 0 ]));

	CDSPRealFFTKeeper ffto( LenBits );
	ffto -> forward( ip );

	// Create the "log |c|" spectrum while saving the original power spectrum
	// in the "ip2" buffer.

	#if R8B_FLOATFFT
		float* const aip = (float*) &ip[ 0 ];
		float* const aip2 = (float*) &ip2[ 0 ];
		const float nzbias = 1e-35;
	#else // R8B_FLOATFFT
		double* const aip = &ip[ 0 ];
		double* const aip2 = &ip2[ 0 ];
		const double nzbias = 1e-300;
	#endif // R8B_FLOATFFT

	aip2[ 0 ] = aip[ 0 ];
	aip[ 0 ] = log( fabs( aip[ 0 ]) + nzbias );
	aip2[ Len2 ] = aip[ 1 ];
	aip[ 1 ] = log( fabs( aip[ 1 ]) + nzbias );

	for( i = 1; i < Len2; i++ )
	{
		aip2[ i ] = sqrt( aip[ i * 2 ] * aip[ i * 2 ] +
			aip[ i * 2 + 1 ] * aip[ i * 2 + 1 ]);

		aip[ i * 2 ] = log( aip2[ i ] + nzbias );
		aip[ i * 2 + 1 ] = 0.0;
	}

	// Convert to cepstrum and apply discrete Hilbert transform.

	ffto -> inverse( ip );

	const double m1 = ffto -> getInvMulConst();
	const double m2 = -m1;

	ip[ 0 ] = 0.0;

	for( i = 1; i < Len2; i++ )
	{
		ip[ i ] *= m1;
	}

	ip[ Len2 ] = 0.0;

	for( i = Len2 + 1; i < Len; i++ )
	{
		ip[ i ] *= m2;
	}

	// Convert Hilbert-transformed cepstrum back to the "log |c|" spectrum and
	// perform its exponentiation, multiplied by the power spectrum previously
	// saved in the "ip2" buffer.

	ffto -> forward( ip );

	aip[ 0 ] = aip2[ 0 ];
	aip[ 1 ] = aip2[ Len2 ];

	for( i = 1; i < Len2; i++ )
	{
		aip[ i * 2 + 0 ] = cos( aip[ i * 2 + 1 ]) * aip2[ i ];
		aip[ i * 2 + 1 ] = sin( aip[ i * 2 + 1 ]) * aip2[ i ];
	}

	ffto -> inverse( ip );

	if( DoFinalMul )
	{
		for( i = 0; i < KernelLen; i++ )
		{
			Kernel[ i ] = ip[ i ] * m1;
		}
	}
	else
	{
		memcpy( &Kernel[ 0 ], &ip[ 0 ], KernelLen * sizeof( Kernel[ 0 ]));
	}

	if( DCGroupDelay != NULL )
	{
		double tmp;

		calcFIRFilterResponseAndGroupDelay( Kernel, KernelLen, 0.0,
			tmp, tmp, *DCGroupDelay );
	}
}

} // namespace r8b

#endif // VOX_CDSPREALFFT_INCLUDED
