/*
     File:       Dictionary.h
 
     Contains:   Dictionary Manager Interfaces
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1992-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __DICTIONARY__
#define __DICTIONARY__

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

#ifndef __AEREGISTRY__
#include <AERegistry.h>
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
    #pragma options align=power
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif

/*
=============================================================================================
 Modern Dictionary Manager
=============================================================================================
*/
/*
    Dictionary information
*/
enum {
  kDictionaryFileType           = FOUR_CHAR_CODE('dict'),
  kDCMDictionaryHeaderSignature = FOUR_CHAR_CODE('dict'),
  kDCMDictionaryHeaderVersion   = 2
};

enum {
  kDCMAnyFieldTag               = typeWildCard,
  kDCMAnyFieldType              = typeWildCard
};

/*
    Contents of a Field Info Record (an AERecord)
*/
enum {
  keyDCMFieldTag                = FOUR_CHAR_CODE('ftag'), /* typeEnumeration */
  keyDCMFieldType               = FOUR_CHAR_CODE('ftyp'), /* typeEnumeration */
  keyDCMMaxRecordSize           = FOUR_CHAR_CODE('mrsz'), /* typeMagnitude */
  keyDCMFieldAttributes         = FOUR_CHAR_CODE('fatr'),
  keyDCMFieldDefaultData        = FOUR_CHAR_CODE('fdef'),
  keyDCMFieldName               = FOUR_CHAR_CODE('fnam'), /* typeChar */
  keyDCMFieldFindMethods        = FOUR_CHAR_CODE('ffnd') /* typeAEList of typeDCMFindMethod */
};

/*
    Special types for fields of a Field Info Record
*/
enum {
  typeDCMFieldAttributes        = FOUR_CHAR_CODE('fatr'),
  typeDCMFindMethod             = FOUR_CHAR_CODE('fmth')
};


/*
    Field attributes
*/
enum {
  kDCMIndexedFieldMask          = 0x00000001,
  kDCMRequiredFieldMask         = 0x00000002,
  kDCMIdentifyFieldMask         = 0x00000004,
  kDCMFixedSizeFieldMask        = 0x00000008,
  kDCMHiddenFieldMask           = (long)0x80000000
};

typedef OptionBits                      DCMFieldAttributes;
/*
    Standard dictionary properties
*/
enum {
  pDCMAccessMethod              = FOUR_CHAR_CODE('amtd'), /* data type: typeChar ReadOnly */
  pDCMPermission                = FOUR_CHAR_CODE('perm'), /* data type: typeUInt16 */
  pDCMListing                   = FOUR_CHAR_CODE('list'), /* data type: typeUInt16 */
  pDCMMaintenance               = FOUR_CHAR_CODE('mtnc'), /* data type: typeUInt16 */
  pDCMLocale                    = FOUR_CHAR_CODE('locl'), /* data type: typeUInt32.  Optional; default = kLocaleIdentifierWildCard */
  pDCMClass                     = pClass, /* data type: typeUInt16 */
  pDCMCopyright                 = FOUR_CHAR_CODE('info') /* data type: typeChar */
};

/*
    pDCMPermission property constants
*/
enum {
  kDCMReadOnlyDictionary        = 0,
  kDCMReadWriteDictionary       = 1
};

/*
    pDCMListing property constants
*/
enum {
  kDCMAllowListing              = 0,
  kDCMProhibitListing           = 1
};

/*
    pDCMClass property constants
*/
enum {
  kDCMUserDictionaryClass       = 0,
  kDCMSpecificDictionaryClass   = 1,
  kDCMBasicDictionaryClass      = 2
};

/*
    Standard search method
*/
enum {
  kDCMFindMethodExactMatch      = kAEEquals,
  kDCMFindMethodBeginningMatch  = kAEBeginsWith,
  kDCMFindMethodContainsMatch   = kAEContains,
  kDCMFindMethodEndingMatch     = kAEEndsWith,
  kDCMFindMethodForwardTrie     = FOUR_CHAR_CODE('ftri'), /* used for morphological analysis*/
  kDCMFindMethodBackwardTrie    = FOUR_CHAR_CODE('btri') /* used for morphological analysis*/
};

