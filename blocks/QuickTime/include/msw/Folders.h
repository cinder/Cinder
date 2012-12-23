/*
     File:       Folders.h
 
     Contains:   Folder Manager Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1995-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __FOLDERS__
#define __FOLDERS__

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

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif

enum {
  kOnSystemDisk                 = -32768L, /* previously was 0x8000 but that is an unsigned value whereas vRefNum is signed*/
  kOnAppropriateDisk            = -32767, /* Generally, the same as kOnSystemDisk, but it's clearer that this isn't always the 'boot' disk.*/
                                        /* Folder Domains - Carbon only.  The constants above can continue to be used, but the folder/volume returned will*/
                                        /* be from one of the domains below.*/
  kSystemDomain                 = -32766, /* Read-only system hierarchy.*/
  kLocalDomain                  = -32765, /* All users of a single machine have access to these resources.*/
  kNetworkDomain                = -32764, /* All users configured to use a common network server has access to these resources.*/
  kUserDomain                   = -32763, /* Read/write. Resources that are private to the user.*/
  kClassicDomain                = -32762 /* Domain referring to the currently configured Classic System Folder*/
};

enum {
  kCreateFolder                 = true,
  kDontCreateFolder             = false
};

enum {
  kSystemFolderType             = FOUR_CHAR_CODE('macs'), /* the system folder */
  kDesktopFolderType            = FOUR_CHAR_CODE('desk'), /* the desktop folder; objects in this folder show on the desk top. */
  kSystemDesktopFolderType      = FOUR_CHAR_CODE('sdsk'), /* the desktop folder at the root of the hard drive, never the redirected user desktop folder */
  kTrashFolderType              = FOUR_CHAR_CODE('trsh'), /* the trash folder; objects in this folder show up in the trash */
  kSystemTrashFolderType        = FOUR_CHAR_CODE('strs'), /* the trash folder at the root of the drive, never the redirected user trash folder */
  kWhereToEmptyTrashFolderType  = FOUR_CHAR_CODE('empt'), /* the "empty trash" folder; Finder starts empty from here down */
  kPrintMonitorDocsFolderType   = FOUR_CHAR_CODE('prnt'), /* Print Monitor documents */
  kStartupFolderType            = FOUR_CHAR_CODE('strt'), /* Finder objects (applications, documents, DAs, aliases, to...) to open at startup go here */
  kShutdownFolderType           = FOUR_CHAR_CODE('shdf'), /* Finder objects (applications, documents, DAs, aliases, to...) to open at shutdown go here */
  kAppleMenuFolderType          = FOUR_CHAR_CODE('amnu'), /* Finder objects to put into the Apple menu go here */
  kControlPanelFolderType       = FOUR_CHAR_CODE('ctrl'), /* Control Panels go here (may contain INITs) */
  kSystemControlPanelFolderType = FOUR_CHAR_CODE('sctl'), /* System control panels folder - never the redirected one, always "Control Panels" inside the System Folder */
  kExtensionFolderType          = FOUR_CHAR_CODE('extn'), /* System extensions go here */
  kFontsFolderType              = FOUR_CHAR_CODE('font'), /* Fonts go here */
  kPreferencesFolderType        = FOUR_CHAR_CODE('pref'), /* preferences for applications go here */
  kSystemPreferencesFolderType  = FOUR_CHAR_CODE('sprf'), /* System-type Preferences go here - this is always the system's preferences folder, never a logged in user's */
  kTemporaryFolderType          = FOUR_CHAR_CODE('temp') /* temporary files go here (deleted periodically, but don't rely on it.) */
};

/*
 *  FindFolder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FindFolder(
  short     vRefNum,
  OSType    folderType,
  Boolean   createFolder,
  short *   foundVRefNum,
  long *    foundDirID)                                       TWOWORDINLINE(0x7000, 0xA823);


/*
 *  FindFolderExtended()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FindFolderExtended(
  short     vol,
  OSType    foldType,
  Boolean   createFolder,
  UInt32    flags,
  void *    data,
  short *   vRefNum,
  long *    dirID)                                            THREEWORDINLINE(0x303C, 0x0B2C, 0xA823);


/*
 *  ReleaseFolder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FoldersLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ReleaseFolder(
  short    vRefNum,
  OSType   folderType)                                        TWOWORDINLINE(0x700B, 0xA823);



#if !TARGET_OS_MAC
/* Since non-mac targets don't know about VRef's or DirID's, the Ex version returns
   the found folder path.
 */
