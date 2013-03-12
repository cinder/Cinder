/*
     File:       QDPictToCGContext.h
 
     Contains:   API to draw Quickdraw PICTs into CoreGraphics context
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __QDPICTTOCGCONTEXT__
#define __QDPICTTOCGCONTEXT__

#ifndef __CGCONTEXT__
#include <CGContext.h>
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

typedef struct QDPict*                  QDPictRef;
/*
    Note: QuickDraw picture data typically comes in two forms: a PICT resource
    that begins the picture header data at the beginning of the resource and PICT
    files that begin with 512 bytes of arbitrary data, followed by
    the picture header data. For this reason, the routines that create a QDPictRef
    attempt to find the picture header data beginning at either the first byte
    of the data provided or at byte 513 of the data provided.
    
    Additionally the Picture Bounds must not be an empty rect.
*/
/* Create a QDPict reference, using `provider' to obtain the QDPict's data. 
 * It is assumed that either the first byte or the 513th byte of data
 * in the file referenced by the URL is the first byte of the
 * picture header. If the URL does not begin PICT data at one
 * of these places in the data fork then the QDPictRef returned will be NULL.
*/
/*
 *  QDPictCreateWithProvider()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( QDPictRef )
QDPictCreateWithProvider(CGDataProviderRef provider);


/* Create a QDPict reference from `url'. 
 * It is assumed that either the first byte or the 513th byte of data
 * in the file referenced by the URL is the first byte of the
 * picture header. If the URL does not begin PICT data at one
 * of these places in the data fork then the QDPictRef returned will be NULL.
*/
/*
 *  QDPictCreateWithURL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( QDPictRef )
QDPictCreateWithURL(CFURLRef url);


/* Increment the retain count of `pictRef' and return it.  All 
 * pictRefs are created with an initial retain count of 1. */
/*
 *  QDPictRetain()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( QDPictRef )
QDPictRetain(QDPictRef pictRef);


/* Decrement the retain count of `pictRef'.  If the retain count reaches 0,
 * then free it and any associated resources. */
/*
 *  QDPictRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
QDPictRelease(QDPictRef pictRef);


/* Return the Picture Bounds of the QuickDraw picture represented by `pictRef'. This
    rectangle is in the default user space with one unit = 1/72 inch.
*/
/*
 *  QDPictGetBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( CGRect )
QDPictGetBounds(QDPictRef pictRef);


/* Return the resolution of the QuickDraw picture represented by `pictRef'.
    This data, together with the CGRect returned by QDPictGetBounds, can be
    used to compute the size of the picture in pixels, which is what QuickDraw
    really records into pictures.
*/
/*
 *  QDPictGetResolution()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
QDPictGetResolution(
  QDPictRef   pictRef,
  float *     xRes,
  float *     yRes);


/* Draw `pictRef' in the rectangular area specified by `rect'.
 * The PICT bounds of the page is scaled, if necessary, to fit into
 * `rect'. To get unscaled results, supply a rect the size of the rect
 * returned by QDPictGetBounds.
*/
/*
 *  QDPictDrawToCGContext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
QDPictDrawToCGContext(
  CGContextRef   ctx,
  CGRect         rect,
  QDPictRef      pictRef);



#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __QDPICTTOCGCONTEXT__ */

