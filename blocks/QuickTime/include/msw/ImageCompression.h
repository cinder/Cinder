/*
     File:       ImageCompression.h
 
     Contains:   QuickTime Image Compression Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1990-2007 by Apple Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __IMAGECOMPRESSION__
#define __IMAGECOMPRESSION__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __QDOFFSCREEN__
#include <QDOffscreen.h>
#endif

#ifndef __COMPONENTS__
#include <Components.h>
#endif

#ifndef __DIALOGS__
#include <Dialogs.h>
#endif

#ifndef __STANDARDFILE__
#include <StandardFile.h>
#endif

#if TARGET_OS_MAC

#include <OpenGL/OpenGL.h>
#include <QuartzCore/CoreVideo.h>

#else

#include <CoreVideo.h>

#endif  /* TARGET_OS_MAC */



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


#ifndef AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER
#define AVAILABLE_MAC_OS_X_VERSION_10_5_AND_LATER       WEAK_IMPORT_ATTRIBUTE
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5
#define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5       AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5
#define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5       AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5
#define AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5       AVAILABLE_MAC_OS_X_VERSION_10_2_AND_LATER
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5
#define AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5       AVAILABLE_MAC_OS_X_VERSION_10_3_AND_LATER
#endif
#ifndef AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5
#define AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER_BUT_DEPRECATED_IN_MAC_OS_X_VERSION_10_5       AVAILABLE_MAC_OS_X_VERSION_10_4_AND_LATER
#endif

/* QuickTime is not available to 64-bit clients */

#if !__LP64__

struct MatrixRecord {
  Fixed               matrix[3][3];
};
typedef struct MatrixRecord             MatrixRecord;
typedef MatrixRecord *                  MatrixRecordPtr;
enum {
  kRawCodecType                 = FOUR_CHAR_CODE('raw '),
  kCinepakCodecType             = FOUR_CHAR_CODE('cvid'),
  kGraphicsCodecType            = FOUR_CHAR_CODE('smc '),
  kAnimationCodecType           = FOUR_CHAR_CODE('rle '),
  kVideoCodecType               = FOUR_CHAR_CODE('rpza'),
  kComponentVideoCodecType      = FOUR_CHAR_CODE('yuv2'),
  kJPEGCodecType                = FOUR_CHAR_CODE('jpeg'),
  kMotionJPEGACodecType         = FOUR_CHAR_CODE('mjpa'),
  kMotionJPEGBCodecType         = FOUR_CHAR_CODE('mjpb'),
  kSGICodecType                 = FOUR_CHAR_CODE('.SGI'),
  kPlanarRGBCodecType           = FOUR_CHAR_CODE('8BPS'),
  kMacPaintCodecType            = FOUR_CHAR_CODE('PNTG'),
  kGIFCodecType                 = FOUR_CHAR_CODE('gif '),
  kPhotoCDCodecType             = FOUR_CHAR_CODE('kpcd'),
  kQuickDrawGXCodecType         = FOUR_CHAR_CODE('qdgx'),
  kAVRJPEGCodecType             = FOUR_CHAR_CODE('avr '),
  kOpenDMLJPEGCodecType         = FOUR_CHAR_CODE('dmb1'),
  kBMPCodecType                 = FOUR_CHAR_CODE('WRLE'),
  kWindowsRawCodecType          = FOUR_CHAR_CODE('WRAW'),
  kVectorCodecType              = FOUR_CHAR_CODE('path'),
  kQuickDrawCodecType           = FOUR_CHAR_CODE('qdrw'),
  kWaterRippleCodecType         = FOUR_CHAR_CODE('ripl'),
  kFireCodecType                = FOUR_CHAR_CODE('fire'),
  kCloudCodecType               = FOUR_CHAR_CODE('clou'),
  kH261CodecType                = FOUR_CHAR_CODE('h261'),
  kH263CodecType                = FOUR_CHAR_CODE('h263'),
  kDVCNTSCCodecType             = FOUR_CHAR_CODE('dvc '), /* DV - NTSC and DVCPRO NTSC (available in QuickTime 6.0 or later)*/
                                        /* NOTE: kDVCProNTSCCodecType is deprecated.  */
                                        /* Use kDVCNTSCCodecType instead -- as far as the codecs are concerned, */
                                        /* the two data formats are identical.*/
  kDVCPALCodecType              = FOUR_CHAR_CODE('dvcp'),
  kDVCProPALCodecType           = FOUR_CHAR_CODE('dvpp'), /* available in QuickTime 6.0 or later*/
  kDVCPro50NTSCCodecType        = FOUR_CHAR_CODE('dv5n'),
  kDVCPro50PALCodecType         = FOUR_CHAR_CODE('dv5p'),
  kDVCPro100NTSCCodecType       = FOUR_CHAR_CODE('dv1n'),
  kDVCPro100PALCodecType        = FOUR_CHAR_CODE('dv1p'),
  kDVCPROHD720pCodecType        = FOUR_CHAR_CODE('dvhp'),
  kDVCPROHD1080i60CodecType     = FOUR_CHAR_CODE('dvh6'),
  kDVCPROHD1080i50CodecType     = FOUR_CHAR_CODE('dvh5'),
  kBaseCodecType                = FOUR_CHAR_CODE('base'),
  kFLCCodecType                 = FOUR_CHAR_CODE('flic'),
  kTargaCodecType               = FOUR_CHAR_CODE('tga '),
  kPNGCodecType                 = FOUR_CHAR_CODE('png '),
  kTIFFCodecType                = FOUR_CHAR_CODE('tiff'), /* NOTE: despite what might seem obvious from the two constants*/
                                        /* below and their names, they really are correct. 'yuvu' really */
                                        /* does mean signed, and 'yuvs' really does mean unsigned. Really. */
  kComponentVideoSigned         = FOUR_CHAR_CODE('yuvu'),
  kComponentVideoUnsigned       = FOUR_CHAR_CODE('yuvs'),
  kCMYKCodecType                = FOUR_CHAR_CODE('cmyk'),
  kMicrosoftVideo1CodecType     = FOUR_CHAR_CODE('msvc'),
  kSorensonCodecType            = FOUR_CHAR_CODE('SVQ1'),
  kSorenson3CodecType           = FOUR_CHAR_CODE('SVQ3'), /* available in QuickTime 5 and later*/
  kIndeo4CodecType              = FOUR_CHAR_CODE('IV41'),
  kMPEG4VisualCodecType         = FOUR_CHAR_CODE('mp4v'),
  k64ARGBCodecType              = FOUR_CHAR_CODE('b64a'),
  k48RGBCodecType               = FOUR_CHAR_CODE('b48r'),
  k32AlphaGrayCodecType         = FOUR_CHAR_CODE('b32a'),
  k16GrayCodecType              = FOUR_CHAR_CODE('b16g'),
  kMpegYUV420CodecType          = FOUR_CHAR_CODE('myuv'),
  kYUV420CodecType              = FOUR_CHAR_CODE('y420'),
  kSorensonYUV9CodecType        = FOUR_CHAR_CODE('syv9'),
  k422YpCbCr8CodecType          = FOUR_CHAR_CODE('2vuy'), /* Component Y'CbCr 8-bit 4:2:2  */
  k444YpCbCr8CodecType          = FOUR_CHAR_CODE('v308'), /* Component Y'CbCr 8-bit 4:4:4  */
  k4444YpCbCrA8CodecType        = FOUR_CHAR_CODE('v408'), /* Component Y'CbCrA 8-bit 4:4:4:4 */
  k422YpCbCr16CodecType         = FOUR_CHAR_CODE('v216'), /* Component Y'CbCr 10,12,14,16-bit 4:2:2*/
  k422YpCbCr10CodecType         = FOUR_CHAR_CODE('v210'), /* Component Y'CbCr 10-bit 4:2:2 */
  k444YpCbCr10CodecType         = FOUR_CHAR_CODE('v410'), /* Component Y'CbCr 10-bit 4:4:4 */
  k4444YpCbCrA8RCodecType       = FOUR_CHAR_CODE('r408'), /* Component Y'CbCrA 8-bit 4:4:4:4, rendering format. full range alpha, zero biased yuv*/
  kJPEG2000CodecType            = FOUR_CHAR_CODE('mjp2'),
  kPixletCodecType              = FOUR_CHAR_CODE('pxlt'),
  kH264CodecType                = FOUR_CHAR_CODE('avc1')
};


/* one source effects */
enum {
  kBlurImageFilterType          = FOUR_CHAR_CODE('blur'),
  kSharpenImageFilterType       = FOUR_CHAR_CODE('shrp'),
  kEdgeDetectImageFilterType    = FOUR_CHAR_CODE('edge'),
  kEmbossImageFilterType        = FOUR_CHAR_CODE('embs'),
  kConvolveImageFilterType      = FOUR_CHAR_CODE('genk'),
  kAlphaGainImageFilterType     = FOUR_CHAR_CODE('gain'),
  kRGBColorBalanceImageFilterType = FOUR_CHAR_CODE('rgbb'),
  kHSLColorBalanceImageFilterType = FOUR_CHAR_CODE('hslb'),
  kColorSyncImageFilterType     = FOUR_CHAR_CODE('sync'),
  kFilmNoiseImageFilterType     = FOUR_CHAR_CODE('fmns'),
  kSolarizeImageFilterType      = FOUR_CHAR_CODE('solr'),
  kColorTintImageFilterType     = FOUR_CHAR_CODE('tint'),
  kLensFlareImageFilterType     = FOUR_CHAR_CODE('lens'),
  kBrightnessContrastImageFilterType = FOUR_CHAR_CODE('brco')
};

/* two source effects */
enum {
  kAlphaCompositorTransitionType = FOUR_CHAR_CODE('blnd'),
  kCrossFadeTransitionType      = FOUR_CHAR_CODE('dslv'),
  kChannelCompositeEffectType   = FOUR_CHAR_CODE('chan'),
  kChromaKeyTransitionType      = FOUR_CHAR_CODE('ckey'),
  kImplodeTransitionType        = FOUR_CHAR_CODE('mplo'),
  kExplodeTransitionType        = FOUR_CHAR_CODE('xplo'),
  kGradientTransitionType       = FOUR_CHAR_CODE('matt'),
  kPushTransitionType           = FOUR_CHAR_CODE('push'),
  kSlideTransitionType          = FOUR_CHAR_CODE('slid'),
  kWipeTransitionType           = FOUR_CHAR_CODE('smpt'),
  kIrisTransitionType           = FOUR_CHAR_CODE('smp2'),
  kRadialTransitionType         = FOUR_CHAR_CODE('smp3'),
  kMatrixTransitionType         = FOUR_CHAR_CODE('smp4'),
  kZoomTransitionType           = FOUR_CHAR_CODE('zoom')
};

/* three source effects */
enum {
  kTravellingMatteEffectType    = FOUR_CHAR_CODE('trav')
};


/* Supported by QTNewGWorld in QuickTime 4.0 and later */
enum {
  kCMYKPixelFormat              = FOUR_CHAR_CODE('cmyk'), /* CMYK, 8-bit */
  k64ARGBPixelFormat            = FOUR_CHAR_CODE('b64a'), /* ARGB, 16-bit big-endian samples */
  k48RGBPixelFormat             = FOUR_CHAR_CODE('b48r'), /* RGB, 16-bit big-endian samples */
  k32AlphaGrayPixelFormat       = FOUR_CHAR_CODE('b32a'), /* AlphaGray, 16-bit big-endian samples */
  k16GrayPixelFormat            = FOUR_CHAR_CODE('b16g'), /* Grayscale, 16-bit big-endian samples */
  k422YpCbCr8PixelFormat        = FOUR_CHAR_CODE('2vuy') /* Component Y'CbCr 8-bit 4:2:2, ordered Cb Y'0 Cr Y'1 */
};

/* Supported by QTNewGWorld in QuickTime 4.1.2 and later */
enum {
  k4444YpCbCrA8PixelFormat      = FOUR_CHAR_CODE('v408'), /* Component Y'CbCrA 8-bit 4:4:4:4, ordered Cb Y' Cr A */
  k4444YpCbCrA8RPixelFormat     = FOUR_CHAR_CODE('r408') /* Component Y'CbCrA 8-bit 4:4:4:4, rendering format. full range alpha, zero biased yuv, ordered A Y' Cb Cr */
};

/* Supported by QTNewGWorld in QuickTime 6.0 and later */
enum {
  kYUV420PixelFormat            = FOUR_CHAR_CODE('y420') /* Planar Component Y'CbCr 8-bit 4:2:0.  PixMap baseAddr points to a big-endian PlanarPixmapInfoYUV420 struct; see ImageCodec.i. */
};


/* These are the bits that are set in the Component flags, and also in the codecInfo struct. */
enum {
  codecInfoDoes1                = (1L << 0), /* codec can work with 1-bit pixels */
  codecInfoDoes2                = (1L << 1), /* codec can work with 2-bit pixels */
  codecInfoDoes4                = (1L << 2), /* codec can work with 4-bit pixels */
  codecInfoDoes8                = (1L << 3), /* codec can work with 8-bit pixels */
  codecInfoDoes16               = (1L << 4), /* codec can work with 16-bit pixels */
  codecInfoDoes32               = (1L << 5), /* codec can work with 32-bit pixels */
  codecInfoDoesDither           = (1L << 6), /* codec can do ditherMode */
  codecInfoDoesStretch          = (1L << 7), /* codec can stretch to arbitrary sizes */
  codecInfoDoesShrink           = (1L << 8), /* codec can shrink to arbitrary sizes */
  codecInfoDoesMask             = (1L << 9), /* codec can mask to clipping regions */
  codecInfoDoesTemporal         = (1L << 10), /* codec can handle temporal redundancy */
  codecInfoDoesDouble           = (1L << 11), /* codec can stretch to double size exactly */
  codecInfoDoesQuad             = (1L << 12), /* codec can stretch to quadruple size exactly */
  codecInfoDoesHalf             = (1L << 13), /* codec can shrink to half size */
  codecInfoDoesQuarter          = (1L << 14), /* codec can shrink to quarter size */
  codecInfoDoesRotate           = (1L << 15), /* codec can rotate on decompress */
  codecInfoDoesHorizFlip        = (1L << 16), /* codec can flip horizontally on decompress */
  codecInfoDoesVertFlip         = (1L << 17), /* codec can flip vertically on decompress */
  codecInfoHasEffectParameterList = (1L << 18), /* codec implements get effects parameter list call, once was codecInfoDoesSkew */
  codecInfoDoesBlend            = (1L << 19), /* codec can blend on decompress */
  codecInfoDoesReorder          = (1L << 19), /* codec can rearrange frames during compression */
  codecInfoDoesWarp             = (1L << 20), /* codec can warp arbitrarily on decompress */
  codecInfoDoesMultiPass        = (1L << 20), /* codec can perform multi-pass compression */
  codecInfoDoesRecompress       = (1L << 21), /* codec can recompress image without accumulating errors */
  codecInfoDoesSpool            = (1L << 22), /* codec can spool image data */
  codecInfoDoesRateConstrain    = (1L << 23) /* codec can data rate constrain */
};


enum {
  codecInfoDepth1               = (1L << 0), /* compressed data at 1 bpp depth available */
  codecInfoDepth2               = (1L << 1), /* compressed data at 2 bpp depth available */
  codecInfoDepth4               = (1L << 2), /* compressed data at 4 bpp depth available */
  codecInfoDepth8               = (1L << 3), /* compressed data at 8 bpp depth available */
  codecInfoDepth16              = (1L << 4), /* compressed data at 16 bpp depth available */
  codecInfoDepth32              = (1L << 5), /* compressed data at 32 bpp depth available */
  codecInfoDepth24              = (1L << 6), /* compressed data at 24 bpp depth available */
  codecInfoDepth33              = (1L << 7), /* compressed data at 1 bpp monochrome depth  available */
  codecInfoDepth34              = (1L << 8), /* compressed data at 2 bpp grayscale depth available */
  codecInfoDepth36              = (1L << 9), /* compressed data at 4 bpp grayscale depth available */
  codecInfoDepth40              = (1L << 10), /* compressed data at 8 bpp grayscale depth available */
  codecInfoStoresClut           = (1L << 11), /* compressed data can have custom cluts */
  codecInfoDoesLossless         = (1L << 12), /* compressed data can be stored in lossless format */
  codecInfoSequenceSensitive    = (1L << 13) /* compressed data is sensitive to out of sequence decoding */
};


/* input sequence flags*/
enum {
  codecFlagUseImageBuffer       = (1L << 0), /* decompress*/
  codecFlagUseScreenBuffer      = (1L << 1), /* decompress*/
  codecFlagUpdatePrevious       = (1L << 2), /* compress*/
  codecFlagNoScreenUpdate       = (1L << 3), /* decompress*/
  codecFlagWasCompressed        = (1L << 4), /* compress*/
  codecFlagDontOffscreen        = (1L << 5), /* decompress*/
  codecFlagUpdatePreviousComp   = (1L << 6), /* compress*/
  codecFlagForceKeyFrame        = (1L << 7), /* compress*/
  codecFlagOnlyScreenUpdate     = (1L << 8), /* decompress*/
  codecFlagLiveGrab             = (1L << 9), /* compress*/
  codecFlagDiffFrame            = (1L << 9), /* decompress*/
  codecFlagDontUseNewImageBuffer = (1L << 10), /* decompress*/
  codecFlagInterlaceUpdate      = (1L << 11), /* decompress*/
  codecFlagCatchUpDiff          = (1L << 12), /* decompress*/
  codecFlagSupportDisable       = (1L << 13), /* decompress*/
  codecFlagReenable             = (1L << 14) /* decompress*/
};


/* output sequence flags*/
enum {
  codecFlagOutUpdateOnNextIdle  = (1L << 9),
  codecFlagOutUpdateOnDataSourceChange = (1L << 10),
  codecFlagSequenceSensitive    = (1L << 11),
  codecFlagOutUpdateOnTimeChange = (1L << 12),
  codecFlagImageBufferNotSourceImage = (1L << 13),
  codecFlagUsedNewImageBuffer   = (1L << 14),
  codecFlagUsedImageBuffer      = (1L << 15)
};



enum {
                                        /* The minimum data size for spooling in or out data */
  codecMinimumDataSize          = 32768L
};



enum {
  compressorComponentType       = FOUR_CHAR_CODE('imco'), /* the type for "Components" which compress images */
  decompressorComponentType     = FOUR_CHAR_CODE('imdc') /* the type for "Components" which decompress images */
};

typedef Component                       CompressorComponent;
typedef Component                       DecompressorComponent;
typedef Component                       CodecComponent;
#define anyCodec                ((CodecComponent)0)
#define bestSpeedCodec          ((CodecComponent)-1)
#define bestFidelityCodec       ((CodecComponent)-2)
#define bestCompressionCodec    ((CodecComponent)-3)
typedef OSType                          CodecType;
typedef unsigned short                  CodecFlags;
typedef unsigned long                   CodecQ;
enum {
  codecLosslessQuality          = 0x00000400,
  codecMaxQuality               = 0x000003FF,
  codecMinQuality               = 0x00000000,
  codecLowQuality               = 0x00000100,
  codecNormalQuality            = 0x00000200,
  codecHighQuality              = 0x00000300
};

enum {
  codecLockBitsShieldCursor     = (1 << 0) /* shield cursor */
};

enum {
  codecCompletionSource         = (1 << 0), /* asynchronous codec is done with source data */
  codecCompletionDest           = (1 << 1), /* asynchronous codec is done with destination data */
  codecCompletionDontUnshield   = (1 << 2), /* on dest complete don't unshield cursor */
  codecCompletionWentOffscreen  = (1 << 3), /* codec used offscreen buffer */
  codecCompletionUnlockBits     = (1 << 4), /* on dest complete, call ICMSequenceUnlockBits */
  codecCompletionForceChainFlush = (1 << 5), /* ICM needs to flush the whole chain */
  codecCompletionDropped        = (1 << 6), /* codec decided to drop this frame */
  codecCompletionDecoded        = (1 << 10), /* codec has decoded this frame; if it is cancelled and rescheduled, set icmFrameAlreadyDecoded in ICMFrameTimeRecord.flags */
  codecCompletionNotDisplayable = (1 << 11), /* the frame may still be scheduled for decode, but will not be able to be displayed because the buffer containing it will need to be recycled to display earlier frames. */
  codecCompletionNotDrawn       = (1 << 12) /* set in conjunction with codecCompletionDest to indicate that the frame was not drawn */
};

enum {
  codecProgressOpen             = 0,
  codecProgressUpdatePercent    = 1,
  codecProgressClose            = 2
};

typedef CALLBACK_API( OSErr , ICMDataProcPtr )(Ptr *dataP, long bytesNeeded, long refcon);
typedef CALLBACK_API( OSErr , ICMFlushProcPtr )(Ptr data, long bytesAdded, long refcon);
typedef CALLBACK_API( void , ICMCompletionProcPtr )(OSErr result, short flags, long refcon);
typedef CALLBACK_API( OSErr , ICMProgressProcPtr )(short message, Fixed completeness, long refcon);
typedef CALLBACK_API( void , StdPixProcPtr )(PixMap *src, Rect *srcRect, MatrixRecord *matrix, short mode, RgnHandle mask, PixMap *matte, Rect *matteRect, short flags);
typedef CALLBACK_API( void , QDPixProcPtr )(PixMap *src, Rect *srcRect, MatrixRecord *matrix, short mode, RgnHandle mask, PixMap *matte, Rect *matteRect, short flags);
typedef CALLBACK_API( void , ICMAlignmentProcPtr )(Rect *rp, long refcon);
typedef CALLBACK_API( void , ICMCursorShieldedProcPtr )(const Rect *r, void *refcon, long flags);
typedef CALLBACK_API( void , ICMMemoryDisposedProcPtr )(Ptr memoryBlock, void *refcon);
typedef void *                          ICMCursorNotify;
typedef CALLBACK_API( OSErr , ICMConvertDataFormatProcPtr )(void *refCon, long flags, Handle desiredFormat, Handle sourceDataFormat, void *srcData, long srcDataSize, void **dstData, long *dstDataSize);
typedef STACK_UPP_TYPE(ICMDataProcPtr)                          ICMDataUPP;
typedef STACK_UPP_TYPE(ICMFlushProcPtr)                         ICMFlushUPP;
typedef STACK_UPP_TYPE(ICMCompletionProcPtr)                    ICMCompletionUPP;
typedef STACK_UPP_TYPE(ICMProgressProcPtr)                      ICMProgressUPP;
typedef STACK_UPP_TYPE(StdPixProcPtr)                           StdPixUPP;
typedef STACK_UPP_TYPE(QDPixProcPtr)                            QDPixUPP;
typedef STACK_UPP_TYPE(ICMAlignmentProcPtr)                     ICMAlignmentUPP;
typedef STACK_UPP_TYPE(ICMCursorShieldedProcPtr)                ICMCursorShieldedUPP;
typedef STACK_UPP_TYPE(ICMMemoryDisposedProcPtr)                ICMMemoryDisposedUPP;
typedef STACK_UPP_TYPE(ICMConvertDataFormatProcPtr)             ICMConvertDataFormatUPP;
typedef long                            ImageSequence;
typedef long                            ImageSequenceDataSource;
typedef long                            ImageTranscodeSequence;
typedef long                            ImageFieldSequence;
struct ICMProgressProcRecord {
  ICMProgressUPP      progressProc;
  long                progressRefCon;
};
typedef struct ICMProgressProcRecord    ICMProgressProcRecord;
typedef ICMProgressProcRecord *         ICMProgressProcRecordPtr;
struct ICMCompletionProcRecord {
  ICMCompletionUPP    completionProc;
  long                completionRefCon;
};
typedef struct ICMCompletionProcRecord  ICMCompletionProcRecord;
typedef ICMCompletionProcRecord *       ICMCompletionProcRecordPtr;
struct ICMDataProcRecord {
  ICMDataUPP          dataProc;
  long                dataRefCon;
};
typedef struct ICMDataProcRecord        ICMDataProcRecord;
typedef ICMDataProcRecord *             ICMDataProcRecordPtr;
struct ICMFlushProcRecord {
  ICMFlushUPP         flushProc;
  long                flushRefCon;
};
typedef struct ICMFlushProcRecord       ICMFlushProcRecord;
typedef ICMFlushProcRecord *            ICMFlushProcRecordPtr;
struct ICMAlignmentProcRecord {
  ICMAlignmentUPP     alignmentProc;
  long                alignmentRefCon;
};
typedef struct ICMAlignmentProcRecord   ICMAlignmentProcRecord;
typedef ICMAlignmentProcRecord *        ICMAlignmentProcRecordPtr;
struct DataRateParams {
  long                dataRate;
  long                dataOverrun;
  long                frameDuration;
  long                keyFrameRate;
  CodecQ              minSpatialQuality;
  CodecQ              minTemporalQuality;
};
typedef struct DataRateParams           DataRateParams;
typedef DataRateParams *                DataRateParamsPtr;
struct ImageDescription {
  long                idSize;                 /* total size of ImageDescription including extra data ( CLUTs and other per sequence data ) */
  CodecType           cType;                  /* what kind of codec compressed this data */
  long                resvd1;                 /* reserved for Apple use */
  short               resvd2;                 /* reserved for Apple use */
  short               dataRefIndex;           /* set to zero  */
  short               version;                /* which version is this data */
  short               revisionLevel;          /* what version of that codec did this */
  long                vendor;                 /* whose  codec compressed this data */
  CodecQ              temporalQuality;        /* what was the temporal quality factor  */
  CodecQ              spatialQuality;         /* what was the spatial quality factor */
  short               width;                  /* how many pixels wide is this data */
  short               height;                 /* how many pixels high is this data */
  Fixed               hRes;                   /* horizontal resolution */
  Fixed               vRes;                   /* vertical resolution */
  long                dataSize;               /* if known, the size of data for this image descriptor */
  short               frameCount;             /* number of frames this description applies to */
  Str31               name;                   /* name of codec ( in case not installed )  */
  short               depth;                  /* what depth is this data (1-32) or ( 33-40 grayscale ) */
  short               clutID;                 /* clut id or if 0 clut follows  or -1 if no clut */
};
typedef struct ImageDescription         ImageDescription;
typedef ImageDescription *              ImageDescriptionPtr;
typedef ImageDescriptionPtr *           ImageDescriptionHandle;
struct CodecInfo {
  Str31               typeName;               /* name of the codec type i.e.: 'Apple Image Compression' */
  short               version;                /* version of the codec data that this codec knows about */
  short               revisionLevel;          /* revision level of this codec i.e: 0x00010001 (1.0.1) */
  long                vendor;                 /* Maker of this codec i.e: 'appl' */
  long                decompressFlags;        /* codecInfo flags for decompression capabilities */
  long                compressFlags;          /* codecInfo flags for compression capabilities */
  long                formatFlags;            /* codecInfo flags for compression format details */
  UInt8               compressionAccuracy;    /* measure (1-255) of accuracy of this codec for compress (0 if unknown) */
  UInt8               decompressionAccuracy;  /* measure (1-255) of accuracy of this codec for decompress (0 if unknown) */
  unsigned short      compressionSpeed;       /* ( millisecs for compressing 320x240 on base mac II) (0 if unknown)  */
  unsigned short      decompressionSpeed;     /* ( millisecs for decompressing 320x240 on mac II)(0 if unknown)  */
  UInt8               compressionLevel;       /* measure (1-255) of compression level of this codec (0 if unknown)  */
  UInt8               resvd;                  /* pad */
  short               minimumHeight;          /* minimum height of image (block size) */
  short               minimumWidth;           /* minimum width of image (block size) */
  short               decompressPipelineLatency; /* in milliseconds ( for asynchronous codecs ) */
  short               compressPipelineLatency; /* in milliseconds ( for asynchronous codecs ) */
  long                privateData;
};
typedef struct CodecInfo                CodecInfo;
struct CodecNameSpec {
  CodecComponent      codec;
  CodecType           cType;
  Str31               typeName;
  Handle              name;
};
typedef struct CodecNameSpec            CodecNameSpec;
struct CodecNameSpecList {
  short               count;
  CodecNameSpec       list[1];
};
typedef struct CodecNameSpecList        CodecNameSpecList;
typedef CodecNameSpecList *             CodecNameSpecListPtr;
enum {
  defaultDither                 = 0,
  forceDither                   = 1,
  suppressDither                = 2,
  useColorMatching              = 4
};

enum {
  callStdBits                   = 1,
  callOldBits                   = 2,
  noDefaultOpcodes              = 4
};

enum {
  graphicsModeStraightAlpha     = 256,
  graphicsModePreWhiteAlpha     = 257,
  graphicsModePreBlackAlpha     = 258,
  graphicsModeComposition       = 259,
  graphicsModeStraightAlphaBlend = 260,
  graphicsModePreMulColorAlpha  = 261,
  graphicsModePerComponentAlpha = 272
};

enum {
  evenField1ToEvenFieldOut      = 1 << 0,
  evenField1ToOddFieldOut       = 1 << 1,
  oddField1ToEvenFieldOut       = 1 << 2,
  oddField1ToOddFieldOut        = 1 << 3,
  evenField2ToEvenFieldOut      = 1 << 4,
  evenField2ToOddFieldOut       = 1 << 5,
  oddField2ToEvenFieldOut       = 1 << 6,
  oddField2ToOddFieldOut        = 1 << 7
};

/* Flags for ICMFrameTimeRecord.flags */
enum {
  icmFrameTimeHasVirtualStartTimeAndDuration = 1 << 0,
  icmFrameAlreadyDecoded        = 1 << 1,
  icmFrameTimeIsNonScheduledDisplayTime = 1 << 2,
  icmFrameTimeHasDecodeTime     = 1 << 3,
  icmFrameTimeDecodeImmediately = 1 << 4,
  icmFrameTimeDoNotDisplay      = 1 << 5
};

struct ICMFrameTimeRecord {
  wide                value;                  /* frame display time*/
  long                scale;                  /* timescale of value/duration fields*/
  void *              base;                   /* timebase*/

  long                duration;               /* duration frame is to be displayed (0 if unknown)*/
  Fixed               rate;                   /* rate of timebase relative to wall-time*/

  long                recordSize;             /* total number of bytes in ICMFrameTimeRecord*/

  long                frameNumber;            /* number of frame, zero if not known*/

  long                flags;

  wide                virtualStartTime;       /* conceptual start time*/
  long                virtualDuration;        /* conceptual duration*/

                                              /* The following fields only exist for QuickTime 7.0 and greater. */
  TimeValue64         decodeTime;             /* suggested decode time, if icmFrameTimeHasDecodeTime is set in flags*/
};
typedef struct ICMFrameTimeRecord       ICMFrameTimeRecord;
typedef ICMFrameTimeRecord *            ICMFrameTimePtr;
#ifndef __QTUUID__
#define __QTUUID__ 1
/* QuickTime flavor of universally unique identifier (uuid)*/
struct QTUUID {
  UInt32              data1;
  UInt16              data2;
  UInt16              data3;
  UInt8               data4[8];
};
typedef struct QTUUID                   QTUUID;
typedef QTUUID                          QTMediaContextID;
#endif  /* !defined(__QTUUID__) */

