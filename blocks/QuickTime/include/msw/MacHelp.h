/*
     File:       MacHelp.h
 
     Contains:   Macintosh Help Package Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1998-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __MACHELP__
#define __MACHELP__

#ifndef __BALLOONS__
#include <Balloons.h>
#endif

#ifndef __CONTROLS__
#include <Controls.h>
#endif

#ifndef __DIALOGS__
#include <Dialogs.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
#endif

#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

#ifndef __MENUS__
#include <Menus.h>
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

/*----------------------------------------------------------------------------------*/
/* Help Manager constants, etc.                                                     */
/*----------------------------------------------------------------------------------*/
enum {
  kMacHelpVersion               = 0x0003
};

typedef SInt16 HMContentRequest;
enum {
  kHMSupplyContent              = 0,
  kHMDisposeContent             = 1
};

typedef UInt32 HMContentType;
enum {
  kHMNoContent                  = FOUR_CHAR_CODE('none'),
  kHMCFStringContent            = FOUR_CHAR_CODE('cfst'), /* CFStringRef*/
  kHMCFStringLocalizedContent   = FOUR_CHAR_CODE('cfsl'), /* CFStringRef; name of a localized string. Supported by Jaguar and later.*/
  kHMPascalStrContent           = FOUR_CHAR_CODE('pstr'),
  kHMStringResContent           = FOUR_CHAR_CODE('str#'),
  kHMTEHandleContent            = FOUR_CHAR_CODE('txth'), /* Supported by CarbonLib and Jaguar and later*/
  kHMTextResContent             = FOUR_CHAR_CODE('text'), /* Supported by CarbonLib and Jaguar and later*/
  kHMStrResContent              = FOUR_CHAR_CODE('str ')
};


/*
 *  HMTagDisplaySide
 *  
 *  Discussion:
 *    Help tag display locations relative to absolute hot rect
 */
typedef SInt16 HMTagDisplaySide;
enum {

  /*
   * System default location
   */
  kHMDefaultSide                = 0,

  /*
   * Above, aligned with left or right depending on system script
   */
  kHMOutsideTopScriptAligned    = 1,

  /*
   * To the left, centered vertically
   */
  kHMOutsideLeftCenterAligned   = 2,
  kHMOutsideBottomScriptAligned = 3,

  /*
   * To the right, centered vertically
   */
  kHMOutsideRightCenterAligned  = 4,

  /*
   * Above, aligned with left
   */
  kHMOutsideTopLeftAligned      = 5,

  /*
   * Above, aligned with right
   */
  kHMOutsideTopRightAligned     = 6,

  /*
   * To the left, aligned with top
   */
  kHMOutsideLeftTopAligned      = 7,

  /*
   * To the left, aligned with bottom
   */
  kHMOutsideLeftBottomAligned   = 8,

  /*
   * To the right, aligned with top
   */
  kHMOutsideBottomLeftAligned   = 9,

  /*
   * To the right, aligned with bottom
   */
  kHMOutsideBottomRightAligned  = 10,
  kHMOutsideRightTopAligned     = 11,
  kHMOutsideRightBottomAligned  = 12,

  /*
   * Above, centered horizontally
   */
  kHMOutsideTopCenterAligned    = 13,

  /*
   * Below, centered horizontally
   */
  kHMOutsideBottomCenterAligned = 14,

  /*
   * Inside, aligned with right, centered vertically
   */
  kHMInsideRightCenterAligned   = 15,

  /*
   * Inside, aligned with left, centered vertically
   */
  kHMInsideLeftCenterAligned    = 16,

  /*
   * Inside, aligned with bottom, centered horizontally
   */
  kHMInsideBottomCenterAligned  = 17,

  /*
   * Inside, aligned with top, centered horizontally
   */
  kHMInsideTopCenterAligned     = 18,

  /*
   * Inside, aligned with top and left
   */
  kHMInsideTopLeftCorner        = 19,

  /*
   * Inside, aligned with top and right
   */
  kHMInsideTopRightCorner       = 20,

  /*
   * Inside, aligned with bottom and left
   */
  kHMInsideBottomLeftCorner     = 21,

  /*
   * Inside, aligned with bottom and right
   */
  kHMInsideBottomRightCorner    = 22,

  /*
   * Centered vertically and horizontally
   */
  kHMAbsoluteCenterAligned      = 23
};

