/*
     File:       Threads.h
 
     Contains:   Thread Manager Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1991-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __THREADS__
#define __THREADS__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
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

/* Thread states*/
typedef UInt16 ThreadState;
enum {
  kReadyThreadState             = 0,
  kStoppedThreadState           = 1,
  kRunningThreadState           = 2
};

/* Error codes have been moved to Errors.(pah)*/

/* Thread environment characteristics*/
typedef void *                          ThreadTaskRef;
/* Thread characteristics*/
typedef UInt32 ThreadStyle;
enum {
  kCooperativeThread            = 1L << 0,
  kPreemptiveThread             = 1L << 1
};

/* Thread identifiers*/
typedef UInt32 ThreadID;
enum {
  kNoThreadID                   = 0,
  kCurrentThreadID              = 1,
  kApplicationThreadID          = 2
};

/* Options when creating a thread*/
typedef UInt32 ThreadOptions;
enum {
  kNewSuspend                   = (1 << 0),
  kUsePremadeThread             = (1 << 1),
  kCreateIfNeeded               = (1 << 2),
  kFPUNotNeeded                 = (1 << 3),
  kExactMatchThread             = (1 << 4)
};

/* Information supplied to the custom scheduler*/
struct SchedulerInfoRec {
  UInt32              InfoRecSize;
  ThreadID            CurrentThreadID;
  ThreadID            SuggestedThreadID;
  ThreadID            InterruptedCoopThreadID;
};
typedef struct SchedulerInfoRec         SchedulerInfoRec;
typedef SchedulerInfoRec *              SchedulerInfoRecPtr;

/*
    The following ProcPtrs cannot be interchanged with UniversalProcPtrs because
    of differences between 680x0 and PowerPC runtime architectures with regard to
    the implementation of the Thread Manager.
 */
