/*
     File:       FindByContent.h
 
     Contains:   Public search interface for the Find by Content shared library
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1997-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __FINDBYCONTENT__
#define __FINDBYCONTENT__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
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
   ***************************************************************************
   Language constants used with FBCIndexItemsInLanguages: these numbers are bits
   in a 64-bit array that consists of two UInt32 words.  In the current implementation
   the low word is always 0, so values for the high word are given.  If both UInt32
   words are 0, the default value of kDefaultLanguagesHighWord is used.
   ***************************************************************************
*/

/* These are the new names for the language constants*/
enum {
                                        /* languages that use the Roman character mapping*/
  kFBCenglishHighWord           = (long)0x80000000,
  kFBCdutchHighWord             = 0x40000000, /* also Afrikaans*/
  kFBCgermanHighWord            = 0x20000000,
  kFBCswedishHighWord           = 0x10000000, /* also Norwegian*/
  kFBCdanishHighWord            = 0x08000000,
  kFBCspanishHighWord           = 0x04000000, /* also Catalan*/
  kFBCportugueseHighWord        = 0x02000000,
  kFBCitalianHighWord           = 0x01000000,
  kFBCfrenchHighWord            = 0x00800000,
  kFBCromanHighWord             = 0x00400000, /* other languages using Roman alphabet*/
                                        /* Languages that use other mappings*/
  kFBCicelandicHighWord         = 0x00200000, /* also Faroese*/
  kFBChebrewHighWord            = 0x00100000, /* also Yiddish*/
  kFBCarabicHighWord            = 0x00080000, /* also Farsi, Urdu*/
  kFBCcenteuroHighWord          = 0x00040000, /* Central European languages not using Cyrillic*/
  kFBCcroatianHighWord          = 0x00020000,
  kFBCturkishHighWord           = 0x00010000,
  kFBCromanianHighWord          = 0x00008000,
  kFBCgreekHighWord             = 0x00004000,
  kFBCcyrillicHighWord          = 0x00002000, /* all languages using Cyrillic*/
  kFBCdevanagariHighWord        = 0x00001000,
  kFBCgujuratiHighWord          = 0x00000800,
  kFBCgurmukhiHighWord          = 0x00000400,
  kFBCjapaneseHighWord          = 0x00000200,
  kFBCkoreanHighWord            = 0x00000100,
  kFBCdefaultLanguagesHighWord  = (long)0xFF800000 /* sum of first 9*/
};

/*A new error, needs to be moved to MacErrors.h*/
enum {
  kFBCnotAllFoldersSearchable   = -30533
};


/*
   ***************************************************************************
   Phase values
   These values are passed to the client's callback function to indicate what
   the FBC code is doing.  They are meaningless in OS X.
   ***************************************************************************
*/
enum {
                                        /* indexing phases*/
  kFBCphIndexing                = 0,
  kFBCphFlushing                = 1,
  kFBCphMerging                 = 2,
  kFBCphMakingIndexAccessor     = 3,
  kFBCphCompacting              = 4,
  kFBCphIndexWaiting            = 5,    /* access phases*/
  kFBCphSearching               = 6,
  kFBCphMakingAccessAccessor    = 7,
  kFBCphAccessWaiting           = 8,    /* summarization*/
  kFBCphSummarizing             = 9,    /* indexing or access*/
  kFBCphIdle                    = 10,
  kFBCphCanceling               = 11
};

enum {
  kFBCsummarizationFailed       = -30533
};


/*
   ***************************************************************************
   Pointer types
   These point to memory allocated by the FBC shared library, and must be deallocated
   by calls that are defined below.
   ***************************************************************************
*/

/* A collection of state information for searching*/
typedef struct OpaqueFBCSearchSession*  FBCSearchSession;
/* An object containing summary information, from which summary text can be obtained*/
typedef struct OpaqueFBCSummaryRef*     FBCSummaryRef;
/* An ordinary C string (used for hit/doc terms)*/
typedef char *                          FBCWordItem;
/* An array of WordItems*/
typedef FBCWordItem *                   FBCWordList;
/*
   ***************************************************************************
   Callback function type for progress reporting and cancelation during
   searching and indexing.  The client's callback function should call
   WaitNextEvent; a "sleep" value of 1 is suggested.  If the callback function
   wants to cancel the current operation (indexing, search, or doc-terms
   retrieval) it should return true.
   ***************************************************************************
*/

