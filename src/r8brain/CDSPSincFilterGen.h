//$ nobt
//$ nocpp

/**
 * @file CDSPSincFilterGen.h
 *
 * @brief Sinc function-based FIR filter generator class.
 *
 * This file includes the CDSPSincFilterGen class implementation that
 * generates FIR filters.
 *
 * r8brain-free-src Copyright (c) 2013 Aleksey Vaneev
 * See the "License.txt" file for license.
 */

#ifndef R8B_CDSPSINCFILTERGEN_INCLUDED
#define R8B_CDSPSINCFILTERGEN_INCLUDED

#include "r8bbase.h"

namespace r8b {

/**
 * @brief Sinc function-based FIR filter generator class.
 *
 * Structure that holds state used to perform generation of sinc functions of
 * various types, windowed by the Blackman window by default (but the window
 * function can be changed if necessary).
 */

class CDSPSincFilterGen
{
public:
	double Len2; ///< Required half filter kernel's length in samples (can be
		///< a fractional value). Final physical kernel length will be
		///< provided in the KernelLen variable. Len2 should be >= 2.
		///<
	int KernelLen; ///< Resulting length of the filter kernel, this variable
		///< is set after the call to one of the "init" functions.
		///<
	int fl2; ///< Internal "half kernel length" value. This value can be used
		///< as filter's latency in samples (taps), this variable is set after
		///< the call to one of the "init" functions.
		///<

	union
	{
		struct
		{
			double Freq1; ///< Required corner circular frequency 1 [0; pi].
				///< Used only in the generateBand() function.
				///<
			double Freq2; ///< Required corner circular frequency 2 [0; pi].
				///< Used only in the generateBand() function. The range
				///< [Freq1; Freq2] defines a pass band for the generateBand()
				///< function.
				///<
		};

		struct
		{
			double FracDelay; ///< Fractional delay in the range [0; 1], used
				///< only in the generateFrac() function. Note that the
				///< FracDelay parameter is actually inversed. At 0.0 value it
				///< produces 1 sample delay (with the latency equal to fl2),
				///< at 1.0 value it produces 0 sample delay (with the latency
				///< equal to fl2 - 1).
				///<
		};
	};

	/**
	 * Window function type.
	 */

	enum EWindowFunctionType
	{
		wftCosine, ///< Generalized cosine window function. No parameters
			///< required. The "Power" parameter is optional.
			///<
		wftKaiser, ///< Kaiser window function. Requires the "Beta" parameter.
			///< The "Power" parameter is optional.
			///<
		wftGaussian, ///< Gaussian window function. Requires the "Sigma"
			///< parameter. The "Power" parameter is optional.
			///<
		wftVaneev ///< Vaneev window function, mainly used for short
			///< fractional delay filters, requires 4 cosine width parameters,
			///< plus the "Power" parameter which is mandatory.
			///< 
	};

	typedef double( CDSPSincFilterGen :: *CWindowFunc )(); ///< Window
		///< calculation function pointer type.
		///<

	/**
	 * Function initializes *this structure for generation of a window
	 * function, odd-sized.
	 *
	 * @param WinType Window function type.
	 * @param Params Window function's parameters. If NULL, the table values
	 * may be used.
	 * @param UsePower "True" if the power factor should be used to raise the
	 * window function. If "true", the power factor should be specified as the
	 * last value in the Params array. If Params is NULL, the table or default
	 * value of -1.0 (off) will be used.
	 */

	void initWindow( const EWindowFunctionType WinType = wftCosine,
		const double* const Params = NULL, const bool UsePower = false )
	{
		R8BASSERT( Len2 >= 2.0 );

		fl2 = (int) floor( Len2 );
		KernelLen = fl2 + fl2 + 1;

		setWindow( WinType, Params, UsePower, true );
	}

	/**
	 * Function initializes *this structure for generation of band-limited
	 * sinc filter kernel. The generateBand() or generateBandPow() functions
	 * should be used to calculate the filter.
	 *
	 * @param WinType Window function type.
	 * @param Params Window function's parameters. If NULL, the table values
	 * may be used.
	 * @param UsePower "True" if the power factor should be used to raise the
	 * window function. If "true", the power factor should be specified as the
	 * last value in the Params array. If Params is NULL, the table or default
	 * value of -1.0 (off) will be used.
	 */

