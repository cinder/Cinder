/*
     File:       Aliases.h
 
     Contains:   Alias Manager Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1989-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __ALIASES__
#define __ALIASES__

#ifndef __MACTYPES__
#include <MacTypes.h>
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

typedef UInt32                          FSAliasInfoBitmap;
enum {
  kFSAliasInfoNone              = 0x00000000, /* no valid info*/
  kFSAliasInfoVolumeCreateDate  = 0x00000001, /* volume creation date is valid*/
  kFSAliasInfoTargetCreateDate  = 0x00000002, /* target creation date is valid*/
  kFSAliasInfoFinderInfo        = 0x00000004, /* file type and creator are valid*/
  kFSAliasInfoIsDirectory       = 0x00000008, /* isDirectory boolean is valid*/
  kFSAliasInfoIDs               = 0x00000010, /* parentDirID and nodeID are valid*/
  kFSAliasInfoFSInfo            = 0x00000020, /* filesystemID and signature are valid*/
  kFSAliasInfoVolumeFlags       = 0x00000040 /* volumeIsBootVolume, volumeIsAutomounted, volumeIsEjectable and volumeHasPersistentFileIDs are valid*/
};

enum {
  rAliasType                    = FOUR_CHAR_CODE('alis') /* Aliases are stored as resources of this type */
};

enum {
                                        /* define alias resolution action rules mask */
  kARMMountVol                  = 0x00000001, /* mount the volume automatically */
  kARMNoUI                      = 0x00000002, /* no user interface allowed during resolution */
  kARMMultVols                  = 0x00000008, /* search on multiple volumes */
  kARMSearch                    = 0x00000100, /* search quickly */
  kARMSearchMore                = 0x00000200, /* search further */
  kARMSearchRelFirst            = 0x00000400, /* search target on a relative path first */
  kARMTryFileIDFirst            = 0x00000800 /* search by file id before path */
};

enum {
                                        /* define alias record information types */
  asiZoneName                   = -3,   /* get zone name */
  asiServerName                 = -2,   /* get server name */
  asiVolumeName                 = -1,   /* get volume name */
  asiAliasName                  = 0,    /* get aliased file/folder/volume name */
  asiParentName                 = 1     /* get parent folder name */
};

/* ResolveAliasFileWithMountFlags options */
enum {
  kResolveAliasFileNoUI         = 0x00000001, /* no user interaction during resolution */
  kResolveAliasTryFileIDFirst   = 0x00000002 /* search by file id before path */
};

/* define the alias record that will be the blackbox for the caller */
struct AliasRecord {
  OSType              userType;               /* appl stored type like creator type */
  unsigned short      aliasSize;              /* alias record size in bytes, for appl usage */
};
typedef struct AliasRecord              AliasRecord;
typedef AliasRecord *                   AliasPtr;
typedef AliasPtr *                      AliasHandle;
/* info block to pass to FSCopyAliasInfo */
struct FSAliasInfo {
  UTCDateTime         volumeCreateDate;
  UTCDateTime         targetCreateDate;
  OSType              fileType;
  OSType              fileCreator;
  UInt32              parentDirID;
  UInt32              nodeID;
  UInt16              filesystemID;
  UInt16              signature;
  Boolean             volumeIsBootVolume;
  Boolean             volumeIsAutomounted;
  Boolean             volumeIsEjectable;
  Boolean             volumeHasPersistentFileIDs;
  Boolean             isDirectory;
};
typedef struct FSAliasInfo              FSAliasInfo;
typedef FSAliasInfo *                   FSAliasInfoPtr;
/* alias record information type */
typedef short                           AliasInfoType;
/*
 *  NewAlias()
 *  
 *  Summary:
 *    create a new alias between fromFile and target, returns alias
 *    record handle
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NewAlias(
  const FSSpec *  fromFile,       /* can be NULL */
  const FSSpec *  target,
  AliasHandle *   alias)                                      TWOWORDINLINE(0x7002, 0xA823);