typedef CALLBACK_API_C( Boolean , FBCCallbackProcPtr )(UInt16 phase, float percentDone, void *data);
typedef TVECTOR_UPP_TYPE(FBCCallbackProcPtr)                    FBCCallbackUPP;
/*
 *  NewFBCCallbackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( FBCCallbackUPP )
NewFBCCallbackUPP(FBCCallbackProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppFBCCallbackProcInfo = 0x00000F91 };  /* 1_byte Func(2_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(FBCCallbackUPP) NewFBCCallbackUPP(FBCCallbackProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewFBCCallbackUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  DisposeFBCCallbackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeFBCCallbackUPP(FBCCallbackUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeFBCCallbackUPP(FBCCallbackUPP) {}
  #else
      #define DisposeFBCCallbackUPP(userUPP)
  #endif
#endif

/*
 *  InvokeFBCCallbackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeFBCCallbackUPP(
  UInt16          phase,
  float           percentDone,
  void *          data,
  FBCCallbackUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeFBCCallbackUPP(UInt16 phase, float percentDone, void * data, FBCCallbackUPP userUPP) { return (*userUPP)(phase, percentDone, data); }
  #else
      #define InvokeFBCCallbackUPP(phase, percentDone, data, userUPP) (*userUPP)(phase, percentDone, data)
  #endif
#endif

/*
   ***************************************************************************
   Set the callback function for progress reporting and cancelation during
   searching and indexing.
   ***************************************************************************
*/

/*
 *  FBCSetSessionCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
FBCSetSessionCallback(
  FBCSearchSession   searchSession,
  FBCCallbackUPP     fn,
  void *             data);


/*      OS X DEPRECATED, use FBCSetSessionCallback*/
/*
 *  FBCSetCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
FBCSetCallback(
  FBCCallbackUPP   fn,
  void *           data);


/*
   ***************************************************************************
   Callback function type for hit testing during searching
   ***************************************************************************
*/

typedef CALLBACK_API_C( Boolean , FBCHitTestProcPtr )(const FSRef *theFile, void *data);
typedef TVECTOR_UPP_TYPE(FBCHitTestProcPtr)                     FBCHitTestUPP;
#if CALL_NOT_IN_CARBON
/*
 *  NewFBCHitTestUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( FBCHitTestUPP )
NewFBCHitTestUPP(FBCHitTestProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppFBCHitTestProcInfo = 0x000003D1 };  /* 1_byte Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(FBCHitTestUPP) NewFBCHitTestUPP(FBCHitTestProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewFBCHitTestUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  DisposeFBCHitTestUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
DisposeFBCHitTestUPP(FBCHitTestUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeFBCHitTestUPP(FBCHitTestUPP) {}
  #else
      #define DisposeFBCHitTestUPP(userUPP)
  #endif
#endif

/*
 *  InvokeFBCHitTestUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
InvokeFBCHitTestUPP(
  const FSRef *  theFile,
  void *         data,
  FBCHitTestUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeFBCHitTestUPP(const FSRef * theFile, void * data, FBCHitTestUPP userUPP) { return (*userUPP)(theFile, data); }
  #else
      #define InvokeFBCHitTestUPP(theFile, data, userUPP) (*userUPP)(theFile, data)
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

/*
   ***************************************************************************
   Set the hit-testing function for searches.
   ***************************************************************************
*/
/*
 *  FBCSetSessionHitTest()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
FBCSetSessionHitTest(
  FBCSearchSession   theSession,
  FBCHitTestUPP      fn,
  void *             data);


/*
   ***************************************************************************
   Set the amount of heap space to reserve for the client's use when FBC
   allocates memory.
   ***************************************************************************
*/

/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCSetHeapReservation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
FBCSetHeapReservation(UInt32 bytes);


/*
   ***************************************************************************
   Find out whether a volume is indexed.
   ***************************************************************************
*/

/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCVolumeIsIndexed()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
FBCVolumeIsIndexed(SInt16 theVRefNum);


/*
   ***************************************************************************
   Find out whether a volume is remote.
   ***************************************************************************
*/

/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCVolumeIsRemote()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
FBCVolumeIsRemote(SInt16 theVRefNum);


/*
   ***************************************************************************
   Find out the date & time of an index's last completed  update.
   ***************************************************************************
*/

/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCVolumeIndexTimeStamp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCVolumeIndexTimeStamp(
  SInt16    theVRefNum,
  UInt32 *  timeStamp);


/*
   ***************************************************************************
   Find out the physical size of an index.
   ***************************************************************************
*/

/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCVolumeIndexPhysicalSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCVolumeIndexPhysicalSize(
  SInt16    theVRefNum,
  UInt32 *  size);


/*
   ***************************************************************************
   Create & configure a search session
   ***************************************************************************
*/

