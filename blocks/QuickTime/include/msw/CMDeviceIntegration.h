/*
     File:       CMDeviceIntegration.h
 
     Contains:   Color Management Device Interfaces
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __CMDEVICEINTEGRATION__
#define __CMDEVICEINTEGRATION__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __CMAPPLICATION__
#include <CMApplication.h>
#endif

#ifndef __CMICCPROFILE__
#include <CMICCProfile.h>
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

/*
    The current versions of the data structure
    containing information on registered devices.
*/
enum {
  cmDeviceInfoVersion1          = 0x00010000,
  cmDeviceProfileInfoVersion1   = 0x00010000,
  cmDeviceProfileInfoVersion2   = 0x00020000
};

enum {
  cmCurrentDeviceInfoVersion    = cmDeviceInfoVersion1,
  cmCurrentProfileInfoVersion   = cmDeviceProfileInfoVersion1
};

/*
    Certain APIs require a device ID or profile ID.  
    In some cases, a "default ID" can be used.
*/
enum {
  cmDefaultDeviceID             = 0,
  cmDefaultProfileID            = 0
};

/*
    Possible values for device states accessible by the
    CMGetDeviceState() and CMSetDeviceState() APIs.
*/
enum {
  cmDeviceStateDefault          = 0x00000000,
  cmDeviceStateOffline          = 0x00000001,
  cmDeviceStateBusy             = 0x00000002,
  cmDeviceStateForceNotify      = (long)0x80000000,
  cmDeviceStateDeviceRsvdBits   = 0x00FF0000,
  cmDeviceStateAppleRsvdBits    = (long)0xFF00FFFF
};

/*
    Possible values for flags passed to the
    CMIterateDeviceProfiles() API.
    
    "Factory" profiles are registered via the
    CMSetDeviceFactoryProfiles() API.
    
    "Custom" profiles are those which are meant to take
    the place of the factory profiles, as a result of
    customization or calibration.  These profiles are
    registered via the CMSetDeviceProfiles() API.
    
    To retrieve all of the the former for all devices,
    use cmIterateFactoryDeviceProfiles as the flags
    value when calling CMIterateDeviceProfiles().
    
    To retrieve only the latter for all devices, use
    the cmIterateCustomDeviceProfiles, as the flags
    value when calling CMIterateDeviceProfiles().
    
    To get the profiles in use for all devices, use
    cmIterateCurrentDeviceProfiles as the flags value.
    This will replace the factory profiles with any
    overrides, yielding the currently used set.
    
    To get all profiles, without replacement, use
    cmIterateAllDeviceProfiles. 
*/
enum {
  cmIterateFactoryDeviceProfiles = 0x00000001,
  cmIterateCustomDeviceProfiles = 0x00000002,
  cmIterateCurrentDeviceProfiles = 0x00000003,
  cmIterateAllDeviceProfiles    = 0x00000004,
  cmIterateDeviceProfilesMask   = 0x0000000F
};

/*
    Errors returned by CMDeviceIntegration APIs
*/
enum {
  cmDeviceDBNotFoundErr         = -4227, /* Prefs not found/loaded */
  cmDeviceAlreadyRegistered     = -4228, /* Re-registration of device */
  cmDeviceNotRegistered         = -4229, /* Device not found */
  cmDeviceProfilesNotFound      = -4230, /* Profiles not found */
  cmInternalCFErr               = -4231 /* CoreFoundation failure */
};


/*
    Clients can register for notifications of device evolutions:
   
   DeviceRegistered                A new device was registered
    DeviceUnregistered              A device was unregistered
  DeviceOnline                    Change to Online as a result of CMSetDeviceState
   DeviceOffline                   Change to Offline as a result of CMSetDeviceState
  DeviceState                     Any change to a device state
   DefaultDevice                   A default device for any device class changed
  DeviceProfiles                  Any change to any device's profiles
    DefaultDeviceProfile            The default profile for any device changed
*/
#define   kCMDeviceRegisteredNotification     CFSTR("CMDeviceRegisteredNotification")
#define kCMDeviceUnregisteredNotification   CFSTR("CMDeviceUnregisteredNotification")
#define   kCMDeviceOnlineNotification         CFSTR("CMDeviceOnlineNotification")
#define kCMDeviceOfflineNotification        CFSTR("CMDeviceOfflineNotification")
#define    kCMDeviceStateNotification          CFSTR("CMDeviceStateNotification")
#define  kCMDefaultDeviceNotification        CFSTR("CMDefaultDeviceNotification")
#define    kCMDeviceProfilesNotification       CFSTR("CMDeviceProfilesNotification")
#define   kCMDefaultDeviceProfileNotification CFSTR("CMDefaultDeviceProfileNotification")

