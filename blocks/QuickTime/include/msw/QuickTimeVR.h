/*
     File:       QuickTimeVR.h
 
     Contains:   QuickTime VR interfaces
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1997-2007 by Apple Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __QUICKTIMEVR__
#define __QUICKTIMEVR__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MOVIES__
#include <Movies.h>
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

/* QuickTime is not available to 64-bit clients */

#if !__LP64__

typedef struct OpaqueQTVRInstance*      QTVRInstance;

/* Released API Version numbers */
#define kQTVRAPIMajorVersion05  (0x05)
#define kQTVRAPIMajorVersion02  (0x02)
#define kQTVRAPIMinorVersion00  (0x00)
#define kQTVRAPIMinorVersion01  (0x01)
#define kQTVRAPIMinorVersion10  (0x10)
#define kQTVRAPIMinorVersion20  (0x20)

/* Version numbers for the API described in this header */
#define kQTVRAPIMajorVersion kQTVRAPIMajorVersion05
#define kQTVRAPIMinorVersion kQTVRAPIMinorVersion00


enum {
  kQTVRControllerSubType        = FOUR_CHAR_CODE('ctyp'),
  kQTVRQTVRType                 = FOUR_CHAR_CODE('qtvr'),
  kQTVRPanoramaType             = FOUR_CHAR_CODE('pano'),
  kQTVRObjectType               = FOUR_CHAR_CODE('obje'),
  kQTVROldPanoType              = FOUR_CHAR_CODE('STpn'), /* Used in QTVR 1.0 release*/
  kQTVROldObjectType            = FOUR_CHAR_CODE('stna') /* Used in QTVR 1.0 release*/
};

#define kQTVRUnknownType '\?\?\?\?' /* Unknown node type */
/* QTVR hot spot types*/
enum {
  kQTVRHotSpotLinkType          = FOUR_CHAR_CODE('link'),
  kQTVRHotSpotURLType           = FOUR_CHAR_CODE('url '),
  kQTVRHotSpotUndefinedType     = FOUR_CHAR_CODE('undf')
};

/* Special Values for nodeID in QTVRGoToNodeID*/
enum {
  kQTVRCurrentNode              = 0,
  kQTVRPreviousNode             = (long)0x80000000,
  kQTVRDefaultNode              = (long)0x80000001
};

/* Panorama correction modes used for the kQTVRImagingCorrection imaging property*/
enum {
  kQTVRNoCorrection             = 0,
  kQTVRPartialCorrection        = 1,
  kQTVRFullCorrection           = 2
};

/* Imaging Modes used by QTVRSetImagingProperty, QTVRGetImagingProperty, QTVRUpdate, QTVRBeginUpdate*/
typedef UInt32 QTVRImagingMode;
enum {
  kQTVRStatic                   = 1,
  kQTVRMotion                   = 2,
  kQTVRCurrentMode              = 0,    /* Special Value for QTVRUpdate*/
  kQTVRAllModes                 = 100   /* Special value for QTVRSetProperty*/
};

/* Imaging Properties used by QTVRSetImagingProperty, QTVRGetImagingProperty*/
enum {
  kQTVRImagingCorrection        = 1,
  kQTVRImagingQuality           = 2,
  kQTVRImagingDirectDraw        = 3,
  kQTVRImagingCurrentMode       = 100   /* Get Only*/
};

/* OR the above with kImagingDefaultValue to get/set the default value*/
enum {
  kImagingDefaultValue          = (long)0x80000000
};

/* Transition Types used by QTVRSetTransitionProperty, QTVREnableTransition*/
enum {
  kQTVRTransitionSwing          = 1
};

/* Transition Properties QTVRSetTransitionProperty*/
enum {
  kQTVRTransitionSpeed          = 1,
  kQTVRTransitionDirection      = 2
};

/* Constraint values used to construct value returned by GetConstraintStatus*/
enum {
  kQTVRUnconstrained            = 0L,
  kQTVRCantPanLeft              = 1L << 0,
  kQTVRCantPanRight             = 1L << 1,
  kQTVRCantPanUp                = 1L << 2,
  kQTVRCantPanDown              = 1L << 3,
  kQTVRCantZoomIn               = 1L << 4,
  kQTVRCantZoomOut              = 1L << 5,
  kQTVRCantTranslateLeft        = 1L << 6,
  kQTVRCantTranslateRight       = 1L << 7,
  kQTVRCantTranslateUp          = 1L << 8,
  kQTVRCantTranslateDown        = 1L << 9
};

/* Object-only mouse mode values used to construct value returned by QTVRGetCurrentMouseMode*/
enum {
  kQTVRPanning                  = 1L << 0, /* standard objects, "object only" controllers*/
  kQTVRTranslating              = 1L << 1, /* all objects*/
  kQTVRZooming                  = 1L << 2, /* all objects*/
  kQTVRScrolling                = 1L << 3, /* standard object arrow scrollers and joystick object*/
  kQTVRSelecting                = 1L << 4 /* object absolute controller*/
};

/* Properties for use with QTVRSetInteractionProperty/GetInteractionProperty*/
enum {
  kQTVRInteractionMouseClickHysteresis = 1, /* pixels within which the mouse is considered not to have moved (UInt16)*/
  kQTVRInteractionMouseClickTimeout = 2, /* ticks after which a mouse click times out and turns into panning (UInt32)*/
  kQTVRInteractionPanTiltSpeed  = 3,    /* control the relative pan/tilt speed from 1 (slowest) to 10 (fastest). (UInt32) Default is 5;*/
  kQTVRInteractionZoomSpeed     = 4,    /* control the relative zooming speed from 1 (slowest) to 10 (fastest). (UInt32) Default is 5;*/
  kQTVRInteractionTranslateOnMouseDown = 101, /* Holding MouseDown with this setting translates zoomed object movies (Boolean)*/
  kQTVRInteractionMouseMotionScale = 102, /* The maximum angle of rotation caused by dragging across the display window. (* float)*/
  kQTVRInteractionNudgeMode     = 103   /* A QTVRNudgeMode: rotate, translate, or the same as the current mouse mode. Requires QTVR 2.1*/
};

/* OR the above with kQTVRInteractionDefaultValue to get/set the default value*/
enum {
  kQTVRInteractionDefaultValue  = (long)0x80000000
};


