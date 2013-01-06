/*
     File:       Drag.h
 
     Contains:   Drag and Drop Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1992-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __DRAG__
#define __DRAG__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
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


/*
 *  HIPoint
 *  
 *  Discussion:
 *    HIPoint is a new, floating point-based type to help express
 *    coordinates in a much richer fashion than the classic QuickDraw
 *    points. It will, in time, be more heavily used throughout the
 *    Toolbox. For now, it is replacing our use of typeQDPoint in mouse
 *    events. This is to better support sub-pixel tablet coordinates.
 *    If you ask for a mouse location with typeQDPoint, and the point
 *    is actually stored as typeHIPoint, it will automatically be
 *    coerced to typeQDPoint for you, so this change should be largely
 *    transparent to applications. HIPoints are in screen space, i.e.
 *    the top left of the screen is 0, 0.
 */
typedef CGPoint                         HIPoint;

/*
 *  HISize
 *  
 *  Discussion:
 *    HISize is a floating point-based type to help express dimensions
 *    in a much richer fashion than the classic QuickDraw coordinates.
 */
typedef CGSize                          HISize;

/*
 *  HIRect
 *  
 *  Discussion:
 *    HIRect is a new, floating point-based type to help express
 *    rectangles in a much richer fashion than the classic QuickDraw
 *    rects. It will, in time, be more heavily used throughout the
 *    Toolbox. HIRects are in screen space, i.e. the top left of the
 *    screen is 0, 0.
 */
typedef CGRect                          HIRect;
/*
  _________________________________________________________________________________________________________
      
   o DRAG MANAGER DATA TYPES
  _________________________________________________________________________________________________________
*/
typedef struct OpaqueDragRef*           DragRef;
typedef UInt32                          DragItemRef;
typedef OSType                          FlavorType;
/*
  _________________________________________________________________________________________________________
      
   o DRAG ATTRIBUTES
  _________________________________________________________________________________________________________
*/
typedef UInt32 DragAttributes;
enum {
  kDragHasLeftSenderWindow      = (1L << 0), /* drag has left the source window since TrackDrag*/
  kDragInsideSenderApplication  = (1L << 1), /* drag is occurring within the sender application*/
  kDragInsideSenderWindow       = (1L << 2) /* drag is occurring within the sender window*/
};

/*
  _________________________________________________________________________________________________________
      
   o DRAG BEHAVIORS
  _________________________________________________________________________________________________________
*/
typedef UInt32 DragBehaviors;
enum {
  kDragBehaviorNone             = 0,
  kDragBehaviorZoomBackAnimation = (1L << 0) /* do zoomback animation for failed drags (normally enabled).*/
};

/*
  _________________________________________________________________________________________________________
      
   o DRAG IMAGE FLAGS
  _________________________________________________________________________________________________________
*/
typedef UInt32 DragImageFlags;
enum {
  kDragRegionAndImage           = (1L << 4) /* drag region and image*/
};

/*
  _________________________________________________________________________________________________________
      
   o DRAG IMAGE TRANSLUCENCY LEVELS
  _________________________________________________________________________________________________________
*/

enum {
  kDragStandardTranslucency     = 0L,   /* 65% image translucency (standard)*/
  kDragDarkTranslucency         = 1L,   /* 50% image translucency*/
  kDragDarkerTranslucency       = 2L,   /* 25% image translucency*/
  kDragOpaqueTranslucency       = 3L    /* 0% image translucency (opaque)*/
};

/*
  _________________________________________________________________________________________________________
      
   o DRAG DRAWING PROCEDURE MESSAGES
  _________________________________________________________________________________________________________
*/

typedef SInt16 DragRegionMessage;
enum {
  kDragRegionBegin              = 1,    /* initialize drawing*/
  kDragRegionDraw               = 2,    /* draw drag feedback*/
  kDragRegionHide               = 3,    /* hide drag feedback*/
  kDragRegionIdle               = 4,    /* drag feedback idle time*/
  kDragRegionEnd                = 5     /* end of drawing*/
};

/*
  _________________________________________________________________________________________________________
      
   o ZOOM ACCELERATION
  _________________________________________________________________________________________________________
*/

typedef SInt16 ZoomAcceleration;
enum {
  kZoomNoAcceleration           = 0,    /* use linear interpolation*/
  kZoomAccelerate               = 1,    /* ramp up step size*/
  kZoomDecelerate               = 2     /* ramp down step size*/
};

/*
  _________________________________________________________________________________________________________
      
   o FLAVOR FLAGS
  _________________________________________________________________________________________________________
*/

typedef UInt32 FlavorFlags;
enum {
  flavorSenderOnly              = (1 << 0), /* flavor is available to sender only*/
  flavorSenderTranslated        = (1 << 1), /* flavor is translated by sender*/
  flavorNotSaved                = (1 << 2), /* flavor should not be saved*/
  flavorSystemTranslated        = (1 << 8), /* flavor is translated by system*/
  flavorDataPromised            = (1 << 9) /* flavor data is promised by sender*/
};

/*
  _________________________________________________________________________________________________________
      
   o FILE SYSTEM CONSTANTS
  _________________________________________________________________________________________________________
*/

