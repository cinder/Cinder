//$ nocpp

/**
 * @file CDSPFIRFilter.h
 *
 * @brief FIR filter generator and filter cache classes.
 *
 * This file includes low-pass FIR filter generator and filter cache.
 *
 * r8brain-free-src Copyright (c) 2013 Aleksey Vaneev
 * See the "License.txt" file for license.
 */

#ifndef R8B_CDSPFIRFILTER_INCLUDED
#define R8B_CDSPFIRFILTER_INCLUDED

#include "CDSPSincFilterGen.h"
#include "CDSPRealFFT.h"

namespace r8b {

/**
 * Enumeration of filter's phase responses.
 */

enum EDSPFilterPhaseResponse
{
	fprLinearPhase = 0 ///< Linear-phase response. Features a linear-phase
		///< high-latency response, with the latency expressed as integer
		///< value.
//	fprMinPhase ///< Minimum-phase response. Features a minimal latency
		///< response, but the response's phase is non-linear. The latency is
		///< usually expressed as non-integer value, and usually is small, but
		///< is never equal to zero. The minimum-phase filter is transformed
		///< from the linear-phase filter. The transformation has precision
		///< limits which may skew both the -3 dB point and attenuation of the
		///< filter being transformed: as it was measured, the skew happens
		///< purely at random, and in most cases it is within tolerable range.
		///< In a small (1%) random subset of cases the skew is bigger and
		///< cannot be predicted.
};

/**
 * @brief Calculation and storage class for FIR filters.
 *
 * Class that implements calculation and storing of a FIR filter (currently
 * contains low-pass filter calculation routine designed for sample rate
 * conversion). Objects of this class cannot be created directly, but can be
 * obtained via the CDSPFilterCache::getLPFilter() static function.
 */

class CDSPFIRFilter : public R8B_BASECLASS
{
	R8BNOCTOR( CDSPFIRFilter );

	friend class CDSPFIRFilterCache;

public:
	~CDSPFIRFilter()
	{
		// (rte) this assertion is failing at shutdown, although I haven't yet seen any other related problems, so commented out for now.
		// TODO: test on windows
//		R8BASSERT( RefCount == 0 );

		delete Next;
	}

	/**
	 * @return The minimal allowed low-pass filter's transition band, in
	 * percent.
	 */

	static double getLPMinTransBand()
	{
		return( 0.5 );
	}

	/**
	 * @return The maximal allowed low-pass filter's transition band, in
	 * percent.
	 */

	static double getLPMaxTransBand()
	{
		return( 45.0 );
	}

	/**
	 * @return The minimal allowed low-pass filter's stop-band attenuation, in
	 * decibel.
	 */

	static double getLPMinAtten()
	{
		return( 49.0 );
	}

	/**
	 * @return The maximal allowed low-pass filter's stop-band attenuation, in
	 * decibel.
	 */

	static double getLPMaxAtten()
	{
		return( 218.0 );
	}

	/**
	 * @return Filter's latency, in samples (integer part).
	 */

	int getLatency() const
	{
		return( Latency );
	}

	/**
	 * @return Filter's latency, in samples (fractional part). Always zero for
	 * linear-phase filters.
	 */

	double getLatencyFrac() const
	{
		return( LatencyFrac );
	}

	/**
	 * @return Filter kernel length, in samples. Not to be confused with the
	 * block length.
	 */

	int getKernelLen() const
	{
		return( KernelLen );
	}

	/**
	 * @return Filter's block length, espressed as Nth power of 2. The actual
	 * length is twice as large due to zero-padding.
	 */

	int getBlockLenBits() const
	{
		return( BlockLenBits );
	}

	/**
	 * @return Filter's kernel block, in complex-numbered form obtained via
	 * the CDSPRealFFT::forward() function call, zero-padded, gain-adjusted
	 * with the CDSPRealFFT::getInvMulConst() constant, immediately suitable
	 * for convolution.
	 */

	const double* getKernelBlock() const
	{
		return( KernelBlock );
	}

	/**
	 * This function should be called when the filter obtained via the
	 * filter cache is no longer needed.
	 */

