/*
     File:       LaunchServices.h
 
     Contains:   Public interfaces for LaunchServices.framework
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __LAUNCHSERVICES__
#define __LAUNCHSERVICES__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __CFURL__
#include <CFURL.h>
#endif

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
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



/* ======================================================================================================== */
/* LaunchServices Structures and Enums                                                                      */
/* ======================================================================================================== */

enum {
  kLSUnknownErr                 = -10810,
  kLSNotAnApplicationErr        = -10811,
  kLSNotInitializedErr          = -10812,
  kLSDataUnavailableErr         = -10813, /* e.g. no kind string*/
  kLSApplicationNotFoundErr     = -10814, /* e.g. no application claims the file*/
  kLSUnknownTypeErr             = -10815,
  kLSDataTooOldErr              = -10816,
  kLSDataErr                    = -10817,
  kLSLaunchInProgressErr        = -10818, /* e.g. opening an alreay opening application*/
  kLSNotRegisteredErr           = -10819,
  kLSAppDoesNotClaimTypeErr     = -10820,
  kLSAppDoesNotSupportSchemeWarning = -10821, /* not an error, just a warning*/
  kLSServerCommunicationErr     = -10822, /* cannot set recent items*/
  kLSCannotSetInfoErr           = -10823 /* you may not set item info for this item*/
};

typedef OptionBits                      LSInitializeFlags;
enum {
  kLSInitializeDefaults         = 0x00000001
};

enum {
  kLSMinCatInfoBitmap           = (kFSCatInfoNodeFlags | kFSCatInfoParentDirID | kFSCatInfoFinderInfo | kFSCatInfoFinderXInfo) /* minimum info needed to avoid a FSGetCatalogInfo call when fetching item information */
};

enum {
  kLSInvalidExtensionIndex      = (unsigned long)0xFFFFFFFF /* Index returned from LSGetExtensionInfo when name has no extension*/
};

typedef OptionBits                      LSRequestedInfo;
enum {
  kLSRequestExtension           = 0x00000001, /* safe to use from threads*/
  kLSRequestTypeCreator         = 0x00000002, /* safe to use from threads*/
  kLSRequestBasicFlagsOnly      = 0x00000004, /* all but type of application and extension flags - safe to use from threads*/
  kLSRequestAppTypeFlags        = 0x00000008, /* NOT SAFE to use from threads*/
  kLSRequestAllFlags            = 0x00000010, /* NOT SAFE to use from threads*/
  kLSRequestIconAndKind         = 0x00000020, /* NOT SAFE to use from threads*/
  kLSRequestExtensionFlagsOnly  = 0x00000040, /* safe to use from threads*/
  kLSRequestAllInfo             = (unsigned long)0xFFFFFFFF /* NOT SAFE to use from threads*/
};

typedef OptionBits                      LSItemInfoFlags;
enum {
  kLSItemInfoIsPlainFile        = 0x00000001, /* none of the following applies*/
  kLSItemInfoIsPackage          = 0x00000002, /* app, doc, or bundle package*/
  kLSItemInfoIsApplication      = 0x00000004, /* single-file or packaged*/
  kLSItemInfoIsContainer        = 0x00000008, /* folder or volume*/
  kLSItemInfoIsAliasFile        = 0x00000010, /* Alias file (includes sym links)*/
  kLSItemInfoIsSymlink          = 0x00000020, /* UNIX sym link*/
  kLSItemInfoIsInvisible        = 0x00000040, /* does not include '.' files or '.hidden' entries*/
  kLSItemInfoIsNativeApp        = 0x00000080, /* Carbon or Cocoa native app*/
  kLSItemInfoIsClassicApp       = 0x00000100, /* CFM Classic app*/
  kLSItemInfoAppPrefersNative   = 0x00000200, /* Carbon app that prefers to be launched natively*/
  kLSItemInfoAppPrefersClassic  = 0x00000400, /* Carbon app that prefers to be launched in Classic*/
  kLSItemInfoAppIsScriptable    = 0x00000800, /* App can be scripted*/
  kLSItemInfoIsVolume           = 0x00001000, /* item is a volume*/
  kLSItemInfoExtensionIsHidden  = 0x00100000 /* item has a hidden extension*/
};

