//$ nobt
//$ nocpp

/**
 * @file CDSPResampler.h
 *
 * @brief The master sample rate converter (resampler) class.
 *
 * This file includes the master sample rate converter (resampler) class that
 * combines all elements of this library into a single front-end class.
 *
 * r8brain-free-src Copyright (c) 2013-2022 Aleksey Vaneev
 * See the "LICENSE" file for license.
 */

#ifndef R8B_CDSPRESAMPLER_INCLUDED
#define R8B_CDSPRESAMPLER_INCLUDED

#include "CDSPHBDownsampler.h"
#include "CDSPHBUpsampler.h"
#include "CDSPBlockConvolver.h"
#include "CDSPFracInterpolator.h"

namespace r8b {

/**
 * @brief The master sample rate converter (resampler) class.
 *
 * This class can be considered the "master" sample rate converter (resampler)
 * class since it combines all functionality of this library into a single
 * front-end class to perform sample rate conversion to/from any sample rate,
 * including non-integer sample rates.
 *
 * Note that objects of this class can be constructed on the stack as it has a
 * small member data size. The default template parameters of this class are
 * suited for 27-bit fixed point resampling.
 *
 * Use the CDSPResampler16 class for 16-bit resampling.
 *
 * Use the CDSPResampler16IR class for 16-bit impulse response resampling.
 *
 * Use the CDSPResampler24 class for 24-bit resampling (including 32-bit
 * floating point resampling).
 */

class CDSPResampler : public CDSPProcessor
{
public:
	/**
	 * Constructor initalizes the resampler object.
	 *
	 * Note that increasing the transition band and decreasing attenuation
	 * reduces the filter length, this in turn reduces the "input before
	 * output" delay. However, the filter length has only a minor influence on
	 * the overall resampling speed.
	 *
	 * It should be noted that the ReqAtten specifies the minimal difference
	 * between the loudest input signal component and the produced aliasing
	 * artifacts during resampling. For example, if ReqAtten=100 was specified
	 * when performing 2x upsampling, the analysis of the resulting signal may
	 * display high-frequency components which are quieter than the loudest
	 * part of the input signal by only 100 decibel meaning the high-frequency
	 * part did not become "magically" completely silent after resampling. You
	 * have to specify a higher ReqAtten value if you need a totally clean
	 * high-frequency content. On the other hand, it may not be reasonable to
	 * have a high-frequency content cleaner than the input signal itself: if
	 * the input signal is 16-bit, setting ReqAtten to 180 will make its
	 * high-frequency content 24-bit, but the original part of the signal will
	 * remain 16-bit.
	 *
	 * @param SrcSampleRate Source signal sample rate. Both sample rates can
	 * be specified as a ratio, e.g. SrcSampleRate = 1.0, DstSampleRate = 2.0.
	 * @param DstSampleRate Destination signal sample rate. The "power of 2"
	 * ratios between the source and destination sample rates force resampler
	 * to use several fast "power of 2" resampling steps, without using
	 * fractional interpolation at all.
	 * @param aMaxInLen The maximal planned length of the input buffer (in
	 * samples) that will be passed to the resampler. The resampler relies on
	 * this value as it allocates intermediate buffers. Input buffers longer
	 * than this value should never be supplied to the resampler. Note that
	 * upsampling produces more samples than was provided on input, so at
	 * higher upsampling ratios it is advisable to use smaller MaxInLen
	 * values to reduce memory footprint. When downsampling, a larger MaxInLen
	 * is suggested in order to increase downsampling performance.
	 * @param ReqTransBand Required transition band, in percent of the
	 * spectral space of the input signal (or the output signal if
	 * downsampling is performed) between filter's -3 dB point and the Nyquist
	 * frequency. The range is from CDSPFIRFilter::getLPMinTransBand() to
	 * CDSPFIRFilter::getLPMaxTransBand(), inclusive. When upsampling 88200 or
	 * 96000 audio to a higher sample rates the ReqTransBand can be
	 * considerably increased, up to 30. The selection of ReqTransBand depends
	 * on the level of desire to preserve the high-frequency content. While
	 * values 0.5 to 2 are extremely "greedy" settings, not necessary in most
	 * cases, values 2 to 3 can be used in most cases. Values 3 to 4 are
	 * relaxed settings, but they still offer a flat frequency response up to
	 * 21kHz with 44.1k source or destination sample rate.
	 * @param ReqAtten Required stop-band attenuation in decibel, in the
	 * range CDSPFIRFilter::getLPMinAtten() to CDSPFIRFilter::getLPMaxAtten(),
	 * inclusive. The actual attenuation may be 0.40-4.46 dB higher. The
	 * general formula for selecting the ReqAtten is 6.02 * Bits + 40, where
	 * "Bits" is the bit resolution (e.g. 16, 24), "40" is an added resolution
	 * for dynamic signals; this value can be decreased to 20 to 10 if the
	 * signal being resampled is non-dynamic (e.g., an impulse response or
	 * filter, with a non-steep frequency response).
	 * @param ReqPhase Required filter's phase response. Note that this
	 * setting does not affect interpolator's phase response which is always
	 * linear-phase. Also note that if the "power of 2" resampling was engaged
	 * by the resampler together with the minimum-phase response, the audio
	 * stream may become fractionally delayed, depending on the minimum-phase
	 * filter's actual fractional delay. Linear-phase filters do not have
	 * fractional delay.
	 * @see CDSPFIRFilterCache::getLPFilter()
	 */