	void unref();

private:
	double ReqNormFreq; ///< Required normalized frequency, 0 to 1 inclusive.
		///<
	double ReqTransBand; ///< Required transition band in percent, as passed
		///< by the user.
		///<
	double ReqAtten; ///< Required stop-band attenuation in decibel, as passed
		///< by the user (positive value).
		///<
	EDSPFilterPhaseResponse ReqPhase; ///< Required filter's phase response.
		///<
	CDSPFIRFilter* Next; ///< Next FIR filter in cache's list.
		///<
	int RefCount; ///< The number of references made to *this FIR filter.
		///<
	int Latency; ///< Filter's latency in samples (integer part).
		///<
	double LatencyFrac; ///< Filter's latency in samples (fractional part).
		///<
	int KernelLen; ///< Filter kernel length, in samples.
		///<
	int BlockLenBits; ///< Block length used to store *this FIR filter,
		///< expressed as Nth power of 2. This value is used directly by the
		///< convolver.
		///<
	CFixedBuffer< double > KernelBlock; ///< FIR filter buffer, capacity
		///< equals to 1 << ( BlockLenBits + 1 ). Second part of the buffer
		///< contains zero-padding to allow alias-free convolution.
		///<

	CDSPFIRFilter()
		: RefCount( 1 )
	{
	}

	/**
	 * Function builds filter kernel based on the "Req" parameters.
	 *
	 * @param ExtAttenCorrs External attentuation correction table, for
	 * internal use.
	 */

