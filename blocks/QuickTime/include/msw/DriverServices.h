/*
     File:       DriverServices.h
 
     Contains:   Driver Services Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __DRIVERSERVICES__
#define __DRIVERSERVICES__

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __MACHINEEXCEPTIONS__
#include <MachineExceptions.h>
#endif

#ifndef __DEVICES__
#include <Devices.h>
#endif

#ifndef __DRIVERSYNCHRONIZATION__
#include <DriverSynchronization.h>
#endif

#ifndef __NAMEREGISTRY__
#include <NameRegistry.h>
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

/******************************************************************
 *
 *      Previously in Kernel.h
 *
 ******************************************************************/
/* Kernel basics*/
typedef struct OpaqueIOPreparationID*   IOPreparationID;
typedef struct OpaqueSoftwareInterruptID*  SoftwareInterruptID;
typedef struct OpaqueTaskID*            TaskID;
typedef struct OpaqueTimerID*           TimerID;
/* Tasking*/
typedef UInt32                          ExecutionLevel;
enum {
  kTaskLevel                    = 0,
  kSoftwareInterruptLevel       = 1,
  kAcceptFunctionLevel          = 2,
  kKernelLevel                  = 3,
  kSIHAcceptFunctionLevel       = 4,
  kSecondaryInterruptLevel      = 5,
  kHardwareInterruptLevel       = 6,
  kMPTaskLevel                  = 7
};

typedef CALLBACK_API_C( void , SoftwareInterruptHandler )(void *p1, void *p2);
typedef CALLBACK_API_C( OSStatus , SecondaryInterruptHandler2 )(void *p1, void *p2);
#define kCurrentAddressSpaceID ((AddressSpaceID) -1)
/* Memory System basics*/
struct LogicalAddressRange {
  LogicalAddress      address;
  ByteCount           count;
};
typedef struct LogicalAddressRange      LogicalAddressRange;
typedef LogicalAddressRange *           LogicalAddressRangePtr;
struct PhysicalAddressRange {
  PhysicalAddress     address;
  ByteCount           count;
};
typedef struct PhysicalAddressRange     PhysicalAddressRange;
typedef PhysicalAddressRange *          PhysicalAddressRangePtr;
/* For PrepareMemoryForIO and CheckpointIO*/
typedef OptionBits                      IOPreparationOptions;
enum {
  kIOMultipleRanges             = 0x00000001,
  kIOLogicalRanges              = 0x00000002,
  kIOMinimalLogicalMapping      = 0x00000004,
  kIOShareMappingTables         = 0x00000008,
  kIOIsInput                    = 0x00000010,
  kIOIsOutput                   = 0x00000020,
  kIOCoherentDataPath           = 0x00000040,
  kIOTransferIsLogical          = 0x00000080,
  kIOClientIsUserMode           = 0x00000080
};

typedef OptionBits                      IOPreparationState;
enum {
  kIOStateDone                  = 0x00000001
};

enum {
  kInvalidPageAddress           = (-1)
};

struct AddressRange {
  void *              base;
  ByteCount           length;
};
typedef struct AddressRange             AddressRange;
/* C's treatment of arrays and array pointers is atypical*/

typedef LogicalAddress *                LogicalMappingTablePtr;
typedef PhysicalAddress *               PhysicalMappingTablePtr;
typedef AddressRange *                  AddressRangeTablePtr;
struct MultipleAddressRange {
  ItemCount           entryCount;
  AddressRangeTablePtr  rangeTable;
};
typedef struct MultipleAddressRange     MultipleAddressRange;
/*
   Separate C definition so that union has a name.  A future version of the interfacer
   tool will allow a name (that gets thrown out in Pascal and Asm).
*/
struct IOPreparationTable {
  IOPreparationOptions  options;
  IOPreparationState  state;
  IOPreparationID     preparationID;
  AddressSpaceID      addressSpace;
  ByteCount           granularity;
  ByteCount           firstPrepared;
  ByteCount           lengthPrepared;
  ItemCount           mappingEntryCount;
  LogicalMappingTablePtr  logicalMapping;
  PhysicalMappingTablePtr  physicalMapping;
  union {
    AddressRange        range;
    MultipleAddressRange  multipleRanges;
  }                       rangeInfo;
};
typedef struct IOPreparationTable       IOPreparationTable;

