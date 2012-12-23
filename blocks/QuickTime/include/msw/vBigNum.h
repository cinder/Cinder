/*
     File:       vBigNum.h
 
     Contains:   Algebraic and logical operations on large operands.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __VBIGNUM__
#define __VBIGNUM__

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
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

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif

#ifdef __VEC__
/************************************************************************************
*                                                                                   *
*  This library provides a set of subroutines for basic algebraic and some logical  *
*  operations performed on operands with the following sizes:                       *
*                                                                                   *
*            128 - bits                                                             *
*            256 - bits                                                             *
*            512 - bits                                                             *
*           1024 - bits                                                             *
*                                                                                   *
*  Following basic and algebraic operations are included:                           *
*                                                                                   *
*            Addition                                                               *
*            Subtraction                                                            *
*            Multiplication                                                         *
*            Division                                                               *
*            Mod                                                                    *
*            Shift Right                                                            *
*            Shift Right Arithmatic                                                 *
*            Shift Left                                                             *
*            Rotate Right                                                           *
*            Rotate Left                                                            *
*                                                                                   *
*                                                                                   *
************************************************************************************/
/***********************************************************************************
*   Following abbreviations are used in the names of functions in this library:    *
*                                                                                  *
*      v            Vector                                                         *
*      U            Unsigned                                                       *
*      S            Signed                                                         *
*      128          128  - bit                                                     *
*      256          256  - bit                                                     *
*      512          512  - bit                                                     *
*      1024         1024 - bit                                                     *
*      Add          Addition, modular arithmetic                                   *
*      AddS         Addition with Saturation                                       *
*      Sub          Subtraction, modular arithmetic                                *
*      SubS         Subtraction with Saturation                                    *
*      Multiply     Multiplication                                                 *
*      Divide       Division                                                       *
*      Half         Half (multiplication, width of result is the same as width of  *
*                      operands)                                                   *                         
*      Full         Full (multiplication, width of result is twice width of each   *
*                      operand)                                                    *
*                                                                                  *
*      Mod          Modular operation                                              *
*      Neg          Negate a number                                                *
*      A            Algebraic                                                      *
*      LL           Logical Left                                                   *
*      LR           Logical Right                                                  *
*      Shift        Shift                                                          *
*      Rotate       Rotation                                                       *
*                                                                                  *
***********************************************************************************/
/************************************************************************************
*                                                                                   *
*  A few explanations for the choices made in naming, passing arguments, and        *
*  various functions.                                                               *
*                                                                                   *
*      1) Names for the functions are made compatible with the names used in the    *
*      vBasicOps library. The format of the names are the same and include a        *
*      designation to show a vector operation, then a symbol for the type of data   *
*      (signed or unsigned), followed by the size of operands, then the operation   *
*      performed.                                                                   *
*                                                                                   *
*      2) Note that the logical and arithmetic shiftLeft operation are the same.    *
*                                                                                   *
*      3) Rotate operation is performed on unsigned and signed numbers.             *
*                                                                                   *
************************************************************************************/

/************************************************************************************
*                                                                                   *
*  Following are a set of structures for vector data types and scalar data types    *
*                                                                                   *
************************************************************************************/

