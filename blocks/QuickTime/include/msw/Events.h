/*
     File:       Events.h
 
     Contains:   Event Manager Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __EVENTS__
#define __EVENTS__

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#if !TARGET_OS_MAC || !TARGET_API_MAC_OS8
#ifndef __ENDIAN__
#include <Endian.h>
#endif

#endif  /* !TARGET_OS_MAC || !TARGET_API_MAC_OS8 */

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
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

typedef UInt16                          EventKind;
typedef UInt16                          EventMask;
enum {
  nullEvent                     = 0,
  mouseDown                     = 1,
  mouseUp                       = 2,
  keyDown                       = 3,
  keyUp                         = 4,
  autoKey                       = 5,
  updateEvt                     = 6,
  diskEvt                       = 7,    /* Not sent in Carbon. See kEventClassVolume in CarbonEvents.h*/
  activateEvt                   = 8,
  osEvt                         = 15,
  kHighLevelEvent               = 23
};

enum {
  mDownMask                     = 1 << mouseDown, /* mouse button pressed*/
  mUpMask                       = 1 << mouseUp, /* mouse button released*/
  keyDownMask                   = 1 << keyDown, /* key pressed*/
  keyUpMask                     = 1 << keyUp, /* key released*/
  autoKeyMask                   = 1 << autoKey, /* key repeatedly held down*/
  updateMask                    = 1 << updateEvt, /* window needs updating*/
  diskMask                      = 1 << diskEvt, /* disk inserted*/
  activMask                     = 1 << activateEvt, /* activate/deactivate window*/
  highLevelEventMask            = 0x0400, /* high-level events (includes AppleEvents)*/
  osMask                        = 1 << osEvt, /* operating system events (suspend, resume)*/
  everyEvent                    = 0xFFFF /* all of the above*/
};

enum {
  charCodeMask                  = 0x000000FF,
  keyCodeMask                   = 0x0000FF00,
  adbAddrMask                   = 0x00FF0000,
  osEvtMessageMask              = (unsigned long)0xFF000000
};

enum {
                                        /* OS event messages.  Event (sub)code is in the high byte of the message field.*/
  mouseMovedMessage             = 0x00FA,
  suspendResumeMessage          = 0x0001
};

enum {
  resumeFlag                    = 1     /* Bit 0 of message indicates resume vs suspend*/
};

#if CALL_NOT_IN_CARBON
/*  convertClipboardFlag is not ever set under Carbon. This is because scrap conversion is  */
/*  not tied to suspend/resume events any longer. Your application should instead use the   */
/*  scrap promise mechanism and fulfill scrap requests only when your promise keeper proc   */
/*  is called. If you need to know if the scrap has changed, you can cache the last         */
/*  ScrapRef you received and compare it with the current ScrapRef                          */
enum {
  convertClipboardFlag          = 2     /* Bit 1 in resume message indicates clipboard change*/
};

#endif  /* CALL_NOT_IN_CARBON */

/*
    CARBON ALERT! BATTLESTATIONS!
    
    The EventModifiers bits defined here are also used in the newer Carbon Event
    key modifiers parameters. There are two main differences:
    
    1)  The Carbon key modifiers parameter is a UInt32, not a UInt16. Never try to
        extract the key modifiers parameter from a Carbon Event into an EventModifiers
        type. You will probably get your stack trashed.
    2)  The Carbon key modifiers is just that: key modifiers. That parameter will
        never contain the button state bit.
*/
typedef UInt16                          EventModifiers;
enum {
                                        /* modifiers */
  activeFlagBit                 = 0,    /* activate? (activateEvt and mouseDown)*/
  btnStateBit                   = 7,    /* state of button?*/
  cmdKeyBit                     = 8,    /* command key down?*/
  shiftKeyBit                   = 9,    /* shift key down?*/
  alphaLockBit                  = 10,   /* alpha lock down?*/
  optionKeyBit                  = 11,   /* option key down?*/
  controlKeyBit                 = 12,   /* control key down?*/
  rightShiftKeyBit              = 13,   /* right shift key down?*/
  rightOptionKeyBit             = 14,   /* right Option key down?*/
  rightControlKeyBit            = 15    /* right Control key down?*/
};

