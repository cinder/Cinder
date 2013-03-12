/*
     File:       CGContext.h
 
     Contains:   CoreGraphics context
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef CGCONTEXT_H_
#define CGCONTEXT_H_

#ifndef __CGBASE__
#include <CGBase.h>
#endif

#ifndef __CGAFFINETRANSFORM__
#include <CGAffineTransform.h>
#endif

#ifndef __CGCOLORSPACE__
#include <CGColorSpace.h>
#endif

#ifndef __CGFONT__
#include <CGFont.h>
#endif

#ifndef __CGIMAGE__
#include <CGImage.h>
#endif

#ifndef __CGPDFDOCUMENT__
#include <CGPDFDocument.h>
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

#if PRAGMA_ENUM_ALWAYSINT
    #if defined(__fourbyteints__) && !__fourbyteints__ 
        #define __CGCONTEXT__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __CGCONTEXT__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

typedef struct CGContext*               CGContextRef;
typedef struct CGPattern*               CGPatternRef;
/* Line join styles. */
enum CGLineJoin {
  kCGLineJoinMiter              = 0,
  kCGLineJoinRound              = 1,
  kCGLineJoinBevel              = 2
};
typedef enum CGLineJoin CGLineJoin;

/* Line cap styles. */
enum CGLineCap {
  kCGLineCapButt                = 0,
  kCGLineCapRound               = 1,
  kCGLineCapSquare              = 2
};
typedef enum CGLineCap CGLineCap;

/* Drawing modes for paths. */
enum CGPathDrawingMode {
  kCGPathFill                   = 0,
  kCGPathEOFill                 = 1,
  kCGPathStroke                 = 2,
  kCGPathFillStroke             = 3,
  kCGPathEOFillStroke           = 4
};
typedef enum CGPathDrawingMode CGPathDrawingMode;

/* Drawing modes for text. */
enum CGTextDrawingMode {
  kCGTextFill                   = 0,
  kCGTextStroke                 = 1,
  kCGTextFillStroke             = 2,
  kCGTextInvisible              = 3,
  kCGTextFillClip               = 4,
  kCGTextStrokeClip             = 5,
  kCGTextFillStrokeClip         = 6,
  kCGTextClip                   = 7
};
typedef enum CGTextDrawingMode CGTextDrawingMode;

/* Text encodings. */
enum CGTextEncoding {
  kCGEncodingFontSpecific       = 0,
  kCGEncodingMacRoman           = 1
};
typedef enum CGTextEncoding CGTextEncoding;

enum CGInterpolationQuality {
  kCGInterpolationDefault       = 0,    /* Let the context decide. */
  kCGInterpolationNone          = 1,    /* Never interpolate. */
  kCGInterpolationLow           = 2,    /* Fast, low quality. */
  kCGInterpolationHigh          = 3     /* Slow, high quality. */
};
typedef enum CGInterpolationQuality CGInterpolationQuality;


/** Graphics state functions. **/
/* Push a copy of the current graphics state onto the graphics state
 * stack. Note that the path is not considered part of the gstate, and is
 * not saved. */
/*
 *  CGContextSaveGState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSaveGState(CGContextRef ctx);


/* Restore the current graphics state from the one on the top of the
 * graphics state stack, popping the graphics state stack in the
 * process. */
/*
 *  CGContextRestoreGState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextRestoreGState(CGContextRef ctx);


/** Coordinate space transformations. **/
/* Scale the current graphics state's transformation matrix (the CTM) by
 * `(sx, sy)'. */
/*
 *  CGContextScaleCTM()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextScaleCTM(
  CGContextRef   ctx,
  float          sx,
  float          sy);


/* Translate the current graphics state's transformation matrix (the CTM)
 * by `(tx, ty)'. */
/*
 *  CGContextTranslateCTM()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextTranslateCTM(
  CGContextRef   ctx,
  float          tx,
  float          ty);


/* Rotate the current graphics state's transformation matrix (the CTM) by
 * `angle' radians. */
