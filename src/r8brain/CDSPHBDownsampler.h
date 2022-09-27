//$ nobt
//$ nocpp

/**
 * @file CDSPHBDownsampler.h
 *
 * @brief Half-band downsampling convolver class.
 *
 * This file includes half-band downsampling convolver class.
 *
 * r8brain-free-src Copyright (c) 2013-2022 Aleksey Vaneev
 * See the "LICENSE" file for license.
 */

#ifndef R8B_CDSPHBDOWNSAMPLER_INCLUDED
#define R8B_CDSPHBDOWNSAMPLER_INCLUDED

#include "CDSPHBUpsampler.h"

namespace r8b {

/**
 * @brief Half-band downsampler class.
 *
 * Class implements brute-force half-band 2X downsampling that uses small
 * sparse symmetric FIR filters. The output has 2.0 gain.
 */

class CDSPHBDownsampler : public CDSPProcessor
{
public:
	/**
	 * Constructor initalizes the half-band downsampler.
	 *
	 * @param ReqAtten Required half-band filter attentuation.
	 * @param SteepIndex Steepness index - 0=steepest. Corresponds to general
	 * downsampling ratio, e.g. at 4x downsampling 0 is used, at 8x
	 * downsampling 1 is used, etc.
	 * @param IsThird "True" if 1/3 resampling is performed.
	 * @param PrevLatency Latency, in samples (any value >=0), which was left
	 * in the output signal by a previous process. Whole-number latency will
	 * be consumed by *this object while remaining fractional latency can be
	 * obtained via the getLatencyFrac() function.
	 */

	CDSPHBDownsampler( const double ReqAtten, const int SteepIndex,
		const bool IsThird, const double PrevLatency )
	{
		static const CConvolveFn FltConvFn[ 14 ] = {
			&CDSPHBDownsampler :: convolve1, &CDSPHBDownsampler :: convolve2,
			&CDSPHBDownsampler :: convolve3, &CDSPHBDownsampler :: convolve4,
			&CDSPHBDownsampler :: convolve5, &CDSPHBDownsampler :: convolve6,
			&CDSPHBDownsampler :: convolve7, &CDSPHBDownsampler :: convolve8,
			&CDSPHBDownsampler :: convolve9, &CDSPHBDownsampler :: convolve10,
			&CDSPHBDownsampler :: convolve11, &CDSPHBDownsampler :: convolve12,
			&CDSPHBDownsampler :: convolve13,
			&CDSPHBDownsampler :: convolve14 };

		int fltt;
		double att;

		if( IsThird )
		{
			CDSPHBUpsampler :: getHBFilterThird( ReqAtten, SteepIndex, fltp,
				fltt, att );
		}
		else
		{
			CDSPHBUpsampler :: getHBFilter( ReqAtten, SteepIndex, fltp, fltt,
				att );
		}

		convfn = FltConvFn[ fltt - 1 ];
		fll = fltt * 2 - 1;
		fl2 = fll;
		flo = fll + fl2;
		flb = BufLen - fll;
		BufRP = Buf + fll;

		LatencyFrac = PrevLatency * 0.5;
		Latency = (int) LatencyFrac;
		LatencyFrac -= Latency;

		R8BASSERT( Latency >= 0 );

		R8BCONSOLE( "CDSPHBDownsampler: taps=%i third=%i att=%.1f io=1/2\n",
			fltt, (int) IsThird, att );

		clear();
	}

	virtual int getLatency() const
	{
		return( 0 );
	}

	virtual double getLatencyFrac() const
	{
		return( LatencyFrac );
	}

	virtual int getMaxOutLen( const int MaxInLen ) const
	{
		R8BASSERT( MaxInLen >= 0 );

		return(( MaxInLen + 1 ) >> 1 );
	}

	virtual void clear()
	{
		LatencyLeft = Latency;
		BufLeft = 0;
		WritePos = 0;
		ReadPos = flb; // Set "read" position to account for filter's latency.

		memset( &Buf[ ReadPos ], 0, ( BufLen - flb ) * sizeof( Buf[ 0 ]));
	}

