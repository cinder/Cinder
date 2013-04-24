/*
     File:       ICAApplication.h
 
     Contains:   General purpose Image Capture definitions
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __ICAAPPLICATION__
#define __ICAAPPLICATION__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __CFDICTIONARY__
#include <CFDictionary.h>
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

typedef struct OpaqueICAObject*         ICAObject;
typedef struct OpaqueICAProperty*       ICAProperty;
typedef struct OpaqueICAConnectionID*   ICAConnectionID;
typedef struct OpaqueICAEventDataCookie*  ICAEventDataCookie;
typedef struct OpaqueICAScannerSessionID*  ICAScannerSessionID;
/*
--------------- Defines --------------- 
*/
/* Error codes (Image Capture range = -9900...-9949)*/
enum {
  kICACommunicationErr          = -9900,
  kICADeviceNotFoundErr         = -9901,
  kICADeviceNotOpenErr          = -9902,
  kICAFileCorruptedErr          = -9903,
  kICAIOPendingErr              = -9904,
  kICAInvalidObjectErr          = -9905,
  kICAInvalidPropertyErr        = -9906,
  kICAIndexOutOfRangeErr        = -9907,
  kICAPropertyTypeNotFoundErr   = -9908
};

/* ICAObject types and subtypes */
enum {
  kICADevice                    = FOUR_CHAR_CODE('icdv'), /* Also creator of device library files */
  kICADeviceCamera              = FOUR_CHAR_CODE('cmra'), /* Also file type of device library files */
  kICADeviceScanner             = FOUR_CHAR_CODE('scan'), /* Also file type of device library files */
  kICADeviceMFP                 = FOUR_CHAR_CODE('mfp '), /* Also file type of device library files */
  kICAList                      = FOUR_CHAR_CODE('objl'),
  kICADirectory                 = FOUR_CHAR_CODE('dire'),
  kICAFile                      = FOUR_CHAR_CODE('file'),
  kICAFileImage                 = FOUR_CHAR_CODE('imag'),
  kICAFileMovie                 = FOUR_CHAR_CODE('moov'),
  kICAFileAudio                 = FOUR_CHAR_CODE('audo'),
  kICAFileFirmware              = FOUR_CHAR_CODE('firm'),
  kICAFileOther                 = FOUR_CHAR_CODE('othe')
};

/* ICAProperties */
enum {
  kICAProperty                  = FOUR_CHAR_CODE('prop'), /* */
                                        /*    file properties */
                                        /* */
                                        /* for images, refer to 'Digital Still Camera Image File Format Standard' Exif Version 2.1 section 2.6.4. and 2.6.5.*/
  kICAPropertyImageWidth        = FOUR_CHAR_CODE('0100'), /* UInt32 */
  kICAPropertyImageHeight       = FOUR_CHAR_CODE('0101'), /* UInt32 */
  kICAPropertyImageBitDepth     = FOUR_CHAR_CODE('0102'), /* UInt32 */
  kICAPropertyImageDPI          = FOUR_CHAR_CODE('011A'), /* UInt32 */
  kICAPropertyImageExposureTime = FOUR_CHAR_CODE('829A'),
  kICAPropertyImageFNumber      = FOUR_CHAR_CODE('829D'),
  kICAPropertyImageDateOriginal = FOUR_CHAR_CODE('9003'), /* null terminated string (YYYYMMDDThhmmss.s) */
  kICAPropertyImageDateDigitized = FOUR_CHAR_CODE('9004'), /* null terminated string (YYYYMMDDThhmmss.s) */
  kICAPropertyImageShutterSpeed = FOUR_CHAR_CODE('9201'), /* */
  kICAPropertyImageAperture     = FOUR_CHAR_CODE('9202'), /* */
  kICAPropertyImageFlash        = FOUR_CHAR_CODE('9209'), /* UInt16*/
  kICAPropertyColorSpace        = FOUR_CHAR_CODE('A001'), /* UInt16*/
  kICAPropertyImageFilename     = FOUR_CHAR_CODE('ifil'), /* null terminated string */
  kICAPropertyImageSize         = FOUR_CHAR_CODE('isiz'), /* UInt32 */
  kICAPropertyImageData         = FOUR_CHAR_CODE('idat'), /* void * */
  kICAPropertyImageThumbnail    = FOUR_CHAR_CODE('thum'), /* void * */
  kICAPropertyColorSyncProfile  = FOUR_CHAR_CODE('prof')
};