typedef OptionBits                      LSRolesMask;
enum {
  kLSRolesNone                  = 0x00000001, /* no claim is made about support for this type/scheme*/
  kLSRolesViewer                = 0x00000002, /* claim to be able to view this type/scheme*/
  kLSRolesEditor                = 0x00000004, /* claim to be able to edit this type/scheme*/
  kLSRolesAll                   = (unsigned long)0xFFFFFFFF /* claim to do it all*/
};

typedef UInt32                          LSKindID;
enum {
  kLSUnknownKindID              = 0
};

enum {
  kLSUnknownType                = 0,
  kLSUnknownCreator             = 0
};

struct LSItemInfoRecord {
  LSItemInfoFlags     flags;
  OSType              filetype;
  OSType              creator;                /* release when finished*/
  CFStringRef         extension;              /* release when finished*/
  CFStringRef         iconFileName;           /* not for general use*/
  LSKindID            kindID;                 /* not for general use*/
};
typedef struct LSItemInfoRecord         LSItemInfoRecord;

typedef OptionBits                      LSAcceptanceFlags;
enum {
  kLSAcceptDefault              = 0x00000001,
  kLSAcceptAllowLoginUI         = 0x00000002 /* show UI to log in if necessary*/
};

typedef OptionBits                      LSLaunchFlags;
enum {
  kLSLaunchDefaults             = 0x00000001, /* default = open, async, use Info.plist, start Classic*/
  kLSLaunchAndPrint             = 0x00000002, /* print items instead of open them*/
  kLSLaunchReserved2            = 0x00000004,
  kLSLaunchReserved3            = 0x00000008,
  kLSLaunchReserved4            = 0x00000010,
  kLSLaunchReserved5            = 0x00000020,
  kLSLaunchReserved6            = 0x00000040,
  kLSLaunchInhibitBGOnly        = 0x00000080, /* causes launch to fail if target is background-only.*/
  kLSLaunchDontAddToRecents     = 0x00000100, /* do not add app or documents to recents menus.*/
  kLSLaunchDontSwitch           = 0x00000200, /* don't bring new app to the foreground.*/
  kLSLaunchNoParams             = 0x00000800, /* Use Info.plist to determine launch parameters*/
  kLSLaunchAsync                = 0x00010000, /* launch async; obtain results from kCPSNotifyLaunch.*/
  kLSLaunchStartClassic         = 0x00020000, /* start up Classic environment if required for app.*/
  kLSLaunchInClassic            = 0x00040000, /* force app to launch in Classic environment.*/
  kLSLaunchNewInstance          = 0x00080000, /* Instantiate app even if it is already running.*/
  kLSLaunchAndHide              = 0x00100000, /* Send child a "hide" request as soon as it checks in.*/
  kLSLaunchAndHideOthers        = 0x00200000 /* Hide all other apps when child checks in.*/
};

struct LSLaunchFSRefSpec {
  const FSRef *       appRef;                 /* app to use, can be NULL*/
  UInt32              numDocs;                /* items to open/print, can be NULL*/
  const FSRef *       itemRefs;               /* array of FSRefs*/
  const AEDesc *      passThruParams;         /* passed untouched to application as optional parameter*/
  LSLaunchFlags       launchFlags;
  void *              asyncRefCon;            /* used if you register for app birth/death notification*/
};
typedef struct LSLaunchFSRefSpec        LSLaunchFSRefSpec;
struct LSLaunchURLSpec {
  CFURLRef            appURL;                 /* app to use, can be NULL*/
  CFArrayRef          itemURLs;               /* items to open/print, can be NULL*/
  const AEDesc *      passThruParams;         /* passed untouched to application as optional parameter*/
  LSLaunchFlags       launchFlags;
  void *              asyncRefCon;            /* used if you register for app birth/death notification*/
};
typedef struct LSLaunchURLSpec          LSLaunchURLSpec;


/* ======================================================================================================== */
/* LaunchServices Public Entry Points                                                                       */
/* ======================================================================================================== */

