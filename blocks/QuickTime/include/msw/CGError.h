/*
     File:       CGError.h
 
     Contains:   CoreGraphics error codes
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/

#ifndef CGERROR_H_
#define CGERROR_H_

#ifndef __CGBASE__
#include <CGBase.h>
#endif




#if PRAGMA_ONCE
#pragma once
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

#if PRAGMA_ENUM_ALWAYSINT
    #if defined(__fourbyteints__) && !__fourbyteints__ 
        #define __CGERROR__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __CGERROR__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

/* Types used for error and error handler */
enum CGError {
  kCGErrorSuccess               = 0,
  kCGErrorFirst                 = 1000,
  kCGErrorFailure               = kCGErrorFirst,
  kCGErrorIllegalArgument       = 1001,
  kCGErrorInvalidConnection     = 1002,
  kCGErrorInvalidContext        = 1003,
  kCGErrorCannotComplete        = 1004,
  kCGErrorNameTooLong           = 1005,
  kCGErrorNotImplemented        = 1006,
  kCGErrorRangeCheck            = 1007,
  kCGErrorTypeCheck             = 1008,
  kCGErrorNoCurrentPoint        = 1009,
  kCGErrorInvalidOperation      = 1010,
  kCGErrorNoneAvailable         = 1011,
  kCGErrorLast                  = kCGErrorNoneAvailable
};
typedef enum CGError CGError;


#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __CGERROR__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__CGERROR__RESTORE_PACKED_ENUMS)
    #pragma options(pack_enums)
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif


#endif /* CGERROR_H_ */