	void initBand( const EWindowFunctionType WinType = wftCosine,
		const double* const Params = NULL, const bool UsePower = false )
	{
		R8BASSERT( Len2 >= 2.0 );

		fl2 = (int) floor( Len2 );
		KernelLen = fl2 + fl2 + 1;
		f1.init( Freq1, 2.0, 0.0 );
		f2.init( Freq2, 2.0, 0.0 );

		setWindow( WinType, Params, UsePower, true );
	}

	/**
	 * Function initializes *this structure for Hilbert transformation filter
	 * calculation. Freq1 and Freq2 variables are not used.
	 * The generateHilbert() function should be used to calculate the filter.
	 *
	 * @param WinType Window function type.
	 * @param Params Window function's parameters. If NULL, the table values
	 * may be used.
	 * @param UsePower "True" if the power factor should be used to raise the
	 * window function. If "true", the power factor should be specified as the
	 * last value in the Params array. If Params is NULL, the table or default
	 * value of -1.0 (off) will be used.
	 */

	void initHilbert( const EWindowFunctionType WinType = wftCosine,
		const double* const Params = NULL, const bool UsePower = false )
	{
		R8BASSERT( Len2 >= 2.0 );

		fl2 = (int) floor( Len2 );
		KernelLen = fl2 + fl2 + 1;

		setWindow( WinType, Params, UsePower, true );
	}

	/**
	 * Function initializes *this structure for generation of full-bandwidth
	 * fractional delay sinc filter kernel. Freq1 and Freq2 variables are not
	 * used. The generateFrac() function should be used to calculate the
	 * filter.
	 *
	 * @param WinType Window function type.
	 * @param Params Window function's parameters. If NULL, the table values
	 * may be used.
	 * @param UsePower "True" if the power factor should be used to raise the
	 * window function. If "true", the power factor should be specified as the
	 * last value in the Params array. If Params is NULL, the table or default
	 * value of -1.0 (off) will be used.
	 */

	void initFrac( const EWindowFunctionType WinType = wftCosine,
		const double* const Params = NULL, const bool UsePower = false )
	{
		R8BASSERT( Len2 >= 2.0 );

		fl2 = (int) ceil( Len2 );
		KernelLen = fl2 + fl2;

		setWindow( WinType, Params, UsePower, false, FracDelay );
	}

	/**
	 * @return The next "Hann" window function coefficient.
	 */

	double calcWindowHann()
	{
		return( 0.5 + 0.5 * w1.gen() );
	}

	/**
	 * @return The next "Hamming" window function coefficient.
	 */

	double calcWindowHamming()
	{
		return( 0.54 + 0.46 * w1.gen() );
	}

	/**
	 * @return The next "Blackman" window function coefficient.
	 */

	double calcWindowBlackman()
	{
		return( 0.42 + 0.5 * w1.gen() + 0.08 * w2.gen() );
	}

	/**
	 * @return The next "Nuttall" window function coefficient.
	 */

	double calcWindowNuttall()
	{
		return( 0.355768 + 0.487396 * w1.gen() + 0.144232 * w2.gen() +
			0.012604 * w3.gen() );
	}

	/**
	 * @return The next "Blackman-Nuttall" window function coefficient.
	 */

	double calcWindowBlackmanNuttall()
	{
		return( 0.3635819 + 0.4891775 * w1.gen() + 0.1365995 * w2.gen() +
			0.0106411 * w3.gen() );
	}

	/**
	 * @return The next "Kaiser" window function coefficient.
	 */

	double calcWindowKaiser()
	{
		const double n = 1.0 - sqr( wn / Len2 + KaiserLen2Frac );
		wn++;

		if( n < 0.0 )
		{
			return( 0.0 );
		}

		return( besselI0( KaiserBeta * sqrt( n )) / KaiserDiv );
	}

	/**
	 * @return The next "Gaussian" window function coefficient.
	 */

	double calcWindowGaussian()
	{
		const double f = exp( -0.5 * sqr( wn / GaussianSigma +
			GaussianSigmaFrac ));

		wn++;

		return( f );
	}

	/**
	 * @return The next "Vaneev" windowing function coefficient, for use with
	 * the fractional delay filters.
	 */

	double calcWindowVaneev()
	{
		const double v1 = 0.5 + 0.5 * w1.gen();
		const double v2 = 0.5 + 0.5 * w2.gen();
		const double v3 = 0.5 + 0.5 * w3.gen();
		const double v4 = 0.5 + 0.5 * w4.gen();

		return( v1 * sqr( v2 ) * sqr( sqr( v3 )) * sqr( sqr( sqr( v4 ))));
	}

