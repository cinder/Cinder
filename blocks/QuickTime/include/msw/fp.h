/*
     File:       fp.h
 
     Contains:   FPCE Floating-Point Definitions and Declarations.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1987-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __FP__
#define __FP__

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

/********************************************************************************
*                                                                               *
*    A collection of numerical functions designed to facilitate a wide          *
*    range of numerical programming as required by C9X.                         *
*                                                                               *
*    The <fp.h> declares many functions in support of numerical programming.    *
*    It provides a superset of <math.h> and <SANE.h> functions.  Some           *
*    functionality previously found in <SANE.h> and not in the FPCE <fp.h>      *
*    can be found in this <fp.h> under the heading "__NOEXTENSIONS__".          *
*                                                                               *
*    All of these functions are IEEE 754 aware and treat exceptions, NaNs,      *
*    positive and negative zero and infinity consistent with the floating-      *
*    point standard.                                                            *
*                                                                               *
********************************************************************************/



#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif

/********************************************************************************
*                                                                               *
*                            Efficient types                                    *
*                                                                               *
*    float_t         Most efficient type at least as wide as float              *
*    double_t        Most efficient type at least as wide as double             *
*                                                                               *
*      CPU            float_t(bits)                double_t(bits)               *
*    --------        -----------------            -----------------             *
*    PowerPC          float(32)                    double(64)                   *
*    68K              long double(80/96)           long double(80/96)           *
*    x86              double(64)                   double(64)                   *
*                                                                               *
********************************************************************************/
#if (defined(__MWERKS__) && defined(__cmath__)) || (TARGET_RT_MAC_MACHO && defined(__MATH__))
/* these types were already defined in math.h */
#else
#if TARGET_CPU_PPC
typedef float                           float_t;
typedef double                          double_t;
#elif TARGET_CPU_68K
typedef long double                     float_t;
typedef long double                     double_t;
#elif TARGET_CPU_X86
typedef double                          float_t;
typedef double                          double_t;
#elif TARGET_CPU_MIPS
typedef double                          float_t;
typedef double                          double_t;
#elif TARGET_CPU_ALPHA
typedef double                          float_t;
typedef double                          double_t;
#elif TARGET_CPU_SPARC
typedef double                          float_t;
typedef double                          double_t;
#else
#error unsupported CPU
#endif  /*  */

/********************************************************************************
*                                                                               *
*                              Define some constants.                           *
*                                                                               *
*    HUGE_VAL            IEEE 754 value of infinity.                            *
*    INFINITY            IEEE 754 value of infinity.                            *
*    NAN                 A generic NaN (Not A Number).                          *
*    DECIMAL_DIG         Satisfies the constraint that the conversion from      *
*                        double to decimal and back is the identity function.   *
*                                                                               *
********************************************************************************/
#if TARGET_OS_MAC
  #if !TARGET_RT_MAC_MACHO
    #define   HUGE_VAL                __inf()
    #define   INFINITY                __inf()
    #define   NAN                     nan("255")
  #endif
#else
  #define     NAN                     sqrt(-1)
#endif

#if TARGET_CPU_PPC
  #define      DECIMAL_DIG              17 /* does not exist for double-double */
#elif TARGET_CPU_68K
  #define      DECIMAL_DIG              21