/* Geometry constants used in QTVRSetBackBufferPrefs, QTVRGetBackBufferSettings, QTVRGetBackBufferMemInfo*/
enum {
  kQTVRUseMovieGeometry         = 0,
  kQTVRVerticalCylinder         = FOUR_CHAR_CODE('vcyl'),
  kQTVRHorizontalCylinder       = FOUR_CHAR_CODE('hcyl'),
  kQTVRCube                     = FOUR_CHAR_CODE('cube')
};

/* Resolution constants used in QTVRSetBackBufferPrefs, QTVRGetBackBufferSettings, QTVRGetBackBufferMemInfo*/
enum {
  kQTVRDefaultRes               = 0,
  kQTVRFullRes                  = 1L << 0,
  kQTVRHalfRes                  = 1L << 1,
  kQTVRQuarterRes               = 1L << 2
};

/* QTVR-specific pixelFormat constants used in QTVRSetBackBufferPrefs, QTVRGetBackBufferSettings, QTVRGetBackBufferMemInfo*/
enum {
  kQTVRUseMovieDepth            = 0
};

/* Cache Size Pref constants used in QTVRSetBackBufferPrefs, QTVRGetBackBufferSettings*/
enum {
  kQTVRMinimumCache             = -1,
  kQTVRSuggestedCache           = 0,
  kQTVRFullCache                = 1
};

/* Angular units used by QTVRSetAngularUnits*/
typedef UInt32 QTVRAngularUnits;
enum {
  kQTVRDegrees                  = 0,
  kQTVRRadians                  = 1
};

/* Values for enableFlag parameter in QTVREnableHotSpot*/
enum {
  kQTVRHotSpotID                = 0,
  kQTVRHotSpotType              = 1,
  kQTVRAllHotSpots              = 2
};

/* Values for viewParameter for QTVRSet/GetViewParameter*/
enum {
  kQTVRPanAngle                 = 0x0100, /* default units; &float, &float*/
  kQTVRTiltAngle                = 0x0101, /* default units; &float, &float*/
  kQTVRFieldOfViewAngle         = 0x0103, /* default units; &float, &float*/
  kQTVRViewCenter               = 0x0104, /* pixels (per object movies); &QTVRFloatPoint, &QTVRFloatPoint*/
  kQTVRHotSpotsVisible          = 0x0200 /* Boolean, &Boolean*/
};

/* Values for flagsIn for QTVRSet/GetViewParameter*/
enum {
  kQTVRValueIsRelative          = 1L << 0, /* Is the value absolute or relative to the current value?*/
  kQTVRValueIsRate              = 1L << 1, /* Is the value absolute or a rate of change to be applied?*/
  kQTVRValueIsUserPrefRelative  = 1L << 2 /* Is the value a percentage of the user rate pref?*/
};

/* Values for kind parameter in QTVRGet/SetConstraints, QTVRGetViewingLimits*/
enum {
  kQTVRPan                      = 0,
  kQTVRTilt                     = 1,
  kQTVRFieldOfView              = 2,
  kQTVRViewCenterH              = 4,    /* WrapAndConstrain only*/
  kQTVRViewCenterV              = 5     /* WrapAndConstrain only*/
};

/* Values for setting parameter in QTVRSetAnimationSetting, QTVRGetAnimationSetting*/
typedef UInt32 QTVRObjectAnimationSetting;
enum {
                                        /* View Frame Animation Settings*/
  kQTVRPalindromeViewFrames     = 1,
  kQTVRStartFirstViewFrame      = 2,
  kQTVRDontLoopViewFrames       = 3,
  kQTVRPlayEveryViewFrame       = 4,    /* Requires QTVR 2.1 (kQTVRAPIMajorVersion02 + kQTVRAPIMinorVersion10)*/
                                        /* View Animation Settings*/
  kQTVRSyncViewToFrameRate      = 16,
  kQTVRPalindromeViews          = 17,
  kQTVRPlayStreamingViews       = 18    /* Requires QTVR 2.1 (kQTVRAPIMajorVersion02 + kQTVRAPIMinorVersion10)*/
};

typedef UInt32 QTVRControlSetting;
enum {
  kQTVRWrapPan                  = 1,
  kQTVRWrapTilt                 = 2,
  kQTVRCanZoom                  = 3,
  kQTVRReverseHControl          = 4,
  kQTVRReverseVControl          = 5,
  kQTVRSwapHVControl            = 6,
  kQTVRTranslation              = 7
};

typedef UInt32 QTVRViewStateType;
enum {
  kQTVRDefault                  = 0,
  kQTVRCurrent                  = 2,
  kQTVRMouseDown                = 3
};

typedef UInt32 QTVRNudgeControl;
enum {
  kQTVRRight                    = 0,
  kQTVRUpRight                  = 45,
  kQTVRUp                       = 90,
  kQTVRUpLeft                   = 135,
  kQTVRLeft                     = 180,
  kQTVRDownLeft                 = 225,
  kQTVRDown                     = 270,
  kQTVRDownRight                = 315
};

typedef UInt32 QTVRNudgeMode;
enum {
  kQTVRNudgeRotate              = 0,
  kQTVRNudgeTranslate           = 1,
  kQTVRNudgeSameAsMouse         = 2
};


/* Flags to control elements of the QTVR control bar (set via mcActionSetFlags) */
enum {
  mcFlagQTVRSuppressBackBtn     = 1L << 16,
  mcFlagQTVRSuppressZoomBtns    = 1L << 17,
  mcFlagQTVRSuppressHotSpotBtn  = 1L << 18,
  mcFlagQTVRSuppressTranslateBtn = 1L << 19,
  mcFlagQTVRSuppressHelpText    = 1L << 20,
  mcFlagQTVRSuppressHotSpotNames = 1L << 21,
  mcFlagQTVRExplicitFlagSet     = 1L << 31 /* bits 0->30 should be interpreted as "explicit on" for the corresponding suppression bits*/
};

/* Cursor types used in type field of QTVRCursorRecord*/
enum {
  kQTVRUseDefaultCursor         = 0,
  kQTVRStdCursorType            = 1,
  kQTVRColorCursorType          = 2
};

/* Values for flags parameter in QTVRMouseOverHotSpot callback*/
enum {
  kQTVRHotSpotEnter             = 0,
  kQTVRHotSpotWithin            = 1,
  kQTVRHotSpotLeave             = 2
};

/* Values for flags parameter in QTVRSetPrescreenImagingCompleteProc*/
enum {
  kQTVRPreScreenEveryIdle       = 1L << 0 /* Requires QTVR 2.1 (kQTVRAPIMajorVersion02 + kQTVRAPIMinorVersion10)*/
};

