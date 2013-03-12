/*
     File:       Quickdraw.h
 
     Contains:   Interface to Quickdraw Graphics
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-2003 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __QUICKDRAW__
#define __QUICKDRAW__

#ifndef __COMPONENTS__
#include <Components.h>
#endif

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

#ifndef __QUICKDRAWTEXT__
#include <QuickdrawText.h>
#endif

#ifndef __CGCONTEXT__
#include <CGContext.h>
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

enum {
  invalColReq                   = -1    /*invalid color table request*/
};

enum {
                                        /* transfer modes */
  srcCopy                       = 0,    /*the 16 transfer modes*/
  srcOr                         = 1,
  srcXor                        = 2,
  srcBic                        = 3,
  notSrcCopy                    = 4,
  notSrcOr                      = 5,
  notSrcXor                     = 6,
  notSrcBic                     = 7,
  patCopy                       = 8,
  patOr                         = 9,
  patXor                        = 10,
  patBic                        = 11,
  notPatCopy                    = 12,
  notPatOr                      = 13,
  notPatXor                     = 14,
  notPatBic                     = 15,   /* Special Text Transfer Mode */
  grayishTextOr                 = 49,
  hilitetransfermode            = 50,
  hilite                        = 50,   /* Arithmetic transfer modes */
  blend                         = 32,
  addPin                        = 33,
  addOver                       = 34,
  subPin                        = 35,
  addMax                        = 37,
  adMax                         = 37,
  subOver                       = 38,
  adMin                         = 39,
  ditherCopy                    = 64,   /* Transparent mode constant */
  transparent                   = 36
};

enum {
  italicBit                     = 1,
  ulineBit                      = 2,
  outlineBit                    = 3,
  shadowBit                     = 4,
  condenseBit                   = 5,
  extendBit                     = 6
};

enum {
                                        /* QuickDraw color separation constants */
  normalBit                     = 0,    /*normal screen mapping*/
  inverseBit                    = 1,    /*inverse screen mapping*/
  redBit                        = 4,    /*RGB additive mapping*/
  greenBit                      = 3,
  blueBit                       = 2,
  cyanBit                       = 8,    /*CMYBk subtractive mapping*/
  magentaBit                    = 7,
  yellowBit                     = 6,
  blackBit                      = 5
};

enum {
  blackColor                    = 33,   /*colors expressed in these mappings*/
  whiteColor                    = 30,
  redColor                      = 205,
  greenColor                    = 341,
  blueColor                     = 409,
  cyanColor                     = 273,
  magentaColor                  = 137,
  yellowColor                   = 69
};

enum {
  picLParen                     = 0,    /*standard picture comments*/
  picRParen                     = 1,
  clutType                      = 0,    /*0 if lookup table*/
  fixedType                     = 1,    /*1 if fixed table*/
  directType                    = 2,    /*2 if direct values*/
  gdDevType                     = 0     /*0 = monochrome 1 = color*/
};

enum {
  interlacedDevice              = 2,    /* 1 if single pixel lines look bad */
  hwMirroredDevice              = 4,    /* 1 if device is HW mirrored */
  roundedDevice                 = 5,    /* 1 if device has been "rounded" into the GrayRgn */
  hasAuxMenuBar                 = 6,    /* 1 if device has an aux menu bar on it */
  burstDevice                   = 7,
  ext32Device                   = 8,
  ramInit                       = 10,   /*1 if initialized from 'scrn' resource*/
  mainScreen                    = 11,   /* 1 if main screen */
  allInit                       = 12,   /* 1 if all devices initialized */
  screenDevice                  = 13,   /*1 if screen device [not used]*/
  noDriver                      = 14,   /* 1 if no driver for this GDevice */
  screenActive                  = 15,   /*1 if in use*/
  hiliteBit                     = 7,    /*flag bit in LMGet/SetHiliteMode*/
  pHiliteBit                    = 0,    /*flag bit in LMGet/SetHiliteMode when used with BitClr*/
  defQDColors                   = 127,  /*resource ID of clut for default QDColors*/
                                        /* pixel type */
  RGBDirect                     = 16,   /* 16 & 32 bits/pixel pixelType value */
                                        /* pmVersion values */
  baseAddr32                    = 4     /*pixmap base address is 32-bit address*/
};


enum {
  sysPatListID                  = 0,
  iBeamCursor                   = 1,
  crossCursor                   = 2,
  plusCursor                    = 3,
  watchCursor                   = 4
};

enum {
  kQDGrafVerbFrame              = 0,
  kQDGrafVerbPaint              = 1,
  kQDGrafVerbErase              = 2,
  kQDGrafVerbInvert             = 3,
  kQDGrafVerbFill               = 4
};

#if OLDROUTINENAMES
enum {
  frame                         = kQDGrafVerbFrame,
  paint                         = kQDGrafVerbPaint,
  erase                         = kQDGrafVerbErase,
  invert                        = kQDGrafVerbInvert,
  fill                          = kQDGrafVerbFill
};

#endif  /* OLDROUTINENAMES */

typedef SInt8                           GrafVerb;
enum {
  chunky                        = 0,
  chunkyPlanar                  = 1,
  planar                        = 2
};

typedef SInt8                           PixelType;
typedef short                           Bits16[16];

/***************   IMPORTANT NOTE REGARDING Pattern  **************************************
   Patterns were originally defined as:
   
        C:          typedef unsigned char Pattern[8];
        Pascal:     Pattern = PACKED ARRAY [0..7] OF 0..255;
        
   The old array definition of Pattern would cause 68000 based CPU's to crash in certain circum-
   stances. The new struct definition is safe, but may require source code changes to compile.
    
*********************************************************************************************/
struct Pattern {
  UInt8               pat[8];
};
typedef struct Pattern                  Pattern;
/*
 ConstPatternParam is no longer needed.  It was first created when Pattern was an array.
 Now that Pattern is a struct, it is more straight forward to just add the "const" qualifier
 on the parameter type (e.g. "const Pattern * pat" instead of "ConstPatternParam pat").
*/
typedef const Pattern *                 ConstPatternParam;
typedef Pattern *                       PatPtr;
typedef PatPtr *                        PatHandle;
typedef SignedByte                      QDByte;
typedef QDByte *                        QDPtr;
typedef QDPtr *                         QDHandle;
typedef short                           QDErr;
enum {
  singleDevicesBit              = 0,
  dontMatchSeedsBit             = 1,
  allDevicesBit                 = 2
};

enum {
  singleDevices                 = 1 << singleDevicesBit,
  dontMatchSeeds                = 1 << dontMatchSeedsBit,
  allDevices                    = 1 << allDevicesBit
};

typedef unsigned long                   DeviceLoopFlags;
/*
    PrinterStatusOpcode.  For communication with downloading and printing services.
*/
typedef SInt32                          PrinterStatusOpcode;
enum {
  kPrinterFontStatus            = 0,
  kPrinterScalingStatus         = 1
};

struct PrinterFontStatus {
  SInt32              oResult;
  SInt16              iFondID;
  Style               iStyle;
};
typedef struct PrinterFontStatus        PrinterFontStatus;
struct PrinterScalingStatus {
  Point               oScalingFactors;
};
typedef struct PrinterScalingStatus     PrinterScalingStatus;
struct BitMap {
  Ptr                 baseAddr;
  short               rowBytes;
  Rect                bounds;
};
typedef struct BitMap                   BitMap;
typedef BitMap *                        BitMapPtr;
typedef BitMapPtr *                     BitMapHandle;
struct Cursor {
  Bits16              data;
  Bits16              mask;
  Point               hotSpot;
};
typedef struct Cursor                   Cursor;
typedef Cursor *                        CursPtr;
typedef CursPtr *                       CursHandle;
struct PenState {
  Point               pnLoc;
  Point               pnSize;
  short               pnMode;
  Pattern             pnPat;
};
typedef struct PenState                 PenState;
#if !OPAQUE_TOOLBOX_STRUCTS
struct MacRegion {
  unsigned short      rgnSize;                /* size in bytes; don't rely on it */
  Rect                rgnBBox;                /* enclosing rectangle; in Carbon use GetRegionBounds */
};
typedef struct MacRegion                MacRegion;
typedef MacRegion *                     RgnPtr;
typedef RgnPtr *                        RgnHandle;
#else
typedef struct OpaqueRgnHandle*         RgnHandle;
#endif  /* !OPAQUE_TOOLBOX_STRUCTS */