/*
 *  FBCCreateSearchSession()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCCreateSearchSession(FBCSearchSession * searchSession);


/*
 *  FBCCloneSearchSession()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCCloneSearchSession(
  FBCSearchSession    original,
  FBCSearchSession *  clone);


/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCAddAllVolumesToSession()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCAddAllVolumesToSession(
  FBCSearchSession   theSession,
  Boolean            includeRemote);


/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCSetSessionVolumes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCSetSessionVolumes(
  FBCSearchSession   theSession,
  const SInt16       vRefNums[],
  UInt16             numVolumes);


/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCAddVolumeToSession()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCAddVolumeToSession(
  FBCSearchSession   theSession,
  SInt16             vRefNum);


/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCRemoveVolumeFromSession()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCRemoveVolumeFromSession(
  FBCSearchSession   theSession,
  SInt16             vRefNum);


/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCGetSessionVolumeCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCGetSessionVolumeCount(
  FBCSearchSession   theSession,
  UInt16 *           count);


/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCGetSessionVolumes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCGetSessionVolumes(
  FBCSearchSession   theSession,
  SInt16             vRefNums[],
  UInt16 *           numVolumes);


/*
   ***************************************************************************
   Execute a search
   ***************************************************************************
*/

/*
 *  FBCDoQuerySearch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCDoQuerySearch(
  FBCSearchSession   theSession,
  char *             queryText,
  const FSSpec       targetDirs[],
  UInt32             numTargets,
  UInt32             maxHits,
  UInt32             maxHitWords);


/*
 *  FBCDoCFStringSearch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCDoCFStringSearch(
  FBCSearchSession   theSession,
  CFStringRef        queryString,
  const FSSpec       targetDirs[],
  UInt32             numTargets,
  UInt32             maxHits,
  UInt32             maxHitWords);


/*
 *  FBCDoExampleSearch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCDoExampleSearch(
  FBCSearchSession   theSession,
  const UInt32 *     exampleHitNums,
  UInt32             numExamples,
  const FSSpec       targetDirs[],
  UInt32             numTargets,
  UInt32             maxHits,
  UInt32             maxHitWords);


/* OS X DEPRECATED, use FBCBlindExampleSearchWithCallback to be able to cancel*/
/*
 *  FBCBlindExampleSearch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCBlindExampleSearch(
  const FSSpec        examples[],
  UInt32              numExamples,
  const FSSpec        targetDirs[],
  UInt32              numTargets,
  UInt32              maxHits,
  UInt32              maxHitWords,
  Boolean             allIndexes,
  Boolean             includeRemote,
  FBCSearchSession *  theSession);


/*
 *  FBCBlindExampleSearchWithCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSErr )
FBCBlindExampleSearchWithCallback(
  const FSSpec        examples[],
  UInt32              numExamples,
  const FSSpec        targetDirs[],
  UInt32              numTargets,
  UInt32              maxHits,
  UInt32              maxHitWords,
  Boolean             allIndexes,
  Boolean             includeRemote,
  FBCSearchSession *  theSession,
  FBCCallbackUPP      callback,
  void *              callbackData,
  FBCHitTestUPP       userHitTest,
  void *              userHitTestData);



/*
   ***************************************************************************
   Get information about hits [wrapper for THitItem C++ API]
   ***************************************************************************
*/

/*
 *  FBCGetHitCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCGetHitCount(
  FBCSearchSession   theSession,
  UInt32 *           count);


/*
 *  FBCGetHitDocument()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCGetHitDocument(
  FBCSearchSession   theSession,
  UInt32             hitNumber,
  FSSpec *           theDocument);


/*
 *  FBCGetHitDocumentRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCGetHitDocumentRef(
  FBCSearchSession   theSession,
  UInt32             hitNumber,
  FSRef *            theDocument);


/*
 *  FBCGetHitScore()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCGetHitScore(
  FBCSearchSession   theSession,
  UInt32             hitNumber,
  float *            score);



/*
   ***************************************************************************
   Summarize text
   ***************************************************************************
*/

/*
 *  FBCSummarize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCSummarize(
  const void *  inBuf,
  UInt32        inLength,
  void *        outBuf,
  UInt32 *      outLength,
  UInt32 *      numSentences);


/*
 *  FBCSummarizeCFString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
FBCSummarizeCFString(
  CFStringRef    inString,
  CFStringRef *  outString,
  UInt32 *       numSentences);


/*
 *  FBCGetSummaryOfCFString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
FBCGetSummaryOfCFString(
  CFStringRef      inString,
  FBCSummaryRef *  summary);


/*
 *  FBCGetSummarySentenceCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
FBCGetSummarySentenceCount(
  FBCSummaryRef   summary,
  UInt32 *        numSentences);


/*
 *  FBCGetSummarySentences()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
FBCGetSummarySentences(
  FBCSummaryRef   summary,
  CFStringRef *   outString,
  UInt32 *        numSentences,
  Boolean         paragraphs);


/*
 *  FBCDisposeSummary()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
FBCDisposeSummary(FBCSummaryRef summary);



/*
   ***************************************************************************
   Deallocate hit lists and search sessions
   ***************************************************************************
*/

