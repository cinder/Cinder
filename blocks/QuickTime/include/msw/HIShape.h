/*
     File:       HIShape.h
 
     Contains:   Generic Abstract Shape API
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __HISHAPE__
#define __HISHAPE__

#ifndef __APPLICATIONSERVICES__
#include <ApplicationServices.h>
#endif

#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
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
 *  HIShape
 *  
 *  Discussion:
 *    HIShape is an abstract shape object for use with some of the
 *    HIToolbox APIs. It is designed as a replacement for RgnHandles
 *    (though it is currently implemented in terms of them at the time
 *    of this writing). This abstraction will allow us to avoid using
 *    QD types in our APIs, particularly in HIView. 
 *    
 *    One of the biggest benefits of HIShape is that we have mutable
 *    and immutable variants. This means that immutable shapes can be
 *    created and passed around and 'copied' very quickly, since they
 *    are actually refcounted when copied. This avoids needing to do
 *    the handle-to-handle copies that occur right now with
 *    RgnHandle-based APIs.
 */
typedef const struct __HIShape*         HIShapeRef;
typedef struct __HIShape*               HIMutableShapeRef;


/*
 *  HIShapeGetTypeID()
 *  
 *  Discussion:
 *    Returns the CF type ID for the HIShape class.
 *  
 *  Result:
 *    A CF type ID.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( CFTypeID )
HIShapeGetTypeID(void);





/*======================================================================================*/
/*  IMMUTABLE FUNCTIONS                                                                 */
/*======================================================================================*/
/*
 *  HIShapeCreateWithQDRgn()
 *  
 *  Discussion:
 *    Creates an immutable shape based on an existing Quickdraw region
 *    handle.
 *  
 *  Parameters:
 *    
 *    inRgn:
 *      The Quickdraw region from which to create the HIShape.
 *  
 *  Result:
 *    An immutable HIShape reference.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIShapeRef )
HIShapeCreateWithQDRgn(RgnHandle inRgn);


/*
 *  HIShapeCreateWithRect()
 *  
 *  Discussion:
 *    Creates an immutable, rectangular shape based on a given
 *    rectangle.
 *  
 *  Parameters:
 *    
 *    inRect:
 *      The HIRect from which to create the resulting shape.
 *  
 *  Result:
 *    An immutable HIShape reference.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIShapeRef )
HIShapeCreateWithRect(const HIRect * inRect);


/*
 *  HIShapeCreateCopy()
 *  
 *  Discussion:
 *    Creates an immutable copy of a mutable or immutable HIShape.
 *  
 *  Parameters:
 *    
 *    inShape:
 *      The existing HIShapeRef you wish to copy.
 *  
 *  Result:
 *    An immutable HIShape reference.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIShapeRef )
HIShapeCreateCopy(HIShapeRef inShape);


/*
 *  HIShapeCreateIntersection()
 *  
 *  Discussion:
 *    Creates a new immutable shape which is the intersection of two
 *    others.
 *  
 *  Parameters:
 *    
 *    inShape1:
 *      An existing HIShapeRef.
 *    
 *    inShape2:
 *      An existing HIShapeRef.
 *  
 *  Result:
 *    A new immutable HIShapeRef.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIShapeRef )
HIShapeCreateIntersection(
  HIShapeRef   inShape1,
  HIShapeRef   inShape2);


/*
 *  HIShapeCreateDifference()
 *  
 *  Discussion:
 *    Creates a new immutable shape which is the difference of two
 *    others. The second shape is subtracted from the first.
 *  
 *  Parameters:
 *    
 *    inShape1:
 *      An existing HIShapeRef.
 *    
 *    inShape2:
 *      An existing HIShapeRef.
 *  
 *  Result:
 *    A new immutable HIShapeRef.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIShapeRef )
HIShapeCreateDifference(
  HIShapeRef   inShape1,
  HIShapeRef   inShape2);


/*
 *  HIShapeCreateUnion()
 *  
 *  Discussion:
 *    Creates a new immutable shape which is the union of two others.
 *  
 *  Parameters:
 *    
 *    inShape1:
 *      An existing HIShapeRef.
 *    
 *    inShape2:
 *      An existing HIShapeRef.
 *  
 *  Result:
 *    A new immutable HIShapeRef.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIShapeRef )
HIShapeCreateUnion(
  HIShapeRef   inShape1,
  HIShapeRef   inShape2);


/*
 *  HIShapeIsEmpty()
 *  
 *  Discussion:
 *    Returns true if the given HIShapeRef is empty, i.e. its area is
 *    empty.
 *  
 *  Parameters:
 *    
 *    inShape:
 *      The existing HIShapeRef you wish to test.
 *  
 *  Result:
 *    A boolean result.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIShapeIsEmpty(HIShapeRef inShape);


/*
 *  HIShapeIsRectangular()
 *  
 *  Discussion:
 *    Returns true if the given HIShapeRef is rectangular.
 *  
 *  Parameters:
 *    
 *    inShape:
 *      The existing HIShapeRef you wish to test.
 *  
 *  Result:
 *    A boolean result.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIShapeIsRectangular(HIShapeRef inShape);


/*
 *  HIShapeContainsPoint()
 *  
 *  Discussion:
 *    Returns true if the given HIShapeRef contains the point passed in.
 *  
 *  Parameters:
 *    
 *    inShape:
 *      An existing HIShapeRef.
 *    
 *    inPoint:
 *      The point to check.
 *  
 *  Result:
 *    A boolean result.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIShapeContainsPoint(
  HIShapeRef       inShape,
  const HIPoint *  inPoint);


/*
 *  HIShapeGetBounds()
 *  
 *  Discussion:
 *    Returns the bounding rectangle of a given HIShapeRef.
 *  
 *  Parameters:
 *    
 *    inShape:
 *      An existing HIShapeRef.
 *    
 *    outRect:
 *      Receives the bounding rectangle.
 *  
 *  Result:
 *    A pointer to the rectangle you passed in, for convenience.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIRect * )
HIShapeGetBounds(
  HIShapeRef   inShape,
  HIRect *     outRect);


/*
 *  HIShapeGetAsQDRgn()
 *  
 *  Discussion:
 *    Changes a given Quickdraw region handle to have the same shape as
 *    a given HIShapeRef. Essentially you are converting an HIShapeRef
 *    into a RgnHandle. This conversion may lose fidelity depending on
 *    how the shape was created originally.
 *  
 *  Parameters:
 *    
 *    inShape:
 *      An existing HIShapeRef.
 *    
 *    outRgn:
 *      An existing region to change.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIShapeGetAsQDRgn(
  HIShapeRef   inShape,
  RgnHandle    outRgn);


/*
 *  HIShapeReplacePathInCGContext()
 *  
 *  Discussion:
 *    Given an HIShapeRef and a CGContextRef, make the current path in
 *    the context represent the shape. You might use this to clip to a
 *    shape, for example. You could call this function and then
 *    immediately call CGContextClip.
 *  
 *  Parameters:
 *    
 *    inShape:
 *      An existing HIShapeRef.
 *    
 *    inContext:
 *      The context to apply the shape to.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIShapeReplacePathInCGContext(
  HIShapeRef     inShape,
  CGContextRef   inContext);


/*
 *  HIShapeSetQDClip()
 *  
 *  Discussion:
 *    Given an HIShapeRef and a Quickdraw port, set the current clip in
 *    the port to the shape.
 *  
 *  Parameters:
 *    
 *    inShape:
 *      An existing HIShapeRef.
 *    
 *    inPort:
 *      The port to set the clip for.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIShapeSetQDClip(
  HIShapeRef   inShape,
  CGrafPtr     inPort);




/*======================================================================================*/
/*  MUTABLE FUNCTIONS                                                                   */
/*======================================================================================*/
/*
 *  HIShapeCreateMutable()
 *  
 *  Discussion:
 *    Creates a new, mutable, empty shape.
 *  
 *  Result:
 *    A mutable shape reference.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIMutableShapeRef )
HIShapeCreateMutable(void);


/*
 *  HIShapeCreateMutableCopy()
 *  
 *  Discussion:
 *    Given an existing HIShapeRef, creates a new mutable copy.
 *  
 *  Parameters:
 *    
 *    inOrig:
 *      The shape to copy.
 *  
 *  Result:
 *    A mutable shape reference.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIMutableShapeRef )
HIShapeCreateMutableCopy(HIShapeRef inOrig);


/*
 *  HIShapeSetEmpty()
 *  
 *  Discussion:
 *    Sets a mutable shape to be an empty shape.
 *  
 *  Parameters:
 *    
 *    inShape:
 *      The shape to empty.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIShapeSetEmpty(HIMutableShapeRef inShape);


/*
 *  HIShapeIntersect()
 *  
 *  Discussion:
 *    Takes two shapes and sets a third to be their intersection.
 *  
 *  Parameters:
 *    
 *    inShape1:
 *      The first shape.
 *    
 *    inShape2:
 *      The second shape.
 *    
 *    outResult:
 *      The shape to receive the result of the intersection. This can
 *      be one of the source shapes.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIShapeIntersect(
  HIShapeRef          inShape1,
  HIShapeRef          inShape2,
  HIMutableShapeRef   outResult);


/*
 *  HIShapeDifference()
 *  
 *  Discussion:
 *    Takes two shapes and sets a third to be their difference. The
 *    second shape is subtracted from the first.
 *  
 *  Parameters:
 *    
 *    inShape1:
 *      The first shape.
 *    
 *    inShape2:
 *      The second shape.
 *    
 *    outResult:
 *      The shape to receive the result of the intersection. This can
 *      be one of the source shapes.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIShapeDifference(
  HIShapeRef          inShape1,
  HIShapeRef          inShape2,
  HIMutableShapeRef   outResult);


/*
 *  HIShapeUnion()
 *  
 *  Discussion:
 *    Takes two shapes and sets a third to be their union.
 *  
 *  Parameters:
 *    
 *    inShape1:
 *      The first shape.
 *    
 *    inShape2:
 *      The second shape.
 *    
 *    outResult:
 *      The shape to receive the result of the union. This can be one
 *      of the source shapes.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIShapeUnion(
  HIShapeRef          inShape1,
  HIShapeRef          inShape2,
  HIMutableShapeRef   outResult);


/*
 *  HIShapeOffset()
 *  
 *  Discussion:
 *    Offsets a shape by some delta.
 *  
 *  Parameters:
 *    
 *    inShape:
 *      The shape to offset.
 *    
 *    inDX:
 *      The delta to move the shape on the X axis.
 *    
 *    inDY:
 *      The delta to move the shape on the Y axis.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIShapeOffset(
  HIMutableShapeRef   inShape,
  float               inDX,
  float               inDY);




#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HISHAPE__ */

