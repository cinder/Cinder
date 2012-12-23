/*
     File:       CGFont.h
 
     Contains:   CoreGraphics font
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/

#ifndef CGFONT_H_
#define CGFONT_H_

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

#if PRAGMA_ENUM_ALWAYSINT
    #if defined(__fourbyteints__) && !__fourbyteints__ 
        #define __CGFONT__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __CGFONT__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

typedef struct CGFont*                  CGFontRef;
typedef unsigned short                  CGGlyph;
/*** Font creation. ***/
/* Create a CGFont using `platformFontReference', a pointer to a
 * platform-specific font reference.  For MacOS X, `platformFontReference'
 * should be a pointer to an ATSFontRef. */
/*
 *  CGFontCreateWithPlatformFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGFontRef )
CGFontCreateWithPlatformFont(void * platformFontReference);


/*** Retain & release. ***/
/* Increment the retain count of `font' and return it.  All fonts are
 * created with an initial retain count of 1. */
/*
 *  CGFontRetain()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGFontRef )
CGFontRetain(CGFontRef font);


/* Decrement the retain count of `font'.  If the retain count reaches 0,
 * then release it and any associated resources. */
/*
 *  CGFontRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
CGFontRelease(CGFontRef font);






#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __CGFONT__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__CGFONT__RESTORE_PACKED_ENUMS)
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

#endif /* CGFONT_H_ */