/* Values for flags field of areasOfInterest in QTVRSetBackBufferImagingProc*/
enum {
  kQTVRBackBufferEveryUpdate    = 1L << 0,
  kQTVRBackBufferEveryIdle      = 1L << 1,
  kQTVRBackBufferAlwaysRefresh  = 1L << 2,
  kQTVRBackBufferHorizontal     = 1L << 3 /* Requires that backbuffer proc be long-rowBytes aware (gestaltQDHasLongRowBytes)*/
};

/* Values for flagsIn parameter in QTVRBackBufferImaging callback*/
enum {
  kQTVRBackBufferRectVisible    = 1L << 0,
  kQTVRBackBufferWasRefreshed   = 1L << 1
};

/* Values for flagsOut parameter in QTVRBackBufferImaging callback*/
enum {
  kQTVRBackBufferFlagDidDraw    = 1L << 0,
  kQTVRBackBufferFlagLastFlag   = 1L << 31
};

/* QTVRCursorRecord used in QTVRReplaceCursor*/
struct QTVRCursorRecord {
  UInt16              theType;                /* field was previously named "type"*/
  SInt16              rsrcID;
  Handle              handle;
};
typedef struct QTVRCursorRecord         QTVRCursorRecord;
struct QTVRFloatPoint {
  float               x;
  float               y;
};
typedef struct QTVRFloatPoint           QTVRFloatPoint;
/* Struct used for areasOfInterest parameter in QTVRSetBackBufferImagingProc*/
struct QTVRAreaOfInterest {
  float               panAngle;
  float               tiltAngle;
  float               width;
  float               height;
  UInt32              flags;
};
typedef struct QTVRAreaOfInterest       QTVRAreaOfInterest;
/*
  =================================================================================================
   Callback routines 
  -------------------------------------------------------------------------------------------------
*/

typedef CALLBACK_API( OSErr , QTVRLeavingNodeProcPtr )(QTVRInstance qtvr, UInt32 fromNodeID, UInt32 toNodeID, Boolean *cancel, SInt32 refCon);
typedef CALLBACK_API( OSErr , QTVREnteringNodeProcPtr )(QTVRInstance qtvr, UInt32 nodeID, SInt32 refCon);
typedef CALLBACK_API( OSErr , QTVRMouseOverHotSpotProcPtr )(QTVRInstance qtvr, UInt32 hotSpotID, UInt32 flags, SInt32 refCon);
typedef CALLBACK_API( OSErr , QTVRImagingCompleteProcPtr )(QTVRInstance qtvr, SInt32 refCon);
typedef CALLBACK_API( OSErr , QTVRBackBufferImagingProcPtr )(QTVRInstance qtvr, Rect *drawRect, UInt16 areaIndex, UInt32 flagsIn, UInt32 *flagsOut, SInt32 refCon);
typedef STACK_UPP_TYPE(QTVRLeavingNodeProcPtr)                  QTVRLeavingNodeUPP;
typedef STACK_UPP_TYPE(QTVREnteringNodeProcPtr)                 QTVREnteringNodeUPP;
typedef STACK_UPP_TYPE(QTVRMouseOverHotSpotProcPtr)             QTVRMouseOverHotSpotUPP;
typedef STACK_UPP_TYPE(QTVRImagingCompleteProcPtr)              QTVRImagingCompleteUPP;
typedef STACK_UPP_TYPE(QTVRBackBufferImagingProcPtr)            QTVRBackBufferImagingUPP;
/*
 *  NewQTVRLeavingNodeUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QTVRLeavingNodeUPP )
NewQTVRLeavingNodeUPP(QTVRLeavingNodeProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQTVRLeavingNodeProcInfo = 0x0000FFE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QTVRLeavingNodeUPP) NewQTVRLeavingNodeUPP(QTVRLeavingNodeProcPtr userRoutine) { return (QTVRLeavingNodeUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTVRLeavingNodeProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQTVRLeavingNodeUPP(userRoutine) (QTVRLeavingNodeUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTVRLeavingNodeProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQTVREnteringNodeUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QTVREnteringNodeUPP )
NewQTVREnteringNodeUPP(QTVREnteringNodeProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQTVREnteringNodeProcInfo = 0x00000FE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QTVREnteringNodeUPP) NewQTVREnteringNodeUPP(QTVREnteringNodeProcPtr userRoutine) { return (QTVREnteringNodeUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTVREnteringNodeProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQTVREnteringNodeUPP(userRoutine) (QTVREnteringNodeUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTVREnteringNodeProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQTVRMouseOverHotSpotUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QTVRMouseOverHotSpotUPP )
NewQTVRMouseOverHotSpotUPP(QTVRMouseOverHotSpotProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQTVRMouseOverHotSpotProcInfo = 0x00003FE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QTVRMouseOverHotSpotUPP) NewQTVRMouseOverHotSpotUPP(QTVRMouseOverHotSpotProcPtr userRoutine) { return (QTVRMouseOverHotSpotUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTVRMouseOverHotSpotProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQTVRMouseOverHotSpotUPP(userRoutine) (QTVRMouseOverHotSpotUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTVRMouseOverHotSpotProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQTVRImagingCompleteUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QTVRImagingCompleteUPP )
NewQTVRImagingCompleteUPP(QTVRImagingCompleteProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQTVRImagingCompleteProcInfo = 0x000003E0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QTVRImagingCompleteUPP) NewQTVRImagingCompleteUPP(QTVRImagingCompleteProcPtr userRoutine) { return (QTVRImagingCompleteUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTVRImagingCompleteProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQTVRImagingCompleteUPP(userRoutine) (QTVRImagingCompleteUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTVRImagingCompleteProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewQTVRBackBufferImagingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QTVRBackBufferImagingUPP )
NewQTVRBackBufferImagingUPP(QTVRBackBufferImagingProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQTVRBackBufferImagingProcInfo = 0x0003FBE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 2_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QTVRBackBufferImagingUPP) NewQTVRBackBufferImagingUPP(QTVRBackBufferImagingProcPtr userRoutine) { return (QTVRBackBufferImagingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTVRBackBufferImagingProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQTVRBackBufferImagingUPP(userRoutine) (QTVRBackBufferImagingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTVRBackBufferImagingProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeQTVRLeavingNodeUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQTVRLeavingNodeUPP(QTVRLeavingNodeUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQTVRLeavingNodeUPP(QTVRLeavingNodeUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQTVRLeavingNodeUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQTVREnteringNodeUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQTVREnteringNodeUPP(QTVREnteringNodeUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQTVREnteringNodeUPP(QTVREnteringNodeUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQTVREnteringNodeUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQTVRMouseOverHotSpotUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQTVRMouseOverHotSpotUPP(QTVRMouseOverHotSpotUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQTVRMouseOverHotSpotUPP(QTVRMouseOverHotSpotUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQTVRMouseOverHotSpotUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQTVRImagingCompleteUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQTVRImagingCompleteUPP(QTVRImagingCompleteUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQTVRImagingCompleteUPP(QTVRImagingCompleteUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQTVRImagingCompleteUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeQTVRBackBufferImagingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQTVRBackBufferImagingUPP(QTVRBackBufferImagingUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQTVRBackBufferImagingUPP(QTVRBackBufferImagingUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQTVRBackBufferImagingUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeQTVRLeavingNodeUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeQTVRLeavingNodeUPP(
  QTVRInstance        qtvr,
  UInt32              fromNodeID,
  UInt32              toNodeID,
  Boolean *           cancel,
  SInt32              refCon,
  QTVRLeavingNodeUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeQTVRLeavingNodeUPP(QTVRInstance qtvr, UInt32 fromNodeID, UInt32 toNodeID, Boolean * cancel, SInt32 refCon, QTVRLeavingNodeUPP userUPP) { return (OSErr)CALL_FIVE_PARAMETER_UPP(userUPP, uppQTVRLeavingNodeProcInfo, qtvr, fromNodeID, toNodeID, cancel, refCon); }
  #else
    #define InvokeQTVRLeavingNodeUPP(qtvr, fromNodeID, toNodeID, cancel, refCon, userUPP) (OSErr)CALL_FIVE_PARAMETER_UPP((userUPP), uppQTVRLeavingNodeProcInfo, (qtvr), (fromNodeID), (toNodeID), (cancel), (refCon))
  #endif
#endif

/*
 *  InvokeQTVREnteringNodeUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeQTVREnteringNodeUPP(
  QTVRInstance         qtvr,
  UInt32               nodeID,
  SInt32               refCon,
  QTVREnteringNodeUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeQTVREnteringNodeUPP(QTVRInstance qtvr, UInt32 nodeID, SInt32 refCon, QTVREnteringNodeUPP userUPP) { return (OSErr)CALL_THREE_PARAMETER_UPP(userUPP, uppQTVREnteringNodeProcInfo, qtvr, nodeID, refCon); }
  #else
    #define InvokeQTVREnteringNodeUPP(qtvr, nodeID, refCon, userUPP) (OSErr)CALL_THREE_PARAMETER_UPP((userUPP), uppQTVREnteringNodeProcInfo, (qtvr), (nodeID), (refCon))
  #endif
#endif

/*
 *  InvokeQTVRMouseOverHotSpotUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeQTVRMouseOverHotSpotUPP(
  QTVRInstance             qtvr,
  UInt32                   hotSpotID,
  UInt32                   flags,
  SInt32                   refCon,
  QTVRMouseOverHotSpotUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeQTVRMouseOverHotSpotUPP(QTVRInstance qtvr, UInt32 hotSpotID, UInt32 flags, SInt32 refCon, QTVRMouseOverHotSpotUPP userUPP) { return (OSErr)CALL_FOUR_PARAMETER_UPP(userUPP, uppQTVRMouseOverHotSpotProcInfo, qtvr, hotSpotID, flags, refCon); }
  #else
    #define InvokeQTVRMouseOverHotSpotUPP(qtvr, hotSpotID, flags, refCon, userUPP) (OSErr)CALL_FOUR_PARAMETER_UPP((userUPP), uppQTVRMouseOverHotSpotProcInfo, (qtvr), (hotSpotID), (flags), (refCon))
  #endif
#endif

/*
 *  InvokeQTVRImagingCompleteUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeQTVRImagingCompleteUPP(
  QTVRInstance            qtvr,
  SInt32                  refCon,
  QTVRImagingCompleteUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeQTVRImagingCompleteUPP(QTVRInstance qtvr, SInt32 refCon, QTVRImagingCompleteUPP userUPP) { return (OSErr)CALL_TWO_PARAMETER_UPP(userUPP, uppQTVRImagingCompleteProcInfo, qtvr, refCon); }
  #else
    #define InvokeQTVRImagingCompleteUPP(qtvr, refCon, userUPP) (OSErr)CALL_TWO_PARAMETER_UPP((userUPP), uppQTVRImagingCompleteProcInfo, (qtvr), (refCon))
  #endif
#endif

/*
 *  InvokeQTVRBackBufferImagingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeQTVRBackBufferImagingUPP(
  QTVRInstance              qtvr,
  Rect *                    drawRect,
  UInt16                    areaIndex,
  UInt32                    flagsIn,
  UInt32 *                  flagsOut,
  SInt32                    refCon,
  QTVRBackBufferImagingUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeQTVRBackBufferImagingUPP(QTVRInstance qtvr, Rect * drawRect, UInt16 areaIndex, UInt32 flagsIn, UInt32 * flagsOut, SInt32 refCon, QTVRBackBufferImagingUPP userUPP) { return (OSErr)CALL_SIX_PARAMETER_UPP(userUPP, uppQTVRBackBufferImagingProcInfo, qtvr, drawRect, areaIndex, flagsIn, flagsOut, refCon); }
  #else
    #define InvokeQTVRBackBufferImagingUPP(qtvr, drawRect, areaIndex, flagsIn, flagsOut, refCon, userUPP) (OSErr)CALL_SIX_PARAMETER_UPP((userUPP), uppQTVRBackBufferImagingProcInfo, (qtvr), (drawRect), (areaIndex), (flagsIn), (flagsOut), (refCon))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewQTVRLeavingNodeProc(userRoutine)                 NewQTVRLeavingNodeUPP(userRoutine)
    #define NewQTVREnteringNodeProc(userRoutine)                NewQTVREnteringNodeUPP(userRoutine)
    #define NewQTVRMouseOverHotSpotProc(userRoutine)            NewQTVRMouseOverHotSpotUPP(userRoutine)
    #define NewQTVRImagingCompleteProc(userRoutine)             NewQTVRImagingCompleteUPP(userRoutine)
    #define NewQTVRBackBufferImagingProc(userRoutine)           NewQTVRBackBufferImagingUPP(userRoutine)
    #define CallQTVRLeavingNodeProc(userRoutine, qtvr, fromNodeID, toNodeID, cancel, refCon) InvokeQTVRLeavingNodeUPP(qtvr, fromNodeID, toNodeID, cancel, refCon, userRoutine)
    #define CallQTVREnteringNodeProc(userRoutine, qtvr, nodeID, refCon) InvokeQTVREnteringNodeUPP(qtvr, nodeID, refCon, userRoutine)
    #define CallQTVRMouseOverHotSpotProc(userRoutine, qtvr, hotSpotID, flags, refCon) InvokeQTVRMouseOverHotSpotUPP(qtvr, hotSpotID, flags, refCon, userRoutine)
    #define CallQTVRImagingCompleteProc(userRoutine, qtvr, refCon) InvokeQTVRImagingCompleteUPP(qtvr, refCon, userRoutine)
    #define CallQTVRBackBufferImagingProc(userRoutine, qtvr, drawRect, areaIndex, flagsIn, flagsOut, refCon) InvokeQTVRBackBufferImagingUPP(qtvr, drawRect, areaIndex, flagsIn, flagsOut, refCon, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*
  =================================================================================================
    QTVR Intercept Struct, Callback, Routine Descriptors 
  -------------------------------------------------------------------------------------------------
*/