#endif      
#endif  /* (defined(__MWERKS__) && defined(__cmath__)) || (TARGET_RT_MAC_MACHO && defined(__MATH__)) */
#if TARGET_OS_MAC
/* MSL or math.h already defines these */
#if (!defined(__MWERKS__) || !defined(__cmath__)) && (!TARGET_RT_MAC_MACHO || !defined(__MATH__))
/********************************************************************************
*                                                                               *
*                            Trigonometric functions                            *
*                                                                               *
*   acos        result is in [0,pi].                                            *
*   asin        result is in [-pi/2,pi/2].                                      *
*   atan        result is in [-pi/2,pi/2].                                      *
*   atan2       Computes the arc tangent of y/x in [-pi,pi] using the sign of   *
*               both arguments to determine the quadrant of the computed value. *
*                                                                               *
********************************************************************************/
/*
 *  cos()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) cos(double_t x);


/*
 *  sin()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) sin(double_t x);


/*
 *  tan()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) tan(double_t x);


/*
 *  acos()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) acos(double_t x);


/*
 *  asin()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) asin(double_t x);


/*
 *  atan()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) atan(double_t x);


/*
 *  atan2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) atan2(double_t y, double_t x);




/********************************************************************************
*                                                                               *
*                              Hyperbolic functions                             *
*                                                                               *
********************************************************************************/
/*
 *  cosh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) cosh(double_t x);


/*
 *  sinh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) sinh(double_t x);


/*
 *  tanh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) tanh(double_t x);


/*
 *  acosh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) acosh(double_t x);


/*
 *  asinh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) asinh(double_t x);


/*
 *  atanh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) atanh(double_t x);




/********************************************************************************
*                                                                               *
*                              Exponential functions                            *
*                                                                               *
*   expm1       expm1(x) = exp(x) - 1.  But, for small enough arguments,        *
*               expm1(x) is expected to be more accurate than exp(x) - 1.       *
*   frexp       Breaks a floating-point number into a normalized fraction       *
*               and an integral power of 2.  It stores the integer in the       *
*               object pointed by *exponent.                                    *
*   ldexp       Multiplies a floating-point number by an integer power of 2.    *
*   log1p       log1p = log(1 + x). But, for small enough arguments,            *
*               log1p is expected to be more accurate than log(1 + x).          *
*   logb        Extracts the exponent of its argument, as a signed integral     *
*               value. A subnormal argument is treated as though it were first  *
*               normalized. Thus:                                               *
*                                  1   <=   x * 2^(-logb(x))   <   2            *
*   modf        Returns fractional part of x as function result and returns     *
*               integral part of x via iptr. Note C9X uses double not double_t. *
*   scalb       Computes x * 2^n efficently.  This is not normally done by      *
*               computing 2^n explicitly.                                       *
*                                                                               *
********************************************************************************/
/*
 *  exp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) exp(double_t x);


/*
 *  expm1()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) expm1(double_t x);


/*
 *  exp2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) exp2(double_t x);


/*
 *  frexp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) frexp(double_t x, int *exponent);


/*
 *  ldexp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) ldexp(double_t x, int n);


/*
 *  log()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) log(double_t x);


/*
 *  log2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) log2(double_t x);


/*
 *  log1p()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) log1p(double_t x);


/*
 *  log10()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) log10(double_t x);


/*
 *  logb()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) logb(double_t x);


#if !TYPE_LONGDOUBLE_IS_DOUBLE
/*
 *  modfl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) modfl(long double x, long double *iptrl);


#endif  /* !TYPE_LONGDOUBLE_IS_DOUBLE */

/*
 *  modf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) modf(double_t x, double_t *iptr);


/*
 *  modff()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( float ) modff(float x, float *iptrf);



/*
    Note: For compatiblity scalb(x,n) has n of type
            int  on Mac OS X 
            long on Mac OS
*/
typedef long                            _scalb_n_type;
/*
 *  scalb()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) scalb(double_t x, _scalb_n_type n);




/********************************************************************************
*                                                                               *
*                     Power and absolute value functions                        *
*                                                                               *
*   hypot       Computes the square root of the sum of the squares of its       *
*               arguments, without undue overflow or underflow.                 *
*   pow         Returns x raised to the power of y.  Result is more accurate    *
*               than using exp(log(x)*y).                                       *
*                                                                               *
********************************************************************************/
/*
 *  fabs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) fabs(double_t x);


/*
 *  hypot()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) hypot(double_t x, double_t y);


/*
 *  pow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) pow(double_t x, double_t y);


/*
 *  sqrt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) sqrt(double_t x);




/********************************************************************************
*                                                                               *
*                        Gamma and Error functions                              *
*                                                                               *
*   erf         The error function.                                             *
*   erfc        Complementary error function.                                   *
*   gamma       The gamma function.                                             *
*   lgamma      Computes the base-e logarithm of the absolute value of          *
*               gamma of its argument x, for x > 0.                             *
*                                                                               *
********************************************************************************/
/*
 *  erf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) erf(double_t x);


/*
 *  erfc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) erfc(double_t x);


/*
 *  gamma()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) gamma(double_t x);


/*
 *  lgamma()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) lgamma(double_t x);




/********************************************************************************
*                                                                               *
*                        Nearest integer functions                              *
*                                                                               *
*   rint        Rounds its argument to an integral value in floating point      *
*               format, honoring the current rounding direction.                *
*                                                                               *
*   nearbyint   Differs from rint only in that it does not raise the inexact    *
*               exception. It is the nearbyint function recommended by the      *
*               IEEE floating-point standard 854.                               *
*                                                                               *
*   rinttol     Rounds its argument to the nearest long int using the current   *
*               rounding direction.  NOTE: if the rounded value is outside      *
*               the range of long int, then the result is undefined.            *
*                                                                               *
*   round       Rounds the argument to the nearest integral value in floating   *
*               point format similar to the Fortran "anint" function. That is:  *
*               add half to the magnitude and chop.                             *
*                                                                               *
*   roundtol    Similar to the Fortran function nint or to the Pascal round.    *
*               NOTE: if the rounded value is outside the range of long int,    *
*               then the result is undefined.                                   *
*                                                                               *
*   trunc       Computes the integral value, in floating format, nearest to     *
*               but no larger in magnitude than its argument.   NOTE: on 68K    *
*               compilers when using -elems881, trunc must return an int        *
*                                                                               *
********************************************************************************/
/*
 *  ceil()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) ceil(double_t x);


/*
 *  floor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) floor(double_t x);


/*
 *  rint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) rint(double_t x);


/*
 *  nearbyint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) nearbyint(double_t x);


/*
 *  rinttol()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) rinttol(double_t x);


/*
 *  round()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) round(double_t x);


/*
 *  roundtol()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) roundtol(double_t round);


/*
    Note: For compatiblity trunc(x) has a return type of
            int       for classic 68K with FPU enabled
            double_t  everywhere else
*/
#if TARGET_RT_MAC_68881
typedef int                             _trunc_return_type;
#else
typedef double_t                        _trunc_return_type;
#endif  /* TARGET_RT_MAC_68881 */