/* Messages */
enum {
  kICAMessageConnect            = FOUR_CHAR_CODE('open'),
  kICAMessageDisconnect         = FOUR_CHAR_CODE('clos'),
  kICAMessageReset              = FOUR_CHAR_CODE('rese'),
  kICAMessageCheckDevice        = FOUR_CHAR_CODE('chkd')
};


/* Data type definitions, mapped to AppleEvent types */
enum {
  kICATypeUInt16                = FOUR_CHAR_CODE('ui16'), /* UInt16 */
  kICATypeUInt32                = FOUR_CHAR_CODE('ui32'), /* UInt32 */
  kICATypeUInt64                = FOUR_CHAR_CODE('ui64'), /* UInt64 */
  kICATypeSInt16                = FOUR_CHAR_CODE('si16'), /* SInt16 */
  kICATypeSInt32                = FOUR_CHAR_CODE('si32'), /* SInt32 */
  kICATypeSInt64                = FOUR_CHAR_CODE('si64'), /* SInt64 */
  kICATypeFixed                 = FOUR_CHAR_CODE('sing'), /* typeIEEE32BitFloatingPoint */
  kICATypeBoolean               = FOUR_CHAR_CODE('bool'), /* typeBoolean */
  kICATypeString                = FOUR_CHAR_CODE('TEXT'), /* typeChar */
  kICATypeData                  = FOUR_CHAR_CODE('data'), /* void * */
  kICATypeThumbnail             = FOUR_CHAR_CODE('thum') /* ICAThumbnail*/
};


/* Flags for PropertyInfo flag element */
enum {
  kICAFlagReadWriteAccess       = 1L << 0,
  kICAFlagReadAccess            = 1L << 1
};



/* Notification types (Refer to section 12.4 of PTP spec) */
enum {
  kICAEventCancelTransaction    = FOUR_CHAR_CODE('ecnt'),
  kICAEventObjectAdded          = FOUR_CHAR_CODE('eoba'),
  kICAEventObjectRemoved        = FOUR_CHAR_CODE('eobr'),
  kICAEventStoreAdded           = FOUR_CHAR_CODE('esta'),
  kICAEventStoreRemoved         = FOUR_CHAR_CODE('estr'),
  kICAEventDeviceAdded          = FOUR_CHAR_CODE('edea'),
  kICAEventDeviceRemoved        = FOUR_CHAR_CODE('eder'),
  kICAEventDevicePropChanged    = FOUR_CHAR_CODE('edpc'),
  kICAEventObjectInfoChanged    = FOUR_CHAR_CODE('eoic'),
  kICAEventDeviceInfoChanged    = FOUR_CHAR_CODE('edic'),
  kICAEventRequestObjectTransfer = FOUR_CHAR_CODE('erot'),
  kICAEventStoreFull            = FOUR_CHAR_CODE('estf'),
  kICAEventDeviceReset          = FOUR_CHAR_CODE('edvr'),
  kICAEventStorageInfoChanged   = FOUR_CHAR_CODE('esic'),
  kICAEventCaptureComplete      = FOUR_CHAR_CODE('ecpc'),
  kICAEventUnreportedStatus     = FOUR_CHAR_CODE('eurs')
};


/* Used for partial reads via ICAGetPropertyData */
enum {
  kICAStartAtBeginning          = 0,
  kICAEntireLength              = -1
};

/* ICADownloadFile flags */
enum {
  kDeleteAfterDownload          = 0x00000001,
  kCreateCustomIcon             = 0x00000002,
  kAddMetaDataToFinderComment   = 0x00000004,
  kAdjustCreationDate           = 0x00000008,
  kSetFileTypeAndCreator        = 0x00000010,
  kEmbedColorSyncProfile        = 0x00000020,
  kRotateImage                  = 0x00000040
};

