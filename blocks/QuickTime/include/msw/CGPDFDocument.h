/*
     File:       CGPDFDocument.h
 
     Contains:   CoreGraphics PDF document
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef CGPDFDOCUMENT_H_
#define CGPDFDOCUMENT_H_

#ifndef __CGBASE__
#include <CGBase.h>
#endif

#ifndef __CGDATAPROVIDER__
#include <CGDataProvider.h>
#endif

#ifndef __CGGEOMETRY__
#include <CGGeometry.h>
#endif

#ifndef __CFURL__
#include <CFURL.h>
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

typedef struct CGPDFDocument*           CGPDFDocumentRef;
/* Create a PDF document, using `provider' to obtain the document's
 * data. */
/*
 *  CGPDFDocumentCreateWithProvider()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGPDFDocumentRef )
CGPDFDocumentCreateWithProvider(CGDataProviderRef provider);


/* Create a PDF document from `url'. */
/*
 *  CGPDFDocumentCreateWithURL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGPDFDocumentRef )
CGPDFDocumentCreateWithURL(CFURLRef url);


/* Increment the retain count of `document' and return it.  All PDF
 * documents are created with an initial retain count of 1. */
/*
 *  CGPDFDocumentRetain()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGPDFDocumentRef )
CGPDFDocumentRetain(CGPDFDocumentRef document);


/* Decrement the retain count of `document'.  If the retain count reaches 0,
 * then free it and any associated resources. */
/*
 *  CGPDFDocumentRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGPDFDocumentRelease(CGPDFDocumentRef document);


/* Return the number of pages in `document'. */
/*
 *  CGPDFDocumentGetNumberOfPages()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( int )
CGPDFDocumentGetNumberOfPages(CGPDFDocumentRef document);


/* Return the media box of page number `page' in `document'. */
/*
 *  CGPDFDocumentGetMediaBox()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGPDFDocumentGetMediaBox(
  CGPDFDocumentRef   document,
  int                page);


/* Return the crop box of page number `page' in `document'. */
/*
 *  CGPDFDocumentGetCropBox()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGPDFDocumentGetCropBox(
  CGPDFDocumentRef   document,
  int                page);


/* Return the bleed box of page number `page' in `document'. */
/*
 *  CGPDFDocumentGetBleedBox()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGPDFDocumentGetBleedBox(
  CGPDFDocumentRef   document,
  int                page);


/* Return the trim box of page number `page' in `document'. */
/*
 *  CGPDFDocumentGetTrimBox()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGPDFDocumentGetTrimBox(
  CGPDFDocumentRef   document,
  int                page);


/* Return the art box of page number `page' in `document'. */
/*
 *  CGPDFDocumentGetArtBox()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGRect )
CGPDFDocumentGetArtBox(
  CGPDFDocumentRef   document,
  int                page);


/* Return the rotation angle (in degrees) of page number `page' in
 * `document'. */
/*
 *  CGPDFDocumentGetRotationAngle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( int )
CGPDFDocumentGetRotationAngle(
  CGPDFDocumentRef   document,
  int                page);



#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* CGPDFDOCUMENT_H_ */