	CDSPResampler( const double SrcSampleRate, const double DstSampleRate,
		const int aMaxInLen, const double ReqTransBand = 2.0,
		const double ReqAtten = 206.91,
		const EDSPFilterPhaseResponse ReqPhase = fprLinearPhase )
		: StepCapacity( 0 )
		, StepCount( 0 )
		, MaxInLen( aMaxInLen )
		, CurMaxOutLen( aMaxInLen )
		, LatencyFrac( 0.0 )
	{
		R8BASSERT( SrcSampleRate > 0.0 );
		R8BASSERT( DstSampleRate > 0.0 );
		R8BASSERT( MaxInLen > 0 );

		R8BCONSOLE( "* CDSPResampler: src=%.1f dst=%.1f len=%i tb=%.1f "
			"att=%.2f ph=%i\n", SrcSampleRate, DstSampleRate, aMaxInLen,
			ReqTransBand, ReqAtten, (int) ReqPhase );

		if( SrcSampleRate == DstSampleRate )
		{
			return;
		}

		TmpBufCapacities[ 0 ] = 0;
		TmpBufCapacities[ 1 ] = 0;
		CurTmpBuf = 0;

		// Try some common efficient ratios requiring only a single step.

		const int CommonRatioCount = 5;
		const int CommonRatios[ CommonRatioCount ][ 2 ] = {
			{ 1, 2 },
			{ 1, 3 },
			{ 2, 3 },
			{ 3, 2 },
			{ 3, 4 }
		};

		int i;

		for( i = 0; i < CommonRatioCount; i++ )
		{
			const int num = CommonRatios[ i ][ 0 ];
			const int den = CommonRatios[ i ][ 1 ];

			if( SrcSampleRate * num == DstSampleRate * den )
			{
				addProcessor( new CDSPBlockConvolver(
					CDSPFIRFilterCache :: getLPFilter(
					1.0 / ( num > den ? num : den ), ReqTransBand,
					ReqAtten, ReqPhase, num ), num, den, LatencyFrac ));

				createTmpBuffers();
				return;
			}
		}

		// Try whole-number power-of-2 or 3*power-of-2 upsampling.

		for( i = 2; i <= 3; i++ )
		{
			bool WasFound = false;
			int c = 0;

			while( true )
			{
				const double NewSR = SrcSampleRate * ( i << c );

				if( NewSR == DstSampleRate )
				{
					WasFound = true;
					break;
				}

				if( NewSR > DstSampleRate )
				{
					break;
				}

				c++;
			}

			if( WasFound )
			{
				addProcessor( new CDSPBlockConvolver(
					CDSPFIRFilterCache :: getLPFilter( 1.0 / i, ReqTransBand,
					ReqAtten, ReqPhase, i ), i, 1, LatencyFrac ));

				const bool IsThird = ( i == 3 );

				for( i = 0; i < c; i++ )
				{
					addProcessor( new CDSPHBUpsampler( ReqAtten, i, IsThird,
						LatencyFrac ));
				}

				createTmpBuffers();
				return;
			}
		}

		if( DstSampleRate * 2.0 > SrcSampleRate )
		{
			// Upsampling or fractional downsampling down to 2X.

			const double NormFreq = ( DstSampleRate > SrcSampleRate ? 0.5 :
				0.5 * DstSampleRate / SrcSampleRate );

			addProcessor( new CDSPBlockConvolver(
				CDSPFIRFilterCache :: getLPFilter( NormFreq, ReqTransBand,
				ReqAtten, ReqPhase, 2.0 ), 2, 1, LatencyFrac ));

			// Try intermediate interpolated resampling with subsequent 2X
			// or 3X upsampling.

			const double tbw = 0.0175; // Intermediate filter's transition
				// band extension coefficient.
			const double ThreshSampleRate = SrcSampleRate /
				( 1.0 - tbw * ReqTransBand ); // Make sure intermediate
				// filter's transition band is not steeper than ReqTransBand
				// (this keeps the latency under control).

			int c = 0;
			int div = 1;

			while( true )
			{
				const int ndiv = div * 2;

				if( DstSampleRate < ThreshSampleRate * ndiv )
				{
					break;
				}

				div = ndiv;
				c++;
			}

			int c2 = 0;
			int div2 = 1;

			while( true )
			{
				const int ndiv = div * ( c2 == 0 ? 3 : 2 );

				if( DstSampleRate < ThreshSampleRate * ndiv )
				{
					break;
				}

				div2 = ndiv;
				c2++;
			}

			const double SrcSampleRate2 = SrcSampleRate * 2.0;
			int tmp1;
			int tmp2;

			if( c == 1 && getWholeStepping( SrcSampleRate2, DstSampleRate,
				tmp1, tmp2 ))
			{
				// Do not use intermediate interpolation if whole stepping is
				// available as it performs very fast.

				c = 0;
			}

			if( c > 0 )
			{
				// Add steps using intermediate interpolation.

				int num;

				if( c2 > 0 && div2 > div )
				{
					div = div2;
					c = c2;
					num = 3;
				}
				else
				{
					num = 2;
				}

				addProcessor( new CDSPFracInterpolator( SrcSampleRate2 * div,
					DstSampleRate, ReqAtten, false, LatencyFrac ));

				double tb = ( 1.0 - SrcSampleRate * div / DstSampleRate ) /
					tbw; // Divide TransBand by a constant that assures a
					// linear response in the pass-band.

				if( tb > CDSPFIRFilter :: getLPMaxTransBand() )
				{
					tb = CDSPFIRFilter :: getLPMaxTransBand();
				}

				addProcessor( new CDSPBlockConvolver(
					CDSPFIRFilterCache :: getLPFilter( 1.0 / num, tb,
					ReqAtten, ReqPhase, num ), num, 1, LatencyFrac ));

				const bool IsThird = ( num == 3 );

				for( i = 1; i < c; i++ )
				{
					addProcessor( new CDSPHBUpsampler( ReqAtten, i - 1,
						IsThird, LatencyFrac ));
				}
			}
			else
			{
				addProcessor( new CDSPFracInterpolator( SrcSampleRate2,
					DstSampleRate, ReqAtten, false, LatencyFrac ));
			}

			createTmpBuffers();
			return;
		}

		// Use downsampling steps, including power-of-2 downsampling.

		double CheckSR = DstSampleRate * 4.0;
		int c = 0;
		double FinGain = 1.0;

		while( CheckSR <= SrcSampleRate )
		{
			c++;
			CheckSR *= 2.0;
			FinGain *= 0.5;
		}

		const int SrcSRDiv = ( 1 << c );
		int downf;
		double NormFreq = 0.5;
		bool UseInterp = true;
		bool IsThird = false;

		for( downf = 2; downf <= 3; downf++ )
		{
			if( DstSampleRate * SrcSRDiv * downf == SrcSampleRate )
			{
				NormFreq = 1.0 / downf;
				UseInterp = false;
				IsThird = ( downf == 3 );
				break;
			}
		}

		if( UseInterp )
		{
			downf = 1;
			NormFreq = DstSampleRate * SrcSRDiv / SrcSampleRate;
			IsThird = ( NormFreq * 3.0 <= 1.0 );
		}

		for( i = 0; i < c; i++ )
		{
			// Use a fixed very relaxed 2X downsampling filters, that at
			// the final stage only guarantees stop-band between 0.75 and
			// pi. 0.5-0.75 range will be aliased to 0.25-0.5 range which
			// will then be filtered out by the final filter.

			addProcessor( new CDSPHBDownsampler( ReqAtten, c - 1 - i, IsThird,
				LatencyFrac ));
		}

		addProcessor( new CDSPBlockConvolver(
			CDSPFIRFilterCache :: getLPFilter( NormFreq, ReqTransBand,
			ReqAtten, ReqPhase, FinGain ), 1, downf, LatencyFrac ));

		if( UseInterp )
		{
			addProcessor( new CDSPFracInterpolator( SrcSampleRate,
				DstSampleRate * SrcSRDiv, ReqAtten, IsThird, LatencyFrac ));
		}

		createTmpBuffers();
	}

