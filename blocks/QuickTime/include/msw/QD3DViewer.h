/*
     File:       QD3DViewer.h
 
     Contains:   MacOS Viewer Controller Interface File.
 
     Version:    Technology: Quickdraw 3D 1.6
                 Release:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1995-1998 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __QD3DVIEWER__
#define __QD3DVIEWER__

#ifndef __QD3D__
#include <QD3D.h>
#endif

#ifndef __QD3DGROUP__
#include <QD3DGroup.h>
#endif


#if TARGET_OS_MAC
#ifndef __EVENTS__
#include <Events.h>
#endif

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

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
    #pragma options align=power
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif

#if PRAGMA_ENUM_ALWAYSINT
    #if defined(__fourbyteints__) && !__fourbyteints__ 
        #define __QD3DVIEWER__RESTORE_TWOBYTEINTS
        #pragma fourbyteints on
    #endif
    #pragma enumsalwaysint on
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=int
#elif PRAGMA_ENUM_PACK
    #if __option(pack_enums)
        #define __QD3DVIEWER__RESTORE_PACKED_ENUMS
        #pragma options(!pack_enums)
    #endif
#endif

typedef void *                          TQ3ViewerObject;

typedef CALLBACK_API_C( TQ3Status , TQ3ViewerWindowResizeCallbackMethod )(TQ3ViewerObject theViewer, const void *data);
typedef CALLBACK_API_C( TQ3Status , TQ3ViewerPaneResizeNotifyCallbackMethod )(TQ3ViewerObject theViewer, const void *data);

#if TARGET_OS_MAC
typedef CALLBACK_API_C( OSErr , TQ3ViewerDrawingCallbackMethod )(TQ3ViewerObject theViewer, const void *data);

enum {
  kQ3ViewerShowBadge            = 1 << 0,
  kQ3ViewerActive               = 1 << 1,
  kQ3ViewerControllerVisible    = 1 << 2,
  kQ3ViewerDrawFrame            = 1 << 3,
  kQ3ViewerDraggingOff          = 1 << 4,
  kQ3ViewerButtonCamera         = 1 << 5,
  kQ3ViewerButtonTruck          = 1 << 6,
  kQ3ViewerButtonOrbit          = 1 << 7,
  kQ3ViewerButtonZoom           = 1 << 8,
  kQ3ViewerButtonDolly          = 1 << 9,
  kQ3ViewerButtonReset          = 1 << 10,
  kQ3ViewerOutputTextMode       = 1 << 11,
  kQ3ViewerDragMode             = 1 << 12,
  kQ3ViewerDrawGrowBox          = 1 << 13,
  kQ3ViewerDrawDragBorder       = 1 << 14,
  kQ3ViewerDraggingInOff        = 1 << 15,
  kQ3ViewerDraggingOutOff       = 1 << 16,
  kQ3ViewerButtonOptions        = 1 << 17,
  kQ3ViewerPaneGrowBox          = 1 << 18,
  kQ3ViewerDefault              = 1 << 31
};

enum {
  kQ3ViewerEmpty                = 0,
  kQ3ViewerHasModel             = 1 << 0,
  kQ3ViewerHasUndo              = 1 << 1
};

enum TQ3ViewerCameraView {
  kQ3ViewerCameraRestore        = 0,
  kQ3ViewerCameraFit            = 1,
  kQ3ViewerCameraFront          = 2,
  kQ3ViewerCameraBack           = 3,
  kQ3ViewerCameraLeft           = 4,
  kQ3ViewerCameraRight          = 5,
  kQ3ViewerCameraTop            = 6,
  kQ3ViewerCameraBottom         = 7
};
typedef enum TQ3ViewerCameraView TQ3ViewerCameraView;




/******************************************************************************
 **                                                                          **
 **     Return viewer version number                                         **
 **                                                                          **
 *****************************************************************************/