/*
 *  LSInit()
 *  
 *  Summary:
 *    Initialize LaunchServices for use.
 *  
 *  Discussion:
 *    LSInit is deprecated. Launch Services is initialized implicitly
 *    when first called.
 *  
 *  Parameters:
 *    
 *    inFlags:
 *      Use kLSInitializeDefaults.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSInit(LSInitializeFlags inFlags);



/*
 *  LSTerm()
 *  
 *  Summary:
 *    Terminate LaunchServices use.
 *  
 *  Discussion:
 *    LSTerm is deprecated. It does not need to be called.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSTerm(void);



/*
 *  LSCopyItemInfoForRef()
 *  
 *  Summary:
 *    Return information about an item.
 *  
 *  Discussion:
 *    Returns as much or as little information as requested about
 *    inItemRef. Some information is available in a thread-safe manner,
 *    some is not. All CFStrings must be released after use.
 *  
 *  Parameters:
 *    
 *    inItemRef:
 *      The FSRef of the item about which information is requested.
 *    
 *    inWhichInfo:
 *      Flags indicating which information to return
 *    
 *    outItemInfo:
 *      Information is returned in this structure. Must not be NULL
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSCopyItemInfoForRef(
  const FSRef *       inItemRef,
  LSRequestedInfo     inWhichInfo,
  LSItemInfoRecord *  outItemInfo);



/*
 *  LSCopyItemInfoForURL()
 *  
 *  Summary:
 *    Return information about an item.
 *  
 *  Discussion:
 *    Returns as much or as little information as requested about
 *    inURL. Some information is available in a thread-safe manner,
 *    some is not. All CFStrings must be released after use.
 *  
 *  Parameters:
 *    
 *    inURL:
 *      The CFURLRef of the item about which information is requested.
 *    
 *    inWhichInfo:
 *      Flags indicating which information to return
 *    
 *    outItemInfo:
 *      Information is returned in this structure. Must not be NULL
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSCopyItemInfoForURL(
  CFURLRef            inURL,
  LSRequestedInfo     inWhichInfo,
  LSItemInfoRecord *  outItemInfo);


/*
 *  LSGetExtensionInfo()
 *  
 *  Summary:
 *    Get information about the extension for a file system name.
 *  
 *  Discussion:
 *    Returns the starting index of the extension (not including the
 *    period) or kLSInvalidExtensionIndex if the input name has no
 *    extension.
 *  
 *  Parameters:
 *    
 *    inNameLen:
 *      The number of the UniChars in inNameBuffer.
 *    
 *    inNameBuffer:
 *      The buffer containing the name's Unicode characters.
 *    
 *    outExtStartIndex:
 *      On success, the starting index of the extension if there is one
 *      (not including the period). Set to kLSInvalidExtensionIndex if
 *      inNameBuffer does not contain a valid extension.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSStatus )
LSGetExtensionInfo(
  UniCharCount    inNameLen,
  const UniChar   inNameBuffer[],
  UniCharCount *  outExtStartIndex);



/*
 *  LSCopyDisplayNameForRef()
 *  
 *  Summary:
 *    Get the display name for an FSRef.
 *  
 *  Discussion:
 *    Return a copy of the display name for an FSRef. Takes into
 *    consideration whether this item has a hidden extension or not.
 *  
 *  Parameters:
 *    
 *    inRef:
 *      The FSRef for which the display name is desired.
 *    
 *    outDisplayName:
 *      Pointer to the CFString into which the display name should be
 *      copied. Callers must dispose of the resulting CFString.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSStatus )
LSCopyDisplayNameForRef(
  const FSRef *  inRef,
  CFStringRef *  outDisplayName);



/*
 *  LSCopyDisplayNameForURL()
 *  
 *  Summary:
 *    Get the display name for a CFURLRef.
 *  
 *  Discussion:
 *    Return a copy of the display name for a CFURLRef. Takes into
 *    consideration whether this item has a hidden extension or not.
 *  
 *  Parameters:
 *    
 *    inURL:
 *      The URL for which the display name is desired.
 *    
 *    outDisplayName:
 *      Pointer to the CFString into which the display name should be
 *      copied. Callers must dispose of the resulting CFString.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSStatus )
LSCopyDisplayNameForURL(
  CFURLRef       inURL,
  CFStringRef *  outDisplayName);



/*
 *  LSSetExtensionHiddenForRef()
 *  
 *  Summary:
 *    Sets whether the extension for an FSRef is hidden or not.
 *  
 *  Discussion:
 *    Sets the necessary file system state to indicate that the
 *    extension for inRef is hidden, as in the Finder. You can
 *    determine if an FSRef's extension is hidden using
 *    LSCopyItemInfoForRef.
 *  
 *  Parameters:
 *    
 *    inRef:
 *      The FSRef for which the extension is to be hidden or shown.
 *    
 *    inHide:
 *      True to hide inRef's extension, false to show it.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSStatus )
LSSetExtensionHiddenForRef(
  const FSRef *  inRef,
  Boolean        inHide);



/*
 *  LSSetExtensionHiddenForURL()
 *  
 *  Summary:
 *    Sets whether the extension for a CFURLRef is hidden or not.
 *  
 *  Discussion:
 *    Sets the necessary file system state to indicate that the
 *    extension for inURL is hidden, as in the Finder. You can
 *    determine if a CFURLRef's extension is hidden using
 *    LSCopyItemInfoForURL.
 *  
 *  Parameters:
 *    
 *    inURL:
 *      The CFURLRef for which the extension is to be hidden or shown.
 *    
 *    inHide:
 *      True to hide inURL's extension, false to show it.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSStatus )
LSSetExtensionHiddenForURL(
  CFURLRef   inURL,
  Boolean    inHide);



/*
 *  LSCopyKindStringForRef()
 *  
 *  Summary:
 *    Get the kind string for an item.
 *  
 *  Discussion:
 *    Returns the kind string as used in the Finder and elsewhere for
 *    inFSRef. The CFStringRef must be released after use.
 *  
 *  Parameters:
 *    
 *    inFSRef:
 *      The item for which the kind string is requested.
 *    
 *    outKindString:
 *      A CFStringRef* to receive the copied kind string object. This
 *      CFStringRef must be released eventually.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSCopyKindStringForRef(
  const FSRef *  inFSRef,
  CFStringRef *  outKindString);



/*
 *  LSCopyKindStringForURL()
 *  
 *  Summary:
 *    Get the kind string for an item.
 *  
 *  Discussion:
 *    Returns the kind string as used in the Finder and elsewhere for
 *    inURL. The CFStringRef must be released after use.
 *  
 *  Parameters:
 *    
 *    inURL:
 *      The item for which the kind string is requested.
 *    
 *    outKindString:
 *      A CFStringRef* to receive the copied kind string object. This
 *      CFStringRef must be released eventually.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSCopyKindStringForURL(
  CFURLRef       inURL,
  CFStringRef *  outKindString);



/*
 *  LSCopyKindStringForTypeInfo()
 *  
 *  Summary:
 *    Return the kind string for items like the provided info
 *  
 *  Discussion:
 *    Returns the kind string as shown in the Finder for the those
 *    items whose type, creator, and/or extension match the provided
 *    information. The kind string returned will be the one that most
 *    closely describes all the information provided. The kind string
 *    is subject to the document binding preferences that have been
 *    specified by the user. For example, if a creator is specified but
 *    the user has asked for files with the given
 *    creator/type/extension combination to open in an application with
 *    a different creator, the kind string will be loaded from the
 *    user's preferred application.
 *  
 *  Parameters:
 *    
 *    inType:
 *      The OSType file type for which you want a kind string. Specify
 *      kLSUnknownType if no file type information is available.
 *    
 *    inCreator:
 *      The OSType creator for which you want a kind string. Specify
 *      kLSUnknownCreator if no creator information is available.
 *    
 *    inExtension:
 *      The extension for which you want a kind string. Specify NULL if
 *      no extension information is available.
 *    
 *    outKindString:
 *      A CFStringRef* to receive the copied kind string object. This
 *      CFStringRef must be released eventually.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( OSStatus )
LSCopyKindStringForTypeInfo(
  OSType         inType,
  OSType         inCreator,
  CFStringRef    inExtension,         /* can be NULL */
  CFStringRef *  outKindString);