#if CALL_NOT_IN_CARBON
/*
 *  FindFolderEx()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
FindFolderEx(
  short     vRefNum,
  OSType    folderType,
  Boolean   createFolder,
  short *   foundVRefNum,
  long *    foundDirID,
  char *    foundFolder);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* !TARGET_OS_MAC */

/******************************************/
/* Extensible Folder Manager declarations */
/******************************************/

/****************************/
/* Folder Manager constants */
/****************************/

enum {
  kExtensionDisabledFolderType  = FOUR_CHAR_CODE('extD'),
  kControlPanelDisabledFolderType = FOUR_CHAR_CODE('ctrD'),
  kSystemExtensionDisabledFolderType = FOUR_CHAR_CODE('macD'),
  kStartupItemsDisabledFolderType = FOUR_CHAR_CODE('strD'),
  kShutdownItemsDisabledFolderType = FOUR_CHAR_CODE('shdD'),
  kApplicationsFolderType       = FOUR_CHAR_CODE('apps'),
  kDocumentsFolderType          = FOUR_CHAR_CODE('docs')
};

enum {
                                        /* new constants */
  kVolumeRootFolderType         = FOUR_CHAR_CODE('root'), /* root folder of a volume */
  kChewableItemsFolderType      = FOUR_CHAR_CODE('flnt'), /* items deleted at boot */
  kApplicationSupportFolderType = FOUR_CHAR_CODE('asup'), /* third-party items and folders */
  kTextEncodingsFolderType      = 0xC4746578, /* encoding tables */
  kStationeryFolderType         = FOUR_CHAR_CODE('odst'), /* stationery */
  kOpenDocFolderType            = FOUR_CHAR_CODE('odod'), /* OpenDoc root */
  kOpenDocShellPlugInsFolderType = FOUR_CHAR_CODE('odsp'), /* OpenDoc Shell Plug-Ins in OpenDoc folder */
  kEditorsFolderType            = FOUR_CHAR_CODE('oded'), /* OpenDoc editors in MacOS Folder */
  kOpenDocEditorsFolderType     = 0xC46F6466, /* OpenDoc subfolder of Editors folder */
  kOpenDocLibrariesFolderType   = FOUR_CHAR_CODE('odlb'), /* OpenDoc libraries folder */
  kGenEditorsFolderType         = 0xC4656469, /* CKH general editors folder at root level of Sys folder */
  kHelpFolderType               = 0xC4686C70, /* CKH help folder currently at root of system folder */
  kInternetPlugInFolderType     = 0xC46E6574, /* CKH internet plug ins for browsers and stuff */
  kModemScriptsFolderType       = 0xC46D6F64, /* CKH modem scripts, get 'em OUT of the Extensions folder */
  kPrinterDescriptionFolderType = FOUR_CHAR_CODE('ppdf'), /* CKH new folder at root of System folder for printer descs. */
  kPrinterDriverFolderType      = 0xC4707264, /* CKH new folder at root of System folder for printer drivers */
  kScriptingAdditionsFolderType = 0xC4736372, /* CKH at root of system folder */
  kSharedLibrariesFolderType    = 0xC46C6962, /* CKH for general shared libs. */
  kVoicesFolderType             = FOUR_CHAR_CODE('fvoc'), /* CKH macintalk can live here */
  kControlStripModulesFolderType = FOUR_CHAR_CODE('sdev'), /* CKH for control strip modules */
  kAssistantsFolderType         = 0x617374C4, /* SJF for Assistants (MacOS Setup Assistant, etc) */
  kUtilitiesFolderType          = 0x757469C4, /* SJF for Utilities folder */
  kAppleExtrasFolderType        = 0x616578C4, /* SJF for Apple Extras folder */
  kContextualMenuItemsFolderType = FOUR_CHAR_CODE('cmnu'), /* SJF for Contextual Menu items */
  kMacOSReadMesFolderType       = 0x6D6F72C4, /* SJF for MacOS ReadMes folder */
  kALMModulesFolderType         = FOUR_CHAR_CODE('walk'), /* EAS for Location Manager Module files except type 'thng' (within kExtensionFolderType) */
  kALMPreferencesFolderType     = FOUR_CHAR_CODE('trip'), /* EAS for Location Manager Preferences (within kPreferencesFolderType; contains kALMLocationsFolderType) */
  kALMLocationsFolderType       = FOUR_CHAR_CODE('fall'), /* EAS for Location Manager Locations (within kALMPreferencesFolderType) */
  kColorSyncProfilesFolderType  = FOUR_CHAR_CODE('prof'), /* for ColorSync(tm) Profiles */
  kThemesFolderType             = FOUR_CHAR_CODE('thme'), /* for Theme data files */
  kFavoritesFolderType          = FOUR_CHAR_CODE('favs'), /* Favorties folder for Navigation Services */
  kInternetFolderType           = 0x696E74C4, /* Internet folder (root level of startup volume) */
  kAppearanceFolderType         = FOUR_CHAR_CODE('appr'), /* Appearance folder (root of system folder) */
  kSoundSetsFolderType          = FOUR_CHAR_CODE('snds'), /* Sound Sets folder (in Appearance folder) */
  kDesktopPicturesFolderType    = 0x647470C4, /* Desktop Pictures folder (in Appearance folder) */
  kInternetSearchSitesFolderType = FOUR_CHAR_CODE('issf'), /* Internet Search Sites folder */
  kFindSupportFolderType        = FOUR_CHAR_CODE('fnds'), /* Find support folder */
  kFindByContentFolderType      = FOUR_CHAR_CODE('fbcf'), /* Find by content folder */
  kInstallerLogsFolderType      = FOUR_CHAR_CODE('ilgf'), /* Installer Logs folder */
  kScriptsFolderType            = 0x736372C4, /* Scripts folder */
  kFolderActionsFolderType      = FOUR_CHAR_CODE('fasf'), /* Folder Actions Scripts folder */
  kLauncherItemsFolderType      = FOUR_CHAR_CODE('laun'), /* Launcher Items folder */
  kRecentApplicationsFolderType = FOUR_CHAR_CODE('rapp'), /* Recent Applications folder */
  kRecentDocumentsFolderType    = FOUR_CHAR_CODE('rdoc'), /* Recent Documents folder */
  kRecentServersFolderType      = FOUR_CHAR_CODE('rsvr'), /* Recent Servers folder */
  kSpeakableItemsFolderType     = FOUR_CHAR_CODE('spki'), /* Speakable Items folder */
  kKeychainFolderType           = FOUR_CHAR_CODE('kchn'), /* Keychain folder */
  kQuickTimeExtensionsFolderType = FOUR_CHAR_CODE('qtex'), /* QuickTime Extensions Folder (in Extensions folder) */
  kDisplayExtensionsFolderType  = FOUR_CHAR_CODE('dspl'), /* Display Extensions Folder (in Extensions folder) */
  kMultiprocessingFolderType    = FOUR_CHAR_CODE('mpxf'), /* Multiprocessing Folder (in Extensions folder) */
  kPrintingPlugInsFolderType    = FOUR_CHAR_CODE('pplg') /* Printing Plug-Ins Folder (in Extensions folder) */
};


