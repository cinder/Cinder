/*
     File:       KeychainHI.h
 
     Contains:   Keychain API's with Human Interfaces
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __KEYCHAINCORE__
#include <KeychainCore.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __CFARRAY__
#include <CFArray.h>
#endif

#ifndef __CFDATE__
#include <CFDate.h>
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

/* High-level interface for storing passwords */
/*
 *  KCAddAppleSharePassword()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
KCAddAppleSharePassword(
  AFPServerSignature *  serverSignature,
  StringPtr             serverAddress,
  StringPtr             serverName,
  StringPtr             volumeName,
  StringPtr             accountName,
  UInt32                passwordLength,
  const void *          passwordData,
  KCItemRef *           item);                 /* can be NULL */


/*
 *  KCAddInternetPassword()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
KCAddInternetPassword(
  StringPtr     serverName,
  StringPtr     securityDomain,
  StringPtr     accountName,
  UInt16        port,
  OSType        protocol,
  OSType        authType,
  UInt32        passwordLength,
  const void *  passwordData,
  KCItemRef *   item);                /* can be NULL */


/*
 *  KCAddInternetPasswordWithPath()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
KCAddInternetPasswordWithPath(
  StringPtr     serverName,
  StringPtr     securityDomain,
  StringPtr     accountName,
  StringPtr     path,
  UInt16        port,
  OSType        protocol,
  OSType        authType,
  UInt32        passwordLength,
  const void *  passwordData,
  KCItemRef *   item);                /* can be NULL */


/*
 *  KCAddGenericPassword()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
KCAddGenericPassword(
  StringPtr     serviceName,
  StringPtr     accountName,
  UInt32        passwordLength,
  const void *  passwordData,
  KCItemRef *   item);                /* can be NULL */


/* Low-level interface for storing keychain items */
/*
 *  KCAddItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
KCAddItem(KCItemRef item);


/* Managing keychains */
/*
 *  KCUnlock()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
KCUnlock(
  KCRef       keychain,       /* can be NULL */
  StringPtr   password);      /* can be NULL */


/*
 *  KCCreateKeychain()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
KCCreateKeychain(
  StringPtr   password,       /* can be NULL */
  KCRef *     keychain);      /* can be NULL */


/*
 *  KCChangeSettings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
KCChangeSettings(KCRef keychain);


/*
 *  kcunlock()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
kcunlock(
  KCRef         keychain,       /* can be NULL */
  const char *  password);      /* can be NULL */


/*
 *  kccreatekeychain()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
kccreatekeychain(
  const char *  password,       /* can be NULL */
  KCRef *       keychain);      /* can be NULL */


/*
 *  kcaddapplesharepassword()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
kcaddapplesharepassword(
  AFPServerSignature *  serverSignature,
  const char *          serverAddress,
  const char *          serverName,
  const char *          volumeName,
  const char *          accountName,
  UInt32                passwordLength,
  const void *          passwordData,
  KCItemRef *           item);                 /* can be NULL */


/*
 *  kcaddinternetpassword()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
kcaddinternetpassword(
  const char *  serverName,
  const char *  securityDomain,
  const char *  accountName,
  UInt16        port,
  OSType        protocol,
  OSType        authType,
  UInt32        passwordLength,
  const void *  passwordData,
  KCItemRef *   item);                /* can be NULL */


/*
 *  kcaddinternetpasswordwithpath()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
kcaddinternetpasswordwithpath(
  const char *  serverName,
  const char *  securityDomain,
  const char *  accountName,
  const char *  path,
  UInt16        port,
  OSType        protocol,
  OSType        authType,
  UInt32        passwordLength,
  const void *  passwordData,
  KCItemRef *   item);                /* can be NULL */


/*
 *  kcaddgenericpassword()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
kcaddgenericpassword(
  const char *  serviceName,
  const char *  accountName,
  UInt32        passwordLength,
  const void *  passwordData,
  KCItemRef *   item);                /* can be NULL */


/* Working with certificates */
#if CALL_NOT_IN_CARBON
/*
 *  KCFindX509Certificates()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 2.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSStatus )
KCFindX509Certificates(
  KCRef                 keychain,
  CFStringRef           name,
  CFStringRef           emailAddress,
  KCCertSearchOptions   options,
  CFMutableArrayRef *   certificateItems);      /* can be NULL */


/*
 *  KCChooseCertificate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in KeychainLib 2.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSStatus )
KCChooseCertificate(
  CFArrayRef       items,
  KCItemRef *      certificate,
  CFArrayRef       policyOIDs,
  KCVerifyStopOn   stopOn);



#endif  /* CALL_NOT_IN_CARBON */


#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif
