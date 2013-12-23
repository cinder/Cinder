/*
     File:       URLAccess.h
 
     Contains:   URL Access Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1994-2000 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __URLACCESS__
#define __URLACCESS__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
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

/* Data structures and types */
typedef struct OpaqueURLReference*      URLReference;
typedef UInt32 URLOpenFlags;
enum {
  kURLReplaceExistingFlag       = 1 << 0,
  kURLBinHexFileFlag            = 1 << 1, /* Binhex before uploading if necessary*/
  kURLExpandFileFlag            = 1 << 2, /* Use StuffIt engine to expand file if necessary*/
  kURLDisplayProgressFlag       = 1 << 3,
  kURLDisplayAuthFlag           = 1 << 4, /* Display auth dialog if guest connection fails*/
  kURLUploadFlag                = 1 << 5, /* Do an upload instead of a download*/
  kURLIsDirectoryHintFlag       = 1 << 6, /* Hint: the URL is a directory*/
  kURLDoNotTryAnonymousFlag     = 1 << 7, /* Don't try to connect anonymously before getting logon info*/
  kURLDirectoryListingFlag      = 1 << 8, /* Download the directory listing, not the whole directory*/
  kURLExpandAndVerifyFlag       = 1 << 9, /* Expand file and then verify using signature resource*/
  kURLNoAutoRedirectFlag        = 1 << 10, /* Do not automatically redirect to new URL*/
  kURLDebinhexOnlyFlag          = 1 << 11, /* Do not use Stuffit Expander - just internal debinhex engine*/
  kURLDoNotDeleteOnErrorFlag    = 1 << 12, /* Do not delete the downloaded file if an error or abort occurs.*/
                                        /* This flag applies to downloading only and should be used if*/
                                        /* interested in later resuming the download.*/
  kURLResumeDownloadFlag        = 1 << 13, /* The passed in file is partially downloaded, attempt to resume*/
                                        /* it.  Currently works for HTTP only.  If no FSSpec passed in,*/
                                        /* this flag will be ignored. Overriden by kURLReplaceExistingFlag. */
  kURLReservedFlag              = (unsigned long)(1 << 31) /* reserved for Apple internal use*/
};

typedef UInt32 URLState;
enum {
  kURLNullState                 = 0,
  kURLInitiatingState           = 1,
  kURLLookingUpHostState        = 2,
  kURLConnectingState           = 3,
  kURLResourceFoundState        = 4,
  kURLDownloadingState          = 5,
  kURLDataAvailableState        = 0x10 + kURLDownloadingState,
  kURLTransactionCompleteState  = 6,
  kURLErrorOccurredState        = 7,
  kURLAbortingState             = 8,
  kURLCompletedState            = 9,
  kURLUploadingState            = 10
};

typedef UInt32 URLEvent;
enum {
  kURLInitiatedEvent            = kURLInitiatingState,
  kURLResourceFoundEvent        = kURLResourceFoundState,
  kURLDownloadingEvent          = kURLDownloadingState,
  kURLAbortInitiatedEvent       = kURLAbortingState,
  kURLCompletedEvent            = kURLCompletedState,
  kURLErrorOccurredEvent        = kURLErrorOccurredState,
  kURLDataAvailableEvent        = kURLDataAvailableState,
  kURLTransactionCompleteEvent  = kURLTransactionCompleteState,
  kURLUploadingEvent            = kURLUploadingState,
  kURLSystemEvent               = 29,
  kURLPercentEvent              = 30,
  kURLPeriodicEvent             = 31,
  kURLPropertyChangedEvent      = 32
};

typedef unsigned long                   URLEventMask;
enum {
  kURLInitiatedEventMask        = 1 << (kURLInitiatedEvent - 1),
  kURLResourceFoundEventMask    = 1 << (kURLResourceFoundEvent - 1),
  kURLDownloadingMask           = 1 << (kURLDownloadingEvent - 1),
  kURLUploadingMask             = 1 << (kURLUploadingEvent - 1),
  kURLAbortInitiatedMask        = 1 << (kURLAbortInitiatedEvent - 1),
  kURLCompletedEventMask        = 1 << (kURLCompletedEvent - 1),
  kURLErrorOccurredEventMask    = 1 << (kURLErrorOccurredEvent - 1),
  kURLDataAvailableEventMask    = 1 << (kURLDataAvailableEvent - 1),
  kURLTransactionCompleteEventMask = 1 << (kURLTransactionCompleteEvent - 1),
  kURLSystemEventMask           = 1 << (kURLSystemEvent - 1),
  kURLPercentEventMask          = 1 << (kURLPercentEvent - 1),
  kURLPeriodicEventMask         = 1 << (kURLPeriodicEvent - 1),
  kURLPropertyChangedEventMask  = 1 << (kURLPropertyChangedEvent - 1),
  kURLAllBufferEventsMask       = kURLDataAvailableEventMask + kURLTransactionCompleteEventMask,
  kURLAllNonBufferEventsMask    = kURLInitiatedEventMask + kURLDownloadingMask + kURLUploadingMask + kURLAbortInitiatedMask + kURLCompletedEventMask + kURLErrorOccurredEventMask + kURLPercentEventMask + kURLPeriodicEventMask + kURLPropertyChangedEventMask,
  kURLAllEventsMask             = (long)0xFFFFFFFF
};


