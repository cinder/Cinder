/*
     File:       MultiprocessingInfo.h
 
     Contains:   Multiprocessing Information interfaces
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1996-2001 by Apple Computer, Inc. and (c) 1995-1997 DayStar Digital, Inc.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/

/*
   ==========================================================================================================================
   *** WARNING: You must properly check the availability of MP services before calling them!
   See the section titled "Checking API Availability".
   ==========================================================================================================================
*/


#ifndef __MULTIPROCESSINGINFO__
#define __MULTIPROCESSINGINFO__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MULTIPROCESSING__
#include <Multiprocessing.h>
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
   ==========================================================================================================================
   This is the header file for version 2.3 of the Mac OS multiprocessing information support. 
   ==========================================================================================================================
*/


/*
   ==========================================================================================================================
   The following services are new in version 2.1:
    MPGetNextTaskID
    MPGetNextCpuID
   ==========================================================================================================================
*/

/*
   ==========================================================================================================================
   The following services are new in version 2.2:
    MPGetPageSizeClasses
    MPGetPageSize
    MPGetNextAreaID
   ==========================================================================================================================
*/

/*
   ==========================================================================================================================
   The following services are new in version 2.3:
    MPGetNextCoherenceID
    MPGetNextProcessID
    MPGetNextAddressSpaceID
    MPGetNextQueueID
    MPGetNextSemaphoreID
    MPGetNextCriticalRegionID
    MPGetNextTimerID
    MPGetNextEventID
    MPGetNextNotificationID
    MPGetNextConsoleID
   ==========================================================================================================================
*/



/*
   .
   ==========================================================================================================================
   Page size Services
   ==================
*/

#if CALL_NOT_IN_CARBON
/*
 *  MPGetPageSizeClasses()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( MPPageSizeClass )
MPGetPageSizeClasses(void);


/* The number of page size classes, 1 to n.*/
#endif  /* CALL_NOT_IN_CARBON */

/* -------------------------------------------------------------------------------------------*/
#if CALL_NOT_IN_CARBON
/*
 *  MPGetPageSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( ByteCount )
MPGetPageSize(MPPageSizeClass pageClass);


/* The page size in bytes.*/
#endif  /* CALL_NOT_IN_CARBON */


/*
   .
   ==========================================================================================================================
   ID Iterator Services
   ==========================
*/

#if CALL_NOT_IN_CARBON
/*
 *  MPGetNextCoherenceID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextCoherenceID(MPCoherenceID * coherenceID);


/*
 *  MPGetNextCpuID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextCpuID(
  MPCoherenceID   owningCoherenceID,
  MPCpuID *       cpuID);


/*
 *  MPGetNextProcessID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextProcessID(MPProcessID * processID);


/*
 *  MPGetNextAddressSpaceID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextAddressSpaceID(
  MPProcessID         owningProcessID,
  MPAddressSpaceID *  addressSpaceID);


/*
 *  MPGetNextTaskID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextTaskID(
  MPProcessID   owningProcessID,
  MPTaskID *    taskID);


/*
 *  MPGetNextQueueID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextQueueID(
  MPProcessID   owningProcessID,
  MPQueueID *   queueID);


/*
 *  MPGetNextSemaphoreID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextSemaphoreID(
  MPProcessID      owningProcessID,
  MPSemaphoreID *  semaphoreID);


/*
 *  MPGetNextCriticalRegionID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextCriticalRegionID(
  MPProcessID           owningProcessID,
  MPCriticalRegionID *  criticalRegionID);


/*
 *  MPGetNextTimerID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextTimerID(
  MPProcessID   owningProcessID,
  MPTimerID *   timerID);


/*
 *  MPGetNextEventID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextEventID(
  MPProcessID   owningProcessID,
  MPEventID *   eventID);


/*
 *  MPGetNextNotificationID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextNotificationID(
  MPProcessID         owningProcessID,
  MPNotificationID *  notificationID);


#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON
/*
 *  MPGetNextAreaID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSStatus )
MPGetNextAreaID(
  MPAddressSpaceID   owningSpaceID,
  MPAreaID *         areaID);


#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON
/*
 *  MPGetNextConsoleID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextConsoleID(MPConsoleID * consoleID);




/* -------------------------------------------------------------------------------------------*/


/*
 *  MPGetNextID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNextID(
  MPOpaqueIDClass   kind,
  MPOpaqueID *      id);




/*
   .
   ==========================================================================================================================
   Object Information Services
   ===========================
*/


/*
   ----------------------------------------------------------------------------------------------
   ! The implementation of MPGetObjectInfo assumes that all info records are in 4 byte multiples.
*/


#endif  /* CALL_NOT_IN_CARBON */