typedef UInt32 QTVRProcSelector;
enum {
  kQTVRSetPanAngleSelector      = 0x2000,
  kQTVRSetTiltAngleSelector     = 0x2001,
  kQTVRSetFieldOfViewSelector   = 0x2002,
  kQTVRSetViewCenterSelector    = 0x2003,
  kQTVRMouseEnterSelector       = 0x2004,
  kQTVRMouseWithinSelector      = 0x2005,
  kQTVRMouseLeaveSelector       = 0x2006,
  kQTVRMouseDownSelector        = 0x2007,
  kQTVRMouseStillDownSelector   = 0x2008,
  kQTVRMouseUpSelector          = 0x2009,
  kQTVRTriggerHotSpotSelector   = 0x200A,
  kQTVRGetHotSpotTypeSelector   = 0x200B, /* Requires QTVR 2.1 (kQTVRAPIMajorVersion02 + kQTVRAPIMinorVersion10)*/
  kQTVRSetViewParameterSelector = 0x200C, /* Requires QTVR 5.0 (kQTVRAPIMajorVersion05 + kQTVRAPIMinorVersion00)*/
  kQTVRGetViewParameterSelector = 0x200D /* Requires QTVR 5.0 (kQTVRAPIMajorVersion05 + kQTVRAPIMinorVersion00)*/
};

struct QTVRInterceptRecord {
  SInt32              reserved1;
  SInt32              selector;

  SInt32              reserved2;
  SInt32              reserved3;

  SInt32              paramCount;
  void *              parameter[6];
};
typedef struct QTVRInterceptRecord      QTVRInterceptRecord;
typedef QTVRInterceptRecord *           QTVRInterceptPtr;
/* Prototype for Intercept Proc callback*/
typedef CALLBACK_API( void , QTVRInterceptProcPtr )(QTVRInstance qtvr, QTVRInterceptPtr qtvrMsg, SInt32 refCon, Boolean *cancel);
typedef STACK_UPP_TYPE(QTVRInterceptProcPtr)                    QTVRInterceptUPP;
/*
 *  NewQTVRInterceptUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( QTVRInterceptUPP )
NewQTVRInterceptUPP(QTVRInterceptProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppQTVRInterceptProcInfo = 0x00003FC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(QTVRInterceptUPP) NewQTVRInterceptUPP(QTVRInterceptProcPtr userRoutine) { return (QTVRInterceptUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTVRInterceptProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewQTVRInterceptUPP(userRoutine) (QTVRInterceptUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppQTVRInterceptProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeQTVRInterceptUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeQTVRInterceptUPP(QTVRInterceptUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeQTVRInterceptUPP(QTVRInterceptUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeQTVRInterceptUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeQTVRInterceptUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeQTVRInterceptUPP(
  QTVRInstance      qtvr,
  QTVRInterceptPtr  qtvrMsg,
  SInt32            refCon,
  Boolean *         cancel,
  QTVRInterceptUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeQTVRInterceptUPP(QTVRInstance qtvr, QTVRInterceptPtr qtvrMsg, SInt32 refCon, Boolean * cancel, QTVRInterceptUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppQTVRInterceptProcInfo, qtvr, qtvrMsg, refCon, cancel); }
  #else
    #define InvokeQTVRInterceptUPP(qtvr, qtvrMsg, refCon, cancel, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppQTVRInterceptProcInfo, (qtvr), (qtvrMsg), (refCon), (cancel))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewQTVRInterceptProc(userRoutine)                   NewQTVRInterceptUPP(userRoutine)
    #define CallQTVRInterceptProc(userRoutine, qtvr, qtvrMsg, refCon, cancel) InvokeQTVRInterceptUPP(qtvr, qtvrMsg, refCon, cancel, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*
  =================================================================================================
    Initialization QTVR calls 
  -------------------------------------------------------------------------------------------------
   Requires QTVR 2.1 (kQTVRAPIMajorVersion02 + kQTVRAPIMinorVersion10) and only work on Non-Macintosh platforms
*/
#if !TARGET_OS_MAC
#if CALL_NOT_IN_CARBON
/*
 *  InitializeQTVR()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
InitializeQTVR(void);


/*
 *  TerminateQTVR()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
TerminateQTVR(void);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* !TARGET_OS_MAC */

/*
  =================================================================================================
    General QTVR calls 
  -------------------------------------------------------------------------------------------------
*/
/*
 *  QTVRGetQTVRTrack()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( Track )
QTVRGetQTVRTrack(
  Movie    theMovie,
  SInt32   index);


/*
 *  QTVRGetQTVRInstance()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetQTVRInstance(
  QTVRInstance *    qtvr,
  Track             qtvrTrack,
  MovieController   mc);


/*
  =================================================================================================
    Viewing Angles and Zooming 
  -------------------------------------------------------------------------------------------------
*/

/* QTVRSetViewParameter requires QTVR 5.0 (kQTVRAPIMajorVersion05 + kQTVRAPIMinorVersion00)*/
/*
 *  QTVRSetViewParameter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 5.0 and later
 */
EXTERN_API_C( OSErr )
QTVRSetViewParameter(
  QTVRInstance   qtvr,
  UInt32         viewParameter,
  void *         value,
  UInt32         flagsIn);


/* QTVRGetViewParameter requires QTVR 5.0 (kQTVRAPIMajorVersion05 + kQTVRAPIMinorVersion00)*/
/*
 *  QTVRGetViewParameter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 5.0 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 5.0 and later
 */