/* Obsoleted constants HMTagDisplaySides, use the new ones, please */
enum {
  kHMTopSide                    = kHMOutsideTopScriptAligned,
  kHMLeftSide                   = kHMOutsideLeftCenterAligned,
  kHMBottomSide                 = kHMOutsideBottomScriptAligned,
  kHMRightSide                  = kHMOutsideRightCenterAligned,
  kHMTopLeftCorner              = kHMOutsideTopLeftAligned,
  kHMTopRightCorner             = kHMOutsideTopRightAligned,
  kHMLeftTopCorner              = kHMOutsideLeftTopAligned,
  kHMLeftBottomCorner           = kHMOutsideLeftBottomAligned,
  kHMBottomLeftCorner           = kHMOutsideBottomLeftAligned,
  kHMBottomRightCorner          = kHMOutsideBottomRightAligned,
  kHMRightTopCorner             = kHMOutsideRightTopAligned,
  kHMRightBottomCorner          = kHMOutsideRightBottomAligned
};

typedef SInt16 HMContentProvidedType;
enum {
  kHMContentProvided            = 0,
  kHMContentNotProvided         = 1,
  kHMContentNotProvidedDontPropagate = 2
};

enum {
  kHMMinimumContentIndex        = 0,    /* first entry in HMHelpContentRec.content is the minimum content */
  kHMMaximumContentIndex        = 1     /* second entry in HMHelpContentRec.content is the maximum content */
};

enum {
  errHMIllegalContentForMinimumState = -10980, /* unrecognized content type for minimum content */
  errHMIllegalContentForMaximumState = -10981 /* unrecognized content type for maximum content */
};

/* obsolete names; will be removed*/
enum {
  kHMIllegalContentForMinimumState = errHMIllegalContentForMinimumState
};

enum {
  kHelpTagEventHandlerTag       = FOUR_CHAR_CODE('hevt')
};