enum {
  kDragFlavorTypeHFS            = FOUR_CHAR_CODE('hfs '), /* flavor type for HFS data*/
  kDragFlavorTypePromiseHFS     = FOUR_CHAR_CODE('phfs'), /* flavor type for promised HFS data*/
  flavorTypeHFS                 = kDragFlavorTypeHFS, /* old name*/
  flavorTypePromiseHFS          = kDragFlavorTypePromiseHFS /* old name*/
};

enum {
  kDragPromisedFlavorFindFile   = FOUR_CHAR_CODE('rWm1'), /* promisedFlavor value for Find File*/
  kDragPromisedFlavor           = FOUR_CHAR_CODE('fssP') /* promisedFlavor value for everything else*/
};

enum {
  kDragPseudoCreatorVolumeOrDirectory = FOUR_CHAR_CODE('MACS'), /* "creator code" for volume or directory*/
  kDragPseudoFileTypeVolume     = FOUR_CHAR_CODE('disk'), /* "file type" for volume*/
  kDragPseudoFileTypeDirectory  = FOUR_CHAR_CODE('fold') /* "file type" for directory*/
};

/*
  _________________________________________________________________________________________________________
      
   o SPECIAL FLAVORS
  _________________________________________________________________________________________________________
*/

enum {
  flavorTypeDirectory           = FOUR_CHAR_CODE('diry') /* flavor type for AOCE directories*/
};

/*
  _________________________________________________________________________________________________________
      
   o FLAVORS FOR FINDER 8.0 AND LATER
  _________________________________________________________________________________________________________
*/

enum {
  kFlavorTypeClippingName       = FOUR_CHAR_CODE('clnm'), /* name hint for clipping file (preferred over 'clfn')*/
  kFlavorTypeClippingFilename   = FOUR_CHAR_CODE('clfn'), /* name for clipping file*/
  kFlavorTypeUnicodeClippingName = FOUR_CHAR_CODE('ucln'), /* unicode name hint for clipping file (preferred over 'uclf')*/
  kFlavorTypeUnicodeClippingFilename = FOUR_CHAR_CODE('uclf'), /* unicode name for clipping file*/
  kFlavorTypeDragToTrashOnly    = FOUR_CHAR_CODE('fdtt'), /* for apps that want to allow dragging private data to the trash*/
  kFlavorTypeFinderNoTrackingBehavior = FOUR_CHAR_CODE('fntb') /* Finder completely ignores any drag containing this flavor*/
};

/*
  _________________________________________________________________________________________________________
      
   o DRAG TRACKING HANDLER MESSAGES
  _________________________________________________________________________________________________________
*/

typedef SInt16 DragTrackingMessage;
enum {
  kDragTrackingEnterHandler     = 1,    /* drag has entered handler*/
  kDragTrackingEnterWindow      = 2,    /* drag has entered window*/
  kDragTrackingInWindow         = 3,    /* drag is moving within window*/
  kDragTrackingLeaveWindow      = 4,    /* drag has exited window*/
  kDragTrackingLeaveHandler     = 5     /* drag has exited handler*/
};

/*
  ---------------------------------------------------------------------------------------------------------
   o STANDARD DROP LOCATIONS
  ---------------------------------------------------------------------------------------------------------
*/


/*
 *  Summary:
 *    Standard Drop Location constants
 *  
 *  Discussion:
 *    The following constants define common "meta" drop locations.
 */
enum {

  /*
   * The drop location was in the trash.  This is set when a drag is
   * dropped on the trash icon.  Setting this standard drop location
   * sets the traditional drop location to an alias to the trash folder
   * automatically.
   */
  kDragStandardDropLocationTrash = FOUR_CHAR_CODE('trsh'),

  /*
   * The receiver did not specify a drop lcoation. This is the default.
   */
  kDragStandardDropLocationUnknown = FOUR_CHAR_CODE('unkn')
};

typedef OSType                          StandardDropLocation;
/*
  _________________________________________________________________________________________________________
      
   o DRAG ACTIONS
  _________________________________________________________________________________________________________
*/

/*
 *  Summary:
 *    Drag Action constants
 *  
 *  Discussion:
 *    The following constants define, in a general way, what actions a
 *    drag should or has performed.  Some drag actions enforce a mode
 *    of operation while others are flexible suggestions.  These
 *    constants are used in conjunction with the
 *    Get/SetDragAllowableActions() and Get/SetDragDropAction() APIs. 
 *    Adopting the Drag Action APIs increases compatability with the
 *    Cocoa drag operation model.
 */
enum {

  /*
   * Suggests nothing should be/was done with the data in a drag.  When
   * set as an allowable action for remote drags, the drag will not be
   * sent to apps other than the sender.
   */
  kDragActionNothing            = 0L,

  /*
   * Suggests the data contained within the drag can be/was copied.
   */
  kDragActionCopy               = 1L,

  /*
   * Suggests the data contained within the drag can be/is shared.
   */
  kDragActionAlias              = (1L << 1),

  /*
   * Suggests the drag action is can be defined by the drag destination
   * or was not defined by the drag destination.
   */
  kDragActionGeneric            = (1L << 2),

