/*
     File:       Math64.h
 
     Contains:   64-bit integer math Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1994-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __MATH64__
#define __MATH64__

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

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


#if TYPE_LONGLONG && TARGET_OS_WIN32
    #define S64Max() 9223372036854775807i64
#elif TYPE_LONGLONG && defined(__MWERKS__) && (__MWERKS__ < 0x1800)
    #define S64Max() 9223372036854775807
#else
/*
 *  S64Max()
 *  
 *  Discussion:
 *    Returns largest possible SInt64 value
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64Max(void);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64Max(void) { return 9223372036854775807LL; }
  #else
    #define S64Max() (9223372036854775807LL)
  #endif
#endif


#endif
/*
 *  S64Min()
 *  
 *  Discussion:
 *    Returns smallest possible SInt64 value
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64Min(void);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64Min(void) { return -S64Max() - 1; }
  #else
    #define S64Min() (-S64Max() - 1)
  #endif
#endif



/*
 *  S64Add()
 *  
 *  Discussion:
 *    Adds two integers, producing an integer result.  If an overflow
 *    occurs the result is congruent mod (2^64) as if the operands and
 *    result were unsigned.  No overflow is signaled.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64Add(
  SInt64   left,
  SInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64Add(SInt64 left, SInt64 right) { return (SInt64)(left) + (SInt64)(right); }
  #else
    #define S64Add(left, right) ((SInt64)(left) + (SInt64)(right))
  #endif
#endif



/*
 *  S64Subtract()
 *  
 *  Discussion:
 *    Subtracts two integers, producing an integer result.  If an
 *    overflow occurs the result is congruent mod (2^64) as if the
 *    operands and result were unsigned.  No overflow is signaled.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64Subtract(
  SInt64   left,
  SInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64Subtract(SInt64 left, SInt64 right) { return (SInt64)(left) - (SInt64)(right); }
  #else
    #define S64Subtract(left, right) ((SInt64)(left) - (SInt64)(right))
  #endif
#endif



/*
 *  S64Negate()
 *  
 *  Discussion:
 *    Returns the additive inverse of a signed number (i.e. it returns
 *    0 - the number).  S64Negate (S64Min) is not representable (in
 *    fact, it returns S64Min).
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64Negate(SInt64 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64Negate(SInt64 value) { return -(SInt64)(value); }
  #else
    #define S64Negate(value) (-(SInt64)(value))
  #endif
#endif



#if !TYPE_LONGLONG
/*
 *  S64Absolute()
 *  
 *  Discussion:
 *    Returns the absolute value of the number (i.e. the number if it
 *    is positive, or 0 - the number if it is negative). Disabled for
 *    compilers that support long long until llabs() is available
 *    everywhere.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64Absolute(SInt64 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64Absolute(SInt64 value) { return llabs((SInt64)value); }
  #else
    #define S64Absolute(value) (llabs((SInt64)value))
  #endif
#endif


#endif  /* !TYPE_LONGLONG */

/*
 *  S64Multiply()
 *  
 *  Discussion:
 *    Multiplies two signed numbers, producing a signed result. 
 *    Overflow is ignored and the low-order part of the product is
 *    returned.  The sign of the result is not guaranteed to be correct
 *    if the magnitude of the product is not representable.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64Multiply(
  SInt64   left,
  SInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64Multiply(SInt64 left, SInt64 right) { return (SInt64)(left) * (SInt64)(right); }
  #else
    #define S64Multiply(left, right) ((SInt64)(left) * (SInt64)(right))
  #endif
#endif



/*
 *  S64Mod()
 *  
 *  Discussion:
 *    Returns the remainder of divide of dividend by divisor.  The sign
 *    of the remainder is the same as the sign of the dividend (i.e.,
 *    it takes the absolute values of the operands, does the division,
 *    then fixes the sign of the quotient and remainder).
 *  
 *  Availability:
 *    Implemented by client
 */