typedef OptionBits                      IOCheckpointOptions;
enum {
  kNextIOIsInput                = 0x00000001,
  kNextIOIsOutput               = 0x00000002,
  kMoreIOTransfers              = 0x00000004
};

/* For SetProcessorCacheMode*/

typedef UInt32                          ProcessorCacheMode;
enum {
  kProcessorCacheModeDefault    = 0,
  kProcessorCacheModeInhibited  = 1,
  kProcessorCacheModeWriteThrough = 2,
  kProcessorCacheModeCopyBack   = 3
};

/*
   For GetPageInformation
   (Note: if kPageInformationVersion fails, try 0 -- old versions of DSL defined  kPageInformationVersion as 0)
*/

enum {
  kPageInformationVersion       = 1
};

typedef UInt32                          PageStateInformation;
enum {
  kPageIsProtected              = 0x00000001,
  kPageIsProtectedPrivileged    = 0x00000002,
  kPageIsModified               = 0x00000004,
  kPageIsReferenced             = 0x00000008,
  kPageIsLockedResident         = 0x00000010, /* held and locked resident*/
  kPageIsInMemory               = 0x00000020,
  kPageIsShared                 = 0x00000040,
  kPageIsWriteThroughCached     = 0x00000080,
  kPageIsCopyBackCached         = 0x00000100,
  kPageIsHeldResident           = 0x00000200, /* held resident - use kPageIsLockedResident to check for locked state*/
  kPageIsLocked                 = kPageIsLockedResident, /* Deprecated*/
  kPageIsResident               = kPageIsInMemory /* Deprecated*/
};

struct PageInformation {
  AreaID              area;
  ItemCount           count;
  PageStateInformation  information[1];
};
typedef struct PageInformation          PageInformation;
typedef PageInformation *               PageInformationPtr;

/*  Tasks  */
#if CALL_NOT_IN_CARBON
/*
 *  CurrentExecutionLevel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( ExecutionLevel )
CurrentExecutionLevel(void);


/*
 *  CurrentTaskID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TaskID )
CurrentTaskID(void);


/*
 *  DelayFor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DelayFor(Duration delayDuration);


/*
 *  InPrivilegedMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
InPrivilegedMode(void);



/*  Software Interrupts  */
/*
 *  CreateSoftwareInterrupt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
CreateSoftwareInterrupt(
  SoftwareInterruptHandler   handler,
  TaskID                     task,
  void *                     p1,
  Boolean                    persistent,
  SoftwareInterruptID *      theSoftwareInterrupt);



/*
 *  SendSoftwareInterrupt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
SendSoftwareInterrupt(
  SoftwareInterruptID   theSoftwareInterrupt,
  void *                p2);


/*
 *  DeleteSoftwareInterrupt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DeleteSoftwareInterrupt(SoftwareInterruptID theSoftwareInterrupt);


#endif  /* CALL_NOT_IN_CARBON */

#if TARGET_OS_MAC
/*  Secondary Interrupts  */
#if CALL_NOT_IN_CARBON
/*
 *  CallSecondaryInterruptHandler2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
CallSecondaryInterruptHandler2(
  SecondaryInterruptHandler2   theHandler,
  ExceptionHandler             exceptionHandler,
  void *                       p1,
  void *                       p2);


/*
 *  QueueSecondaryInterruptHandler()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
QueueSecondaryInterruptHandler(
  SecondaryInterruptHandler2   theHandler,
  ExceptionHandler             exceptionHandler,
  void *                       p1,
  void *                       p2);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_OS_MAC */

