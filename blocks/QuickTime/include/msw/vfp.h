/*
     File:       vfp.h
 
     Contains:   MathLib style functions for vectors
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __VFP__
#define __VFP__

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

/*
-------------------------------------------------------------------------------
                                                                                
    A collection of numerical functions designed to facilitate a wide         
    range of numerical programming for the Altivec Programming model.       
                                                                                
-------------------------------------------------------------------------------
*/
#ifdef __VEC__
/*
------------------------[ Computational Functions]-----------------------------
                                                                                
    vdivf        C = A . B                                                          
    vsqrtf       B = .A                                                         
    vrsqrtf      B = 1/.A                                                       
                                                                                
-------------------------------------------------------------------------------
*/
/*
 *  vdivf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vdivf(vector float A, vector float B);


/*
 *  vsqrtf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vsqrtf(vector float X);


/*
 *  vrsqrtf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vrsqrtf(vector float X);




/*
--------------------------[ Exponential Functions]-----------------------------
                                                                                
    vexpf       B = Exp(A)                                                      
    vexpm1f     ExpM1(x) = Exp(x) - 1.  But, for small enough arguments,        
                ExpM1(x) is expected to be more accurate than Exp(x) - 1.       
    vlogf       B = Log(A)                                                      
    vlog1pf     Log1P = Log(1 + x). But, for small enough arguments,            
                Log1P is expected to be more accurate than Log(1 + x).          
    vlogbf      Extracts the exponent of its argument, as a signed integral     
                value. A subnormal argument is treated as though it were first  
                normalized. Thus:                                               
                1   <=   x * 2^(-logb(x))   <   2                           
    vscalbf     Computes x * 2^n efficently.  This is not normally done by      
                computing 2^n explicitly.                                       
                                                                                
-------------------------------------------------------------------------------
*/
/*
 *  vexpf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vexpf(vector float X);


/*
 *  vexpm1f()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vexpm1f(vector float X);


/*
 *  vlogf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vlogf(vector float X);


/*
 *  vlog1pf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vlog1pf(vector float X);


/*
 *  vlogbf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vlogbf(vector float X);


/*
 *  vscalbf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vscalbf(vector float X, vector signed int n);




/*
---------------------------[ Auxiliary Functions]------------------------------
                                                                                
    vfabf           Absolute value is part of the programming model, however    
                    completeness it is included in the library.                 
    vcopysignf      Produces a value with the magnitude of its first argument   
                    and sign of its second argument.  NOTE: the order of the    
                    arguments matches the recommendation of the IEEE 754        
                    floating point standard,  which is opposite from the SANE   
                    copysign function.                                          
    vnextafterf     Computes the next representable value after 'x' in the      
                    direction of 'y'.  if x == y, then y is returned.           
                                                                                
-------------------------------------------------------------------------------
*/
/*
 *  vfabf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vfabf(vector float v);


/*
 *  vcopysignf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vcopysignf(vector float arg2, vector float arg1);


/*
 *  vnextafterf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vnextafterf(vector float x, vector float y);




/*
-----------------------------[ Inquiry Functions]------------------------------
                                                                                
    vclassifyf      Returns one of the FP_* values.                             
    vsignbitf       Non-zero if and only if the sign of the argument x is       
                    negative.  This includes, NaNs, infinities and zeros.       
                                                                                
-------------------------------------------------------------------------------
*/
/*
 *  vclassifyf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector unsigned int ) vclassifyf(vector float arg);


/*
 *  vsignbitf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector unsigned int ) vsignbitf(vector float arg);




/*
-------------------------[ Transcendental Functions]----------------------------
                                                                                
    vsinf           B = Sin(A).                                                 
    vcosf           B = Cos(A).                                                 
    vtanf           B = Tan(A).                                                 
                                                                                
-------------------------------------------------------------------------------
*/
/*
 *  vsinf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vsinf(vector float arg);


/*
 *  vcosf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vcosf(vector float arg);


/*
 *  vtanf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vtanf(vector float arg);




/*
-------------------------[ Trigonometric Functions]----------------------------
                                                                                
    vasinf      result is in [-pi/2,pi/2].                                      
    vacosf      result is in [0,pi].                                            
    vatanf      result is in [-pi/2,pi/2].                                      
    vatan2f     Computes the arc tangent of y/x in [-pi,pi] using the sign of   
                both arguments to determine the quadrant of the computed value. 
                                                                                
-------------------------------------------------------------------------------
*/
/*
 *  vasinf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vasinf(vector float arg);


/*
 *  vacosf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vacosf(vector float arg);


/*
 *  vatanf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vatanf(vector float arg);


/*
 *  vatan2f()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vatan2f(vector float arg1, vector float arg2);




/*
--------------------------[ Hyperbolic Functions]------------------------------
                                                                                
    vsinhf      Sine Hyperbolic.                                                
    vcoshf      Cosine Hyperbolic.                                              
    vtanhf      Tangent Hyperbolic.                                             
    vasinhf     Arcsine Hyperbolic.
    vacoshf     Arccosine Hyperbolic.
    vatanhf     Atctangent Hyperbolic.
                                                                                
-------------------------------------------------------------------------------
*/
/*
 *  vsinhf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vsinhf(vector float X);


/*
 *  vcoshf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vcoshf(vector float X);


/*
 *  vtanhf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vtanhf(vector float X);


/*
 *  vasinhf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vasinhf(vector float X);


/*
 *  vacoshf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vacoshf(vector float X);


/*
 *  vatanhf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vatanhf(vector float X);




/*
---------------------------[ Remainder Functions]------------------------------
                                                                                
    vfmodf          B = X mod Y.                                                
    vremainderf     IEEE 754 floating point standard for remainder.             
    vremquof        SANE remainder.  It stores into 'quotient' the 7 low-order  
                    bits of the integer quotient x/y, such that:                
                    -127 <= quotient <= 127.                                
                                                                                
-------------------------------------------------------------------------------
*/
/*
 *  vfmodf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vfmodf(vector float X, vector float Y);


/*
 *  vremainderf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vremainderf(vector float X, vector float Y);


/*
 *  vremquof()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vremquof(vector float X, vector float Y, vector unsigned int *QUO);




/*
------------------------------[ Power Functions]------------------------------
                                                                                
    vipowf          Returns x raised to the integer power of y.                 
    vpowf           Returns x raised to the power of y.  Result is more         
                    accurate than using exp(log(x)*y).                          
                                                                                
-------------------------------------------------------------------------------
*/
/*
 *  vipowf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vipowf(vector float X, vector signed int Y);


/*
 *  vpowf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector float ) vpowf(vector float X, vector float Y);




/*
-------------------------------------------------------------------------------
    Useful                                                                      
-------------------------------------------------------------------------------
*/
/*
 *  vtablelookup()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in vecLib 1.0 and later
 *    CarbonLib:        not in Carbon, but vecLib is compatible with CarbonLib
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( vector unsigned int ) vtablelookup(vector signed int Index_Vect, unsigned long *Table);


#endif  /* defined(__VEC__) */


#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VFP__ */

