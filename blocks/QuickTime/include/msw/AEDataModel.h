/*
     File:       AEDataModel.h
 
     Contains:   AppleEvent Data Model Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1996-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __AEDATAMODEL__
#define __AEDATAMODEL__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
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

/* Apple event descriptor types */
enum {
  typeBoolean                   = FOUR_CHAR_CODE('bool'),
  typeChar                      = FOUR_CHAR_CODE('TEXT')
};

/* Preferred numeric Apple event descriptor types */
enum {
  typeSInt16                    = FOUR_CHAR_CODE('shor'),
  typeSInt32                    = FOUR_CHAR_CODE('long'),
  typeUInt32                    = FOUR_CHAR_CODE('magn'),
  typeSInt64                    = FOUR_CHAR_CODE('comp'),
  typeIEEE32BitFloatingPoint    = FOUR_CHAR_CODE('sing'),
  typeIEEE64BitFloatingPoint    = FOUR_CHAR_CODE('doub'),
  type128BitFloatingPoint       = FOUR_CHAR_CODE('ldbl'),
  typeDecimalStruct             = FOUR_CHAR_CODE('decm')
};

/* Non-preferred Apple event descriptor types */
enum {
  typeSMInt                     = typeSInt16,
  typeShortInteger              = typeSInt16,
  typeInteger                   = typeSInt32,
  typeLongInteger               = typeSInt32,
  typeMagnitude                 = typeUInt32,
  typeComp                      = typeSInt64,
  typeSMFloat                   = typeIEEE32BitFloatingPoint,
  typeShortFloat                = typeIEEE32BitFloatingPoint,
  typeFloat                     = typeIEEE64BitFloatingPoint,
  typeLongFloat                 = typeIEEE64BitFloatingPoint,
  typeExtended                  = FOUR_CHAR_CODE('exte')
};

/* More Apple event descriptor types */
enum {
  typeAEList                    = FOUR_CHAR_CODE('list'),
  typeAERecord                  = FOUR_CHAR_CODE('reco'),
  typeAppleEvent                = FOUR_CHAR_CODE('aevt'),
  typeEventRecord               = FOUR_CHAR_CODE('evrc'),
  typeTrue                      = FOUR_CHAR_CODE('true'),
  typeFalse                     = FOUR_CHAR_CODE('fals'),
  typeAlias                     = FOUR_CHAR_CODE('alis'),
  typeEnumerated                = FOUR_CHAR_CODE('enum'),
  typeType                      = FOUR_CHAR_CODE('type'),
  typeAppParameters             = FOUR_CHAR_CODE('appa'),
  typeProperty                  = FOUR_CHAR_CODE('prop'),
  typeFSS                       = FOUR_CHAR_CODE('fss '),
  typeFSRef                     = FOUR_CHAR_CODE('fsrf'),
  typeFileURL                   = FOUR_CHAR_CODE('furl'),
  typeKeyword                   = FOUR_CHAR_CODE('keyw'),
  typeSectionH                  = FOUR_CHAR_CODE('sect'),
  typeWildCard                  = FOUR_CHAR_CODE('****'),
  typeApplSignature             = FOUR_CHAR_CODE('sign'),
  typeQDRectangle               = FOUR_CHAR_CODE('qdrt'),
  typeFixed                     = FOUR_CHAR_CODE('fixd'),
  typeProcessSerialNumber       = FOUR_CHAR_CODE('psn '),
  typeApplicationURL            = FOUR_CHAR_CODE('aprl'),
  typeNull                      = FOUR_CHAR_CODE('null') /* null or nonexistent data */
};

#if CALL_NOT_IN_CARBON
/* Deprecated addressing modes under Carbon */
enum {
  typeSessionID                 = FOUR_CHAR_CODE('ssid'),
  typeTargetID                  = FOUR_CHAR_CODE('targ'),
  typeDispatcherID              = FOUR_CHAR_CODE('dspt')
};

#endif  /* CALL_NOT_IN_CARBON */

