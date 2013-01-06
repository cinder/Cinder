/*
     File:       QTML.h
 
     Contains:   QuickTime Cross-platform specific interfaces
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1997-2007 by Apple Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __QTML__
#define __QTML__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

#ifndef __FILES__
#include <Files.h>
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

/* QuickTime is not available to 64-bit clients */

#if !__LP64__

/*
 *  QTMLYieldCPU()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 3.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
QTMLYieldCPU(void);


/* QTMLYieldCPUTime flags*/
enum {
  kQTMLHandlePortEvents         = (1L << 0) /* ask for event handling during the yield*/
};

/*
 *  QTMLYieldCPUTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 3.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
QTMLYieldCPUTime(
  long            milliSeconds,
  unsigned long   flags);


typedef struct OpaqueQTMLMutex*         QTMLMutex;
typedef struct OpaqueQTMLSyncVar*       QTMLSyncVar;
typedef QTMLSyncVar *                   QTMLSyncVarPtr;
/* InitializeQTML flags*/
enum {
  kInitializeQTMLNoSoundFlag    = (1L << 0), /* flag for requesting no sound when calling InitializeQTML*/
  kInitializeQTMLUseGDIFlag     = (1L << 1), /* flag for requesting GDI when calling InitializeQTML*/
  kInitializeQTMLDisableDirectSound = (1L << 2), /* disables QTML's use of DirectSound*/
  kInitializeQTMLUseExclusiveFullScreenModeFlag = (1L << 3), /* later than QTML 3.0: qtml starts up in exclusive full screen mode*/
  kInitializeQTMLDisableDDClippers = (1L << 4), /* flag for requesting QTML not to use DirectDraw clipper objects; QTML 5.0 and later*/
  kInitializeQTMLEnableDoubleBufferedSurface = (1L << 6) /* flag for requesting QuickTime use a double-buffered destination surface; QT6.4 and later*/
};

#if CALL_NOT_IN_CARBON
/*
 *  InitializeQTML()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
InitializeQTML(long flag);


/*
 *  TerminateQTML()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
TerminateQTML(void);



/* CreatePortAssociation flags*/
#endif  /* CALL_NOT_IN_CARBON */

enum {
  kQTMLNoIdleEvents             = (1L << 1), /* ask for a non-auto-idled port to be created*/
  kQTMLNoDoubleBufferPort       = (1L << 2) /* ask for QTML not to double-buffer this port*/
};

#define kQTMLIsDoubleBuffered "UsesDoubleBuffer"
#if CALL_NOT_IN_CARBON
/*
 *  CreatePortAssociation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( GrafPtr )
CreatePortAssociation(
  void *  theWnd,
  Ptr     storage,
  long    flags);


/*
 *  DestroyPortAssociation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
DestroyPortAssociation(CGrafPtr cgp);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  QTMLGrabMutex()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 3.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
QTMLGrabMutex(QTMLMutex mu);


/*
 *  QTMLTryGrabMutex()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 4.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.1 and later
 */
EXTERN_API( Boolean )
QTMLTryGrabMutex(QTMLMutex mu);


/*
 *  QTMLReturnMutex()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 3.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
QTMLReturnMutex(QTMLMutex mu);


/*
 *  QTMLCreateMutex()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 3.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( QTMLMutex )
QTMLCreateMutex(void);


/*
 *  QTMLDestroyMutex()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 3.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
QTMLDestroyMutex(QTMLMutex mu);



#if CALL_NOT_IN_CARBON
/*
 *  QTMLCreateSyncVar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( QTMLSyncVarPtr )
QTMLCreateSyncVar(void);


/*
 *  QTMLDestroySyncVar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
QTMLDestroySyncVar(QTMLSyncVarPtr p);


/*
 *  QTMLTestAndSetSyncVar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( long )
QTMLTestAndSetSyncVar(QTMLSyncVarPtr sync);


/*
 *  QTMLWaitAndSetSyncVar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
QTMLWaitAndSetSyncVar(QTMLSyncVarPtr sync);


/*
 *  QTMLResetSyncVar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
QTMLResetSyncVar(QTMLSyncVarPtr sync);



/*
 *  InitializeQHdr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
InitializeQHdr(QHdr * qhdr);


/*
 *  TerminateQHdr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
TerminateQHdr(QHdr * qhdr);



/*
 *  QTMLAcquireWindowList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
QTMLAcquireWindowList(void);


/*
 *  QTMLReleaseWindowList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
QTMLReleaseWindowList(void);


/*
   These routines are here to support "interrupt level" code
      These are dangerous routines, only use if you know what you are doing.
*/