	virtual int process( double* ip, int l, double*& op0 )
	{
		R8BASSERT( l >= 0 );

		double* op = op0;

		while( l > 0 )
		{
			// Add new input samples to both halves of the ring buffer.

			const int b = min( min( l, BufLen - WritePos ), flb - BufLeft );

			double* const wp1 = Buf + WritePos;
			memcpy( wp1, ip, b * sizeof( wp1[ 0 ]));

			if( WritePos < flo )
			{
				const int c = min( b, flo - WritePos );
				memcpy( wp1 + BufLen, wp1, c * sizeof( wp1[ 0 ]));
			}

			ip += b;
			WritePos = ( WritePos + b ) & BufLenMask;
			l -= b;
			BufLeft += b;

			// Produce output.

			if( BufLeft > fl2 )
			{
				const int c = ( BufLeft - fl2 + 1 ) >> 1;

				double* const opend = op + c;
				( *convfn )( op, opend, fltp, BufRP, ReadPos );

				op = opend;
				const int c2 = c + c;
				ReadPos = ( ReadPos + c2 ) & BufLenMask;
				BufLeft -= c2;
			}
		}

		int ol = (int) ( op - op0 );

		if( LatencyLeft != 0 )
		{
			if( LatencyLeft >= ol )
			{
				LatencyLeft -= ol;
				return( 0 );
			}

			ol -= LatencyLeft;
			op0 += LatencyLeft;
			LatencyLeft = 0;
		}

		return( ol );
	}

private:
	static const int BufLenBits = 10; ///< The length of the ring buffer,
		///< expressed as Nth power of 2. This value can be reduced if it is
		///< known that only short input buffers will be passed to the
		///< interpolator. The minimum value of this parameter is 5, and
		///< 1 << BufLenBits should be at least 3 times larger than the
		///< FilterLen.
		///<
	static const int BufLen = 1 << BufLenBits; ///< The length of the ring
		///< buffer. The actual length is twice as long to allow "beyond max
		///< position" positioning.
		///<
	static const int BufLenMask = BufLen - 1; ///< Mask used for quick buffer
		///< position wrapping.
		///<
	double Buf[ BufLen + 54 ]; ///< The ring buffer, including overrun
		///< protection for the largest filter.
		///<
	const double* fltp; ///< Half-band filter taps.
		///<
	int fll; ///< Input latency.
		///<
	int fl2; ///< Right-side filter length.
		///<
	int flo; ///< Overrun length.
		///<
	int flb; ///< Initial read position and maximal buffer write length.
		///<
	const double* BufRP; ///< Offseted Buf pointer at ReadPos=0.
		///<
	int Latency; ///< Initial latency that should be removed from the output.
		///<
	double LatencyFrac; ///< Fractional latency left on the output.
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
	int LatencyLeft; ///< Latency left to remove.
		///<
	typedef void( *CConvolveFn )( double* op, double* const opend,
		const double* const flt, const double* const rp0, int rpos ); ///<
		///< Convolution function type.
		///<
	CConvolveFn convfn; ///< Convolution function in use.
		///<

#define R8BHBC1( fn ) \
	static void fn( double* op, double* const opend, const double* const flt, \
		const double* const rp0, int rpos ) \
	{ \
		while( op != opend ) \
		{ \
			const double* const rp = rp0 + rpos; \
			*op = rp[ 0 ] +

#define R8BHBC2 \
			rpos = ( rpos + 2 ) & BufLenMask; \
			op++; \
		} \
	}

	R8BHBC1( convolve1 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]);
	R8BHBC2