/* Keywords for Apple event attributes */
enum {
  keyTransactionIDAttr          = FOUR_CHAR_CODE('tran'),
  keyReturnIDAttr               = FOUR_CHAR_CODE('rtid'),
  keyEventClassAttr             = FOUR_CHAR_CODE('evcl'),
  keyEventIDAttr                = FOUR_CHAR_CODE('evid'),
  keyAddressAttr                = FOUR_CHAR_CODE('addr'),
  keyOptionalKeywordAttr        = FOUR_CHAR_CODE('optk'),
  keyTimeoutAttr                = FOUR_CHAR_CODE('timo'),
  keyInteractLevelAttr          = FOUR_CHAR_CODE('inte'), /* this attribute is read only - will be set in AESend */
  keyEventSourceAttr            = FOUR_CHAR_CODE('esrc'), /* this attribute is read only - returned as typeShortInteger */
  keyMissedKeywordAttr          = FOUR_CHAR_CODE('miss'), /* this attribute is read only */
  keyOriginalAddressAttr        = FOUR_CHAR_CODE('from'), /* new in 1.0.1 */
  keyAcceptTimeoutAttr          = FOUR_CHAR_CODE('actm') /* new for Mac OS X */
};


/*  Constants used for specifying the factoring of AEDescLists. */
enum {
  kAEDescListFactorNone         = 0,
  kAEDescListFactorType         = 4,
  kAEDescListFactorTypeAndSize  = 8
};

/* Constants used creating an AppleEvent */
enum {
                                        /* Constant for the returnID param of AECreateAppleEvent */
  kAutoGenerateReturnID         = -1,   /* AECreateAppleEvent will generate a session-unique ID */
                                        /* Constant for transaction ID's */
  kAnyTransactionID             = 0     /* no transaction is in use */
};

/* Apple event manager data types */
typedef ResType                         DescType;
typedef FourCharCode                    AEKeyword;
#if OPAQUE_TOOLBOX_STRUCTS
typedef struct OpaqueAEDataStorageType*  AEDataStorageType;
#else
typedef Ptr                             AEDataStorageType;
#endif  /* OPAQUE_TOOLBOX_STRUCTS */

typedef AEDataStorageType *             AEDataStorage;
struct AEDesc {
  DescType            descriptorType;
  AEDataStorage       dataHandle;
};
typedef struct AEDesc                   AEDesc;
typedef AEDesc *                        AEDescPtr;
struct AEKeyDesc {
  AEKeyword           descKey;
  AEDesc              descContent;
};
typedef struct AEKeyDesc                AEKeyDesc;
/* a list of AEDesc's is a special kind of AEDesc */

typedef AEDesc                          AEDescList;
/* AERecord is a list of keyworded AEDesc's */
typedef AEDescList                      AERecord;
/* an AEDesc which contains address data */
typedef AEDesc                          AEAddressDesc;
/* an AERecord that contains an AppleEvent, and related data types */
typedef AERecord                        AppleEvent;
typedef AppleEvent *                    AppleEventPtr;
typedef SInt16                          AEReturnID;
typedef SInt32                          AETransactionID;
typedef FourCharCode                    AEEventClass;
typedef FourCharCode                    AEEventID;
typedef SInt8                           AEArrayType;
enum {
  kAEDataArray                  = 0,
  kAEPackedArray                = 1,
  kAEDescArray                  = 3,
  kAEKeyDescArray               = 4
};


enum {
  kAEHandleArray                = 2
};

union AEArrayData {
  short               kAEDataArray[1];
  char                kAEPackedArray[1];
  Handle              kAEHandleArray[1];
  AEDesc              kAEDescArray[1];
  AEKeyDesc           kAEKeyDescArray[1];
};
typedef union AEArrayData               AEArrayData;
typedef AEArrayData *                   AEArrayDataPointer;
/**************************************************************************
  These constants are used by AEMach and AEInteraction APIs.  They are not
  strictly part of the data format, but are declared here due to layering.
**************************************************************************/
typedef SInt16 AESendPriority;
enum {
  kAENormalPriority             = 0x00000000, /* post message at the end of the event queue */
  kAEHighPriority               = 0x00000001 /* post message at the front of the event queue (same as nAttnMsg) */
};


typedef SInt32 AESendMode;
enum {
  kAENoReply                    = 0x00000001, /* sender doesn't want a reply to event */
  kAEQueueReply                 = 0x00000002, /* sender wants a reply but won't wait */
  kAEWaitReply                  = 0x00000003, /* sender wants a reply and will wait */
  kAEDontReconnect              = 0x00000080, /* don't reconnect if there is a sessClosedErr from PPCToolbox */
  kAEWantReceipt                = 0x00000200, /* (nReturnReceipt) sender wants a receipt of message */
  kAENeverInteract              = 0x00000010, /* server should not interact with user */
  kAECanInteract                = 0x00000020, /* server may try to interact with user */
  kAEAlwaysInteract             = 0x00000030, /* server should always interact with user where appropriate */
  kAECanSwitchLayer             = 0x00000040, /* interaction may switch layer */
  kAEDontRecord                 = 0x00001000, /* don't record this event - available only in vers 1.0.1 and greater */
  kAEDontExecute                = 0x00002000, /* don't send the event for recording - available only in vers 1.0.1 and greater */
  kAEProcessNonReplyEvents      = 0x00008000 /* allow processing of non-reply events while awaiting synchronous AppleEvent reply */
};