/* extended notification */
enum {
  kExtendedNotificationPB       = FOUR_CHAR_CODE('extd'),
  kMessageGetEventData          = FOUR_CHAR_CODE('mged'),
  kEventClassPTPStandard        = FOUR_CHAR_CODE('PTPs'),
  kEventClassPTPVendor          = FOUR_CHAR_CODE('PTPv')
};

/*
--------------- Structures --------------- 
*/
struct ICAObjectInfo {
  OSType              objectType;             /* i.e. kICAFile*/
  OSType              objectSubtype;          /* i.e. kICAFileImage  */
};
typedef struct ICAObjectInfo            ICAObjectInfo;
 
 
struct ICAPropertyInfo {
  OSType              propertyType;
  OSType              dataType;
  UInt32              dataSize;
  UInt32              dataFlags;
};
typedef struct ICAPropertyInfo          ICAPropertyInfo;
 
 
struct ICAMessage {
  OSType              messageType;            /* <--  i.e. kICAMessageCameraCaptureNewImage */
  UInt32              startByte;              /* <-- */
  void *              dataPtr;                /* <-- */
  UInt32              dataSize;               /* <-- */
  OSType              dataType;               /* <-- */
};
typedef struct ICAMessage               ICAMessage;
 
 
struct ICAThumbnail {
  UInt32              width;
  UInt32              height;
  UInt32              dataSize;
  UInt8               data[1];                /* 8-bit RGB data (RGBRGBRGB...)*/
};
typedef struct ICAThumbnail             ICAThumbnail;
enum {
  kICAPBVersion                 = 0x00010000
};

/* 
--------------- Completion Procs --------------- 
*/
/*
   
   NOTE: the parameter for the completion proc (ICAHeader*) has to be casted to the appropriate type
   e.g. (ICAGetChildCountPB*), ...
   
*/

typedef struct ICAHeader                ICAHeader;
typedef CALLBACK_API_C( void , ICACompletion )(ICAHeader * pb);
/* 
--------------- ICAHeader --------------- 
*/
 
 
struct ICAHeader {
  OSErr               err;                    /* --> */
  UInt32              refcon;                 /* <-- */
};

