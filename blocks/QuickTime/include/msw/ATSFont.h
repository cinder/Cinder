/*
     File:       ATSFont.h
 
     Contains:   Public interface to the font access and data management functions of ATS.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (C) 2000-2002 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __ATSFONT__
#define __ATSFONT__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __CFMESSAGEPORT__
#include <CFMessagePort.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __TEXTCOMMON__
#include <TextCommon.h>
#endif

#ifndef __ATSTYPES__
#include <ATSTypes.h>
#endif

#ifndef __SFNTTYPES__
#include <SFNTTypes.h>
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

#if PRAGMA_ENUM_ALWAYSINT
    #if defined(__fourbyteints__) && !__fourbyteints__ 
        #define __ATSFONT__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __ATSFONT__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

enum {
  kATSOptionFlagsDefault        = kNilOptions,
  kATSOptionFlagsComposeFontPostScriptName = 1 << 0, /* ATSFontGetPostScriptName */
  kATSOptionFlagsUseDataForkAsResourceFork = 1 << 8, /* ATSFontActivateFromFileSpecification */
  kATSOptionFlagsUseResourceFork = 2 << 8,
  kATSOptionFlagsUseDataFork    = 3 << 8
};

enum {
  kATSIterationCompleted        = -980L,
  kATSInvalidFontFamilyAccess   = -981L,
  kATSInvalidFontAccess         = -982L,
  kATSIterationScopeModified    = -983L,
  kATSInvalidFontTableAccess    = -984L,
  kATSInvalidFontContainerAccess = -985L
};

/* Activation Option Flags */
typedef UInt32                          ATSFontContext;
enum {
  kATSFontContextUnspecified    = 0,
  kATSFontContextGlobal         = 1,
  kATSFontContextLocal          = 2
};

enum {
  kATSOptionFlagsDoNotNotify    = 0x00000001 << 8, /* Do not notify after global activation/deactivation */
  kATSOptionFlagsIterationScopeMask = 0x00000007 << 12, /* Mask option bits 12-14 for iteration scopes */
  kATSOptionFlagsDefaultScope   = 0x00000000 << 12,
  kATSOptionFlagsUnRestrictedScope = 0x00000001 << 12,
  kATSOptionFlagsRestrictedScope = 0x00000002 << 12,
  kATSOptionFlagsProcessSubdirectories = 0x00000001 << 6
};

/* Iteration Option Flags */
enum {
  kATSOptionFlagsIterateByPrecedenceMask = 0x00000001 << 5
};

typedef UInt32                          ATSFontFormat;
enum {
  kATSFontFormatUnspecified     = 0
};

typedef CALLBACK_API_C( OSStatus , ATSFontFamilyApplierFunction )(ATSFontFamilyRef iFamily, void *iRefCon);
typedef CALLBACK_API_C( OSStatus , ATSFontApplierFunction )(ATSFontRef iFont, void *iRefCon);
typedef struct ATSFontFamilyIterator_*  ATSFontFamilyIterator;
typedef struct ATSFontIterator_*        ATSFontIterator;
enum {
  kATSFontFilterCurrentVersion  = 0
};

enum ATSFontFilterSelector {
  kATSFontFilterSelectorUnspecified = 0,
  kATSFontFilterSelectorGeneration = 3,
  kATSFontFilterSelectorFontFamily = 7,
  kATSFontFilterSelectorFontFamilyApplierFunction = 8,
  kATSFontFilterSelectorFontApplierFunction = 9
};
typedef enum ATSFontFilterSelector ATSFontFilterSelector;

struct ATSFontFilter {
  UInt32              version;
  ATSFontFilterSelector  filterSelector;
  union {
    ATSGeneration       generationFilter;
    ATSFontFamilyRef    fontFamilyFilter;
    ATSFontFamilyApplierFunction  fontFamilyApplierFunctionFilter;
    ATSFontApplierFunction  fontApplierFunctionFilter;
  }                       filter;
};
typedef struct ATSFontFilter            ATSFontFilter;
/* Notification related */
typedef struct ATSFontNotificationRef_*  ATSFontNotificationRef;
typedef struct ATSFontNotificationInfoRef_*  ATSFontNotificationInfoRef;