enum {
                                        /* The version of the MPAreaInfo structure requested.*/
  kMPQueueInfoVersion           = 1L | (kOpaqueQueueID << 16),
  kMPSemaphoreInfoVersion       = 1L | (kOpaqueSemaphoreID << 16),
  kMPEventInfoVersion           = 1L | (kOpaqueEventID << 16),
  kMPCriticalRegionInfoVersion  = 1L | (kOpaqueCriticalRegionID << 16),
  kMPNotificationInfoVersion    = 1L | (kOpaqueNotificationID << 16),
  kMPAddressSpaceInfoVersion    = 1L | (kOpaqueAddressSpaceID << 16)
};


struct MPQueueInfo {
  PBVersion           version;                /* Version of the data structure requested*/

  MPProcessID         processID;              /* Owning process ID*/
  OSType              queueName;              /* Queue name*/

  ItemCount           nWaiting;
  MPTaskID            waitingTaskID;          /* First waiting task.*/

  ItemCount           nMessages;
  ItemCount           nReserved;

  void *              p1;                     /* First message parameters...*/
  void *              p2;
  void *              p3;
};
typedef struct MPQueueInfo              MPQueueInfo;
struct MPSemaphoreInfo {
  PBVersion           version;                /* Version of the data structure requested*/

  MPProcessID         processID;              /* Owning process ID*/
  OSType              semaphoreName;          /* Semaphore name*/

  ItemCount           nWaiting;
  MPTaskID            waitingTaskID;          /* First waiting task.*/

  ItemCount           maximum;
  ItemCount           count;
};
typedef struct MPSemaphoreInfo          MPSemaphoreInfo;
struct MPEventInfo {
  PBVersion           version;                /* Version of the data structure requested*/

  MPProcessID         processID;              /* Owning process ID*/
  OSType              eventName;              /* Event name*/

  ItemCount           nWaiting;
  MPTaskID            waitingTaskID;          /* First waiting task.*/

  MPEventFlags        events;
};
typedef struct MPEventInfo              MPEventInfo;
struct MPCriticalRegionInfo {
  PBVersion           version;                /* Version of the data structure requested*/

  MPProcessID         processID;              /* Owning process ID*/
  OSType              regionName;             /* Critical region name*/

  ItemCount           nWaiting;
  MPTaskID            waitingTaskID;          /* First waiting task.*/

  MPTaskID            owningTask;
  ItemCount           count;
};
typedef struct MPCriticalRegionInfo     MPCriticalRegionInfo;
struct MPNotificationInfo {
  PBVersion           version;                /* Version of the data structure requested*/

  MPProcessID         processID;              /* Owning process ID*/
  OSType              notificationName;       /* Notification name*/

  MPQueueID           queueID;                /* Queue to notify.*/
  void *              p1;
  void *              p2;
  void *              p3;

  MPEventID           eventID;                /* Event to set.*/
  MPEventFlags        events;

  MPSemaphoreID       semaphoreID;            /* Sempahore to signal.   */
};
typedef struct MPNotificationInfo       MPNotificationInfo;
struct MPAddressSpaceInfo {
  PBVersion           version;                /* Version of the data structure requested*/

  MPProcessID         processID;              /* Owning process ID*/
  MPCoherenceID       groupID;                /* Related coherence group.*/
  ItemCount           nTasks;                 /* Number of tasks in this space.*/
  UInt32              vsid[16];               /* Segment register VSIDs.*/
};
typedef struct MPAddressSpaceInfo       MPAddressSpaceInfo;
/* *** We should put the task info call here instead of in MPExtractTaskState.*/


#if CALL_NOT_IN_CARBON
/*
 *  MPGetQueueInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetQueueInfo(
  MPQueueID      id,
  PBVersion      version,
  MPQueueInfo *  info_o);


/*
 *  MPGetSemaphoreInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetSemaphoreInfo(
  MPSemaphoreID      id,
  PBVersion          version,
  MPSemaphoreInfo *  info_o);


/*
 *  MPGetEventInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetEventInfo(
  MPEventID      id,
  PBVersion      version,
  MPEventInfo *  info_o);


/*
 *  MPGetCriticalRegionInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetCriticalRegionInfo(
  MPCriticalRegionID      id,
  PBVersion               version,
  MPCriticalRegionInfo *  info_o);


/*
 *  MPGetNotificationInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetNotificationInfo(
  MPNotificationID      id,
  PBVersion             version,
  MPNotificationInfo *  info_o);


/*
 *  MPGetAddressSpaceInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MPDiagnostics 2.3 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MPGetAddressSpaceInfo(
  MPAddressSpaceID      id,
  PBVersion             version,
  MPAddressSpaceInfo *  info_o);



/* ==========================================================================================================================*/



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

#endif /* __MULTIPROCESSINGINFO__ */