/*
 *  trunc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( _trunc_return_type ) trunc(double_t x);





/********************************************************************************
*                                                                               *
*                            Remainder functions                                *
*                                                                               *
*   remainder       IEEE 754 floating point standard for remainder.             *
*   remquo          SANE remainder.  It stores into 'quotient' the 7 low-order  *
*                   bits of the integer quotient x/y, such that:                *
*                       -127 <= quotient <= 127.                                *
*                                                                               *
********************************************************************************/
/*
 *  fmod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) fmod(double_t x, double_t y);


/*
 *  remainder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) remainder(double_t x, double_t y);


/*
 *  remquo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) remquo(double_t x, double_t y, int *quo);




/********************************************************************************
*                                                                               *
*                             Auxiliary functions                               *
*                                                                               *
*   copysign        Produces a value with the magnitude of its first argument   *
*                   and sign of its second argument.  NOTE: the order of the    *
*                   arguments matches the recommendation of the IEEE 754        *
*                   floating point standard,  which is opposite from the SANE   *
*                   copysign function.                                          *
*                                                                               *
*   nan             The call 'nan("n-char-sequence")' returns a quiet NaN       *
*                   with content indicated through tagp in the selected         *
*                   data type format.                                           *
*                                                                               *
*   nextafter       Computes the next representable value after 'x' in the      *
*                   direction of 'y'.  if x == y, then y is returned.           *
*                                                                               *
********************************************************************************/
/*
 *  copysign()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) copysign(double_t x, double_t y);


/*
 *  nan()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double ) nan(const char * tagp);


/*
 *  nanf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( float ) nanf(const char * tagp);



/*
 *  nanl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) nanl(const char * tagp);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) nanl(const char *tagp) { return (long double) nan(tagp); }
  #else
    #define nanl(tagp) ((long double) nan(tagp))
  #endif
#endif


/*
 *  nextafterd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double ) nextafterd(double x, double y);


/*
 *  nextafterf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( float ) nextafterf(float x, float y);



/*
 *  nextafterl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) nextafterl(long double x, long double y);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) nextafterl(long double x, long double y) { return (long double) nextafterd((double)(x),(double)(y)); }
  #else
    #define nextafterl(x, y) ((long double) nextafterd((double)(x),(double)(y)))
  #endif
#endif




/*
 *  __fpclassifyd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) __fpclassifyd(double x);


/*
 *  __fpclassifyf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) __fpclassifyf(float x);



/*
 *  __fpclassify()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long ) __fpclassify(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long ) __fpclassify(long double x) { return __fpclassifyd((double)(x)); }
  #else
    #define __fpclassify(x) (__fpclassifyd((double)(x)))
  #endif
#endif


/*
 *  __isnormald()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) __isnormald(double x);


/*
 *  __isnormalf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) __isnormalf(float x);



/*
 *  __isnormal()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long ) __isnormal(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long ) __isnormal(long double x) { return __isnormald((double)(x)); }
  #else
    #define __isnormal(x) (__isnormald((double)(x)))
  #endif
#endif



/*
 *  __isfinited()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) __isfinited(double x);


/*
 *  __isfinitef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) __isfinitef(float x);



/*
 *  __isfinite()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long ) __isfinite(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long ) __isfinite(long double x) { return __isfinited((double)(x)); }
  #else
    #define __isfinite(x) (__isfinited((double)(x)))
  #endif
#endif


/*
 *  __isnand()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) __isnand(double x);


/*
 *  __isnanf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) __isnanf(float x);



/*
 *  __isnan()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long ) __isnan(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long ) __isnan(long double x) { return __isnand((double)(x)); }
  #else
    #define __isnan(x) (__isnand((double)(x)))
  #endif
#endif



/*
 *  __signbitd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) __signbitd(double x);


/*
 *  __signbitf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) __signbitf(float x);



/*
 *  __signbit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long ) __signbit(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long ) __signbit(long double x) { return __signbitd((double)(x)); }
  #else
    #define __signbit(x) (__signbitd((double)(x)))
  #endif
#endif


/*
 *  __inf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) __inf(void);




/********************************************************************************
*                                                                               *
*                              Inquiry macros                                   *
*                                                                               *
*   fpclassify      Returns one of the FP_* values.                             *
*   isnormal        Non-zero if and only if the argument x is normalized.       *
*   isfinite        Non-zero if and only if the argument x is finite.           *
*   isnan           Non-zero if and only if the argument x is a NaN.            *
*   signbit         Non-zero if and only if the sign of the argument x is       *
*                   negative.  This includes, NaNs, infinities and zeros.       *
*                                                                               *
********************************************************************************/
enum {
  FP_SNAN                       = 0,    /*      signaling NaN                         */
  FP_QNAN                       = 1,    /*      quiet NaN                             */
  FP_INFINITE                   = 2,    /*      + or - infinity                       */
  FP_ZERO                       = 3,    /*      + or - zero                           */
  FP_NORMAL                     = 4,    /*      all normal numbers                    */
  FP_SUBNORMAL                  = 5     /*      denormal numbers                      */
};