/*
 *  ATSFontNotifyOption
 *  
 *  Discussion:
 *    Options used with ATSFontNotificationSubscribe.  Any of the
 *    options that follow may be used together in order to alter the
 *    default behavior of ATS notifications.
 */
enum ATSFontNotifyOption {

  /*
   * Default behavior of ATSFontNotificationSubscribe.
   */
  kATSFontNotifyOptionDefault   = 0,

  /*
   * Normally applications will only receive ATS notifications while in
   * the foreground.   If suspended, the notification will be delivered
   * when then application comes to the foreground.  This is the
   * default.  You should set this option if you are a server or tool
   * that performs font management functions and require immediate
   * notification when something changes.
   */
  kATSFontNotifyOptionReceiveWhileSuspended = 1L << 0
};
typedef enum ATSFontNotifyOption ATSFontNotifyOption;




/*
 *  ATSFontNotifyAction
 *  
 *  Discussion:
 *    Used with ATSFontNotify.   The following is a list of actions you
 *    might wish the ATS server to perform and notify clients if
 *    appropriate.
 */
enum ATSFontNotifyAction {

  /*
   * Used after a batch (de)activation of fonts occurs.   Typically the
   * caller has exercised multiple global (De)Activation calls with the
   * kATSOptionFlagsDoNotNotify set. Once all calls are completed, one
   * may use ATSFontNotify with this action to ask ATS to notify all
   * clients.
   */
  kATSFontNotifyActionFontsChanged = 1,

  /*
   * The ATS system with the help of the Finder keeps track of changes
   * to any of the fonts directories ( System, Local, Network, User, &
   * Classic). However, one may wish to add/remove fonts to these
   * locations programmatically.   This action is used to let ATS
   * server to rescan these directories and post notifications if
   * necessary.
   */
  kATSFontNotifyActionDirectoriesChanged = 2
};
typedef enum ATSFontNotifyAction ATSFontNotifyAction;



/*
 *  ATSNotificationCallback
 *  
 *  Discussion:
 *    Callback delivered for ATS notifications.
 *  
 *  Parameters:
 *    
 *    info:
 *      Parameter is placed here for future improvements.  Initially
 *      the contents of this parameter will be NULL.
 *    
 *    refCon:
 *      User data/state to be supplied to callback function
 */
typedef CALLBACK_API_C( void , ATSNotificationCallback )(ATSFontNotificationInfoRef info, void *refCon);
/* ----------------------------------------------------------------------------------------- */
/* Font container                                                                            */
/* ----------------------------------------------------------------------------------------- */
/*
 *  ATSGetGeneration()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSGeneration )
ATSGetGeneration(void);


/*
 *  ATSFontActivateFromFileSpecification()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontActivateFromFileSpecification(
  const FSSpec *         iFile,
  ATSFontContext         iContext,
  ATSFontFormat          iFormat,
  void *                 iReserved,
  ATSOptionFlags         iOptions,
  ATSFontContainerRef *  oContainer);


/*
 *  ATSFontActivateFromMemory()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontActivateFromMemory(
  LogicalAddress         iData,
  ByteCount              iLength,
  ATSFontContext         iContext,
  ATSFontFormat          iFormat,
  void *                 iReserved,
  ATSOptionFlags         iOptions,
  ATSFontContainerRef *  oContainer);


/*
 *  ATSFontDeactivate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontDeactivate(
  ATSFontContainerRef   iContainer,
  void *                iRefCon,
  ATSOptionFlags        iOptions);


/* ----------------------------------------------------------------------------------------- */
/* Font family                                                                               */
/* ----------------------------------------------------------------------------------------- */
/*
 *  ATSFontFamilyApplyFunction()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontFamilyApplyFunction(
  ATSFontFamilyApplierFunction   iFunction,
  void *                         iRefCon);


/*
 *  ATSFontFamilyIteratorCreate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontFamilyIteratorCreate(
  ATSFontContext           iContext,
  const ATSFontFilter *    iFilter,          /* can be NULL */
  void *                   iRefCon,
  ATSOptionFlags           iOptions,
  ATSFontFamilyIterator *  ioIterator);