EXTERN_API_C( SInt64 )
S64Mod(
  SInt64   dividend,
  SInt64   divisor);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64Mod(SInt64 dividend, SInt64 divisor) { return (SInt64)(dividend) % (SInt64)(divisor); }
  #else
    #define S64Mod(dividend, divisor) ((SInt64)(dividend) % (SInt64)(divisor))
  #endif
#endif




/*
 *  S64Divide()
 *  
 *  Discussion:
 *    Divides dividend by divisor, returning the quotient.  The
 *    remainder is returned in *remainder if remainder (the pointer) is
 *    non-NULL. The sign of the remainder is the same as the sign of
 *    the dividend (i.e. it takes the absolute values of the operands,
 *    does the division, then fixes the sign of the quotient and
 *    remainder).  If the divisor is zero, then S64Max() will be
 *    returned (or S64Min() if the dividend is negative), and the
 *    remainder will be the dividend; no error is reported.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64Divide(
  SInt64    dividend,
  SInt64    divisor,
  SInt64 *  remainder);      /* can be NULL */
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64Divide(SInt64 dividend, SInt64 divisor, SInt64 *remainder) { return ( (void)((remainder) && (*((SInt64*)(remainder)) = ((SInt64)(dividend) % (SInt64)(divisor)))), ((SInt64)(dividend) / (SInt64)(divisor)) ); }
  #else
    #define S64Divide(dividend, divisor, remainder) (( (void)((remainder) && (*((SInt64*)(remainder)) = ((SInt64)(dividend) % (SInt64)(divisor)))), ((SInt64)(dividend) / (SInt64)(divisor)) ))
  #endif
#endif



/*
 *  S64Div()
 *  
 *  Discussion:
 *    Divides dividend by divisor, returning the quotient.
 *  
 *  Availability:
 *    Implemented by client
 */
EXTERN_API_C( SInt64 )
S64Div(
  SInt64   dividend,
  SInt64   divisor);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64Div(SInt64 dividend, SInt64 divisor) { return S64Divide((dividend), (divisor), NULL); }
  #else
    #define S64Div(dividend, divisor) (S64Divide((dividend), (divisor), NULL))
  #endif
#endif