#define      fpclassify(x)    ( ( sizeof ( x ) == sizeof(double) ) ?           \
                              __fpclassifyd  ( x ) :                           \
                                ( sizeof ( x ) == sizeof(float) ) ?            \
                              __fpclassifyf ( x ) :                            \
                              __fpclassify  ( x ) )
#define      isnormal(x)      ( ( sizeof ( x ) == sizeof(double) ) ?           \
                              __isnormald ( x ) :                              \
                                ( sizeof ( x ) == sizeof(float) ) ?            \
                              __isnormalf ( x ) :                              \
                              __isnormal  ( x ) )
#define      isfinite(x)      ( ( sizeof ( x ) == sizeof(double) ) ?           \
                              __isfinited ( x ) :                              \
                                ( sizeof ( x ) == sizeof(float) ) ?            \
                              __isfinitef ( x ) :                              \
                              __isfinite  ( x ) )
#define      isnan(x)         ( ( sizeof ( x ) == sizeof(double) ) ?           \
                              __isnand ( x ) :                                 \
                                ( sizeof ( x ) == sizeof(float) ) ?            \
                              __isnanf ( x ) :                                 \
                              __isnan  ( x ) )
#define      signbit(x)       ( ( sizeof ( x ) == sizeof(double) ) ?           \
                              __signbitd ( x ) :                               \
                                ( sizeof ( x ) == sizeof(float) ) ?            \
                              __signbitf ( x ) :                               \
                              __signbit  ( x ) )



/********************************************************************************
*                                                                               *
*                      Max, Min and Positive Difference                         *
*                                                                               *
*   fdim        Determines the 'positive difference' between its arguments:     *
*               { x - y, if x > y }, { +0, if x <= y }.  If one argument is     *
*               NaN, then fdim returns that NaN.  if both arguments are NaNs,   *
*               then fdim returns the first argument.                           *
*                                                                               *
*   fmax        Returns the maximum of the two arguments.  Corresponds to the   *
*               max function in FORTRAN.  NaN arguments are treated as missing  *
*               data.  If one argument is NaN and the other is a number, then   *
*               the number is returned.  If both are NaNs then the first        *
*               argument is returned.                                           *
*                                                                               *
*   fmin        Returns the minimum of the two arguments.  Corresponds to the   *
*               min function in FORTRAN.  NaN arguments are treated as missing  *
*               data.  If one argument is NaN and the other is a number, then   *
*               the number is returned.  If both are NaNs then the first        *
*               argument is returned.                                           *
*                                                                               *
********************************************************************************/
/*
 *  fdim()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) fdim(double_t x, double_t y);


/*
 *  fmax()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) fmax(double_t x, double_t y);


/*
 *  fmin()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) fmin(double_t x, double_t y);


#endif /* (defined(__MWERKS__) && defined(__cmath__)) || (TARGET_RT_MAC_MACHO && defined(__MATH__)) */