struct Picture {
  short               picSize;
  Rect                picFrame;
};
typedef struct Picture                  Picture;
typedef Picture *                       PicPtr;
typedef PicPtr *                        PicHandle;
struct MacPolygon {
  short               polySize;
  Rect                polyBBox;
  Point               polyPoints[1];
};
typedef struct MacPolygon               MacPolygon;
typedef MacPolygon *                    PolyPtr;
typedef PolyPtr *                       PolyHandle;
typedef CALLBACK_API( void , QDTextProcPtr )(short byteCount, const void *textBuf, Point numer, Point denom);
typedef CALLBACK_API( void , QDLineProcPtr )(Point newPt);
typedef CALLBACK_API( void , QDRectProcPtr )(GrafVerb verb, const Rect *r);
typedef CALLBACK_API( void , QDRRectProcPtr )(GrafVerb verb, const Rect *r, short ovalWidth, short ovalHeight);
typedef CALLBACK_API( void , QDOvalProcPtr )(GrafVerb verb, const Rect *r);
typedef CALLBACK_API( void , QDArcProcPtr )(GrafVerb verb, const Rect *r, short startAngle, short arcAngle);
typedef CALLBACK_API( void , QDPolyProcPtr )(GrafVerb verb, PolyHandle poly);
typedef CALLBACK_API( void , QDRgnProcPtr )(GrafVerb verb, RgnHandle rgn);
typedef CALLBACK_API( void , QDBitsProcPtr )(const BitMap *srcBits, const Rect *srcRect, const Rect *dstRect, short mode, RgnHandle maskRgn);
typedef CALLBACK_API( void , QDCommentProcPtr )(short kind, short dataSize, Handle dataHandle);
typedef CALLBACK_API( short , QDTxMeasProcPtr )(short byteCount, const void *textAddr, Point *numer, Point *denom, FontInfo *info);
typedef CALLBACK_API( void , QDGetPicProcPtr )(void *dataPtr, short byteCount);
typedef CALLBACK_API( void , QDPutPicProcPtr )(const void *dataPtr, short byteCount);
typedef CALLBACK_API( void , QDOpcodeProcPtr )(const Rect *fromRect, const Rect *toRect, UInt16 opcode, SInt16 version);
typedef CALLBACK_API_C( OSStatus , QDStdGlyphsProcPtr )(void *dataStream, ByteCount size);
typedef CALLBACK_API( void , QDJShieldCursorProcPtr )(short left, short top, short right, short bottom);
typedef STACK_UPP_TYPE(QDTextProcPtr)                           QDTextUPP;
typedef STACK_UPP_TYPE(QDLineProcPtr)                           QDLineUPP;
typedef STACK_UPP_TYPE(QDRectProcPtr)                           QDRectUPP;
typedef STACK_UPP_TYPE(QDRRectProcPtr)                          QDRRectUPP;
typedef STACK_UPP_TYPE(QDOvalProcPtr)                           QDOvalUPP;
typedef STACK_UPP_TYPE(QDArcProcPtr)                            QDArcUPP;
typedef STACK_UPP_TYPE(QDPolyProcPtr)                           QDPolyUPP;
typedef STACK_UPP_TYPE(QDRgnProcPtr)                            QDRgnUPP;
typedef STACK_UPP_TYPE(QDBitsProcPtr)                           QDBitsUPP;
typedef STACK_UPP_TYPE(QDCommentProcPtr)                        QDCommentUPP;
typedef STACK_UPP_TYPE(QDTxMeasProcPtr)                         QDTxMeasUPP;
typedef STACK_UPP_TYPE(QDGetPicProcPtr)                         QDGetPicUPP;
typedef STACK_UPP_TYPE(QDPutPicProcPtr)                         QDPutPicUPP;
typedef STACK_UPP_TYPE(QDOpcodeProcPtr)                         QDOpcodeUPP;
typedef STACK_UPP_TYPE(QDStdGlyphsProcPtr)                      QDStdGlyphsUPP;
typedef STACK_UPP_TYPE(QDJShieldCursorProcPtr)                  QDJShieldCursorUPP;
struct QDProcs {
  QDTextUPP           textProc;
  QDLineUPP           lineProc;
  QDRectUPP           rectProc;
  QDRRectUPP          rRectProc;
  QDOvalUPP           ovalProc;
  QDArcUPP            arcProc;
  QDPolyUPP           polyProc;
  QDRgnUPP            rgnProc;
  QDBitsUPP           bitsProc;
  QDCommentUPP        commentProc;
  QDTxMeasUPP         txMeasProc;
  QDGetPicUPP         getPicProc;
  QDPutPicUPP         putPicProc;
};
typedef struct QDProcs                  QDProcs;
typedef QDProcs *                       QDProcsPtr;
/*
 *  NewQDTextUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDTextUPP )
NewQDTextUPP(QDTextProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDTextProcInfo = 0x00003F80 };  /* pascal no_return_value Func(2_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDTextUPP) NewQDTextUPP(QDTextProcPtr userRoutine) { return (QDTextUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDTextProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDTextUPP(userRoutine) (QDTextUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDTextProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDLineUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDLineUPP )
NewQDLineUPP(QDLineProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDLineProcInfo = 0x000000C0 };  /* pascal no_return_value Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDLineUPP) NewQDLineUPP(QDLineProcPtr userRoutine) { return (QDLineUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDLineProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDLineUPP(userRoutine) (QDLineUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDLineProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDRectUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDRectUPP )
NewQDRectUPP(QDRectProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDRectProcInfo = 0x00000340 };  /* pascal no_return_value Func(1_byte, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDRectUPP) NewQDRectUPP(QDRectProcPtr userRoutine) { return (QDRectUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDRectProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDRectUPP(userRoutine) (QDRectUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDRectProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDRRectUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDRRectUPP )
NewQDRRectUPP(QDRRectProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDRRectProcInfo = 0x00002B40 };  /* pascal no_return_value Func(1_byte, 4_bytes, 2_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDRRectUPP) NewQDRRectUPP(QDRRectProcPtr userRoutine) { return (QDRRectUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDRRectProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDRRectUPP(userRoutine) (QDRRectUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDRRectProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDOvalUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDOvalUPP )
NewQDOvalUPP(QDOvalProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDOvalProcInfo = 0x00000340 };  /* pascal no_return_value Func(1_byte, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDOvalUPP) NewQDOvalUPP(QDOvalProcPtr userRoutine) { return (QDOvalUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDOvalProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDOvalUPP(userRoutine) (QDOvalUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDOvalProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDArcUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDArcUPP )
NewQDArcUPP(QDArcProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDArcProcInfo = 0x00002B40 };  /* pascal no_return_value Func(1_byte, 4_bytes, 2_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDArcUPP) NewQDArcUPP(QDArcProcPtr userRoutine) { return (QDArcUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDArcProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDArcUPP(userRoutine) (QDArcUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDArcProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDPolyUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDPolyUPP )
NewQDPolyUPP(QDPolyProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDPolyProcInfo = 0x00000340 };  /* pascal no_return_value Func(1_byte, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDPolyUPP) NewQDPolyUPP(QDPolyProcPtr userRoutine) { return (QDPolyUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDPolyProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDPolyUPP(userRoutine) (QDPolyUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDPolyProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDRgnUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDRgnUPP )
NewQDRgnUPP(QDRgnProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDRgnProcInfo = 0x00000340 };  /* pascal no_return_value Func(1_byte, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDRgnUPP) NewQDRgnUPP(QDRgnProcPtr userRoutine) { return (QDRgnUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDRgnProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDRgnUPP(userRoutine) (QDRgnUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDRgnProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDBitsUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDBitsUPP )
NewQDBitsUPP(QDBitsProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDBitsProcInfo = 0x0000EFC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes, 2_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDBitsUPP) NewQDBitsUPP(QDBitsProcPtr userRoutine) { return (QDBitsUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDBitsProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDBitsUPP(userRoutine) (QDBitsUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDBitsProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDCommentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDCommentUPP )
NewQDCommentUPP(QDCommentProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDCommentProcInfo = 0x00000E80 };  /* pascal no_return_value Func(2_bytes, 2_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDCommentUPP) NewQDCommentUPP(QDCommentProcPtr userRoutine) { return (QDCommentUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDCommentProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDCommentUPP(userRoutine) (QDCommentUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDCommentProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDTxMeasUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDTxMeasUPP )
NewQDTxMeasUPP(QDTxMeasProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDTxMeasProcInfo = 0x0000FFA0 };  /* pascal 2_bytes Func(2_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDTxMeasUPP) NewQDTxMeasUPP(QDTxMeasProcPtr userRoutine) { return (QDTxMeasUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDTxMeasProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDTxMeasUPP(userRoutine) (QDTxMeasUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDTxMeasProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDGetPicUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDGetPicUPP )
NewQDGetPicUPP(QDGetPicProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDGetPicProcInfo = 0x000002C0 };  /* pascal no_return_value Func(4_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDGetPicUPP) NewQDGetPicUPP(QDGetPicProcPtr userRoutine) { return (QDGetPicUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDGetPicProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDGetPicUPP(userRoutine) (QDGetPicUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDGetPicProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDPutPicUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDPutPicUPP )
NewQDPutPicUPP(QDPutPicProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDPutPicProcInfo = 0x000002C0 };  /* pascal no_return_value Func(4_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDPutPicUPP) NewQDPutPicUPP(QDPutPicProcPtr userRoutine) { return (QDPutPicUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDPutPicProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDPutPicUPP(userRoutine) (QDPutPicUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDPutPicProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDOpcodeUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDOpcodeUPP )
NewQDOpcodeUPP(QDOpcodeProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDOpcodeProcInfo = 0x00002BC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 2_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDOpcodeUPP) NewQDOpcodeUPP(QDOpcodeProcPtr userRoutine) { return (QDOpcodeUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDOpcodeProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDOpcodeUPP(userRoutine) (QDOpcodeUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDOpcodeProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDStdGlyphsUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDStdGlyphsUPP )
NewQDStdGlyphsUPP(QDStdGlyphsProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDStdGlyphsProcInfo = 0x000003F1 };  /* 4_bytes Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDStdGlyphsUPP) NewQDStdGlyphsUPP(QDStdGlyphsProcPtr userRoutine) { return (QDStdGlyphsUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDStdGlyphsProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDStdGlyphsUPP(userRoutine) (QDStdGlyphsUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDStdGlyphsProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQDJShieldCursorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QDJShieldCursorUPP )
NewQDJShieldCursorUPP(QDJShieldCursorProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDJShieldCursorProcInfo = 0x00002A80 };  /* pascal no_return_value Func(2_bytes, 2_bytes, 2_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDJShieldCursorUPP) NewQDJShieldCursorUPP(QDJShieldCursorProcPtr userRoutine) { return (QDJShieldCursorUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDJShieldCursorProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDJShieldCursorUPP(userRoutine) (QDJShieldCursorUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDJShieldCursorProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeQDTextUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDTextUPP(QDTextUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDTextUPP(QDTextUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDTextUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDLineUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDLineUPP(QDLineUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDLineUPP(QDLineUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDLineUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDRectUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDRectUPP(QDRectUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDRectUPP(QDRectUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDRectUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDRRectUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDRRectUPP(QDRRectUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDRRectUPP(QDRRectUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDRRectUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDOvalUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDOvalUPP(QDOvalUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDOvalUPP(QDOvalUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDOvalUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDArcUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDArcUPP(QDArcUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDArcUPP(QDArcUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDArcUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDPolyUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDPolyUPP(QDPolyUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDPolyUPP(QDPolyUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDPolyUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDRgnUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDRgnUPP(QDRgnUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDRgnUPP(QDRgnUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDRgnUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDBitsUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDBitsUPP(QDBitsUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDBitsUPP(QDBitsUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDBitsUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDCommentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDCommentUPP(QDCommentUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDCommentUPP(QDCommentUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDCommentUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDTxMeasUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDTxMeasUPP(QDTxMeasUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDTxMeasUPP(QDTxMeasUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDTxMeasUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDGetPicUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDGetPicUPP(QDGetPicUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDGetPicUPP(QDGetPicUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDGetPicUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDPutPicUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDPutPicUPP(QDPutPicUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDPutPicUPP(QDPutPicUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDPutPicUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDOpcodeUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDOpcodeUPP(QDOpcodeUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDOpcodeUPP(QDOpcodeUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDOpcodeUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDStdGlyphsUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDStdGlyphsUPP(QDStdGlyphsUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDStdGlyphsUPP(QDStdGlyphsUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDStdGlyphsUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQDJShieldCursorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQDJShieldCursorUPP(QDJShieldCursorUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDJShieldCursorUPP(QDJShieldCursorUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDJShieldCursorUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeQDTextUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDTextUPP(
  short         byteCount,
  const void *  textBuf,
  Point         numer,
  Point         denom,
  QDTextUPP     userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDTextUPP(short byteCount, const void * textBuf, Point numer, Point denom, QDTextUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppQDTextProcInfo, byteCount, textBuf, numer, denom); }
  #else
    #define InvokeQDTextUPP(byteCount, textBuf, numer, denom, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppQDTextProcInfo, (byteCount), (textBuf), (numer), (denom))
  #endif
#endif

/*
 *  InvokeQDLineUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDLineUPP(
  Point      newPt,
  QDLineUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDLineUPP(Point newPt, QDLineUPP userUPP) { CALL_ONE_PARAMETER_UPP(userUPP, uppQDLineProcInfo, newPt); }
  #else
    #define InvokeQDLineUPP(newPt, userUPP) CALL_ONE_PARAMETER_UPP((userUPP), uppQDLineProcInfo, (newPt))
  #endif
#endif

/*
 *  InvokeQDRectUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDRectUPP(
  GrafVerb      verb,
  const Rect *  r,
  QDRectUPP     userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDRectUPP(GrafVerb verb, const Rect * r, QDRectUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppQDRectProcInfo, verb, r); }
  #else
    #define InvokeQDRectUPP(verb, r, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppQDRectProcInfo, (verb), (r))
  #endif
#endif

/*
 *  InvokeQDRRectUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDRRectUPP(
  GrafVerb      verb,
  const Rect *  r,
  short         ovalWidth,
  short         ovalHeight,
  QDRRectUPP    userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDRRectUPP(GrafVerb verb, const Rect * r, short ovalWidth, short ovalHeight, QDRRectUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppQDRRectProcInfo, verb, r, ovalWidth, ovalHeight); }
  #else
    #define InvokeQDRRectUPP(verb, r, ovalWidth, ovalHeight, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppQDRRectProcInfo, (verb), (r), (ovalWidth), (ovalHeight))
  #endif
#endif

/*
 *  InvokeQDOvalUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDOvalUPP(
  GrafVerb      verb,
  const Rect *  r,
  QDOvalUPP     userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDOvalUPP(GrafVerb verb, const Rect * r, QDOvalUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppQDOvalProcInfo, verb, r); }
  #else
    #define InvokeQDOvalUPP(verb, r, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppQDOvalProcInfo, (verb), (r))
  #endif
#endif

/*
 *  InvokeQDArcUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDArcUPP(
  GrafVerb      verb,
  const Rect *  r,
  short         startAngle,
  short         arcAngle,
  QDArcUPP      userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDArcUPP(GrafVerb verb, const Rect * r, short startAngle, short arcAngle, QDArcUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppQDArcProcInfo, verb, r, startAngle, arcAngle); }
  #else
    #define InvokeQDArcUPP(verb, r, startAngle, arcAngle, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppQDArcProcInfo, (verb), (r), (startAngle), (arcAngle))
  #endif
#endif

/*
 *  InvokeQDPolyUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDPolyUPP(
  GrafVerb    verb,
  PolyHandle  poly,
  QDPolyUPP   userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDPolyUPP(GrafVerb verb, PolyHandle poly, QDPolyUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppQDPolyProcInfo, verb, poly); }
  #else
    #define InvokeQDPolyUPP(verb, poly, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppQDPolyProcInfo, (verb), (poly))
  #endif
#endif

/*
 *  InvokeQDRgnUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDRgnUPP(
  GrafVerb   verb,
  RgnHandle  rgn,
  QDRgnUPP   userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDRgnUPP(GrafVerb verb, RgnHandle rgn, QDRgnUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppQDRgnProcInfo, verb, rgn); }
  #else
    #define InvokeQDRgnUPP(verb, rgn, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppQDRgnProcInfo, (verb), (rgn))
  #endif
#endif

/*
 *  InvokeQDBitsUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDBitsUPP(
  const BitMap *  srcBits,
  const Rect *    srcRect,
  const Rect *    dstRect,
  short           mode,
  RgnHandle       maskRgn,
  QDBitsUPP       userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDBitsUPP(const BitMap * srcBits, const Rect * srcRect, const Rect * dstRect, short mode, RgnHandle maskRgn, QDBitsUPP userUPP) { CALL_FIVE_PARAMETER_UPP(userUPP, uppQDBitsProcInfo, srcBits, srcRect, dstRect, mode, maskRgn); }
  #else
    #define InvokeQDBitsUPP(srcBits, srcRect, dstRect, mode, maskRgn, userUPP) CALL_FIVE_PARAMETER_UPP((userUPP), uppQDBitsProcInfo, (srcBits), (srcRect), (dstRect), (mode), (maskRgn))
  #endif
#endif

/*
 *  InvokeQDCommentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDCommentUPP(
  short         kind,
  short         dataSize,
  Handle        dataHandle,
  QDCommentUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDCommentUPP(short kind, short dataSize, Handle dataHandle, QDCommentUPP userUPP) { CALL_THREE_PARAMETER_UPP(userUPP, uppQDCommentProcInfo, kind, dataSize, dataHandle); }
  #else
    #define InvokeQDCommentUPP(kind, dataSize, dataHandle, userUPP) CALL_THREE_PARAMETER_UPP((userUPP), uppQDCommentProcInfo, (kind), (dataSize), (dataHandle))
  #endif
#endif

/*
 *  InvokeQDTxMeasUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( short )
InvokeQDTxMeasUPP(
  short         byteCount,
  const void *  textAddr,
  Point *       numer,
  Point *       denom,
  FontInfo *    info,
  QDTxMeasUPP   userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(short) InvokeQDTxMeasUPP(short byteCount, const void * textAddr, Point * numer, Point * denom, FontInfo * info, QDTxMeasUPP userUPP) { return (short)CALL_FIVE_PARAMETER_UPP(userUPP, uppQDTxMeasProcInfo, byteCount, textAddr, numer, denom, info); }
  #else
    #define InvokeQDTxMeasUPP(byteCount, textAddr, numer, denom, info, userUPP) (short)CALL_FIVE_PARAMETER_UPP((userUPP), uppQDTxMeasProcInfo, (byteCount), (textAddr), (numer), (denom), (info))
  #endif
#endif

/*
 *  InvokeQDGetPicUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDGetPicUPP(
  void *       dataPtr,
  short        byteCount,
  QDGetPicUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDGetPicUPP(void * dataPtr, short byteCount, QDGetPicUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppQDGetPicProcInfo, dataPtr, byteCount); }
  #else
    #define InvokeQDGetPicUPP(dataPtr, byteCount, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppQDGetPicProcInfo, (dataPtr), (byteCount))
  #endif
#endif

/*
 *  InvokeQDPutPicUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDPutPicUPP(
  const void *  dataPtr,
  short         byteCount,
  QDPutPicUPP   userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDPutPicUPP(const void * dataPtr, short byteCount, QDPutPicUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppQDPutPicProcInfo, dataPtr, byteCount); }
  #else
    #define InvokeQDPutPicUPP(dataPtr, byteCount, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppQDPutPicProcInfo, (dataPtr), (byteCount))
  #endif
#endif

/*
 *  InvokeQDOpcodeUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDOpcodeUPP(
  const Rect *  fromRect,
  const Rect *  toRect,
  UInt16        opcode,
  SInt16        version,
  QDOpcodeUPP   userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDOpcodeUPP(const Rect * fromRect, const Rect * toRect, UInt16 opcode, SInt16 version, QDOpcodeUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppQDOpcodeProcInfo, fromRect, toRect, opcode, version); }
  #else
    #define InvokeQDOpcodeUPP(fromRect, toRect, opcode, version, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppQDOpcodeProcInfo, (fromRect), (toRect), (opcode), (version))
  #endif
#endif

/*
 *  InvokeQDStdGlyphsUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeQDStdGlyphsUPP(
  void *          dataStream,
  ByteCount       size,
  QDStdGlyphsUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeQDStdGlyphsUPP(void * dataStream, ByteCount size, QDStdGlyphsUPP userUPP) { return (OSStatus)CALL_TWO_PARAMETER_UPP(userUPP, uppQDStdGlyphsProcInfo, dataStream, size); }
  #else
    #define InvokeQDStdGlyphsUPP(dataStream, size, userUPP) (OSStatus)CALL_TWO_PARAMETER_UPP((userUPP), uppQDStdGlyphsProcInfo, (dataStream), (size))
  #endif
#endif

/*
 *  InvokeQDJShieldCursorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQDJShieldCursorUPP(
  short               left,
  short               top,
  short               right,
  short               bottom,
  QDJShieldCursorUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQDJShieldCursorUPP(short left, short top, short right, short bottom, QDJShieldCursorUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppQDJShieldCursorProcInfo, left, top, right, bottom); }
  #else
    #define InvokeQDJShieldCursorUPP(left, top, right, bottom, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppQDJShieldCursorProcInfo, (left), (top), (right), (bottom))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewQDTextProc(userRoutine)                          NewQDTextUPP(userRoutine)
    #define NewQDLineProc(userRoutine)                          NewQDLineUPP(userRoutine)
    #define NewQDRectProc(userRoutine)                          NewQDRectUPP(userRoutine)
    #define NewQDRRectProc(userRoutine)                         NewQDRRectUPP(userRoutine)
    #define NewQDOvalProc(userRoutine)                          NewQDOvalUPP(userRoutine)
    #define NewQDArcProc(userRoutine)                           NewQDArcUPP(userRoutine)
    #define NewQDPolyProc(userRoutine)                          NewQDPolyUPP(userRoutine)
    #define NewQDRgnProc(userRoutine)                           NewQDRgnUPP(userRoutine)
    #define NewQDBitsProc(userRoutine)                          NewQDBitsUPP(userRoutine)
    #define NewQDCommentProc(userRoutine)                       NewQDCommentUPP(userRoutine)
    #define NewQDTxMeasProc(userRoutine)                        NewQDTxMeasUPP(userRoutine)
    #define NewQDGetPicProc(userRoutine)                        NewQDGetPicUPP(userRoutine)
    #define NewQDPutPicProc(userRoutine)                        NewQDPutPicUPP(userRoutine)
    #define NewQDOpcodeProc(userRoutine)                        NewQDOpcodeUPP(userRoutine)
    #define NewQDStdGlyphsProc(userRoutine)                     NewQDStdGlyphsUPP(userRoutine)
    #define NewQDJShieldCursorProc(userRoutine)                 NewQDJShieldCursorUPP(userRoutine)
    #define CallQDTextProc(userRoutine, byteCount, textBuf, numer, denom) InvokeQDTextUPP(byteCount, textBuf, numer, denom, userRoutine)
    #define CallQDLineProc(userRoutine, newPt)                  InvokeQDLineUPP(newPt, userRoutine)
    #define CallQDRectProc(userRoutine, verb, r)                InvokeQDRectUPP(verb, r, userRoutine)
    #define CallQDRRectProc(userRoutine, verb, r, ovalWidth, ovalHeight) InvokeQDRRectUPP(verb, r, ovalWidth, ovalHeight, userRoutine)
    #define CallQDOvalProc(userRoutine, verb, r)                InvokeQDOvalUPP(verb, r, userRoutine)
    #define CallQDArcProc(userRoutine, verb, r, startAngle, arcAngle) InvokeQDArcUPP(verb, r, startAngle, arcAngle, userRoutine)
    #define CallQDPolyProc(userRoutine, verb, poly)             InvokeQDPolyUPP(verb, poly, userRoutine)
    #define CallQDRgnProc(userRoutine, verb, rgn)               InvokeQDRgnUPP(verb, rgn, userRoutine)
    #define CallQDBitsProc(userRoutine, srcBits, srcRect, dstRect, mode, maskRgn) InvokeQDBitsUPP(srcBits, srcRect, dstRect, mode, maskRgn, userRoutine)
    #define CallQDCommentProc(userRoutine, kind, dataSize, dataHandle) InvokeQDCommentUPP(kind, dataSize, dataHandle, userRoutine)
    #define CallQDTxMeasProc(userRoutine, byteCount, textAddr, numer, denom, info) InvokeQDTxMeasUPP(byteCount, textAddr, numer, denom, info, userRoutine)
    #define CallQDGetPicProc(userRoutine, dataPtr, byteCount)   InvokeQDGetPicUPP(dataPtr, byteCount, userRoutine)
    #define CallQDPutPicProc(userRoutine, dataPtr, byteCount)   InvokeQDPutPicUPP(dataPtr, byteCount, userRoutine)
    #define CallQDOpcodeProc(userRoutine, fromRect, toRect, opcode, version) InvokeQDOpcodeUPP(fromRect, toRect, opcode, version, userRoutine)
    #define CallQDStdGlyphsProc(userRoutine, dataStream, size)  InvokeQDStdGlyphsUPP(dataStream, size, userRoutine)
    #define CallQDJShieldCursorProc(userRoutine, left, top, right, bottom) InvokeQDJShieldCursorUPP(left, top, right, bottom, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

#if !OPAQUE_TOOLBOX_STRUCTS
struct GrafPort {
  short               device;                 /* not available in Carbon*/
  BitMap              portBits;               /* in Carbon use GetPortBitMapForCopyBits or IsPortColor*/
  Rect                portRect;               /* in Carbon use Get/SetPortBounds*/
  RgnHandle           visRgn;                 /* in Carbon use Get/SetPortVisibleRegion*/
  RgnHandle           clipRgn;                /* in Carbon use Get/SetPortClipRegion*/
  Pattern             bkPat;                  /* not available in Carbon all GrafPorts are CGrafPorts*/
  Pattern             fillPat;                /* not available in Carbon all GrafPorts are CGrafPorts*/
  Point               pnLoc;                  /* in Carbon use GetPortPenLocation or MoveTo*/
  Point               pnSize;                 /* in Carbon use Get/SetPortPenSize*/
  short               pnMode;                 /* in Carbon use Get/SetPortPenMode*/
  Pattern             pnPat;                  /* not available in Carbon all GrafPorts are CGrafPorts*/
  short               pnVis;                  /* in Carbon use GetPortPenVisibility or Show/HidePen*/
  short               txFont;                 /* in Carbon use GetPortTextFont or TextFont*/
  StyleField          txFace;                 /* in Carbon use GetPortTextFace or TextFace*/
                                              /*StyleField occupies 16-bits, but only first 8-bits are used*/
  short               txMode;                 /* in Carbon use GetPortTextMode or TextMode*/
  short               txSize;                 /* in Carbon use GetPortTextSize or TextSize*/
  Fixed               spExtra;                /* in Carbon use GetPortSpExtra or SpaceExtra*/
  long                fgColor;                /* not available in Carbon */
  long                bkColor;                /* not available in Carbon*/
  short               colrBit;                /* not available in Carbon*/
  short               patStretch;             /* not available in Carbon*/
  Handle              picSave;                /* in Carbon use IsPortPictureBeingDefined*/
  Handle              rgnSave;                /* not available in Carbon*/
  Handle              polySave;               /* not available in Carbon*/
  QDProcsPtr          grafProcs;              /* not available in Carbon all GrafPorts are CGrafPorts*/
};
typedef struct GrafPort                 GrafPort;
typedef GrafPort *                      GrafPtr;
/*
 *  This set of definitions "belongs" in Windows.
 *  But, there is a circularity in the headers where Windows includes Controls and
 *  Controls includes Windows. To break the circle, the information
 *  needed by Controls is moved from Windows to Quickdraw.
 */
