//$ nocpp

/**
 * @file CDSPFracInterpolator.h
 *
 * @brief Fractional delay interpolator and filter bank classes.
 *
 * This file includes fractional delay interpolator class.
 *
 * r8brain-free-src Copyright (c) 2013 Aleksey Vaneev
 * See the "License.txt" file for license.
 */

#ifndef R8B_CDSPFRACINTERPOLATOR_INCLUDED
#define R8B_CDSPFRACINTERPOLATOR_INCLUDED

#include "CDSPSincFilterGen.h"

namespace r8b {

/**
 * @brief Sinc function-based fractional delay filter bank class.
 *
 * Class implements storage and initialization of a bank of sinc-based
 * fractional delay filters, expressed as 1st, 2nd or 3rd order polynomial
 * interpolation coefficients. The filters are windowed by either the "Vaneev"
 * or "Kaiser" power-raised window function. The FilterLen and FilterFracs
 * parameters can be varied freely without breaking the resampler.
 *
 * @param FilterLen Specifies the number of samples (taps) each fractional
 * delay filter should have. This must be an even value, the minimal value for
 * FilterLen is 6, the maximal value is 30. To achieve a higher resampling
 * precision, the oversampling should be used in the first place instead of
 * using a higher FilterLen value. The lower this value is the lower the
 * signal-to-noise performance of the interpolator will be. Each FilterLen
 * decrease by 2 decreases SNR by approximately 12 to 14 decibel.
 * @param FilterFracs The number of fractional delay positions to sample. For
 * a high signal-to-noise ratio this has to be a larger value. The larger the
 * FilterLen is the larger the FilterFracs should be. Approximate FilterLen to
 * FilterFracs correspondence (for 2nd order interpolation only): 6:11, 8:17,
 * 10:23, 12:41, 14:67, 16:97, 18:137, 20:211, 22:353, 24:673, 26:1051,
 * 28:1733, 30:2833. The FilterFracs can be considerably reduced with 3rd
 * order interpolation in use. In order to get consistent results when
 * resampling to/from different sample rates, it is suggested to set this
 * parameter to a suitable prime number.
 * @param ElementSize The size of each filter's tap, in "double" values. This
 * parameter corresponds to the complexity of interpolation. 4 should be set
 * for 3rd order, 3 for 2nd order, 2 for linear interpolation.
 * @param InterpPoints The number of points the interpolation is based on.
 * This value should not be confused with the ElementSize. Set to 2 for linear
 * interpolation.
 */

template< int FilterLen, int FilterFracs, int ElementSize, int InterpPoints >
class CDSPFracDelayFilterBank : public R8B_BASECLASS
{
public:
	CDSPFracDelayFilterBank()
	{
		R8BASSERT( FilterLen >= 6 );
		R8BASSERT( FilterLen <= 30 );
		R8BASSERT(( FilterLen & 1 ) == 0 );
		R8BASSERT( FilterFracs > 0 );
		R8BASSERT( ElementSize >= 2 && ElementSize <= 4 );
		R8BASSERT( InterpPoints == 2 || InterpPoints == 8 );

		calculate();
	}

	/**
	 * Function calculates the filter bank.
	 *
	 * @param Params Window function's parameters. If NULL then the built-in
	 * table values for the current FilterLen will be used.
	 */

