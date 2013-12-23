/*
     File:       Displays.h
 
     Contains:   Display Manager Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1993-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __DISPLAYS__
#define __DISPLAYS__

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

#ifndef __COMPONENTS__
#include <Components.h>
#endif

#ifndef __VIDEO__
#include <Video.h>
#endif

#ifndef __CMAPPLICATION__
#include <CMApplication.h>
#endif

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
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

typedef void *                          DMProcessInfoPtr;
typedef void *                          DMModalFilterUPP;
enum {
                                        /* AppleEvents Core Suite */
  kAESystemConfigNotice         = FOUR_CHAR_CODE('cnfg'), /* Core Suite types */
  kAEDisplayNotice              = FOUR_CHAR_CODE('dspl'),
  kAEDisplaySummary             = FOUR_CHAR_CODE('dsum'),
  keyDMConfigVersion            = FOUR_CHAR_CODE('dmcv'),
  keyDMConfigFlags              = FOUR_CHAR_CODE('dmcf'),
  keyDMConfigReserved           = FOUR_CHAR_CODE('dmcr'),
  keyDisplayID                  = FOUR_CHAR_CODE('dmid'),
  keyDisplayComponent           = FOUR_CHAR_CODE('dmdc'),
  keyDisplayDevice              = FOUR_CHAR_CODE('dmdd'),
  keyDisplayFlags               = FOUR_CHAR_CODE('dmdf'),
  keyDisplayMode                = FOUR_CHAR_CODE('dmdm'),
  keyDisplayModeReserved        = FOUR_CHAR_CODE('dmmr'),
  keyDisplayReserved            = FOUR_CHAR_CODE('dmdr'),
  keyDisplayMirroredId          = FOUR_CHAR_CODE('dmmi'),
  keyDeviceFlags                = FOUR_CHAR_CODE('dddf'),
  keyDeviceDepthMode            = FOUR_CHAR_CODE('dddm'),
  keyDeviceRect                 = FOUR_CHAR_CODE('dddr'),
  keyPixMapRect                 = FOUR_CHAR_CODE('dpdr'),
  keyPixMapHResolution          = FOUR_CHAR_CODE('dphr'),
  keyPixMapVResolution          = FOUR_CHAR_CODE('dpvr'),
  keyPixMapPixelType            = FOUR_CHAR_CODE('dppt'),
  keyPixMapPixelSize            = FOUR_CHAR_CODE('dpps'),
  keyPixMapCmpCount             = FOUR_CHAR_CODE('dpcc'),
  keyPixMapCmpSize              = FOUR_CHAR_CODE('dpcs'),
  keyPixMapAlignment            = FOUR_CHAR_CODE('dppa'),
  keyPixMapResReserved          = FOUR_CHAR_CODE('dprr'),
  keyPixMapReserved             = FOUR_CHAR_CODE('dppr'),
  keyPixMapColorTableSeed       = FOUR_CHAR_CODE('dpct'),
  keySummaryMenubar             = FOUR_CHAR_CODE('dsmb'),
  keySummaryChanges             = FOUR_CHAR_CODE('dsch'),
  keyDisplayOldConfig           = FOUR_CHAR_CODE('dold'),
  keyDisplayNewConfig           = FOUR_CHAR_CODE('dnew')
};

enum {
  dmOnlyActiveDisplays          = true,
  dmAllDisplays                 = false
};


enum {
                                        /* DMSendDependentNotification notifyClass */
  kDependentNotifyClassShowCursor = FOUR_CHAR_CODE('shcr'), /* When display mgr shows a hidden cursor during an unmirror */
  kDependentNotifyClassDriverOverride = FOUR_CHAR_CODE('ndrv'), /* When a driver is overridden */
  kDependentNotifyClassDisplayMgrOverride = FOUR_CHAR_CODE('dmgr'), /* When display manager is upgraded */
  kDependentNotifyClassProfileChanged = FOUR_CHAR_CODE('prof') /* When DMSetProfileByAVID is called */
};


enum {
                                        /* Switch Flags */
  kNoSwitchConfirmBit           = 0,    /* Flag indicating that there is no need to confirm a switch to this mode */
  kDepthNotAvailableBit         = 1,    /* Current depth not available in new mode */
  kShowModeBit                  = 3,    /* Show this mode even though it requires a confirm. */
  kModeNotResizeBit             = 4,    /* Do not use this mode to resize display (for cards that mode drives a different connector). */
  kNeverShowModeBit             = 5     /* This mode should not be shown in the user interface. */
};

/*    Summary Change Flags (sticky bits indicating an operation was performed)
    For example, moving a display then moving it back will still set the kMovedDisplayBit.
*/
enum {
  kBeginEndConfigureBit         = 0,
  kMovedDisplayBit              = 1,
  kSetMainDisplayBit            = 2,
  kSetDisplayModeBit            = 3,
  kAddDisplayBit                = 4,
  kRemoveDisplayBit             = 5,
  kNewDisplayBit                = 6,
  kDisposeDisplayBit            = 7,
  kEnabledDisplayBit            = 8,
  kDisabledDisplayBit           = 9,
  kMirrorDisplayBit             = 10,
  kUnMirrorDisplayBit           = 11
};


enum {
                                        /* Notification Messages for extended call back routines */
  kDMNotifyRequestConnectionProbe = 0,  /* Like kDMNotifyRequestDisplayProbe only not for smart displays (used in wake before all busses are awake) */
  kDMNotifyInstalled            = 1,    /* At install time */
  kDMNotifyEvent                = 2,    /* Post change time */
  kDMNotifyRemoved              = 3,    /* At remove time */
  kDMNotifyPrep                 = 4,    /* Pre change time */
  kDMNotifyExtendEvent          = 5,    /* Allow registrees to extend apple event before it is sent */
  kDMNotifyDependents           = 6,    /* Minor notification check without full update */
  kDMNotifySuspendConfigure     = 7,    /* Temporary end of configuration */
  kDMNotifyResumeConfigure      = 8,    /* Resume configuration */
  kDMNotifyRequestDisplayProbe  = 9,    /* Request smart displays re-probe (used in sleep and hot plugging) */
  kDMNotifyDisplayWillSleep     = 10,   /* Mac OS X only */
  kDMNotifyDisplayDidWake       = 11,   /* Mac OS X only */
                                        /* Notification Flags */
  kExtendedNotificationProc     = (1L << 16)
};