typedef GrafPtr                         WindowPtr;
typedef WindowPtr                       DialogPtr;
#else
typedef struct OpaqueWindowPtr*         WindowPtr;
typedef struct OpaqueDialogPtr*         DialogPtr;
typedef struct OpaqueGrafPtr*           GrafPtr;
#endif  /* !OPAQUE_TOOLBOX_STRUCTS */

typedef WindowPtr                       WindowRef;
/* DragConstraint constants to pass to DragGray,DragTheRgn, or ConstrainedDragRgn*/
typedef UInt16                          DragConstraint;
enum {
  kNoConstraint                 = 0,
  kVerticalConstraint           = 1,
  kHorizontalConstraint         = 2
};


typedef CALLBACK_API( void , DragGrayRgnProcPtr )(void);
/*
 *  Here ends the list of things that "belong" in Windows.
 */


struct RGBColor {
  unsigned short      red;                    /*magnitude of red component*/
  unsigned short      green;                  /*magnitude of green component*/
  unsigned short      blue;                   /*magnitude of blue component*/
};
typedef struct RGBColor                 RGBColor;
typedef RGBColor *                      RGBColorPtr;
typedef RGBColorPtr *                   RGBColorHdl;
typedef CALLBACK_API( Boolean , ColorSearchProcPtr )(RGBColor *rgb, long *position);
typedef CALLBACK_API( Boolean , ColorComplementProcPtr )(RGBColor * rgb);
typedef STACK_UPP_TYPE(DragGrayRgnProcPtr)                      DragGrayRgnUPP;
typedef STACK_UPP_TYPE(ColorSearchProcPtr)                      ColorSearchUPP;
typedef STACK_UPP_TYPE(ColorComplementProcPtr)                  ColorComplementUPP;
/*
 *  NewDragGrayRgnUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DragGrayRgnUPP )
NewDragGrayRgnUPP(DragGrayRgnProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDragGrayRgnProcInfo = 0x00000000 };  /* pascal no_return_value Func() */
  #ifdef __cplusplus
    inline DEFINE_API_C(DragGrayRgnUPP) NewDragGrayRgnUPP(DragGrayRgnProcPtr userRoutine) { return (DragGrayRgnUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDragGrayRgnProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDragGrayRgnUPP(userRoutine) (DragGrayRgnUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDragGrayRgnProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewColorSearchUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ColorSearchUPP )
NewColorSearchUPP(ColorSearchProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppColorSearchProcInfo = 0x000003D0 };  /* pascal 1_byte Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ColorSearchUPP) NewColorSearchUPP(ColorSearchProcPtr userRoutine) { return (ColorSearchUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppColorSearchProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewColorSearchUPP(userRoutine) (ColorSearchUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppColorSearchProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewColorComplementUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ColorComplementUPP )
NewColorComplementUPP(ColorComplementProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppColorComplementProcInfo = 0x000000D0 };  /* pascal 1_byte Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ColorComplementUPP) NewColorComplementUPP(ColorComplementProcPtr userRoutine) { return (ColorComplementUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppColorComplementProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewColorComplementUPP(userRoutine) (ColorComplementUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppColorComplementProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeDragGrayRgnUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDragGrayRgnUPP(DragGrayRgnUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDragGrayRgnUPP(DragGrayRgnUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDragGrayRgnUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeColorSearchUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeColorSearchUPP(ColorSearchUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeColorSearchUPP(ColorSearchUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeColorSearchUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeColorComplementUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeColorComplementUPP(ColorComplementUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeColorComplementUPP(ColorComplementUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeColorComplementUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeDragGrayRgnUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDragGrayRgnUPP(DragGrayRgnUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDragGrayRgnUPP(DragGrayRgnUPP userUPP) { CALL_ZERO_PARAMETER_UPP(userUPP, uppDragGrayRgnProcInfo); }
  #else
    #define InvokeDragGrayRgnUPP(userUPP) CALL_ZERO_PARAMETER_UPP((userUPP), uppDragGrayRgnProcInfo)
  #endif
#endif

/*
 *  InvokeColorSearchUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeColorSearchUPP(
  RGBColor *      rgb,
  long *          position,
  ColorSearchUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeColorSearchUPP(RGBColor * rgb, long * position, ColorSearchUPP userUPP) { return (Boolean)CALL_TWO_PARAMETER_UPP(userUPP, uppColorSearchProcInfo, rgb, position); }
  #else
    #define InvokeColorSearchUPP(rgb, position, userUPP) (Boolean)CALL_TWO_PARAMETER_UPP((userUPP), uppColorSearchProcInfo, (rgb), (position))
  #endif
#endif

/*
 *  InvokeColorComplementUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeColorComplementUPP(
  RGBColor *          rgb,
  ColorComplementUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeColorComplementUPP(RGBColor * rgb, ColorComplementUPP userUPP) { return (Boolean)CALL_ONE_PARAMETER_UPP(userUPP, uppColorComplementProcInfo, rgb); }
  #else
    #define InvokeColorComplementUPP(rgb, userUPP) (Boolean)CALL_ONE_PARAMETER_UPP((userUPP), uppColorComplementProcInfo, (rgb))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewDragGrayRgnProc(userRoutine)                     NewDragGrayRgnUPP(userRoutine)
    #define NewColorSearchProc(userRoutine)                     NewColorSearchUPP(userRoutine)
    #define NewColorComplementProc(userRoutine)                 NewColorComplementUPP(userRoutine)
    #define CallDragGrayRgnProc(userRoutine)                    InvokeDragGrayRgnUPP(userRoutine)
    #define CallColorSearchProc(userRoutine, rgb, position)     InvokeColorSearchUPP(rgb, position, userRoutine)
    #define CallColorComplementProc(userRoutine, rgb)           InvokeColorComplementUPP(rgb, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

struct ColorSpec {
  short               value;                  /*index or other value*/
  RGBColor            rgb;                    /*true color*/
};
typedef struct ColorSpec                ColorSpec;
typedef ColorSpec *                     ColorSpecPtr;
typedef ColorSpec                       CSpecArray[1];
struct ColorTable {
  long                ctSeed;                 /*unique identifier for table*/
  short               ctFlags;                /*high bit: 0 = PixMap; 1 = device*/
  short               ctSize;                 /*number of entries in CTTable*/
  CSpecArray          ctTable;                /*array [0..0] of ColorSpec*/
};
typedef struct ColorTable               ColorTable;
typedef ColorTable *                    CTabPtr;
typedef CTabPtr *                       CTabHandle;
struct xColorSpec {
  short               value;                  /*index or other value*/
  RGBColor            rgb;                    /*true color*/
  short               xalpha;
};
typedef struct xColorSpec               xColorSpec;
typedef xColorSpec *                    xColorSpecPtr;
typedef xColorSpec                      xCSpecArray[1];
struct MatchRec {
  unsigned short      red;
  unsigned short      green;
  unsigned short      blue;
  long                matchData;
};
typedef struct MatchRec                 MatchRec;
/*
    QuickTime 3.0 makes PixMap data structure available on non-Mac OS's.
    In order to implement PixMap in these alternate environments, the PixMap
    had to be extended. The pmReserved field was changed to pmExt which is
    a Handle to extra info.  The planeBytes field was changed to pixelFormat.
    
    In OS X, Quickdraw also uses the new PixMap data structure.
*/
#ifndef OLDPIXMAPSTRUCT
#if TARGET_OS_MAC && TARGET_API_MAC_OS8
#define OLDPIXMAPSTRUCT 1
#else
#define OLDPIXMAPSTRUCT 0
#endif  /* TARGET_OS_MAC && TARGET_API_MAC_OS8 */

#endif  /* !defined(OLDPIXMAPSTRUCT) */


#if OLDPIXMAPSTRUCT
   #define GETPIXMAPPIXELFORMAT(pm)        ( (pm)->pixelSize )
#else
  #define GETPIXMAPPIXELFORMAT(pm)        ( ((pm)->pixelFormat != 0) ? (pm)->pixelFormat : (pm)->pixelSize )
#endif

/* You MUST remove the TARGET_API_MAC_OS8 because this must be 0 for OSX */

#if TARGET_OS_MAC && !TARGET_RT_LITTLE_ENDIAN
    #define NON_MAC_PIXEL_FORMATS   0
#else
    #define NON_MAC_PIXEL_FORMATS   1
#endif

/* pixel formats*/
enum {
  k1MonochromePixelFormat       = 0x00000001, /* 1 bit indexed*/
  k2IndexedPixelFormat          = 0x00000002, /* 2 bit indexed*/
  k4IndexedPixelFormat          = 0x00000004, /* 4 bit indexed*/
  k8IndexedPixelFormat          = 0x00000008, /* 8 bit indexed*/
  k16BE555PixelFormat           = 0x00000010, /* 16 bit BE rgb 555 (Mac)*/
  k24RGBPixelFormat             = 0x00000018, /* 24 bit rgb */
  k32ARGBPixelFormat            = 0x00000020, /* 32 bit argb    (Mac)*/
  k1IndexedGrayPixelFormat      = 0x00000021, /* 1 bit indexed gray*/
  k2IndexedGrayPixelFormat      = 0x00000022, /* 2 bit indexed gray*/
  k4IndexedGrayPixelFormat      = 0x00000024, /* 4 bit indexed gray*/
  k8IndexedGrayPixelFormat      = 0x00000028 /* 8 bit indexed gray*/
};


/* values for PixMap.pixelFormat*/
enum {
  k16LE555PixelFormat           = FOUR_CHAR_CODE('L555'), /* 16 bit LE rgb 555 (PC)*/
  k16LE5551PixelFormat          = FOUR_CHAR_CODE('5551'), /* 16 bit LE rgb 5551*/
  k16BE565PixelFormat           = FOUR_CHAR_CODE('B565'), /* 16 bit BE rgb 565*/
  k16LE565PixelFormat           = FOUR_CHAR_CODE('L565'), /* 16 bit LE rgb 565*/
  k24BGRPixelFormat             = FOUR_CHAR_CODE('24BG'), /* 24 bit bgr */
  k32BGRAPixelFormat            = FOUR_CHAR_CODE('BGRA'), /* 32 bit bgra    (Matrox)*/
  k32ABGRPixelFormat            = FOUR_CHAR_CODE('ABGR'), /* 32 bit abgr    */
  k32RGBAPixelFormat            = FOUR_CHAR_CODE('RGBA'), /* 32 bit rgba    */
  kYUVSPixelFormat              = FOUR_CHAR_CODE('yuvs'), /* YUV 4:2:2 byte ordering 16-unsigned = 'YUY2'*/
  kYUVUPixelFormat              = FOUR_CHAR_CODE('yuvu'), /* YUV 4:2:2 byte ordering 16-signed*/
  kYVU9PixelFormat              = FOUR_CHAR_CODE('YVU9'), /* YVU9 Planar    9*/
  kYUV411PixelFormat            = FOUR_CHAR_CODE('Y411'), /* YUV 4:1:1 Interleaved  16*/
  kYVYU422PixelFormat           = FOUR_CHAR_CODE('YVYU'), /* YVYU 4:2:2 byte ordering   16*/
  kUYVY422PixelFormat           = FOUR_CHAR_CODE('UYVY'), /* UYVY 4:2:2 byte ordering   16*/
  kYUV211PixelFormat            = FOUR_CHAR_CODE('Y211'), /* YUV 2:1:1 Packed   8*/
  k2vuyPixelFormat              = FOUR_CHAR_CODE('2vuy') /* UYVY 4:2:2 byte ordering   16*/
};


struct PixMap {
  Ptr                 baseAddr;               /*pointer to pixels*/
  short               rowBytes;               /*offset to next line*/
  Rect                bounds;                 /*encloses bitmap*/
  short               pmVersion;              /*pixMap version number*/
  short               packType;               /*defines packing format*/
  long                packSize;               /*length of pixel data*/
  Fixed               hRes;                   /*horiz. resolution (ppi)*/
  Fixed               vRes;                   /*vert. resolution (ppi)*/
  short               pixelType;              /*defines pixel type*/
  short               pixelSize;              /*# bits in pixel*/
  short               cmpCount;               /*# components in pixel*/
  short               cmpSize;                /*# bits per component*/
#if OLDPIXMAPSTRUCT
  long                planeBytes;             /*offset to next plane*/
  CTabHandle          pmTable;                /*color map for this pixMap*/
  long                pmReserved;
#else
  OSType                          pixelFormat;                /*fourCharCode representation*/
    CTabHandle                      pmTable;                    /*color map for this pixMap*/
  void*                           pmExt;                      /*Handle to pixMap extension*/
#endif
};
typedef struct PixMap                   PixMap;
typedef PixMap *                        PixMapPtr;
typedef PixMapPtr *                     PixMapHandle;
struct PixPat {
  short               patType;                /*type of pattern*/
  PixMapHandle        patMap;                 /*the pattern's pixMap*/
  Handle              patData;                /*pixmap's data*/
  Handle              patXData;               /*expanded Pattern data*/
  short               patXValid;              /*flags whether expanded Pattern valid*/
  Handle              patXMap;                /*Handle to expanded Pattern data*/
  Pattern             pat1Data;               /*old-Style pattern/RGB color*/
};
typedef struct PixPat                   PixPat;
typedef PixPat *                        PixPatPtr;
typedef PixPatPtr *                     PixPatHandle;
struct CCrsr {
  short               crsrType;               /*type of cursor*/
  PixMapHandle        crsrMap;                /*the cursor's pixmap*/
  Handle              crsrData;               /*cursor's data*/
  Handle              crsrXData;              /*expanded cursor data*/
  short               crsrXValid;             /*depth of expanded data (0 if none)*/
  Handle              crsrXHandle;            /*future use*/
  Bits16              crsr1Data;              /*one-bit cursor*/
  Bits16              crsrMask;               /*cursor's mask*/
  Point               crsrHotSpot;            /*cursor's hotspot*/
  long                crsrXTable;             /*private*/
  long                crsrID;                 /*private*/
};
typedef struct CCrsr                    CCrsr;
typedef CCrsr *                         CCrsrPtr;
typedef CCrsrPtr *                      CCrsrHandle;
struct GammaTbl {
  short               gVersion;               /*gamma version number*/
  short               gType;                  /*gamma data type*/
  short               gFormulaSize;           /*Formula data size*/
  short               gChanCnt;               /*number of channels of data*/
  short               gDataCnt;               /*number of values/channel*/
  short               gDataWidth;             /*bits/corrected value (data packed to next larger byte size)*/
  short               gFormulaData[1];        /*data for formulas followed by gamma values*/
};
typedef struct GammaTbl                 GammaTbl;
typedef GammaTbl *                      GammaTblPtr;
typedef GammaTblPtr *                   GammaTblHandle;
struct ITab {
  long                iTabSeed;               /*copy of CTSeed from source CTable*/
  short               iTabRes;                /*bits/channel resolution of iTable*/
  Byte                iTTable[1];             /*byte colortable index values*/
};
typedef struct ITab                     ITab;
typedef ITab *                          ITabPtr;
typedef ITabPtr *                       ITabHandle;
struct SProcRec {
  Handle              nxtSrch;                /*SProcHndl Handle to next SProcRec*/
  ColorSearchUPP      srchProc;               /*search procedure proc ptr*/
};
typedef struct SProcRec                 SProcRec;
typedef SProcRec *                      SProcPtr;
typedef SProcPtr *                      SProcHndl;
struct CProcRec {
  Handle              nxtComp;                /*CProcHndl Handle to next CProcRec*/
  ColorComplementUPP  compProc;               /*complement procedure proc ptr*/
};
typedef struct CProcRec                 CProcRec;
typedef CProcRec *                      CProcPtr;
typedef CProcPtr *                      CProcHndl;
/*
    QuickTime 3.0 makes GDevice data structure available on non-Mac OS's.
    In order to implement GDevice in these alternate environments, the GDevice
    had to be extended. The gdReserved field was changed to gdExt which is
    a Handle to extra info.  
*/
#ifndef OLDGDEVICESTRUCT
#if TARGET_OS_MAC && TARGET_API_MAC_OS8
#define OLDGDEVICESTRUCT 1
#else
#define OLDGDEVICESTRUCT 0
#endif  /* TARGET_OS_MAC && TARGET_API_MAC_OS8 */

#endif  /* !defined(OLDGDEVICESTRUCT) */

typedef struct GDevice                  GDevice;
typedef GDevice *                       GDPtr;
typedef GDPtr *                         GDHandle;
struct GDevice {
  short               gdRefNum;               /*driver's unit number*/
  short               gdID;                   /*client ID for search procs*/
  short               gdType;                 /*fixed/CLUT/direct*/
  ITabHandle          gdITable;               /*Handle to inverse lookup table*/
  short               gdResPref;              /*preferred resolution of GDITable*/
  SProcHndl           gdSearchProc;           /*search proc list head*/
  CProcHndl           gdCompProc;             /*complement proc list*/
  short               gdFlags;                /*grafDevice flags word*/
  PixMapHandle        gdPMap;                 /*describing pixMap*/
  long                gdRefCon;               /*reference value*/
  GDHandle            gdNextGD;               /*GDHandle Handle of next gDevice*/
  Rect                gdRect;                 /* device's bounds in global coordinates*/
  long                gdMode;                 /*device's current mode*/
  short               gdCCBytes;              /*depth of expanded cursor data*/
  short               gdCCDepth;              /*depth of expanded cursor data*/
  Handle              gdCCXData;              /*Handle to cursor's expanded data*/
  Handle              gdCCXMask;              /*Handle to cursor's expanded mask*/
#if OLDGDEVICESTRUCT
  long                gdReserved;             /*future use. MUST BE 0*/
#else
  Handle                          gdExt;                      /*QuickTime 3.0 private info*/
#endif
};

struct GrafVars {
  RGBColor            rgbOpColor;             /*color for addPin  subPin and average*/
  RGBColor            rgbHiliteColor;         /*color for hiliting*/
  Handle              pmFgColor;              /*palette Handle for foreground color*/
  short               pmFgIndex;              /*index value for foreground*/
  Handle              pmBkColor;              /*palette Handle for background color*/
  short               pmBkIndex;              /*index value for background*/
  short               pmFlags;                /*flags for Palette Manager*/
};
typedef struct GrafVars                 GrafVars;
typedef GrafVars *                      GVarPtr;
typedef GVarPtr *                       GVarHandle;

#if !OPAQUE_TOOLBOX_STRUCTS
typedef struct CGrafPort                CGrafPort;
typedef CGrafPort *                     CGrafPtr;
#else
typedef GrafPtr                         CGrafPtr;
#endif  /* !OPAQUE_TOOLBOX_STRUCTS */

typedef CALLBACK_API_C( OSStatus , QDPrinterStatusProcPtr )(PrinterStatusOpcode opcode, CGrafPtr currentPort, void *printerStatus);
typedef STACK_UPP_TYPE(QDPrinterStatusProcPtr)                  QDPrinterStatusUPP;

struct CQDProcs {
  QDTextUPP           textProc;
  QDLineUPP           lineProc;
  QDRectUPP           rectProc;
  QDRRectUPP          rRectProc;
  QDOvalUPP           ovalProc;
  QDArcUPP            arcProc;
  QDPolyUPP           polyProc;
  QDRgnUPP            rgnProc;
  QDBitsUPP           bitsProc;
  QDCommentUPP        commentProc;
  QDTxMeasUPP         txMeasProc;
  QDGetPicUPP         getPicProc;
  QDPutPicUPP         putPicProc;
  QDOpcodeUPP         opcodeProc;
  UniversalProcPtr    newProc1;               /* this is the StdPix bottleneck -- see ImageCompression.h */
  QDStdGlyphsUPP      glyphsProc;             /* was newProc2; now used in Unicode text drawing */
  QDPrinterStatusUPP  printerStatusProc;      /* was newProc3;  now used to communicate status between Printing code and System imaging code */
  UniversalProcPtr    newProc4;
  UniversalProcPtr    newProc5;
  UniversalProcPtr    newProc6;
};
typedef struct CQDProcs                 CQDProcs;
typedef CQDProcs *                      CQDProcsPtr;
#if !OPAQUE_TOOLBOX_STRUCTS
struct CGrafPort {
  short               device;                 /* not available in Carbon*/
  PixMapHandle        portPixMap;             /* in Carbon use GetPortPixMap*/
  short               portVersion;            /* in Carbon use IsPortColor*/
  Handle              grafVars;               /* not available in Carbon*/
  short               chExtra;                /* in Carbon use GetPortChExtra*/
  short               pnLocHFrac;             /* in Carbon use Get/SetPortFracHPenLocation*/
  Rect                portRect;               /* in Carbon use Get/SetPortBounds*/
  RgnHandle           visRgn;                 /* in Carbon use Get/SetPortVisibleRegion*/
  RgnHandle           clipRgn;                /* in Carbon use Get/SetPortClipRegion*/
  PixPatHandle        bkPixPat;               /* in Carbon use GetPortBackPixPat or BackPixPat*/
  RGBColor            rgbFgColor;             /* in Carbon use GetPortForeColor or RGBForeColor*/
  RGBColor            rgbBkColor;             /* in Carbon use GetPortBackColor or RGBBackColor*/
  Point               pnLoc;                  /* in Carbon use GetPortPenLocation or MoveTo*/
  Point               pnSize;                 /* in Carbon use Get/SetPortPenSize*/
  short               pnMode;                 /* in Carbon use Get/SetPortPenMode*/
  PixPatHandle        pnPixPat;               /* in Carbon use Get/SetPortPenPixPat*/
  PixPatHandle        fillPixPat;             /* in Carbon use GetPortFillPixPat*/
  short               pnVis;                  /* in Carbon use GetPortPenVisibility or Show/HidePen*/
  short               txFont;                 /* in Carbon use GetPortTextFont or TextFont*/
  StyleField          txFace;                 /* in Carbon use GetPortTextFace or TextFace*/
                                              /*StyleField occupies 16-bits, but only first 8-bits are used*/
  short               txMode;                 /* in Carbon use GetPortTextMode or TextMode*/
  short               txSize;                 /* in Carbon use GetPortTextSize or TextSize*/
  Fixed               spExtra;                /* in Carbon use GetPortSpExtra or SpaceExtra*/
  long                fgColor;                /* not available in Carbon*/
  long                bkColor;                /* not available in Carbon*/
  short               colrBit;                /* not available in Carbon*/
  short               patStretch;             /* not available in Carbon*/
  Handle              picSave;                /* in Carbon use IsPortPictureBeingDefined*/
  Handle              rgnSave;                /* in Carbon use IsPortRegionBeingDefined*/
  Handle              polySave;               /* in Carbon use IsPortPolyBeingDefined*/
  CQDProcsPtr         grafProcs;              /* in Carbon use Get/SetPortGrafProcs*/
};

#endif  /* !OPAQUE_TOOLBOX_STRUCTS */

#if OPAQUE_TOOLBOX_STRUCTS
typedef WindowPtr                       CWindowPtr;
#else
typedef CGrafPtr                        CWindowPtr;
#endif  /* OPAQUE_TOOLBOX_STRUCTS */

struct ReqListRec {
  short               reqLSize;               /*request list size*/
  short               reqLData[1];            /*request list data*/
};
typedef struct ReqListRec               ReqListRec;
struct OpenCPicParams {
  Rect                srcRect;
  Fixed               hRes;
  Fixed               vRes;
  short               version;
  short               reserved1;
  long                reserved2;
};
typedef struct OpenCPicParams           OpenCPicParams;
enum {
  kCursorImageMajorVersion      = 0x0001,
  kCursorImageMinorVersion      = 0x0000
};

struct CursorImageRec {
  UInt16              majorVersion;
  UInt16              minorVersion;
  PixMapHandle        cursorPixMap;
  BitMapHandle        cursorBitMask;
};
typedef struct CursorImageRec           CursorImageRec;
typedef CursorImageRec *                CursorImagePtr;
typedef CALLBACK_API( void , DeviceLoopDrawingProcPtr )(short depth, short deviceFlags, GDHandle targetDevice, long userData);
typedef STACK_UPP_TYPE(DeviceLoopDrawingProcPtr)                DeviceLoopDrawingUPP;
#if CALL_NOT_IN_CARBON
/*
 *  NewQDPrinterStatusUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( QDPrinterStatusUPP )
NewQDPrinterStatusUPP(QDPrinterStatusProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQDPrinterStatusProcInfo = 0x00000FF1 };  /* 4_bytes Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QDPrinterStatusUPP) NewQDPrinterStatusUPP(QDPrinterStatusProcPtr userRoutine) { return (QDPrinterStatusUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDPrinterStatusProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQDPrinterStatusUPP(userRoutine) (QDPrinterStatusUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQDPrinterStatusProcInfo, GetCurrentArchitecture())
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

/*
 *  NewDeviceLoopDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DeviceLoopDrawingUPP )
NewDeviceLoopDrawingUPP(DeviceLoopDrawingProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDeviceLoopDrawingProcInfo = 0x00003E80 };  /* pascal no_return_value Func(2_bytes, 2_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DeviceLoopDrawingUPP) NewDeviceLoopDrawingUPP(DeviceLoopDrawingProcPtr userRoutine) { return (DeviceLoopDrawingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDeviceLoopDrawingProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDeviceLoopDrawingUPP(userRoutine) (DeviceLoopDrawingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDeviceLoopDrawingProcInfo, GetCurrentArchitecture())
  #endif
#endif

#if CALL_NOT_IN_CARBON
/*
 *  DisposeQDPrinterStatusUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeQDPrinterStatusUPP(QDPrinterStatusUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQDPrinterStatusUPP(QDPrinterStatusUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQDPrinterStatusUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

/*
 *  DisposeDeviceLoopDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDeviceLoopDrawingUPP(DeviceLoopDrawingUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDeviceLoopDrawingUPP(DeviceLoopDrawingUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDeviceLoopDrawingUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

#if CALL_NOT_IN_CARBON
/*
 *  InvokeQDPrinterStatusUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
InvokeQDPrinterStatusUPP(
  PrinterStatusOpcode  opcode,
  CGrafPtr             currentPort,
  void *               printerStatus,
  QDPrinterStatusUPP   userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeQDPrinterStatusUPP(PrinterStatusOpcode opcode, CGrafPtr currentPort, void * printerStatus, QDPrinterStatusUPP userUPP) { return (OSStatus)CALL_THREE_PARAMETER_UPP(userUPP, uppQDPrinterStatusProcInfo, opcode, currentPort, printerStatus); }
  #else
    #define InvokeQDPrinterStatusUPP(opcode, currentPort, printerStatus, userUPP) (OSStatus)CALL_THREE_PARAMETER_UPP((userUPP), uppQDPrinterStatusProcInfo, (opcode), (currentPort), (printerStatus))
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

/*
 *  InvokeDeviceLoopDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDeviceLoopDrawingUPP(
  short                 depth,
  short                 deviceFlags,
  GDHandle              targetDevice,
  long                  userData,
  DeviceLoopDrawingUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDeviceLoopDrawingUPP(short depth, short deviceFlags, GDHandle targetDevice, long userData, DeviceLoopDrawingUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppDeviceLoopDrawingProcInfo, depth, deviceFlags, targetDevice, userData); }
  #else
    #define InvokeDeviceLoopDrawingUPP(depth, deviceFlags, targetDevice, userData, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppDeviceLoopDrawingProcInfo, (depth), (deviceFlags), (targetDevice), (userData))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewQDPrinterStatusProc(userRoutine)                 NewQDPrinterStatusUPP(userRoutine)
    #define NewDeviceLoopDrawingProc(userRoutine)               NewDeviceLoopDrawingUPP(userRoutine)
    #define CallQDPrinterStatusProc(userRoutine, opcode, currentPort, printerStatus) InvokeQDPrinterStatusUPP(opcode, currentPort, printerStatus, userRoutine)
    #define CallDeviceLoopDrawingProc(userRoutine, depth, deviceFlags, targetDevice, userData) InvokeDeviceLoopDrawingUPP(depth, deviceFlags, targetDevice, userData, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

#if !OPAQUE_TOOLBOX_STRUCTS || !TARGET_API_MAC_CARBON
struct QDGlobals {
  char                privates[76];
  long                randSeed;               /* in Carbon use GetQDGlobalsRandomSeed*/
  BitMap              screenBits;             /* in Carbon use GetQDGlobalsScreenBits*/
  Cursor              arrow;                  /* in Carbon use GetQDGlobalsArrow*/
  Pattern             dkGray;                 /* in Carbon use GetQDGlobalsDarkGray*/
  Pattern             ltGray;                 /* in Carbon use GetQDGlobalsLightGray*/
  Pattern             gray;                   /* in Carbon use GetQDGlobalsGray*/
  Pattern             black;                  /* in Carbon use GetQDGlobalsBlack*/
  Pattern             white;                  /* in Carbon use GetQDGlobalsWhite*/
  GrafPtr             thePort;                /* in Carbon use GetQDGlobalsThePort*/
};
typedef struct QDGlobals                QDGlobals;
typedef QDGlobals *                     QDGlobalsPtr;
typedef QDGlobalsPtr *                  QDGlobalsHdl;
extern QDGlobals qd;