/*
    Device state data.
*/
typedef UInt32                          CMDeviceState;
/*
    A CMDeviceID must be unique within a device's class.
*/
typedef UInt32                          CMDeviceID;
/*
    A CMDeviceProfileID must only be unique per device.
*/
typedef UInt32                          CMDeviceProfileID;
/*
    DeviceClass type.
*/
enum {
  cmScannerDeviceClass          = FOUR_CHAR_CODE('scnr'),
  cmCameraDeviceClass           = FOUR_CHAR_CODE('cmra'),
  cmDisplayDeviceClass          = FOUR_CHAR_CODE('mntr'),
  cmPrinterDeviceClass          = FOUR_CHAR_CODE('prtr'),
  cmProofDeviceClass            = FOUR_CHAR_CODE('pruf')
};

typedef OSType                          CMDeviceClass;
/*
    CMDeviceScope
    Structure specifying a device's or a device setting's scope.
*/
struct CMDeviceScope {
  CFStringRef         deviceUser;             /* kCFPreferencesCurrentUser | _AnyUser */
  CFStringRef         deviceHost;             /* kCFPreferencesCurrentHost | _AnyHost */
};
typedef struct CMDeviceScope            CMDeviceScope;
typedef CMDeviceScope                   CMDeviceProfileScope;
/*
    CMDeviceInfo
    Structure containing information on a given device.
*/
struct CMDeviceInfo {
  UInt32              dataVersion;            /* cmDeviceInfoVersion1 */
  CMDeviceClass       deviceClass;            /* device class */
  CMDeviceID          deviceID;               /* device ID */
  CMDeviceScope       deviceScope;            /* device's scope */
  CMDeviceState       deviceState;            /* Device State flags */
  CMDeviceProfileID   defaultProfileID;       /* Can change */
  CFDictionaryRef *   deviceName;             /* Ptr to storage for CFDictionary of */
                                              /* localized device names (could be nil) */
  UInt32              profileCount;           /* Count of registered profiles */
  UInt32              reserved;               /* Reserved for use by ColorSync */
};
typedef struct CMDeviceInfo             CMDeviceInfo;
typedef CMDeviceInfo *                  CMDeviceInfoPtr;
/*
    CMDeviceProfileInfo
    Structure containing information on a device profile.
*/
struct CMDeviceProfileInfo {
  UInt32              dataVersion;            /* cmDeviceProfileInfoVersion1 */
  CMDeviceProfileID   profileID;              /* The identifier for this profile */
  CMProfileLocation   profileLoc;             /* The profile's location */
  CFDictionaryRef     profileName;            /* CFDictionary of localized profile names */
  UInt32              reserved;               /* Reserved for use by ColorSync */
};
typedef struct CMDeviceProfileInfo      CMDeviceProfileInfo;
struct NCMDeviceProfileInfo {
  UInt32              dataVersion;            /* cmDeviceProfileInfoVersion2 */
  CMDeviceProfileID   profileID;              /* The identifier for this profile */
  CMProfileLocation   profileLoc;             /* The profile's location */
  CFDictionaryRef     profileName;            /* CFDictionary of localized profile names */
  CMDeviceProfileScope  profileScope;         /* The scope this profile applies to */
  UInt32              reserved;               /* Reserved for use by ColorSync */
};
typedef struct NCMDeviceProfileInfo     NCMDeviceProfileInfo;
/*
    CMDeviceProfileArray
    Structure containing the profiles for a device.
*/
struct CMDeviceProfileArray {
  UInt32              profileCount;           /* Count of profiles in array */
  CMDeviceProfileInfo  profiles[1];           /* The profile info records */
};
typedef struct CMDeviceProfileArray     CMDeviceProfileArray;
typedef CMDeviceProfileArray *          CMDeviceProfileArrayPtr;
/*
    Caller-supplied iterator functions
*/
typedef CALLBACK_API_C( OSErr , CMIterateDeviceInfoProcPtr )(const CMDeviceInfo *deviceInfo, void *refCon);
typedef CALLBACK_API_C( OSErr , CMIterateDeviceProfileProcPtr )(const CMDeviceInfo *deviceInfo, const NCMDeviceProfileInfo *profileInfo, void *refCon);
/*
    Device Registration
*/
/*
 *  CMRegisterColorDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMRegisterColorDevice(
  CMDeviceClass          deviceClass,
  CMDeviceID             deviceID,
  CFDictionaryRef        deviceName,
  const CMDeviceScope *  deviceScope);


/*
 *  CMUnregisterColorDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMUnregisterColorDevice(
  CMDeviceClass   deviceClass,
  CMDeviceID      deviceID);


/*
    Default Device accessors
*/
/*
 *  CMSetDefaultDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMSetDefaultDevice(
  CMDeviceClass   deviceClass,
  CMDeviceID      deviceID);


/*
 *  CMGetDefaultDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMGetDefaultDevice(
  CMDeviceClass   deviceClass,
  CMDeviceID *    deviceID);


/*
    Device Profile Registration & Access
*/
/*
 *  CMSetDeviceFactoryProfiles()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMSetDeviceFactoryProfiles(
  CMDeviceClass                 deviceClass,
  CMDeviceID                    deviceID,
  CMDeviceProfileID             defaultProfID,
  const CMDeviceProfileArray *  deviceProfiles);


/*
 *  CMGetDeviceFactoryProfiles()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMGetDeviceFactoryProfiles(
  CMDeviceClass           deviceClass,
  CMDeviceID              deviceID,
  CMDeviceProfileID *     defaultProfID,
  UInt32 *                arraySize,
  CMDeviceProfileArray *  deviceProfiles);


/*
 *  CMSetDeviceProfiles()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMSetDeviceProfiles(
  CMDeviceClass                 deviceClass,
  CMDeviceID                    deviceID,
  const CMDeviceProfileScope *  profileScope,
  const CMDeviceProfileArray *  deviceProfiles);


/*
 *  CMGetDeviceProfiles()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMGetDeviceProfiles(
  CMDeviceClass           deviceClass,
  CMDeviceID              deviceID,
  UInt32 *                arraySize,
  CMDeviceProfileArray *  deviceProfiles);


/*
 *  CMSetDeviceDefaultProfileID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMSetDeviceDefaultProfileID(
  CMDeviceClass       deviceClass,
  CMDeviceID          deviceID,
  CMDeviceProfileID   defaultProfID);


/*
 *  CMGetDeviceDefaultProfileID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMGetDeviceDefaultProfileID(
  CMDeviceClass        deviceClass,
  CMDeviceID           deviceID,
  CMDeviceProfileID *  defaultProfID);


/*
 *  CMGetDeviceProfile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMGetDeviceProfile(
  CMDeviceClass        deviceClass,
  CMDeviceID           deviceID,
  CMDeviceProfileID    profileID,
  CMProfileLocation *  deviceProfLoc);


/*
 *  CMSetDeviceProfile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMSetDeviceProfile(
  CMDeviceClass                 deviceClass,
  CMDeviceID                    deviceID,
  const CMDeviceProfileScope *  profileScope,
  CMDeviceProfileID             profileID,
  const CMProfileLocation *     deviceProfLoc);


/*
    Other Device State/Info accessors
*/
/*
 *  CMSetDeviceState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMSetDeviceState(
  CMDeviceClass   deviceClass,
  CMDeviceID      deviceID,
  CMDeviceState   deviceState);


/*
 *  CMGetDeviceState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMGetDeviceState(
  CMDeviceClass    deviceClass,
  CMDeviceID       deviceID,
  CMDeviceState *  deviceState);


/*
 *  CMGetDeviceInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMGetDeviceInfo(
  CMDeviceClass   deviceClass,
  CMDeviceID      deviceID,
  CMDeviceInfo *  deviceInfo);


/*
    Device Info & Profile Iterators
*/
/*
 *  CMIterateColorDevices()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMIterateColorDevices(
  CMIterateDeviceInfoProcPtr   proc,
  UInt32 *                     seed,
  UInt32 *                     count,
  void *                       refCon);


/*
 *  CMIterateDeviceProfiles()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CMError )
CMIterateDeviceProfiles(
  CMIterateDeviceProfileProcPtr   proc,
  UInt32 *                        seed,
  UInt32 *                        count,
  UInt32                          flags,
  void *                          refCon);



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

#endif /* __CMDEVICEINTEGRATION__ */