/*
 *  NewAliasMinimal()
 *  
 *  Summary:
 *    create a minimal new alias for a target and return alias record
 *    handle
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NewAliasMinimal(
  const FSSpec *  target,
  AliasHandle *   alias)                                      TWOWORDINLINE(0x7008, 0xA823);


/*
 *  NewAliasMinimalFromFullPath()
 *  
 *  Summary:
 *    create a minimal new alias from a target fullpath (optional zone
 *    and server name) and return alias record handle
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NewAliasMinimalFromFullPath(
  short             fullPathLength,
  const void *      fullPath,
  ConstStr32Param   zoneName,
  ConstStr31Param   serverName,
  AliasHandle *     alias)                                    TWOWORDINLINE(0x7009, 0xA823);


/*
 *  ResolveAlias()
 *  
 *  Summary:
 *    given an alias handle and fromFile, resolve the alias, update the
 *    alias record and return aliased filename and wasChanged flag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ResolveAlias(
  const FSSpec *  fromFile,         /* can be NULL */
  AliasHandle     alias,
  FSSpec *        target,
  Boolean *       wasChanged)                                 TWOWORDINLINE(0x7003, 0xA823);


/*
 *  GetAliasInfo()
 *  
 *  Summary:
 *    given an alias handle and an index specifying requested alias
 *    information type, return the information from alias record as a
 *    string.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetAliasInfo(
  AliasHandle     alias,
  AliasInfoType   index,
  Str63           theString)                                  TWOWORDINLINE(0x7007, 0xA823);



/*
 *  IsAliasFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
IsAliasFile(
  const FSSpec *  fileFSSpec,
  Boolean *       aliasFileFlag,
  Boolean *       folderFlag)                                 TWOWORDINLINE(0x702A, 0xA823);


/*
 *  ResolveAliasWithMountFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ResolveAliasWithMountFlags(
  const FSSpec *  fromFile,         /* can be NULL */
  AliasHandle     alias,
  FSSpec *        target,
  Boolean *       wasChanged,
  unsigned long   mountFlags)                                 TWOWORDINLINE(0x702B, 0xA823);


/*
 *  ResolveAliasFile()
 *  
 *  Summary:
 *    Given a file spec, return target file spec if input file spec is
 *    an alias. It resolves the entire alias chain or one step of the
 *    chain.  It returns info about whether the target is a folder or
 *    file; and whether the input file spec was an alias or not.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ResolveAliasFile(
  FSSpec *   theSpec,
  Boolean    resolveAliasChains,
  Boolean *  targetIsFolder,
  Boolean *  wasAliased)                                      TWOWORDINLINE(0x700C, 0xA823);



/*
 *  ResolveAliasFileWithMountFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ResolveAliasFileWithMountFlags(
  FSSpec *        theSpec,
  Boolean         resolveAliasChains,
  Boolean *       targetIsFolder,
  Boolean *       wasAliased,
  unsigned long   mountFlags)                                 TWOWORDINLINE(0x7029, 0xA823);


/*
 *  FollowFinderAlias()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FollowFinderAlias(
  const FSSpec *  fromFile,         /* can be NULL */
  AliasHandle     alias,
  Boolean         logon,
  FSSpec *        target,
  Boolean *       wasChanged)                                 TWOWORDINLINE(0x700F, 0xA823);


/* 
   Low Level Routines 
*/
/*
 *  UpdateAlias()
 *  
 *  Summary:
 *    given a fromFile-target pair and an alias handle, update the
 *    alias record pointed to by alias handle to represent target as
 *    the new alias.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
UpdateAlias(
  const FSSpec *  fromFile,         /* can be NULL */
  const FSSpec *  target,
  AliasHandle     alias,
  Boolean *       wasChanged)                                 TWOWORDINLINE(0x7006, 0xA823);



