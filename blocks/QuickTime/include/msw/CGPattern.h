/*
     File:       CGPattern.h
 
     Contains:   CoreGraphics base types
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef CGPATTERN_H_
#define CGPATTERN_H_

#ifndef __CGBASE__
#include <CGBase.h>
#endif

#ifndef __CGCONTEXT__
#include <CGContext.h>
#endif




/* kCGPatternTilingNoDistortion: The pattern cell is not distorted when
 * painted, however the spacing between pattern cells may vary by as much
 * as 1 device pixel.
 *
 * kCGPatternTilingConstantSpacingMinimalDistortion: Pattern cells are
 * spaced consistently, however the pattern cell may be distorted by as
 * much as 1 device pixel when the pattern is painted.
 *
 * kCGPatternTilingConstantSpacing: Pattern cells are spaced consistently
 * as with kCGPatternTilingConstantSpacingMinimalDistortion, however the
 * pattern cell may be distorted additionally to permit a more efficient
 * implementation. */


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
        #define __CGPATTERN__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __CGPATTERN__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

enum CGPatternTiling {
  kCGPatternTilingNoDistortion  = 0,
  kCGPatternTilingConstantSpacingMinimalDistortion = 1,
  kCGPatternTilingConstantSpacing = 2
};
typedef enum CGPatternTiling CGPatternTiling;


/* The drawing of the pattern is delegated to the callbacks.  The callbacks
 * may be called one or many times to draw the pattern.
 *
 * `version' is the version number of the structure passed in as a
 * parameter to the CGPattern creation functions. The structure defined
 * below is version 0.
 *
 * `drawPattern' should draw the pattern in the context `c'. `info' is the
 * parameter originally passed to the CGPattern creation functions.
 *
 * `releaseInfo' is called when the pattern is deallocated. */
typedef CALLBACK_API_C( void , CGDrawPatternProcPtr )(void *info, CGContextRef c);
typedef CALLBACK_API_C( void , CGReleaseInfoProcPtr )(void * info);
struct CGPatternCallbacks {
  unsigned int        version;
  CGDrawPatternProcPtr  drawPattern;
  CGReleaseInfoProcPtr  releaseInfo;
};
typedef struct CGPatternCallbacks       CGPatternCallbacks;
/* Create a pattern. */
/*
 *  CGPatternCreate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( CGPatternRef )
CGPatternCreate(
  void *                      info,
  CGRect                      bounds,
  CGAffineTransform           matrix,
  float                       xStep,
  float                       yStep,
  CGPatternTiling             tiling,
  int                         isColored,
  const CGPatternCallbacks *  callbacks);


/* Increment the retain count of `pattern' and return it.  All patterns are
 * created with an initial retain count of 1. */
/*
 *  CGPatternRetain()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( CGPatternRef )
CGPatternRetain(CGPatternRef pattern);


/* Decrement the retain count of `pattern'.  If the retain count reaches 0,
 * then free it and release any associated resources. */
/*
 *  CGPatternRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
CGPatternRelease(CGPatternRef pattern);



#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __CGPATTERN__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__CGPATTERN__RESTORE_PACKED_ENUMS)
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

#endif /* CGPATTERN_H_ */

