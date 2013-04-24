/*
     File:       CMTypes.h
 
     Contains:   ColorSync types
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/


#ifndef __CMTYPES__
#define __CMTYPES__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif


/* Standard type for ColorSync and other system error codes */


#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

typedef long                            CMError;
/* Abstract data type for memory-based Profile */
typedef struct OpaqueCMProfileRef*      CMProfileRef;
/* Abstract data type for Profile search result */
typedef struct OpaqueCMProfileSearchRef*  CMProfileSearchRef;
/* Abstract data type for BeginMatching(...) reference */
typedef struct OpaqueCMMatchRef*        CMMatchRef;
/* Abstract data type for ColorWorld reference */
typedef struct OpaqueCMWorldRef*        CMWorldRef;
/* Data type for ColorSync DisplayID reference */
/* On 8 & 9 this is a AVIDType */
/* On X this is a CGSDisplayID */
typedef UInt32                          CMDisplayIDType;

/* Caller-supplied flatten function */
typedef CALLBACK_API( OSErr , CMFlattenProcPtr )(long command, long *size, void *data, void *refCon);
/* Caller-supplied progress function for Bitmap & PixMap matching routines */
typedef CALLBACK_API( Boolean , CMBitmapCallBackProcPtr )(long progress, void *refCon);
/* Caller-supplied progress function for NCMMConcatInit & NCMMNewLinkProfile routines */
typedef CALLBACK_API( Boolean , CMConcatCallBackProcPtr )(long progress, void *refCon);
/* Caller-supplied filter function for Profile search */
typedef CALLBACK_API( Boolean , CMProfileFilterProcPtr )(CMProfileRef prof, void *refCon);
/* Caller-supplied function for profile access */
typedef CALLBACK_API( OSErr , CMProfileAccessProcPtr )(long command, long offset, long *size, void *data, void *refCon);
typedef STACK_UPP_TYPE(CMFlattenProcPtr)                        CMFlattenUPP;
typedef STACK_UPP_TYPE(CMBitmapCallBackProcPtr)                 CMBitmapCallBackUPP;
typedef STACK_UPP_TYPE(CMConcatCallBackProcPtr)                 CMConcatCallBackUPP;
typedef STACK_UPP_TYPE(CMProfileFilterProcPtr)                  CMProfileFilterUPP;
typedef STACK_UPP_TYPE(CMProfileAccessProcPtr)                  CMProfileAccessUPP;
/*
 *  NewCMFlattenUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CMFlattenUPP )
NewCMFlattenUPP(CMFlattenProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppCMFlattenProcInfo = 0x00003FE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(CMFlattenUPP) NewCMFlattenUPP(CMFlattenProcPtr userRoutine) { return (CMFlattenUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCMFlattenProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewCMFlattenUPP(userRoutine) (CMFlattenUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCMFlattenProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewCMBitmapCallBackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CMBitmapCallBackUPP )
NewCMBitmapCallBackUPP(CMBitmapCallBackProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppCMBitmapCallBackProcInfo = 0x000003D0 };  /* pascal 1_byte Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(CMBitmapCallBackUPP) NewCMBitmapCallBackUPP(CMBitmapCallBackProcPtr userRoutine) { return (CMBitmapCallBackUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCMBitmapCallBackProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewCMBitmapCallBackUPP(userRoutine) (CMBitmapCallBackUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCMBitmapCallBackProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewCMConcatCallBackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CMConcatCallBackUPP )
NewCMConcatCallBackUPP(CMConcatCallBackProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppCMConcatCallBackProcInfo = 0x000003D0 };  /* pascal 1_byte Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(CMConcatCallBackUPP) NewCMConcatCallBackUPP(CMConcatCallBackProcPtr userRoutine) { return (CMConcatCallBackUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCMConcatCallBackProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewCMConcatCallBackUPP(userRoutine) (CMConcatCallBackUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCMConcatCallBackProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewCMProfileFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CMProfileFilterUPP )
NewCMProfileFilterUPP(CMProfileFilterProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppCMProfileFilterProcInfo = 0x000003D0 };  /* pascal 1_byte Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(CMProfileFilterUPP) NewCMProfileFilterUPP(CMProfileFilterProcPtr userRoutine) { return (CMProfileFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCMProfileFilterProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewCMProfileFilterUPP(userRoutine) (CMProfileFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCMProfileFilterProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewCMProfileAccessUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CMProfileAccessUPP )
NewCMProfileAccessUPP(CMProfileAccessProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppCMProfileAccessProcInfo = 0x0000FFE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(CMProfileAccessUPP) NewCMProfileAccessUPP(CMProfileAccessProcPtr userRoutine) { return (CMProfileAccessUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCMProfileAccessProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewCMProfileAccessUPP(userRoutine) (CMProfileAccessUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCMProfileAccessProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeCMFlattenUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeCMFlattenUPP(CMFlattenUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeCMFlattenUPP(CMFlattenUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeCMFlattenUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeCMBitmapCallBackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeCMBitmapCallBackUPP(CMBitmapCallBackUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeCMBitmapCallBackUPP(CMBitmapCallBackUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeCMBitmapCallBackUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeCMConcatCallBackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeCMConcatCallBackUPP(CMConcatCallBackUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeCMConcatCallBackUPP(CMConcatCallBackUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeCMConcatCallBackUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeCMProfileFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeCMProfileFilterUPP(CMProfileFilterUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeCMProfileFilterUPP(CMProfileFilterUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeCMProfileFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeCMProfileAccessUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeCMProfileAccessUPP(CMProfileAccessUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeCMProfileAccessUPP(CMProfileAccessUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeCMProfileAccessUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeCMFlattenUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeCMFlattenUPP(
  long          command,
  long *        size,
  void *        data,
  void *        refCon,
  CMFlattenUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeCMFlattenUPP(long command, long * size, void * data, void * refCon, CMFlattenUPP userUPP) { return (OSErr)CALL_FOUR_PARAMETER_UPP(userUPP, uppCMFlattenProcInfo, command, size, data, refCon); }
  #else
    #define InvokeCMFlattenUPP(command, size, data, refCon, userUPP) (OSErr)CALL_FOUR_PARAMETER_UPP((userUPP), uppCMFlattenProcInfo, (command), (size), (data), (refCon))
  #endif
#endif

/*
 *  InvokeCMBitmapCallBackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeCMBitmapCallBackUPP(
  long                 progress,
  void *               refCon,
  CMBitmapCallBackUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeCMBitmapCallBackUPP(long progress, void * refCon, CMBitmapCallBackUPP userUPP) { return (Boolean)CALL_TWO_PARAMETER_UPP(userUPP, uppCMBitmapCallBackProcInfo, progress, refCon); }
  #else
    #define InvokeCMBitmapCallBackUPP(progress, refCon, userUPP) (Boolean)CALL_TWO_PARAMETER_UPP((userUPP), uppCMBitmapCallBackProcInfo, (progress), (refCon))
  #endif
#endif

/*
 *  InvokeCMConcatCallBackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeCMConcatCallBackUPP(
  long                 progress,
  void *               refCon,
  CMConcatCallBackUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeCMConcatCallBackUPP(long progress, void * refCon, CMConcatCallBackUPP userUPP) { return (Boolean)CALL_TWO_PARAMETER_UPP(userUPP, uppCMConcatCallBackProcInfo, progress, refCon); }
  #else
    #define InvokeCMConcatCallBackUPP(progress, refCon, userUPP) (Boolean)CALL_TWO_PARAMETER_UPP((userUPP), uppCMConcatCallBackProcInfo, (progress), (refCon))
  #endif
#endif

/*
 *  InvokeCMProfileFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeCMProfileFilterUPP(
  CMProfileRef        prof,
  void *              refCon,
  CMProfileFilterUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeCMProfileFilterUPP(CMProfileRef prof, void * refCon, CMProfileFilterUPP userUPP) { return (Boolean)CALL_TWO_PARAMETER_UPP(userUPP, uppCMProfileFilterProcInfo, prof, refCon); }
  #else
    #define InvokeCMProfileFilterUPP(prof, refCon, userUPP) (Boolean)CALL_TWO_PARAMETER_UPP((userUPP), uppCMProfileFilterProcInfo, (prof), (refCon))
  #endif
#endif

/*
 *  InvokeCMProfileAccessUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeCMProfileAccessUPP(
  long                command,
  long                offset,
  long *              size,
  void *              data,
  void *              refCon,
  CMProfileAccessUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeCMProfileAccessUPP(long command, long offset, long * size, void * data, void * refCon, CMProfileAccessUPP userUPP) { return (OSErr)CALL_FIVE_PARAMETER_UPP(userUPP, uppCMProfileAccessProcInfo, command, offset, size, data, refCon); }
  #else
    #define InvokeCMProfileAccessUPP(command, offset, size, data, refCon, userUPP) (OSErr)CALL_FIVE_PARAMETER_UPP((userUPP), uppCMProfileAccessProcInfo, (command), (offset), (size), (data), (refCon))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewCMFlattenProc(userRoutine)                       NewCMFlattenUPP(userRoutine)
    #define NewCMBitmapCallBackProc(userRoutine)                NewCMBitmapCallBackUPP(userRoutine)
    #define NewCMConcatCallBackProc(userRoutine)                NewCMConcatCallBackUPP(userRoutine)
    #define NewCMProfileFilterProc(userRoutine)                 NewCMProfileFilterUPP(userRoutine)
    #define NewCMProfileAccessProc(userRoutine)                 NewCMProfileAccessUPP(userRoutine)
    #define CallCMFlattenProc(userRoutine, command, size, data, refCon) InvokeCMFlattenUPP(command, size, data, refCon, userRoutine)
    #define CallCMBitmapCallBackProc(userRoutine, progress, refCon) InvokeCMBitmapCallBackUPP(progress, refCon, userRoutine)
    #define CallCMConcatCallBackProc(userRoutine, progress, refCon) InvokeCMConcatCallBackUPP(progress, refCon, userRoutine)
    #define CallCMProfileFilterProc(userRoutine, prof, refCon)  InvokeCMProfileFilterUPP(prof, refCon, userRoutine)
    #define CallCMProfileAccessProc(userRoutine, command, offset, size, data, refCon) InvokeCMProfileAccessUPP(command, offset, size, data, refCon, userRoutine)
#endif /* CALL_NOT_IN_CARBON */


#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CMTYPES__ */

