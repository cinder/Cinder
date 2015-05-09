//$ nocpp

/**
 * @file CDSPBlockConvolver.h
 *
 * @brief Single-block overlap-save convolution processor class.
 *
 * This file includes single-block overlap-save convolution processor class.
 *
 * r8brain-free-src Copyright (c) 2013 Aleksey Vaneev
 * See the "License.txt" file for license.
 */

#ifndef R8B_CDSPBLOCKCONVOLVER_INCLUDED
#define R8B_CDSPBLOCKCONVOLVER_INCLUDED

#include "CDSPFIRFilter.h"

namespace r8b {

/**
 * Enumeration of the built-in resampling modes.
 */

enum EDSPResamplingMode
{
	rsmNone = 0, ///< No resampling.
	rsmUpsample2X, ///< 2x upsampling.
	rsmDownsample2X ///< 2x downsampling.
};

/**
 * @brief Single-block overlap-save convolution processing class.
 *
 * Class that implements single-block overlap-save convolution processing. The
 * length of a single FFT block used depends on the length of the filter
 * kernel.
 *
 * The rationale behind "single-block" processing is that increasing the FFT
 * block length by 2 is more efficient than performing convolution at the same
 * FFT block length but using two blocks.
 *
 * This class also implements a built-in resampling (2X up or 2X down) which
 * simplifies the overall resampling objects topology.
 */

class CDSPBlockConvolver : public R8B_BASECLASS
{
	R8BNOCTOR( CDSPBlockConvolver );

public:
	/**
	 * Constructor initializes internal variables and constants of *this
	 * object.
	 *
	 * @param aFilter Pre-calculated filter data. Reference to this object is
	 * inhertied by *this object, and the object will be released when *this
	 * object is destroyed.
	 * @param aResamplingMode Resampling mode to use. Note that the filter
	 * should meet alias-free resampling requirements if resampling is used:
	 * the normalized low-pass frequency of the filter should be equal to 0.5
	 * or lower.
	 * @param PrevLatencyFrac Latency, in samples (>=0), which was left in the
	 * output signal by a previous convolver. This value is usually non-zero
	 * if the minimum-phase filters are in use. This value is always zero if
	 * the linear-phase filters are in use.
	 */

	CDSPBlockConvolver( CDSPFIRFilter& aFilter,
		const EDSPResamplingMode aResamplingMode,
		const double PrevLatencyFrac = 0.0 )
		: Filter( &aFilter )
		, ffto( Filter -> getBlockLenBits() + 1 )
		, ResamplingMode( aResamplingMode )
		, UpShift( ResamplingMode == rsmUpsample2X ? 1 : 0 )
		, BlockLen( 1 << Filter -> getBlockLenBits() )
		, Latency( BlockLen + Filter -> getLatency() )
	{
		R8BASSERT( PrevLatencyFrac >= 0.0 && PrevLatencyFrac < 1.0 );

		LatencyFrac = Filter -> getLatencyFrac() +
			PrevLatencyFrac * ( UpShift + 1 );

		const int lt = (int) LatencyFrac;
		Latency += lt;
		LatencyFrac -= lt;

		if( ResamplingMode == rsmDownsample2X )
		{
			LatencyFrac *= 0.5;
		}

		const int bs = BlockLen * (int) sizeof( double );

		PrevInput.alloc( bs );

		WorkBlocks[ 0 ].alloc( bs * 2 );
		CurInput = WorkBlocks[ 0 ];

		WorkBlocks[ 1 ].alloc( bs * 2 );
		CurOutput = WorkBlocks[ 1 ];

		clear();
	}

	~CDSPBlockConvolver()
	{
		Filter -> unref();
	}

	/**
	 * @return Fractional latency, in samples, which is present in the output
	 * signal. This value is always zero if a linear-phase filter is in use.
	 */

	double getLatencyFrac() const
	{
		return( LatencyFrac );
	}

	/**
	 * @param NextInLen The number of input samples required before the output
	 * starts on the next resampling step.
	 * @return The cumulative number of samples that should be passed to *this
	 * object before the actual output starts. This value includes latencies
	 * induced by the convolver, filter, and the next resampling step.
	 */

	int getInLenBeforeOutStart( const int NextInLen ) const
	{
		if( ResamplingMode == rsmNone )
		{
			return( Latency + NextInLen );
		}

		if( ResamplingMode == rsmDownsample2X )
		{
			return( Latency + NextInLen * 2 );
		}

		return(( Latency + NextInLen ) / 2 );
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

		if( ResamplingMode == rsmUpsample2X )
		{
			return( MaxInLen * 2 );
		}

		if( ResamplingMode == rsmNone )
		{
			return( MaxInLen );
		}

		return(( MaxInLen + 1 ) >> 1 );
	}

	/**
	 * Function clears (resets) the state of *this object and returns it to
	 * the state after construction. All input data accumulated in the
	 * internal buffer so far will be discarded.
	 */

	void clear()
	{
		memset( &PrevInput[ 0 ], 0, BlockLen * sizeof( double ));

		InDataLeft = BlockLen;
		LatencyLeft = Latency;
		DownSkip = 0;
	}