typedef void *                          voidPtr;
/* Prototype for thread's entry (main) routine*/
typedef CALLBACK_API( voidPtr , ThreadEntryProcPtr )(void * threadParam);
/* Prototype for custom thread scheduler routine*/
typedef CALLBACK_API( ThreadID , ThreadSchedulerProcPtr )(SchedulerInfoRecPtr schedulerInfo);
/* Prototype for custom thread switcher routine*/
typedef CALLBACK_API( void , ThreadSwitchProcPtr )(ThreadID threadBeingSwitched, void *switchProcParam);
/* Prototype for thread termination notification routine*/
typedef CALLBACK_API( void , ThreadTerminationProcPtr )(ThreadID threadTerminated, void *terminationProcParam);
/* Prototype for debugger NewThread notification*/
typedef CALLBACK_API( void , DebuggerNewThreadProcPtr )(ThreadID threadCreated);
/* Prototype for debugger DisposeThread notification*/
typedef CALLBACK_API( void , DebuggerDisposeThreadProcPtr )(ThreadID threadDeleted);
/* Prototype for debugger schedule notification*/
typedef CALLBACK_API( ThreadID , DebuggerThreadSchedulerProcPtr )(SchedulerInfoRecPtr schedulerInfo);
typedef TVECTOR_UPP_TYPE(ThreadEntryProcPtr)                    ThreadEntryUPP;
typedef TVECTOR_UPP_TYPE(ThreadSchedulerProcPtr)                ThreadSchedulerUPP;
typedef TVECTOR_UPP_TYPE(ThreadSwitchProcPtr)                   ThreadSwitchUPP;
typedef TVECTOR_UPP_TYPE(ThreadTerminationProcPtr)              ThreadTerminationUPP;
typedef TVECTOR_UPP_TYPE(DebuggerNewThreadProcPtr)              DebuggerNewThreadUPP;
typedef TVECTOR_UPP_TYPE(DebuggerDisposeThreadProcPtr)          DebuggerDisposeThreadUPP;
typedef TVECTOR_UPP_TYPE(DebuggerThreadSchedulerProcPtr)        DebuggerThreadSchedulerUPP;
/*
 *  NewThreadEntryUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ThreadEntryUPP )
NewThreadEntryUPP(ThreadEntryProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppThreadEntryProcInfo = 0x000000F0 };  /* pascal 4_bytes Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ThreadEntryUPP) NewThreadEntryUPP(ThreadEntryProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewThreadEntryUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewThreadSchedulerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ThreadSchedulerUPP )
NewThreadSchedulerUPP(ThreadSchedulerProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppThreadSchedulerProcInfo = 0x000000F0 };  /* pascal 4_bytes Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ThreadSchedulerUPP) NewThreadSchedulerUPP(ThreadSchedulerProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewThreadSchedulerUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewThreadSwitchUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ThreadSwitchUPP )
NewThreadSwitchUPP(ThreadSwitchProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppThreadSwitchProcInfo = 0x000003C0 };  /* pascal no_return_value Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ThreadSwitchUPP) NewThreadSwitchUPP(ThreadSwitchProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewThreadSwitchUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewThreadTerminationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ThreadTerminationUPP )
NewThreadTerminationUPP(ThreadTerminationProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppThreadTerminationProcInfo = 0x000003C0 };  /* pascal no_return_value Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ThreadTerminationUPP) NewThreadTerminationUPP(ThreadTerminationProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewThreadTerminationUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewDebuggerNewThreadUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DebuggerNewThreadUPP )
NewDebuggerNewThreadUPP(DebuggerNewThreadProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDebuggerNewThreadProcInfo = 0x000000C0 };  /* pascal no_return_value Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DebuggerNewThreadUPP) NewDebuggerNewThreadUPP(DebuggerNewThreadProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewDebuggerNewThreadUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewDebuggerDisposeThreadUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DebuggerDisposeThreadUPP )
NewDebuggerDisposeThreadUPP(DebuggerDisposeThreadProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDebuggerDisposeThreadProcInfo = 0x000000C0 };  /* pascal no_return_value Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DebuggerDisposeThreadUPP) NewDebuggerDisposeThreadUPP(DebuggerDisposeThreadProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewDebuggerDisposeThreadUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewDebuggerThreadSchedulerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DebuggerThreadSchedulerUPP )
NewDebuggerThreadSchedulerUPP(DebuggerThreadSchedulerProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDebuggerThreadSchedulerProcInfo = 0x000000F0 };  /* pascal 4_bytes Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DebuggerThreadSchedulerUPP) NewDebuggerThreadSchedulerUPP(DebuggerThreadSchedulerProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewDebuggerThreadSchedulerUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  DisposeThreadEntryUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeThreadEntryUPP(ThreadEntryUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeThreadEntryUPP(ThreadEntryUPP) {}
  #else
      #define DisposeThreadEntryUPP(userUPP)
  #endif
#endif

/*
 *  DisposeThreadSchedulerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeThreadSchedulerUPP(ThreadSchedulerUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeThreadSchedulerUPP(ThreadSchedulerUPP) {}
  #else
      #define DisposeThreadSchedulerUPP(userUPP)
  #endif
#endif

/*
 *  DisposeThreadSwitchUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeThreadSwitchUPP(ThreadSwitchUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeThreadSwitchUPP(ThreadSwitchUPP) {}
  #else
      #define DisposeThreadSwitchUPP(userUPP)
  #endif
#endif

/*
 *  DisposeThreadTerminationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeThreadTerminationUPP(ThreadTerminationUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeThreadTerminationUPP(ThreadTerminationUPP) {}
  #else
      #define DisposeThreadTerminationUPP(userUPP)
  #endif
#endif

/*
 *  DisposeDebuggerNewThreadUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDebuggerNewThreadUPP(DebuggerNewThreadUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDebuggerNewThreadUPP(DebuggerNewThreadUPP) {}
  #else
      #define DisposeDebuggerNewThreadUPP(userUPP)
  #endif
#endif

/*
 *  DisposeDebuggerDisposeThreadUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDebuggerDisposeThreadUPP(DebuggerDisposeThreadUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDebuggerDisposeThreadUPP(DebuggerDisposeThreadUPP) {}
  #else
      #define DisposeDebuggerDisposeThreadUPP(userUPP)
  #endif
#endif

/*
 *  DisposeDebuggerThreadSchedulerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDebuggerThreadSchedulerUPP(DebuggerThreadSchedulerUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDebuggerThreadSchedulerUPP(DebuggerThreadSchedulerUPP) {}
  #else
      #define DisposeDebuggerThreadSchedulerUPP(userUPP)
  #endif
#endif

/*
 *  InvokeThreadEntryUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( voidPtr )
InvokeThreadEntryUPP(
  void *          threadParam,
  ThreadEntryUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(voidPtr) InvokeThreadEntryUPP(void * threadParam, ThreadEntryUPP userUPP) { return (*userUPP)(threadParam); }
  #else
      #define InvokeThreadEntryUPP(threadParam, userUPP) (*userUPP)(threadParam)
  #endif
#endif

/*
 *  InvokeThreadSchedulerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ThreadID )
InvokeThreadSchedulerUPP(
  SchedulerInfoRecPtr  schedulerInfo,
  ThreadSchedulerUPP   userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(ThreadID) InvokeThreadSchedulerUPP(SchedulerInfoRecPtr schedulerInfo, ThreadSchedulerUPP userUPP) { return (*userUPP)(schedulerInfo); }
  #else
      #define InvokeThreadSchedulerUPP(schedulerInfo, userUPP) (*userUPP)(schedulerInfo)
  #endif
#endif

/*
 *  InvokeThreadSwitchUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeThreadSwitchUPP(
  ThreadID         threadBeingSwitched,
  void *           switchProcParam,
  ThreadSwitchUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeThreadSwitchUPP(ThreadID threadBeingSwitched, void * switchProcParam, ThreadSwitchUPP userUPP) { (*userUPP)(threadBeingSwitched, switchProcParam); }
  #else
      #define InvokeThreadSwitchUPP(threadBeingSwitched, switchProcParam, userUPP) (*userUPP)(threadBeingSwitched, switchProcParam)
  #endif
#endif

/*
 *  InvokeThreadTerminationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeThreadTerminationUPP(
  ThreadID              threadTerminated,
  void *                terminationProcParam,
  ThreadTerminationUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeThreadTerminationUPP(ThreadID threadTerminated, void * terminationProcParam, ThreadTerminationUPP userUPP) { (*userUPP)(threadTerminated, terminationProcParam); }
  #else
      #define InvokeThreadTerminationUPP(threadTerminated, terminationProcParam, userUPP) (*userUPP)(threadTerminated, terminationProcParam)
  #endif
#endif

/*
 *  InvokeDebuggerNewThreadUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDebuggerNewThreadUPP(
  ThreadID              threadCreated,
  DebuggerNewThreadUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDebuggerNewThreadUPP(ThreadID threadCreated, DebuggerNewThreadUPP userUPP) { (*userUPP)(threadCreated); }
  #else
      #define InvokeDebuggerNewThreadUPP(threadCreated, userUPP) (*userUPP)(threadCreated)
  #endif
#endif

/*
 *  InvokeDebuggerDisposeThreadUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDebuggerDisposeThreadUPP(
  ThreadID                  threadDeleted,
  DebuggerDisposeThreadUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDebuggerDisposeThreadUPP(ThreadID threadDeleted, DebuggerDisposeThreadUPP userUPP) { (*userUPP)(threadDeleted); }
  #else
      #define InvokeDebuggerDisposeThreadUPP(threadDeleted, userUPP) (*userUPP)(threadDeleted)
  #endif
#endif

/*
 *  InvokeDebuggerThreadSchedulerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ThreadID )
InvokeDebuggerThreadSchedulerUPP(
  SchedulerInfoRecPtr         schedulerInfo,
  DebuggerThreadSchedulerUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(ThreadID) InvokeDebuggerThreadSchedulerUPP(SchedulerInfoRecPtr schedulerInfo, DebuggerThreadSchedulerUPP userUPP) { return (*userUPP)(schedulerInfo); }
  #else
      #define InvokeDebuggerThreadSchedulerUPP(schedulerInfo, userUPP) (*userUPP)(schedulerInfo)
  #endif
#endif

/*
   Thread Manager function pointers (TPP):
   on classic 68k use raw function pointers (same as UPP's)
   on classic PowerPC, use raw function pointers
   on classic PowerPC with OPAQUE_UPP_TYPES=1, use UPP's
   on CFM-68K, use UPP's
   on Carbon, use UPP's
*/