	virtual ~CDSPResampler()
	{
		int i;

		for( i = 0; i < StepCount; i++ )
		{
			delete Steps[ i ];
		}
	}

	virtual int getLatency() const
	{
		return( 0 );
	}

	virtual double getLatencyFrac() const
	{
		return( LatencyFrac );
	}

	/**
	 * This function ignores the supplied parameter and returns the maximal
	 * output buffer length that depends on the MaxInLen supplied to the
	 * constructor.
	 */

	virtual int getMaxOutLen( const int/* MaxInLen */ ) const
	{
		return( CurMaxOutLen );
	}

	/**
	 * Function clears (resets) the state of *this object and returns it to
	 * the state after construction. All input data accumulated in the
	 * internal buffer so far will be discarded.
	 *
	 * This function makes it possible to use *this object for converting
	 * separate streams from the same source sample rate to the same
	 * destination sample rate without reconstructing the object. It is more
	 * efficient to clear the state of the resampler object than to destroy it
	 * and create a new object.
	 */

	virtual void clear()
	{
		int i;

		for( i = 0; i < StepCount; i++ )
		{
			Steps[ i ] -> clear();
		}
	}

	/**
	 * Function performs sample rate conversion.
	 *
	 * If the source and destination sample rates are equal, the resampler
	 * will do nothing and will simply return the input buffer unchanged.
	 *
	 * You do not need to allocate an intermediate output buffer for use with
	 * this function. If required, the resampler will allocate a suitable
	 * intermediate output buffer itself.
	 *
	 * @param ip0 Input buffer. This buffer is never used as output buffer by
	 * this function. This pointer may be returned in "op0" if no resampling
	 * is happening (source sample rate equals destination sample rate).
	 * @param l The number of samples available in the input buffer. Should
	 * not exceed the MaxInLen supplied in the constructor.
	 * @param[out] op0 This variable receives the pointer to the resampled
	 * data. On function's return, this pointer points to *this object's
	 * internal buffer. In real-time applications it is suggested to pass this
	 * pointer to the next output audio block and consume any data left from
	 * the previous output audio block first before calling the process()
	 * function again. The buffer pointed to by the "op0" on return is owned
	 * by the resampler, so it should not be freed by the caller.
	 * @return The number of samples available in the "op0" output buffer. If
	 * the data from the output buffer "op0" is going to be written to a
	 * bigger output buffer, it is suggested to check the returned number of
	 * samples so that no overflow of the bigger output buffer happens.
	 */