typedef OSType                          DCMFindMethod;
/*
    AccessMethod features
*/
enum {
  kDCMCanUseFileDictionaryMask  = 0x00000001,
  kDCMCanUseMemoryDictionaryMask = 0x00000002,
  kDCMCanStreamDictionaryMask   = 0x00000004,
  kDCMCanHaveMultipleIndexMask  = 0x00000008,
  kDCMCanModifyDictionaryMask   = 0x00000010,
  kDCMCanCreateDictionaryMask   = 0x00000020,
  kDCMCanAddDictionaryFieldMask = 0x00000040,
  kDCMCanUseTransactionMask     = 0x00000080
};

typedef OptionBits                      DCMAccessMethodFeature;
typedef UInt32                          DCMUniqueID;
typedef struct OpaqueDCMObjectID*       DCMObjectID;
typedef DCMObjectID                     DCMAccessMethodID;
typedef DCMObjectID                     DCMDictionaryID;
#define kDCMInvalidObjectID ((DCMObjectID) kInvalidID)
typedef struct OpaqueDCMObjectRef*      DCMObjectRef;
typedef DCMObjectRef                    DCMDictionaryRef;
typedef DCMObjectRef                    DCMDictionaryStreamRef;
#define kDCMInvalidObjectRef ((DCMObjectRef) kInvalidID)
typedef struct OpaqueDCMObjectIterator*  DCMObjectIterator;
typedef DCMObjectIterator               DCMAccessMethodIterator;
typedef DCMObjectIterator               DCMDictionaryIterator;
typedef struct OpaqueDCMFoundRecordIterator*  DCMFoundRecordIterator;
/*
    Field specification declarations
*/
typedef DescType                        DCMFieldTag;
typedef DescType                        DCMFieldType;
/*
    Dictionary header information
*/
struct DCMDictionaryHeader {
  FourCharCode        headerSignature;
  UInt32              headerVersion;
  ByteCount           headerSize;
  Str63               accessMethod;
};
typedef struct DCMDictionaryHeader      DCMDictionaryHeader;
/*
    Callback routines
*/
typedef CALLBACK_API( Boolean , DCMProgressFilterProcPtr )(Boolean determinateProcess, UInt16 percentageComplete, UInt32 callbackUD);
typedef STACK_UPP_TYPE(DCMProgressFilterProcPtr)                DCMProgressFilterUPP;
#if CALL_NOT_IN_CARBON
/*
 *  NewDCMProgressFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( DCMProgressFilterUPP )
NewDCMProgressFilterUPP(DCMProgressFilterProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDCMProgressFilterProcInfo = 0x00000E50 };  /* pascal 1_byte Func(1_byte, 2_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DCMProgressFilterUPP) NewDCMProgressFilterUPP(DCMProgressFilterProcPtr userRoutine) { return (DCMProgressFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDCMProgressFilterProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDCMProgressFilterUPP(userRoutine) (DCMProgressFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDCMProgressFilterProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeDCMProgressFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeDCMProgressFilterUPP(DCMProgressFilterUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDCMProgressFilterUPP(DCMProgressFilterUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDCMProgressFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeDCMProgressFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
InvokeDCMProgressFilterUPP(
  Boolean               determinateProcess,
  UInt16                percentageComplete,
  UInt32                callbackUD,
  DCMProgressFilterUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeDCMProgressFilterUPP(Boolean determinateProcess, UInt16 percentageComplete, UInt32 callbackUD, DCMProgressFilterUPP userUPP) { return (Boolean)CALL_THREE_PARAMETER_UPP(userUPP, uppDCMProgressFilterProcInfo, determinateProcess, percentageComplete, callbackUD); }
  #else
    #define InvokeDCMProgressFilterUPP(determinateProcess, percentageComplete, callbackUD, userUPP) (Boolean)CALL_THREE_PARAMETER_UPP((userUPP), uppDCMProgressFilterProcInfo, (determinateProcess), (percentageComplete), (callbackUD))
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewDCMProgressFilterProc(userRoutine)               NewDCMProgressFilterUPP(userRoutine)
    #define CallDCMProgressFilterProc(userRoutine, determinateProcess, percentageComplete, callbackUD) InvokeDCMProgressFilterUPP(determinateProcess, percentageComplete, callbackUD, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*
    Library version
*/
/*
 *  DCMLibraryVersion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt32 )
DCMLibraryVersion(void);


/*
    Create/delete dictionary
*/
/*
 *  DCMNewDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMNewDictionary(
  DCMAccessMethodID   accessMethodID,
  const FSSpec *      newDictionaryFile,
  ScriptCode          scriptTag,
  const AEDesc *      listOfFieldInfoRecords,
  Boolean             invisible,
  ItemCount           recordCapacity,
  DCMDictionaryID *   newDictionary);


/*
 *  DCMDeriveNewDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMDeriveNewDictionary(
  DCMDictionaryID    srcDictionary,
  const FSSpec *     newDictionaryFile,
  ScriptCode         scriptTag,
  Boolean            invisible,
  ItemCount          recordCapacity,
  DCMDictionaryID *  newDictionary);


/*
 *  DCMDeleteDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMDeleteDictionary(DCMDictionaryID dictionaryID);


/*
    Register dictionary
*/
/*
 *  DCMRegisterDictionaryFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMRegisterDictionaryFile(
  const FSSpec *     dictionaryFile,
  DCMDictionaryID *  dictionaryID);


/*
 *  DCMUnregisterDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMUnregisterDictionary(DCMDictionaryID dictionaryID);


/*
    Open dictionary
*/
/*
 *  DCMOpenDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMOpenDictionary(
  DCMDictionaryID       dictionaryID,
  ByteCount             protectKeySize,
  ConstLogicalAddress   protectKey,
  DCMDictionaryRef *    dictionaryRef);


/*
 *  DCMCloseDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMCloseDictionary(DCMDictionaryRef dictionaryRef);


/*
    Change access privilege
*/
/*
 *  DCMGetDictionaryWriteAccess()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetDictionaryWriteAccess(
  DCMDictionaryRef   dictionaryRef,
  Duration           timeOutDuration);


/*
 *  DCMReleaseDictionaryWriteAccess()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMReleaseDictionaryWriteAccess(
  DCMDictionaryRef   dictionaryRef,
  Boolean            commitTransaction);


/*
    Find records
*/
/*
 *  DCMFindRecords()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMFindRecords(
  DCMDictionaryRef          dictionaryRef,
  DCMFieldTag               keyFieldTag,
  ByteCount                 keySize,
  ConstLogicalAddress       keyData,
  DCMFindMethod             findMethod,
  ItemCount                 preFetchedDataNum,
  DCMFieldTag               preFetchedData[],
  ItemCount                 skipCount,
  ItemCount                 maxRecordCount,
  DCMFoundRecordIterator *  recordIterator);


/*
 *  DCMCountRecordIterator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ItemCount )
DCMCountRecordIterator(DCMFoundRecordIterator recordIterator);


/*
 *  DCMIterateFoundRecord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMIterateFoundRecord(
  DCMFoundRecordIterator   recordIterator,
  ByteCount                maxKeySize,
  ByteCount *              actualKeySize,
  LogicalAddress           keyData,
  DCMUniqueID *            uniqueID,
  AEDesc *                 dataList);


/*
 *  DCMDisposeRecordIterator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMDisposeRecordIterator(DCMFoundRecordIterator recordIterator);


/*
    Dump dictionary
*/
/*
 *  DCMCountRecord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMCountRecord(
  DCMDictionaryID   dictionaryID,
  ItemCount *       count);


/*
 *  DCMGetRecordSequenceNumber()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetRecordSequenceNumber(
  DCMDictionaryRef      dictionaryRef,
  DCMFieldTag           keyFieldTag,
  ByteCount             keySize,
  ConstLogicalAddress   keyData,
  DCMUniqueID           uniqueID,
  ItemCount *           sequenceNum);


/*
 *  DCMGetNthRecord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetNthRecord(
  DCMDictionaryRef   dictionaryRef,
  DCMFieldTag        keyFieldTag,
  ItemCount          serialNum,
  ByteCount          maxKeySize,
  ByteCount *        keySize,
  LogicalAddress     keyData,
  DCMUniqueID *      uniqueID);


/*
 *  DCMGetNextRecord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetNextRecord(
  DCMDictionaryRef      dictionaryRef,
  DCMFieldTag           keyFieldTag,
  ByteCount             keySize,
  ConstLogicalAddress   keyData,
  DCMUniqueID           uniqueID,
  ByteCount             maxKeySize,
  ByteCount *           nextKeySize,
  LogicalAddress        nextKeyData,
  DCMUniqueID *         nextUniqueID);


/*
 *  DCMGetPrevRecord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetPrevRecord(
  DCMDictionaryRef      dictionaryRef,
  DCMFieldTag           keyFieldTag,
  ByteCount             keySize,
  ConstLogicalAddress   keyData,
  DCMUniqueID           uniqueID,
  ByteCount             maxKeySize,
  ByteCount *           prevKeySize,
  LogicalAddress        prevKeyData,
  DCMUniqueID *         prevUniqueID);


/*
    Get field data
*/
/*
 *  DCMGetFieldData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetFieldData(
  DCMDictionaryRef      dictionaryRef,
  DCMFieldTag           keyFieldTag,
  ByteCount             keySize,
  ConstLogicalAddress   keyData,
  DCMUniqueID           uniqueID,
  ItemCount             numOfData,
  const DCMFieldTag     dataTag[],
  AEDesc *              dataList);


/*
 *  DCMSetFieldData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMSetFieldData(
  DCMDictionaryRef      dictionaryRef,
  DCMFieldTag           keyFieldTag,
  ByteCount             keySize,
  ConstLogicalAddress   keyData,
  DCMUniqueID           uniqueID,
  const AEDesc *        dataList);


/*
    Add record
*/
/*
 *  DCMAddRecord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMAddRecord(
  DCMDictionaryRef      dictionaryRef,
  DCMFieldTag           keyFieldTag,
  ByteCount             keySize,
  ConstLogicalAddress   keyData,
  Boolean               checkOnly,
  const AEDesc *        dataList,
  DCMUniqueID *         newUniqueID);


/*
 *  DCMDeleteRecord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMDeleteRecord(
  DCMDictionaryRef      dictionaryRef,
  DCMFieldTag           keyFieldTag,
  ByteCount             keySize,
  ConstLogicalAddress   keyData,
  DCMUniqueID           uniqueID);


/*
    Reorganize/compact dictionary
*/
/*
 *  DCMReorganizeDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMReorganizeDictionary(
  DCMDictionaryID        dictionaryID,
  ItemCount              extraCapacity,
  DCMProgressFilterUPP   progressProc,
  UInt32                 userData);


/*
 *  DCMCompactDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMCompactDictionary(
  DCMDictionaryID        dictionaryID,
  DCMProgressFilterUPP   progressProc,
  UInt32                 userData);


/*
    DictionaryID utilities
*/
/*
 *  DCMGetFileFromDictionaryID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetFileFromDictionaryID(
  DCMDictionaryID   dictionaryID,
  FSSpec *          fileRef);


/*
 *  DCMGetDictionaryIDFromFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetDictionaryIDFromFile(
  const FSSpec *     fileRef,
  DCMDictionaryID *  dictionaryID);


/*
 *  DCMGetDictionaryIDFromRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( DCMDictionaryID )
DCMGetDictionaryIDFromRef(DCMDictionaryRef dictionaryRef);


/*
    Field information and manipulation
*/
/*
 *  DCMGetDictionaryFieldInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetDictionaryFieldInfo(
  DCMDictionaryID   dictionaryID,
  DCMFieldTag       fieldTag,
  AEDesc *          fieldInfoRecord);


/*
    Dictionary property
*/
/*
 *  DCMGetDictionaryProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetDictionaryProperty(
  DCMDictionaryID   dictionaryID,
  DCMFieldTag       propertyTag,
  ByteCount         maxPropertySize,
  ByteCount *       actualSize,
  LogicalAddress    propertyValue);


/*
 *  DCMSetDictionaryProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMSetDictionaryProperty(
  DCMDictionaryID       dictionaryID,
  DCMFieldTag           propertyTag,
  ByteCount             propertySize,
  ConstLogicalAddress   propertyValue);


/*
 *  DCMGetDictionaryPropertyList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetDictionaryPropertyList(
  DCMDictionaryID   dictionaryID,
  ItemCount         maxPropertyNum,
  ItemCount *       numProperties,
  DCMFieldTag       propertyTag[]);


/*
    Seaarch dictionary
*/
/*
 *  DCMCreateDictionaryIterator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMCreateDictionaryIterator(DCMDictionaryIterator * dictionaryIterator);


/*
    Search AccessMethod
*/
/*
 *  DCMCreateAccessMethodIterator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMCreateAccessMethodIterator(DCMAccessMethodIterator * accessMethodIterator);


/*
    Iterator Operation
*/
/*
 *  DCMCountObjectIterator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ItemCount )
DCMCountObjectIterator(DCMObjectIterator iterator);


/*
 *  DCMIterateObject()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMIterateObject(
  DCMObjectIterator   iterator,
  DCMObjectID *       objectID);


/*
 *  DCMResetObjectIterator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMResetObjectIterator(DCMObjectIterator iterator);


/*
 *  DCMDisposeObjectIterator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMDisposeObjectIterator(DCMObjectIterator iterator);


/*
    Get AccessMethod information
*/
/*
 *  DCMGetAccessMethodIDFromName()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetAccessMethodIDFromName(
  ConstStr63Param      accessMethodName,
  DCMAccessMethodID *  accessMethodID);


/*
    Field Info Record routines
*/
/*
 *  DCMCreateFieldInfoRecord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMCreateFieldInfoRecord(
  DescType             fieldTag,
  DescType             fieldType,
  ByteCount            maxRecordSize,
  DCMFieldAttributes   fieldAttributes,
  AEDesc *             fieldDefaultData,
  ItemCount            numberOfFindMethods,
  DCMFindMethod        findMethods[],
  AEDesc *             fieldInfoRecord);


/*
 *  DCMGetFieldTagAndType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetFieldTagAndType(
  const AEDesc *  fieldInfoRecord,
  DCMFieldTag *   fieldTag,
  DCMFieldType *  fieldType);


/*
 *  DCMGetFieldMaxRecordSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetFieldMaxRecordSize(
  const AEDesc *  fieldInfoRecord,
  ByteCount *     maxRecordSize);


/*
 *  DCMGetFieldAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetFieldAttributes(
  const AEDesc *        fieldInfoRecord,
  DCMFieldAttributes *  attributes);


/*
 *  DCMGetFieldDefaultData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetFieldDefaultData(
  const AEDesc *  fieldInfoRecord,
  DescType        desiredType,
  AEDesc *        fieldDefaultData);


/*
 *  DCMGetFieldFindMethods()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DictionaryMgrLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DCMGetFieldFindMethods(
  const AEDesc *  fieldInfoRecord,
  ItemCount       findMethodsArrayMaxSize,
  DCMFindMethod   findMethods[],
  ItemCount *     actualNumberOfFindMethods);


/*
    Check Dictionary Manager availability
*/
#if TARGET_RT_MAC_CFM
#ifdef __cplusplus
    inline pascal Boolean DCMDictionaryManagerAvailable() { return (DCMLibraryVersion != (void*)kUnresolvedCFragSymbolAddress); }
