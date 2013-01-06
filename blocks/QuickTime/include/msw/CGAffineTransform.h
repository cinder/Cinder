/*
     File:       CGAffineTransform.h
 
     Contains:   CoreGraphics affine transforms
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef CGAFFINETRANSFORM_H_
#define CGAFFINETRANSFORM_H_

#ifndef __CGBASE__
#include <CGBase.h>
#endif

#ifndef __CGGEOMETRY__
#include <CGGeometry.h>
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

struct CGAffineTransform {
  float               a;
  float               b;
  float               c;
  float               d;
  float               tx;
  float               ty;
};
typedef struct CGAffineTransform        CGAffineTransform;
/* The identity transform: [ 1 0 0 1 0 0 ]. */
/*
 *  CGAffineTransformIdentity
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern const CGAffineTransform CGAffineTransformIdentity;
/* Return the transform [ a b c d tx ty ]. */
/*
 *  CGAffineTransformMake()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGAffineTransform )
CGAffineTransformMake(
  float   a,
  float   b,
  float   c,
  float   d,
  float   tx,
  float   ty);


/* Return a transform which translates by `(tx, ty)':
 *   t' = [ 1 0 0 1 tx ty ] */
/*
 *  CGAffineTransformMakeTranslation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGAffineTransform )
CGAffineTransformMakeTranslation(
  float   tx,
  float   ty);


/* Return a transform which scales by `(sx, sy)':
 *   t' = [ sx 0 0 sy 0 0 ] */
/*
 *  CGAffineTransformMakeScale()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGAffineTransform )
CGAffineTransformMakeScale(
  float   sx,
  float   sy);


/* Return a transform which rotates by `angle' radians:
 *   t' = [ cos(angle) sin(angle) -sin(angle) cos(angle) 0 0 ] */
/*
 *  CGAffineTransformMakeRotation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGAffineTransform )
CGAffineTransformMakeRotation(float angle);


/* Translate `t' by `(tx, ty)' and return the result:
 *   t' = [ 1 0 0 1 tx ty ] * t */
/*
 *  CGAffineTransformTranslate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGAffineTransform )
CGAffineTransformTranslate(
  CGAffineTransform   t,
  float               tx,
  float               ty);


/* Scale `t' by `(sx, sy)' and return the result:
 *   t' = [ sx 0 0 sy 0 0 ] * t */
/*
 *  CGAffineTransformScale()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGAffineTransform )
CGAffineTransformScale(
  CGAffineTransform   t,
  float               sx,
  float               sy);


/* Rotate `t' by `angle' radians and return the result:
 *   t' =  [ cos(angle) sin(angle) -sin(angle) cos(angle) 0 0 ] * t */
/*
 *  CGAffineTransformRotate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGAffineTransform )
CGAffineTransformRotate(
  CGAffineTransform   t,
  float               angle);


/* Invert `t' and return the result.  If `t' has zero determinant, then `t'
 * is returned unchanged. */
/*
 *  CGAffineTransformInvert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGAffineTransform )
CGAffineTransformInvert(CGAffineTransform t);


/* Concatenate `t2' to `t1' and returne the result:
 *   t' = t1 * t2 */
/*
 *  CGAffineTransformConcat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGAffineTransform )
CGAffineTransformConcat(
  CGAffineTransform   t1,
  CGAffineTransform   t2);


/* Transform `point' by `t' and return the result:
 *   p' = p * t
 * where p = [ x y 1 ]. */
/*
 *  CGPointApplyAffineTransform()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGPoint )
CGPointApplyAffineTransform(
  CGPoint             point,
  CGAffineTransform   t);


/* Transform `size' by `t' and return the result:
 *   s' = s * t
 * where s = [ width height 0 ]. */
/*
 *  CGSizeApplyAffineTransform()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGSize )
CGSizeApplyAffineTransform(
  CGSize              size,
  CGAffineTransform   t);





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

#endif /* CGAFFINETRANSFORM_H_ */