/*
 *  CGContextRotateCTM()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextRotateCTM(
  CGContextRef   ctx,
  float          angle);


/* Concatenate the current graphics state's transformation matrix (the CTM)
 * with the affine transform `transform'. */
/*
 *  CGContextConcatCTM()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextConcatCTM(
  CGContextRef        ctx,
  CGAffineTransform   transform);


/* Return the current graphics state's transformation matrix. */
/*
 *  CGContextGetCTM()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGAffineTransform )
CGContextGetCTM(CGContextRef ctx);


/** Drawing attribute functions. **/
/* Set the line width in the current graphics state to `width'. */
/*
 *  CGContextSetLineWidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetLineWidth(
  CGContextRef   ctx,
  float          width);


/* Set the line cap in the current graphics state to `cap'. */
/*
 *  CGContextSetLineCap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetLineCap(
  CGContextRef   ctx,
  CGLineCap      cap);


/* Set the line join in the current graphics state to `join'. */
/*
 *  CGContextSetLineJoin()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetLineJoin(
  CGContextRef   ctx,
  CGLineJoin     join);


/* Set the miter limit in the current graphics state to `limit'. */
/*
 *  CGContextSetMiterLimit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetMiterLimit(
  CGContextRef   ctx,
  float          limit);


/* Set the line dash patttern in the current graphics state. */
/*
 *  CGContextSetLineDash()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetLineDash(
  CGContextRef   ctx,
  float          phase,
  const float    lengths[],
  size_t         count);


/* Set the path flatness parameter in the current graphics state to
 * `flatness'. */
/*
 *  CGContextSetFlatness()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetFlatness(
  CGContextRef   ctx,
  float          flatness);


/* Set the alpha value in the current graphics state to `alpha'. */
/*
 *  CGContextSetAlpha()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetAlpha(
  CGContextRef   ctx,
  float          alpha);


/** Path construction functions. **/
/* Note that a context has a single path in use at any time: a path is not
 * part of the graphics state. */
/* Begin a new path.  The old path is discarded. */
/*
 *  CGContextBeginPath()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextBeginPath(CGContextRef ctx);


/* Start a new subpath at point `(x, y)' in the context's path. */
/*
 *  CGContextMoveToPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextMoveToPoint(
  CGContextRef   ctx,
  float          x,
  float          y);


/* Append a straight line segment from the current point to `(x, y)'. */
/*
 *  CGContextAddLineToPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextAddLineToPoint(
  CGContextRef   ctx,
  float          x,
  float          y);


/* Append a cubic Bezier curve from the current point to `(x,y)', with
 * control points `(cp1x, cp1y)' and `(cp2x, cp2y)'. */
/*
 *  CGContextAddCurveToPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextAddCurveToPoint(
  CGContextRef   ctx,
  float          cp1x,
  float          cp1y,
  float          cp2x,
  float          cp2y,
  float          x,
  float          y);


/* Append a quadratic curve from the current point to `(x, y)', with
 * control point `(cpx, cpy)'. */
/*
 *  CGContextAddQuadCurveToPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextAddQuadCurveToPoint(
  CGContextRef   ctx,
  float          cpx,
  float          cpy,
  float          x,
  float          y);


/* Close the current subpath of the context's path. */
/*
 *  CGContextClosePath()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextClosePath(CGContextRef ctx);


/** Path construction convenience functions. **/
/* Add a single rect to the context's path. */
/*
 *  CGContextAddRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextAddRect(
  CGContextRef   ctx,
  CGRect         rect);


/* Add a set of rects to the context's path. */
/*
 *  CGContextAddRects()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextAddRects(
  CGContextRef   ctx,
  const CGRect   rects[],
  size_t         count);


/* Add a set of lines to the context's path. */
/*
 *  CGContextAddLines()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextAddLines(
  CGContextRef    ctx,
  const CGPoint   points[],
  size_t          count);


/* Add an arc of a circle to the context's path, possibly preceded by a
 * straight line segment.  `(x, y)' is the center of the arc; `radius' is
 * its radius; `startAngle' is the angle to the first endpoint of the arc;
 * `endAngle' is the angle to the second endpoint of the arc; and
 * `clockwise' is 1 if the arc is to be drawn clockwise, 0 otherwise.
 * `startAngle' and `endAngle' are measured in radians. */