/* See Movies.h for the flags themselves. */
typedef UInt32                          MediaSampleFlags;
/*
 *  NewICMDataUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ICMDataUPP )
NewICMDataUPP(ICMDataProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppICMDataProcInfo = 0x00000FE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ICMDataUPP) NewICMDataUPP(ICMDataProcPtr userRoutine) { return (ICMDataUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMDataProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewICMDataUPP(userRoutine) (ICMDataUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMDataProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewICMFlushUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ICMFlushUPP )
NewICMFlushUPP(ICMFlushProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppICMFlushProcInfo = 0x00000FE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ICMFlushUPP) NewICMFlushUPP(ICMFlushProcPtr userRoutine) { return (ICMFlushUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMFlushProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewICMFlushUPP(userRoutine) (ICMFlushUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMFlushProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewICMCompletionUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ICMCompletionUPP )
NewICMCompletionUPP(ICMCompletionProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppICMCompletionProcInfo = 0x00000E80 };  /* pascal no_return_value Func(2_bytes, 2_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ICMCompletionUPP) NewICMCompletionUPP(ICMCompletionProcPtr userRoutine) { return (ICMCompletionUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMCompletionProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewICMCompletionUPP(userRoutine) (ICMCompletionUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMCompletionProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewICMProgressUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ICMProgressUPP )
NewICMProgressUPP(ICMProgressProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppICMProgressProcInfo = 0x00000FA0 };  /* pascal 2_bytes Func(2_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ICMProgressUPP) NewICMProgressUPP(ICMProgressProcPtr userRoutine) { return (ICMProgressUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMProgressProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewICMProgressUPP(userRoutine) (ICMProgressUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMProgressProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewStdPixUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( StdPixUPP )
NewStdPixUPP(StdPixProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppStdPixProcInfo = 0x002FEFC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes, 2_bytes, 4_bytes, 4_bytes, 4_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(StdPixUPP) NewStdPixUPP(StdPixProcPtr userRoutine) { return (StdPixUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppStdPixProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewStdPixUPP(userRoutine) (StdPixUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppStdPixProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDPixUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDPixUPP )
NewQDPixUPP(QDPixProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDPixProcInfo = 0x002FEFC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes, 2_bytes, 4_bytes, 4_bytes, 4_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDPixUPP) NewQDPixUPP(QDPixProcPtr userRoutine) { return (QDPixUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDPixProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDPixUPP(userRoutine) (QDPixUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDPixProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewICMAlignmentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ICMAlignmentUPP )
NewICMAlignmentUPP(ICMAlignmentProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppICMAlignmentProcInfo = 0x000003C0 };  /* pascal no_return_value Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ICMAlignmentUPP) NewICMAlignmentUPP(ICMAlignmentProcPtr userRoutine) { return (ICMAlignmentUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMAlignmentProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewICMAlignmentUPP(userRoutine) (ICMAlignmentUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMAlignmentProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewICMCursorShieldedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ICMCursorShieldedUPP )
NewICMCursorShieldedUPP(ICMCursorShieldedProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppICMCursorShieldedProcInfo = 0x00000FC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ICMCursorShieldedUPP) NewICMCursorShieldedUPP(ICMCursorShieldedProcPtr userRoutine) { return (ICMCursorShieldedUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMCursorShieldedProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewICMCursorShieldedUPP(userRoutine) (ICMCursorShieldedUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMCursorShieldedProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewICMMemoryDisposedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ICMMemoryDisposedUPP )
NewICMMemoryDisposedUPP(ICMMemoryDisposedProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppICMMemoryDisposedProcInfo = 0x000003C0 };  /* pascal no_return_value Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ICMMemoryDisposedUPP) NewICMMemoryDisposedUPP(ICMMemoryDisposedProcPtr userRoutine) { return (ICMMemoryDisposedUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMMemoryDisposedProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewICMMemoryDisposedUPP(userRoutine) (ICMMemoryDisposedUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMMemoryDisposedProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewICMConvertDataFormatUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ICMConvertDataFormatUPP )
NewICMConvertDataFormatUPP(ICMConvertDataFormatProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppICMConvertDataFormatProcInfo = 0x003FFFE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ICMConvertDataFormatUPP) NewICMConvertDataFormatUPP(ICMConvertDataFormatProcPtr userRoutine) { return (ICMConvertDataFormatUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMConvertDataFormatProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewICMConvertDataFormatUPP(userRoutine) (ICMConvertDataFormatUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppICMConvertDataFormatProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeICMDataUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeICMDataUPP(ICMDataUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeICMDataUPP(ICMDataUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeICMDataUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeICMFlushUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeICMFlushUPP(ICMFlushUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeICMFlushUPP(ICMFlushUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeICMFlushUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeICMCompletionUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeICMCompletionUPP(ICMCompletionUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeICMCompletionUPP(ICMCompletionUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeICMCompletionUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeICMProgressUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeICMProgressUPP(ICMProgressUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeICMProgressUPP(ICMProgressUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeICMProgressUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeStdPixUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeStdPixUPP(StdPixUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeStdPixUPP(StdPixUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeStdPixUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDPixUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDPixUPP(QDPixUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDPixUPP(QDPixUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDPixUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeICMAlignmentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeICMAlignmentUPP(ICMAlignmentUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeICMAlignmentUPP(ICMAlignmentUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeICMAlignmentUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeICMCursorShieldedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeICMCursorShieldedUPP(ICMCursorShieldedUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeICMCursorShieldedUPP(ICMCursorShieldedUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeICMCursorShieldedUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeICMMemoryDisposedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeICMMemoryDisposedUPP(ICMMemoryDisposedUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeICMMemoryDisposedUPP(ICMMemoryDisposedUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeICMMemoryDisposedUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeICMConvertDataFormatUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeICMConvertDataFormatUPP(ICMConvertDataFormatUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeICMConvertDataFormatUPP(ICMConvertDataFormatUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeICMConvertDataFormatUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeICMDataUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeICMDataUPP(
  Ptr *       dataP,
  long        bytesNeeded,
  long        refcon,
  ICMDataUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeICMDataUPP(Ptr * dataP, long bytesNeeded, long refcon, ICMDataUPP userUPP) { return (OSErr)CALL_THREE_PARAMETER_UPP(userUPP, uppICMDataProcInfo, dataP, bytesNeeded, refcon); }
  #else
    #define InvokeICMDataUPP(dataP, bytesNeeded, refcon, userUPP) (OSErr)CALL_THREE_PARAMETER_UPP((userUPP), uppICMDataProcInfo, (dataP), (bytesNeeded), (refcon))
  #endif
#endif

/*
 *  InvokeICMFlushUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeICMFlushUPP(
  Ptr          data,
  long         bytesAdded,
  long         refcon,
  ICMFlushUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeICMFlushUPP(Ptr data, long bytesAdded, long refcon, ICMFlushUPP userUPP) { return (OSErr)CALL_THREE_PARAMETER_UPP(userUPP, uppICMFlushProcInfo, data, bytesAdded, refcon); }
  #else
    #define InvokeICMFlushUPP(data, bytesAdded, refcon, userUPP) (OSErr)CALL_THREE_PARAMETER_UPP((userUPP), uppICMFlushProcInfo, (data), (bytesAdded), (refcon))
  #endif
#endif

/*
 *  InvokeICMCompletionUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeICMCompletionUPP(
  OSErr             result,
  short             flags,
  long              refcon,
  ICMCompletionUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeICMCompletionUPP(OSErr result, short flags, long refcon, ICMCompletionUPP userUPP) { CALL_THREE_PARAMETER_UPP(userUPP, uppICMCompletionProcInfo, result, flags, refcon); }
  #else
    #define InvokeICMCompletionUPP(result, flags, refcon, userUPP) CALL_THREE_PARAMETER_UPP((userUPP), uppICMCompletionProcInfo, (result), (flags), (refcon))
  #endif
#endif

/*
 *  InvokeICMProgressUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeICMProgressUPP(
  short           message,
  Fixed           completeness,
  long            refcon,
  ICMProgressUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeICMProgressUPP(short message, Fixed completeness, long refcon, ICMProgressUPP userUPP) { return (OSErr)CALL_THREE_PARAMETER_UPP(userUPP, uppICMProgressProcInfo, message, completeness, refcon); }
  #else
    #define InvokeICMProgressUPP(message, completeness, refcon, userUPP) (OSErr)CALL_THREE_PARAMETER_UPP((userUPP), uppICMProgressProcInfo, (message), (completeness), (refcon))
  #endif
#endif

/*
 *  InvokeStdPixUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeStdPixUPP(
  PixMap *        src,
  Rect *          srcRect,
  MatrixRecord *  matrix,
  short           mode,
  RgnHandle       mask,
  PixMap *        matte,
  Rect *          matteRect,
  short           flags,
  StdPixUPP       userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeStdPixUPP(PixMap * src, Rect * srcRect, MatrixRecord * matrix, short mode, RgnHandle mask, PixMap * matte, Rect * matteRect, short flags, StdPixUPP userUPP) { CALL_EIGHT_PARAMETER_UPP(userUPP, uppStdPixProcInfo, src, srcRect, matrix, mode, mask, matte, matteRect, flags); }
  #else
    #define InvokeStdPixUPP(src, srcRect, matrix, mode, mask, matte, matteRect, flags, userUPP) CALL_EIGHT_PARAMETER_UPP((userUPP), uppStdPixProcInfo, (src), (srcRect), (matrix), (mode), (mask), (matte), (matteRect), (flags))
  #endif
#endif

/*
 *  InvokeQDPixUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDPixUPP(
  PixMap *        src,
  Rect *          srcRect,
  MatrixRecord *  matrix,
  short           mode,
  RgnHandle       mask,
  PixMap *        matte,
  Rect *          matteRect,
  short           flags,
  QDPixUPP        userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDPixUPP(PixMap * src, Rect * srcRect, MatrixRecord * matrix, short mode, RgnHandle mask, PixMap * matte, Rect * matteRect, short flags, QDPixUPP userUPP) { CALL_EIGHT_PARAMETER_UPP(userUPP, uppQDPixProcInfo, src, srcRect, matrix, mode, mask, matte, matteRect, flags); }
  #else
    #define InvokeQDPixUPP(src, srcRect, matrix, mode, mask, matte, matteRect, flags, userUPP) CALL_EIGHT_PARAMETER_UPP((userUPP), uppQDPixProcInfo, (src), (srcRect), (matrix), (mode), (mask), (matte), (matteRect), (flags))
  #endif
#endif

/*
 *  InvokeICMAlignmentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeICMAlignmentUPP(
  Rect *           rp,
  long             refcon,
  ICMAlignmentUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeICMAlignmentUPP(Rect * rp, long refcon, ICMAlignmentUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppICMAlignmentProcInfo, rp, refcon); }
  #else
    #define InvokeICMAlignmentUPP(rp, refcon, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppICMAlignmentProcInfo, (rp), (refcon))
  #endif
#endif

/*
 *  InvokeICMCursorShieldedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeICMCursorShieldedUPP(
  const Rect *          r,
  void *                refcon,
  long                  flags,
  ICMCursorShieldedUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeICMCursorShieldedUPP(const Rect * r, void * refcon, long flags, ICMCursorShieldedUPP userUPP) { CALL_THREE_PARAMETER_UPP(userUPP, uppICMCursorShieldedProcInfo, r, refcon, flags); }
  #else
    #define InvokeICMCursorShieldedUPP(r, refcon, flags, userUPP) CALL_THREE_PARAMETER_UPP((userUPP), uppICMCursorShieldedProcInfo, (r), (refcon), (flags))
  #endif
#endif

/*
 *  InvokeICMMemoryDisposedUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeICMMemoryDisposedUPP(
  Ptr                   memoryBlock,
  void *                refcon,
  ICMMemoryDisposedUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeICMMemoryDisposedUPP(Ptr memoryBlock, void * refcon, ICMMemoryDisposedUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppICMMemoryDisposedProcInfo, memoryBlock, refcon); }
  #else
    #define InvokeICMMemoryDisposedUPP(memoryBlock, refcon, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppICMMemoryDisposedProcInfo, (memoryBlock), (refcon))
  #endif
#endif

/*
 *  InvokeICMConvertDataFormatUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeICMConvertDataFormatUPP(
  void *                   refCon,
  long                     flags,
  Handle                   desiredFormat,
  Handle                   sourceDataFormat,
  void *                   srcData,
  long                     srcDataSize,
  void **                  dstData,
  long *                   dstDataSize,
  ICMConvertDataFormatUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeICMConvertDataFormatUPP(void * refCon, long flags, Handle desiredFormat, Handle sourceDataFormat, void * srcData, long srcDataSize, void ** dstData, long * dstDataSize, ICMConvertDataFormatUPP userUPP) { return (OSErr)CALL_EIGHT_PARAMETER_UPP(userUPP, uppICMConvertDataFormatProcInfo, refCon, flags, desiredFormat, sourceDataFormat, srcData, srcDataSize, dstData, dstDataSize); }
  #else
    #define InvokeICMConvertDataFormatUPP(refCon, flags, desiredFormat, sourceDataFormat, srcData, srcDataSize, dstData, dstDataSize, userUPP) (OSErr)CALL_EIGHT_PARAMETER_UPP((userUPP), uppICMConvertDataFormatProcInfo, (refCon), (flags), (desiredFormat), (sourceDataFormat), (srcData), (srcDataSize), (dstData), (dstDataSize))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewICMDataProc(userRoutine)                         NewICMDataUPP(userRoutine)
    #define NewICMFlushProc(userRoutine)                        NewICMFlushUPP(userRoutine)
    #define NewICMCompletionProc(userRoutine)                   NewICMCompletionUPP(userRoutine)
    #define NewICMProgressProc(userRoutine)                     NewICMProgressUPP(userRoutine)
    #define NewStdPixProc(userRoutine)                          NewStdPixUPP(userRoutine)
    #define NewQDPixProc(userRoutine)                           NewQDPixUPP(userRoutine)
    #define NewICMAlignmentProc(userRoutine)                    NewICMAlignmentUPP(userRoutine)
    #define NewICMCursorShieldedProc(userRoutine)               NewICMCursorShieldedUPP(userRoutine)
    #define NewICMMemoryDisposedProc(userRoutine)               NewICMMemoryDisposedUPP(userRoutine)
    #define NewICMConvertDataFormatProc(userRoutine)            NewICMConvertDataFormatUPP(userRoutine)
    #define CallICMDataProc(userRoutine, dataP, bytesNeeded, refcon) InvokeICMDataUPP(dataP, bytesNeeded, refcon, userRoutine)
    #define CallICMFlushProc(userRoutine, data, bytesAdded, refcon) InvokeICMFlushUPP(data, bytesAdded, refcon, userRoutine)
    #define CallICMCompletionProc(userRoutine, result, flags, refcon) InvokeICMCompletionUPP(result, flags, refcon, userRoutine)
    #define CallICMProgressProc(userRoutine, message, completeness, refcon) InvokeICMProgressUPP(message, completeness, refcon, userRoutine)
    #define CallStdPixProc(userRoutine, src, srcRect, matrix, mode, mask, matte, matteRect, flags) InvokeStdPixUPP(src, srcRect, matrix, mode, mask, matte, matteRect, flags, userRoutine)
    #define CallQDPixProc(userRoutine, src, srcRect, matrix, mode, mask, matte, matteRect, flags) InvokeQDPixUPP(src, srcRect, matrix, mode, mask, matte, matteRect, flags, userRoutine)
    #define CallICMAlignmentProc(userRoutine, rp, refcon)       InvokeICMAlignmentUPP(rp, refcon, userRoutine)
    #define CallICMCursorShieldedProc(userRoutine, r, refcon, flags) InvokeICMCursorShieldedUPP(r, refcon, flags, userRoutine)
    #define CallICMMemoryDisposedProc(userRoutine, memoryBlock, refcon) InvokeICMMemoryDisposedUPP(memoryBlock, refcon, userRoutine)
    #define CallICMConvertDataFormatProc(userRoutine, refCon, flags, desiredFormat, sourceDataFormat, srcData, srcDataSize, dstData, dstDataSize) InvokeICMConvertDataFormatUPP(refCon, flags, desiredFormat, sourceDataFormat, srcData, srcDataSize, dstData, dstDataSize, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*
 *  CodecManagerVersion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CodecManagerVersion(long * version);


/*
 *  GetCodecNameList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetCodecNameList(
  CodecNameSpecListPtr *  list,
  short                   showAll);


/*
 *  DisposeCodecNameList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
DisposeCodecNameList(CodecNameSpecListPtr list);


/*
 *  GetCodecInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetCodecInfo(
  CodecInfo *      info,
  CodecType        cType,
  CodecComponent   codec);


/*
 *  GetMaxCompressionSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetMaxCompressionSize(
  PixMapHandle          src,
  const Rect *          srcRect,
  short                 colorDepth,
  CodecQ                quality,
  CodecType             cType,
  CompressorComponent   codec,
  long *                size);


/*
 *  GetCSequenceMaxCompressionSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetCSequenceMaxCompressionSize(
  ImageSequence   seqID,
  PixMapHandle    src,
  long *          size);


/*
 *  GetCompressionTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetCompressionTime(
  PixMapHandle          src,
  const Rect *          srcRect,
  short                 colorDepth,
  CodecType             cType,
  CompressorComponent   codec,
  CodecQ *              spatialQuality,
  CodecQ *              temporalQuality,
  unsigned long *       compressTime);


/*
 *  CompressImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CompressImage(
  PixMapHandle             src,
  const Rect *             srcRect,
  CodecQ                   quality,
  CodecType                cType,
  ImageDescriptionHandle   desc,
  Ptr                      data);


/*
 *  FCompressImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
FCompressImage(
  PixMapHandle               src,
  const Rect *               srcRect,
  short                      colorDepth,
  CodecQ                     quality,
  CodecType                  cType,
  CompressorComponent        codec,
  CTabHandle                 ctable,
  CodecFlags                 flags,
  long                       bufferSize,
  ICMFlushProcRecordPtr      flushProc,
  ICMProgressProcRecordPtr   progressProc,
  ImageDescriptionHandle     desc,
  Ptr                        data);


/*
 *  DecompressImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
DecompressImage(
  Ptr                      data,
  ImageDescriptionHandle   desc,
  PixMapHandle             dst,
  const Rect *             srcRect,
  const Rect *             dstRect,
  short                    mode,
  RgnHandle                mask);


/*
 *  FDecompressImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
FDecompressImage(
  Ptr                        data,
  ImageDescriptionHandle     desc,
  PixMapHandle               dst,
  const Rect *               srcRect,
  MatrixRecordPtr            matrix,
  short                      mode,
  RgnHandle                  mask,
  PixMapHandle               matte,
  const Rect *               matteRect,
  CodecQ                     accuracy,
  DecompressorComponent      codec,
  long                       bufferSize,
  ICMDataProcRecordPtr       dataProc,
  ICMProgressProcRecordPtr   progressProc);


/* For video compression, consider using ICMCompressionSessionCreate etc. instead of CompressSequenceBegin etc. */
/*
 *  CompressSequenceBegin()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CompressSequenceBegin(
  ImageSequence *          seqID,
  PixMapHandle             src,
  PixMapHandle             prev,
  const Rect *             srcRect,
  const Rect *             prevRect,
  short                    colorDepth,
  CodecType                cType,
  CompressorComponent      codec,
  CodecQ                   spatialQuality,
  CodecQ                   temporalQuality,
  long                     keyFrameRate,
  CTabHandle               ctable,
  CodecFlags               flags,
  ImageDescriptionHandle   desc);


/*
 *  CompressSequenceFrame()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CompressSequenceFrame(
  ImageSequence                seqID,
  PixMapHandle                 src,
  const Rect *                 srcRect,
  CodecFlags                   flags,
  Ptr                          data,
  long *                       dataSize,
  UInt8 *                      similarity,
  ICMCompletionProcRecordPtr   asyncCompletionProc);


/* For video decompression, consider using ICMDecompressionSessionCreate etc. instead of DecompressSequenceBegin etc. */
/*
 *  DecompressSequenceBegin()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
DecompressSequenceBegin(
  ImageSequence *          seqID,
  ImageDescriptionHandle   desc,
  CGrafPtr                 port,
  GDHandle                 gdh,
  const Rect *             srcRect,
  MatrixRecordPtr          matrix,
  short                    mode,
  RgnHandle                mask,
  CodecFlags               flags,
  CodecQ                   accuracy,
  DecompressorComponent    codec);


/* For video decompression, consider using ICMDecompressionSessionCreate etc. instead of DecompressSequenceBeginS etc. */
/*
 *  DecompressSequenceBeginS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
DecompressSequenceBeginS(
  ImageSequence *          seqID,
  ImageDescriptionHandle   desc,
  Ptr                      data,
  long                     dataSize,
  CGrafPtr                 port,
  GDHandle                 gdh,
  const Rect *             srcRect,
  MatrixRecordPtr          matrix,
  short                    mode,
  RgnHandle                mask,
  CodecFlags               flags,
  CodecQ                   accuracy,
  DecompressorComponent    codec);


/*
 *  DecompressSequenceFrame()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
DecompressSequenceFrame(
  ImageSequence                seqID,
  Ptr                          data,
  CodecFlags                   inFlags,
  CodecFlags *                 outFlags,
  ICMCompletionProcRecordPtr   asyncCompletionProc);


/*
 *  DecompressSequenceFrameS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
DecompressSequenceFrameS(
  ImageSequence                seqID,
  Ptr                          data,
  long                         dataSize,
  CodecFlags                   inFlags,
  CodecFlags *                 outFlags,
  ICMCompletionProcRecordPtr   asyncCompletionProc);


/*
 *  DecompressSequenceFrameWhen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
DecompressSequenceFrameWhen(
  ImageSequence                seqID,
  Ptr                          data,
  long                         dataSize,
  CodecFlags                   inFlags,
  CodecFlags *                 outFlags,
  ICMCompletionProcRecordPtr   asyncCompletionProc,
  const ICMFrameTimeRecord *   frameTime);


/*
 *  CDSequenceFlush()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceFlush(ImageSequence seqID);


/*
 *  SetDSequenceMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetDSequenceMatrix(
  ImageSequence     seqID,
  MatrixRecordPtr   matrix);


/*
 *  GetDSequenceMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( OSErr )
GetDSequenceMatrix(
  ImageSequence     seqID,
  MatrixRecordPtr   matrix);


/*
 *  SetDSequenceMatte()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetDSequenceMatte(
  ImageSequence   seqID,
  PixMapHandle    matte,
  const Rect *    matteRect);


/*
 *  SetDSequenceMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetDSequenceMask(
  ImageSequence   seqID,
  RgnHandle       mask);


/*
 *  SetDSequenceTransferMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetDSequenceTransferMode(
  ImageSequence     seqID,
  short             mode,
  const RGBColor *  opColor);


/*
 *  SetDSequenceDataProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetDSequenceDataProc(
  ImageSequence          seqID,
  ICMDataProcRecordPtr   dataProc,
  long                   bufferSize);


/*
 *  SetDSequenceAccuracy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetDSequenceAccuracy(
  ImageSequence   seqID,
  CodecQ          accuracy);


/*
 *  SetDSequenceSrcRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetDSequenceSrcRect(
  ImageSequence   seqID,
  const Rect *    srcRect);


/*
 *  SetDSequenceFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( OSErr )
SetDSequenceFlags(
  ImageSequence   seqID,
  long            flags,
  long            flagsMask);


enum {
  codecDSequenceDisableOverlaySurface = (1L << 5),
  codecDSequenceSingleField     = (1L << 6),
  codecDSequenceBidirectionalPrediction = (1L << 7),
  codecDSequenceFlushInsteadOfDirtying = (1L << 8),
  codecDSequenceEnableSubPixelPositioning = (1L << 9),
  codecDSequenceDeinterlaceFields = (1L << 10)
};

typedef CodecComponent *                CodecComponentPtr;
typedef CodecComponentPtr *             CodecComponentHandle;
/* selectors for ICMSequenceGet/SetInfo*/
enum {
  kICMSequenceTaskWeight        = FOUR_CHAR_CODE('twei'), /* data is pointer to UInt32*/
  kICMSequenceTaskName          = FOUR_CHAR_CODE('tnam'), /* data is pointer to OSType*/
  kICMSequenceUserPreferredCodecs = FOUR_CHAR_CODE('punt') /* data is pointer to CodecComponentHandle*/
};

/*
 *  ICMSequenceGetInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( OSErr )
ICMSequenceGetInfo(
  ImageSequence   seqID,
  OSType          which,
  void *          data);


/*
 *  ICMSequenceSetInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( OSErr )
ICMSequenceSetInfo(
  ImageSequence   seqID,
  OSType          which,
  void *          data,
  Size            dataSize);


/*
 *  GetDSequenceImageBuffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetDSequenceImageBuffer(
  ImageSequence   seqID,
  GWorldPtr *     gworld);


/*
 *  GetDSequenceScreenBuffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetDSequenceScreenBuffer(
  ImageSequence   seqID,
  GWorldPtr *     gworld);


/*
 *  SetCSequenceQuality()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetCSequenceQuality(
  ImageSequence   seqID,
  CodecQ          spatialQuality,
  CodecQ          temporalQuality);


/*
 *  SetCSequencePrev()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetCSequencePrev(
  ImageSequence   seqID,
  PixMapHandle    prev,
  const Rect *    prevRect);


/*
 *  SetCSequenceFlushProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetCSequenceFlushProc(
  ImageSequence           seqID,
  ICMFlushProcRecordPtr   flushProc,
  long                    bufferSize);


/*
 *  SetCSequenceKeyFrameRate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetCSequenceKeyFrameRate(
  ImageSequence   seqID,
  long            keyFrameRate);


/*
 *  GetCSequenceKeyFrameRate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetCSequenceKeyFrameRate(
  ImageSequence   seqID,
  long *          keyFrameRate);


/*
 *  GetCSequencePrevBuffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetCSequencePrevBuffer(
  ImageSequence   seqID,
  GWorldPtr *     gworld);


/*
 *  CDSequenceBusy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceBusy(ImageSequence seqID);


/*
 *  CDSequenceEnd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceEnd(ImageSequence seqID);


/*
 *  CDSequenceEquivalentImageDescription()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceEquivalentImageDescription(
  ImageSequence            seqID,
  ImageDescriptionHandle   newDesc,
  Boolean *                equivalent);


/*
 *  CDSequenceEquivalentImageDescriptionS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( OSErr )
CDSequenceEquivalentImageDescriptionS(
  ImageSequence            seqID,
  ImageDescriptionHandle   newDesc,
  Boolean *                equivalent,
  Boolean *                canSwitch);


/*
 *  ReplaceDSequenceImageDescription()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( OSErr )
ReplaceDSequenceImageDescription(
  ImageSequence            seqID,
  ImageDescriptionHandle   newDesc);


/*
 *  GetCompressedImageSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetCompressedImageSize(
  ImageDescriptionHandle   desc,
  Ptr                      data,
  long                     bufferSize,
  ICMDataProcRecordPtr     dataProc,
  long *                   dataSize);


/*
 *  GetSimilarity()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetSimilarity(
  PixMapHandle             src,
  const Rect *             srcRect,
  ImageDescriptionHandle   desc,
  Ptr                      data,
  Fixed *                  similarity);


enum {
  kImageDescriptionSampleFormat = FOUR_CHAR_CODE('idfm'), /* image description extension describing sample format*/
  kImageDescriptionClassicAtomFormat = FOUR_CHAR_CODE('atom'), /* sample contains classic atom structure (ie, GX codec and Curve codec)*/
  kImageDescriptionQTAtomFormat = FOUR_CHAR_CODE('qtat'), /* sample contains QT atom structure*/
  kImageDescriptionEffectDataFormat = FOUR_CHAR_CODE('fxat'), /* sample describes an effect (as QTAtoms)*/
  kImageDescriptionPrivateDataFormat = FOUR_CHAR_CODE('priv'), /* sample is in a private codec specific format*/
  kImageDescriptionAlternateCodec = FOUR_CHAR_CODE('subs'), /* image description extension containing the OSType of a substitute codec should the main codec not be available*/
  kImageDescriptionColorSpace   = FOUR_CHAR_CODE('cspc') /* image description extension containing an OSType naming the native pixel format of an image (only used for pixel formats not supported by classic Color QuickDraw)*/
};

/*
 *  GetImageDescriptionCTable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetImageDescriptionCTable(
  ImageDescriptionHandle   desc,
  CTabHandle *             ctable);


/*
 *  SetImageDescriptionCTable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetImageDescriptionCTable(
  ImageDescriptionHandle   desc,
  CTabHandle               ctable);


/*
 *  GetImageDescriptionExtension()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetImageDescriptionExtension(
  ImageDescriptionHandle   desc,
  Handle *                 extension,
  long                     idType,
  long                     index);


/*
 *  AddImageDescriptionExtension()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
AddImageDescriptionExtension(
  ImageDescriptionHandle   desc,
  Handle                   extension,
  long                     idType);


/*
 *  RemoveImageDescriptionExtension()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
RemoveImageDescriptionExtension(
  ImageDescriptionHandle   desc,
  long                     idType,
  long                     index);


/*
 *  CountImageDescriptionExtensionType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CountImageDescriptionExtensionType(
  ImageDescriptionHandle   desc,
  long                     idType,
  long *                   count);


/*
 *  GetNextImageDescriptionExtensionType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetNextImageDescriptionExtensionType(
  ImageDescriptionHandle   desc,
  long *                   idType);


/*
 *  FindCodec()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
FindCodec(
  CodecType                cType,
  CodecComponent           specCodec,
  CompressorComponent *    compressor,
  DecompressorComponent *  decompressor);


/*
 *  CompressPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CompressPicture(
  PicHandle   srcPicture,
  PicHandle   dstPicture,
  CodecQ      quality,
  CodecType   cType);


/*
 *  FCompressPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
FCompressPicture(
  PicHandle                  srcPicture,
  PicHandle                  dstPicture,
  short                      colorDepth,
  CTabHandle                 ctable,
  CodecQ                     quality,
  short                      doDither,
  short                      compressAgain,
  ICMProgressProcRecordPtr   progressProc,
  CodecType                  cType,
  CompressorComponent        codec);


/*
 *  CompressPictureFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CompressPictureFile(
  short       srcRefNum,
  short       dstRefNum,
  CodecQ      quality,
  CodecType   cType);


/*
 *  FCompressPictureFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
FCompressPictureFile(
  short                      srcRefNum,
  short                      dstRefNum,
  short                      colorDepth,
  CTabHandle                 ctable,
  CodecQ                     quality,
  short                      doDither,
  short                      compressAgain,
  ICMProgressProcRecordPtr   progressProc,
  CodecType                  cType,
  CompressorComponent        codec);


/*
 *  GetPictureFileHeader()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetPictureFileHeader(
  short             refNum,
  Rect *            frame,
  OpenCPicParams *  header);


/*
 *  DrawPictureFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
DrawPictureFile(
  short                      refNum,
  const Rect *               frame,
  ICMProgressProcRecordPtr   progressProc);


/*
 *  DrawTrimmedPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
DrawTrimmedPicture(
  PicHandle                  srcPicture,
  const Rect *               frame,
  RgnHandle                  trimMask,
  short                      doDither,
  ICMProgressProcRecordPtr   progressProc);


/*
 *  DrawTrimmedPictureFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
DrawTrimmedPictureFile(
  short                      srcRefnum,
  const Rect *               frame,
  RgnHandle                  trimMask,
  short                      doDither,
  ICMProgressProcRecordPtr   progressProc);


/*
 *  MakeThumbnailFromPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
MakeThumbnailFromPicture(
  PicHandle                  picture,
  short                      colorDepth,
  PicHandle                  thumbnail,
  ICMProgressProcRecordPtr   progressProc);


/*
 *  MakeThumbnailFromPictureFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
MakeThumbnailFromPictureFile(
  short                      refNum,
  short                      colorDepth,
  PicHandle                  thumbnail,
  ICMProgressProcRecordPtr   progressProc);


/*
 *  MakeThumbnailFromPixMap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
MakeThumbnailFromPixMap(
  PixMapHandle               src,
  const Rect *               srcRect,
  short                      colorDepth,
  PicHandle                  thumbnail,
  ICMProgressProcRecordPtr   progressProc);


/*
 *  TrimImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
TrimImage(
  ImageDescriptionHandle     desc,
  Ptr                        inData,
  long                       inBufferSize,
  ICMDataProcRecordPtr       dataProc,
  Ptr                        outData,
  long                       outBufferSize,
  ICMFlushProcRecordPtr      flushProc,
  Rect *                     trimRect,
  ICMProgressProcRecordPtr   progressProc);


/*
 *  ConvertImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ConvertImage(
  ImageDescriptionHandle   srcDD,
  Ptr                      srcData,
  short                    colorDepth,
  CTabHandle               ctable,
  CodecQ                   accuracy,
  CodecQ                   quality,
  CodecType                cType,
  CodecComponent           codec,
  ImageDescriptionHandle   dstDD,
  Ptr                      dstData);


/*
 *  GetCompressedPixMapInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetCompressedPixMapInfo(
  PixMapPtr                 pix,
  ImageDescriptionHandle *  desc,
  Ptr *                     data,
  long *                    bufferSize,
  ICMDataProcRecord *       dataProc,
  ICMProgressProcRecord *   progressProc);


/*
 *  SetCompressedPixMapInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetCompressedPixMapInfo(
  PixMapPtr                  pix,
  ImageDescriptionHandle     desc,
  Ptr                        data,
  long                       bufferSize,
  ICMDataProcRecordPtr       dataProc,
  ICMProgressProcRecordPtr   progressProc);


/*
 *  StdPix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
StdPix(
  PixMapPtr         src,
  const Rect *      srcRect,
  MatrixRecordPtr   matrix,
  short             mode,
  RgnHandle         mask,
  PixMapPtr         matte,
  const Rect *      matteRect,
  short             flags);


/*
 *  TransformRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
TransformRgn(
  MatrixRecordPtr   matrix,
  RgnHandle         rgn);


/***********
    preview stuff
***********/
#if CALL_NOT_IN_CARBON
/*
 *  SFGetFilePreview()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
SFGetFilePreview(
  Point                where,
  ConstStr255Param     prompt,
  FileFilterUPP        fileFilter,
  short                numTypes,
  ConstSFTypeListPtr   typeList,
  DlgHookUPP           dlgHook,
  SFReply *            reply);


/*
 *  SFPGetFilePreview()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
SFPGetFilePreview(
  Point                where,
  ConstStr255Param     prompt,
  FileFilterUPP        fileFilter,
  short                numTypes,
  ConstSFTypeListPtr   typeList,
  DlgHookUPP           dlgHook,
  SFReply *            reply,
  short                dlgID,
  ModalFilterUPP       filterProc);


/*
 *  StandardGetFilePreview()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
StandardGetFilePreview(
  FileFilterUPP        fileFilter,
  short                numTypes,
  ConstSFTypeListPtr   typeList,
  StandardFileReply *  reply);


/*
 *  CustomGetFilePreview()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
CustomGetFilePreview(
  FileFilterYDUPP          fileFilter,
  short                    numTypes,
  ConstSFTypeListPtr       typeList,
  StandardFileReply *      reply,
  short                    dlgID,
  Point                    where,
  DlgHookYDUPP             dlgHook,
  ModalFilterYDUPP         filterProc,
  ActivationOrderListPtr   activeList,
  ActivateYDUPP            activateProc,
  void *                   yourDataPtr);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  MakeFilePreview()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
MakeFilePreview(
  short                      resRefNum,
  ICMProgressProcRecordPtr   progress);


/*
 *  AddFilePreview()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
AddFilePreview(
  short    resRefNum,
  OSType   previewType,
  Handle   previewData);


enum {
  sfpItemPreviewAreaUser        = 11,
  sfpItemPreviewStaticText      = 12,
  sfpItemPreviewDividerUser     = 13,
  sfpItemCreatePreviewButton    = 14,
  sfpItemShowPreviewButton      = 15
};

struct PreviewResourceRecord {
  unsigned long       modDate;
  short               version;
  OSType              resType;
  short               resID;
};
typedef struct PreviewResourceRecord    PreviewResourceRecord;
typedef PreviewResourceRecord *         PreviewResourcePtr;
typedef PreviewResourcePtr *            PreviewResource;
/*
 *  AlignScreenRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
AlignScreenRect(
  Rect *                      rp,
  ICMAlignmentProcRecordPtr   alignmentProc);


/*
 *  AlignWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
AlignWindow(
  WindowRef                   wp,
  Boolean                     front,
  const Rect *                alignmentRect,
  ICMAlignmentProcRecordPtr   alignmentProc);


/*
 *  DragAlignedWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
DragAlignedWindow(
  WindowRef                   wp,
  Point                       startPt,
  Rect *                      boundsRect,
  Rect *                      alignmentRect,
  ICMAlignmentProcRecordPtr   alignmentProc);


/*
 *  DragAlignedGrayRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( long )
DragAlignedGrayRgn(
  RgnHandle                   theRgn,
  Point                       startPt,
  Rect *                      boundsRect,
  Rect *                      slopRect,
  short                       axis,
  UniversalProcPtr            actionProc,
  Rect *                      alignmentRect,
  ICMAlignmentProcRecordPtr   alignmentProc);


/*
 *  SetCSequenceDataRateParams()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetCSequenceDataRateParams(
  ImageSequence       seqID,
  DataRateParamsPtr   params);


/*
 *  SetCSequenceFrameNumber()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetCSequenceFrameNumber(
  ImageSequence   seqID,
  long            frameNumber);


/*
 *  SetCSequencePreferredPacketSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetCSequencePreferredPacketSize(
  ImageSequence   seqID,
  long            preferredPacketSizeInBytes);


/*
 *  NewImageGWorld()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
NewImageGWorld(
  GWorldPtr *              gworld,
  ImageDescriptionHandle   idh,
  GWorldFlags              flags);


/*
 *  GetCSequenceDataRateParams()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetCSequenceDataRateParams(
  ImageSequence       seqID,
  DataRateParamsPtr   params);


/*
 *  GetCSequenceFrameNumber()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetCSequenceFrameNumber(
  ImageSequence   seqID,
  long *          frameNumber);


/*
 *  GetBestDeviceRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetBestDeviceRect(
  GDHandle *  gdh,
  Rect *      rp);


/*
 *  SetSequenceProgressProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetSequenceProgressProc(
  ImageSequence            seqID,
  ICMProgressProcRecord *  progressProc);


/*
 *  GDHasScale()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GDHasScale(
  GDHandle   gdh,
  short      depth,
  Fixed *    scale);


/*
 *  GDGetScale()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GDGetScale(
  GDHandle   gdh,
  Fixed *    scale,
  short *    flags);


/*
 *  GDSetScale()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GDSetScale(
  GDHandle   gdh,
  Fixed      scale,
  short      flags);


/*
 *  ICMShieldSequenceCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ICMShieldSequenceCursor(ImageSequence seqID);


/*
 *  ICMDecompressComplete()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
ICMDecompressComplete(
  ImageSequence                seqID,
  OSErr                        err,
  short                        flag,
  ICMCompletionProcRecordPtr   completionRtn);


/*
 *  ICMDecompressCompleteS()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ICMDecompressCompleteS(
  ImageSequence                seqID,
  OSErr                        err,
  short                        flag,
  ICMCompletionProcRecordPtr   completionRtn);


/*
 *  ICMSequenceLockBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ICMSequenceLockBits(
  ImageSequence   seqID,
  PixMapPtr       dst,
  long            flags);


/*
 *  ICMSequenceUnlockBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ICMSequenceUnlockBits(
  ImageSequence   seqID,
  long            flags);


enum {
  kICMPixelFormatIsPlanarMask   = 0x0F, /* these bits in formatFlags indicate how many planes there are; they're 0 if chunky*/
  kICMPixelFormatIsIndexed      = (1L << 4),
  kICMPixelFormatIsSupportedByQD = (1L << 5),
  kICMPixelFormatIsMonochrome   = (1L << 6),
  kICMPixelFormatHasAlphaChannel = (1L << 7)
};

struct ICMPixelFormatInfo {
  long                size;                   /* caller MUST fill this in with sizeof(ICMPixelFormatInfo) before calling ICMGet/SetPixelFormatInfo*/
  unsigned long       formatFlags;
  short               bitsPerPixel[14];       /* list each plane's bits per pixel separately if planar*/
                                              /* new field for QuickTime 4.1*/
  Fixed               defaultGammaLevel;
                                              /* new fields for QuickTime 6.0*/
  short               horizontalSubsampling[14]; /* per plane; use 1 if plane is not subsampled*/
  short               verticalSubsampling[14]; /* per plane; use 1 if plane is not subsampled*/
                                              /* new fields for QuickTime 6.5*/
  short               cmpCount;               /* for use in PixMap.cmpCount*/
  short               cmpSize;                /* for use in PixMap.cmpSize*/
};
typedef struct ICMPixelFormatInfo       ICMPixelFormatInfo;
typedef ICMPixelFormatInfo *            ICMPixelFormatInfoPtr;
/* IMPORTANT: Fill in theInfo->size with sizeof(ICMPixelFormatInfo) before calling ICMGetPixelFormatInfo */
/*
 *  ICMGetPixelFormatInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ICMGetPixelFormatInfo(
  OSType                  PixelFormat,
  ICMPixelFormatInfoPtr   theInfo);


/* IMPORTANT: Fill in theInfo->size with sizeof(ICMPixelFormatInfo) before calling ICMSetPixelFormatInfo */
/*
 *  ICMSetPixelFormatInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ICMSetPixelFormatInfo(
  OSType                  PixelFormat,
  ICMPixelFormatInfoPtr   theInfo);


enum {
  kICMGetChainUltimateParent    = 0,
  kICMGetChainParent            = 1,
  kICMGetChainChild             = 2,
  kICMGetChainUltimateChild     = 3
};

/*
 *  ICMSequenceGetChainMember()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ICMSequenceGetChainMember(
  ImageSequence    seqID,
  ImageSequence *  retSeqID,
  long             flags);


/*
 *  SetDSequenceTimeCode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
SetDSequenceTimeCode(
  ImageSequence   seqID,
  void *          timeCodeFormat,
  void *          timeCodeTime);


/*
 *  CDSequenceNewMemory()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceNewMemory(
  ImageSequence          seqID,
  Ptr *                  data,
  Size                   dataSize,
  long                   dataUse,
  ICMMemoryDisposedUPP   memoryGoneProc,
  void *                 refCon);


/*
 *  CDSequenceDisposeMemory()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceDisposeMemory(
  ImageSequence   seqID,
  Ptr             data);


/*
 *  CDSequenceNewDataSource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceNewDataSource(
  ImageSequence              seqID,
  ImageSequenceDataSource *  sourceID,
  OSType                     sourceType,
  long                       sourceInputNumber,
  Handle                     dataDescription,
  ICMConvertDataFormatUPP    transferProc,
  void *                     refCon);


/*
 *  CDSequenceDisposeDataSource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceDisposeDataSource(ImageSequenceDataSource sourceID);


/*
 *  CDSequenceSetSourceData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceSetSourceData(
  ImageSequenceDataSource   sourceID,
  void *                    data,
  long                      dataSize);


/*
 *  CDSequenceChangedSourceData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceChangedSourceData(ImageSequenceDataSource sourceID);


/*
 *  CDSequenceSetSourceDataQueue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceSetSourceDataQueue(
  ImageSequenceDataSource   sourceID,
  QHdrPtr                   dataQueue);


/*
 *  CDSequenceGetDataSource()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceGetDataSource(
  ImageSequence              seqID,
  ImageSequenceDataSource *  sourceID,
  OSType                     sourceType,
  long                       sourceInputNumber);


/*
 *  PtInDSequenceData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
PtInDSequenceData(
  ImageSequence   seqID,
  void *          data,
  Size            dataSize,
  Point           where,
  Boolean *       hit);


/*
 *  HitTestDSequenceData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
HitTestDSequenceData(
  ImageSequence   seqID,
  void *          data,
  Size            dataSize,
  Point           where,
  long *          hit,
  long            hitFlags);


/*
 *  SetDSequenceNonScheduledDisplayTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 *    Windows:          in qtmlClient.lib version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API( OSErr )
SetDSequenceNonScheduledDisplayTime(
  ImageSequence   sequence,
  TimeValue64     displayTime,
  TimeScale       displayTimeScale,
  UInt32          flags);


/*
 *  GetDSequenceNonScheduledDisplayTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 *    Windows:          in qtmlClient.lib version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API( OSErr )
GetDSequenceNonScheduledDisplayTime(
  ImageSequence   sequence,
  TimeValue64 *   displayTime,
  TimeScale *     displayTimeScale);


/*
 *  SetDSequenceNonScheduledDisplayDirection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 *    Windows:          in qtmlClient.lib version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API( OSErr )
SetDSequenceNonScheduledDisplayDirection(
  ImageSequence   sequence,
  Fixed           rate);


/*
 *  GetDSequenceNonScheduledDisplayDirection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 *    Windows:          in qtmlClient.lib version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API( OSErr )
GetDSequenceNonScheduledDisplayDirection(
  ImageSequence   sequence,
  Fixed *         rate);


/*
 *  GetGraphicsImporterForFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetGraphicsImporterForFile(
  const FSSpec *       theFile,
  ComponentInstance *  gi);


/*
 *  GetGraphicsImporterForDataRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetGraphicsImporterForDataRef(
  Handle               dataRef,
  OSType               dataRefType,
  ComponentInstance *  gi);


enum {
  kDontUseValidateToFindGraphicsImporter = 1L << 0
};

/*
 *  GetGraphicsImporterForFileWithFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetGraphicsImporterForFileWithFlags(
  const FSSpec *       theFile,
  ComponentInstance *  gi,
  long                 flags);


/*
 *  GetGraphicsImporterForDataRefWithFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
GetGraphicsImporterForDataRefWithFlags(
  Handle               dataRef,
  OSType               dataRefType,
  ComponentInstance *  gi,
  long                 flags);


/*
 *  QTGetFileNameExtension()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
QTGetFileNameExtension(
  ConstStrFileNameParam   fileName,
  OSType                  fileType,
  OSType *                extension);



/*
 *  ImageTranscodeSequenceBegin()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ImageTranscodeSequenceBegin(
  ImageTranscodeSequence *  its,
  ImageDescriptionHandle    srcDesc,
  OSType                    destType,
  ImageDescriptionHandle *  dstDesc,
  void *                    data,
  long                      dataSize);


/*
 *  ImageTranscodeSequenceEnd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ImageTranscodeSequenceEnd(ImageTranscodeSequence its);


/*
 *  ImageTranscodeFrame()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ImageTranscodeFrame(
  ImageTranscodeSequence   its,
  void *                   srcData,
  long                     srcDataSize,
  void **                  dstData,
  long *                   dstDataSize);


/*
 *  ImageTranscodeDisposeFrameData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ImageTranscodeDisposeFrameData(
  ImageTranscodeSequence   its,
  void *                   dstData);


/*
 *  CDSequenceInvalidate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceInvalidate(
  ImageSequence   seqID,
  RgnHandle       invalRgn);


/*
 *  CDSequenceSetTimeBase()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
CDSequenceSetTimeBase(
  ImageSequence   seqID,
  void *          base);


/*
 *  ImageFieldSequenceBegin()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ImageFieldSequenceBegin(
  ImageFieldSequence *     ifs,
  ImageDescriptionHandle   desc1,
  ImageDescriptionHandle   desc2,
  ImageDescriptionHandle   descOut);


/*
 *  ImageFieldSequenceExtractCombine()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ImageFieldSequenceExtractCombine(
  ImageFieldSequence   ifs,
  long                 fieldFlags,
  void *               data1,
  long                 dataSize1,
  void *               data2,
  long                 dataSize2,
  void *               outputData,
  long *               outDataSize);


/*
 *  ImageFieldSequenceEnd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
ImageFieldSequenceEnd(ImageFieldSequence ifs);


enum {
  kICMTempThenAppMemory         = 1L << 12,
  kICMAppThenTempMemory         = 1L << 13
};

/*
 *  QTNewGWorld()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
QTNewGWorld(
  GWorldPtr *   offscreenGWorld,
  OSType        PixelFormat,
  const Rect *  boundsRect,
  CTabHandle    cTable,
  GDHandle      aGDevice,
  GWorldFlags   flags);


/*
 *  QTNewGWorldFromPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( OSErr )
QTNewGWorldFromPtr(
  GWorldPtr *   gw,
  OSType        pixelFormat,
  const Rect *  boundsRect,
  CTabHandle    cTable,
  GDHandle      aGDevice,
  GWorldFlags   flags,
  void *        baseAddr,
  long          rowBytes);


/*
 *  QTUpdateGWorld()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( GWorldFlags )
QTUpdateGWorld(
  GWorldPtr *   offscreenGWorld,
  OSType        PixelFormat,
  const Rect *  boundsRect,
  CTabHandle    cTable,
  GDHandle      aGDevice,
  GWorldFlags   flags);


/*
 *  MakeImageDescriptionForPixMap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
MakeImageDescriptionForPixMap(
  PixMapHandle              pixmap,
  ImageDescriptionHandle *  idh);


/*
 *  MakeImageDescriptionForEffect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( OSErr )
MakeImageDescriptionForEffect(
  OSType                    effectType,
  ImageDescriptionHandle *  idh);


/*
 *  QTGetPixelSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( short )
QTGetPixelSize(OSType PixelFormat);


/*
 *  QTGetPixelFormatDepthForImageDescription()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 6.0 and later
 *    CarbonLib:        in CarbonLib 1.6 and later
 *    Mac OS X:         in version 10.2 and later
 *    Windows:          in qtmlClient.lib 6.0 and later
 */