	virtual int process( double* ip0, int l, double*& op0 )
	{
		R8BASSERT( l >= 0 );

		double* ip = ip0;
		int i;

		for( i = 0; i < StepCount; i++ )
		{
			double* op = TmpBufs[ i & 1 ];
			l = Steps[ i ] -> process( ip, l, op );
			ip = op;
		}

		op0 = ip;
		return( l );
	}

	/**
	 * Function performs resampling of an input sample buffer of the specified
	 * length in the "one-shot" mode. This function can be useful when impulse
	 * response resampling is required.
	 *
	 * @param ip Input buffer pointer.
	 * @param iplen Length of the input buffer in samples.
	 * @param[out] op Output buffer pointer.
	 * @param oplen Length of the output buffer in samples.
	 * @tparam Tin Input buffer's element type.
	 * @tparam Tout Output buffer's element type.
	 */

	template< typename Tin, typename Tout >
	void oneshot( const Tin* ip, int iplen, Tout* op, int oplen )
	{
		CFixedBuffer< double > Buf( MaxInLen );
		bool IsZero = false;

		while( oplen > 0 )
		{
			int rc;
			double* p;
			int i;

			if( iplen == 0 )
			{
				rc = MaxInLen;
				p = &Buf[ 0 ];

				if( !IsZero )
				{
					IsZero = true;
					memset( p, 0, MaxInLen * sizeof( p[ 0 ]));
				}
			}
			else
			{
				rc = min( iplen, MaxInLen );

				if( sizeof( Tin ) == sizeof( double ))
				{
					p = (double*) ip;
				}
				else
				{
					p = &Buf[ 0 ];

					for( i = 0; i < rc; i++ )
					{
						p[ i ] = ip[ i ];
					}
				}

				ip += rc;
				iplen -= rc;
			}

			double* op0;
			int wc = process( p, rc, op0 );
			wc = min( oplen, wc );

			for( i = 0; i < wc; i++ )
			{
				op[ i ] = (Tout) op0[ i ];
			}

			op += wc;
			oplen -= wc;
		}

		clear();
	}