/* New Folder Types to accommodate the Mac OS X Folder Manager */
/* These folder types are not applicable on Mac OS 9.          */
enum {
  kDomainTopLevelFolderType     = FOUR_CHAR_CODE('dtop'), /* The top-level of a Folder domain, e.g. "/System"*/
  kDomainLibraryFolderType      = FOUR_CHAR_CODE('dlib'), /* the Library subfolder of a particular domain*/
  kColorSyncFolderType          = FOUR_CHAR_CODE('sync'), /* Contains ColorSync-related folders*/
  kColorSyncCMMFolderType       = FOUR_CHAR_CODE('ccmm'), /* ColorSync CMMs*/
  kColorSyncScriptingFolderType = FOUR_CHAR_CODE('cscr'), /* ColorSync Scripting support*/
  kPrintersFolderType           = FOUR_CHAR_CODE('impr'), /* Contains Printing-related folders*/
  kSpeechFolderType             = FOUR_CHAR_CODE('spch'), /* Contains Speech-related folders*/
  kCarbonLibraryFolderType      = FOUR_CHAR_CODE('carb'), /* Contains Carbon-specific file*/
  kDocumentationFolderType      = FOUR_CHAR_CODE('info'), /* Contains Documentation files (not user documents)*/
  kDeveloperDocsFolderType      = FOUR_CHAR_CODE('ddoc'), /* Contains Developer Documentation files and folders*/
  kDeveloperHelpFolderType      = FOUR_CHAR_CODE('devh'), /* Contains Developer Help related files*/
  kISSDownloadsFolderType       = FOUR_CHAR_CODE('issd'), /* Contains Internet Search Sites downloaded from the Internet*/
  kUserSpecificTmpFolderType    = FOUR_CHAR_CODE('utmp'), /* Contains temporary items created on behalf of the current user*/
  kCachedDataFolderType         = FOUR_CHAR_CODE('cach'), /* Contains various cache files for different clients*/
  kFrameworksFolderType         = FOUR_CHAR_CODE('fram'), /* Contains MacOS X Framework folders     */
  kPrivateFrameworksFolderType  = FOUR_CHAR_CODE('pfrm'), /* Contains MacOS X Private Framework folders     */
  kClassicDesktopFolderType     = FOUR_CHAR_CODE('sdsk'), /* MacOS 9 compatible desktop folder - same as */
                                        /* kSystemDesktopFolderType but with a more appropriate*/
                                        /* name for Mac OS X code.*/
  kDeveloperFolderType          = FOUR_CHAR_CODE('devf'), /* Contains MacOS X Developer Resources*/
  kSystemSoundsFolderType       = FOUR_CHAR_CODE('ssnd'), /* Contains Mac OS X System Sound Files*/
  kComponentsFolderType         = FOUR_CHAR_CODE('cmpd'), /* Contains Mac OS X components*/
  kQuickTimeComponentsFolderType = FOUR_CHAR_CODE('wcmp'), /* Contains QuickTime components for Mac OS X*/
  kCoreServicesFolderType       = FOUR_CHAR_CODE('csrv'), /* Refers to the "CoreServices" folder on Mac OS X*/
  kPictureDocumentsFolderType   = FOUR_CHAR_CODE('pdoc'), /* Refers to the "Pictures" folder in a users home directory*/
  kMovieDocumentsFolderType     = FOUR_CHAR_CODE('mdoc'), /* Refers to the "Movies" folder in a users home directory*/
  kMusicDocumentsFolderType     = 0xB5646F63, /* Refers to the "Music" folder in a users home directory*/
  kInternetSitesFolderType      = FOUR_CHAR_CODE('site'), /* Refers to the "Sites" folder in a users home directory*/
  kPublicFolderType             = FOUR_CHAR_CODE('pubb'), /* Refers to the "Public" folder in a users home directory*/
  kAudioSupportFolderType       = FOUR_CHAR_CODE('adio'), /* Refers to the Audio support folder for Mac OS X*/
  kAudioSoundsFolderType        = FOUR_CHAR_CODE('asnd'), /* Refers to the Sounds subfolder of Audio Support*/
  kAudioSoundBanksFolderType    = FOUR_CHAR_CODE('bank'), /* Refers to the Banks subfolder of the Sounds Folder*/
  kAudioAlertSoundsFolderType   = FOUR_CHAR_CODE('alrt'), /* Refers to the Alert Sounds subfolder of the Sound Folder*/
  kAudioPlugInsFolderType       = FOUR_CHAR_CODE('aplg'), /* Refers to the Plug-ins subfolder of the Audio Folder   */
  kAudioComponentsFolderType    = FOUR_CHAR_CODE('acmp'), /* Refers to the Components subfolder of the Audio Plug-ins Folder    */
  kKernelExtensionsFolderType   = FOUR_CHAR_CODE('kext'), /* Refers to the Kernel Extensions Folder on Mac OS X*/
  kDirectoryServicesFolderType  = FOUR_CHAR_CODE('dsrv'), /* Refers to the Directory Services folder on Mac OS X*/
  kDirectoryServicesPlugInsFolderType = FOUR_CHAR_CODE('dplg'), /* Refers to the Directory Services Plug-Ins folder on Mac OS X */
  kInstallerReceiptsFolderType  = FOUR_CHAR_CODE('rcpt'), /* Refers to the "Receipts" folder in Mac OS X*/
  kFileSystemSupportFolderType  = FOUR_CHAR_CODE('fsys'), /* Refers to the [domain]/Library/Filesystems folder in Mac OS X*/
  kAppleShareSupportFolderType  = FOUR_CHAR_CODE('shar'), /* Refers to the [domain]/Library/Filesystems/AppleShare folder in Mac OS X*/
  kAppleShareAuthenticationFolderType = FOUR_CHAR_CODE('auth'), /* Refers to the [domain]/Library/Filesystems/AppleShare/Authentication folder in Mac OS X*/
  kMIDIDriversFolderType        = FOUR_CHAR_CODE('midi'), /* Refers to the MIDI Drivers folder on Mac OS X*/
  kKeyboardLayoutsFolderType    = FOUR_CHAR_CODE('klay'), /* Refers to the [domain]/Library/KeyboardLayouts folder in Mac OS X*/
  kIndexFilesFolderType         = FOUR_CHAR_CODE('indx'), /* Refers to the [domain]/Library/Indexes folder in Mac OS X*/
  kFindByContentIndexesFolderType = FOUR_CHAR_CODE('fbcx'), /* Refers to the [domain]/Library/Indexes/FindByContent folder in Mac OS X*/
  kManagedItemsFolderType       = FOUR_CHAR_CODE('mang'), /* Refers to the Managed Items folder for Mac OS X */
  kBootTimeStartupItemsFolderType = FOUR_CHAR_CODE('empz') /* Refers to the "StartupItems" folder of Mac OS X */
};

