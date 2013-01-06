/*
     File:       Resources.h
 
     Contains:   Resource Manager Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __RESOURCES__
#define __RESOURCES__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
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


enum {
  resSysHeap                    = 64,   /*System or application heap?*/
  resPurgeable                  = 32,   /*Purgeable resource?*/
  resLocked                     = 16,   /*Load it in locked?*/
  resProtected                  = 8,    /*Protected?*/
  resPreload                    = 4,    /*Load in on OpenResFile?*/
  resChanged                    = 2,    /*Resource changed?*/
  mapReadOnly                   = 128,  /*Resource file read-only*/
  mapCompact                    = 64,   /*Compact resource file*/
  mapChanged                    = 32    /*Write map out at update*/
};

enum {
  resSysRefBit                  = 7,    /*reference to system/local reference*/
  resSysHeapBit                 = 6,    /*In system/in application heap*/
  resPurgeableBit               = 5,    /*Purgeable/not purgeable*/
  resLockedBit                  = 4,    /*Locked/not locked*/
  resProtectedBit               = 3,    /*Protected/not protected*/
  resPreloadBit                 = 2,    /*Read in at OpenResource?*/
  resChangedBit                 = 1,    /*Existing resource changed since last update*/
  mapReadOnlyBit                = 7,    /*is this file read-only?*/
  mapCompactBit                 = 6,    /*Is a compact necessary?*/
  mapChangedBit                 = 5     /*Is it necessary to write map?*/
};

enum {
  kResFileNotOpened             = -1,   /*ref num return as error when opening a resource file*/
  kSystemResFile                = 0     /*this is the default ref num to the system file*/
};


typedef CALLBACK_API_REGISTER68K( void , ResErrProcPtr, (OSErr thErr) );
typedef REGISTER_UPP_TYPE(ResErrProcPtr)                        ResErrUPP;
/*
 *  NewResErrUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ResErrUPP )
NewResErrUPP(ResErrProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppResErrProcInfo = 0x00001002 };  /* register no_return_value Func(2_bytes:D0) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ResErrUPP) NewResErrUPP(ResErrProcPtr userRoutine) { return (ResErrUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppResErrProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewResErrUPP(userRoutine) (ResErrUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppResErrProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeResErrUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeResErrUPP(ResErrUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeResErrUPP(ResErrUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeResErrUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeResErrUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter InvokeResErrUPP(__D0, __A0)
#endif
EXTERN_API_C( void )
InvokeResErrUPP(
  OSErr      thErr,
  ResErrUPP  userUPP)                                         ONEWORDINLINE(0x4E90);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeResErrUPP(OSErr thErr, ResErrUPP userUPP) { CALL_ONE_PARAMETER_UPP(userUPP, uppResErrProcInfo, thErr); }
  #else
    #define InvokeResErrUPP(thErr, userUPP) CALL_ONE_PARAMETER_UPP((userUPP), uppResErrProcInfo, (thErr))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewResErrProc(userRoutine)                          NewResErrUPP(userRoutine)
    #define CallResErrProc(userRoutine, thErr)                  InvokeResErrUPP(thErr, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/* QuickTime 3.0*/