/*******************************************************************************
*                                Constants                                     *
*******************************************************************************/
/*
 *  pi
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
extern const double_t pi;
/********************************************************************************
*                                                                               *
*                              Non NCEG extensions                              *
*                                                                               *
********************************************************************************/
#ifndef __NOEXTENSIONS__
/********************************************************************************
*                                                                               *
*                              Financial functions                              *
*                                                                               *
*   compound        Computes the compound interest factor "(1 + rate)^periods"  *
*                   more accurately than the straightforward computation with   *
*                   the Power function.  This is SANE's compound function.      *
*                                                                               *
*   annuity         Computes the present value factor for an annuity            *
*                   "(1 - (1 + rate)^(-periods)) /rate" more accurately than    *
*                   the straightforward computation with the Power function.    *
*                   This is SANE's annuity function.                            *
*                                                                               *
********************************************************************************/
/*
 *  compound()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) compound(double_t rate, double_t periods);


/*
 *  annuity()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) annuity(double_t rate, double_t periods);




/********************************************************************************
*                                                                               *
*                              Random function                                  *
*                                                                               *
*   randomx         A pseudorandom number generator.  It uses the iteration:    *
*                               (7^5*x)mod(2^31-1)                              *
*                                                                               *
********************************************************************************/
/*
 *  randomx()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) randomx(double_t * x);




/*******************************************************************************
*                              Relational operator                             *
*******************************************************************************/
/*      relational operator      */
typedef short                           relop;
enum {
  GREATERTHAN                   = 0,
  LESSTHAN                      = 1,
  EQUALTO                       = 2,
  UNORDERED                     = 3
};

#if !defined(__MWERKS__) || !defined(__cmath__)
/*
 *  relation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( relop ) relation(double_t x, double_t y);


#endif /* !defined(__MWERKS__) || !defined(__cmath__) */


/********************************************************************************
*                                                                               *
*                         Binary to decimal conversions                         *
*                                                                               *
*   SIGDIGLEN   Significant decimal digits.                                     *
*                                                                               *
*   decimal     A record which provides an intermediate unpacked form for       *
*               programmers who wish to do their own parsing of numeric input   *
*               or formatting of numeric output.                                *
*                                                                               *
*   decform     Controls each conversion to a decimal string.  The style field  *
*               is either FLOATDECIMAL or FIXEDDECIMAL. If FLOATDECIMAL, the    *
*               value of the field digits is the number of significant digits.  *
*               If FIXEDDECIMAL value of the field digits is the number of      *
*               digits to the right of the decimal point.                       *
*                                                                               *
*   num2dec     Converts a double_t to a decimal record using a decform.        *
*   dec2num     Converts a decimal record d to a double_t value.                *
*   dec2str     Converts a decform and decimal to a string using a decform.     *
*   str2dec     Converts a string to a decimal struct.                          *
*   dec2d       Similar to dec2num except a double is returned (68k only).      *
*   dec2f       Similar to dec2num except a float is returned.                  *
*   dec2s       Similar to dec2num except a short is returned.                  *
*   dec2l       Similar to dec2num except a long is returned.                   *
*                                                                               *
********************************************************************************/
#if TARGET_CPU_PPC
    #define SIGDIGLEN      36  
#elif TARGET_CPU_68K
    #define SIGDIGLEN      20
#elif TARGET_CPU_X86
    #define SIGDIGLEN      20
#endif
#define      DECSTROUTLEN   80               /* max length for dec2str output */
#define      FLOATDECIMAL   ((char)(0))
#define      FIXEDDECIMAL   ((char)(1))
struct decimal {
    char                            sgn;                        /* sign 0 for +, 1 for - */
    char                            unused;
    short                           exp;                        /* decimal exponent */
    struct {
        unsigned char                   length;
        unsigned char                   text[SIGDIGLEN];        /* significant digits */
        unsigned char                   unused;
    }                               sig;
};
typedef struct decimal decimal;

struct decform {
    char                            style;                      /*  FLOATDECIMAL or FIXEDDECIMAL */
    char                            unused;
    short                           digits;
};
typedef struct decform decform;
/*
 *  num2dec()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void ) num2dec(const decform *f, double_t x, decimal *d);


/*
 *  dec2num()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double_t ) dec2num(const decimal * d);


/*
 *  dec2str()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void ) dec2str(const decform *f, const decimal *d, char *s);


/*
 *  str2dec()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void ) str2dec(const char *s, short *ix, decimal *d, short *vp);


#if TARGET_CPU_68K
#if CALL_NOT_IN_CARBON
/*
 *  dec2d()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( double ) dec2d(const decimal * d);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_CPU_68K */