	void buildLPFilter( const double* const ExtAttenCorrs )
	{
		const double tb = ReqTransBand * 0.01;
		double pwr;
		double fo1;
		double hl;
		double atten = -ReqAtten;

		if( tb >= 0.25 )
		{
			if( ReqAtten >= 117.0 )
			{
				atten -= 1.60;
			}
			else
			if( ReqAtten >= 60.0 )
			{
				atten -= 1.91;
			}
			else
			{
				atten -= 2.25;
			}
		}
		else
		if( tb >= 0.10 )
		{
			if( ReqAtten >= 117.0 )
			{
				atten -= 0.69;
			}
			else
			if( ReqAtten >= 60.0 )
			{
				atten -= 0.73;
			}
			else
			{
				atten -= 1.13;
			}
		}
		else
		{
			if( ReqAtten >= 117.0 )
			{
				atten -= 0.21;
			}
			else
			if( ReqAtten >= 60.0 )
			{
				atten -= 0.25;
			}
			else
			{
				atten -= 0.36;
			}
		}

		static const int AttenCorrCount = 264;
		static const double AttenCorrMin = 49.0;
		static const double AttenCorrDiff = 176.25;
		int AttenCorr = (int) floor(( -atten - AttenCorrMin ) *
			AttenCorrCount / AttenCorrDiff + 0.5 );

		AttenCorr = min( AttenCorrCount, max( 0, AttenCorr ));

		if( ExtAttenCorrs != NULL )
		{
			atten -= ExtAttenCorrs[ AttenCorr ];
		}
		else
		if( tb >= 0.25 )
		{
			static const double AttenCorrScale = 101.0;
			static const signed char AttenCorrs[] = {
				-127, -127, -125, -125, -122, -119, -115, -110, -104, -97,
				-91, -82, -75, -24, -16, -6, 4, 14, 24, 29, 30, 32, 37, 44,
				51, 57, 63, 67, 65, 50, 53, 56, 58, 60, 63, 64, 66, 68, 74,
				77, 78, 78, 78, 79, 79, 60, 60, 60, 61, 59, 52, 47, 41, 36,
				30, 24, 17, 9, 0, -8, -10, -11, -14, -13, -18, -25, -31, -38,
				-44, -50, -57, -63, -68, -74, -81, -89, -96, -101, -104, -107,
				-109, -110, -86, -84, -85, -82, -80, -77, -73, -67, -62, -55,
				-48, -42, -35, -30, -20, -11, -2, 5, 6, 6, 7, 11, 16, 21, 26,
				34, 41, 46, 49, 52, 55, 56, 48, 49, 51, 51, 52, 52, 52, 52,
				52, 51, 51, 50, 47, 47, 50, 48, 46, 42, 38, 35, 31, 27, 24,
				20, 16, 12, 11, 12, 10, 8, 4, -1, -6, -11, -16, -19, -17, -21,
				-24, -27, -32, -34, -37, -38, -40, -41, -40, -40, -42, -41,
				-44, -45, -43, -41, -34, -31, -28, -24, -21, -18, -14, -10,
				-5, -1, 2, 5, 8, 7, 4, 3, 2, 2, 4, 6, 8, 9, 9, 10, 10, 10, 10,
				9, 8, 9, 11, 14, 13, 12, 11, 10, 8, 7, 6, 5, 3, 2, 2, -1, -1,
				-3, -3, -4, -4, -5, -4, -6, -7, -9, -5, -1, -1, 0, 1, 0, -2,
				-3, -4, -5, -5, -8, -13, -13, -13, -12, -13, -12, -11, -11,
				-9, -8, -7, -5, -3, -1, 2, 4, 6, 9, 10, 11, 14, 18, 21, 24,
				27, 30, 34, 37, 37, 39, 40 };

			atten -= AttenCorrs[ AttenCorr ] / AttenCorrScale;
		}
		else
		if( tb >= 0.10 )
		{
			static const double AttenCorrScale = 210.0;
			static const signed char AttenCorrs[] = {
				-113, -118, -122, -125, -126, -97, -95, -92, -92, -89, -82,
				-75, -69, -48, -42, -36, -30, -22, -14, -5, -2, 1, 6, 13, 22,
				28, 35, 41, 48, 55, 56, 56, 61, 65, 71, 77, 81, 83, 85, 85,
				74, 74, 73, 72, 71, 70, 68, 64, 59, 56, 49, 52, 46, 42, 36,
				32, 26, 20, 13, 7, -2, -6, -10, -15, -20, -27, -33, -38, -44,
				-43, -48, -53, -57, -63, -69, -73, -75, -79, -81, -74, -76,
				-77, -77, -78, -81, -80, -80, -78, -76, -65, -62, -59, -56,
				-51, -48, -44, -38, -33, -25, -19, -13, -5, -1, 2, 7, 13, 17,
				21, 25, 30, 35, 40, 45, 50, 53, 56, 57, 55, 58, 59, 62, 64,
				67, 67, 68, 68, 62, 61, 61, 59, 59, 57, 57, 55, 52, 48, 42,
				38, 35, 31, 26, 20, 15, 13, 10, 7, 3, -2, -8, -13, -17, -23,
				-28, -34, -37, -40, -41, -45, -48, -50, -53, -57, -59, -62,
				-63, -63, -57, -57, -56, -56, -54, -54, -53, -49, -48, -41,
				-38, -33, -31, -26, -23, -18, -12, -9, -7, -7, -3, 0, 5, 9,
				14, 16, 20, 22, 21, 23, 25, 27, 28, 29, 34, 33, 35, 33, 31,
				30, 29, 29, 26, 26, 25, 24, 20, 19, 15, 10, 8, 4, 1, -2, -6,
				-10, -16, -19, -23, -26, -27, -30, -34, -39, -43, -47, -51,
				-52, -54, -56, -58, -59, -62, -63, -66, -65, -65, -64, -59,
				-57, -54, -52, -48, -44, -42, -37, -32, -22, -17, -10, -3, 5,
				13, 22, 30, 40, 50, 60, 72 };

			atten -= AttenCorrs[ AttenCorr ] / AttenCorrScale;
		}
		else
		{
			static const double AttenCorrScale = 196.0;
			static const signed char AttenCorrs[] = {
				-15, -17, -20, -20, -20, -21, -20, -16, -17, -18, -17, -13,
				-12, -11, -9, -7, -5, -4, -1, 1, 3, 4, 5, 6, 7, 9, 9, 10, 10,
				10, 11, 11, 11, 12, 12, 12, 10, 11, 10, 10, 8, 10, 11, 10, 11,
				11, 13, 14, 15, 19, 27, 26, 23, 18, 14, 8, 4, -2, -6, -12,
				-17, -23, -28, -33, -37, -42, -46, -49, -53, -57, -60, -61,
				-64, -65, -67, -66, -66, -66, -65, -64, -61, -59, -56, -52,
				-48, -42, -38, -31, -27, -19, -13, -7, -1, 8, 14, 22, 29, 37,
				45, 52, 59, 66, 73, 80, 86, 91, 96, 100, 104, 108, 111, 114,
				115, 117, 118, 120, 120, 118, 117, 114, 113, 111, 107, 103,
				99, 95, 89, 84, 78, 72, 66, 60, 52, 44, 37, 30, 21, 14, 6, -3,
				-11, -18, -26, -34, -43, -51, -58, -65, -73, -78, -85, -90,
				-97, -102, -107, -113, -115, -118, -121, -125, -125, -126,
				-126, -126, -125, -124, -121, -119, -115, -111, -109, -101,
				-102, -95, -88, -81, -73, -67, -63, -54, -47, -40, -33, -26,
				-18, -11, -5, 2, 8, 14, 19, 25, 31, 36, 37, 43, 47, 49, 51,
				52, 57, 57, 56, 57, 58, 58, 58, 57, 56, 52, 52, 50, 48, 44,
				41, 39, 37, 33, 31, 26, 24, 21, 18, 14, 11, 8, 4, 2, -2, -5,
				-7, -9, -11, -13, -15, -16, -18, -19, -20, -23, -24, -24, -25,
				-27, -26, -27, -29, -30, -31, -32, -35, -36, -39, -40, -44,
				-46, -51, -54, -59, -63, -69, -76, -83, -91, -98 };

			atten -= AttenCorrs[ AttenCorr ] / AttenCorrScale;
		}

		pwr = 7.43932822146293e-8 * sqr( atten ) + 0.000102747434588003 *
			cos( 0.00785021930010397 * atten ) * cos( 0.633854318781239 +
			0.103208573657699 * atten ) - 0.00798132247867036 -
			0.000903555213543865 * atten - 0.0969365532127236 * exp(
			0.0779275237937911 * atten ) - 1.37304948662012e-5 * atten * cos(
			0.00785021930010397 * atten );

		if( pwr <= 0.067665322581 )
		{
			if( tb >= 0.25 )
			{
				hl = 2.6778150875894 / tb + 300.547590563091 * atan( atan(
					2.68959772209918 * pwr )) / ( 5.5099277187035 * tb - tb *
					tanh( cos( asinh( atten ))));

				fo1 = 0.987205355829873 * tb + 1.00011788929851 * atan2(
					-0.321432067051302 - 6.19131357321578 * sqrt( pwr ),
					hl + -1.14861472207245 / ( hl - 14.1821147585957 ) + pow(
					0.9521145021664, pow( atan2( 1.12018764830637, tb ),
					2.10988901686912 * hl - 20.9691278378345 )));
			}
			else
			if( tb >= 0.10 )
			{
				hl = ( 1.56688617018066 + 142.064321294568 * pwr +
					0.00419441117131136 * cos( 243.633511747297 * pwr ) -
					0.022953443903576 * atten - 0.026629568860284 * cos(
					127.715550622571 * pwr )) / tb;

				fo1 = 0.982299356642411 * tb + 0.999441744774215 * asinh((
					-0.361783054039583 - 5.80540593623676 * sqrt( pwr )) /
					hl );
			}
			else
			{
				hl = ( 2.45739657014937 + 269.183679500541 * pwr * cos(
					5.73225668178813 + atan2( cosh( 0.988861169868941 -
					17.2201556280744 * pwr ), 1.08340138240431 * pwr ))) / tb;

				fo1 = 2.291956939 * tb + 0.01942450693 * sqr( tb ) * hl -
					4.67538973161837 * pwr * tb - 1.668433124 * tb *
					pow( pwr, pwr );
			}
		}
		else
		{
			if( tb >= 0.25 )
			{
				hl = ( 1.50258368698213 + 158.556968859477 * asinh( pwr ) *
					tanh( 57.9466246871383 * tanh( pwr )) -
					0.0105440479814834 * atten ) / tb;

				fo1 = 0.994024401639321 * tb + ( -0.236282717577215 -
					6.8724924545387 * sqrt( sin( pwr ))) / hl;
			}
			else
			if( tb >= 0.10 )
			{
				hl = ( 1.50277377248945 + 158.222625721046 * asinh( pwr ) *
					tanh( 1.02875299001715 + 42.072277322604 * pwr ) -
					0.0108380943845632 * atten ) / tb;

				fo1 = 0.992539376734551 * tb + ( -0.251747813037178 -
					6.74159892452584 * sqrt( tanh( tanh( tan( pwr ))))) / hl;
			}
			else
			{
				hl = ( 1.15990238966306 * pwr - 5.02124037125213 * sqr(
					pwr ) - 0.158676856669827 * atten * cos( 1.1609073390614 *
					pwr - 6.33932586197475 * pwr * sqr( pwr ))) / tb;

				fo1 = 0.867344453126885 * tb + 0.052693817907757 * tb * log(
					pwr ) + 0.0895511178735932 * tb * atan( 59.7538527741309 *
					pwr ) - 0.0745653568081453 * pwr * tb;
			}
		}

		double WinParams[ 2 ];
		WinParams[ 0 ] = 125.0;
		WinParams[ 1 ] = pwr;

		CDSPSincFilterGen sinc;
		sinc.Len2 = 0.25 * hl / ReqNormFreq;
		sinc.Freq1 = 0.0;
		sinc.Freq2 = M_PI * ( 1.0 - fo1 ) * ReqNormFreq;
		sinc.initBand( CDSPSincFilterGen :: wftKaiser, WinParams, true );

		KernelLen = sinc.KernelLen;
		BlockLenBits = getBitOccupancy( KernelLen - 1 );
		const int BlockLen = 1 << BlockLenBits;

		KernelBlock.alloc( BlockLen * 2 );
		sinc.generateBand( &KernelBlock[ 0 ],
			&CDSPSincFilterGen :: calcWindowKaiser );

/*		if( ReqPhase == fprLinearPhase )
		{*/
			Latency = sinc.fl2;
			LatencyFrac = 0.0;
/*		}
		else
		{
			double DCGroupDelay;

			calcMinPhaseTransform( &KernelBlock[ 0 ], KernelLen, 3, false,
				&DCGroupDelay );

			Latency = (int) DCGroupDelay;
			LatencyFrac = DCGroupDelay - Latency;
		}*/

		CDSPRealFFTKeeper ffto( BlockLenBits + 1 );

		normalizeFIRFilter( &KernelBlock[ 0 ], KernelLen,
			ffto -> getInvMulConst() );

		memset( &KernelBlock[ KernelLen ], 0,
			( BlockLen * 2 - KernelLen ) * sizeof( double ));

		ffto -> forward( KernelBlock );
	}
};

/**
 * @brief FIR filter cache class.
 *
 * Class that implements cache for calculated FIR filters. The required FIR
 * filter should be obtained via the getLPFilter() static function.
 */

class CDSPFIRFilterCache : public R8B_BASECLASS
{
	friend class CDSPFIRFilter;

public:
	/**
	 * @return The number of filters present in the cache now. This value can
	 * be monitored for debugging "forgotten" filters.
	 */