/*
 *  CGContextAddArc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextAddArc(
  CGContextRef   ctx,
  float          x,
  float          y,
  float          radius,
  float          startAngle,
  float          endAngle,
  int            clockwise);


/* Add an arc of a circle to the context's path, possibly preceded by a
 * straight line segment.  `radius' is the radius of the arc.  The arc is
 * tangent to the line from the current point to `(x1, y1)', and the line
 * from `(x1, y1)' to `(x2, y2)'. */
/*
 *  CGContextAddArcToPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextAddArcToPoint(
  CGContextRef   ctx,
  float          x1,
  float          y1,
  float          x2,
  float          y2,
  float          radius);


/** Path information functions. **/
/* Return 1 if the context's path contains no elements, 0 otherwise. */
/*
 *  CGContextIsPathEmpty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( int )
CGContextIsPathEmpty(CGContextRef ctx);


/* Return the current point of the current subpath of the context's
 * path. */
/*
 *  CGContextGetPathCurrentPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGPoint )
CGContextGetPathCurrentPoint(CGContextRef ctx);


/* Return the bounding box of the context's path.  The bounding box is the
 * smallest rectangle completely enclosing all points in the path,
 * including control points for Bezier and quadratic curves. */
/*
 *  CGContextGetPathBoundingBox()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGContextGetPathBoundingBox(CGContextRef ctx);


/** Path drawing functions. **/
/* Draw the context's path using drawing mode `mode'. */
/*
 *  CGContextDrawPath()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextDrawPath(
  CGContextRef        ctx,
  CGPathDrawingMode   mode);


/** Path drawing convenience functions. **/
/* Fill the context's path using the winding-number fill rule.  Any open
 * subpath of the path is implicitly closed. */
/*
 *  CGContextFillPath()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextFillPath(CGContextRef ctx);


/* Fill the context's path using the even-odd fill rule.  Any open subpath
 * of the path is implicitly closed. */
/*
 *  CGContextEOFillPath()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextEOFillPath(CGContextRef ctx);


/* Stroke the context's path. */
/*
 *  CGContextStrokePath()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextStrokePath(CGContextRef ctx);


/* Fill `rect' with the current fill color. */
/*
 *  CGContextFillRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextFillRect(
  CGContextRef   ctx,
  CGRect         rect);


/* Fill `rects', an array of `count' CGRects, with the current fill
 * color. */
/*
 *  CGContextFillRects()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextFillRects(
  CGContextRef   ctx,
  const CGRect   rects[],
  size_t         count);


/* Stroke `rect' with the current stroke color and the current linewidth. */
/*
 *  CGContextStrokeRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextStrokeRect(
  CGContextRef   ctx,
  CGRect         rect);


/* Stroke `rect' with the current stroke color, using `width' as the the
 * line width. */
/*
 *  CGContextStrokeRectWithWidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextStrokeRectWithWidth(
  CGContextRef   ctx,
  CGRect         rect,
  float          width);


/* Clear `rect' (that is, set the region within the rect to
 * transparent). */
/*
 *  CGContextClearRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextClearRect(
  CGContextRef   c,
  CGRect         rect);


/** Clipping functions. **/
/* Intersect the context's path with the current clip path and use the
 * resulting path as the clip path for subsequent rendering operations.
 * Use the winding-number fill rule for deciding what's inside the path. */