#if TARGET_OS_MAC && TARGET_CPU_PPC && !OPAQUE_UPP_TYPES
/* use raw function pointers*/
typedef ThreadEntryProcPtr              ThreadEntryTPP;
typedef ThreadSchedulerProcPtr          ThreadSchedulerTPP;
typedef ThreadSwitchProcPtr             ThreadSwitchTPP;
typedef ThreadTerminationProcPtr        ThreadTerminationTPP;
typedef DebuggerNewThreadProcPtr        DebuggerNewThreadTPP;
typedef DebuggerDisposeThreadProcPtr    DebuggerDisposeThreadTPP;
typedef DebuggerThreadSchedulerProcPtr  DebuggerThreadSchedulerTPP;
#else
/* use UPP's*/
typedef ThreadEntryUPP                  ThreadEntryTPP;
typedef ThreadSchedulerUPP              ThreadSchedulerTPP;
typedef ThreadSwitchUPP                 ThreadSwitchTPP;
typedef ThreadTerminationUPP            ThreadTerminationTPP;
typedef DebuggerNewThreadUPP            DebuggerNewThreadTPP;
typedef DebuggerDisposeThreadUPP        DebuggerDisposeThreadTPP;
typedef DebuggerThreadSchedulerUPP      DebuggerThreadSchedulerTPP;
#endif  /* TARGET_OS_MAC && TARGET_CPU_PPC && !OPAQUE_UPP_TYPES */