	void calculate( const double* const Params = NULL )
	{
		CDSPSincFilterGen sinc;
		sinc.Len2 = FilterLen / 2;

		double* p = Table;
		const int pc2 = InterpPoints / 2;
		int i;

		if( FilterLen <= 20 )
		{
			for( i = -pc2 + 1; i <= FilterFracs + pc2; i++ )
			{
				sinc.FracDelay = (double) ( FilterFracs - i ) / FilterFracs;
				sinc.initFrac( CDSPSincFilterGen :: wftVaneev, Params );
				sinc.generateFrac( p, &CDSPSincFilterGen :: calcWindowVaneev,
					ElementSize );

				normalizeFIRFilter( p, FilterLen, 1.0, ElementSize );
				p += FilterSize;
			}
		}
		else
		{
			for( i = -pc2 + 1; i <= FilterFracs + pc2; i++ )
			{
				sinc.FracDelay = (double) ( FilterFracs - i ) / FilterFracs;
				sinc.initFrac( CDSPSincFilterGen :: wftKaiser, Params, true );
				sinc.generateFrac( p, &CDSPSincFilterGen :: calcWindowKaiser,
					ElementSize );

				normalizeFIRFilter( p, FilterLen, 1.0, ElementSize );
				p += FilterSize;
			}
		}

		const int TablePos2 = FilterSize;
		const int TablePos3 = FilterSize * 2;
		const int TablePos4 = FilterSize * 3;
		const int TablePos5 = FilterSize * 4;
		const int TablePos6 = FilterSize * 5;
		const int TablePos7 = FilterSize * 6;
		const int TablePos8 = FilterSize * 7;
		double* const TableEnd = Table + ( FilterFracs + 1 ) * FilterSize;
		p = Table;

		if( InterpPoints == 8 )
		{
			if( ElementSize == 3 )
			{
				// Calculate 2nd order spline (polynomial) interpolation
				// coefficients using 8 points.

				while( p < TableEnd )
				{
					calcSpline2p8Coeffs( p, p[ 0 ], p[ TablePos2 ],
						p[ TablePos3 ], p[ TablePos4 ], p[ TablePos5 ],
						p[ TablePos6 ], p[ TablePos7 ], p[ TablePos8 ]);

					p += ElementSize;
				}
			}
			else
			if( ElementSize == 4 )
			{
				// Calculate 3rd order spline (polynomial) interpolation
				// coefficients using 8 points.

				while( p < TableEnd )
				{
					calcSpline3p8Coeffs( p, p[ 0 ], p[ TablePos2 ],
						p[ TablePos3 ], p[ TablePos4 ], p[ TablePos5 ],
						p[ TablePos6 ], p[ TablePos7 ], p[ TablePos8 ]);

					p += ElementSize;
				}
			}
		}
		else
		{
			// Calculate linear interpolation coefficients.

			while( p < TableEnd )
			{
				p[ 1 ] = p[ TablePos2 ] - p[ 0 ];
				p += ElementSize;
			}
		}
	}

	/**
	 * @param i Filter index, in the range 0 to FilterFracs, inclusive.
	 * @return Reference to the filter.
	 */

	const double& operator []( const int i ) const
	{
		R8BASSERT( i >= 0 && i <= FilterFracs );

		return( Table[ i * FilterSize ]);
	}

private:
	static const int FilterSize = FilterLen * ElementSize; ///< This constant
		///< specifies the "size" of a single filter in "double" elements.
		///<
	double Table[ FilterSize * ( FilterFracs + InterpPoints )]; ///< The
		///< table of fractional delay filters for all discrete fractional
		///< x = 0..1 sample positions, and interpolation coefficients.
		///<
};

/**
 * @brief Fractional delay filter bank-based interpolator class.
 *
 * Class implements the fractional delay interpolator. This implementation at
 * first puts the input signal into a ring buffer and then performs
 * interpolation. The interpolation is performed using sinc-based fractional
 * delay filters. These filters are contained in a bank, and for higher
 * precision they are interpolated between adjacent filters.
 *
 * To increase sample timing precision, this class uses "resettable counter"
 * approach. This gives less than "1 per 100 billion" sample timing error when
 * converting 44100 to 48000 sample rate.
 *
 * VERY IMPORTANT: the interpolation step should not exceed FilterLen / 2 + 1
 * samples or the algorithm in its current form will fail. However, this
 * condition can be easily met if the input signal is suitably downsampled
 * first before the interpolation is performed.
 *
 * @param FilterLen Specifies the number of samples (taps) each fractional
 * delay filter should have. See the r8b::CDSPFracDelayFilterBank class for
 * more details.
 * @param FilterFracs The number of fractional delay positions to sample. See
 * the r8b::CDSPFracDelayFilterBank class for more details.
 * @param BufLenBits The length of the ring buffer, expressed as Nth power of
 * 2. This value can be reduced if it is known that only short input buffers
 * will be passed to the interpolator. The minimum value of this parameter is
 * 5, and 1 << BufLenBits should be at least 3 times larger than the
 * FilterLen.
 */

template< int FilterLen, int FilterFracs, int BufLenBits >
class CDSPFracInterpolator : public R8B_BASECLASS
{
	R8BNOCTOR( CDSPFracInterpolator );

public:
	/**
	 * Constructor initalizes the interpolator. It is important to call the
	 * getMaxOutLen() function afterwards to obtain the optimal output buffer
	 * length.
	 *
	 * @param aSrcSampleRate Source sample rate.
	 * @param aDstSampleRate Destination sample rate.
	 * @param aInitFracPos Initial fractional position, in samples, in the
	 * range [0; 1). A non-zero value can be specified to remove the
	 * fractional delay introduced by a minimum-phase filter. This value is
	 * usually equal to the CDSPBlockConvolver.getLatencyFrac() value.
	 */

