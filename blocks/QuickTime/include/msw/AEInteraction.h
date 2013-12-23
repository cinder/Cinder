/*
     File:       AEInteraction.h
 
     Contains:   AppleEvent functions that deal with Events and interacting with user
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __AEINTERACTION__
#define __AEINTERACTION__

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

#ifndef __NOTIFICATION__
#include <Notification.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
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

/**************************************************************************
  AppleEvent callbacks. 
**************************************************************************/
typedef CALLBACK_API( Boolean , AEIdleProcPtr )(EventRecord *theEvent, long *sleepTime, RgnHandle *mouseRgn);
typedef CALLBACK_API( Boolean , AEFilterProcPtr )(EventRecord *theEvent, long returnID, long transactionID, const AEAddressDesc *sender);
typedef STACK_UPP_TYPE(AEIdleProcPtr)                           AEIdleUPP;
typedef STACK_UPP_TYPE(AEFilterProcPtr)                         AEFilterUPP;

/**************************************************************************
  The next couple of calls are basic routines used to create, send,
  and process AppleEvents. 
**************************************************************************/
/*
 *  AESend()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AESend(
  const AppleEvent *  theAppleEvent,
  AppleEvent *        reply,
  AESendMode          sendMode,
  AESendPriority      sendPriority,
  long                timeOutInTicks,
  AEIdleUPP           idleProc,             /* can be NULL */
  AEFilterUPP         filterProc)           /* can be NULL */ THREEWORDINLINE(0x303C, 0x0D17, 0xA816);


/*
 *  AEProcessAppleEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEProcessAppleEvent(const EventRecord * theEventRecord)       THREEWORDINLINE(0x303C, 0x021B, 0xA816);



/* 
 Note: during event processing, an event handler may realize that it is likely
 to exceed the client's timeout limit. Passing the reply to this
 routine causes a wait event to be generated that asks the client
 for more time. 
*/
/*
 *  AEResetTimer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEResetTimer(const AppleEvent * reply)                        THREEWORDINLINE(0x303C, 0x0219, 0xA816);



/**************************************************************************
  The following three calls are used to allow applications to behave
  courteously when a user interaction such as a dialog box is needed. 
**************************************************************************/

typedef SInt8 AEInteractAllowed;
enum {
  kAEInteractWithSelf           = 0,
  kAEInteractWithLocal          = 1,
  kAEInteractWithAll            = 2
};

/*
 *  AEGetInteractionAllowed()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEGetInteractionAllowed(AEInteractAllowed * level)            THREEWORDINLINE(0x303C, 0x021D, 0xA816);


/*
 *  AESetInteractionAllowed()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AESetInteractionAllowed(AEInteractAllowed level)              THREEWORDINLINE(0x303C, 0x011E, 0xA816);


/*
 *  AEInteractWithUser()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEInteractWithUser(
  long        timeOutInTicks,
  NMRecPtr    nmReqPtr,
  AEIdleUPP   idleProc)                                       THREEWORDINLINE(0x303C, 0x061C, 0xA816);



/**************************************************************************
 The following four calls are available for applications which need more
 sophisticated control over when and how events are processed. Applications
 which implement multi-session servers or which implement their own
 internal event queueing will probably be the major clients of these
 routines. They can be called from within a handler to prevent the AEM from
 disposing of the AppleEvent when the handler returns. They can be used to
 asynchronously process the event (as MacApp does).
**************************************************************************/
/*
 *  AESuspendTheCurrentEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AESuspendTheCurrentEvent(const AppleEvent * theAppleEvent)    THREEWORDINLINE(0x303C, 0x022B, 0xA816);


/* 
 Note: The following routine tells the AppleEvent manager that processing
 is either about to resume or has been completed on a previously suspended
 event. The procPtr passed in as the dispatcher parameter will be called to
 attempt to redispatch the event. Several constants for the dispatcher
 parameter allow special behavior. They are:
    - kAEUseStandardDispatch means redispatch as if the event was just
      received, using the standard AppleEvent dispatch mechanism.
    - kAENoDispatch means ignore the parameter.
      Use this in the case where the event has been handled and no
      redispatch is needed.
    - non nil means call the routine which the dispatcher points to.
*/
/* Constants for Refcon in AEResumeTheCurrentEvent with kAEUseStandardDispatch */
enum {
  kAEDoNotIgnoreHandler         = 0x00000000,
  kAEIgnoreAppPhacHandler       = 0x00000001, /* available only in vers 1.0.1 and greater */
  kAEIgnoreAppEventHandler      = 0x00000002, /* available only in vers 1.0.1 and greater */
  kAEIgnoreSysPhacHandler       = 0x00000004, /* available only in vers 1.0.1 and greater */
  kAEIgnoreSysEventHandler      = 0x00000008, /* available only in vers 1.0.1 and greater */
  kAEIngoreBuiltInEventHandler  = 0x00000010, /* available only in vers 1.0.1 and greater */
  kAEDontDisposeOnResume        = (long)0x80000000 /* available only in vers 1.0.1 and greater */
};