EXTERN_API( short )
QTGetPixelFormatDepthForImageDescription(OSType PixelFormat);


/*
 *  QTGetPixMapPtrRowBytes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( long )
QTGetPixMapPtrRowBytes(PixMapPtr pm);


/*
 *  QTGetPixMapHandleRowBytes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( long )
QTGetPixMapHandleRowBytes(PixMapHandle pm);


/*
 *  QTSetPixMapPtrRowBytes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( OSErr )
QTSetPixMapPtrRowBytes(
  PixMapPtr   pm,
  long        rowBytes);


/*
 *  QTSetPixMapHandleRowBytes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( OSErr )
QTSetPixMapHandleRowBytes(
  PixMapHandle   pm,
  long           rowBytes);


enum {
  kQTUsePlatformDefaultGammaLevel = 0,  /* When decompressing into this PixMap, gamma-correct to the platform's standard gamma. */
  kQTUseSourceGammaLevel        = -1L,  /* When decompressing into this PixMap, don't perform gamma-correction. */
  kQTCCIR601VideoGammaLevel     = 0x00023333 /* 2.2, standard television video gamma.*/
};

/*
 *  QTGetPixMapPtrGammaLevel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( Fixed )
QTGetPixMapPtrGammaLevel(PixMapPtr pm);


/*
 *  QTSetPixMapPtrGammaLevel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( OSErr )
QTSetPixMapPtrGammaLevel(
  PixMapPtr   pm,
  Fixed       gammaLevel);


/*
 *  QTGetPixMapHandleGammaLevel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( Fixed )
QTGetPixMapHandleGammaLevel(PixMapHandle pm);


/*
 *  QTSetPixMapHandleGammaLevel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( OSErr )
QTSetPixMapHandleGammaLevel(
  PixMapHandle   pm,
  Fixed          gammaLevel);


/*
 *  QTGetPixMapPtrRequestedGammaLevel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( Fixed )
QTGetPixMapPtrRequestedGammaLevel(PixMapPtr pm);


/*
 *  QTSetPixMapPtrRequestedGammaLevel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( OSErr )
QTSetPixMapPtrRequestedGammaLevel(
  PixMapPtr   pm,
  Fixed       requestedGammaLevel);


/*
 *  QTGetPixMapHandleRequestedGammaLevel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( Fixed )
QTGetPixMapHandleRequestedGammaLevel(PixMapHandle pm);


/*
 *  QTSetPixMapHandleRequestedGammaLevel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 5.0 and later
 */
EXTERN_API( OSErr )
QTSetPixMapHandleRequestedGammaLevel(
  PixMapHandle   pm,
  Fixed          requestedGammaLevel);


/*
 *  QuadToQuadMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( OSErr )
QuadToQuadMatrix(
  const Fixed *   source,
  const Fixed *   dest,
  MatrixRecord *  map);




enum {
  identityMatrixType            = 0x00, /* result if matrix is identity */
  translateMatrixType           = 0x01, /* result if matrix translates */
  scaleMatrixType               = 0x02, /* result if matrix scales */
  scaleTranslateMatrixType      = 0x03, /* result if matrix scales and translates */
  linearMatrixType              = 0x04, /* result if matrix is general 2 x 2 */
  linearTranslateMatrixType     = 0x05, /* result if matrix is general 2 x 2 and translates */
  perspectiveMatrixType         = 0x06  /* result if matrix is general 3 x 3 */
};

typedef unsigned short                  MatrixFlags;
/*
 *  GetMatrixType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( short )
GetMatrixType(const MatrixRecord * m);


/*
 *  CopyMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
CopyMatrix(
  const MatrixRecord *  m1,
  MatrixRecord *        m2);


/*
 *  EqualMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( Boolean )
EqualMatrix(
  const MatrixRecord *  m1,
  const MatrixRecord *  m2);


/*
 *  SetIdentityMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
SetIdentityMatrix(MatrixRecord * matrix);


/*
 *  TranslateMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
TranslateMatrix(
  MatrixRecord *  m,
  Fixed           deltaH,
  Fixed           deltaV);


/*
 *  RotateMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
RotateMatrix(
  MatrixRecord *  m,
  Fixed           degrees,
  Fixed           aboutX,
  Fixed           aboutY);


/*
 *  ScaleMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
ScaleMatrix(
  MatrixRecord *  m,
  Fixed           scaleX,
  Fixed           scaleY,
  Fixed           aboutX,
  Fixed           aboutY);


/*
 *  SkewMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
SkewMatrix(
  MatrixRecord *  m,
  Fixed           skewX,
  Fixed           skewY,
  Fixed           aboutX,
  Fixed           aboutY);


/*
 *  TransformFixedPoints()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
TransformFixedPoints(
  const MatrixRecord *  m,
  FixedPoint *          fpt,
  long                  count);


/*
 *  TransformPoints()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( OSErr )
TransformPoints(
  const MatrixRecord *  mp,
  Point *               pt1,
  long                  count);


/*
 *  TransformFixedRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( Boolean )
TransformFixedRect(
  const MatrixRecord *  m,
  FixedRect *           fr,
  FixedPoint *          fpp);


/*
 *  TransformRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( Boolean )
TransformRect(
  const MatrixRecord *  m,
  Rect *                r,
  FixedPoint *          fpp);


/*
 *  InverseMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( Boolean )
InverseMatrix(
  const MatrixRecord *  m,
  MatrixRecord *        im);


/*
 *  ConcatMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
ConcatMatrix(
  const MatrixRecord *  a,
  MatrixRecord *        b);


/*
 *  RectMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
RectMatrix(
  MatrixRecord *  matrix,
  const Rect *    srcRect,
  const Rect *    dstRect);


/*
 *  MapMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
MapMatrix(
  MatrixRecord *  matrix,
  const Rect *    fromRect,
  const Rect *    toRect);








/*
 *  CompAdd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
CompAdd(
  wide *  src,
  wide *  dst);


/*
 *  CompSub()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
CompSub(
  wide *  src,
  wide *  dst);


/*
 *  CompNeg()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
CompNeg(wide * dst);


/*
 *  CompShift()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
CompShift(
  wide *  src,
  short   shift);


/*
 *  CompMul()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
CompMul(
  long    src1,
  long    src2,
  wide *  dst);


/*
 *  CompDiv()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( long )
CompDiv(
  wide *  numerator,
  long    denominator,
  long *  remainder);


/*
 *  CompFixMul()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
CompFixMul(
  wide *  compSrc,
  Fixed   fixSrc,
  wide *  compDst);


/*
 *  CompMulDiv()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
CompMulDiv(
  wide *  co,
  long    mul,
  long    divisor);


/*
 *  CompMulDivTrunc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( void )
CompMulDivTrunc(
  wide *  co,
  long    mul,
  long    divisor,
  long *  remainder);


/*
 *  CompCompare()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( long )
CompCompare(
  const wide *  a,
  const wide *  minusb);


/*
 *  CompSquareRoot()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( unsigned long )
CompSquareRoot(const wide * src);


/*
 *  FixMulDiv()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( Fixed )
FixMulDiv(
  Fixed   src,
  Fixed   mul,
  Fixed   divisor);


/*
 *  UnsignedFixMulDiv()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( Fixed )
UnsignedFixMulDiv(
  Fixed   src,
  Fixed   mul,
  Fixed   divisor);


/*
 *  FracSinCos()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( Fract )
FracSinCos(
  Fixed    degree,
  Fract *  cosOut);


/*
 *  FixExp2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( Fixed )
FixExp2(Fixed src);


/*
 *  FixLog2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( Fixed )
FixLog2(Fixed src);


/*
 *  FixPow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( Fixed )
FixPow(
  Fixed   base,
  Fixed   exp);





typedef ComponentInstance               GraphicsImportComponent;
enum {
  GraphicsImporterComponentType = FOUR_CHAR_CODE('grip')
};

/* Component flags for Graphics Importer components */
enum {
  graphicsImporterIsBaseImporter = 1L << 0,
  graphicsImporterCanValidateFile = 1L << 9,
  graphicsImporterSubTypeIsFileExtension = 1L << 12,
  graphicsImporterHasMIMEList   = 1L << 14,
  graphicsImporterUsesImageDecompressor = 1L << 23
};

/* Atom types for QuickTime Image files */
enum {
  quickTimeImageFileImageDescriptionAtom = FOUR_CHAR_CODE('idsc'),
  quickTimeImageFileImageDataAtom = FOUR_CHAR_CODE('idat'),
  quickTimeImageFileMetaDataAtom = FOUR_CHAR_CODE('meta'),
  quickTimeImageFileColorSyncProfileAtom = FOUR_CHAR_CODE('iicc')
};

/* Flags for GraphicsImportDoesDrawAllPixels */
enum {
  graphicsImporterDrawsAllPixels = 0,
  graphicsImporterDoesntDrawAllPixels = 1,
  graphicsImporterDontKnowIfDrawAllPixels = 2
};

/* Flags for GraphicsImportSetFlags */
enum {
  kGraphicsImporterDontDoGammaCorrection = 1L << 0,
  kGraphicsImporterTrustResolutionFromFile = 1L << 1,
  kGraphicsImporterEnableSubPixelPositioning = 1L << 2,
  kGraphicsImporterDontUseColorMatching = 1L << 3 /* set this flag (*before* calling GraphicsImportGetColorSyncProfile) if you do matching yourself */
};

/* Flags for GraphicsImportCreateCGImage */
enum {
  kGraphicsImportCreateCGImageUsingCurrentSettings = 1L << 0
};

enum {
  kGraphicsExportGroup          = FOUR_CHAR_CODE('expo'),
  kGraphicsExportFileType       = FOUR_CHAR_CODE('ftyp'),
  kGraphicsExportMIMEType       = FOUR_CHAR_CODE('mime'),
  kGraphicsExportExtension      = FOUR_CHAR_CODE('ext '),
  kGraphicsExportDescription    = FOUR_CHAR_CODE('desc')
};

/* User data types for layers of Photoshop files */
enum {
  kQTPhotoshopLayerMode         = FOUR_CHAR_CODE('lmod'), /* OSType */
  kQTPhotoshopLayerOpacity      = FOUR_CHAR_CODE('lopa'), /* UInt8, 0 = transparent .. 255 = opaque */
  kQTPhotoshopLayerClipping     = FOUR_CHAR_CODE('lclp'), /* UInt8, 0 = base, 1 = non-base */
  kQTPhotoshopLayerFlags        = FOUR_CHAR_CODE('lflg'), /* UInt8 */
  kQTPhotoshopLayerName         = 0xA96C6E6D, /* Text */
  kQTPhotoshopLayerUnicodeName  = FOUR_CHAR_CODE('luni') /* Unicode characters, not terminated */
};

/* User data returned by graphics importers to suggest intended use for indexed images */
enum {
  kQTIndexedImageType           = FOUR_CHAR_CODE('nth?'), /* 1 or more OSTypes, such as the following values: */
  kQTIndexedImageIsThumbnail    = FOUR_CHAR_CODE('n=th'), /* The image at this index is a thumbnail. */
  kQTIndexedImageIsLayer        = FOUR_CHAR_CODE('n=ly'), /* The image at this index is a layer. */
  kQTIndexedImageIsPage         = FOUR_CHAR_CODE('n=pg'), /* The image at this index is a page. */
  kQTIndexedImageIsMultiResolution = FOUR_CHAR_CODE('n=rs') /* The image at this index is one of several identical images at different resolutions. */
};

/* Other user data types returned by graphics importers */
enum {
  kQTTIFFUserDataPrefix         = 0x74690000, /* Added to some tag values in TIFF IFDs to generate user data codes.  (0x7469 is 'ti'.) */
                                        /* For example, YCbCrPositioning is tag 0x0213, so its user data code is 0x74690213. */
  kQTTIFFExifUserDataPrefix     = 0x65780000, /* Added to tag values in Exif IFDs to generate user data codes.  (0x6578 is 'ex'.) */
                                        /* For example, DateTimeOriginal is tag 0x9003, so its user data code is 0x65789003. */
  kQTTIFFExifGPSUserDataPrefix  = 0x67700000, /* Added to tag values in Exif GPS IFDs to generate user data codes.  (0x6770 is 'gp'.) */
                                        /* For example, GPSAltitude is tag 0x0006, so its user data code is 0x6770006. */
  kQTAlphaMode                  = FOUR_CHAR_CODE('almo'), /* UInt32; eg, graphicsModeStraightAlpha or graphicsModePreBlackAlpha */
  kQTAlphaModePreMulColor       = FOUR_CHAR_CODE('almp'), /* RGBColor; used if kQTAlphaMode is graphicsModePreMulColorAlpha */
  kUserDataIPTC                 = FOUR_CHAR_CODE('iptc')
};

/* Found in TIFF and Exif JPEG files */
enum {
  kQTTIFFUserDataOrientation    = 0x74690112, /* 1 SHORT */
  kQTTIFFUserDataTransferFunction = 0x7469012D, /* n SHORTs */
  kQTTIFFUserDataWhitePoint     = 0x7469013E, /* 2 RATIONALs */
  kQTTIFFUserDataPrimaryChromaticities = 0x7469013F, /* 6 RATIONALs */
  kQTTIFFUserDataTransferRange  = 0x74690156, /* 6 SHORTs */
  kQTTIFFUserDataYCbCrPositioning = 0x74690213, /* 1 SHORT */
  kQTTIFFUserDataReferenceBlackWhite = 0x74690214 /* n LONGs */
};

/* Found in GeoTIFF files; defined in the GeoTIFF 1.0 spec */
enum {
  kQTTIFFUserDataModelPixelScale = 0x7469830E, /* 3 DOUBLEs */
  kQTTIFFUserDataModelTransformation = 0x746985D8, /* 16 DOUBLEs */
  kQTTIFFUserDataModelTiepoint  = 0x74698482, /* n DOUBLEs */
  kQTTIFFUserDataGeoKeyDirectory = 0x746987AF, /* n SHORTs */
  kQTTIFFUserDataGeoDoubleParams = 0x746987B0, /* n DOUBLEs */
  kQTTIFFUserDataGeoAsciiParams = 0x746987B1, /* n ASCIIs */
  kQTTIFFUserDataIntergraphMatrix = 0x74698480 /* 16 or 17 DOUBLEs */
};

/* Found in Exif TIFF and Exif JPEG files; defined in the Exif 2.1 spec */
enum {
  kQTExifUserDataExifVersion    = 0x65789000, /* 4 bytes (import only) */
  kQTExifUserDataFlashPixVersion = 0x6578A000, /* 4 bytes */
  kQTExifUserDataColorSpace     = 0x6578A001, /* 1 SHORT */
  kQTExifUserDataComponentsConfiguration = 0x65789101, /* 4 bytes */
  kQTExifUserDataCompressedBitsPerPixel = 0x65789102, /* 1 RATIONAL */
  kQTExifUserDataPixelXDimension = 0x6578A002, /* 1 SHORT or LONG */
  kQTExifUserDataPixelYDimension = 0x6578A003, /* 1 SHORT or LONG */
  kQTExifUserDataMakerNote      = 0x6578927C, /* n bytes */
  kQTExifUserDataUserComment    = 0x65789286, /* n bytes (Note: this constant was erroneously 0x6578928C)*/
  kQTExifUserDataRelatedSoundFile = 0x6578A004, /* 13 ASCIIs*/
  kQTExifUserDataDateTimeOriginal = 0x65789003, /* 20 ASCIIs */
  kQTExifUserDataDateTimeDigitized = 0x65789004, /* 20 ASCIIs */
  kQTExifUserDataSubSecTime     = 0x65789290, /* n ASCIIs */
  kQTExifUserDataSubSecTimeOriginal = 0x65789291, /* n ASCIIs */
  kQTExifUserDataSubSecTimeDigitized = 0x65789292, /* n ASCIIs */
  kQTExifUserDataExposureTime   = 0x6578829A, /* 1 RATIONAL */
  kQTExifUserDataFNumber        = 0x6578829D, /* 1 RATIONAL */
  kQTExifUserDataExposureProgram = 0x65788822, /* 1 SHORT */
  kQTExifUserDataSpectralSensitivity = 0x65788824, /* n ASCIIs */
  kQTExifUserDataISOSpeedRatings = 0x65788827, /* n SHORTs */
  kQTExifUserDataShutterSpeedValue = 0x65789201, /* 1 SIGNED RATIONAL */
  kQTExifUserDataApertureValue  = 0x65789202, /* 1 RATIONAL */
  kQTExifUserDataBrightnessValue = 0x65789203, /* 1 SIGNED RATIONAL */
  kQTExifUserDataExposureBiasValue = 0x65789204, /* 1 SIGNED RATIONAL */
  kQTExifUserDataMaxApertureValue = 0x65789205, /* 1 RATIONAL */
  kQTExifUserDataSubjectDistance = 0x65789206, /* 1 RATIONAL */
  kQTExifUserDataMeteringMode   = 0x65789207, /* 1 SHORT */
  kQTExifUserDataLightSource    = 0x65789208, /* 1 SHORT */
  kQTExifUserDataFlash          = 0x65789209, /* 1 SHORT */
  kQTExifUserDataFocalLength    = 0x6578920A, /* 1 RATIONAL */
  kQTExifUserDataFlashEnergy    = 0x6578A20B, /* 1 RATIONAL */
  kQTExifUserDataFocalPlaneXResolution = 0x6578A20E, /* 1 RATIONAL */
  kQTExifUserDataFocalPlaneYResolution = 0x6578A20F, /* 1 RATIONAL */
  kQTExifUserDataFocalPlaneResolutionUnit = 0x6578A210, /* 1 SHORT */
  kQTExifUserDataSubjectLocation = 0x6578A214, /* 1 SHORT */
  kQTExifUserDataExposureIndex  = 0x6578A215, /* 1 RATIONAL */
  kQTExifUserDataSensingMethod  = 0x6578A217, /* 1 SHORT */
  kQTExifUserDataFileSource     = 0x6578A300, /* 1 UNDEFINED */
  kQTExifUserDataSceneType      = 0x6578A301 /* 1 UNDEFINED */
};

/* Found in some Exif TIFF and Exif JPEG files; defined in the Exif 2.1 spec */
/* Note: these were wrong in the QuickTime 6.0 headers -- the high two bytes were 0x677 instead of 0x6770. */
enum {
  kQTExifUserDataGPSVersionID   = 0x67700000, /* 4 BYTEs */
  kQTExifUserDataGPSLatitudeRef = 0x67700001, /* 2 ASCIIs*/
  kQTExifUserDataGPSLatitude    = 0x67700002, /* 3 RATIONALs */
  kQTExifUserDataGPSLongitudeRef = 0x67700003, /* 2 ASCIIs */
  kQTExifUserDataGPSLongitude   = 0x67700004, /* 3 RATIONALs */
  kQTExifUserDataGPSAltitudeRef = 0x67700005, /* 1 BYTE */
  kQTExifUserDataGPSAltitude    = 0x67700006, /* 1 RATIONAL */
  kQTExifUserDataGPSTimeStamp   = 0x67700007, /* 3 RATIONALs */
  kQTExifUserDataGPSSatellites  = 0x67700008, /* n ASCIIs */
  kQTExifUserDataGPSStatus      = 0x67700009, /* 2 ASCIIs */
  kQTExifUserDataGPSMeasureMode = 0x6770000A, /* 2 ASCIIs */
  kQTExifUserDataGPSDOP         = 0x6770000B, /* 1 RATIONAL */
  kQTExifUserDataGPSSpeedRef    = 0x6770000C, /* 2 ASCIIs */
  kQTExifUserDataGPSSpeed       = 0x6770000D, /* 1 RATIONAL */
  kQTExifUserDataGPSTrackRef    = 0x6770000E, /* 2 ASCIIs */
  kQTExifUserDataGPSTrack       = 0x6770000F, /* 1 RATIONAL */
  kQTExifUserDataGPSImgDirectionRef = 0x67700010, /* 2 ASCIIs */
  kQTExifUserDataGPSImgDirection = 0x67700011, /* 1 RATIONAL */
  kQTExifUserDataGPSMapDatum    = 0x67700012, /* n ASCII */
  kQTExifUserDataGPSDestLatitudeRef = 0x67700013, /* 2 ASCIIs */
  kQTExifUserDataGPSDestLatitude = 0x67700014, /* 3 RATIONALs */
  kQTExifUserDataGPSDestLongitudeRef = 0x67700015, /* 2 ASCIIs */
  kQTExifUserDataGPSDestLongitude = 0x67700016, /* 3 RATIONALs */
  kQTExifUserDataGPSDestBearingRef = 0x67700017, /* 2 ASCIIs */
  kQTExifUserDataGPSDestBearing = 0x67700018, /* 1 RATIONAL */
  kQTExifUserDataGPSDestDistanceRef = 0x67700019, /* 2 ASCIIs */
  kQTExifUserDataGPSDestDistance = 0x6770001A /* 1 RATIONAL */
};


/** These are GraphicsImport procedures **/
/*
 *  GraphicsImportSetDataReference()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetDataReference(
  GraphicsImportComponent   ci,
  Handle                    dataRef,
  OSType                    dataReType)                       FIVEWORDINLINE(0x2F3C, 0x0008, 0x0001, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDataReference()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDataReference(
  GraphicsImportComponent   ci,
  Handle *                  dataRef,
  OSType *                  dataReType)                       FIVEWORDINLINE(0x2F3C, 0x0008, 0x0002, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetDataFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetDataFile(
  GraphicsImportComponent   ci,
  const FSSpec *            theFile)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x0003, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDataFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDataFile(
  GraphicsImportComponent   ci,
  FSSpec *                  theFile)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x0004, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetDataHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetDataHandle(
  GraphicsImportComponent   ci,
  Handle                    h)                                FIVEWORDINLINE(0x2F3C, 0x0004, 0x0005, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDataHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDataHandle(
  GraphicsImportComponent   ci,
  Handle *                  h)                                FIVEWORDINLINE(0x2F3C, 0x0004, 0x0006, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetImageDescription()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetImageDescription(
  GraphicsImportComponent   ci,
  ImageDescriptionHandle *  desc)                             FIVEWORDINLINE(0x2F3C, 0x0004, 0x0007, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDataOffsetAndSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDataOffsetAndSize(
  GraphicsImportComponent   ci,
  unsigned long *           offset,
  unsigned long *           size)                             FIVEWORDINLINE(0x2F3C, 0x0008, 0x0008, 0x7000, 0xA82A);


/*
 *  GraphicsImportReadData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportReadData(
  GraphicsImportComponent   ci,
  void *                    dataPtr,
  unsigned long             dataOffset,
  unsigned long             dataSize)                         FIVEWORDINLINE(0x2F3C, 0x000C, 0x0009, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetClip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetClip(
  GraphicsImportComponent   ci,
  RgnHandle                 clipRgn)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x000A, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetClip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetClip(
  GraphicsImportComponent   ci,
  RgnHandle *               clipRgn)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x000B, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetSourceRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetSourceRect(
  GraphicsImportComponent   ci,
  const Rect *              sourceRect)                       FIVEWORDINLINE(0x2F3C, 0x0004, 0x000C, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetSourceRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetSourceRect(
  GraphicsImportComponent   ci,
  Rect *                    sourceRect)                       FIVEWORDINLINE(0x2F3C, 0x0004, 0x000D, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetNaturalBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetNaturalBounds(
  GraphicsImportComponent   ci,
  Rect *                    naturalBounds)                    FIVEWORDINLINE(0x2F3C, 0x0004, 0x000E, 0x7000, 0xA82A);


/*
 *  GraphicsImportDraw()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportDraw(GraphicsImportComponent ci)                FIVEWORDINLINE(0x2F3C, 0x0000, 0x000F, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetGWorld()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetGWorld(
  GraphicsImportComponent   ci,
  CGrafPtr                  port,
  GDHandle                  gd)                               FIVEWORDINLINE(0x2F3C, 0x0008, 0x0010, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetGWorld()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetGWorld(
  GraphicsImportComponent   ci,
  CGrafPtr *                port,
  GDHandle *                gd)                               FIVEWORDINLINE(0x2F3C, 0x0008, 0x0011, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetMatrix(
  GraphicsImportComponent   ci,
  const MatrixRecord *      matrix)                           FIVEWORDINLINE(0x2F3C, 0x0004, 0x0012, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetMatrix(
  GraphicsImportComponent   ci,
  MatrixRecord *            matrix)                           FIVEWORDINLINE(0x2F3C, 0x0004, 0x0013, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetBoundsRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetBoundsRect(
  GraphicsImportComponent   ci,
  const Rect *              bounds)                           FIVEWORDINLINE(0x2F3C, 0x0004, 0x0014, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetBoundsRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetBoundsRect(
  GraphicsImportComponent   ci,
  Rect *                    bounds)                           FIVEWORDINLINE(0x2F3C, 0x0004, 0x0015, 0x7000, 0xA82A);


/*
 *  GraphicsImportSaveAsPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSaveAsPicture(
  GraphicsImportComponent   ci,
  const FSSpec *            fss,
  ScriptCode                scriptTag)                        FIVEWORDINLINE(0x2F3C, 0x0006, 0x0016, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetGraphicsMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetGraphicsMode(
  GraphicsImportComponent   ci,
  long                      graphicsMode,
  const RGBColor *          opColor)                          FIVEWORDINLINE(0x2F3C, 0x0008, 0x0017, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetGraphicsMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetGraphicsMode(
  GraphicsImportComponent   ci,
  long *                    graphicsMode,
  RGBColor *                opColor)                          FIVEWORDINLINE(0x2F3C, 0x0008, 0x0018, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetQuality()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetQuality(
  GraphicsImportComponent   ci,
  CodecQ                    quality)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x0019, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetQuality()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetQuality(
  GraphicsImportComponent   ci,
  CodecQ *                  quality)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x001A, 0x7000, 0xA82A);


/*
 *  GraphicsImportSaveAsQuickTimeImageFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSaveAsQuickTimeImageFile(
  GraphicsImportComponent   ci,
  const FSSpec *            fss,
  ScriptCode                scriptTag)                        FIVEWORDINLINE(0x2F3C, 0x0006, 0x001B, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetDataReferenceOffsetAndLimit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetDataReferenceOffsetAndLimit(
  GraphicsImportComponent   ci,
  unsigned long             offset,
  unsigned long             limit)                            FIVEWORDINLINE(0x2F3C, 0x0008, 0x001C, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDataReferenceOffsetAndLimit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDataReferenceOffsetAndLimit(
  GraphicsImportComponent   ci,
  unsigned long *           offset,
  unsigned long *           limit)                            FIVEWORDINLINE(0x2F3C, 0x0008, 0x001D, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetAliasedDataReference()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetAliasedDataReference(
  GraphicsImportComponent   ci,
  Handle *                  dataRef,
  OSType *                  dataRefType)                      FIVEWORDINLINE(0x2F3C, 0x0008, 0x001E, 0x7000, 0xA82A);


/*
 *  GraphicsImportValidate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportValidate(
  GraphicsImportComponent   ci,
  Boolean *                 valid)                            FIVEWORDINLINE(0x2F3C, 0x0004, 0x001F, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetMetaData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetMetaData(
  GraphicsImportComponent   ci,
  void *                    userData)                         FIVEWORDINLINE(0x2F3C, 0x0004, 0x0020, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetMIMETypeList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetMIMETypeList(
  GraphicsImportComponent   ci,
  void *                    qtAtomContainerPtr)               FIVEWORDINLINE(0x2F3C, 0x0004, 0x0021, 0x7000, 0xA82A);


/*
 *  GraphicsImportDoesDrawAllPixels()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportDoesDrawAllPixels(
  GraphicsImportComponent   ci,
  short *                   drawsAllPixels)                   FIVEWORDINLINE(0x2F3C, 0x0004, 0x0022, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetAsPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetAsPicture(
  GraphicsImportComponent   ci,
  PicHandle *               picture)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x0023, 0x7000, 0xA82A);


/*
 *  GraphicsImportExportImageFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportExportImageFile(
  GraphicsImportComponent   ci,
  OSType                    fileType,
  OSType                    fileCreator,
  const FSSpec *            fss,
  ScriptCode                scriptTag)                        FIVEWORDINLINE(0x2F3C, 0x000E, 0x0024, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetExportImageTypeList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetExportImageTypeList(
  GraphicsImportComponent   ci,
  void *                    qtAtomContainerPtr)               FIVEWORDINLINE(0x2F3C, 0x0004, 0x0025, 0x7000, 0xA82A);


/*
 *  GraphicsImportDoExportImageFileDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportDoExportImageFileDialog(
  GraphicsImportComponent   ci,
  const FSSpec *            inDefaultSpec,
  StringPtr                 prompt,
  ModalFilterYDUPP          filterProc,
  OSType *                  outExportedType,
  FSSpec *                  outExportedSpec,
  ScriptCode *              outScriptTag)                     FIVEWORDINLINE(0x2F3C, 0x0018, 0x0026, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetExportSettingsAsAtomContainer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetExportSettingsAsAtomContainer(
  GraphicsImportComponent   ci,
  void *                    qtAtomContainerPtr)               FIVEWORDINLINE(0x2F3C, 0x0004, 0x0027, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetExportSettingsFromAtomContainer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetExportSettingsFromAtomContainer(
  GraphicsImportComponent   ci,
  void *                    qtAtomContainer)                  FIVEWORDINLINE(0x2F3C, 0x0004, 0x0028, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetProgressProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetProgressProc(
  GraphicsImportComponent    ci,
  ICMProgressProcRecordPtr   progressProc)                    FIVEWORDINLINE(0x2F3C, 0x0004, 0x0029, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetProgressProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 3.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetProgressProc(
  GraphicsImportComponent    ci,
  ICMProgressProcRecordPtr   progressProc)                    FIVEWORDINLINE(0x2F3C, 0x0004, 0x002A, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetImageCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetImageCount(
  GraphicsImportComponent   ci,
  unsigned long *           imageCount)                       FIVEWORDINLINE(0x2F3C, 0x0004, 0x002B, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetImageIndex()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetImageIndex(
  GraphicsImportComponent   ci,
  unsigned long             imageIndex)                       FIVEWORDINLINE(0x2F3C, 0x0004, 0x002C, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetImageIndex()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetImageIndex(
  GraphicsImportComponent   ci,
  unsigned long *           imageIndex)                       FIVEWORDINLINE(0x2F3C, 0x0004, 0x002D, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDataOffsetAndSize64()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDataOffsetAndSize64(
  GraphicsImportComponent   ci,
  wide *                    offset,
  wide *                    size)                             FIVEWORDINLINE(0x2F3C, 0x0008, 0x002E, 0x7000, 0xA82A);


/*
 *  GraphicsImportReadData64()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportReadData64(
  GraphicsImportComponent   ci,
  void *                    dataPtr,
  const wide *              dataOffset,
  unsigned long             dataSize)                         FIVEWORDINLINE(0x2F3C, 0x000C, 0x002F, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetDataReferenceOffsetAndLimit64()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetDataReferenceOffsetAndLimit64(
  GraphicsImportComponent   ci,
  const wide *              offset,
  const wide *              limit)                            FIVEWORDINLINE(0x2F3C, 0x0008, 0x0030, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDataReferenceOffsetAndLimit64()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDataReferenceOffsetAndLimit64(
  GraphicsImportComponent   ci,
  wide *                    offset,
  wide *                    limit)                            FIVEWORDINLINE(0x2F3C, 0x0008, 0x0031, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDefaultMatrix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDefaultMatrix(
  GraphicsImportComponent   ci,
  MatrixRecord *            defaultMatrix)                    FIVEWORDINLINE(0x2F3C, 0x0004, 0x0032, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDefaultClip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDefaultClip(
  GraphicsImportComponent   ci,
  RgnHandle *               defaultRgn)                       FIVEWORDINLINE(0x2F3C, 0x0004, 0x0033, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDefaultGraphicsMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDefaultGraphicsMode(
  GraphicsImportComponent   ci,
  long *                    defaultGraphicsMode,
  RGBColor *                defaultOpColor)                   FIVEWORDINLINE(0x2F3C, 0x0008, 0x0034, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDefaultSourceRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDefaultSourceRect(
  GraphicsImportComponent   ci,
  Rect *                    defaultSourceRect)                FIVEWORDINLINE(0x2F3C, 0x0004, 0x0035, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetColorSyncProfile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetColorSyncProfile(
  GraphicsImportComponent   ci,
  Handle *                  profile)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x0036, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetDestRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetDestRect(
  GraphicsImportComponent   ci,
  const Rect *              destRect)                         FIVEWORDINLINE(0x2F3C, 0x0004, 0x0037, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDestRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDestRect(
  GraphicsImportComponent   ci,
  Rect *                    destRect)                         FIVEWORDINLINE(0x2F3C, 0x0004, 0x0038, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetFlags(
  GraphicsImportComponent   ci,
  long                      flags)                            FIVEWORDINLINE(0x2F3C, 0x0004, 0x0039, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetFlags(
  GraphicsImportComponent   ci,
  long *                    flags)                            FIVEWORDINLINE(0x2F3C, 0x0004, 0x003A, 0x7000, 0xA82A);


/* 2 private selectors */
/*
 *  GraphicsImportGetBaseDataOffsetAndSize64()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0.2 and later
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.1 and later
 *    Windows:          in qtmlClient.lib 5.0.2 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetBaseDataOffsetAndSize64(
  GraphicsImportComponent   ci,
  wide *                    offset,
  wide *                    size)                             FIVEWORDINLINE(0x2F3C, 0x0008, 0x003D, 0x7000, 0xA82A);


/*
 *  GraphicsImportSetImageIndexToThumbnail()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 6.0 and later
 *    CarbonLib:        in CarbonLib 1.6 and later
 *    Mac OS X:         in version 10.2 and later
 *    Windows:          in qtmlClient.lib 6.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetImageIndexToThumbnail(GraphicsImportComponent ci) FIVEWORDINLINE(0x2F3C, 0x0000, 0x003E, 0x7000, 0xA82A);


#if TARGET_API_MAC_OSX
/*
 *  GraphicsImportCreateCGImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
GraphicsImportCreateCGImage(
  GraphicsImportComponent   ci,
  CGImageRef *              imageRefOut,
  UInt32                    flags)                            FIVEWORDINLINE(0x2F3C, 0x0008, 0x003F, 0x7000, 0xA82A);


#endif  /* TARGET_API_MAC_OSX */

/*
 *  GraphicsImportSaveAsPictureToDataRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 *    Windows:          in qtmlClient.lib 6.5 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSaveAsPictureToDataRef(
  GraphicsImportComponent   ci,
  Handle                    dataRef,
  OSType                    dataRefType)                      FIVEWORDINLINE(0x2F3C, 0x0008, 0x0040, 0x7000, 0xA82A);


/*
 *  GraphicsImportSaveAsQuickTimeImageFileToDataRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 *    Windows:          in qtmlClient.lib 6.5 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSaveAsQuickTimeImageFileToDataRef(
  GraphicsImportComponent   ci,
  Handle                    dataRef,
  OSType                    dataRefType)                      FIVEWORDINLINE(0x2F3C, 0x0008, 0x0041, 0x7000, 0xA82A);


/*
 *  GraphicsImportExportImageFileToDataRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 *    Windows:          in qtmlClient.lib 6.5 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportExportImageFileToDataRef(
  GraphicsImportComponent   ci,
  OSType                    fileType,
  OSType                    fileCreator,
  Handle                    dataRef,
  OSType                    dataRefType)                      FIVEWORDINLINE(0x2F3C, 0x0010, 0x0042, 0x7000, 0xA82A);


/*
 *  GraphicsImportDoExportImageFileToDataRefDialog()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 *    Windows:          in qtmlClient.lib 6.5 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportDoExportImageFileToDataRefDialog(
  GraphicsImportComponent   ci,
  Handle                    inDataRef,
  OSType                    inDataRefType,
  CFStringRef               prompt,
  ModalFilterYDUPP          filterProc,
  OSType *                  outExportedType,
  Handle *                  outDataRef,
  OSType *                  outDataRefType)                   FIVEWORDINLINE(0x2F3C, 0x001C, 0x0043, 0x7000, 0xA82A);


#if TARGET_API_MAC_OSX
/* NOTE: If the source override ColorSync profile is NULL, then the image's ColorSync profile may be used if available, otherwise a generic ColorSync profile may be used. */
/*
 *  GraphicsImportSetOverrideSourceColorSyncProfileRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetOverrideSourceColorSyncProfileRef(
  GraphicsImportComponent   ci,
  CMProfileRef              newOverrideSourceProfileRef)      FIVEWORDINLINE(0x2F3C, 0x0004, 0x0044, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetOverrideSourceColorSyncProfileRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetOverrideSourceColorSyncProfileRef(
  GraphicsImportComponent   ci,
  CMProfileRef *            outOverrideSourceProfileRef)      FIVEWORDINLINE(0x2F3C, 0x0004, 0x0045, 0x7000, 0xA82A);


/* NOTE: If the destination ColorSync profile is NULL, then a generic ColorSync profile may be used. */
/*
 *  GraphicsImportSetDestinationColorSyncProfileRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetDestinationColorSyncProfileRef(
  GraphicsImportComponent   ci,
  CMProfileRef              newDestinationProfileRef)         FIVEWORDINLINE(0x2F3C, 0x0004, 0x0046, 0x7000, 0xA82A);


/*
 *  GraphicsImportGetDestinationColorSyncProfileRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetDestinationColorSyncProfileRef(
  GraphicsImportComponent   ci,
  CMProfileRef *            destinationProfileRef)            FIVEWORDINLINE(0x2F3C, 0x0004, 0x0047, 0x7000, 0xA82A);


#endif  /* TARGET_API_MAC_OSX */