	/**
	 * Function calculates window function only.
	 *
	 * @param[out] op Output buffer, length = KernelLen.
	 * @param wfunc Window calculation function to use.
	 */

	template< class T >
	void generateWindow( T* op,
		CWindowFunc wfunc = &CDSPSincFilterGen :: calcWindowBlackman )
	{
		op += fl2;
		T* op2 = op;

		int l = fl2;

		if( Power < 0.0 )
		{
			*op = ( *this.*wfunc )();

			while( l > 0 )
			{
				const double v = ( *this.*wfunc )();

				op++;
				op2--;
				*op = v;
				*op2 = v;
				l--;
			}
		}
		else
		{
			*op = pows(( *this.*wfunc )(), Power );

			while( l > 0 )
			{
				const double v = pows(( *this.*wfunc )(), Power );

				op++;
				op2--;
				*op = v;
				*op2 = v;
				l--;
			}
		}
	}

	/**
	 * Function calculates band-limited windowed sinc function-based filter
	 * kernel.
	 *
	 * @param[out] op Output buffer, length = KernelLen.
	 * @param wfunc Window calculation function to use.
	 */

	template< class T >
	void generateBand( T* op,
		CWindowFunc wfunc = &CDSPSincFilterGen :: calcWindowBlackman )
	{
		op += fl2;
		T* op2 = op;
		f1.gen();
		f2.gen();
		int t = 1;

		if( Power < 0.0 )
		{
			*op = ( Freq2 - Freq1 ) * ( *this.*wfunc )() / M_PI;

			while( t <= fl2 )
			{
				const double v = ( f2.gen() - f1.gen() ) *
					( *this.*wfunc )() / t / M_PI;

				op++;
				op2--;
				*op = v;
				*op2 = v;
				t++;
			}
		}
		else
		{
			*op = ( Freq2 - Freq1 ) * pows(( *this.*wfunc )(), Power ) / M_PI;

			while( t <= fl2 )
			{
				const double v = ( f2.gen() - f1.gen() ) *
					pows(( *this.*wfunc )(), Power ) / t / M_PI;

				op++;
				op2--;
				*op = v;
				*op2 = v;
				t++;
			}
		}
	}

	/**
	 * Function calculates windowed Hilbert transformer filter kernel.
	 *
	 * @param[out] op Output buffer, length = KernelLen.
	 * @param wfunc Window calculation function to use.
	 */

	template< class T >
	void generateHilbert( T* op,
		CWindowFunc wfunc = &CDSPSincFilterGen :: calcWindowBlackman )
	{
		static const double fvalues[ 2 ] = { 0.0, 2.0 };
		op += fl2;
		T* op2 = op;

		( *this.*wfunc )();
		*op = 0.0;

		int t = 1;

		if( Power < 0.0 )
		{
			while( t <= fl2 )
			{
				const double v = fvalues[ t & 1 ] *
					( *this.*wfunc )() / t / M_PI;

				op++;
				op2--;
				*op = v;
				*op2 = -v;
				t++;
			}
		}
		else
		{
			while( t <= fl2 )
			{
				const double v = fvalues[ t & 1 ] *
					pows( ( *this.*wfunc )(), Power ) / t / M_PI;

				op++;
				op2--;
				*op = v;
				*op2 = -v;
				t++;
			}
		}
	}

	/**
	 * Function calculates windowed fractional delay filter kernel.
	 *
	 * @param[out] op Output buffer, length = KernelLen.
	 * @param wfunc Window calculation function to use.
	 * @param opinc Output buffer increment, in "op" elements.
	 */