#else
    #define DCMDictionaryManagerAvailable()     ((DCMLibraryVersion != (void*)kUnresolvedCFragSymbolAddress)
#endif
#elif TARGET_RT_MAC_MACHO
/* Dictionary Manager is always available on OS X */
#ifdef __cplusplus
    inline pascal Boolean DCMDictionaryManagerAvailable() { return true; }
#else
    #define DCMDictionaryManagerAvailable()     (true)
#endif
#endif  /*  */

/*
=============================================================================================
    Definitions for Japanese Analysis Module
=============================================================================================
*/
/*
    Default dictionary access method for Japanese analysis
*/
#define kAppleJapaneseDefaultAccessMethodName   "\pDAM:Apple Backward Trie Access Method"
/*
    Data length limitations of Apple Japanese dictionaries
*/
enum {
  kMaxYomiLengthInAppleJapaneseDictionary = 40,
  kMaxKanjiLengthInAppleJapaneseDictionary = 64
};

/*
    Defined field tags of Apple Japanese dictionary
*/
enum {
  kDCMJapaneseYomiTag           = FOUR_CHAR_CODE('yomi'),
  kDCMJapaneseHyokiTag          = FOUR_CHAR_CODE('hyok'),
  kDCMJapaneseHinshiTag         = FOUR_CHAR_CODE('hins'),
  kDCMJapaneseWeightTag         = FOUR_CHAR_CODE('hind'),
  kDCMJapanesePhoneticTag       = FOUR_CHAR_CODE('hton'),
  kDCMJapaneseAccentTag         = FOUR_CHAR_CODE('acnt'),
  kDCMJapaneseOnKunReadingTag   = FOUR_CHAR_CODE('OnKn'),
  kDCMJapaneseFukugouInfoTag    = FOUR_CHAR_CODE('fuku')
};

