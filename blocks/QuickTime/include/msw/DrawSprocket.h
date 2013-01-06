/*
     File:       DrawSprocket.h
 
     Contains:   Games Sprockets: DrawSprocket interfaces
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __DRAWSPROCKET__
#define __DRAWSPROCKET__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __QDOFFSCREEN__
#include <QDOffscreen.h>
#endif

#ifndef __DISPLAYS__
#include <Displays.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
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

#if PRAGMA_ENUM_ALWAYSINT
    #if defined(__fourbyteints__) && !__fourbyteints__ 
        #define __DRAWSPROCKET__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __DRAWSPROCKET__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

/*
********************************************************************************
** constants
********************************************************************************
*/
enum DSpDepthMask {
  kDSpDepthMask_1               = 1 << 0,
  kDSpDepthMask_2               = 1 << 1,
  kDSpDepthMask_4               = 1 << 2,
  kDSpDepthMask_8               = 1 << 3,
  kDSpDepthMask_16              = 1 << 4,
  kDSpDepthMask_32              = 1 << 5,
  kDSpDepthMask_All             = -1L
};
typedef enum DSpDepthMask DSpDepthMask;

enum DSpColorNeeds {
  kDSpColorNeeds_DontCare       = 0L,
  kDSpColorNeeds_Request        = 1L,
  kDSpColorNeeds_Require        = 2L
};
typedef enum DSpColorNeeds DSpColorNeeds;

enum DSpContextState {
  kDSpContextState_Active       = 0L,
  kDSpContextState_Paused       = 1L,
  kDSpContextState_Inactive     = 2L
};
typedef enum DSpContextState DSpContextState;

/* kDSpContextOption_QD3DAccel not yet implemented */
enum DSpContextOption {
                                        /*    kDSpContextOption_QD3DAccel       = 1<<0,*/
  kDSpContextOption_PageFlip    = 1 << 1,
  kDSpContextOption_DontSyncVBL = 1 << 2,
  kDSpContextOption_Stereoscopic = 1 << 3
};
typedef enum DSpContextOption DSpContextOption;

enum DSpAltBufferOption {
  kDSpAltBufferOption_RowBytesEqualsWidth = 1 << 0
};
typedef enum DSpAltBufferOption DSpAltBufferOption;

enum DSpBufferKind {
  kDSpBufferKind_Normal         = 0
};
typedef enum DSpBufferKind DSpBufferKind;

enum DSpBlitMode {
  kDSpBlitMode_Plain            = 0,
  kDSpBlitMode_SrcKey           = 1 << 0,
  kDSpBlitMode_DstKey           = 1 << 1,
  kDSpBlitMode_Interpolation    = 1 << 2
};
typedef enum DSpBlitMode DSpBlitMode;