  /*
   * Suggests the drag action should be negotiated privately between
   * the drag source and destination.
   */
  kDragActionPrivate            = (1L << 3),

  /*
   * Description forthcoming.
   */
  kDragActionMove               = (1L << 4),

  /*
   * Description forthcoming.
   */
  kDragActionDelete             = (1L << 5),

  /*
   * All of the above drag actions are allowed.
   */
  kDragActionAll                = (long)0xFFFFFFFF
};

typedef UInt32                          DragActions;
/*
  _________________________________________________________________________________________________________
      
   o HFS FLAVORS
  _________________________________________________________________________________________________________
*/
struct HFSFlavor {
  OSType              fileType;               /* file type */
  OSType              fileCreator;            /* file creator */
  UInt16              fdFlags;                /* Finder flags */
  FSSpec              fileSpec;               /* file system specification */
};
typedef struct HFSFlavor                HFSFlavor;
struct PromiseHFSFlavor {
  OSType              fileType;               /* file type */
  OSType              fileCreator;            /* file creator */
  UInt16              fdFlags;                /* Finder flags */
  FlavorType          promisedFlavor;         /* promised flavor containing an FSSpec */
};
typedef struct PromiseHFSFlavor         PromiseHFSFlavor;
/*
  _________________________________________________________________________________________________________
      
   o APPLICATION-DEFINED DRAG HANDLER ROUTINES
  _________________________________________________________________________________________________________
*/
typedef CALLBACK_API( OSErr , DragTrackingHandlerProcPtr )(DragTrackingMessage message, WindowRef theWindow, void *handlerRefCon, DragRef theDrag);
typedef CALLBACK_API( OSErr , DragReceiveHandlerProcPtr )(WindowRef theWindow, void *handlerRefCon, DragRef theDrag);
typedef STACK_UPP_TYPE(DragTrackingHandlerProcPtr)              DragTrackingHandlerUPP;
typedef STACK_UPP_TYPE(DragReceiveHandlerProcPtr)               DragReceiveHandlerUPP;
/*
 *  NewDragTrackingHandlerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DragTrackingHandlerUPP )
NewDragTrackingHandlerUPP(DragTrackingHandlerProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDragTrackingHandlerProcInfo = 0x00003FA0 };  /* pascal 2_bytes Func(2_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DragTrackingHandlerUPP) NewDragTrackingHandlerUPP(DragTrackingHandlerProcPtr userRoutine) { return (DragTrackingHandlerUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDragTrackingHandlerProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDragTrackingHandlerUPP(userRoutine) (DragTrackingHandlerUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDragTrackingHandlerProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewDragReceiveHandlerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DragReceiveHandlerUPP )
NewDragReceiveHandlerUPP(DragReceiveHandlerProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDragReceiveHandlerProcInfo = 0x00000FE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DragReceiveHandlerUPP) NewDragReceiveHandlerUPP(DragReceiveHandlerProcPtr userRoutine) { return (DragReceiveHandlerUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDragReceiveHandlerProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDragReceiveHandlerUPP(userRoutine) (DragReceiveHandlerUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDragReceiveHandlerProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeDragTrackingHandlerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDragTrackingHandlerUPP(DragTrackingHandlerUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDragTrackingHandlerUPP(DragTrackingHandlerUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDragTrackingHandlerUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeDragReceiveHandlerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDragReceiveHandlerUPP(DragReceiveHandlerUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDragReceiveHandlerUPP(DragReceiveHandlerUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDragReceiveHandlerUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeDragTrackingHandlerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeDragTrackingHandlerUPP(
  DragTrackingMessage     message,
  WindowRef               theWindow,
  void *                  handlerRefCon,
  DragRef                 theDrag,
  DragTrackingHandlerUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeDragTrackingHandlerUPP(DragTrackingMessage message, WindowRef theWindow, void * handlerRefCon, DragRef theDrag, DragTrackingHandlerUPP userUPP) { return (OSErr)CALL_FOUR_PARAMETER_UPP(userUPP, uppDragTrackingHandlerProcInfo, message, theWindow, handlerRefCon, theDrag); }
  #else
    #define InvokeDragTrackingHandlerUPP(message, theWindow, handlerRefCon, theDrag, userUPP) (OSErr)CALL_FOUR_PARAMETER_UPP((userUPP), uppDragTrackingHandlerProcInfo, (message), (theWindow), (handlerRefCon), (theDrag))
  #endif
#endif

/*
 *  InvokeDragReceiveHandlerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeDragReceiveHandlerUPP(
  WindowRef              theWindow,
  void *                 handlerRefCon,
  DragRef                theDrag,
  DragReceiveHandlerUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeDragReceiveHandlerUPP(WindowRef theWindow, void * handlerRefCon, DragRef theDrag, DragReceiveHandlerUPP userUPP) { return (OSErr)CALL_THREE_PARAMETER_UPP(userUPP, uppDragReceiveHandlerProcInfo, theWindow, handlerRefCon, theDrag); }
  #else
    #define InvokeDragReceiveHandlerUPP(theWindow, handlerRefCon, theDrag, userUPP) (OSErr)CALL_THREE_PARAMETER_UPP((userUPP), uppDragReceiveHandlerProcInfo, (theWindow), (handlerRefCon), (theDrag))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewDragTrackingHandlerProc(userRoutine)             NewDragTrackingHandlerUPP(userRoutine)
    #define NewDragReceiveHandlerProc(userRoutine)              NewDragReceiveHandlerUPP(userRoutine)
    #define CallDragTrackingHandlerProc(userRoutine, message, theWindow, handlerRefCon, theDrag) InvokeDragTrackingHandlerUPP(message, theWindow, handlerRefCon, theDrag, userRoutine)
    #define CallDragReceiveHandlerProc(userRoutine, theWindow, handlerRefCon, theDrag) InvokeDragReceiveHandlerUPP(theWindow, handlerRefCon, theDrag, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*
  _________________________________________________________________________________________________________
      
   o APPLICATION-DEFINED ROUTINES
  _________________________________________________________________________________________________________
*/
typedef CALLBACK_API( OSErr , DragSendDataProcPtr )(FlavorType theType, void *dragSendRefCon, DragItemRef theItemRef, DragRef theDrag);
typedef CALLBACK_API( OSErr , DragInputProcPtr )(Point *mouse, SInt16 *modifiers, void *dragInputRefCon, DragRef theDrag);
typedef CALLBACK_API( OSErr , DragDrawingProcPtr )(DragRegionMessage message, RgnHandle showRegion, Point showOrigin, RgnHandle hideRegion, Point hideOrigin, void *dragDrawingRefCon, DragRef theDrag);
typedef STACK_UPP_TYPE(DragSendDataProcPtr)                     DragSendDataUPP;
typedef STACK_UPP_TYPE(DragInputProcPtr)                        DragInputUPP;
typedef STACK_UPP_TYPE(DragDrawingProcPtr)                      DragDrawingUPP;
/*
 *  NewDragSendDataUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DragSendDataUPP )
NewDragSendDataUPP(DragSendDataProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDragSendDataProcInfo = 0x00003FE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DragSendDataUPP) NewDragSendDataUPP(DragSendDataProcPtr userRoutine) { return (DragSendDataUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDragSendDataProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDragSendDataUPP(userRoutine) (DragSendDataUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDragSendDataProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewDragInputUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DragInputUPP )
NewDragInputUPP(DragInputProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDragInputProcInfo = 0x00003FE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DragInputUPP) NewDragInputUPP(DragInputProcPtr userRoutine) { return (DragInputUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDragInputProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDragInputUPP(userRoutine) (DragInputUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDragInputProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewDragDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DragDrawingUPP )
NewDragDrawingUPP(DragDrawingProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDragDrawingProcInfo = 0x000FFFA0 };  /* pascal 2_bytes Func(2_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DragDrawingUPP) NewDragDrawingUPP(DragDrawingProcPtr userRoutine) { return (DragDrawingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDragDrawingProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDragDrawingUPP(userRoutine) (DragDrawingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDragDrawingProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeDragSendDataUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDragSendDataUPP(DragSendDataUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDragSendDataUPP(DragSendDataUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDragSendDataUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeDragInputUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDragInputUPP(DragInputUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDragInputUPP(DragInputUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDragInputUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeDragDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDragDrawingUPP(DragDrawingUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDragDrawingUPP(DragDrawingUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDragDrawingUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeDragSendDataUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeDragSendDataUPP(
  FlavorType       theType,
  void *           dragSendRefCon,
  DragItemRef      theItemRef,
  DragRef          theDrag,
  DragSendDataUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeDragSendDataUPP(FlavorType theType, void * dragSendRefCon, DragItemRef theItemRef, DragRef theDrag, DragSendDataUPP userUPP) { return (OSErr)CALL_FOUR_PARAMETER_UPP(userUPP, uppDragSendDataProcInfo, theType, dragSendRefCon, theItemRef, theDrag); }
  #else
    #define InvokeDragSendDataUPP(theType, dragSendRefCon, theItemRef, theDrag, userUPP) (OSErr)CALL_FOUR_PARAMETER_UPP((userUPP), uppDragSendDataProcInfo, (theType), (dragSendRefCon), (theItemRef), (theDrag))
  #endif
#endif

/*
 *  InvokeDragInputUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeDragInputUPP(
  Point *       mouse,
  SInt16 *      modifiers,
  void *        dragInputRefCon,
  DragRef       theDrag,
  DragInputUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeDragInputUPP(Point * mouse, SInt16 * modifiers, void * dragInputRefCon, DragRef theDrag, DragInputUPP userUPP) { return (OSErr)CALL_FOUR_PARAMETER_UPP(userUPP, uppDragInputProcInfo, mouse, modifiers, dragInputRefCon, theDrag); }
  #else
    #define InvokeDragInputUPP(mouse, modifiers, dragInputRefCon, theDrag, userUPP) (OSErr)CALL_FOUR_PARAMETER_UPP((userUPP), uppDragInputProcInfo, (mouse), (modifiers), (dragInputRefCon), (theDrag))
  #endif
#endif

/*
 *  InvokeDragDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
InvokeDragDrawingUPP(
  DragRegionMessage  message,
  RgnHandle          showRegion,
  Point              showOrigin,
  RgnHandle          hideRegion,
  Point              hideOrigin,
  void *             dragDrawingRefCon,
  DragRef            theDrag,
  DragDrawingUPP     userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSErr) InvokeDragDrawingUPP(DragRegionMessage message, RgnHandle showRegion, Point showOrigin, RgnHandle hideRegion, Point hideOrigin, void * dragDrawingRefCon, DragRef theDrag, DragDrawingUPP userUPP) { return (OSErr)CALL_SEVEN_PARAMETER_UPP(userUPP, uppDragDrawingProcInfo, message, showRegion, showOrigin, hideRegion, hideOrigin, dragDrawingRefCon, theDrag); }
  #else
    #define InvokeDragDrawingUPP(message, showRegion, showOrigin, hideRegion, hideOrigin, dragDrawingRefCon, theDrag, userUPP) (OSErr)CALL_SEVEN_PARAMETER_UPP((userUPP), uppDragDrawingProcInfo, (message), (showRegion), (showOrigin), (hideRegion), (hideOrigin), (dragDrawingRefCon), (theDrag))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewDragSendDataProc(userRoutine)                    NewDragSendDataUPP(userRoutine)
    #define NewDragInputProc(userRoutine)                       NewDragInputUPP(userRoutine)
    #define NewDragDrawingProc(userRoutine)                     NewDragDrawingUPP(userRoutine)
    #define CallDragSendDataProc(userRoutine, theType, dragSendRefCon, theItemRef, theDrag) InvokeDragSendDataUPP(theType, dragSendRefCon, theItemRef, theDrag, userRoutine)
    #define CallDragInputProc(userRoutine, mouse, modifiers, dragInputRefCon, theDrag) InvokeDragInputUPP(mouse, modifiers, dragInputRefCon, theDrag, userRoutine)
    #define CallDragDrawingProc(userRoutine, message, showRegion, showOrigin, hideRegion, hideOrigin, dragDrawingRefCon, theDrag) InvokeDragDrawingUPP(message, showRegion, showOrigin, hideRegion, hideOrigin, dragDrawingRefCon, theDrag, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*
  _________________________________________________________________________________________________________
      
   o INSTALLING AND REMOVING HANDLERS API'S
  _________________________________________________________________________________________________________
*/