typedef CALLBACK_API( OSErr , ResourceEndianFilterPtr )(Handle theResource, Boolean currentlyNativeEndian);
#if CALL_NOT_IN_CARBON
/*
 *  InitResources()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
InitResources(void)                                           ONEWORDINLINE(0xA995);


/*
 *  RsrcZoneInit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
RsrcZoneInit(void)                                            ONEWORDINLINE(0xA996);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  CloseResFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
CloseResFile(short refNum)                                    ONEWORDINLINE(0xA99A);


/*
 *  ResError()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ResError(void)                                                ONEWORDINLINE(0xA9AF);


/*
 *  CurResFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
CurResFile(void)                                              ONEWORDINLINE(0xA994);


/*
 *  HomeResFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
HomeResFile(Handle theResource)                               ONEWORDINLINE(0xA9A4);


#if CALL_NOT_IN_CARBON
/*
 *  CreateResFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
CreateResFile(ConstStr255Param fileName)                      ONEWORDINLINE(0xA9B1);


/*
 *  OpenResFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( short )
OpenResFile(ConstStr255Param fileName)                        ONEWORDINLINE(0xA997);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  UseResFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
UseResFile(short refNum)                                      ONEWORDINLINE(0xA998);


/*
 *  CountTypes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
CountTypes(void)                                              ONEWORDINLINE(0xA99E);


/*
 *  Count1Types()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
Count1Types(void)                                             ONEWORDINLINE(0xA81C);


/*
 *  GetIndType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetIndType(
  ResType *  theType,
  short      index)                                           ONEWORDINLINE(0xA99F);


/*
 *  Get1IndType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
Get1IndType(
  ResType *  theType,
  short      index)                                           ONEWORDINLINE(0xA80F);


/*
 *  SetResLoad()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetResLoad(Boolean load)                                      ONEWORDINLINE(0xA99B);


/*
 *  CountResources()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
CountResources(ResType theType)                               ONEWORDINLINE(0xA99C);


/*
 *  Count1Resources()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
Count1Resources(ResType theType)                              ONEWORDINLINE(0xA80D);


/*
 *  GetIndResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
GetIndResource(
  ResType   theType,
  short     index)                                            ONEWORDINLINE(0xA99D);


/*
 *  Get1IndResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
Get1IndResource(
  ResType   theType,
  short     index)                                            ONEWORDINLINE(0xA80E);


/*
 *  GetResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
GetResource(
  ResType   theType,
  short     theID)                                            ONEWORDINLINE(0xA9A0);


/*
 *  Get1Resource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
Get1Resource(
  ResType   theType,
  short     theID)                                            ONEWORDINLINE(0xA81F);


/*
 *  GetNamedResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
GetNamedResource(
  ResType            theType,
  ConstStr255Param   name)                                    ONEWORDINLINE(0xA9A1);


/*
 *  Get1NamedResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
Get1NamedResource(
  ResType            theType,
  ConstStr255Param   name)                                    ONEWORDINLINE(0xA820);


/*
 *  [Mac]LoadResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacLoadResource LoadResource
#endif
EXTERN_API( void )
MacLoadResource(Handle theResource)                           ONEWORDINLINE(0xA9A2);


/*
 *  ReleaseResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ReleaseResource(Handle theResource)                           ONEWORDINLINE(0xA9A3);


/*
 *  DetachResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DetachResource(Handle theResource)                            ONEWORDINLINE(0xA992);


/*
 *  UniqueID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
UniqueID(ResType theType)                                     ONEWORDINLINE(0xA9C1);


/*
 *  Unique1ID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
Unique1ID(ResType theType)                                    ONEWORDINLINE(0xA810);


/*
 *  GetResAttrs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
GetResAttrs(Handle theResource)                               ONEWORDINLINE(0xA9A6);


/*
 *  GetResInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetResInfo(
  Handle     theResource,
  short *    theID,
  ResType *  theType,
  Str255     name)                                            ONEWORDINLINE(0xA9A8);


/*
 *  SetResInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetResInfo(
  Handle             theResource,
  short              theID,
  ConstStr255Param   name)                                    ONEWORDINLINE(0xA9A9);


/*
 *  AddResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
AddResource(
  Handle             theData,
  ResType            theType,
  short              theID,
  ConstStr255Param   name)                                    ONEWORDINLINE(0xA9AB);


/*
 *  GetResourceSizeOnDisk()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
GetResourceSizeOnDisk(Handle theResource)                     ONEWORDINLINE(0xA9A5);


/*
 *  GetMaxResourceSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
GetMaxResourceSize(Handle theResource)                        ONEWORDINLINE(0xA821);


#if CALL_NOT_IN_CARBON
/*
 *  RsrcMapEntry()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( long )
RsrcMapEntry(Handle theResource)                              ONEWORDINLINE(0xA9C5);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  SetResAttrs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetResAttrs(
  Handle   theResource,
  short    attrs)                                             ONEWORDINLINE(0xA9A7);


/*
 *  ChangedResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ChangedResource(Handle theResource)                           ONEWORDINLINE(0xA9AA);


/*
 *  RemoveResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
RemoveResource(Handle theResource)                            ONEWORDINLINE(0xA9AD);


/*
 *  UpdateResFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
UpdateResFile(short refNum)                                   ONEWORDINLINE(0xA999);


/*
 *  WriteResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
WriteResource(Handle theResource)                             ONEWORDINLINE(0xA9B0);


/*
 *  SetResPurge()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetResPurge(Boolean install)                                  ONEWORDINLINE(0xA993);


/*
 *  GetResFileAttrs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
GetResFileAttrs(short refNum)                                 ONEWORDINLINE(0xA9F6);


/*
 *  SetResFileAttrs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetResFileAttrs(
  short   refNum,
  short   attrs)                                              ONEWORDINLINE(0xA9F7);


/*
 *  OpenRFPerm()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
OpenRFPerm(
  ConstStr255Param   fileName,
  short              vRefNum,
  SInt8              permission)                              ONEWORDINLINE(0xA9C4);


#if CALL_NOT_IN_CARBON
/*
 *  RGetResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Handle )
RGetResource(
  ResType   theType,
  short     theID)                                            ONEWORDINLINE(0xA80C);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  HOpenResFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
HOpenResFile(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   fileName,
  SInt8              permission)                              ONEWORDINLINE(0xA81A);


/*
 *  HCreateResFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HCreateResFile(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   fileName)                                ONEWORDINLINE(0xA81B);


/*
 *  FSpOpenResFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
FSpOpenResFile(
  const FSSpec *  spec,
  SignedByte      permission)                                 TWOWORDINLINE(0x700D, 0xAA52);


/*
 *  FSpCreateResFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FSpCreateResFile(
  const FSSpec *  spec,
  OSType          creator,
  OSType          fileType,
  ScriptCode      scriptTag)                                  TWOWORDINLINE(0x700E, 0xAA52);


/*
 *  ReadPartialResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ReadPartialResource(
  Handle   theResource,
  long     offset,
  void *   buffer,
  long     count)                                             TWOWORDINLINE(0x7001, 0xA822);


/*
 *  WritePartialResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
WritePartialResource(
  Handle        theResource,
  long          offset,
  const void *  buffer,
  long          count)                                        TWOWORDINLINE(0x7002, 0xA822);


/*
 *  SetResourceSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetResourceSize(
  Handle   theResource,
  long     newSize)                                           TWOWORDINLINE(0x7003, 0xA822);


/*
 *  GetNextFOND()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
GetNextFOND(Handle fondHandle)                                TWOWORDINLINE(0x700A, 0xA822);


/* QuickTime 3.0*/
#if CALL_NOT_IN_CARBON
/*
 *  RegisterResourceEndianFilter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
RegisterResourceEndianFilter(
  ResType                   theType,
  ResourceEndianFilterPtr   theFilterProc);


/* Use TempInsertROMMap to force the ROM resource map to be
   inserted into the chain in front of the system. Note that
   this call is only temporary - the modified resource chain
   is only used for the next call to the resource manager.
   See IM IV 19 for more information. 
*/
/*
 *  TempInsertROMMap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
TempInsertROMMap(Boolean tempResLoad)                         FIVEWORDINLINE(0x70FF, 0x4A1F, 0x56C0, 0x31C0, 0x0B9E);


/*
  _________________________________________________________________________________________________________
      
   o RESOURCE CHAIN LOCATION - for use with the Resource Manager chain manipulation routines under Carbon.
  _________________________________________________________________________________________________________
*/