union vU128 {
  vector unsigned int  v;
  struct {
    unsigned long       MSW;
    unsigned long       d2;
    unsigned long       d3;
    unsigned long       LSW;
  }                       s;
};
typedef union vU128                     vU128;
union vS128 {
  vector unsigned int  v;
  struct {
    signed long         MSW;
    unsigned long       d2;
    unsigned long       d3;
    unsigned long       LSW;
  }                       s;
};
typedef union vS128                     vS128;
union vU256 {
  vector unsigned int  v[2];
  struct {
    unsigned long       MSW;
    unsigned long       d2;
    unsigned long       d3;
    unsigned long       d4;
    unsigned long       d5;
    unsigned long       d6;
    unsigned long       d7;
    unsigned long       LSW;
  }                       s;
};
typedef union vU256                     vU256;
union vS256 {
  vector unsigned int  v[2];
  struct {
    signed long         MSW;
    unsigned long       d2;
    unsigned long       d3;
    unsigned long       d4;
    unsigned long       d5;
    unsigned long       d6;
    unsigned long       d7;
    unsigned long       LSW;
  }                       s;
};
typedef union vS256                     vS256;
union vU512 {
  vector unsigned int  v[4];
  struct {
    unsigned long       MSB;
    unsigned long       d2;
    unsigned long       d3;
    unsigned long       d4;
    unsigned long       d5;
    unsigned long       d6;
    unsigned long       d7;
    unsigned long       d8;
    unsigned long       d9;
    unsigned long       d10;
    unsigned long       d11;
    unsigned long       d12;
    unsigned long       d13;
    unsigned long       d14;
    unsigned long       d15;
    unsigned long       LSB;
  }                       s;
};
typedef union vU512                     vU512;
union vS512 {
  vector unsigned int  v[4];
  struct {
    signed long         MSW;
    unsigned long       d2;
    unsigned long       d3;
    unsigned long       d4;
    unsigned long       d5;
    unsigned long       d6;
    unsigned long       d7;
    unsigned long       d8;
    unsigned long       d9;
    unsigned long       d10;
    unsigned long       d11;
    unsigned long       d12;
    unsigned long       d13;
    unsigned long       d14;
    unsigned long       d15;
    unsigned long       LSW;
  }                       s;
};
typedef union vS512                     vS512;
union vU1024 {
  vector unsigned int  v[8];
  struct {
    unsigned long       MSW;
    unsigned long       d2;
    unsigned long       d3;
    unsigned long       d4;
    unsigned long       d5;
    unsigned long       d6;
    unsigned long       d7;
    unsigned long       d8;
    unsigned long       d9;
    unsigned long       d10;
    unsigned long       d11;
    unsigned long       d12;
    unsigned long       d13;
    unsigned long       d14;
    unsigned long       d15;
    unsigned long       d16;
    unsigned long       d17;
    unsigned long       d18;
    unsigned long       d19;
    unsigned long       d20;
    unsigned long       d21;
    unsigned long       d22;
    unsigned long       d23;
    unsigned long       d24;
    unsigned long       d25;
    unsigned long       d26;
    unsigned long       d27;
    unsigned long       d28;
    unsigned long       d29;
    unsigned long       d30;
    unsigned long       d31;
    unsigned long       LSW;
  }                       s;
};
typedef union vU1024                    vU1024;
union vS1024 {
  vector unsigned int  v[8];
  struct {
    signed long         MSW;
    unsigned long       d2;
    unsigned long       d3;
    unsigned long       d4;
    unsigned long       d5;
    unsigned long       d6;
    unsigned long       d7;
    unsigned long       d8;
    unsigned long       d9;
    unsigned long       d10;
    unsigned long       d11;
    unsigned long       d12;
    unsigned long       d13;
    unsigned long       d14;
    unsigned long       d15;
    unsigned long       d16;
    unsigned long       d17;
    unsigned long       d18;
    unsigned long       d19;
    unsigned long       d20;
    unsigned long       d21;
    unsigned long       d22;
    unsigned long       d23;
    unsigned long       d24;
    unsigned long       d25;
    unsigned long       d26;
    unsigned long       d27;
    unsigned long       d28;
    unsigned long       d29;
    unsigned long       d30;
    unsigned long       d31;
    unsigned long       LSW;
  }                       s;
};
typedef union vS1024                    vS1024;

/************************************************************************************
*                                                                                   *
*                                Division operations                                *
*                                                                                   *
************************************************************************************/


/*
 *  vU256Divide()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU256Divide(
  const vU256 *  numerator,
  const vU256 *  divisor,
  vU256 *        result,
  vU256 *        remainder);


/*
 *  vS256Divide()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS256Divide(
  const vS256 *  numerator,
  const vS256 *  divisor,
  vS256 *        result,
  vS256 *        remainder);


/*
 *  vU512Divide()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU512Divide(
  const vU512 *  numerator,
  const vU512 *  divisor,
  vU512 *        result,
  vU512 *        remainder);


/*
 *  vS512Divide()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS512Divide(
  const vS512 *  numerator,
  const vS512 *  divisor,
  vS512 *        result,
  vS512 *        remainder);


/*
 *  vU1024Divide()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU1024Divide(
  const vU1024 *  numerator,
  const vU1024 *  divisor,
  vU1024 *        result,
  vU1024 *        remainder);


/*
 *  vS1024Divide()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS1024Divide(
  const vS1024 *  numerator,
  const vS1024 *  divisor,
  vS1024 *        result,
  vS1024 *        remainder);



/************************************************************************************
*                                                                                   *
*                              Multiply operations                                  *
*                                                                                   *
************************************************************************************/