struct URLCallbackInfo {
  UInt32              version;
  URLReference        urlRef;
  const char *        property;
  UInt32              currentSize;
  EventRecord *       systemEvent;
};
typedef struct URLCallbackInfo          URLCallbackInfo;

/* authentication type flags*/
enum {
  kUserNameAndPasswordFlag      = 0x00000001
};

#define kURLURL                         "URLString"
#define kURLResourceSize                "URLResourceSize"
#define kURLLastModifiedTime            "URLLastModifiedTime"
#define kURLMIMEType                    "URLMIMEType"
#define kURLFileType                    "URLFileType"
#define kURLFileCreator                 "URLFileCreator"
#define kURLCharacterSet                "URLCharacterSet"
#define kURLResourceName                "URLResourceName"
#define kURLHost                        "URLHost"
#define kURLAuthType                    "URLAuthType"
#define kURLUserName                    "URLUserName"
#define kURLPassword                    "URLPassword"
#define kURLStatusString                "URLStatusString"
#define kURLIsSecure                    "URLIsSecure"
#define kURLCertificate                 "URLCertificate"
#define kURLTotalItems                  "URLTotalItems"
#define kURLConnectTimeout              "URLConnectTimeout"
/* http and https properties*/
#define kURLHTTPRequestMethod           "URLHTTPRequestMethod"
#define kURLHTTPRequestHeader           "URLHTTPRequestHeader"
#define kURLHTTPRequestBody             "URLHTTPRequestBody"
#define kURLHTTPRespHeader              "URLHTTPRespHeader"
#define kURLHTTPUserAgent               "URLHTTPUserAgent"
#define kURLHTTPRedirectedURL           "URLHTTPRedirectedURL"
#define kURLSSLCipherSuite              "URLSSLCipherSuite"




/*
 *  URLGetURLAccessVersion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLGetURLAccessVersion(UInt32 * returnVers);



#if TARGET_RT_MAC_CFM
#ifdef __cplusplus
    inline pascal Boolean URLAccessAvailable() { return ((URLGetURLAccessVersion != (void*)kUnresolvedCFragSymbolAddress) ); }
#else
    #define URLAccessAvailable()    ((URLGetURLAccessVersion != (void*)kUnresolvedCFragSymbolAddress) )
#endif
#elif TARGET_RT_MAC_MACHO
/* URL Access is always available on OS X */
#ifdef __cplusplus
    inline pascal Boolean URLAccessAvailable() { return true; }
#else
    #define URLAccessAvailable()    (true)
#endif
#endif  /*  */