/* types for notifyType */
enum {
  kFullNotify                   = 0,    /* This is the appleevent whole nine yards notify */
  kFullDependencyNotify         = 1     /* Only sends to those who want to know about interrelated functionality (used for updating UI) */
};

/* DisplayID/DeviceID constants */
enum {
  kDummyDeviceID                = 0x00FF, /* This is the ID of the dummy display, used when the last "real" display is disabled.*/
  kInvalidDisplayID             = 0x0000, /* This is the invalid ID*/
  kFirstDisplayID               = 0x0100
};

enum {
                                        /* bits for panelListFlags */
  kAllowDuplicatesBit           = 0
};

enum {
                                        /* bits for nameFlags */
  kSuppressNumberBit            = 0,
  kSuppressNumberMask           = 1,
  kForceNumberBit               = 1,
  kForceNumberMask              = 2,
  kSuppressNameBit              = 2,
  kSuppressNameMask             = 4
};

/* DMGetNameByAVID masks*/
enum {
  kDMSupressNumbersMask         = (1 << 0), /* Supress the numbers and return only names*/
  kDMForceNumbersMask           = (1 << 1), /* Force numbers to always be shown (even on single display configs)*/
  kDMSupressNameMask            = (1 << 2) /* Supress the names and return only numbers.*/
};



/* Constants for fidelity checks */
enum {
  kNoFidelity                   = 0,
  kMinimumFidelity              = 1,
  kDefaultFidelity              = 500,  /* I'm just picking a number for Apple default panels and engines*/
  kDefaultManufacturerFidelity  = 1000  /* I'm just picking a number for Manufacturer's panels and engines (overrides apple defaults)*/
};

enum {
  kAnyPanelType                 = 0,    /* Pass to DMNewEngineList for list of all panels (as opposed to specific types)*/
  kAnyEngineType                = 0,    /* Pass to DMNewEngineList for list of all engines*/
  kAnyDeviceType                = 0,    /* Pass to DMNewDeviceList for list of all devices*/
  kAnyPortType                  = 0     /* Pass to DMNewDevicePortList for list of all devices*/
};

/* portListFlags for DM_NewDevicePortList */
enum {
                                        /* Should offline devices be put into the port list (such as dummy display) */
  kPLIncludeOfflineDevicesBit   = 0
};


/* confirmFlags for DMConfirmConfiguration */
enum {
  kForceConfirmBit              = 0,    /* Force a confirm dialog */
  kForceConfirmMask             = (1 << kForceConfirmBit)
};


/* Flags for displayModeFlags */
enum {
  kDisplayModeListNotPreferredBit = 0,
  kDisplayModeListNotPreferredMask = (1 << kDisplayModeListNotPreferredBit)
};


/* Flags for itemFlags */
enum {
  kComponentListNotPreferredBit = 0,
  kComponentListNotPreferredMask = (1 << kComponentListNotPreferredBit)
};

enum {
  kDisplayTimingInfoVersionZero = 1,
  kDisplayTimingInfoReservedCountVersionZero = 16,
  kDisplayModeEntryVersionZero  = 0,    /* displayModeVersion - original version*/
  kDisplayModeEntryVersionOne   = 1     /* displayModeVersion - added displayModeOverrideInfo*/
};


enum {
  kMakeAndModelReservedCount    = 4     /* Number of reserved fields*/
};


/* Display Gestalt for DMDisplayGestalt*/
enum {
  kDisplayGestaltDisplayCommunicationAttr = FOUR_CHAR_CODE('comm'),
  kDisplayGestaltForbidI2CMask  = (1 << 0), /* Some displays have firmware problems if they get I2C communication.  If this bit is set, then I2C communication is forbidden*/
  kDisplayGestaltUseI2CPowerMask = (1 << 1), /* Some displays require I2C power settings (most use DPMS).*/
  kDisplayGestaltCalibratorAttr = FOUR_CHAR_CODE('cali'),
  kDisplayGestaltBrightnessAffectsGammaMask = (1 << 0), /* Used by default calibrator (should we show brightness panel) */
  kDisplayGestaltViewAngleAffectsGammaMask = (1 << 1) /* Currently not used by color sync*/
};


typedef UInt32                          DMFidelityType;
/*
   AVID is an ID for ports and devices the old DisplayID type
    is carried on for compatibility
*/


typedef void *                          DMListType;
typedef unsigned long                   DMListIndexType;
typedef VDPowerStateRec                 AVPowerStateRec;
typedef VDPowerStateRec *               AVPowerStatePtr;
struct DMDisplayTimingInfoRec {
  UInt32              timingInfoVersion;
  UInt32              timingInfoAttributes;   /* Flags */
  SInt32              timingInfoRelativeQuality; /* quality of the timing */
  SInt32              timingInfoRelativeDefault; /* relative default of the timing */

  UInt32              timingInfoReserved[16]; /* Reserved */
};
typedef struct DMDisplayTimingInfoRec   DMDisplayTimingInfoRec;
typedef DMDisplayTimingInfoRec *        DMDisplayTimingInfoPtr;

struct DMComponentListEntryRec {
  DisplayIDType       itemID;                 /* DisplayID Manager*/
  Component           itemComponent;          /* Component Manager*/
  ComponentDescription  itemDescription;      /* We can always construct this if we use something beyond the compontent mgr.*/

  ResType             itemClass;              /* Class of group to put this panel (eg geometry/color/etc for panels, brightness/contrast for engines, video out/sound/etc for devices)*/
  DMFidelityType      itemFidelity;           /* How good is this item for the specified search?*/
  ResType             itemSubClass;           /* Subclass of group to put this panel.  Can use to do sub-grouping (eg volume for volume panel and mute panel)*/
  Point               itemSort;               /* Set to 0 - future to sort the items in a sub group.*/