/*
 *  NewThread()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NewThread(
  ThreadStyle      threadStyle,
  ThreadEntryTPP   threadEntry,
  void *           threadParam,
  Size             stackSize,
  ThreadOptions    options,
  void **          threadResult,       /* can be NULL */
  ThreadID *       threadMade)                                THREEWORDINLINE(0x303C, 0x0E03, 0xABF2);


/*
 *  SetThreadScheduler()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetThreadScheduler(ThreadSchedulerTPP threadScheduler)        THREEWORDINLINE(0x303C, 0x0209, 0xABF2);


/*
 *  SetThreadSwitcher()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetThreadSwitcher(
  ThreadID          thread,
  ThreadSwitchTPP   threadSwitcher,
  void *            switchProcParam,
  Boolean           inOrOut)                                  THREEWORDINLINE(0x303C, 0x070A, 0xABF2);


/*
 *  SetThreadTerminator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetThreadTerminator(
  ThreadID               thread,
  ThreadTerminationTPP   threadTerminator,
  void *                 terminationProcParam)                THREEWORDINLINE(0x303C, 0x0611, 0xABF2);


/*
 *  SetDebuggerNotificationProcs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDebuggerNotificationProcs(
  DebuggerNewThreadTPP         notifyNewThread,
  DebuggerDisposeThreadTPP     notifyDisposeThread,
  DebuggerThreadSchedulerTPP   notifyThreadScheduler)         THREEWORDINLINE(0x303C, 0x060D, 0xABF2);


/*
 *  CreateThreadPool()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CreateThreadPool(
  ThreadStyle   threadStyle,
  SInt16        numToCreate,
  Size          stackSize)                                    THREEWORDINLINE(0x303C, 0x0501, 0xABF2);


/*
 *  GetFreeThreadCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetFreeThreadCount(
  ThreadStyle   threadStyle,
  SInt16 *      freeCount)                                    THREEWORDINLINE(0x303C, 0x0402, 0xABF2);


/*
 *  GetSpecificFreeThreadCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetSpecificFreeThreadCount(
  ThreadStyle   threadStyle,
  Size          stackSize,
  SInt16 *      freeCount)                                    THREEWORDINLINE(0x303C, 0x0615, 0xABF2);


/*
 *  GetDefaultThreadStackSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetDefaultThreadStackSize(
  ThreadStyle   threadStyle,
  Size *        stackSize)                                    THREEWORDINLINE(0x303C, 0x0413, 0xABF2);


/*
 *  ThreadCurrentStackSpace()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ThreadCurrentStackSpace(
  ThreadID   thread,
  UInt32 *   freeStack)                                       THREEWORDINLINE(0x303C, 0x0414, 0xABF2);


/*
 *  DisposeThread()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DisposeThread(
  ThreadID   threadToDump,
  void *     threadResult,
  Boolean    recycleThread)                                   THREEWORDINLINE(0x303C, 0x0504, 0xABF2);


/*
 *  YieldToThread()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
YieldToThread(ThreadID suggestedThread)                       THREEWORDINLINE(0x303C, 0x0205, 0xABF2);


/*
 *  YieldToAnyThread()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
YieldToAnyThread(void)                                        FOURWORDINLINE(0x42A7, 0x303C, 0x0205, 0xABF2);


/*
 *  [Mac]GetCurrentThread()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacGetCurrentThread GetCurrentThread
#endif
EXTERN_API( OSErr )
MacGetCurrentThread(ThreadID * currentThreadID)               THREEWORDINLINE(0x303C, 0x0206, 0xABF2);


/*
 *  GetThreadState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetThreadState(
  ThreadID       threadToGet,
  ThreadState *  threadState)                                 THREEWORDINLINE(0x303C, 0x0407, 0xABF2);


/*
 *  SetThreadState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetThreadState(
  ThreadID      threadToSet,
  ThreadState   newState,
  ThreadID      suggestedThread)                              THREEWORDINLINE(0x303C, 0x0508, 0xABF2);


/*
 *  SetThreadStateEndCritical()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetThreadStateEndCritical(
  ThreadID      threadToSet,
  ThreadState   newState,
  ThreadID      suggestedThread)                              THREEWORDINLINE(0x303C, 0x0512, 0xABF2);


/*
 *  ThreadBeginCritical()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ThreadBeginCritical(void)                                     THREEWORDINLINE(0x303C, 0x000B, 0xABF2);


/*
 *  ThreadEndCritical()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ThreadEndCritical(void)                                       THREEWORDINLINE(0x303C, 0x000C, 0xABF2);


/*
 *  GetThreadCurrentTaskRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetThreadCurrentTaskRef(ThreadTaskRef * threadTRef)           THREEWORDINLINE(0x303C, 0x020E, 0xABF2);


/*
 *  GetThreadStateGivenTaskRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetThreadStateGivenTaskRef(
  ThreadTaskRef   threadTRef,
  ThreadID        threadToGet,
  ThreadState *   threadState)                                THREEWORDINLINE(0x303C, 0x060F, 0xABF2);


/*
 *  SetThreadReadyGivenTaskRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ThreadsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetThreadReadyGivenTaskRef(
  ThreadTaskRef   threadTRef,
  ThreadID        threadToSet)                                THREEWORDINLINE(0x303C, 0x0410, 0xABF2);



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

#endif /* __THREADS__ */

