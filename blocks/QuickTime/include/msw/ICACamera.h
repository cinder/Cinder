/*
     File:       ICACamera.h
 
     Contains:   Digital still camera-specific selectors and structures
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2000-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __ICACAMERA__
#define __ICACAMERA__



#if PRAGMA_ONCE
#pragma once
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

/*
   -------------------------------------------------------------------------
                                Selectors           
   -------------------------------------------------------------------------
*/
enum {
                                        /* Camera properties*/
                                        /* Refer to section 13 of the PIMA 15740 (PTP) specification for*/
                                        /* descriptions and usage notes for these standard properties*/
  kICAPropertyCameraBatteryLevel = FOUR_CHAR_CODE('5001'), /* UInt8   enum/range*/
  kICAPropertyCameraFunctionalMode = FOUR_CHAR_CODE('5002'), /* UInt16     enum*/
  kICAPropertyCameraImageSize   = FOUR_CHAR_CODE('5003'), /* CFString     enum/range*/
  kICAPropertyCameraCompressionSetting = FOUR_CHAR_CODE('5004'), /* UInt8   enum/range*/
  kICAPropertyCameraWhiteBalance = FOUR_CHAR_CODE('5005'), /* UInt16     enum*/
  kICAPropertyCameraRGBGain     = FOUR_CHAR_CODE('5006'), /* null terminated string enum/range*/
  kICAPropertyCameraFNumber     = FOUR_CHAR_CODE('5007'), /* UInt16     enum*/
  kICAPropertyCameraFocalLength = FOUR_CHAR_CODE('5008'), /* UInt32     enum/range*/
  kICAPropertyCameraFocusDistance = FOUR_CHAR_CODE('5009'), /* UInt16     enum/range*/
  kICAPropertyCameraFocusMode   = FOUR_CHAR_CODE('500A'), /* UInt16     enum*/
  kICAPropertyCameraExposureMeteringMode = FOUR_CHAR_CODE('500B'), /* UInt16     enum*/
  kICAPropertyCameraFlashMode   = FOUR_CHAR_CODE('500C'), /* UInt16     enum*/
  kICAPropertyCameraExposureTime = FOUR_CHAR_CODE('500D'), /* UInt32     enum/range*/
  kICAPropertyCameraExposureProgramMode = FOUR_CHAR_CODE('500E'), /* UInt16     enum*/
  kICAPropertyCameraExposureIndex = FOUR_CHAR_CODE('500F'), /* UInt16     enum/range*/
  kICAPropertyCameraExposureBiasCompensation = FOUR_CHAR_CODE('5010'), /* UInt16     enum/range*/
  kICAPropertyCameraDateTime    = FOUR_CHAR_CODE('5011'), /* null terminated string     none*/
  kICAPropertyCameraCaptureDelay = FOUR_CHAR_CODE('5012'), /* UInt32     enum/range*/
  kICAPropertyCameraStillCaptureMode = FOUR_CHAR_CODE('5013'), /* UInt16     enum*/
  kICAPropertyCameraContrast    = FOUR_CHAR_CODE('5014'), /* UInt8   enum/range*/
  kICAPropertyCameraSharpness   = FOUR_CHAR_CODE('5015'), /* UInt8   enum/range*/
  kICAPropertyCameraDigitalZoom = FOUR_CHAR_CODE('5016'), /* UInt8   enum/range*/
  kICAPropertyCameraEffectMode  = FOUR_CHAR_CODE('5017'), /* UInt16     enum*/
  kICAPropertyCameraBurstNumber = FOUR_CHAR_CODE('5018'), /* UInt16     enum/range*/
  kICAPropertyCameraBurstInterval = FOUR_CHAR_CODE('5019'), /* UInt16     enum/range*/
  kICAPropertyCameraTimelapseNumber = FOUR_CHAR_CODE('501A'), /* UInt16     enum/range*/
  kICAPropertyCameraTimelapseInterval = FOUR_CHAR_CODE('501B'), /* UInt32     enum/range*/
  kICAPropertyCameraFocusMeteringMode = FOUR_CHAR_CODE('501C') /* UInt16     enum*/
};

enum {
                                        /* Refer to section 5.5.3 of the PTP spec*/
  kICAPropertyCameraStorageType = FOUR_CHAR_CODE('stor'), /* UInt16*/
  kICAPropertyCameraFilesystemType = FOUR_CHAR_CODE('fsys'), /* UInt16*/
  kICAPropertyCameraAccessCapability = FOUR_CHAR_CODE('acap'), /* UInt16*/
  kICAPropertyCameraMaxCapacity = FOUR_CHAR_CODE('maxc'), /* UInt64*/
  kICAPropertyCameraFreeSpaceInBytes = FOUR_CHAR_CODE('fres'), /* UInt64*/
  kICAPropertyCameraFreeSpaceInImages = FOUR_CHAR_CODE('frei'), /* UInt32*/
  kICAPropertyCameraStorageDescription = FOUR_CHAR_CODE('stod'), /* null terminated string*/
  kICAPropertyCameraVolumeLabel = FOUR_CHAR_CODE('voll') /* null terminated string*/
};

enum {
                                        /* ICA specific*/
  kICAPropertyCameraIcon        = FOUR_CHAR_CODE('icon'), /* ICAThumbnail*/
  kICAPropertyCameraSupportedMessages = FOUR_CHAR_CODE('msgs') /* array of OSTypes*/
};

enum {
                                        /* Values for kICAPropertyCameraStorageType*/
  kICAStorageFixedROM           = 0x0001,
  kICAStorageRemovableROM       = 0x0002,
  kICAStorageFixedRAM           = 0x0003,
  kICAStorageRemovableRAM       = 0x0004
};

enum {
                                        /* Values for kICAPropertyCameraFilesystemType*/
  kICAFileystemGenericFlat      = 0x0001,
  kICAFileystemGenericHierarchical = 0x0002,
  kICAFileystemDCF              = 0x0003
};

enum {
                                        /* Values for kICAPropertyCameraAccessCapability*/
  kICAAccessReadWrite           = 0x0000,
  kICAAccessReadOnly            = 0x0001,
  kICAAccessReadOnlyWithObjectDeletion = 0x0002
};

enum {
                                        /* Camera messages*/
  kICAMessageCameraCaptureNewImage = FOUR_CHAR_CODE('ccni'),
  kICAMessageCameraDeleteOne    = FOUR_CHAR_CODE('del1'),
  kICAMessageCameraDeleteAll    = FOUR_CHAR_CODE('dela'),
  kICAMessageCameraSyncClock    = FOUR_CHAR_CODE('sclk'),
  kICAMessageCameraUploadData   = FOUR_CHAR_CODE('load')
};


#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif


#endif /* __ICACAMERA__ */