  unsigned long       itemFlags;              /* Set to 0 (future expansion)*/
  ResType             itemReserved;           /* What kind of code does the itemReference point to  (right now - kPanelEntryTypeComponentMgr only)*/
  unsigned long       itemFuture1;            /* Set to 0 (future expansion - probably an alternate code style)*/
  unsigned long       itemFuture2;            /* Set to 0 (future expansion - probably an alternate code style)*/
  unsigned long       itemFuture3;            /* Set to 0 (future expansion - probably an alternate code style)*/
  unsigned long       itemFuture4;            /* Set to 0 (future expansion - probably an alternate code style)*/
};
typedef struct DMComponentListEntryRec  DMComponentListEntryRec;
typedef DMComponentListEntryRec *       DMComponentListEntryPtr;
/* ooo Move AVLocationRec to AVComponents.i AFTER AVComponents.i is created*/
struct AVLocationRec {
  unsigned long       locationConstant;       /* Set to 0 (future expansion - probably an alternate code style)*/
};
typedef struct AVLocationRec            AVLocationRec;
typedef AVLocationRec *                 AVLocationPtr;
struct DMDepthInfoRec {
  VDSwitchInfoPtr     depthSwitchInfo;        /* This is the switch mode to choose this timing/depth */
  VPBlockPtr          depthVPBlock;           /* VPBlock (including size, depth and format) */
  UInt32              depthFlags;             /* VDVideoParametersInfoRec.csDepthFlags  */
  UInt32              depthReserved1;         /* Reserved */
  UInt32              depthReserved2;         /* Reserved */
};
typedef struct DMDepthInfoRec           DMDepthInfoRec;
typedef DMDepthInfoRec *                DMDepthInfoPtr;
struct DMDepthInfoBlockRec {
  unsigned long       depthBlockCount;        /* How many depths are there? */
  DMDepthInfoPtr      depthVPBlock;           /* Array of DMDepthInfoRec */
  unsigned long       depthBlockFlags;        /* Reserved */
  unsigned long       depthBlockReserved1;    /* Reserved */
  unsigned long       depthBlockReserved2;    /* Reserved */
};
typedef struct DMDepthInfoBlockRec      DMDepthInfoBlockRec;
typedef DMDepthInfoBlockRec *           DMDepthInfoBlockPtr;
struct DMDisplayModeListEntryRec {
  UInt32              displayModeFlags;
  VDSwitchInfoPtr     displayModeSwitchInfo;
  VDResolutionInfoPtr  displayModeResolutionInfo;
  VDTimingInfoPtr     displayModeTimingInfo;
  DMDepthInfoBlockPtr  displayModeDepthBlockInfo; /* Information about all the depths*/
  UInt32              displayModeVersion;     /* What version is this record (now kDisplayModeEntryVersionOne)*/
  StringPtr           displayModeName;        /* Name of the timing mode*/
  DMDisplayTimingInfoPtr  displayModeDisplayInfo; /* Information from the display.*/
};
typedef struct DMDisplayModeListEntryRec DMDisplayModeListEntryRec;
typedef DMDisplayModeListEntryRec *     DMDisplayModeListEntryPtr;

struct DependentNotifyRec {
  ResType             notifyType;             /* What type was the engine that made the change (may be zero)*/
  ResType             notifyClass;            /* What class was the change (eg geometry, color etc)*/
  DisplayIDType       notifyPortID;           /* Which device was touched (kInvalidDisplayID -> all or none)*/
  ComponentInstance   notifyComponent;        /* What engine did it (may be 0)?*/

  unsigned long       notifyVersion;          /* Set to 0 (future expansion)*/
  unsigned long       notifyFlags;            /* Set to 0 (future expansion)*/
  unsigned long       notifyReserved;         /* Set to 0 (future expansion)*/
  unsigned long       notifyFuture;           /* Set to 0 (future expansion)*/
};
typedef struct DependentNotifyRec       DependentNotifyRec;
typedef DependentNotifyRec *            DependentNotifyPtr;

struct DMMakeAndModelRec {
  ResType             manufacturer;
  UInt32              model;
  UInt32              serialNumber;
  UInt32              manufactureDate;

  UInt32              makeReserved[4];
};
typedef struct DMMakeAndModelRec        DMMakeAndModelRec;
typedef DMMakeAndModelRec *             DMMakeAndModelPtr;
/* DMNewDisplayList displayListIncludeFlags*/
enum {
  kIncludeOnlineActiveDisplaysMask = (1 << 0),
  kIncludeOnlineDisabledDisplaysMask = (1 << 1),
  kIncludeOfflineDisplaysMask   = (1 << 2),
  kIncludeOfflineDummyDisplaysMask = (1 << 3),
  kIncludeHardwareMirroredDisplaysMask = (1 << 4)
};


enum {
                                        /* modeListFlags for DMNewDisplayModeList */
  kDMModeListIncludeAllModesMask = (1 << 0), /* Include all timing modes not _explicitly_ excluded (see other bits)*/
  kDMModeListIncludeOfflineModesMask = (1 << 1),
  kDMModeListExcludeDriverModesMask = (1 << 2), /* Exclude old-style timing modes (cscGetNextResolution/kDisplayModeIDFindFirstResolution modes)*/
  kDMModeListExcludeDisplayModesMask = (1 << 3), /* Exclude timing modes that come from the display (always arbritrary timing modes)*/
  kDMModeListExcludeCustomModesMask = (1 << 4), /* Exclude custom modes that came neither from the driver or display (need a better name)*/
  kDMModeListPreferStretchedModesMask = (1 << 5), /* Prefer modes that are stretched over modes that are letterboxed when setting kDisplayModeListNotPreferredBit*/
  kDMModeListPreferSafeModesMask = (1 << 6) /* Prefer modes that are safe over modes that are not when setting kDisplayModeListNotPreferredBit*/
};


/* DMNewDisplayList displayListFlags*/
struct DisplayListEntryRec {
  GDHandle            displayListEntryGDevice;
  DisplayIDType       displayListEntryDisplayID;
  UInt32              displayListEntryIncludeFlags; /* Reason this entry was included*/
  UInt32              displayListEntryReserved1;