/*  Timers  */
#if CALL_NOT_IN_CARBON
/*
 *  SetInterruptTimer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
SetInterruptTimer(
  const AbsoluteTime *         expirationTime,
  SecondaryInterruptHandler2   handler,
  void *                       p1,
  TimerID *                    theTimer);


/*
 *  SetPersistentTimer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
SetPersistentTimer(
  Duration                     frequency,
  SecondaryInterruptHandler2   theHandler,
  void *                       p1,
  TimerID *                    theTimer);


/*
 *  CancelTimer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
CancelTimer(
  TimerID         theTimer,
  AbsoluteTime *  timeRemaining);



/*  I/O related Operations  */
/*
 *  PrepareMemoryForIO()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
PrepareMemoryForIO(IOPreparationTable * theIOPreparationTable);


/*
 *  CheckpointIO()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
CheckpointIO(
  IOPreparationID       theIOPreparation,
  IOCheckpointOptions   options);



/*  Memory Operations  */
/*
 *  GetPageInformation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
GetPageInformation(
  AddressSpaceID        addressSpace,
  ConstLogicalAddress   base,
  ByteCount             length,
  PBVersion             version,
  PageInformation *     thePageInfo);


/*  Processor Cache Related  */
/*
 *  SetProcessorCacheMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
SetProcessorCacheMode(
  AddressSpaceID        addressSpace,
  ConstLogicalAddress   base,
  ByteCount             length,
  ProcessorCacheMode    cacheMode);


/******************************************************************
 *
 *      Was in DriverSupport.h or DriverServices.h
 *
 ******************************************************************/
#define kAAPLDeviceLogicalAddress "AAPL,address"
#endif  /* CALL_NOT_IN_CARBON */

typedef LogicalAddress *                DeviceLogicalAddressPtr;
enum {
  durationMicrosecond           = -1L,  /* Microseconds are negative*/
  durationMillisecond           = 1L,   /* Milliseconds are positive*/
  durationSecond                = 1000L, /* 1000 * durationMillisecond*/
  durationMinute                = 60000L, /* 60 * durationSecond,*/
  durationHour                  = 3600000L, /* 60 * durationMinute,*/
  durationDay                   = 86400000L, /* 24 * durationHour,*/
  durationNoWait                = 0L,   /* don't block*/
  durationForever               = 0x7FFFFFFF /* no time limit*/
};

enum {
  k8BitAccess                   = 0,    /* access as 8 bit*/
  k16BitAccess                  = 1,    /* access as 16 bit*/
  k32BitAccess                  = 2     /* access as 32 bit*/
};

typedef UnsignedWide                    Nanoseconds;