/*
 *  GraphicsImportWillUseColorMatching()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 *    Windows:          in qtmlClient.lib 6.5 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportWillUseColorMatching(
  GraphicsImportComponent   ci,
  Boolean *                 outWillMatch)                     FIVEWORDINLINE(0x2F3C, 0x0004, 0x0048, 0x7000, 0xA82A);


#if TARGET_API_MAC_OSX
/* This convenience API is implemented by the base graphics importer for format-specific importers. */
/*
 *  GraphicsImportGetGenericColorSyncProfile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetGenericColorSyncProfile(
  GraphicsImportComponent   ci,
  OSType                    pixelFormat,
  void *                    reservedSetToNULL,
  UInt32                    flags,
  Handle *                  genericColorSyncProfileOut)       FIVEWORDINLINE(0x2F3C, 0x0010, 0x0049, 0x7000, 0xA82A);


#endif  /* TARGET_API_MAC_OSX */

/* Format-specific importers that implement GetColorSyncProfile and that want the base graphics 
   importer to automatically support ColorSync matching should:
   (a) implement GraphicsImportSetReturnGenericColorSyncProfile; when it is called, set an internal flag
   (b) change GraphicsImportGetColorSyncProfile so that, if this internal flag is set,
       when the source image file contains a profile 
       and the kGraphicsImporterDontUseColorMatching flag is NOT set,
       it returns a generic profile of the appropriate colorspace instead.
   Other importers should *not* implement GraphicsImportSetReturnGenericColorSyncProfile. */
/* WARNING: Applications should not call this API; it is internal graphics importer plumbing.
   Set kGraphicsImporterDontUseColorMatching instead. */
/*
 *  GraphicsImportSetReturnGenericColorSyncProfile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 *    Windows:          in qtmlClient.lib 6.5 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportSetReturnGenericColorSyncProfile(
  GraphicsImportComponent   ci,
  Boolean                   returnGenericProfilesUnlessDontMatchFlagSet) FIVEWORDINLINE(0x2F3C, 0x0002, 0x004A, 0x7000, 0xA82A);


/* WARNING: Applications should not call this API; it is internal graphics importer plumbing. */
/*
 *  GraphicsImportGetReturnGenericColorSyncProfile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 *    Windows:          in qtmlClient.lib 6.5 and later
 */
EXTERN_API( ComponentResult )
GraphicsImportGetReturnGenericColorSyncProfile(
  GraphicsImportComponent   ci,
  Boolean *                 returnGenericProfilesUnlessDontMatchFlagSet) FIVEWORDINLINE(0x2F3C, 0x0004, 0x004B, 0x7000, 0xA82A);




typedef ComponentInstance               GraphicsExportComponent;
enum {
  GraphicsExporterComponentType = FOUR_CHAR_CODE('grex'),
  kBaseGraphicsExporterSubType  = FOUR_CHAR_CODE('base')
};

/* Component flags for Graphics Exporter components */
enum {
  graphicsExporterIsBaseExporter = 1L << 0,
  graphicsExporterCanTranscode  = 1L << 1,
  graphicsExporterUsesImageCompressor = 1L << 2
};

struct QTResolutionSettings {
  Fixed               horizontalResolution;
  Fixed               verticalResolution;
};
typedef struct QTResolutionSettings     QTResolutionSettings;
struct QTTargetDataSize {
  unsigned long       targetDataSize;
};
typedef struct QTTargetDataSize         QTTargetDataSize;
struct QTThumbnailSettings {
  long                enableThumbnail;        /* a thoroughly padded Boolean*/
  long                maxThumbnailWidth;      /* set to zero to let someone else decide*/
  long                maxThumbnailHeight;     /* set to zero to let someone else decide*/
};
typedef struct QTThumbnailSettings      QTThumbnailSettings;
enum {
  kQTResolutionSettings         = FOUR_CHAR_CODE('reso'),
  kQTTargetDataSize             = FOUR_CHAR_CODE('dasz'),
  kQTDontRecompress             = FOUR_CHAR_CODE('dntr'),
  kQTInterlaceStyle             = FOUR_CHAR_CODE('ilac'),
  kQTColorSyncProfile           = FOUR_CHAR_CODE('iccp'),
  kQTThumbnailSettings          = FOUR_CHAR_CODE('thum'),
  kQTEnableExif                 = FOUR_CHAR_CODE('exif'), /* UInt8 (boolean)*/
  kQTMetaData                   = FOUR_CHAR_CODE('meta')
};

enum {
  kQTTIFFCompressionMethod      = FOUR_CHAR_CODE('tifc'), /* UInt32*/
  kQTTIFFCompression_None       = 1,
  kQTTIFFCompression_PackBits   = 32773L,
  kQTTIFFLittleEndian           = FOUR_CHAR_CODE('tife') /* UInt8 (boolean)*/
};

enum {
  kQTPNGFilterPreference        = FOUR_CHAR_CODE('pngf'), /* UInt32*/
  kQTPNGFilterBestForColorType  = FOUR_CHAR_CODE('bflt'),
  kQTPNGFilterNone              = 0,
  kQTPNGFilterSub               = 1,
  kQTPNGFilterUp                = 2,
  kQTPNGFilterAverage           = 3,
  kQTPNGFilterPaeth             = 4,
  kQTPNGFilterAdaptivePerRow    = FOUR_CHAR_CODE('aflt'),
  kQTPNGInterlaceStyle          = FOUR_CHAR_CODE('ilac'), /* UInt32*/
  kQTPNGInterlaceNone           = 0,
  kQTPNGInterlaceAdam7          = 1
};

enum {
  kQTJPEGQuantizationTables     = FOUR_CHAR_CODE('jpqt'),
  kQTJPEGHuffmanTables          = FOUR_CHAR_CODE('jpht')
};


/** These are GraphicsExport procedures **/
/* To use: set the input and output (and other settings as desired) and call GEDoExport. */
/*
 *  GraphicsExportDoExport()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportDoExport(
  GraphicsExportComponent   ci,
  unsigned long *           actualSizeWritten)                FIVEWORDINLINE(0x2F3C, 0x0004, 0x0001, 0x7000, 0xA82A);


/* Used for internal communication between the base and format-specific graphics exporter: */
/*
 *  GraphicsExportCanTranscode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportCanTranscode(
  GraphicsExportComponent   ci,
  Boolean *                 canTranscode)                     FIVEWORDINLINE(0x2F3C, 0x0004, 0x0002, 0x7000, 0xA82A);


/*
 *  GraphicsExportDoTranscode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportDoTranscode(GraphicsExportComponent ci)         FIVEWORDINLINE(0x2F3C, 0x0000, 0x0003, 0x7000, 0xA82A);


/*
 *  GraphicsExportCanUseCompressor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportCanUseCompressor(
  GraphicsExportComponent   ci,
  Boolean *                 canUseCompressor,
  void *                    codecSettingsAtomContainerPtr)    FIVEWORDINLINE(0x2F3C, 0x0008, 0x0004, 0x7000, 0xA82A);


/*
 *  GraphicsExportDoUseCompressor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportDoUseCompressor(
  GraphicsExportComponent   ci,
  void *                    codecSettingsAtomContainer,
  ImageDescriptionHandle *  outDesc)                          FIVEWORDINLINE(0x2F3C, 0x0008, 0x0005, 0x7000, 0xA82A);


/*
 *  GraphicsExportDoStandaloneExport()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportDoStandaloneExport(GraphicsExportComponent ci)  FIVEWORDINLINE(0x2F3C, 0x0000, 0x0006, 0x7000, 0xA82A);


/* Queries applications can make of a format-specific graphics exporter: */
/*
 *  GraphicsExportGetDefaultFileTypeAndCreator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetDefaultFileTypeAndCreator(
  GraphicsExportComponent   ci,
  OSType *                  fileType,
  OSType *                  fileCreator)                      FIVEWORDINLINE(0x2F3C, 0x0008, 0x0007, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetDefaultFileNameExtension()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetDefaultFileNameExtension(
  GraphicsExportComponent   ci,
  OSType *                  fileNameExtension)                FIVEWORDINLINE(0x2F3C, 0x0004, 0x0008, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetMIMETypeList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetMIMETypeList(
  GraphicsExportComponent   ci,
  void *                    qtAtomContainerPtr)               FIVEWORDINLINE(0x2F3C, 0x0004, 0x0009, 0x7000, 0xA82A);


/* (1 unused selector) */
/* Graphics exporter settings: */
/*
 *  GraphicsExportRequestSettings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportRequestSettings(
  GraphicsExportComponent   ci,
  ModalFilterYDUPP          filterProc,
  void *                    yourDataProc)                     FIVEWORDINLINE(0x2F3C, 0x0008, 0x000B, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetSettingsFromAtomContainer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetSettingsFromAtomContainer(
  GraphicsExportComponent   ci,
  void *                    qtAtomContainer)                  FIVEWORDINLINE(0x2F3C, 0x0004, 0x000C, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetSettingsAsAtomContainer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetSettingsAsAtomContainer(
  GraphicsExportComponent   ci,
  void *                    qtAtomContainerPtr)               FIVEWORDINLINE(0x2F3C, 0x0004, 0x000D, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetSettingsAsText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetSettingsAsText(
  GraphicsExportComponent   ci,
  Handle *                  theText)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x000E, 0x7000, 0xA82A);


/* Graphics exporters may implement some or none of the following: */
/*
 *  GraphicsExportSetDontRecompress()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetDontRecompress(
  GraphicsExportComponent   ci,
  Boolean                   dontRecompress)                   FIVEWORDINLINE(0x2F3C, 0x0002, 0x000F, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetDontRecompress()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetDontRecompress(
  GraphicsExportComponent   ci,
  Boolean *                 dontRecompress)                   FIVEWORDINLINE(0x2F3C, 0x0004, 0x0010, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetInterlaceStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetInterlaceStyle(
  GraphicsExportComponent   ci,
  unsigned long             interlaceStyle)                   FIVEWORDINLINE(0x2F3C, 0x0004, 0x0011, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInterlaceStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInterlaceStyle(
  GraphicsExportComponent   ci,
  unsigned long *           interlaceStyle)                   FIVEWORDINLINE(0x2F3C, 0x0004, 0x0012, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetMetaData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetMetaData(
  GraphicsExportComponent   ci,
  void *                    userData)                         FIVEWORDINLINE(0x2F3C, 0x0004, 0x0013, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetMetaData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetMetaData(
  GraphicsExportComponent   ci,
  void *                    userData)                         FIVEWORDINLINE(0x2F3C, 0x0004, 0x0014, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetTargetDataSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetTargetDataSize(
  GraphicsExportComponent   ci,
  unsigned long             targetDataSize)                   FIVEWORDINLINE(0x2F3C, 0x0004, 0x0015, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetTargetDataSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetTargetDataSize(
  GraphicsExportComponent   ci,
  unsigned long *           targetDataSize)                   FIVEWORDINLINE(0x2F3C, 0x0004, 0x0016, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetCompressionMethod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetCompressionMethod(
  GraphicsExportComponent   ci,
  long                      compressionMethod)                FIVEWORDINLINE(0x2F3C, 0x0004, 0x0017, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetCompressionMethod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetCompressionMethod(
  GraphicsExportComponent   ci,
  long *                    compressionMethod)                FIVEWORDINLINE(0x2F3C, 0x0004, 0x0018, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetCompressionQuality()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetCompressionQuality(
  GraphicsExportComponent   ci,
  CodecQ                    spatialQuality)                   FIVEWORDINLINE(0x2F3C, 0x0004, 0x0019, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetCompressionQuality()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetCompressionQuality(
  GraphicsExportComponent   ci,
  CodecQ *                  spatialQuality)                   FIVEWORDINLINE(0x2F3C, 0x0004, 0x001A, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetResolution()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetResolution(
  GraphicsExportComponent   ci,
  Fixed                     horizontalResolution,
  Fixed                     verticalResolution)               FIVEWORDINLINE(0x2F3C, 0x0008, 0x001B, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetResolution()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetResolution(
  GraphicsExportComponent   ci,
  Fixed *                   horizontalResolution,
  Fixed *                   verticalResolution)               FIVEWORDINLINE(0x2F3C, 0x0008, 0x001C, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetDepth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetDepth(
  GraphicsExportComponent   ci,
  long                      depth)                            FIVEWORDINLINE(0x2F3C, 0x0004, 0x001D, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetDepth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetDepth(
  GraphicsExportComponent   ci,
  long *                    depth)                            FIVEWORDINLINE(0x2F3C, 0x0004, 0x001E, 0x7000, 0xA82A);


/* (2 unused selectors) */
/*
 *  GraphicsExportSetColorSyncProfile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetColorSyncProfile(
  GraphicsExportComponent   ci,
  Handle                    colorSyncProfile)                 FIVEWORDINLINE(0x2F3C, 0x0004, 0x0021, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetColorSyncProfile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetColorSyncProfile(
  GraphicsExportComponent   ci,
  Handle *                  colorSyncProfile)                 FIVEWORDINLINE(0x2F3C, 0x0004, 0x0022, 0x7000, 0xA82A);


/* Always implemented by the base graphics exporter: */
/*
 *  GraphicsExportSetProgressProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetProgressProc(
  GraphicsExportComponent    ci,
  ICMProgressProcRecordPtr   progressProc)                    FIVEWORDINLINE(0x2F3C, 0x0004, 0x0023, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetProgressProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetProgressProc(
  GraphicsExportComponent    ci,
  ICMProgressProcRecordPtr   progressProc)                    FIVEWORDINLINE(0x2F3C, 0x0004, 0x0024, 0x7000, 0xA82A);


/* Sources for the input image: */
/*
 *  GraphicsExportSetInputDataReference()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetInputDataReference(
  GraphicsExportComponent   ci,
  Handle                    dataRef,
  OSType                    dataRefType,
  ImageDescriptionHandle    desc)                             FIVEWORDINLINE(0x2F3C, 0x000C, 0x0025, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputDataReference()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputDataReference(
  GraphicsExportComponent   ci,
  Handle *                  dataRef,
  OSType *                  dataRefType)                      FIVEWORDINLINE(0x2F3C, 0x0008, 0x0026, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetInputFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetInputFile(
  GraphicsExportComponent   ci,
  const FSSpec *            theFile,
  ImageDescriptionHandle    desc)                             FIVEWORDINLINE(0x2F3C, 0x0008, 0x0027, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputFile(
  GraphicsExportComponent   ci,
  FSSpec *                  theFile)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x0028, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetInputHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetInputHandle(
  GraphicsExportComponent   ci,
  Handle                    h,
  ImageDescriptionHandle    desc)                             FIVEWORDINLINE(0x2F3C, 0x0008, 0x0029, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputHandle(
  GraphicsExportComponent   ci,
  Handle *                  h)                                FIVEWORDINLINE(0x2F3C, 0x0004, 0x002A, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetInputPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetInputPtr(
  GraphicsExportComponent   ci,
  Ptr                       p,
  unsigned long             size,
  ImageDescriptionHandle    desc)                             FIVEWORDINLINE(0x2F3C, 0x000C, 0x002B, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputPtr(
  GraphicsExportComponent   ci,
  Ptr *                     p,
  unsigned long *           size)                             FIVEWORDINLINE(0x2F3C, 0x0008, 0x002C, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetInputGraphicsImporter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetInputGraphicsImporter(
  GraphicsExportComponent   ci,
  GraphicsImportComponent   grip)                             FIVEWORDINLINE(0x2F3C, 0x0004, 0x002D, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputGraphicsImporter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputGraphicsImporter(
  GraphicsExportComponent    ci,
  GraphicsImportComponent *  grip)                            FIVEWORDINLINE(0x2F3C, 0x0004, 0x002E, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetInputPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetInputPicture(
  GraphicsExportComponent   ci,
  PicHandle                 picture)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x002F, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputPicture(
  GraphicsExportComponent   ci,
  PicHandle *               picture)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x0030, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetInputGWorld()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetInputGWorld(
  GraphicsExportComponent   ci,
  GWorldPtr                 gworld)                           FIVEWORDINLINE(0x2F3C, 0x0004, 0x0031, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputGWorld()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputGWorld(
  GraphicsExportComponent   ci,
  GWorldPtr *               gworld)                           FIVEWORDINLINE(0x2F3C, 0x0004, 0x0032, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetInputPixmap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetInputPixmap(
  GraphicsExportComponent   ci,
  PixMapHandle              pixmap)                           FIVEWORDINLINE(0x2F3C, 0x0004, 0x0033, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputPixmap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputPixmap(
  GraphicsExportComponent   ci,
  PixMapHandle *            pixmap)                           FIVEWORDINLINE(0x2F3C, 0x0004, 0x0034, 0x7000, 0xA82A);


/* Only applicable when the input is a data reference, file, handle or ptr: */
/*
 *  GraphicsExportSetInputOffsetAndLimit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetInputOffsetAndLimit(
  GraphicsExportComponent   ci,
  unsigned long             offset,
  unsigned long             limit)                            FIVEWORDINLINE(0x2F3C, 0x0008, 0x0035, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputOffsetAndLimit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputOffsetAndLimit(
  GraphicsExportComponent   ci,
  unsigned long *           offset,
  unsigned long *           limit)                            FIVEWORDINLINE(0x2F3C, 0x0008, 0x0036, 0x7000, 0xA82A);


/* Used by format-specific graphics exporters when transcoding: */
/*
 *  GraphicsExportMayExporterReadInputData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportMayExporterReadInputData(
  GraphicsExportComponent   ci,
  Boolean *                 mayReadInputData)                 FIVEWORDINLINE(0x2F3C, 0x0004, 0x0037, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputDataSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputDataSize(
  GraphicsExportComponent   ci,
  unsigned long *           size)                             FIVEWORDINLINE(0x2F3C, 0x0004, 0x0038, 0x7000, 0xA82A);


/*
 *  GraphicsExportReadInputData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportReadInputData(
  GraphicsExportComponent   ci,
  void *                    dataPtr,
  unsigned long             dataOffset,
  unsigned long             dataSize)                         FIVEWORDINLINE(0x2F3C, 0x000C, 0x0039, 0x7000, 0xA82A);


/* Used by format-specific graphics exporters, especially when doing standalone export: */
/*
 *  GraphicsExportGetInputImageDescription()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputImageDescription(
  GraphicsExportComponent   ci,
  ImageDescriptionHandle *  desc)                             FIVEWORDINLINE(0x2F3C, 0x0004, 0x003A, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputImageDimensions()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputImageDimensions(
  GraphicsExportComponent   ci,
  Rect *                    dimensions)                       FIVEWORDINLINE(0x2F3C, 0x0004, 0x003B, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputImageDepth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputImageDepth(
  GraphicsExportComponent   ci,
  long *                    inputDepth)                       FIVEWORDINLINE(0x2F3C, 0x0004, 0x003C, 0x7000, 0xA82A);


/*
 *  GraphicsExportDrawInputImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportDrawInputImage(
  GraphicsExportComponent   ci,
  CGrafPtr                  gw,
  GDHandle                  gd,
  const Rect *              srcRect,
  const Rect *              dstRect)                          FIVEWORDINLINE(0x2F3C, 0x0010, 0x003D, 0x7000, 0xA82A);


/* Destinations for the output image: */
/*
 *  GraphicsExportSetOutputDataReference()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetOutputDataReference(
  GraphicsExportComponent   ci,
  Handle                    dataRef,
  OSType                    dataRefType)                      FIVEWORDINLINE(0x2F3C, 0x0008, 0x003E, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetOutputDataReference()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetOutputDataReference(
  GraphicsExportComponent   ci,
  Handle *                  dataRef,
  OSType *                  dataRefType)                      FIVEWORDINLINE(0x2F3C, 0x0008, 0x003F, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetOutputFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetOutputFile(
  GraphicsExportComponent   ci,
  const FSSpec *            theFile)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x0040, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetOutputFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetOutputFile(
  GraphicsExportComponent   ci,
  FSSpec *                  theFile)                          FIVEWORDINLINE(0x2F3C, 0x0004, 0x0041, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetOutputHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetOutputHandle(
  GraphicsExportComponent   ci,
  Handle                    h)                                FIVEWORDINLINE(0x2F3C, 0x0004, 0x0042, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetOutputHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetOutputHandle(
  GraphicsExportComponent   ci,
  Handle *                  h)                                FIVEWORDINLINE(0x2F3C, 0x0004, 0x0043, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetOutputOffsetAndMaxSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetOutputOffsetAndMaxSize(
  GraphicsExportComponent   ci,
  unsigned long             offset,
  unsigned long             maxSize,
  Boolean                   truncateFile)                     FIVEWORDINLINE(0x2F3C, 0x000A, 0x0044, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetOutputOffsetAndMaxSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetOutputOffsetAndMaxSize(
  GraphicsExportComponent   ci,
  unsigned long *           offset,
  unsigned long *           maxSize,
  Boolean *                 truncateFile)                     FIVEWORDINLINE(0x2F3C, 0x000C, 0x0045, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetOutputFileTypeAndCreator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetOutputFileTypeAndCreator(
  GraphicsExportComponent   ci,
  OSType                    fileType,
  OSType                    fileCreator)                      FIVEWORDINLINE(0x2F3C, 0x0008, 0x0046, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetOutputFileTypeAndCreator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetOutputFileTypeAndCreator(
  GraphicsExportComponent   ci,
  OSType *                  fileType,
  OSType *                  fileCreator)                      FIVEWORDINLINE(0x2F3C, 0x0008, 0x0047, 0x7000, 0xA82A);


/* Used by format-specific graphics exporters: */
/*
 *  GraphicsExportWriteOutputData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportWriteOutputData(
  GraphicsExportComponent   ci,
  const void *              dataPtr,
  unsigned long             dataSize)                         FIVEWORDINLINE(0x2F3C, 0x0008, 0x0048, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetOutputMark()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetOutputMark(
  GraphicsExportComponent   ci,
  unsigned long             mark)                             FIVEWORDINLINE(0x2F3C, 0x0004, 0x0049, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetOutputMark()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetOutputMark(
  GraphicsExportComponent   ci,
  unsigned long *           mark)                             FIVEWORDINLINE(0x2F3C, 0x0004, 0x004A, 0x7000, 0xA82A);


/*
 *  GraphicsExportReadOutputData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 4.0 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 4.0 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportReadOutputData(
  GraphicsExportComponent   ci,
  void *                    dataPtr,
  unsigned long             dataOffset,
  unsigned long             dataSize)                         FIVEWORDINLINE(0x2F3C, 0x000C, 0x004B, 0x7000, 0xA82A);


/* Allows embedded thumbnail creation, if supported. */
/*
 *  GraphicsExportSetThumbnailEnabled()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0.2 and later
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.1 and later
 *    Windows:          in qtmlClient.lib 5.0.2 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetThumbnailEnabled(
  GraphicsExportComponent   ci,
  Boolean                   enableThumbnail,
  long                      maxThumbnailWidth,
  long                      maxThumbnailHeight)               FIVEWORDINLINE(0x2F3C, 0x000A, 0x004C, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetThumbnailEnabled()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0.2 and later
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.1 and later
 *    Windows:          in qtmlClient.lib 5.0.2 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetThumbnailEnabled(
  GraphicsExportComponent   ci,
  Boolean *                 thumbnailEnabled,
  long *                    maxThumbnailWidth,
  long *                    maxThumbnailHeight)               FIVEWORDINLINE(0x2F3C, 0x000C, 0x004D, 0x7000, 0xA82A);


/* Allows export of Exif files, if supported.  This disables Exif-incompatible settings such as grayscale JPEG and compressed TIFF, and enables export of Exif metadata. */
/*
 *  GraphicsExportSetExifEnabled()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0.2 and later
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.1 and later
 *    Windows:          in qtmlClient.lib 5.0.2 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetExifEnabled(
  GraphicsExportComponent   ci,
  Boolean                   enableExif)                       FIVEWORDINLINE(0x2F3C, 0x0002, 0x004E, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetExifEnabled()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 5.0.2 and later
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.1 and later
 *    Windows:          in qtmlClient.lib 5.0.2 and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetExifEnabled(
  GraphicsExportComponent   ci,
  Boolean *                 exifEnabled)                      FIVEWORDINLINE(0x2F3C, 0x0004, 0x004F, 0x7000, 0xA82A);


#if TARGET_API_MAC_OSX
/*
 *  GraphicsExportSetInputCGImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetInputCGImage(
  GraphicsExportComponent   ci,
  CGImageRef                imageRef)                         FIVEWORDINLINE(0x2F3C, 0x0004, 0x0050, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputCGImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputCGImage(
  GraphicsExportComponent   ci,
  CGImageRef *              imageRefOut)                      FIVEWORDINLINE(0x2F3C, 0x0004, 0x0051, 0x7000, 0xA82A);


/*
 *  GraphicsExportSetInputCGBitmapContext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetInputCGBitmapContext(
  GraphicsExportComponent   ci,
  CGContextRef              bitmapContextRef)                 FIVEWORDINLINE(0x2F3C, 0x0004, 0x0052, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetInputCGBitmapContext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetInputCGBitmapContext(
  GraphicsExportComponent   ci,
  CGContextRef *            bitmapContextRefOut)              FIVEWORDINLINE(0x2F3C, 0x0004, 0x0053, 0x7000, 0xA82A);


#endif  /* TARGET_API_MAC_OSX */

/*
 *  GraphicsExportSetFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
GraphicsExportSetFlags(
  GraphicsExportComponent   ci,
  UInt32                    flags)                            FIVEWORDINLINE(0x2F3C, 0x0004, 0x0054, 0x7000, 0xA82A);


/*
 *  GraphicsExportGetFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
GraphicsExportGetFlags(
  GraphicsExportComponent   ci,
  UInt32 *                  flagsOut)                         FIVEWORDINLINE(0x2F3C, 0x0004, 0x0055, 0x7000, 0xA82A);




typedef ComponentInstance               ImageTranscoderComponent;
enum {
  ImageTranscodererComponentType = FOUR_CHAR_CODE('imtc')
};


/** These are ImageTranscoder procedures **/
/*
 *  ImageTranscoderBeginSequence()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
ImageTranscoderBeginSequence(
  ImageTranscoderComponent   itc,
  ImageDescriptionHandle     srcDesc,
  ImageDescriptionHandle *   dstDesc,
  void *                     data,
  long                       dataSize)                        FIVEWORDINLINE(0x2F3C, 0x0010, 0x0001, 0x7000, 0xA82A);


/*
 *  ImageTranscoderConvert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
ImageTranscoderConvert(
  ImageTranscoderComponent   itc,
  void *                     srcData,
  long                       srcDataSize,
  void **                    dstData,
  long *                     dstDataSize)                     FIVEWORDINLINE(0x2F3C, 0x0010, 0x0002, 0x7000, 0xA82A);


/*
 *  ImageTranscoderDisposeData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
ImageTranscoderDisposeData(
  ImageTranscoderComponent   itc,
  void *                     dstData)                         FIVEWORDINLINE(0x2F3C, 0x0004, 0x0003, 0x7000, 0xA82A);


/*
 *  ImageTranscoderEndSequence()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeLib 2.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in qtmlClient.lib 3.0 and later
 */
EXTERN_API( ComponentResult )
ImageTranscoderEndSequence(ImageTranscoderComponent itc)      FIVEWORDINLINE(0x2F3C, 0x0000, 0x0004, 0x7000, 0xA82A);




#if (MAC_OS_X_VERSION_MAX_ALLOWED == MAC_OS_X_VERSION_10_2) || !defined(kComponentPropertyListenerCollectionContextVersion)



/* MixedMode ProcInfo constants for component property calls */
enum {
    uppCallComponentGetComponentPropertyInfoProcInfo = 0x0003FFF0,
    uppCallComponentGetComponentPropertyProcInfo = 0x0003FFF0,
    uppCallComponentSetComponentPropertyProcInfo = 0x0000FFF0,
    uppCallComponentAddComponentPropertyListenerProcInfo = 0x0000FFF0,
    uppCallComponentRemoveComponentPropertyListenerProcInfo = 0x0000FFF0
};



/* == CallComponentGetComponentPropertyInfo flags == */
enum {
  kComponentPropertyFlagCanSetLater = (1L << 0),
  kComponentPropertyFlagCanSetNow = (1L << 1),
  kComponentPropertyFlagCanGetLater = (1L << 2),
  kComponentPropertyFlagCanGetNow = (1L << 3),
  kComponentPropertyFlagHasExtendedInfo = (1L << 4),
  kComponentPropertyFlagValueMustBeReleased = (1L << 5),
  kComponentPropertyFlagValueIsCFTypeRef = (1L << 6),
  kComponentPropertyFlagGetBufferMustBeInitialized = (1L << 7),
  kComponentPropertyFlagWillNotifyListeners = (1L << 8)
};


typedef OSType                          ComponentPropertyClass;
typedef OSType                          ComponentPropertyID;
typedef OSType                          ComponentValueType;
typedef void *                          ComponentValuePtr;
typedef const void *                    ConstComponentValuePtr;

/* == standard property class constants == */
enum {
  kComponentPropertyClassPropertyInfo = FOUR_CHAR_CODE('pnfo'), /* property info class */
                                        /* property info property IDs */
  kComponentPropertyInfoList    = FOUR_CHAR_CODE('list'), /* array of ComponentPropertyInfo (CFData), one for each property */
  kComponentPropertyCacheSeed   = FOUR_CHAR_CODE('seed'), /* property cache seed value */
  kComponentPropertyCacheFlags  = FOUR_CHAR_CODE('flgs'), /* see kComponentPropertyCache flags */
  kComponentPropertyExtendedInfo = FOUR_CHAR_CODE('meta') /* CFDictionary with extended property information*/
};


/* values for kComponentPropertyClassPropertyInfo/kComponentPropertyCacheFlags standard component property */
enum {
  kComponentPropertyCacheFlagNotPersistent = (1L << 0), /* property metadata should not be saved in persistent cache*/
  kComponentPropertyCacheFlagIsDynamic = (1L << 1) /* property metadata should not cached at all*/
};


struct ComponentPropertyInfo {
  ComponentPropertyClass  propClass;
  ComponentPropertyID  propID;
  ComponentValueType  propType;
  ByteCount           propSize;
  UInt32              propFlags;
};
typedef struct ComponentPropertyInfo    ComponentPropertyInfo;


#endif  /* #MAC_OS_X_VERSION_10_3 <= MAC_OS_X_VERSION_MAX_ALLOWED */ 



/* == "QT" prefixed Component Property calls == */

typedef CALLBACK_API( void , QTComponentPropertyListenerProcPtr )(ComponentInstance inComponent, ComponentPropertyClass inPropClass, ComponentPropertyID inPropID, void *inUserData);
typedef STACK_UPP_TYPE(QTComponentPropertyListenerProcPtr)      QTComponentPropertyListenerUPP;




/*
 *  QTGetComponentPropertyInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
QTGetComponentPropertyInfo(
  ComponentInstance        inComponent,
  ComponentPropertyClass   inPropClass,
  ComponentPropertyID      inPropID,
  ComponentValueType *     outPropType,            /* can be NULL */
  ByteCount *              outPropValueSize,       /* can be NULL */
  UInt32 *                 outPropertyFlags)       /* can be NULL */ FIVEWORDINLINE(0x2F3C, 0x0014, 0xFFF5, 0x7000, 0xA82A);


/*
 *  QTGetComponentProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
QTGetComponentProperty(
  ComponentInstance        inComponent,
  ComponentPropertyClass   inPropClass,
  ComponentPropertyID      inPropID,
  ByteCount                inPropValueSize,
  ComponentValuePtr        outPropValueAddress,
  ByteCount *              outPropValueSizeUsed)       /* can be NULL */ FIVEWORDINLINE(0x2F3C, 0x0014, 0xFFF4, 0x7000, 0xA82A);


/*
 *  QTSetComponentProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
QTSetComponentProperty(
  ComponentInstance        inComponent,
  ComponentPropertyClass   inPropClass,
  ComponentPropertyID      inPropID,
  ByteCount                inPropValueSize,
  ConstComponentValuePtr   inPropValueAddress)                FIVEWORDINLINE(0x2F3C, 0x0010, 0xFFF3, 0x7000, 0xA82A);



/*
 *  QTAddComponentPropertyListener()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
QTAddComponentPropertyListener(
  ComponentInstance                inComponent,
  ComponentPropertyClass           inPropClass,
  ComponentPropertyID              inPropID,
  QTComponentPropertyListenerUPP   inDispatchProc,
  void *                           inUserData)           /* can be NULL */ FIVEWORDINLINE(0x2F3C, 0x0010, 0xFFF2, 0x7000, 0xA82A);


/*
 *  QTRemoveComponentPropertyListener()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API( ComponentResult )
QTRemoveComponentPropertyListener(
  ComponentInstance                inComponent,
  ComponentPropertyClass           inPropClass,
  ComponentPropertyID              inPropID,
  QTComponentPropertyListenerUPP   inDispatchProc,
  void *                           inUserData)           /* can be NULL */ FIVEWORDINLINE(0x2F3C, 0x0010, 0xFFF1, 0x7000, 0xA82A);




/* == "QT" prefixed Component Property Listener helpers == */


typedef CFTypeRef                       QTComponentPropertyListenersRef;
typedef struct QTComponentPropertyListenerCollectionContext  QTComponentPropertyListenerCollectionContext;
typedef CALLBACK_API( Boolean , QTComponentPropertyListenerFilterProcPtr )(QTComponentPropertyListenersRef inCollection, const QTComponentPropertyListenerCollectionContext *inCollectionContext, ComponentInstance inNotifier, ComponentPropertyClass inPropClass, ComponentPropertyID inPropID, QTComponentPropertyListenerUPP inListenerCallbackProc, const void *inListenerProcRefCon, const void *inFilterProcRefCon);
typedef STACK_UPP_TYPE(QTComponentPropertyListenerFilterProcPtr)  QTComponentPropertyListenerFilterUPP;
#define kQTComponentPropertyListenerCollectionContextVersion 1
struct QTComponentPropertyListenerCollectionContext {
  UInt32              version;                /* struct version */
  QTComponentPropertyListenerFilterUPP  filterProcUPP;
  void *              filterProcData;
};

#if CALL_NOT_IN_CARBON
/*
 *  NewQTComponentPropertyListenerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API_C( QTComponentPropertyListenerUPP )
NewQTComponentPropertyListenerUPP(QTComponentPropertyListenerProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQTComponentPropertyListenerProcInfo = 0x00003FC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QTComponentPropertyListenerUPP) NewQTComponentPropertyListenerUPP(QTComponentPropertyListenerProcPtr userRoutine) { return (QTComponentPropertyListenerUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTComponentPropertyListenerProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQTComponentPropertyListenerUPP(userRoutine) (QTComponentPropertyListenerUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTComponentPropertyListenerProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQTComponentPropertyListenerFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API_C( QTComponentPropertyListenerFilterUPP )
NewQTComponentPropertyListenerFilterUPP(QTComponentPropertyListenerFilterProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQTComponentPropertyListenerFilterProcInfo = 0x003FFFD0 };  /* pascal 1_byte Func(4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QTComponentPropertyListenerFilterUPP) NewQTComponentPropertyListenerFilterUPP(QTComponentPropertyListenerFilterProcPtr userRoutine) { return (QTComponentPropertyListenerFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTComponentPropertyListenerFilterProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQTComponentPropertyListenerFilterUPP(userRoutine) (QTComponentPropertyListenerFilterUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTComponentPropertyListenerFilterProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeQTComponentPropertyListenerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API_C( void )
DisposeQTComponentPropertyListenerUPP(QTComponentPropertyListenerUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQTComponentPropertyListenerUPP(QTComponentPropertyListenerUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQTComponentPropertyListenerUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQTComponentPropertyListenerFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API_C( void )
DisposeQTComponentPropertyListenerFilterUPP(QTComponentPropertyListenerFilterUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQTComponentPropertyListenerFilterUPP(QTComponentPropertyListenerFilterUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQTComponentPropertyListenerFilterUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeQTComponentPropertyListenerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API_C( void )
InvokeQTComponentPropertyListenerUPP(
  ComponentInstance               inComponent,
  ComponentPropertyClass          inPropClass,
  ComponentPropertyID             inPropID,
  void *                          inUserData,
  QTComponentPropertyListenerUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQTComponentPropertyListenerUPP(ComponentInstance inComponent, ComponentPropertyClass inPropClass, ComponentPropertyID inPropID, void * inUserData, QTComponentPropertyListenerUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppQTComponentPropertyListenerProcInfo, inComponent, inPropClass, inPropID, inUserData); }
  #else
    #define InvokeQTComponentPropertyListenerUPP(inComponent, inPropClass, inPropID, inUserData, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppQTComponentPropertyListenerProcInfo, (inComponent), (inPropClass), (inPropID), (inUserData))
  #endif
#endif

/*
 *  InvokeQTComponentPropertyListenerFilterUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API_C( Boolean )
InvokeQTComponentPropertyListenerFilterUPP(
  QTComponentPropertyListenersRef                       inCollection,
  const QTComponentPropertyListenerCollectionContext *  inCollectionContext,
  ComponentInstance                                     inNotifier,
  ComponentPropertyClass                                inPropClass,
  ComponentPropertyID                                   inPropID,
  QTComponentPropertyListenerUPP                        inListenerCallbackProc,
  const void *                                          inListenerProcRefCon,
  const void *                                          inFilterProcRefCon,
  QTComponentPropertyListenerFilterUPP                  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeQTComponentPropertyListenerFilterUPP(QTComponentPropertyListenersRef inCollection, const QTComponentPropertyListenerCollectionContext * inCollectionContext, ComponentInstance inNotifier, ComponentPropertyClass inPropClass, ComponentPropertyID inPropID, QTComponentPropertyListenerUPP inListenerCallbackProc, const void * inListenerProcRefCon, const void * inFilterProcRefCon, QTComponentPropertyListenerFilterUPP userUPP) { return (Boolean)CALL_EIGHT_PARAMETER_UPP(userUPP, uppQTComponentPropertyListenerFilterProcInfo, inCollection, inCollectionContext, inNotifier, inPropClass, inPropID, inListenerCallbackProc, inListenerProcRefCon, inFilterProcRefCon); }
  #else
    #define InvokeQTComponentPropertyListenerFilterUPP(inCollection, inCollectionContext, inNotifier, inPropClass, inPropID, inListenerCallbackProc, inListenerProcRefCon, inFilterProcRefCon, userUPP) (Boolean)CALL_EIGHT_PARAMETER_UPP((userUPP), uppQTComponentPropertyListenerFilterProcInfo, (inCollection), (inCollectionContext), (inNotifier), (inPropClass), (inPropID), (inListenerCallbackProc), (inListenerProcRefCon), (inFilterProcRefCon))
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewQTComponentPropertyListenerProc(userRoutine)     NewQTComponentPropertyListenerUPP(userRoutine)
    #define NewQTComponentPropertyListenerFilterProc(userRoutine) NewQTComponentPropertyListenerFilterUPP(userRoutine)
    #define CallQTComponentPropertyListenerProc(userRoutine, inComponent, inPropClass, inPropID, inUserData) InvokeQTComponentPropertyListenerUPP(inComponent, inPropClass, inPropID, inUserData, userRoutine)
    #define CallQTComponentPropertyListenerFilterProc(userRoutine, inCollection, inCollectionContext, inNotifier, inPropClass, inPropID, inListenerCallbackProc, inListenerProcRefCon, inFilterProcRefCon) InvokeQTComponentPropertyListenerFilterUPP(inCollection, inCollectionContext, inNotifier, inPropClass, inPropID, inListenerCallbackProc, inListenerProcRefCon, inFilterProcRefCon, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*
 *  QTComponentPropertyListenerCollectionCreate()
 *  
 *  Summary:
 *    Create a collection to use with the functions
 *    ComponentPropertyListenerCollectionAddListener,
 *    ComponentPropertyListenerCollectionRemoveListener,
 *    ComponentPropertyListenerCollectionNotifyListeners,
 *    ComponentPropertyListenerCollectionIsEmpty, and
 *    ComponentPropertyListenerCollectionHasListenersForProperty.
 *  
 *  Parameters:
 *    
 *    outCollection:
 *      Returns the new, empty, listener collection.
 *    
 *    inAllocator:
 *      Allocator used to create the collection and it's contents.
 *    
 *    inContext:
 *      The listener collection context. May be NULL.  A copy of the
 *      contents of the structure is made, so a pointer to a structure
 *      on the stack can be passed.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API_C( OSStatus )
QTComponentPropertyListenerCollectionCreate(
  CFAllocatorRef                                        inAllocator,         /* can be NULL */
  const QTComponentPropertyListenerCollectionContext *  inContext,           /* can be NULL */
  QTComponentPropertyListenersRef *                     outCollection);