enum {
  activeFlag                    = 1 << activeFlagBit,
  btnState                      = 1 << btnStateBit,
  cmdKey                        = 1 << cmdKeyBit,
  shiftKey                      = 1 << shiftKeyBit,
  alphaLock                     = 1 << alphaLockBit,
  optionKey                     = 1 << optionKeyBit,
  controlKey                    = 1 << controlKeyBit,
  rightShiftKey                 = 1 << rightShiftKeyBit,
  rightOptionKey                = 1 << rightOptionKeyBit,
  rightControlKey               = 1 << rightControlKeyBit
};

/* MacRoman character codes*/
enum {
  kNullCharCode                 = 0,
  kHomeCharCode                 = 1,
  kEnterCharCode                = 3,
  kEndCharCode                  = 4,
  kHelpCharCode                 = 5,
  kBellCharCode                 = 7,
  kBackspaceCharCode            = 8,
  kTabCharCode                  = 9,
  kLineFeedCharCode             = 10,
  kVerticalTabCharCode          = 11,
  kPageUpCharCode               = 11,
  kFormFeedCharCode             = 12,
  kPageDownCharCode             = 12,
  kReturnCharCode               = 13,
  kFunctionKeyCharCode          = 16,
  kCommandCharCode              = 17,   /* glyph available only in system fonts*/
  kCheckCharCode                = 18,   /* glyph available only in system fonts*/
  kDiamondCharCode              = 19,   /* glyph available only in system fonts*/
  kAppleLogoCharCode            = 20,   /* glyph available only in system fonts*/
  kEscapeCharCode               = 27,
  kClearCharCode                = 27,
  kLeftArrowCharCode            = 28,
  kRightArrowCharCode           = 29,
  kUpArrowCharCode              = 30,
  kDownArrowCharCode            = 31,
  kSpaceCharCode                = 32,
  kDeleteCharCode               = 127,
  kBulletCharCode               = 165,
  kNonBreakingSpaceCharCode     = 202
};

/* useful Unicode code points*/
enum {
  kShiftUnicode                 = 0x21E7, /* Unicode UPWARDS WHITE ARROW*/
  kControlUnicode               = 0x2303, /* Unicode UP ARROWHEAD*/
  kOptionUnicode                = 0x2325, /* Unicode OPTION KEY*/
  kCommandUnicode               = 0x2318, /* Unicode PLACE OF INTEREST SIGN*/
  kPencilUnicode                = 0x270E, /* Unicode LOWER RIGHT PENCIL*/
  kCheckUnicode                 = 0x2713, /* Unicode CHECK MARK*/
  kDiamondUnicode               = 0x25C6, /* Unicode BLACK DIAMOND*/
  kBulletUnicode                = 0x2022, /* Unicode BULLET*/
  kAppleLogoUnicode             = 0xF8FF /* Unicode APPLE LOGO*/
};