  UInt32              displayListEntryReserved2; /* Zero*/
  UInt32              displayListEntryReserved3; /* Zero*/
  UInt32              displayListEntryReserved4; /* Zero*/
  UInt32              displayListEntryReserved5; /* Zero*/
};
typedef struct DisplayListEntryRec      DisplayListEntryRec;
typedef DisplayListEntryRec *           DisplayListEntryPtr;
struct DMProfileListEntryRec {
  CMProfileRef        profileRef;
  Ptr                 profileReserved1;       /* Reserved*/
  Ptr                 profileReserved2;       /* Reserved*/
  Ptr                 profileReserved3;       /* Reserved*/
};
typedef struct DMProfileListEntryRec    DMProfileListEntryRec;
typedef DMProfileListEntryRec *         DMProfileListEntryPtr;
typedef CALLBACK_API( void , DMNotificationProcPtr )(AppleEvent * theEvent);
typedef CALLBACK_API( void , DMExtendedNotificationProcPtr )(void *userData, short theMessage, void *notifyData);
typedef CALLBACK_API( void , DMComponentListIteratorProcPtr )(void *userData, DMListIndexType itemIndex, DMComponentListEntryPtr componentInfo);
typedef CALLBACK_API( void , DMDisplayModeListIteratorProcPtr )(void *userData, DMListIndexType itemIndex, DMDisplayModeListEntryPtr displaymodeInfo);
typedef CALLBACK_API( void , DMProfileListIteratorProcPtr )(void *userData, DMListIndexType itemIndex, DMProfileListEntryPtr profileInfo);
typedef CALLBACK_API( void , DMDisplayListIteratorProcPtr )(void *userData, DMListIndexType itemIndex, DisplayListEntryPtr displaymodeInfo);
typedef STACK_UPP_TYPE(DMNotificationProcPtr)                   DMNotificationUPP;
typedef STACK_UPP_TYPE(DMExtendedNotificationProcPtr)           DMExtendedNotificationUPP;
typedef STACK_UPP_TYPE(DMComponentListIteratorProcPtr)          DMComponentListIteratorUPP;
typedef STACK_UPP_TYPE(DMDisplayModeListIteratorProcPtr)        DMDisplayModeListIteratorUPP;
typedef STACK_UPP_TYPE(DMProfileListIteratorProcPtr)            DMProfileListIteratorUPP;
typedef STACK_UPP_TYPE(DMDisplayListIteratorProcPtr)            DMDisplayListIteratorUPP;
/*
 *  NewDMNotificationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DMNotificationUPP )
NewDMNotificationUPP(DMNotificationProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDMNotificationProcInfo = 0x000000C0 };  /* pascal no_return_value Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DMNotificationUPP) NewDMNotificationUPP(DMNotificationProcPtr userRoutine) { return (DMNotificationUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDMNotificationProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDMNotificationUPP(userRoutine) (DMNotificationUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDMNotificationProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewDMExtendedNotificationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DMExtendedNotificationUPP )
NewDMExtendedNotificationUPP(DMExtendedNotificationProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDMExtendedNotificationProcInfo = 0x00000EC0 };  /* pascal no_return_value Func(4_bytes, 2_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DMExtendedNotificationUPP) NewDMExtendedNotificationUPP(DMExtendedNotificationProcPtr userRoutine) { return (DMExtendedNotificationUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDMExtendedNotificationProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDMExtendedNotificationUPP(userRoutine) (DMExtendedNotificationUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDMExtendedNotificationProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewDMComponentListIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DMComponentListIteratorUPP )
NewDMComponentListIteratorUPP(DMComponentListIteratorProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDMComponentListIteratorProcInfo = 0x00000FC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DMComponentListIteratorUPP) NewDMComponentListIteratorUPP(DMComponentListIteratorProcPtr userRoutine) { return (DMComponentListIteratorUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDMComponentListIteratorProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDMComponentListIteratorUPP(userRoutine) (DMComponentListIteratorUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDMComponentListIteratorProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewDMDisplayModeListIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DMDisplayModeListIteratorUPP )
NewDMDisplayModeListIteratorUPP(DMDisplayModeListIteratorProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDMDisplayModeListIteratorProcInfo = 0x00000FC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DMDisplayModeListIteratorUPP) NewDMDisplayModeListIteratorUPP(DMDisplayModeListIteratorProcPtr userRoutine) { return (DMDisplayModeListIteratorUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDMDisplayModeListIteratorProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDMDisplayModeListIteratorUPP(userRoutine) (DMDisplayModeListIteratorUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDMDisplayModeListIteratorProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewDMProfileListIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DMProfileListIteratorUPP )
NewDMProfileListIteratorUPP(DMProfileListIteratorProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDMProfileListIteratorProcInfo = 0x00000FC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DMProfileListIteratorUPP) NewDMProfileListIteratorUPP(DMProfileListIteratorProcPtr userRoutine) { return (DMProfileListIteratorUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDMProfileListIteratorProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDMProfileListIteratorUPP(userRoutine) (DMProfileListIteratorUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDMProfileListIteratorProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewDMDisplayListIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DMDisplayListIteratorUPP )
NewDMDisplayListIteratorUPP(DMDisplayListIteratorProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDMDisplayListIteratorProcInfo = 0x00000FC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DMDisplayListIteratorUPP) NewDMDisplayListIteratorUPP(DMDisplayListIteratorProcPtr userRoutine) { return (DMDisplayListIteratorUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDMDisplayListIteratorProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDMDisplayListIteratorUPP(userRoutine) (DMDisplayListIteratorUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDMDisplayListIteratorProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeDMNotificationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDMNotificationUPP(DMNotificationUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDMNotificationUPP(DMNotificationUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDMNotificationUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeDMExtendedNotificationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDMExtendedNotificationUPP(DMExtendedNotificationUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDMExtendedNotificationUPP(DMExtendedNotificationUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDMExtendedNotificationUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeDMComponentListIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDMComponentListIteratorUPP(DMComponentListIteratorUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDMComponentListIteratorUPP(DMComponentListIteratorUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDMComponentListIteratorUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeDMDisplayModeListIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDMDisplayModeListIteratorUPP(DMDisplayModeListIteratorUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDMDisplayModeListIteratorUPP(DMDisplayModeListIteratorUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDMDisplayModeListIteratorUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeDMProfileListIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDMProfileListIteratorUPP(DMProfileListIteratorUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDMProfileListIteratorUPP(DMProfileListIteratorUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDMProfileListIteratorUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeDMDisplayListIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDMDisplayListIteratorUPP(DMDisplayListIteratorUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDMDisplayListIteratorUPP(DMDisplayListIteratorUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDMDisplayListIteratorUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeDMNotificationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDMNotificationUPP(
  AppleEvent *       theEvent,
  DMNotificationUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDMNotificationUPP(AppleEvent * theEvent, DMNotificationUPP userUPP) { CALL_ONE_PARAMETER_UPP(userUPP, uppDMNotificationProcInfo, theEvent); }
  #else
    #define InvokeDMNotificationUPP(theEvent, userUPP) CALL_ONE_PARAMETER_UPP((userUPP), uppDMNotificationProcInfo, (theEvent))
  #endif
#endif

/*
 *  InvokeDMExtendedNotificationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDMExtendedNotificationUPP(
  void *                     userData,
  short                      theMessage,
  void *                     notifyData,
  DMExtendedNotificationUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDMExtendedNotificationUPP(void * userData, short theMessage, void * notifyData, DMExtendedNotificationUPP userUPP) { CALL_THREE_PARAMETER_UPP(userUPP, uppDMExtendedNotificationProcInfo, userData, theMessage, notifyData); }
  #else
    #define InvokeDMExtendedNotificationUPP(userData, theMessage, notifyData, userUPP) CALL_THREE_PARAMETER_UPP((userUPP), uppDMExtendedNotificationProcInfo, (userData), (theMessage), (notifyData))
  #endif
#endif

/*
 *  InvokeDMComponentListIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDMComponentListIteratorUPP(
  void *                      userData,
  DMListIndexType             itemIndex,
  DMComponentListEntryPtr     componentInfo,
  DMComponentListIteratorUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDMComponentListIteratorUPP(void * userData, DMListIndexType itemIndex, DMComponentListEntryPtr componentInfo, DMComponentListIteratorUPP userUPP) { CALL_THREE_PARAMETER_UPP(userUPP, uppDMComponentListIteratorProcInfo, userData, itemIndex, componentInfo); }
  #else
    #define InvokeDMComponentListIteratorUPP(userData, itemIndex, componentInfo, userUPP) CALL_THREE_PARAMETER_UPP((userUPP), uppDMComponentListIteratorProcInfo, (userData), (itemIndex), (componentInfo))
  #endif
#endif

/*
 *  InvokeDMDisplayModeListIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDMDisplayModeListIteratorUPP(
  void *                        userData,
  DMListIndexType               itemIndex,
  DMDisplayModeListEntryPtr     displaymodeInfo,
  DMDisplayModeListIteratorUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDMDisplayModeListIteratorUPP(void * userData, DMListIndexType itemIndex, DMDisplayModeListEntryPtr displaymodeInfo, DMDisplayModeListIteratorUPP userUPP) { CALL_THREE_PARAMETER_UPP(userUPP, uppDMDisplayModeListIteratorProcInfo, userData, itemIndex, displaymodeInfo); }
  #else
    #define InvokeDMDisplayModeListIteratorUPP(userData, itemIndex, displaymodeInfo, userUPP) CALL_THREE_PARAMETER_UPP((userUPP), uppDMDisplayModeListIteratorProcInfo, (userData), (itemIndex), (displaymodeInfo))
  #endif
#endif

/*
 *  InvokeDMProfileListIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDMProfileListIteratorUPP(
  void *                    userData,
  DMListIndexType           itemIndex,
  DMProfileListEntryPtr     profileInfo,
  DMProfileListIteratorUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDMProfileListIteratorUPP(void * userData, DMListIndexType itemIndex, DMProfileListEntryPtr profileInfo, DMProfileListIteratorUPP userUPP) { CALL_THREE_PARAMETER_UPP(userUPP, uppDMProfileListIteratorProcInfo, userData, itemIndex, profileInfo); }
  #else
    #define InvokeDMProfileListIteratorUPP(userData, itemIndex, profileInfo, userUPP) CALL_THREE_PARAMETER_UPP((userUPP), uppDMProfileListIteratorProcInfo, (userData), (itemIndex), (profileInfo))
  #endif
#endif

/*
 *  InvokeDMDisplayListIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDMDisplayListIteratorUPP(
  void *                    userData,
  DMListIndexType           itemIndex,
  DisplayListEntryPtr       displaymodeInfo,
  DMDisplayListIteratorUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDMDisplayListIteratorUPP(void * userData, DMListIndexType itemIndex, DisplayListEntryPtr displaymodeInfo, DMDisplayListIteratorUPP userUPP) { CALL_THREE_PARAMETER_UPP(userUPP, uppDMDisplayListIteratorProcInfo, userData, itemIndex, displaymodeInfo); }
  #else
    #define InvokeDMDisplayListIteratorUPP(userData, itemIndex, displaymodeInfo, userUPP) CALL_THREE_PARAMETER_UPP((userUPP), uppDMDisplayListIteratorProcInfo, (userData), (itemIndex), (displaymodeInfo))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewDMNotificationProc(userRoutine)                  NewDMNotificationUPP(userRoutine)
    #define NewDMExtendedNotificationProc(userRoutine)          NewDMExtendedNotificationUPP(userRoutine)
    #define NewDMComponentListIteratorProc(userRoutine)         NewDMComponentListIteratorUPP(userRoutine)
    #define NewDMDisplayModeListIteratorProc(userRoutine)       NewDMDisplayModeListIteratorUPP(userRoutine)
    #define NewDMProfileListIteratorProc(userRoutine)           NewDMProfileListIteratorUPP(userRoutine)
    #define NewDMDisplayListIteratorProc(userRoutine)           NewDMDisplayListIteratorUPP(userRoutine)
    #define CallDMNotificationProc(userRoutine, theEvent)       InvokeDMNotificationUPP(theEvent, userRoutine)
    #define CallDMExtendedNotificationProc(userRoutine, userData, theMessage, notifyData) InvokeDMExtendedNotificationUPP(userData, theMessage, notifyData, userRoutine)
    #define CallDMComponentListIteratorProc(userRoutine, userData, itemIndex, componentInfo) InvokeDMComponentListIteratorUPP(userData, itemIndex, componentInfo, userRoutine)
    #define CallDMDisplayModeListIteratorProc(userRoutine, userData, itemIndex, displaymodeInfo) InvokeDMDisplayModeListIteratorUPP(userData, itemIndex, displaymodeInfo, userRoutine)
    #define CallDMProfileListIteratorProc(userRoutine, userData, itemIndex, profileInfo) InvokeDMProfileListIteratorUPP(userData, itemIndex, profileInfo, userRoutine)
    #define CallDMDisplayListIteratorProc(userRoutine, userData, itemIndex, displaymodeInfo) InvokeDMDisplayListIteratorUPP(userData, itemIndex, displaymodeInfo, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON
/*
 *  DMDisplayGestalt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DMDisplayGestalt(
  DisplayIDType   theDisplayID,
  ResType         displayGestaltSelector,
  UInt32 *        displayGestaltResponse)                     THREEWORDINLINE(0x303C, 0x06D3, 0xABEB);


/*
 *  DMUseScreenPrefs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DMUseScreenPrefs(
  Boolean   usePrefs,
  Handle    displayState)                                     THREEWORDINLINE(0x303C, 0x03EC, 0xABEB);


/*
 *  DMSuspendConfigure()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DMSuspendConfigure(
  Handle          displayState,
  unsigned long   reserved1)                                  THREEWORDINLINE(0x303C, 0x04E9, 0xABEB);


/*
 *  DMResumeConfigure()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DMResumeConfigure(
  Handle          displayState,
  unsigned long   reserved1)                                  THREEWORDINLINE(0x303C, 0x04E8, 0xABEB);


/*
 *  DMSetGammaByAVID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DMSetGammaByAVID(
  AVIDType         gammaAVID,
  UInt32           setGammaFlags,
  GammaTblHandle   theGamma)                                  THREEWORDINLINE(0x303C, 0x06D1, 0xABEB);


/*
 *  DMGetGammaByAVID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DMGetGammaByAVID(
  AVIDType          gammaAVID,
  UInt32            getGammaFlags,
  GammaTblHandle *  theGamma)                                 THREEWORDINLINE(0x303C, 0x06D0, 0xABEB);


/*
 *  DMGetMakeAndModelByAVID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DMGetMakeAndModelByAVID(
  AVIDType            theAVID,
  DMMakeAndModelPtr   theMakeAndModel)                        THREEWORDINLINE(0x303C, 0x04D7, 0xABEB);


/*
 *  DMNewDisplayList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DMNewDisplayList(
  UInt32             displayListIncludeFlags,
  UInt32             reserved1,
  UInt32             reserved2,
  DMListIndexType *  theCount,
  DMListType *       theDisplayList)                          THREEWORDINLINE(0x303C, 0x0AD6, 0xABEB);


/*
 *  DMGetIndexedDisplayFromList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DMGetIndexedDisplayFromList(
  DMListType                 theDisplayList,
  DMListIndexType            itemIndex,
  UInt32                     reserved,
  DMDisplayListIteratorUPP   listIterator,
  void *                     userData)                        THREEWORDINLINE(0x303C, 0x0AD5, 0xABEB);


/*
 *  DMNewProfileListByAVID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DMNewProfileListByAVID(
  AVIDType           theAVID,
  UInt32             reserved,
  DMListIndexType *  profileCount,
  DMListType *       profileList)                             THREEWORDINLINE(0x303C, 0x08DC, 0xABEB);


/*
 *  DMGetIndexedProfileFromList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DMGetIndexedProfileFromList(
  DMListType                 profileList,
  DMListIndexType            itemIndex,
  UInt32                     reserved,
  DMProfileListIteratorUPP   listIterator,
  void *                     userData)                        THREEWORDINLINE(0x303C, 0x0ADB, 0xABEB);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  DMGetFirstScreenDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( GDHandle )
DMGetFirstScreenDevice(Boolean activeOnly)                    TWOWORDINLINE(0x7000, 0xABEB);


/*
 *  DMGetNextScreenDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( GDHandle )
DMGetNextScreenDevice(
  GDHandle   theDevice,
  Boolean    activeOnly)                                      TWOWORDINLINE(0x7001, 0xABEB);


/*
 *  DMDrawDesktopRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DMDrawDesktopRect(Rect * globalRect)                          TWOWORDINLINE(0x7002, 0xABEB);


/*
 *  DMDrawDesktopRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DMDrawDesktopRegion(RgnHandle globalRgn)                      TWOWORDINLINE(0x7003, 0xABEB);




/*
 *  DMBeginConfigureDisplays()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMBeginConfigureDisplays(Handle * displayState)               THREEWORDINLINE(0x303C, 0x0206, 0xABEB);


/*
 *  DMEndConfigureDisplays()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMEndConfigureDisplays(Handle displayState)                   THREEWORDINLINE(0x303C, 0x0207, 0xABEB);


/*
 *  DMAddDisplay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMAddDisplay(
  GDHandle        newDevice,
  short           driver,
  unsigned long   mode,
  unsigned long   reserved,
  unsigned long   displayID,
  Component       displayComponent,
  Handle          displayState)                               THREEWORDINLINE(0x303C, 0x0D08, 0xABEB);


/*
 *  DMMoveDisplay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMMoveDisplay(
  GDHandle   moveDevice,
  short      x,
  short      y,
  Handle     displayState)                                    THREEWORDINLINE(0x303C, 0x0609, 0xABEB);


/*
 *  DMDisableDisplay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMDisableDisplay(
  GDHandle   disableDevice,
  Handle     displayState)                                    THREEWORDINLINE(0x303C, 0x040A, 0xABEB);


/*
 *  DMEnableDisplay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMEnableDisplay(
  GDHandle   enableDevice,
  Handle     displayState)                                    THREEWORDINLINE(0x303C, 0x040B, 0xABEB);


/*
 *  DMRemoveDisplay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMRemoveDisplay(
  GDHandle   removeDevice,
  Handle     displayState)                                    THREEWORDINLINE(0x303C, 0x040C, 0xABEB);





/*
 *  DMSetMainDisplay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMSetMainDisplay(
  GDHandle   newMainDevice,
  Handle     displayState)                                    THREEWORDINLINE(0x303C, 0x0410, 0xABEB);


/*
 *  DMSetDisplayMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMSetDisplayMode(
  GDHandle         theDevice,
  unsigned long    mode,
  unsigned long *  depthMode,
  unsigned long    reserved,
  Handle           displayState)                              THREEWORDINLINE(0x303C, 0x0A11, 0xABEB);


/*
 *  DMCheckDisplayMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMCheckDisplayMode(
  GDHandle         theDevice,
  unsigned long    mode,
  unsigned long    depthMode,
  unsigned long *  switchFlags,
  unsigned long    reserved,
  Boolean *        modeOk)                                    THREEWORDINLINE(0x303C, 0x0C12, 0xABEB);


/*
 *  DMGetDeskRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetDeskRegion(RgnHandle * desktopRegion)                    THREEWORDINLINE(0x303C, 0x0213, 0xABEB);


/*
 *  DMRegisterNotifyProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMRegisterNotifyProc(
  DMNotificationUPP   notificationProc,
  DMProcessInfoPtr    whichPSN)                               THREEWORDINLINE(0x303C, 0x0414, 0xABEB);


/*
 *  DMRemoveNotifyProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMRemoveNotifyProc(
  DMNotificationUPP   notificationProc,
  DMProcessInfoPtr    whichPSN)                               THREEWORDINLINE(0x303C, 0x0415, 0xABEB);


/*
 *  DMQDIsMirroringCapable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMQDIsMirroringCapable(Boolean * qdIsMirroringCapable)        THREEWORDINLINE(0x303C, 0x0216, 0xABEB);


/*
 *  DMCanMirrorNow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMCanMirrorNow(Boolean * canMirrorNow)                        THREEWORDINLINE(0x303C, 0x0217, 0xABEB);


/*
 *  DMIsMirroringOn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMIsMirroringOn(Boolean * isMirroringOn)                      THREEWORDINLINE(0x303C, 0x0218, 0xABEB);


/*
 *  DMMirrorDevices()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMMirrorDevices(
  GDHandle   gD1,
  GDHandle   gD2,
  Handle     displayState)                                    THREEWORDINLINE(0x303C, 0x0619, 0xABEB);


/*
 *  DMUnmirrorDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMUnmirrorDevice(
  GDHandle   gDevice,
  Handle     displayState)                                    THREEWORDINLINE(0x303C, 0x041A, 0xABEB);


/*
 *  DMGetNextMirroredDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetNextMirroredDevice(
  GDHandle    gDevice,
  GDHandle *  mirroredDevice)                                 THREEWORDINLINE(0x303C, 0x041B, 0xABEB);


/*
 *  DMBlockMirroring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMBlockMirroring(void)                                        TWOWORDINLINE(0x701C, 0xABEB);


/*
 *  DMUnblockMirroring()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMUnblockMirroring(void)                                      TWOWORDINLINE(0x701D, 0xABEB);


#if CALL_NOT_IN_CARBON
/*
 *  DMGetDisplayMgrA5World()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DMGetDisplayMgrA5World(Ptr * dmA5)                            THREEWORDINLINE(0x303C, 0x021E, 0xABEB);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  DMGetDisplayIDByGDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetDisplayIDByGDevice(
  GDHandle         displayDevice,
  DisplayIDType *  displayID,
  Boolean          failToMain)                                THREEWORDINLINE(0x303C, 0x051F, 0xABEB);


/*
 *  DMGetGDeviceByDisplayID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetGDeviceByDisplayID(
  DisplayIDType   displayID,
  GDHandle *      displayDevice,
  Boolean         failToMain)                                 THREEWORDINLINE(0x303C, 0x0520, 0xABEB);


/*
 *  DMSetDisplayComponent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMSetDisplayComponent(
  GDHandle    theDevice,
  Component   displayComponent)                               THREEWORDINLINE(0x303C, 0x0421, 0xABEB);


/*
 *  DMGetDisplayComponent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetDisplayComponent(
  GDHandle     theDevice,
  Component *  displayComponent)                              THREEWORDINLINE(0x303C, 0x0422, 0xABEB);


/*
 *  DMNewDisplay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMNewDisplay(
  GDHandle *      newDevice,
  short           driverRefNum,
  unsigned long   mode,
  unsigned long   reserved,
  DisplayIDType   displayID,
  Component       displayComponent,
  Handle          displayState)                               THREEWORDINLINE(0x303C, 0x0D23, 0xABEB);


/*
 *  DMDisposeDisplay()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMDisposeDisplay(
  GDHandle   disposeDevice,
  Handle     displayState)                                    THREEWORDINLINE(0x303C, 0x0424, 0xABEB);


/*
 *  DMResolveDisplayComponents()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMResolveDisplayComponents(void)                              TWOWORDINLINE(0x7025, 0xABEB);


/*
 *  DMRegisterExtendedNotifyProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMRegisterExtendedNotifyProc(
  DMExtendedNotificationUPP   notifyProc,
  void *                      notifyUserData,
  unsigned short              nofifyOnFlags,
  DMProcessInfoPtr            whichPSN)                       THREEWORDINLINE(0x303C, 0x07EF, 0xABEB);


/*
 *  DMRemoveExtendedNotifyProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMRemoveExtendedNotifyProc(
  DMExtendedNotificationUPP   notifyProc,
  void *                      notifyUserData,
  DMProcessInfoPtr            whichPSN,
  unsigned short              removeFlags)                    THREEWORDINLINE(0x303C, 0x0726, 0xABEB);


/*
 *  DMNewAVPanelList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMNewAVPanelList(
  DisplayIDType      displayID,
  ResType            panelType,
  DMFidelityType     minimumFidelity,
  unsigned long      panelListFlags,
  unsigned long      reserved,
  DMListIndexType *  thePanelCount,
  DMListType *       thePanelList)                            THREEWORDINLINE(0x303C, 0x0C27, 0xABEB);


/*
 *  DMNewAVEngineList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMNewAVEngineList(
  DisplayIDType      displayID,
  ResType            engineType,
  DMFidelityType     minimumFidelity,
  unsigned long      engineListFlags,
  unsigned long      reserved,
  DMListIndexType *  engineCount,
  DMListType *       engineList)                              THREEWORDINLINE(0x303C, 0x0C28, 0xABEB);


/*
 *  DMNewAVDeviceList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMNewAVDeviceList(
  ResType            deviceType,
  unsigned long      deviceListFlags,
  unsigned long      reserved,
  DMListIndexType *  deviceCount,
  DMListType *       deviceList)                              THREEWORDINLINE(0x303C, 0x0A29, 0xABEB);


/*
 *  DMNewAVPortListByPortType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMNewAVPortListByPortType(
  ResType            subType,
  unsigned long      portListFlags,
  unsigned long      reserved,
  DMListIndexType *  devicePortCount,
  DMListType *       theDevicePortList)                       THREEWORDINLINE(0x303C, 0x0A2A, 0xABEB);


/*
 *  DMGetIndexedComponentFromList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetIndexedComponentFromList(
  DMListType                   panelList,
  DMListIndexType              itemIndex,
  unsigned long                reserved,
  DMComponentListIteratorUPP   listIterator,
  void *                       userData)                      THREEWORDINLINE(0x303C, 0x0A2B, 0xABEB);


/*
 *  DMDisposeList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMDisposeList(DMListType panelList)                           THREEWORDINLINE(0x303C, 0x022C, 0xABEB);


/*
 *  DMGetNameByAVID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetNameByAVID(
  AVIDType        theID,
  unsigned long   nameFlags,
  Str255          name)                                       THREEWORDINLINE(0x303C, 0x062D, 0xABEB);


/*
 *  DMNewAVIDByPortComponent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMNewAVIDByPortComponent(
  Component       thePortComponent,
  ResType         portKind,
  unsigned long   reserved,
  AVIDType *      newID)                                      THREEWORDINLINE(0x303C, 0x082E, 0xABEB);


/*
 *  DMGetPortComponentByAVID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetPortComponentByAVID(
  DisplayIDType           thePortID,
  Component *             thePortComponent,
  ComponentDescription *  theDesciption,
  ResType *               thePortKind)                        THREEWORDINLINE(0x303C, 0x082F, 0xABEB);


/*
 *  DMSendDependentNotification()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMSendDependentNotification(
  ResType             notifyType,
  ResType             notifyClass,
  AVIDType            displayID,
  ComponentInstance   notifyComponent)                        THREEWORDINLINE(0x303C, 0x0830, 0xABEB);


/*
 *  DMDisposeAVComponent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMDisposeAVComponent(Component theAVComponent)                THREEWORDINLINE(0x303C, 0x0231, 0xABEB);


/*
 *  DMSaveScreenPrefs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMSaveScreenPrefs(
  unsigned long   reserved1,
  unsigned long   saveFlags,
  unsigned long   reserved2)                                  THREEWORDINLINE(0x303C, 0x0632, 0xABEB);


/*
 *  DMNewAVIDByDeviceComponent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMNewAVIDByDeviceComponent(
  Component        theDeviceComponent,
  ResType          portKind,
  unsigned long    reserved,
  DisplayIDType *  newID)                                     THREEWORDINLINE(0x303C, 0x0833, 0xABEB);


/*
 *  DMNewAVPortListByDeviceAVID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMNewAVPortListByDeviceAVID(
  AVIDType           theID,
  DMFidelityType     minimumFidelity,
  unsigned long      portListFlags,
  unsigned long      reserved,
  DMListIndexType *  devicePortCount,
  DMListType *       theDevicePortList)                       THREEWORDINLINE(0x303C, 0x0C34, 0xABEB);


/*
 *  DMGetDeviceComponentByAVID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetDeviceComponentByAVID(
  AVIDType                theDeviceID,
  Component *             theDeviceComponent,
  ComponentDescription *  theDesciption,
  ResType *               theDeviceKind)                      THREEWORDINLINE(0x303C, 0x0835, 0xABEB);


/*
 *  DMNewDisplayModeList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMNewDisplayModeList(
  DisplayIDType      displayID,
  unsigned long      modeListFlags,
  unsigned long      reserved,
  DMListIndexType *  thePanelCount,
  DMListType *       thePanelList)                            THREEWORDINLINE(0x303C, 0x0A36, 0xABEB);


/*
 *  DMGetIndexedDisplayModeFromList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetIndexedDisplayModeFromList(
  DMListType                     panelList,
  DMListIndexType                itemIndex,
  unsigned long                  reserved,
  DMDisplayModeListIteratorUPP   listIterator,
  void *                         userData)                    THREEWORDINLINE(0x303C, 0x0A37, 0xABEB);


/*
 *  DMGetGraphicInfoByAVID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetGraphicInfoByAVID(
  AVIDType         theID,
  PicHandle *      theAVPcit,
  Handle *         theAVIconSuite,
  AVLocationRec *  theAVLocation)                             THREEWORDINLINE(0x303C, 0x0838, 0xABEB);


/*
 *  DMGetAVPowerState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetAVPowerState(
  AVIDType          theID,
  AVPowerStatePtr   getPowerState,
  unsigned long     reserved1)                                THREEWORDINLINE(0x303C, 0x0839, 0xABEB);


/*
 *  DMSetAVPowerState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMSetAVPowerState(
  AVIDType          theID,
  AVPowerStatePtr   setPowerState,
  unsigned long     powerFlags,
  Handle            displayState)                             THREEWORDINLINE(0x303C, 0x083A, 0xABEB);


/*
 *  DMGetDeviceAVIDByPortAVID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetDeviceAVIDByPortAVID(
  AVIDType    portAVID,
  AVIDType *  deviceAVID)                                     THREEWORDINLINE(0x303C, 0x043B, 0xABEB);


/*
 *  DMGetEnableByAVID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetEnableByAVID(
  AVIDType   theAVID,
  Boolean *  isAVIDEnabledNow,
  Boolean *  canChangeEnableNow)                              THREEWORDINLINE(0x303C, 0x063C, 0xABEB);


/*
 *  DMSetEnableByAVID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMSetEnableByAVID(
  AVIDType   theAVID,
  Boolean    doEnable,
  Handle     displayState)                                    THREEWORDINLINE(0x303C, 0x053D, 0xABEB);


/*
 *  DMGetDisplayMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib68k 2.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMGetDisplayMode(
  GDHandle          theDevice,
  VDSwitchInfoPtr   switchInfo)                               THREEWORDINLINE(0x303C, 0x043E, 0xABEB);


/*
 *  DMConfirmConfiguration()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DisplayLib 2.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DMConfirmConfiguration(
  DMModalFilterUPP   filterProc,
  UInt32             confirmFlags,
  UInt32             reserved,
  Handle             displayState)                            THREEWORDINLINE(0x303C, 0x083F, 0xABEB);



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

#endif /* __DISPLAYS__ */