enum {
  kLocalesFolderType            = 0xC46C6F63, /* PKE for Locales folder */
  kFindByContentPluginsFolderType = FOUR_CHAR_CODE('fbcp') /* Find By Content Plug-ins */
};

enum {
  kUsersFolderType              = FOUR_CHAR_CODE('usrs'), /* "Users" folder, contains one folder for each user. */
  kCurrentUserFolderType        = FOUR_CHAR_CODE('cusr'), /* The folder for the currently logged on user. */
  kCurrentUserRemoteFolderLocation = FOUR_CHAR_CODE('rusf'), /* The remote folder for the currently logged on user */
  kCurrentUserRemoteFolderType  = FOUR_CHAR_CODE('rusr'), /* The remote folder location for the currently logged on user */
  kSharedUserDataFolderType     = FOUR_CHAR_CODE('sdat'), /* A Shared "Documents" folder, readable & writeable by all users */
  kVolumeSettingsFolderType     = FOUR_CHAR_CODE('vsfd') /* Volume specific user information goes here */
};

/* FolderDescFlags values */
enum {
  kCreateFolderAtBoot           = 0x00000002,
  kCreateFolderAtBootBit        = 1,
  kFolderCreatedInvisible       = 0x00000004,
  kFolderCreatedInvisibleBit    = 2,
  kFolderCreatedNameLocked      = 0x00000008,
  kFolderCreatedNameLockedBit   = 3,
  kFolderCreatedAdminPrivs      = 0x00000010,
  kFolderCreatedAdminPrivsBit   = 4
};