	template< class T >
	void generateFrac( T* op,
		CWindowFunc wfunc = &CDSPSincFilterGen :: calcWindowBlackman,
		const int opinc = 1 )
	{
		R8BASSERT( opinc != 0 );

		double f[ 2 ];
		f[ 0 ] = sin( FracDelay * M_PI );
		f[ 1 ] = -f[ 0 ];

		int t = -fl2;

		if( t + FracDelay < -Len2 )
		{
			( *this.*wfunc )();
			*op = 0.0;
			op += opinc;
			t++;
		}

		int mt = ( FracDelay >= 1.0 - 1e-13 && FracDelay <= 1.0 + 1e-13 ?
			-1 : 0 );

		if( Power < 0.0 )
		{
			while( t < mt )
			{
				*op = f[ t & 1 ] * ( *this.*wfunc )() / ( t + FracDelay ) /
					M_PI;

				op += opinc;
				t++;
			}

			double ut = t + FracDelay;
			*op = ( fabs( ut ) <= 1e-13 ? ( *this.*wfunc )() :
				f[ t & 1 ] * ( *this.*wfunc )() / ut / M_PI );

			mt = fl2 - 2;

			while( t < mt )
			{
				op += opinc;
				t++;
				*op = f[ t & 1 ] * ( *this.*wfunc )() / ( t + FracDelay ) /
					M_PI;
			}

			op += opinc;
			t++;
			ut = t + FracDelay;
			*op = ( ut > Len2 ? 0.0 :
				f[ t & 1 ] * ( *this.*wfunc )() / ut / M_PI );
		}
		else
		{
			while( t < mt )
			{
				*op = f[ t & 1 ] * pows( ( *this.*wfunc )(), Power ) /
					( t + FracDelay ) / M_PI;

				op += opinc;
				t++;
			}

			double ut = t + FracDelay;
			*op = ( fabs( ut ) <= 1e-13 ? pows( ( *this.*wfunc )(), Power ) :
				f[ t & 1 ] * pows( ( *this.*wfunc )(), Power ) / ut / M_PI );

			mt = fl2 - 2;

			while( t < mt )
			{
				op += opinc;
				t++;
				*op = f[ t & 1 ] * pows( ( *this.*wfunc )(), Power ) /
					( t + FracDelay ) / M_PI;
			}

			op += opinc;
			t++;
			ut = t + FracDelay;
			*op = ( ut > Len2 ? 0.0 :
				f[ t & 1 ] * pows( ( *this.*wfunc )(), Power ) / ut / M_PI );
		}
	}

private:
	double Power; ///< The power factor used to raise the window function.
		///< Equals a negative value if the power factor should not be used.
		///<
	CSineGen f1; ///< Sine function 1. Used in the generateBand() function.
		///<
	CSineGen f2; ///< Sine function 2. Used in the generateBand() function.
		///<
	int wn; ///< Window function integer position. 0 - center of the window
		///< function. This variable may not be used by some window functions.
		///<

	union
	{
		struct
		{
			CSineGen w1; ///< Cosine wave 1 for window function.
				///<
			CSineGen w2; ///< Cosine wave 2 for window function.
				///<
			CSineGen w3; ///< Cosine wave 3 for window function.
				///<
			CSineGen w4; ///< Cosine wave 4 for window function.
				///<
		};

		struct
		{
			double KaiserBeta; ///< Kaiser window function's "Beta"
				///< coefficient.
				///<
			double KaiserDiv; ///< Kaiser window function's divisor.
				///<
			double KaiserLen2Frac; ///< Equals FracDelay / Len2.
				///<
		};

		struct
		{
			double GaussianSigma; ///< Gaussian window function's "Sigma"
				///< coefficient.
				///<
			double GaussianSigmaFrac; ///< Equals FracDelay / GaussianSigma.
				///<
		};
	};

	/**
	 * @param FilterLen2 Half filter length in samples (taps).
	 * @return The Kaiser power-raised window function parameters for the
	 * specified filter length.
	 */

	static const double* getKaiserParams( const int FilterLen2 )
	{
		R8BASSERT( FilterLen2 >= 3 && FilterLen2 <= 15 );

		static const double Coeffs[][ 2 ] = {
			{ 3.41547411, 1.41275111 }, // 6 @ 51.38
			{ 3.72300147, 1.75212634 }, // 8 @ 67.60
			{ 4.34839223, 1.85801372 }, // 10 @ 79.86
			{ 4.90860405, 1.97194591 }, // 12 @ 93.29
			{ 5.17430411, 2.20609617 }, // 14 @ 106.74
			{ 21.08445389, 0.59684098 }, // 16 @ 119.71
			{ 9.14552738, 1.57619894 }, // 18 @ 134.53
			{ 22.02344341, 0.71669064 }, // 20 @ 148.44
			{ 16.41763757, 1.05884118 }, // 22 @ 164.61
			{ 12.55262798, 1.51553897 }, // 24 @ 180.15
			{ 9.84861210, 2.09912671 }, // 26 @ 194.15
			{ 9.73150659, 2.29079494 }, // 28 @ 206.91
			{ 10.42657217, 2.29183875 }, // 30 @ 218.20
		};

		return( Coeffs[ FilterLen2 - 3 ]);
	}