/*
 *  LSCopyKindStringForMIMEType()
 *  
 *  Summary:
 *    Get the kind string for the specified MIME type.
 *  
 *  Discussion:
 *    Returns the localized kind string describing the specified MIME
 *    type.
 *  
 *  Parameters:
 *    
 *    inMIMEType:
 *      The string specifying the MIME type.
 *    
 *    outKindString:
 *      A CFStringRef* to receive the copied kind string object. This
 *      CFStringRef must be released eventually.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( OSStatus )
LSCopyKindStringForMIMEType(
  CFStringRef    inMIMEType,
  CFStringRef *  outKindString);


/*
 *  LSGetApplicationForItem()
 *  
 *  Summary:
 *    Return the application used to open an item.
 *  
 *  Discussion:
 *    Consults the binding tables to return the application that would
 *    be used to open inItemRef if it were double-clicked in the
 *    Finder. This application will be the user-specified override if
 *    appropriate or the default otherwise. If no application is known
 *    to LaunchServices suitable for opening this item,
 *    kLSApplicationNotFoundErr will be returned.
 *  
 *  Parameters:
 *    
 *    inItemRef:
 *      The FSRef of the item for which the application is requested.
 *    
 *    inRoleMask:
 *      Whether to return the editor or viewer for inItemRef. If you
 *      don't care which, use kLSRolesAll.
 *    
 *    outAppRef:
 *      Filled in with the FSRef of the application if not NULL.
 *    
 *    outAppURL:
 *      Filled in with the CFURLRef of the application if not NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSGetApplicationForItem(
  const FSRef *  inItemRef,
  LSRolesMask    inRoleMask,
  FSRef *        outAppRef,        /* can be NULL */
  CFURLRef *     outAppURL);       /* can be NULL */