/*
********************************************************************************
** data types
********************************************************************************
*/
typedef struct OpaqueDSpAltBufferReference*  DSpAltBufferReference;
typedef struct OpaqueDSpContextReference*  DSpContextReference;
typedef const struct OpaqueDSpContextReference*  DSpContextReferenceConst;
#define kDSpEveryContext ((DSpContextReference)NULL)
typedef CALLBACK_API_C( Boolean , DSpEventProcPtr )(EventRecord * inEvent);
typedef CALLBACK_API_C( Boolean , DSpCallbackProcPtr )(DSpContextReference inContext, void *inRefCon);
typedef TVECTOR_UPP_TYPE(DSpEventProcPtr)                       DSpEventUPP;
typedef TVECTOR_UPP_TYPE(DSpCallbackProcPtr)                    DSpCallbackUPP;
#if CALL_NOT_IN_CARBON
/*
 *  NewDSpEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( DSpEventUPP )
NewDSpEventUPP(DSpEventProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDSpEventProcInfo = 0x000000D1 };  /* 1_byte Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DSpEventUPP) NewDSpEventUPP(DSpEventProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewDSpEventUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  NewDSpCallbackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( DSpCallbackUPP )
NewDSpCallbackUPP(DSpCallbackProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDSpCallbackProcInfo = 0x000003D1 };  /* 1_byte Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DSpCallbackUPP) NewDSpCallbackUPP(DSpCallbackProcPtr userRoutine) { return userRoutine; }
  #else
    #define NewDSpCallbackUPP(userRoutine) (userRoutine)
  #endif
#endif

/*
 *  DisposeDSpEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeDSpEventUPP(DSpEventUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDSpEventUPP(DSpEventUPP) {}
  #else
      #define DisposeDSpEventUPP(userUPP)
  #endif
#endif

/*
 *  DisposeDSpCallbackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeDSpCallbackUPP(DSpCallbackUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDSpCallbackUPP(DSpCallbackUPP) {}
  #else
      #define DisposeDSpCallbackUPP(userUPP)
  #endif
#endif

/*
 *  InvokeDSpEventUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
InvokeDSpEventUPP(
  EventRecord *  inEvent,
  DSpEventUPP    userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeDSpEventUPP(EventRecord * inEvent, DSpEventUPP userUPP) { return (*userUPP)(inEvent); }
  #else
      #define InvokeDSpEventUPP(inEvent, userUPP) (*userUPP)(inEvent)
  #endif
#endif

/*
 *  InvokeDSpCallbackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
InvokeDSpCallbackUPP(
  DSpContextReference  inContext,
  void *               inRefCon,
  DSpCallbackUPP       userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeDSpCallbackUPP(DSpContextReference inContext, void * inRefCon, DSpCallbackUPP userUPP) { return (*userUPP)(inContext, inRefCon); }
  #else
      #define InvokeDSpCallbackUPP(inContext, inRefCon, userUPP) (*userUPP)(inContext, inRefCon)
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

struct DSpContextAttributes {
  Fixed               frequency;
  UInt32              displayWidth;
  UInt32              displayHeight;
  UInt32              reserved1;
  UInt32              reserved2;
  UInt32              colorNeeds;
  CTabHandle          colorTable;
  OptionBits          contextOptions;
  OptionBits          backBufferDepthMask;
  OptionBits          displayDepthMask;
  UInt32              backBufferBestDepth;
  UInt32              displayBestDepth;
  UInt32              pageCount;
  char                filler[3];
  Boolean             gameMustConfirmSwitch;
  UInt32              reserved3[4];
};
typedef struct DSpContextAttributes     DSpContextAttributes;
typedef DSpContextAttributes *          DSpContextAttributesPtr;
struct DSpAltBufferAttributes {
  UInt32              width;
  UInt32              height;
  DSpAltBufferOption  options;
  UInt32              reserved[4];
};
typedef struct DSpAltBufferAttributes   DSpAltBufferAttributes;
typedef struct DSpBlitInfo              DSpBlitInfo;
typedef CALLBACK_API_C( void , DSpBlitDoneProc )(DSpBlitInfo * info);
struct DSpBlitInfo {
  Boolean             completionFlag;
  char                filler[3];
  DSpBlitDoneProc     completionProc;
  DSpContextReference  srcContext;
  CGrafPtr            srcBuffer;
  Rect                srcRect;
  UInt32              srcKey;

  DSpContextReference  dstContext;
  CGrafPtr            dstBuffer;
  Rect                dstRect;
  UInt32              dstKey;

  DSpBlitMode         mode;
  UInt32              reserved[4];
};


typedef DSpBlitInfo *                   DSpBlitInfoPtr;
/*
********************************************************************************
** function prototypes
********************************************************************************
*/