#endif  /* !OPAQUE_TOOLBOX_STRUCTS || !TARGET_API_MAC_CARBON */

#if CALL_NOT_IN_CARBON
/*
 *  InitGraf()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
InitGraf(void * globalPtr)                                    ONEWORDINLINE(0xA86E);


/*
 *  OpenPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
OpenPort(GrafPtr port)                                        ONEWORDINLINE(0xA86F);


/*
 *  InitPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
InitPort(GrafPtr port)                                        ONEWORDINLINE(0xA86D);


/*
 *  ClosePort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
ClosePort(GrafPtr port)                                       ONEWORDINLINE(0xA87D);


/*
   These are Carbon only routines. They do nothing at all on
   Mac OS 8, but work flawlessly on Mac OS X.
*/
#endif  /* CALL_NOT_IN_CARBON */

/*
 *  LockPortBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
LockPortBits(GrafPtr port);


/*
 *  UnlockPortBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
UnlockPortBits(GrafPtr port);


/* Break a region up into rectangles.*/

enum {
  kQDParseRegionFromTop         = (1 << 0),
  kQDParseRegionFromBottom      = (1 << 1),
  kQDParseRegionFromLeft        = (1 << 2),
  kQDParseRegionFromRight       = (1 << 3),
  kQDParseRegionFromTopLeft     = kQDParseRegionFromTop | kQDParseRegionFromLeft,
  kQDParseRegionFromBottomRight = kQDParseRegionFromBottom | kQDParseRegionFromRight
};

typedef SInt32                          QDRegionParseDirection;
enum {
  kQDRegionToRectsMsgInit       = 1,
  kQDRegionToRectsMsgParse      = 2,
  kQDRegionToRectsMsgTerminate  = 3
};

typedef CALLBACK_API_C( OSStatus , RegionToRectsProcPtr )(UInt16 message, RgnHandle rgn, const Rect *rect, void *refCon);
typedef STACK_UPP_TYPE(RegionToRectsProcPtr)                    RegionToRectsUPP;
/*
 *  NewRegionToRectsUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( RegionToRectsUPP )
NewRegionToRectsUPP(RegionToRectsProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppRegionToRectsProcInfo = 0x00003FB1 };  /* 4_bytes Func(2_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(RegionToRectsUPP) NewRegionToRectsUPP(RegionToRectsProcPtr userRoutine) { return (RegionToRectsUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppRegionToRectsProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewRegionToRectsUPP(userRoutine) (RegionToRectsUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppRegionToRectsProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeRegionToRectsUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeRegionToRectsUPP(RegionToRectsUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeRegionToRectsUPP(RegionToRectsUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeRegionToRectsUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeRegionToRectsUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeRegionToRectsUPP(
  UInt16            message,
  RgnHandle         rgn,
  const Rect *      rect,
  void *            refCon,
  RegionToRectsUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeRegionToRectsUPP(UInt16 message, RgnHandle rgn, const Rect * rect, void * refCon, RegionToRectsUPP userUPP) { return (OSStatus)CALL_FOUR_PARAMETER_UPP(userUPP, uppRegionToRectsProcInfo, message, rgn, rect, refCon); }
  #else
    #define InvokeRegionToRectsUPP(message, rgn, rect, refCon, userUPP) (OSStatus)CALL_FOUR_PARAMETER_UPP((userUPP), uppRegionToRectsProcInfo, (message), (rgn), (rect), (refCon))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewRegionToRectsProc(userRoutine)                   NewRegionToRectsUPP(userRoutine)
    #define CallRegionToRectsProc(userRoutine, message, rgn, rect, refCon) InvokeRegionToRectsUPP(message, rgn, rect, refCon, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*
 *  QDRegionToRects()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
QDRegionToRects(
  RgnHandle                rgn,
  QDRegionParseDirection   dir,
  RegionToRectsUPP         proc,
  void *                   userData);


#if !TARGET_OS_MAC
#if CALL_NOT_IN_CARBON
/*
 *  UpdatePort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
UpdatePort(GrafPtr port);


/*
 *  GetPortNativeWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void * )
GetPortNativeWindow(GrafPtr macPort);


/*
 *  GetNativeWindowPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( GrafPtr )
GetNativeWindowPort(void * nativeWindow);


/*
 *  MacRegionToNativeRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void * )
MacRegionToNativeRegion(RgnHandle macRegion);


/*
 *  NativeRegionToMacRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( RgnHandle )
NativeRegionToMacRegion(void * nativeRegion);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* !TARGET_OS_MAC */