enum {
  kDCMJapaneseYomiType          = typeUnicodeText,
  kDCMJapaneseHyokiType         = typeUnicodeText,
  kDCMJapaneseHinshiType        = FOUR_CHAR_CODE('hins'),
  kDCMJapaneseWeightType        = typeShortInteger,
  kDCMJapanesePhoneticType      = typeUnicodeText,
  kDCMJapaneseAccentType        = FOUR_CHAR_CODE('byte'),
  kDCMJapaneseOnKunReadingType  = typeUnicodeText,
  kDCMJapaneseFukugouInfoType   = FOUR_CHAR_CODE('fuku')
};


/*
=============================================================================================
 System 7 Dictionary Manager
=============================================================================================
*/

#if PRAGMA_STRUCT_ALIGN
#pragma options align=reset
#pragma options align=mac68k
#endif

enum {
                                        /* Dictionary data insertion modes */
  kInsert                       = 0,    /* Only insert the input entry if there is nothing in the dictionary that matches the key. */
  kReplace                      = 1,    /* Only replace the entries which match the key with the input entry. */
  kInsertOrReplace              = 2     /* Insert the entry if there is nothing in the dictionary which matches the key, otherwise replaces the existing matched entries with the input entry. */
};

/* This Was InsertMode */

typedef short                           DictionaryDataInsertMode;
enum {
                                        /* Key attribute constants */
  kIsCaseSensitive              = 0x10, /* case sensitive = 16       */
  kIsNotDiacriticalSensitive    = 0x20  /* diac not sensitive = 32    */
};