/*
 *  QTMLRegisterInterruptSafeThread()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( long )
QTMLRegisterInterruptSafeThread(
  unsigned long   threadID,
  void *          threadInfo);


/*
 *  QTMLUnregisterInterruptSafeThread()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( long )
QTMLUnregisterInterruptSafeThread(unsigned long threadID);



/*
 *  NativeEventToMacEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( long )
NativeEventToMacEvent(
  void *         nativeEvent,
  EventRecord *  macEvent);


#endif  /* CALL_NOT_IN_CARBON */

#if TARGET_OS_WIN32
#if CALL_NOT_IN_CARBON
/*
 *  WinEventToMacEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( long )
WinEventToMacEvent(
  void *         winMsg,
  EventRecord *  macEvent);


#define WinEventToMacEvent  NativeEventToMacEvent
/*
 *  IsTaskBarVisible()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( Boolean )
IsTaskBarVisible(void);


/*
 *  ShowHideTaskBar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
ShowHideTaskBar(Boolean showIt);


#endif  /* CALL_NOT_IN_CARBON */

enum {
  kDDSurfaceLocked              = (1L << 0),
  kDDSurfaceStatic              = (1L << 1)
};

#if CALL_NOT_IN_CARBON
/*
 *  QTGetDDObject()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
QTGetDDObject(void ** lpDDObject);


/*
 *  QTSetDDObject()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
QTSetDDObject(void * lpNewDDObject);


/*
 *  QTSetDDPrimarySurface()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
QTSetDDPrimarySurface(
  void *          lpNewDDSurface,
  unsigned long   flags);



/*
 *  QTMLGetVolumeRootPath()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
QTMLGetVolumeRootPath(
  char *          fullPath,
  char *          volumeRootPath,
  unsigned long   volumeRootLen);



/*
 *  QTMLSetWindowWndProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
QTMLSetWindowWndProc(
  WindowRef   theWindow,
  void *      windowProc);


/*
 *  QTMLGetWindowWndProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void * )
QTMLGetWindowWndProc(WindowRef theWindow);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_OS_WIN32 */

#if CALL_NOT_IN_CARBON
/*
 *  QTMLGetCanonicalPathName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
QTMLGetCanonicalPathName(
  char *          inName,
  char *          outName,
  unsigned long   outLen);


#endif  /* CALL_NOT_IN_CARBON */

enum {
  kFullNativePath               = 0,
  kFileNameOnly                 = (1 << 0),
  kDirectoryPathOnly            = (1 << 1),
  kUFSFullPathName              = (1 << 2),
  kTryVDIMask                   = (1 << 3), /*    Used in NativePathNameToFSSpec to specify to search VDI mountpoints*/
  kFullPathSpecifiedMask        = (1 << 4) /*    the passed in name is a fully qualified full path*/
};

#if CALL_NOT_IN_CARBON
/*
 *  FSSpecToNativePathName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
FSSpecToNativePathName(
  const FSSpec *  inFile,
  char *          outName,
  unsigned long   outLen,
  long            flags);


#endif  /* CALL_NOT_IN_CARBON */

enum {
  kErrorIfFileNotFound          = 1L << 31
};

#if CALL_NOT_IN_CARBON
/*
 *  NativePathNameToFSSpec()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
NativePathNameToFSSpec(
  char *    inName,
  FSSpec *  outFile,
  long      flags);


/*
 *  QTGetAliasInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( OSErr )
QTGetAliasInfo(
  AliasHandle     alias,
  AliasInfoType   index,
  char *          outBuf,
  long            bufLen,
  long *          outLen,
  unsigned long   flags);



#endif // !__LP64__

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

#endif /* __QTML__ */