/*
 *  S64Set()
 *  
 *  Discussion:
 *    Given an SInt32, returns an SInt64 with the same value.  Use this
 *    routine instead of coding 64-bit constants (at least when the
 *    constant will fit in an SInt32).
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64Set(SInt32 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64Set(SInt32 value) { return (SInt64)(value); }
  #else
    #define S64Set(value) ((SInt64)(value))
  #endif
#endif



/*
 *  S64SetU()
 *  
 *  Discussion:
 *    Given a UInt32, returns a SInt64 with the same value.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64SetU(UInt32 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64SetU(UInt32 value) { return (SInt64)(value); }
  #else
    #define S64SetU(value) ((SInt64)(value))
  #endif
#endif


/*
 *  S32Set()
 *  
 *  Discussion:
 *    Given an SInt64, returns an SInt32 by discarding the high-order
 *    32 bits.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt32 )
S32Set(SInt64 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt32 ) S32Set(SInt64 value) { return (SInt32)(value); }
  #else
    #define S32Set(value) ((SInt32)(value))
  #endif
#endif



/*
 *  S64And()
 *  
 *  Discussion:
 *    Returns one if left and right are non-zero, otherwise returns zero
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
S64And(
  SInt64   left,
  SInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(Boolean ) S64And(SInt64 left, SInt64 right) { return (SInt64)(left) && (SInt64)(right); }
  #else
    #define S64And(left, right) ((SInt64)(left) && (SInt64)(right))
  #endif
#endif



/*
 *  S64Or()
 *  
 *  Discussion:
 *    Returns one if left or right are non-zero, otherwise returns zero
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
S64Or(
  SInt64   left,
  SInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(Boolean ) S64Or(SInt64 left, SInt64 right) { return (SInt64)(left) || (SInt64)(right); }
  #else
    #define S64Or(left, right) ((SInt64)(left) || (SInt64)(right))
  #endif
#endif



/*
 *  S64Eor()
 *  
 *  Discussion:
 *    Returns one if left xor right are non-zero, otherwise returns zero
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
S64Eor(
  SInt64   left,
  SInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(Boolean ) S64Eor(SInt64 left, SInt64 right) { return (Boolean)(((SInt64)(left) ? 1 : 0) ^ ((SInt64)(right) ? 1 : 0)); }
  #else
    #define S64Eor(left, right) ((Boolean)(((SInt64)(left) ? 1 : 0) ^ ((SInt64)(right) ? 1 : 0)))
  #endif
#endif



/*
 *  S64Not()
 *  
 *  Discussion:
 *    Returns one if value is non-zero, otherwisze returns zero.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
S64Not(SInt64 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(Boolean ) S64Not(SInt64 value) { return !((SInt64)(value)); }
  #else
    #define S64Not(value) (!((SInt64)(value)))
  #endif
#endif



/*
 *  S64Compare()
 *  
 *  Discussion:
 *    Given two signed numbers, left and right, returns an SInt32 that
 *    compares with zero the same way left compares with right.  If you
 *    wanted to perform a comparison on 64-bit integers of the
 *    form:
 *    operand_1 <operation> operand_2
 *    then you could use an expression of the form:
 *     xxxS64Compare(operand_1,operand_2) <operation> 0
 *    to test for the same condition. CAUTION: DO NOT depend on the
 *    exact value returned by this routine. Only the sign (i.e.
 *    positive, zero, or negative) of the result is guaranteed.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt32 )
S64Compare(
  SInt64   left,
  SInt64   right);



/*
 *  S64BitwiseAnd()
 *  
 *  Discussion:
 *    bitwise AND
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64BitwiseAnd(
  SInt64   left,
  SInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64BitwiseAnd(SInt64 left, SInt64 right) { return (SInt64)(left) & (SInt64)(right); }
  #else
    #define S64BitwiseAnd(left, right) ((SInt64)(left) & (SInt64)(right))
  #endif
#endif



/*
 *  S64BitwiseOr()
 *  
 *  Discussion:
 *    bitwise OR
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64BitwiseOr(
  SInt64   left,
  SInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64BitwiseOr(SInt64 left, SInt64 right) { return (SInt64)(left) | (SInt64)(right); }
  #else
    #define S64BitwiseOr(left, right) ((SInt64)(left) | (SInt64)(right))
  #endif
#endif



/*
 *  S64BitwiseEor()
 *  
 *  Discussion:
 *    bitwise XOR
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64BitwiseEor(
  SInt64   left,
  SInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64BitwiseEor(SInt64 left, SInt64 right) { return (SInt64)(left) ^ (SInt64)(right); }
  #else
    #define S64BitwiseEor(left, right) ((SInt64)(left) ^ (SInt64)(right))
  #endif
#endif



/*
 *  S64BitwiseNot()
 *  
 *  Discussion:
 *    bitwise negate
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64BitwiseNot(SInt64 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64BitwiseNot(SInt64 value) { return ~((SInt64)(value)); }
  #else
    #define S64BitwiseNot(value) (~((SInt64)(value)))
  #endif
#endif



/*
 *  S64ShiftRight()
 *  
 *  Discussion:
 *    Arithmetic shift of value by the lower 7 bits of the shift.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64ShiftRight(
  SInt64   value,
  UInt32   shift);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64ShiftRight(SInt64 value, UInt32 shift) { return (SInt64)(value) >> ((shift) & 0x7F); }
  #else
    #define S64ShiftRight(value, shift) ((SInt64)(value) >> ((shift) & 0x7F))
  #endif
#endif



/*
 *  S64ShiftLeft()
 *  
 *  Discussion:
 *    Logical shift of value by the lower 7 bits of the shift.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
S64ShiftLeft(
  SInt64   value,
  UInt32   shift);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) S64ShiftLeft(SInt64 value, UInt32 shift) { return (SInt64)(value) << ((shift) & 0x7F); }
  #else
    #define S64ShiftLeft(value, shift) ((SInt64)(value) << ((shift) & 0x7F))
  #endif
#endif



#if !TYPE_LONGDOUBLE_IS_DOUBLE
/*
 *  SInt64ToLongDouble()
 *  
 *  Discussion:
 *    Converts SInt64 to long double.  Note all SInt64s fit exactly
 *    into long doubles, thus, the binary -> decimal conversion
 *    routines in fp.h can be used to achieve SInt64 -> long double ->
 *    decimal conversions. Note: The function implementation assumes
 *    long double is a 128-bit floating point on PowerPC and 80-bit
 *    type on 68K
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double )
SInt64ToLongDouble(SInt64 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) SInt64ToLongDouble(SInt64 value) { return (long double)(value); }
  #else
    #define SInt64ToLongDouble(value) ((long double)(value))
  #endif
#endif


/*
 *  LongDoubleToSInt64()
 *  
 *  Discussion:
 *    Converts a long double to a SInt64.  Any decimal string that fits
 *    into a SInt64 can be converted exactly into a long double, using
 *    the conversion routines found in fp.h.  Then this routine can be
 *    used to complete the conversion to SInt64. Note: The function
 *    implementation assumes long double is a 128-bit floating point on
 *    PowerPC and 80-bit type on 68K
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( SInt64 )
LongDoubleToSInt64(long double value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) LongDoubleToSInt64(long double value) { return (SInt64)(value); }
  #else
    #define LongDoubleToSInt64(value) ((SInt64)(value))
  #endif
#endif


#endif  /* !TYPE_LONGDOUBLE_IS_DOUBLE */


