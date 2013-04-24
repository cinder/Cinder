/*
     File:       TSMTE.h
 
     Contains:   Text Services Managerfor TextEdit Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1991-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __TSMTE__
#define __TSMTE__

#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

#ifndef __DIALOGS__
#include <Dialogs.h>
#endif

#ifndef __TEXTSERVICES__
#include <TextServices.h>
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

/* signature, interface types*/
enum {
  kTSMTESignature               = FOUR_CHAR_CODE('tmTE'),
  kTSMTEInterfaceType           = FOUR_CHAR_CODE('tmTE')
};

/*
    In Carbon, since DialogRef is opaque, the TSMDialogRecord is removed.
    Only one kind of TSMTE dialog remains, with extended data managed by TSMTE.
    Use kTSMTESignature for the dialog refCon, and use the accessors below,
    i.e. GetTSMTEDialogTSMTERecHandle, to get at the old TSMDialogRecord info.
*/
#if CALL_NOT_IN_CARBON
enum {
  kTSMTEDialog                  = FOUR_CHAR_CODE('tmDI')
};

#endif  /* CALL_NOT_IN_CARBON */

/* update flag for TSMTERec*/
enum {
  kTSMTEAutoScroll              = 1
};


/* callback procedure definitions*/

typedef CALLBACK_API( void , TSMTEPreUpdateProcPtr )(TEHandle textH, long refCon);
typedef CALLBACK_API( void , TSMTEPostUpdateProcPtr )(TEHandle textH, long fixLen, long inputAreaStart, long inputAreaEnd, long pinStart, long pinEnd, long refCon);
typedef STACK_UPP_TYPE(TSMTEPreUpdateProcPtr)                   TSMTEPreUpdateUPP;
typedef STACK_UPP_TYPE(TSMTEPostUpdateProcPtr)                  TSMTEPostUpdateUPP;


/* data types*/
struct TSMTERec {
  TEHandle            textH;
  TSMTEPreUpdateUPP   preUpdateProc;
  TSMTEPostUpdateUPP  postUpdateProc;
  long                updateFlag;
  long                refCon;
};
typedef struct TSMTERec                 TSMTERec;
typedef TSMTERec *                      TSMTERecPtr;
typedef TSMTERecPtr *                   TSMTERecHandle;
#if !OPAQUE_TOOLBOX_STRUCTS
struct TSMDialogRecord {
  DialogRecord        fDialog;
  TSMDocumentID       fDocID;
  TSMTERecHandle      fTSMTERecH;
  long                fTSMTERsvd[3];          /* reserved*/
};
typedef struct TSMDialogRecord          TSMDialogRecord;
typedef TSMDialogRecord *               TSMDialogPtr;
typedef TSMDialogPtr                    TSMDialogPeek;
#endif  /* !OPAQUE_TOOLBOX_STRUCTS */

