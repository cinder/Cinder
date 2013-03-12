/*
     File:       Collections.h
 
     Contains:   Collection Manager Interfaces
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1989-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __COLLECTIONS__
#define __COLLECTIONS__

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

/*************/
/* Constants */
/*************/
/* Convenience constants for functions which optionally return values */
enum {
  kCollectionDontWantTag        = 0L,
  kCollectionDontWantId         = 0L,
  kCollectionDontWantSize       = 0L,
  kCollectionDontWantAttributes = 0L,
  kCollectionDontWantIndex      = 0L,
  kCollectionDontWantData       = 0L
};


/* attributes bits */
enum {
  kCollectionNoAttributes       = 0x00000000, /* no attributes bits set */
  kCollectionAllAttributes      = (long)0xFFFFFFFF, /* all attributes bits set */
  kCollectionUserAttributes     = 0x0000FFFF, /* user attributes bits */
  kCollectionDefaultAttributes  = 0x40000000 /* default attributes - unlocked, persistent */
};


/* 
    Attribute bits 0 through 15 (entire low word) are reserved for use by the application.
    Attribute bits 16 through 31 (entire high word) are reserved for use by the Collection Manager.
    Only bits 31 (kCollectionLockBit) and 30 (kCollectionPersistenceBit) currently have meaning.
*/
enum {
  kCollectionUser0Bit           = 0,
  kCollectionUser1Bit           = 1,
  kCollectionUser2Bit           = 2,
  kCollectionUser3Bit           = 3,
  kCollectionUser4Bit           = 4,
  kCollectionUser5Bit           = 5,
  kCollectionUser6Bit           = 6,
  kCollectionUser7Bit           = 7,
  kCollectionUser8Bit           = 8,
  kCollectionUser9Bit           = 9,
  kCollectionUser10Bit          = 10,
  kCollectionUser11Bit          = 11,
  kCollectionUser12Bit          = 12,
  kCollectionUser13Bit          = 13,
  kCollectionUser14Bit          = 14,
  kCollectionUser15Bit          = 15,
  kCollectionReserved0Bit       = 16,
  kCollectionReserved1Bit       = 17,
  kCollectionReserved2Bit       = 18,
  kCollectionReserved3Bit       = 19,
  kCollectionReserved4Bit       = 20,
  kCollectionReserved5Bit       = 21,
  kCollectionReserved6Bit       = 22,
  kCollectionReserved7Bit       = 23,
  kCollectionReserved8Bit       = 24,
  kCollectionReserved9Bit       = 25,
  kCollectionReserved10Bit      = 26,
  kCollectionReserved11Bit      = 27,
  kCollectionReserved12Bit      = 28,
  kCollectionReserved13Bit      = 29,
  kCollectionPersistenceBit     = 30,
  kCollectionLockBit            = 31
};


/* attribute masks */
enum {
  kCollectionUser0Mask          = 1L << kCollectionUser0Bit,
  kCollectionUser1Mask          = 1L << kCollectionUser1Bit,
  kCollectionUser2Mask          = 1L << kCollectionUser2Bit,
  kCollectionUser3Mask          = 1L << kCollectionUser3Bit,
  kCollectionUser4Mask          = 1L << kCollectionUser4Bit,
  kCollectionUser5Mask          = 1L << kCollectionUser5Bit,
  kCollectionUser6Mask          = 1L << kCollectionUser6Bit,
  kCollectionUser7Mask          = 1L << kCollectionUser7Bit,
  kCollectionUser8Mask          = 1L << kCollectionUser8Bit,
  kCollectionUser9Mask          = 1L << kCollectionUser9Bit,
  kCollectionUser10Mask         = 1L << kCollectionUser10Bit,
  kCollectionUser11Mask         = 1L << kCollectionUser11Bit,
  kCollectionUser12Mask         = 1L << kCollectionUser12Bit,
  kCollectionUser13Mask         = 1L << kCollectionUser13Bit,
  kCollectionUser14Mask         = 1L << kCollectionUser14Bit,
  kCollectionUser15Mask         = 1L << kCollectionUser15Bit,
  kCollectionReserved0Mask      = 1L << kCollectionReserved0Bit,
  kCollectionReserved1Mask      = 1L << kCollectionReserved1Bit,
  kCollectionReserved2Mask      = 1L << kCollectionReserved2Bit,
  kCollectionReserved3Mask      = 1L << kCollectionReserved3Bit,
  kCollectionReserved4Mask      = 1L << kCollectionReserved4Bit,
  kCollectionReserved5Mask      = 1L << kCollectionReserved5Bit,
  kCollectionReserved6Mask      = 1L << kCollectionReserved6Bit,
  kCollectionReserved7Mask      = 1L << kCollectionReserved7Bit,
  kCollectionReserved8Mask      = 1L << kCollectionReserved8Bit,
  kCollectionReserved9Mask      = 1L << kCollectionReserved9Bit,
  kCollectionReserved10Mask     = 1L << kCollectionReserved10Bit,
  kCollectionReserved11Mask     = 1L << kCollectionReserved11Bit,
  kCollectionReserved12Mask     = 1L << kCollectionReserved12Bit,
  kCollectionReserved13Mask     = 1L << kCollectionReserved13Bit,
  kCollectionPersistenceMask    = 1L << kCollectionPersistenceBit,
  kCollectionLockMask           = 1L << kCollectionLockBit
};


/***********/
/* Types   */
/***********/
/* abstract data type for a collection */
typedef struct OpaqueCollection*        Collection;
/* collection member 4 byte tag */
typedef FourCharCode                    CollectionTag;
typedef CALLBACK_API( OSErr , CollectionFlattenProcPtr )(SInt32 size, void *data, void *refCon);
typedef CALLBACK_API( OSErr , CollectionExceptionProcPtr )(Collection c, OSErr status);
typedef STACK_UPP_TYPE(CollectionFlattenProcPtr)                CollectionFlattenUPP;
typedef STACK_UPP_TYPE(CollectionExceptionProcPtr)              CollectionExceptionUPP;
/*
 *  NewCollectionFlattenUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CollectionFlattenUPP )
NewCollectionFlattenUPP(CollectionFlattenProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppCollectionFlattenProcInfo = 0x00000FE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(CollectionFlattenUPP) NewCollectionFlattenUPP(CollectionFlattenProcPtr userRoutine) { return (CollectionFlattenUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCollectionFlattenProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewCollectionFlattenUPP(userRoutine) (CollectionFlattenUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCollectionFlattenProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewCollectionExceptionUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CollectionExceptionUPP )
NewCollectionExceptionUPP(CollectionExceptionProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppCollectionExceptionProcInfo = 0x000002E0 };  /* pascal 2_bytes Func(4_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(CollectionExceptionUPP) NewCollectionExceptionUPP(CollectionExceptionProcPtr userRoutine) { return (CollectionExceptionUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCollectionExceptionProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewCollectionExceptionUPP(userRoutine) (CollectionExceptionUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppCollectionExceptionProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeCollectionFlattenUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeCollectionFlattenUPP(CollectionFlattenUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeCollectionFlattenUPP(CollectionFlattenUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeCollectionFlattenUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeCollectionExceptionUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeCollectionExceptionUPP(CollectionExceptionUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeCollectionExceptionUPP(CollectionExceptionUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeCollectionExceptionUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeCollectionFlattenUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeCollectionFlattenUPP(
  SInt32                size,
  void *                data,
  void *                refCon,
  CollectionFlattenUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeCollectionFlattenUPP(SInt32 size, void * data, void * refCon, CollectionFlattenUPP userUPP) { return (OSErr)CALL_THREE_PARAMETER_UPP(userUPP, uppCollectionFlattenProcInfo, size, data, refCon); }
  #else
    #define InvokeCollectionFlattenUPP(size, data, refCon, userUPP) (OSErr)CALL_THREE_PARAMETER_UPP((userUPP), uppCollectionFlattenProcInfo, (size), (data), (refCon))
  #endif
#endif

/*
 *  InvokeCollectionExceptionUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeCollectionExceptionUPP(
  Collection              c,
  OSErr                   status,
  CollectionExceptionUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeCollectionExceptionUPP(Collection c, OSErr status, CollectionExceptionUPP userUPP) { return (OSErr)CALL_TWO_PARAMETER_UPP(userUPP, uppCollectionExceptionProcInfo, c, status); }
  #else
    #define InvokeCollectionExceptionUPP(c, status, userUPP) (OSErr)CALL_TWO_PARAMETER_UPP((userUPP), uppCollectionExceptionProcInfo, (c), (status))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewCollectionFlattenProc(userRoutine)               NewCollectionFlattenUPP(userRoutine)
    #define NewCollectionExceptionProc(userRoutine)             NewCollectionExceptionUPP(userRoutine)
    #define CallCollectionFlattenProc(userRoutine, size, data, refCon) InvokeCollectionFlattenUPP(size, data, refCon, userRoutine)
    #define CallCollectionExceptionProc(userRoutine, c, status) InvokeCollectionExceptionUPP(c, status, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*********************************************/