enum {
  kFolderInUserFolder           = 0x00000020,
  kFolderInUserFolderBit        = 5,
  kFolderTrackedByAlias         = 0x00000040,
  kFolderTrackedByAliasBit      = 6,
  kFolderInRemoteUserFolderIfAvailable = 0x00000080,
  kFolderInRemoteUserFolderIfAvailableBit = 7,
  kFolderNeverMatchedInIdentifyFolder = 0x00000100,
  kFolderNeverMatchedInIdentifyFolderBit = 8,
  kFolderMustStayOnSameVolume   = 0x00000200,
  kFolderMustStayOnSameVolumeBit = 9,
  kFolderManagerFolderInMacOS9FolderIfMacOSXIsInstalledMask = 0x00000400,
  kFolderManagerFolderInMacOS9FolderIfMacOSXIsInstalledBit = 10,
  kFolderInLocalOrRemoteUserFolder = kFolderInUserFolder | kFolderInRemoteUserFolderIfAvailable
};

typedef UInt32                          FolderDescFlags;
/* FolderClass values */
enum {
  kRelativeFolder               = FOUR_CHAR_CODE('relf'),
  kSpecialFolder                = FOUR_CHAR_CODE('spcf')
};

typedef OSType                          FolderClass;
/* special folder locations */
enum {
  kBlessedFolder                = FOUR_CHAR_CODE('blsf'),
  kRootFolder                   = FOUR_CHAR_CODE('rotf')
};

enum {
  kCurrentUserFolderLocation    = FOUR_CHAR_CODE('cusf') /*    the magic 'Current User' folder location*/
};

typedef OSType                          FolderType;
typedef OSType                          FolderLocation;

struct FolderDesc {
  Size                descSize;
  FolderType          foldType;
  FolderDescFlags     flags;
  FolderClass         foldClass;
  FolderType          foldLocation;
  OSType              badgeSignature;
  OSType              badgeType;
  UInt32              reserved;
  StrFileName         name;                   /* Str63 on MacOS*/
};
typedef struct FolderDesc               FolderDesc;
typedef FolderDesc *                    FolderDescPtr;