#if TYPE_LONGLONG && TARGET_OS_WIN32
    #define U64Max() 0xffffffffffffffffui64
#elif TYPE_LONGLONG && defined(__MWERKS__) && (__MWERKS__ < 0x1800)
    #define U64Max() 0xffffffffffffffff
#else
/*
 *  U64Max()
 *  
 *  Discussion:
 *    Returns largest possible UInt64 value
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64Max(void);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64Max(void) { return 0xffffffffffffffffULL; }
  #else
    #define U64Max() (0xffffffffffffffffULL)
  #endif
#endif


#endif
/*
 *  U64Add()
 *  
 *  Discussion:
 *    Adds two unsigned integers, producing an integer result.  If an
 *    overflow occurs the result is congruent mod (2^64) as if the
 *    operands and result were unsigned.  No overflow is signaled.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64Add(
  UInt64   left,
  UInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64Add(UInt64 left, UInt64 right) { return (UInt64)(left) + (UInt64)(right); }
  #else
    #define U64Add(left, right) ((UInt64)(left) + (UInt64)(right))
  #endif
#endif


/*
 *  U64Subtract()
 *  
 *  Discussion:
 *    Subtracts two unsigned integers, producing an integer result.  If
 *    an overflow occurs the result is congruent mod (2^64) as if the
 *    operands and result were unsigned.  No overflow is signaled.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64Subtract(
  UInt64   left,
  UInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64Subtract(UInt64 left, UInt64 right) { return (UInt64)(left) - (UInt64)(right); }
  #else
    #define U64Subtract(left, right) ((UInt64)(left) - (UInt64)(right))
  #endif
#endif



/*
 *  U64Multiply()
 *  
 *  Discussion:
 *    Multiplies two unsigned numbers, producing a signed result. 
 *    Overflow is ignored and the low-order part of the product is
 *    returned.  The sign of the result is not guaranteed to be correct
 *    if the magnitude of the product is not representable.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64Multiply(
  UInt64   left,
  UInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64Multiply(UInt64 left, UInt64 right) { return (UInt64)(left) * (UInt64)(right); }
  #else
    #define U64Multiply(left, right) ((UInt64)(left) * (UInt64)(right))
  #endif
#endif



/*
 *  U64Mod()
 *  
 *  Discussion:
 *    Returns the remainder of divide of dividend by divisor.  The sign
 *    of the remainder is the same as the sign of the dividend (i.e.,
 *    it takes the absolute values of the operands, does the division,
 *    then fixes the sign of the quotient and remainder).
 *  
 *  Availability:
 *    Implemented by client
 */