	CDSPFracInterpolator( const double aSrcSampleRate,
		const double aDstSampleRate, const double aInitFracPos )
		: SrcSampleRate( aSrcSampleRate )
		, DstSampleRate( aDstSampleRate )
		, InitFracPos( aInitFracPos )
	{
		R8BASSERT( SrcSampleRate > 0.0 );
		R8BASSERT( DstSampleRate > 0.0 );
		R8BASSERT( InitFracPos >= 0.0 && InitFracPos < 1.0 );
		R8BASSERT( BufLenBits >= 5 );
		R8BASSERT(( 1 << BufLenBits ) >= FilterLen * 3 );

		clear();
	}

	/**
	 * @return The number of samples that should be passed to *this object
	 * before the actual output starts.
	 */

	int getInLenBeforeOutStart() const
	{
		return( FilterLenD2 );
	}

	/**
	 * @param MaxInLen The number of samples planned to process at once, at
	 * most.
	 * @return The maximal length of the output buffer required when
	 * processing the "MaxInLen" number of input samples.
	 */

	int getMaxOutLen( const int MaxInLen ) const
	{
		R8BASSERT( MaxInLen >= 0 );

		return( (int) ceil( MaxInLen * DstSampleRate / SrcSampleRate ) + 1 );
	}

	/**
	 * Function changes the destination sample rate "on the fly". Note that
	 * the getMaxOutLen() function may needed to be called after calling this
	 * function as the maximal number of output samples produced by the
	 * interpolator depends on the destination sample rate.
	 *
	 * It can be a useful approach to construct *this object passing the
	 * maximal possible destination sample rate to the constructor, obtaining
	 * the getMaxOutLen() value and then setting the destination sample rate
	 * to whatever lower value is needed.
	 *
	 * It is advisable to change the sample rate in small increments, and as
	 * rarely as possible: e.g. every several samples.
	 *
	 * @param NewDstSampleRate New destination sample rate.
	 */

	void setDstSampleRate( const double NewDstSampleRate )
	{
		R8BASSERT( DstSampleRate > 0.0 );

		DstSampleRate = NewDstSampleRate;
		InCounter = 0;
		InPosInt = 0;
		InPosShift = InPosFrac;
	}

	/**
	 * Function clears (resets) the state of *this object and returns it to
	 * the state after construction. All input data accumulated in the
	 * internal buffer so far will be discarded.
	 *
	 * Note that the destination sample rate will remain unchanged, even if it
	 * was changed since the time of *this object's construction.
	 */

	void clear()
	{
		BufLeft = 0;
		WritePos = 0;
		ReadPos = BufLen - FilterLenD2Minus1; // Set "read" position to
			// account for filter's latency at zero fractional delay which
			// equals to FilterLenD2Minus1.

		memset( &Buf[ ReadPos ], 0, FilterLenD2Minus1 * sizeof( double ));

		InCounter = 0;
		InPosInt = 0;
		InPosFrac = InitFracPos;
		InPosShift = InitFracPos;
	}

	/**
	 * Function performs input sample stream interpolation.
	 *
	 * @param ip Input sample buffer.
	 * @param[out] op0 Output sample buffer, the capacity of this buffer
	 * should be equal to the value returned by the getMaxOutLen() function
	 * for the given "l". This buffer can be equal to "ip" only if the
	 * getMaxOutLen( l ) function's returned value is lesser than "l".
	 * @param l The number of samples available in the input sample buffer.
	 * @return The number of output samples written to the "op0" buffer.
	 */