typedef UInt32                          RoutingFlags;
struct FolderRouting {
  Size                descSize;
  OSType              fileType;
  FolderType          routeFromFolder;
  FolderType          routeToFolder;
  RoutingFlags        flags;
};
typedef struct FolderRouting            FolderRouting;
typedef FolderRouting *                 FolderRoutingPtr;
/* routing constants */
/*  These are bits in the .flags field of the FindFolderUserRedirectionGlobals struct*/
enum {
                                        /*    Set this bit to 1 in the .flags field of a FindFolderUserRedirectionGlobals*/
                                        /*    structure if the userName in the struct should be used as the current*/
                                        /*    "User" name*/
  kFindFolderRedirectionFlagUseDistinctUserFoldersBit = 0, /*    Set this bit to 1 and the currentUserFolderVRefNum and currentUserFolderDirID*/
                                        /*    fields of the user record will get used instead of finding the user folder*/
                                        /*    with the userName field.*/
  kFindFolderRedirectionFlagUseGivenVRefAndDirIDAsUserFolderBit = 1, /*    Set this bit to 1 and the remoteUserFolderVRefNum and remoteUserFolderDirID*/
                                        /*    fields of the user record will get used instead of finding the user folder*/
                                        /*    with the userName field.*/
  kFindFolderRedirectionFlagsUseGivenVRefNumAndDirIDAsRemoteUserFolderBit = 2
};

struct FindFolderUserRedirectionGlobals {
  UInt32              version;
  UInt32              flags;

  Str31               userName;
  short               userNameScript;

  short               currentUserFolderVRefNum;
  long                currentUserFolderDirID;

  short               remoteUserFolderVRefNum;
  long                remoteUserFolderDirID;
};
typedef struct FindFolderUserRedirectionGlobals FindFolderUserRedirectionGlobals;
typedef FindFolderUserRedirectionGlobals * FindFolderUserRedirectionGlobalsPtr;
enum {
  kFolderManagerUserRedirectionGlobalsCurrentVersion = 1
};

/*
    These are passed into FindFolderExtended(), FindFolderInternalExtended(), and
    FindFolderNewInstallerEntryExtended() in the flags field. 
*/
enum {
  kFindFolderExtendedFlagsDoNotFollowAliasesBit = 0,
  kFindFolderExtendedFlagsDoNotUseUserFolderBit = 1,
  kFindFolderExtendedFlagsUseOtherUserRecord = 0x01000000
};

typedef CALLBACK_API( OSStatus , FolderManagerNotificationProcPtr )(OSType message, void *arg, void *userRefCon);
typedef STACK_UPP_TYPE(FolderManagerNotificationProcPtr)        FolderManagerNotificationUPP;
/*
 *  NewFolderManagerNotificationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( FolderManagerNotificationUPP )
NewFolderManagerNotificationUPP(FolderManagerNotificationProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppFolderManagerNotificationProcInfo = 0x00000FF0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(FolderManagerNotificationUPP) NewFolderManagerNotificationUPP(FolderManagerNotificationProcPtr userRoutine) { return (FolderManagerNotificationUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppFolderManagerNotificationProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewFolderManagerNotificationUPP(userRoutine) (FolderManagerNotificationUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppFolderManagerNotificationProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeFolderManagerNotificationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeFolderManagerNotificationUPP(FolderManagerNotificationUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeFolderManagerNotificationUPP(FolderManagerNotificationUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeFolderManagerNotificationUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeFolderManagerNotificationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeFolderManagerNotificationUPP(
  OSType                        message,
  void *                        arg,
  void *                        userRefCon,
  FolderManagerNotificationUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeFolderManagerNotificationUPP(OSType message, void * arg, void * userRefCon, FolderManagerNotificationUPP userUPP) { return (OSStatus)CALL_THREE_PARAMETER_UPP(userUPP, uppFolderManagerNotificationProcInfo, message, arg, userRefCon); }
  #else
    #define InvokeFolderManagerNotificationUPP(message, arg, userRefCon, userUPP) (OSStatus)CALL_THREE_PARAMETER_UPP((userUPP), uppFolderManagerNotificationProcInfo, (message), (arg), (userRefCon))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewFolderManagerNotificationProc(userRoutine)       NewFolderManagerNotificationUPP(userRoutine)
    #define CallFolderManagerNotificationProc(userRoutine, message, arg, userRefCon) InvokeFolderManagerNotificationUPP(message, arg, userRefCon, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

enum {
  kFolderManagerNotificationMessageUserLogIn = FOUR_CHAR_CODE('log+'), /*    Sent by system & third party software after a user logs in.  arg should point to a valid FindFolderUserRedirectionGlobals structure or nil for the owner*/
  kFolderManagerNotificationMessagePreUserLogIn = FOUR_CHAR_CODE('logj'), /*    Sent by system & third party software before a user logs in.  arg should point to a valid FindFolderUserRedirectionGlobals structure or nil for the owner*/
  kFolderManagerNotificationMessageUserLogOut = FOUR_CHAR_CODE('log-'), /*    Sent by system & third party software before a user logs out.  arg should point to a valid FindFolderUserRedirectionGlobals structure or nil for the owner*/
  kFolderManagerNotificationMessagePostUserLogOut = FOUR_CHAR_CODE('logp'), /*    Sent by system & third party software after a user logs out.  arg should point to a valid FindFolderUserRedirectionGlobals structure or nil for the owner*/
  kFolderManagerNotificationDiscardCachedData = FOUR_CHAR_CODE('dche'), /*    Sent by system & third party software when the entire Folder Manager cache should be flushed*/
  kFolderManagerNotificationMessageLoginStartup = FOUR_CHAR_CODE('stup') /*    Sent by 'Login' application the first time it starts up after each boot*/
};