/*
--------------- Object parameter blocks --------------- 
*/
 
 
struct ICAGetChildCountPB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  UInt32              count;                  /* --> */
};
typedef struct ICAGetChildCountPB       ICAGetChildCountPB;
 
 
struct ICAGetNthChildPB {
  ICAHeader           header;
  ICAObject           parentObject;           /* <-- */
  UInt32              index;                  /* <-- zero based */
  ICAObject           childObject;            /* --> */
  ICAObjectInfo       childInfo;              /* --> */
};
typedef struct ICAGetNthChildPB         ICAGetNthChildPB;
 
 
struct ICAGetObjectInfoPB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  ICAObjectInfo       objectInfo;             /* --> */
};
typedef struct ICAGetObjectInfoPB       ICAGetObjectInfoPB;
 
 
struct ICAGetParentOfObjectPB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  ICAObject           parentObject;           /* --> */
  ICAObjectInfo       parentInfo;             /* --> */
};
typedef struct ICAGetParentOfObjectPB   ICAGetParentOfObjectPB;
 
 
struct ICAGetRootOfObjectPB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  ICAObject           rootObject;             /* --> */
  ICAObjectInfo       rootInfo;               /* --> */
};
typedef struct ICAGetRootOfObjectPB     ICAGetRootOfObjectPB;
 
 
struct ICAGetObjectRefConPB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  UInt32              objectRefCon;           /* --> */
};
typedef struct ICAGetObjectRefConPB     ICAGetObjectRefConPB;
 
 
struct ICASetObjectRefConPB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  UInt32              objectRefCon;           /* <-- */
};
typedef struct ICASetObjectRefConPB     ICASetObjectRefConPB;
/*
--------------- Property parameter blocks --------------- 
*/
 
 
struct ICAGetPropertyCountPB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  UInt32              count;                  /* --> */
};
typedef struct ICAGetPropertyCountPB    ICAGetPropertyCountPB;
 
 
struct ICAGetNthPropertyPB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  UInt32              index;                  /* <-- zero based */
  ICAProperty         property;               /* --> */
  ICAPropertyInfo     propertyInfo;           /* --> */
};
typedef struct ICAGetNthPropertyPB      ICAGetNthPropertyPB;
 
 
struct ICAGetPropertyByTypePB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  OSType              propertyType;           /* <-- */
  ICAProperty         property;               /* --> */
  ICAPropertyInfo     propertyInfo;           /* --> */
};
typedef struct ICAGetPropertyByTypePB   ICAGetPropertyByTypePB;
 
 
struct ICAGetPropertyInfoPB {
  ICAHeader           header;
  ICAProperty         property;               /* <-- */
  ICAPropertyInfo     propertyInfo;           /* --> */
};
typedef struct ICAGetPropertyInfoPB     ICAGetPropertyInfoPB;
 
 
struct ICAGetPropertyDataPB {
  ICAHeader           header;
  ICAProperty         property;               /* <-- */
  UInt32              startByte;              /* <-- */
  UInt32              requestedSize;          /* <-- */
  void *              dataPtr;                /* <-> */
  UInt32              actualSize;             /* --> */
  OSType              dataType;               /* --> */
};
typedef struct ICAGetPropertyDataPB     ICAGetPropertyDataPB;
 
 
struct ICASetPropertyDataPB {
  ICAHeader           header;
  ICAProperty         property;               /* <-- */
  UInt32              startByte;              /* <-- */
  void *              dataPtr;                /* <-- */
  UInt32              dataSize;               /* <-- */
  OSType              dataType;               /* <-- */
};
typedef struct ICASetPropertyDataPB     ICASetPropertyDataPB;
 
 
struct ICAGetParentOfPropertyPB {
  ICAHeader           header;
  ICAProperty         property;               /* <-- */
  ICAObject           parentObject;           /* --> */
  ICAObjectInfo       parentInfo;             /* --> */
};
typedef struct ICAGetParentOfPropertyPB ICAGetParentOfPropertyPB;
 
 
struct ICAGetRootOfPropertyPB {
  ICAHeader           header;
  ICAProperty         property;               /* <-- */
  ICAObject           rootObject;             /* --> */
  ICAObjectInfo       rootInfo;               /* --> */
};
typedef struct ICAGetRootOfPropertyPB   ICAGetRootOfPropertyPB;
 
 
struct ICAGetPropertyRefConPB {
  ICAHeader           header;
  ICAProperty         property;               /* <-- */
  UInt32              propertyRefCon;         /* --> */
};
typedef struct ICAGetPropertyRefConPB   ICAGetPropertyRefConPB;
 
 
struct ICASetPropertyRefConPB {
  ICAHeader           header;
  ICAProperty         property;               /* <-- */
  UInt32              propertyRefCon;         /* <-- */
};
typedef struct ICASetPropertyRefConPB   ICASetPropertyRefConPB;
/*
--------------- Device parameter blocks --------------- 
*/
 
 
struct ICAGetDeviceListPB {
  ICAHeader           header;
  ICAObject           object;                 /* --> */
};
typedef struct ICAGetDeviceListPB       ICAGetDeviceListPB;
 
 
struct ICAObjectSendMessagePB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  ICAMessage          message;                /* <-- */
  UInt32              result;                 /* --> message specific result*/
};
typedef struct ICAObjectSendMessagePB   ICAObjectSendMessagePB;
 
 
struct ICARegisterEventNotificationPB {
  ICAHeader           header;
  ICAObject           object;                 /* <-> */
  OSType              notifyType;             /* <-> */
  ICACompletion       notifyProc;             /* <-- */
};
typedef struct ICARegisterEventNotificationPB ICARegisterEventNotificationPB;
 
 
struct ICAExtendedRegisterEventNotificationPB {
  ICAHeader           header;
  ICAObject           object;                 /* <-> */
  OSType              extd;                   /* <-- */
  ICACompletion       notifyProc;             /* <-- */