	/**
	 * Function performs convolution processing.
	 *
	 * @param ip Input data pointer.
	 * @param[out] op0 Output data pointer (can be equal to "ip" only if
	 * rsmNone or rsmDownsample2X resampling mode is used). On function's exit
	 * this variable will point to the actual output data within the output
	 * buffer. The length of the output buffer if resampling mode
	 * rsmUpsample2X is used should be twice as large as the input buffer. If
	 * rsmDownsample2X is used the length should be equal to "l0 / 2 + 1".
	 * @param l0 How many samples to process.
	 * @return The number of output samples available after processing. This
	 * value can be smaller in comparison to the original "l0" value due to
	 * processing and filter's latency compensation that took place, and due
	 * to resampling if it is used.
	 */

	int process( const double* ip, double*& op0, int l0 )
	{
		R8BASSERT( l0 >= 0 );
		R8BASSERT( UpShift == 0 || ip != op0 || l0 == 0 );

		double* op = op0;
		l0 <<= UpShift;
		int l = l0;

		while( l > 0 )
		{
			const int Offs = BlockLen - InDataLeft;

			if( l < InDataLeft )
			{
				InDataLeft -= l;
				acquireInput( ip, Offs, l );
				memcpy( op, &CurOutput[ Offs ], l * sizeof( double ));

				break;
			}

			const int b = InDataLeft;
			InDataLeft = BlockLen;

			acquireInput( ip, Offs, b );
			memcpy( op, &CurOutput[ Offs ], b * sizeof( double ));

			ffto -> forward( CurInput );
			ffto -> multiplyBlocks( Filter -> getKernelBlock(), CurInput );
			ffto -> inverse( CurInput );

			double* const tmp = CurInput;
			CurInput = CurOutput;
			CurOutput = tmp;

			ip += b >> UpShift;
			op += b;
			l -= b;
		}

		if( LatencyLeft > 0 )
		{
			if( LatencyLeft >= l0 )
			{
				LatencyLeft -= l0;
				return( 0 );
			}

			op0 += LatencyLeft;
			l0 -= LatencyLeft;
			LatencyLeft = 0;
		}

		if( ResamplingMode == rsmDownsample2X && l0 > 0 )
		{
			// Perform quick 2x downsampling.

			const double* ip = op0 + DownSkip;
			l = ( l0 + 1 - DownSkip ) >> 1;
			DownSkip += ( l << 1 ) - l0;
			l0 = l;
			op = op0;

			while( l > 0 )
			{
				*op = *ip;
				op++;
				ip += 2;
				l--;
			}
		}

		return( l0 );
	}

private:
	CDSPFIRFilter* Filter; ///< Filter in use.
		///<
	CDSPRealFFTKeeper ffto; ///< FFT object, length = Filter.BlockLenBits + 1.
		///<
	EDSPResamplingMode ResamplingMode; ///< Built-in resampling mode to use.
		///<
	int UpShift; ///< Upsampling shift. Equals 1 if 2x upsampling is used,
		///< 0 otherwise.
		///<
	int BlockLen; ///< Block length, in samples.
		///<
	int Latency; ///< Processing and filter kernel's latency, in samples.
		///<
	double LatencyFrac; ///< Fractional latency, in samples, that is left in
		///< the output signal.
		///<
	CFixedBuffer< double > PrevInput; ///< Previous input data,
		///< capacity = BlockLen.
		///<
	CFixedBuffer< double > WorkBlocks[ 2 ]; ///< Input and output blocks,
		///< capacity = BlockLen * 2 each. Used in the flip-flop manner.
		///<
	double* CurInput; ///< Input data buffer.
		///<
	double* CurOutput; ///< Output data buffer.
		///<
	int InDataLeft; ///< Samples left before processing input and output FFT
		///< blocks.
		///<
	int LatencyLeft; ///< Latency in samples left to skip.
		///<
	int DownSkip; ///< The current 2x downsampling sample skip (0 or 1).
		///<

	CDSPBlockConvolver()
	{
	}

	/**
	 * Function acquires input data and distributes it over the work buffers.
	 *
	 * @param ip Input buffer.
	 * @param Offs Current input/output offset, 0..BlockLen-1.
	 * @param l Sample count.
	 */

	void acquireInput( const double* const ip, const int Offs, const int l )
	{
		double* const prev = &PrevInput[ Offs ];
		double* const op = &CurInput[ Offs ];
		double* const opoffs = op + BlockLen;
		int i;

		if( UpShift == 0 )
		{
			for( i = 0; i < l; i++ )
			{
				opoffs[ i ] = prev[ i ];
				prev[ i ] = ip[ i ];
				op[ i ] = ip[ i ];
			}
		}
		else
		{
			// Insert zero after each input sample to perform quick 2x
			// upsampling. "l" is always an even value in such resampling
			// mode.

			for( i = 0; i < l; i += 2 )
			{
				opoffs[ i ] = prev[ i ];
				opoffs[ i + 1 ] = 0.0;
				const double v = ip[ i >> 1 ] * 2.0;
				prev[ i ] = v;
				op[ i ] = v;
				op[ i + 1 ] = 0.0;
			}
		}
	}
};

} // namespace r8b

#endif // R8B_CDSPBLOCKCONVOLVER_INCLUDED
