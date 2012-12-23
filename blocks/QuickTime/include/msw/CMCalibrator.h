/*
     File:       CMCalibrator.h
 
     Contains:   ColorSync Calibration API
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1998-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __CMCALIBRATOR__
#define __CMCALIBRATOR__

#ifndef __CMAPPLICATION__
#include <CMApplication.h>
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

typedef CALLBACK_API( void , CalibrateEventProcPtr )(EventRecord * event);
typedef STACK_UPP_TYPE(CalibrateEventProcPtr)                   CalibrateEventUPP;

/* Interface for new ColorSync monitor calibrators (ColorSync 2.6 and greater) */

enum {
  kCalibratorNamePrefix         = FOUR_CHAR_CODE('cali')
};

struct CalibratorInfo {
  UInt32              dataSize;               /* Size of this structure - compatibility */
  CMDisplayIDType     displayID;              /* Contains an hDC on Win32 */
  UInt32              profileLocationSize;    /* Max size for returned profile location */
  CMProfileLocation * profileLocationPtr;     /* For returning the profile */
  CalibrateEventUPP   eventProc;              /* Ignored on Win32 */
  Boolean             isGood;                 /* true or false */
};
typedef struct CalibratorInfo           CalibratorInfo;
typedef CALLBACK_API( Boolean , CanCalibrateProcPtr )(CMDisplayIDType displayID, Str255 errMessage);
typedef CALLBACK_API( OSErr , CalibrateProcPtr )(CalibratorInfo * theInfo);
typedef STACK_UPP_TYPE(CanCalibrateProcPtr)                     CanCalibrateUPP;
typedef STACK_UPP_TYPE(CalibrateProcPtr)                        CalibrateUPP;
/*
 *  NewCalibrateEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CalibrateEventUPP )
NewCalibrateEventUPP(CalibrateEventProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppCalibrateEventProcInfo = 0x000000C0 };  /* pascal no_return_value Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(CalibrateEventUPP) NewCalibrateEventUPP(CalibrateEventProcPtr userRoutine) { return (CalibrateEventUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCalibrateEventProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewCalibrateEventUPP(userRoutine) (CalibrateEventUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCalibrateEventProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewCanCalibrateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( CanCalibrateUPP )
NewCanCalibrateUPP(CanCalibrateProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppCanCalibrateProcInfo = 0x000003D0 };  /* pascal 1_byte Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(CanCalibrateUPP) NewCanCalibrateUPP(CanCalibrateProcPtr userRoutine) { return (CanCalibrateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCanCalibrateProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewCanCalibrateUPP(userRoutine) (CanCalibrateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCanCalibrateProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewCalibrateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( CalibrateUPP )
NewCalibrateUPP(CalibrateProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppCalibrateProcInfo = 0x000000E0 };  /* pascal 2_bytes Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(CalibrateUPP) NewCalibrateUPP(CalibrateProcPtr userRoutine) { return (CalibrateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCalibrateProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewCalibrateUPP(userRoutine) (CalibrateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCalibrateProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeCalibrateEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeCalibrateEventUPP(CalibrateEventUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeCalibrateEventUPP(CalibrateEventUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeCalibrateEventUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeCanCalibrateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeCanCalibrateUPP(CanCalibrateUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeCanCalibrateUPP(CanCalibrateUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeCanCalibrateUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeCalibrateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeCalibrateUPP(CalibrateUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeCalibrateUPP(CalibrateUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeCalibrateUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeCalibrateEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeCalibrateEventUPP(
  EventRecord *      event,
  CalibrateEventUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeCalibrateEventUPP(EventRecord * event, CalibrateEventUPP userUPP) { CALL_ONE_PARAMETER_UPP(userUPP, uppCalibrateEventProcInfo, event); }
  #else
    #define InvokeCalibrateEventUPP(event, userUPP) CALL_ONE_PARAMETER_UPP((userUPP), uppCalibrateEventProcInfo, (event))
  #endif
#endif

/*
 *  InvokeCanCalibrateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
InvokeCanCalibrateUPP(
  CMDisplayIDType  displayID,
  Str255           errMessage,
  CanCalibrateUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeCanCalibrateUPP(CMDisplayIDType displayID, Str255 errMessage, CanCalibrateUPP userUPP) { return (Boolean)CALL_TWO_PARAMETER_UPP(userUPP, uppCanCalibrateProcInfo, displayID, errMessage); }
  #else
    #define InvokeCanCalibrateUPP(displayID, errMessage, userUPP) (Boolean)CALL_TWO_PARAMETER_UPP((userUPP), uppCanCalibrateProcInfo, (displayID), (errMessage))
  #endif
#endif

/*
 *  InvokeCalibrateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
InvokeCalibrateUPP(
  CalibratorInfo *  theInfo,
  CalibrateUPP      userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeCalibrateUPP(CalibratorInfo * theInfo, CalibrateUPP userUPP) { return (OSErr)CALL_ONE_PARAMETER_UPP(userUPP, uppCalibrateProcInfo, theInfo); }
  #else
    #define InvokeCalibrateUPP(theInfo, userUPP) (OSErr)CALL_ONE_PARAMETER_UPP((userUPP), uppCalibrateProcInfo, (theInfo))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewCalibrateEventProc(userRoutine)                  NewCalibrateEventUPP(userRoutine)
    #define NewCanCalibrateProc(userRoutine)                    NewCanCalibrateUPP(userRoutine)
    #define NewCalibrateProc(userRoutine)                       NewCalibrateUPP(userRoutine)
    #define CallCalibrateEventProc(userRoutine, event)          InvokeCalibrateEventUPP(event, userRoutine)
    #define CallCanCalibrateProc(userRoutine, displayID, errMessage) InvokeCanCalibrateUPP(displayID, errMessage, userRoutine)
    #define CallCalibrateProc(userRoutine, theInfo)             InvokeCalibrateUPP(theInfo, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*
 *  CMCalibrateDisplay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CMCalibrateDisplay(CalibratorInfo * theInfo);



#if OLDROUTINENAMES
/* Interface for original ColorSync monitor calibrators (ColorSync 2.5.x) */
enum {
  kOldCalibratorNamePrefix      = FOUR_CHAR_CODE('Cali')
};