  UInt32              rawEventType;           /* -->*/
  OSType              eventType;              /* <->*/
  OSType              eventClass;             /* -->*/
  UInt32              eventDataSize;          /* -->*/
  ICAEventDataCookie  eventDataCookie;        /* -->*/
  ICAObject           deviceObject;           /* -->*/
};
typedef struct ICAExtendedRegisterEventNotificationPB ICAExtendedRegisterEventNotificationPB;
 
 
struct ICADownloadFilePB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  FSRef *             dirFSRef;               /* <-- */
  UInt32              flags;                  /* <-- */
  OSType              fileType;               /* <-- */
  OSType              fileCreator;            /* <-- */
  Fixed               rotationAngle;          /* <-- */
  FSRef *             fileFSRef;              /* --> can be NULL*/
};
typedef struct ICADownloadFilePB        ICADownloadFilePB;
 
 
struct ICACopyObjectPropertyDictionaryPB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  CFDictionaryRef *   theDict;                /* --> */
};
typedef struct ICACopyObjectPropertyDictionaryPB ICACopyObjectPropertyDictionaryPB;
/*
   
   NOTE: for all APIs - pass NULL as completion parameter to make a synchronous call 
   
*/
/*
--------------- Object functions --------------- 
*/

/*
 *  ICAGetChildCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetChildCount(
  ICAGetChildCountPB *  pb,
  ICACompletion         completion);      /* can be NULL */


/*
 *  ICAGetNthChild()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetNthChild(
  ICAGetNthChildPB *  pb,
  ICACompletion       completion);      /* can be NULL */


/*
 *  ICAGetObjectInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetObjectInfo(
  ICAGetObjectInfoPB *  pb,
  ICACompletion         completion);      /* can be NULL */


/*
 *  ICAGetParentOfObject()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetParentOfObject(
  ICAGetParentOfObjectPB *  pb,
  ICACompletion             completion);      /* can be NULL */


/*
 *  ICAGetRootOfObject()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetRootOfObject(
  ICAGetRootOfObjectPB *  pb,
  ICACompletion           completion);      /* can be NULL */


/*
 *  ICAGetObjectRefCon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetObjectRefCon(
  ICAGetObjectRefConPB *  pb,
  ICACompletion           completion);      /* can be NULL */


/*
 *  ICASetObjectRefCon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICASetObjectRefCon(
  ICASetObjectRefConPB *  pb,
  ICACompletion           completion);      /* can be NULL */



/*
--------------- Property functions --------------- 
*/
/*
 *  ICAGetPropertyCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetPropertyCount(
  ICAGetPropertyCountPB *  pb,
  ICACompletion            completion);      /* can be NULL */


/*
 *  ICAGetNthProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetNthProperty(
  ICAGetNthPropertyPB *  pb,
  ICACompletion          completion);      /* can be NULL */


/*
 *  ICAGetPropertyByType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetPropertyByType(
  ICAGetPropertyByTypePB *  pb,
  ICACompletion             completion);      /* can be NULL */


/*
 *  ICAGetPropertyInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetPropertyInfo(
  ICAGetPropertyInfoPB *  pb,
  ICACompletion           completion);      /* can be NULL */


/*
 *  ICAGetPropertyData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetPropertyData(
  ICAGetPropertyDataPB *  pb,
  ICACompletion           completion);      /* can be NULL */


/*
 *  ICASetPropertyData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICASetPropertyData(
  ICASetPropertyDataPB *  pb,
  ICACompletion           completion);      /* can be NULL */


/*
 *  ICAGetParentOfProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetParentOfProperty(
  ICAGetParentOfPropertyPB *  pb,
  ICACompletion               completion);      /* can be NULL */


/*
 *  ICAGetRootOfProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetRootOfProperty(
  ICAGetRootOfPropertyPB *  pb,
  ICACompletion             completion);      /* can be NULL */


/*
 *  ICAGetPropertyRefCon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetPropertyRefCon(
  ICAGetPropertyRefConPB *  pb,
  ICACompletion             completion);      /* can be NULL */


/*
 *  ICASetPropertyRefCon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICASetPropertyRefCon(
  ICASetPropertyRefConPB *  pb,
  ICACompletion             completion);      /* can be NULL */



/* 
--------------- Device functions --------------- 
*/
/*
 *  ICAGetDeviceList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAGetDeviceList(
  ICAGetDeviceListPB *  pb,
  ICACompletion         completion);      /* can be NULL */