/************* Public interfaces *************/
/*********************************************/
/*
 *  NewCollection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Collection )
NewCollection(void)                                           TWOWORDINLINE(0x7000, 0xABF6);



/*
 *  DisposeCollection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisposeCollection(Collection c)                               TWOWORDINLINE(0x7001, 0xABF6);



/*
 *  CloneCollection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Collection )
CloneCollection(Collection c)                                 TWOWORDINLINE(0x7002, 0xABF6);



/*
 *  CountCollectionOwners()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 )
CountCollectionOwners(Collection c)                           TWOWORDINLINE(0x7003, 0xABF6);



/*
 *  RetainCollection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSStatus )
RetainCollection(Collection c);



/*
 *  ReleaseCollection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSStatus )
ReleaseCollection(Collection c);



/*
 *  GetCollectionRetainCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( ItemCount )
GetCollectionRetainCount(Collection c);



/*
 *  CopyCollection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Collection )
CopyCollection(
  Collection   srcCollection,
  Collection   dstCollection)                                 TWOWORDINLINE(0x7004, 0xABF6);



/*
 *  GetCollectionDefaultAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 )
GetCollectionDefaultAttributes(Collection c)                  TWOWORDINLINE(0x7005, 0xABF6);



/*
 *  SetCollectionDefaultAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetCollectionDefaultAttributes(
  Collection   c,
  SInt32       whichAttributes,
  SInt32       newAttributes)                                 TWOWORDINLINE(0x7006, 0xABF6);



/*
 *  CountCollectionItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 )
CountCollectionItems(Collection c)                            TWOWORDINLINE(0x7007, 0xABF6);



/*
 *  AddCollectionItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AddCollectionItem(
  Collection      c,
  CollectionTag   tag,
  SInt32          id,
  SInt32          itemSize,
  const void *    itemData)                                   TWOWORDINLINE(0x7008, 0xABF6);



/*
 *  GetCollectionItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetCollectionItem(
  Collection      c,
  CollectionTag   tag,
  SInt32          id,
  SInt32 *        itemSize,
  void *          itemData)                                   TWOWORDINLINE(0x7009, 0xABF6);



/*
 *  RemoveCollectionItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
RemoveCollectionItem(
  Collection      c,
  CollectionTag   tag,
  SInt32          id)                                         TWOWORDINLINE(0x700A, 0xABF6);



/*
 *  SetCollectionItemInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetCollectionItemInfo(
  Collection      c,
  CollectionTag   tag,
  SInt32          id,
  SInt32          whichAttributes,
  SInt32          newAttributes)                              TWOWORDINLINE(0x700B, 0xABF6);



/*
 *  GetCollectionItemInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetCollectionItemInfo(
  Collection      c,
  CollectionTag   tag,
  SInt32          id,
  SInt32 *        index,
  SInt32 *        itemSize,
  SInt32 *        attributes)                                 TWOWORDINLINE(0x700C, 0xABF6);



/*
 *  ReplaceIndexedCollectionItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ReplaceIndexedCollectionItem(
  Collection    c,
  SInt32        index,
  SInt32        itemSize,
  const void *  itemData)                                     TWOWORDINLINE(0x700D, 0xABF6);



/*
 *  GetIndexedCollectionItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetIndexedCollectionItem(
  Collection   c,
  SInt32       index,
  SInt32 *     itemSize,
  void *       itemData)                                      TWOWORDINLINE(0x700E, 0xABF6);



/*
 *  RemoveIndexedCollectionItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
RemoveIndexedCollectionItem(
  Collection   c,
  SInt32       index)                                         TWOWORDINLINE(0x700F, 0xABF6);



/*
 *  SetIndexedCollectionItemInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetIndexedCollectionItemInfo(
  Collection   c,
  SInt32       index,
  SInt32       whichAttributes,
  SInt32       newAttributes)                                 TWOWORDINLINE(0x7010, 0xABF6);



/*
 *  GetIndexedCollectionItemInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetIndexedCollectionItemInfo(
  Collection       c,
  SInt32           index,
  CollectionTag *  tag,
  SInt32 *         id,
  SInt32 *         itemSize,
  SInt32 *         attributes)                                TWOWORDINLINE(0x7011, 0xABF6);



/*
 *  CollectionTagExists()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
CollectionTagExists(
  Collection      c,
  CollectionTag   tag)                                        TWOWORDINLINE(0x7012, 0xABF6);



/*
 *  CountCollectionTags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 )
CountCollectionTags(Collection c)                             TWOWORDINLINE(0x7013, 0xABF6);



/*
 *  GetIndexedCollectionTag()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetIndexedCollectionTag(
  Collection       c,
  SInt32           tagIndex,
  CollectionTag *  tag)                                       TWOWORDINLINE(0x7014, 0xABF6);



/*
 *  CountTaggedCollectionItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 )
CountTaggedCollectionItems(
  Collection      c,
  CollectionTag   tag)                                        TWOWORDINLINE(0x7015, 0xABF6);



/*
 *  GetTaggedCollectionItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetTaggedCollectionItem(
  Collection      c,
  CollectionTag   tag,
  SInt32          whichItem,
  SInt32 *        itemSize,
  void *          itemData)                                   TWOWORDINLINE(0x7016, 0xABF6);



/*
 *  GetTaggedCollectionItemInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetTaggedCollectionItemInfo(
  Collection      c,
  CollectionTag   tag,
  SInt32          whichItem,
  SInt32 *        id,
  SInt32 *        index,
  SInt32 *        itemSize,
  SInt32 *        attributes)                                 TWOWORDINLINE(0x7017, 0xABF6);



/*
 *  PurgeCollection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PurgeCollection(
  Collection   c,
  SInt32       whichAttributes,
  SInt32       matchingAttributes)                            TWOWORDINLINE(0x7018, 0xABF6);



/*
 *  PurgeCollectionTag()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PurgeCollectionTag(
  Collection      c,
  CollectionTag   tag)                                        TWOWORDINLINE(0x7019, 0xABF6);



/*
 *  EmptyCollection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
EmptyCollection(Collection c)                                 TWOWORDINLINE(0x701A, 0xABF6);


/*
 *  FlattenCollection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FlattenCollection(
  Collection             c,
  CollectionFlattenUPP   flattenProc,
  void *                 refCon)                              TWOWORDINLINE(0x701B, 0xABF6);



/*
 *  FlattenPartialCollection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FlattenPartialCollection(
  Collection             c,
  CollectionFlattenUPP   flattenProc,
  void *                 refCon,
  SInt32                 whichAttributes,
  SInt32                 matchingAttributes)                  TWOWORDINLINE(0x701C, 0xABF6);



/*
 *  UnflattenCollection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
UnflattenCollection(
  Collection             c,
  CollectionFlattenUPP   flattenProc,
  void *                 refCon)                              TWOWORDINLINE(0x701D, 0xABF6);



/*
 *  GetCollectionExceptionProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( CollectionExceptionUPP )
GetCollectionExceptionProc(Collection c)                      TWOWORDINLINE(0x701E, 0xABF6);



/*
 *  SetCollectionExceptionProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetCollectionExceptionProc(
  Collection               c,
  CollectionExceptionUPP   exceptionProc)                     TWOWORDINLINE(0x701F, 0xABF6);



/*
 *  GetNewCollection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Collection )
GetNewCollection(SInt16 collectionID)                         TWOWORDINLINE(0x7020, 0xABF6);



/**********************************************************************/
/************** Utility routines for handle-based access **************/
/**********************************************************************/
/*
 *  AddCollectionItemHdl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AddCollectionItemHdl(
  Collection      aCollection,
  CollectionTag   tag,
  SInt32          id,
  Handle          itemData)                                   TWOWORDINLINE(0x7021, 0xABF6);



/*
 *  GetCollectionItemHdl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetCollectionItemHdl(
  Collection      aCollection,
  CollectionTag   tag,
  SInt32          id,
  Handle          itemData)                                   TWOWORDINLINE(0x7022, 0xABF6);



/*
 *  ReplaceIndexedCollectionItemHdl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ReplaceIndexedCollectionItemHdl(
  Collection   aCollection,
  SInt32       index,
  Handle       itemData)                                      TWOWORDINLINE(0x7023, 0xABF6);



/*
 *  GetIndexedCollectionItemHdl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetIndexedCollectionItemHdl(
  Collection   aCollection,
  SInt32       index,
  Handle       itemData)                                      TWOWORDINLINE(0x7024, 0xABF6);



/*
 *  FlattenCollectionToHdl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
FlattenCollectionToHdl(
  Collection   aCollection,
  Handle       flattened)                                     TWOWORDINLINE(0x7025, 0xABF6);



/*
 *  UnflattenCollectionFromHdl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CollectionsLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
UnflattenCollectionFromHdl(
  Collection   aCollection,
  Handle       flattened)                                     TWOWORDINLINE(0x7026, 0xABF6);



#if OLDROUTINENAMES
enum {
  dontWantTag                   = kCollectionDontWantTag,
  dontWantId                    = kCollectionDontWantId,
  dontWantSize                  = kCollectionDontWantSize,
  dontWantAttributes            = kCollectionDontWantAttributes,
  dontWantIndex                 = kCollectionDontWantIndex,
  dontWantData                  = kCollectionDontWantData
};

enum {
  noCollectionAttributes        = kCollectionNoAttributes,
  allCollectionAttributes       = kCollectionAllAttributes,
  userCollectionAttributes      = kCollectionUserAttributes,
  defaultCollectionAttributes   = kCollectionDefaultAttributes
};

enum {
  collectionUser0Bit            = kCollectionUser0Bit,
  collectionUser1Bit            = kCollectionUser1Bit,
  collectionUser2Bit            = kCollectionUser2Bit,
  collectionUser3Bit            = kCollectionUser3Bit,
  collectionUser4Bit            = kCollectionUser4Bit,
  collectionUser5Bit            = kCollectionUser5Bit,
  collectionUser6Bit            = kCollectionUser6Bit,
  collectionUser7Bit            = kCollectionUser7Bit,
  collectionUser8Bit            = kCollectionUser8Bit,
  collectionUser9Bit            = kCollectionUser9Bit,
  collectionUser10Bit           = kCollectionUser10Bit,
  collectionUser11Bit           = kCollectionUser11Bit,
  collectionUser12Bit           = kCollectionUser12Bit,
  collectionUser13Bit           = kCollectionUser13Bit,
  collectionUser14Bit           = kCollectionUser14Bit,
  collectionUser15Bit           = kCollectionUser15Bit,
  collectionReserved0Bit        = kCollectionReserved0Bit,
  collectionReserved1Bit        = kCollectionReserved1Bit,
  collectionReserved2Bit        = kCollectionReserved2Bit,
  collectionReserved3Bit        = kCollectionReserved3Bit,
  collectionReserved4Bit        = kCollectionReserved4Bit,
  collectionReserved5Bit        = kCollectionReserved5Bit,
  collectionReserved6Bit        = kCollectionReserved6Bit,
  collectionReserved7Bit        = kCollectionReserved7Bit,
  collectionReserved8Bit        = kCollectionReserved8Bit,
  collectionReserved9Bit        = kCollectionReserved9Bit,
  collectionReserved10Bit       = kCollectionReserved10Bit,
  collectionReserved11Bit       = kCollectionReserved11Bit,
  collectionReserved12Bit       = kCollectionReserved12Bit,
  collectionReserved13Bit       = kCollectionReserved13Bit,
  collectionPersistenceBit      = kCollectionPersistenceBit,
  collectionLockBit             = kCollectionLockBit
};

enum {
  collectionUser0Mask           = kCollectionUser0Mask,
  collectionUser1Mask           = kCollectionUser1Mask,
  collectionUser2Mask           = kCollectionUser2Mask,
  collectionUser3Mask           = kCollectionUser3Mask,
  collectionUser4Mask           = kCollectionUser4Mask,
  collectionUser5Mask           = kCollectionUser5Mask,
  collectionUser6Mask           = kCollectionUser6Mask,
  collectionUser7Mask           = kCollectionUser7Mask,
  collectionUser8Mask           = kCollectionUser8Mask,
  collectionUser9Mask           = kCollectionUser9Mask,
  collectionUser10Mask          = kCollectionUser10Mask,
  collectionUser11Mask          = kCollectionUser11Mask,
  collectionUser12Mask          = kCollectionUser12Mask,
  collectionUser13Mask          = kCollectionUser13Mask,
  collectionUser14Mask          = kCollectionUser14Mask,
  collectionUser15Mask          = kCollectionUser15Mask,
  collectionReserved0Mask       = kCollectionReserved0Mask,
  collectionReserved1Mask       = kCollectionReserved1Mask,
  collectionReserved2Mask       = kCollectionReserved2Mask,
  collectionReserved3Mask       = kCollectionReserved3Mask,
  collectionReserved4Mask       = kCollectionReserved4Mask,
  collectionReserved5Mask       = kCollectionReserved5Mask,
  collectionReserved6Mask       = kCollectionReserved6Mask,
  collectionReserved7Mask       = kCollectionReserved7Mask,
  collectionReserved8Mask       = kCollectionReserved8Mask,
  collectionReserved9Mask       = kCollectionReserved9Mask,
  collectionReserved10Mask      = kCollectionReserved10Mask,
  collectionReserved11Mask      = kCollectionReserved11Mask,
  collectionReserved12Mask      = kCollectionReserved12Mask,
  collectionReserved13Mask      = kCollectionReserved13Mask,
  collectionPersistenceMask     = kCollectionPersistenceMask,
  collectionLockMask            = kCollectionLockMask
};

#endif  /* OLDROUTINENAMES */



#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __COLLECTIONS__ */