#endif  /* CALL_NOT_IN_CARBON */

typedef SInt16                          RsrcChainLocation;
enum {
  kRsrcChainBelowSystemMap      = 0,    /* Below the system's resource map*/
  kRsrcChainBelowApplicationMap = 1,    /* Below the application's resource map*/
  kRsrcChainAboveApplicationMap = 2,    /* Above the application's resource map*/
  kRsrcChainAboveAllMaps        = 4     /* Above all resource maps*/
};

/*
   If the file is already in the resource chain, it is removed and re-inserted at the specified location
   If the file has been detached, it is added to the resource chain at the specified location
   Returns resFNotFound if it's not currently open.
*/
/*
 *  InsertResourceFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
InsertResourceFile(
  SInt16              refNum,
  RsrcChainLocation   where);


/*
   If the file is not currently in the resource chain, this returns resNotFound
   Otherwise, the resource file is removed from the resource chain.
*/
/*
 *  DetachResourceFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DetachResourceFile(SInt16 refNum);


/*
   Returns true if the resource file is already open and known by the Resource Manager (i.e., it is
   either in the current resource chain or it's a detached resource file.)  If it's in the resource 
   chain, the inChain Boolean is set to true on exit and true is returned.  If it's an open file, but
   the file is currently detached, inChain is set to false and true is returned.  If the file is open,
   the refNum to the file is returned.
*/
/*
 *  FSpResourceFileAlreadyOpen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
FSpResourceFileAlreadyOpen(
  const FSSpec *  resourceFile,
  Boolean *       inChain,
  SInt16 *        refNum)                                     TWOWORDINLINE(0x7010, 0xA822);


/*
   FSpOpenOrphanResFile should be used to open a resource file that is persistent across all contexts,
   because using OpenResFile normally loads a map and all preloaded resources into the application
   context.  FSpOpenOrphanResFile loads everything into the system context and detaches the file 
   from the context in which it was opened.  If the file is already in the resource chain and a new
   instance is not opened, FSpOpenOrphanResFile will return a paramErr.
   Use with care, as can and will fail if the map is very large or a lot of preload
   resources exist.
*/
/*
 *  FSpOpenOrphanResFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FSpOpenOrphanResFile(
  const FSSpec *  spec,
  SignedByte      permission,
  SInt16 *        refNum);


/*
   GetTopResourceFile returns the refNum of the top most resource map in the current resource chain. If
   the resource chain is empty it returns resFNotFound.
*/
/*
 *  GetTopResourceFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetTopResourceFile(SInt16 * refNum);


/*
   GetNextResourceFile can be used to iterate over resource files in the resource chain. By passing a
   valid refNum in curRefNum it will return in nextRefNum the refNum of the next file in 
   the chain. If curRefNum is not found in the resource chain, GetNextResourceFile returns resFNotFound.
   When the end of the chain is reached GetNextResourceFile will return noErr and nextRefNum will be NIL.
*/
/*
 *  GetNextResourceFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetNextResourceFile(
  SInt16    curRefNum,
  SInt16 *  nextRefNum);



#if CALL_NOT_IN_CARBON
/*
 *  getnamedresource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Handle )
getnamedresource(
  ResType       theType,
  const char *  name);


/*
 *  get1namedresource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Handle )
get1namedresource(
  ResType       theType,
  const char *  name);


/*
 *  openrfperm()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( short )
openrfperm(
  const char *  fileName,
  short         vRefNum,
  char          permission);


/*
 *  openresfile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( short )
openresfile(const char * fileName);


/*
 *  createresfile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
createresfile(const char * fileName);


/*
 *  getresinfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
getresinfo(
  Handle     theResource,
  short *    theID,
  ResType *  theType,
  char *     name);


/*
 *  setresinfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
setresinfo(
  Handle        theResource,
  short         theID,
  const char *  name);


/*
 *  addresource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
addresource(
  Handle        theResource,
  ResType       theType,
  short         theID,
  const char *  name);


#endif  /* CALL_NOT_IN_CARBON */