/*  These get used in the options parameter of FolderManagerRegisterNotificationProc()*/
enum {
  kDoNotRemoveWhenCurrentApplicationQuitsBit = 0,
  kDoNotRemoveWheCurrentApplicationQuitsBit = kDoNotRemoveWhenCurrentApplicationQuitsBit /*    Going away soon, use kDoNotRemoveWheCurrentApplicationQuitsBit*/
};

/*  These get used in the options parameter of FolderManagerCallNotificationProcs()*/
enum {
  kStopIfAnyNotificationProcReturnsErrorBit = 31
};

/***************************/
/* Folder Manager routines */
/***************************/
/* Folder Manager administration routines */
/*
 *  AddFolderDescriptor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FoldersLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AddFolderDescriptor(
  FolderType              foldType,
  FolderDescFlags         flags,
  FolderClass             foldClass,
  FolderLocation          foldLocation,
  OSType                  badgeSignature,
  OSType                  badgeType,
  ConstStrFileNameParam   name,
  Boolean                 replaceFlag)                        TWOWORDINLINE(0x7020, 0xA823);


/*
 *  GetFolderDescriptor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FoldersLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetFolderDescriptor(
  FolderType    foldType,
  Size          descSize,
  FolderDesc *  foldDesc)                                     TWOWORDINLINE(0x7023, 0xA823);


/*
 *  GetFolderTypes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FoldersLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetFolderTypes(
  UInt32        requestedTypeCount,
  UInt32 *      totalTypeCount,
  FolderType *  theTypes)                                     TWOWORDINLINE(0x7024, 0xA823);


/*
 *  RemoveFolderDescriptor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FoldersLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
RemoveFolderDescriptor(FolderType foldType)                   TWOWORDINLINE(0x7021, 0xA823);


/* legacy routines */
/*
 *  GetFolderName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FoldersLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetFolderName(
  short         vRefNum,
  OSType        foldType,
  short *       foundVRefNum,
  StrFileName   name)                                         TWOWORDINLINE(0x700E, 0xA823);


/* routing routines */
/*
 *  AddFolderRouting()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FoldersLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AddFolderRouting(
  OSType         fileType,
  FolderType     routeFromFolder,
  FolderType     routeToFolder,
  RoutingFlags   flags,
  Boolean        replaceFlag)                                 THREEWORDINLINE(0x303C, 0x0926, 0xA823);


/*
 *  RemoveFolderRouting()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FoldersLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
RemoveFolderRouting(
  OSType       fileType,
  FolderType   routeFromFolder)                               THREEWORDINLINE(0x303C, 0x0427, 0xA823);


/*
 *  FindFolderRouting()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FoldersLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FindFolderRouting(
  OSType          fileType,
  FolderType      routeFromFolder,
  FolderType *    routeToFolder,
  RoutingFlags *  flags)                                      THREEWORDINLINE(0x303C, 0x0822, 0xA823);


/*
 *  GetFolderRoutings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FoldersLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetFolderRoutings(
  UInt32           requestedRoutingCount,
  UInt32 *         totalRoutingCount,
  Size             routingSize,
  FolderRouting *  theRoutings)                               THREEWORDINLINE(0x303C, 0x081E, 0xA823);


/*
 *  InvalidateFolderDescriptorCache()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FoldersLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
InvalidateFolderDescriptorCache(
  short   vRefNum,
  long    dirID)                                              THREEWORDINLINE(0x303C, 0x0325, 0xA823);


/*
 *  IdentifyFolder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FoldersLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
IdentifyFolder(
  short         vRefNum,
  long          dirID,
  FolderType *  foldType)                                     THREEWORDINLINE(0x303C, 0x051F, 0xA823);



/*
 *  FolderManagerRegisterNotificationProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FolderManagerRegisterNotificationProc(
  FolderManagerNotificationUPP   notificationProc,
  void *                         refCon,
  UInt32                         options)                     THREEWORDINLINE(0x303C, 0x062F, 0xA823);


/*
 *  FolderManagerUnregisterNotificationProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FolderManagerUnregisterNotificationProc(
  FolderManagerNotificationUPP   notificationProc,
  void *                         refCon)                      THREEWORDINLINE(0x303C, 0x0430, 0xA823);


/*
 *  FolderManagerRegisterCallNotificationProcs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
FolderManagerRegisterCallNotificationProcs(
  OSType   message,
  void *   arg,
  UInt32   options)                                           THREEWORDINLINE(0x303C, 0x0631, 0xA823);


/*******************************/
/* MultiUser (At Ease) globals */
/*******************************/
/*
   This structure has been through some evolution since the early days of At Ease 1.0.  The structure
   has been expanded (and developers should assume that it will continue this way into the future).  Older
   fields have been obsoleted as the features have changed in newer versions of the code.
*/