/*
 *  QTComponentPropertyListenerCollectionAddListener()
 *  
 *  Summary:
 *    Add a listener callback for the specified property class and ID
 *    to a property listener collection.
 *  
 *  Parameters:
 *    
 *    inCollection:
 *      The property listener collection.
 *    
 *    inPropClass:
 *      The property class.
 *    
 *    inPropID:
 *      The property ID.
 *    
 *    inListenerProc:
 *      The property listener callback function.
 *    
 *    inListenerProcRefCon:
 *      The data parameter to pass to the listener callback function.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API_C( OSStatus )
QTComponentPropertyListenerCollectionAddListener(
  QTComponentPropertyListenersRef   inCollection,
  ComponentPropertyClass            inPropClass,
  ComponentPropertyID               inPropID,
  QTComponentPropertyListenerUPP    inListenerProc,
  const void *                      inListenerProcRefCon);


/*
 *  QTComponentPropertyListenerCollectionRemoveListener()
 *  
 *  Summary:
 *    Remove a listener callback for the specified property class and
 *    ID from a property listener collection.
 *  
 *  Parameters:
 *    
 *    inCollection:
 *      The property listener collection.
 *    
 *    inPropClass:
 *      The property class.
 *    
 *    inPropID:
 *      The property ID.
 *    
 *    inListenerProc:
 *      The property listener callback function.
 *    
 *    inListenerProcRefCon:
 *      The data parameter to pass to the listener callback function.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API_C( OSStatus )
QTComponentPropertyListenerCollectionRemoveListener(
  QTComponentPropertyListenersRef   inCollection,
  ComponentPropertyClass            inPropClass,
  ComponentPropertyID               inPropID,
  QTComponentPropertyListenerUPP    inListenerProc,
  const void *                      inListenerProcRefCon);


/*
 *  QTComponentPropertyListenerCollectionNotifyListeners()
 *  
 *  Summary:
 *    Call all listener callbacks in the collection registered for the
 *    specified property class and ID.
 *  
 *  Discussion:
 *    If the "filterProcUPP" in the collection's context is non-NULL,
 *    the filter function will be called before each registered
 *    listener that matches the specified property class and ID. If the
 *    filter function return false, the listener proc will not be
 *    called. This is intended to allow a component to change the
 *    calling semantics (call another thread, etc), to use a different
 *    listener callback signature, etc.
 *  
 *  Parameters:
 *    
 *    inCollection:
 *      The property listener collection.
 *    
 *    inNotifier:
 *      The calling ComponentInstance.
 *    
 *    inPropClass:
 *      The property class.
 *    
 *    inPropID:
 *      The property ID.
 *    
 *    inFilterProcRefCon:
 *      The data parameter to pass to the filter function.
 *    
 *    inFlags:
 *      Flags.  Must be set to 0.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API_C( OSStatus )
QTComponentPropertyListenerCollectionNotifyListeners(
  QTComponentPropertyListenersRef   inCollection,
  ComponentInstance                 inNotifier,
  ComponentPropertyClass            inPropClass,
  ComponentPropertyID               inPropID,
  const void *                      inFilterProcRefCon,       /* can be NULL */
  UInt32                            inFlags);


/*
 *  QTComponentPropertyListenerCollectionIsEmpty()
 *  
 *  Summary:
 *    Return true if the listener collection is empty.
 *  
 *  Parameters:
 *    
 *    inCollection:
 *      The property listener collection.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API_C( Boolean )
QTComponentPropertyListenerCollectionIsEmpty(QTComponentPropertyListenersRef inCollection);


/*
 *  QTComponentPropertyListenerCollectionHasListenersForProperty()
 *  
 *  Summary:
 *    Returns true if there are any listeners registered for the
 *    specified property class and ID.
 *  
 *  Parameters:
 *    
 *    inCollection:
 *      The property listener collection.
 *    
 *    inPropClass:
 *      The property class.
 *    
 *    inPropID:
 *      The property ID.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.3 (or QuickTime 6.4) and later
 */
EXTERN_API_C( Boolean )
QTComponentPropertyListenerCollectionHasListenersForProperty(
  QTComponentPropertyListenersRef   inCollection,
  ComponentPropertyClass            inPropClass,
  ComponentPropertyID               inPropID);


/* DRM properties*/
enum {
  kQTPropertyClass_DRM          = FOUR_CHAR_CODE('drm ')
};

enum {
  kQTDRMPropertyID_InteractWithUser = FOUR_CHAR_CODE('shui'), /* Boolean**/
  kQTDRMPropertyID_IsProtected  = FOUR_CHAR_CODE('prot'), /* Boolean**/
  kQTDRMPropertyID_IsAuthorized = FOUR_CHAR_CODE('auth') /* Boolean**/
};



/* UPP call backs */

/* selectors for component calls */
enum {
    kGraphicsImportSetDataReferenceSelect      = 0x0001,
    kGraphicsImportGetDataReferenceSelect      = 0x0002,
    kGraphicsImportSetDataFileSelect           = 0x0003,
    kGraphicsImportGetDataFileSelect           = 0x0004,
    kGraphicsImportSetDataHandleSelect         = 0x0005,
    kGraphicsImportGetDataHandleSelect         = 0x0006,
    kGraphicsImportGetImageDescriptionSelect   = 0x0007,
    kGraphicsImportGetDataOffsetAndSizeSelect  = 0x0008,
    kGraphicsImportReadDataSelect              = 0x0009,
    kGraphicsImportSetClipSelect               = 0x000A,
    kGraphicsImportGetClipSelect               = 0x000B,
    kGraphicsImportSetSourceRectSelect         = 0x000C,
    kGraphicsImportGetSourceRectSelect         = 0x000D,
    kGraphicsImportGetNaturalBoundsSelect      = 0x000E,
    kGraphicsImportDrawSelect                  = 0x000F,
    kGraphicsImportSetGWorldSelect             = 0x0010,
    kGraphicsImportGetGWorldSelect             = 0x0011,
    kGraphicsImportSetMatrixSelect             = 0x0012,
    kGraphicsImportGetMatrixSelect             = 0x0013,
    kGraphicsImportSetBoundsRectSelect         = 0x0014,
    kGraphicsImportGetBoundsRectSelect         = 0x0015,
    kGraphicsImportSaveAsPictureSelect         = 0x0016,
    kGraphicsImportSetGraphicsModeSelect       = 0x0017,
    kGraphicsImportGetGraphicsModeSelect       = 0x0018,
    kGraphicsImportSetQualitySelect            = 0x0019,
    kGraphicsImportGetQualitySelect            = 0x001A,
    kGraphicsImportSaveAsQuickTimeImageFileSelect = 0x001B,
    kGraphicsImportSetDataReferenceOffsetAndLimitSelect = 0x001C,
    kGraphicsImportGetDataReferenceOffsetAndLimitSelect = 0x001D,
    kGraphicsImportGetAliasedDataReferenceSelect = 0x001E,
    kGraphicsImportValidateSelect              = 0x001F,
    kGraphicsImportGetMetaDataSelect           = 0x0020,
    kGraphicsImportGetMIMETypeListSelect       = 0x0021,
    kGraphicsImportDoesDrawAllPixelsSelect     = 0x0022,
    kGraphicsImportGetAsPictureSelect          = 0x0023,
    kGraphicsImportExportImageFileSelect       = 0x0024,
    kGraphicsImportGetExportImageTypeListSelect = 0x0025,
    kGraphicsImportDoExportImageFileDialogSelect = 0x0026,
    kGraphicsImportGetExportSettingsAsAtomContainerSelect = 0x0027,
    kGraphicsImportSetExportSettingsFromAtomContainerSelect = 0x0028,
    kGraphicsImportSetProgressProcSelect       = 0x0029,
    kGraphicsImportGetProgressProcSelect       = 0x002A,
    kGraphicsImportGetImageCountSelect         = 0x002B,
    kGraphicsImportSetImageIndexSelect         = 0x002C,
    kGraphicsImportGetImageIndexSelect         = 0x002D,
    kGraphicsImportGetDataOffsetAndSize64Select = 0x002E,
    kGraphicsImportReadData64Select            = 0x002F,
    kGraphicsImportSetDataReferenceOffsetAndLimit64Select = 0x0030,
    kGraphicsImportGetDataReferenceOffsetAndLimit64Select = 0x0031,
    kGraphicsImportGetDefaultMatrixSelect      = 0x0032,
    kGraphicsImportGetDefaultClipSelect        = 0x0033,
    kGraphicsImportGetDefaultGraphicsModeSelect = 0x0034,
    kGraphicsImportGetDefaultSourceRectSelect  = 0x0035,
    kGraphicsImportGetColorSyncProfileSelect   = 0x0036,
    kGraphicsImportSetDestRectSelect           = 0x0037,
    kGraphicsImportGetDestRectSelect           = 0x0038,
    kGraphicsImportSetFlagsSelect              = 0x0039,
    kGraphicsImportGetFlagsSelect              = 0x003A,
    kGraphicsImportGetBaseDataOffsetAndSize64Select = 0x003D,
    kGraphicsImportSetImageIndexToThumbnailSelect = 0x003E,
    kGraphicsImportCreateCGImageSelect         = 0x003F,
    kGraphicsImportSaveAsPictureToDataRefSelect = 0x0040,
    kGraphicsImportSaveAsQuickTimeImageFileToDataRefSelect = 0x0041,
    kGraphicsImportExportImageFileToDataRefSelect = 0x0042,
    kGraphicsImportDoExportImageFileToDataRefDialogSelect = 0x0043,
    kGraphicsImportSetOverrideSourceColorSyncProfileRefSelect = 0x0044,
    kGraphicsImportGetOverrideSourceColorSyncProfileRefSelect = 0x0045,
    kGraphicsImportSetDestinationColorSyncProfileRefSelect = 0x0046,
    kGraphicsImportGetDestinationColorSyncProfileRefSelect = 0x0047,
    kGraphicsImportWillUseColorMatchingSelect  = 0x0048,
    kGraphicsImportGetGenericColorSyncProfileSelect = 0x0049,
    kGraphicsImportSetReturnGenericColorSyncProfileSelect = 0x004A,
    kGraphicsImportGetReturnGenericColorSyncProfileSelect = 0x004B,
    kGraphicsExportDoExportSelect              = 0x0001,
    kGraphicsExportCanTranscodeSelect          = 0x0002,
    kGraphicsExportDoTranscodeSelect           = 0x0003,
    kGraphicsExportCanUseCompressorSelect      = 0x0004,
    kGraphicsExportDoUseCompressorSelect       = 0x0005,
    kGraphicsExportDoStandaloneExportSelect    = 0x0006,
    kGraphicsExportGetDefaultFileTypeAndCreatorSelect = 0x0007,
    kGraphicsExportGetDefaultFileNameExtensionSelect = 0x0008,
    kGraphicsExportGetMIMETypeListSelect       = 0x0009,
    kGraphicsExportRequestSettingsSelect       = 0x000B,
    kGraphicsExportSetSettingsFromAtomContainerSelect = 0x000C,
    kGraphicsExportGetSettingsAsAtomContainerSelect = 0x000D,
    kGraphicsExportGetSettingsAsTextSelect     = 0x000E,
    kGraphicsExportSetDontRecompressSelect     = 0x000F,
    kGraphicsExportGetDontRecompressSelect     = 0x0010,
    kGraphicsExportSetInterlaceStyleSelect     = 0x0011,
    kGraphicsExportGetInterlaceStyleSelect     = 0x0012,
    kGraphicsExportSetMetaDataSelect           = 0x0013,
    kGraphicsExportGetMetaDataSelect           = 0x0014,
    kGraphicsExportSetTargetDataSizeSelect     = 0x0015,
    kGraphicsExportGetTargetDataSizeSelect     = 0x0016,
    kGraphicsExportSetCompressionMethodSelect  = 0x0017,
    kGraphicsExportGetCompressionMethodSelect  = 0x0018,
    kGraphicsExportSetCompressionQualitySelect = 0x0019,
    kGraphicsExportGetCompressionQualitySelect = 0x001A,
    kGraphicsExportSetResolutionSelect         = 0x001B,
    kGraphicsExportGetResolutionSelect         = 0x001C,
    kGraphicsExportSetDepthSelect              = 0x001D,
    kGraphicsExportGetDepthSelect              = 0x001E,
    kGraphicsExportSetColorSyncProfileSelect   = 0x0021,
    kGraphicsExportGetColorSyncProfileSelect   = 0x0022,
    kGraphicsExportSetProgressProcSelect       = 0x0023,
    kGraphicsExportGetProgressProcSelect       = 0x0024,
    kGraphicsExportSetInputDataReferenceSelect = 0x0025,
    kGraphicsExportGetInputDataReferenceSelect = 0x0026,
    kGraphicsExportSetInputFileSelect          = 0x0027,
    kGraphicsExportGetInputFileSelect          = 0x0028,
    kGraphicsExportSetInputHandleSelect        = 0x0029,
    kGraphicsExportGetInputHandleSelect        = 0x002A,
    kGraphicsExportSetInputPtrSelect           = 0x002B,
    kGraphicsExportGetInputPtrSelect           = 0x002C,
    kGraphicsExportSetInputGraphicsImporterSelect = 0x002D,
    kGraphicsExportGetInputGraphicsImporterSelect = 0x002E,
    kGraphicsExportSetInputPictureSelect       = 0x002F,
    kGraphicsExportGetInputPictureSelect       = 0x0030,
    kGraphicsExportSetInputGWorldSelect        = 0x0031,
    kGraphicsExportGetInputGWorldSelect        = 0x0032,
    kGraphicsExportSetInputPixmapSelect        = 0x0033,
    kGraphicsExportGetInputPixmapSelect        = 0x0034,
    kGraphicsExportSetInputOffsetAndLimitSelect = 0x0035,
    kGraphicsExportGetInputOffsetAndLimitSelect = 0x0036,
    kGraphicsExportMayExporterReadInputDataSelect = 0x0037,
    kGraphicsExportGetInputDataSizeSelect      = 0x0038,
    kGraphicsExportReadInputDataSelect         = 0x0039,
    kGraphicsExportGetInputImageDescriptionSelect = 0x003A,
    kGraphicsExportGetInputImageDimensionsSelect = 0x003B,
    kGraphicsExportGetInputImageDepthSelect    = 0x003C,
    kGraphicsExportDrawInputImageSelect        = 0x003D,
    kGraphicsExportSetOutputDataReferenceSelect = 0x003E,
    kGraphicsExportGetOutputDataReferenceSelect = 0x003F,
    kGraphicsExportSetOutputFileSelect         = 0x0040,
    kGraphicsExportGetOutputFileSelect         = 0x0041,
    kGraphicsExportSetOutputHandleSelect       = 0x0042,
    kGraphicsExportGetOutputHandleSelect       = 0x0043,
    kGraphicsExportSetOutputOffsetAndMaxSizeSelect = 0x0044,
    kGraphicsExportGetOutputOffsetAndMaxSizeSelect = 0x0045,
    kGraphicsExportSetOutputFileTypeAndCreatorSelect = 0x0046,
    kGraphicsExportGetOutputFileTypeAndCreatorSelect = 0x0047,
    kGraphicsExportWriteOutputDataSelect       = 0x0048,
    kGraphicsExportSetOutputMarkSelect         = 0x0049,
    kGraphicsExportGetOutputMarkSelect         = 0x004A,
    kGraphicsExportReadOutputDataSelect        = 0x004B,
    kGraphicsExportSetThumbnailEnabledSelect   = 0x004C,
    kGraphicsExportGetThumbnailEnabledSelect   = 0x004D,
    kGraphicsExportSetExifEnabledSelect        = 0x004E,
    kGraphicsExportGetExifEnabledSelect        = 0x004F,
    kGraphicsExportSetInputCGImageSelect       = 0x0050,
    kGraphicsExportGetInputCGImageSelect       = 0x0051,
    kGraphicsExportSetInputCGBitmapContextSelect = 0x0052,
    kGraphicsExportGetInputCGBitmapContextSelect = 0x0053,
    kGraphicsExportSetFlagsSelect              = 0x0054,
    kGraphicsExportGetFlagsSelect              = 0x0055,
    kImageTranscoderBeginSequenceSelect        = 0x0001,
    kImageTranscoderConvertSelect              = 0x0002,
    kImageTranscoderDisposeDataSelect          = 0x0003,
    kImageTranscoderEndSequenceSelect          = 0x0004,
    kQTGetComponentPropertyInfoSelect          = -11,
    kQTGetComponentPropertySelect              = -12,
    kQTSetComponentPropertySelect              = -13,
    kQTAddComponentPropertyListenerSelect      = -14,
    kQTRemoveComponentPropertyListenerSelect   = -15
};
/* Aperture modes */

/*
 *  Summary:
 *    Aperture modes
 *  
 *  Discussion:
 *    You can set the aperture mode property on a movie to indicate
 *    whether aspect ratio and clean aperture correction should be
 *    performed (kQTPropertyClass_Visual /
 *    kQTVisualPropertyID_ApertureMode). When a movie is in clean,
 *    production or encoded pixels aperture mode, each track's
 *    dimensions are overriden by special dimensions for that mode. The
 *    original track dimensions are preserved and can be restored by
 *    setting the movie into classic aperture mode. Aperture modes are
 *    not saved in movies. 
 *    You can set the aperture mode property on a decompression session
 *    options object to indicate whether pixel buffers should be tagged
 *    to enable aspect ratio and clean aperture correction
 *    (kQTPropertyClass_ICMDecompressionSessionOptions /
 *    kICMDecompressionSessionOptionsPropertyID_ApertureMode).
 */
enum {

  /*
   * An aperture mode which gives compatibility with behavior in
   * QuickTime 7.0.x and earlier. 
   * A movie in classic aperture mode uses track dimensions as set in
   * NewMovieTrack and SetTrackDimensions. 
   * A decompression session in classic aperture mode does not set the
   * clean aperture or pixel aspect ratio attachments on emitted pixel
   * buffers. 
   * Movies default to classic aperture mode. If you call
   * SetTrackDimensions on a track, the movie is automatically switched
   * into classic aperture mode.
   */
  kQTApertureMode_Classic       = FOUR_CHAR_CODE('clas'),

  /*
   * An aperture mode for general display. 
   * Where possible, video will be displayed at the correct pixel
   * aspect ratio, trimmed to the clean aperture. A movie in clean
   * aperture mode sets each track's dimensions to match its
   * kQTVisualPropertyID_CleanApertureDimensions. 
   * A decompression session in clean aperture mode sets the clean
   * aperture and pixel aspect ratio attachments on emitted pixel
   * buffers based on the image description.
   */
  kQTApertureMode_CleanAperture = FOUR_CHAR_CODE('clea'),

  /*
   * An aperture mode for modal use in authoring applications. 
   *  Where possible, video will be displayed at the correct pixel
   * aspect ratio, but without trimming to the clean aperture so that
   * the edge processing region can be viewed. A movie in production
   * aperture mode sets each track's dimensions to match its
   * kQTVisualPropertyID_ProductionApertureDimensions. 
   * A decompression session in production aperture mode sets the pixel
   * aspect ratio attachments on emitted pixel buffers based on the
   * image description.
   */
  kQTApertureMode_ProductionAperture = FOUR_CHAR_CODE('prod'),

  /*
   * An aperture mode for technical use. 
   * Displays all encoded pixels with no aspect ratio or clean aperture
   * compensation. A movie in encoded pixels aperture mode sets each
   * track's dimensions to match its
   * kQTVisualPropertyID_EncodedPixelsDimensions. 
   * A decompression session in encoded pixels aperture mode does not
   * set the clean aperture or pixel aspect ratio attachments on
   * emitted pixel buffers.
   */
  kQTApertureMode_EncodedPixels = FOUR_CHAR_CODE('enco')
};

/* Property interface for Image Descriptions */

/*
 *  Summary:
 *    Properties of image descriptions.
 */
enum {

  /*
   * Class identifier for image description properties.
   */
  kQTPropertyClass_ImageDescription = FOUR_CHAR_CODE('idsc'),

  /*
   * The width of the encoded image. Usually, but not always, this is
   * the ImageDescription's width field.
   */
  kICMImageDescriptionPropertyID_EncodedWidth = FOUR_CHAR_CODE('encw'), /* SInt32, Read/Write */

  /*
   * The height of the encoded image. Usually, but not always, this is
   * the ImageDescription's height field.
   */
  kICMImageDescriptionPropertyID_EncodedHeight = FOUR_CHAR_CODE('ench'), /* SInt32, Read/Write */

  /*
   * Describes the clean aperture of the buffer. If not specified
   * explicitly in the image description, the default clean aperture
   * (full encoded width and height) will be returned.
   */
  kICMImageDescriptionPropertyID_CleanAperture = FOUR_CHAR_CODE('clap'), /* Native-endian CleanApertureImageDescriptionExtension, Read/Write */

  /*
   * Describes the pixel aspect ratio. If not specified explicitly in
   * the image description, a square (1:1) pixel aspect ratio will be
   * returned.
   */
  kICMImageDescriptionPropertyID_PixelAspectRatio = FOUR_CHAR_CODE('pasp'), /* Native-endian PixelAspectRatioImageDescriptionExtension, Read/Write */

  /*
   * Dimensions at which the image could be displayed on a square-pixel
   * display, generally calculated using the clean aperture and pixel
   * aspect ratio. 
   * Note that this value is returned as a FixedPoint; the width and
   * height can also be read separately as rounded SInt32s via
   * kICMImageDescriptionPropertyID_CleanApertureDisplayWidth and
   * kICMImageDescriptionPropertyID_CleanApertureDisplayHeight.
   */
  kICMImageDescriptionPropertyID_CleanApertureDisplayDimensions = FOUR_CHAR_CODE('cadi'), /* FixedPoint, Read */

  /*
   * Dimensions at which the image could be displayed on a square-pixel
   * display, disregarding any clean aperture but honoring the pixel
   * aspect ratio. This may be useful for authoring applications that
   * want to expose the edge processing region. For general viewing,
   * use kICMImageDescriptionPropertyID_CleanApertureDimensions
   * instead. 
   * Note that this value is returned as a FixedPoint; the width and
   * height can also be read separately as rounded SInt32s via
   * kICMImageDescriptionPropertyID_ProductionApertureDisplayWidth and
   * kICMImageDescriptionPropertyID_ProductionApertureDisplayHeight.
   */
  kICMImageDescriptionPropertyID_ProductionApertureDisplayDimensions = FOUR_CHAR_CODE('prdi'), /* FixedPoint, Read */

  /*
   * Dimensions of the encoded image. 
   * Note that this value is returned as a FixedPoint for convenience;
   * the width and height can also be read separately as SInt32s via
   * kICMImageDescriptionPropertyID_EncodedWidth and
   * kICMImageDescriptionPropertyID_EncodedHeight.
   */
  kICMImageDescriptionPropertyID_EncodedPixelsDimensions = FOUR_CHAR_CODE('endi'), /* FixedPoint, Read */

  /*
   * A width at which the image could be displayed on a square-pixel
   * display, possibly calculated using the clean aperture and pixel
   * aspect ratio.
   */
  kICMImageDescriptionPropertyID_CleanApertureDisplayWidth = FOUR_CHAR_CODE('disw'), /* SInt32, Read */

  /*
   * A height at which the image could be displayed on a square-pixel
   * display, possibly calculated using the clean aperture and pixel
   * aspect ratio.
   */
  kICMImageDescriptionPropertyID_CleanApertureDisplayHeight = FOUR_CHAR_CODE('dish'), /* SInt32, Read */

  /*
   * A width at which the image could be displayed on a square-pixel
   * display, disregarding any clean aperture but honoring the pixel
   * aspect ratio. This may be useful for authoring applications that
   * want to expose the edge processing region. For general viewing,
   * use kICMImageDescriptionPropertyID_CleanApertureDisplayWidth
   * instead.
   */
  kICMImageDescriptionPropertyID_ProductionApertureDisplayWidth = FOUR_CHAR_CODE('pdsw'), /* SInt32, Read */

  /*
   * A height at which the image could be displayed on a square-pixel
   * display, disregarding any clean aperture but honoring the pixel
   * aspect ratio. This may be useful for authoring applications that
   * want to expose the edge processing region. For general viewing,
   * use kICMImageDescriptionPropertyID_CleanApertureDisplayHeight
   * instead.
   */
  kICMImageDescriptionPropertyID_ProductionApertureDisplayHeight = FOUR_CHAR_CODE('pdsh'), /* SInt32, Read */

  /*
   * Synonym for
   * kICMImageDescriptionPropertyID_CleanApertureDisplayWidth.
   */
  kICMImageDescriptionPropertyID_DisplayWidth = FOUR_CHAR_CODE('disw'), /* SInt32, Read */

  /*
   * Synonym for
   * kICMImageDescriptionPropertyID_CleanApertureDisplayHeight.
   */
  kICMImageDescriptionPropertyID_DisplayHeight = FOUR_CHAR_CODE('dish'), /* SInt32, Read */

  /*
   * Synonym for
   * kICMImageDescriptionPropertyID_ProductionApertureDisplayWidth.
   */
  kICMImageDescriptionPropertyID_ProductionDisplayWidth = FOUR_CHAR_CODE('pdsw'), /* SInt32, Read */

  /*
   * Synonym for
   * kICMImageDescriptionPropertyID_ProductionApertureDisplayHeight.
   */
  kICMImageDescriptionPropertyID_ProductionDisplayHeight = FOUR_CHAR_CODE('pdsh'), /* SInt32, Read */

  /*
   * Color information, if available in the
   * NCLCColorInfoImageDescriptionExtension format.
   */
  kICMImageDescriptionPropertyID_NCLCColorInfo = FOUR_CHAR_CODE('nclc'), /* Native-endian NCLCColorInfoImageDescriptionExtension, Read/Write */

  /*
   * A CGColorSpaceRef for the colorspace described by the image
   * description, constructed from video color info or ICC Profile.
   * IMPORTANT NOTE: The YCbCr matrix from the video color info is not
   * represented in the CGColorSpaceRef. The caller of GetProperty is
   * responsible for releasing this, eg, by calling
   * CGColorSpaceRelease. Only supported on Mac OS X.
   */
  kICMImageDescriptionPropertyID_CGColorSpace = FOUR_CHAR_CODE('cgcs'), /* CGColorSpaceRef, Read -- caller of GetProperty must call CGColorSpaceRelease */

  /*
   * A CFDataRef containing the serialized ICC profile described by the
   * image description. The caller of GetProperty is responsible for
   * releasing this, eg, by calling CFRelease.
   */
  kICMImageDescriptionPropertyID_ICCProfile = FOUR_CHAR_CODE('iccp'), /* CFDataRef, Read/Write -- caller of GetProperty must call CFRelease */

  /*
   * The gamma level described by the image description.
   */
  kICMImageDescriptionPropertyID_GammaLevel = FOUR_CHAR_CODE('gama'), /* Fixed, Read/Write */

  /*
   * Information about the number and order of fields, if available.
   */
  kICMImageDescriptionPropertyID_FieldInfo = FOUR_CHAR_CODE('fiel'), /* FieldInfoImageDescriptionExtension2, Read/Write */

  /*
   * The offset in bytes from the start of one row to the next. Only
   * valid if the codec type is a chunky pixel format.
   */
  kICMImageDescriptionPropertyID_RowBytes = FOUR_CHAR_CODE('rowb'), /* SInt32, Read/Write */

  /*
   * A track width suitable for passing to NewMovieTrack when creating
   * a new track to hold this image data.
   */
  kICMImageDescriptionPropertyID_ClassicTrackWidth = FOUR_CHAR_CODE('claw'), /* Fixed, Read */

  /*
   * A track height suitable for passing to NewMovieTrack when creating
   * a new track to hold this image data.
   */
  kICMImageDescriptionPropertyID_ClassicTrackHeight = FOUR_CHAR_CODE('clah'), /* Fixed, Read */

  /*
   * Defines a duration for quantizing time. This is applicable for
   * cases where a single media sample generates visual output that
   * varies continuously through its duration. By interpreting this
   * property, such a sample may be considered to have internal "step
   * points" at multiples of the stepping duration. This can be used to
   * throttle frame generation during playback, and when stepping using
   * InterestingTime APIs. Setting a step duration with value zero
   * removes any current step duration.
   */
  kICMImageDescriptionPropertyID_StepDuration = FOUR_CHAR_CODE('step'), /* TimeRecord (base ignored), Read/Write */

  /*
   * The clean aperture as a FixedRect in source coordinates, within
   * the rectangle defined by the image description width and height,
   * suitable for use as a source rectangle in a decompression
   * sequence. 
   * For historical reasons, the DVCPROHD codecs store the production
   * aperture display dimensions in the image description width and
   * height; the actual encoded dimensions are smaller. For DVCPROHD,
   * the clip rect will be relative to the image description width and
   * height, not the encoded dimensions.
   */
  kICMImageDescriptionPropertyID_CleanApertureClipRect = FOUR_CHAR_CODE('cacr'), /* FixedRect, Read */

  /*
   * A matrix transforming the clean aperture clip rect to the origin,
   * scaled to the clean aperture display dimensions. 
   * For historical reasons, the DVCPROHD codecs store the production
   * aperture display dimensions in the image description width and
   * height; the actual encoded dimensions are smaller. For DVCPROHD,
   * the matrix will be relative to the image description width and
   * height, not the encoded dimensions.
   */
  kICMImageDescriptionPropertyID_CleanApertureMatrix = FOUR_CHAR_CODE('camx'), /* MatrixRecord, Read */

  /*
   * A matrix transforming the image to the origin, scaled to the
   * production aperture display dimensions. 
   * For historical reasons, the DVCPROHD codecs store the production
   * aperture display dimensions in the image description width and
   * height; the actual encoded dimensions are smaller. For DVCPROHD,
   * the matrix will be relative to the image description width and
   * height, not the encoded dimensions.
   */
  kICMImageDescriptionPropertyID_ProductionApertureMatrix = FOUR_CHAR_CODE('pamx'), /* MatrixRecord, Read */

  /*
   * A localized, human readable string summarizing the image as a
   * CFString, ie: "Apple DV, 720 x 480 (640 x 480), Millions". 
   *  The elements are: the codec name, the encoded pixels dimensions,
   * then parenthetically the clean aperture mode dimensions, but only
   * if they are different from the encoded pixels dimensions; then the
   * depth. 
   * The codec name shall be from the localized decompressor component
   * name string if exactly one decompressor with the correct cType is
   * available; otherwise the string in the image description shall be
   * used. The caller of GetProperty is responsible for releasing this
   * CFString, eg, by calling CFRelease.
   */
  kICMImageDescriptionPropertyID_SummaryString = FOUR_CHAR_CODE('isum') /* CFStringRef, Read - caller of GetProperty must call CFRelease*/
};

/*
 *  ICMImageDescriptionGetPropertyInfo()
 *  
 *  Summary:
 *    Gets info about a particular property of a ImageDescription.
 *  
 *  Parameters:
 *    
 *    inDesc:
 *      ImageDescriptionHandle being interrogated
 *    
 *    inPropClass:
 *      The class of property being requested
 *    
 *    inPropID:
 *      The ID of the property being requested
 *    
 *    outPropType:
 *      The type of property is returned here (can be NULL)
 *    
 *    outPropValueSize:
 *      The size of property is returned here (can be NULL)
 *    
 *    outPropertyFlags:
 *      The property flags are returned here (can be NULL)
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMImageDescriptionGetPropertyInfo(
  ImageDescriptionHandle   inDesc,
  ComponentPropertyClass   inPropClass,
  ComponentPropertyID      inPropID,
  ComponentValueType *     outPropType,            /* can be NULL */
  ByteCount *              outPropValueSize,       /* can be NULL */
  UInt32 *                 outPropertyFlags);      /* can be NULL */


/*
 *  ICMImageDescriptionGetProperty()
 *  
 *  Summary:
 *    Gets a particular property of a ImageDescriptionHandle.
 *  
 *  Parameters:
 *    
 *    inDesc:
 *      ImageDescriptionHandle being interrogated
 *    
 *    inPropClass:
 *      The class of property being requested
 *    
 *    inPropID:
 *      The ID of the property being requested
 *    
 *    inPropValueSize:
 *      The size of the property value buffer
 *    
 *    outPropValueAddress:
 *      Points to the buffer to receive the property value
 *    
 *    outPropValueSizeUsed:
 *      Points to a variable to receive the actual size of returned
 *      property value (can be NULL)
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMImageDescriptionGetProperty(
  ImageDescriptionHandle   inDesc,
  ComponentPropertyClass   inPropClass,
  ComponentPropertyID      inPropID,
  ByteCount                inPropValueSize,
  ComponentValuePtr        outPropValueAddress,
  ByteCount *              outPropValueSizeUsed);      /* can be NULL */


/*
 *  ICMImageDescriptionSetProperty()
 *  
 *  Summary:
 *    Sets a particular property of a ImageDescriptionHandle.
 *  
 *  Parameters:
 *    
 *    inDesc:
 *      ImageDescriptionHandle being modified
 *    
 *    inPropClass:
 *      The class of property being set
 *    
 *    inPropID:
 *      The ID of the property being set
 *    
 *    inPropValueSize:
 *      The size of property value
 *    
 *    inPropValueAddress:
 *      Points to the property value buffer
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMImageDescriptionSetProperty(
  ImageDescriptionHandle   inDesc,
  ComponentPropertyClass   inPropClass,
  ComponentPropertyID      inPropID,
  ByteCount                inPropValueSize,
  ConstComponentValuePtr   inPropValueAddress);




/*
 *  ICMValidTimeFlags
 *  
 *  Summary:
 *    Flags to describe which time values are valid.
 */
typedef UInt32 ICMValidTimeFlags;
enum {

  /*
   * Indicates that a display time stamp is valid.
   */
  kICMValidTime_DisplayTimeStampIsValid = 1L << 0,

  /*
   * Indicates that a display duration is valid.
   */
  kICMValidTime_DisplayDurationIsValid = 1L << 1,

  /*
   * Indicates that a decode time stamp is valid.
   */
  kICMValidTime_DecodeTimeStampIsValid = 1L << 2,

  /*
   * Indicates that a decode duration is valid.
   */
  kICMValidTime_DecodeDurationIsValid = 1L << 3,

  /*
   * Indicates that a display offset (the offset from a decode time
   * stamp to a display time stamp) is valid.
   */
  kICMValidTime_DisplayOffsetIsValid = 1L << 4
};


/*
 *  ICMDecompressionSessionRef
 *  
 *  Summary:
 *    Represents a decompression session that emits CVPixelBufferRefs
 *    containing frames, tagged with display times.
 *  
 *  Discussion:
 *    ICMDecompressionSession does not support source extraction,
 *    matrix transformations, graphics transfer modes, region-based
 *    clipping or data-loading procedures. Note that the pixel buffers
 *    may be output out of display order, provided that the client opts
 *    in.
 */
typedef struct OpaqueICMDecompressionSession*  ICMDecompressionSessionRef;

/*
 *  QTVisualContextRef
 *  
 *  Summary:
 *    Represents a destination visual rendering environment.
 *  
 *  Discussion:
 *    The QTVisualContextRef type encapsulates a connection to a
 *    generic visual destination. A single visual context object may
 *    not be associated with more than one movie at a time.
 */
typedef struct OpaqueQTVisualContext*   QTVisualContextRef;

/*
 *  ICMDecompressionSessionOptionsRef
 *  
 *  Summary:
 *    Holds options for a decompression session.
 */
typedef struct OpaqueICMDecompressionSessionOptions*  ICMDecompressionSessionOptionsRef;

/*
 *  ICMDecompressionFrameOptionsRef
 *  
 *  Summary:
 *    Holds options for decompressing an individual frame.
 */
typedef struct OpaqueICMDecompressionFrameOptions*  ICMDecompressionFrameOptionsRef;

/*
 *  ICMDecompressionTrackingFlags
 *  
 *  Summary:
 *    Describes changes in state of a frame queued with an ICM
 *    decompression session.
 */
typedef UInt32 ICMDecompressionTrackingFlags;
enum {

  /*
   * Indicates that this is the last call for this sourceFrameRefCon.
   */
  kICMDecompressionTracking_LastCall = 1L << 0,

  /*
   * Indicates that the session no longer needs the source data pointer.
   */
  kICMDecompressionTracking_ReleaseSourceData = 1L << 1,

  /*
   * Indicates that a frame is being emitted. The pixelBuffer parameter
   * contains the decompressed frame. If the decompression session is
   * targetting a visual context, the frame has not yet been sent to
   * the visual context but will be after the callback returns.
   */
  kICMDecompressionTracking_EmittingFrame = 1L << 2,

  /*
   * Indicates that this frame was decoded.
   */
  kICMDecompressionTracking_FrameDecoded = 1L << 3,

  /*
   * Indicates that the codec decided to drop this frame.
   */
  kICMDecompressionTracking_FrameDropped = 1L << 4,

  /*
   * Indicates that this frame will not be able to be displayed unless
   * it is queued for redecode (also known as FrameNotDisplayable).
   */
  kICMDecompressionTracking_FrameNeedsRequeueing = 1L << 5
};