/*
 *  LSGetApplicationForInfo()
 *  
 *  Summary:
 *    Return the application used to open items with particular data.
 *  
 *  Discussion:
 *    Consults the binding tables to return the application that would
 *    be used to open items with type, creator, and/or extension as
 *    provided if they were double-clicked in the Finder. This
 *    application will be the default for items like this if one has
 *    been set. If no application is known to LaunchServices suitable
 *    for opening such items, kLSApplicationNotFoundErr will be
 *    returned. Not all three input parameters can be NULL at the same
 *    time nor can both output parameters be NULL at the same time.
 *  
 *  Parameters:
 *    
 *    inType:
 *      The file type to consider. Can be kLSUnknownType.
 *    
 *    inCreator:
 *      The file creator to consider. Can be kLSUnknownCreator.
 *    
 *    inExtension:
 *      The file name extension to consider. Can be NULL.
 *    
 *    inRoleMask:
 *      Whether to return the editor or viewer for inItemRef. If you
 *      don't care which, use kLSRolesAll.
 *    
 *    outAppRef:
 *      Filled in with the FSRef of the application if not NULL.
 *    
 *    outAppURL:
 *      Filled in with the CFURLRef of the application if not NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSGetApplicationForInfo(
  OSType        inType,
  OSType        inCreator,
  CFStringRef   inExtension,       /* can be NULL */
  LSRolesMask   inRoleMask,
  FSRef *       outAppRef,         /* can be NULL */
  CFURLRef *    outAppURL);        /* can be NULL */