struct EventRecord {
  EventKind           what;
  UInt32              message;
  UInt32              when;
  Point               where;
  EventModifiers      modifiers;
};
typedef struct EventRecord              EventRecord;
typedef CALLBACK_API( void , FKEYProcPtr )(void);
typedef STACK_UPP_TYPE(FKEYProcPtr)                             FKEYUPP;
#if CALL_NOT_IN_CARBON
/*
 *  NewFKEYUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( FKEYUPP )
NewFKEYUPP(FKEYProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppFKEYProcInfo = 0x00000000 };  /* pascal no_return_value Func() */
  #ifdef __cplusplus
    inline DEFINE_API_C(FKEYUPP) NewFKEYUPP(FKEYProcPtr userRoutine) { return (FKEYUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppFKEYProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewFKEYUPP(userRoutine) (FKEYUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppFKEYProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeFKEYUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeFKEYUPP(FKEYUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeFKEYUPP(FKEYUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeFKEYUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeFKEYUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
InvokeFKEYUPP(FKEYUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeFKEYUPP(FKEYUPP userUPP) { CALL_ZERO_PARAMETER_UPP(userUPP, uppFKEYProcInfo); }
  #else
    #define InvokeFKEYUPP(userUPP) CALL_ZERO_PARAMETER_UPP((userUPP), uppFKEYProcInfo)
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewFKEYProc(userRoutine)                            NewFKEYUPP(userRoutine)
    #define CallFKEYProc(userRoutine)                           InvokeFKEYUPP(userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*
 *  GetMouse()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetMouse(Point * mouseLoc)                                    ONEWORDINLINE(0xA972);


/*
 *  Button()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
Button(void)                                                  ONEWORDINLINE(0xA974);


/*
 *  StillDown()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
StillDown(void)                                               ONEWORDINLINE(0xA973);


/*
 *  WaitMouseUp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
WaitMouseUp(void)                                             ONEWORDINLINE(0xA977);


/*
 *  KeyTranslate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt32 )
KeyTranslate(
  const void *  transData,
  UInt16        keycode,
  UInt32 *      state)                                        ONEWORDINLINE(0xA9C3);


/*
 *  GetCaretTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt32 )
GetCaretTime(void)                                            TWOWORDINLINE(0x2EB8, 0x02F4);



/* 
    QuickTime 3.0 supports GetKeys() on win32.
    But, on little endian machines you will have to be
    careful about bit numberings and/or use a KeyMapByteArray
    instead.
*/
#if TARGET_OS_MAC && TARGET_API_MAC_OS8

typedef UInt32                          KeyMap[4];
#else
typedef BigEndianLong                   KeyMap[4];
#endif  /* TARGET_OS_MAC && TARGET_API_MAC_OS8 */

typedef UInt8                           KeyMapByteArray[16];
/*
 *  GetKeys()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetKeys(KeyMap theKeys)                                       ONEWORDINLINE(0xA976);


/* Obsolete event types & masks */
enum {
  networkEvt                    = 10,
  driverEvt                     = 11,
  app1Evt                       = 12,
  app2Evt                       = 13,
  app3Evt                       = 14,
  app4Evt                       = 15,
  networkMask                   = 0x0400,
  driverMask                    = 0x0800,
  app1Mask                      = 0x1000,
  app2Mask                      = 0x2000,
  app3Mask                      = 0x4000,
  app4Mask                      = 0x8000
};

struct EvQEl {
  QElemPtr            qLink;
  SInt16              qType;
  EventKind           evtQWhat;               /* this part is identical to the EventRecord as defined above */
  UInt32              evtQMessage;
  UInt32              evtQWhen;
  Point               evtQWhere;
  EventModifiers      evtQModifiers;
};
typedef struct EvQEl                    EvQEl;
typedef EvQEl *                         EvQElPtr;
typedef CALLBACK_API_REGISTER68K( void , GetNextEventFilterProcPtr, (EventRecord *theEvent, Boolean *result) );
typedef REGISTER_UPP_TYPE(GetNextEventFilterProcPtr)            GetNextEventFilterUPP;
#if CALL_NOT_IN_CARBON
/*
 *  NewGetNextEventFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( GetNextEventFilterUPP )
NewGetNextEventFilterUPP(GetNextEventFilterProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppGetNextEventFilterProcInfo = 0x000000BF };  /* SPECIAL_CASE_PROCINFO(11) */
  #ifdef __cplusplus
    inline DEFINE_API_C(GetNextEventFilterUPP) NewGetNextEventFilterUPP(GetNextEventFilterProcPtr userRoutine) { return (GetNextEventFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppGetNextEventFilterProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewGetNextEventFilterUPP(userRoutine) (GetNextEventFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppGetNextEventFilterProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeGetNextEventFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeGetNextEventFilterUPP(GetNextEventFilterUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeGetNextEventFilterUPP(GetNextEventFilterUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeGetNextEventFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeGetNextEventFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
InvokeGetNextEventFilterUPP(
  EventRecord *          theEvent,
  Boolean *              result,
  GetNextEventFilterUPP  userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeGetNextEventFilterUPP(EventRecord * theEvent, Boolean * result, GetNextEventFilterUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppGetNextEventFilterProcInfo, theEvent, result); }
  #else
    #define InvokeGetNextEventFilterUPP(theEvent, result, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppGetNextEventFilterProcInfo, (theEvent), (result))
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewGetNextEventFilterProc(userRoutine)              NewGetNextEventFilterUPP(userRoutine)
    #define CallGetNextEventFilterProc(userRoutine, theEvent, result) InvokeGetNextEventFilterUPP(theEvent, result, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

typedef GetNextEventFilterUPP           GNEFilterUPP;
/*
 *  GetDblTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt32 )
GetDblTime(void)                                              TWOWORDINLINE(0x2EB8, 0x02F0);


/*
 *  SetEventMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetEventMask(EventMask value)                                 TWOWORDINLINE(0x31DF, 0x0144);


#if CALL_NOT_IN_CARBON
/*
 *  GetEvQHdr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( QHdrPtr )
GetEvQHdr(void)                                               THREEWORDINLINE(0x2EBC, 0x0000, 0x014A);


#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON
/*
 *  PPostEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __D0 PPostEvent(__A0, __D0, __A1)
#endif
EXTERN_API( OSErr )
PPostEvent(
  EventKind   eventCode,
  UInt32      eventMsg,
  EvQElPtr *  qEl)                                            TWOWORDINLINE(0xA12F, 0x2288);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  GetNextEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
GetNextEvent(
  EventMask      eventMask,
  EventRecord *  theEvent)                                    ONEWORDINLINE(0xA970);


/*
 *  WaitNextEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
WaitNextEvent(
  EventMask      eventMask,
  EventRecord *  theEvent,
  UInt32         sleep,
  RgnHandle      mouseRgn)        /* can be NULL */           ONEWORDINLINE(0xA860);


/*
 *  EventAvail()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
EventAvail(
  EventMask      eventMask,
  EventRecord *  theEvent)                                    ONEWORDINLINE(0xA971);


/*
 *  PostEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __D0 PostEvent(__A0, __D0)
#endif
EXTERN_API( OSErr )
PostEvent(
  EventKind   eventNum,
  UInt32      eventMsg)                                       ONEWORDINLINE(0xA02F);


/*
    For Carbon, use EventAvail, TickCount, GetGlobalMouse,
    GetKeys, or GetCurrentKeyModifiers instead of
    OSEventAvail, and use GetNextEvent or WaitNextEvent
    instead of GetOSEvent.
*/

#if CALL_NOT_IN_CARBON
/*
 *  OSEventAvail()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __D0 OSEventAvail(__D0, __A0)
#endif
EXTERN_API( Boolean )
OSEventAvail(
  EventMask      mask,
  EventRecord *  theEvent)                                    TWOWORDINLINE(0xA030, 0x5240);


/*
 *  GetOSEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __D0 GetOSEvent(__D0, __A0)
#endif
EXTERN_API( Boolean )
GetOSEvent(
  EventMask      mask,
  EventRecord *  theEvent)                                    TWOWORDINLINE(0xA031, 0x5240);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  FlushEvents()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FlushEvents(
  EventMask   whichMask,
  EventMask   stopMask)                                       TWOWORDINLINE(0x201F, 0xA032);


#if CALL_NOT_IN_CARBON
/*
 *  SystemClick()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
SystemClick(
  const EventRecord *  theEvent,
  WindowRef            theWindow)                             ONEWORDINLINE(0xA9B3);


/*
 *  SystemTask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
SystemTask(void)                                              ONEWORDINLINE(0xA9B4);


/*
 *  SystemEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Boolean )
SystemEvent(const EventRecord * theEvent)                     ONEWORDINLINE(0xA9B2);


#endif  /* CALL_NOT_IN_CARBON */

#if OLDROUTINENAMES
#define KeyTrans(transData, keycode, state) KeyTranslate(transData, keycode, state)
#endif  /* OLDROUTINENAMES */

/*
    GetGlobalMouse, GetCurrentKeyModifiers, and CheckEventQueueForUserCancel
    are only available as part of the Carbon API.
*/

/*
 *  GetGlobalMouse()
 *  
 *  Summary:
 *    Returns the position of the mouse in global coordinates.
 *  
 *  Parameters:
 *    
 *    globalMouse:
 *      On exit, contains the mouse position in global coordinates.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetGlobalMouse(Point * globalMouse);


/*
 *  GetCurrentKeyModifiers()
 *  
 *  Summary:
 *    Returns the current hardware keyboard modifier state.
 *  
 *  Discussion:
 *    In most cases, you should not use GetCurrentKeyModifiers, but
 *    should use the GetCurrentEventKeyModifiers function instead.
 *    GetCurrentEventKeyModifiers is much faster than
 *    GetCurrentKeyModifiers because it returns the locally cached
 *    modifier state; GetCurrentKeyModifiers must get the modifier
 *    state from the window server, which is slower. Using
 *    GetCurrentKeyModifiers also can prevent your application from
 *    being operated by remote posting of events, since the hardware
 *    input device is not actually changing state in that case. Most
 *    commonly, you might need to use GetCurrentKeyModifiers when your
 *    application is not the active application (as determined by the
 *    Process Manager function GetFrontProcess). In that case, the
 *    cached modifier state returned by GetCurrentEventKeyModifiers is
 *    not valid because modifier-changed events are not flowing to your
 *    application, and you must use GetCurrentKeyModifiers to determine
 *    the current hardware state.
 *  
 *  Result:
 *    The hardware state of the keyboard modifiers. The format of the
 *    return value is the same as the modifiers field of an EventRecord
 *    (but only includes keyboard modifiers and not the other modifier
 *    flags included in an EventRecord).
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt32 )
GetCurrentKeyModifiers(void);


/*
 *  CheckEventQueueForUserCancel()
 *  
 *  Summary:
 *    Determines if there is a cancel event in the main thread's event
 *    queue.
 *  
 *  Discussion:
 *    This API supports two cancel events: Escape and Cmd-Period. The
 *    cancel event itself, as well as mouse or keyboard events in front
 *    of the cancel event in the event queue, will be removed from the
 *    queue.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
CheckEventQueueForUserCancel(void);



/*
 *  KeyScript()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
KeyScript(short code)                                         FOURWORDINLINE(0x2F3C, 0x8002, 0x0004, 0xA8B5);


/*
 *  IsCmdChar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsCmdChar(
  const EventRecord *  event,
  short                test)                                  FOURWORDINLINE(0x2F3C, 0x8206, 0xFFD0, 0xA8B5);



/* 
    LowMem accessor functions previously in LowMem.h
*/
/*
 *  LMGetKeyThresh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 )
LMGetKeyThresh(void)                                          TWOWORDINLINE(0x3EB8, 0x018E);


/*
 *  LMSetKeyThresh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetKeyThresh(SInt16 value)                                  TWOWORDINLINE(0x31DF, 0x018E);



/*
 *  LMGetKeyRepThresh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 )
LMGetKeyRepThresh(void)                                       TWOWORDINLINE(0x3EB8, 0x0190);


/*
 *  LMSetKeyRepThresh()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetKeyRepThresh(SInt16 value)                               TWOWORDINLINE(0x31DF, 0x0190);


/*
 *  LMGetKbdLast()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt8 )
LMGetKbdLast(void)                                            TWOWORDINLINE(0x1EB8, 0x0218);


/*
 *  LMSetKbdLast()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetKbdLast(UInt8 value)                                     TWOWORDINLINE(0x11DF, 0x0218);



/*
 *  LMGetKbdType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt8 )
LMGetKbdType(void)                                            TWOWORDINLINE(0x1EB8, 0x021E);


/*
 *  LMSetKbdType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetKbdType(UInt8 value)                                     TWOWORDINLINE(0x11DF, 0x021E);



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

#endif /* __EVENTS__ */

