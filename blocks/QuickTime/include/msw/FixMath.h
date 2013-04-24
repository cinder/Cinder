/*
     File:       FixMath.h
 
     Contains:   Fixed Math Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __FIXMATH__
#define __FIXMATH__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif




#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif


#define fixed1              ((Fixed) 0x00010000L)
#define fract1              ((Fract) 0x40000000L)
#define positiveInfinity    ((long)  0x7FFFFFFFL)
#define negativeInfinity    ((long)  0x80000000L)
/*
    FixRatio, FixMul, and FixRound were previously in ToolUtils.h
*/
/*
 *  FixRatio()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fixed )
FixRatio(
  short   numer,
  short   denom)                                              ONEWORDINLINE(0xA869);


/*
 *  FixMul()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fixed )
FixMul(
  Fixed   a,
  Fixed   b)                                                  ONEWORDINLINE(0xA868);


/*
 *  FixRound()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
FixRound(Fixed x)                                             ONEWORDINLINE(0xA86C);


/*
 *  Fix2Frac()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fract )
Fix2Frac(Fixed x)                                             ONEWORDINLINE(0xA841);


/*
 *  Fix2Long()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
Fix2Long(Fixed x)                                             ONEWORDINLINE(0xA840);


/*
 *  Long2Fix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fixed )
Long2Fix(long x)                                              ONEWORDINLINE(0xA83F);


/*
 *  Frac2Fix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fixed )
Frac2Fix(Fract x)                                             ONEWORDINLINE(0xA842);


/*
 *  FracMul()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fract )
FracMul(
  Fract   x,
  Fract   y)                                                  ONEWORDINLINE(0xA84A);


/*
 *  FixDiv()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fixed )
FixDiv(
  Fixed   x,
  Fixed   y)                                                  ONEWORDINLINE(0xA84D);


/*
 *  FracDiv()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fract )
FracDiv(
  Fract   x,
  Fract   y)                                                  ONEWORDINLINE(0xA84B);


/*
 *  FracSqrt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fract )
FracSqrt(Fract x)                                             ONEWORDINLINE(0xA849);


/*
 *  FracSin()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fract )
FracSin(Fixed x)                                              ONEWORDINLINE(0xA848);


/*
 *  FracCos()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fract )
FracCos(Fixed x)                                              ONEWORDINLINE(0xA847);


/*
 *  FixATan2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fixed )
FixATan2(
  long   x,
  long   y)                                                   ONEWORDINLINE(0xA818);


/*
    Frac2X, Fix2X, X2Fix, and X2Frac translate to and from
    the floating point type "extended" (that's what the X is for).
    On the original Mac this was 80-bits and the functions could be
    accessed via A-Traps.  When the 68881 co-processor was added,
    it used 96-bit floating point types, so the A-Traps could not 
    be used.  When PowerPC was added, it used 64-bit floating point
    types, so yet another prototype was added.
*/
#if TARGET_CPU_68K
#if TARGET_RT_MAC_68881
#if CALL_NOT_IN_CARBON
/*
 *  Frac2X()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( long double )
Frac2X(Fract x);


/*
 *  Fix2X()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( long double )
Fix2X(Fixed x);


/*
 *  X2Fix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Fixed )
X2Fix(long double x);


/*
 *  X2Frac()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Fract )
X2Frac(long double x);


#endif  /* CALL_NOT_IN_CARBON */

#else
#if CALL_NOT_IN_CARBON
/*
 *  Frac2X()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( long double )
Frac2X(Fract x)                                               ONEWORDINLINE(0xA845);


/*
 *  Fix2X()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( long double )
Fix2X(Fixed x)                                                ONEWORDINLINE(0xA843);


/*
 *  X2Fix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Fixed )
X2Fix(long double x)                                          ONEWORDINLINE(0xA844);


/*
 *  X2Frac()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Fract )
X2Frac(long double x)                                         ONEWORDINLINE(0xA846);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_RT_MAC_68881 */

#else
/*
 *  Frac2X()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( double )
Frac2X(Fract x);


/*
 *  Fix2X()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( double )
Fix2X(Fixed x);


/*
 *  X2Fix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fixed )
X2Fix(double x);


/*
 *  X2Frac()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fract )
X2Frac(double x);


#endif  /* TARGET_CPU_68K */

/* QuickTime 3.0 makes these Wide routines available on other platforms*/
#if TARGET_CPU_PPC || !TARGET_OS_MAC || TARGET_CPU_X86
/*
 *  WideCompare()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( short )
WideCompare(
  const wide *  target,
  const wide *  source);


/*
 *  WideAdd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( wide * )
WideAdd(
  wide *        target,
  const wide *  source);


/*
 *  WideSubtract()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( wide * )
WideSubtract(
  wide *        target,
  const wide *  source);


/*
 *  WideNegate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( wide * )
WideNegate(wide * target);


/*
 *  WideShift()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( wide * )
WideShift(
  wide *  target,
  long    shift);


/*
 *  WideSquareRoot()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( unsigned long )
WideSquareRoot(const wide * source);


/*
 *  WideMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( wide * )
WideMultiply(
  long    multiplicand,
  long    multiplier,
  wide *  target);


/* returns the quotient */
/*
 *  WideDivide()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long )
WideDivide(
  const wide *  dividend,
  long          divisor,
  long *        remainder);


/* quotient replaces dividend */
/*
 *  WideWideDivide()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( wide * )
WideWideDivide(
  wide *  dividend,
  long    divisor,
  long *  remainder);


/*
 *  WideBitShift()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( wide * )
WideBitShift(
  wide *  src,
  long    shift);


#endif  /* TARGET_CPU_PPC || !TARGET_OS_MAC || TARGET_CPU_X86 */



#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __FIXMATH__ */