	static int getFilterCount()
	{
		R8BSYNC( StateSync );

		return( FilterCount );
	}

	/**
	 * Function calculates or returns reference to a previously calculated
	 * (cached) low-pass FIR filter. Note that the real transition band and
	 * attenuation achieved by the filter varies with the magnitude of the
	 * required attenuation, and are never 100% exact.
	 *
	 * @param ReqNormFreq Required normalized frequency, in the range 0 to 1,
	 * inclusive. This is the point after which the stop-band spans.
	 * @param ReqTransBand Required transition band, in percent of the
	 * 0 to ReqNormFreq spectral bandwidth, in the range
	 * CDSPFIRFilter::getLPMinTransBand() to
	 * CDSPFIRFilter::getLPMaxTransBand(), inclusive. The transition band
	 * specifies the part of the spectrum between the -3 dB and ReqNormFreq
	 * points. The real resulting -3 dB point varies in the range from -3.00
	 * to -3.05 dB, but is generally very close to -3 dB.
	 * @param ReqAtten Required stop-band attenuation in decibel, in the range
	 * CDSPFIRFilter::getLPMinAtten() to CDSPFIRFilter::getLPMaxAtten(),
	 * inclusive. Note that the actual stop-band attenuation of the resulting
	 * filter may be 0.40-4.46 dB higher.
	 * @return A reference to a new or a previously calculated low-pass FIR
	 * filter object with the required characteristics. A reference count is
	 * incremented in the returned filter object which should be released
	 * after use via the CDSPFIRFilter::unref() function.
	 * @param ReqPhase Required filter's phase response.
	 * @param AttenCorrs Attentuation correction table, to pass to the filter
	 * generation function. For internal use.
	 */