struct HMHelpContent {
  HMContentType       contentType;
  union {
    CFStringRef         tagCFString;          /* CFStringRef*/
    Str255              tagString;            /* Pascal String*/
    HMStringResType     tagStringRes;         /* STR# resource ID and index*/
    TEHandle            tagTEHandle;          /* TextEdit handle (NOT SUPPORTED ON MAC OS X)*/
    SInt16              tagTextRes;           /* TEXT/styl resource ID (NOT SUPPORTED ON MAC OS X)*/
    SInt16              tagStrRes;            /* STR resource ID*/
  }                       u;
};
typedef struct HMHelpContent            HMHelpContent;
struct HMHelpContentRec {
  SInt32              version;
  Rect                absHotRect;
  HMTagDisplaySide    tagSide;
  HMHelpContent       content[2];
};
typedef struct HMHelpContentRec         HMHelpContentRec;
typedef HMHelpContentRec *              HMHelpContentPtr;
/*------------------------------------------------------------------------------------------*/
/* Callback procs                                       */
/*------------------------------------------------------------------------------------------ */
typedef CALLBACK_API( OSStatus , HMControlContentProcPtr )(ControlRef inControl, Point inGlobalMouse, HMContentRequest inRequest, HMContentProvidedType *outContentProvided, HMHelpContentPtr ioHelpContent);
typedef CALLBACK_API( OSStatus , HMWindowContentProcPtr )(WindowRef inWindow, Point inGlobalMouse, HMContentRequest inRequest, HMContentProvidedType *outContentProvided, HMHelpContentPtr ioHelpContent);
typedef CALLBACK_API( OSStatus , HMMenuTitleContentProcPtr )(MenuRef inMenu, HMContentRequest inRequest, HMContentProvidedType *outContentProvided, HMHelpContentPtr ioHelpContent);
typedef CALLBACK_API( OSStatus , HMMenuItemContentProcPtr )(const MenuTrackingData *inTrackingData, HMContentRequest inRequest, HMContentProvidedType *outContentProvided, HMHelpContentPtr ioHelpContent);
typedef STACK_UPP_TYPE(HMControlContentProcPtr)                 HMControlContentUPP;
typedef STACK_UPP_TYPE(HMWindowContentProcPtr)                  HMWindowContentUPP;
typedef STACK_UPP_TYPE(HMMenuTitleContentProcPtr)               HMMenuTitleContentUPP;
typedef STACK_UPP_TYPE(HMMenuItemContentProcPtr)                HMMenuItemContentUPP;
/*
 *  NewHMControlContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( HMControlContentUPP )
NewHMControlContentUPP(HMControlContentProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppHMControlContentProcInfo = 0x0000FBF0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes, 2_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(HMControlContentUPP) NewHMControlContentUPP(HMControlContentProcPtr userRoutine) { return (HMControlContentUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHMControlContentProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewHMControlContentUPP(userRoutine) (HMControlContentUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHMControlContentProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewHMWindowContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( HMWindowContentUPP )
NewHMWindowContentUPP(HMWindowContentProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppHMWindowContentProcInfo = 0x0000FBF0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes, 2_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(HMWindowContentUPP) NewHMWindowContentUPP(HMWindowContentProcPtr userRoutine) { return (HMWindowContentUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHMWindowContentProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewHMWindowContentUPP(userRoutine) (HMWindowContentUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHMWindowContentProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewHMMenuTitleContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( HMMenuTitleContentUPP )
NewHMMenuTitleContentUPP(HMMenuTitleContentProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppHMMenuTitleContentProcInfo = 0x00003EF0 };  /* pascal 4_bytes Func(4_bytes, 2_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(HMMenuTitleContentUPP) NewHMMenuTitleContentUPP(HMMenuTitleContentProcPtr userRoutine) { return (HMMenuTitleContentUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHMMenuTitleContentProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewHMMenuTitleContentUPP(userRoutine) (HMMenuTitleContentUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHMMenuTitleContentProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewHMMenuItemContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( HMMenuItemContentUPP )
NewHMMenuItemContentUPP(HMMenuItemContentProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppHMMenuItemContentProcInfo = 0x00003EF0 };  /* pascal 4_bytes Func(4_bytes, 2_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(HMMenuItemContentUPP) NewHMMenuItemContentUPP(HMMenuItemContentProcPtr userRoutine) { return (HMMenuItemContentUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHMMenuItemContentProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewHMMenuItemContentUPP(userRoutine) (HMMenuItemContentUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppHMMenuItemContentProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeHMControlContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeHMControlContentUPP(HMControlContentUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeHMControlContentUPP(HMControlContentUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeHMControlContentUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeHMWindowContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeHMWindowContentUPP(HMWindowContentUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeHMWindowContentUPP(HMWindowContentUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeHMWindowContentUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeHMMenuTitleContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeHMMenuTitleContentUPP(HMMenuTitleContentUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeHMMenuTitleContentUPP(HMMenuTitleContentUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeHMMenuTitleContentUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeHMMenuItemContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeHMMenuItemContentUPP(HMMenuItemContentUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeHMMenuItemContentUPP(HMMenuItemContentUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeHMMenuItemContentUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeHMControlContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeHMControlContentUPP(
  ControlRef               inControl,
  Point                    inGlobalMouse,
  HMContentRequest         inRequest,
  HMContentProvidedType *  outContentProvided,
  HMHelpContentPtr         ioHelpContent,
  HMControlContentUPP      userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeHMControlContentUPP(ControlRef inControl, Point inGlobalMouse, HMContentRequest inRequest, HMContentProvidedType * outContentProvided, HMHelpContentPtr ioHelpContent, HMControlContentUPP userUPP) { return (OSStatus)CALL_FIVE_PARAMETER_UPP(userUPP, uppHMControlContentProcInfo, inControl, inGlobalMouse, inRequest, outContentProvided, ioHelpContent); }
  #else
    #define InvokeHMControlContentUPP(inControl, inGlobalMouse, inRequest, outContentProvided, ioHelpContent, userUPP) (OSStatus)CALL_FIVE_PARAMETER_UPP((userUPP), uppHMControlContentProcInfo, (inControl), (inGlobalMouse), (inRequest), (outContentProvided), (ioHelpContent))
  #endif
#endif

/*
 *  InvokeHMWindowContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeHMWindowContentUPP(
  WindowRef                inWindow,
  Point                    inGlobalMouse,
  HMContentRequest         inRequest,
  HMContentProvidedType *  outContentProvided,
  HMHelpContentPtr         ioHelpContent,
  HMWindowContentUPP       userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeHMWindowContentUPP(WindowRef inWindow, Point inGlobalMouse, HMContentRequest inRequest, HMContentProvidedType * outContentProvided, HMHelpContentPtr ioHelpContent, HMWindowContentUPP userUPP) { return (OSStatus)CALL_FIVE_PARAMETER_UPP(userUPP, uppHMWindowContentProcInfo, inWindow, inGlobalMouse, inRequest, outContentProvided, ioHelpContent); }
  #else
    #define InvokeHMWindowContentUPP(inWindow, inGlobalMouse, inRequest, outContentProvided, ioHelpContent, userUPP) (OSStatus)CALL_FIVE_PARAMETER_UPP((userUPP), uppHMWindowContentProcInfo, (inWindow), (inGlobalMouse), (inRequest), (outContentProvided), (ioHelpContent))
  #endif
#endif

/*
 *  InvokeHMMenuTitleContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeHMMenuTitleContentUPP(
  MenuRef                  inMenu,
  HMContentRequest         inRequest,
  HMContentProvidedType *  outContentProvided,
  HMHelpContentPtr         ioHelpContent,
  HMMenuTitleContentUPP    userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeHMMenuTitleContentUPP(MenuRef inMenu, HMContentRequest inRequest, HMContentProvidedType * outContentProvided, HMHelpContentPtr ioHelpContent, HMMenuTitleContentUPP userUPP) { return (OSStatus)CALL_FOUR_PARAMETER_UPP(userUPP, uppHMMenuTitleContentProcInfo, inMenu, inRequest, outContentProvided, ioHelpContent); }
  #else
    #define InvokeHMMenuTitleContentUPP(inMenu, inRequest, outContentProvided, ioHelpContent, userUPP) (OSStatus)CALL_FOUR_PARAMETER_UPP((userUPP), uppHMMenuTitleContentProcInfo, (inMenu), (inRequest), (outContentProvided), (ioHelpContent))
  #endif
#endif

/*
 *  InvokeHMMenuItemContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeHMMenuItemContentUPP(
  const MenuTrackingData *  inTrackingData,
  HMContentRequest          inRequest,
  HMContentProvidedType *   outContentProvided,
  HMHelpContentPtr          ioHelpContent,
  HMMenuItemContentUPP      userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeHMMenuItemContentUPP(const MenuTrackingData * inTrackingData, HMContentRequest inRequest, HMContentProvidedType * outContentProvided, HMHelpContentPtr ioHelpContent, HMMenuItemContentUPP userUPP) { return (OSStatus)CALL_FOUR_PARAMETER_UPP(userUPP, uppHMMenuItemContentProcInfo, inTrackingData, inRequest, outContentProvided, ioHelpContent); }
  #else
    #define InvokeHMMenuItemContentUPP(inTrackingData, inRequest, outContentProvided, ioHelpContent, userUPP) (OSStatus)CALL_FOUR_PARAMETER_UPP((userUPP), uppHMMenuItemContentProcInfo, (inTrackingData), (inRequest), (outContentProvided), (ioHelpContent))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewHMControlContentProc(userRoutine)                NewHMControlContentUPP(userRoutine)
    #define NewHMWindowContentProc(userRoutine)                 NewHMWindowContentUPP(userRoutine)
    #define NewHMMenuTitleContentProc(userRoutine)              NewHMMenuTitleContentUPP(userRoutine)
    #define NewHMMenuItemContentProc(userRoutine)               NewHMMenuItemContentUPP(userRoutine)
    #define CallHMControlContentProc(userRoutine, inControl, inGlobalMouse, inRequest, outContentProvided, ioHelpContent) InvokeHMControlContentUPP(inControl, inGlobalMouse, inRequest, outContentProvided, ioHelpContent, userRoutine)
    #define CallHMWindowContentProc(userRoutine, inWindow, inGlobalMouse, inRequest, outContentProvided, ioHelpContent) InvokeHMWindowContentUPP(inWindow, inGlobalMouse, inRequest, outContentProvided, ioHelpContent, userRoutine)
    #define CallHMMenuTitleContentProc(userRoutine, inMenu, inRequest, outContentProvided, ioHelpContent) InvokeHMMenuTitleContentUPP(inMenu, inRequest, outContentProvided, ioHelpContent, userRoutine)
    #define CallHMMenuItemContentProc(userRoutine, inTrackingData, inRequest, outContentProvided, ioHelpContent) InvokeHMMenuItemContentUPP(inTrackingData, inRequest, outContentProvided, ioHelpContent, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*------------------------------------------------------------------------------------------*/