#if CALL_NOT_IN_CARBON
/*
 *  Q3ViewerGetVersion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerGetVersion(
  unsigned long *  majorRevision,
  unsigned long *  minorRevision);



/******************************************************************************
 **                                                                          **
 **     Return viewer release version number                                 **
 **     (in 'vers' format - e.g. 0x01518000 ==> 1.5.1 release)               **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerGetReleaseVersion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerGetReleaseVersion(unsigned long * releaseRevision);



/******************************************************************************
 **                                                                          **
 **                     Creation and destruction                             **
 **             Note that this is not a QuickDraw 3D object                  **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerNew()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3ViewerObject )
Q3ViewerNew(
  CGrafPtr        port,
  Rect *          rect,
  unsigned long   flags);


/*
 *  Q3ViewerDispose()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerDispose(TQ3ViewerObject theViewer);



/******************************************************************************
 **                                                                          **
 **                 Functions to attach data to a viewer                     **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerUseFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerUseFile(
  TQ3ViewerObject   theViewer,
  long              refNum);


/*
 *  Q3ViewerUseData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerUseData(
  TQ3ViewerObject   theViewer,
  void *            data,
  long              size);



/******************************************************************************
 **                                                                          **
 **     Functions to write data out from the Viewer                          **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerWriteFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerWriteFile(
  TQ3ViewerObject   theViewer,
  long              refNum);


/*
 *  Q3ViewerWriteData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( unsigned long )
Q3ViewerWriteData(
  TQ3ViewerObject   theViewer,
  Handle            data);



/******************************************************************************
 **                                                                          **
 **     Use this function to force the Viewer to re-draw                     **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerDraw()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerDraw(TQ3ViewerObject theViewer);


/*
 *  Q3ViewerDrawContent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerDrawContent(TQ3ViewerObject theViewer);


/*
 *  Q3ViewerDrawControlStrip()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerDrawControlStrip(TQ3ViewerObject theViewer);



/******************************************************************************
 **                                                                          **
 **     Function used by the Viewer to filter and handle events              **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
Q3ViewerEvent(
  TQ3ViewerObject   theViewer,
  EventRecord *     evt);



/******************************************************************************
 **                                                                          **
 **     This function returns a PICT of the contents of the                  **
 **     Viewer's window.  The application should dispose the PICT.           **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerGetPict()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( PicHandle )
Q3ViewerGetPict(TQ3ViewerObject theViewer);



/******************************************************************************
 **                                                                          **
 **                     Calls for dealing with Buttons                       **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerGetButtonRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerGetButtonRect(
  TQ3ViewerObject   theViewer,
  unsigned long     button,
  Rect *            rect);


/*
 *  Q3ViewerGetCurrentButton()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( unsigned long )
Q3ViewerGetCurrentButton(TQ3ViewerObject theViewer);


/*
 *  Q3ViewerSetCurrentButton()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetCurrentButton(
  TQ3ViewerObject   theViewer,
  unsigned long     button);



/******************************************************************************
 **                                                                          **
 **     Functions to set/get the group to be displayed by the Viewer.        **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerUseGroup()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerUseGroup(
  TQ3ViewerObject   theViewer,
  TQ3GroupObject    group);


/*
 *  Q3ViewerGetGroup()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3GroupObject )
Q3ViewerGetGroup(TQ3ViewerObject theViewer);



/******************************************************************************
 **                                                                          **
 **     Functions to set/get the color used to clear the window              **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerSetBackgroundColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetBackgroundColor(
  TQ3ViewerObject   theViewer,
  TQ3ColorARGB *    color);


/*
 *  Q3ViewerGetBackgroundColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerGetBackgroundColor(
  TQ3ViewerObject   theViewer,
  TQ3ColorARGB *    color);



/******************************************************************************
 **                                                                          **
 **     Getting/Setting a Viewer's View object.                              **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerGetView()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( TQ3ViewObject )
Q3ViewerGetView(TQ3ViewerObject theViewer);


/*
 *  Q3ViewerRestoreView()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerRestoreView(TQ3ViewerObject theViewer);



/******************************************************************************
 **                                                                          **
 **     Calls for setting/getting viewer flags                               **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerSetFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetFlags(
  TQ3ViewerObject   theViewer,
  unsigned long     flags);


/*
 *  Q3ViewerGetFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( unsigned long )
Q3ViewerGetFlags(TQ3ViewerObject theViewer);



/******************************************************************************
 **                                                                          **
 **     Calls related to bounds/dimensions.  Bounds is the size of           **
 **     the window.  Dimensions can either be the Rect from the ViewHints    **
 **     or the current dimensions of the window (if you do a Set).           **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerSetBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetBounds(
  TQ3ViewerObject   theViewer,
  Rect *            bounds);


/*
 *  Q3ViewerGetBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerGetBounds(
  TQ3ViewerObject   theViewer,
  Rect *            bounds);


/*
 *  Q3ViewerSetDimension()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetDimension(
  TQ3ViewerObject   theViewer,
  unsigned long     width,
  unsigned long     height);


/*
 *  Q3ViewerGetDimension()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerGetDimension(
  TQ3ViewerObject   theViewer,
  unsigned long *   width,
  unsigned long *   height);


/*
 *  Q3ViewerGetMinimumDimension()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerGetMinimumDimension(
  TQ3ViewerObject   theViewer,
  unsigned long *   width,
  unsigned long *   height);



/******************************************************************************
 **                                                                          **
 **                         Port related calls                               **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerSetPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetPort(
  TQ3ViewerObject   theViewer,
  CGrafPtr          port);


/*
 *  Q3ViewerGetPort()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( CGrafPtr )
Q3ViewerGetPort(TQ3ViewerObject theViewer);



/******************************************************************************
 **                                                                          **
 **     Adjust Cursor should be called from idle loop to allow the Viewer    **
 **     to change the cursor according to the cursor position/object under   **
 **     the cursor.                                                          **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerAdjustCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
Q3ViewerAdjustCursor(
  TQ3ViewerObject   theViewer,
  Point *           pt);


/*
 *  Q3ViewerCursorChanged()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerCursorChanged(TQ3ViewerObject theViewer);



/******************************************************************************
 **                                                                          **
 **     Returns the state of the viewer.  See the constant defined at the    **
 **     top of this file.                                                    **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerGetState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( unsigned long )
Q3ViewerGetState(TQ3ViewerObject theViewer);



/******************************************************************************
 **                                                                          **
 **                         Clipboard utilities                              **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerClear()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerClear(TQ3ViewerObject theViewer);


/*
 *  Q3ViewerCut()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerCut(TQ3ViewerObject theViewer);


/*
 *  Q3ViewerCopy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerCopy(TQ3ViewerObject theViewer);


/*
 *  Q3ViewerPaste()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerPaste(TQ3ViewerObject theViewer);



/******************************************************************************
 **                                                                          **
 **                         New Event Model                                  **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerMouseDown()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
Q3ViewerMouseDown(
  TQ3ViewerObject   theViewer,
  long              x,
  long              y);


/*
 *  Q3ViewerContinueTracking()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
Q3ViewerContinueTracking(
  TQ3ViewerObject   theViewer,
  long              x,
  long              y);


/*
 *  Q3ViewerMouseUp()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
Q3ViewerMouseUp(
  TQ3ViewerObject   theViewer,
  long              x,
  long              y);


/*
 *  Q3ViewerHandleKeyEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
Q3ViewerHandleKeyEvent(
  TQ3ViewerObject   theViewer,
  EventRecord *     evt);



/******************************************************************************
 **                                                                          **
 **                                 CallBacks                                **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerSetDrawingCallbackMethod()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetDrawingCallbackMethod(
  TQ3ViewerObject                  theViewer,
  TQ3ViewerDrawingCallbackMethod   callbackMethod,
  const void *                     data);


/*
 *  Q3ViewerSetWindowResizeCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetWindowResizeCallback(
  TQ3ViewerObject                       theViewer,
  TQ3ViewerWindowResizeCallbackMethod   windowResizeCallbackMethod,
  const void *                          data);


/*
 *  Q3ViewerSetPaneResizeNotifyCallback()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetPaneResizeNotifyCallback(
  TQ3ViewerObject                           theViewer,
  TQ3ViewerPaneResizeNotifyCallbackMethod   paneResizeNotifyCallbackMethod,
  const void *                              data);



/******************************************************************************
 **                                                                          **
 **                                 Undo                                     **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerUndo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerUndo(TQ3ViewerObject theViewer);


/*
 *  Q3ViewerGetUndoString()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
Q3ViewerGetUndoString(
  TQ3ViewerObject   theViewer,
  char *            str,
  unsigned long *   cnt);



/******************************************************************************
 **                                                                          **
 **                             Camera Support                               **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerGetCameraCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerGetCameraCount(
  TQ3ViewerObject   theViewer,
  unsigned long *   cnt);


/*
 *  Q3ViewerSetCameraByNumber()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetCameraByNumber(
  TQ3ViewerObject   theViewer,
  unsigned long     cameraNo);


/*
 *  Q3ViewerSetCameraByView()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetCameraByView(
  TQ3ViewerObject       theViewer,
  TQ3ViewerCameraView   viewType);



/******************************************************************************
 **                                                                          **
 **                         Pop-up Button Options                            **
 **                                                                          **
 *****************************************************************************/