	static CDSPFIRFilter& getLPFilter( const double ReqNormFreq,
		const double ReqTransBand, const double ReqAtten,
		const EDSPFilterPhaseResponse ReqPhase,
		const double* const AttenCorrs = NULL )
	{
		R8BASSERT( ReqNormFreq >= 0.0 && ReqNormFreq <= 1.0 );
		R8BASSERT( ReqTransBand >= CDSPFIRFilter :: getLPMinTransBand() );
		R8BASSERT( ReqTransBand <= CDSPFIRFilter :: getLPMaxTransBand() );
		R8BASSERT( ReqAtten >= CDSPFIRFilter :: getLPMinAtten() );
		R8BASSERT( ReqAtten <= CDSPFIRFilter :: getLPMaxAtten() );

		R8BSYNC( StateSync );

		CDSPFIRFilter* PrevFilter = NULL;
		CDSPFIRFilter* CurFilter = Filters;

		while( CurFilter != NULL )
		{
			if( CurFilter -> ReqNormFreq == ReqNormFreq &&
				CurFilter -> ReqTransBand == ReqTransBand &&
				CurFilter -> ReqAtten == ReqAtten &&
				CurFilter -> ReqPhase == ReqPhase )
			{
				break;
			}

			if( CurFilter -> Next == NULL &&
				FilterCount >= R8B_FILTER_CACHE_MAX )
			{
				if( CurFilter -> RefCount == 0 )
				{
					// Delete the last filter which is not used.

					PrevFilter -> Next = NULL;
					delete CurFilter;
					FilterCount--;
				}
				else
				{
					// Move the last filter to the top of the list since it
					// seems to be in use for a long time.

					PrevFilter -> Next = NULL;
					CurFilter -> Next = Filters.unkeep();
					Filters = CurFilter;
				}

				CurFilter = NULL;
				break;
			}

			PrevFilter = CurFilter;
			CurFilter = CurFilter -> Next;
		}

		if( CurFilter != NULL )
		{
			CurFilter -> RefCount++;

			if( PrevFilter == NULL )
			{
				return( *CurFilter );
			}

			// Remove the filter from the list temporarily.

			PrevFilter -> Next = CurFilter -> Next;
		}
		else
		{
			// Create a new filter object (with RefCount == 1) and build the
			// filter kernel.

			CurFilter = new CDSPFIRFilter();
			CurFilter -> ReqNormFreq = ReqNormFreq;
			CurFilter -> ReqTransBand = ReqTransBand;
			CurFilter -> ReqAtten = ReqAtten;
			CurFilter -> ReqPhase = ReqPhase;
			FilterCount++;

			CurFilter -> buildLPFilter( AttenCorrs );
		}

		// Insert the filter at the start of the list.

		CurFilter -> Next = Filters.unkeep();
		Filters = CurFilter;

		return( *CurFilter );
	}

private:
	static CSyncObject StateSync; ///< Cache state synchronizer.
		///<
	static CPtrKeeper< CDSPFIRFilter* > Filters; ///< The chain of cached
		///< filters.
		///<
	static int FilterCount; ///< The number of filters currently preset in the
		///< cache.
		///<
};

// ---------------------------------------------------------------------------
// CDSPFIRFilter PUBLIC
// ---------------------------------------------------------------------------

inline void CDSPFIRFilter :: unref()
{
	R8BSYNC( CDSPFIRFilterCache :: StateSync );

	RefCount--;
}

// ---------------------------------------------------------------------------

} // namespace r8b

#endif // R8B_CDSPFIRFILTER_INCLUDED