/* Constants for AEResumeTheCurrentEvent */
enum {
  kAENoDispatch                 = 0,    /* dispatch parameter to AEResumeTheCurrentEvent takes a pointer to a dispatch */
  kAEUseStandardDispatch        = (long)0xFFFFFFFF /* table, or one of these two constants */
};

/*
 *  AEResumeTheCurrentEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEResumeTheCurrentEvent(
  const AppleEvent *  theAppleEvent,
  const AppleEvent *  reply,
  AEEventHandlerUPP   dispatcher,          /* can be NULL */
  long                handlerRefcon)                          THREEWORDINLINE(0x303C, 0x0818, 0xA816);


/*
 *  AEGetTheCurrentEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEGetTheCurrentEvent(AppleEvent * theAppleEvent)              THREEWORDINLINE(0x303C, 0x021A, 0xA816);


/*
 *  AESetTheCurrentEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AESetTheCurrentEvent(const AppleEvent * theAppleEvent)        THREEWORDINLINE(0x303C, 0x022C, 0xA816);



/**************************************************************************
  AppleEvent callbacks. 
**************************************************************************/
/*
 *  NewAEIdleUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AEIdleUPP )
NewAEIdleUPP(AEIdleProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppAEIdleProcInfo = 0x00000FD0 };  /* pascal 1_byte Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(AEIdleUPP) NewAEIdleUPP(AEIdleProcPtr userRoutine) { return (AEIdleUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAEIdleProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewAEIdleUPP(userRoutine) (AEIdleUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAEIdleProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewAEFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AEFilterUPP )
NewAEFilterUPP(AEFilterProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppAEFilterProcInfo = 0x00003FD0 };  /* pascal 1_byte Func(4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(AEFilterUPP) NewAEFilterUPP(AEFilterProcPtr userRoutine) { return (AEFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAEFilterProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewAEFilterUPP(userRoutine) (AEFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAEFilterProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeAEIdleUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeAEIdleUPP(AEIdleUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeAEIdleUPP(AEIdleUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeAEIdleUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeAEFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeAEFilterUPP(AEFilterUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeAEFilterUPP(AEFilterUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeAEFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeAEIdleUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeAEIdleUPP(
  EventRecord *  theEvent,
  long *         sleepTime,
  RgnHandle *    mouseRgn,
  AEIdleUPP      userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeAEIdleUPP(EventRecord * theEvent, long * sleepTime, RgnHandle * mouseRgn, AEIdleUPP userUPP) { return (Boolean)CALL_THREE_PARAMETER_UPP(userUPP, uppAEIdleProcInfo, theEvent, sleepTime, mouseRgn); }
  #else
    #define InvokeAEIdleUPP(theEvent, sleepTime, mouseRgn, userUPP) (Boolean)CALL_THREE_PARAMETER_UPP((userUPP), uppAEIdleProcInfo, (theEvent), (sleepTime), (mouseRgn))
  #endif
#endif

/*
 *  InvokeAEFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeAEFilterUPP(
  EventRecord *          theEvent,
  long                   returnID,
  long                   transactionID,
  const AEAddressDesc *  sender,
  AEFilterUPP            userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeAEFilterUPP(EventRecord * theEvent, long returnID, long transactionID, const AEAddressDesc * sender, AEFilterUPP userUPP) { return (Boolean)CALL_FOUR_PARAMETER_UPP(userUPP, uppAEFilterProcInfo, theEvent, returnID, transactionID, sender); }
  #else
    #define InvokeAEFilterUPP(theEvent, returnID, transactionID, sender, userUPP) (Boolean)CALL_FOUR_PARAMETER_UPP((userUPP), uppAEFilterProcInfo, (theEvent), (returnID), (transactionID), (sender))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewAEIdleProc(userRoutine)                          NewAEIdleUPP(userRoutine)
    #define NewAEFilterProc(userRoutine)                        NewAEFilterUPP(userRoutine)
    #define CallAEIdleProc(userRoutine, theEvent, sleepTime, mouseRgn) InvokeAEIdleUPP(theEvent, sleepTime, mouseRgn, userRoutine)
    #define CallAEFilterProc(userRoutine, theEvent, returnID, transactionID, sender) InvokeAEFilterUPP(theEvent, returnID, transactionID, sender, userRoutine)
#endif /* CALL_NOT_IN_CARBON */





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

#endif /* __AEINTERACTION__ */