/* Constants for timeout durations */
enum {
  kAEDefaultTimeout             = -1,   /* timeout value determined by AEM */
  kNoTimeOut                    = -2    /* wait until reply comes back, however long it takes */
};


/**************************************************************************
  These calls are used to set up and modify the coercion dispatch table.
**************************************************************************/
typedef CALLBACK_API( OSErr , AECoerceDescProcPtr )(const AEDesc *fromDesc, DescType toType, long handlerRefcon, AEDesc *toDesc);
typedef CALLBACK_API( OSErr , AECoercePtrProcPtr )(DescType typeCode, const void *dataPtr, Size dataSize, DescType toType, long handlerRefcon, AEDesc *result);
typedef STACK_UPP_TYPE(AECoerceDescProcPtr)                     AECoerceDescUPP;
typedef STACK_UPP_TYPE(AECoercePtrProcPtr)                      AECoercePtrUPP;
/*
 *  NewAECoerceDescUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AECoerceDescUPP )
NewAECoerceDescUPP(AECoerceDescProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppAECoerceDescProcInfo = 0x00003FE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(AECoerceDescUPP) NewAECoerceDescUPP(AECoerceDescProcPtr userRoutine) { return (AECoerceDescUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAECoerceDescProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewAECoerceDescUPP(userRoutine) (AECoerceDescUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAECoerceDescProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewAECoercePtrUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AECoercePtrUPP )
NewAECoercePtrUPP(AECoercePtrProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppAECoercePtrProcInfo = 0x0003FFE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(AECoercePtrUPP) NewAECoercePtrUPP(AECoercePtrProcPtr userRoutine) { return (AECoercePtrUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAECoercePtrProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewAECoercePtrUPP(userRoutine) (AECoercePtrUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAECoercePtrProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeAECoerceDescUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeAECoerceDescUPP(AECoerceDescUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeAECoerceDescUPP(AECoerceDescUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeAECoerceDescUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeAECoercePtrUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeAECoercePtrUPP(AECoercePtrUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeAECoercePtrUPP(AECoercePtrUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeAECoercePtrUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeAECoerceDescUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeAECoerceDescUPP(
  const AEDesc *   fromDesc,
  DescType         toType,
  long             handlerRefcon,
  AEDesc *         toDesc,
  AECoerceDescUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeAECoerceDescUPP(const AEDesc * fromDesc, DescType toType, long handlerRefcon, AEDesc * toDesc, AECoerceDescUPP userUPP) { return (OSErr)CALL_FOUR_PARAMETER_UPP(userUPP, uppAECoerceDescProcInfo, fromDesc, toType, handlerRefcon, toDesc); }
  #else
    #define InvokeAECoerceDescUPP(fromDesc, toType, handlerRefcon, toDesc, userUPP) (OSErr)CALL_FOUR_PARAMETER_UPP((userUPP), uppAECoerceDescProcInfo, (fromDesc), (toType), (handlerRefcon), (toDesc))
  #endif
#endif

/*
 *  InvokeAECoercePtrUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeAECoercePtrUPP(
  DescType        typeCode,
  const void *    dataPtr,
  Size            dataSize,
  DescType        toType,
  long            handlerRefcon,
  AEDesc *        result,
  AECoercePtrUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeAECoercePtrUPP(DescType typeCode, const void * dataPtr, Size dataSize, DescType toType, long handlerRefcon, AEDesc * result, AECoercePtrUPP userUPP) { return (OSErr)CALL_SIX_PARAMETER_UPP(userUPP, uppAECoercePtrProcInfo, typeCode, dataPtr, dataSize, toType, handlerRefcon, result); }
  #else
    #define InvokeAECoercePtrUPP(typeCode, dataPtr, dataSize, toType, handlerRefcon, result, userUPP) (OSErr)CALL_SIX_PARAMETER_UPP((userUPP), uppAECoercePtrProcInfo, (typeCode), (dataPtr), (dataSize), (toType), (handlerRefcon), (result))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewAECoerceDescProc(userRoutine)                    NewAECoerceDescUPP(userRoutine)
    #define NewAECoercePtrProc(userRoutine)                     NewAECoercePtrUPP(userRoutine)
    #define CallAECoerceDescProc(userRoutine, fromDesc, toType, handlerRefcon, toDesc) InvokeAECoerceDescUPP(fromDesc, toType, handlerRefcon, toDesc, userRoutine)
    #define CallAECoercePtrProc(userRoutine, typeCode, dataPtr, dataSize, toType, handlerRefcon, result) InvokeAECoercePtrUPP(typeCode, dataPtr, dataSize, toType, handlerRefcon, result, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/* a AECoercionHandlerUPP is by default a AECoerceDescUPP.  If you are registering a 
    Ptr based coercion handler you will have to add a cast to AECoerceDescUPP from 
    your AECoercePtrUPP type.  A future release of the interfaces will fix this by
    introducing seperate Desc and Ptr coercion handler installation/remove/query routines. */