#if OLDROUTINENAMES
#define SizeResource(theResource) GetResourceSizeOnDisk(theResource)
#define MaxSizeRsrc(theResource) GetMaxResourceSize(theResource)
#define RmveResource(theResource) RemoveResource(theResource)
#endif  /* OLDROUTINENAMES */

/*
 *  FSCreateResourceFile()
 *  
 *  Summary:
 *    Creates a new resource file.
 *  
 *  Discussion:
 *    This function creates a new file and initializes the specified
 *    named fork as an empty resource fork.  This function allows for
 *    the creation of data fork only files which can be used for
 *    storing resources.  Passing in a null name defaults to using the
 *    data fork.
 *  
 *  Parameters:
 *    
 *    parentRef:
 *      The directory where the file is to be created
 *    
 *    nameLength:
 *      Number of Unicode characters in the file's name
 *    
 *    name:
 *      A pointer to the Unicode name
 *    
 *    whichInfo:
 *      Which catalog info fields to set
 *    
 *    catalogInfo:
 *      The values for catalog info fields to set; may be NULL
 *    
 *    forkNameLength:
 *      The length of the fork name (in Unicode characters)
 *    
 *    forkName:
 *      The name of the fork to initialize (in Unicode); may be NULL
 *    
 *    newRef:
 *      A pointer to the FSRef for the new file; may be NULL
 *    
 *    newSpec:
 *      A pointer to the FSSpec for the new directory; may be NULL
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FSCreateResourceFile(
  const FSRef *          parentRef,
  UniCharCount           nameLength,
  const UniChar *        name,
  FSCatalogInfoBitmap    whichInfo,
  const FSCatalogInfo *  catalogInfo,          /* can be NULL */
  UniCharCount           forkNameLength,
  const UniChar *        forkName,             /* can be NULL */
  FSRef *                newRef,               /* can be NULL */
  FSSpec *               newSpec);             /* can be NULL */