/*
 *  ICMDecompressionTrackingCallback
 *  
 *  Summary:
 *    The callback through which a client of an ICM decompression
 *    session receives decoded frames and information about decoding.
 *  
 *  Discussion:
 *    The client may retain the emitted pixel buffers as long as it
 *    needs; they will not be reused before the client releases them.
 *  
 *  Parameters:
 *    
 *    decompressionTrackingRefCon:
 *      The callback's reference value, copied from the
 *      decompressionTrackingRefCon field of the
 *      ICMDecompressionTrackingCallbackRecord structure.
 *    
 *    result:
 *      Indicates whether there was an error in decompression.
 *    
 *    decompressionTrackingFlags:
 *      One or more flags describing the a frame's state transitions.
 *    
 *    pixelBuffer:
 *      When the kICMDecompressionTracking_EmittingFrame flag is set in
 *      decompressionTrackingFlags, a pixel buffer containing the
 *      decompressed frame.  Otherwise, NULL.
 *    
 *    displayTime:
 *      If kICMValidTime_DisplayTimeStampIsValid is set in
 *      validTimeFlags, the display time of the frame.
 *    
 *    displayDuration:
 *      If kICMValidTime_DisplayDurationIsValid is set in
 *      validTimeFlags, the display duration of the frame.
 *    
 *    validTimeFlags:
 *      Indicates which of displayTime and displayDuration is valid.
 *    
 *    reserved:
 *      Reserved for future use.  Ignore the value of this parameter.
 *    
 *    sourceFrameRefCon:
 *      The frame's reference value, copied from the sourceFrameRefCon
 *      parameter to ICMDecompressionSessionDecodeFrame.
 */
typedef CALLBACK_API_C( void , ICMDecompressionTrackingCallback )(void *decompressionTrackingRefCon, OSStatus result, ICMDecompressionTrackingFlags decompressionTrackingFlags, CVPixelBufferRef pixelBuffer, TimeValue64 displayTime, TimeValue64 displayDuration, ICMValidTimeFlags validTimeFlags, void *reserved, void *sourceFrameRefCon);

/*
 *  ICMDecompressionTrackingCallbackRecord
 *  
 *  Summary:
 *    A tracking callback for an ICM decompression session.
 */
struct ICMDecompressionTrackingCallbackRecord {

  /*
   * The callback function pointer.
   */
  ICMDecompressionTrackingCallback  decompressionTrackingCallback;

  /*
   * The callback's reference value.
   */
  void *              decompressionTrackingRefCon;
};
typedef struct ICMDecompressionTrackingCallbackRecord ICMDecompressionTrackingCallbackRecord;
/*
 *  ICMDecompressionSessionCreate()
 *  
 *  Summary:
 *    Creates a session for decompressing video frames.
 *  
 *  Discussion:
 *    Frames will be output through calls to trackingCallback.
 *  
 *  Parameters:
 *    
 *    allocator:
 *      An allocator for the session.  Pass NULL to use the default
 *      allocator.
 *    
 *    desc:
 *      An image description for the source frames.
 *    
 *    decompressionOptions:
 *      Options for the session. The session will retain this options
 *      object. You may change some options during the session by
 *      modifying the object.
 *    
 *    destinationPixelBufferAttributes:
 *      Describes requirements for emitted pixel buffers.
 *    
 *    trackingCallback:
 *      The callback to be called with information about queued frames,
 *      and pixel buffers containing the decompressed frames.
 *    
 *    decompressionSessionOut:
 *      Points to a variable to receive the new decompression session.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionCreate(
  CFAllocatorRef                            allocator,                              /* can be NULL */
  ImageDescriptionHandle                    desc,
  ICMDecompressionSessionOptionsRef         decompressionOptions,                   /* can be NULL */
  CFDictionaryRef                           destinationPixelBufferAttributes,       /* can be NULL */
  ICMDecompressionTrackingCallbackRecord *  trackingCallback,
  ICMDecompressionSessionRef *              decompressionSessionOut);


/*
 *  ICMDecompressionSessionCreateForVisualContext()
 *  
 *  Summary:
 *    Creates a session for decompressing video frames.
 *  
 *  Discussion:
 *    Frames will be output to a visual context.  If desired, the
 *    trackingCallback may attach additional data to pixel buffers
 *    before they are sent to the visual context.
 *  
 *  Parameters:
 *    
 *    allocator:
 *      An allocator for the session.  Pass NULL to use the default
 *      allocator.
 *    
 *    desc:
 *      An image description for the source frames.
 *    
 *    decompressionOptions:
 *      Options for the session. The session will retain this options
 *      object. You may change some options during the session by
 *      modifying the object.
 *    
 *    visualContext:
 *      The target visual context.
 *    
 *    trackingCallback:
 *      The callback to be called with information about queued frames,
 *      and pixel buffers containing the decompressed frames.
 *    
 *    decompressionSessionOut:
 *      Points to a variable to receive the new decompression session.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionCreateForVisualContext(
  CFAllocatorRef                            allocator,                     /* can be NULL */
  ImageDescriptionHandle                    desc,
  ICMDecompressionSessionOptionsRef         decompressionOptions,          /* can be NULL */
  QTVisualContextRef                        visualContext,
  ICMDecompressionTrackingCallbackRecord *  trackingCallback,
  ICMDecompressionSessionRef *              decompressionSessionOut);


/*
 *  ICMDecompressionSessionRetain()
 *  
 *  Summary:
 *    Increments the retain count of a decompression session.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMDecompressionSessionRef )
ICMDecompressionSessionRetain(ICMDecompressionSessionRef session);


/*
 *  ICMDecompressionSessionRelease()
 *  
 *  Summary:
 *    Decrements the retain count of a decompression session.  If it
 *    drops to zero, the session is disposed.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( void )
ICMDecompressionSessionRelease(ICMDecompressionSessionRef session);


/*
 *  ICMDecompressionSessionGetTypeID()
 *  
 *  Summary:
 *    Returns the CFTypeID for decompression sessions.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( CFTypeID )
ICMDecompressionSessionGetTypeID(void);


/*
 *  ICMDecompressionSessionDecodeFrame()
 *  
 *  Summary:
 *    Queues a frame for decompression.
 *  
 *  Parameters:
 *    
 *    session:
 *      The decompression session.
 *    
 *    data:
 *      Points to the compressed data for this frame. The data must
 *      remain in this location until the tracking callback is called
 *      with the kICMDecompressionTracking_ReleaseSourceData flag set
 *      in decompressionTrackingFlags.
 *    
 *    dataSize:
 *      The number of bytes of compressed data. You may not pass zero
 *      in this parameter.
 *    
 *    frameOptions:
 *      Options for this frame.
 *    
 *    frameTime:
 *      Points to a structure describing the frame's timing information.
 *    
 *    sourceFrameRefCon:
 *      Your reference value for the frame.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionDecodeFrame(
  ICMDecompressionSessionRef        session,
  const UInt8 *                     data,
  ByteCount                         dataSize,
  ICMDecompressionFrameOptionsRef   frameOptions,            /* can be NULL */
  const ICMFrameTimeRecord *        frameTime,
  void *                            sourceFrameRefCon);


/*
 *  ICMDecompressionSessionGetPropertyInfo()
 *  
 *  Summary:
 *    Retrieves information about properties of a decompression session.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionGetPropertyInfo(
  ICMDecompressionSessionRef   session,
  ComponentPropertyClass       inPropClass,
  ComponentPropertyID          inPropID,
  ComponentValueType *         outPropType,            /* can be NULL */
  ByteCount *                  outPropValueSize,       /* can be NULL */
  UInt32 *                     outPropertyFlags);      /* can be NULL */


/*
 *  ICMDecompressionSessionGetProperty()
 *  
 *  Summary:
 *    Retrieves the value of a specific property of a decompression
 *    session.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionGetProperty(
  ICMDecompressionSessionRef   session,
  ComponentPropertyClass       inPropClass,
  ComponentPropertyID          inPropID,
  ByteCount                    inPropValueSize,
  ComponentValuePtr            outPropValueAddress,
  ByteCount *                  outPropValueSizeUsed);      /* can be NULL */


/*
 *  ICMDecompressionSessionSetProperty()
 *  
 *  Summary:
 *    Sets the value of a specific property of a decompression session.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionSetProperty(
  ICMDecompressionSessionRef   session,
  ComponentPropertyClass       inPropClass,
  ComponentPropertyID          inPropID,
  ByteCount                    inPropValueSize,
  ConstComponentValuePtr       inPropValueAddress);



/*
 *  ICMNonScheduledDisplayTime
 *  
 *  Discussion:
 *    The display time for a decompression session.
 */
struct ICMNonScheduledDisplayTime {

  /*
   * A display time.  Usually this is the display time of a
   * non-scheduled queued frame.
   */
  TimeValue64         displayTime;

  /*
   * The timescale according to which displayTime should be interpreted.
   */
  TimeScale           displayTimeScale;

  /*
   * Reserved, set to zero.
   */
  UInt32              flags;
};
typedef struct ICMNonScheduledDisplayTime ICMNonScheduledDisplayTime;

/*
 *  Summary:
 *    Properties of decompression sessions.
 */
enum {

  /*
   * Class identifier for decompression session properties.
   */
  kQTPropertyClass_ICMDecompressionSession = FOUR_CHAR_CODE('icds'),

  /*
   * The non-scheduled display time for a decompression session.
   * Setting this requests display of the non-scheduled queued frame at
   * that display time, if there is one. 
   * See ICMDecompressionSessionSetNonScheduledDisplayTime.
   */
  kICMDecompressionSessionPropertyID_NonScheduledDisplayTime = FOUR_CHAR_CODE('nsti'), /* ICMNonScheduledDisplayTime, Read/Write */

  /*
   * The direction for non-scheduled display time. 
   * See ICMDecompressionSessionSetNonScheduledDisplayDirection.
   */
  kICMDecompressionSessionPropertyID_NonScheduledDisplayDirection = FOUR_CHAR_CODE('nsdu'), /* Fixed, Read/Write */

  /*
   * The pixel buffer pool from which emitted pixel buffers are
   * allocated. Getting this does not change the retain count of the
   * pool.
   */
  kICMDecompressionSessionPropertyID_PixelBufferPool = FOUR_CHAR_CODE('pool'), /* CVPixelBufferPoolRef, Read */

  /*
   * Indicates whether the a common pixel buffer pool is shared between
   * the decompressor and the session client. This is false if separate
   * pools are used because the decompressor's and the client's pixel
   * buffer attributes were incompatible.
   */
  kICMDecompressionSessionPropertyID_PixelBufferPoolIsShared = FOUR_CHAR_CODE('plsh') /* Boolean, Read */
};

/*
 *  ICMDecompressionSessionSetNonScheduledDisplayTime()
 *  
 *  Summary:
 *    Sets the display time for a decompression session, and requests
 *    display of the non-scheduled queued frame at that display time,
 *    if there is one.
 *  
 *  Discussion:
 *    Call ICMDecompressionSessionSetNonScheduledDisplayTime after
 *    queueing non-scheduled frames with
 *    ICMDecompressionSessionDecodeFrame with the
 *    icmFrameTimeIsNonScheduledDisplayTime flag set to request display
 *    of the frame at a particular display time. 
 *    If there is no queued non-scheduled frame with this display time,
 *    the frame with the next earlier display time is displayed. (Which
 *    of two display times is earlier is determined using the
 *    non-scheduled display time direction, which you can set with
 *    ICMDecompressionSessionSetNonScheduledDisplayDirection.) If there
 *    is no such frame, nothing happens. 
 *    This has no effect if frames are scheduled against a timebase.
 *  
 *  Parameters:
 *    
 *    session:
 *      The decompression session.
 *    
 *    displayTime:
 *      A display time.  Usually this is the display time of a
 *      non-scheduled queued frame.
 *    
 *    displayTimeScale:
 *      The timescale according to which displayTime should be
 *      interpreted.
 *    
 *    flags:
 *      Reserved, set to zero.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionSetNonScheduledDisplayTime(
  ICMDecompressionSessionRef   session,
  TimeValue64                  displayTime,
  TimeScale                    displayTimeScale,
  UInt32                       flags);


/*
 *  ICMDecompressionSessionSetNonScheduledDisplayDirection()
 *  
 *  Summary:
 *    Sets the direction for non-scheduled display time.
 *  
 *  Discussion:
 *    If rate is zero or positive, direction is forwards.  If negative,
 *    direction is backwards. 
 *    Any frames queued under the opposite direction will be flushed.
 *    
 *    The non-scheduled display direction defaults to forwards (rate
 *    1.0).
 *  
 *  Parameters:
 *    
 *    session:
 *      The decompression session.
 *    
 *    rate:
 *      Indicates the direction.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionSetNonScheduledDisplayDirection(
  ICMDecompressionSessionRef   session,
  Fixed                        rate);


/*
 *  ICMDecompressionSessionFlush()
 *  
 *  Summary:
 *    Flushes frames queued with a decompression session.
 *  
 *  Discussion:
 *    The tracking callback will be called for each frame with the
 *    result -1.
 *  
 *  Parameters:
 *    
 *    session:
 *      The decompression session.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionFlush(ICMDecompressionSessionRef session);




/*
 *  ICMDecompressionSessionOptionsCreate()
 *  
 *  Summary:
 *    Creates a decompression session options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionOptionsCreate(
  CFAllocatorRef                       allocator,       /* can be NULL */
  ICMDecompressionSessionOptionsRef *  options);


/*
 *  ICMDecompressionSessionOptionsCreateCopy()
 *  
 *  Summary:
 *    Copies a decompression session options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionOptionsCreateCopy(
  CFAllocatorRef                       allocator,             /* can be NULL */
  ICMDecompressionSessionOptionsRef    originalOptions,
  ICMDecompressionSessionOptionsRef *  copiedOptions);


/*
 *  ICMDecompressionSessionOptionsRetain()
 *  
 *  Summary:
 *    Increments the retain count of a decompression session options
 *    object.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMDecompressionSessionOptionsRef )
ICMDecompressionSessionOptionsRetain(ICMDecompressionSessionOptionsRef options);


/*
 *  ICMDecompressionSessionOptionsRelease()
 *  
 *  Summary:
 *    Decrements the retain count of a decompression session options
 *    object.  If it drops to zero, the object is disposed.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( void )
ICMDecompressionSessionOptionsRelease(ICMDecompressionSessionOptionsRef options);


/*
 *  ICMDecompressionSessionOptionsGetTypeID()
 *  
 *  Summary:
 *    Returns the CFTypeID for decompression session options objects.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( CFTypeID )
ICMDecompressionSessionOptionsGetTypeID(void);


/*
 *  ICMDecompressionSessionOptionsGetPropertyInfo()
 *  
 *  Summary:
 *    Retrieves information about properties of a decompression session
 *    options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionOptionsGetPropertyInfo(
  ICMDecompressionSessionOptionsRef   options,
  ComponentPropertyClass              inPropClass,
  ComponentPropertyID                 inPropID,
  ComponentValueType *                outPropType,            /* can be NULL */
  ByteCount *                         outPropValueSize,       /* can be NULL */
  UInt32 *                            outPropertyFlags);      /* can be NULL */


/*
 *  ICMDecompressionSessionOptionsGetProperty()
 *  
 *  Summary:
 *    Retrieves the value of a specific property of a decompression
 *    session options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionOptionsGetProperty(
  ICMDecompressionSessionOptionsRef   options,
  ComponentPropertyClass              inPropClass,
  ComponentPropertyID                 inPropID,
  ByteCount                           inPropValueSize,
  ComponentValuePtr                   outPropValueAddress,
  ByteCount *                         outPropValueSizeUsed);      /* can be NULL */


/*
 *  ICMDecompressionSessionOptionsSetProperty()
 *  
 *  Summary:
 *    Sets the value of a specific property of a decompression session
 *    options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionSessionOptionsSetProperty(
  ICMDecompressionSessionOptionsRef   options,
  ComponentPropertyClass              inPropClass,
  ComponentPropertyID                 inPropID,
  ByteCount                           inPropValueSize,
  ConstComponentValuePtr              inPropValueAddress);



/*
 *  Summary:
 *    Properties of decompression session options objects.
 */
enum {

  /*
   * Class identifier for decompression session options object
   * properties.
   */
  kQTPropertyClass_ICMDecompressionSessionOptions = FOUR_CHAR_CODE('idso'),

  /*
   * By default, this is true, meaning that frames must be output in
   * display order. Set this to false to allow frames to be output in
   * decode order rather than in display order.
   */
  kICMDecompressionSessionOptionsPropertyID_DisplayOrderRequired = FOUR_CHAR_CODE('dorq'), /* Boolean, Read/Write */

  /*
   * A specific decompressor component or component instance to be
   * used, or one of the wildcards anyCodec, bestSpeedCodec,
   * bestFidelityCodec, or bestCompressionCodec. 
   * By default, this is anyCodec.
   */
  kICMDecompressionSessionOptionsPropertyID_DecompressorComponent = FOUR_CHAR_CODE('imdc'), /* DecompressorComponent, Read/Write */

  /*
   * The decompression accuracy. 
   * The default accuracy is codecNormalQuality.
   */
  kICMDecompressionSessionOptionsPropertyID_Accuracy = FOUR_CHAR_CODE('acur'), /* CodecQ, Read/Write */

  /*
   * Requests special handling of fields. Not all codecs will obey this
   * request; some codecs will only handle it at certain accuracy
   * levels. Ignored for non-interlaced content.
   */
  kICMDecompressionSessionOptionsPropertyID_FieldMode = FOUR_CHAR_CODE('fiel'), /* ICMFieldMode, Read/Write */

  /*
   * The maximum number of buffers ahead of the current time that
   * should be decompressed. Used in sessions that target visual
   * contexts. By default, the number of buffers will be determined
   * from the visual context.
   */
  kICMDecompressionSessionOptionsPropertyID_MaxBufferCount = FOUR_CHAR_CODE('m#bf'), /* UInt32, Read/Write */

  /*
   * The minimum time ahead of the current time that frames should be
   * decompressed. Used in sessions that target visual contexts. By
   * default, the output-ahead time will be determined from the visual
   * context.
   */
  kICMDecompressionSessionOptionsPropertyID_OutputAheadTime = FOUR_CHAR_CODE('futu'), /* TimeRecord, Read/Write */

  /*
   * You can set the aperture mode property on a decompression session
   * options object to indicate whether pixel buffers should be tagged
   * to enable aspect ratio and clean aperture correction. The default
   * aperture mode for a decompression session is clean aperture mode.
   */
  kICMDecompressionSessionOptionsPropertyID_ApertureMode = FOUR_CHAR_CODE('apmd') /* OSType, Read/Write */
};


/*
 *  ICMFieldMode
 *  
 *  Summary:
 *    Describes special field handling.
 */
typedef UInt32 ICMFieldMode;
enum {

  /*
   * Both fields should be decompressed.
   */
  kICMFieldMode_BothFields      = 0,

  /*
   * Only the top field should be decompressed, producing a half-height
   * image.
   */
  kICMFieldMode_TopFieldOnly    = 1,

  /*
   * Only the bottom field should be decompressed, producing a
   * half-height image.
   */
  kICMFieldMode_BottomFieldOnly = 2,

  /*
   * Both fields should be decompressed, and then filtered to reduce
   * interlacing artifacts.
   */
  kICMFieldMode_DeinterlaceFields = 3
};


/*
 *  ICMDecompressionFrameOptionsCreate()
 *  
 *  Summary:
 *    Creates a frame decompression options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionFrameOptionsCreate(
  CFAllocatorRef                     allocator,       /* can be NULL */
  ICMDecompressionFrameOptionsRef *  options);


/*
 *  ICMDecompressionFrameOptionsCreateCopy()
 *  
 *  Summary:
 *    Copies a frame decompression options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionFrameOptionsCreateCopy(
  CFAllocatorRef                     allocator,             /* can be NULL */
  ICMDecompressionFrameOptionsRef    originalOptions,
  ICMDecompressionFrameOptionsRef *  copiedOptions);


/*
 *  ICMDecompressionFrameOptionsRetain()
 *  
 *  Summary:
 *    Increments the retain count of a frame decompression options
 *    object.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMDecompressionFrameOptionsRef )
ICMDecompressionFrameOptionsRetain(ICMDecompressionFrameOptionsRef options);


/*
 *  ICMDecompressionFrameOptionsRelease()
 *  
 *  Summary:
 *    Decrements the retain count of a frame decompression options
 *    object.  If it drops to zero, the object is disposed.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( void )
ICMDecompressionFrameOptionsRelease(ICMDecompressionFrameOptionsRef options);


/*
 *  ICMDecompressionFrameOptionsGetTypeID()
 *  
 *  Summary:
 *    Returns the CFTypeID for frame decompression options objects.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( CFTypeID )
ICMDecompressionFrameOptionsGetTypeID(void);


/*
 *  ICMDecompressionFrameOptionsGetPropertyInfo()
 *  
 *  Summary:
 *    Retrieves information about properties of a decompression frame
 *    options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionFrameOptionsGetPropertyInfo(
  ICMDecompressionFrameOptionsRef   options,
  ComponentPropertyClass            inPropClass,
  ComponentPropertyID               inPropID,
  ComponentValueType *              outPropType,            /* can be NULL */
  ByteCount *                       outPropValueSize,       /* can be NULL */
  UInt32 *                          outPropertyFlags);      /* can be NULL */


/*
 *  ICMDecompressionFrameOptionsGetProperty()
 *  
 *  Summary:
 *    Retrieves the value of a specific property of a decompression
 *    frame options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionFrameOptionsGetProperty(
  ICMDecompressionFrameOptionsRef   options,
  ComponentPropertyClass            inPropClass,
  ComponentPropertyID               inPropID,
  ByteCount                         inPropValueSize,
  ComponentValuePtr                 outPropValueAddress,
  ByteCount *                       outPropValueSizeUsed);      /* can be NULL */


/*
 *  ICMDecompressionFrameOptionsSetProperty()
 *  
 *  Summary:
 *    Sets the value of a specific property of a decompression frame
 *    options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMDecompressionFrameOptionsSetProperty(
  ICMDecompressionFrameOptionsRef   options,
  ComponentPropertyClass            inPropClass,
  ComponentPropertyID               inPropID,
  ByteCount                         inPropValueSize,
  ConstComponentValuePtr            inPropValueAddress);



/*
 *  Summary:
 *    Properties of decompression frame options objects.
 */
enum {

  /*
   * Class identifier for decompression frame options object properties.
   */
  kQTPropertyClass_ICMDecompressionFrameOptions = FOUR_CHAR_CODE('idfo'),

  /*
   * A specific pixel buffer that the frame should be decompressed
   * into. Setting this circumvents the pixel buffer pool mechanism. If
   * this buffer is not compatible with the codec's pixel buffer
   * requirements, decompression will fail.
   */
  kICMDecompressionFrameOptionsPropertyID_DestinationPixelBuffer = FOUR_CHAR_CODE('cvpb') /* CVPixelBufferRef, Read/Write */
};


/*
 *  ICMCompressionSessionRef
 *  
 *  Summary:
 *    Represents a compression session for a sequence of images. 
 *    B-frame capable.
 */
typedef struct OpaqueICMCompressionSession*  ICMCompressionSessionRef;

/*
 *  ICMEncodedFrameRef
 *  
 *  Summary:
 *    Represents a frame encoded by a compressor component and emitted
 *    by a compression session.
 *  
 *  Discussion:
 *    Compressor components have access to the mutable flavor of
 *    encoded frame object (ICMMutableEncodedFrameRef); compression
 *    session clients receive the read-only flavor
 *    (ICMMutableEncodedFrameRef).
 */
typedef const struct OpaqueICMEncodedFrame*  ICMEncodedFrameRef;
typedef struct OpaqueICMEncodedFrame*   ICMMutableEncodedFrameRef;

/*
 *  ICMCompressionSessionOptionsRef
 *  
 *  Summary:
 *    An opaque struct which holds options to configure a compression
 *    session.
 */
typedef struct OpaqueICMCompressionSessionOptions*  ICMCompressionSessionOptionsRef;

/*
 *  ICMCompressionFrameOptionsRef
 *  
 *  Summary:
 *    A token which holds options to configure an individual frame
 *    during a compression session.
 */
typedef struct OpaqueICMCompressionFrameOptions*  ICMCompressionFrameOptionsRef;

/*
 *  ICMMultiPassStorageRef
 *  
 *  Summary:
 *    A mechanism for storing information for each frame of a multipass
 *    compression session.
 *  
 *  Discussion:
 *    The ICM provides default storage mechanisms using temporary
 *    files, but clients may override this with custom mechanisms.
 */
typedef struct OpaqueICMMultiPassStorage*  ICMMultiPassStorageRef;

/*
 *  ICMEncodedFrameOutputCallback
 *  
 *  Summary:
 *    The callback through which a client of an ICM compression session
 *    receives encoded frames.
 *  
 *  Discussion:
 *    During the encoded frame output callback, the ICM has a reference
 *    to the passed ICMEncodedFrame, but the ICM will release that
 *    reference afterwards; the callback should retain the encoded
 *    frame if the client wants to keep it after the callback returns.
 *  
 *  Parameters:
 *    
 *    encodedFrameOutputRefCon:
 *      The callback's reference value, copied from the
 *      encodedFrameOutputRefCon field of the
 *      ICMEncodedFrameOutputRecord.
 *    
 *    session:
 *      The compression session.
 *    
 *    error:
 *      Indicates whether there was an error.
 *    
 *    frame:
 *      The encoded frame.
 *    
 *    reserved:
 *      Reserved for future use.  Ignore the value of this parameter.
 *  
 *  Result:
 *    If the callback returns an error, the compressor and ICM will
 *    stop emitting frames so that the error can be propogated back to
 *    the caller of ICMCompressionSessionEncodeFrame or
 *    ICMCompressionSessionCompleteFrames.
 */
typedef CALLBACK_API_C( OSStatus , ICMEncodedFrameOutputCallback )(void *encodedFrameOutputRefCon, ICMCompressionSessionRef session, OSStatus error, ICMEncodedFrameRef frame, void *reserved);

/*
 *  ICMEncodedFrameOutputRecord
 *  
 *  Summary:
 *    Holds an encoded frame callback and reference value.
 */
struct ICMEncodedFrameOutputRecord {

  /*
   * An encoded frame callback.
   */
  ICMEncodedFrameOutputCallback  encodedFrameOutputCallback;

  /*
   * The reference value for the encoded frame callback.
   */
  void *              encodedFrameOutputRefCon;

  /*
   * The allocator for encoded frame data. Pass NULL if you do not need
   * a specific allocator.
   */
  CFAllocatorRef      frameDataAllocator;
};
typedef struct ICMEncodedFrameOutputRecord ICMEncodedFrameOutputRecord;

/*
 *  ICMSourceTrackingFlags
 *  
 */
typedef UInt32 ICMSourceTrackingFlags;
enum {

  /*
   * Indicates that this is the last call for this sourceFrameRefCon.
   */
  kICMSourceTracking_LastCall   = 1L << 0,

  /*
   * Indicates that the session is done with the source pixel buffer
   * and has released any reference to it that it had.
   */
  kICMSourceTracking_ReleasedPixelBuffer = 1L << 1,

  /*
   * Indicates that this frame was encoded.
   */
  kICMSourceTracking_FrameWasEncoded = 1L << 2,

  /*
   * Indicates that this frame was dropped.
   */
  kICMSourceTracking_FrameWasDropped = 1L << 3,

  /*
   * Indicates that this frame was merged into other frames.
   */
  kICMSourceTracking_FrameWasMerged = 1L << 4,

  /*
   * Indicates that the time stamp of this frame was modified.
   */
  kICMSourceTracking_FrameTimeWasChanged = 1L << 5,

  /*
   * Indicates that the ICM has copied the image from the source pixel
   * buffer into another pixel buffer because the source pixel buffer
   * was not compatible with the compressor's required pixel buffer
   * attributes.
   */
  kICMSourceTracking_CopiedPixelBuffer = 1L << 6
};


/*
 *  ICMSourceTrackingCallback
 *  
 *  Summary:
 *    A callback which the ICM calls to provide information about the
 *    status of a frame that was passed to
 *    ICMCompressionSessionEncodeFrame.
 *  
 *  Discussion:
 *    Note that this callback may be called several times.
 *  
 *  Parameters:
 *    
 *    sourceTrackingRefCon:
 *      The callback's reference value, copied from the
 *      sourceTrackingRefCon field of ICMSourceTrackingCallbackRecord.
 *    
 *    sourceTrackingFlags:
 *      Flags describing what has happened to the frame.
 *    
 *    sourceFrameRefCon:
 *      The frame's reference value, copied from the sourceFrameRefCon
 *      parameter to ICMCompressionSessionEncodeFrame.
 *    
 *    reserved:
 *      Reserved for future use.  Ignore the value of this parameter.
 */
typedef CALLBACK_API_C( void , ICMSourceTrackingCallback )(void *sourceTrackingRefCon, ICMSourceTrackingFlags sourceTrackingFlags, void *sourceFrameRefCon, void *reserved);

/*
 *  ICMSourceTrackingCallbackRecord
 *  
 *  Summary:
 *    A tracking callback for an ICM compression session.
 */
struct ICMSourceTrackingCallbackRecord {

  /*
   * The callback function pointer.
   */
  ICMSourceTrackingCallback  sourceTrackingCallback;

  /*
   * The callback's reference value.
   */
  void *              sourceTrackingRefCon;
};
typedef struct ICMSourceTrackingCallbackRecord ICMSourceTrackingCallbackRecord;

/*
 *  ICMFrameType
 *  
 *  Summary:
 *    Informally identifies a type of frame.
 *  
 *  Discussion:
 *    Do not assume that there are no other frame types beyond I, P and
 *    B.
 */
typedef UInt16 ICMFrameType;
enum {
  kICMFrameType_I               = 'I',
  kICMFrameType_P               = 'P',
  kICMFrameType_B               = 'B',
  kICMFrameType_Unknown         = 0
};


/* ICMCompressionSessionRef routines */
/*
 *  ICMCompressionSessionCreate()
 *  
 *  Summary:
 *    Creates a compression session for the given codec type.
 *  
 *  Discussion:
 *    Some compressors do not support arbitrary source dimensions, and
 *    may override the suggested width and height.
 *  
 *  Parameters:
 *    
 *    allocator:
 *      An allocator for the session.  Pass NULL to use the default
 *      allocator.
 *    
 *    width:
 *      The width of frames.
 *    
 *    height:
 *      The height of frames.
 *    
 *    cType:
 *      The codec type.
 *    
 *    timescale:
 *      The timescale to be used for all timestamps and durations used
 *      in the session.
 *    
 *    compressionOptions:
 *      Settings configuring the session.
 *    
 *    sourcePixelBufferAttributes:
 *      Required attributes for source pixel buffers, used when
 *      creating a pixel buffer pool for source frames. If you do not
 *      want the ICM to create one for you, pass NULL. (Using pixel
 *      buffers not allocated by the ICM may increase the chance that
 *      it will be necessary to copy image data.)
 *    
 *    encodedFrameOutputRecord:
 *      The callback that will receive encoded frames.
 *    
 *    compressionSessionOut:
 *      Points to a variable to receive the created session object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionCreate(
  CFAllocatorRef                    allocator,                         /* can be NULL */
  int                               width,
  int                               height,
  CodecType                         cType,
  TimeScale                         timescale,
  ICMCompressionSessionOptionsRef   compressionOptions,                /* can be NULL */
  CFDictionaryRef                   sourcePixelBufferAttributes,       /* can be NULL */
  ICMEncodedFrameOutputRecord *     encodedFrameOutputRecord,
  ICMCompressionSessionRef *        compressionSessionOut);


/*
 *  ICMCompressionSessionRetain()
 *  
 *  Summary:
 *    Increments the retain count of a compression session.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMCompressionSessionRef )
ICMCompressionSessionRetain(ICMCompressionSessionRef session);


/*
 *  ICMCompressionSessionRelease()
 *  
 *  Summary:
 *    Decrements the retain count of a compression session.  If it
 *    drops to zero, the session is disposed.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens. Remember to
 *    call ICMCompressionSessionCompleteFrames first if you want to
 *    ensure any pending frames are emitted.  If you do not, they will
 *    be discarded.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( void )
ICMCompressionSessionRelease(ICMCompressionSessionRef session);


/*
 *  ICMCompressionSessionGetTypeID()
 *  
 *  Summary:
 *    Returns the CFTypeID for compression sessions.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( CFTypeID )
ICMCompressionSessionGetTypeID(void);


/*
 *  ICMCompressionSessionGetPropertyInfo()
 *  
 *  Summary:
 *    Retrieves information about properties of a compression session.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionGetPropertyInfo(
  ICMCompressionSessionRef   session,
  ComponentPropertyClass     inPropClass,
  ComponentPropertyID        inPropID,
  ComponentValueType *       outPropType,            /* can be NULL */
  ByteCount *                outPropValueSize,       /* can be NULL */
  UInt32 *                   outPropertyFlags);      /* can be NULL */


/*
 *  ICMCompressionSessionGetProperty()
 *  
 *  Summary:
 *    Retrieves the value of a specific property of a compression
 *    session.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionGetProperty(
  ICMCompressionSessionRef   session,
  ComponentPropertyClass     inPropClass,
  ComponentPropertyID        inPropID,
  ByteCount                  inPropValueSize,
  ComponentValuePtr          outPropValueAddress,
  ByteCount *                outPropValueSizeUsed);      /* can be NULL */


/*
 *  ICMCompressionSessionSetProperty()
 *  
 *  Summary:
 *    Sets the value of a specific property of a compression session.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionSetProperty(
  ICMCompressionSessionRef   session,
  ComponentPropertyClass     inPropClass,
  ComponentPropertyID        inPropID,
  ByteCount                  inPropValueSize,
  ConstComponentValuePtr     inPropValueAddress);



/*
 *  Summary:
 *    Properties of compression sessions.
 */
enum {

  /*
   * Class identifier for compression session properties.
   */
  kQTPropertyClass_ICMCompressionSession = FOUR_CHAR_CODE('icse'),

  /*
   * The time scale for the compression session.
   */
  kICMCompressionSessionPropertyID_TimeScale = FOUR_CHAR_CODE('tscl'), /* TimeScale, Read */

  /*
   * The compressor's pixel buffer attributes for the compression
   * session. You can use these to create a pixel buffer pool for
   * source pixel buffers. Note that this is not the same as the
   * sourcePixelBufferAttributes passed in to
   * ICMCompressionSessionCreate. Getting this property does not change
   * its retain count.
   */
  kICMCompressionSessionPropertyID_CompressorPixelBufferAttributes = FOUR_CHAR_CODE('batt'), /* CFDictionaryRef, Read */

  /*
   * A pool that can provide ideal source pixel buffers for a
   * compression session. The compression session creates this pixel
   * buffer pool based on the compressor's pixel buffer attributes and
   * any pixel buffer attributes passed in to
   * ICMCompressionSessionCreate. If the source pixel buffer attributes
   * and the compressor pixel buffer attributes can not be reconciled,
   * the pool is based on the source pixel buffer attributes and the
   * ICM converts each CVPixelBuffer internally.
   */
  kICMCompressionSessionPropertyID_PixelBufferPool = FOUR_CHAR_CODE('pool'), /* CVPixelBufferPoolRef, Read */

  /*
   * The image description for the compression session. For some
   * codecs, the image description may not be available before the
   * first frame is compressed. Multiple calls to retrieve this
   * property will return the same handle. The ICM will dispose this
   * handle when the compression session is disposed. 
   * IMPORTANT: The caller must NOT dispose this handle.
   */
  kICMCompressionSessionPropertyID_ImageDescription = FOUR_CHAR_CODE('idsc') /* ImageDescriptionHandle, Read */
};

/*
 *  ICMCompressionSessionGetTimeScale()
 *  
 *  Summary:
 *    Retrieves the time scale for the compression session.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( TimeScale )
ICMCompressionSessionGetTimeScale(ICMCompressionSessionRef session);


/*
 *  ICMCompressionSessionGetPixelBufferPool()
 *  
 *  Summary:
 *    Returns a pool that can provide ideal source pixel buffers for a
 *    compression session.
 *  
 *  Discussion:
 *    The compression session creates this pixel buffer pool based on
 *    the compressor's pixel buffer attributes and any pixel buffer
 *    attributes passed in to ICMCompressionSessionCreate. If the
 *    source pixel buffer attributes and the compressor pixel buffer
 *    attributes can not be reconciled, the pool is based on the source
 *    pixel buffer attributes and the ICM converts each CVPixelBuffer
 *    internally.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( CVPixelBufferPoolRef )
ICMCompressionSessionGetPixelBufferPool(ICMCompressionSessionRef session);


/*
 *  ICMCompressionSessionGetImageDescription()
 *  
 *  Summary:
 *    Retrieves the image description for the compression session.
 *  
 *  Discussion:
 *    For some codecs, this may fail if called before the first frame
 *    is compressed. Multiple calls to
 *    ICMCompressionSessionGetImageDescription will return the same
 *    handle. The ICM will dispose this handle when the compression
 *    session is disposed. 
 *    IMPORTANT: The caller must NOT dispose this handle.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionGetImageDescription(
  ICMCompressionSessionRef   session,
  ImageDescriptionHandle *   imageDescOut);


/* ICMCompressionSessionRef encoding routines */
/*
 *  ICMCompressionSessionEncodeFrame()
 *  
 *  Summary:
 *    Call this function to present frames to the compression session.
 *    Encoded frames may or may not be output before the function
 *    returns.
 *  
 *  Discussion:
 *    The session will retain the pixel buffer as long as necessary;
 *    the client should not modify the pixel data until the session
 *    releases it. (The most practical way to deal with this is by
 *    allocating pixel buffers from a pool.) 
 *    The client may fill in both, either or neither of
 *    displayTimeStamp and displayDuration, but should set the
 *    appropriate flags to indicate which is valid. If the client needs
 *    to track the progress of a source frame, it should provide a
 *    sourceTrackingCallback. 
 *    Note: If multipass compression is enabled, calls to
 *    ICMCompressionSessionEncodeFrame must be bracketed by
 *    ICMCompressionSessionBeginPass ... ICMCompressionSessionEndPass.
 *  
 *  Parameters:
 *    
 *    session:
 *      Identifies the compression session.
 *    
 *    pixelBuffer:
 *      Contains the source image to be compressed. PixelBuffer must
 *      have a nonzero reference count. The session will retain it as
 *      long as necessary. The client should not modify pixel buffer's
 *      pixels until the pixel buffer release callback is called. In a
 *      multipass encoding session pass where the compressor suggested
 *      the flag kICMCompressionPassMode_NoSourceFrames, you may pass
 *      NULL for pixelBuffer.
 *    
 *    displayTimeStamp:
 *      The display timestamp of the frame, using the timescale passed
 *      to ICMCompressionSessionCreate. If you pass a valid value, set
 *      the kICMValidTime_DisplayTimeStampIsValid flag in
 *      validTimeFlags.
 *    
 *    displayDuration:
 *      The display duration of the frame, using the timescale passed
 *      to ICMCompressionSessionCreate. If you pass a valid value, set
 *      the kICMValidTime_DisplayDurationIsValid flag in validTimeFlags.
 *    
 *    validTimeFlags:
 *      Flags to indicate which of displayTimeStamp and displayDuration
 *      are valid.
 *    
 *    frameOptions:
 *      Options for this frame.
 *    
 *    sourceTrackingCallback:
 *      A callback to be notified about the status of this source
 *      frame. Pass NULL if you do not require notification.
 *    
 *    sourceFrameRefCon:
 *      Your reference to the source frame.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionEncodeFrame(
  ICMCompressionSessionRef           session,
  CVPixelBufferRef                   pixelBuffer,
  TimeValue64                        displayTimeStamp,
  TimeValue64                        displayDuration,
  ICMValidTimeFlags                  validTimeFlags,
  ICMCompressionFrameOptionsRef      frameOptions,                 /* can be NULL */
  ICMSourceTrackingCallbackRecord *  sourceTrackingCallback,       /* can be NULL */
  void *                             sourceFrameRefCon);