/*
 *  Q3ViewerSetRendererType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetRendererType(
  TQ3ViewerObject   theViewer,
  TQ3ObjectType     rendererType);


/*
 *  Q3ViewerGetRendererType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerGetRendererType(
  TQ3ViewerObject   theViewer,
  TQ3ObjectType *   rendererType);


/*
 *  Q3ViewerChangeBrightness()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerChangeBrightness(
  TQ3ViewerObject   theViewer,
  float             brightness);


/*
 *  Q3ViewerSetRemoveBackfaces()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetRemoveBackfaces(
  TQ3ViewerObject   theViewer,
  TQ3Boolean        remove);


/*
 *  Q3ViewerGetRemoveBackfaces()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerGetRemoveBackfaces(
  TQ3ViewerObject   theViewer,
  TQ3Boolean *      remove);


/*
 *  Q3ViewerSetPhongShading()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerSetPhongShading(
  TQ3ViewerObject   theViewer,
  TQ3Boolean        phong);


/*
 *  Q3ViewerGetPhongShading()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( OSErr )
Q3ViewerGetPhongShading(
  TQ3ViewerObject   theViewer,
  TQ3Boolean *      phong);



#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_OS_MAC */





#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
    #ifdef __QD3DVIEWER__RESTORE_TWOBYTEINTS
        #pragma fourbyteints off
    #endif
#elif PRAGMA_ENUM_OPTIONS
    #pragma option enum=reset
#elif defined(__QD3DVIEWER__RESTORE_PACKED_ENUMS)
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

#endif /* __QD3DVIEWER__ */