typedef CALLBACK_API( Boolean , AliasFilterProcPtr )(CInfoPBPtr cpbPtr, Boolean *quitFlag, Ptr myDataPtr);
typedef STACK_UPP_TYPE(AliasFilterProcPtr)                      AliasFilterUPP;
/*
 *  NewAliasFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AliasFilterUPP )
NewAliasFilterUPP(AliasFilterProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppAliasFilterProcInfo = 0x00000FD0 };  /* pascal 1_byte Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(AliasFilterUPP) NewAliasFilterUPP(AliasFilterProcPtr userRoutine) { return (AliasFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAliasFilterProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewAliasFilterUPP(userRoutine) (AliasFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAliasFilterProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeAliasFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeAliasFilterUPP(AliasFilterUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeAliasFilterUPP(AliasFilterUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeAliasFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeAliasFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeAliasFilterUPP(
  CInfoPBPtr      cpbPtr,
  Boolean *       quitFlag,
  Ptr             myDataPtr,
  AliasFilterUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeAliasFilterUPP(CInfoPBPtr cpbPtr, Boolean * quitFlag, Ptr myDataPtr, AliasFilterUPP userUPP) { return (Boolean)CALL_THREE_PARAMETER_UPP(userUPP, uppAliasFilterProcInfo, cpbPtr, quitFlag, myDataPtr); }
  #else
    #define InvokeAliasFilterUPP(cpbPtr, quitFlag, myDataPtr, userUPP) (Boolean)CALL_THREE_PARAMETER_UPP((userUPP), uppAliasFilterProcInfo, (cpbPtr), (quitFlag), (myDataPtr))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewAliasFilterProc(userRoutine)                     NewAliasFilterUPP(userRoutine)
    #define CallAliasFilterProc(userRoutine, cpbPtr, quitFlag, myDataPtr) InvokeAliasFilterUPP(cpbPtr, quitFlag, myDataPtr, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*
 *  MatchAlias()
 *  
 *  Summary:
 *    Given an alias handle and fromFile, match the alias and return
 *    FSSpecs to the aliased file(s) and needsUpdate flag
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
MatchAlias(
  const FSSpec *   fromFile,          /* can be NULL */
  unsigned long    rulesMask,
  AliasHandle      alias,
  short *          aliasCount,
  FSSpecArrayPtr   aliasList,
  Boolean *        needsUpdate,
  AliasFilterUPP   aliasFilter,
  void *           yourDataPtr)                               TWOWORDINLINE(0x7005, 0xA823);




/*
 *  ResolveAliasFileWithMountFlagsNoUI()
 *  
 *  Summary:
 *    variation on ResolveAliasFile that does not prompt user with a
 *    dialog
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ResolveAliasFileWithMountFlagsNoUI(
  FSSpec *        theSpec,
  Boolean         resolveAliasChains,
  Boolean *       targetIsFolder,
  Boolean *       wasAliased,
  unsigned long   mountFlags);


/*
 *  MatchAliasNoUI()
 *  
 *  Summary:
 *    variation on MatchAlias that does not prompt user with a dialog
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
MatchAliasNoUI(
  const FSSpec *   fromFile,          /* can be NULL */
  unsigned long    rulesMask,
  AliasHandle      alias,
  short *          aliasCount,
  FSSpecArrayPtr   aliasList,
  Boolean *        needsUpdate,
  AliasFilterUPP   aliasFilter,
  void *           yourDataPtr);