/*
 *  ICMCompressionSessionCompleteFrames()
 *  
 *  Discussion:
 *    Call this function to force the compression session to complete
 *    encoding frames. Set completeAllFrames to direct the session to
 *    complete all pending frames. 
 *    If completeAllFrames is false, only frames with display time
 *    stamps up to and including completeUntilDisplayTimeStamp. 
 *    If ICMCompressionSessionOptionsSetDurationsNeeded is true and you
 *    are passing valid display timestamps but not display durations to
 *    ICMCompressionSessionEncodeFrame, pass the display timestamp of
 *    the next frame that would be passed to EncodeFrame in
 *    nextDisplayTimeStamp. 
 *    Note: This function might return before frames are completed if
 *    the encoded frame callback returns an error.
 *  
 *  Parameters:
 *    
 *    session:
 *      Identifies the compression session.
 *    
 *    completeAllFrames:
 *      Set to direct the session to complete all pending frames.
 *    
 *    completeUntilDisplayTimeStamp:
 *      If completeAllFrames is false, the display timestamp to
 *      complete frames up to. Ignored if completeAllFrames is true.
 *    
 *    nextDisplayTimeStamp:
 *      See above. Ignored unless
 *      ICMCompressionSessionOptionsSetDurationsNeeded set true and
 *      kICMValidTime_DisplayDurationIsValid was clear in
 *      validTimeFlags in last call to ICMCompressionSessionEncodeFrame.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionCompleteFrames(
  ICMCompressionSessionRef   session,
  Boolean                    completeAllFrames,
  TimeValue64                completeUntilDisplayTimeStamp,
  TimeValue64                nextDisplayTimeStamp);



/* ICMCompressionSessionRef multipass support routines */

/*
 *  ICMCompressionPassModeFlags
 *  
 *  Summary:
 *    Flags describing how a compressor should behave in a given pass
 *    of multipass encoding.
 */
typedef UInt32 ICMCompressionPassModeFlags;
enum {

  /*
   * In this pass the compressor shall output encoded frames.
   */
  kICMCompressionPassMode_OutputEncodedFrames = 1L << 0,

  /*
   * In this pass the client need not provide source frame buffers.
   */
  kICMCompressionPassMode_NoSourceFrames = 1L << 1,

  /*
   * In this pass the compressor may write private data to multipass
   * storage.
   */
  kICMCompressionPassMode_WriteToMultiPassStorage = 1L << 2,

  /*
   * In this pass the compressor may read private data from multipass
   * storage.
   */
  kICMCompressionPassMode_ReadFromMultiPassStorage = 1L << 3,

  /*
   * The compressor will set this flag to indicate that it will not be
   * able to output encoded frames in the coming pass. If this flag is
   * not set, then the client is allowed to set the
   * kICMCompressionPassMode_OutputEncodedFrames flag when calling
   * ICMCompressionSessionBeginPass.
   */
  kICMCompressionPassMode_NotReadyToOutputEncodedFrames = 1L << 4
};

/*
 *  ICMCompressionSessionSupportsMultiPassEncoding()
 *  
 *  Summary:
 *    Queries whether a compression session supports multipass encoding.
 *  
 *  Discussion:
 *    Even if this function returns false, if you set the
 *    kICMCompressionSessionOptionsPropertyID_MultiPassStorage property
 *    on the CompressionSessionOptions, you must call
 *    ICMCompressionSessionBeginPass and ICMCompressionSessionEndPass.
 *  
 *  Parameters:
 *    
 *    session:
 *      The compression session.
 *    
 *    multiPassStyleFlags:
 *      Reserved.  Set to zero.
 *    
 *    firstPassModeFlagsOut:
 *      Points to a variable to receive the session's requested mode
 *      flags for the first pass. The client may modify these flags,
 *      but should not set kICMCompressionPassMode_NoSourceFrames. Pass
 *      NULL if you do not want this information.
 *  
 *  Result:
 *    true if the session supports multipass encoding, false otherwise.
 *    If the session does not support multipass encoding,
 *    *firstPassModeFlagsOut will be set to
 *    kICMCompressionPassMode_OutputEncodedFrames.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( Boolean )
ICMCompressionSessionSupportsMultiPassEncoding(
  ICMCompressionSessionRef       session,
  UInt32                         multiPassStyleFlags,
  ICMCompressionPassModeFlags *  firstPassModeFlagsOut);      /* can be NULL */


/*
 *  ICMCompressionSessionBeginPass()
 *  
 *  Summary:
 *    Call to announce the start of a specific compression pass.
 *  
 *  Discussion:
 *    The source frames and frame options for each display timestamp
 *    should be the same across passes. 
 *    During multipass compression, valid displayTimeStamps must be
 *    passed to ICMCompressionSessionEncodeFrame since they are used to
 *    index the compressor's stored state. 
 *    During an analysis pass
 *    (kICMCompressionPassMode_WriteToMultiPassStorage), the compressor
 *    does not output encoded frames, but records compressor-private
 *    information for each frame. 
 *    During repeated analysis passes and the encoding pass
 *    (kICMCompressionPassMode_ReadFromMultiPassStorage), the
 *    compressor may refer to this information for other frames and use
 *    it to improve encoding. 
 *    During an encoding pass
 *    (kICMCompressionPassMode_OutputEncodedFrames), the compressor
 *    must output encoded frames. 
 *    If the compressor set the kICMCompressionPassMode_NoSourceFrames
 *    flag for the pass, the client may pass NULL pixel buffers to
 *    ICMCompressionSessionEncodeFrame. 
 *    By default, the ICM provides local storage that lasts only until
 *    the compression session is disposed. If the client provides
 *    custom multipass storage, pass may be performed at different
 *    times or on different machines; segments of each pass may even be
 *    distributed.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionBeginPass(
  ICMCompressionSessionRef      session,
  ICMCompressionPassModeFlags   passModeFlags,
  UInt32                        flags);


/*
 *  ICMCompressionSessionEndPass()
 *  
 *  Summary:
 *    Call to announce the end of a pass.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionEndPass(ICMCompressionSessionRef session);


/*
 *  ICMCompressionSessionProcessBetweenPasses()
 *  
 *  Summary:
 *    Lets the compressor perform processing between passes.
 *  
 *  Discussion:
 *    Call this function repeatedly, until the compressor sets
 *    *interpassProcessingDoneOut to true to indicate that it is done
 *    with this round of interpass processing. 
 *    When done, the compressor will indicate its preferred mode for
 *    the next pass. 
 *    The client may choose to begin an encoding pass (by ORing in the
 *    kICMCompressionPassMode_OutputEncodedFrames flag) regardless of
 *    the compressor's request.
 *  
 *  Parameters:
 *    
 *    session:
 *      The compression session.
 *    
 *    flags:
 *      Reserved.  Set to zero.
 *    
 *    interpassProcessingDoneOut:
 *      Points to a Boolean that will be set to false if
 *      ICMCompressionSessionProcessBetweenPasses should be called
 *      again, true if not
 *    
 *    requestedNextPassModeFlagsOut:
 *      Points to ICMCompressionPassModeFlags that will be set to the
 *      codec's recommended mode flags for the next pass.
 *      kICMCompressionPassMode_OutputEncodedFrames will only be set if
 *      it recommends that the next pass be the final one.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionProcessBetweenPasses(
  ICMCompressionSessionRef       session,
  UInt32                         flags,
  Boolean *                      interpassProcessingDoneOut,
  ICMCompressionPassModeFlags *  requestedNextPassModeFlagsOut);



/* ICMCompressionSessionOptionsRef routines */
/*
 *  ICMCompressionSessionOptionsCreate()
 *  
 *  Summary:
 *    Creates a compression session options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionOptionsCreate(
  CFAllocatorRef                     allocator,       /* can be NULL */
  ICMCompressionSessionOptionsRef *  options);


/*
 *  ICMCompressionSessionOptionsCreateCopy()
 *  
 *  Summary:
 *    Copies a compression session options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionOptionsCreateCopy(
  CFAllocatorRef                     allocator,             /* can be NULL */
  ICMCompressionSessionOptionsRef    originalOptions,
  ICMCompressionSessionOptionsRef *  copiedOptions);


/*
 *  ICMCompressionSessionOptionsRetain()
 *  
 *  Summary:
 *    Increments the retain count of a compression session options
 *    object.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMCompressionSessionOptionsRef )
ICMCompressionSessionOptionsRetain(ICMCompressionSessionOptionsRef options);


/*
 *  ICMCompressionSessionOptionsRelease()
 *  
 *  Summary:
 *    Decrements the retain count of a compression session options
 *    object.  If it drops to zero, the object is disposed.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( void )
ICMCompressionSessionOptionsRelease(ICMCompressionSessionOptionsRef options);


/*
 *  ICMCompressionSessionOptionsGetTypeID()
 *  
 *  Summary:
 *    Returns the CFTypeID for compression session options objects.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( CFTypeID )
ICMCompressionSessionOptionsGetTypeID(void);


/*
 *  ICMCompressionSessionOptionsGetPropertyInfo()
 *  
 *  Summary:
 *    Retrieves information about properties of a compression session
 *    options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionOptionsGetPropertyInfo(
  ICMCompressionSessionOptionsRef   options,
  ComponentPropertyClass            inPropClass,
  ComponentPropertyID               inPropID,
  ComponentValueType *              outPropType,            /* can be NULL */
  ByteCount *                       outPropValueSize,       /* can be NULL */
  UInt32 *                          outPropertyFlags);      /* can be NULL */


/*
 *  ICMCompressionSessionOptionsGetProperty()
 *  
 *  Summary:
 *    Retrieves the value of a specific property of a compression
 *    session options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionOptionsGetProperty(
  ICMCompressionSessionOptionsRef   options,
  ComponentPropertyClass            inPropClass,
  ComponentPropertyID               inPropID,
  ByteCount                         inPropValueSize,
  ComponentValuePtr                 outPropValueAddress,
  ByteCount *                       outPropValueSizeUsed);      /* can be NULL */


/*
 *  ICMCompressionSessionOptionsSetProperty()
 *  
 *  Summary:
 *    Sets the value of a specific property of a compression session
 *    options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionOptionsSetProperty(
  ICMCompressionSessionOptionsRef   options,
  ComponentPropertyClass            inPropClass,
  ComponentPropertyID               inPropID,
  ByteCount                         inPropValueSize,
  ConstComponentValuePtr            inPropValueAddress);



/*
 */
enum {

  /*
   * Indicates no limit on the number of frames in the compression
   * window.
   */
  kICMUnlimitedFrameDelayCount  = -1L,

  /*
   * Indicates no time limit on the number of frames in the compression
   * window.
   */
  kICMUnlimitedFrameDelayTime   = -1L,

  /*
   * Indicates no CPU time limit on compression.
   */
  kICMUnlimitedCPUTimeBudget    = -1L
};


/*
 *  ICMDataRateLimit
 *  
 *  Summary:
 *    A hard limit on the data rate.
 *  
 *  Discussion:
 *    A hard limit is described by a data size in bytes and a duration
 *    in seconds, and requires that the total size of compressed data
 *    for any continuous segment of that duration (in decode time) must
 *    not exceed the provided data size.
 */
struct ICMDataRateLimit {

  /*
   * The number of bytes.
   */
  SInt32              dataSize;

  /*
   * The number of seconds.
   */
  Float32             dataDuration;
};
typedef struct ICMDataRateLimit         ICMDataRateLimit;

/*
 *  Summary:
 *    Scaling modes
 *  
 *  Discussion:
 *    These constants indicate how source frames to a compression
 *    session should be scaled if the dimensions and/or display aspect
 *    ratio do not match.
 */
enum {

  /*
   * The full width and height of source frames shall be scaled to the
   * full width and height of the destination. This is the default if
   * no other scaling mode is specified.
   */
  kICMScalingMode_StretchProductionAperture = FOUR_CHAR_CODE('sp2p'),

  /*
   * The clean aperture of the source frames shall be scaled to the
   * clean aperture of the destination.
   */
  kICMScalingMode_StretchCleanAperture = FOUR_CHAR_CODE('sc2c'),

  /*
   * The clean aperture of the source frames shall be scaled to fit
   * inside the clean aperture of the destination, preserving the
   * original display aspect ratio. If the display aspect ratios are
   * different, the source frames will be centered with black bars
   * above and below, or to the left and right.
   */
  kICMScalingMode_Letterbox     = FOUR_CHAR_CODE('lett'),

  /*
   * The clean aperture of the source frames shall be scaled to cover
   * the clean aperture of the destination, preserving the original
   * display aspect ratio. If the display aspect ratios are different,
   * the source frames will be centered and cropped.
   */
  kICMScalingMode_Trim          = FOUR_CHAR_CODE('trim')
};


/*
 *  ICMSimpleBoundaryConditions
 *  
 *  Summary:
 *    Indicates whether and how a compression session's frames will be
 *    concatenated with other compressed frames to form a longer series.
 *  
 *  Discussion:
 *    Some clients divide a long series of frames into several shorter
 *    segments, each of which is then compressed by an independent
 *    compression session. Boundary conditions tell the compressor
 *    about how each session fits into the greater series: does this
 *    session stand alone, or will it be used to encode the first
 *    segment, a middle segment, or the last segment in a longer
 *    continuum? 
 *    This information enables compressors to ensure that compressed
 *    segments can be concatenated smoothly -- for example, avoiding
 *    data rate spikes where segments are joined. 
 *    By default, a session is assumed to stand alone.
 */
struct ICMSimpleBoundaryConditions {

  /*
   * True if frames compressed in a separate session will be
   * concatenated before the beginning of this one. False if this is a
   * stand-alone session, or if this session will encode the first
   * segment of a multi-segment compression. By default, false.
   */
  Boolean             moreFramesBeforeStart;

  /*
   * True if frames compressed in a separate session will be
   * concatenated following the end of this one. False if this is a
   * stand-alone session, or if this session will encode the last
   * segment of a multi-segment compression. By default, false.
   */
  Boolean             moreFramesAfterEnd;
};
typedef struct ICMSimpleBoundaryConditions ICMSimpleBoundaryConditions;

/*
 *  Summary:
 *    Properties of compression sessions options objects.
 */
enum {

  /*
   * Class identifier for compression session options object properties.
   */
  kQTPropertyClass_ICMCompressionSessionOptions = FOUR_CHAR_CODE('icso'),

  /*
   * Enables temporal compression. By default, temporal compression is
   * disabled. 
   * IMPORTANT: If you want temporal compression (P frames and/or B
   * frames) you must set this to true.
   */
  kICMCompressionSessionOptionsPropertyID_AllowTemporalCompression = FOUR_CHAR_CODE('p ok'), /* Boolean, Read/Write */

  /*
   * Enables frame reordering. 
   * In order to encode B frames, a compressor must reorder frames,
   * which means that the order in which they will be emitted and
   * stored (the decode order) is different from the order in which
   * they were presented to the compressor (the display order). 
   * By default, frame reordering is disabled. 
   * IMPORTANT: In order to encode using B frames, you must enable
   * frame reordering.
   */
  kICMCompressionSessionOptionsPropertyID_AllowFrameReordering = FOUR_CHAR_CODE('b ok'), /* Boolean, Read/Write */

  /*
   * Indicates that durations of emitted frames are needed. 
   * If this flag is set and source frames are provided with times but
   * not durations, then frames will be delayed so that durations can
   * be calculated as the difference between one frame's time stamp and
   * the next frame's time stamp. 
   * By default, this flag is clear, so frames will not be delayed in
   * order to calculate durations. 
   * IMPORTANT: If you will be passing encoded frames to
   * AddMediaSampleFromEncodedFrame, you must set this flag to true.
   */
  kICMCompressionSessionOptionsPropertyID_DurationsNeeded = FOUR_CHAR_CODE('need'), /* Boolean, Read/Write */

  /*
   * The maximum interval between key frames, also known as the key
   * frame rate. 
   * Key frames, also known as sync frames, reset inter-frame
   * dependencies; decoding a key frame is sufficient to prepare a
   * decompressor for correctly decoding the difference frames that
   * follow. 
   * Compressors are allowed to generate key frames more frequently if
   * this would result in more efficient compression. 
   * The default key frame interval is 0, which indicates that the
   * compressor should choose where to place all key frames. A key
   * frame interval of 1 indicates that every frame must be a key
   * frame, 2 indicates that at least every other frame must be a key
   * frame, etc.
   */
  kICMCompressionSessionOptionsPropertyID_MaxKeyFrameInterval = FOUR_CHAR_CODE('kyfr'), /* SInt32, Read/Write */

  /*
   * The requested maximum interval between partial sync frames. If the
   * interval is n, any sequence of n successive frames must include at
   * least one key or partial sync frame. 
   * Where supported, partial sync frames perform a partial reset of
   * inter-frame dependencies; decoding two partial sync frames and the
   * non-droppable difference frames between them is sufficient to
   * prepare a decompressor for correctly decoding the difference
   * frames that follow. 
   * Compressors are allowed to generate partial sync frames more
   * frequently if this would result in more efficient compression.
   * 
   * The default partial sync frame interval is 0, which indicates that
   * the compressor should choose where to place partial sync frames. A
   * partial sync frame interval of 1 means there can be no difference
   * frames, so it is equivalent to a key frame interval of 1. A
   * partial sync frame interval of 2 means that every other frame must
   * be a key frame or a partial sync frame. 
   * Compressors that do not support partial sync frames will ignore
   * this setting.
   */
  kICMCompressionSessionOptionsPropertyID_MaxPartialSyncFrameInterval = FOUR_CHAR_CODE('psfr'), /* SInt32, Read/Write */

  /*
   * Enables the compressor to modify frame times. 
   * Some compressors are able to identify and coalesce runs of
   * identical frames and output single frames with longer duration, or
   * output frames at a different frame rate from the original. This
   * feature is controlled by the "allow frame time changes" flag. By
   * default, this flag is set to false, which forces compressors to
   * emit one encoded frame for every source frame, and to preserve
   * frame display times. 
   * (Note: this feature replaces the practice of having compressors
   * return special high similarity values to indicate that frames
   * could be dropped.) 
   * If you want to allow the compressor to modify frame times in order
   * to improve compression performance, enable frame time changes.
   */
  kICMCompressionSessionOptionsPropertyID_AllowFrameTimeChanges = FOUR_CHAR_CODE('+ ok'), /* Boolean, Read/Write */

  /*
   * Enables the compressor to call the encoded-frame callback from a
   * different thread. 
   * By default, the flag is false, which means that the compressor
   * must call the encoded-frame callback from the same thread that
   * ICMCompressionSessionEncodeFrame and
   * ICMCompressionSessionCompleteFrames were called on.
   */
  kICMCompressionSessionOptionsPropertyID_AllowAsyncCompletion = FOUR_CHAR_CODE('asok'), /* Boolean, Read/Write */

  /*
   * The maximum frame delay count is the maximum number of frames that
   * a compressor is allowed to hold before it must output a compressed
   * frame. It limits the number of frames that may be held in the
   * "compression window". If the maximum frame delay count is M, then
   * before the call to encode frame N returns, frame N-M must have
   * been emitted. 
   * The default is kICMUnlimitedFrameDelayCount, which sets no limit
   * on the compression window.
   */
  kICMCompressionSessionOptionsPropertyID_MaxFrameDelayCount = FOUR_CHAR_CODE('cwin'), /* SInt32, Read/Write */

  /*
   * The maximum frame delay time is the maximum difference between a
   * source frame's display time and the corresponding encoded frame's
   * decode time. It limits the span of display time that may be held
   * in the "compression window". If the maximum frame delay time is
   * TM, then before the call to encode a frame with display time TN
   * returns, all frames with display times up to and including TN-TM
   * must have been emitted. 
   * The default is kICMUnlimitedFrameDelayTime, which sets no time
   * limit on the compression window.
   */
  kICMCompressionSessionOptionsPropertyID_MaxFrameDelayTime = FOUR_CHAR_CODE('cwit'), /* TimeValue64, Read/Write */

  /*
   * Sets a specific compressor component or component instance to be
   * used, or one of the wildcards anyCodec, bestSpeedCodec,
   * bestFidelityCodec, or bestCompressionCodec. 
   * Use this API to force the Image Compression Manager to use a
   * specific compressor component or compressor component instance. 
   * (If you pass in a component instance that you opened, the ICM will
   * not close that instance; you must do so after the compression
   * session is released.) To allow the Image Compression Manager to
   * choose the compressor component, set the compressorComponent to
   * anyCodec (the default), bestSpeedCodec, bestFidelityCodec or
   * bestCompressionCodec.
   */
  kICMCompressionSessionOptionsPropertyID_CompressorComponent = FOUR_CHAR_CODE('imco'), /* CompressorComponent, Read/Write */

  /*
   * A handle containing compressor settings. The compressor will be
   * configured with these settings (by a call to
   * ImageCodecSetSettings) during ICMCompressionSessionCreate.
   */
  kICMCompressionSessionOptionsPropertyID_CompressorSettings = FOUR_CHAR_CODE('cost'), /* Handle, Read/Write */

  /*
   * The depth for compression. 
   * If a compressor does not support a specific depth, the closest
   * supported depth will be used (preferring deeper depths to
   * shallower depths). The default depth is k24RGBPixelFormat.
   */
  kICMCompressionSessionOptionsPropertyID_Depth = FOUR_CHAR_CODE('deep'), /* UInt32, Read/Write */

  /*
   * The color table for compression.  Used with indexed-color depths.
   * 
   * Clients who get this property are responsible for disposing the
   * returned CTabHandle.
   */
  kICMCompressionSessionOptionsPropertyID_ColorTable = FOUR_CHAR_CODE('clut'), /* CTabHandle, Read/Write*/

  /*
   * The compression quality. 
   * This value is always used to set the spatialQuality; if temporal
   * compression is enabled, it is also used to set temporalQuality.
   * <BR> The default quality is codecNormalQuality.
   */
  kICMCompressionSessionOptionsPropertyID_Quality = FOUR_CHAR_CODE('qual'), /* CodecQ, Read/Write */

  /*
   * The long-term desired average data rate in bytes per second. 
   *  This is not a hard limit. 
   * The default data rate is zero, which indicates that the quality
   * setting should determine the size of compressed data. 
   * Note that data rate settings only have an effect when timing
   * information is provided for source frames, and that some codecs do
   * not support limiting to specified data rates.
   */
  kICMCompressionSessionOptionsPropertyID_AverageDataRate = FOUR_CHAR_CODE('aver'), /* SInt32, Read/Write */

  /*
   * Zero, one or two hard limits on data rate. 
   * Each hard limit is described by a data size in bytes and a
   * duration in seconds, and requires that the total size of
   * compressed data for any contiguous segment of that duration (in
   * decode time) must not exceed the data size. 
   * By default, no data rate limits are set. 
   * When setting this property, the inPropValueSize parameter should
   * be the number of data rate limits multiplied by
   * sizeof(ICMDataRateLimit). 
   * Note that data rate settings only have an effect when timing
   * information is provided for source frames, and that some codecs do
   * not support limiting to specified data rates.
   */
  kICMCompressionSessionOptionsPropertyID_DataRateLimits = FOUR_CHAR_CODE('hard'), /* C array of ICMDataRateLimit struct, Read/Write */

  /*
   * The current number of data rate limits.
   */
  kICMCompressionSessionOptionsPropertyID_DataRateLimitCount = FOUR_CHAR_CODE('har#'), /* UInt32, Read */

  /*
   * The maximum allowed number of data rate limits.  (Currently 2.)
   */
  kICMCompressionSessionOptionsPropertyID_MaxDataRateLimits = FOUR_CHAR_CODE('mhar'), /* UInt32, Read */

  /*
   * Indicates that the source was previously compressed. 
   * This property is purely an optional, informational hint to the
   * compressor; by default it is false.
   */
  kICMCompressionSessionOptionsPropertyID_WasCompressed = FOUR_CHAR_CODE('wasc'), /* Boolean, Read/Write */

  /*
   * Recommends a CPU time budget for the compressor in microseconds
   * per frame. 
   * Zero means to go as fast as possible. 
   * By default, this is set to kICMUnlimitedCPUTimeBudget, which sets
   * no limit. 
   * This is an advisory hint to the compressor, and some compressors
   * may ignore it. Multithreaded compressors may use this amount of
   * CPU time on each processor. 
   * Compressors should not feel compelled to use the full time budget
   * if they complete ahead of time!
   */
  kICMCompressionSessionOptionsPropertyID_CPUTimeBudget = FOUR_CHAR_CODE('cput'), /* UInt32, Read/Write */

  /*
   * Storage for multi-pass compression. 
   * To enable multipass compression, the client must provide a storage
   * location for multipass data.  Use
   * ICMMultiPassStorageCreateWithTemporaryFile to have the ICM store
   * it in a temporary file.  Use
   * ICMMultiPassStorageCreateWithCallbacks to manage the storage
   * yourself. 
   * Note that the amount of multipass data to be stored can be
   * substantial; it could be greater than the size of the output movie
   * file. 
   * If this property is not NULL, the client must call
   * ICMCompressionSessionBeginPass and ICMCompressionSessionEndPass
   * around groups of calls to ICMCompressionSessionEncodeFrame. 
   *  By default, this property is NULL and multipass compression is
   * not enabled. The compression session options object retains the
   * multipass storage object, when one is set.
   */
  kICMCompressionSessionOptionsPropertyID_MultiPassStorage = FOUR_CHAR_CODE('imps'), /* ICMMultiPassStorageRef, Read/Write */

  /*
   * Indicates the number of source frames, if known. If nonzero, this
   * should be the exact number of times that the client calls
   * ICMCompressionSessionEncodeFrame in each pass. 
   * The default is 0, which indicates that the number of source frames
   * is not known.
   */
  kICMCompressionSessionOptionsPropertyID_SourceFrameCount = FOUR_CHAR_CODE('frco'), /* UInt64, Read/Write */

  /*
   * Indicates the expected frame rate, if known. The frame rate is
   * measured in frames per second. This is not used to control the
   * frame rate; it is provided as a hint to the compressor so that it
   * can set up internal configuration before compression begins. The
   * actual frame rate will depend on frame durations and may vary. By
   * default, this is zero, indicating "unknown".
   */
  kICMCompressionSessionOptionsPropertyID_ExpectedFrameRate = FOUR_CHAR_CODE('fran'), /* Fixed, Read/Write */

  /*
   * Indicates how source frames to a compression session should be
   * scaled if the dimensions and/or display aspect ratio do not match.
   */
  kICMCompressionSessionOptionsPropertyID_ScalingMode = FOUR_CHAR_CODE('scam'), /* OSType, Read/Write */

  /*
   * Describes the clean aperture for compressed frames. Note that if
   * the compressor enforces a clean aperture, it will override this
   * setting. The clean aperture will be set on the output image
   * description and may affect scaling in some scaling modes. By
   * default, this is all zeros, meaning unset.
   */
  kICMCompressionSessionOptionsPropertyID_CleanAperture = FOUR_CHAR_CODE('clap'), /* Native-endian CleanApertureImageDescriptionExtension, Read/Write */

  /*
   * Describes the pixel aspect ratio for compressed frames. Note that
   * if the compressor enforces a pixel aspect ratio, it will override
   * this setting. The pixel aspect ratio will be set on the output
   * image description and may affect scaling in some scaling modes. By
   * default, this is all zeros, meaning unset.
   */
  kICMCompressionSessionOptionsPropertyID_PixelAspectRatio = FOUR_CHAR_CODE('pasp'), /* Native-endian PixelAspectRatioImageDescriptionExtension, Read/Write */

  /*
   * Describes the number and order of fields for compressed frames.
   * Note that if the compressor enforces field info, it will override
   * this setting. The field info will be set on the output image
   * description and may affect scaling in some scaling modes. By
   * default, this is all zeros, meaning unset.
   */
  kICMCompressionSessionOptionsPropertyID_FieldInfo = FOUR_CHAR_CODE('fiel'), /* FieldInfoImageDescriptionExtension2, Read/Write */

  /*
   * Indicates whether and how a compression session's frames will be
   * concatenated with other compressed frames to form a longer series.
   */
  kICMCompressionSessionOptionsPropertyID_SimpleBoundaryConditions = FOUR_CHAR_CODE('ends'), /* ICMSimpleBoundaryConditions struct, Read/Write */

  /*
   * Requests additional distortion to be applied to the aspect ratio
   * in the kICMScalingMode_Letterbox and kICMScalingMode_Trim scaling
   * modes. Values greater than fixed1 mean wider, values less than
   * fixed1 mean narrower. For example, a value of X2Fix(2.0) would
   * make the picture aspect ratio twice as wide.
   */
  kICMCompressionSessionOptionsPropertyID_ExtraAspectRatioStretchFactor = FOUR_CHAR_CODE('exsf') /* Fixed, Default fixed1, Read/Write */
};


/*
 *  ICMCompressionSessionOptionsSetAllowTemporalCompression()
 *  
 *  Summary:
 *    Enables temporal compression.
 *  
 *  Discussion:
 *    By default, temporal compression is disabled. 
 *    IMPORTANT: If you want temporal compression (P frames and/or B
 *    frames) you must set this to true.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionOptionsSetAllowTemporalCompression(
  ICMCompressionSessionOptionsRef   options,
  Boolean                           allowTemporalCompression);


/*
 *  ICMCompressionSessionOptionsGetAllowTemporalCompression()
 *  
 *  Summary:
 *    Retrieves the allow temporal compression flag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( Boolean )
ICMCompressionSessionOptionsGetAllowTemporalCompression(ICMCompressionSessionOptionsRef options);


/*
 *  ICMCompressionSessionOptionsSetAllowFrameReordering()
 *  
 *  Summary:
 *    Enables frame reordering.
 *  
 *  Discussion:
 *    In order to encode B frames, a compressor must reorder frames,
 *    which means that the order in which they will be emitted and
 *    stored (the decode order) is different from the order in which
 *    they were presented to the compressor (the display order). 
 *     By default, frame reordering is disabled. 
 *    IMPORTANT: In order to encode using B frames, you must enable
 *    frame reordering.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionOptionsSetAllowFrameReordering(
  ICMCompressionSessionOptionsRef   options,
  Boolean                           allowFrameReordering);


/*
 *  ICMCompressionSessionOptionsGetAllowFrameReordering()
 *  
 *  Summary:
 *    Retrieves the allow frame reordering flag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( Boolean )
ICMCompressionSessionOptionsGetAllowFrameReordering(ICMCompressionSessionOptionsRef options);


/*
 *  ICMCompressionSessionOptionsSetDurationsNeeded()
 *  
 *  Summary:
 *    Indicates that durations of emitted frames are needed.
 *  
 *  Discussion:
 *    If this flag is set and source frames are provided with times but
 *    not durations, then frames will be delayed so that durations can
 *    be calculated as the difference between one frame's time stamp
 *    and the next frame's time stamp. 
 *    By default, this flag is clear, so frames will not be delayed in
 *    order to calculate durations. 
 *    IMPORTANT: If you will be passing encoded frames to
 *    AddMediaSampleFromEncodedFrame, you must set this flag to true.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionOptionsSetDurationsNeeded(
  ICMCompressionSessionOptionsRef   options,
  Boolean                           decodeDurationsNeeded);


/*
 *  ICMCompressionSessionOptionsGetDurationsNeeded()
 *  
 *  Summary:
 *    Retrieves the "durations needed" flag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( Boolean )
ICMCompressionSessionOptionsGetDurationsNeeded(ICMCompressionSessionOptionsRef options);


/*
 *  ICMCompressionSessionOptionsSetMaxKeyFrameInterval()
 *  
 *  Summary:
 *    Sets the maximum interval between key frames (also known as the
 *    key frame rate).
 *  
 *  Discussion:
 *    Compressors are allowed to generate key frames more frequently if
 *    this would result in more efficient compression. 
 *    The default key frame interval is 0, which indicates that the
 *    compressor should choose where to place all key frames. 
 *     (Note: this is a break with previous practice, which used a key
 *    frame rate of zero to disable temporal compression.)
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionOptionsSetMaxKeyFrameInterval(
  ICMCompressionSessionOptionsRef   options,
  SInt32                            maxKeyFrameInterval);


/*
 *  ICMCompressionSessionOptionsGetMaxKeyFrameInterval()
 *  
 *  Summary:
 *    Retrieves the maximum key frame interval.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( SInt32 )
ICMCompressionSessionOptionsGetMaxKeyFrameInterval(ICMCompressionSessionOptionsRef options);


/*
 *  ICMCompressionSessionOptionsSetAllowFrameTimeChanges()
 *  
 *  Summary:
 *    Enables the compressor to modify frame times.
 *  
 *  Discussion:
 *    Some compressors are able to identify and coalesce runs of
 *    identical frames and output single frames with longer duration,
 *    or output frames at a different frame rate from the original.
 *    This feature is controlled by the "allow frame time changes"
 *    flag. By default, this flag is set to false, which forces
 *    compressors to emit one encoded frame for every source frame, and
 *    to preserve frame display times. 
 *    (Note: this feature replaces the practice of having compressors
 *    return special high similarity values to indicate that frames
 *    could be dropped.) 
 *    If you want to allow the compressor to modify frame times in
 *    order to improve compression performance, enable frame time
 *    changes.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionSessionOptionsSetAllowFrameTimeChanges(
  ICMCompressionSessionOptionsRef   options,
  Boolean                           allowFrameTimeChanges);


/*
 *  ICMCompressionSessionOptionsGetAllowFrameTimeChanges()
 *  
 *  Summary:
 *    Retrieves the allow frame time changes flag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( Boolean )
ICMCompressionSessionOptionsGetAllowFrameTimeChanges(ICMCompressionSessionOptionsRef options);




/* ICMMultiPassStorageRef routines */

/*
 *  ICMMultiPassStorageCreationFlags
 *  
 */
typedef UInt32 ICMMultiPassStorageCreationFlags;
enum {

  /*
   * Indicates that the temporary file should not be deleted when the
   * multipass storage is released.
   */
  kICMMultiPassStorage_DoNotDeleteWhenDone = 1L << 0
};

/*
 *  ICMMultiPassStorageCreateWithTemporaryFile()
 *  
 *  Summary:
 *    Creates a multipass storage using a temporary file.
 *  
 *  Discussion:
 *    If you pass NULL for directoryRef, the ICM will use the user's
 *    temporary items folder. 
 *    If you pass NULL for fileName, the ICM will pick a unique name.
 *    
 *    The file will be deleted when the multipass storage is released,
 *    unless you set the kICMMultiPassStorage_DoNotDeleteWhenDone flag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMMultiPassStorageCreateWithTemporaryFile(
  CFAllocatorRef                     allocator,                 /* can be NULL */
  FSRef *                            directoryRef,              /* can be NULL */
  CFStringRef                        fileName,                  /* can be NULL */
  ICMMultiPassStorageCreationFlags   flags,
  ICMMultiPassStorageRef *           multiPassStorageOut);



/*
 *  ICMMultiPassStorageStep
 *  
 *  Summary:
 *    Indicates a jump in time stamps used to index multipass storage.
 */
typedef UInt32 ICMMultiPassStorageStep;
enum {

  /*
   * Requests the first time stamp at which a value is stored.
   */
  kICMMultiPassStorage_GetFirstTimeStamp = 1,

  /*
   * Requests the previous time stamp before the given time stamp at
   * which a value is stored.
   */
  kICMMultiPassStorage_GetPreviousTimeStamp = 2,

  /*
   * Requests the next time stamp after the given time stamp at which a
   * value is stored.
   */
  kICMMultiPassStorage_GetNextTimeStamp = 3,

  /*
   * Requests the last time stamp at which a value is stored.
   */
  kICMMultiPassStorage_GetLastTimeStamp = 4
};



/*
 *  ICMMultiPassSetDataAtTimeStampCallback
 *  
 *  Summary:
 *    Stores a value at a given time stamp.
 *  
 *  Discussion:
 *    The new data should replace any previous data held at that
 *    timestamp with that index.
 *  
 *  Parameters:
 *    
 *    storageRefCon:
 *      The callbacks' reference value.
 *    
 *    timeStamp:
 *      The time stamp at which the value should be stored.
 *    
 *    index:
 *      An index by which multiple values may be stored at a time
 *      stamp. The meaning of individual indexes is private to the
 *      compressor.
 *    
 *    data:
 *      The data to be stored, or NULL to delete the value. The
 *      contents are private to the compressor component.
 */
typedef CALLBACK_API_C( OSStatus , ICMMultiPassSetDataAtTimeStampCallback )(void *storageRefCon, TimeValue64 timeStamp, long index, CFDataRef data);

/*
 *  ICMMultiPassGetTimeStampCallback
 *  
 *  Summary:
 *    Retrieves a time stamp for which a value is stored.
 *  
 *  Parameters:
 *    
 *    storageRefCon:
 *      The callbacks' reference value.
 *    
 *    fromTimeStamp:
 *      The initial time stamp.  Ignored for some values of step.
 *    
 *    step:
 *      Indicates the kind of time stamp search to perform.
 *    
 *    timeStampOut:
 *      Points to a TimeValue64 to receive the found time stamp. Set to
 *      -1 if no time stamp is found.
 */
typedef CALLBACK_API_C( OSStatus , ICMMultiPassGetTimeStampCallback )(void *storageRefCon, TimeValue64 fromTimeStamp, ICMMultiPassStorageStep step, TimeValue64 *timeStampOut);

/*
 *  ICMMultiPassCopyDataAtTimeStampCallback
 *  
 *  Summary:
 *    Retrieves a value at a given time stamp and index.
 *  
 *  Parameters:
 *    
 *    storageRefCon:
 *      The callbacks' reference value.
 *    
 *    timeStamp:
 *      The time stamp at which the value should be retrieved.
 *    
 *    index:
 *      An index by which multiple values may be stored at a time
 *      stamp. The meaning of individual indexes is private to the
 *      compressor.
 *    
 *    dataOut:
 *      Points to a variable to receive the value. Set to a
 *      newly-created CFMutableData containing the value for the given
 *      time stamp and index, or set to NULL if no value is at that
 *      time stamp and index. It will be the callers responsibility to
 *      release the CFMutableData.
 */
typedef CALLBACK_API_C( OSStatus , ICMMultiPassCopyDataAtTimeStampCallback )(void *storageRefCon, TimeValue64 timeStamp, long index, CFMutableDataRef *dataOut);

/*
 *  ICMMultiPassReleaseCallback
 *  
 *  Summary:
 *    Called when the multipass storage's retain count drops to zero.
 *  
 *  Parameters:
 *    
 *    storageRefCon:
 *      The callbacks' reference value.
 */
typedef CALLBACK_API_C( OSStatus , ICMMultiPassReleaseCallback )(void * storageRefCon);

/*
 *  ICMMultiPassStorageCallbacks
 *  
 *  Summary:
 *    A collection of callbacks for creating a custom multipass storage
 *    object.
 */
struct ICMMultiPassStorageCallbacks {

  /*
   * The version of the struct.  Set to
   * kICMMultiPassStorageCallbacksVersionOne.
   */
  UInt32              version;

  /*
   * The callbacks' reference value.
   */
  void *              storageRefCon;

  /*
   * The callback for storing values.
   */
  ICMMultiPassSetDataAtTimeStampCallback  setDataAtTimeStampCallback;

  /*
   * The callback for finding time stamps.
   */
  ICMMultiPassGetTimeStampCallback  getTimeStampCallback;

  /*
   * The callback for retrieving values.
   */
  ICMMultiPassCopyDataAtTimeStampCallback  copyDataAtTimeStampCallback;