typedef AECoerceDescUPP                 AECoercionHandlerUPP;
/*
 *  AEInstallCoercionHandler()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEInstallCoercionHandler(
  DescType               fromType,
  DescType               toType,
  AECoercionHandlerUPP   handler,
  long                   handlerRefcon,
  Boolean                fromTypeIsDesc,
  Boolean                isSysHandler)                        THREEWORDINLINE(0x303C, 0x0A22, 0xA816);


/*
 *  AERemoveCoercionHandler()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AERemoveCoercionHandler(
  DescType               fromType,
  DescType               toType,
  AECoercionHandlerUPP   handler,
  Boolean                isSysHandler)                        THREEWORDINLINE(0x303C, 0x0723, 0xA816);



/*
 *  AEGetCoercionHandler()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEGetCoercionHandler(
  DescType                fromType,
  DescType                toType,
  AECoercionHandlerUPP *  handler,
  long *                  handlerRefcon,
  Boolean *               fromTypeIsDesc,
  Boolean                 isSysHandler)                       THREEWORDINLINE(0x303C, 0x0B24, 0xA816);


/**************************************************************************
  The following calls provide for a coercion interface.
**************************************************************************/
/*
 *  AECoercePtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AECoercePtr(
  DescType      typeCode,
  const void *  dataPtr,
  Size          dataSize,
  DescType      toType,
  AEDesc *      result)                                       THREEWORDINLINE(0x303C, 0x0A02, 0xA816);


/*
 *  AECoerceDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AECoerceDesc(
  const AEDesc *  theAEDesc,
  DescType        toType,
  AEDesc *        result)                                     THREEWORDINLINE(0x303C, 0x0603, 0xA816);



/**************************************************************************
 The following calls apply to any AEDesc. Every 'result' descriptor is
 created for you, so you will be responsible for memory management
 (including disposing) of the descriptors so created.  
**************************************************************************/
/* because AEDescs are opaque under Carbon, this AEInitializeDesc provides a
   'clean' way of initializating them to be empty. */
/*
 *  AEInitializeDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
AEInitializeDesc(AEDesc * desc);



#ifdef __cplusplus
    inline void AEInitializeDescInline(AEDesc* d) { d->descriptorType = typeNull; d->dataHandle = NULL; };
#else
   #define AEInitializeDescInline(__d) do { AEDesc* d = __d; d->descriptorType = typeNull; d->dataHandle = NULL; } while (0)
#endif


/*
 *  AECreateDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AECreateDesc(
  DescType      typeCode,
  const void *  dataPtr,
  Size          dataSize,
  AEDesc *      result)                                       THREEWORDINLINE(0x303C, 0x0825, 0xA816);


/*
 *  AEDisposeDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEDisposeDesc(AEDesc * theAEDesc)                             THREEWORDINLINE(0x303C, 0x0204, 0xA816);


/*
 *  AEDuplicateDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEDuplicateDesc(
  const AEDesc *  theAEDesc,
  AEDesc *        result)                                     THREEWORDINLINE(0x303C, 0x0405, 0xA816);



/*
 * Create an AEDesc with memory "borrowed" from the application. The
 * data passed in *must* be immutable and not freed until the Dispose
 * callback is made.
 * The dispose callback may be made at any time, including during the
 * creation of the descriptor.
 * If possible, the descriptor will be copied to the address space of
 * any recipient process using virtual memory APIs and avoid an
 * actual memory copy.
 */