	/**
	 * Function obtains overall input sample count required to produce first
	 * output sample. Function works by iteratively passing 1 sample at a time
	 * until output begins. This is a relatively CPU-consuming operation. This
	 * function should be called after the clear() function call or after
	 * object's construction. The function itself calls the clear() function
	 * before return.
	 *
	 * Note that it is advisable to cache the value returned by this function,
	 * for each SrcSampleRate/DstSampleRate pair, if it is called frequently.
	 */

	int getInLenBeforeOutStart()
	{
		int inc = 0;

		while( true )
		{
			double ins = 0.0;
			double* op;

			if( process( &ins, 1, op ) > 0 )
			{
				clear();
				return( inc );
			}

			inc++;
		}
	}

private:
	CFixedBuffer< CDSPProcessor* > Steps; ///< Array of processing steps.
		///<
	int StepCapacity; ///< The capacity of the Steps array.
		///<
	int StepCount; ///< The number of created processing steps.
		///<
	int MaxInLen; ///< Maximal input length.
		///<
	CFixedBuffer< double > TmpBufAll; ///< Buffer containing both temporary
		///< buffers.
		///<
	double* TmpBufs[ 2 ]; ///< Temporary output buffers.
		///<
	int TmpBufCapacities[ 2 ]; ///< Capacities of temporary buffers, updated
		///< during processing steps building.
		///<
	int CurTmpBuf; ///< Current temporary buffer.
		///<
	int CurMaxOutLen; ///< Current maximal output length.
		///<
	double LatencyFrac; ///< Current fractional latency. After object's
		///< construction, equals to the remaining fractional latency in the
		///< output.
		///<

	/**
	 * Function adds processor, updates MaxOutLen variable and adjusts length
	 * of temporary internal buffers.
	 *
	 * @param Proc Processor to add. This pointer is inherited and will be
	 * destroyed on *this object's destruction.
	 */

	void addProcessor( CDSPProcessor* const Proc )
	{
		if( StepCount == StepCapacity )
		{
			// Reallocate and increase Steps array's capacity.

			const int NewCapacity = StepCapacity + 8;
			Steps.realloc( StepCapacity, NewCapacity );
			StepCapacity = NewCapacity;
		}

		LatencyFrac = Proc -> getLatencyFrac();
		CurMaxOutLen = Proc -> getMaxOutLen( CurMaxOutLen );

		if( CurMaxOutLen > TmpBufCapacities[ CurTmpBuf ])
		{
			TmpBufCapacities[ CurTmpBuf ] = CurMaxOutLen;
		}

		CurTmpBuf ^= 1;

		Steps[ StepCount ] = Proc;
		StepCount++;
	}