enum {
                                        /* Registered attribute type constants.   */
  kNoun                         = -1,
  kVerb                         = -2,
  kAdjective                    = -3,
  kAdverb                       = -4
};

/* This Was AttributeType */
typedef SInt8                           DictionaryEntryAttribute;
/* Dictionary information record */
struct DictionaryInformation {
  FSSpec              dictionaryFSSpec;
  SInt32              numberOfRecords;
  SInt32              currentGarbageSize;
  ScriptCode          script;
  SInt16              maximumKeyLength;
  SInt8               keyAttributes;
};
typedef struct DictionaryInformation    DictionaryInformation;
struct DictionaryAttributeTable {
  UInt8               datSize;
  DictionaryEntryAttribute  datTable[1];
};
typedef struct DictionaryAttributeTable DictionaryAttributeTable;
typedef DictionaryAttributeTable *      DictionaryAttributeTablePtr;
#if CALL_NOT_IN_CARBON
/*
 *  InitializeDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
InitializeDictionary(
  const FSSpec *  theFsspecPtr,
  SInt16          maximumKeyLength,
  SInt8           keyAttributes,
  ScriptCode      script)                                     THREEWORDINLINE(0x303C, 0x0500, 0xAA53);


/*
 *  OpenDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
OpenDictionary(
  const FSSpec *  theFsspecPtr,
  SInt8           accessPermission,
  SInt32 *        dictionaryReference)                        THREEWORDINLINE(0x303C, 0x0501, 0xAA53);


/*
 *  CloseDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
CloseDictionary(SInt32 dictionaryReference)                   THREEWORDINLINE(0x303C, 0x0202, 0xAA53);


/*
 *  InsertRecordToDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
InsertRecordToDictionary(
  SInt32                     dictionaryReference,
  ConstStr255Param           key,
  Handle                     recordDataHandle,
  DictionaryDataInsertMode   whichMode)                       THREEWORDINLINE(0x303C, 0x0703, 0xAA53);


/*
 *  DeleteRecordFromDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
DeleteRecordFromDictionary(
  SInt32             dictionaryReference,
  ConstStr255Param   key)                                     THREEWORDINLINE(0x303C, 0x0404, 0xAA53);


/*
 *  FindRecordInDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
FindRecordInDictionary(
  SInt32                        dictionaryReference,
  ConstStr255Param              key,
  DictionaryAttributeTablePtr   requestedAttributeTablePointer,
  Handle                        recordDataHandle)             THREEWORDINLINE(0x303C, 0x0805, 0xAA53);


/*
 *  FindRecordByIndexInDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
FindRecordByIndexInDictionary(
  SInt32                        dictionaryReference,
  SInt32                        recordIndex,
  DictionaryAttributeTablePtr   requestedAttributeTablePointer,
  Str255                        recordKey,
  Handle                        recordDataHandle)             THREEWORDINLINE(0x303C, 0x0A06, 0xAA53);


/*
 *  GetDictionaryInformation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
GetDictionaryInformation(
  SInt32                   dictionaryReference,
  DictionaryInformation *  theDictionaryInformation)          THREEWORDINLINE(0x303C, 0x0407, 0xAA53);


/*
 *  CompactDictionary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
CompactDictionary(SInt32 dictionaryReference)                 THREEWORDINLINE(0x303C, 0x0208, 0xAA53);





#endif  /* CALL_NOT_IN_CARBON */


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

#endif /* __DICTIONARY__ */