/*
 *  ICAObjectSendMessage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICAObjectSendMessage(
  ICAObjectSendMessagePB *  pb,
  ICACompletion             completion);      /* can be NULL */


/*
 *  ICARegisterEventNotification()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ICARegisterEventNotification(
  ICARegisterEventNotificationPB *  pb,
  ICACompletion                     completion);      /* can be NULL */




/* 
------------------------------------------------ 
*/
/*
 *  ICADownloadFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSErr )
ICADownloadFile(
  ICADownloadFilePB *  pb,
  ICACompletion        completion);      /* can be NULL */



/*
 *  ICACopyObjectPropertyDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSErr )
ICACopyObjectPropertyDictionary(
  ICACopyObjectPropertyDictionaryPB *  pb,
  ICACompletion                        completion);      /* can be NULL */



/* 
--- scanner support --------------------------------------------- 
*/
struct ICAScannerOpenSessionPB {
  ICAHeader           header;
  ICAObject           object;                 /* <-- */
  ICAScannerSessionID  sessionID;             /* --> */
};
typedef struct ICAScannerOpenSessionPB  ICAScannerOpenSessionPB;
 
 
struct ICAScannerCloseSessionPB {
  ICAHeader           header;
  ICAScannerSessionID  sessionID;             /* <-- */
};
typedef struct ICAScannerCloseSessionPB ICAScannerCloseSessionPB;
 
 
struct ICAScannerInitializePB {
  ICAHeader           header;
  ICAScannerSessionID  sessionID;             /* <-- */
};
typedef struct ICAScannerInitializePB   ICAScannerInitializePB;
 
 
struct ICAScannerGetParametersPB {
  ICAHeader           header;
  ICAScannerSessionID  sessionID;             /* <-- */
  CFMutableDictionaryRef  theDict;            /* --> */
};
typedef struct ICAScannerGetParametersPB ICAScannerGetParametersPB;
 
 
struct ICAScannerSetParametersPB {
  ICAHeader           header;
  ICAScannerSessionID  sessionID;             /* <-- */
  CFMutableDictionaryRef  theDict;            /* <-> */
};
typedef struct ICAScannerSetParametersPB ICAScannerSetParametersPB;
 
 
struct ICAScannerStatusPB {
  ICAHeader           header;
  ICAScannerSessionID  sessionID;             /* <-- */
  UInt32              status;                 /* --> */
};
typedef struct ICAScannerStatusPB       ICAScannerStatusPB;
 
 
struct ICAScannerStartPB {
  ICAHeader           header;
  ICAScannerSessionID  sessionID;             /* <-- */
};
typedef struct ICAScannerStartPB        ICAScannerStartPB;
 
 
/*
 *  ICAScannerOpenSession()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.6 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSErr )
ICAScannerOpenSession(
  ICAScannerOpenSessionPB *  pb,
  ICACompletion              completion);      /* can be NULL */


/*
 *  ICAScannerCloseSession()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.6 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSErr )
ICAScannerCloseSession(
  ICAScannerCloseSessionPB *  pb,
  ICACompletion               completion);      /* can be NULL */


/*
 *  ICAScannerInitialize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.6 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSErr )
ICAScannerInitialize(
  ICAScannerInitializePB *  pb,
  ICACompletion             completion);      /* can be NULL */


/*
 *  ICAScannerGetParameters()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.6 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSErr )
ICAScannerGetParameters(
  ICAScannerGetParametersPB *  pb,
  ICACompletion                completion);      /* can be NULL */


/*
 *  ICAScannerSetParameters()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.6 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSErr )
ICAScannerSetParameters(
  ICAScannerSetParametersPB *  pb,
  ICACompletion                completion);      /* can be NULL */


/*
 *  ICAScannerStatus()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.6 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSErr )
ICAScannerStatus(
  ICAScannerStatusPB *  pb,
  ICACompletion         completion);      /* can be NULL */


/*
 *  ICAScannerStart()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.6 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSErr )
ICAScannerStart(
  ICAScannerStartPB *  pb,
  ICACompletion        completion);      /* can be NULL */



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

#endif /* __ICAAPPLICATION__ */

