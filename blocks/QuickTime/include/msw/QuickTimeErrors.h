/*
     File:       QuickTimeErrors.h
 
     Contains:   QuickTime Error codes.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2007 by Apple Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __QUICKTIMEERRORS__
#define __QUICKTIMEERRORS__



#if PRAGMA_ONCE
#pragma once
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

/* QuickTime is not available to 64-bit clients */

#if !__LP64__

enum {
  kQTMediaDoesNotSupportDisplayOffsetsErr = -9460, /* Returned by new (B-frame-aware) media APIs that attempt to set nonzero decode to display deltas on media that do not support them. */
  kQTMediaHasDisplayOffsetsErr  = -9461, /* Returned by old (non-B-frame-aware) media APIs that cannot cope with nonzero decode to display deltas on media that contain them. */
  kQTDisplayTimeAlreadyInUseErr = -9462, /* Returned if an attempt to add a new media sample fails because there is already a sample with its display time. */
  kQTDisplayTimeTooEarlyErr     = -9463, /* Returned if an attempt to add a new media sample fails because its display time */
                                        /* would be earlier than the display time of an existing sample that does not have the */
                                        /* mediaSampleEarlierDisplayTimesAllowed flag set. */
  kQTTimeValueTooBigErr         = -9464, /* Returned by APIs that use 32-bit TimeValues if a timevalue could not be returned because it did not fit. */
  kQTVisualContextRequiredErr   = -9458, /* Returned if a visual context is required */
  kQTVisualContextNotAllowedErr = -9459, /* Returned if a visual context cannot be used */
  kQTPropertyBadValueSizeErr    = -2184, /* Returned if the size for the property value is incorrect */
  kQTPropertyNotSupportedErr    = -2195, /* Returned if the property is not supported */
  kQTPropertyAskLaterErr        = -2200, /* Returned if the property is not available now, but may be available later */
  kQTPropertyReadOnlyErr        = -2191, /* Returned if the property cannot be set */
  kQTPropertyArrayElementUnprocessedErr = -9465, /* Used with property arrays, set on a property element prior to processing (example usage: NewMovieFromProperties) */
  kQTCannotCoerceValueErr       = -2192, /* Returned if a value cannot be coerced to the request type */
  kQTMessageNotHandledErr       = -2194, /* Returned if the message was not handled */
  kQTMessageCommandNotSupportedErr = -2196, /* Returned if the command message is not supported */
  kQTMessageNoSuchParameterErr  = -2197, /* Returned if the requested parameter does not exist */
  kQTObsoleteLPCMSoundFormatErr = -2171, /* Returned if the LPCM sound format is incorrectly set to something other than 'lpcm' */
  kQTIncompatibleDescriptionErr = -2166, /* Returned if the operation cannot be performed on this SampleDescription */
  kQTMetaDataInvalidMetaDataErr = -2173, /* Returned if the metadata container is invalid */
  kQTMetaDataInvalidItemErr     = -2174, /* Returned if the metadata item is invalid */
  kQTMetaDataInvalidStorageFormatErr = -2175, /* Returned if the storage format is invalid */
  kQTMetaDataInvalidKeyFormatErr = -2176, /* Returned if the key format is invalid */
  kQTMetaDataNoMoreItemsErr     = -2177 /* Returned if there is no more metadata items */
};


enum {
  kICMCodecCantQueueOutOfOrderErr = -8995
};

enum {
  vdImageDescStaleErr           = -2210, /* The ImageDescription previously returned from VDGetImageDescription is no longer valid */
  vdHardwareGoneErr             = -2211, /* The hardware device providing video frames has been unplugged */
  vdDontHaveThatUniqueIDErr     = -2212 /* The hardware device does not have the ID you are asking for */
};


#endif // !__LP64__


#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif


#endif /* __QUICKTIMEERRORS__ */