/* Some fields in here are really only valid for the network version of Macintosh Manager*/

struct MultiUserGestalt {
                                              /*    Version 1 fields.*/
  short               giVersion;              /* structure version: 0 = invalid, 6 = OS 9*/
  short               giReserved0;            /* [OBSOLETE with v3] giIsActive: if true then At Ease is currently running*/
  short               giReserved1;            /* [OBSOLETE] if true then auto create alias*/
  short               giReserved2;            /* [OBSOLETE with v6]  if true then request floppy on new saves*/
  short               giReserved3;            /* [OBSOLETE] if true then hypercard stacks are shown on Applications panel*/
  FSSpec              giReserved4;            /* [OBSOLETE with v6] location of At Ease Items folder*/

                                              /*    Version 2 fields.*/
  short               giDocsVRefNum;          /* vrefnum of user's documents location (only valid if not on floppy)*/
  long                giDocsDirID;            /* directory id of user's documents folder (only valid if not on floppy)*/
  short               giForceSaves;           /* true if user is forced to save to their documents folder*/
  short               giForceOpens;           /* true if user is forced to open from their documents folder*/
  Str31               giSetupName;            /* name of current setup*/
  Str31               giUserName;             /* name of current user*/
  Str31               giFrontAppName;         /* name of the frontmost application*/
  short               giReserved5;            /* [OBSOLETE with v6] true if user has Go To Finder menu item*/
  short               giIsOn;                 /* true if Multiple Users/Macintosh Manager is on right now*/

                                              /*    Version 3 fields.*/
                                              /*  There were no additional fields for version 3.x*/

                                              /*    Version 4 fields.*/
  short               giUserLoggedInType;     /* 0 = normal user, 1 = workgroup admin, 2 = global admin*/
  char                giUserEncryptPwd[16];   /* encrypted user password (our digest form)*/
  short               giUserEnvironment;      /* 0 = panels, 1 = normal Finder, 2 = limited/restricted Finder*/
  long                giReserved6;            /* [OBSOLETE]*/
  long                giReserved7;            /* [OBSOLETE]*/
  Boolean             giDisableScrnShots;     /* true if screen shots are not allowed*/

                                              /*    Version 5 fields.*/
  Boolean             giSupportsAsyncFSCalls; /* Finder uses this to tell if our patches support async trap patches*/
  short               giPrefsVRefNum;         /* vrefnum of preferences*/
  long                giPrefsDirID;           /* dirID of the At Ease Items folder on preferences volume*/
  unsigned long       giUserLogInTime;        /* time in seconds we've been logged in (0 or 1 mean not logged in)*/
  Boolean             giUsingPrintQuotas;     /* true if logged in user is using printer quotas*/
  Boolean             giUsingDiskQuotas;      /* true if logged in user has disk quotas active*/

                                              /* Version 6 fields - As of Mac OS 9's "Multiple Users 1.0"*/
  Boolean             giInSystemAccess;       /* true if system is in System Access (i.e., owner logged in)*/
  Boolean             giUserFolderEnabled;    /* true if FindFolder is redirecting folders (uses giUserName for user)*/
  short               giReserved8;
  long                giReserved9;
  Boolean             giInLoginScreen;        /* true if no user has logged in (including owner)*/


                                              /* May have more fields added in future, so never check for sizeof(GestaltRec)*/

};
typedef struct MultiUserGestalt         MultiUserGestalt;
typedef MultiUserGestalt *              MultiUserGestaltPtr;
typedef MultiUserGestaltPtr *           MultiUserGestaltHandle;



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

#endif /* __FOLDERS__ */