	int process( const double* ip, double* const op0, int l )
	{
		R8BASSERT( l >= 0 );
		R8BASSERT( ip != op0 || l == 0 || SrcSampleRate > DstSampleRate );

		double* op = op0;

		while( l > 0 )
		{
			// Add new input samples to both halves of the ring buffer.

			const int b = min( min( l, BufLen - WritePos ),
				BufLeftMax - BufLeft );

			double* const wp1 = Buf + WritePos;
			double* const wp2 = wp1 + BufLen;
			int i;

			for( i = 0; i < b; i++ )
			{
				wp1[ i ] = ip[ i ];
				wp2[ i ] = ip[ i ];
			}

			ip += b;
			WritePos = ( WritePos + b ) & BufLenMask;
			l -= b;
			BufLeft += b;

			// Produce as many output samples as possible.

			while( BufLeft >= FilterLenD2Plus1 )
			{
				double x = InPosFrac * FilterFracs;
				const int fti = (int) x; // Function table index.
				x -= fti; // Coefficient for interpolation between adjacent
					// fractional delay filters.
				const double x2 = x * x;
				const double* const ftp = &FilterBank[ fti ];
				const double* const rp = Buf + ReadPos;
				double s = 0.0;
				int ii = 0;

			#if R8B_FLTTEST
				const double x3 = x2 * x;
			#endif // R8B_FLTTEST

				for( i = 0; i < FilterLen; i++ )
				{
				#if !R8B_FLTTEST
					s += ( ftp[ ii ] + ftp[ ii + 1 ] * x +
						ftp[ ii + 2 ] * x2 ) * rp[ i ];
				#else // !R8B_FLTTEST
					s += ( ftp[ ii ] + ftp[ ii + 1 ] * x +
						ftp[ ii + 2 ] * x2 + ftp[ ii + 3 ] * x3 ) * rp[ i ];
				#endif // !R8B_FLTTEST

					ii += FilterElementSize;
				}

				*op = s;
				op++;

				InCounter++;
				const double NextInPos =
					InCounter * SrcSampleRate / DstSampleRate + InPosShift;

				const int NextInPosInt = (int) NextInPos;
				const int PosIncr = NextInPosInt - InPosInt;
				InPosInt = NextInPosInt;
				InPosFrac = NextInPos - NextInPosInt;

				ReadPos = ( ReadPos + PosIncr ) & BufLenMask;
				BufLeft -= PosIncr;
			}
		}

		if( InCounter > 1000 )
		{
			// Reset the interpolation position counter to achieve a higher
			// sample timing precision.

			InCounter = 0;
			InPosInt = 0;
			InPosShift = InPosFrac;
		}

		return( (int) ( op - op0 ));
	}

private:
#if !R8B_FLTTEST
	static const int FilterElementSize = 3; ///< The number of "doubles" a
		///< single filter tap consists of (includes interpolation
		///< coefficients).
		///<
#else // !R8B_FLTTEST
	static const int FilterElementSize = 4; ///< The number of "doubles" a
		///< single filter tap consists of (includes interpolation
		///< coefficients). During filter testing a higher precision
		///< interpolation is used.
		///<
#endif // !R8B_FLTTEST

	static const int FilterLenD2 = FilterLen >> 1; ///< = FilterLen / 2.
		///<
	static const int FilterLenD2Minus1 = FilterLenD2 - 1; ///< =
		///< FilterLen / 2 - 1. This value also equals to filter's latency in
		///< samples (taps).
		///<
	static const int FilterLenD2Plus1 = FilterLenD2 + 1; ///< =
		///< FilterLen / 2 + 1.
		///<
	static const int BufLen = 1 << BufLenBits; ///< The length of the ring
		///< buffer. The actual length is twice as long to allow "beyond max
		///< position" positioning.
		///<
	static const int BufLenMask = BufLen - 1; ///< Mask used for quick buffer
		///< position wrapping.
		///<
	static const int BufLeftMax = BufLen - FilterLenD2Minus1; ///< The number
		///< of new samples that the ring buffer can hold at most. The
		///< remaining FilterLenD2Minus1 samples hold "previous" input samples
		///< for the filter.
		///<
	double Buf[ BufLen * 2 ]; ///< The ring buffer.
		///<
	double SrcSampleRate; ///< Source sample rate.
		///<
	double DstSampleRate; ///< Destination sample rate.
		///<
	double InitFracPos; ///< Initial fractional position, in samples, in the
		///< range [0; 1).
		///<
	int BufLeft; ///< The number of samples left in the buffer to process.
		///< When this value is below FilterLenD2Plus1, the interpolation
		///< cycle ends.
		///<
	int WritePos; ///< The current buffer write position. Incremented together
		///< with the BufLeft variable.
		///<
	int ReadPos; ///< The current buffer read position.
		///<
	int InCounter; ///< Interpolation step counter.
		///<
	int InPosInt; ///< Interpolation position (integer part).
		///<
	double InPosFrac; ///< Interpolation position (fractional part).
		///<
	double InPosShift; ///< Interpolation position fractional shift.
		///<

	CDSPFracInterpolator()
	{
	}

#if !R8B_FLTTEST
	static const CDSPFracDelayFilterBank< FilterLen, FilterFracs, 3,
		8 > FilterBank; ///< Filter bank object, defined statically if no
		///< filter test takes place.
		///<
#else // !R8B_FLTTEST
public:
	CDSPFracDelayFilterBank< FilterLen, FilterFracs, 4, 8 > FilterBank; ///<
		///< Filter bank object, defined as a member variable to allow for
		///< recalculation.
		///<
#endif // !R8B_FLTTEST
};

// ---------------------------------------------------------------------------

#if !R8B_FLTTEST
template< int FilterLen, int FilterFracs, int BufLenBits >
const CDSPFracDelayFilterBank< FilterLen, FilterFracs, 3, 8 >
	CDSPFracInterpolator< FilterLen, FilterFracs, BufLenBits > :: FilterBank;
#endif // !R8B_FLTTEST

// ---------------------------------------------------------------------------

} // namespace r8b

#endif // R8B_CDSPFRACINTERPOLATOR_INCLUDED