EXTERN_API_C( UInt64 )
U64Mod(
  UInt64   dividend,
  UInt64   divisor);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64Mod(UInt64 dividend, UInt64 divisor) { return (UInt64)(dividend) % (UInt64)(divisor); }
  #else
    #define U64Mod(dividend, divisor) ((UInt64)(dividend) % (UInt64)(divisor))
  #endif
#endif



/*
 *  U64Divide()
 *  
 *  Discussion:
 *    Divides dividend by divisor, returning the quotient.  The
 *    remainder is returned in *remainder if remainder (the pointer) is
 *    non-NULL. The sign of the remainder is the same as the sign of
 *    the dividend (i.e. it takes the absolute values of the operands,
 *    does the division, then fixes the sign of the quotient and
 *    remainder).  If the divisor is zero, then U64Max() will be
 *    returned (or U64Min() if the dividend is negative), and the
 *    remainder will be the dividend; no error is reported.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64Divide(
  UInt64    dividend,
  UInt64    divisor,
  UInt64 *  remainder);      /* can be NULL */
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64Divide(UInt64 dividend, UInt64 divisor, UInt64 *remainder) { return ( (void)((remainder) && (*((UInt64*)(remainder)) = ((UInt64)(dividend) % (UInt64)(divisor)))), ((UInt64)(dividend) / (UInt64)(divisor)) ); }
  #else
    #define U64Divide(dividend, divisor, remainder) (( (void)((remainder) && (*((UInt64*)(remainder)) = ((UInt64)(dividend) % (UInt64)(divisor)))), ((UInt64)(dividend) / (UInt64)(divisor)) ))
  #endif
#endif





/*
 *  U64Div()
 *  
 *  Discussion:
 *    Divides dividend by divisor, returning the quotient.
 *  
 *  Availability:
 *    Implemented by client
 */
EXTERN_API_C( UInt64 )
U64Div(
  UInt64   dividend,
  UInt64   divisor);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64Div(UInt64 dividend, UInt64 divisor) { return U64Divide((dividend), (divisor), NULL); }
  #else
    #define U64Div(dividend, divisor) (U64Divide((dividend), (divisor), NULL))
  #endif
#endif