/* API                                                                                      */
/*------------------------------------------------------------------------------------------*/
/* Help Menu */
/*
 *  HMGetHelpMenu()
 *  
 *  Summary:
 *    Returns a menu to which applications may add their own help items.
 *  
 *  Parameters:
 *    
 *    outHelpMenu:
 *      On exit, contains the help menu.
 *    
 *    outFirstCustomItemIndex:
 *      On exit, contains the menu item index that will be used by the
 *      first item added by the application. This parameter may be NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMGetHelpMenu(
  MenuRef *        outHelpMenu,
  MenuItemIndex *  outFirstCustomItemIndex);      /* can be NULL */


/* Installing/Retrieving Content */
/* Menu title and item help tags are not yet supported by Carbon or CarbonLib. */
/* They will be fully supported in a future release. */
/*
 *  HMSetControlHelpContent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMSetControlHelpContent(
  ControlRef                inControl,
  const HMHelpContentRec *  inContent);


/*
 *  HMGetControlHelpContent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMGetControlHelpContent(
  ControlRef          inControl,
  HMHelpContentRec *  outContent);


/*
 *  HMSetWindowHelpContent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMSetWindowHelpContent(
  WindowRef                 inWindow,
  const HMHelpContentRec *  inContent);


/*
 *  HMGetWindowHelpContent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMGetWindowHelpContent(
  WindowRef           inWindow,
  HMHelpContentRec *  outContent);


/*
 *  HMSetMenuItemHelpContent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMSetMenuItemHelpContent(
  MenuRef                   inMenu,
  MenuItemIndex             inItem,
  const HMHelpContentRec *  inContent);


/*
 *  HMGetMenuItemHelpContent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMGetMenuItemHelpContent(
  MenuRef             inMenu,
  MenuItemIndex       inItem,
  HMHelpContentRec *  outContent);


/* Installing/Retrieving Content Callbacks */
/*
 *  HMInstallControlContentCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMInstallControlContentCallback(
  ControlRef            inControl,
  HMControlContentUPP   inContentUPP);


/*
 *  HMInstallWindowContentCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMInstallWindowContentCallback(
  WindowRef            inWindow,
  HMWindowContentUPP   inContentUPP);


/*
 *  HMInstallMenuTitleContentCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMInstallMenuTitleContentCallback(
  MenuRef                 inMenu,
  HMMenuTitleContentUPP   inContentUPP);


/*
 *  HMInstallMenuItemContentCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMInstallMenuItemContentCallback(
  MenuRef                inMenu,
  HMMenuItemContentUPP   inContentUPP);


/*
 *  HMGetControlContentCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMGetControlContentCallback(
  ControlRef             inControl,
  HMControlContentUPP *  outContentUPP);


/*
 *  HMGetWindowContentCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMGetWindowContentCallback(
  WindowRef             inWindow,
  HMWindowContentUPP *  outContentUPP);


/*
 *  HMGetMenuTitleContentCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMGetMenuTitleContentCallback(
  MenuRef                  inMenu,
  HMMenuTitleContentUPP *  outContentUPP);


/*
 *  HMGetMenuItemContentCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMGetMenuItemContentCallback(
  MenuRef                 inMenu,
  HMMenuItemContentUPP *  outContentUPP);


/* Enabling and Disabling Help Tags */
/*
 *  HMAreHelpTagsDisplayed()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
HMAreHelpTagsDisplayed(void);


/*
 *  HMSetHelpTagsDisplayed()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMSetHelpTagsDisplayed(Boolean inDisplayTags);


/*
 *  HMSetTagDelay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMSetTagDelay(Duration inDelay);


/*
 *  HMGetTagDelay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMGetTagDelay(Duration * outDelay);


/* Compatibility */
/*
 *  HMSetMenuHelpFromBalloonRsrc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMSetMenuHelpFromBalloonRsrc(
  MenuRef   inMenu,
  SInt16    inHmnuRsrcID);


/*
 *  HMSetDialogHelpFromBalloonRsrc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMSetDialogHelpFromBalloonRsrc(
  DialogRef   inDialog,
  SInt16      inHdlgRsrcID,
  SInt16      inItemStart);


/* Displaying tags */
/*
 *  HMDisplayTag()
 *  
 *  Summary:
 *    Displays a help tag at a user defined location.
 *  
 *  Parameters:
 *    
 *    inContent:
 *      HMHelpContentRec describing the help tag to be displayed.
 *  
 *  Result:
 *    An OSStatus code indicating success or failure.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
HMDisplayTag(const HMHelpContentRec * inContent);


/*
 *  HMHideTag()
 *  
 *  Summary:
 *    HMHideTag hides the currently visible help tag.  If there is no
 *    current help tag, this call does nothing.
 *  
 *  Result:
 *    An OSStatus code indicating success or failure.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.2 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSStatus )
HMHideTag(void);



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

#endif /* __MACHELP__ */

