/*
     File:       Retrace.h
 
     Contains:   Vertical Retrace Interfaces.
 
     Version:    Technology: System 7.5
                 Release:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-1993, 1995-1999 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __RETRACE__
#define __RETRACE__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
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

typedef struct VBLTask                  VBLTask;
typedef VBLTask *                       VBLTaskPtr;
typedef CALLBACK_API_REGISTER68K( void , VBLProcPtr, (VBLTaskPtr vblTaskPtr) );
typedef REGISTER_UPP_TYPE(VBLProcPtr)                           VBLUPP;
struct VBLTask {
  QElemPtr            qLink;
  short               qType;
  VBLUPP              vblAddr;
  short               vblCount;
  short               vblPhase;
};

#if CALL_NOT_IN_CARBON
/*
 *  NewVBLUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( VBLUPP )
NewVBLUPP(VBLProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppVBLProcInfo = 0x00009802 };  /* register no_return_value Func(4_bytes:A0) */
  #ifdef __cplusplus
    inline DEFINE_API_C(VBLUPP) NewVBLUPP(VBLProcPtr userRoutine) { return (VBLUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppVBLProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewVBLUPP(userRoutine) (VBLUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppVBLProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeVBLUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeVBLUPP(VBLUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeVBLUPP(VBLUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeVBLUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeVBLUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter InvokeVBLUPP(__A0, __A1)
#endif
EXTERN_API_C( void )
InvokeVBLUPP(
  VBLTaskPtr  vblTaskPtr,
  VBLUPP      userUPP)                                        ONEWORDINLINE(0x4E91);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeVBLUPP(VBLTaskPtr vblTaskPtr, VBLUPP userUPP) { CALL_ONE_PARAMETER_UPP(userUPP, uppVBLProcInfo, vblTaskPtr); }
  #else
    #define InvokeVBLUPP(vblTaskPtr, userUPP) CALL_ONE_PARAMETER_UPP((userUPP), uppVBLProcInfo, (vblTaskPtr))
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewVBLProc(userRoutine)                             NewVBLUPP(userRoutine)
    #define CallVBLProc(userRoutine, vblTaskPtr)                InvokeVBLUPP(vblTaskPtr, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON
#if CALL_NOT_IN_CARBON
/*
 *  GetVBLQHdr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( QHdrPtr )
GetVBLQHdr(void)                                              THREEWORDINLINE(0x2EBC, 0x0000, 0x0160);


/*
 *  SlotVInstall()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __D0 SlotVInstall(__A0, __D0)
#endif
EXTERN_API( OSErr )
SlotVInstall(
  QElemPtr   vblBlockPtr,
  short      theSlot)                                         ONEWORDINLINE(0xA06F);


/*
 *  SlotVRemove()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __D0 SlotVRemove(__A0, __D0)
#endif
EXTERN_API( OSErr )
SlotVRemove(
  QElemPtr   vblBlockPtr,
  short      theSlot)                                         ONEWORDINLINE(0xA070);


/*
 *  AttachVBL()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __D0 AttachVBL(__D0)
#endif
EXTERN_API( OSErr )
AttachVBL(short theSlot)                                      ONEWORDINLINE(0xA071);


/*
 *  DoVBLTask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __D0 DoVBLTask(__D0)
#endif
EXTERN_API( OSErr )
DoVBLTask(short theSlot)                                      ONEWORDINLINE(0xA072);


/*
 *  VInstall()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __D0 VInstall(__A0)
#endif
EXTERN_API( OSErr )
VInstall(QElemPtr vblTaskPtr)                                 ONEWORDINLINE(0xA033);


/*
 *  VRemove()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __D0 VRemove(__A0)
#endif
EXTERN_API( OSErr )
VRemove(QElemPtr vblTaskPtr)                                  ONEWORDINLINE(0xA034);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* CALL_NOT_IN_CARBON */


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

#endif /* __RETRACE__ */