/*
 *  LSCopyApplicationForMIMEType()
 *  
 *  Summary:
 *    Return the application used to handle data with the specified
 *    MIME type.
 *  
 *  Discussion:
 *    The returned application URL will be the user's preferred handler
 *    for the MIME type if one has been set. If no user preferred
 *    application has been set, Launch Services will select a default
 *    handler for the MIME type. If no application is known to handle
 *    the MIME type, kLSApplicationNotFoundErr will be returned.
 *  
 *  Parameters:
 *    
 *    inMIMEType:
 *      The string specifying the MIME type.
 *    
 *    inRoleMask:
 *      A role mask that the chosen application must satisfy. Use
 *      kLSRolesAll if the role is not important.
 *    
 *    outAppURL:
 *      Receives the copied CFURLRef, which must be released by the
 *      caller.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( OSStatus )
LSCopyApplicationForMIMEType(
  CFStringRef   inMIMEType,
  LSRolesMask   inRoleMask,
  CFURLRef *    outAppURL);



/*
 *  LSGetApplicationForURL()
 *  
 *  Summary:
 *    Return the application used to open an item.
 *  
 *  Discussion:
 *    Consults the binding tables to return the application that would
 *    be used to open inURL if it were double-clicked in the Finder.
 *    This application will be the user-specified override if
 *    appropriate or the default otherwise. If no application is known
 *    to LaunchServices suitable for opening this item,
 *    kLSApplicationNotFoundErr will be returned.
 *  
 *  Parameters:
 *    
 *    inURL:
 *      The CFURLRef of the item for which the application is requested.
 *    
 *    inRoleMask:
 *      Whether to return the editor or viewer for inItemRef. If you
 *      don't care which, use kLSRolesAll.
 *    
 *    outAppRef:
 *      Filled in with the FSRef of the application if not NULL.
 *    
 *    outAppURL:
 *      Filled in with the CFURLRef of the application if not NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSGetApplicationForURL(
  CFURLRef      inURL,
  LSRolesMask   inRoleMask,
  FSRef *       outAppRef,        /* can be NULL */
  CFURLRef *    outAppURL);       /* can be NULL */



/*
 *  LSFindApplicationForInfo()
 *  
 *  Summary:
 *    Locate a specific application.
 *  
 *  Discussion:
 *    Returns the application with the corresponding input information.
 *    The registry of applications is consulted first in order of
 *    bundleID, then creator, then name. All comparisons are case
 *    insensitive and 'ties' are decided first by version, then by
 *    native vs. Classic.
 *  
 *  Parameters:
 *    
 *    inCreator:
 *      The file creator to consider. Can be kLSUnknownCreator.
 *    
 *    inBundleID:
 *      The bundle ID to consider. Can be NULL.
 *    
 *    inName:
 *      The name to consider. Can be NULL. Must include any extensions
 *      that are part of the file system name, e.g. '.app'.
 *    
 *    outAppRef:
 *      Filled in with the FSRef of the application if not NULL.
 *    
 *    outAppURL:
 *      Filled in with the CFURLRef of the application if not NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSFindApplicationForInfo(
  OSType        inCreator,
  CFStringRef   inBundleID,       /* can be NULL */
  CFStringRef   inName,           /* can be NULL */
  FSRef *       outAppRef,        /* can be NULL */
  CFURLRef *    outAppURL);       /* can be NULL */