/*
 *  InstallTrackingHandler()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
InstallTrackingHandler(
  DragTrackingHandlerUPP   trackingHandler,
  WindowRef                theWindow,
  void *                   handlerRefCon)                     TWOWORDINLINE(0x7001, 0xABED);


/*
 *  InstallReceiveHandler()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
InstallReceiveHandler(
  DragReceiveHandlerUPP   receiveHandler,
  WindowRef               theWindow,
  void *                  handlerRefCon)                      TWOWORDINLINE(0x7002, 0xABED);


/*
 *  RemoveTrackingHandler()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
RemoveTrackingHandler(
  DragTrackingHandlerUPP   trackingHandler,
  WindowRef                theWindow)                         TWOWORDINLINE(0x7003, 0xABED);


/*
 *  RemoveReceiveHandler()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
RemoveReceiveHandler(
  DragReceiveHandlerUPP   receiveHandler,
  WindowRef               theWindow)                          TWOWORDINLINE(0x7004, 0xABED);


/*
  _________________________________________________________________________________________________________
      
   o CREATING & DISPOSING
  _________________________________________________________________________________________________________
*/

/*
 *  NewDrag()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
NewDrag(DragRef * theDrag)                                    TWOWORDINLINE(0x7005, 0xABED);


/*
 *  DisposeDrag()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DisposeDrag(DragRef theDrag)                                  TWOWORDINLINE(0x7006, 0xABED);


/*
  _________________________________________________________________________________________________________
      
   o ADDING DRAG ITEM FLAVORS
  _________________________________________________________________________________________________________
*/