	/**
	 * Function initializes Kaiser window function calculation. The FracDelay
	 * variable should be initialized when using this window function.
	 *
	 * @param Params Function parameters. If NULL, the table values will be
	 * used. If not NULL, the first parameter should specify the "Beta" value.
	 * @param UsePower "True" if the power factor should be used to raise the
	 * window function.
	 * @param IsCentered "True" if centered window should be used. This
	 * parameter usually equals to "false" for fractional delay filters only.
	 */

	void setWindowKaiser( const double* Params, const bool UsePower,
		const bool IsCentered )
	{
		wn = ( IsCentered ? 0 : -fl2 );

		if( Params == NULL )
		{
			Params = getKaiserParams( fl2 );
			KaiserBeta = Params[ 0 ];
			Power = ( UsePower ? Params[ 1 ] : -1.0 );
		}
		else
		{
			KaiserBeta = clampr( Params[ 0 ], 1.0, 350.0 );
			Power = ( UsePower ? fabs( Params[ 1 ]) : -1.0 );
		}

		KaiserDiv = besselI0( KaiserBeta );
		KaiserLen2Frac = FracDelay / Len2;
	}

	/**
	 * Function initializes Gaussian window function calculation. The FracDelay
	 * variable should be initialized when using this window function.
	 *
	 * @param Params Function parameters. If NULL, the table values will be
	 * used. If not NULL, the first parameter should specify the "Sigma"
	 * value.
	 * @param UsePower "True" if the power factor should be used to raise the
	 * window function.
	 * @param IsCentered "True" if centered window should be used. This
	 * parameter usually equals to "false" for fractional delay filters only.
	 */

	void setWindowGaussian( const double* Params, const bool UsePower,
		const bool IsCentered )
	{
		wn = ( IsCentered ? 0 : -fl2 );

		if( Params == NULL )
		{
			GaussianSigma = 1.0;
			Power = -1.0;
		}
		else
		{
			GaussianSigma = clampr( fabs( Params[ 0 ]), 1e-1, 100.0 );
			Power = ( UsePower ? fabs( Params[ 1 ]) : -1.0 );
		}

		GaussianSigma *= Len2;
		GaussianSigmaFrac = FracDelay / GaussianSigma;
	}

	/**
	 * Function initializes "Vaneev" window function calculation.
	 *
	 * @param Params Function parameters. If NULL, the table values will be
	 * used. If not NULL, the first 4 parameters should specify the cosine
	 * multipliers while the fifth parameter should specify the "Power" value.
	 * @param IsCentered "True" if centered window should be used. This
	 * parameter usually equals to "false" for fractional delay filters only.
	 */