/*
 *  vU128FullMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU128FullMultiply(
  const vU128 *  a,
  const vU128 *  b,
  vU256 *        result);


/*
 *  vS128FullMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS128FullMultiply(
  const vS128 *  a,
  const vS128 *  b,
  vS256 *        result);


/*
 *  vU256FullMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU256FullMultiply(
  const vU256 *  a,
  const vU256 *  b,
  vU512 *        result);


/*
 *  vS256FullMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS256FullMultiply(
  const vS256 *  a,
  const vS256 *  b,
  vS512 *        result);


/*
 *  vU512FullMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU512FullMultiply(
  const vU512 *  a,
  const vU512 *  b,
  vU1024 *       result);


/*
 *  vS512FullMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS512FullMultiply(
  const vS512 *  a,
  const vS512 *  b,
  vS1024 *       result);


/*
 *  vU256HalfMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU256HalfMultiply(
  const vU256 *  a,
  const vU256 *  b,
  vU256 *        result);


/*
 *  vS256HalfMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS256HalfMultiply(
  const vS256 *  a,
  const vS256 *  b,
  vS256 *        result);


/*
 *  vU512HalfMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU512HalfMultiply(
  const vU512 *  a,
  const vU512 *  b,
  vU512 *        result);


/*
 *  vS512HalfMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS512HalfMultiply(
  const vS512 *  a,
  const vS512 *  b,
  vS512 *        result);


/*
 *  vU1024HalfMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU1024HalfMultiply(
  const vU1024 *  a,
  const vU1024 *  b,
  vU1024 *        result);


/*
 *  vS1024HalfMultiply()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS1024HalfMultiply(
  const vS1024 *  a,
  const vS1024 *  b,
  vS1024 *        result);



/************************************************************************************
*                                                                                   *
*                             Subtraction operations                                *
*                                                                                   *
************************************************************************************/

/*
 *  vU256Sub()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU256Sub(
  const vU256 *  a,
  const vU256 *  b,
  vU256 *        result);


/*
 *  vS256Sub()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS256Sub(
  const vS256 *  a,
  const vS256 *  b,
  vS256 *        result);


/*
 *  vU256SubS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU256SubS(
  const vU256 *  a,
  const vU256 *  b,
  vU256 *        result);


/*
 *  vS256SubS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS256SubS(
  const vS256 *  a,
  const vS256 *  b,
  vS256 *        result);


/*
 *  vU512Sub()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU512Sub(
  const vU512 *  a,
  const vU512 *  b,
  vU512 *        result);


/*
 *  vS512Sub()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS512Sub(
  const vS512 *  a,
  const vS512 *  b,
  vS512 *        result);


/*
 *  vU512SubS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU512SubS(
  const vU512 *  a,
  const vU512 *  b,
  vU512 *        result);


/*
 *  vS512SubS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS512SubS(
  const vS512 *  a,
  const vS512 *  b,
  vS512 *        result);


/*
 *  vU1024Sub()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU1024Sub(
  const vU1024 *  a,
  const vU1024 *  b,
  vU1024 *        result);


/*
 *  vS1024Sub()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS1024Sub(
  const vS1024 *  a,
  const vS1024 *  b,
  vS1024 *        result);


/*
 *  vU1024SubS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU1024SubS(
  const vU1024 *  a,
  const vU1024 *  b,
  vU1024 *        result);


/*
 *  vS1024SubS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS1024SubS(
  const vS1024 *  a,
  const vS1024 *  b,
  vS1024 *        result);



/************************************************************************************
*                                                                                   *
*                                Negate operations                                  *
*                                                                                   *
************************************************************************************/

/*
 *  vU256Neg()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU256Neg(
  const vU256 *  a,
  vU256 *        result);


/*
 *  vS256Neg()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS256Neg(
  const vS256 *  a,
  vS256 *        result);


/*
 *  vU512Neg()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU512Neg(
  const vU512 *  a,
  vU512 *        result);


/*
 *  vS512Neg()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS512Neg(
  const vS512 *  a,
  vS512 *        result);


/*
 *  vU1024Neg()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU1024Neg(
  const vU1024 *  a,
  vU1024 *        result);


/*
 *  vS1024Neg()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS1024Neg(
  const vS1024 *  a,
  vS1024 *        result);



/************************************************************************************
*                                                                                   *
*                                Addition operations                                *
*                                                                                   *
************************************************************************************/