/*
    The method for setting Drag Manager promises differs from that for Scrap Manger promises.  This chart
    describes the method for setting drag promises via AddDragItemFlavor().
    
        dataPtr         dataSize                                result
     pointer value  actual data size    The data of size dataSize pointed to by dataPtr is added to the drag.
        NULL             ignored        A promise is placed on the drag.
*/
/*
 *  AddDragItemFlavor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
AddDragItemFlavor(
  DragRef       theDrag,
  DragItemRef   theItemRef,
  FlavorType    theType,
  const void *  dataPtr,
  Size          dataSize,
  FlavorFlags   theFlags)                                     TWOWORDINLINE(0x7007, 0xABED);


/*
 *  SetDragItemFlavorData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDragItemFlavorData(
  DragRef       theDrag,
  DragItemRef   theItemRef,
  FlavorType    theType,
  const void *  dataPtr,
  Size          dataSize,
  UInt32        dataOffset)                                   TWOWORDINLINE(0x7009, 0xABED);


/*
  _________________________________________________________________________________________________________
      
   o PROVIDING CALLBACK PROCEDURES
  _________________________________________________________________________________________________________
*/

/*
 *  SetDragSendProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDragSendProc(
  DragRef           theDrag,
  DragSendDataUPP   sendProc,
  void *            dragSendRefCon)                           TWOWORDINLINE(0x700A, 0xABED);



/*
 *  SetDragInputProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDragInputProc(
  DragRef        theDrag,
  DragInputUPP   inputProc,
  void *         dragInputRefCon)                             TWOWORDINLINE(0x700B, 0xABED);


/*
 *  SetDragDrawingProc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDragDrawingProc(
  DragRef          theDrag,
  DragDrawingUPP   drawingProc,
  void *           dragDrawingRefCon)                         TWOWORDINLINE(0x700C, 0xABED);


/*
  _________________________________________________________________________________________________________
      
   o SETTING THE DRAG IMAGE
  _________________________________________________________________________________________________________
*/

