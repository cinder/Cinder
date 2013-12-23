/*
     File:       CGPDFContext.h
 
     Contains:   CoreGraphics PDF context
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef CGPDFCONTEXT_H_
#define CGPDFCONTEXT_H_

#ifndef __CGBASE__
#include <CGBase.h>
#endif

#ifndef __CGCONTEXT__
#include <CGContext.h>
#endif

#ifndef __CGDATACONSUMER__
#include <CGDataConsumer.h>
#endif

#ifndef __CFDICTIONARY__
#include <CFDictionary.h>
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

/* Create a PDF context, using `consumer' for output. `mediaBox' is the
 * default page media bounding box; if NULL, then a default page size is
 * used.  `auxiliaryInfo' specifies additional information used by the PDF
 * context when generating the PDF file.  The keys and values in
 * `auxiliaryInfo' must be CFStrings.  The following keys are recognized:
 *   Key       Value
 *   --------  --------
 *   Title     The document's title.
 *   Author    The name of the person who created the document.
 *   Creator   If the document was converted to PDF from another format,
 *             the name of the application that created the original
 *             document from which it was converted. 
 */
/*
 *  CGPDFContextCreate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGContextRef )
CGPDFContextCreate(
  CGDataConsumerRef   consumer,
  const CGRect *      mediaBox,
  CFDictionaryRef     auxiliaryInfo);


/* Convenience function: create a PDF context, writing to `url'. */
/*
 *  CGPDFContextCreateWithURL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGContextRef )
CGPDFContextCreateWithURL(
  CFURLRef          url,
  const CGRect *    mediaBox,
  CFDictionaryRef   auxiliaryInfo);



#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* CGPDFCONTEXT_H_ */