	void setWindowVaneev( const double* Params, const bool IsCentered )
	{
		R8BASSERT( fl2 >= 3 && fl2 <= 15 );

		// This set of parameters was obtained via probabilistic optimization.
		// The number after @ shows the approximate (+/- 1 dB) signal-to-noise
		// ratio for the given filter. SNR can be also decreased by using a
		// filter bank with suboptimal number of sampled fractional delay
		// filters: thus the FilterFracs should be selected with care.

		static const double Coeffs[][ 5 ] = {
			{ 0.35926104, 0.66154037, 0.79264845, 0.31897879, 0.18844972 }, // 6 @ 51.91
			{ 0.81690764, 0.39409966, 0.01546567, 0.02067949, 1.15143000 }, // 8 @ 67.87
			{ 0.26545140, 0.84346586, 0.12114879, 0.23640230, 0.72659219 }, // 10 @ 81.82
			{ 0.56254211, 0.32615646, 0.88375690, 0.46944169, 0.32862728 }, // 12 @ 95.36
			{ 0.51926261, 0.41265523, 0.89552919, 0.47699008, 0.37308306 }, // 14 @ 109.60
			{ 0.55650321, 0.92583533, 0.58934379, 0.16399064, 0.67129777 }, // 16 @ 122.89
			{ 0.27930548, 0.94898807, 0.70335882, 0.32080180, 0.59102482 }, // 18 @ 136.45
			{ 0.12620836, 0.94993219, 0.70209891, 0.34747431, 0.64429174 }, // 20 @ 150.52
			{ 0.83595860, 0.95040751, 0.64127591, 0.30856013, 0.69692727 }, // 22 @ 163.41
			{ 0.41252871, 0.96236749, 0.74895429, 0.41669175, 0.65996102 }, // 24 @ 174.32
			{ 0.98567539, 0.88907131, 0.65652775, 0.34585902, 0.77265757 }, // 26 @ 191.26
			{ 0.64526843, 0.67729329, 0.91813705, 0.43972488, 0.68332682 }, // 28 @ 195.77
			{ 0.65310281, 0.66723395, 0.91751074, 0.43956737, 0.73651421 }, // 30 @ 207.04
		};

		double p[ 4 ];

		if( Params == NULL )
		{
			Params = Coeffs[ fl2 - 3 ];
			Power = Params[ 4 ];
		}
		else
		{
			p[ 0 ] = clampr( Params[ 0 ], -4.0, 4.0 );
			p[ 1 ] = clampr( Params[ 1 ], -4.0, 4.0 );
			p[ 2 ] = clampr( Params[ 2 ], -4.0, 4.0 );
			p[ 3 ] = clampr( Params[ 3 ], -4.0, 4.0 );
			Power = fabs( Params[ 4 ]);
			Params = p;
		}

		if( IsCentered )
		{
			w1.init( Params[ 0 ] * M_PI / Len2, 2.0, M_PI * 0.5 );
			w2.init( Params[ 1 ] * M_PI / Len2, 2.0, M_PI * 0.5 );
			w3.init( Params[ 2 ] * M_PI / Len2, 2.0, M_PI * 0.5 );
			w4.init( Params[ 3 ] * M_PI / Len2, 2.0, M_PI * 0.5 );
		}
		else
		{
			const double step1 = Params[ 0 ] * M_PI / Len2;
			w1.init( step1, 2.0, M_PI * 0.5 - step1 * fl2 +
				step1 * FracDelay );

			const double step2 = Params[ 1 ] * M_PI / Len2;
			w2.init( step2, 2.0, M_PI * 0.5 - step2 * fl2 +
				step2 * FracDelay );

			const double step3 = Params[ 2 ] * M_PI / Len2;
			w3.init( step3, 2.0, M_PI * 0.5 - step3 * fl2 +
				step3 * FracDelay );

			const double step4 = Params[ 3 ] * M_PI / Len2;
			w4.init( step4, 2.0, M_PI * 0.5 - step4 * fl2 +
				step4 * FracDelay );
		}
	}

	/**
	 * Function initializes calculation of window function of the specified
	 * type.
	 *
	 * @param WinType Window function type.
	 * @param Params Window function's parameters. If NULL, the table values
	 * may be used.
	 * @param UsePower "True" if the power factor should be used to raise the
	 * window function. If "true", the power factor should be specified as the
	 * last value in the Params array. If Params is NULL, the table or default
	 * value of -1.0 (off) will be used.
	 * @param IsCentered "True" if centered window should be used. This
	 * parameter usually equals to "false" for fractional delay filters only.
	 * @param UseFracDelay Fractional delay to use.
	 */

	void setWindow( const EWindowFunctionType WinType,
		const double* const Params, const bool UsePower,
		const bool IsCentered, const double UseFracDelay = 0.0 )
	{
		FracDelay = UseFracDelay;

		if( WinType == wftCosine )
		{
			if( IsCentered )
			{
				w1.init( M_PI / Len2, 2.0, M_PI * 0.5 );
				w2.init( M_2PI / Len2, 2.0, M_PI * 0.5 );
				w3.init( M_3PI / Len2, 2.0, M_PI * 0.5 );
			}
			else
			{
				const double step1 = M_PI / Len2;
				w1.init( step1, 2.0, M_PI * 0.5 - step1 * fl2 +
					step1 * FracDelay );

				const double step2 = M_2PI / Len2;
				w2.init( step2, 2.0, M_PI * 0.5 - step2 * fl2 +
					step2 * FracDelay );

				const double step3 = M_3PI / Len2;
				w3.init( step3, 2.0, M_PI * 0.5 - step3 * fl2 +
					step3 * FracDelay );
			}

			Power = ( UsePower && Params != NULL ? Params[ 0 ] : -1.0 );
		}
		else
		if( WinType == wftKaiser )
		{
			setWindowKaiser( Params, UsePower, IsCentered );
		}
		else
		if( WinType == wftGaussian )
		{
			setWindowGaussian( Params, UsePower, IsCentered );
		}
		else
		{
			setWindowVaneev( Params, IsCentered );
		}
	}
};

} // namespace r8b

#endif // R8B_CDSPSINCFILTERGEN_INCLUDED