/*
 *  SetDragImage()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDragImage(
  DragRef          theDrag,
  PixMapHandle     imagePixMap,
  RgnHandle        imageRgn,
  Point            imageOffsetPt,
  DragImageFlags   theImageFlags)                             TWOWORDINLINE(0x7027, 0xABED);


/*
 *  SetDragImageWithCGImage()
 *  
 *  Discussion:
 *    Used by the sender of the drag to set the image, in CGImage
 *    format, to be displayed as user feedback during the drag.  This
 *    API may be called  at any point during the drag to update the
 *    image.
 *  
 *  Parameters:
 *    
 *    inDrag:
 *      The drag reference for which the image will be displayed.
 *    
 *    inCGImage:
 *      The CGImageRef for the image to be displayed during the drag. 
 *      The image is retained internally by the Drag Manager for the
 *      duration of the drag so it may be released by the client
 *      immediately after setting.
 *    
 *    inImageOffsetPt:
 *      A pointer to the offset from the mouse to the upper left of the
 *      image (normally expressed in negative values).  This differs
 *      from the usage of the offset passed to SetDragImage().  Here,
 *      an offset of ( -30, -30 ) will center a 60x60 pixel image on
 *      the drag mouse.
 *    
 *    inImageFlags:
 *      The flags determining image drawing during the drag.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
SetDragImageWithCGImage(
  DragRef          inDrag,
  CGImageRef       inCGImage,
  const HIPoint *  inImageOffsetPt,
  DragImageFlags   inImageFlags);


/*
  _________________________________________________________________________________________________________
      
   o ALTERING THE BEHAVIOR OF A DRAG
  _________________________________________________________________________________________________________
*/
/*
 *  ChangeDragBehaviors()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 9.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ChangeDragBehaviors(
  DragRef         theDrag,
  DragBehaviors   inBehaviorsToSet,
  DragBehaviors   inBehaviorsToClear)                         TWOWORDINLINE(0x7028, 0xABED);


/*
  _________________________________________________________________________________________________________
      
   o PERFORMING A DRAG
  _________________________________________________________________________________________________________
*/
/*
 *  TrackDrag()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
TrackDrag(
  DragRef              theDrag,
  const EventRecord *  theEvent,
  RgnHandle            theRegion)                             TWOWORDINLINE(0x700D, 0xABED);



/*
  _________________________________________________________________________________________________________
      
   o GETTING DRAG ITEM INFORMATION
  _________________________________________________________________________________________________________
*/

/*
 *  CountDragItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CountDragItems(
  DragRef   theDrag,
  UInt16 *  numItems)                                         TWOWORDINLINE(0x700E, 0xABED);


/*
 *  GetDragItemReferenceNumber()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetDragItemReferenceNumber(
  DragRef        theDrag,
  UInt16         index,
  DragItemRef *  theItemRef)                                  TWOWORDINLINE(0x700F, 0xABED);


/*
 *  CountDragItemFlavors()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
CountDragItemFlavors(
  DragRef       theDrag,
  DragItemRef   theItemRef,
  UInt16 *      numFlavors)                                   TWOWORDINLINE(0x7010, 0xABED);


/*
 *  GetFlavorType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetFlavorType(
  DragRef       theDrag,
  DragItemRef   theItemRef,
  UInt16        index,
  FlavorType *  theType)                                      TWOWORDINLINE(0x7011, 0xABED);


/*
 *  GetFlavorFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetFlavorFlags(
  DragRef        theDrag,
  DragItemRef    theItemRef,
  FlavorType     theType,
  FlavorFlags *  theFlags)                                    TWOWORDINLINE(0x7012, 0xABED);


/*
 *  GetFlavorDataSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetFlavorDataSize(
  DragRef       theDrag,
  DragItemRef   theItemRef,
  FlavorType    theType,
  Size *        dataSize)                                     TWOWORDINLINE(0x7013, 0xABED);


/*
 *  GetFlavorData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetFlavorData(
  DragRef       theDrag,
  DragItemRef   theItemRef,
  FlavorType    theType,
  void *        dataPtr,
  Size *        dataSize,
  UInt32        dataOffset)                                   TWOWORDINLINE(0x7014, 0xABED);


/*
  _________________________________________________________________________________________________________
      
   o DRAG ITEM BOUNDS
  _________________________________________________________________________________________________________
*/