typedef CALLBACK_API( void , AEDisposeExternalProcPtr )(const void *dataPtr, Size dataLength, long refcon);
typedef STACK_UPP_TYPE(AEDisposeExternalProcPtr)                AEDisposeExternalUPP;
/*
 *  AECreateDescFromExternalPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
AECreateDescFromExternalPtr(
  OSType                 descriptorType,
  const void *           dataPtr,
  Size                   dataLength,
  AEDisposeExternalUPP   disposeCallback,
  long                   disposeRefcon,
  AEDesc *               theDesc);





/**************************************************************************
  The following calls apply to AEDescList. Since AEDescList is a subtype of
  AEDesc, the calls in the previous section can also be used for AEDescList.
  All list and array indices are 1-based. If the data was greater than
  maximumSize in the routines below, then actualSize will be greater than
  maximumSize, but only maximumSize bytes will actually be retrieved.
**************************************************************************/
/*
 *  AECreateList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AECreateList(
  const void *  factoringPtr,
  Size          factoredSize,
  Boolean       isRecord,
  AEDescList *  resultList)                                   THREEWORDINLINE(0x303C, 0x0706, 0xA816);


/*
 *  AECountItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AECountItems(
  const AEDescList *  theAEDescList,
  long *              theCount)                               THREEWORDINLINE(0x303C, 0x0407, 0xA816);


/*
 *  AEPutPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEPutPtr(
  AEDescList *  theAEDescList,
  long          index,
  DescType      typeCode,
  const void *  dataPtr,
  Size          dataSize)                                     THREEWORDINLINE(0x303C, 0x0A08, 0xA816);


/*
 *  AEPutDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEPutDesc(
  AEDescList *    theAEDescList,
  long            index,
  const AEDesc *  theAEDesc)                                  THREEWORDINLINE(0x303C, 0x0609, 0xA816);


/*
 *  AEGetNthPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEGetNthPtr(
  const AEDescList *  theAEDescList,
  long                index,
  DescType            desiredType,
  AEKeyword *         theAEKeyword,
  DescType *          typeCode,
  void *              dataPtr,
  Size                maximumSize,
  Size *              actualSize)                             THREEWORDINLINE(0x303C, 0x100A, 0xA816);


/*
 *  AEGetNthDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEGetNthDesc(
  const AEDescList *  theAEDescList,
  long                index,
  DescType            desiredType,
  AEKeyword *         theAEKeyword,
  AEDesc *            result)                                 THREEWORDINLINE(0x303C, 0x0A0B, 0xA816);


/*
 *  AESizeOfNthItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AESizeOfNthItem(
  const AEDescList *  theAEDescList,
  long                index,
  DescType *          typeCode,
  Size *              dataSize)                               THREEWORDINLINE(0x303C, 0x082A, 0xA816);


/*
 *  AEGetArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEGetArray(
  const AEDescList *   theAEDescList,
  AEArrayType          arrayType,
  AEArrayDataPointer   arrayPtr,
  Size                 maximumSize,
  DescType *           itemType,
  Size *               itemSize,
  long *               itemCount)                             THREEWORDINLINE(0x303C, 0x0D0C, 0xA816);


/*
 *  AEPutArray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEPutArray(
  AEDescList *         theAEDescList,
  AEArrayType          arrayType,
  const AEArrayData *  arrayPtr,
  DescType             itemType,
  Size                 itemSize,
  long                 itemCount)                             THREEWORDINLINE(0x303C, 0x0B0D, 0xA816);


/*
 *  AEDeleteItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEDeleteItem(
  AEDescList *  theAEDescList,
  long          index)                                        THREEWORDINLINE(0x303C, 0x040E, 0xA816);



/**************************************************************************
 The following calls apply to AERecord. Since AERecord is a subtype of
 AEDescList, the calls in the previous sections can also be used for
 AERecord an AERecord can be created by using AECreateList with isRecord
 set to true. 
**************************************************************************/
/*************************************************************************
 AERecords can have an abitrary descriptorType.  This allows you to
 check if desc is truly an AERecord
************************************************************************/
/*
 *  AECheckIsRecord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
AECheckIsRecord(const AEDesc * theDesc);


/*
  Note: The following #defines map "key" calls on AERecords into "param" calls on 
  AppleEvents.  Although no errors are currently returned if AERecords are 
  passed to "param" calls and AppleEvents to "key" calls, the behavior of 
  this type of API-mixing is not explicitly documented in Inside Macintosh.  
  It just happens that the "key" calls have the same functionality as their 
  "param" counterparts.  Since none of the "key" calls are currently available 
  in the PowerPC IntefaceLib, the #defines exploit the fact that "key" and 
  "param" routines can be used interchangeably, and makes sure that every 
  invocation of a "key" API becomes an invocation of a "param" API.
*/
#define AEPutKeyPtr(theAERecord, theAEKeyword, typeCode, dataPtr, dataSize) \
    AEPutParamPtr((theAERecord), (theAEKeyword), (typeCode), (dataPtr), (dataSize))
