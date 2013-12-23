/*
     File:       CGGeometry.h
 
     Contains:   CoreGraphics geometry
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef CGGEOMETRY_H_
#define CGGEOMETRY_H_

#ifndef __CGBASE__
#include <CGBase.h>
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

#if PRAGMA_ENUM_ALWAYSINT
    #if defined(__fourbyteints__) && !__fourbyteints__ 
        #define __CGGEOMETRY__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __CGGEOMETRY__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

/* Points. */
struct CGPoint {
  float               x;
  float               y;
};
typedef struct CGPoint                  CGPoint;
/* Sizes. */
struct CGSize {
  float               width;
  float               height;
};
typedef struct CGSize                   CGSize;
/* Rectangles. */
struct CGRect {
  CGPoint             origin;
  CGSize              size;
};
typedef struct CGRect                   CGRect;
/* Rectangle edges. */
enum CGRectEdge {
  CGRectMinXEdge                = 0,
  CGRectMinYEdge                = 1,
  CGRectMaxXEdge                = 2,
  CGRectMaxYEdge                = 3
};
typedef enum CGRectEdge CGRectEdge;

#if TARGET_OS_MAC

/* The "zero" point -- equivalent to CGPointMake(0, 0). */
/*
 *  CGPointZero
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern const CGPoint CGPointZero;
/* The "zero" size -- equivalent to CGSizeMake(0, 0). */
/*
 *  CGSizeZero
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern const CGSize CGSizeZero;
/* The "zero" rectangle -- equivalent to CGRectMake(0, 0, 0, 0). */
/*
 *  CGRectZero
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern const CGRect CGRectZero;
/* The "empty" rect.  This is the rectangle returned when, for example, we
 * intersect two disjoint rectangles.  Note that the null rect is not the
 * same as the zero rect. */
/*
 *  CGRectNull
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
extern const CGRect CGRectNull;

#else

#define CGPointZero CGPointMake(0, 0)
#define CGSizeZero CGSizeMake(0, 0)
#define CGRectZero CGRectMake(0, 0, 0, 0)
#define CGRectNull CGRectMake(INFINITY, INFINITY, 0, 0)

#endif


/* Make a point from `(x, y)'. */
/*
 *  CGPointMake()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGPoint )
CGPointMake(
  float   x,
  float   y);


/* Make a size from `(width, height)'. */
/*
 *  CGSizeMake()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGSize )
CGSizeMake(
  float   width,
  float   height);


/* Make a rect from `(x, y; width, height)'. */
/*
 *  CGRectMake()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGRectMake(
  float   x,
  float   y,
  float   width,
  float   height);


/* Return the leftmost x-value of `rect'. */
/*
 *  CGRectGetMinX()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( float )
CGRectGetMinX(CGRect rect);


/* Return the midpoint x-value of `rect'. */
/*
 *  CGRectGetMidX()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( float )
CGRectGetMidX(CGRect rect);


/* Return the rightmost x-value of `rect'. */
/*
 *  CGRectGetMaxX()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( float )
CGRectGetMaxX(CGRect rect);


/* Return the bottommost y-value of `rect'. */
/*
 *  CGRectGetMinY()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( float )
CGRectGetMinY(CGRect rect);


/* Return the midpoint y-value of `rect'. */
/*
 *  CGRectGetMidY()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( float )
CGRectGetMidY(CGRect rect);


/* Return the topmost y-value of `rect'. */
/*
 *  CGRectGetMaxY()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( float )
CGRectGetMaxY(CGRect rect);


/* Return the width of `rect'. */
/*
 *  CGRectGetWidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( float )
CGRectGetWidth(CGRect rect);


/* Return the height of `rect'. */
/*
 *  CGRectGetHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( float )
CGRectGetHeight(CGRect rect);


/* Return 1 if `point1' and `point2' are the same, 0 otherwise. */
/*
 *  CGPointEqualToPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( int )
CGPointEqualToPoint(
  CGPoint   point1,
  CGPoint   point2);


/* Return 1 if `size1' and `size2' are the same, 0 otherwise. */
/*
 *  CGSizeEqualToSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( int )
CGSizeEqualToSize(
  CGSize   size1,
  CGSize   size2);


/* Return 1 if `rect1' and `rect2' are the same, 0 otherwise. */
/*
 *  CGRectEqualToRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( int )
CGRectEqualToRect(
  CGRect   rect1,
  CGRect   rect2);


/* Standardize `rect' -- i.e., convert it to an equivalent rect which has
 * positive width and height. */
/*
 *  CGRectStandardize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGRectStandardize(CGRect rect);


/* Return 1 if `rect' is empty -- i.e., if it has zero width or height.  A
 * null rect is defined to be empty. */
/*
 *  CGRectIsEmpty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( int )
CGRectIsEmpty(CGRect rect);


/* Return 1 if `rect' is null -- e.g., the result of intersecting two
 * disjoint rectangles is a null rect. */
/*
 *  CGRectIsNull()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( int )
CGRectIsNull(CGRect rect);


/* Inset `rect' by `(dx, dy)' -- i.e., offset its origin by `(dx, dy)', and
 * decrease its size by `(2*dx, 2*dy)'. */
/*
 *  CGRectInset()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGRectInset(
  CGRect   rect,
  float    dx,
  float    dy);


/* Expand `rect' to the smallest rect containing it with integral origin
 * and size. */
/*
 *  CGRectIntegral()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGRectIntegral(CGRect rect);


/* Return the union of `r1' and `r2'. */
/*
 *  CGRectUnion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGRectUnion(
  CGRect   r1,
  CGRect   r2);


/* Return the intersection of `r1' and `r2'.  This may return a null
 * rect. */
/*
 *  CGRectIntersection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGRectIntersection(
  CGRect   r1,
  CGRect   r2);


/* Offset `rect' by `(dx, dy)'. */
/*
 *  CGRectOffset()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGRectOffset(
  CGRect   rect,
  float    dx,
  float    dy);


/* Make two new rectangles, `slice' and `remainder', by dividing `rect'
 * with a line that's parallel to one of its sides, specified by `edge' --
 * either `CGRectMinXEdge', `CGRectMinYEdge', `CGRectMaxXEdge', or
 * `CGRectMaxYEdge'.  The size of `slice' is determined by `amount', which
 * measures the distance from the specified edge. */
/*
 *  CGRectDivide()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGRectDivide(
  CGRect       rect,
  CGRect *     slice,
  CGRect *     remainder,
  float        amount,
  CGRectEdge   edge);


/* Return 1 if `point' is contained in `rect', 0 otherwise. */
/*
 *  CGRectContainsPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( int )
CGRectContainsPoint(
  CGRect    rect,
  CGPoint   point);


/* Return 1 if `rect2' is contained in `rect1', 0 otherwise.  `rect2' is
 * contained in `rect1' if the union of `rect1' and `rect2' is equal to
 * `rect1'. */
/*
 *  CGRectContainsRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( int )
CGRectContainsRect(
  CGRect   rect1,
  CGRect   rect2);


/* Return 1 if `rect1' intersects `rect2', 0 otherwise.  `rect1' intersects
 * `rect2' if the intersection of `rect1' and `rect2' is not the null
 * rect. */
/*
 *  CGRectIntersectsRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( int )
CGRectIntersectsRect(
  CGRect   rect1,
  CGRect   rect2);





#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __CGGEOMETRY__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__CGGEOMETRY__RESTORE_PACKED_ENUMS)
    #pragma options(pack_enums)
#endif

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

#endif /* CGGEOMETRY_H_ */