	/**
	 * Function creates temporary buffers.
	 */

	void createTmpBuffers()
	{
		const int ol = TmpBufCapacities[ 0 ] + TmpBufCapacities[ 1 ];

		if( ol > 0 )
		{
			TmpBufAll.alloc( ol );
			TmpBufs[ 0 ] = &TmpBufAll[ 0 ];
			TmpBufs[ 1 ] = &TmpBufAll[ TmpBufCapacities[ 0 ]];
		}

		R8BCONSOLE( "* CDSPResampler: init done\n" );
	}
};

/**
 * @brief The resampler class for 16-bit resampling.
 *
 * This class defines resampling parameters suitable for 16-bit resampling,
 * using linear-phase low-pass filter. See the r8b::CDSPResampler class for
 * details.
 */

class CDSPResampler16 : public CDSPResampler
{
public:
	/**
	 * Constructor initializes the 16-bit resampler. See the
	 * r8b::CDSPResampler class for details.
	 *
	 * @param SrcSampleRate Source signal sample rate.
	 * @param DstSampleRate Destination signal sample rate.
	 * @param aMaxInLen The maximal planned length of the input buffer (in
	 * samples) that will be passed to the resampler.
	 * @param ReqTransBand Required transition band, in percent.
	 */

	CDSPResampler16( const double SrcSampleRate, const double DstSampleRate,
		const int aMaxInLen, const double ReqTransBand = 2.0 )
		: CDSPResampler( SrcSampleRate, DstSampleRate, aMaxInLen, ReqTransBand,
			136.45, fprLinearPhase )
	{
	}
};

/**
 * @brief The resampler class for 16-bit impulse response resampling.
 *
 * This class defines resampling parameters suitable for 16-bit impulse
 * response resampling, using linear-phase low-pass filter. Impulse responses
 * are non-dynamic signals, and thus need resampler with a lesser SNR. See the
 * r8b::CDSPResampler class for details.
 */

class CDSPResampler16IR : public CDSPResampler
{
public:
	/**
	 * Constructor initializes the 16-bit impulse response resampler. See the
	 * r8b::CDSPResampler class for details.
	 *
	 * @param SrcSampleRate Source signal sample rate.
	 * @param DstSampleRate Destination signal sample rate.
	 * @param aMaxInLen The maximal planned length of the input buffer (in
	 * samples) that will be passed to the resampler.
	 * @param ReqTransBand Required transition band, in percent.
	 */

	CDSPResampler16IR( const double SrcSampleRate, const double DstSampleRate,
		const int aMaxInLen, const double ReqTransBand = 2.0 )
		: CDSPResampler( SrcSampleRate, DstSampleRate, aMaxInLen, ReqTransBand,
			109.56, fprLinearPhase )
	{
	}
};

/**
 * @brief The resampler class for 24-bit resampling.
 *
 * This class defines resampling parameters suitable for 24-bit resampling
 * (including 32-bit floating point resampling), using linear-phase low-pass
 * filter. See the r8b::CDSPResampler class for details.
 */

class CDSPResampler24 : public CDSPResampler
{
public:
	/**
	 * Constructor initializes the 24-bit resampler (including 32-bit floating
	 * point). See the r8b::CDSPResampler class for details.
	 *
	 * @param SrcSampleRate Source signal sample rate.
	 * @param DstSampleRate Destination signal sample rate.
	 * @param aMaxInLen The maximal planned length of the input buffer (in
	 * samples) that will be passed to the resampler.
	 * @param ReqTransBand Required transition band, in percent.
	 */

	CDSPResampler24( const double SrcSampleRate, const double DstSampleRate,
		const int aMaxInLen, const double ReqTransBand = 2.0 )
		: CDSPResampler( SrcSampleRate, DstSampleRate, aMaxInLen, ReqTransBand,
			180.15, fprLinearPhase )
	{
	}
};

} // namespace r8b

#endif // R8B_CDSPRESAMPLER_INCLUDED