#define AEPutKeyDesc(theAERecord, theAEKeyword, theAEDesc) \
    AEPutParamDesc((theAERecord), (theAEKeyword), (theAEDesc))
#define AEGetKeyPtr(theAERecord, theAEKeyword, desiredType, typeCode, dataPtr, maxSize, actualSize) \
    AEGetParamPtr((theAERecord), (theAEKeyword), (desiredType), (typeCode), (dataPtr), (maxSize), (actualSize))
#define AEGetKeyDesc(theAERecord, theAEKeyword, desiredType, result) \
    AEGetParamDesc((theAERecord), (theAEKeyword), (desiredType), (result))
#define AESizeOfKeyDesc(theAERecord, theAEKeyword, typeCode, dataSize) \
    AESizeOfParam((theAERecord), (theAEKeyword), (typeCode), (dataSize))
#define AEDeleteKeyDesc(theAERecord, theAEKeyword) \
    AEDeleteParam((theAERecord), (theAEKeyword))
/**************************************************************************
  The following calls create and manipulate the AppleEvent data type.
**************************************************************************/
/*
 *  AECreateAppleEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AECreateAppleEvent(
  AEEventClass           theAEEventClass,
  AEEventID              theAEEventID,
  const AEAddressDesc *  target,
  AEReturnID             returnID,
  AETransactionID        transactionID,
  AppleEvent *           result)                              THREEWORDINLINE(0x303C, 0x0B14, 0xA816);



/**************************************************************************
  The following calls are used to pack and unpack parameters from records
  of type AppleEvent. Since AppleEvent is a subtype of AERecord, the calls
  in the previous sections can also be used for variables of type
  AppleEvent. The next six calls are in fact identical to the six calls
  for AERecord.
**************************************************************************/
/*
 *  AEPutParamPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEPutParamPtr(
  AppleEvent *  theAppleEvent,
  AEKeyword     theAEKeyword,
  DescType      typeCode,
  const void *  dataPtr,
  Size          dataSize)                                     THREEWORDINLINE(0x303C, 0x0A0F, 0xA816);


/*
 *  AEPutParamDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEPutParamDesc(
  AppleEvent *    theAppleEvent,
  AEKeyword       theAEKeyword,
  const AEDesc *  theAEDesc)                                  THREEWORDINLINE(0x303C, 0x0610, 0xA816);


/*
 *  AEGetParamPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEGetParamPtr(
  const AppleEvent *  theAppleEvent,
  AEKeyword           theAEKeyword,
  DescType            desiredType,
  DescType *          typeCode,
  void *              dataPtr,
  Size                maximumSize,
  Size *              actualSize)                             THREEWORDINLINE(0x303C, 0x0E11, 0xA816);


/*
 *  AEGetParamDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEGetParamDesc(
  const AppleEvent *  theAppleEvent,
  AEKeyword           theAEKeyword,
  DescType            desiredType,
  AEDesc *            result)                                 THREEWORDINLINE(0x303C, 0x0812, 0xA816);


/*
 *  AESizeOfParam()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AESizeOfParam(
  const AppleEvent *  theAppleEvent,
  AEKeyword           theAEKeyword,
  DescType *          typeCode,
  Size *              dataSize)                               THREEWORDINLINE(0x303C, 0x0829, 0xA816);


/*
 *  AEDeleteParam()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEDeleteParam(
  AppleEvent *  theAppleEvent,
  AEKeyword     theAEKeyword)                                 THREEWORDINLINE(0x303C, 0x0413, 0xA816);




/**************************************************************************
 The following calls also apply to type AppleEvent. Message attributes are
 far more restricted, and can only be accessed through the following 5
 calls. The various list and record routines cannot be used to access the
 attributes of an event. 
**************************************************************************/
/*
 *  AEGetAttributePtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEGetAttributePtr(
  const AppleEvent *  theAppleEvent,
  AEKeyword           theAEKeyword,
  DescType            desiredType,
  DescType *          typeCode,
  void *              dataPtr,
  Size                maximumSize,
  Size *              actualSize)                             THREEWORDINLINE(0x303C, 0x0E15, 0xA816);


/*
 *  AEGetAttributeDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEGetAttributeDesc(
  const AppleEvent *  theAppleEvent,
  AEKeyword           theAEKeyword,
  DescType            desiredType,
  AEDesc *            result)                                 THREEWORDINLINE(0x303C, 0x0826, 0xA816);


/*
 *  AESizeOfAttribute()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AESizeOfAttribute(
  const AppleEvent *  theAppleEvent,
  AEKeyword           theAEKeyword,
  DescType *          typeCode,
  Size *              dataSize)                               THREEWORDINLINE(0x303C, 0x0828, 0xA816);


/*
 *  AEPutAttributePtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEPutAttributePtr(
  AppleEvent *  theAppleEvent,
  AEKeyword     theAEKeyword,
  DescType      typeCode,
  const void *  dataPtr,
  Size          dataSize)                                     THREEWORDINLINE(0x303C, 0x0A16, 0xA816);


/*
 *  AEPutAttributeDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEPutAttributeDesc(
  AppleEvent *    theAppleEvent,
  AEKeyword       theAEKeyword,
  const AEDesc *  theAEDesc)                                  THREEWORDINLINE(0x303C, 0x0627, 0xA816);



/**************************************************************************
 AppleEvent Serialization Support

    AESizeOfFlattenedDesc, AEFlattenDesc, AEUnflattenDesc
    
    These calls will work for all AppleEvent data types and between different
    versions of the OS (including between Mac OS 9 and X)
    
    Basic types, AEDesc, AEList and AERecord are OK, but AppleEvent records
    themselves may not be reliably flattened for storage.
**************************************************************************/
/*
   AEFlattenDesc
   Returns the amount of buffer space needed to flatten the
   AEDesc. Call this before AEFlattenDesc to make sure your
   buffer has enough room for the operation.
*/