/*
 *  dec2f()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( float ) dec2f(const decimal * d);


/*
 *  dec2s()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( short ) dec2s(const decimal * d);


/*
 *  dec2l()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long ) dec2l(const decimal * d);





/********************************************************************************
*                                                                               *
*                         68k-only Transfer Function Prototypes                 *
*                                                                               *
********************************************************************************/
#if TARGET_CPU_68K
#if CALL_NOT_IN_CARBON
/*
 *  x96tox80()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void ) x96tox80(const extended96 *x, extended80 *x80);


/*
 *  x80tox96()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void ) x80tox96(const extended80 *x80, extended96 *x);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_CPU_68K */

#endif  /* !defined(__NOEXTENSIONS__) */

/********************************************************************************
*                                                                               *
*                         PowerPC-only Function Prototypes                      *
*                                                                               *
********************************************************************************/

#if TARGET_CPU_PPC
#ifndef __MWERKS__  /* Metrowerks does not support double double */

/*
 *  cosl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) cosl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) cosl(long double x) { return (long double) cos((double)(x)); }
  #else
    #define cosl(x) ((long double) cos((double)(x)))
  #endif
#endif



/*
 *  sinl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) sinl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) sinl(long double x) { return (long double) sin((double)(x)); }
  #else
    #define sinl(x) ((long double) sin((double)(x)))
  #endif
#endif



/*
 *  tanl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) tanl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) tanl(long double x) { return (long double) tan((double)(x)); }
  #else
    #define tanl(x) ((long double) tan((double)(x)))
  #endif
#endif



/*
 *  acosl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) acosl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) acosl(long double x) { return (long double) acos((double)(x)); }
  #else
    #define acosl(x) ((long double) acos((double)(x)))
  #endif
#endif



/*
 *  asinl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) asinl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) asinl(long double x) { return (long double) asin((double)(x)); }
  #else
    #define asinl(x) ((long double) asin((double)(x)))
  #endif
#endif



/*
 *  atanl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) atanl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) atanl(long double x) { return (long double) atan((double)(x)); }
  #else
    #define atanl(x) ((long double) atan((double)(x)))
  #endif
#endif



/*
 *  atan2l()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) atan2l(long double y, long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) atan2l(long double y, long double x) { return (long double) atan2((double)(y), (double)(x)); }
  #else
    #define atan2l(y, x) ((long double) atan2((double)(y), (double)(x)))
  #endif
#endif



/*
 *  coshl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) coshl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) coshl(long double x) { return (long double) cosh((double)(x)); }
  #else
    #define coshl(x) ((long double) cosh((double)(x)))
  #endif
#endif



/*
 *  sinhl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) sinhl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) sinhl(long double x) { return (long double) sinh((double)(x)); }
  #else
    #define sinhl(x) ((long double) sinh((double)(x)))
  #endif
#endif



/*
 *  tanhl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) tanhl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) tanhl(long double x) { return (long double) tanh((double)(x)); }
  #else
    #define tanhl(x) ((long double) tanh((double)(x)))
  #endif
#endif



/*
 *  acoshl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) acoshl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) acoshl(long double x) { return (long double) acosh((double)(x)); }
  #else
    #define acoshl(x) ((long double) acosh((double)(x)))
  #endif
#endif



/*
 *  asinhl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) asinhl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) asinhl(long double x) { return (long double) asinh((double)(x)); }
  #else
    #define asinhl(x) ((long double) asinh((double)(x)))
  #endif
#endif



/*
 *  atanhl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) atanhl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) atanhl(long double x) { return (long double) atanh((double)(x)); }
  #else
    #define atanhl(x) ((long double) atanh((double)(x)))
  #endif
#endif



/*
 *  expl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) expl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) expl(long double x) { return (long double) exp((double)(x)); }
  #else
    #define expl(x) ((long double) exp((double)(x)))
  #endif
#endif



/*
 *  expm1l()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) expm1l(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) expm1l(long double x) { return (long double) expm1((double)(x)); }
  #else
    #define expm1l(x) ((long double) expm1((double)(x)))
  #endif
#endif



/*
 *  exp2l()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) exp2l(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) exp2l(long double x) { return (long double) exp2((double)(x)); }
  #else
    #define exp2l(x) ((long double) exp2((double)(x)))
  #endif
#endif



/*
 *  frexpl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) frexpl(long double x, int *exponent);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) frexpl(long double x, int *exponent) { return (long double) frexp((double)(x), (exponent)); }
  #else
    #define frexpl(x, exponent) ((long double) frexp((double)(x), (exponent)))
  #endif
#endif



/*
 *  ldexpl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) ldexpl(long double x, int n);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) ldexpl(long double x, int n) { return (long double) ldexp((double)(x), (n)); }
  #else
    #define ldexpl(x, n) ((long double) ldexp((double)(x), (n)))
  #endif
#endif



/*
 *  logl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) logl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) logl(long double x) { return (long double) log((double)(x)); }
  #else
    #define logl(x) ((long double) log((double)(x)))
  #endif
#endif



/*
 *  log1pl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) log1pl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) log1pl(long double x) { return (long double) log1p((double)(x)); }
  #else
    #define log1pl(x) ((long double) log1p((double)(x)))
  #endif
#endif



/*
 *  log10l()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) log10l(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) log10l(long double x) { return (long double) log10((double)(x)); }
  #else
    #define log10l(x) ((long double) log10((double)(x)))
  #endif
#endif



/*
 *  log2l()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) log2l(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) log2l(long double x) { return (long double) log2((double)(x)); }
  #else
    #define log2l(x) ((long double) log2((double)(x)))
  #endif
#endif



/*
 *  logbl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) logbl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) logbl(long double x) { return (long double) logb((double)(x)); }
  #else
    #define logbl(x) ((long double) logb((double)(x)))
  #endif
#endif



/*
 *  scalbl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) scalbl(long double x, long n);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) scalbl(long double x, long n) { return (long double) scalb((double)(x), (n)); }
  #else
    #define scalbl(x, n) ((long double) scalb((double)(x), (n)))
  #endif
#endif



/*
 *  fabsl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) fabsl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) fabsl(long double x) { return (long double) fabs((double)(x)); }
  #else
    #define fabsl(x) ((long double) fabs((double)(x)))
  #endif
#endif



/*
 *  hypotl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) hypotl(long double x, long double y);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) hypotl(long double x, long double y) { return (long double) hypot((double)(x), (double)(y)); }
  #else
    #define hypotl(x, y) ((long double) hypot((double)(x), (double)(y)))
  #endif
#endif



/*
 *  powl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) powl(long double x, long double y);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) powl(long double x, long double y) { return (long double) pow((double)(x), (double)(y)); }
  #else
    #define powl(x, y) ((long double) pow((double)(x), (double)(y)))
  #endif
#endif



/*
 *  sqrtl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) sqrtl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) sqrtl(long double x) { return (long double) sqrt((double)(x)); }
  #else
    #define sqrtl(x) ((long double) sqrt((double)(x)))
  #endif
#endif



/*
 *  erfl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) erfl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) erfl(long double x) { return (long double) erf((double)(x)); }
  #else
    #define erfl(x) ((long double) erf((double)(x)))
  #endif
#endif



/*
 *  erfcl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) erfcl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) erfcl(long double x) { return (long double) erfc((double)(x)); }
  #else
    #define erfcl(x) ((long double) erfc((double)(x)))
  #endif
#endif



/*
 *  gammal()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) gammal(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) gammal(long double x) { return (long double) gamma((double)(x)); }
  #else
    #define gammal(x) ((long double) gamma((double)(x)))
  #endif
#endif



/*
 *  lgammal()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) lgammal(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) lgammal(long double x) { return (long double) lgamma((double)(x)); }
  #else
    #define lgammal(x) ((long double) lgamma((double)(x)))
  #endif
#endif



/*
 *  ceill()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 2.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) ceill(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) ceill(long double x) { return (long double) ceil((double)(x)); }
  #else
    #define ceill(x) ((long double) ceil((double)(x)))
  #endif
#endif



/*
 *  floorl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) floorl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) floorl(long double x) { return (long double) floor((double)(x)); }
  #else
    #define floorl(x) ((long double) floor((double)(x)))
  #endif
#endif



/*
 *  rintl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) rintl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) rintl(long double x) { return (long double) rint((double)(x)); }
  #else
    #define rintl(x) ((long double) rint((double)(x)))
  #endif
#endif



/*
 *  nearbyintl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) nearbyintl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) nearbyintl(long double x) { return (long double) nearbyint((double)(x)); }
  #else
    #define nearbyintl(x) ((long double) nearbyint((double)(x)))
  #endif
#endif



/*
 *  rinttoll()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long ) rinttoll(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long ) rinttoll(long double x) { return rinttol((double)(x)); }
  #else
    #define rinttoll(x) (rinttol((double)(x)))
  #endif
#endif



/*
 *  roundl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) roundl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) roundl(long double x) { return (long double) round((double)(x)); }
  #else
    #define roundl(x) ((long double) round((double)(x)))
  #endif
#endif



/*
 *  roundtoll()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long ) roundtoll(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long ) roundtoll(long double x) { return roundtol((double)(x)); }
  #else
    #define roundtoll(x) (roundtol((double)(x)))
  #endif
#endif



/*
 *  truncl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) truncl(long double x);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) truncl(long double x) { return (long double) trunc((double)(x)); }
  #else
    #define truncl(x) ((long double) trunc((double)(x)))
  #endif
#endif



/*
 *  remainderl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) remainderl(long double x, long double y);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) remainderl(long double x, long double y) { return (long double) remainder((double)(x), (double)(y)); }
  #else
    #define remainderl(x, y) ((long double) remainder((double)(x), (double)(y)))
  #endif
#endif



/*
 *  remquol()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) remquol(long double x, long double y, int *quo);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) remquol(long double x, long double y, int *quo) { return (long double) remquo((double)(x), (double)(y), (quo)); }
  #else
    #define remquol(x, y, quo) ((long double) remquo((double)(x), (double)(y), (quo)))
  #endif
#endif



/*
 *  copysignl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) copysignl(long double x, long double y);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) copysignl(long double x, long double y) { return (long double) copysign((double)(x), (double)(y)); }
  #else
    #define copysignl(x, y) ((long double) copysign((double)(x), (double)(y)))
  #endif
#endif



/*
 *  fdiml()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) fdiml(long double x, long double y);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) fdiml(long double x, long double y) { return (long double) fdim((double)(x), (double)(y)); }
  #else
    #define fdiml(x, y) ((long double) fdim((double)(x), (double)(y)))
  #endif
#endif



/*
 *  fmaxl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) fmaxl(long double x, long double y);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) fmaxl(long double x, long double y) { return (long double) fmax((double)(x), (double)(y)); }
  #else
    #define fmaxl(x, y) ((long double) fmax((double)(x), (double)(y)))
  #endif
#endif



/*
 *  fminl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) fminl(long double x, long double y);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) fminl(long double x, long double y) { return (long double) fmin((double)(x), (double)(y)); }
  #else
    #define fminl(x, y) ((long double) fmin((double)(x), (double)(y)))
  #endif
#endif


#endif /* __MWERKS__ */
#ifndef __NOEXTENSIONS__
/*
 *  relationl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( relop ) relationl(long double x, long double y);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(relop ) relationl(long double x, long double y) { return relation((double)(x), (double)(y)); }
  #else
    #define relationl(x, y) (relation((double)(x), (double)(y)))
  #endif
#endif



/*
 *  num2decl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( void ) num2decl(const decform *f, long double x, decimal *d);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(void) num2decl(const decform *f, long double x, decimal *d) { num2dec((f), (double)(x), (d)); }
  #else
    #define num2decl(f, x, d) (num2dec((f), (double)(x), (d)))
  #endif
#endif



/*
 *  dec2numl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double ) dec2numl(const decimal * d);
#if TYPE_LONGDOUBLE_IS_DOUBLE
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) dec2numl(const decimal *d) { return (long double) dec2num(d); }
  #else
    #define dec2numl(d) ((long double) dec2num(d))
  #endif
#endif


#endif  /* !defined(__NOEXTENSIONS__) */