/*
 *  FBCReleaseSessionHits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCReleaseSessionHits(FBCSearchSession theSession);


/*
 *  FBCDestroySearchSession()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCDestroySearchSession(FBCSearchSession theSession);


/*
   ***************************************************************************
   Index one or more files and/or folders
   ***************************************************************************
*/

/*      OS X DEPRECATED (will be removed from OS X exports in a future release)*/
/*
 *  FBCIndexItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 9.0 and later
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCIndexItems(
  FSSpecArrayPtr   theItems,
  UInt32           itemCount);


/*
 *  FBCIndexItemsInLanguages()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCIndexItemsInLanguages(
  FSSpecArrayPtr   theItems,
  UInt32           itemCount,
  UInt32           languageHighBits,
  UInt32           languageLowBits);


/*
   ***************************************************************************
   (OS X only) Given a folder, find the folder that contains the index file
   of the given index
   ***************************************************************************
*/

/*
 *  FBCFindIndexFileFolderForFolder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCFindIndexFileFolderForFolder(
  const FSRef *  inFolder,
  FSRef *        outFolder);


/*
   ***************************************************************************
   (OS X only) Given a folder, delete the index file that indexes it
   ***************************************************************************
*/

/*
 *  FBCDeleteIndexFileForFolder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCDeleteIndexFileForFolder(const FSRef * folder);



/*
   ***************************************************************************
   The following are deprecated and obsolete for both OS X and OS 9
   ***************************************************************************
*/

/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCGetMatchedWords()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCGetMatchedWords(
  FBCSearchSession   theSession,
  UInt32             hitNumber,
  UInt32 *           wordCount,
  FBCWordList *      list);


/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCGetTopicWords()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCGetTopicWords(
  FBCSearchSession   theSession,
  UInt32             hitNumber,
  UInt32 *           wordCount,
  FBCWordList *      list);


/*      OS X DEPRECATED, NO-OP (will be removed from OS X exports in a future release)*/
/*
 *  FBCDestroyWordList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in FindByContent 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
FBCDestroyWordList(
  FBCWordList   theList,
  UInt32        wordCount);


/* These names are deprecated, use the new ones above*/
enum {
                                        /* languages that use the Roman character mapping*/
  englishHighWord               = kFBCenglishHighWord,
  dutchHighWord                 = kFBCdutchHighWord, /* also Afrikaans*/
  germanHighWord                = kFBCgermanHighWord,
  swedishHighWord               = kFBCswedishHighWord, /* also Norwegian*/
  danishHighWord                = kFBCdanishHighWord,
  spanishHighWord               = kFBCspanishHighWord, /* also Catalan*/
  portugueseHighWord            = kFBCportugueseHighWord,
  italianHighWord               = kFBCitalianHighWord,
  frenchHighWord                = kFBCfrenchHighWord,
  romanHighWord                 = kFBCromanHighWord, /* other languages using Roman alphabet*/
                                        /* Languages that use other mappings*/
  icelandicHighWord             = kFBCicelandicHighWord, /* also Faroese*/
  hebrewHighWord                = kFBChebrewHighWord, /* also Yiddish*/
  arabicHighWord                = kFBCarabicHighWord, /* also Farsi, Urdu*/
  centeuroHighWord              = kFBCcenteuroHighWord, /* Central European languages not using Cyrillic*/
  croatianHighWord              = kFBCcroatianHighWord,
  turkishHighWord               = kFBCturkishHighWord,
  romanianHighWord              = kFBCromanianHighWord,
  greekHighWord                 = kFBCgreekHighWord,
  cyrillicHighWord              = kFBCcyrillicHighWord, /* all languages using Cyrillic*/
  devanagariHighWord            = kFBCdevanagariHighWord,
  gujuratiHighWord              = kFBCgujuratiHighWord,
  gurmukhiHighWord              = kFBCgurmukhiHighWord,
  japaneseHighWord              = kFBCjapaneseHighWord,
  koreanHighWord                = kFBCkoreanHighWord,
  kDefaultLanguagesHighWord     = kFBCdefaultLanguagesHighWord /* sum of first 9*/
};


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

#endif /* __FINDBYCONTENT__ */