struct OldCalibratorInfo {
  CMDisplayIDType     displayID;              /* Contains an hDC on Win32 */
  CMProfileLocation   profileLocation;
  CalibrateEventUPP   eventProc;              /* Ignored on Win32 */
  UInt32              reserved;               /* Unused */
  UInt32              flags;                  /* Unused */
  Boolean             isGood;                 /* true or false */
  SInt8               byteFiller;             /* Unused */
};
typedef struct OldCalibratorInfo        OldCalibratorInfo;
typedef CALLBACK_API( Boolean , OldCanCalibrateProcPtr )(CMDisplayIDType displayID);
typedef CALLBACK_API( OSErr , OldCalibrateProcPtr )(OldCalibratorInfo * theInfo);
typedef STACK_UPP_TYPE(OldCanCalibrateProcPtr)                  OldCanCalibrateUPP;
typedef STACK_UPP_TYPE(OldCalibrateProcPtr)                     OldCalibrateUPP;
#if CALL_NOT_IN_CARBON
/*
 *  NewOldCanCalibrateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OldCanCalibrateUPP )
NewOldCanCalibrateUPP(OldCanCalibrateProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppOldCanCalibrateProcInfo = 0x000000D0 };  /* pascal 1_byte Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(OldCanCalibrateUPP) NewOldCanCalibrateUPP(OldCanCalibrateProcPtr userRoutine) { return (OldCanCalibrateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppOldCanCalibrateProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewOldCanCalibrateUPP(userRoutine) (OldCanCalibrateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppOldCanCalibrateProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewOldCalibrateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OldCalibrateUPP )
NewOldCalibrateUPP(OldCalibrateProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppOldCalibrateProcInfo = 0x000000E0 };  /* pascal 2_bytes Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(OldCalibrateUPP) NewOldCalibrateUPP(OldCalibrateProcPtr userRoutine) { return (OldCalibrateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppOldCalibrateProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewOldCalibrateUPP(userRoutine) (OldCalibrateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppOldCalibrateProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeOldCanCalibrateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeOldCanCalibrateUPP(OldCanCalibrateUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeOldCanCalibrateUPP(OldCanCalibrateUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeOldCanCalibrateUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeOldCalibrateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeOldCalibrateUPP(OldCalibrateUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeOldCalibrateUPP(OldCalibrateUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeOldCalibrateUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeOldCanCalibrateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
InvokeOldCanCalibrateUPP(
  CMDisplayIDType     displayID,
  OldCanCalibrateUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeOldCanCalibrateUPP(CMDisplayIDType displayID, OldCanCalibrateUPP userUPP) { return (Boolean)CALL_ONE_PARAMETER_UPP(userUPP, uppOldCanCalibrateProcInfo, displayID); }
  #else
    #define InvokeOldCanCalibrateUPP(displayID, userUPP) (Boolean)CALL_ONE_PARAMETER_UPP((userUPP), uppOldCanCalibrateProcInfo, (displayID))
  #endif
#endif

/*
 *  InvokeOldCalibrateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
InvokeOldCalibrateUPP(
  OldCalibratorInfo *  theInfo,
  OldCalibrateUPP      userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeOldCalibrateUPP(OldCalibratorInfo * theInfo, OldCalibrateUPP userUPP) { return (OSErr)CALL_ONE_PARAMETER_UPP(userUPP, uppOldCalibrateProcInfo, theInfo); }
  #else
    #define InvokeOldCalibrateUPP(theInfo, userUPP) (OSErr)CALL_ONE_PARAMETER_UPP((userUPP), uppOldCalibrateProcInfo, (theInfo))
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewOldCanCalibrateProc(userRoutine)                 NewOldCanCalibrateUPP(userRoutine)
    #define NewOldCalibrateProc(userRoutine)                    NewOldCalibrateUPP(userRoutine)
    #define CallOldCanCalibrateProc(userRoutine, displayID)     InvokeOldCanCalibrateUPP(displayID, userRoutine)
    #define CallOldCalibrateProc(userRoutine, theInfo)          InvokeOldCalibrateUPP(theInfo, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

#endif  /* OLDROUTINENAMES */


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

#endif /* __CMCALIBRATOR__ */