/*
 *  U64Set()
 *  
 *  Discussion:
 *    Given an SInt32, returns an UInt64 with the same value.  Use this
 *    routine instead of coding 64-bit constants (at least when the
 *    constant will fit in an SInt32).
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64Set(SInt32 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64Set(SInt32 value) { return (UInt64)(value); }
  #else
    #define U64Set(value) ((UInt64)(value))
  #endif
#endif



/*
 *  U64SetU()
 *  
 *  Discussion:
 *    Given a UInt32, returns a UInt64 with the same value.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64SetU(UInt32 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64SetU(UInt32 value) { return (UInt64)(value); }
  #else
    #define U64SetU(value) ((UInt64)(value))
  #endif
#endif


/*
 *  U32SetU()
 *  
 *  Discussion:
 *    Given an UInt64, returns an UInt32 by discarding the high-order
 *    32 bits.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt32 )
U32SetU(UInt64 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt32 ) U32SetU(UInt64 value) { return (UInt32)(value); }
  #else
    #define U32SetU(value) ((UInt32)(value))
  #endif
#endif



/*
 *  U64And()
 *  
 *  Discussion:
 *    Returns one if left and right are non-zero, otherwise returns zero
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
U64And(
  UInt64   left,
  UInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(Boolean ) U64And(UInt64 left, UInt64 right) { return (UInt64)(left) && (UInt64)(right); }
  #else
    #define U64And(left, right) ((UInt64)(left) && (UInt64)(right))
  #endif
#endif



/*
 *  U64Or()
 *  
 *  Discussion:
 *    Returns one if left or right are non-zero, otherwise returns zero
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
U64Or(
  UInt64   left,
  UInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(Boolean ) U64Or(UInt64 left, UInt64 right) { return (UInt64)(left) || (UInt64)(right); }
  #else
    #define U64Or(left, right) ((UInt64)(left) || (UInt64)(right))
  #endif
#endif



/*
 *  U64Eor()
 *  
 *  Discussion:
 *    Returns one if left xor right are non-zero, otherwise returns zero
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
U64Eor(
  UInt64   left,
  UInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(Boolean ) U64Eor(UInt64 left, UInt64 right) { return (Boolean)(((UInt64)(left) ? 1 : 0) ^ ((UInt64)(right) ? 1 : 0)); }
  #else
    #define U64Eor(left, right) ((Boolean)(((UInt64)(left) ? 1 : 0) ^ ((UInt64)(right) ? 1 : 0)))
  #endif
#endif



/*
 *  U64Not()
 *  
 *  Discussion:
 *    Returns one if value is non-zero, otherwisze returns zero.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
U64Not(UInt64 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(Boolean ) U64Not(UInt64 value) { return !((UInt64)(value)); }
  #else
    #define U64Not(value) (!((UInt64)(value)))
  #endif
#endif



/*
 *  U64Compare()
 *  
 *  Discussion:
 *    Given two unsigned numbers, left and right, returns an SInt32
 *    that compares with zero the same way left compares with right. 
 *    If you wanted to perform a comparison on 64-bit integers of the
 *    form:
 *    operand_1 <operation> operand_2
 *    then you could use an expression of the form:
 *     xxxU64Compare(operand_1,operand_2) <operation> 0
 *    to test for the same condition. CAUTION: DO NOT depend on the
 *    exact value returned by this routine. Only the sign (i.e.
 *    positive, zero, or negative) of the result is guaranteed.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt32 )
U64Compare(
  UInt64   left,
  UInt64   right);


/*
 *  U64BitwiseAnd()
 *  
 *  Discussion:
 *    bitwise AND
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64BitwiseAnd(
  UInt64   left,
  UInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64BitwiseAnd(UInt64 left, UInt64 right) { return (UInt64)(left) & (UInt64)(right); }
  #else
    #define U64BitwiseAnd(left, right) ((UInt64)(left) & (UInt64)(right))
  #endif
#endif



/*
 *  U64BitwiseOr()
 *  
 *  Discussion:
 *    bitwise OR
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64BitwiseOr(
  UInt64   left,
  UInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64BitwiseOr(UInt64 left, UInt64 right) { return (UInt64)(left) | (UInt64)(right); }
  #else
    #define U64BitwiseOr(left, right) ((UInt64)(left) | (UInt64)(right))
  #endif
#endif



/*
 *  U64BitwiseEor()
 *  
 *  Discussion:
 *    bitwise XOR
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64BitwiseEor(
  UInt64   left,
  UInt64   right);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64BitwiseEor(UInt64 left, UInt64 right) { return (UInt64)(left) ^ (UInt64)(right); }
  #else
    #define U64BitwiseEor(left, right) ((UInt64)(left) ^ (UInt64)(right))
  #endif
#endif



/*
 *  U64BitwiseNot()
 *  
 *  Discussion:
 *    bitwise negate
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64BitwiseNot(UInt64 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64BitwiseNot(UInt64 value) { return ~((UInt64)(value)); }
  #else
    #define U64BitwiseNot(value) (~((UInt64)(value)))
  #endif
#endif



/*
 *  U64ShiftRight()
 *  
 *  Discussion:
 *    Arithmetic shift of value by the lower 7 bits of the shift.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64ShiftRight(
  UInt64   value,
  UInt32   shift);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64ShiftRight(UInt64 value, UInt32 shift) { return (UInt64)(value) >> ((shift) & 0x7F); }
  #else
    #define U64ShiftRight(value, shift) ((UInt64)(value) >> ((shift) & 0x7F))
  #endif
#endif



/*
 *  U64ShiftLeft()
 *  
 *  Discussion:
 *    Logical shift of value by the lower 7 bits of the shift.
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
U64ShiftLeft(
  UInt64   value,
  UInt32   shift);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) U64ShiftLeft(UInt64 value, UInt32 shift) { return (UInt64)(value) << ((shift) & 0x7F); }
  #else
    #define U64ShiftLeft(value, shift) ((UInt64)(value) << ((shift) & 0x7F))
  #endif
#endif



#if !TYPE_LONGDOUBLE_IS_DOUBLE
/*
 *  UInt64ToLongDouble()
 *  
 *  Discussion:
 *    Convert an signed 64 bit integer to a long double (128-bit on
 *    PowerPC floating point)
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( long double )
UInt64ToLongDouble(UInt64 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(long double ) UInt64ToLongDouble(UInt64 value) { return (long double)(value); }
  #else
    #define UInt64ToLongDouble(value) ((long double)(value))
  #endif
#endif


/*
 *  LongDoubleToUInt64()
 *  
 *  Discussion:
 *    Convert long double (128-bit on PowerPC floating point) to a
 *    signed 64-bit integer
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( UInt64 )
LongDoubleToUInt64(long double value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) LongDoubleToUInt64(long double value) { return (UInt64)(value); }
  #else
    #define LongDoubleToUInt64(value) ((UInt64)(value))
  #endif
#endif


#endif  /* !TYPE_LONGDOUBLE_IS_DOUBLE */