#if TARGET_OS_WIN32
#if CALL_NOT_IN_CARBON
/*
 *  GetPortHWND()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void * )
GetPortHWND(GrafPtr port);


/*
 *  GetHWNDPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( GrafPtr )
GetHWNDPort(void * theHWND);


#define GetPortHWND(port)  (HWND)GetPortNativeWindow(port)
#define GetHWNDPort(theHWND) GetNativeWindowPort(theHWND)
/*
 *  GetPortHDC()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void * )
GetPortHDC(GrafPtr port);


/*
 *  GetPortHBITMAP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void * )
GetPortHBITMAP(GrafPtr port);


/*
 *  GetPortHPALETTE()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void * )
GetPortHPALETTE(GrafPtr port);


/*
 *  GetPortHFONT()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void * )
GetPortHFONT(GrafPtr port);


/*
 *  GetDIBFromPICT()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void * )
GetDIBFromPICT(PicHandle hPict);


/*
 *  GetPICTFromDIB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( PicHandle )
GetPICTFromDIB(void * h);


/*
 *  QTMLFlushDirtyPorts()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
QTMLFlushDirtyPorts(void);


/*
 *  QTMLFlushPortDirtyRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
QTMLFlushPortDirtyRgn(GrafPtr port);


/*
 *  QTMLAddRgnToDirtyRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
QTMLAddRgnToDirtyRgn(
  GrafPtr     port,
  RgnHandle   dirtyRgn);


/*
 *  QTMLGetBackbufferHDC()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
QTMLGetBackbufferHDC(
  GrafPtr   port,
  void *    backbufferHDC);


/*
 *  QTMLReleaseBackbufferHDC()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
QTMLReleaseBackbufferHDC(
  GrafPtr   port,
  void *    backbufferHDC);


/*
 *  QTMLAddRectToDirtyRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
QTMLAddRectToDirtyRgn(
  GrafPtr   port,
  Rect *    dirtyRect);


/*
 *  QTMLAddNativeRgnToDirtyRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
QTMLAddNativeRgnToDirtyRgn(
  GrafPtr   port,
  void *    dirtyHRGN);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_OS_WIN32 */

/*
 *  [Mac]SetPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacSetPort SetPort
#endif
EXTERN_API( void )
MacSetPort(GrafPtr port)                                      ONEWORDINLINE(0xA873);


/*
 *  GetPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetPort(GrafPtr * port)                                       ONEWORDINLINE(0xA874);


/*
 *  QDSwapPort()
 *  
 *  Summary:
 *    Combines a GetPort(&savePort); SetPort(newPort) sequence.
 *  
 *  Discussion:
 *    On X, the GetPort/SetPort calls act on per-thread globals, and
 *    cost more processor cycles than in the past, where they were
 *    simple memory accessors. To optimize, use the QDSwapPort call
 *    which combines both, and returns a Boolean indicating if the port
 *    actually did change. Typical usage: portChanged =
 *    QDSwapPort(newPort, &savePort); // some drawing into newPort if
 *    (portChanged) QDSwapPort(savePort, NULL);
 *  
 *  Parameters:
 *    
 *    inNewPort:
 *      The new port to be set.
 *    
 *    outOldPort:
 *      Receives the previous port. Can be NULL.
 *  
 *  Result:
 *    A Boolean indicating whether the port was changed, i.e.
 *    (inNewPort != *outOldPort)
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( Boolean )
QDSwapPort(
  CGrafPtr    inNewPort,
  CGrafPtr *  outOldPort);


/*
 *  GrafDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GrafDevice(short device)                                      ONEWORDINLINE(0xA872);


/*
 *  SetPortBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortBits(const BitMap * bm)                                ONEWORDINLINE(0xA875);


/*
 *  PortSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PortSize(
  short   width,
  short   height)                                             ONEWORDINLINE(0xA876);


/*
 *  MovePortTo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
MovePortTo(
  short   leftGlobal,
  short   topGlobal)                                          ONEWORDINLINE(0xA877);


/*
 *  SetOrigin()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetOrigin(
  short   h,
  short   v)                                                  ONEWORDINLINE(0xA878);


/*
 *  SetClip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetClip(RgnHandle rgn)                                        ONEWORDINLINE(0xA879);


/*
 *  GetClip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetClip(RgnHandle rgn)                                        ONEWORDINLINE(0xA87A);


/*
 *  ClipRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ClipRect(const Rect * r)                                      ONEWORDINLINE(0xA87B);


/*
 *  BackPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
BackPat(const Pattern * pat)                                  ONEWORDINLINE(0xA87C);


/*
 *  InitCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InitCursor(void)                                              ONEWORDINLINE(0xA850);


/*
 *  [Mac]SetCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacSetCursor SetCursor
#endif
EXTERN_API( void )
MacSetCursor(const Cursor * crsr)                             ONEWORDINLINE(0xA851);


/*
 *  HideCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HideCursor(void)                                              ONEWORDINLINE(0xA852);


/*
 *  [Mac]ShowCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacShowCursor ShowCursor
#endif
EXTERN_API( void )
MacShowCursor(void)                                           ONEWORDINLINE(0xA853);


/*
 *  ObscureCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ObscureCursor(void)                                           ONEWORDINLINE(0xA856);


/*
 *  HidePen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HidePen(void)                                                 ONEWORDINLINE(0xA896);


/*
 *  ShowPen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ShowPen(void)                                                 ONEWORDINLINE(0xA897);


/*
 *  GetPen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetPen(Point * pt)                                            ONEWORDINLINE(0xA89A);


/*
 *  GetPenState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetPenState(PenState * pnState)                               ONEWORDINLINE(0xA898);


/*
 *  SetPenState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPenState(const PenState * pnState)                         ONEWORDINLINE(0xA899);


/*
 *  PenSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PenSize(
  short   width,
  short   height)                                             ONEWORDINLINE(0xA89B);


/*
 *  PenMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PenMode(short mode)                                           ONEWORDINLINE(0xA89C);


/*
 *  PenPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PenPat(const Pattern * pat)                                   ONEWORDINLINE(0xA89D);


/*
 *  PenNormal()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PenNormal(void)                                               ONEWORDINLINE(0xA89E);


/*
 *  MoveTo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
MoveTo(
  short   h,
  short   v)                                                  ONEWORDINLINE(0xA893);


/*
 *  Move()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
Move(
  short   dh,
  short   dv)                                                 ONEWORDINLINE(0xA894);


/*
 *  [Mac]LineTo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacLineTo LineTo
#endif
EXTERN_API( void )
MacLineTo(
  short   h,
  short   v)                                                  ONEWORDINLINE(0xA891);


/*
 *  Line()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
Line(
  short   dh,
  short   dv)                                                 ONEWORDINLINE(0xA892);


/*
 *  ForeColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ForeColor(long color)                                         ONEWORDINLINE(0xA862);


/*
 *  BackColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
BackColor(long color)                                         ONEWORDINLINE(0xA863);


/*
 *  ColorBit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ColorBit(short whichBit)                                      ONEWORDINLINE(0xA864);


/*
 *  [Mac]SetRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacSetRect SetRect
#endif
EXTERN_API( void )
MacSetRect(
  Rect *  r,
  short   left,
  short   top,
  short   right,
  short   bottom)                                             ONEWORDINLINE(0xA8A7);


/*
 *  [Mac]OffsetRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacOffsetRect OffsetRect
#endif
EXTERN_API( void )
MacOffsetRect(
  Rect *  r,
  short   dh,
  short   dv)                                                 ONEWORDINLINE(0xA8A8);


/*
 *  [Mac]InsetRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacInsetRect InsetRect
#endif
EXTERN_API( void )
MacInsetRect(
  Rect *  r,
  short   dh,
  short   dv)                                                 ONEWORDINLINE(0xA8A9);


/*
 *  SectRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
SectRect(
  const Rect *  src1,
  const Rect *  src2,
  Rect *        dstRect)                                      ONEWORDINLINE(0xA8AA);


/*
 *  [Mac]UnionRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacUnionRect UnionRect
#endif
EXTERN_API( void )
MacUnionRect(
  const Rect *  src1,
  const Rect *  src2,
  Rect *        dstRect)                                      ONEWORDINLINE(0xA8AB);


/*
 *  [Mac]EqualRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacEqualRect EqualRect
#endif
EXTERN_API( Boolean )
MacEqualRect(
  const Rect *  rect1,
  const Rect *  rect2)                                        ONEWORDINLINE(0xA8A6);


/*
 *  EmptyRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
EmptyRect(const Rect * r)                                     ONEWORDINLINE(0xA8AE);


/*
 *  [Mac]FrameRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacFrameRect FrameRect
#endif
EXTERN_API( void )
MacFrameRect(const Rect * r)                                  ONEWORDINLINE(0xA8A1);


/*
 *  PaintRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PaintRect(const Rect * r)                                     ONEWORDINLINE(0xA8A2);


/*
 *  EraseRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
EraseRect(const Rect * r)                                     ONEWORDINLINE(0xA8A3);


/*
 *  [Mac]InvertRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacInvertRect InvertRect
#endif
EXTERN_API( void )
MacInvertRect(const Rect * r)                                 ONEWORDINLINE(0xA8A4);


/*
 *  [Mac]FillRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacFillRect FillRect
#endif
EXTERN_API( void )
MacFillRect(
  const Rect *     r,
  const Pattern *  pat)                                       ONEWORDINLINE(0xA8A5);


/*
 *  FrameOval()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FrameOval(const Rect * r)                                     ONEWORDINLINE(0xA8B7);


/*
 *  PaintOval()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PaintOval(const Rect * r)                                     ONEWORDINLINE(0xA8B8);


/*
 *  EraseOval()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
EraseOval(const Rect * r)                                     ONEWORDINLINE(0xA8B9);


/*
 *  InvertOval()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InvertOval(const Rect * r)                                    ONEWORDINLINE(0xA8BA);


/*
 *  FillOval()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FillOval(
  const Rect *     r,
  const Pattern *  pat)                                       ONEWORDINLINE(0xA8BB);


/*
 *  FrameRoundRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FrameRoundRect(
  const Rect *  r,
  short         ovalWidth,
  short         ovalHeight)                                   ONEWORDINLINE(0xA8B0);


/*
 *  PaintRoundRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PaintRoundRect(
  const Rect *  r,
  short         ovalWidth,
  short         ovalHeight)                                   ONEWORDINLINE(0xA8B1);


/*
 *  EraseRoundRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
EraseRoundRect(
  const Rect *  r,
  short         ovalWidth,
  short         ovalHeight)                                   ONEWORDINLINE(0xA8B2);


/*
 *  InvertRoundRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InvertRoundRect(
  const Rect *  r,
  short         ovalWidth,
  short         ovalHeight)                                   ONEWORDINLINE(0xA8B3);


/*
 *  FillRoundRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FillRoundRect(
  const Rect *     r,
  short            ovalWidth,
  short            ovalHeight,
  const Pattern *  pat)                                       ONEWORDINLINE(0xA8B4);


/*
 *  FrameArc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FrameArc(
  const Rect *  r,
  short         startAngle,
  short         arcAngle)                                     ONEWORDINLINE(0xA8BE);


/*
 *  PaintArc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PaintArc(
  const Rect *  r,
  short         startAngle,
  short         arcAngle)                                     ONEWORDINLINE(0xA8BF);


/*
 *  EraseArc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
EraseArc(
  const Rect *  r,
  short         startAngle,
  short         arcAngle)                                     ONEWORDINLINE(0xA8C0);


/*
 *  InvertArc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InvertArc(
  const Rect *  r,
  short         startAngle,
  short         arcAngle)                                     ONEWORDINLINE(0xA8C1);


/*
 *  FillArc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FillArc(
  const Rect *     r,
  short            startAngle,
  short            arcAngle,
  const Pattern *  pat)                                       ONEWORDINLINE(0xA8C2);


/*
 *  NewRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( RgnHandle )
NewRgn(void)                                                  ONEWORDINLINE(0xA8D8);


/*
 *  OpenRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
OpenRgn(void)                                                 ONEWORDINLINE(0xA8DA);


/*
 *  CloseRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
CloseRgn(RgnHandle dstRgn)                                    ONEWORDINLINE(0xA8DB);


/*
 *  BitMapToRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
BitMapToRegion(
  RgnHandle       region,
  const BitMap *  bMap)                                       ONEWORDINLINE(0xA8D7);


/*
 *  HandleToRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HandleToRgn(
  Handle      oldRegion,
  RgnHandle   region);


/*
 *  RgnToHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( void )
RgnToHandle(
  RgnHandle   region,
  Handle      flattenedRgnDataHdl);


/*
 *  DisposeRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisposeRgn(RgnHandle rgn)                                     ONEWORDINLINE(0xA8D9);


/*
 *  [Mac]CopyRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacCopyRgn CopyRgn
#endif
EXTERN_API( void )
MacCopyRgn(
  RgnHandle   srcRgn,
  RgnHandle   dstRgn)                                         ONEWORDINLINE(0xA8DC);


/*
 *  SetEmptyRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetEmptyRgn(RgnHandle rgn)                                    ONEWORDINLINE(0xA8DD);


/*
 *  [Mac]SetRectRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacSetRectRgn SetRectRgn
#endif
EXTERN_API( void )
MacSetRectRgn(
  RgnHandle   rgn,
  short       left,
  short       top,
  short       right,
  short       bottom)                                         ONEWORDINLINE(0xA8DE);


/*
 *  RectRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
RectRgn(
  RgnHandle     rgn,
  const Rect *  r)                                            ONEWORDINLINE(0xA8DF);


/*
 *  [Mac]OffsetRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacOffsetRgn OffsetRgn
#endif
EXTERN_API( void )
MacOffsetRgn(
  RgnHandle   rgn,
  short       dh,
  short       dv)                                             ONEWORDINLINE(0xA8E0);


/*
 *  InsetRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InsetRgn(
  RgnHandle   rgn,
  short       dh,
  short       dv)                                             ONEWORDINLINE(0xA8E1);


/*
 *  SectRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SectRgn(
  RgnHandle   srcRgnA,
  RgnHandle   srcRgnB,
  RgnHandle   dstRgn)                                         ONEWORDINLINE(0xA8E4);


/*
 *  [Mac]UnionRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacUnionRgn UnionRgn
#endif
EXTERN_API( void )
MacUnionRgn(
  RgnHandle   srcRgnA,
  RgnHandle   srcRgnB,
  RgnHandle   dstRgn)                                         ONEWORDINLINE(0xA8E5);


/*
 *  DiffRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DiffRgn(
  RgnHandle   srcRgnA,
  RgnHandle   srcRgnB,
  RgnHandle   dstRgn)                                         ONEWORDINLINE(0xA8E6);


/*
 *  [Mac]XorRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacXorRgn XorRgn
#endif
EXTERN_API( void )
MacXorRgn(
  RgnHandle   srcRgnA,
  RgnHandle   srcRgnB,
  RgnHandle   dstRgn)                                         ONEWORDINLINE(0xA8E7);


/*
 *  RectInRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
RectInRgn(
  const Rect *  r,
  RgnHandle     rgn)                                          ONEWORDINLINE(0xA8E9);


/*
 *  [Mac]EqualRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacEqualRgn EqualRgn
#endif
EXTERN_API( Boolean )
MacEqualRgn(
  RgnHandle   rgnA,
  RgnHandle   rgnB)                                           ONEWORDINLINE(0xA8E3);


/*
 *  EmptyRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
EmptyRgn(RgnHandle rgn)                                       ONEWORDINLINE(0xA8E2);


/*
 *  [Mac]FrameRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacFrameRgn FrameRgn
#endif
EXTERN_API( void )
MacFrameRgn(RgnHandle rgn)                                    ONEWORDINLINE(0xA8D2);


/*
 *  [Mac]PaintRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacPaintRgn PaintRgn
#endif
EXTERN_API( void )
MacPaintRgn(RgnHandle rgn)                                    ONEWORDINLINE(0xA8D3);


/*
 *  EraseRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
EraseRgn(RgnHandle rgn)                                       ONEWORDINLINE(0xA8D4);


/*
 *  [Mac]InvertRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacInvertRgn InvertRgn
#endif
EXTERN_API( void )
MacInvertRgn(RgnHandle rgn)                                   ONEWORDINLINE(0xA8D5);


/*
 *  [Mac]FillRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacFillRgn FillRgn
#endif
EXTERN_API( void )
MacFillRgn(
  RgnHandle        rgn,
  const Pattern *  pat)                                       ONEWORDINLINE(0xA8D6);


/*
 *  ScrollRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ScrollRect(
  const Rect *  r,
  short         dh,
  short         dv,
  RgnHandle     updateRgn)                                    ONEWORDINLINE(0xA8EF);


/*
 *  CopyBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
CopyBits(
  const BitMap *  srcBits,
  const BitMap *  dstBits,
  const Rect *    srcRect,
  const Rect *    dstRect,
  short           mode,
  RgnHandle       maskRgn)       /* can be NULL */            ONEWORDINLINE(0xA8EC);