/*
 *  ATSFontFamilyIteratorRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontFamilyIteratorRelease(ATSFontFamilyIterator * ioIterator);


/*
 *  ATSFontFamilyIteratorReset()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontFamilyIteratorReset(
  ATSFontContext           iContext,
  const ATSFontFilter *    iFilter,          /* can be NULL */
  void *                   iRefCon,
  ATSOptionFlags           iOptions,
  ATSFontFamilyIterator *  ioIterator);


/*
 *  ATSFontFamilyIteratorNext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontFamilyIteratorNext(
  ATSFontFamilyIterator   iIterator,
  ATSFontFamilyRef *      oFamily);


/*
 *  ATSFontFamilyFindFromName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSFontFamilyRef )
ATSFontFamilyFindFromName(
  CFStringRef      iName,
  ATSOptionFlags   iOptions);


/*
 *  ATSFontFamilyGetGeneration()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSGeneration )
ATSFontFamilyGetGeneration(ATSFontFamilyRef iFamily);


/*
 *  ATSFontFamilyGetName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontFamilyGetName(
  ATSFontFamilyRef   iFamily,
  ATSOptionFlags     iOptions,
  CFStringRef *      oName);


/*
 *  ATSFontFamilyGetEncoding()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( TextEncoding )
ATSFontFamilyGetEncoding(ATSFontFamilyRef iFamily);


/* ----------------------------------------------------------------------------------------- */
/* Font                                                                                      */
/* ----------------------------------------------------------------------------------------- */
/*
 *  ATSFontApplyFunction()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontApplyFunction(
  ATSFontApplierFunction   iFunction,
  void *                   iRefCon);


/*
 *  ATSFontIteratorCreate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontIteratorCreate(
  ATSFontContext         iContext,
  const ATSFontFilter *  iFilter,          /* can be NULL */
  void *                 iRefCon,
  ATSOptionFlags         iOptions,
  ATSFontIterator *      ioIterator);


/*
 *  ATSFontIteratorRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontIteratorRelease(ATSFontIterator * ioIterator);


/*
 *  ATSFontIteratorReset()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontIteratorReset(
  ATSFontContext         iContext,
  const ATSFontFilter *  iFilter,          /* can be NULL */
  void *                 iRefCon,
  ATSOptionFlags         iOptions,
  ATSFontIterator *      ioIterator);


/*
 *  ATSFontIteratorNext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontIteratorNext(
  ATSFontIterator   iIterator,
  ATSFontRef *      oFont);


/*
 *  ATSFontFindFromName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSFontRef )
ATSFontFindFromName(
  CFStringRef      iName,
  ATSOptionFlags   iOptions);


/*
 *  ATSFontFindFromPostScriptName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSFontRef )
ATSFontFindFromPostScriptName(
  CFStringRef      iName,
  ATSOptionFlags   iOptions);


/*
 *  ATSFontFindFromContainer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontFindFromContainer(
  ATSFontContainerRef   iContainer,
  ATSOptionFlags        iOptions,
  ItemCount             iCount,
  ATSFontRef            ioArray[],
  ItemCount *           oCount);


/*
 *  ATSFontGetGeneration()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSGeneration )
ATSFontGetGeneration(ATSFontRef iFont);


/*
 *  ATSFontGetName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontGetName(
  ATSFontRef       iFont,
  ATSOptionFlags   iOptions,
  CFStringRef *    oName);


/*
 *  ATSFontGetPostScriptName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontGetPostScriptName(
  ATSFontRef       iFont,
  ATSOptionFlags   iOptions,
  CFStringRef *    oName);


/*
 *  ATSFontGetTableDirectory()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontGetTableDirectory(
  ATSFontRef   iFont,
  ByteCount    iBufferSize,
  void *       ioBuffer,
  ByteCount *  oSize);            /* can be NULL */