#if CALL_NOT_IN_CARBON
/*
 *  IOCommandIsComplete()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
IOCommandIsComplete(
  IOCommandID   theID,
  OSErr         theResult);


/*
 *  GetIOCommandInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
GetIOCommandInfo(
  IOCommandID          theID,
  IOCommandContents *  theContents,
  IOCommandCode *      theCommand,
  IOCommandKind *      theKind);


/*
 *  UpdateDeviceActivity()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
UpdateDeviceActivity(RegEntryID * deviceEntry);


/*
 *  BlockCopy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
BlockCopy(
  const void *  srcPtr,
  void *        destPtr,
  Size          byteCount);


/*
 *  PoolAllocateResident()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( LogicalAddress )
PoolAllocateResident(
  ByteCount   byteSize,
  Boolean     clear);


/*
 *  PoolDeallocate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
PoolDeallocate(LogicalAddress address);


/*
 *  GetLogicalPageSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( ByteCount )
GetLogicalPageSize(void);


/*
 *  GetDataCacheLineSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( ByteCount )
GetDataCacheLineSize(void);


/*
 *  FlushProcessorCache()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
FlushProcessorCache(
  AddressSpaceID   spaceID,
  LogicalAddress   base,
  ByteCount        length);


/*
 *  MemAllocatePhysicallyContiguous()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( LogicalAddress )
MemAllocatePhysicallyContiguous(
  ByteCount   byteSize,
  Boolean     clear);


/*
 *  MemDeallocatePhysicallyContiguous()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
MemDeallocatePhysicallyContiguous(LogicalAddress address);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  UpTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AbsoluteTime )
UpTime(void);


#if CALL_NOT_IN_CARBON
/*
 *  GetTimeBaseInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
GetTimeBaseInfo(
  UInt32 *  minAbsoluteTimeDelta,
  UInt32 *  theAbsoluteTimeToNanosecondNumerator,
  UInt32 *  theAbsoluteTimeToNanosecondDenominator,
  UInt32 *  theProcessorToAbsoluteTimeNumerator,
  UInt32 *  theProcessorToAbsoluteTimeDenominator);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  AbsoluteToNanoseconds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Nanoseconds )
AbsoluteToNanoseconds(AbsoluteTime absoluteTime);


/*
 *  AbsoluteToDuration()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Duration )
AbsoluteToDuration(AbsoluteTime absoluteTime);


/*
 *  NanosecondsToAbsolute()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AbsoluteTime )
NanosecondsToAbsolute(Nanoseconds nanoseconds);


/*
 *  DurationToAbsolute()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AbsoluteTime )
DurationToAbsolute(Duration duration);


/*
 *  AddAbsoluteToAbsolute()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AbsoluteTime )
AddAbsoluteToAbsolute(
  AbsoluteTime   absoluteTime1,
  AbsoluteTime   absoluteTime2);


/*
 *  SubAbsoluteFromAbsolute()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AbsoluteTime )
SubAbsoluteFromAbsolute(
  AbsoluteTime   leftAbsoluteTime,
  AbsoluteTime   rightAbsoluteTime);


/*
 *  AddNanosecondsToAbsolute()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AbsoluteTime )
AddNanosecondsToAbsolute(
  Nanoseconds    nanoseconds,
  AbsoluteTime   absoluteTime);


/*
 *  AddDurationToAbsolute()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AbsoluteTime )
AddDurationToAbsolute(
  Duration       duration,
  AbsoluteTime   absoluteTime);


/*
 *  SubNanosecondsFromAbsolute()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AbsoluteTime )
SubNanosecondsFromAbsolute(
  Nanoseconds    nanoseconds,
  AbsoluteTime   absoluteTime);


/*
 *  SubDurationFromAbsolute()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( AbsoluteTime )
SubDurationFromAbsolute(
  Duration       duration,
  AbsoluteTime   absoluteTime);


/*
 *  AbsoluteDeltaToNanoseconds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Nanoseconds )
AbsoluteDeltaToNanoseconds(
  AbsoluteTime   leftAbsoluteTime,
  AbsoluteTime   rightAbsoluteTime);


/*
 *  AbsoluteDeltaToDuration()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Duration )
AbsoluteDeltaToDuration(
  AbsoluteTime   leftAbsoluteTime,
  AbsoluteTime   rightAbsoluteTime);


/*
 *  DurationToNanoseconds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Nanoseconds )
DurationToNanoseconds(Duration theDuration);


/*
 *  NanosecondsToDuration()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.6 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Duration )
NanosecondsToDuration(Nanoseconds theNanoseconds);



#if CALL_NOT_IN_CARBON
/*
 *  PBQueueInit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
PBQueueInit(QHdrPtr qHeader);


/*
 *  PBQueueCreate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
PBQueueCreate(QHdrPtr * qHeader);


/*
 *  PBQueueDelete()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
PBQueueDelete(QHdrPtr qHeader);


/*
 *  PBEnqueue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
PBEnqueue(
  QElemPtr   qElement,
  QHdrPtr    qHeader);


/*
 *  PBEnqueueLast()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
PBEnqueueLast(
  QElemPtr   qElement,
  QHdrPtr    qHeader);


/*
 *  PBDequeue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
PBDequeue(
  QElemPtr   qElement,
  QHdrPtr    qHeader);


/*
 *  PBDequeueFirst()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
PBDequeueFirst(
  QHdrPtr     qHeader,
  QElemPtr *  theFirstqElem);


/*
 *  PBDequeueLast()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
PBDequeueLast(
  QHdrPtr     qHeader,
  QElemPtr *  theLastqElem);


/*
 *  CStrCopy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( char * )
CStrCopy(
  char *        dst,
  const char *  src);


/*
 *  PStrCopy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( StringPtr )
PStrCopy(
  StringPtr          dst,
  ConstStr255Param   src);


/*
 *  CStrNCopy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( char * )
CStrNCopy(
  char *        dst,
  const char *  src,
  UInt32        max);


/*
 *  PStrNCopy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( StringPtr )
PStrNCopy(
  StringPtr          dst,
  ConstStr255Param   src,
  UInt32             max);


/*
 *  CStrCat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( char * )
CStrCat(
  char *        dst,
  const char *  src);


/*
 *  PStrCat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( StringPtr )
PStrCat(
  StringPtr          dst,
  ConstStr255Param   src);


/*
 *  CStrNCat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( char * )
CStrNCat(
  char *        dst,
  const char *  src,
  UInt32        max);


/*
 *  PStrNCat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( StringPtr )
PStrNCat(
  StringPtr          dst,
  ConstStr255Param   src,
  UInt32             max);


/*
 *  PStrToCStr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
PStrToCStr(
  char *             dst,
  ConstStr255Param   src);


/*
 *  CStrToPStr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
CStrToPStr(
  Str255        dst,
  const char *  src);


/*
 *  CStrCmp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( SInt16 )
CStrCmp(
  const char *  s1,
  const char *  s2);


/*
 *  PStrCmp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( SInt16 )
PStrCmp(
  ConstStr255Param   str1,
  ConstStr255Param   str2);


/*
 *  CStrNCmp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( SInt16 )
CStrNCmp(
  const char *  s1,
  const char *  s2,
  UInt32        max);


/*
 *  PStrNCmp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( SInt16 )
PStrNCmp(
  ConstStr255Param   str1,
  ConstStr255Param   str2,
  UInt32             max);


/*
 *  CStrLen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( UInt32 )
CStrLen(const char * src);


/*
 *  PStrLen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( UInt32 )
PStrLen(ConstStr255Param src);


/*
 *  DeviceProbe()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DeviceProbe(
  void *   theSrc,
  void *   theDest,
  UInt32   AccessType);


/*
 *  DelayForHardware()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DelayForHardware(AbsoluteTime absoluteTime);




/******************************************************************
 *
 *      Was in Interrupts.h 
 *
 ******************************************************************/