/*
** global operations
*/
/*
 *  DSpStartup()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpStartup(void);


/*
 *  DSpShutdown()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpShutdown(void);


/*
 *  DSpGetVersion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.7 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( NumVersion )
DSpGetVersion(void);


/*
 *  DSpGetFirstContext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpGetFirstContext(
  DisplayIDType          inDisplayID,
  DSpContextReference *  outContext);


/*
 *  DSpGetNextContext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpGetNextContext(
  DSpContextReference    inCurrentContext,
  DSpContextReference *  outContext);


/*
 *  DSpGetCurrentContext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.7 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpGetCurrentContext(
  DisplayIDType          inDisplayID,
  DSpContextReference *  outContext);


/*
 *  DSpFindBestContext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpFindBestContext(
  DSpContextAttributesPtr   inDesiredAttributes,
  DSpContextReference *     outContext);


/*
 *  DSpFindBestContextOnDisplayID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.7 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpFindBestContextOnDisplayID(
  DSpContextAttributesPtr   inDesiredAttributes,
  DSpContextReference *     outContext,
  DisplayIDType             inDisplayID);


#if CALL_NOT_IN_CARBON
/*
 *  DSpCanUserSelectContext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpCanUserSelectContext(
  DSpContextAttributesPtr   inDesiredAttributes,
  Boolean *                 outUserCanSelectContext);


/*
 *  DSpUserSelectContext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpUserSelectContext(
  DSpContextAttributesPtr   inDesiredAttributes,
  DisplayIDType             inDialogDisplayLocation,
  DSpEventUPP               inEventProc,
  DSpContextReference *     outContext);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  DSpProcessEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpProcessEvent(
  EventRecord *  inEvent,
  Boolean *      outEventWasProcessed);


/*
 *  DSpSetBlankingColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpSetBlankingColor(const RGBColor * inRGBColor);


/*
 *  DSpSetDebugMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpSetDebugMode(Boolean inDebugMode);


/*
 *  DSpFindContextFromPoint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpFindContextFromPoint(
  Point                  inGlobalPoint,
  DSpContextReference *  outContext);


/*
 *  DSpGetMouse()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpGetMouse(Point * outGlobalPoint);


/*
** alternate buffer operations
*/
#if CALL_NOT_IN_CARBON
/*
 *  DSpAltBuffer_New()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpAltBuffer_New(
  DSpContextReference       inContext,
  Boolean                   inVRAMBuffer,
  DSpAltBufferAttributes *  inAttributes,
  DSpAltBufferReference *   outAltBuffer);


/*
 *  DSpAltBuffer_Dispose()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpAltBuffer_Dispose(DSpAltBufferReference inAltBuffer);


/*
 *  DSpAltBuffer_InvalRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpAltBuffer_InvalRect(
  DSpAltBufferReference   inAltBuffer,
  const Rect *            inInvalidRect);


/*
 *  DSpAltBuffer_GetCGrafPtr()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpAltBuffer_GetCGrafPtr(
  DSpAltBufferReference   inAltBuffer,
  DSpBufferKind           inBufferKind,
  CGrafPtr *              outCGrafPtr,
  GDHandle *              outGDevice);


/*
** context operations
*/
/* general */
#endif  /* CALL_NOT_IN_CARBON */

/*
 *  DSpContext_GetAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_GetAttributes(
  DSpContextReferenceConst   inContext,
  DSpContextAttributesPtr    outAttributes);


/*
 *  DSpContext_Reserve()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_Reserve(
  DSpContextReference       inContext,
  DSpContextAttributesPtr   inDesiredAttributes);


/*
 *  DSpContext_Queue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.7 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_Queue(
  DSpContextReference       inParentContext,
  DSpContextReference       inChildContext,
  DSpContextAttributesPtr   inDesiredAttributes);


/*
 *  DSpContext_Switch()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.7 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_Switch(
  DSpContextReference   inOldContext,
  DSpContextReference   inNewContext);


/*
 *  DSpContext_Release()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_Release(DSpContextReference inContext);


/*
 *  DSpContext_Dispose()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_Dispose(DSpContextReference inContext);


/*
 *  DSpContext_GetDisplayID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_GetDisplayID(
  DSpContextReferenceConst   inContext,
  DisplayIDType *            outDisplayID);


/*
 *  DSpContext_GlobalToLocal()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_GlobalToLocal(
  DSpContextReferenceConst   inContext,
  Point *                    ioPoint);


/*
 *  DSpContext_LocalToGlobal()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_LocalToGlobal(
  DSpContextReferenceConst   inContext,
  Point *                    ioPoint);


#if CALL_NOT_IN_CARBON
/*
 *  DSpContext_SetVBLProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpContext_SetVBLProc(
  DSpContextReference   inContext,
  DSpCallbackUPP        inProcPtr,
  void *                inRefCon);


/*
 *  DSpContext_GetFlattenedSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpContext_GetFlattenedSize(
  DSpContextReference   inContext,
  UInt32 *              outFlatContextSize);


/*
 *  DSpContext_Flatten()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpContext_Flatten(
  DSpContextReference   inContext,
  void *                outFlatContext);


/*
 *  DSpContext_Restore()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpContext_Restore(
  void *                 inFlatContext,
  DSpContextReference *  outRestoredContext);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  DSpContext_GetMonitorFrequency()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_GetMonitorFrequency(
  DSpContextReferenceConst   inContext,
  Fixed *                    outFrequency);


#if CALL_NOT_IN_CARBON
/*
 *  DSpContext_SetMaxFrameRate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpContext_SetMaxFrameRate(
  DSpContextReference   inContext,
  UInt32                inMaxFPS);


/*
 *  DSpContext_GetMaxFrameRate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpContext_GetMaxFrameRate(
  DSpContextReferenceConst   inContext,
  UInt32 *                   outMaxFPS);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  DSpContext_SetState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_SetState(
  DSpContextReference   inContext,
  DSpContextState       inState);


/*
 *  DSpContext_GetState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_GetState(
  DSpContextReferenceConst   inContext,
  DSpContextState *          outState);


/*
 *  DSpContext_IsBusy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_IsBusy(
  DSpContextReferenceConst   inContext,
  Boolean *                  outBusyFlag);


/* dirty rectangles */
#if CALL_NOT_IN_CARBON
/*
 *  DSpContext_SetDirtyRectGridSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpContext_SetDirtyRectGridSize(
  DSpContextReference   inContext,
  UInt32                inCellPixelWidth,
  UInt32                inCellPixelHeight);


/*
 *  DSpContext_GetDirtyRectGridSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpContext_GetDirtyRectGridSize(
  DSpContextReferenceConst   inContext,
  UInt32 *                   outCellPixelWidth,
  UInt32 *                   outCellPixelHeight);


/*
 *  DSpContext_GetDirtyRectGridUnits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpContext_GetDirtyRectGridUnits(
  DSpContextReferenceConst   inContext,
  UInt32 *                   outCellPixelWidth,
  UInt32 *                   outCellPixelHeight);


/*
 *  DSpContext_InvalBackBufferRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpContext_InvalBackBufferRect(
  DSpContextReference   inContext,
  const Rect *          inRect);


/* underlays */
/*
 *  DSpContext_SetUnderlayAltBuffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpContext_SetUnderlayAltBuffer(
  DSpContextReference     inContext,
  DSpAltBufferReference   inNewUnderlay);


/*
 *  DSpContext_GetUnderlayAltBuffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpContext_GetUnderlayAltBuffer(
  DSpContextReferenceConst   inContext,
  DSpAltBufferReference *    outUnderlay);


/* gamma */
#endif  /* CALL_NOT_IN_CARBON */