/*
 *  ATSFontGetTable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontGetTable(
  ATSFontRef     iFont,
  FourCharCode   iTag,
  ByteOffset     iOffset,
  ByteCount      iBufferSize,
  void *         ioBuffer,
  ByteCount *    oSize);            /* can be NULL */


/*
 *  ATSFontGetHorizontalMetrics()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontGetHorizontalMetrics(
  ATSFontRef        iFont,
  ATSOptionFlags    iOptions,
  ATSFontMetrics *  oMetrics);


/*
 *  ATSFontGetVerticalMetrics()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontGetVerticalMetrics(
  ATSFontRef        iFont,
  ATSOptionFlags    iOptions,
  ATSFontMetrics *  oMetrics);


/* ----------------------------------------------------------------------------------------- */
/* Compatibility                                                                             */
/* ----------------------------------------------------------------------------------------- */
/*
 *  ATSFontFamilyFindFromQuickDrawName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ATSFontFamilyRef )
ATSFontFamilyFindFromQuickDrawName(ConstStr255Param iName);


/*
 *  ATSFontFamilyGetQuickDrawName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontFamilyGetQuickDrawName(
  ATSFontFamilyRef   iFamily,
  Str255             oName);


/*
 *  ATSFontGetFileSpecification()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontGetFileSpecification(
  ATSFontRef   iFont,
  FSSpec *     oFile);


/*
 *  ATSFontGetFontFamilyResource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ATSFontGetFontFamilyResource(
  ATSFontRef   iFont,
  ByteCount    iBufferSize,
  void *       ioBuffer,
  ByteCount *  oSize);            /* can be NULL */




/* ----------------------------------------------------------------------------------------- */
/* Notification                                                                              */
/* ----------------------------------------------------------------------------------------- */
/*
 *  ATSFontNotify()
 *  
 *  Summary:
 *    Used to alert ATS that an action which may require notification
 *    to clients has occurred.
 *  
 *  Parameters:
 *    
 *    action:
 *      Action that should be taken by the ATS Server
 *    
 *    info:
 *      Any required or optional information that may be required by
 *      the action taken.
 *  
 *  Result:
 *    noErr Action successfully reported paramErr Invalid action passed
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ATSFontNotify(
  ATSFontNotifyAction   action,
  void *                info);        /* can be NULL */



/*
 *  ATSFontNotificationSubscribe()
 *  
 *  Summary:
 *    Ask the ATS System to notify caller when certain events have
 *    occurred.  Note that your application must have a CFRunLoop in
 *    order to receive notifications. Any Appkit or Carbon event loop
 *    based application will have one by default.
 *  
 *  Parameters:
 *    
 *    callback:
 *      Function that will be called by the ATS system whenever an
 *      event of interest takes place.
 *    
 *    options:
 *      Set the wanted ATSFontNotificationOptions to modify the default
 *      behavior of ATS Notifications.
 *    
 *    iRefcon:
 *      User data/state which will be passed to the callback funtion
 *    
 *    oNotificationRef:
 *      You may use this reference to un-subscribe to this notification.
 *  
 *  Result:
 *    noErr Subscribed successfully paramErr NULL callback was passed.
 *    memFullErr Could not allocate enough memory for internal data
 *    structures.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ATSFontNotificationSubscribe(
  ATSNotificationCallback   callback,
  ATSFontNotifyOption       options,
  void *                    iRefcon,                /* can be NULL */
  ATSFontNotificationRef *  oNotificationRef);      /* can be NULL */