/*
 *  NewTSMTEPreUpdateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( TSMTEPreUpdateUPP )
NewTSMTEPreUpdateUPP(TSMTEPreUpdateProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppTSMTEPreUpdateProcInfo = 0x000003C0 };  /* pascal no_return_value Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(TSMTEPreUpdateUPP) NewTSMTEPreUpdateUPP(TSMTEPreUpdateProcPtr userRoutine) { return (TSMTEPreUpdateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTSMTEPreUpdateProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewTSMTEPreUpdateUPP(userRoutine) (TSMTEPreUpdateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTSMTEPreUpdateProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewTSMTEPostUpdateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( TSMTEPostUpdateUPP )
NewTSMTEPostUpdateUPP(TSMTEPostUpdateProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppTSMTEPostUpdateProcInfo = 0x000FFFC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(TSMTEPostUpdateUPP) NewTSMTEPostUpdateUPP(TSMTEPostUpdateProcPtr userRoutine) { return (TSMTEPostUpdateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTSMTEPostUpdateProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewTSMTEPostUpdateUPP(userRoutine) (TSMTEPostUpdateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTSMTEPostUpdateProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeTSMTEPreUpdateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeTSMTEPreUpdateUPP(TSMTEPreUpdateUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeTSMTEPreUpdateUPP(TSMTEPreUpdateUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeTSMTEPreUpdateUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeTSMTEPostUpdateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeTSMTEPostUpdateUPP(TSMTEPostUpdateUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeTSMTEPostUpdateUPP(TSMTEPostUpdateUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeTSMTEPostUpdateUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeTSMTEPreUpdateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeTSMTEPreUpdateUPP(
  TEHandle           textH,
  long               refCon,
  TSMTEPreUpdateUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeTSMTEPreUpdateUPP(TEHandle textH, long refCon, TSMTEPreUpdateUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppTSMTEPreUpdateProcInfo, textH, refCon); }
  #else
    #define InvokeTSMTEPreUpdateUPP(textH, refCon, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppTSMTEPreUpdateProcInfo, (textH), (refCon))
  #endif
#endif

/*
 *  InvokeTSMTEPostUpdateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeTSMTEPostUpdateUPP(
  TEHandle            textH,
  long                fixLen,
  long                inputAreaStart,
  long                inputAreaEnd,
  long                pinStart,
  long                pinEnd,
  long                refCon,
  TSMTEPostUpdateUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeTSMTEPostUpdateUPP(TEHandle textH, long fixLen, long inputAreaStart, long inputAreaEnd, long pinStart, long pinEnd, long refCon, TSMTEPostUpdateUPP userUPP) { CALL_SEVEN_PARAMETER_UPP(userUPP, uppTSMTEPostUpdateProcInfo, textH, fixLen, inputAreaStart, inputAreaEnd, pinStart, pinEnd, refCon); }
  #else
    #define InvokeTSMTEPostUpdateUPP(textH, fixLen, inputAreaStart, inputAreaEnd, pinStart, pinEnd, refCon, userUPP) CALL_SEVEN_PARAMETER_UPP((userUPP), uppTSMTEPostUpdateProcInfo, (textH), (fixLen), (inputAreaStart), (inputAreaEnd), (pinStart), (pinEnd), (refCon))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewTSMTEPreUpdateProc(userRoutine)                  NewTSMTEPreUpdateUPP(userRoutine)
    #define NewTSMTEPostUpdateProc(userRoutine)                 NewTSMTEPostUpdateUPP(userRoutine)
    #define CallTSMTEPreUpdateProc(userRoutine, textH, refCon)  InvokeTSMTEPreUpdateUPP(textH, refCon, userRoutine)
    #define CallTSMTEPostUpdateProc(userRoutine, textH, fixLen, inputAreaStart, inputAreaEnd, pinStart, pinEnd, refCon) InvokeTSMTEPostUpdateUPP(textH, fixLen, inputAreaStart, inputAreaEnd, pinStart, pinEnd, refCon, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

#if ACCESSOR_CALLS_ARE_FUNCTIONS
/*
 *  IsTSMTEDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0.2 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsTSMTEDialog(DialogRef dialog);


/* Getters */
/*
 *  GetTSMTEDialogDocumentID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0.2 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( TSMDocumentID )
GetTSMTEDialogDocumentID(DialogRef dialog);


/*
 *  GetTSMTEDialogTSMTERecHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0.2 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( TSMTERecHandle )
GetTSMTEDialogTSMTERecHandle(DialogRef dialog);


/* Setters */
/*
 *  SetTSMTEDialogDocumentID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0.2 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetTSMTEDialogDocumentID(
  DialogRef       dialog,
  TSMDocumentID   documentID);


/*
 *  SetTSMTEDialogTSMTERecHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0.2 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetTSMTEDialogTSMTERecHandle(
  DialogRef        dialog,
  TSMTERecHandle   tsmteRecHandle);


#endif  /* ACCESSOR_CALLS_ARE_FUNCTIONS */


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

#endif /* __TSMTE__ */