/*
 *  CGContextClip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextClip(CGContextRef ctx);


/* Intersect the context's path with the current clip path and use the
 * resulting path as the clip path for subsequent rendering operations.
 * Use the even-odd fill rule for deciding what's inside the path. */
/*
 *  CGContextEOClip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextEOClip(CGContextRef ctx);


/** Clipping convenience functions. **/
/* Intersect the current clipping path with `rect'.  Note that this
 * function resets the context's path to the empty path. */
/*
 *  CGContextClipToRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextClipToRect(
  CGContextRef   ctx,
  CGRect         rect);


/* Intersect the current clipping path with the clipping region formed by
 * creating a path consisting of all rects in `rects'.  Note that this
 * function resets the context's path to the empty path. */
/*
 *  CGContextClipToRects()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextClipToRects(
  CGContextRef   ctx,
  const CGRect   rects[],
  size_t         count);


/** Colorspace functions. **/
/* Set the components of the current fill color in the context `ctx' to the
 * values specifed by `components'.  The number of elements in `components'
 * must be one greater than the number of components in the current fill
 * colorspace (N color components + 1 alpha component).  The current fill
 * colorspace must not be a pattern colorspace. */
/*
 *  CGContextSetFillColorSpace()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetFillColorSpace(
  CGContextRef      ctx,
  CGColorSpaceRef   colorspace);


/* Set the components of the current fill color in the context `ctx' to the
 * values specifed by `components'.  The number of elements in `components'
 * must be one greater than the number of components in the current fill
 * colorspace (N color components + 1 alpha component).  The current fill
 * colorspace must not be a pattern colorspace. */
/*
 *  CGContextSetStrokeColorSpace()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetStrokeColorSpace(
  CGContextRef      ctx,
  CGColorSpaceRef   colorspace);


/** Color functions. **/
/* Set the components of the current fill color in the context `c' to the
 * values specifed by `components'.  The number of elements in `components'
 * must be one greater than the number of components in the current fill
 * colorspace (N color components + 1 alpha component).  The current fill
 * colorspace must not be a pattern colorspace. */
/*
 *  CGContextSetFillColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetFillColor(
  CGContextRef   c,
  const float    components[]);


/* Set the components of the current stroke color in the context `c' to the
 * values specifed by `components'.  The number of elements in `components'
 * must be one greater than the number of components in the current stroke
 * colorspace (N color components + 1 alpha component).  The current stroke
 * colorspace must not be a pattern colorspace. */
/*
 *  CGContextSetStrokeColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetStrokeColor(
  CGContextRef   c,
  const float    components[]);


/** Pattern functions. **/
/* Set the components of the current fill color in the context `c' to the
 * values specifed by `components', and set the current fill pattern to
 * `pattern'.  The number of elements in `components' must be one greater
 * than the number of components in the current fill colorspace (N color
 * components + 1 alpha component).  The current fill colorspace must be a
 * pattern colorspace. */
/*
 *  CGContextSetFillPattern()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
CGContextSetFillPattern(
  CGContextRef   c,
  CGPatternRef   pattern,
  const float    components[]);


/* Set the components of the current stroke color in the context `c' to the
 * values specifed by `components', and set the current stroke pattern to
 * `pattern'.  The number of elements in `components' must be one greater
 * than the number of components in the current stroke colorspace (N color
 * components + 1 alpha component).  The current stroke colorspace must be
 * a pattern colorspace. */
/*
 *  CGContextSetStrokePattern()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
CGContextSetStrokePattern(
  CGContextRef   c,
  CGPatternRef   pattern,
  const float    components[]);



/** Color convenience functions. **/
/* Set the current fill colorspace in the context `c' to `DeviceGray' and
 * set the components of the current fill color to `(gray, alpha)'. */
/*
 *  CGContextSetGrayFillColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetGrayFillColor(
  CGContextRef   ctx,
  float          gray,
  float          alpha);


/* Set the current stroke colorspace in the context `c' to `DeviceGray' and
 * set the components of the current stroke color to `(gray, alpha)'. */