	R8BHBC1( convolve2 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]);
	R8BHBC2

	R8BHBC1( convolve3 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]) +
				flt[ 2 ] * ( rp[ 5 ] + rp[ -5 ]);
	R8BHBC2

	R8BHBC1( convolve4 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]) +
				flt[ 2 ] * ( rp[ 5 ] + rp[ -5 ]) +
				flt[ 3 ] * ( rp[ 7 ] + rp[ -7 ]);
	R8BHBC2

	R8BHBC1( convolve5 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]) +
				flt[ 2 ] * ( rp[ 5 ] + rp[ -5 ]) +
				flt[ 3 ] * ( rp[ 7 ] + rp[ -7 ]) +
				flt[ 4 ] * ( rp[ 9 ] + rp[ -9 ]);
	R8BHBC2

	R8BHBC1( convolve6 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]) +
				flt[ 2 ] * ( rp[ 5 ] + rp[ -5 ]) +
				flt[ 3 ] * ( rp[ 7 ] + rp[ -7 ]) +
				flt[ 4 ] * ( rp[ 9 ] + rp[ -9 ]) +
				flt[ 5 ] * ( rp[ 11 ] + rp[ -11 ]);
	R8BHBC2

	R8BHBC1( convolve7 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]) +
				flt[ 2 ] * ( rp[ 5 ] + rp[ -5 ]) +
				flt[ 3 ] * ( rp[ 7 ] + rp[ -7 ]) +
				flt[ 4 ] * ( rp[ 9 ] + rp[ -9 ]) +
				flt[ 5 ] * ( rp[ 11 ] + rp[ -11 ]) +
				flt[ 6 ] * ( rp[ 13 ] + rp[ -13 ]);
	R8BHBC2

	R8BHBC1( convolve8 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]) +
				flt[ 2 ] * ( rp[ 5 ] + rp[ -5 ]) +
				flt[ 3 ] * ( rp[ 7 ] + rp[ -7 ]) +
				flt[ 4 ] * ( rp[ 9 ] + rp[ -9 ]) +
				flt[ 5 ] * ( rp[ 11 ] + rp[ -11 ]) +
				flt[ 6 ] * ( rp[ 13 ] + rp[ -13 ]) +
				flt[ 7 ] * ( rp[ 15 ] + rp[ -15 ]);
	R8BHBC2

	R8BHBC1( convolve9 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]) +
				flt[ 2 ] * ( rp[ 5 ] + rp[ -5 ]) +
				flt[ 3 ] * ( rp[ 7 ] + rp[ -7 ]) +
				flt[ 4 ] * ( rp[ 9 ] + rp[ -9 ]) +
				flt[ 5 ] * ( rp[ 11 ] + rp[ -11 ]) +
				flt[ 6 ] * ( rp[ 13 ] + rp[ -13 ]) +
				flt[ 7 ] * ( rp[ 15 ] + rp[ -15 ]) +
				flt[ 8 ] * ( rp[ 17 ] + rp[ -17 ]);
	R8BHBC2

	R8BHBC1( convolve10 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]) +
				flt[ 2 ] * ( rp[ 5 ] + rp[ -5 ]) +
				flt[ 3 ] * ( rp[ 7 ] + rp[ -7 ]) +
				flt[ 4 ] * ( rp[ 9 ] + rp[ -9 ]) +
				flt[ 5 ] * ( rp[ 11 ] + rp[ -11 ]) +
				flt[ 6 ] * ( rp[ 13 ] + rp[ -13 ]) +
				flt[ 7 ] * ( rp[ 15 ] + rp[ -15 ]) +
				flt[ 8 ] * ( rp[ 17 ] + rp[ -17 ]) +
				flt[ 9 ] * ( rp[ 19 ] + rp[ -19 ]);
	R8BHBC2

	R8BHBC1( convolve11 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]) +
				flt[ 2 ] * ( rp[ 5 ] + rp[ -5 ]) +
				flt[ 3 ] * ( rp[ 7 ] + rp[ -7 ]) +
				flt[ 4 ] * ( rp[ 9 ] + rp[ -9 ]) +
				flt[ 5 ] * ( rp[ 11 ] + rp[ -11 ]) +
				flt[ 6 ] * ( rp[ 13 ] + rp[ -13 ]) +
				flt[ 7 ] * ( rp[ 15 ] + rp[ -15 ]) +
				flt[ 8 ] * ( rp[ 17 ] + rp[ -17 ]) +
				flt[ 9 ] * ( rp[ 19 ] + rp[ -19 ]) +
				flt[ 10 ] * ( rp[ 21 ] + rp[ -21 ]);
	R8BHBC2

	R8BHBC1( convolve12 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]) +
				flt[ 2 ] * ( rp[ 5 ] + rp[ -5 ]) +
				flt[ 3 ] * ( rp[ 7 ] + rp[ -7 ]) +
				flt[ 4 ] * ( rp[ 9 ] + rp[ -9 ]) +
				flt[ 5 ] * ( rp[ 11 ] + rp[ -11 ]) +
				flt[ 6 ] * ( rp[ 13 ] + rp[ -13 ]) +
				flt[ 7 ] * ( rp[ 15 ] + rp[ -15 ]) +
				flt[ 8 ] * ( rp[ 17 ] + rp[ -17 ]) +
				flt[ 9 ] * ( rp[ 19 ] + rp[ -19 ]) +
				flt[ 10 ] * ( rp[ 21 ] + rp[ -21 ]) +
				flt[ 11 ] * ( rp[ 23 ] + rp[ -23 ]);
	R8BHBC2

	R8BHBC1( convolve13 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]) +
				flt[ 2 ] * ( rp[ 5 ] + rp[ -5 ]) +
				flt[ 3 ] * ( rp[ 7 ] + rp[ -7 ]) +
				flt[ 4 ] * ( rp[ 9 ] + rp[ -9 ]) +
				flt[ 5 ] * ( rp[ 11 ] + rp[ -11 ]) +
				flt[ 6 ] * ( rp[ 13 ] + rp[ -13 ]) +
				flt[ 7 ] * ( rp[ 15 ] + rp[ -15 ]) +
				flt[ 8 ] * ( rp[ 17 ] + rp[ -17 ]) +
				flt[ 9 ] * ( rp[ 19 ] + rp[ -19 ]) +
				flt[ 10 ] * ( rp[ 21 ] + rp[ -21 ]) +
				flt[ 11 ] * ( rp[ 23 ] + rp[ -23 ]) +
				flt[ 12 ] * ( rp[ 25 ] + rp[ -25 ]);
	R8BHBC2

	R8BHBC1( convolve14 )
				flt[ 0 ] * ( rp[ 1 ] + rp[ -1 ]) +
				flt[ 1 ] * ( rp[ 3 ] + rp[ -3 ]) +
				flt[ 2 ] * ( rp[ 5 ] + rp[ -5 ]) +
				flt[ 3 ] * ( rp[ 7 ] + rp[ -7 ]) +
				flt[ 4 ] * ( rp[ 9 ] + rp[ -9 ]) +
				flt[ 5 ] * ( rp[ 11 ] + rp[ -11 ]) +
				flt[ 6 ] * ( rp[ 13 ] + rp[ -13 ]) +
				flt[ 7 ] * ( rp[ 15 ] + rp[ -15 ]) +
				flt[ 8 ] * ( rp[ 17 ] + rp[ -17 ]) +
				flt[ 9 ] * ( rp[ 19 ] + rp[ -19 ]) +
				flt[ 10 ] * ( rp[ 21 ] + rp[ -21 ]) +
				flt[ 11 ] * ( rp[ 23 ] + rp[ -23 ]) +
				flt[ 12 ] * ( rp[ 25 ] + rp[ -25 ]) +
				flt[ 13 ] * ( rp[ 27 ] + rp[ -27 ]);
	R8BHBC2

#undef R8BHBC1
#undef R8BHBC2
};

// ---------------------------------------------------------------------------

} // namespace r8b

#endif // R8B_CDSPHBDOWNSAMPLER_INCLUDED