/*
 *  ATSFontNotificationUnsubscribe()
 *  
 *  Summary:
 *    Release subscription and stop receiving notifications for a given
 *    reference.
 *  
 *  Parameters:
 *    
 *    notificationRef:
 *      Notification reference for which you want to stop receiving
 *      notifications. Note, if more than one notification has been
 *      requested of ATS, you will still receive notifications on those
 *      requests.
 *  
 *  Result:
 *    noErr Unsubscribed successfully paramErr NULL/invalid
 *    notificationRef passed
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ATSFontNotificationUnsubscribe(ATSFontNotificationRef notificationRef);



/* ----------------------------------------------------------------------------------------- */
/* Font query message hooks                                                                  */
/* ----------------------------------------------------------------------------------------- */

/*
 *  ATSFontQuerySourceContext
 *  
 *  Summary:
 *    A parameter block for client information to be retained by ATS
 *    and passed back to an ATSFontQueryCallback function.
 */
struct ATSFontQuerySourceContext {

  /*
   * A 32-bit unsigned integer that indicates the version of this
   * structure. This should be set to 0.
   */
  UInt32              version;

  /*
   * A pointer-sized client datum that should be passed back to an
   * ATSFontQueryCallback function.
   */
  void *              refCon;

  /*
   * The callback used to add a retain to the refCon.
   */
  CFAllocatorRetainCallBack  retain;

  /*
   * The callback used to remove a retain to the refCon.
   */
  CFAllocatorReleaseCallBack  release;
};
typedef struct ATSFontQuerySourceContext ATSFontQuerySourceContext;

/*
 *  ATSFontQueryMessageID
 *  
 *  Discussion:
 *    Constants for ATS font query message types.
 */
enum ATSFontQueryMessageID {

  /*
   * The message ID for a font request query. The data for a message
   * with this ID is a flattened CFDictionaryRef with keys and values
   * as decribed below. A query dictionary may have any or all of these
   * entries.
   */
  kATSQueryActivateFontMessage  = FOUR_CHAR_CODE('atsa')
};
typedef enum ATSFontQueryMessageID ATSFontQueryMessageID;


/*
 *  ATSFontQueryCallback
 *  
 *  Summary:
 *    Callback for receiving font-related queries from ATS.
 *  
 *  Parameters:
 *    
 *    msgid:
 *      An ATSFontQueryMessageID that identifies the message type.
 *    
 *    data:
 *      A CFPropertyListRef that represents the query. The content is
 *      message type-specific.
 *    
 *    refCon:
 *      A pointer-sized client datum that was optionally provided to
 *      ATSCreateFontQueryRunLoopSource.
 *  
 *  Result:
 *    A CFPropertyListRef that represents the message type-specific
 *    response to the query. May be NULL.
 */
typedef CALLBACK_API_C( CFPropertyListRef , ATSFontQueryCallback )(ATSFontQueryMessageID msgid, CFPropertyListRef data, void *refCon);
/*
 *  ATSCreateFontQueryRunLoopSource()
 *  
 *  Summary:
 *    Creates a CFRunLoopSourceRef that will be used to convey font
 *    queries from ATS.
 *  
 *  Parameters:
 *    
 *    queryOrder:
 *      A CFIndex that specifies the priority of this query receiver
 *      relative to others. When ATS makes a font query, it will send
 *      the query to each receiver in priority order, from highest to
 *      lowest. "Normal" priority is 0.
 *    
 *    sourceOrder:
 *      The order of the created run loop source.
 *    
 *    callout:
 *      A function pointer of type ATSFontQueryCallback that will be
 *      called to process a font query.
 *    
 *    context:
 *      An ATSFontQuerySourceContext parameter block that provides a
 *      pointer-sized client datum which will be retained by ATS and
 *      passed to the callout function. May be NULL.
 *  
 *  Result:
 *    A CFRunLoopSourceRef. To stop receiving queries, invalidate this
 *    run loop source.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( CFRunLoopSourceRef )
ATSCreateFontQueryRunLoopSource(
  CFIndex                            queryOrder,
  CFIndex                            sourceOrder,
  ATSFontQueryCallback               callout,
  const ATSFontQuerySourceContext *  context);          /* can be NULL */