/*
 *  CGContextSetGrayStrokeColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetGrayStrokeColor(
  CGContextRef   ctx,
  float          gray,
  float          alpha);


/* Set the current fill colorspace in the context `c' to `DeviceRGB' and
 * set the components of the current fill color to `(red, green, blue,
 * alpha)'. */
/*
 *  CGContextSetRGBFillColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetRGBFillColor(
  CGContextRef   ctx,
  float          red,
  float          green,
  float          blue,
  float          alpha);


/* Set the current stroke colorspace in the context `c' to `DeviceRGB' and
 * set the components of the current stroke color to `(red, green, blue,
 * alpha)'. */
/*
 *  CGContextSetRGBStrokeColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetRGBStrokeColor(
  CGContextRef   ctx,
  float          red,
  float          green,
  float          blue,
  float          alpha);


/* Set the current fill colorspace in the context `c' to `DeviceCMYK' and
 * set the components of the current fill color to `(cyan, magenta, yellow,
 * black, alpha)'. */
/*
 *  CGContextSetCMYKFillColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetCMYKFillColor(
  CGContextRef   ctx,
  float          cyan,
  float          magenta,
  float          yellow,
  float          black,
  float          alpha);


/* Set the current stroke colorspace in the context `c' to `DeviceCMYK' and
 * set the components of the current stroke color to `(cyan, magenta,
 * yellow, black, alpha)'. */
/*
 *  CGContextSetCMYKStrokeColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetCMYKStrokeColor(
  CGContextRef   ctx,
  float          cyan,
  float          magenta,
  float          yellow,
  float          black,
  float          alpha);


/** Rendering intent. **/
/* Set the rendering intent in the graphics state to `intent'. */
/*
 *  CGContextSetRenderingIntent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetRenderingIntent(
  CGContextRef             c,
  CGColorRenderingIntent   intent);


/** Image functions. **/
/* Draw `image' in the rectangular area specified by `rect'.  The image is
 * scaled, if necessary, to fit into `rect'. */
/*
 *  CGContextDrawImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextDrawImage(
  CGContextRef   ctx,
  CGRect         rect,
  CGImageRef     image);


/* Return the interpolation quality for image rendering of the context `c'.
 * The interpolation quality is a gstate-parameter which controls the level
 * of interpolation performed when an image is interpolated (for example,
 * when scaling the image). Note that it is merely a hint to the context:
 * not all contexts support all interpolation quality levels. */
/*
 *  CGContextGetInterpolationQuality()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( CGInterpolationQuality )
CGContextGetInterpolationQuality(CGContextRef c);


/* Set the interpolation quality of the context `c' to `quality'. */
/*
 *  CGContextSetInterpolationQuality()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
CGContextSetInterpolationQuality(
  CGContextRef             c,
  CGInterpolationQuality   quality);



/** Text functions. **/
/* Set the current character spacing in the context `ctx' to `spacing'.  The
 * character spacing is added to the displacement between the origin of one
 * character and the origin of the next. */
/*
 *  CGContextSetCharacterSpacing()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetCharacterSpacing(
  CGContextRef   ctx,
  float          spacing);


/* Set the user-space point at which text will be drawn to (x,y). */
/*
 *  CGContextSetTextPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetTextPosition(
  CGContextRef   ctx,
  float          x,
  float          y);


/* Return the current user-space point at which text will be drawn to (x,y). */
/*
 *  CGContextGetTextPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGPoint )
CGContextGetTextPosition(CGContextRef ctx);


/* Set the text matrix to `transform'. */
/*
 *  CGContextSetTextMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetTextMatrix(
  CGContextRef        ctx,
  CGAffineTransform   transform);


/* Return the text matrix. */
/*
 *  CGContextGetTextMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGAffineTransform )
CGContextGetTextMatrix(CGContextRef ctx);


/* Set the text drawing mode to `mode'. */
/*
 *  CGContextSetTextDrawingMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetTextDrawingMode(
  CGContextRef        ctx,
  CGTextDrawingMode   mode);


/* Set the current font to `font'. */
/*
 *  CGContextSetFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetFont(
  CGContextRef   ctx,
  CGFontRef      font);


/* Set the current font size to `size'. */
/*
 *  CGContextSetFontSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetFontSize(
  CGContextRef   ctx,
  float          size);



/* Attempts to find the font named `name'.  If successful, scales it to
 * `size' units in user space.  `textEncoding' specifies how to translate
 * from bytes to glyphs. */