/*
 *  LSCanRefAcceptItem()
 *  
 *  Summary:
 *    Determine whether an item can accept another item.
 *  
 *  Discussion:
 *    Returns in outAcceptsItem whether inTargetRef can accept
 *    inItemFSRef as in a drag and drop operation. If inRoleMask is
 *    other than kLSRolesAll then make sure inTargetRef claims to
 *    fulfill the requested role.
 *  
 *  Parameters:
 *    
 *    inItemFSRef:
 *      FSRef of the item about which acceptance is requested.
 *    
 *    inTargetRef:
 *      FSRef of the potential target.
 *    
 *    inRoleMask:
 *      The role(s) the target must claim in order to consider
 *      acceptance.
 *    
 *    inFlags:
 *      Use kLSAcceptDefault.
 *    
 *    outAcceptsItem:
 *      Filled in with result. Must not be NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSCanRefAcceptItem(
  const FSRef *       inItemFSRef,
  const FSRef *       inTargetRef,
  LSRolesMask         inRoleMask,
  LSAcceptanceFlags   inFlags,
  Boolean *           outAcceptsItem);



/*
 *  LSCanURLAcceptURL()
 *  
 *  Summary:
 *    Determine whether an item can accept another item.
 *  
 *  Discussion:
 *    Returns in outAcceptsItem whether inTargetURL can accept
 *    inItemURL as in a drag and drop operation. If inRoleMask is other
 *    than kLSRolesAll then make sure inTargetRef claims to fulfill the
 *    requested role.
 *  
 *  Parameters:
 *    
 *    inItemURL:
 *      CFURLRef of the item about which acceptance is requested.
 *    
 *    inTargetURL:
 *      CFURLRef of the potential target.
 *    
 *    inRoleMask:
 *      The role(s) the target must claim in order to consider
 *      acceptance.
 *    
 *    inFlags:
 *      Use kLSAcceptDefault.
 *    
 *    outAcceptsItem:
 *      Filled in with result. Must not be NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSCanURLAcceptURL(
  CFURLRef            inItemURL,
  CFURLRef            inTargetURL,
  LSRolesMask         inRoleMask,
  LSAcceptanceFlags   inFlags,
  Boolean *           outAcceptsItem);



/*
 *  LSOpenFSRef()
 *  
 *  Summary:
 *    Open an application, document, or folder.
 *  
 *  Discussion:
 *    Opens applications, documents, and folders. Applications are
 *    opened via an 'oapp' or 'rapp' event. Documents are opened in
 *    their user-overridden or default applications as appropriate.
 *    Folders are opened in the Finder. Use the more specific
 *    LSOpenFromRefSpec for more control over launching.
 *  
 *  Parameters:
 *    
 *    inRef:
 *      The FSRef of the item to launch.
 *    
 *    outLaunchedRef:
 *      The FSRef of the item actually launched. For inRefs that are
 *      documents, outLaunchedRef will be the application used to
 *      launch the document. Can be NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSOpenFSRef(
  const FSRef *  inRef,
  FSRef *        outLaunchedRef);      /* can be NULL */



/*
 *  LSOpenCFURLRef()
 *  
 *  Summary:
 *    Open an application, document, or folder.
 *  
 *  Discussion:
 *    Opens applications, documents, and folders. Applications are
 *    opened via an 'oapp' or 'rapp' event. Documents are opened in
 *    their user-overridden or default applications as appropriate.
 *    Folders are opened in the Finder. Use the more specific
 *    LSOpenFromURLSpec for more control over launching.
 *  
 *  Parameters:
 *    
 *    inURL:
 *      The CFURLRef of the item to launch.
 *    
 *    outLaunchedURL:
 *      The CFURLRef of the item actually launched. For inURLs that are
 *      documents, outLaunchedURL will be the application used to
 *      launch the document. Can be NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSOpenCFURLRef(
  CFURLRef    inURL,
  CFURLRef *  outLaunchedURL);      /* can be NULL */



/*
 *  LSOpenFromRefSpec()
 *  
 *  Summary:
 *    Opens an application or one or more documents or folders.
 *  
 *  Discussion:
 *    Opens applications, documents, and folders.
 *  
 *  Parameters:
 *    
 *    inLaunchSpec:
 *      The specification of what to launch and how to launch it.
 *    
 *    outLaunchedRef:
 *      The FSRef of the item actually launched. For inRefs that are
 *      documents, outLaunchedRef will be the application used to
 *      launch the document. Can be NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSOpenFromRefSpec(
  const LSLaunchFSRefSpec *  inLaunchSpec,
  FSRef *                    outLaunchedRef);      /* can be NULL */



/*
 *  LSOpenFromURLSpec()
 *  
 *  Summary:
 *    Opens an application or one or more documents or folders.
 *  
 *  Discussion:
 *    Opens applications, documents, and folders.
 *  
 *  Parameters:
 *    
 *    inLaunchSpec:
 *      The specification of what to launch and how to launch it.
 *    
 *    outLaunchedURL:
 *      The CFURLRef of the item actually launched. For inURLs that are
 *      documents, outLaunchedURL will be the application used to
 *      launch the document. Can be NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
LSOpenFromURLSpec(
  const LSLaunchURLSpec *  inLaunchSpec,
  CFURLRef *               outLaunchedURL);      /* can be NULL */




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

#endif /* __LAUNCHSERVICES__ */