EXTERN_API_C( OSErr )
QTVRGetViewParameter(
  QTVRInstance   qtvr,
  UInt32         viewParameter,
  void *         value,
  UInt32         flagsIn,
  UInt32 *       flagsOut);


/*
 *  QTVRSetPanAngle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetPanAngle(
  QTVRInstance   qtvr,
  float          panAngle);


/*
 *  QTVRGetPanAngle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( float )
QTVRGetPanAngle(QTVRInstance qtvr);


/*
 *  QTVRSetTiltAngle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetTiltAngle(
  QTVRInstance   qtvr,
  float          tiltAngle);


/*
 *  QTVRGetTiltAngle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( float )
QTVRGetTiltAngle(QTVRInstance qtvr);


/*
 *  QTVRSetFieldOfView()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetFieldOfView(
  QTVRInstance   qtvr,
  float          fieldOfView);


/*
 *  QTVRGetFieldOfView()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( float )
QTVRGetFieldOfView(QTVRInstance qtvr);


/*
 *  QTVRShowDefaultView()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRShowDefaultView(QTVRInstance qtvr);


/* Object Specific*/
/*
 *  QTVRSetViewCenter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetViewCenter(
  QTVRInstance            qtvr,
  const QTVRFloatPoint *  viewCenter);


/*
 *  QTVRGetViewCenter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetViewCenter(
  QTVRInstance      qtvr,
  QTVRFloatPoint *  viewCenter);


/*
 *  QTVRNudge()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRNudge(
  QTVRInstance       qtvr,
  QTVRNudgeControl   direction);


/* QTVRInteractionNudge requires QTVR 2.1 (kQTVRAPIMajorVersion02 + kQTVRAPIMinorVersion10)*/
/*
 *  QTVRInteractionNudge()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRInteractionNudge(
  QTVRInstance       qtvr,
  QTVRNudgeControl   direction);


/*
  =================================================================================================
    Scene and Node Location Information 
  -------------------------------------------------------------------------------------------------
*/

/*
 *  QTVRGetVRWorld()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetVRWorld(
  QTVRInstance       qtvr,
  QTAtomContainer *  VRWorld);


/*
 *  QTVRGetNodeInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetNodeInfo(
  QTVRInstance       qtvr,
  UInt32             nodeID,
  QTAtomContainer *  nodeInfo);


/*
 *  QTVRGoToNodeID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGoToNodeID(
  QTVRInstance   qtvr,
  UInt32         nodeID);


/*
 *  QTVRGetCurrentNodeID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( UInt32 )
QTVRGetCurrentNodeID(QTVRInstance qtvr);


/*
 *  QTVRGetNodeType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSType )
QTVRGetNodeType(
  QTVRInstance   qtvr,
  UInt32         nodeID);


/*
  =================================================================================================
    Hot Spot related calls 
  -------------------------------------------------------------------------------------------------
*/

/*
 *  QTVRPtToHotSpotID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRPtToHotSpotID(
  QTVRInstance   qtvr,
  Point          pt,
  UInt32 *       hotSpotID);


/* QTVRGetHotSpotType requires QTVR 2.1 (kQTVRAPIMajorVersion02 + kQTVRAPIMinorVersion10)*/
/*
 *  QTVRGetHotSpotType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetHotSpotType(
  QTVRInstance   qtvr,
  UInt32         hotSpotID,
  OSType *       hotSpotType);


/*
 *  QTVRTriggerHotSpot()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRTriggerHotSpot(
  QTVRInstance      qtvr,
  UInt32            hotSpotID,
  QTAtomContainer   nodeInfo,
  QTAtom            selectedAtom);


/*
 *  QTVRSetMouseOverHotSpotProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetMouseOverHotSpotProc(
  QTVRInstance              qtvr,
  QTVRMouseOverHotSpotUPP   mouseOverHotSpotProc,
  SInt32                    refCon,
  UInt32                    flags);


/*
 *  QTVREnableHotSpot()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVREnableHotSpot(
  QTVRInstance   qtvr,
  UInt32         enableFlag,
  UInt32         hotSpotValue,
  Boolean        enable);


/*
 *  QTVRGetVisibleHotSpots()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( UInt32 )
QTVRGetVisibleHotSpots(
  QTVRInstance   qtvr,
  Handle         hotSpots);


/*
 *  QTVRGetHotSpotRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetHotSpotRegion(
  QTVRInstance   qtvr,
  UInt32         hotSpotID,
  RgnHandle      hotSpotRegion);


/*
  =================================================================================================
    Event & Cursor Handling Calls 
  -------------------------------------------------------------------------------------------------
*/

/*
 *  QTVRSetMouseOverTracking()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetMouseOverTracking(
  QTVRInstance   qtvr,
  Boolean        enable);


/*
 *  QTVRGetMouseOverTracking()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( Boolean )
QTVRGetMouseOverTracking(QTVRInstance qtvr);


/*
 *  QTVRSetMouseDownTracking()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetMouseDownTracking(
  QTVRInstance   qtvr,
  Boolean        enable);


/*
 *  QTVRGetMouseDownTracking()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( Boolean )
QTVRGetMouseDownTracking(QTVRInstance qtvr);


/*
 *  QTVRMouseEnter()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRMouseEnter(
  QTVRInstance   qtvr,
  Point          pt,
  UInt32 *       hotSpotID,
  WindowRef      w);


/*
 *  QTVRMouseWithin()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRMouseWithin(
  QTVRInstance   qtvr,
  Point          pt,
  UInt32 *       hotSpotID,
  WindowRef      w);


/*
 *  QTVRMouseLeave()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRMouseLeave(
  QTVRInstance   qtvr,
  Point          pt,
  WindowRef      w);


/*
 *  QTVRMouseDown()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRMouseDown(
  QTVRInstance   qtvr,
  Point          pt,
  UInt32         when,
  UInt16         modifiers,
  UInt32 *       hotSpotID,
  WindowRef      w);


/*
 *  QTVRMouseStillDown()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRMouseStillDown(
  QTVRInstance   qtvr,
  Point          pt,
  UInt32 *       hotSpotID,
  WindowRef      w);


/*
 *  QTVRMouseUp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRMouseUp(
  QTVRInstance   qtvr,
  Point          pt,
  UInt32 *       hotSpotID,
  WindowRef      w);


/* These require QTVR 2.01 (kQTVRAPIMajorVersion02 + kQTVRAPIMinorVersion01)*/
/*
 *  QTVRMouseStillDownExtended()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRMouseStillDownExtended(
  QTVRInstance   qtvr,
  Point          pt,
  UInt32 *       hotSpotID,
  WindowRef      w,
  UInt32         when,
  UInt16         modifiers);


/*
 *  QTVRMouseUpExtended()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRMouseUpExtended(
  QTVRInstance   qtvr,
  Point          pt,
  UInt32 *       hotSpotID,
  WindowRef      w,
  UInt32         when,
  UInt16         modifiers);


/*
  =================================================================================================
    Intercept Routines 
  -------------------------------------------------------------------------------------------------
*/