/*
 *  CGContextSelectFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSelectFont(
  CGContextRef     ctx,
  const char *     name,
  float            size,
  CGTextEncoding   textEncoding);


/* Draw `string', a string of `length' bytes, at the point specified by the
 * current text matrix.  Each byte of the string is mapped through the
 * encoding vector of the current font to obtain the glyph to display. */
/*
 *  CGContextShowText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextShowText(
  CGContextRef   ctx,
  const char *   cstring,
  size_t         length);


/* Draw the glyphs pointed to by `g', an array of `count' glyphs, at the
 * point specified by the current text matrix. */
/*
 *  CGContextShowGlyphs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextShowGlyphs(
  CGContextRef    ctx,
  const CGGlyph   g[],
  size_t          count);



/** Text convenience functions. **/
/* Draw `string', a string of `length' bytes, at the point `(x, y)',
 * specified in user space.  Each byte of the string is mapped through the
 * encoding vector of the current font to obtain the glyph to display. */
/*
 *  CGContextShowTextAtPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextShowTextAtPoint(
  CGContextRef   ctx,
  float          x,
  float          y,
  const char *   cstring,
  size_t         length);


/* Display the glyphs pointed to by `g', an array of `count' glyph ids, at
 * the point `(x, y)', specified in user space. */
/*
 *  CGContextShowGlyphsAtPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextShowGlyphsAtPoint(
  CGContextRef    ctx,
  float           x,
  float           y,
  const CGGlyph   g[],
  size_t          count);



/** PDF document functions. **/
/* Draw `page' in `document' in the rectangular area specified by `rect'.
 * The media box of the page is scaled, if necessary, to fit into
 * `rect'. */
/*
 *  CGContextDrawPDFDocument()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextDrawPDFDocument(
  CGContextRef       ctx,
  CGRect             rect,
  CGPDFDocumentRef   document,
  int                page);


/** Page functions. **/
/* Begin a new page. */
/*
 *  CGContextBeginPage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextBeginPage(
  CGContextRef    ctx,
  const CGRect *  mediaBox);


/* End the current page. */
/*
 *  CGContextEndPage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextEndPage(CGContextRef ctx);


/** Context functions. **/
/* Increment the retain count of `ctx' and return it.  All contexts are
 * created with an initial retain count of 1. */
/*
 *  CGContextRetain()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGContextRef )
CGContextRetain(CGContextRef ctx);


/* Decrement the retain count of `ctx'.  If the retain count reaches 0,
 * then free `ctx' and any associated resources. */
/*
 *  CGContextRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextRelease(CGContextRef ctx);


/* Flush all drawing to the destination. */
/*
 *  CGContextFlush()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextFlush(CGContextRef ctx);


/* Synchronized drawing. */
/*
 *  CGContextSynchronize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSynchronize(CGContextRef ctx);


/** Antialiasing functions. **/
/* Turn off antialiasing if `shouldAntialias' is zero; turn it on
 * otherwise.  This parameter is part of the graphics state. */
/*
 *  CGContextSetShouldAntialias()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGContextSetShouldAntialias(
  CGContextRef   ctx,
  int            shouldAntialias);



#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __CGCONTEXT__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__CGCONTEXT__RESTORE_PACKED_ENUMS)
    #pragma options(pack_enums)
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* CGCONTEXT_H_ */