typedef CALLBACK_API( OSStatus , URLNotifyProcPtr )(void *userContext, URLEvent event, URLCallbackInfo *callbackInfo);
typedef CALLBACK_API( OSStatus , URLSystemEventProcPtr )(void *userContext, EventRecord *event);
typedef TVECTOR_UPP_TYPE(URLNotifyProcPtr)                      URLNotifyUPP;
typedef TVECTOR_UPP_TYPE(URLSystemEventProcPtr)                 URLSystemEventUPP;
/*
 *  NewURLNotifyUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( URLNotifyUPP )
NewURLNotifyUPP(URLNotifyProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppURLNotifyProcInfo = 0x00000FF0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(URLNotifyUPP) NewURLNotifyUPP(URLNotifyProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewURLNotifyUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewURLSystemEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( URLSystemEventUPP )
NewURLSystemEventUPP(URLSystemEventProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppURLSystemEventProcInfo = 0x000003F0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(URLSystemEventUPP) NewURLSystemEventUPP(URLSystemEventProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewURLSystemEventUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  DisposeURLNotifyUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeURLNotifyUPP(URLNotifyUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeURLNotifyUPP(URLNotifyUPP) {}
  #else
      #define DisposeURLNotifyUPP(userUPP)
  #endif
#endif

/*
 *  DisposeURLSystemEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeURLSystemEventUPP(URLSystemEventUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeURLSystemEventUPP(URLSystemEventUPP) {}
  #else
      #define DisposeURLSystemEventUPP(userUPP)
  #endif
#endif

/*
 *  InvokeURLNotifyUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeURLNotifyUPP(
  void *             userContext,
  URLEvent           event,
  URLCallbackInfo *  callbackInfo,
  URLNotifyUPP       userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeURLNotifyUPP(void * userContext, URLEvent event, URLCallbackInfo * callbackInfo, URLNotifyUPP userUPP) { return (*userUPP)(userContext, event, callbackInfo); }
  #else
      #define InvokeURLNotifyUPP(userContext, event, callbackInfo, userUPP) (*userUPP)(userContext, event, callbackInfo)
  #endif
#endif

/*
 *  InvokeURLSystemEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeURLSystemEventUPP(
  void *             userContext,
  EventRecord *      event,
  URLSystemEventUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeURLSystemEventUPP(void * userContext, EventRecord * event, URLSystemEventUPP userUPP) { return (*userUPP)(userContext, event); }
  #else
      #define InvokeURLSystemEventUPP(userContext, event, userUPP) (*userUPP)(userContext, event)
  #endif
#endif

/*
 *  URLSimpleDownload()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLSimpleDownload(
  const char *        url,
  FSSpec *            destination,             /* can be NULL */
  Handle              destinationHandle,       /* can be NULL */
  URLOpenFlags        openFlags,
  URLSystemEventUPP   eventProc,               /* can be NULL */
  void *              userContext);            /* can be NULL */


/*
 *  URLDownload()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLDownload(
  URLReference        urlRef,
  FSSpec *            destination,             /* can be NULL */
  Handle              destinationHandle,       /* can be NULL */
  URLOpenFlags        openFlags,
  URLSystemEventUPP   eventProc,               /* can be NULL */
  void *              userContext);            /* can be NULL */


/*
 *  URLSimpleUpload()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLSimpleUpload(
  const char *        url,
  const FSSpec *      source,
  URLOpenFlags        openFlags,
  URLSystemEventUPP   eventProc,         /* can be NULL */
  void *              userContext);      /* can be NULL */


/*
 *  URLUpload()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLUpload(
  URLReference        urlRef,
  const FSSpec *      source,
  URLOpenFlags        openFlags,
  URLSystemEventUPP   eventProc,         /* can be NULL */
  void *              userContext);      /* can be NULL */


/*
 *  URLNewReference()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLNewReference(
  const char *    url,
  URLReference *  urlRef);


/*
 *  URLDisposeReference()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLDisposeReference(URLReference urlRef);


/*
 *  URLOpen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLOpen(
  URLReference   urlRef,
  FSSpec *       fileSpec,            /* can be NULL */
  URLOpenFlags   openFlags,
  URLNotifyUPP   notifyProc,          /* can be NULL */
  URLEventMask   eventRegister,
  void *         userContext);        /* can be NULL */


/*
 *  URLAbort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLAbort(URLReference urlRef);


/*
 *  URLGetDataAvailable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLGetDataAvailable(
  URLReference   urlRef,
  Size *         dataSize);


/*
 *  URLGetBuffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLGetBuffer(
  URLReference   urlRef,
  void **        buffer,
  Size *         bufferSize);


/*
 *  URLReleaseBuffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLReleaseBuffer(
  URLReference   urlRef,
  void *         buffer);


/*
 *  URLGetProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLGetProperty(
  URLReference   urlRef,
  const char *   property,
  void *         propertyBuffer,
  Size           bufferSize);


/*
 *  URLGetPropertySize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLGetPropertySize(
  URLReference   urlRef,
  const char *   property,
  Size *         propertySize);


/*
 *  URLSetProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLSetProperty(
  URLReference   urlRef,
  const char *   property,
  void *         propertyBuffer,
  Size           bufferSize);


/*
 *  URLGetCurrentState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLGetCurrentState(
  URLReference   urlRef,
  URLState *     state);


/*
 *  URLGetError()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLGetError(
  URLReference   urlRef,
  OSStatus *     urlError);


/*
 *  URLIdle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLIdle(void);


/*
 *  URLGetFileInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in URLAccessLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
URLGetFileInfo(
  StringPtr   fName,
  OSType *    fType,
  OSType *    fCreator);



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

#endif /* __URLACCESS__ */

