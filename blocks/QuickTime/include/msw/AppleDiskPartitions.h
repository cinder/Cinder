/*
     File:       AppleDiskPartitions.h
 
     Contains:   The Apple disk partition scheme as defined in Inside Macintosh: Volume V.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __APPLEDISKPARTITIONS__
#define __APPLEDISKPARTITIONS__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif




#if PRAGMA_ONCE
#pragma once
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

/* Block 0 Definitions */
enum {
  sbSIGWord                     = 0x4552, /* signature word for Block 0 ('ER') */
  sbMac                         = 1     /* system type for Mac */
};

/* Partition Map Signatures */
enum {
  pMapSIG                       = 0x504D, /* partition map signature ('PM') */
  pdSigWord                     = 0x5453, /* partition map signature ('TS') */
  oldPMSigWord                  = pdSigWord,
  newPMSigWord                  = pMapSIG
};


/* Driver Descriptor Map */
struct Block0 {
  UInt16              sbSig;                  /* unique value for SCSI block 0 */
  UInt16              sbBlkSize;              /* block size of device */
  UInt32              sbBlkCount;             /* number of blocks on device */
  UInt16              sbDevType;              /* device type */
  UInt16              sbDevId;                /* device id */
  UInt32              sbData;                 /* not used */
  UInt16              sbDrvrCount;            /* driver descriptor count */
  UInt32              ddBlock;                /* 1st driver's starting block */
  UInt16              ddSize;                 /* size of 1st driver (512-byte blks) */
  UInt16              ddType;                 /* system type (1 for Mac+) */
  UInt16              ddPad[243];             /* ARRAY[0..242] OF INTEGER; not used */
};
typedef struct Block0                   Block0;
/* Driver descriptor */
struct DDMap {
  UInt32              ddBlock;                /* 1st driver's starting block */
  UInt16              ddSize;                 /* size of 1st driver (512-byte blks) */
  UInt16              ddType;                 /* system type (1 for Mac+) */
};
typedef struct DDMap                    DDMap;
/* Constants for the ddType field of the DDMap structure. */
enum {
  kDriverTypeMacSCSI            = 0x0001,
  kDriverTypeMacATA             = 0x0701,
  kDriverTypeMacSCSIChained     = 0xFFFF,
  kDriverTypeMacATAChained      = 0xF8FF
};

/* Partition Map Entry */
struct Partition {
  UInt16              pmSig;                  /* unique value for map entry blk */
  UInt16              pmSigPad;               /* currently unused */
  UInt32              pmMapBlkCnt;            /* # of blks in partition map */
  UInt32              pmPyPartStart;          /* physical start blk of partition */
  UInt32              pmPartBlkCnt;           /* # of blks in this partition */
  UInt8               pmPartName[32];         /* ASCII partition name */
  UInt8               pmParType[32];          /* ASCII partition type */
  UInt32              pmLgDataStart;          /* log. # of partition's 1st data blk */
  UInt32              pmDataCnt;              /* # of blks in partition's data area */
  UInt32              pmPartStatus;           /* bit field for partition status */
  UInt32              pmLgBootStart;          /* log. blk of partition's boot code */
  UInt32              pmBootSize;             /* number of bytes in boot code */
  UInt32              pmBootAddr;             /* memory load address of boot code */
  UInt32              pmBootAddr2;            /* currently unused */
  UInt32              pmBootEntry;            /* entry point of boot code */
  UInt32              pmBootEntry2;           /* currently unused */
  UInt32              pmBootCksum;            /* checksum of boot code */
  UInt8               pmProcessor[16];        /* ASCII for the processor type */
  UInt16              pmPad[188];             /* ARRAY[0..187] OF INTEGER; not used */
};
typedef struct Partition                Partition;

/* Flags for the pmPartStatus field of the Partition data structure. */
enum {
  kPartitionAUXIsValid          = 0x00000001,
  kPartitionAUXIsAllocated      = 0x00000002,
  kPartitionAUXIsInUse          = 0x00000004,
  kPartitionAUXIsBootValid      = 0x00000008,
  kPartitionAUXIsReadable       = 0x00000010,
  kPartitionAUXIsWriteable      = 0x00000020,
  kPartitionAUXIsBootCodePositionIndependent = 0x00000040,
  kPartitionIsWriteable         = 0x00000020,
  kPartitionIsMountedAtStartup  = 0x40000000,
  kPartitionIsStartup           = (long)0x80000000,
  kPartitionIsChainCompatible   = 0x00000100,
  kPartitionIsRealDeviceDriver  = 0x00000200,
  kPartitionCanChainToNext      = 0x00000400
};




/* Well known driver signatures, stored in the first four byte of pmPad. */
enum {
  kPatchDriverSignature         = FOUR_CHAR_CODE('ptDR'), /* SCSI and ATA[PI] patch driver    */
  kSCSIDriverSignature          = 0x00010600, /* SCSI  hard disk driver           */
  kATADriverSignature           = FOUR_CHAR_CODE('wiki'), /* ATA   hard disk driver           */
  kSCSICDDriverSignature        = FOUR_CHAR_CODE('CDvr'), /* SCSI  CD-ROM    driver           */
  kATAPIDriverSignature         = FOUR_CHAR_CODE('ATPI'), /* ATAPI CD-ROM    driver           */
  kDriveSetupHFSSignature       = FOUR_CHAR_CODE('DSU1') /* Drive Setup HFS partition        */
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


#endif /* __APPLEDISKPARTITIONS__ */