/*
 *  UInt64ToSInt64()
 *  
 *  Discussion:
 *    converts UInt64 -> SInt64
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( SInt64 )
UInt64ToSInt64(UInt64 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(SInt64 ) UInt64ToSInt64(UInt64 value) { return (SInt64)(value); }
  #else
    #define UInt64ToSInt64(value) ((SInt64)(value))
  #endif
#endif



/*
 *  SInt64ToUInt64()
 *  
 *  Discussion:
 *    converts SInt64 -> UInt64
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt64 )
SInt64ToUInt64(SInt64 value);
#if TYPE_LONGLONG
  #ifdef __cplusplus
    inline DEFINE_API_C(UInt64 ) SInt64ToUInt64(SInt64 value) { return (UInt64)(value); }
  #else
    #define SInt64ToUInt64(value) ((UInt64)(value))
  #endif
#endif






/* 
   Functions to convert between [Unsigned]Wide and [S|U]Int64 types.
  
   These functions are necessary if source code which uses both
   wide and SInt64 is to compile under a compiler that supports
   long long.
 
   SInt64ToWide
   
               Converts a SInt64 to a wide struct.  If SInt64 is implemented
              as a typedef of wide, the macro does nothing. If SInt64 is 
                implemented as a long long, it casts the long long into a 
             wide struct.
   
   WideToSInt64
   
               Converts a wide struct into a SInt64.  If SInt64 is implemented
                as a typedef of wide, the macro does nothing. If SInt64 is 
                implemented as a long long, it reads the struct into a long long.
*/
#if TYPE_LONGLONG 
    #define SInt64ToWide(x)         (*((wide*)(&(x))))
 #define WideToSInt64(x)         (*((SInt64*)(&(x))))
   #define UInt64ToUnsignedWide(x) (*((UnsignedWide*)(&(x))))
 #define UnsignedWideToUInt64(x) (*((UInt64*)(&(x))))
#else
 #define SInt64ToWide(x)         (x)
    #define WideToSInt64(x)         (x)
    #define UInt64ToUnsignedWide(x) (x)
    #define UnsignedWideToUInt64(x) (x)
#endif




#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MATH64__ */