/*
 *  QTVRInstallInterceptProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRInstallInterceptProc(
  QTVRInstance       qtvr,
  QTVRProcSelector   selector,
  QTVRInterceptUPP   interceptProc,
  SInt32             refCon,
  UInt32             flags);


/*
 *  QTVRCallInterceptedProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRCallInterceptedProc(
  QTVRInstance           qtvr,
  QTVRInterceptRecord *  qtvrMsg);


/*
  =================================================================================================
    Object Movie Specific Calls 
  -------------------------------------------------------------------------------------------------
   QTVRGetCurrentMouseMode requires QTRVR 2.1 (kQTVRAPIMajorVersion02 + kQTVRAPIMinorVersion10)
*/
/*
 *  QTVRGetCurrentMouseMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( UInt32 )
QTVRGetCurrentMouseMode(QTVRInstance qtvr);


/*
 *  QTVRSetFrameRate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetFrameRate(
  QTVRInstance   qtvr,
  float          rate);


/*
 *  QTVRGetFrameRate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( float )
QTVRGetFrameRate(QTVRInstance qtvr);


/*
 *  QTVRSetViewRate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetViewRate(
  QTVRInstance   qtvr,
  float          rate);


/*
 *  QTVRGetViewRate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( float )
QTVRGetViewRate(QTVRInstance qtvr);


/*
 *  QTVRSetViewCurrentTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetViewCurrentTime(
  QTVRInstance   qtvr,
  TimeValue      time);


/*
 *  QTVRGetViewCurrentTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( TimeValue )
QTVRGetViewCurrentTime(QTVRInstance qtvr);


/*
 *  QTVRGetCurrentViewDuration()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( TimeValue )
QTVRGetCurrentViewDuration(QTVRInstance qtvr);


/*
  =================================================================================================
   View State Calls - QTVR Object Only
  -------------------------------------------------------------------------------------------------
*/

/*
 *  QTVRSetViewState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetViewState(
  QTVRInstance        qtvr,
  QTVRViewStateType   viewStateType,
  UInt16              state);


/*
 *  QTVRGetViewState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetViewState(
  QTVRInstance        qtvr,
  QTVRViewStateType   viewStateType,
  UInt16 *            state);


/*
 *  QTVRGetViewStateCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( UInt16 )
QTVRGetViewStateCount(QTVRInstance qtvr);


/*
 *  QTVRSetAnimationSetting()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetAnimationSetting(
  QTVRInstance                 qtvr,
  QTVRObjectAnimationSetting   setting,
  Boolean                      enable);


/*
 *  QTVRGetAnimationSetting()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetAnimationSetting(
  QTVRInstance                 qtvr,
  QTVRObjectAnimationSetting   setting,
  Boolean *                    enable);


/*
 *  QTVRSetControlSetting()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetControlSetting(
  QTVRInstance         qtvr,
  QTVRControlSetting   setting,
  Boolean              enable);


/*
 *  QTVRGetControlSetting()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetControlSetting(
  QTVRInstance         qtvr,
  QTVRControlSetting   setting,
  Boolean *            enable);


/*
 *  QTVREnableFrameAnimation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVREnableFrameAnimation(
  QTVRInstance   qtvr,
  Boolean        enable);


/*
 *  QTVRGetFrameAnimation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( Boolean )
QTVRGetFrameAnimation(QTVRInstance qtvr);


/*
 *  QTVREnableViewAnimation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVREnableViewAnimation(
  QTVRInstance   qtvr,
  Boolean        enable);


/*
 *  QTVRGetViewAnimation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( Boolean )
QTVRGetViewAnimation(QTVRInstance qtvr);



/*
  =================================================================================================
    Imaging Characteristics 
  -------------------------------------------------------------------------------------------------
*/

/*
 *  QTVRSetVisible()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetVisible(
  QTVRInstance   qtvr,
  Boolean        visible);


/*
 *  QTVRGetVisible()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( Boolean )
QTVRGetVisible(QTVRInstance qtvr);


/*
 *  QTVRSetImagingProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetImagingProperty(
  QTVRInstance      qtvr,
  QTVRImagingMode   imagingMode,
  UInt32            imagingProperty,
  SInt32            propertyValue);


/*
 *  QTVRGetImagingProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetImagingProperty(
  QTVRInstance      qtvr,
  QTVRImagingMode   imagingMode,
  UInt32            imagingProperty,
  SInt32 *          propertyValue);


/*
 *  QTVRUpdate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRUpdate(
  QTVRInstance      qtvr,
  QTVRImagingMode   imagingMode);


/*
 *  QTVRBeginUpdateStream()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRBeginUpdateStream(
  QTVRInstance      qtvr,
  QTVRImagingMode   imagingMode);


/*
 *  QTVREndUpdateStream()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVREndUpdateStream(QTVRInstance qtvr);


/*
 *  QTVRSetTransitionProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetTransitionProperty(
  QTVRInstance   qtvr,
  UInt32         transitionType,
  UInt32         transitionProperty,
  SInt32         transitionValue);


/*
 *  QTVREnableTransition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVREnableTransition(
  QTVRInstance   qtvr,
  UInt32         transitionType,
  Boolean        enable);


/*
  =================================================================================================
    Basic Conversion and Math Routines 
  -------------------------------------------------------------------------------------------------
*/