/*
 *  vU256Add()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU256Add(
  const vU256 *  a,
  const vU256 *  b,
  vU256 *        result);


/*
 *  vS256Add()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS256Add(
  const vS256 *  a,
  const vS256 *  b,
  vS256 *        result);


/*
 *  vU256AddS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU256AddS(
  const vU256 *  a,
  const vU256 *  b,
  vU256 *        result);


/*
 *  vS256AddS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS256AddS(
  const vS256 *  a,
  const vS256 *  b,
  vS256 *        result);


/*
 *  vU512Add()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU512Add(
  const vU512 *  a,
  const vU512 *  b,
  vU512 *        result);


/*
 *  vS512Add()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS512Add(
  const vS512 *  a,
  const vS512 *  b,
  vS512 *        result);


/*
 *  vU512AddS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU512AddS(
  const vU512 *  a,
  const vU512 *  b,
  vU512 *        result);


/*
 *  vS512AddS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS512AddS(
  const vS512 *  a,
  const vS512 *  b,
  vS512 *        result);


/*
 *  vU1024Add()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU1024Add(
  const vU1024 *  a,
  const vU1024 *  b,
  vU1024 *        result);


/*
 *  vS1024Add()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS1024Add(
  const vS1024 *  a,
  const vS1024 *  b,
  vS1024 *        result);


/*
 *  vU1024AddS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU1024AddS(
  const vU1024 *  a,
  const vU1024 *  b,
  vU1024 *        result);


/*
 *  vS1024AddS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS1024AddS(
  const vS1024 *  a,
  const vS1024 *  b,
  vS1024 *        result);



/************************************************************************************
*                                                                                   *
*                                   Mod operations                                  *
*                                                                                   *
************************************************************************************/

/*
 *  vU256Mod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU256Mod(
  const vU256 *  numerator,
  const vU256 *  divisor,
  vU256 *        remainder);


/*
 *  vS256Mod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS256Mod(
  const vS256 *  numerator,
  const vS256 *  divisor,
  vS256 *        remainder);


/*
 *  vU512Mod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU512Mod(
  const vU512 *  numerator,
  const vU512 *  divisor,
  vU512 *        remainder);


/*
 *  vS512Mod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS512Mod(
  const vS512 *  numerator,
  const vS512 *  divisor,
  vS512 *        remainder);


/*
 *  vU1024Mod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vU1024Mod(
  const vU1024 *  numerator,
  const vU1024 *  divisor,
  vU1024 *        remainder);


/*
 *  vS1024Mod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vS1024Mod(
  const vS1024 *  numerator,
  const vS1024 *  divisor,
  vS1024 *        remainder);



/************************************************************************************
*                                                                                   *
*                                Shift operations                                   *
*                                                                                   *
************************************************************************************/

/*
 *  vLL256Shift()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vLL256Shift(
  const vU256 *   a,
  unsigned long   shiftAmount,
  vU256 *         result);


/*
 *  vLL512Shift()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vLL512Shift(
  const vU512 *   a,
  unsigned long   shiftAmount,
  vU512 *         result);


/*
 *  vLL1024Shift()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vLL1024Shift(
  const vU1024 *  a,
  unsigned long   shiftAmount,
  vU1024 *        result);


/*
 *  vLR256Shift()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vLR256Shift(
  const vU256 *   a,
  unsigned long   shiftAmount,
  vU256 *         result);


/*
 *  vLR512Shift()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vLR512Shift(
  const vU512 *   a,
  unsigned long   shiftAmount,
  vU512 *         result);


/*
 *  vLR1024Shift()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vLR1024Shift(
  const vU1024 *  a,
  unsigned long   shiftAmount,
  vU1024 *        result);


/*
 *  vA256Shift()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vA256Shift(
  const vS256 *   a,
  unsigned long   shiftAmount,
  vS256 *         result);


/*
 *  vA512Shift()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vA512Shift(
  const vS512 *   a,
  unsigned long   shiftAmount,
  vS512 *         result);


/*
 *  vA1024Shift()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vA1024Shift(
  const vS1024 *  a,
  unsigned long   shiftAmount,
  vS1024 *        result);



/************************************************************************************
*                                                                                   *
*                                  Rotate operations                                *
*                                                                                   *
************************************************************************************/

/*
 *  vL256Rotate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vL256Rotate(
  const vU256 *   a,
  unsigned long   rotateAmount,
  vU256 *         result);


/*
 *  vL512Rotate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vL512Rotate(
  const vU512 *   a,
  unsigned long   rotateAmount,
  vU512 *         result);


/*
 *  vL1024Rotate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vL1024Rotate(
  const vU1024 *  a,
  unsigned long   rotateAmount,
  vU1024 *        result);


/*
 *  vR256Rotate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vR256Rotate(
  const vU256 *   a,
  unsigned long   rotateAmount,
  vU256 *         result);


/*
 *  vR512Rotate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vR512Rotate(
  const vU512 *   a,
  unsigned long   rotateAmount,
  vU512 *         result);


/*
 *  vR1024Rotate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
vR1024Rotate(
  const vU1024 *  a,
  unsigned long   rotateAmount,
  vU1024 *        result);



#endif  /* defined(__VEC__) */


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

#endif /* __VBIGNUM__ */

