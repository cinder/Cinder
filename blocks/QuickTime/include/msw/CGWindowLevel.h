/*
     File:       CGWindowLevel.h
 
     Contains:   CoreGraphics window levels
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/

#ifndef CGWINDOWLEVEL_H_
#define CGWINDOWLEVEL_H_

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
        #define __CGWINDOWLEVEL__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __CGWINDOWLEVEL__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

/*
 * Windows may be assigned to a particular level. When assigned to a level,
 * the window is ordered relative to all other windows in that level.
 * Windows with a higher level are sorted in front of windows with a lower
 * level.
 *
 * A common set of window levels is defined here for use within higher
 * level frameworks.  The levels are accessed via a key and function,
 * so that levels may be changed or adjusted in future releases without
 * breaking binary compatability.
 */
typedef int32_t                         CGWindowLevel;
typedef int32_t                         CGWindowLevelKey;
enum _CGCommonWindowLevelKey {
  kCGBaseWindowLevelKey         = 0,
  kCGMinimumWindowLevelKey      = 1,
  kCGDesktopWindowLevelKey      = 2,
  kCGBackstopMenuLevelKey       = 3,
  kCGNormalWindowLevelKey       = 4,
  kCGFloatingWindowLevelKey     = 5,
  kCGTornOffMenuWindowLevelKey  = 6,
  kCGDockWindowLevelKey         = 7,
  kCGMainMenuWindowLevelKey     = 8,
  kCGStatusWindowLevelKey       = 9,
  kCGModalPanelWindowLevelKey   = 10,
  kCGPopUpMenuWindowLevelKey    = 11,
  kCGDraggingWindowLevelKey     = 12,
  kCGScreenSaverWindowLevelKey  = 13,
  kCGMaximumWindowLevelKey      = 14,
  kCGOverlayWindowLevelKey      = 15,
  kCGHelpWindowLevelKey         = 16,
  kCGUtilityWindowLevelKey      = 17,
  kCGDesktopIconWindowLevelKey  = 18,
  kCGNumberOfWindowLevelKeys    = 19    /* Internal bookkeeping; must be last */
};
typedef enum _CGCommonWindowLevelKey _CGCommonWindowLevelKey;

/*
 *  CGWindowLevelForKey()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGWindowLevel )
CGWindowLevelForKey(CGWindowLevelKey key);


/* number of levels above kCGMaximumWindowLevel reserved for internal use */
enum {
  kCGNumReservedWindowLevels    = 16
};

/* Definitions of older constant values as calls */
#define kCGBaseWindowLevel          CGWindowLevelForKey(kCGBaseWindowLevelKey)  /* LONG_MIN */
#define kCGMinimumWindowLevel       CGWindowLevelForKey(kCGMinimumWindowLevelKey)   /* (kCGBaseWindowLevel + 1) */
#define kCGDesktopWindowLevel       CGWindowLevelForKey(kCGDesktopWindowLevelKey)   /* kCGMinimumWindowLevel */
#define kCGDesktopIconWindowLevel   CGWindowLevelForKey(kCGDesktopIconWindowLevelKey)   /* kCGMinimumWindowLevel + 20 */
#define kCGBackstopMenuLevel        CGWindowLevelForKey(kCGBackstopMenuLevelKey)    /* -20 */
#define kCGNormalWindowLevel        CGWindowLevelForKey(kCGNormalWindowLevelKey)    /* 0 */
#define kCGFloatingWindowLevel      CGWindowLevelForKey(kCGFloatingWindowLevelKey)  /* 3 */
#define kCGTornOffMenuWindowLevel   CGWindowLevelForKey(kCGTornOffMenuWindowLevelKey)   /* 3 */
#define kCGDockWindowLevel          CGWindowLevelForKey(kCGDockWindowLevelKey)  /* 10 */
#define kCGMainMenuWindowLevel      CGWindowLevelForKey(kCGMainMenuWindowLevelKey)  /* 20 */
#define kCGStatusWindowLevel        CGWindowLevelForKey(kCGStatusWindowLevelKey)    /* 21 */
#define kCGModalPanelWindowLevel    CGWindowLevelForKey(kCGModalPanelWindowLevelKey)    /* 8 */
#define kCGPopUpMenuWindowLevel     CGWindowLevelForKey(kCGPopUpMenuWindowLevelKey) /* 101 */
#define kCGDraggingWindowLevel      CGWindowLevelForKey(kCGDraggingWindowLevelKey)  /* 500 */
#define kCGScreenSaverWindowLevel   CGWindowLevelForKey(kCGScreenSaverWindowLevelKey)   /* 1000 */
#define kCGOverlayWindowLevel       CGWindowLevelForKey(kCGOverlayWindowLevelKey)   /* 102 */
#define kCGHelpWindowLevel          CGWindowLevelForKey(kCGHelpWindowLevelKey)  /* 102 */
#define kCGUtilityWindowLevel       CGWindowLevelForKey(kCGUtilityWindowLevelKey)   /* 19 */
#define kCGMaximumWindowLevel       CGWindowLevelForKey(kCGMaximumWindowLevelKey)   /* LONG_MAX - kCGNumReservedWindowLevels */

#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __CGWINDOWLEVEL__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__CGWINDOWLEVEL__RESTORE_PACKED_ENUMS)
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

#endif /* CGWINDOWLEVEL_H_ */