/*
 *  AESizeOfFlattenedDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Size )
AESizeOfFlattenedDesc(const AEDesc * theAEDesc);


/*
   AEFlattenDesc
   Fills a buffer with a flattened representation of the
   AEDesc and returns the amount of buffer used in actualSize.
   If bufferSize was too small it returns errAEBufferTooSmall
   (-1741) and does not fill in any of the buffer. The resulting
   buffer is only useful with an AEUnflattenDesc call.
   
   Note: if you pass a NULL buffer pointer it returns noErr but
   fills in the actualSize field anyway.
*/

/*
 *  AEFlattenDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEFlattenDesc(
  const AEDesc *  theAEDesc,
  Ptr             buffer,
  Size            bufferSize,
  Size *          actualSize);


/*
   AEUnflattenDesc
   Allocates an AEDesc (given a Null Desc) given a flattened
   data buffer. It assumes it was given a good buffer filled
   in by AEFlattenDesc. It returns paramErr if it discovers
   something fishy about the buffer.
*/

/*
 *  AEUnflattenDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
AEUnflattenDesc(
  Ptr       buffer,
  AEDesc *  result);


/**************************************************************************
 The following calls are necessary to deal with opaque data in AEDescs, because the
 traditional way of dealing with a basic AEDesc has been to dereference the dataHandle
 directly.  This is not supported under Carbon.
**************************************************************************/
#if ACCESSOR_CALLS_ARE_FUNCTIONS
/*
        AEGetDescData no longer supports automatic coercion. If you'd like to
        coerce the descriptor use AECoerceDesc.
    */
/*
 *  AEGetDescData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEGetDescData(
  const AEDesc *  theAEDesc,
  void *          dataPtr,
  Size            maximumSize);


/*
 *  AEGetDescDataSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Size )
AEGetDescDataSize(const AEDesc * theAEDesc);


/*
 *  AEReplaceDescData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AEReplaceDescData(
  DescType      typeCode,
  const void *  dataPtr,
  Size          dataSize,
  AEDesc *      theAEDesc);


#endif  /* ACCESSOR_CALLS_ARE_FUNCTIONS */