/*
 *  FSNewAliasUnicode()
 *  
 *  Summary:
 *    Creates an alias given a ref to the target's parent directory and
 *    the target's unicode name.  If the target does not exist fnfErr
 *    will be returned but the alias will still be created.  This
 *    allows the creation of aliases to targets that do not exist.
 *  
 *  Parameters:
 *    
 *    fromFile:
 *      The starting point for a relative search.
 *    
 *    targetParentRef:
 *      An FSRef to the parent directory of the target.
 *    
 *    targetNameLength:
 *      Number of Unicode characters in the target's name.
 *    
 *    targetName:
 *      A pointer to the Unicode name.
 *    
 *    inAlias:
 *      A Handle to the newly created alias record.
 *    
 *    isDirectory:
 *      On input, if target does not exist, a flag to indicate whether
 *      or not the target is a directory.  On output, if the target did
 *      exist, an flag indicating if the target is a directory.  Pass
 *      NULL in the non-existant case if unsure.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSErr )
FSNewAliasUnicode(
  const FSRef *    fromFile,               /* can be NULL */
  const FSRef *    targetParentRef,
  UniCharCount     targetNameLength,
  const UniChar *  targetName,
  AliasHandle *    inAlias,
  Boolean *        isDirectory);           /* can be NULL */


/*
 *  FSNewAliasMinimalUnicode()
 *  
 *  Summary:
 *    Creates a minimal alias given a ref to the target's parent
 *    directory and the target's unicode name.  If the target does not
 *    exist fnfErr will be returned but the alias will still be created.
 *  
 *  Parameters:
 *    
 *    targetParentRef:
 *      An FSRef to the parent directory of the target.
 *    
 *    targetNameLength:
 *      Number of Unicode characters in the target's name.
 *    
 *    targetName:
 *      A pointer to the Unicode name.
 *    
 *    inAlias:
 *      A Handle to the newly created alias record.
 *    
 *    isDirectory:
 *      On input, if target does not exist, a flag to indicate whether
 *      or not the
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSErr )
FSNewAliasMinimalUnicode(
  const FSRef *    targetParentRef,
  UniCharCount     targetNameLength,
  const UniChar *  targetName,
  AliasHandle *    inAlias,
  Boolean *        isDirectory);           /* can be NULL */


/*
 *  FSMatchAlias()
 *  
 *  Summary:
 *    Given an alias handle and fromFile, match the alias and return
 *    FSRefs to the aliased file(s) and needsUpdate flag
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSErr )
FSMatchAlias(
  const FSRef *    fromFile,          /* can be NULL */
  unsigned long    rulesMask,
  AliasHandle      inAlias,
  short *          aliasCount,
  FSRef *          aliasList,
  Boolean *        needsUpdate,
  AliasFilterUPP   aliasFilter,
  void *           yourDataPtr);


/*
 *  FSMatchAliasNoUI()
 *  
 *  Summary:
 *    variation on FSMatchAlias that does not prompt user with a dialog
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSErr )
FSMatchAliasNoUI(
  const FSRef *    fromFile,          /* can be NULL */
  unsigned long    rulesMask,
  AliasHandle      inAlias,
  short *          aliasCount,
  FSRef *          aliasList,
  Boolean *        needsUpdate,
  AliasFilterUPP   aliasFilter,
  void *           yourDataPtr);


/*
 *  FSCopyAliasInfo()
 *  
 *  Discussion:
 *    This routine will return the requested information from the
 *    passed in aliasHandle.  The information is gathered only from the
 *    alias record so it may not match what is on disk (no disk i/o is
 *    performed).  The whichInfo paramter is an output parameter that
 *    signifies which fields in the info record contain valid data.
 *  
 *  Parameters:
 *    
 *    inAlias:
 *      A handle to the alias record to get the information from.
 *    
 *    targetName:
 *      The name of the target item.
 *    
 *    volumeName:
 *      The name of the volume the target resides on.
 *    
 *    pathString:
 *      POSIX path to target.
 *    
 *    whichInfo:
 *      An indication of which fields in the info block contain valid
 *      data.
 *    
 *    info:
 *      Returned information about the alias.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
FSCopyAliasInfo(
  AliasHandle          inAlias,
  HFSUniStr255 *       targetName,       /* can be NULL */
  HFSUniStr255 *       volumeName,       /* can be NULL */
  CFStringRef *        pathString,       /* can be NULL */
  FSAliasInfoBitmap *  whichInfo,        /* can be NULL */
  FSAliasInfo *        info);            /* can be NULL */




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

#endif /* __ALIASES__ */