/* ----------------------------------------------------------------------------------------- */
/* Font request query message content                                                        */
/* ----------------------------------------------------------------------------------------- */
/* Keys in a font request query dictionary. */
/* These keys appear in the dictionary for a kATSQueryActivateFontMessage query. */

/*
 *  kATSQueryClientPID
 *  
 *  Discussion:
 *    The process ID of the application making the query. The
 *    corresponding value is a CFNumberRef that contains a pid_t.
 */
#define kATSQueryClientPID              CFSTR("ATS client pid")

/*
 *  kATSQueryQDFamilyName
 *  
 *  Discussion:
 *    The Quickdraw-style family name of the font being requested, e.g.
 *    the name passed to GetFNum. The corresponding value is a
 *    CFStringRef.
 */
#define kATSQueryQDFamilyName           CFSTR("font family name")

/*
 *  kATSQueryFontName
 *  
 *  Discussion:
 *    The name of the font being requested. The corresponding value is
 *    a CFStringRef suitable as an argument to ATSFontFindFromName().
 *    This should match a candidate font's unique or full name.
 */
#define kATSQueryFontName               CFSTR("font name")

/*
 *  kATSQueryFontPostScriptName
 *  
 *  Discussion:
 *    The PostScript name of the font being requested. The
 *    corresponding value is a CFStringRef suitable as an argument to
 *    ATSFontFindFromPostScriptName(). This should match either the
 *    PostScript name derived from the font's FOND resource or its sfnt
 *    name table, with preference given to the FOND PostScript name.
 */
#define kATSQueryFontPostScriptName     CFSTR("font PS name")

/*
 *  kATSQueryFontNameTableEntries
 *  
 *  Discussion:
 *    A descriptor for sfnt name table entries that the requested font
 *    must have. The corresponding value is a CFArrayRef of
 *    CFDictionaryRefs that describe name table entries. A font must
 *    have all of the specified entries to be considered a match.
 */
#define kATSQueryFontNameTableEntries   CFSTR("font name table entries")
/* Keys in a font raw name descriptor dictionary. */

/*
 *  kATSFontNameTableCode
 *  
 *  Discussion:
 *    The font name's name code. The corresponding value is a
 *    CFNumberRef. If missing, assume kFontNoNameCode.
 */
#define kATSFontNameTableCode           CFSTR("font name code")

/*
 *  kATSFontNameTablePlatform
 *  
 *  Discussion:
 *    The font name's platform code. The corresponding value is a
 *    CFNumberRef. If missing, assume kFontNoPlatformCode.
 */
#define kATSFontNameTablePlatform       CFSTR("font platform code")

/*
 *  kATSFontNameTableScript
 *  
 *  Discussion:
 *    The font name's script code. The corresponding value is a
 *    CFNumberRef. If missing, assume kFontNoScriptCode.
 */
#define kATSFontNameTableScript         CFSTR("font script code")

/*
 *  kATSFontNameTableLanguage
 *  
 *  Discussion:
 *    The font name's language code. The corresponding value is a
 *    CFNumberRef. If missing, assume kFontNoLanguageCode.
 */
#define kATSFontNameTableLanguage       CFSTR("font language code")

/*
 *  kATSFontNameTableBytes
 *  
 *  Discussion:
 *    The raw bytes of the font name. The corresponding value is a
 *    CFDataRef that contains the raw name bytes.
 */
#define kATSFontNameTableBytes          CFSTR("font name table bytes")

#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __ATSFONT__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__ATSFONT__RESTORE_PACKED_ENUMS)
    #pragma options(pack_enums)
#endif

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

#endif /* __ATSFONT__ */