/*  Interrupt types  */
#endif  /* CALL_NOT_IN_CARBON */

typedef struct OpaqueInterruptSetID*    InterruptSetID;
typedef long                            InterruptMemberNumber;
struct InterruptSetMember {
  InterruptSetID      setID;
  InterruptMemberNumber  member;
};
typedef struct InterruptSetMember       InterruptSetMember;
enum {
  kISTChipInterruptSource       = 0,
  kISTOutputDMAInterruptSource  = 1,
  kISTInputDMAInterruptSource   = 2,
  kISTPropertyMemberCount       = 3
};

typedef InterruptSetMember              ISTProperty[3];
#define kISTPropertyName    "driver-ist" 

typedef long                            InterruptReturnValue;
enum {
  kFirstMemberNumber            = 1,
  kIsrIsComplete                = 0,
  kIsrIsNotComplete             = -1,
  kMemberNumberParent           = -2
};

typedef Boolean                         InterruptSourceState;
enum {
  kSourceWasEnabled             = true,
  kSourceWasDisabled            = false
};


typedef CALLBACK_API_C( InterruptMemberNumber , InterruptHandler )(InterruptSetMember ISTmember, void *refCon, UInt32 theIntCount);
typedef CALLBACK_API_C( void , InterruptEnabler )(InterruptSetMember ISTmember, void *refCon);
typedef CALLBACK_API_C( InterruptSourceState , InterruptDisabler )(InterruptSetMember ISTmember, void *refCon);
enum {
  kReturnToParentWhenComplete   = 0x00000001,
  kReturnToParentWhenNotComplete = 0x00000002
};

typedef OptionBits                      InterruptSetOptions;
/*  Interrupt Services  */
#if CALL_NOT_IN_CARBON
/*
 *  CreateInterruptSet()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
CreateInterruptSet(
  InterruptSetID          parentSet,
  InterruptMemberNumber   parentMember,
  InterruptMemberNumber   setSize,
  InterruptSetID *        setID,
  InterruptSetOptions     options);



/*
 *  InstallInterruptFunctions()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
InstallInterruptFunctions(
  InterruptSetID          setID,
  InterruptMemberNumber   member,
  void *                  refCon,
  InterruptHandler        handlerFunction,
  InterruptEnabler        enableFunction,
  InterruptDisabler       disableFunction);



/*
 *  GetInterruptFunctions()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
GetInterruptFunctions(
  InterruptSetID          setID,
  InterruptMemberNumber   member,
  void **                 refCon,
  InterruptHandler *      handlerFunction,
  InterruptEnabler *      enableFunction,
  InterruptDisabler *     disableFunction);


/*
 *  ChangeInterruptSetOptions()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
ChangeInterruptSetOptions(
  InterruptSetID        setID,
  InterruptSetOptions   options);


/*
 *  GetInterruptSetOptions()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DriverServicesLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
GetInterruptSetOptions(
  InterruptSetID         setID,
  InterruptSetOptions *  options);




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

#endif /* __DRIVERSERVICES__ */