/*
 *  GetDragItemBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetDragItemBounds(
  DragRef       theDrag,
  DragItemRef   theItemRef,
  Rect *        itemBounds)                                   TWOWORDINLINE(0x7015, 0xABED);


/*
 *  SetDragItemBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDragItemBounds(
  DragRef       theDrag,
  DragItemRef   theItemRef,
  const Rect *  itemBounds)                                   TWOWORDINLINE(0x7016, 0xABED);


/*
  _________________________________________________________________________________________________________
      
   o DROP LOCATIONS
  _________________________________________________________________________________________________________
*/

/*
 *  GetDropLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetDropLocation(
  DragRef   theDrag,
  AEDesc *  dropLocation)                                     TWOWORDINLINE(0x7017, 0xABED);


/*
 *  SetDropLocation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDropLocation(
  DragRef         theDrag,
  const AEDesc *  dropLocation)                               TWOWORDINLINE(0x7018, 0xABED);


/*
  _________________________________________________________________________________________________________
      
   o STANDARD DROP LOCATIONS
  _________________________________________________________________________________________________________
*/
/*
 *  GetStandardDropLocation()
 *  
 *  Discussion:
 *    Gets the standard drop location that was set by the receiver of
 *    the drag.
 *  
 *  Parameters:
 *    
 *    theDrag:
 *      The drag reference from which to retrieve the allowable drag
 *      actions.
 *    
 *    outDropLocation:
 *      A pointer to the standard drop location, set by the receiver,
 *      representing the location where the drag was dropped.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
GetStandardDropLocation(
  DragRef                 theDrag,
  StandardDropLocation *  outDropLocation)                    TWOWORDINLINE(0x7017, 0xABED);


/*
 *  SetStandardDropLocation()
 *  
 *  Discussion:
 *    Used by the receiver of the drag to set the standard drop
 *    location.
 *  
 *  Parameters:
 *    
 *    theDrag:
 *      The drag reference from which to retrieve the allowable drag
 *      actions.
 *    
 *    dropLocation:
 *      The standard drop location representing the location where the
 *      drag was dropped.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
SetStandardDropLocation(
  DragRef                theDrag,
  StandardDropLocation   dropLocation)                        TWOWORDINLINE(0x7018, 0xABED);


/*
  _________________________________________________________________________________________________________
      
   o GETTING INFORMATION ABOUT A DRAG
  _________________________________________________________________________________________________________
*/

/*
 *  GetDragAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetDragAttributes(
  DragRef           theDrag,
  DragAttributes *  flags)                                    TWOWORDINLINE(0x7019, 0xABED);


/*
 *  GetDragMouse()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetDragMouse(
  DragRef   theDrag,
  Point *   mouse,
  Point *   globalPinnedMouse)                                TWOWORDINLINE(0x701A, 0xABED);


/*
 *  SetDragMouse()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDragMouse(
  DragRef   theDrag,
  Point     globalPinnedMouse)                                TWOWORDINLINE(0x701B, 0xABED);


/*
 *  GetDragOrigin()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetDragOrigin(
  DragRef   theDrag,
  Point *   globalInitialMouse)                               TWOWORDINLINE(0x701C, 0xABED);


/*
 *  GetDragModifiers()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetDragModifiers(
  DragRef   theDrag,
  SInt16 *  modifiers,
  SInt16 *  mouseDownModifiers,
  SInt16 *  mouseUpModifiers)                                 TWOWORDINLINE(0x701D, 0xABED);


/*
  _________________________________________________________________________________________________________
      
   o ACCESSING DRAG ACTIONS
  _________________________________________________________________________________________________________
*/

/*
 *  GetDragAllowableActions()
 *  
 *  Discussion:
 *    Gets the actions the drag sender has allowed the receiver to
 *    perform. These are not requirements, but they highly suggested
 *    actions which allows the drag receiver to improve harmony across
 *    the system.  The allowable actions received are always those
 *    local to the caller's process.
 *  
 *  Parameters:
 *    
 *    theDrag:
 *      The drag reference from which to retreive the allowable drag
 *      actions.
 *    
 *    outActions:
 *      A pointer to receive the field of allowable drag actions.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
GetDragAllowableActions(
  DragRef        theDrag,
  DragActions *  outActions);


/*
 *  SetDragAllowableActions()
 *  
 *  Discussion:
 *    Sets the actions the receiver of the drag is allowed to perform. 
 *    These are not requirements, but they highly suggested actions
 *    which allows the drag receiver to improve harmony across the
 *    system.  The caller may select wether these drag actions apply to
 *    a local or remote process.
 *  
 *  Parameters:
 *    
 *    theDrag:
 *      The drag reference in which to set the allowable drag actions.
 *    
 *    inActions:
 *      A field of allowable drag actions to be set.
 *    
 *    isLocal:
 *      A boolean value allowing the drag sender to distinguish between
 *      those drag actions allowable by the local receiver versus a
 *      remote one.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
SetDragAllowableActions(
  DragRef       theDrag,
  DragActions   inActions,
  Boolean       isLocal);


/*
 *  GetDragDropAction()
 *  
 *  Discussion:
 *    Gets the action performed by the receiver of the drag.  More than
 *    one action may have been performed.
 *  
 *  Parameters:
 *    
 *    theDrag:
 *      The drag reference from which to retreive the performed drop
 *      action.
 *    
 *    outAction:
 *      A pointer to receive the drag action performed.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
GetDragDropAction(
  DragRef        theDrag,
  DragActions *  outAction);


/*
 *  SetDragDropAction()
 *  
 *  Discussion:
 *    Sets the action performed by the receiver of the drag.  More than
 *    one action may be performed.
 *  
 *  Parameters:
 *    
 *    theDrag:
 *      The drag reference in which to set the performed drop action.
 *    
 *    inAction:
 *      The drop action performed.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
SetDragDropAction(
  DragRef       theDrag,
  DragActions   inAction);


/*
  _________________________________________________________________________________________________________
      
   o DRAG HIGHLIGHTING
  _________________________________________________________________________________________________________
*/