  /*
   * The callback for disposing the callback's state when done.
   */
  ICMMultiPassReleaseCallback  releaseCallback;
};
typedef struct ICMMultiPassStorageCallbacks ICMMultiPassStorageCallbacks;
enum {
  kICMMultiPassStorageCallbacksVersionOne = 1
};

/*
 *  ICMMultiPassStorageCreateWithCallbacks()
 *  
 *  Summary:
 *    Assembles a multipass storage mechanism from callbacks.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMMultiPassStorageCreateWithCallbacks(
  CFAllocatorRef                  allocator,                 /* can be NULL */
  ICMMultiPassStorageCallbacks *  callbacks,
  ICMMultiPassStorageRef *        multiPassStorageOut);


/*
 *  ICMMultiPassStorageRetain()
 *  
 *  Summary:
 *    Increments the retain count of a multipass storage object.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMMultiPassStorageRef )
ICMMultiPassStorageRetain(ICMMultiPassStorageRef multiPassStorage);


/*
 *  ICMMultiPassStorageRelease()
 *  
 *  Summary:
 *    Decrements the retain count of a multipass storage object.  If it
 *    drops to zero, the object is disposed.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( void )
ICMMultiPassStorageRelease(ICMMultiPassStorageRef multiPassStorage);


/*
 *  ICMMultiPassStorageGetTypeID()
 *  
 *  Summary:
 *    Returns the CFTypeID for multipass storage objects.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( CFTypeID )
ICMMultiPassStorageGetTypeID(void);



/* ICMCompressionFrameOptionsRef routines */
/*
 *  ICMCompressionFrameOptionsCreate()
 *  
 *  Summary:
 *    Creates a frame compression options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionFrameOptionsCreate(
  CFAllocatorRef                   allocator,       /* can be NULL */
  ICMCompressionSessionRef         session,
  ICMCompressionFrameOptionsRef *  options);


/*
 *  ICMCompressionFrameOptionsCreateCopy()
 *  
 *  Summary:
 *    Copies a frame compression options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionFrameOptionsCreateCopy(
  CFAllocatorRef                   allocator,             /* can be NULL */
  ICMCompressionFrameOptionsRef    originalOptions,
  ICMCompressionFrameOptionsRef *  copiedOptions);


/*
 *  ICMCompressionFrameOptionsRetain()
 *  
 *  Summary:
 *    Increments the retain count of a frame compression options object.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMCompressionFrameOptionsRef )
ICMCompressionFrameOptionsRetain(ICMCompressionFrameOptionsRef options);


/*
 *  ICMCompressionFrameOptionsRelease()
 *  
 *  Summary:
 *    Decrements the retain count of a frame compression options
 *    object.  If it drops to zero, the object is disposed.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( void )
ICMCompressionFrameOptionsRelease(ICMCompressionFrameOptionsRef options);


/*
 *  ICMCompressionFrameOptionsGetTypeID()
 *  
 *  Summary:
 *    Returns the CFTypeID for frame compression options objects.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( CFTypeID )
ICMCompressionFrameOptionsGetTypeID(void);


/*
 *  ICMCompressionFrameOptionsGetPropertyInfo()
 *  
 *  Summary:
 *    Retrieves information about properties of a compression frame
 *    options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionFrameOptionsGetPropertyInfo(
  ICMCompressionFrameOptionsRef   options,
  ComponentPropertyClass          inPropClass,
  ComponentPropertyID             inPropID,
  ComponentValueType *            outPropType,            /* can be NULL */
  ByteCount *                     outPropValueSize,       /* can be NULL */
  UInt32 *                        outPropertyFlags);      /* can be NULL */


/*
 *  ICMCompressionFrameOptionsGetProperty()
 *  
 *  Summary:
 *    Retrieves the value of a specific property of a compression frame
 *    options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionFrameOptionsGetProperty(
  ICMCompressionFrameOptionsRef   options,
  ComponentPropertyClass          inPropClass,
  ComponentPropertyID             inPropID,
  ByteCount                       inPropValueSize,
  ComponentValuePtr               outPropValueAddress,
  ByteCount *                     outPropValueSizeUsed);      /* can be NULL */


/*
 *  ICMCompressionFrameOptionsSetProperty()
 *  
 *  Summary:
 *    Sets the value of a specific property of a compression frame
 *    options object.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionFrameOptionsSetProperty(
  ICMCompressionFrameOptionsRef   options,
  ComponentPropertyClass          inPropClass,
  ComponentPropertyID             inPropID,
  ByteCount                       inPropValueSize,
  ConstComponentValuePtr          inPropValueAddress);



/*
 *  Summary:
 *    Properties of compression frame options objects.
 */
enum {

  /*
   * Class identifier for compression frame options object properties.
   */
  kQTPropertyClass_ICMCompressionFrameOptions = FOUR_CHAR_CODE('icfo'),

  /*
   * Forces frames to be compressed as key frames. 
   * The compressor must obey the "force key frame" flag if set. By
   * default this property is false.
   */
  kICMCompressionFrameOptionsPropertyID_ForceKeyFrame = FOUR_CHAR_CODE('keyf'), /* Boolean, Read/Write */

  /*
   * Requests a frame be compressed as a particular frame type. 
   *  The frame type setting may be ignored by the compressor if not
   * appropriate. 
   * By default this is set to kICMFrameType_Unknown. 
   * Do not assume that kICMFrameType_I means a key frame; if you need
   * a key frame, set the "force key frame" property.
   */
  kICMCompressionFrameOptionsPropertyID_FrameType = FOUR_CHAR_CODE('frty') /* ICMFrameType, Read/Write */
};

/*
 *  ICMCompressionFrameOptionsSetForceKeyFrame()
 *  
 *  Summary:
 *    Forces frames to be compressed as key frames.
 *  
 *  Discussion:
 *    The compressor must obey the "force key frame" flag if set. By
 *    default this is set false.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionFrameOptionsSetForceKeyFrame(
  ICMCompressionFrameOptionsRef   options,
  Boolean                         forceKeyFrame);


/*
 *  ICMCompressionFrameOptionsGetForceKeyFrame()
 *  
 *  Summary:
 *    Retrieves the "force key frame" flag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( Boolean )
ICMCompressionFrameOptionsGetForceKeyFrame(ICMCompressionFrameOptionsRef options);


/*
 *  ICMCompressionFrameOptionsSetFrameType()
 *  
 *  Summary:
 *    Requests a frame be compressed as a particular frame type.
 *  
 *  Discussion:
 *    The frame type setting may be ignored by the compressor if not
 *    appropriate. 
 *    By default this is set to kICMFrameType_Unknown. 
 *    Do not assume that kICMFrameType_I means a key frame; if you need
 *    a key frame, set forceKeyFrame.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressionFrameOptionsSetFrameType(
  ICMCompressionFrameOptionsRef   options,
  ICMFrameType                    frameType);


/*
 *  ICMCompressionFrameOptionsGetFrameType()
 *  
 *  Summary:
 *    Retrieves the frame type setting.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMFrameType )
ICMCompressionFrameOptionsGetFrameType(ICMCompressionFrameOptionsRef options);



/* ICMEncodedFrameRef routines */
/*
 *  ICMEncodedFrameRetain()
 *  
 *  Summary:
 *    Increments the retain count of an encoded frame object.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMEncodedFrameRef )
ICMEncodedFrameRetain(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameRelease()
 *  
 *  Summary:
 *    Decrements the retain count of an encoded frame object.  If it
 *    drops to zero, the object is disposed.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( void )
ICMEncodedFrameRelease(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameGetTypeID()
 *  
 *  Summary:
 *    Returns the CFTypeID for encoded frame objects.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( CFTypeID )
ICMEncodedFrameGetTypeID(void);


/*
 *  ICMEncodedFrameGetDataPtr()
 *  
 *  Summary:
 *    Gets the data buffer.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( UInt8 * )
ICMEncodedFrameGetDataPtr(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameGetBufferSize()
 *  
 *  Summary:
 *    Gets the size of the data buffer.
 *  
 *  Discussion:
 *    This is the physical size of the buffer.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ByteCount )
ICMEncodedFrameGetBufferSize(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameGetDataSize()
 *  
 *  Summary:
 *    Gets the data size of the compressed frame in the buffer.
 *  
 *  Discussion:
 *    This is the logical size of the frame data. It may be less than
 *    the physical size of the buffer.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ByteCount )
ICMEncodedFrameGetDataSize(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameSetDataSize()
 *  
 *  Summary:
 *    Sets the data size of the compressed frame in the buffer.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMEncodedFrameSetDataSize(
  ICMMutableEncodedFrameRef   frame,
  ByteCount                   dataSize);


/*
 *  ICMEncodedFrameGetDecodeNumber()
 *  
 *  Summary:
 *    Retrieves the decode number.
 *  
 *  Discussion:
 *    The ICM automatically stamps ascending decode numbers on frames
 *    after the compressor emits them. The first decode number in a
 *    session is 1. 
 *    Note: Compressors should not call this function.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( UInt32 )
ICMEncodedFrameGetDecodeNumber(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameGetTimeScale()
 *  
 *  Summary:
 *    Retrieves the timescale.
 *  
 *  Discussion:
 *    This is always the same as the timescale of the compression
 *    session.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( TimeScale )
ICMEncodedFrameGetTimeScale(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameGetImageDescription()
 *  
 *  Summary:
 *    Retrieves the image description.
 *  
 *  Discussion:
 *    Returns the same image description handle as
 *    ICMCompressionSessionGetImageDescription. 
 *    IMPORTANT: The caller should NOT dispose this handle.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMEncodedFrameGetImageDescription(
  ICMEncodedFrameRef        frame,
  ImageDescriptionHandle *  imageDescOut);


/*
 *  ICMEncodedFrameGetValidTimeFlags()
 *  
 *  Summary:
 *    Retrieves flags indicating which of the time stamps and durations
 *    are valid.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMValidTimeFlags )
ICMEncodedFrameGetValidTimeFlags(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameGetDecodeTimeStamp()
 *  
 *  Summary:
 *    Retrieves the frame's decode time stamp.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( TimeValue64 )
ICMEncodedFrameGetDecodeTimeStamp(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameGetDisplayTimeStamp()
 *  
 *  Summary:
 *    Retrieves the frame's display time stamp.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( TimeValue64 )
ICMEncodedFrameGetDisplayTimeStamp(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameGetDisplayOffset()
 *  
 *  Summary:
 *    Retrieves the frame's display offset, which is the offset from
 *    decode time stamp to display time stamp.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( TimeValue64 )
ICMEncodedFrameGetDisplayOffset(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameGetDecodeDuration()
 *  
 *  Summary:
 *    Retrieves the frame's decode duration.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( TimeValue64 )
ICMEncodedFrameGetDecodeDuration(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameGetDisplayDuration()
 *  
 *  Summary:
 *    Retrieves the frame's display duration.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( TimeValue64 )
ICMEncodedFrameGetDisplayDuration(ICMEncodedFrameRef frame);



/*
 *  ICMEncodedFrameSetValidTimeFlags()
 *  
 *  Summary:
 *    Sets flags that indicate which of the time stamps and durations
 *    are valid.
 *  
 *  Discussion:
 *    Note that setting the (decode/display) (timestamp/duration)
 *    automatically sets the corresponding valid time flags. For
 *    example, calling ICMEncodedFrameSetDecodeTimeStamp sets
 *    kICMValidTime_DisplayTimeStampIsValid. If both decode timestamp
 *    and display timestamp are valid,
 *    kICMValidTime_DisplayOffsetIsValid is automatically set.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMEncodedFrameSetValidTimeFlags(
  ICMMutableEncodedFrameRef   frame,
  ICMValidTimeFlags           validTimeFlags);


/*
 *  ICMEncodedFrameSetDecodeTimeStamp()
 *  
 *  Summary:
 *    Sets the decode time stamp.
 *  
 *  Discussion:
 *    This automatically sets the kICMValidTime_DecodeTimeStampIsValid
 *    flag. If the display timestamp is valid, it also sets the
 *    kICMValidTime_DisplayOffsetIsValid flag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMEncodedFrameSetDecodeTimeStamp(
  ICMMutableEncodedFrameRef   frame,
  TimeValue64                 decodeTimeStamp);


/*
 *  ICMEncodedFrameSetDisplayTimeStamp()
 *  
 *  Summary:
 *    Sets the display time stamp.
 *  
 *  Discussion:
 *    This automatically sets the kICMValidTime_DisplayTimeStampIsValid
 *    flag. If the decode timestamp is valid, it also sets the
 *    kICMValidTime_DisplayOffsetIsValid flag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMEncodedFrameSetDisplayTimeStamp(
  ICMMutableEncodedFrameRef   frame,
  TimeValue64                 displayTimeStamp);


/*
 *  ICMEncodedFrameSetDecodeDuration()
 *  
 *  Summary:
 *    Sets the decode duration.
 *  
 *  Discussion:
 *    This automatically sets the kICMValidTime_DecodeDurationIsValid
 *    flag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMEncodedFrameSetDecodeDuration(
  ICMMutableEncodedFrameRef   frame,
  TimeValue64                 decodeDuration);


/*
 *  ICMEncodedFrameSetDisplayDuration()
 *  
 *  Summary:
 *    Sets the display duration.
 *  
 *  Discussion:
 *    This automatically sets the kICMValidTime_DisplayDurationIsValid
 *    flag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMEncodedFrameSetDisplayDuration(
  ICMMutableEncodedFrameRef   frame,
  TimeValue64                 displayDuration);


/*
 *  ICMEncodedFrameGetMediaSampleFlags()
 *  
 *  Summary:
 *    Retrieves the media sample flags for an encoded frame.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( MediaSampleFlags )
ICMEncodedFrameGetMediaSampleFlags(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameSetMediaSampleFlags()
 *  
 *  Summary:
 *    Sets the media sample flags for an encoded frame.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMEncodedFrameSetMediaSampleFlags(
  ICMMutableEncodedFrameRef   frame,
  MediaSampleFlags            mediaSampleFlags);


/*
 *  ICMEncodedFrameGetFrameType()
 *  
 *  Summary:
 *    Retrieves the frame type for an encoded frame.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMFrameType )
ICMEncodedFrameGetFrameType(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameSetFrameType()
 *  
 *  Summary:
 *    Sets the frame type for an encoded frame.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMEncodedFrameSetFrameType(
  ICMMutableEncodedFrameRef   frame,
  ICMFrameType                frameType);


/*
 *  ICMEncodedFrameGetSimilarity()
 *  
 *  Summary:
 *    Retrieves the similarity for an encoded frame.
 *  
 *  Discussion:
 *    1.0 means identical.  0.0 means not at all alike. By default,
 *    this is set to -1.0, which means unknown.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( Float32 )
ICMEncodedFrameGetSimilarity(ICMEncodedFrameRef frame);


/*
 *  ICMEncodedFrameSetSimilarity()
 *  
 *  Summary:
 *    Sets the similarity for an encoded frame.
 *  
 *  Discussion:
 *    1.0 means identical.  0.0 means not at all alike.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMEncodedFrameSetSimilarity(
  ICMMutableEncodedFrameRef   frame,
  Float32                     similarity);


/*
 *  ICMEncodedFrameGetSourceFrameRefCon()
 *  
 *  Summary:
 *    Retrieves the source frame reference value.
 *  
 *  Discussion:
 *    This is copied from the sourceFrameRefCon parameter to
 *    ICMCompressionSessionEncodeFrame.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( void * )
ICMEncodedFrameGetSourceFrameRefCon(ICMEncodedFrameRef frame);


/* ICMCompressorSession interface for compressor components */

/*
 *  ICMCompressorSessionRef
 *  
 *  Summary:
 *    Represents the session between the ICM and an image compressor
 *    component.
 *  
 *  Discussion:
 *    (Do not confuse this with ICMCompressionSessionRef, which is the
 *    session between the client and the ICM.) Note: compressors do not
 *    need to make any retain or release calls on this token.
 */
typedef struct OpaqueICMCompressorSession*  ICMCompressorSessionRef;

/*
 *  ICMCompressorSourceFrameRef
 *  
 *  Summary:
 *    An opaque token that represents a frame that has been passed to
 *    ICMCompressionSessionEncodeFrame.
 *  
 *  Discussion:
 *    Such tokens are passed to the compressor component, which may
 *    retain a window of them in order to perform out-of-order encoding.
 */
typedef struct OpaqueICMCompressorSourceFrame*  ICMCompressorSourceFrameRef;
/*
 *  ICMCompressorSourceFrameRetain()
 *  
 *  Summary:
 *    Increments the retain count of a source frame object.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMCompressorSourceFrameRef )
ICMCompressorSourceFrameRetain(ICMCompressorSourceFrameRef sourceFrame);


/*
 *  ICMCompressorSourceFrameRelease()
 *  
 *  Summary:
 *    Decrements the retain count of a source frame object.  If it
 *    drops to zero, the object is disposed.
 *  
 *  Discussion:
 *    If you pass NULL to this function, nothing happens.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( void )
ICMCompressorSourceFrameRelease(ICMCompressorSourceFrameRef sourceFrame);


/*
 *  ICMCompressorSourceFrameGetTypeID()
 *  
 *  Summary:
 *    Returns the CFTypeID for source frame objects.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( CFTypeID )
ICMCompressorSourceFrameGetTypeID(void);


/*
 *  ICMCompressorSourceFrameGetPixelBuffer()
 *  
 *  Summary:
 *    Retrieves a source frame's pixel buffer.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( CVPixelBufferRef )
ICMCompressorSourceFrameGetPixelBuffer(ICMCompressorSourceFrameRef sourceFrame);


/*
 *  ICMCompressorSourceFrameGetDisplayNumber()
 *  
 *  Summary:
 *    Retrieves a source frame's display number.
 *  
 *  Discussion:
 *    The ICM tags source frames with display numbers in the order they
 *    are passed to ICMCompressionSessionEncodeFrame; the first display
 *    number is 1. Compressors may compare these numbers to work out
 *    whether prediction is forward or backward even when display times
 *    are not provided.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( long )
ICMCompressorSourceFrameGetDisplayNumber(ICMCompressorSourceFrameRef sourceFrame);


/*
 *  ICMCompressorSourceFrameGetDisplayTimeStampAndDuration()
 *  
 *  Summary:
 *    Retrieves the display time stamp and duration of a source frame.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressorSourceFrameGetDisplayTimeStampAndDuration(
  ICMCompressorSourceFrameRef   sourceFrame,
  TimeValue64 *                 displayTimeStampOut,       /* can be NULL */
  TimeValue64 *                 displayDurationOut,        /* can be NULL */
  TimeScale *                   timeScaleOut,              /* can be NULL */
  ICMValidTimeFlags *           validTimeFlagsOut);        /* can be NULL */


/*
 *  ICMCompressorSourceFrameGetFrameOptions()
 *  
 *  Summary:
 *    Retrieves the frame compression options for a source frame.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( ICMCompressionFrameOptionsRef )
ICMCompressorSourceFrameGetFrameOptions(ICMCompressorSourceFrameRef sourceFrame);


/*
 *  ICMCompressorSourceFrameDetachPixelBuffer()
 *  
 *  Summary:
 *    Disconnects the pixel buffer from the source frame and allows it
 *    to be released.
 *  
 *  Discussion:
 *    Compressor components often need to hold onto
 *    ICMCompressorSourceFrameRefs for some time after they are done
 *    with the pixel buffers. In order to allow pixel buffer memory to
 *    be released earlier, they may call
 *    ICMCompressorSourceFrameDetachPixelBuffer to declare that they
 *    have no further need for the source frame's pixel buffer. After
 *    calling this, ICMCompressorSourceFrameGetPixelBuffer will return
 *    NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressorSourceFrameDetachPixelBuffer(ICMCompressorSourceFrameRef sourceFrame);



/*
 *  ICMCompressorSessionDropFrame()
 *  
 *  Summary:
 *    Called by a compressor to indicate that sourceFrame has been
 *    dropped and will not contribute to any encoded frames.
 *  
 *  Discussion:
 *    Calling this function does not automatically release the source
 *    frame; if the compressor called ICMCompressorSourceFrameRetain it
 *    should still call ICMCompressorSourceFrameRelease.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressorSessionDropFrame(
  ICMCompressorSessionRef       session,
  ICMCompressorSourceFrameRef   sourceFrame);


/*
 *  ICMCompressorSessionEmitEncodedFrame()
 *  
 *  Summary:
 *    Called by a compressor to output an encoded frame corresponding
 *    to one (or more) source frames.
 *  
 *  Discussion:
 *    (Encoded frames may only correspond to more than one source frame
 *    if allowFrameTimeChanges is set in the
 *    compressionSessionOptions.) 
 *    After calling this, the compressor should release the encoded
 *    frame by calling ICMEncodedFrameRelease. 
 *    Calling this function does not automatically release the source
 *    frames; if the compressor called ICMCompressorSourceFrameRetain
 *    it should still call ICMCompressorSourceFrameRelease.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMCompressorSessionEmitEncodedFrame(
  ICMCompressorSessionRef       session,
  ICMMutableEncodedFrameRef     encodedFrame,
  long                          numberOfSourceFrames,
  ICMCompressorSourceFrameRef   sourceFrames[]);


/*
 *  ICMEncodedFrameCreateMutable()
 *  
 *  Summary:
 *    Called by a compressor to create an encoded-frame token
 *    corresponding to a given source frame.
 *  
 *  Discussion:
 *    The encoded frame will initially have zero mediaSampleFlags; if
 *    the frame is not a key frame, the compressor must call
 *    ICMEncodedFrameSetMediaSampleFlags to set mediaSampleNotSync. If
 *    the frame is droppable, the compressor should set
 *    mediaSampleDroppable. If the frame is a partial key frame, the
 *    compressor should set mediaSamplePartialSync. 
 *    The encoded frame will initially have undefined decodeTimeStamp
 *    and decodeDuration. The compressor may set these directly by
 *    calling ICMEncodedFrameSetDecodeTimeStamp and/or
 *    ICMEncodedFrameSetDecodeDuration. If these are not set by the
 *    compressor, the ICM will attempt to derive them.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMEncodedFrameCreateMutable(
  ICMCompressorSessionRef       session,
  ICMCompressorSourceFrameRef   sourceFrame,
  ByteCount                     bufferSize,
  ICMMutableEncodedFrameRef *   frameOut);


/* Multi-pass storage access routines for compressor components */
/*
 *  ICMMultiPassStorageSetDataAtTimeStamp()
 *  
 *  Summary:
 *    Called by a multipass-capable compressor to store data at a given
 *    timestamp.
 *  
 *  Discussion:
 *    The new data replaces any previous data held at that timestamp.
 *    If data is NULL, the data for that timestamp is deleted. The
 *    format of the data is private to the compressor.
 *  
 *  Parameters:
 *    
 *    multiPassStorage:
 *      The multipass storage object.
 *    
 *    timeStamp:
 *      The time stamp at which the value should be stored.
 *    
 *    index:
 *      An index by which multiple values may be stored at a time
 *      stamp. The meaning of individual indexes is private to the
 *      compressor.
 *    
 *    data:
 *      The data to be stored, or NULL to delete the value.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMMultiPassStorageSetDataAtTimeStamp(
  ICMMultiPassStorageRef   multiPassStorage,
  TimeValue64              timeStamp,
  long                     index,
  CFDataRef                data);


/*
 *  ICMMultiPassStorageGetTimeStamp()
 *  
 *  Summary:
 *    Called by a multipass-capable compressor to retrieve a time stamp
 *    for which a value is stored.
 *  
 *  Parameters:
 *    
 *    multiPassStorage:
 *      The multipass storage object.
 *    
 *    fromTimeStamp:
 *      The initial time stamp.  Ignored for some values of step.
 *    
 *    step:
 *      Indicates the kind of time stamp search to perform.
 *    
 *    timeStampOut:
 *      Points to a TimeValue64 to receive the found time stamp. It
 *      will be set to -1 if no time stamp is found.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMMultiPassStorageGetTimeStamp(
  ICMMultiPassStorageRef    multiPassStorage,
  TimeValue64               fromTimeStamp,
  ICMMultiPassStorageStep   step,
  TimeValue64 *             timeStampOut);


/*
 *  ICMMultiPassStorageCopyDataAtTimeStamp()
 *  
 *  Summary:
 *    Called by a multipass-capable compressor to retrieve data at a
 *    given timestamp.
 *  
 *  Parameters:
 *    
 *    multiPassStorage:
 *      The multipass storage object.
 *    
 *    timeStamp:
 *      The time stamp at which the value should be retrieved.
 *    
 *    index:
 *      An index by which multiple values may be stored at a time
 *      stamp. The meaning of individual indexes is private to the
 *      compressor.
 *    
 *    dataOut:
 *      Points to a CFMutableDataRef to receive the value. It will be
 *      set to a newly-created CFMutableData containing the value for
 *      the given time stamp and index, or set to NULL if no value is
 *      at that time stamp and index. It is the caller's responsibility
 *      to release the CFMutableData.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
ICMMultiPassStorageCopyDataAtTimeStamp(
  ICMMultiPassStorageRef   multiPassStorage,
  TimeValue64              timeStamp,
  long                     index,
  CFMutableDataRef *       dataOut);




#if TARGET_OS_MAC
/*
 *  kQTVisualContextTypeKey
 *  
 *  Summary:
 *    Read-only CFStringRef: Type of the visual context.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
extern const CFStringRef kQTVisualContextTypeKey;
/*
 *  kQTVisualContextType_PixelBuffer
 *  
 *  Summary:
 *    Value for kQTVisualContextTypeKey for pixel buffer visual
 *    contexts.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
extern const CFStringRef kQTVisualContextType_PixelBuffer;
/*
 *  kQTVisualContextType_OpenGLTexture
 *  
 *  Summary:
 *    Value for kQTVisualContextTypeKey for OpenGL texture visual
 *    contexts.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
extern const CFStringRef kQTVisualContextType_OpenGLTexture;
/*
 *  kQTVisualContextWorkingColorSpaceKey
 *  
 *  Summary:
 *    CGColorSpaceRef: Color space in which QuickTime will perform
 *    image processing. If this attribute is not set, images will be
 *    processed in the output color space.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
extern const CFStringRef kQTVisualContextWorkingColorSpaceKey;
/*
 *  kQTVisualContextOutputColorSpaceKey
 *  
 *  Summary:
 *    CGColorSpaceRef: Color space of images produced by this visual
 *    context. If this attribute is not set, images may be in any color
 *    space.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
extern const CFStringRef kQTVisualContextOutputColorSpaceKey;
/*
 *  kQTVisualContextExpectedReadAheadKey
 *  
 *  Summary:
 *    CFNumberRef: Number of seconds ahead of real-time that the client
 *    expects to pull images out of the visual context.  Applications
 *    using the CoreVideo display link should set this attribute
 *    according to value returned from
 *    CVDisplayLinkGetOutputVideoLatency().
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
extern const CFStringRef kQTVisualContextExpectedReadAheadKey;
/*
 *  kQTVisualContextPixelBufferAttributesKey
 *  
 *  Summary:
 *    CFDictionaryRef: Dictionary containing pixel buffer attributes as
 *    described for the CoreVideo pixel buffer pool.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
extern const CFStringRef kQTVisualContextPixelBufferAttributesKey;
/*
 *  kQTVisualContextTargetDimensionsKey
 *  
 *  Summary:
 *    CFDictionaryRef: Dictionary containing
 *    kQTVisualContextTargetDimensions_WidthKey and
 *    kQTVisualContextTargetDimensions_HeightKey.  This is only a hint
 *    to optimize certain media types, such as text, that can render at
 *    any resolution. If this attribute is not set, the movie will
 *    render at its native resolution.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
extern const CFStringRef kQTVisualContextTargetDimensionsKey;
/*
 *  kQTVisualContextTargetDimensions_WidthKey
 *  
 *  Summary:
 *    CFNumberRef: Width, in pixels, of the rendering target.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
extern const CFStringRef kQTVisualContextTargetDimensions_WidthKey;
/*
 *  kQTVisualContextTargetDimensions_HeightKey
 *  
 *  Summary:
 *    CFNumberRef: Height, in pixels, of the rendering target.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
extern const CFStringRef kQTVisualContextTargetDimensions_HeightKey;
#else
#define kQTVisualContextTypeKey     CFSTR("Type")
#define kQTVisualContextType_PixelBuffer        CFSTR("PixelBuffer")
#define kQTVisualContextType_OpenGLTexture      CFSTR("OpenGLTexture")
#define kQTVisualContextType_Direct3DTexture        CFSTR("Direct3DTexture")
#define kQTVisualContextWorkingColorSpaceKey        CFSTR("WorkingColorSpace")
#define kQTVisualContextOutputColorSpaceKey     CFSTR("OutputColorSpace")
#define kQTVisualContextExpectedReadAheadKey        CFSTR("ExpectedReadAhead")
#define kQTVisualContextPixelBufferAttributesKey        CFSTR("PixelBufferAttributes")
#define kQTVisualContextTargetDimensionsKey         CFSTR("TargetDimensions")
#define kQTVisualContextTargetDimensions_WidthKey       CFSTR("Width")
#define kQTVisualContextTargetDimensions_HeightKey      CFSTR("Height")
#endif  /* TARGET_OS_MAC */

/*
 *  QTVisualContextRetain()
 *  
 *  Summary:
 *    Increments the visual context's reference count.
 *  
 *  Discussion:
 *    The same visual context is returned for convenience. If
 *    visualContext is NULL, nothing happens.
 *  
 *  Parameters:
 *    
 *    visualContext:
 *      [in] The visual context to retain.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( QTVisualContextRef )
QTVisualContextRetain(QTVisualContextRef visualContext);


/*
 *  QTVisualContextRelease()
 *  
 *  Summary:
 *    Decrements the visual context's reference count.
 *  
 *  Discussion:
 *    If the retain count decreases to zero, the visual context is
 *    disposed. If visualContext is NULL, nothing happens.
 *  
 *  Parameters:
 *    
 *    visualContext:
 *      [in] The visual context to release.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( void )
QTVisualContextRelease(QTVisualContextRef visualContext);


/*
 *  QTVisualContextGetTypeID()
 *  
 *  Summary:
 *    Returns the CFTypeID for QTVisualContextRef.
 *  
 *  Discussion:
 *    You could use this to test whether a CFTypeRef that extracted
 *    from a CF container such as a CFArray was a QTVisualContextRef. 
 *    All visual contexts have the same CFTypeID.  If you need to
 *    distinguish between different types of visual contexts (eg.
 *    PixelBuffer vs. OpenGLTexture), query for kQTVisualContextTypeKey
 *    with QTVisualContextGetAttribute().
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( CFTypeID )
QTVisualContextGetTypeID(void);


/*
 *  QTVisualContextSetAttribute()
 *  
 *  Summary:
 *    Sets a visual context attribute.
 *  
 *  Parameters:
 *    
 *    visualContext:
 *      [in] The visual context.
 *    
 *    attributeKey:
 *      [in] Identifier of attribute to set.
 *    
 *    attributeValue:
 *      [in] Value of attribute to set, or NULL to remove a value.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
QTVisualContextSetAttribute(
  QTVisualContextRef   visualContext,
  CFStringRef          attributeKey,
  CFTypeRef            attributeValue);      /* can be NULL */


/*
 *  QTVisualContextGetAttribute()
 *  
 *  Summary:
 *    Gets a visual context attribute.
 *  
 *  Parameters:
 *    
 *    visualContext:
 *      [in]  The visual context.
 *    
 *    attributeKey:
 *      [in]  Identifier of attribute to get.
 *    
 *    attributeValueOut:
 *      [out] Pointer to variable that will receive the attribute
 *      value, or NULL if the attribute is not set.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
QTVisualContextGetAttribute(
  QTVisualContextRef   visualContext,
  CFStringRef          attributeKey,
  CFTypeRef *          attributeValueOut);



/*
 *  QTVisualContextImageAvailableCallback
 *  
 *  Summary:
 *    User-defined callback function to recieve notifications when a
 *    new image becomes available.
 *  
 *  Discussion:
 *    Due to unpredictible activity, such as user seeks or the arrival
 *    of streaming video packets from a network, new images may become
 *    available for times supposedly occupied by previous images. 
 *    Applications using the CoreVideo display link to drive rendering
 *    probably do not need to install a callback of this type since
 *    they will already be checking for new images at a sufficient rate.
 *  
 *  Parameters:
 *    
 *    visualContext:
 *      [in] The visual context invoking the callback.
 *    
 *    timeStamp:
 *      [in] Time for which a new image has become available.
 *    
 *    refCon:
 *      [in] User-defined value passed to
 *      QTVisualContextSetImageAvailableCallback.
 */
typedef CALLBACK_API_C( void , QTVisualContextImageAvailableCallback )(QTVisualContextRef visualContext, const CVTimeStamp *timeStamp, void *refCon);
/*
 *  QTVisualContextSetImageAvailableCallback()
 *  
 *  Summary:
 *    Installs user-defined callback to be notified when new images
 *    become available.
 *  
 *  Discussion:
 *    This routine installs the user defined
 *    QTVisualContextImageAvailableCallback callback. There can only be
 *    one callback associated with a QTVisualContext at a given time.
 *  
 *  Parameters:
 *    
 *    visualContext:
 *      [in] The visual context.
 *    
 *    imageAvailableCallback:
 *      [in] User-defined callback function to recieve notifications.
 *      Pass NULL to remove any existing callback.
 *    
 *    refCon:
 *      [in] User-defined value to pass to
 *      QTVisualContextImageAvailableCallback.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
QTVisualContextSetImageAvailableCallback(
  QTVisualContextRef                      visualContext,
  QTVisualContextImageAvailableCallback   imageAvailableCallback,       /* can be NULL */
  void *                                  refCon);                      /* can be NULL */


/*
 *  QTVisualContextIsNewImageAvailable()
 *  
 *  Summary:
 *    Queries whether a new image is available for a given time.
 *  
 *  Discussion:
 *    This function returns true if there is a image available for the
 *    specified time that is different from the last image retrieved
 *    from QTVisualContextCopyImageForTime. See
 *    QTVisualContextCopyImageForTime for restrictions on time-stamps.
 *  
 *  Parameters:
 *    
 *    visualContext:
 *      [in] The visual context.
 *    
 *    timeStamp:
 *      [in] Time in question.  Pass NULL to request the image at the
 *      current time.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( Boolean )
QTVisualContextIsNewImageAvailable(
  QTVisualContextRef   visualContext,
  const CVTimeStamp *  timeStamp);          /* can be NULL */


/*
 *  QTVisualContextCopyImageForTime()
 *  
 *  Summary:
 *    Retrieves an image buffer from the visual context, indexed by the
 *    provided timestamp.
 *  
 *  Discussion:
 *    You should not request image buffers further ahead of the current
 *    time than the read-ahead time specified with the
 *    kQTVisualContextExpectedReadAheadKey attribute.  You may skip
 *    images by passing later times, but you may not pass an earlier
 *    time than passed to a previous call to this function.
 *  
 *  Parameters:
 *    
 *    visualContext:
 *      [in]  The visual context.
 *    
 *    allocator:
 *      [in]  Allocator used to create new CVImageBufferRef.
 *    
 *    timeStamp:
 *      [in]  Time in question.  Pass NULL to request the image at the
 *      current time.
 *    
 *    newImage:
 *      [out] Points to variable to receive new image. If there is no
 *      video at the requested time, this variable will be set to NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
QTVisualContextCopyImageForTime(
  QTVisualContextRef   visualContext,
  CFAllocatorRef       allocator,           /* can be NULL */
  const CVTimeStamp *  timeStamp,           /* can be NULL */
  CVImageBufferRef *   newImage);


/*
 *  QTVisualContextTask()
 *  
 *  Summary:
 *    Causes visual context to release internally held resources for
 *    later re-use.
 *  
 *  Discussion:
 *    For optimal resource management, this function should be called
 *    in every rendering pass, after old images have been released, new
 *    images have been used and all rendering has been flushed to the
 *    screen.  This call is not mandatory.
 *  
 *  Parameters:
 *    
 *    visualContext:
 *      [in] The visual context.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( void )
QTVisualContextTask(QTVisualContextRef visualContext);


#if TARGET_OS_MAC
/*
 *  QTOpenGLTextureContextCreate()
 *  
 *  Summary:
 *    Creates a new OpenGL texture context for the given OpenGL context
 *    and pixel format.
 *  
 *  Discussion:
 *    This function will fail if the graphics hardware is insufficient.
 *  
 *  Parameters:
 *    
 *    allocator:
 *      [in]  Allocator used to create the texture context.
 *    
 *    cglContext:
 *      [in]  OpenGL context used to create textures.
 *    
 *    cglPixelFormat:
 *      [in]  OpenGL pixel format used to create the OpenGL context.
 *    
 *    attributes:
 *      [in]  Dictionary of attributes.
 *    
 *    newTextureContext:
 *      [out] Points to a variable to recieve the new OpenGL texture
 *      context.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
QTOpenGLTextureContextCreate(
  CFAllocatorRef        allocator,               /* can be NULL */
  CGLContextObj         cglContext,
  CGLPixelFormatObj     cglPixelFormat,
  CFDictionaryRef       attributes,              /* can be NULL */
  QTVisualContextRef *  newTextureContext);


/*
 *  QTPixelBufferContextCreate()
 *  
 *  Summary:
 *    Creates a new pixel buffer context with the given attributes.
 *  
 *  Discussion:
 *    This function will fail if the graphics hardware is insufficient.
 *  
 *  Parameters:
 *    
 *    allocator:
 *      [in]  Allocator used to create the pixel buffer context.
 *    
 *    attributes:
 *      [in]  Dictionary of attributes.
 *    
 *    newPixelBufferContext:
 *      [out] Points to a variable to recieve the new pixel buffer
 *      context.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.4 (or QuickTime 7.0) and later
 */
EXTERN_API_C( OSStatus )
QTPixelBufferContextCreate(
  CFAllocatorRef        allocator,                   /* can be NULL */
  CFDictionaryRef       attributes,                  /* can be NULL */
  QTVisualContextRef *  newPixelBufferContext);


#endif  /* TARGET_OS_MAC */

#if TARGET_OS_WIN32
#if CALL_NOT_IN_CARBON
/*
 *  QTDirect3DTextureContextCreate()
 *  
 *  Summary:
 *    Creates a new Direct3D texture context for the given Direct3D
 *    device and pixel format.
 *  
 *  Discussion:
 *    This function will fail if the graphics hardware is insufficient.
 *  
 *  Parameters:
 *    
 *    allocator:
 *      [in]  Allocator used to create the texture context.
 *    
 *    d3dDevice:
 *      [in]  Direct3D device used to create textures.
 *    
 *    d3dPixelFormat:
 *      [in]  Direct3D pixel format used to create the Direct3D device.
 *    
 *    attributes:
 *      [in]  Dictionary of attributes.
 *    
 *    newTextureContext:
 *      [out] Points to a variable to recieve the new Direct3D texture
 *      context.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in qtmlClient.lib 6.6 and later
 */
EXTERN_API_C( OSStatus )
QTDirect3DTextureContextCreate(
  CFAllocatorRef        allocator,               /* can be NULL */
  void *                d3dDevice,
  UInt32                d3dPixelFormat,
  CFDictionaryRef       attributes,              /* can be NULL */
  QTVisualContextRef *  newTextureContext);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_OS_WIN32 */


#endif // !__LP64__



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

#endif /* __IMAGECOMPRESSION__ */