/*
 *  DSpContext_FadeGammaOut()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_FadeGammaOut(
  DSpContextReference   inContext,
  RGBColor *            inZeroIntensityColor);


/*
 *  DSpContext_FadeGammaIn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_FadeGammaIn(
  DSpContextReference   inContext,
  RGBColor *            inZeroIntensityColor);


/*
 *  DSpContext_FadeGamma()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_FadeGamma(
  DSpContextReference   inContext,
  SInt32                inPercentOfOriginalIntensity,
  RGBColor *            inZeroIntensityColor);


/* buffering */
/*
 *  DSpContext_SwapBuffers()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_SwapBuffers(
  DSpContextReference   inContext,
  DSpCallbackUPP        inBusyProc,
  void *                inUserRefCon);


/*
 *  DSpContext_GetBackBuffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_GetBackBuffer(
  DSpContextReference   inContext,
  DSpBufferKind         inBufferKind,
  CGrafPtr *            outBackBuffer);


/*
 *  DSpContext_GetFrontBuffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_GetFrontBuffer(
  DSpContextReferenceConst   inContext,
  CGrafPtr *                 outFrontBuffer);


/* clut operations */
/*
 *  DSpContext_SetCLUTEntries()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_SetCLUTEntries(
  DSpContextReference   inContext,
  const ColorSpec *     inEntries,
  UInt16                inStartingEntry,
  UInt16                inLastEntry);


/*
 *  DSpContext_GetCLUTEntries()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
DSpContext_GetCLUTEntries(
  DSpContextReferenceConst   inContext,
  ColorSpec *                outEntries,
  UInt16                     inStartingEntry,
  UInt16                     inLastEntry);


/* blit operations */
#if CALL_NOT_IN_CARBON
/*
 *  DSpBlit_Faster()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpBlit_Faster(
  DSpBlitInfoPtr   inBlitInfo,
  Boolean          inAsyncFlag);


/*
 *  DSpBlit_Fastest()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DrawSprocketLib 1.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSStatus )
DSpBlit_Fastest(
  DSpBlitInfoPtr   inBlitInfo,
  Boolean          inAsyncFlag);



#endif  /* CALL_NOT_IN_CARBON */


#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __DRAWSPROCKET__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__DRAWSPROCKET__RESTORE_PACKED_ENUMS)
    #pragma options(pack_enums)
#endif

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

#endif /* __DRAWSPROCKET__ */