/*
 *  ShowDragHilite()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ShowDragHilite(
  DragRef     theDrag,
  RgnHandle   hiliteFrame,
  Boolean     inside)                                         TWOWORDINLINE(0x701E, 0xABED);


/*
 *  HideDragHilite()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
HideDragHilite(DragRef theDrag)                               TWOWORDINLINE(0x701F, 0xABED);


/*
 *  DragPreScroll()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DragPreScroll(
  DragRef   theDrag,
  SInt16    dH,
  SInt16    dV)                                               TWOWORDINLINE(0x7020, 0xABED);


/*
 *  DragPostScroll()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
DragPostScroll(DragRef theDrag)                               TWOWORDINLINE(0x7021, 0xABED);


/*
 *  UpdateDragHilite()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
UpdateDragHilite(
  DragRef     theDrag,
  RgnHandle   updateRgn)                                      TWOWORDINLINE(0x7022, 0xABED);


/*
 *  GetDragHiliteColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 7.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetDragHiliteColor(
  WindowRef   window,
  RGBColor *  color)                                          TWOWORDINLINE(0x7026, 0xABED);



/*
  _________________________________________________________________________________________________________
      
   o UTILITIES
  _________________________________________________________________________________________________________
*/


/*
 *  WaitMouseMoved()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
WaitMouseMoved(Point initialGlobalMouse)                      TWOWORDINLINE(0x7023, 0xABED);



/*
 *  ZoomRects()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ZoomRects(
  const Rect *       fromRect,
  const Rect *       toRect,
  SInt16             zoomSteps,
  ZoomAcceleration   acceleration)                            TWOWORDINLINE(0x7024, 0xABED);


/*
 *  ZoomRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DragLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
ZoomRegion(
  RgnHandle          region,
  Point              zoomDistance,
  SInt16             zoomSteps,
  ZoomAcceleration   acceleration)                            TWOWORDINLINE(0x7025, 0xABED);



/*
  _________________________________________________________________________________________________________
   o OLD NAMES
     These are provided for compatiblity with older source bases.  It is recommended to not use them since
     they may removed from this interface file at any time.
  _________________________________________________________________________________________________________
*/


typedef DragRef                         DragReference;
typedef DragItemRef                     ItemReference;
#if OLDROUTINENAMES
enum {
  dragHasLeftSenderWindow       = kDragHasLeftSenderWindow, /* drag has left the source window since TrackDrag */
  dragInsideSenderApplication   = kDragInsideSenderApplication, /* drag is occurring within the sender application */
  dragInsideSenderWindow        = kDragInsideSenderWindow /* drag is occurring within the sender window */
};

enum {
  dragTrackingEnterHandler      = kDragTrackingEnterHandler, /* drag has entered handler */
  dragTrackingEnterWindow       = kDragTrackingEnterWindow, /* drag has entered window */
  dragTrackingInWindow          = kDragTrackingInWindow, /* drag is moving within window */
  dragTrackingLeaveWindow       = kDragTrackingLeaveWindow, /* drag has exited window */
  dragTrackingLeaveHandler      = kDragTrackingLeaveHandler /* drag has exited handler */
};

enum {
  dragRegionBegin               = kDragRegionBegin, /* initialize drawing */
  dragRegionDraw                = kDragRegionDraw, /* draw drag feedback */
  dragRegionHide                = kDragRegionHide, /* hide drag feedback */
  dragRegionIdle                = kDragRegionIdle, /* drag feedback idle time */
  dragRegionEnd                 = kDragRegionEnd /* end of drawing */
};

enum {
  zoomNoAcceleration            = kZoomNoAcceleration, /* use linear interpolation */
  zoomAccelerate                = kZoomAccelerate, /* ramp up step size */
  zoomDecelerate                = kZoomDecelerate /* ramp down step size */
};

enum {
  kDragStandardImage            = kDragStandardTranslucency, /* 65% image translucency (standard)*/
  kDragDarkImage                = kDragDarkTranslucency, /* 50% image translucency*/
  kDragDarkerImage              = kDragDarkerTranslucency, /* 25% image translucency*/
  kDragOpaqueImage              = kDragOpaqueTranslucency /* 0% image translucency (opaque)*/
};

#endif  /* OLDROUTINENAMES */


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

#endif /* __DRAG__ */