/*
 *  FSCreateResourceFork()
 *  
 *  Summary:
 *    Creates the named forked and initializes it as an empty resource
 *    fork.
 *  
 *  Discussion:
 *    This function allows a resource fork to be added to an existing
 *    file.  Passing in a null forkname will result in the data fork
 *    being used.  If the named fork already exists this function does
 *    nothing and returns errFSForkExists.
 *  
 *  Parameters:
 *    
 *    ref:
 *      The file to add the fork to
 *    
 *    forkNameLength:
 *      The length of the fork name (in Unicode characters)
 *    
 *    forkName:
 *      The name of the fork to open (in Unicode); may be NULL
 *    
 *    flags:
 *      Pass in zero
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSErr )
FSCreateResourceFork(
  const FSRef *    ref,
  UniCharCount     forkNameLength,
  const UniChar *  forkName,             /* can be NULL */
  UInt32           flags);


/*
 *  FSOpenResourceFile()
 *  
 *  Summary:
 *    Opens the specified named fork as a resource fork.
 *  
 *  Discussion:
 *    This function allows any named fork of a file to be used for
 *    storing resources.  Passing in a null forkname will result in the
 *    data fork being used.
 *  
 *  Parameters:
 *    
 *    ref:
 *      The file containing the fork to open
 *    
 *    forkNameLength:
 *      The length of the fork name (in Unicode characters)
 *    
 *    forkName:
 *      The name of the fork to open (in Unicode); may be NULL
 *    
 *    permissions:
 *      The access (read and/or write) you want
 *    
 *    refNum:
 *      On exit the reference number for accessing the open fork
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FSOpenResourceFile(
  const FSRef *    ref,
  UniCharCount     forkNameLength,
  const UniChar *  forkName,             /* can be NULL */
  SInt8            permissions,
  SInt16 *         refNum);


/*
    These typedefs were originally created for the Copland Resource Mangager
*/
typedef short                           ResFileRefNum;
typedef short                           ResID;
typedef short                           ResAttributes;
typedef short                           ResFileAttributes;
#if CALL_NOT_IN_CARBON
/*
 *  SortResourceFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.2 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
SortResourceFile(short resFileRefNum)                         TWOWORDINLINE(0x7016, 0xA822);




#endif  /* CALL_NOT_IN_CARBON */


#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __RESOURCES__ */