/*
 *  QTVRSetAngularUnits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetAngularUnits(
  QTVRInstance       qtvr,
  QTVRAngularUnits   units);


/*
 *  QTVRGetAngularUnits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( QTVRAngularUnits )
QTVRGetAngularUnits(QTVRInstance qtvr);


/* Pano specific routines*/
/*
 *  QTVRPtToAngles()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRPtToAngles(
  QTVRInstance   qtvr,
  Point          pt,
  float *        panAngle,
  float *        tiltAngle);


/*
 *  QTVRCoordToAngles()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRCoordToAngles(
  QTVRInstance      qtvr,
  QTVRFloatPoint *  coord,
  float *           panAngle,
  float *           tiltAngle);


/*
 *  QTVRAnglesToCoord()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRAnglesToCoord(
  QTVRInstance      qtvr,
  float             panAngle,
  float             tiltAngle,
  QTVRFloatPoint *  coord);


/* Object specific routines*/
/*
 *  QTVRPanToColumn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( short )
QTVRPanToColumn(
  QTVRInstance   qtvr,
  float          panAngle);


/* zero based   */
/*
 *  QTVRColumnToPan()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( float )
QTVRColumnToPan(
  QTVRInstance   qtvr,
  short          column);


/* zero based   */
/*
 *  QTVRTiltToRow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( short )
QTVRTiltToRow(
  QTVRInstance   qtvr,
  float          tiltAngle);


/* zero based   */
/*
 *  QTVRRowToTilt()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( float )
QTVRRowToTilt(
  QTVRInstance   qtvr,
  short          row);


/* zero based               */
/*
 *  QTVRWrapAndConstrain()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRWrapAndConstrain(
  QTVRInstance   qtvr,
  short          kind,
  float          value,
  float *        result);



/*
  =================================================================================================
    Interaction Routines 
  -------------------------------------------------------------------------------------------------
*/

/*
 *  QTVRSetEnteringNodeProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetEnteringNodeProc(
  QTVRInstance          qtvr,
  QTVREnteringNodeUPP   enteringNodeProc,
  SInt32                refCon,
  UInt32                flags);


/*
 *  QTVRSetLeavingNodeProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetLeavingNodeProc(
  QTVRInstance         qtvr,
  QTVRLeavingNodeUPP   leavingNodeProc,
  SInt32               refCon,
  UInt32               flags);


/*
 *  QTVRSetInteractionProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetInteractionProperty(
  QTVRInstance   qtvr,
  UInt32         property,
  void *         value);


/*
 *  QTVRGetInteractionProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetInteractionProperty(
  QTVRInstance   qtvr,
  UInt32         property,
  void *         value);


/*
 *  QTVRReplaceCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRReplaceCursor(
  QTVRInstance        qtvr,
  QTVRCursorRecord *  cursRecord);


/*
  =================================================================================================
    Viewing Limits and Constraints 
  -------------------------------------------------------------------------------------------------
*/

/*
 *  QTVRGetViewingLimits()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetViewingLimits(
  QTVRInstance   qtvr,
  UInt16         kind,
  float *        minValue,
  float *        maxValue);


/*
 *  QTVRGetConstraintStatus()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( UInt32 )
QTVRGetConstraintStatus(QTVRInstance qtvr);


/*
 *  QTVRGetConstraints()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetConstraints(
  QTVRInstance   qtvr,
  UInt16         kind,
  float *        minValue,
  float *        maxValue);


/*
 *  QTVRSetConstraints()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetConstraints(
  QTVRInstance   qtvr,
  UInt16         kind,
  float          minValue,
  float          maxValue);



/*
  =================================================================================================
    Back Buffer Memory Management 
  -------------------------------------------------------------------------------------------------
*/

/*
 *  QTVRGetAvailableResolutions()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetAvailableResolutions(
  QTVRInstance   qtvr,
  UInt16 *       resolutionsMask);


/* These require QTVR 2.1 (kQTVRAPIMajorVersion02 + kQTVRAPIMinorVersion10)*/
/*
 *  QTVRGetBackBufferMemInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetBackBufferMemInfo(
  QTVRInstance   qtvr,
  UInt32         geometry,
  UInt16         resolution,
  UInt32         cachePixelFormat,
  SInt32 *       minCacheBytes,
  SInt32 *       suggestedCacheBytes,
  SInt32 *       fullCacheBytes);


/*
 *  QTVRGetBackBufferSettings()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRGetBackBufferSettings(
  QTVRInstance   qtvr,
  UInt32 *       geometry,
  UInt16 *       resolution,
  UInt32 *       cachePixelFormat,
  SInt16 *       cacheSize);


/*
 *  QTVRSetBackBufferPrefs()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.1 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetBackBufferPrefs(
  QTVRInstance   qtvr,
  UInt32         geometry,
  UInt16         resolution,
  UInt32         cachePixelFormat,
  SInt16         cacheSize);


/*
  =================================================================================================
    Buffer Access 
  -------------------------------------------------------------------------------------------------
*/

/*
 *  QTVRSetPrescreenImagingCompleteProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetPrescreenImagingCompleteProc(
  QTVRInstance             qtvr,
  QTVRImagingCompleteUPP   imagingCompleteProc,
  SInt32                   refCon,
  UInt32                   flags);


/*
 *  QTVRSetBackBufferImagingProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRSetBackBufferImagingProc(
  QTVRInstance               qtvr,
  QTVRBackBufferImagingUPP   backBufferImagingProc,
  UInt16                     numAreas,
  QTVRAreaOfInterest         areasOfInterest[],
  SInt32                     refCon);


/*
 *  QTVRRefreshBackBuffer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in QuickTimeVRLib 2.0 and later
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 *    Windows:          in QTVR.lib 2.1 and later
 */
EXTERN_API_C( OSErr )
QTVRRefreshBackBuffer(
  QTVRInstance   qtvr,
  UInt32         flags);




/*
  =================================================================================================
    Old Names
  -------------------------------------------------------------------------------------------------
*/
#if OLDROUTINENAMES
typedef QTVRCursorRecord                CursorRecord;
typedef QTVRAreaOfInterest              AreaOfInterest;
typedef QTVRFloatPoint                  FloatPoint;
typedef QTVRLeavingNodeProcPtr          LeavingNodeProcPtr;
typedef QTVRLeavingNodeUPP              LeavingNodeUPP;
typedef QTVREnteringNodeProcPtr         EnteringNodeProcPtr;
typedef QTVREnteringNodeUPP             EnteringNodeUPP;
typedef QTVRMouseOverHotSpotProcPtr     MouseOverHotSpotProcPtr;
typedef QTVRMouseOverHotSpotUPP         MouseOverHotSpotUPP;
typedef QTVRImagingCompleteProcPtr      ImagingCompleteProcPtr;
typedef QTVRImagingCompleteUPP          ImagingCompleteUPP;
typedef QTVRBackBufferImagingProcPtr    BackBufferImagingProcPtr;
typedef QTVRBackBufferImagingUPP        BackBufferImagingUPP;
#endif  /* OLDROUTINENAMES */



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

#endif /* __QUICKTIMEVR__ */