/*
 *  SeedFill()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SeedFill(
  const void *  srcPtr,
  void *        dstPtr,
  short         srcRow,
  short         dstRow,
  short         height,
  short         words,
  short         seedH,
  short         seedV)                                        ONEWORDINLINE(0xA839);


/*
 *  CalcMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
CalcMask(
  const void *  srcPtr,
  void *        dstPtr,
  short         srcRow,
  short         dstRow,
  short         height,
  short         words)                                        ONEWORDINLINE(0xA838);


/*
 *  CopyMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
CopyMask(
  const BitMap *  srcBits,
  const BitMap *  maskBits,
  const BitMap *  dstBits,
  const Rect *    srcRect,
  const Rect *    maskRect,
  const Rect *    dstRect)                                    ONEWORDINLINE(0xA817);


/*
 *  OpenPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( PicHandle )
OpenPicture(const Rect * picFrame)                            ONEWORDINLINE(0xA8F3);


/*
 *  PicComment()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PicComment(
  short    kind,
  short    dataSize,
  Handle   dataHandle)                                        ONEWORDINLINE(0xA8F2);


/*
 *  ClosePicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ClosePicture(void)                                            ONEWORDINLINE(0xA8F4);


/*
 *  DrawPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DrawPicture(
  PicHandle     myPicture,
  const Rect *  dstRect)                                      ONEWORDINLINE(0xA8F6);


/*
 *  KillPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
KillPicture(PicHandle myPicture)                              ONEWORDINLINE(0xA8F5);


/*
 *  OpenPoly()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( PolyHandle )
OpenPoly(void)                                                ONEWORDINLINE(0xA8CB);


/*
 *  ClosePoly()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ClosePoly(void)                                               ONEWORDINLINE(0xA8CC);


/*
 *  KillPoly()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
KillPoly(PolyHandle poly)                                     ONEWORDINLINE(0xA8CD);


/*
 *  OffsetPoly()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
OffsetPoly(
  PolyHandle   poly,
  short        dh,
  short        dv)                                            ONEWORDINLINE(0xA8CE);


/*
 *  FramePoly()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FramePoly(PolyHandle poly)                                    ONEWORDINLINE(0xA8C6);


/*
 *  PaintPoly()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PaintPoly(PolyHandle poly)                                    ONEWORDINLINE(0xA8C7);


/*
 *  ErasePoly()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ErasePoly(PolyHandle poly)                                    ONEWORDINLINE(0xA8C8);


/*
 *  InvertPoly()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InvertPoly(PolyHandle poly)                                   ONEWORDINLINE(0xA8C9);


/*
 *  FillPoly()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FillPoly(
  PolyHandle       poly,
  const Pattern *  pat)                                       ONEWORDINLINE(0xA8CA);


/*
 *  SetPt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPt(
  Point *  pt,
  short    h,
  short    v)                                                 ONEWORDINLINE(0xA880);


/*
 *  LocalToGlobal()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LocalToGlobal(Point * pt)                                     ONEWORDINLINE(0xA870);


/*
 *  GlobalToLocal()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GlobalToLocal(Point * pt)                                     ONEWORDINLINE(0xA871);


/*
 *  Random()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
Random(void)                                                  ONEWORDINLINE(0xA861);


/*
 *  StuffHex()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StuffHex(
  void *             thingPtr,
  ConstStr255Param   s)                                       ONEWORDINLINE(0xA866);


/*
 *  [Mac]GetPixel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacGetPixel GetPixel
#endif
EXTERN_API( Boolean )
MacGetPixel(
  short   h,
  short   v)                                                  ONEWORDINLINE(0xA865);


/*
 *  ScalePt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ScalePt(
  Point *       pt,
  const Rect *  srcRect,
  const Rect *  dstRect)                                      ONEWORDINLINE(0xA8F8);


/*
 *  MapPt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
MapPt(
  Point *       pt,
  const Rect *  srcRect,
  const Rect *  dstRect)                                      ONEWORDINLINE(0xA8F9);


/*
 *  MapRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
MapRect(
  Rect *        r,
  const Rect *  srcRect,
  const Rect *  dstRect)                                      ONEWORDINLINE(0xA8FA);


/*
 *  MapRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
MapRgn(
  RgnHandle     rgn,
  const Rect *  srcRect,
  const Rect *  dstRect)                                      ONEWORDINLINE(0xA8FB);


/*
 *  MapPoly()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
MapPoly(
  PolyHandle    poly,
  const Rect *  srcRect,
  const Rect *  dstRect)                                      ONEWORDINLINE(0xA8FC);


/*
 *  SetStdProcs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetStdProcs(QDProcs * procs)                                  ONEWORDINLINE(0xA8EA);


/*
 *  StdRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdRect(
  GrafVerb      verb,
  const Rect *  r)                                            ONEWORDINLINE(0xA8A0);


/*
 *  StdRRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdRRect(
  GrafVerb      verb,
  const Rect *  r,
  short         ovalWidth,
  short         ovalHeight)                                   ONEWORDINLINE(0xA8AF);


/*
 *  StdOval()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdOval(
  GrafVerb      verb,
  const Rect *  r)                                            ONEWORDINLINE(0xA8B6);


/*
 *  StdArc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdArc(
  GrafVerb      verb,
  const Rect *  r,
  short         startAngle,
  short         arcAngle)                                     ONEWORDINLINE(0xA8BD);


/*
 *  StdPoly()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdPoly(
  GrafVerb     verb,
  PolyHandle   poly)                                          ONEWORDINLINE(0xA8C5);


/*
 *  StdRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdRgn(
  GrafVerb    verb,
  RgnHandle   rgn)                                            ONEWORDINLINE(0xA8D1);


/*
 *  StdBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdBits(
  const BitMap *  srcBits,
  const Rect *    srcRect,
  const Rect *    dstRect,
  short           mode,
  RgnHandle       maskRgn)                                    ONEWORDINLINE(0xA8EB);


/*
 *  StdComment()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdComment(
  short    kind,
  short    dataSize,
  Handle   dataHandle)                                        ONEWORDINLINE(0xA8F1);


/*
 *  StdGetPic()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdGetPic(
  void *  dataPtr,
  short   byteCount)                                          ONEWORDINLINE(0xA8EE);


/*
 *  StdPutPic()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdPutPic(
  const void *  dataPtr,
  short         byteCount)                                    ONEWORDINLINE(0xA8F0);


/*
 *  StdOpcode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdOpcode(
  const Rect *  fromRect,
  const Rect *  toRect,
  UInt16        opcode,
  SInt16        version)                                      ONEWORDINLINE(0xABF8);


/*
 *  AddPt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
AddPt(
  Point    src,
  Point *  dst)                                               ONEWORDINLINE(0xA87E);


/*
 *  EqualPt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
EqualPt(
  Point   pt1,
  Point   pt2)                                                ONEWORDINLINE(0xA881);


/*
 *  [Mac]PtInRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacPtInRect PtInRect
#endif
EXTERN_API( Boolean )
MacPtInRect(
  Point         pt,
  const Rect *  r)                                            ONEWORDINLINE(0xA8AD);


/*
 *  Pt2Rect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
Pt2Rect(
  Point   pt1,
  Point   pt2,
  Rect *  dstRect)                                            ONEWORDINLINE(0xA8AC);


/*
 *  PtToAngle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PtToAngle(
  const Rect *  r,
  Point         pt,
  short *       angle)                                        ONEWORDINLINE(0xA8C3);


/*
 *  SubPt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SubPt(
  Point    src,
  Point *  dst)                                               ONEWORDINLINE(0xA87F);


/*
 *  PtInRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
PtInRgn(
  Point       pt,
  RgnHandle   rgn)                                            ONEWORDINLINE(0xA8E8);


/*
 *  StdLine()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
StdLine(Point newPt)                                          ONEWORDINLINE(0xA890);


#if CALL_NOT_IN_CARBON
/*
 *  OpenCPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
OpenCPort(CGrafPtr port)                                      ONEWORDINLINE(0xAA00);


/*
 *  InitCPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
InitCPort(CGrafPtr port)                                      ONEWORDINLINE(0xAA01);


/*
 *  CloseCPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
CloseCPort(CGrafPtr port)                                     ONEWORDINLINE(0xAA02);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  NewPixMap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( PixMapHandle )
NewPixMap(void)                                               ONEWORDINLINE(0xAA03);


/*
 *  DisposePixMap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisposePixMap(PixMapHandle pm)                                ONEWORDINLINE(0xAA04);


/*
 *  CopyPixMap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
CopyPixMap(
  PixMapHandle   srcPM,
  PixMapHandle   dstPM)                                       ONEWORDINLINE(0xAA05);


/*
 *  NewPixPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( PixPatHandle )
NewPixPat(void)                                               ONEWORDINLINE(0xAA07);


/*
 *  DisposePixPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisposePixPat(PixPatHandle pp)                                ONEWORDINLINE(0xAA08);


/*
 *  CopyPixPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
CopyPixPat(
  PixPatHandle   srcPP,
  PixPatHandle   dstPP)                                       ONEWORDINLINE(0xAA09);


/*
 *  PenPixPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PenPixPat(PixPatHandle pp)                                    ONEWORDINLINE(0xAA0A);


/*
 *  BackPixPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
BackPixPat(PixPatHandle pp)                                   ONEWORDINLINE(0xAA0B);


/*
 *  GetPixPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( PixPatHandle )
GetPixPat(short patID)                                        ONEWORDINLINE(0xAA0C);


/*
 *  MakeRGBPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
MakeRGBPat(
  PixPatHandle      pp,
  const RGBColor *  myColor)                                  ONEWORDINLINE(0xAA0D);


/*
 *  FillCRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FillCRect(
  const Rect *   r,
  PixPatHandle   pp)                                          ONEWORDINLINE(0xAA0E);


/*
 *  FillCOval()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FillCOval(
  const Rect *   r,
  PixPatHandle   pp)                                          ONEWORDINLINE(0xAA0F);


/*
 *  FillCRoundRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FillCRoundRect(
  const Rect *   r,
  short          ovalWidth,
  short          ovalHeight,
  PixPatHandle   pp)                                          ONEWORDINLINE(0xAA10);


/*
 *  FillCArc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FillCArc(
  const Rect *   r,
  short          startAngle,
  short          arcAngle,
  PixPatHandle   pp)                                          ONEWORDINLINE(0xAA11);


/*
 *  FillCRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FillCRgn(
  RgnHandle      rgn,
  PixPatHandle   pp)                                          ONEWORDINLINE(0xAA12);


/*
 *  FillCPoly()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FillCPoly(
  PolyHandle     poly,
  PixPatHandle   pp)                                          ONEWORDINLINE(0xAA13);


/*
 *  RGBForeColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
RGBForeColor(const RGBColor * color)                          ONEWORDINLINE(0xAA14);


/*
 *  RGBBackColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
RGBBackColor(const RGBColor * color)                          ONEWORDINLINE(0xAA15);


/*
 *  SetCPixel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetCPixel(
  short             h,
  short             v,
  const RGBColor *  cPix)                                     ONEWORDINLINE(0xAA16);


/*
 *  SetPortPix()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortPix(PixMapHandle pm)                                   ONEWORDINLINE(0xAA06);


/*
 *  GetCPixel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetCPixel(
  short       h,
  short       v,
  RGBColor *  cPix)                                           ONEWORDINLINE(0xAA17);


/*
 *  GetForeColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetForeColor(RGBColor * color)                                ONEWORDINLINE(0xAA19);


/*
 *  GetBackColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetBackColor(RGBColor * color)                                ONEWORDINLINE(0xAA1A);


/*
 *  SeedCFill()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SeedCFill(
  const BitMap *   srcBits,
  const BitMap *   dstBits,
  const Rect *     srcRect,
  const Rect *     dstRect,
  short            seedH,
  short            seedV,
  ColorSearchUPP   matchProc,
  long             matchData)                                 ONEWORDINLINE(0xAA50);


/*
 *  CalcCMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
CalcCMask(
  const BitMap *    srcBits,
  const BitMap *    dstBits,
  const Rect *      srcRect,
  const Rect *      dstRect,
  const RGBColor *  seedRGB,
  ColorSearchUPP    matchProc,
  long              matchData)                                ONEWORDINLINE(0xAA4F);


/*
 *  OpenCPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( PicHandle )
OpenCPicture(const OpenCPicParams * newHeader)                ONEWORDINLINE(0xAA20);


/*
 *  OpColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
OpColor(const RGBColor * color)                               ONEWORDINLINE(0xAA21);


/*
 *  HiliteColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HiliteColor(const RGBColor * color)                           ONEWORDINLINE(0xAA22);


/*
 *  DisposeCTable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisposeCTable(CTabHandle cTable)                              ONEWORDINLINE(0xAA24);


/*
 *  GetCTable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( CTabHandle )
GetCTable(short ctID)                                         ONEWORDINLINE(0xAA18);


/*
 *  GetCCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( CCrsrHandle )
GetCCursor(short crsrID)                                      ONEWORDINLINE(0xAA1B);


/*
 *  SetCCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetCCursor(CCrsrHandle cCrsr)                                 ONEWORDINLINE(0xAA1C);


/*
 *  AllocCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
AllocCursor(void)                                             ONEWORDINLINE(0xAA1D);


/*
 *  DisposeCCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisposeCCursor(CCrsrHandle cCrsr)                             ONEWORDINLINE(0xAA26);


/* GetCIcon(), PlotCIcon(), and DisposeCIcon() moved to Icons.h*/

/*
 *  SetStdCProcs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetStdCProcs(CQDProcs * procs)                                ONEWORDINLINE(0xAA4E);


/*
 *  GetMaxDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( GDHandle )
GetMaxDevice(const Rect * globalRect)                         ONEWORDINLINE(0xAA27);


/*
 *  GetCTSeed()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
GetCTSeed(void)                                               ONEWORDINLINE(0xAA28);


/*
 *  GetDeviceList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( GDHandle )
GetDeviceList(void)                                           ONEWORDINLINE(0xAA29);


/*
 *  GetMainDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( GDHandle )
GetMainDevice(void)                                           ONEWORDINLINE(0xAA2A);


/*
 *  GetNextDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( GDHandle )
GetNextDevice(GDHandle curDevice)                             ONEWORDINLINE(0xAA2B);


/*
 *  TestDeviceAttribute()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
TestDeviceAttribute(
  GDHandle   gdh,
  short      attribute)                                       ONEWORDINLINE(0xAA2C);


/*
 *  SetDeviceAttribute()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetDeviceAttribute(
  GDHandle   gdh,
  short      attribute,
  Boolean    value)                                           ONEWORDINLINE(0xAA2D);


/*
 *  InitGDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InitGDevice(
  short      qdRefNum,
  long       mode,
  GDHandle   gdh)                                             ONEWORDINLINE(0xAA2E);


/*
 *  NewGDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( GDHandle )
NewGDevice(
  short   refNum,
  long    mode)                                               ONEWORDINLINE(0xAA2F);


/*
 *  DisposeGDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisposeGDevice(GDHandle gdh)                                  ONEWORDINLINE(0xAA30);


/*
 *  SetGDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetGDevice(GDHandle gd)                                       ONEWORDINLINE(0xAA31);


/*
 *  GetGDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( GDHandle )
GetGDevice(void)                                              ONEWORDINLINE(0xAA32);


/*
 *  Color2Index()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
Color2Index(const RGBColor * myColor)                         ONEWORDINLINE(0xAA33);


/*
 *  Index2Color()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
Index2Color(
  long        index,
  RGBColor *  aColor)                                         ONEWORDINLINE(0xAA34);


/*
 *  InvertColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InvertColor(RGBColor * myColor)                               ONEWORDINLINE(0xAA35);


/*
 *  RealColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
RealColor(const RGBColor * color)                             ONEWORDINLINE(0xAA36);


/*
 *  GetSubTable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetSubTable(
  CTabHandle   myColors,
  short        iTabRes,
  CTabHandle   targetTbl)                                     ONEWORDINLINE(0xAA37);


/*
 *  MakeITable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
MakeITable(
  CTabHandle   cTabH,
  ITabHandle   iTabH,
  short        res)                                           ONEWORDINLINE(0xAA39);


/*
 *  AddSearch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
AddSearch(ColorSearchUPP searchProc)                          ONEWORDINLINE(0xAA3A);


/*
 *  AddComp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
AddComp(ColorComplementUPP compProc)                          ONEWORDINLINE(0xAA3B);


/*
 *  DelSearch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DelSearch(ColorSearchUPP searchProc)                          ONEWORDINLINE(0xAA4C);


/*
 *  DelComp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DelComp(ColorComplementUPP compProc)                          ONEWORDINLINE(0xAA4D);


/*
 *  SetClientID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetClientID(short id)                                         ONEWORDINLINE(0xAA3C);


/*
 *  ProtectEntry()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ProtectEntry(
  short     index,
  Boolean   protect)                                          ONEWORDINLINE(0xAA3D);


/*
 *  ReserveEntry()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ReserveEntry(
  short     index,
  Boolean   reserve)                                          ONEWORDINLINE(0xAA3E);


/*
 *  SetEntries()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetEntries(
  short        start,
  short        count,
  CSpecArray   aTable)                                        ONEWORDINLINE(0xAA3F);


/*
 *  SaveEntries()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SaveEntries(
  CTabHandle    srcTable,
  CTabHandle    resultTable,
  ReqListRec *  selection)                                    ONEWORDINLINE(0xAA49);


/*
 *  RestoreEntries()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
RestoreEntries(
  CTabHandle    srcTable,
  CTabHandle    dstTable,
  ReqListRec *  selection)                                    ONEWORDINLINE(0xAA4A);


/*
 *  QDError()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
QDError(void)                                                 ONEWORDINLINE(0xAA40);


/*
 *  CopyDeepMask()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
CopyDeepMask(
  const BitMap *  srcBits,
  const BitMap *  maskBits,
  const BitMap *  dstBits,
  const Rect *    srcRect,
  const Rect *    maskRect,
  const Rect *    dstRect,
  short           mode,
  RgnHandle       maskRgn)        /* can be NULL */           ONEWORDINLINE(0xAA51);