/*
 * Retrieve a range of bytes from an AEDesc.  This obviates the need
 * to retrieve the entire data from the event using AEGetDescData.
 * This is only valid for data type AEDescs.  If the requested length
 * and offset are such that they do not fit entirely with the data of the
 * desc, errAEBufferTooSmall is returned.
 */
/*
 *  AEGetDescDataRange()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
AEGetDescDataRange(
  const AEDesc *  dataDesc,
  void *          buffer,
  Size            offset,
  Size            length);


/**************************************************************************
  A AEEventHandler is installed to process an AppleEvent 
**************************************************************************/
typedef CALLBACK_API( OSErr , AEEventHandlerProcPtr )(const AppleEvent *theAppleEvent, AppleEvent *reply, long handlerRefcon);
typedef STACK_UPP_TYPE(AEEventHandlerProcPtr)                   AEEventHandlerUPP;
/*
 *  NewAEDisposeExternalUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( AEDisposeExternalUPP )
NewAEDisposeExternalUPP(AEDisposeExternalProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppAEDisposeExternalProcInfo = 0x00000FC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(AEDisposeExternalUPP) NewAEDisposeExternalUPP(AEDisposeExternalProcPtr userRoutine) { return (AEDisposeExternalUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAEDisposeExternalProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewAEDisposeExternalUPP(userRoutine) (AEDisposeExternalUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAEDisposeExternalProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewAEEventHandlerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AEEventHandlerUPP )
NewAEEventHandlerUPP(AEEventHandlerProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppAEEventHandlerProcInfo = 0x00000FE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(AEEventHandlerUPP) NewAEEventHandlerUPP(AEEventHandlerProcPtr userRoutine) { return (AEEventHandlerUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAEEventHandlerProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewAEEventHandlerUPP(userRoutine) (AEEventHandlerUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppAEEventHandlerProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeAEDisposeExternalUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
DisposeAEDisposeExternalUPP(AEDisposeExternalUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeAEDisposeExternalUPP(AEDisposeExternalUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeAEDisposeExternalUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeAEEventHandlerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeAEEventHandlerUPP(AEEventHandlerUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeAEEventHandlerUPP(AEEventHandlerUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeAEEventHandlerUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeAEDisposeExternalUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
InvokeAEDisposeExternalUPP(
  const void *          dataPtr,
  Size                  dataLength,
  long                  refcon,
  AEDisposeExternalUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeAEDisposeExternalUPP(const void * dataPtr, Size dataLength, long refcon, AEDisposeExternalUPP userUPP) { CALL_THREE_PARAMETER_UPP(userUPP, uppAEDisposeExternalProcInfo, dataPtr, dataLength, refcon); }
  #else
    #define InvokeAEDisposeExternalUPP(dataPtr, dataLength, refcon, userUPP) CALL_THREE_PARAMETER_UPP((userUPP), uppAEDisposeExternalProcInfo, (dataPtr), (dataLength), (refcon))
  #endif
#endif

/*
 *  InvokeAEEventHandlerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeAEEventHandlerUPP(
  const AppleEvent *  theAppleEvent,
  AppleEvent *        reply,
  long                handlerRefcon,
  AEEventHandlerUPP   userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeAEEventHandlerUPP(const AppleEvent * theAppleEvent, AppleEvent * reply, long handlerRefcon, AEEventHandlerUPP userUPP) { return (OSErr)CALL_THREE_PARAMETER_UPP(userUPP, uppAEEventHandlerProcInfo, theAppleEvent, reply, handlerRefcon); }
  #else
    #define InvokeAEEventHandlerUPP(theAppleEvent, reply, handlerRefcon, userUPP) (OSErr)CALL_THREE_PARAMETER_UPP((userUPP), uppAEEventHandlerProcInfo, (theAppleEvent), (reply), (handlerRefcon))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewAEDisposeExternalProc(userRoutine)               NewAEDisposeExternalUPP(userRoutine)
    #define NewAEEventHandlerProc(userRoutine)                  NewAEEventHandlerUPP(userRoutine)
    #define CallAEDisposeExternalProc(userRoutine, dataPtr, dataLength, refcon) InvokeAEDisposeExternalUPP(dataPtr, dataLength, refcon, userRoutine)
    #define CallAEEventHandlerProc(userRoutine, theAppleEvent, reply, handlerRefcon) InvokeAEEventHandlerUPP(theAppleEvent, reply, handlerRefcon, userRoutine)
#endif /* CALL_NOT_IN_CARBON */



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

#endif /* __AEDATAMODEL__ */