#endif  /* TARGET_CPU_PPC */

#endif  /* TARGET_OS_MAC */

#ifndef __NOEXTENSIONS__
/*    
        MathLib v2 has two new transfer functions: x80tod and dtox80.  They can 
        be used to directly transform 68k 80-bit extended data types to double
        and back for PowerPC based machines without using the functions
        x80told or ldtox80.  Double rounding may occur. 
    */
/*
 *  x80tod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( double ) x80tod(const extended80 * x80);


/*
 *  dtox80()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void ) dtox80(const double *x, extended80 *x80);


/*
 *  x80told()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( void ) x80told(const extended80 *x80, long double *x);
#if TYPE_LONGDOUBLE_IS_DOUBLE && !TARGET_OS_WIN32
  #ifdef __cplusplus
    inline DEFINE_API_C(void) x80told(const extended80 *x80, long double *x) { *(x) = (long double) x80tod(x80); }
  #else
    #define x80told(x80, x) (*(x) = (long double) x80tod(x80))
  #endif
#endif



/*
 *  ldtox80()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( void ) ldtox80(const long double *x, extended80 *x80);
#if TYPE_LONGDOUBLE_IS_DOUBLE && !TARGET_OS_WIN32
  #ifdef __cplusplus
    inline DEFINE_API_C(void) ldtox80(const long double *x, extended80 *x80) { double d = (double) *(x); dtox80(&d, (x80)); }
  #else
    #define ldtox80(x, x80) do { double d = (double) *(x); dtox80(&d, (x80)); } while (false)
  #endif
#endif


#endif  /* !defined(__NOEXTENSIONS__) */


#if PRAGMA_STRUCT_ALIGN
    #pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
    #pragma pack()
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __FP__ */