/*
 *  DeviceLoop()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DeviceLoop(
  RgnHandle              drawingRgn,
  DeviceLoopDrawingUPP   drawingProc,
  long                   userData,
  DeviceLoopFlags        flags)                               ONEWORDINLINE(0xABCA);




/*
 *  GetMaskTable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC && TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#pragma parameter __A0 GetMaskTable
#endif
EXTERN_API( Ptr )
GetMaskTable(void)                                            ONEWORDINLINE(0xA836);



/*
 *  GetPattern()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( PatHandle )
GetPattern(short patternID)                                   ONEWORDINLINE(0xA9B8);


/*
 *  [Mac]GetCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacGetCursor GetCursor
#endif
EXTERN_API( CursHandle )
MacGetCursor(short cursorID)                                  ONEWORDINLINE(0xA9B9);


/*
 *  GetPicture()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( PicHandle )
GetPicture(short pictureID)                                   ONEWORDINLINE(0xA9BC);


/*
 *  DeltaPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
DeltaPoint(
  Point   ptA,
  Point   ptB)                                                ONEWORDINLINE(0xA94F);


/*
 *  ShieldCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ShieldCursor(
  const Rect *  shieldRect,
  Point         offsetPt)                                     ONEWORDINLINE(0xA855);


/*
 *  ScreenRes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ScreenRes(
  short *  scrnHRes,
  short *  scrnVRes)                                          SIXWORDINLINE(0x225F, 0x32B8, 0x0102, 0x225F, 0x32B8, 0x0104);


/*
 *  GetIndPattern()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetIndPattern(
  Pattern *  thePat,
  short      patternListID,
  short      index);


#if CALL_NOT_IN_CARBON
/*
 *  ptinrect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
ptinrect(
  const Point *  pt,
  const Rect *   r);


/*
 *  pt2rect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
pt2rect(
  const Point *  pt1,
  const Point *  pt2,
  Rect *         destRect);


/*
 *  pttoangle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
pttoangle(
  const Rect *   r,
  const Point *  pt,
  short *        angle);


/*
 *  ptinrgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
ptinrgn(
  const Point *  pt,
  RgnHandle      rgn);


/*
 *  addpt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
addpt(
  const Point *  src,
  Point *        dst);


/*
 *  subpt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
subpt(
  const Point *  src,
  Point *        dst);


/*
 *  equalpt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
equalpt(
  const Point *  pt1,
  const Point *  pt2);


/*
 *  stuffhex()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
stuffhex(
  void *        thingPtr,
  const char *  s);


/*
 *  stdline()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
stdline(const Point * newPt);


/*
 *  shieldcursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
shieldcursor(
  const Rect *  shieldRect,
  Point *       offsetPt);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  deltapoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( long )
deltapoint(
  Point *  ptA,
  Point *  ptB);


#if OLDROUTINENAMES
#define DisposPixMap(pm) DisposePixMap(pm)
#define DisposPixPat(pp) DisposePixPat(pp)
#define DisposCTable(cTable) DisposeCTable(cTable)
#define DisposCCursor(cCrsr) DisposeCCursor(cCrsr)
#define DisposGDevice(gdh) DisposeGDevice(gdh)
#endif  /* OLDROUTINENAMES */

/*
    From ToolUtils.i
*/
/*
 *  PackBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
PackBits(
  Ptr *   srcPtr,
  Ptr *   dstPtr,
  short   srcBytes)                                           ONEWORDINLINE(0xA8CF);


/*
 *  UnpackBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
UnpackBits(
  Ptr *   srcPtr,
  Ptr *   dstPtr,
  short   dstBytes)                                           ONEWORDINLINE(0xA8D0);


/*
 *  SlopeFromAngle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fixed )
SlopeFromAngle(short angle)                                   ONEWORDINLINE(0xA8BC);


/*
 *  AngleFromSlope()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
AngleFromSlope(Fixed slope)                                   ONEWORDINLINE(0xA8C4);


/* New transfer modes */
enum {
  colorXorXFer                  = 52,
  noiseXFer                     = 53,
  customXFer                    = 54
};

/* Custom XFer flags */
enum {
  kXFer1PixelAtATime            = 0x00000001, /* 1 pixel passed to custom XFer proc*/
  kXFerConvertPixelToRGB32      = 0x00000002 /* All color depths converted to 32 bit RGB*/
};

struct CustomXFerRec {
  UInt32              version;
  void *              srcPixels;
  void *              destPixels;
  void *              resultPixels;
  UInt32              refCon;
  UInt32              pixelSize;
  UInt32              pixelCount;
  Point               firstPixelHV;
  Rect                destBounds;
};
typedef struct CustomXFerRec            CustomXFerRec;
typedef CustomXFerRec *                 CustomXFerRecPtr;
typedef CALLBACK_API( void , CustomXFerProcPtr )(CustomXFerRecPtr info);
/*
 *  GetPortCustomXFerProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetPortCustomXFerProc(
  CGrafPtr             port,
  CustomXFerProcPtr *  proc,
  UInt32 *             flags,
  UInt32 *             refCon)                                FOURWORDINLINE(0x203C, 0x0010, 0x0019, 0xAB1D);


/*
 *  SetPortCustomXFerProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetPortCustomXFerProc(
  CGrafPtr            port,
  CustomXFerProcPtr   proc,
  UInt32              flags,
  UInt32              refCon)                                 FOURWORDINLINE(0x203C, 0x0010, 0x001A, 0xAB1D);



enum {
  kCursorComponentsVersion      = 0x00010001
};

enum {
  kCursorComponentType          = FOUR_CHAR_CODE('curs')
};

/* Cursor Component capabilities flags */
enum {
  cursorDoesAnimate             = 1L << 0,
  cursorDoesHardware            = 1L << 1,
  cursorDoesUnreadableScreenBits = 1L << 2
};

/* Cursor Component output mode flags */
enum {
  kRenderCursorInHardware       = 1L << 0,
  kRenderCursorInSoftware       = 1L << 1
};

/* Cursor Component Info */
struct CursorInfo {
  long                version;                /* use kCursorComponentsVersion */
  long                capabilities;
  long                animateDuration;        /* approximate time between animate tickles */
  Rect                bounds;
  Point               hotspot;
  long                reserved;               /* must set to zero */

};
typedef struct CursorInfo               CursorInfo;
/* Cursor Component Selectors */
enum {
  kCursorComponentInit          = 0x0001,
  kCursorComponentGetInfo       = 0x0002,
  kCursorComponentSetOutputMode = 0x0003,
  kCursorComponentSetData       = 0x0004,
  kCursorComponentReconfigure   = 0x0005,
  kCursorComponentDraw          = 0x0006,
  kCursorComponentErase         = 0x0007,
  kCursorComponentMove          = 0x0008,
  kCursorComponentAnimate       = 0x0009,
  kCursorComponentLastReserved  = 0x0050
};

/*
 *  OpenCursorComponent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
OpenCursorComponent(
  Component            c,
  ComponentInstance *  ci)                                    FOURWORDINLINE(0x203C, 0x0008, 0x000B, 0xABE0);


/*
 *  CloseCursorComponent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CloseCursorComponent(ComponentInstance ci)                    FOURWORDINLINE(0x203C, 0x0004, 0x000C, 0xABE0);


/*
 *  SetCursorComponent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetCursorComponent(ComponentInstance ci)                      FOURWORDINLINE(0x203C, 0x0004, 0x000D, 0xABE0);


/*
 *  CursorComponentChanged()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CursorComponentChanged(ComponentInstance ci)                  FOURWORDINLINE(0x203C, 0x0004, 0x000E, 0xABE0);


/*
 *  CursorComponentSetData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CursorComponentSetData(
  ComponentInstance   ci,
  long                data)                                   FOURWORDINLINE(0x203C, 0x0008, 0x000F, 0xABE0);


/* Quickdraw-specific ColorSync matching */
/* Available in CarbonLib... */
/*
 *  IsValidPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsValidPort(CGrafPtr port);



#if ACCESSOR_CALLS_ARE_FUNCTIONS
/* GrafPort */
/* Getters */
/*
 *  GetPortPixMap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( PixMapHandle )
GetPortPixMap(CGrafPtr port);


/*
 *  GetPortBitMapForCopyBits()
 *  
 *  Discussion:
 *    GetPortBitMapForCopyBits is provided for the specific purpose of
 *    using the return value as a parameter to CopyBits. The return
 *    value can be used as the srcBits or dstBits parameter to CopyBits
 *    regardless of whether the port is color. If the port parameter is
 *    a color port, however, the returned BitMapPtr does not actually
 *    point to a BitMap; it points to the PixMapHandle and other fields
 *    in the CGrafPort structure. You should not dereference the
 *    BitMapPtr or otherwise depend on its contents unless you've
 *    confirmed that this port is a non-color port.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0.2 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( const BitMap * )
GetPortBitMapForCopyBits(CGrafPtr port);


/*
 *  GetPortBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Rect * )
GetPortBounds(
  CGrafPtr   port,
  Rect *     rect);


/*
 *  GetPortForeColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( RGBColor * )
GetPortForeColor(
  CGrafPtr    port,
  RGBColor *  foreColor);


/*
 *  GetPortBackColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( RGBColor * )
GetPortBackColor(
  CGrafPtr    port,
  RGBColor *  backColor);


/*
 *  GetPortOpColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( RGBColor * )
GetPortOpColor(
  CGrafPtr    port,
  RGBColor *  opColor);


/*
 *  GetPortHiliteColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( RGBColor * )
GetPortHiliteColor(
  CGrafPtr    port,
  RGBColor *  hiliteColor);


/*
 *  GetPortGrafProcs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( CQDProcsPtr )
GetPortGrafProcs(CGrafPtr port);


/*
 *  GetPortTextFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
GetPortTextFont(CGrafPtr port);


/*
 *  GetPortTextFace()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Style )
GetPortTextFace(CGrafPtr port);


/*
 *  GetPortTextMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
GetPortTextMode(CGrafPtr port);


/*
 *  GetPortTextSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
GetPortTextSize(CGrafPtr port);


/*
 *  GetPortChExtra()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
GetPortChExtra(CGrafPtr port);


/*
 *  GetPortFracHPenLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
GetPortFracHPenLocation(CGrafPtr port);


/*
 *  GetPortSpExtra()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Fixed )
GetPortSpExtra(CGrafPtr port);


/*
 *  GetPortPenVisibility()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
GetPortPenVisibility(CGrafPtr port);


/*
 *  GetPortVisibleRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( RgnHandle )
GetPortVisibleRegion(
  CGrafPtr    port,
  RgnHandle   visRgn);


/*
 *  GetPortClipRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( RgnHandle )
GetPortClipRegion(
  CGrafPtr    port,
  RgnHandle   clipRgn);


/*
 *  GetPortBackPixPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( PixPatHandle )
GetPortBackPixPat(
  CGrafPtr       port,
  PixPatHandle   backPattern);


/*
 *  GetPortPenPixPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( PixPatHandle )
GetPortPenPixPat(
  CGrafPtr       port,
  PixPatHandle   penPattern);


/*
 *  GetPortFillPixPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( PixPatHandle )
GetPortFillPixPat(
  CGrafPtr       port,
  PixPatHandle   fillPattern);


/*
 *  GetPortPenSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Point * )
GetPortPenSize(
  CGrafPtr   port,
  Point *    penSize);


/*
 *  GetPortPenMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 )
GetPortPenMode(CGrafPtr port);


/*
 *  GetPortPenLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Point * )
GetPortPenLocation(
  CGrafPtr   port,
  Point *    penLocation);


/*
 *  IsPortRegionBeingDefined()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsPortRegionBeingDefined(CGrafPtr port);


/*
 *  IsPortPictureBeingDefined()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsPortPictureBeingDefined(CGrafPtr port);


/*
 *  IsPortPolyBeingDefined()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.3 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsPortPolyBeingDefined(CGrafPtr port);


/*
 *  IsPortOffscreen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsPortOffscreen(CGrafPtr port);


/*
 *  IsPortColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0.2 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsPortColor(CGrafPtr port);


/*
 *  IsPortVisibleRegionEmpty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( Boolean )
IsPortVisibleRegionEmpty(CGrafPtr port);


/*
 *  IsPortClipRegionEmpty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( Boolean )
IsPortClipRegionEmpty(CGrafPtr port);


/*
 *  SectRegionWithPortClipRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( void )
SectRegionWithPortClipRegion(
  CGrafPtr    port,
  RgnHandle   ioRegion);


/*
 *  SectRegionWithPortVisibleRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( void )
SectRegionWithPortVisibleRegion(
  CGrafPtr    port,
  RgnHandle   ioRegion);



/* Swappers */
/*
 *  SwapPortPicSaveHandle()
 *  
 *  Summary:
 *    Sets the port's picSave Handle, and returns the previous picSave
 *  
 *  Discussion:
 *    Historically, the picSave field in a GrafPort is != NULL if a
 *    Picture is being defined; and it has been documented that picture
 *    definition can be temporarily suspended by saving the current
 *    picSave Handle and setting picSave to NULL. Restoring the saved
 *    picSave Handle resumes picture definition.
 *  
 *  Parameters:
 *    
 *    port:
 *      The port whose picSave field is being swapped.
 *    
 *    inPicSaveHdl:
 *      The picSave Handle to be set.
 *  
 *  Result:
 *    The previous picSave Handle in the port.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( Handle )
SwapPortPicSaveHandle(
  CGrafPtr   port,
  Handle     inPicSaveHdl);


/* Similarly: */
/*
 *  SwapPortPolySaveHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.6 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( Handle )
SwapPortPolySaveHandle(
  CGrafPtr   port,
  Handle     inPolySaveHdl);


/*
 *  SwapPortRegionSaveHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.6 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( Handle )
SwapPortRegionSaveHandle(
  CGrafPtr   port,
  Handle     inRegionSaveHdl);



/* Setters */
/*
 *  SetPortBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortBounds(
  CGrafPtr      port,
  const Rect *  rect);


/*
 *  SetPortOpColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortOpColor(
  CGrafPtr          port,
  const RGBColor *  opColor);


/*
 *  SetPortGrafProcs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortGrafProcs(
  CGrafPtr      port,
  CQDProcsPtr   procs);


/*
 *  SetPortTextFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( void )
SetPortTextFont(
  CGrafPtr   port,
  short      txFont);


/*
 *  SetPortTextSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( void )
SetPortTextSize(
  CGrafPtr   port,
  short      txSize);


/*
 *  SetPortTextFace()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( void )
SetPortTextFace(
  CGrafPtr         port,
  StyleParameter   face);


/*
 *  SetPortTextMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( void )
SetPortTextMode(
  CGrafPtr   port,
  short      mode);


/*
 *  SetPortVisibleRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortVisibleRegion(
  CGrafPtr    port,
  RgnHandle   visRgn);


/*
 *  SetPortClipRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortClipRegion(
  CGrafPtr    port,
  RgnHandle   clipRgn);


/*
 *  SetPortPenPixPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortPenPixPat(
  CGrafPtr       port,
  PixPatHandle   penPattern);


/*
 *  SetPortFillPixPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.2 and later
 *    CarbonLib:        in CarbonLib 1.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortFillPixPat(
  CGrafPtr       port,
  PixPatHandle   penPattern);


/*
 *  SetPortBackPixPat()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortBackPixPat(
  CGrafPtr       port,
  PixPatHandle   backPattern);


/*
 *  SetPortPenSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortPenSize(
  CGrafPtr   port,
  Point      penSize);


/*
 *  SetPortPenMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortPenMode(
  CGrafPtr   port,
  SInt32     penMode);


/*
 *  SetPortFracHPenLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetPortFracHPenLocation(
  CGrafPtr   port,
  short      pnLocHFrac);


/* PixMap */
/*
 *  GetPixBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Rect * )
GetPixBounds(
  PixMapHandle   pixMap,
  Rect *         bounds);


/*
 *  GetPixDepth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
GetPixDepth(PixMapHandle pixMap);


/* QDGlobals */
/* Getters */
/*
 *  GetQDGlobalsRandomSeed()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
GetQDGlobalsRandomSeed(void);


/*
 *  GetQDGlobalsScreenBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( BitMap * )
GetQDGlobalsScreenBits(BitMap * screenBits);


/*
 *  GetQDGlobalsArrow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Cursor * )
GetQDGlobalsArrow(Cursor * arrow);


/*
 *  GetQDGlobalsDarkGray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Pattern * )
GetQDGlobalsDarkGray(Pattern * dkGray);


/*
 *  GetQDGlobalsLightGray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Pattern * )
GetQDGlobalsLightGray(Pattern * ltGray);


/*
 *  GetQDGlobalsGray()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Pattern * )
GetQDGlobalsGray(Pattern * gray);


/*
 *  GetQDGlobalsBlack()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Pattern * )
GetQDGlobalsBlack(Pattern * black);


/*
 *  GetQDGlobalsWhite()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Pattern * )
GetQDGlobalsWhite(Pattern * white);


/*
 *  GetQDGlobalsThePort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( CGrafPtr )
GetQDGlobalsThePort(void);


/* Setters */
/*
 *  SetQDGlobalsRandomSeed()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetQDGlobalsRandomSeed(long randomSeed);


/*
 *  SetQDGlobalsArrow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetQDGlobalsArrow(const Cursor * arrow);


/* Regions */
/*
 *  GetRegionBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Rect * )
GetRegionBounds(
  RgnHandle   region,
  Rect *      bounds);


/*
 *  IsRegionRectangular()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsRegionRectangular(RgnHandle region);


/* Utilities */
/* To prevent upward dependencies, GetWindowFromPort() is defined in Window Manager interface: */
/*      pascal WindowRef        GetWindowFromPort(CGrafPtr port); */
/* NewPtr/OpenCPort doesn't work with opaque structures */
/*
 *  CreateNewPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( CGrafPtr )
CreateNewPort(void);


/*
 *  DisposePort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisposePort(CGrafPtr port);



/*
 *  SetQDError()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0.2 and later
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetQDError(OSErr err);


#endif  /* ACCESSOR_CALLS_ARE_FUNCTIONS */

/* Helpful Carbon-only utilities (finally made public)*/

/*
 *  QDLocalToGlobalPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Point * )
QDLocalToGlobalPoint(
  CGrafPtr   port,
  Point *    point);


/*
 *  QDGlobalToLocalPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Point * )
QDGlobalToLocalPoint(
  CGrafPtr   port,
  Point *    point);


/*
 *  QDLocalToGlobalRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Rect * )
QDLocalToGlobalRect(
  CGrafPtr   port,
  Rect *     bounds);


/*
 *  QDGlobalToLocalRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Rect * )
QDGlobalToLocalRect(
  CGrafPtr   port,
  Rect *     bounds);


/*
 *  QDLocalToGlobalRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( RgnHandle )
QDLocalToGlobalRegion(
  CGrafPtr    port,
  RgnHandle   region);


/*
 *  QDGlobalToLocalRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( RgnHandle )
QDGlobalToLocalRegion(
  CGrafPtr    port,
  RgnHandle   region);



/*
   Routines available on Mac OS X to flush buffered window ports...
   These calls do nothing on Mac OS 8/9. QDIsPortBuffered will always return false there.
*/

/*
 *  QDIsPortBuffered()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
QDIsPortBuffered(CGrafPtr port);


/*
 *  QDIsPortBufferDirty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
QDIsPortBufferDirty(CGrafPtr port);


/*
 *  QDFlushPortBuffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
QDFlushPortBuffer(
  CGrafPtr    port,
  RgnHandle   region);      /* can be NULL */


/*
 *  QDGetDirtyRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
QDGetDirtyRegion(
  CGrafPtr    port,
  RgnHandle   rgn);


/*
 *  QDSetDirtyRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
QDSetDirtyRegion(
  CGrafPtr    port,
  RgnHandle   rgn);


/*
 *  QDAddRectToDirtyRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
QDAddRectToDirtyRegion(
  CGrafPtr      inPort,
  const Rect *  inBounds);


/*
 *  QDAddRegionToDirtyRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
QDAddRegionToDirtyRegion(
  CGrafPtr    inPort,
  RgnHandle   inRegion);



/*
 *  CreateCGContextForPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
CreateCGContextForPort(
  CGrafPtr        inPort,
  CGContextRef *  outContext);


/*
 *  ClipCGContextToRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
ClipCGContextToRegion(
  CGContextRef   gc,
  const Rect *   portRect,
  RgnHandle      region);


/*
 *  SyncCGContextOriginWithPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
SyncCGContextOriginWithPort(
  CGContextRef   inContext,
  CGrafPtr       port);


/*
 *  QDBeginCGContext()
 *  
 *  Summary:
 *    Allow CoreGraphics drawing in a CGrafPort
 *  
 *  Discussion:
 *    So far, CreateCGContextForPort() was used to create a CGContext
 *    for CG drawing from a CGrafPort. However, if the current port is
 *    a printing port, the CreateCGContextForPort fails; consequently,
 *    there was no way to mix Quickdraw and CoreGraphics drawing and
 *    still print it. If, instead, the CoreGraphics drawing is
 *    bracketed by QDBeginCGContext/QDEndCGContext calls, the drawing
 *    will also appear in print. There are some restrictions on the
 *    usage of QDBeginCGContext/QDEndCGContext:
 *    - Between QDBeginCGContext and QDEndCGContext, Quickdraw drawing
 *    is disabled; only CoreGraphics drawing is allowed
 *    - QDBeginCGContext/QDEndCGContext calls can not be nested
 *    - QDEndCGContext releases the CGContext returned from
 *    QDBeginCGContext and sets it to NULL.
 *  
 *  Parameters:
 *    
 *    inPort:
 *      The current port
 *    
 *    outContext:
 *      The CGContextRef to be used for CG drawing
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
QDBeginCGContext(
  CGrafPtr        inPort,
  CGContextRef *  outContext);


/*
 *  QDEndCGContext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
QDEndCGContext(
  CGrafPtr        inPort,
  CGContextRef *  inoutContext);


/*
    The following routines are implemented in CarbonLib, and on Mac OS X in QD proper.
    They save the pixel data of a region in a packed format for quick save/restore 
    without using a lot of memory to do a large, hollow region, such as the region
    used when drag hiliting (which is where this is used).
*/

typedef struct OpaqueQDRegionBitsRef*   QDRegionBitsRef;
/*
 *  QDSaveRegionBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( QDRegionBitsRef )
QDSaveRegionBits(RgnHandle region);


/*
 *  QDRestoreRegionBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSStatus )
QDRestoreRegionBits(
  RgnHandle         region,
  QDRegionBitsRef   regionBits);


/*
 *  QDDisposeRegionBits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( OSStatus )
QDDisposeRegionBits(QDRegionBitsRef regionBits);


/*
   Developers need a way to go from a CGDirectDisplay environment to Quickdraw.
   The following is equivalent to CreateNewPort(), but instead of taking the
   portPixMap from the current GDevice, it uses the GDevice corresponding to
   the CGSDisplayID passed in. If the CGSDisplayID is invalid, the mainDevice
   is used instead.
*/
/*
 *  CreateNewPortForCGDisplayID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CGrafPtr )
CreateNewPortForCGDisplayID(UInt32 inCGDisplayID);


/*
   In Mac OS X, developers should be able to turn the WaitCursor (spinning wheel)
   on and off. QDDisplayWaitCursor() keeps track of nested calls.
   Passing FALSE will resume automatic wait cursor operation.
   Call this function only from an application in the foreground.
*/
/*
 *  QDDisplayWaitCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
QDDisplayWaitCursor(Boolean forceWaitCursor);


/*
 *  QDSetPatternOrigin()
 *  
 *  Summary:
 *    Sets the pattern origin for the current port.
 *  
 *  Discussion:
 *    When a QuickDraw drawing operation uses a pattern (either a
 *    black&white pattern or a PixPat), the pattern's image is aligned
 *    with port origin, modified by the pattern origin of the port. For
 *    example, if the background pattern is a 10x10 image, and a
 *    rectangle with coordinates (3, 3, 10, 10) is filled with that
 *    pattern, then only the bottom right 7x7 portion of the pattern
 *    image will be drawn into the rectangle. When drawing a pattern,
 *    QuickDraw always starts with the port origin and then adjusts it
 *    by the pattern origin to determine the actual origin point of
 *    pattern drawing. QDSetPatternOrigin can be used to set the
 *    pattern origin relative to the port origin. It is often used in
 *    conjuction with SetOrigin to maintain the pattern alignment at
 *    (0,0) in a window's content area, regardless of the port origin;
 *    for example, after changing the port's origin to (10,10), an
 *    application might change the port's pattern origin to (-10, -10)
 *    so that patterns are still aligned with the window's content area.
 *  
 *  Parameters:
 *    
 *    origin:
 *      The new pattern origin of the port.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NQD 8.5 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
QDSetPatternOrigin(Point origin);


/*
 *  QDGetPatternOrigin()
 *  
 *  Summary:
 *    Returns the pattern origin of the current port.
 *  
 *  Parameters:
 *    
 *    origin:
 *      On exit, contains the current port's pattern origin.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in NQD 8.5 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
QDGetPatternOrigin(Point * origin);



/*
 *  QDIsNamedPixMapCursorRegistered()
 *  
 *  Summary:
 *    Returns whether a named cursor has already been registered.
 *  
 *  Discussion:
 *    The CoreGraphics Scalable Cursor Registry provides support for
 *    cursors based on PixMaps for crsrData and crsrMask, with sizes up
 *    to 64x64 pixels. Such cursors need to be registered via
 *    QDRegisterNamedPixMapCursor, and can then be set by
 *    QDSetNamedPixMapCursor.
 *  
 *  Parameters:
 *    
 *    name:
 *      (see below at QDRegisterNamedPixMapCursor)
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
QDIsNamedPixMapCursorRegistered(const char name[128]);



/*
 *  QDRegisterNamedPixMapCursor()
 *  
 *  Summary:
 *    Register a new cursor by name
 *  
 *  Discussion:
 *    In order to set a PixMapCursor, it needs to be registered first
 *    by name.
 *  
 *  Parameters:
 *    
 *    crsrData:
 *      (description forthcoming)
 *    
 *    crsrMask:
 *      (description forthcoming)
 *    
 *    hotSpot:
 *      (description forthcoming)
 *    
 *    name:
 *      A naming convention involving the name of your application and
 *      descriptive cursor names or resource IDs is suggested. Cursor
 *      names are 0-terminated C-strings up to a length of 127.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
QDRegisterNamedPixMapCursor(
  PixMapHandle   crsrData,
  PixMapHandle   crsrMask,
  Point          hotSpot,
  const char     name[128]);


/*
 *  QDUnregisterNamedPixMapCursur()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
QDUnregisterNamedPixMapCursur(const char name[128]);


/*
 *  QDSetNamedPixMapCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
QDSetNamedPixMapCursor(const char name[128]);


/*
 *  QDSetCursorScale()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
QDSetCursorScale(float scale);




enum {
  kQDUseDefaultTextRendering    = 0,    /* Sets whatever is specified in system defaults.*/
                                        /* Currently sets kQDUseTrueTypeScalerGlyphs if nothing is specified.*/
  kQDUseTrueTypeScalerGlyphs    = (1 << 0), /* bit 0*/
  kQDUseCGTextRendering         = (1 << 1), /* bit 1*/
  kQDUseCGTextMetrics           = (1 << 2), /* bit 2*/
  kQDSupportedFlags             = kQDUseTrueTypeScalerGlyphs | kQDUseCGTextRendering | kQDUseCGTextMetrics,
  kQDDontChangeFlags            = (long)0xFFFFFFFF /* to request the current state, without changing anything*/
};



/*
 *  QDSwapTextFlags()
 *  
 *  Summary:
 *    Returns current flags and optionally sets new flags.
 *  
 *  Discussion:
 *    Currently, only the flag bits in the enum above are supported.
 *    The typical usage is UInt32 savedFlags =
 *    QDSwapTextFlags(newFlags); // ... draw text under the conditions
 *    of "newFlags" ... (void)QDSwapTextFlags(savedFlags);  // restore
 *    previous setting
 *  
 *  Parameters:
 *    
 *    newFlags:
 *      Use the enums above; example "kQDUseCGTextRendering |
 *      kQDUseCGTextMetrics".
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( UInt32 )
QDSwapTextFlags(UInt32 newFlags);


/*
 *  QDSwapPortTextFlags()
 *  
 *  Summary:
 *    Same as QDSwapTextFlags, but per GrafPort.
 *  
 *  Parameters:
 *    
 *    port:
 *      Settings per port override any global settings. If port ==
 *      NULL, the current port is used.
 *    
 *    newFlags:
 *      As in QDSwapTextFlags, above.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( UInt32 )
QDSwapPortTextFlags(
  CGrafPtr   port,
  UInt32     newFlags);




/* 
    LowMem accessor functions previously in LowMem.h
*/
/*
 *  LMGetScrVRes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 )
LMGetScrVRes(void)                                            TWOWORDINLINE(0x3EB8, 0x0102);


/*
 *  LMSetScrVRes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetScrVRes(SInt16 value)                                    TWOWORDINLINE(0x31DF, 0x0102);


/*
 *  LMGetScrHRes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 )
LMGetScrHRes(void)                                            TWOWORDINLINE(0x3EB8, 0x0104);


/*
 *  LMSetScrHRes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetScrHRes(SInt16 value)                                    TWOWORDINLINE(0x31DF, 0x0104);


/*
 *  LMGetMainDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( GDHandle )
LMGetMainDevice(void)                                         TWOWORDINLINE(0x2EB8, 0x08A4);


/*
 *  LMSetMainDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetMainDevice(GDHandle value)                               TWOWORDINLINE(0x21DF, 0x08A4);


/*
 *  LMGetDeviceList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( GDHandle )
LMGetDeviceList(void)                                         TWOWORDINLINE(0x2EB8, 0x08A8);


/*
 *  LMSetDeviceList()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetDeviceList(GDHandle value)                               TWOWORDINLINE(0x21DF, 0x08A8);


/*
 *  LMGetQDColors()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
LMGetQDColors(void)                                           TWOWORDINLINE(0x2EB8, 0x08B0);


/*
 *  LMSetQDColors()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetQDColors(Handle value)                                   TWOWORDINLINE(0x21DF, 0x08B0);


/*
 *  LMGetWidthListHand()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
LMGetWidthListHand(void)                                      TWOWORDINLINE(0x2EB8, 0x08E4);


/*
 *  LMSetWidthListHand()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetWidthListHand(Handle value)                              TWOWORDINLINE(0x21DF, 0x08E4);


/*
 *  LMGetHiliteMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt8 )
LMGetHiliteMode(void)                                         TWOWORDINLINE(0x1EB8, 0x0938);


/*
 *  LMSetHiliteMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetHiliteMode(UInt8 value)                                  TWOWORDINLINE(0x11DF, 0x0938);


/*
 *  LMGetWidthPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Ptr )
LMGetWidthPtr(void)                                           TWOWORDINLINE(0x2EB8, 0x0B10);


/*
 *  LMSetWidthPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetWidthPtr(Ptr value)                                      TWOWORDINLINE(0x21DF, 0x0B10);


/*
 *  LMGetWidthTabHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
LMGetWidthTabHandle(void)                                     TWOWORDINLINE(0x2EB8, 0x0B2A);


/*
 *  LMSetWidthTabHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetWidthTabHandle(Handle value)                             TWOWORDINLINE(0x21DF, 0x0B2A);


/*
 *  LMGetLastSPExtra()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt32 )
LMGetLastSPExtra(void)                                        TWOWORDINLINE(0x2EB8, 0x0B4C);


/*
 *  LMSetLastSPExtra()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetLastSPExtra(SInt32 value)                                TWOWORDINLINE(0x21DF, 0x0B4C);


/*
 *  LMGetLastFOND()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Handle )
LMGetLastFOND(void)                                           TWOWORDINLINE(0x2EB8, 0x0BC2);


/*
 *  LMSetLastFOND()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetLastFOND(Handle value)                                   TWOWORDINLINE(0x21DF, 0x0BC2);


/*
 *  LMGetFractEnable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt8 )
LMGetFractEnable(void)                                        TWOWORDINLINE(0x1EB8, 0x0BF4);


/*
 *  LMSetFractEnable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetFractEnable(UInt8 value)                                 TWOWORDINLINE(0x11DF, 0x0BF4);


/*
 *  LMGetTheGDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( GDHandle )
LMGetTheGDevice(void)                                         TWOWORDINLINE(0x2EB8, 0x0CC8);


/*
 *  LMSetTheGDevice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetTheGDevice(GDHandle value)                               TWOWORDINLINE(0x21DF, 0x0CC8);



#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
#define LMGetHiliteRGB(hiliteRGBValue)  (*(hiliteRGBValue) = *(RGBColor*)0x0DA0)
#define LMSetHiliteRGB(hiliteRGBValue)  ((* (RGBColor *) 0x0DA0) = *(hiliteRGBValue))
#else
/*
 *  LMGetHiliteRGB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMGetHiliteRGB(RGBColor * hiliteRGBValue);


/*
 *  LMSetHiliteRGB()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetHiliteRGB(const RGBColor * hiliteRGBValue);


#endif  /* TARGET_CPU_68K && !TARGET_RT_MAC_CFM */

/*
 *  LMGetCursorNew()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
LMGetCursorNew(void)                                          TWOWORDINLINE(0x1EB8, 0x08CE);


/*
 *  LMSetCursorNew()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
LMSetCursorNew(Boolean value)                                 TWOWORDINLINE(0x11DF, 0x08CE);



















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

#endif /* __QUICKDRAW__ */

