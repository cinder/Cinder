/*
     File:       CarbonEvents.h
 
     Contains:   Carbon Event Manager
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2002 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __CARBONEVENTS__
#define __CARBONEVENTS__

#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

#ifndef __CARBONEVENTSCORE__
#include <CarbonEventsCore.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
#endif

#ifndef __MENUS__
#include <Menus.h>
#endif

#ifndef __CONTROLS__
#include <Controls.h>
#endif

#ifndef __MACWINDOWS__
#include <MacWindows.h>
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
 *  Discussion:
 *    These are returned from calls to TrackMouseLocation and
 *    TrackMouseRegion. Those routines are designed as replacements to
 *    calls such as StillDown and WaitMouseUp. The advantage over those
 *    routines is that TrackMouseLocation and TrackMouseRegion will
 *    block if the user is not moving the mouse, whereas mouse tracking
 *    loops based on StillDown and WaitMouseUp will spin, chewing up
 *    valuable CPU time that could be better spent elsewhere. It is
 *    highly recommended that any tracking loops in your application
 *    stop using StillDown and WaitMouseUp and start using
 *    TrackMouseLocation/Region. See the notes on those routines for
 *    more information.
 */
enum {
  kTrackMouseLocationOptionDontConsumeMouseUp = (1 << 0)
};

typedef UInt16 MouseTrackingResult;
enum {
  kMouseTrackingMouseDown       = 1,
  kMouseTrackingMouseUp         = 2,
  kMouseTrackingMouseExited     = 3,
  kMouseTrackingMouseEntered    = 4,
  kMouseTrackingMouseDragged    = 5,
  kMouseTrackingKeyModifiersChanged = 6,
  kMouseTrackingUserCancelled   = 7,
  kMouseTrackingTimedOut        = 8,
  kMouseTrackingMouseMoved      = 9
};



/*--------------------------------------------------------------------------------------*/
/*  o Helpful utilities                                                                 */
/*--------------------------------------------------------------------------------------*/

/*
 *  IsUserCancelEventRef()
 *  
 *  Discussion:
 *    Tests the event given to see whether the event represents a 'user
 *    cancel' event. Currently this is defined to be either the escape
 *    key being pressed, or command-period being pressed.
 *  
 *  Result:
 *    A boolean value indicating whether the event is a user cancel
 *    event.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsUserCancelEventRef(EventRef event);


/*
 *  TrackMouseLocation()
 *  
 *  Discussion:
 *    Once entered, this routine waits for certain mouse events (move,
 *    mouse down, mouse up). When one of these events occurs, the
 *    function returns and tells the caller what happened and where the
 *    mouse is currently located. While there is no activity, the
 *    current event loop is run, effectively blocking the current
 *    thread (save for any timers that fire). This helps to minimize
 *    CPU usage when there is nothing going on. 
 *    
 *    On Mac OS X 10.1 and earlier, and CarbonLib 1.5 and earlier,
 *    TrackMouseLocation, TrackMouseLocationWithOptions, and
 *    TrackMouseRegion only support mouse-tracking when a mouse button
 *    is pressed. They cannot be used for mouse-tracking when no mouse
 *    button is pressed; if called when no button is pressed, they will
 *    simply block until a button is pressed and will not return when
 *    the mouse is moved. On Mac OS X 10.2 and CarbonLib 1.6 and later,
 *    TrackMouseLocation, TrackMouseLocationWithOptions, and
 *    TrackMouseRegion support mouse-tracking without a pressed mouse
 *    button; TrackMouseLocation and TrackMouseLocationWithOptions
 *    return kMouseTrackingMouseMoved if the mouse is moved while no
 *    button is pressed, and TrackMouseRegion returns
 *    kMouseTrackingMouseEntered/Exited if the mouse moves into or out
 *    of the specified region while no button is pressed.
 *  
 *  Parameters:
 *    
 *    inPort:
 *      The grafport to consider for mouse coordinates. You can pass
 *      NULL for this parameter to indicate the current port. The mouse
 *      location is returned in terms of local coordinates of this
 *      port. You can pass -1 for this parameter to indicate that the
 *      mouse location should be returned in global coordinates instead
 *      of local coordinates.
 *    
 *    outPt:
 *      On exit, this parameter receives the mouse location from the
 *      last mouse event that caused this function to exit.
 *    
 *    outResult:
 *      On exit, this parameter receives a value representing what kind
 *      of event was received that cause the function to exit, such as
 *      kMouseTrackingMouseUp.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TrackMouseLocation(
  GrafPtr                inPort,          /* can be NULL */
  Point *                outPt,
  MouseTrackingResult *  outResult);


/*
 *  TrackMouseLocationWithOptions()
 *  
 *  Discussion:
 *    This routine is largely identical to TrackMouseLocation. Please
 *    read the notes on that function as well.
 *    TrackMouseLocationWithOptions supports additional parameters for
 *    leaving mouse-up events in the event queue, specifying a timeout,
 *    and retrieving the current mouse position and keyboard modifiers.
 *  
 *  Parameters:
 *    
 *    inPort:
 *      The grafport to consider for mouse coordinates. You can pass
 *      NULL for this parameter to indicate the current port. The mouse
 *      location is returned in terms of local coordinates of this
 *      port. You can pass -1 for this parameter to indicate that the
 *      mouse location should be returned in global coordinates instead
 *      of local coordinates.
 *    
 *    inOptions:
 *      The only option supported by this routine at present is
 *      kTrackMouseLocationOptionDontConsumeMouseUp, which indicates
 *      that the toolbox should leave mouse-up events in the queue. You
 *      may also pass zero for this parameter to get the default
 *      behavior, which is to remove mouse-up events from the queue
 *      before returning.
 *    
 *    inTimeout:
 *      The amount of time to wait for an event. If no events arrive
 *      within this time, kMouseTrackingTimedOut is returned in
 *      outResult.
 *    
 *    outPt:
 *      On exit, this parameter receives the mouse location from the
 *      last mouse event that caused this function to exit. If a
 *      timeout or key modifiers changed event caused this function to
 *      exit, the current mouse position at the time is returned.
 *    
 *    outModifiers:
 *      On exit, this parameter receives the most recent state of the
 *      keyboard modifiers. If a timeout caused this function to exit,
 *      the current keyboard modifiers at the time are returned.
 *    
 *    outResult:
 *      On exit, this parameter receives a value representing what kind
 *      of event was received that cause the function to exit, such as
 *      kMouseTrackingMouseUp.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TrackMouseLocationWithOptions(
  GrafPtr                inPort,             /* can be NULL */
  OptionBits             inOptions,
  EventTimeout           inTimeout,
  Point *                outPt,
  UInt32 *               outModifiers,       /* can be NULL */
  MouseTrackingResult *  outResult);


/*
 *  TrackMouseRegion()
 *  
 *  Discussion:
 *    This routine is largely identical to TrackMouseLocation. Please
 *    read the notes on that function as well. TrackMouseRegion differs
 *    from TrackMouseLocation by only returning when the mouse enters
 *    or exits a specified region that you pass in to the function, as
 *    opposed to whenever the mouse moves (it also returns for mouse
 *    up/down events). This is useful if you don't need to know
 *    intermediate mouse events, but rather just if the mouse enters or
 *    leaves an area.
 *  
 *  Parameters:
 *    
 *    inPort:
 *      The grafport to consider for mouse coordinates. You can pass
 *      NULL for this parameter to indicate the current port. You can
 *      pass -1 for this parameter to indicate that the mouse region
 *      should be interpreted in global coordinates instead of local
 *      coordinates.
 *    
 *    inRegion:
 *      The region to consider. This should be in the coordinates of
 *      the port you passed to inPort, or global coordinates if you
 *      passed -1 for the inPort parameter.
 *    
 *    ioWasInRgn:
 *      On entry, this parameter should be set to true if the mouse is
 *      currently inside the region passed in inRegion, or false if the
 *      mouse is currently outside the region. On exit, this parameter
 *      is updated to reflect the current reality; e.g. if the
 *      outResult parameter returns kMouseTrackingMouseExited,
 *      ioWasInRgn will be set to false when this function exits.
 *      Because it is updated from within, you should only need to set
 *      this yourself before the first call to this function in your
 *      tracking loop.
 *    
 *    outResult:
 *      On exit, this parameter receives a value representing what kind
 *      of event was received that cause the function to exit, such as
 *      kMouseTrackingMouseEntered.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TrackMouseRegion(
  GrafPtr                inPort,           /* can be NULL */
  RgnHandle              inRegion,
  Boolean *              ioWasInRgn,
  MouseTrackingResult *  outResult);


/*
 *  ConvertEventRefToEventRecord()
 *  
 *  Discussion:
 *    This is a convenience routine to help you if there are places in
 *    your application where you need an EventRecord and all you have
 *    is an EventRef. If the event can be converted, outEvent is filled
 *    in and the function returns true. If not, false is returned and
 *    outEvent will contain a nullEvent.
 *  
 *  Parameters:
 *    
 *    inEvent:
 *      The EventRef to convert into an EventRecord.
 *    
 *    outEvent:
 *      The EventRecord to fill out.
 *  
 *  Result:
 *    A boolean indicating if the conversion was successful (true) or
 *    not (false).
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
ConvertEventRefToEventRecord(
  EventRef       inEvent,
  EventRecord *  outEvent);


/*
 *  IsEventInMask()
 *  
 *  Discussion:
 *    This is a companion function for ConvertEventRefToEventRecord,
 *    and is provided as a convenience routine to help you if there are
 *    places in your application where you want to check an EventRef to
 *    see if it matches a classic EventMask bitfield. If the event
 *    matches, the function returns true.
 *  
 *  Parameters:
 *    
 *    inEvent:
 *      The EventRef to convert into an EventRecord.
 *    
 *    inMask:
 *      The mask to consider.
 *  
 *  Result:
 *    A boolean indicating if the event was considered to be in the
 *    mask provided.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsEventInMask(
  EventRef    inEvent,
  EventMask   inMask);


/*
 *  GetLastUserEventTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( EventTime )
GetLastUserEventTime(void);


/*--------------------------------------------------------------------------------------*/
/*  o Mouse Coalescing                                                                  */
/*--------------------------------------------------------------------------------------*/
/*
 *  IsMouseCoalescingEnabled()
 *  
 *  Discussion:
 *    Returns true if mouse coalescing is current enabled. When
 *    enabled, we coalesce mouse moved and mouse dragged events. By
 *    default, coalescing is on, but you can use
 *    SetMouseCoalescingEnabled to disable it if you want finer-grained
 *    mouse movement events, which is useful for drawing with tablets.
 *  
 *  Result:
 *    A boolean indicating if coalescing is enabled.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( Boolean )
IsMouseCoalescingEnabled(void);


/*
 *  SetMouseCoalescingEnabled()
 *  
 *  Discussion:
 *    Allows you to set mouse move/drag event coalescing on or off. By
 *    default, coalescing is on, but you can use this function to
 *    disable it if you want finer-grained mouse movement events, which
 *    is useful for drawing with tablets.
 *  
 *  Parameters:
 *    
 *    inNewState:
 *      A boolean indicating if coalescing should be enabled (true) or
 *      disabled (false).
 *    
 *    outOldState:
 *      A boolean which receives the prior state of mouse coalescing
 *      for restoration later. You can pass NULL for this parameter if
 *      you don't care.
 *  
 *  Result:
 *    An operating system status result.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
SetMouseCoalescingEnabled(
  Boolean    inNewState,
  Boolean *  outOldState);      /* can be NULL */



/*======================================================================================*/
/*  EVENT CLASSES                                                                       */
/*======================================================================================*/

/*
 *  Discussion:
 *    Event classes
 */
enum {

  /*
   * Events related to the mouse (mouse down/up/moved).
   */
  kEventClassMouse              = FOUR_CHAR_CODE('mous'),

  /*
   * Events related to the keyboard.
   */
  kEventClassKeyboard           = FOUR_CHAR_CODE('keyb'),

  /*
   * Events related to text input (by keyboard, or by input method).
   */
  kEventClassTextInput          = FOUR_CHAR_CODE('text'),

  /*
   * Application-level events (launch, quit, etc.).
   */
  kEventClassApplication        = FOUR_CHAR_CODE('appl'),

  /*
   * Apple Events.
   */
  kEventClassAppleEvent         = FOUR_CHAR_CODE('eppc'),

  /*
   * Menu-related events.
   */
  kEventClassMenu               = FOUR_CHAR_CODE('menu'),

  /*
   * Window-related events.
   */
  kEventClassWindow             = FOUR_CHAR_CODE('wind'),

  /*
   * Control-related events.
   */
  kEventClassControl            = FOUR_CHAR_CODE('cntl'),

  /*
   * Command events (HICommands).
   */
  kEventClassCommand            = FOUR_CHAR_CODE('cmds'),

  /*
   * Events related to tablets.
   */
  kEventClassTablet             = FOUR_CHAR_CODE('tblt'),

  /*
   * Events related to File Manager volumes.
   */
  kEventClassVolume             = FOUR_CHAR_CODE('vol '),

  /*
   * Events related to the Appearance Manager.
   */
  kEventClassAppearance         = FOUR_CHAR_CODE('appm'),

  /*
   * Events related to the Services Manager.
   */
  kEventClassService            = FOUR_CHAR_CODE('serv'),

  /*
   * Events related to toolbars.
   */
  kEventClassToolbar            = FOUR_CHAR_CODE('tbar'),

  /*
   * Events related to toolbar items.
   */
  kEventClassToolbarItem        = FOUR_CHAR_CODE('tbit'),

  /*
   * Events related application accessibility.
   */
  kEventClassAccessibility      = FOUR_CHAR_CODE('acce')
};

/*--------------------------------------------------------------------------------------*/
/* Mouse Events                                                                         */
/*--------------------------------------------------------------------------------------*/
/* Mouse Events */

/*
 *  Discussion:
 *    Mouse events (kEventClassMouse)
 */
enum {

  /*
   * A mouse button was pressed.
   */
  kEventMouseDown               = 1,

  /*
   * A mouse button was released.
   */
  kEventMouseUp                 = 2,

  /*
   * The mouse was moved.
   */
  kEventMouseMoved              = 5,

  /*
   * The mouse was moved, and a button was down.
   */
  kEventMouseDragged            = 6,

  /*
   * The mouse entered a tracking area.
   */
  kEventMouseEntered            = 8,

  /*
   * The mouse exited a tracking area.
   */
  kEventMouseExited             = 9,

  /*
   * The mouse wheel was moved. (Mac OS X only)
   */
  kEventMouseWheelMoved         = 10
};

/*
    Parameters for mouse events:

    // NOTE: As of Mac OS X 10.1, mouse events carry more information which allow you
    //       to do less work and gain accuracy of hit testing. First, there is the
    //       kEventParamWindowRef parameter. This parameter tells you what window the
    //       mouse click/move/etc occurred over. In mouse dragged events, this is the
    //       window the mouse went down in, NOT the window the mouse is currently over.
    //       Next, there is the kEventParamWindowMouseLocation parameter. This is the
    //       window-relative position of the mouse in the window given in the
    //       kEventParamWindowRef parameter. 0, 0 is at the top left of the structure
    //       of the window.

    kEventMouseDown
        -->     kEventParamMouseLocation        typeHIPoint
        -->     kEventParamWindowRef            typeWindowRef (Mac OS X 10.1 or later)
        -->     kEventParamWindowMouseLocation  typeHIPoint (Mac OS X 10.1 or later)
        -->     kEventParamKeyModifiers         typeUInt32
        -->     kEventParamMouseButton          typeMouseButton
        -->     kEventParamClickCount           typeUInt32
        -->     kEventParamMouseChord           typeUInt32  (Mac OS X only)
        -->     kEventParamTabletEventType      typeUInt32  (Mac OS X 10.1, CarbonLib 1.5, and later)
        -->     kEventParamTabletPointRec       typeTabletPointRec          (if kEventParamTabletEventType is kEventTabletPoint)
        -->     kEventParamTabletProximityRec   typeTabletProximityRec  (if kEventParamTabletEventType is kEventTabletProximity)
        
    kEventMouseUp
        -->     kEventParamMouseLocation        typeHIPoint
        -->     kEventParamWindowRef            typeWindowRef (Mac OS X 10.1 or later)
        -->     kEventParamWindowMouseLocation  typeHIPoint (Mac OS X 10.1 or later)
        -->     kEventParamKeyModifiers         typeUInt32
        -->     kEventParamMouseButton          typeMouseButton
        -->     kEventParamClickCount           typeUInt32
        -->     kEventParamMouseChord           typeUInt32  (Mac OS X only)
        -->     kEventParamTabletEventType      typeUInt32  (Mac OS X 10.1, CarbonLib 1.5, and later)
        -->     kEventParamTabletPointRec       typeTabletPointRec          (if kEventParamTabletEventType is kEventTabletPoint)
        -->     kEventParamTabletProximityRec   typeTabletProximityRec  (if kEventParamTabletEventType is kEventTabletProximity)

    kEventMouseMoved
        -->     kEventParamMouseLocation        typeHIPoint
        -->     kEventParamWindowRef            typeWindowRef (Mac OS X 10.1 or later)
        -->     kEventParamWindowMouseLocation  typeHIPoint (Mac OS X 10.1 or later)
        -->     kEventParamMouseDelta           typeHIPoint (Mac OS X only)
        -->     kEventParamKeyModifiers         typeUInt32
        -->     kEventParamTabletEventType      typeUInt32  (Mac OS X 10.1, CarbonLib 1.5, and later)
        -->     kEventParamTabletPointRec       typeTabletPointRec          (if kEventParamTabletEventType is kEventTabletPoint)
        -->     kEventParamTabletProximityRec   typeTabletProximityRec  (if kEventParamTabletEventType is kEventTabletProximity)

    kEventMouseDragged
        -->     kEventParamMouseLocation        typeHIPoint
        -->     kEventParamWindowRef            typeWindowRef (Mac OS X 10.1 or later)
        -->     kEventParamWindowMouseLocation  typeHIPoint (Mac OS X 10.1 or later)
        -->     kEventParamMouseDelta           typeHIPoint (Mac OS X only)
        -->     kEventParamKeyModifiers         typeUInt32
        -->     kEventParamMouseButton          typeMouseButton
        -->     kEventParamMouseChord           typeUInt32  (Mac OS X only)
        -->     kEventParamTabletEventType      typeUInt32  (Mac OS X 10.1, CarbonLib 1.5, and later)
        -->     kEventParamTabletPointRec       typeTabletPointRec          (if kEventParamTabletEventType is kEventTabletPoint)
        -->     kEventParamTabletProximityRec   typeTabletProximityRec  (if kEventParamTabletEventType is kEventTabletProximity)

    kEventMouseEntered
        -->     kEventParamMouseTrackingRef     typeMouseTrackingRef
        -->     kEventParamWindowRef            typeWindowRef
        -->     kEventParamMouseLocation        typeHIPoint
        -->     kEventParamWindowMouseLocation  typeHIPoint
        -->     kEventParamKeyModifiers         typeUInt32

    kEventMouseExited
        -->     kEventParamMouseTrackingRef     typeMouseTrackingRef
        -->     kEventParamWindowRef            typeWindowRef
        -->     kEventParamMouseLocation        typeHIPoint
        -->     kEventParamWindowMouseLocation  typeHIPoint
        -->     kEventParamKeyModifiers         typeUInt32

    kEventMouseWheelMoved
        -->     kEventParamMouseLocation        typeHIPoint
        -->     kEventParamWindowRef            typeWindowRef (Mac OS X 10.1 or later)
        -->     kEventParamWindowMouseLocation  typeHIPoint (Mac OS X 10.1 or later)
        -->     kEventParamKeyModifiers         typeUInt32
        -->     kEventParamMouseWheelAxis       typeMouseWheelAxis
        -->     kEventParamMouseWheelDelta      typeLongInteger
*/

/*
 *  EventMouseButton
 *  
 */
typedef UInt16 EventMouseButton;
enum {

  /*
   * Only button for a one-button mouse (usually left button for
   * multi-button mouse)
   */
  kEventMouseButtonPrimary      = 1,

  /*
   * Usually right button for a multi-button mouse
   */
  kEventMouseButtonSecondary    = 2,

  /*
   * Usually middle button for a three-button mouse
   */
  kEventMouseButtonTertiary     = 3
};



/*
 *  EventMouseWheelAxis
 *  
 */
typedef UInt16 EventMouseWheelAxis;
enum {

  /*
   * The X axis (left or right)
   */
  kEventMouseWheelAxisX         = 0,

  /*
   * The Y axis (up or down)
   */
  kEventMouseWheelAxisY         = 1
};




/*--------------------------------------------------------------------------------------*/
/* Text Input Events                                                                    */
/*--------------------------------------------------------------------------------------*/

/*
 *  Summary:
 *    Text input events (kEventClassTextInput)
 *  
 *  Discussion:
 *    The following TextInput events reimplement the AppleEvents
 *    defined in Inside Mac Text - Text Services Manager, and provide
 *    the benefits of Carbon Event targeting, dispatching and
 *    propagation to applications that have formerly handled the TSM
 *    suite of AppleEvents. TextInput handlers may be installed on
 *    controls, windows, or the application event target (equivalent to
 *    AppleEvent-based handling). In all cases, if a given TextInput
 *    handler is not installed, TSM will convert that TextInput to an
 *    AppleEvent and redispatch via AESend to the current process,
 *    making adoption as gradual as is desired.
 */
enum {

  /*
   * Tells the application/text engine to initiate/terminate or manage
   * the content of inline input session.
   */
  kEventTextInputUpdateActiveInputArea = 1,

  /*
   * Unicode text resulting from a key event originated by TSM (not by
   * an input method).  A client need not be fully TSM-aware to process
   * or receive this event, which has become the standard way of
   * getting Unicode text from key events.  You can also get Mac
   * encoding characters from the raw keyboard event contained in this
   * event.  If no UnicodeForKeyEvent handler is installed, and no
   * kUnicodeNotFromInputMethod AppleEvent handler is installed (or the
   * application has not created a Unicode TSMDocument), the Mac
   * encoding charCodes (if these can be converted from the Unicodes)
   * are provided to WaitNextEvent.
   */
  kEventTextInputUnicodeForKeyEvent = 2,

  /*
   * Convert from inline session text offset to global QD Point.  This
   * event is typically be produced by an Input Method so that it can
   * best position a palette "near" the text being operated on by the
   * user.
   */
  kEventTextInputOffsetToPos    = 3,

  /*
   * Convert from global QD point to inline session text offset.  This
   * event is typically produced by an input method to perform proper
   * cursor management as the cursor moves over various subranges, or
   * clauses of text (or the boundaries between these) in the inline
   * input session.
   */
  kEventTextInputPosToOffset    = 4,

  /*
   * Show/Hide the bottom line input window.  This event is produced by
   * Input Methods to control the Text Services Manager bottom-line
   * input window, and is not normally handled by an application.
   */
  kEventTextInputShowHideBottomWindow = 5,

  /*
   * Get the text selected (or character before/after insertion point
   * based on leadingEdge parameter) from the application's text engine.
   */
  kEventTextInputGetSelectedText = 6,
  kEventTextInputUnicodeText    = 7
};

/*
    Parameters for TextInput events:
    
    kEventTextInputUpdateActiveInputArea
        Required parameters:
        -->     kEventParamTextInputSendComponentInstance           typeComponentInstance
        -->     kEventParamTextInputSendRefCon                      typeLongInteger
        -->     kEventParamTextInputSendSLRec                       typeIntlWritingCode
        -->     kEventParamTextInputSendFixLen                      typeLongInteger
        -->     kEventParamTextInputSendText                        typeUnicodeText, typeChar
                    (data type depends on TSMDocument type created via NewTSMDocument...
                     typeChar for kTextService document, typeUnicodeText for kUnicodeDocument)
        
        Optional parameters:
        -->     kEventParamTextInputSendUpdateRng                   typeTextRangeArray
        -->     kEventParamTextInputSendHiliteRng                   typeTextRangeArray
        -->     kEventParamTextInputSendClauseRng                   typeOffsetArray
        -->     kEventParamTextInputSendPinRng                      typeTextRange
        -->     kEventParamTextInputSendPinRng                      typeTextRange
        -->     kEventParamTextInputSendTextServiceEncoding         typeUInt32
        -->     kEventParamTextInputSendTextServiceMacEncoding      typeUInt32
        -->     kEventParamTextInputGlyphInfoArray                  typeGlyphInfoArray
    
    kEventTextInputUnicodeForKeyEvent
        Required parameters:
        -->     kEventParamTextInputSendComponentInstance           typeComponentInstance
        -->     kEventParamTextInputSendRefCon                      typeLongInteger
        -->     kEventParamTextInputSendSLRec                       typeIntlWritingCode
        -->     kEventParamTextInputSendText                        typeUnicodeText
        -->     kEventParamTextInputSendKeyboardEvent               typeEventRef
                    (This parameter is the original raw keyboard event that produced the
                     text.  It enables access to kEventParamKeyModifiers and 
                     kEventParamKeyCode parameters.
                     You can also extract from this event either Unicodes or Mac encoding
                     characters as follows:
                            kEventParamKeyUnicodes              typeUnicodeText
                            kEventParamKeyMacCharCodes          typeChar (if available)
                     The kEventParamKeyUnicodes parameter of the raw keyboard event is
                     identical to the TextInput event's kEventParamTextInputSendText
                     parameter.  Note that when contents of TSM's bottom-line input
                     window (i.e. during typing Chinese, Korean, or Japanese) are confirmed,
                     the raw keyboard event's keyCode and modifiers are set to default values.)
        -->     kEventParamTextInputGlyphInfoArray                  typeGlyphInfoArray

    kEventTextInputOffsetToPos
        Required parameters:
        -->     kEventParamTextInputSendComponentInstance           typeComponentInstance
        -->     kEventParamTextInputSendRefCon                      typeLongInteger
        -->     kEventParamTextInputSendTextOffset                  typeLongInteger
        <--     kEventParamTextInputReplyPoint                      typeQDPoint
        
        Optional parameters:
        -->     kEventParamTextInputSendSLRec                       typeIntlWritingCode
        -->     kEventParamTextInputSendLeadingEdge                 typeBoolean
        <--     kEventParamTextInputReplySLRec                      typeIntlWritingCode
        <--     kEventParamTextInputReplyFont                       typeLongInteger
        <--     kEventParamTextInputReplyFMFont                     typeUInt32
        <--     kEventParamTextInputReplyPointSize                  typeFixed
        <--     kEventParamTextInputReplyLineHeight                 typeShortInteger
        <--     kEventParamTextInputReplyLineAscent                 typeShortInteger
        <--     kEventParamTextInputReplyTextAngle                  typeFixed

    kEventTextInputPosToOffset
        Required parameters:
        -->     kEventParamTextInputSendComponentInstance           typeComponentInstance
        -->     kEventParamTextInputSendRefCon                      typeLongInteger
        -->     kEventParamTextInputSendCurrentPoint                typeQDPoint
        <--     kEventParamTextInputReplyRegionClass                typeLongInteger
        <--     kEventParamTextInputReplyTextOffset                 typeLongInteger
                    (required if the position is inside the document's body)
        
        Optional parameters:
        -->     kEventParamTextInputSendDraggingMode                typeBoolean
        <--     kEventParamTextInputReplyLeadingEdge                typeBoolean
        <--     kEventParamTextInputReplySLRec                      typeIntlWritingCode
        
    kEventTextInputShowHideBottomWindow
        Required parameters:
        -->     kEventParamTextInputSendComponentInstance           typeComponentInstance
        -->     kEventParamTextInputSendRefCon                      typeLongInteger
        
        Optional parameters:
        -->     kEventParamTextInputSendShowHide                    typeBoolean
        <--     kEventParamTextInputReplyShowHide                   typeBoolean

    kEventTextInputGetSelectedText
        Required parameters:
        -->     kEventParamTextInputSendComponentInstance           typeComponentInstance
        -->     kEventParamTextInputSendRefCon                      typeLongInteger
        
        Optional parameters:
        -->     kEventParamTextInputSendLeadingEdge                 typeBoolean
        -->     kEventParamTextInputSendTextServiceEncoding         typeUInt32
        -->     kEventParamTextInputSendTextServiceMacEncoding      typeUInt32
        <--     kEventParamTextInputReplyText                       typeUnicodeText, typeChar
                    (data type depends on TSMDocument.  See kEventTextInputUpdateActiveInputArea Notes)
        <--     kEventParamTextInputReplySLRec                      typeIntlWritingCode
        <--     kEventParamTextInputGlyphInfoArray                  typeGlyphInfoArray

    kEventTextInputUnicodeText
        Required parameters:
        -->     kEventParamTextInputSendComponentInstance           typeComponentInstance
        -->     kEventParamTextInputSendSLRec                       typeIntlWritingCode
        -->     kEventParamTextInputSendText                        typeUnicodeText
        
        Optional parameters:
        -->     kEventParamTextInputSendTextServiceEncoding         typeUInt32
        -->     kEventParamTextInputSendTextServiceMacEncoding      typeUInt32
        -->     kEventParamTextInputGlyphInfoArray                  typeGlyphInfoArray

*/
/*--------------------------------------------------------------------------------------*/
/* Raw Keyboard Events                                                                  */
/*--------------------------------------------------------------------------------------*/

/*
 *  Summary:
 *    Keyboard events (kEventClassKeyboard)
 *  
 *  Discussion:
 *    These events are the lowest-level keyboard events.
 */
enum {

  /*
   * A key was pressed.
   */
  kEventRawKeyDown              = 1,    /* hardware-level events*/

  /*
   * Sent periodically as a key is held down by the user.
   */
  kEventRawKeyRepeat            = 2,

  /*
   * A key was released.
   */
  kEventRawKeyUp                = 3,

  /*
   * The keyboard modifiers (bucky bits) have changed.
   */
  kEventRawKeyModifiersChanged  = 4,

  /*
   * A registered Hot Key was pressed.
   */
  kEventHotKeyPressed           = 5,

  /*
   * A registered Hot Key was released (this is only sent on Mac OS X).
   */
  kEventHotKeyReleased          = 6     /* Mac OS X only*/
};



/*
 *  Summary:
 *    Key modifier change event bits and masks
 *  
 *  Discussion:
 *    From bit 8, cmdKeyBit, to bit 15, rightControlKeyBit, are
 *    compatible with Event Manager modifiers.
 */
enum {

  /*
   * The Num Lock state bit (Mac OS X only).
   */
  kEventKeyModifierNumLockBit   = 16,   /* Num Lock is on? (Mac OS X only)*/

  /*
   * The Fn key state bit (Mac OS X only).
   */
  kEventKeyModifierFnBit        = 17    /* Fn key is down? (Mac OS X only)*/
};

enum {
  kEventKeyModifierNumLockMask  = 1L << kEventKeyModifierNumLockBit,
  kEventKeyModifierFnMask       = 1L << kEventKeyModifierFnBit
};

/*
    Parameters for keyboard events:

    kEventRawKeyDown
        -->     kEventParamKeyMacCharCodes  typeChar
        -->     kEventParamKeyCode          typeUInt32
        -->     kEventParamKeyModifiers     typeUInt32
        -->     kEventParamKeyboardType     typeUInt32

    kEventRawKeyRepeat
        -->     kEventParamKeyMacCharCodes  typeChar
        -->     kEventParamKeyCode          typeUInt32
        -->     kEventParamKeyModifiers     typeUInt32
        -->     kEventParamKeyboardType     typeUInt32

    kEventRawKeyUp
        -->     kEventParamKeyMacCharCodes  typeChar
        -->     kEventParamKeyCode          typeUInt32
        -->     kEventParamKeyModifiers     typeUInt32
        -->     kEventParamKeyboardType     typeUInt32

    kEventRawKeyModifiersChanged
        -->     kEventParamKeyModifiers     typeUInt32

    kEventHotKeyPressed
        -->     kEventParamDirectObject     typeEventHotKeyID

    kEventHotKeyReleased
        -->     kEventParamDirectObject     typeEventHotKeyID
*/

/*--------------------------------------------------------------------------------------*/
/* Application Events                                                                   */
/*--------------------------------------------------------------------------------------*/

/*
 *  Summary:
 *    Application events (kEventClassApplication)
 */
enum {

  /*
   * The current app has been activated (resume event). May optionally
   * contain a kEventParamWindowRef parameter if a click on an
   * application window was the cause of the app activation.
   */
  kEventAppActivated            = 1,

  /*
   * The current app has just been deactivated (suspend event).
   */
  kEventAppDeactivated          = 2,

  /*
   * Request to quit. Sent by QuitApplicationEventLoop to the
   * application target. In response, the event handler installed by
   * RunApplicationEventLoop will cause RunApplicationEventLoop to
   * exit. Most applications will not need to handle this event;
   * instead, an application should install an AppleEvent handler for
   * the kAEQuitApplication event handler if it needs to check for
   * unsaved documents or do other cleanup before quitting.
   */
  kEventAppQuit                 = 3,

  /*
   * An async launch request response. This event is received when you
   * use LaunchServices to launch a process asynchronously. It is only
   * received on Mac OS X.
   */
  kEventAppLaunchNotification   = 4,

  /*
   * Another app was launched. Available on Mac OS X and CarbonLib
   * 1.3.1 and later.
   */
  kEventAppLaunched             = 5,

  /*
   * Another app terminated. Available on Mac OS X and CarbonLib 1.3.1
   * and later.
   */
  kEventAppTerminated           = 6,

  /*
   * The front (active) application has changed. Available on Mac OS X
   * and CarbonLib 1.3.1 and later.
   */
  kEventAppFrontSwitched        = 7,

  /*
   * The user has requested keyboard focus on the menubar. This event
   * is handled automatically by the default application event handler
   * installed by the Carbon Event Manager. Sent on Mac OS X 10.2 and
   * later.
   */
  kEventAppFocusMenuBar         = 8,

  /*
   * The user has requested keyboard focus on a document window. The
   * application should cycle to the next (or previous, if the shift
   * key is down) document window, or if there are no more windows to
   * activate in the application's window list, to the next or previous
   * document window in the next or previous process. User focus (see
   * SetUserFocusWindow) should be applied to the new front document
   * window. If something other than a document window has the focus at
   * the time you receive this event, the frontmost document window
   * should be given the user focus instead, and no z-order change
   * should be made. Additionally, the keyboard focus should be moved
   * to the main control in the newly focused window if no keyboard
   * focus exists within the window. This event is handled
   * automatically by the default application event handler installed
   * by the Carbon Event Manager. An application which chooses to
   * handle this event should never override it entirely; if necessary,
   * it should only check if the user focus is somewhere other than a
   * document window, and if so, set the focus on the active document
   * window. If the focus is already on a document window, a handler
   * for this event should always return eventNotHandledErr so that the
   * default handler can rotate to the next window across all
   * processes. Sent on Mac OS X 10.2 and later.
   */
  kEventAppFocusNextDocumentWindow = 9,

  /*
   * The application should cycle to the next (or previous, if the
   * shift key is down) floating window in the application. User focus
   * (see SetUserFocusWindow) should be applied to the new front
   * floating window. If something other than a floating window has the
   * focus at the time you receive this event, the frontmost floating
   * window should be given the user focus instead, and no z-order
   * change should be made. Additionally, the keyboard focus should be
   * moved to the main control in the newly focused window if no
   * keyboard focus exists within the window. This event is handled
   * automatically by the default application event handler installed
   * by the Carbon Event Manager. The default handler sends a
   * kEventCommandProcess event containing
   * kHICommandRotateFloatingWindowsForward/Backward when it detects
   * that floating windows should be cycled. Sent on Mac OS X 10.2 and
   * later.
   */
  kEventAppFocusNextFloatingWindow = 10,

  /*
   * The application should put focus on the first control in the
   * toolbar in the focused window, if a toolbar is present. For
   * windows that use the standard Toolbar control, this event is
   * handled automatically by the default application event handler
   * installed by the Carbon Event Manager. Sent on Mac OS X 10.2 and
   * later.
   */
  kEventAppFocusToolbar         = 11,

  /*
   * A request for a menu to be displayed by the application's dock
   * tile. The default handler will return the menu, if any, that was
   * provided by the SetApplicationDockTileMenu API. The sender of this
   * event will release the menu after the Dock has displayed it, so if
   * you return a permanently allocated MenuRef, you should call
   * RetainMenu on it before returning from your event handler. For
   * most applications, it will be easier to use the
   * SetApplicationDockTileMenu API directly rather than installing a
   * handler for this event. Available in Mac OS X 10.1 and later.
   */
  kEventAppGetDockTileMenu      = 20,

  /*
   * The application was just hidden. Sent on Mac OS X 10.2 and later.
   */
  kEventAppHidden               = 107,

  /*
   * The application was just shown. It was previously hidden. Sent on
   * Mac OS X 10.2 and later.
   */
  kEventAppShown                = 108,

  /*
   * The system UI mode of the frontmost application has changed. Sent
   * on Mac OS X 10.2 and later.
   */
  kEventAppSystemUIModeChanged  = 109
};

/*
    Parameters for application events:

    kEventAppActivated
        -->     kEventParamWindowRef        typeWindowRef
    
    kEventAppDeactivated
        (no parameters)
    
    kEventAppQuit
        (no parameters)
    
    kEventAppLaunchNotification
        -->     kEventParamProcessID        typeProcessSerialNumber
        -->     kEventParamLaunchRefCon     typeUInt32
        -->     kEventParamLaunchErr        typeOSStatus
        
    kEventAppLaunched
        -->     kEventParamProcessID        typeProcessSerialNumber
        
    kEventAppTerminated
        -->     kEventParamProcessID        typeProcessSerialNumber
        
    kEventAppFrontSwitched
        -->     kEventParamProcessID        typeProcessSerialNumber
    
    kEventAppFocusMenuBar
        -->     kEventParamKeyModifiers     typeUInt32
        
    kEventAppFocusNextDocumentWindow
        -->     kEventParamKeyModifiers     typeUInt32
        
    kEventAppFocusNextFloatingWindow
        -->     kEventParamKeyModifiers     typeUInt32
        
    kEventAppFocusToolbar
        -->     kEventParamKeyModifiers     typeUInt32
        
    kEventAppGetDockTileMenu
        <--     kEventParamMenuRef          typeMenuRef

    kEventAppHidden
        (no parameters)

    kEventAppShown
        (no parameters)
        
    kEventAppSystemUIModeChanged
        -->     kEventParamSystemUIMode     typeUInt32
*/
/*--------------------------------------------------------------------------------------*/
/*  Apple Events                                                                        */
/*--------------------------------------------------------------------------------------*/

/*
 *  Summary:
 *    Apple events (kEventClassAppleEvent)
 */
enum {

  /*
   * Sent when a high-level event is received. The default handler will
   * call AEProcessAppleEvent.
   */
  kEventAppleEvent              = 1
};


/*
    Parameters for Apple events:

    kEventAppleEvent
        -->     kEventParamAEEventClass     typeType        // the eventClass of the Apple event
        -->     kEventParamAEEventID        typeType        // the eventID of the Apple event
*/
/*--------------------------------------------------------------------------------------*/
/*  Window Events                                                                       */
/*--------------------------------------------------------------------------------------*/

/*
 *  Summary:
 *    Window refresh events (kEventClassWindow)
 *  
 *  Discussion:
 *    Events related to drawing a window's content.
 */
enum {

  /*
   * Low-level update event. Sent to any window that needs updating
   * regardless of whether the window has the standard handler
   * installed. You must call BeginUpdate, call SetPort, draw your
   * window content, and then call EndUpdate.
   */
  kEventWindowUpdate            = 1,

  /*
   * Only sent to windows with the standard handler installed.
   * BeginUpdate, SetPort, and EndUpdate are called for you; all you do
   * is draw. No, really.
   */
  kEventWindowDrawContent       = 2
};

/*
    Parameters for window refresh events:

    kEventWindowUpdate
        -->     kEventParamDirectObject     typeWindowRef

    kEventWindowDrawContent
        -->     kEventParamDirectObject     typeWindowRef
*/

/*
 *  Summary:
 *    Window activation events (kEventClassWindow)
 *  
 *  Discussion:
 *    Events related to activating and deactivating a window.
 */
enum {

  /*
   * The window is active now. Sent to any window that is activated,
   * regardless of whether the window has the standard handler
   * installed.
   */
  kEventWindowActivated         = 5,

  /*
   * The window is inactive now. Sent to any window that is
   * deactivated, regardless of whether the window has the standard
   * handler installed.
   */
  kEventWindowDeactivated       = 6,

  /*
   * Sent when a click occurs in a background window. Only sent to
   * windows with the standard handler installed. The default behavior
   * is to bring the window forward and eat the click. You have the
   * option of overriding the behavior to support click-thru or
   * select-and-click.
   */
  kEventWindowGetClickActivation = 7
};

/*
    Parameters for window activation events:

    kEventWindowActivated
        -->     kEventParamDirectObject     typeWindowRef
        
    kEventWindowDeactivated
        -->     kEventParamDirectObject     typeWindowRef

    kEventWindowGetClickActivation
        -->     kEventParamDirectObject     typeWindowRef
        -->     kEventParamMouseLocation    typeQDPoint
        -->     kEventParamKeyModifiers     typeUInt32
        -->     kEventParamWindowDefPart    typeWindowDefPartCode
        -->     kEventParamControlRef       typeControlRef (only present if the click was on a control)
        <--     kEventParamClickActivation  typeClickActivationResult
*/

/*
 *  Summary:
 *    Window state change events (kEventClassWindow)
 *  
 *  Discussion:
 *    Events that notify of a change in the window's state. These
 *    events are sent to all windows, regardless of whether the window
 *    has the standard handler installed.
 */
enum {

  /*
   * A window is being shown. This is sent inside ShowHide.
   */
  kEventWindowShowing           = 22,

  /*
   * A window is being hidden. This is sent inside ShowHide.
   */
  kEventWindowHiding            = 23,

  /*
   * Indicates that the window has been shown.
   */
  kEventWindowShown             = 24,

  /*
   * Indicates that the window has been hidden.
   */
  kEventWindowHidden            = 25,

  /*
   * Notification that the window is about to collapse. Available in
   * Mac OS X 10.1 and later.
   */
  kEventWindowCollapsing        = 86,

  /*
   * Notification that the window has successfully collapsed.
   */
  kEventWindowCollapsed         = 67,

  /*
   * Notification that the window is about to expand. Available in Mac
   * OS X 10.1 and later.
   */
  kEventWindowExpanding         = 87,

  /*
   * Notification that the window has successfully expanded.
   */
  kEventWindowExpanded          = 70,

  /*
   * Notification that the window has been successfully zoomed. In
   * CarbonLib 1.1 through CarbonLib 1.4, and Mac OS X 10.0 through
   * 10.1.x, this event is only sent by the standard window event
   * handler after handling kEventWindowZoom; starting with CarbonLib
   * 1.5 and Mac OS X 10.2, this event is sent by ZoomWindow and
   * ZoomWindowIdeal.
   */
  kEventWindowZoomed            = 76,

  /*
   * Sent during DragWindow or ResizeWindow, before the window is
   * actually moved or resized. Alter the current bounds
   * (kEventParamCurrentBounds) in the event to change the eventual
   * location of the window. You may change the size, origin, or both
   * of the window's bounds. Do not, however, call SizeWindow or
   * SetWindowBounds yourself from inside a handler for this event.
   * 
   * In Mac OS X 10.1 and later, kEventWindowBoundsChanging is sent
   * before all changes to a window's bounds, regardless of whether the
   * change is initiated by the user or by a direct call to a Window
   * Manager API. Applications may intercept the event and modify the
   * bounds. When the event is sent because of a direct call to the
   * Window Manager, the kWindowBoundsChangeUserDrag/Resize attribute
   * bits will not be set.
   */
  kEventWindowBoundsChanging    = 26,

  /*
   * Indicates that the window has been moved or resized (or both). Do
   * not call SizeWindow or SetWindowBounds from inside a handler for
   * this event; if you need to enforce a certain window bounds,
   * install a kEventWindowBoundsChanging handler instead.
   */
  kEventWindowBoundsChanged     = 27,

  /*
   * Indicates that the user has just started to resize a window.
   */
  kEventWindowResizeStarted     = 28,

  /*
   * Indicates that the user has just finished resizing a window.
   */
  kEventWindowResizeCompleted   = 29,

  /*
   * Indicates that the user has just started to drag a window.
   */
  kEventWindowDragStarted       = 30,

  /*
   * Indicates that the user has just finished dragging a window.
   */
  kEventWindowDragCompleted     = 31,

  /*
   * Dispatched by DisposeWindow before the window is disposed.
   */
  kEventWindowClosed            = 73
};


/*
 *  Summary:
 *    Window bounds change event attributes
 *  
 *  Discussion:
 *    When the toolbox sends out a kEventWindowBoundsChanging or
 *    kEventWindowBoundsChanged event, it also sends along a parameter
 *    containing attributes of the event. These attributes can be used
 *    to determine what aspect of the window changed (origin, size, or
 *    both), and whether or not some user action is driving the change
 *    (drag or resize).
 */
enum {

  /*
   * The bounds is changing because the user is dragging the window
   * around.
   */
  kWindowBoundsChangeUserDrag   = (1 << 0),

  /*
   * The bounds is changing because the user is resizing the window.
   */
  kWindowBoundsChangeUserResize = (1 << 1),

  /*
   * The dimensions of the window (width and height) are changing.
   */
  kWindowBoundsChangeSizeChanged = (1 << 2),

  /*
   * The top left corner (origin) is changing.
   */
  kWindowBoundsChangeOriginChanged = (1 << 3),

  /*
   * The bounds is changing because ZoomWindow or ZoomWindowIdeal was
   * called. Note that this flag does not imply any user interaction;
   * if the application calls ZoomWindow itself without user request,
   * this flag will still be set. Available in Mac OS X 10.2 and
   * CarbonLib 1.6, and later.
   */
  kWindowBoundsChangeZoom       = (1 << 4)
};


/*
    Parameters for window state change events:
    
    kEventWindowShowing
        -->     kEventParamDirectObject     typeWindowRef

    kEventWindowHiding
        -->     kEventParamDirectObject     typeWindowRef
    
    kEventWindowShown
        -->     kEventParamDirectObject     typeWindowRef

    kEventWindowHidden
        -->     kEventParamDirectObject     typeWindowRef
    
    kEventWindowBoundsChanging
        -->     kEventParamDirectObject     typeWindowRef
        -->     kEventParamAttributes       typeUInt32
        -->     kEventParamOriginalBounds   typeQDRectangle
        -->     kEventParamPreviousBounds   typeQDRectangle
        <->     kEventParamCurrentBounds    typeQDRectangle
        
    kEventWindowBoundsChanged
        -->     kEventParamDirectObject     typeWindowRef
        -->     kEventParamAttributes       typeUInt32
        -->     kEventParamOriginalBounds   typeQDRectangle
        -->     kEventParamPreviousBounds   typeQDRectangle
        -->     kEventParamCurrentBounds    typeQDRectangle
        
    kEventWindowResizeStarted
        -->     kEventParamDirectObject     typeWindowRef

    kEventWindowResizeCompleted
        -->     kEventParamDirectObject     typeWindowRef
    
    kEventWindowDragStarted
        -->     kEventParamDirectObject     typeWindowRef

    kEventWindowDragCompleted
        -->     kEventParamDirectObject     typeWindowRef
*/

/*
 *  Summary:
 *    Window click events (kEventClassWindow)
 *  
 *  Discussion:
 *    Low-level events which generate higher-level "action" events.
 *    These events are only generated for windows with the standard
 *    window handler installed. Most clients should allow the standard
 *    handler to implement these events. 
 *    
 *    Window click events are generated from lower-level
 *    kEventMouseDown events by copying the mouse event and changing
 *    the class and kind, so window click events will have all of the
 *    parameters of the mouse down event, in addition to those
 *    parameters documented here that are specific to the window events.
 */
enum {

  /*
   * Sent when the mouse is down in the drag region. The standard
   * handler calls DragWindow.
   */
  kEventWindowClickDragRgn      = 32,

  /*
   * Sent when the mouse is down in the resize area. The standard
   * handler calls ResizeWindow.
   */
  kEventWindowClickResizeRgn    = 33,

  /*
   * Sent when the mouse is down in the collapse widget. The standard
   * handler calls CollapseWindow, and then generates
   * kEventWindowExpand or kEventWindowCollapse (whichever is the
   * opposite of the window's original collapse state).
   */
  kEventWindowClickCollapseRgn  = 34,

  /*
   * Sent when the mouse is down in the close widget. The standard
   * handler calls TrackGoAway, and then generates kEventWindowClose.
   */
  kEventWindowClickCloseRgn     = 35,

  /*
   * Sent when the mouse is down in the zoom widget. The standard
   * handler calls TrackBox, and then generates kEventWindowZoom.
   */
  kEventWindowClickZoomRgn      = 36,

  /*
   * Sent when the mouse is down in the content region. The standard
   * handler checks for contextual menu clicks and clicks on controls,
   * and sends kEventWindowContextualMenuSelect, kEventControlClick,
   * and kEventWindowHandleContentClick events as appropriate.
   */
  kEventWindowClickContentRgn   = 37,

  /*
   * Sent when the mouse is down in the proxy icon. The standard
   * handler handles proxy icon dragging, and generates proxy icon
   * events.
   */
  kEventWindowClickProxyIconRgn = 38,

  /*
   * Sent when the mouse is down in the toolbar button. The standard
   * handler calls TrackBox, and then generates
   * kEventWindowToolbarSwitchMode. (Mac OS X only)
   */
  kEventWindowClickToolbarButtonRgn = 41,

  /*
   * Sent when the mouse is down in some part of the window structure
   * other than the window widgets, drag region, or resize region.
   */
  kEventWindowClickStructureRgn = 42
};




/*
 *  Summary:
 *    Window cursor change events (kEventClassWindow)
 */
enum {

  /*
   * Sent when the mouse is moving over the content region. Available
   * in Mac OS X and in CarbonLib 1.5 and later. This event is used to
   * manage ownership of the cursor.  You should only change the cursor
   * if you receive this event; otherwise, someone else needed to
   * adjust the cursor and handled the event (e.g., a TSM Input Method
   * when the mouse is over an inline input region). This event is only
   * sent to the window itself; it is not propagated to controls in the
   * window.
   */
  kEventWindowCursorChange      = 40
};

/*
    Parameters for window cursor change events:
    
    kEventWindowCursorChange
        -->     kEventParamDirectObject     typeWindowRef
        -->     kEventParamMouseLocation    typeQDPoint
        -->     kEventParamKeyModifiers     typeUInt32
*/

/*
 *  Summary:
 *    Window action events
 *  
 *  Discussion:
 *    Events which indicate that certain changes have been made to the
 *    window. These events have greater semantic meaning than the
 *    low-level window click events and are usually prefered for
 *    overriding.
 */
enum {

  /*
   * If the window is not collapsed, this event is sent by the standard
   * window handler after it has received kEventWindowClickCollapseRgn
   * and received true from a call to TrackBox. The standard window
   * handler calls CollapseWindow, which sends kEventWindowCollapsing
   * and kEventWindowCollapsed.
   */
  kEventWindowCollapse          = 66,

  /*
   * Sent by the standard window handler (when the option key is down)
   * after it has received kEventWindowClickCollapseRgn and then
   * received true from a call to TrackBox.  The standard window
   * handler's response is to send each window of the same class as the
   * clicked window a kEventWindowCollapse event.
   */
  kEventWindowCollapseAll       = 68,

  /*
   * If the window is collapsed, this event is sent by the standard
   * window handler after it has received kEventWindowClickCollapseRgn
   * and received true from a call to TrackBox.  The standard window
   * handler's response is to call CollapseWindow, which sends
   * kEventWindowExpanding and kEventWindowExpanded. Note that you will
   * not receive this event before a window is expanded from the dock,
   * since minimized windows in the dock don't uses collapse boxes to
   * unminimize. However, you will still receive kEventWindowExpanding
   * and kEventWindowExpanded in that case.
   */
  kEventWindowExpand            = 69,

  /*
   * Sent by the standard window handler (when the option key is down)
   * after it has received kEventWindowClickCollapseRgn and then
   * received true from a call to TrackBox.  The standard window
   * handler's response is to send each window of the same class as the
   * clicked window a kEventWindowExpand event.
   */
  kEventWindowExpandAll         = 71,

  /*
   * Sent by the standard window handler after it has received
   * kEventWindowClickCloseRgn and successfully called TrackBox. Your
   * application might intercept this event to check if the document is
   * dirty, and display a Save/Don'tSave/Cancel alert.
   */
  kEventWindowClose             = 72,

  /*
   * Sent by the standard window handler (when the option key is down)
   * after it has received kEventWindowClickCloseRgn and received true
   * from a call to TrackGoAway.  The standard window handler's
   * response is to send each window with the same class as the clicked
   * window a kEventWindowClose event.
   */
  kEventWindowCloseAll          = 74,

  /*
   * Sent by the standard window handler upon receiving
   * kEventWindowClickZoomRgn and then receiving true from a call to
   * TrackBox.  The standard window handler's response is to zoom the
   * window using ZoomWindowIdeal. Upon successful zoom,
   * kEventWindowZoomed is sent.
   */
  kEventWindowZoom              = 75,

  /*
   * Sent by the standard window handler (when the option key is down)
   * after it has received kEventObjectClickZoomRgn and received true
   * from a call to TrackBox.  The standard window handler's response
   * is to send each window with the same class as the clicked window a
   * kEventObjectZoom event and then to reposition all zoomed windows
   * using the kWindowCascadeOnParentWindowScreen positioning method.
   */
  kEventWindowZoomAll           = 77,

  /*
   * Sent when either the right mouse button is pressed, or the control
   * key is held down and the left mouse button is pressed. The
   * standard window handler ignores this event.
   */
  kEventWindowContextualMenuSelect = 78,

  /*
   * Sent when IsWindowPathSelectClick would return true.  Set the
   * MenuRef in the event if you wish to customize the menu passed to
   * WindowPathSelect.
   */
  kEventWindowPathSelect        = 79,

  /*
   * Sent by the standard window handler to determine the ideal size of
   * the window's content region (used during window zooming).
   */
  kEventWindowGetIdealSize      = 80,

  /*
   * Sent by the standard window handler to determine the minimum size
   * of the window's content region (used during window resizing). On
   * Mac OS X 10.2 and CarbonLib 1.6 and later, this event is also sent
   * by ResizeWindow and GrowWindow if the sizeConstraints parameter is
   * NULL.
   */
  kEventWindowGetMinimumSize    = 81,

  /*
   * Sent by the standard window handler to determine the maximum size
   * of the window's content region (used during window resizing). On
   * Mac OS X 10.2 and CarbonLib 1.6 and later, this event is also sent
   * by ResizeWindow and GrowWindow if the sizeConstraints parameter is
   * NULL.
   */
  kEventWindowGetMaximumSize    = 82,

  /*
   * Sent by the Window Manager to warn of a change in the available
   * window positioning bounds on the window (i.e., screen resolution
   * or Dock size change). Available only in Mac OS X. In Mac OS
   * 10.0.x, the standard window handler always constrains the window
   * to the rect returned by GetAvailableWindowPositioningBounds for
   * the window's device. In Mac OS 10.1 and later, event handlers may
   * change the rect in the kEventParamAvailableBounds parameter, and
   * the standard window handler will constrain the window to that
   * rectangle. In Mac OS X 10.2, this event may optionally contain a
   * kEventParamAttributes parameter; if present, this parameter should
   * be formed from constants in the WindowConstrainOptions
   * enumeration. The standard window handler will pass these
   * attributes to ConstrainWindowToScreen. If this event parameter is
   * not present, the standard window handler passes
   * kWindowConstrainMoveRegardlessOfFit to ConstrainWindowToScreen in
   * 10.0.x, and kWindowConstrainMoveRegardlessOfFit |
   * kWindowConstrainAllowPartial in Mac OS 10.1 and later. In Mac OS X
   * 10.2, this event may optionally contain a
   * kEventParamWindowRegionCode parameter; if present, this parameter
   * should be a WindowRegionCode. The standard window handler will
   * pass this code to ConstrainWindowToScreen. If this event parameter
   * is not present, the standard window handler passes kWindowDragRgn
   * to ContrainWindowToScreen. In Mac OS X 10.2, this event may
   * optionally contain a kEventParamRgnHandle parameter; if present,
   * this parameter is a RgnHandle containing the GrayRgn before a
   * GDevice configuration change. An event handler may compare this
   * region with the current GrayRgn to more intelligently determine
   * whether the window should be constrained to current GrayRgn. The
   * standard window handler in Mac OS X 10.2 will not constrain
   * windows that were not onscreen before the device configuration
   * change. In Mac OS X 10.2, this event may optionally contain
   * kEventParamPreviousDockRect and kEventParamCurrentDockRect
   * parameters; if present, these parameters contain the previous and
   * current bounds of the Dock.
   */
  kEventWindowConstrain         = 83,

  /*
   * Sent by the standard window handler in response to
   * kEventWindowClickContentRgn when a mouse click is in the content
   * region but is not a contextual menu invocation or a click on a
   * control. Available starting with Mac OS X and CarbonLib 1.3.1.
   */
  kEventWindowHandleContentClick = 85,

  /*
   * A request for a menu to be displayed by a window's dock tile. The
   * basic window handler will return the menu, if any, that was
   * provided by the SetWindowDockTileMenu API. The sender of this
   * event will release the menu after the Dock has displayed it, so if
   * you return a permanently allocated MenuRef, you should call
   * RetainMenu on it before returning from your event handler. For
   * most applications, it will be easier to use the
   * SetWindowDockTileMenu API directly rather than installing a
   * handler for this event. Available in Mac OS X 10.2 and later.
   */
  kEventWindowGetDockTileMenu   = 90,

  /*
   * Sent before a proxy icon drag; you can attach data to the DragRef
   * in the event.
   */
  kEventWindowProxyBeginDrag    = 128,

  /*
   * Sent after the proxy icon drag is complete, whether successful or
   * not.
   */
  kEventWindowProxyEndDrag      = 129,

  /*
   * Sent by the standard window handler in response to
   * kEventWindowClickToolbarButtonRgn and then receiving true from a
   * call to TrackBox. You actually DO NOT need to use the standard
   * window event handler to receive this event. Any window with the
   * toolbar button will receive this event if the toolbar button is
   * clicked. The Toolbox does not track the state of the toolbar. We
   * only report that the button was clicked. The application should
   * react accordingly. Available in Mac OS X only.
   */
  kEventWindowToolbarSwitchMode = 150
};

/*
    Parameters for window action events:
    
    kEventWindowCollapse
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowCollapsed
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowCollapseAll
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowExpand
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowExpanded
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowExpandAll
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowClose
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowClosed
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowCloseAll
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowZoom
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowZoomed
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowZoomAll
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowContextualMenuSelect
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowPathSelect
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowGetIdealSize
        --> kEventParamDirectObject     typeWindowRef
        <-- kEventParamDimensions       typeQDPoint
        
    kEventWindowGetMinimumSize
        --> kEventParamDirectObject     typeWindowRef
        <-- kEventParamDimensions       typeQDPoint
        
    kEventWindowGetMaximumSize
        --> kEventParamDirectObject     typeWindowRef
        <-- kEventParamDimensions       typeQDPoint
    
    kEventWindowConstrain
        --> kEventParamDirectObject         typeWindowRef
        --> kEventParamAvailableBounds      typeQDRectangle
        --> kEventParamAttributes           typeUInt32 (optional; available in Jaguar)
        --> kEventParamWindowRegionCode     typeWindowRegionCode (optional; available in Jaguar)
        --> kEventParamRgnHandle            typeQDRgnHandle (optional; available in Jaguar)
        --> kEventParamPreviousDockRect     typeHIRect (available in Jaguar)
        --> kEventParamCurrentDockRect      typeHIRect (available in Jaguar)
    
    kEventWindowHandleContentClick
        --> kEventParamDirectObject     typeWindowRef
        --> [other parameters from kEventMouseDown]

    kEventWindowGetDockTileMenu
        --> kEventParamDirectObject     typeWindowRef
        <-- kEventParamMenuRef          typeMenuRef
        
    kEventWindowProxyBeginDrag
        --> kEventParamDirectObject     typeWindowRef
        
    kEventWindowProxyEndDrag
        --> kEventParamDirectObject     typeWindowRef
*/

/*
 *  Summary:
 *    Window focus events (kEventClassWindow)
 *  
 *  Discussion:
 *    Events related to focus changes between windows. These events are
 *    generated by SetUserFocusWindow; since that API is only called by
 *    default by the standard window handler, these events are normally
 *    only sent to windows with the standard handler installed.
 */
enum {

  /*
   * The user (or some other action) has caused the focus to shift to
   * your window. In response to this, you should focus any control
   * that might need to be focused. The standard window handler calls
   * SetKeyboardFocus to hilite the first control in the window.
   */
  kEventWindowFocusAcquired     = 200,

  /*
   * The toolbox is notifying you of a focus change. You should make
   * sure to unhilite the focus, etc. The standard window handler
   * clears the current keyboard focus.
   */
  kEventWindowFocusRelinquish   = 201,

  /*
   * If the content area of your window is not already focused, you
   * should set the focus to the main part/control/view of the window.
   * If the content area of your window is already focused, do nothing.
   * Sent on Mac OS X 10.2 and later.
   */
  kEventWindowFocusContent      = 202,

  /*
   * If your window's toolbar is not already focused, you should set
   * the focus to the first item in the toolbar. If your window's
   * toolbar is already focused, do nothing. Sent on Mac OS X 10.2 and
   * later.
   */
  kEventWindowFocusToolbar      = 203
};

/*
    Parameters for window focus events:
    
    kEventWindowFocusAcquire
        -->     kEventParamDirectObject     typeWindowRef

    kEventWindowFocusRelinquish
        -->     kEventParamDirectObject     typeWindowRef
*/

/*
 *  Summary:
 *    Carbon events related to drawers.
 */
enum {

  /*
   * Sent to the drawer and its parent window when the drawer begins
   * opening. An event handler for this event may return
   * userCanceledErr if the drawer should not open.
   */
  kEventWindowDrawerOpening     = 220,

  /*
   * Sent to the drawer and its parent window when the drawer is fully
   * open.
   */
  kEventWindowDrawerOpened      = 221,

  /*
   * Sent to the drawer and its parent window when the drawer begins
   * closing. An event handler for this event may return
   * userCanceledErr if the drawer should not close.
   */
  kEventWindowDrawerClosing     = 222,

  /*
   * Sent to the drawer and its parent window when the drawer is fully
   * closed.
   */
  kEventWindowDrawerClosed      = 223
};

/*
    Parameters for window drawer events:
    
    kEventWindowDrawerOpening
        -->     kEventParamDirectObject     typeWindowRef
    
    kEventWindowDrawerOpened
        -->     kEventParamDirectObject     typeWindowRef
    
    kEventWindowDrawerClosing
        -->     kEventParamDirectObject     typeWindowRef
    
    kEventWindowDrawerClosed
        -->     kEventParamDirectObject     typeWindowRef
*/

/*
 *  Summary:
 *    Window definition events (kEventClassWindow)
 *  
 *  Discussion:
 *    Events that correspond to WDEF messages. Sent to all windows,
 *    regardless of whether they have the standard window handler
 *    installed.
 */
enum {

  /*
   * Sent by the Window Manager when it's time to draw a window's
   * structure. This is the replacement to the old wDraw defProc
   * message (though it is a special case of the 0 part code indicating
   * to draw the entire window frame).
   */
  kEventWindowDrawFrame         = 1000,

  /*
   * Sent by the Window Manager when it's time to draw a specific part
   * of a window's structure, such as the close box. This is typically
   * sent during window tracking.
   */
  kEventWindowDrawPart          = 1001,

  /*
   * Sent by the Window Manager when it needs to get a specific region
   * from a window, or when the GetWindowRegion API is called. The
   * region you should modify is sent in the kEventParamRgnHandle
   * parameter.
   */
  kEventWindowGetRegion         = 1002,

  /*
   * Sent when the Window Manager needs to determine what part of a
   * window would be 'hit' with a given mouse location in global
   * coordinates. If you handle this event, you should set the
   * kEventParamWindowDefPart parameter to reflect the part code hit.
   */
  kEventWindowHitTest           = 1003,

  /*
   * Sent by the Window Manager when the window is being created. This
   * is a hook to allow your window definition to do any initialization
   * you might need to do. However, see the note under
   * kEventWindowDispose; this event can be sent more than once to a
   * window if the window definition function for the window is changed.
   */
  kEventWindowInit              = 1004,

  /*
   * Sent by the Window Manager to notify the window definition that it
   * should dispose of any private data structures attached to the
   * window. Note that this event does not directly correspond with
   * destruction of the window; the Window Manager occasionally needs
   * to change the window definition of a window (for example, when
   * ChangeWindowAttributes is used to change the visual appearance of
   * a window), and in that case, the window will receive a
   * kEventWindowDispose followed by a kEventWindowInit so that the old
   * and new window definitions can disconnect and connect to the
   * window. If you need to know when a window is really being
   * destroyed, install a handler for kEventWindowClosed.
   */
  kEventWindowDispose           = 1005,

  /*
   * Sent by the Window Manager when it is time to draw/erase any drag
   * hilite in the window structure. This is typically sent from within
   * HiliteWindowFrameForDrag.
   */
  kEventWindowDragHilite        = 1006,

  /*
   * Sent by the Window Manager when it is time to redraw window
   * structure to account for a change in the document modified state.
   * This is typically sent from within SetWindowModified.
   */
  kEventWindowModified          = 1007,

  /*
   * Sent by the Window Manager when it is time to generate a drag
   * image for the window proxy. This is typically sent from within
   * BeginWindowProxyDrag.
   */
  kEventWindowSetupProxyDragImage = 1008,

  /*
   * Sent by the Window Manager when a particular window state changes.
   * See the state-change flags in MacWindows.h.
   */
  kEventWindowStateChanged      = 1009,

  /*
   * Sent when the Window Manager needs to know how much space the
   * window's title area takes up.
   */
  kEventWindowMeasureTitle      = 1010,

  /*
   * This is a compatibility event harkening back to the old days
   * before Mac OS 8. Not very useful these days. When the DrawGrowIcon
   * API is called, this event is sent to the window to tell it to draw
   * the grow box. This is only really needed for windows that do not
   * have the grow box integrated into the window frame. Scroll bar
   * delimiter lines are also drawn.
   */
  kEventWindowDrawGrowBox       = 1011,

  /*
   * This is a special way for a window to override the standard resize
   * outline for windows that do not do live resizing. As the user
   * resizes the window, this event is sent with the current size the
   * user has chosen expressed as a rectangle. You should calculate
   * your window outline and modify the kEventParamRgnHandle parameter
   * to reflect your desired outline.
   */
  kEventWindowGetGrowImageRegion = 1012,

  /*
   * When the Window Manager needs to paint a window (e.g, when a
   * window is first displayed), the kEventWindowPaint event is sent to
   * allow the window to control all aspect of painting, including the
   * window frame. If a window does not respond to this event, the
   * Window Manager ends up sending kEventWindowDrawFrame and then
   * erasing the window to the window content color as usual. This is
   * mostly used for specialty windows, such as help tags or appliance
   * apps might have. (Mac OS X only)
   */
  kEventWindowPaint             = 1013
};

/*
    Parameters for window definition events:
    
    kEventWindowDrawFrame
        -->     kEventParamDirectObject     typeWindowRef               the window

    kEventWindowDrawPart
        -->     kEventParamDirectObject     typeWindowRef               the window
        -->     kEventParamWindowDefPart    typeWindowDefPartCode       the part to draw

    kEventWindowGetRegion
        -->     kEventParamDirectObject     typeWindowRef               the window
        -->     kEventParamWindowRegionCode typeWindowRegionCode        the region to get
        -->     kEventParamRgnHandle        typeQDRgnHandle             the region to munge

    kEventWindowHitTest
        -->     kEventParamDirectObject     typeWindowRef               the window
        -->     kEventParamMouseLocation    typeQDPoint                 the mouse location
        <--     kEventParamWindowDefPart    typeWindowDefPartCode       the part hit

    kEventWindowInit
        -->     kEventParamDirectObject     typeWindowRef               the window
        <--     kEventParamWindowFeatures   typeUInt32                  the window features

    kEventWindowDispose
        -->     kEventParamDirectObject     typeWindowRef               the window

    kEventWindowDragHilite
        -->     kEventParamDirectObject             typeWindowRef       the window
        -->     kEventParamWindowDragHiliteFlag     typeBoolean         whether to draw (true) or
                                                                        erase (false) the hilite

    kEventWindowModified
        -->     kEventParamDirectObject             typeWindowRef       the window
        -->     kEventParamWindowModifiedFlag       typeBoolean         the new modified state

    kEventWindowSetupProxyDragImage
        -->     kEventParamDirectObject             typeWindowRef       the window
        -->     kEventParamWindowProxyImageRgn      typeQDRgnHandle     the region you modify
                                                                        to contain the clip
                                                                        region for the GWorld.
        -->     kEventParamWindowProxyOutlineRgn    typeQDRgnHandle     the region you modify
                                                                        to contain the drag
                                                                        outline used when the
                                                                        GWorld cannot be used.
        <--     kEventParamWindowProxyGWorldPtr     typeGWorldPtr       a GWorld you allocate
                                                                        which contains the
                                                                        drag image.

    kEventWindowStateChanged
        -->     kEventParamDirectObject             typeWindowRef       the window
        -->     kEventParamWindowStateChangedFlags  typeUInt32          the state change flags

    kEventWindowMeasureTitle
        -->     kEventParamDirectObject             typeWindowRef       the window
        <--     kEventParamWindowTitleFullWidth     typeSInt16          the length of the whole title area
        <--     kEventParamWindowTitleTextWidth     typeSInt16          the length just the title text

    kEventWindowDrawGrowBox
        -->     kEventParamDirectObject     typeWindowRef               the window

    kEventWindowGetGrowImageRegion
        -->     kEventParamDirectObject     typeWindowRef               the window
        -->     kEventParamWindowGrowRect   typeQDRectangle             the global rect
        -->     kEventParamRgnHandle        typeQDRgnHandle             the region to modify

    kEventWindowPaint
        -->     kEventParamDirectObject     typeWindowRef               the window
*/
/*--------------------------------------------------------------------------------------*/
/*  Menu Events                                                                         */
/*--------------------------------------------------------------------------------------*/

/*
 *  Summary:
 *    Menu events (kEventClassMenu)
 */
enum {

  /*
   * The user has begun tracking the menubar or a pop-up menu. If
   * tracking a popup menu, the direct object parameter is the pop-up;
   * if tracking the menubar, the direct object parameter is the root
   * menu on Mac OS X, but will be NULL in CarbonLib. The
   * kEventParamCurrentMenuTrackingMode parameter indicates whether the
   * user is tracking the menus using the mouse or the keyboard. 
   *  The handler may return userCanceledErr to stop menu tracking.
   */
  kEventMenuBeginTracking       = 1,

  /*
   * The user has finished tracking the menubar or a pop-up menu.
   */
  kEventMenuEndTracking         = 2,

  /*
   * The user has switched from selecting a menu with the mouse to
   * selecting with the keyboard, or from selecting with the keyboard
   * to selecting with the mouse. Only available on post-10.0.x
   * versions of Mac OS X.
   */
  kEventMenuChangeTrackingMode  = 3,

  /*
   * A menu is opening. This event is sent each time that the menu is
   * opened (i.e., more than once during a given tracking session if
   * the user opens the menu multiple times). It is sent before the
   * menu is actually drawn, so you can update the menu contents
   * (including making changes that will alter the menu size) and the
   * new contents will be drawn correctly. 
   * 
   * The kEventParamMenuFirstOpen parameter indicates whether this is
   * the first time this menu has been opened during this menu tracking
   * session. 
   * 
   * The handler may return userCanceledErr to prevent this menu from
   * opening (Mac OS X only).
   */
  kEventMenuOpening             = 4,

  /*
   * A menu has been closed. Sent after the menu is hidden.
   */
  kEventMenuClosed              = 5,

  /*
   * The mouse is moving over a particular menu item. This event is
   * sent for both enabled and disabled items.
   */
  kEventMenuTargetItem          = 6,

  /*
   * A menu is about to be examined for items that match a command key
   * event. A handler for this event may perform its own command key
   * matching and override the Menu Manager's default matching
   * algorithms. Returning noErr from your handler indicates that you
   * have found a match. The handler for this event should not examine
   * submenus of this menu for a match; a separate event will be sent
   * for each submenu. 
   * 
   * Handlers for this event must be installed directly on the menu
   * containing the item to be matched. To improve performance of
   * command key matching, the Menu Manager only sends this event if
   * the menu itself has a handler; if a handler for this event is
   * installed on any other event target, it will not be sent the
   * event. 
   * 
   * When called from IsMenuKeyEvent, the kEventParamEventRef parameter
   * contains the EventRef that was passed to IsMenuKeyEvent, for your
   * handler to examine; when called from MenuKey or MenuEvent, the
   * EventRef parameter contains an event created from the information
   * passed to MenuKey or MenuEvent. Note that in the MenuKey case, no
   * virtual keycode (kEventParamKeyCode) or key modifiers
   * (kEventParamKeyModifiers) will be available. 
   * 
   * The kEventParamMenuEventOptions parameter contains a copy of the
   * options that were passed to IsMenuKeyEvent, or 0 if called from
   * MenuKey or MenuEvent. The only option that your handler will need
   * to obey is kMenuEventIncludeDisabledItems. 
   * 
   * If your handler finds a match, it should set the
   * kEventParamMenuItemIndex parameter to contain the item index of
   * the matching item, and return noErr. If it does not find a match,
   * it should return menuItemNotFoundErr. Any other return value will
   * cause the Menu Manager to use its default command key matching
   * algorithm for this menu. 
   * This event is sent after kEventMenuEnableItems.
   */
  kEventMenuMatchKey            = 7,

  /*
   * A request that the items in the menu be properly enabled or
   * disabled according to the current state of the application. This
   * event is sent from inside MenuKey, MenuEvent, and IsMenuKeyEvent
   * before those APIs examine the menu for an item that matches a
   * keyboard event. It is also sent during menu tracking before a menu
   * is first made visible; it is sent right after kEventMenuOpening,
   * once per menu per menu tracking session. It is also sent to the
   * top-level menus in the menubar when the application is idle after
   * user input has occurred, to allow the menu titles to be enabled or
   * disabled appropriately according to the current user focus. You
   * can distinquish between all these cases by examining the contents
   * of the kEventParamMenuContext parameter. 
   * 
   * If you install an event handler for kEventProcessCommand, you
   * should also install a handler for either kEventMenuEnableItems or
   * kEventCommandUpdateStatus. This is necessary because the Carbon
   * event system will attempt to match command keys against the
   * available menus before returning the keyboard event to your
   * application via WaitNextEvent. If you have menu command event
   * handlers installed for your menu items, your handlers will be
   * called without your ever receiving the keyboard event or calling
   * MenuKey/MenuEvent/IsMenuKeyEvent yourself. Therefore, you have no
   * opportunity to enable your menu items properly other than from a
   * kEventMenuEnableItems or kEventCommandUpdateStatus handler. 
   *  It is not necessary to handle this event if you do not install
   * kEventProcessCommand handlers for your menu items; in that case,
   * the command key event will be returned from WaitNextEvent or
   * ReceiveNextEvent as normal, and you can set up your menus before
   * calling MenuKey/MenuEvent/ IsMenuKeyEvent. 
   * 
   * The kEventParamEnableMenuForKeyEvent parameter indicates whether
   * this menu should be enabled for key event matching (true) or
   * because the menu itself is about to become visible (false). If
   * true, only the item enable state, command key, command key
   * modifiers, and (optionally) the command key glyph need to be
   * correct. If false, the entire menu item contents must be correct.
   * This may be useful if you have custom menu content that is
   * expensive to prepare.
   */
  kEventMenuEnableItems         = 8,

  /*
   * kEventMenuPopulate is intended to be handled by applications that
   * dynamically create their menu contents just-in-time before the
   * menu is displayed. Previously, kEventMenuOpening was recommended
   * for this purpose. kEventMenuPopulate is now recommended because it
   * will be sent by the Menu Manager before it searches a menu for a
   * matching command key, to allow a handler to dynamically add menu
   * items that may have command key equivalents to the menu;
   * kEventMenuOpening is only sent before the menu is displayed.
   * kEventMenuPopulate is also sent just before kEventMenuOpening as a
   * menu is being displayed. Finally, kEventMenuPopulate is only sent
   * once per menu tracking session for a given menu, even if that menu
   * is closed and opened multiple times by the user; kEventMenuOpening
   * is sent each time that the menu is displayed. 
   * 
   * You can distinguish the command-key case from the
   * menu-being-displayed case by examining the contents of the
   * kEventParamMenuContext parameter; the kMenuContextKeyMatching flag
   * will be set if the event is sent during command key matching, and
   * either the kMenuContextMenuBarTracking or
   * kMenuContextPopUpTracking flags will be sent if the event is sent
   * before actual display of the menu. 
   * 
   * In Mac OS X 10.2 and CarbonLib 1.6, kEventMenuPopulate is also
   * sent to menus before the menu is searched for a command ID by the
   * CountMenuItemsWithCommandID and GetIndMenuItemWithCommandID APIs.
   * You can distinguish this case by checking for the
   * kMenuContextCommandIDSearch flag in the kEventParamMenuContext
   * parameter. In this case, the event also includes a
   * kEventParamMenuCommand parameter with the command ID being
   * searched for as the event parameter data. 
   * 
   * kEventMenuPopulate is available on Mac OS X 10.1 and later, and in
   * CarbonLib 1.5 and later.
   */
  kEventMenuPopulate            = 9,

  /*
   * Sent by the standard menu definition when a menu item has the
   * kMenuItemAttrCustomDraw attribute. Handlers for this event should
   * be installed directly on the menu. A handler for this event may
   * respond by providing a customized width for the menu item. If no
   * handler is installed, the standard menu definition provides a
   * default handler that will return the standard width for the item.
   */
  kEventMenuMeasureItemWidth    = 100,

  /*
   * Sent by the standard menu definition when a menu item has the
   * kMenuItemAttrCustomDraw attribute. Handlers for this event should
   * be installed directly on the menu. A handler for this event may
   * respond by providing a customized height for the menu item. If no
   * handler is installed, the standard menu definition provides a
   * default handler that will return the standard height for the item.
   */
  kEventMenuMeasureItemHeight   = 101,

  /*
   * Sent by the standard menu definition when a menu item has the
   * kMenuItemAttrCustomDraw attribute. Handlers for this event should
   * be installed directly on the menu. A handler for this event may
   * respond by completely overriding the drawing of the menu item. If
   * no handler is installed, the standard menu definition provides a
   * default handler that calls DrawThemeMenuItem to draw the menu item
   * background and content.
   */
  kEventMenuDrawItem            = 102,

  /*
   * Sent by the standard menu definition when a menu item has the
   * kMenuItemAttrCustomDraw attribute. Handlers for this event should
   * be installed directly on the menu. A handler for this event may
   * respond by overriding the drawing of the menu item content: the
   * mark character, icon, text, and command key information. At the
   * time when this event is sent, the background of the menu item has
   * already been drawn using the standard system appearance, and if
   * the item is selected, the background is drawn with a hilite. If no
   * handler is installed, the standard menu definition provides a
   * default handler that draws the standard menu item content. The
   * standard handler also adds event parameters to the event
   * indicating the bounding boxes of the different portions of the
   * menu item content (mark, icon, text, and command keys), and an
   * event parameter with the baseline of the menu item text; this
   * allows handlers to use CallNextEventHandler to call through to the
   * standard system handler, and then modify the system appearance by
   * drawing on top of the standard content.
   */
  kEventMenuDrawItemContent     = 103,

  /*
   * Sent when the menu is being destroyed.
   */
  kEventMenuDispose             = 1001
};


/*
 *  Discussion:
 *    Menu context flags indicate the context or usage of a
 *    menu-related Carbon event.
 */
enum {
                                        /* menu location*/

  /*
   * Indicates that this menu is inserted in the menubar, or is a
   * (directly or indirectly) a submenu of a menu that is inserted in
   * the menubar.
   */
  kMenuContextMenuBar           = 1 << 0, /* menu type*/

  /*
   * Indicates that this menu is a pull-down menu inserted in the
   * menubar.
   */
  kMenuContextPullDown          = 1 << 8,

  /*
   * Indicates that this menu is a popup menu being displayed by
   * PopUpMenuSelect.
   */
  kMenuContextPopUp             = 1 << 9,

  /*
   * Indicates that this menu is a submenu of some other pull-down or
   * popup menu.
   */
  kMenuContextSubmenu           = 1 << 10, /* menu usage*/

  /*
   * Indicates that this Carbon event has been sent during a menubar
   * tracking session.
   */
  kMenuContextMenuBarTracking   = 1 << 16,

  /*
   * Indicates that this Carbon event has been sent during a popup menu
   * tracking session.
   */
  kMenuContextPopUpTracking     = 1 << 17,

  /*
   * Indicates that this Carbon event has been sent during command key
   * matching.
   */
  kMenuContextKeyMatching       = 1 << 18,

  /*
   * Indicates that this Carbon event has been sent at idle time to
   * update the enabled state of the menus. Available on Mac OS X 10.1
   * and later, and in CarbonLib 1.5 and later; on earlier releases,
   * the kMenuContextKeyMatching flag is set when an event is sent
   * during menu enabling.
   */
  kMenuContextMenuEnabling      = 1 << 19,

  /*
   * Indicates that this Carbon event has been sent during during a
   * search for a menu item command ID by the
   * CountMenuItemsWithCommandID or GetIndMenuItemWithCommandID APIs.
   * Available on Mac OS X 10.2 and CarbonLib 1.6.
   */
  kMenuContextCommandIDSearch   = 1 << 20
};

/*
    Parameters for menu events:
    
    kEventMenuBeginTracking
        -->     kEventParamDirectObject             typeMenuRef
        -->     kEventParamCurrentMenuTrackingMode  typeMenuTrackingMode
        -->     kEventParamMenuContext              typeUInt32 (on Mac OS X 10.1 and later, and CarbonLib 1.5 and later)
        
    kEventMenuEndTracking
        -->     kEventParamDirectObject             typeMenuRef
        -->     kEventParamMenuContext              typeUInt32 (on Mac OS X 10.1 and later, and CarbonLib 1.5 and later)
        
    kEventMenuChangeTrackingMode
        -->     kEventParamCurrentMenuTrackingMode  typeMenuTrackingMode
        -->     kEventParamNewMenuTrackingMode      typeMenuTrackingMode
        -->     kEventParamMenuContext              typeUInt32
    
    kEventMenuOpening
        -->     kEventParamDirectObject             typeMenuRef
        -->     kEventParamMenuFirstOpen            typeBoolean
        -->     kEventParamMenuContext              typeUInt32 (in CarbonLib 1.5 and later)
        
    kEventMenuClosed
        -->     kEventParamDirectObject             typeMenuRef
        -->     kEventParamMenuContext              typeUInt32 (on Mac OS X 10.1 and later, and CarbonLib 1.5 and later)
        
    kEventMenuTargetItem
        -->     kEventParamDirectObject             typeMenuRef
        -->     kEventParamMenuItemIndex            typeMenuItemIndex
        -->     kEventParamMenuCommand              typeMenuCommand
        -->     kEventParamMenuContext              typeUInt32 (on Mac OS X 10.1 and later, and CarbonLib 1.5 and later)
    
    kEventMenuMatchKey
        -->     kEventParamDirectObject             typeMenuRef
        -->     kEventParamEventRef                 typeEventRef
        -->     kEventParamMenuEventOptions         typeMenuEventOptions
        -->     kEventParamMenuContext              typeUInt32 (on Mac OS X 10.1 and later, and CarbonLib 1.5 and later)
        <--     kEventParamMenuItemIndex            typeMenuItemIndex
        
    kEventMenuEnableItems
        -->     kEventParamDirectObject             typeMenuRef
        -->     kEventParamEnableMenuForKeyEvent    typeBoolean
        -->     kEventParamMenuContext              typeUInt32 (in CarbonLib 1.3.1 and later)
        
    kEventMenuPopulate (on Mac OS X 10.1 and later, and CarbonLib 1.5 and later)
        -->     kEventParamDirectObject             typeMenuRef
        -->     kEventParamMenuContext              typeUInt32
        -->     kEventParamMenuCommand              typeMenuCommand (Jaguar and CarbonLib 1.6, for kMenuContextCommandIDSearch)
        
    kEventMenuMeasureItemWidth (on Mac OS X 10.1 and later, and CarbonLib 1.5 and later)
        -->     kEventParamDirectObject             typeMenuRef
        -->     kEventParamMenuItemIndex            typeMenuItemIndex
        <--     kEventParamMenuItemWidth            typeShortInteger
    
    kEventMenuMeasureItemHeight (on Mac OS X 10.1 and later, and CarbonLib 1.5 and later)
        -->     kEventParamDirectObject             typeMenuRef
        -->     kEventParamMenuItemIndex            typeMenuItemIndex
        <--     kEventParamMenuItemHeight           typeShortInteger

    kEventMenuDrawItem (on Mac OS X 10.1 and later, and CarbonLib 1.5 and later)
        -->     kEventParamDirectObject             typeMenuRef
        -->     kEventParamCurrentBounds            typeQDRectangle
        -->     kEventParamMenuItemIndex            typeMenuItemIndex
        -->     kEventParamMenuItemBounds           typeQDRectangle
        -->     kEventParamMenuVirtualTop           typeLongInteger
        -->     kEventParamMenuVirtualBottom        typeLongInteger
        -->     kEventParamMenuDrawState            typeThemeMenuState
        -->     kEventParamMenuItemType             typeThemeMenuItemType
        -->     kEventParamCGContextRef             typeCGContextRef
        <--     kEventParamMenuMarkBounds           typeQDRectangle (added to event by the default event handler)
        <--     kEventParamMenuIconBounds           typeQDRectangle (added to event by the default event handler)
        <--     kEventParamMenuTextBounds           typeQDRectangle (added to event by the default event handler)
        <--     kEventParamMenuTextBaseline         typeShortInteger (added to event by the default event handler)
        <--     kEventParamMenuCommandKeyBounds     typeQDRectangle (added to event by the default event handler)
        
    kEventMenuDrawItemContent (on Mac OS X 10.1 and later, and CarbonLib 1.5 and later)
        -->     kEventParamDirectObject             typeMenuRef
        -->     kEventParamMenuItemIndex            typeMenuItemIndex
        -->     kEventParamMenuItemBounds           typeQDRectangle
        -->     kEventParamDeviceDepth              typeShortInteger
        -->     kEventParamDeviceColor              typeBoolean
        -->     kEventParamCGContextRef             typeCGContextRef
        <--     kEventParamMenuMarkBounds           typeQDRectangle (added to event by the default event handler)
        <--     kEventParamMenuIconBounds           typeQDRectangle (added to event by the default event handler)
        <--     kEventParamMenuTextBounds           typeQDRectangle (added to event by the default event handler)
        <--     kEventParamMenuTextBaseline         typeShortInteger (added to event by the default event handler)
        <--     kEventParamMenuCommandKeyBounds     typeQDRectangle (added to event by the default event handler)
    
    kEventMenuDispose
        -->     kEventParamDirectObject             typeMenuRef
*/
/*--------------------------------------------------------------------------------------*/
/*  Command Events                                                                      */
/*--------------------------------------------------------------------------------------*/

/*
 *  Summary:
 *    Command events (kEventClassCommand)
 */
enum {
  kEventProcessCommand          = 1,

  /*
   * A command has been invoked and the application should handle it.
   * This event is sent when the user chooses a menu item or clicks a
   * control. Any menu item selection, from either the menubar, a popup
   * or contextual menu, or a popup or bevel button control, will cause
   * this event to be sent; if the menu item does not have a command
   * ID, the commandID field of the HICommand parameter will be zero,
   * but the event will still contain a valid MenuRef and
   * MenuItemIndex. Controls will send this event only if the control
   * has a non-zero command ID set with SetControlCommandID. 
   * 
   * Some senders of this event will also include the modifier keys
   * that were pressed by the user when the command was invoked, but
   * this parameter is optional. 
   * 
   * When a command event is sent from a menu, a MenuContext parameter
   * will be included on Mac OS X 10.2 and CarbonLib 1.6, indicating
   * whether the command was sent from a menu in the menubar
   * (kMenuContextMenuBar will be set) or from a popup menu
   * (kMenuContextMenuBar will not be set). The MenuContext parameter
   * also indicates whether the event was sent by a selection of a menu
   * item with the mouse (kMenuContextMenuBarTracking or
   * kMenuContextPopUpTracking will be set), or by a command key press
   * (kMenuContextKeyMatching will be set). 
   * 
   * It is essential that your event handler for this event return
   * eventNotHandledErr for any command events that you do not handle,
   * especially for commands that are sent from menus attached to popup
   * or bevel button controls; if, for example, you return noErr for
   * command events sent in response to a menu selection from a popup
   * button, the Menu Manager will return zero from PopUpMenuSelect,
   * and the popup button control will not know that an item was
   * selected from the popup menu and will not redraw with the new
   * selection.
   */
  kEventCommandProcess          = 1,

  /*
   * The status of a command is in question. When you receive this
   * event, you should update the necessary UI elements in your
   * application to reflect the current status of the command. For
   * example, if the command has the kHICommandFromMenu bit set, you
   * should update the menu item state, text, etc. to reflect the
   * current reality in your application.
   */
  kEventCommandUpdateStatus     = 2
};

/*
    Parameters for command events:

    kEventCommandProcess
        -->     kEventParamDirectObject     typeHICommand
        -->     kEventParamKeyModifiers     typeUInt32 (optional)
        -->     kEventParamMenuContext      typeUInt32 (optional, in Jaguar and CarbonLib 1.6)

    kEventCommandUpdateStatus
        -->     kEventParamDirectObject     typeHICommand
        -->     kEventParamMenuContext      typeUInt32
*/
/* HI Commands */

/*
 *  Summary:
 *    Common command IDs
 */
enum {

  /*
   * The OK button in a dialog or alert.
   */
  kHICommandOK                  = FOUR_CHAR_CODE('ok  '),

  /*
   * The Cancel button in a dialog or alert.
   */
  kHICommandCancel              = FOUR_CHAR_CODE('not!'),

  /*
   * The application should quit.
   */
  kHICommandQuit                = FOUR_CHAR_CODE('quit'),

  /*
   * The last editing operation should be undone.
   */
  kHICommandUndo                = FOUR_CHAR_CODE('undo'),

  /*
   * The last editing operation should be redone.
   */
  kHICommandRedo                = FOUR_CHAR_CODE('redo'),

  /*
   * The selected items should be cut.
   */
  kHICommandCut                 = FOUR_CHAR_CODE('cut '),

  /*
   * The selected items should be copied.
   */
  kHICommandCopy                = FOUR_CHAR_CODE('copy'),

  /*
   * The contents of the clipboard should be pasted.
   */
  kHICommandPaste               = FOUR_CHAR_CODE('past'),

  /*
   * The selected items should be deleted.
   */
  kHICommandClear               = FOUR_CHAR_CODE('clea'),

  /*
   * All items in the active window should be selected.
   */
  kHICommandSelectAll           = FOUR_CHAR_CODE('sall'),

  /*
   * The application should be hidden. The Menu Manager will respond to
   * this command automatically; your application does not need to
   * handle it.
   */
  kHICommandHide                = FOUR_CHAR_CODE('hide'),

  /*
   * Other applications should be hidden. The Menu Manager will respond
   * to this command automatically; your application does not need to
   * handle it.
   */
  kHICommandHideOthers          = FOUR_CHAR_CODE('hido'),

  /*
   * All applications should become visible. The Menu Manager will
   * respond to this command automatically; your application does not
   * need to handle it.
   */
  kHICommandShowAll             = FOUR_CHAR_CODE('shal'),

  /*
   * The Preferences menu item has been selected.
   */
  kHICommandPreferences         = FOUR_CHAR_CODE('pref'),

  /*
   * The active window should be zoomed in or out. The Window Manager
   * will respond to this event automatically; your application does
   * not need to handle it, but you may want to install a Carbon event
   * handler for kEventWindowGetIdealSize to return the ideal size for
   * your document windows.
   */
  kHICommandZoomWindow          = FOUR_CHAR_CODE('zoom'),

  /*
   * The active window should be minimized. The Window Manager will
   * respond to this event automatically; your application does not
   * need to handle it.
   */
  kHICommandMinimizeWindow      = FOUR_CHAR_CODE('mini'),

  /*
   * All windows of the same class as the active window that have
   * collapse boxes should be minimized. The Window Manager will
   * respond to this event automatically; your application does not
   * need to handle it.
   */
  kHICommandMinimizeAll         = FOUR_CHAR_CODE('mina'),

  /*
   * The active window should be maximized. Only sent on Mac OS 9. The
   * Window Manager will respond to this event automatically; your
   * application does not need to handle it.
   */
  kHICommandMaximizeWindow      = FOUR_CHAR_CODE('maxi'),

  /*
   * All windows of the same class as the active window that have
   * collapse boxes should be maximized. The Window Manager will
   * respond to this event automatically; your application does not
   * need to handle it.
   */
  kHICommandMaximizeAll         = FOUR_CHAR_CODE('maxa'),

  /*
   * All document-class windows should be arranged in a stack. The
   * Window Manager will respond to this event automatically; your
   * application does not need to handle it.
   */
  kHICommandArrangeInFront      = FOUR_CHAR_CODE('frnt'),

  /*
   * All windows of this application should be brought in front of
   * windows from other applications. Only sent on Mac OS X. The Window
   * Manager will respond to this event automatically; your application
   * does not need to handle it.
   */
  kHICommandBringAllToFront     = FOUR_CHAR_CODE('bfrt'),

  /*
   * This command ID is used as a placeholder to mark the separator
   * item dividing the Zoom/Minimize/Maximize/Arrange menu items in the
   * standard Window menu from the menu items listing the visible
   * windows. If you need to add your own menu items to the standard
   * Window menu before the window list section, you can look for the
   * menu item with this command ID using GetIndMenuItemWithCommandID,
   * and insert your menu items before the item with this ID.
   */
  kHICommandWindowListSeparator = FOUR_CHAR_CODE('wldv'),

  /*
   * This command ID is used as a placeholder to mark the end of the
   * window list section of the standard Window menu. If you need to
   * add your own menu items to the standard Window menu after the
   * window list section, you can look for the menu item with this
   * command ID using GetIndMenuItemWithCommandID, and insert your
   * items after the item with this ID.
   */
  kHICommandWindowListTerminator = FOUR_CHAR_CODE('wlst'),

  /*
   * A window in the standard Window menu has been selected and should
   * be activated. The Window Manager will respond to this event
   * automatically; your application does not need to handle it.
   */
  kHICommandSelectWindow        = FOUR_CHAR_CODE('swin'),

  /*
   * The Rotate Windows hotkey (cmd-~ by default) has been pressed, and
   * non-floating windows should be rotated so that the window after
   * the active window is activated. The Window Manager will respond to
   * this event automatically; your application does not need to handle
   * it.
   */
  kHICommandRotateWindowsForward = FOUR_CHAR_CODE('rotw'),

  /*
   * The Rotate Windows hotkey (cmd-~ by default) has been pressed, and
   * non-floating windows should be rotated so that the window before
   * the active window is activated. The Window Manager will respond to
   * this event automatically; your application does not need to handle
   * it.
   */
  kHICommandRotateWindowsBackward = FOUR_CHAR_CODE('rotb'),

  /*
   * The floating window focus hotkey (ctl-F6 by default) has been
   * pressed, and floating windows should be rotated so that the window
   * after the focused window is activated. The Window Manager will
   * respond to this event automatically; your application does not
   * need to handle it.
   */
  kHICommandRotateFloatingWindowsForward = FOUR_CHAR_CODE('rtfw'),

  /*
   * The floating window focus hotkey (ctl-F6 by default) has been
   * pressed, and floating windows should be rotated so that the window
   * before the focused window is activated. The Window Manager will
   * respond to this event automatically; your application does not
   * need to handle it.
   */
  kHICommandRotateFloatingWindowsBackward = FOUR_CHAR_CODE('rtfb'),

  /*
   * The About menu item has been selected.
   */
  kHICommandAbout               = FOUR_CHAR_CODE('abou'),

  /*
   * A new document or item should be created.
   */
  kHICommandNew                 = FOUR_CHAR_CODE('new '),

  /*
   * The user wants to open an existing document.
   */
  kHICommandOpen                = FOUR_CHAR_CODE('open'),

  /*
   * The active window should be closed.
   */
  kHICommandClose               = FOUR_CHAR_CODE('clos'),

  /*
   * The active document should be saved.
   */
  kHICommandSave                = FOUR_CHAR_CODE('save'),

  /*
   * The user wants to save the active document under a new name.
   */
  kHICommandSaveAs              = FOUR_CHAR_CODE('svas'),

  /*
   * The contents of the active document should be reverted to the last
   * saved version.
   */
  kHICommandRevert              = FOUR_CHAR_CODE('rvrt'),

  /*
   * The active window should be printed.
   */
  kHICommandPrint               = FOUR_CHAR_CODE('prnt'),

  /*
   * The user wants to configure the current page margins, formatting,
   * and print options.
   */
  kHICommandPageSetup           = FOUR_CHAR_CODE('page'),

  /*
   * The application's help book should be displayed. Used by the Help
   * Manager when it adds the "<AppName> Help" menu item to the Help
   * menu. The Help Manager will respond to this event automatically;
   * your application does not need to handle it.
   */
  kHICommandAppHelp             = FOUR_CHAR_CODE('ahlp')
};


/*
 *  Summary:
 *    Values for the attributes field of the HICommand and
 *    HICommandExtended structures.
 *  
 *  Discussion:
 *    These bit masks are mutually exclusive; only one should be set at
 *    any given time. Some HICommand and HICommandExtended structures
 *    will have an attributes value of zero; in this case, there is no
 *    information available about the source of the command.
 */
enum {

  /*
   * Indicates that the command originates from a menu item. The
   * HICommand.menu and HICommandExtended.source.menu fields are valid.
   */
  kHICommandFromMenu            = (1L << 0),

  /*
   * Indicates that the command originates from a control. The
   * HICommandExtended.source.control field is valid.
   */
  kHICommandFromControl         = (1L << 1),

  /*
   * Indicates that the command originates from a window. The
   * HICommandExtended.source.window field is valid.
   */
  kHICommandFromWindow          = (1L << 2)
};

struct HICommand {
  UInt32              attributes;
  UInt32              commandID;
  struct {
    MenuRef             menuRef;
    MenuItemIndex       menuItemIndex;
  }                       menu;
};
typedef struct HICommand                HICommand;
struct HICommandExtended {
  UInt32              attributes;
  UInt32              commandID;
  union {
    ControlRef          control;
    WindowRef           window;
    struct {
      MenuRef             menuRef;
      MenuItemIndex       menuItemIndex;
    }                       menu;
  }                       source;
};
typedef struct HICommandExtended        HICommandExtended;
/*--------------------------------------------------------------------------------------*/
/*  Control Events                                                                      */
/*--------------------------------------------------------------------------------------*/

/*
 *  Summary:
 *    Control events (kEventClassControl)
 */
enum {

  /*
   * Allows the control to initialize private data.
   */
  kEventControlInitialize       = 1000,

  /*
   * Allows the control to dispose of private data.
   */
  kEventControlDispose          = 1001,

  /*
   * Allows the control to report its best size and its text baseline
   * based on its current settings. You should set the
   * kEventParamControlOptimalBounds parameter to an appropriate
   * rectangle. You should also set the
   * kEventParamControlOptimalBaselineOffset parameter to be the offset
   * from the top of your optimal bounds of a text baseline, if any.
   * (Mac OS X only)
   */
  kEventControlGetOptimalBounds = 1003,
  kEventControlDefInitialize    = kEventControlInitialize,
  kEventControlDefDispose       = kEventControlDispose,

  /*
   * Sent by TrackControl and HandleControlClick after handling a click
   * in a control.
   */
  kEventControlHit              = 1,

  /*
   * Sent when your control should simulate a click in response to some
   * other action, such as a return key for a default button. The event
   * contains the part code the control should simulate a click for.
   * The control may modify this on output to reflect the same sort of
   * part code it would pass back while handling kEventControlTrack.
   * (Mac OS X only)
   */
  kEventControlSimulateHit      = 2,

  /*
   * Sent when someone wants to find out what part of your control is
   * at a given point in local coordinates. You should set the
   * kEventParamControlPart parameter to the appropriate part. (Mac OS
   * X only)
   */
  kEventControlHitTest          = 3,

  /*
   * Sent when your control should draw itself. The event can
   * optionally contain a port in which to draw and a part to constrain
   * drawing to. (Mac OS X only)
   */
  kEventControlDraw             = 4,

  /*
   * Sent when your control should apply its background color/pattern
   * to the port specified so the subcontrol can properly erase. The
   * port is optional; if it does not exist you should apply the
   * background to the current port. (Mac OS X only)
   */
  kEventControlApplyBackground  = 5,

  /*
   * Sent when your control should apply a color/pattern to the
   * specified port and context so a subcontrol can draw text which
   * looks appropriate for your control's background. The port is
   * optional; if it does not exist, you should apply the text color to
   * the current port. The context context is also optional. (Mac OS X
   * only)
   */
  kEventControlApplyTextColor   = 6,

  /*
   * Sent when your control is gaining, losing, or changing the focus.
   * Set the kEventParamControlPart param to the resulting focused
   * part. If the kEventParamControlFocusEverything param is present,
   * use its value as an indication of whether to advance the focus
   * onto a part that isn't one that accepts general keyboard input;
   * regardless of the param's value, the control must always allow the
   * focus to be removed. (Mac OS X only)
   */
  kEventControlSetFocusPart     = 7,

  /*
   * Sent when your the Control Manager wants to know what part of your
   * control is currently focused. Set the kEventParamControlPart param
   * to your currently focused part. (Mac OS X only)
   */
  kEventControlGetFocusPart     = 8,

  /*
   * Sent when your control becomes active as a result of a call to
   * ActivateControl. (Mac OS X only)
   */
  kEventControlActivate         = 9,

  /*
   * Sent when your control becomes inactive as a result of a call to
   * DeactivateControl. (Mac OS X only)
   */
  kEventControlDeactivate       = 10,

  /*
   * Sent when your control is asked to change the cursor as a result
   * of a call to HandleControlSetCursor. (Mac OS X only)
   */
  kEventControlSetCursor        = 11,

  /*
   * Sent when your control is asked to display a contextual menu as a
   * result of a call to HandleControlContextualMenuClick. (Mac OS X
   * only)
   */
  kEventControlContextualMenuClick = 12,

  /*
   * A mouse down occurred in a control. The standard window handler
   * sets the keyboard focus to the control if it takes focus on
   * clicks, and calls HandleControlClick. Available on Mac OS X and
   * CarbonLib 1.3.1 and later.
   */
  kEventControlClick            = 13,

  /*
   * Sent so that a given control can customize the focus order of its
   * subcontrols. The recipient will receive a start subcontrol in the
   * kEventParamStartControl parameter and a focusing direction in the
   * kEventParamControlPart parameter. The part code stored in the
   * kEventParamControlPart parameter will be one of the focusing meta
   * part codes, such as kControlFocusNextPart and
   * kControlFocusPrevPart. The kEventParamStartControl parameter may
   * be non-existent or NULL; this is a request to find the *first*
   * subcontrol in the appropriate focusing direction. The recipient
   * must store the next subcontrol in the kEventParamNextControl
   * parameter of the event before returning; if there is no next
   * subcontrol in the given focusing direction, the recipient must
   * either exclude the kEventParamNextControl parameter or set it to
   * NULL. This event is sent in Mac OS X 10.2 or later.
   */
  kEventControlGetNextFocusCandidate = 14, /* Available in Jaguar or later*/

  /*
   * This event is send when we are going to autotoggle a control. You
   * can specify the value use based on the current value of your
   * control. This event is available in Mac OS X 10.2 or later.
   */
  kEventControlGetAutoToggleValue = 15, /* Available in Jaguar or later */

  /*
   * This is sent when the HIViewGetViewForMouseEvent API is called.
   * The control manager calls it before decending into any subviews.
   * Controls can override this to intercept clicks which would
   * normally be destined for their children. For example, the Toolbar
   * control intercepts cmd-clicks to handle dragging its children. To
   * accomplish this, it overrides this event, looking for the cmd key.
   * When it's down, it just returns noErr as the result from its event
   * handler. This tells the control manager to stop decending and
   * return the view that it called as the event target. The standard
   * window handler calls GetViewForMouseEvent and then HIViewClick
   * with the resultant view. This event is available in Mac OS X 10.2
   * or later.
   */
  kEventControlInterceptSubviewClick = 16, /* Available in Jaguar or later*/

  /*
   * This is very much like the window class version of this event. The
   * difference is that the mouse location is view-relative, and there
   * is no window part code passed to you. This event is actually a
   * copy of the mouse down event, and so it has all the parameters
   * that such an event would have (modifiers, button number, etc.)
   */
  kEventControlGetClickActivation = 17, /* Available in Jaguar or later*/

  /*
   * A drag has entered your control. If you at all wish to handle it,
   * you must return noErr from your event handler in response to this
   * event. If you return eventNotHandledErr, you will not receive
   * 'within' or 'leave' messages, nor will you be eligible to receive
   * the drag. This is done to try to be as efficient as possible wrt
   * sending events during the drag operation.
   */
  kEventControlDragEnter        = 18,   /* Available in Jaguar or later*/

  /*
   * A drag has moved with your airspace (but not on any subcontrol).
   * If the user moves into a subcontrol, it becomes the target of the
   * drag and your control will not receive 'within' messages any
   * longer.
   */
  kEventControlDragWithin       = 19,   /* Available in Jaguar or later*/

  /*
   * A drag is leaving your view. You might want to do things like
   * unhighlight yourself, etc.
   */
  kEventControlDragLeave        = 20,   /* Available in Jaguar or later*/

  /*
   * Congratulations, you are the lucky recipient of a drag. Enjoy it.
   */
  kEventControlDragReceive      = 21,   /* Available in Jaguar or later*/

  /*
   * Sent to allow your control to completely replace the normal
   * tracking that is part of a call to TrackControl or
   * HandleControlClick. Set the kEventParamControlPart to the part hit
   * during tracking. (Mac OS X only)
   */
  kEventControlTrack            = 51,

  /*
   * Sent so your control can support Scroll To Here behavior during
   * tracking. Set the kEventParamMouseLocation parameter to the mouse
   * location in local coordinates which represents where a click would
   * have needed to be to cause your indicator to be dragged to the
   * incoming mouse location. (Mac OS X only)
   */
  kEventControlGetScrollToHereStartPoint = 52,

  /*
   * Sent so your control can constrain the movement of its indicator
   * during tracking. Set the kEventParamControlIndicatorDragConstraint
   * parameter to the appropriate constraint. (Mac OS X only)
   */
  kEventControlGetIndicatorDragConstraint = 53,

  /*
   * Sent during live-tracking of the indicator so your control can
   * update its value based on the new indicator position. During
   * non-live tracking, this event lets you redraw the indicator ghost
   * at the appropriate place. (Mac OS X only)
   */
  kEventControlIndicatorMoved   = 54,

  /*
   * Sent at the end of non-live indicator tracking so your control can
   * update its value based on the final ghost location. (Mac OS X only)
   */
  kEventControlGhostingFinished = 55,

  /*
   * Sent during tracking so your control can alter the part that is
   * passed to its action proc based on modifier keys, etc. Set the
   * kEventParamControlPart to the part you want to have sent. (Mac OS
   * X only)
   */
  kEventControlGetActionProcPart = 56,

  /*
   * Sent when a client wants to get a particular region of your
   * control. See the GetControlRegion API. The
   * kEventParamControlRegion contains a region for you to modify. (Mac
   * OS X only)
   */
  kEventControlGetPartRegion    = 101,

  /*
   * Sent when a client wants to get a particular rectangle of your
   * control when it may be more efficient than asking for a region.
   * Set the kEventParamControlPartBounds parameter to the appropriate
   * rectangle. (Mac OS X only)
   */
  kEventControlGetPartBounds    = 102,

  /*
   * Sent when a client wants to change an arbitrary setting of your
   * control. See the SetControlData API. (Mac OS X only)
   */
  kEventControlSetData          = 103,

  /*
   * Sent when a client wants to get an arbitrary setting of your
   * control. See the GetControlData API. (Mac OS X only)
   */
  kEventControlGetData          = 104,

  /*
   * This event is sent when the HIViewGetSizeConstraints API is
   * called. It allows your custom control/view to specify its minimum
   * and maximum size. This is different from the optimal size event
   * above. The optimal size might be larger than a view's minimum
   * size. A parent view can use this information to help it lay out
   * subviews. The toolbar control uses this information to help lay
   * out toolbar items, for example.
   */
  kEventControlGetSizeConstraints = 105, /* Available in Jaguar or later*/

  /*
   * Sent when your control's value, min, max, or view size has
   * changed. Useful so other entities can watch for your control's
   * value to change. (Mac OS X only)
   */
  kEventControlValueFieldChanged = 151,

  /*
   * Sent when a control was embedded within your control. (Mac OS X
   * only)
   */
  kEventControlAddedSubControl  = 152,

  /*
   * Sent when one of your child controls will be removed from your
   * control. (Mac OS X only)
   */
  kEventControlRemovingSubControl = 153,

  /*
   * Sent when one your control's bounding rectangle has changed. (Mac
   * OS X only)
   */
  kEventControlBoundsChanged    = 154,

  /*
   * Sent when the title of your control changes. Available in Mac OS X
   * 10.2 or later.
   */
  kEventControlTitleChanged     = 158,  /* Available in Jaguar or later*/

  /*
   * Sent when one your control's owning window has changed.  Useful to
   * udpate any dependencies that your control has on its owning 
   * window. (Mac OS X only)
   */
  kEventControlOwningWindowChanged = 159,

  /*
   * Sent when the hilite state changes in a control. This is here to
   * support custom views/controls which need to update information
   * when the hilite state changes. This event is available in Mac OS X
   * 10.2 or later.
   */
  kEventControlHiliteChanged    = 160,  /* Available in Jaguar or later*/

  /*
   * Sent when the enabled state changes in a control. This is here to
   * support custom views/controls which need to update information
   * when the enabled state changes. This event is available in Mac OS
   * X 10.2 or later.
   */
  kEventControlEnabledStateChanged = 161, /* Available in Jaguar or later*/

  /*
   * Sent when someone is trying to send an old-style CDEF message to
   * your control. (Mac OS X only)
   */
  kEventControlArbitraryMessage = 201
};


/*
 *  Summary:
 *    Control bounds change event attributes
 *  
 *  Discussion:
 *    When the toolbox sends out a kEventControlBoundsChanged event, it
 *    also sends along a parameter containing attributes of the event.
 *    These attributes can be used to determine what aspect of the
 *    control changed (position, size, or both).
 */
enum {

  /*
   * The dimensions of the control (width and height) changed.
   */
  kControlBoundsChangeSizeChanged = (1 << 2),

  /*
   * The top left corner (position) changed.
   */
  kControlBoundsChangePositionChanged = (1 << 3)
};

/*
    Parameters for control events:

    kEventControlInitialize
        -->     kEventParamDirectObject     typeControlRef
        -->     kEventParamInitCollection   typeCollection
        <--     kEventParamControlFeatures  typeUInt32
        
    kEventControlDispose
        -->     kEventParamDirectObject     typeControlRef

    kEventControlGetOptimalBounds
        Required parameters:
        -->     kEventParamDirectObject                     typeControlRef
        <--     kEventParamControlOptimalBounds             typeQDRectangle
        
        Optional parameters:
        <--     kEventParamControlOptimalBaselineOffset     typeShortInteger (optional)
        
    kEventControlHit
        -->     kEventParamDirectObject     typeControlRef
        -->     kEventParamControlPart      typeControlPartCode
        -->     kEventParamKeyModifiers     typeUInt32

    kEventControlSimulateHit
        -->     kEventParamDirectObject     typeControlRef
        <->     kEventParamKeyModifiers     typeUInt32
        <->     kEventParamControlPart      typeControlPartCode
        
    kEventControlHitTest
        -->     kEventParamDirectObject     typeControlRef
        -->     kEventParamMouseLocation    typeQDPoint
        <--     kEventParamControlPart      typeControlPartCode
        
    kEventControlDraw
        Required parameters:
        -->     kEventParamDirectObject     typeControlRef
        
        Optional parameters:
        -->     kEventParamControlPart      typeControlPartCode
                    (draw the entire control if kEventParamControlPart is not present)
        -->     kEventParamGrafPort         typeGrafPtr
                    (draw into the current port if kEventParamGrafPort is not present)
        -->     kEventParamRgnHandle        typeQDRgnHandle
                    Sent only on Jaguar and in certain situations in compositing mode
                    (when view is visible, etc.) Allows you to constrain your drawing
                    to the region the Toolbox has clipped you to.
        -->     kEventParamCGContextRef     typeCGContextRef
                    Sent to your control when in compositing mode. It is already
                    transformed and clipped appropriately. You should use this instead
                    of creating your own CGContext. This will be extremely important
                    for printing, etc.

    kEventControlApplyBackground
        Required parameters:
        -->     kEventParamDirectObject         typeControlRef
        -->     kEventParamControlSubControl    typeControlRef
        -->     kEventParamControlDrawDepth     typeShortInteger
        -->     kEventParamControlDrawInColor   typeBoolean
        
        Optional parameters:
        -->     kEventParamGrafPort             typeGrafPtr
                    (apply to the current port if kEventParamGrafPort is not present)

    kEventControlApplyTextColor
        Required parameters:
        -->     kEventParamDirectObject         typeControlRef
        -->     kEventParamControlSubControl    typeControlRef
        -->     kEventParamControlDrawDepth     typeShortInteger
        -->     kEventParamControlDrawInColor   typeBoolean
        
        Optional parameters:
        -->     kEventParamCGContextRef         typeCGContextRef
        -->     kEventParamGrafPort             typeGrafPtr
                    (apply to the current port if kEventParamGrafPort is not present)

    kEventControlGetNextFocusCandidate
        Required parameters:
        -->     kEventParamControlPart          typeControlPartCode
        <--     kEventParamNextControl          typeControlRef
        
        Optional parameters:
        -->     kEventParamStartControl         typeControlRef
                    (find the first/last subcontrol of yourself if kEventParamStartControl is not present)
        
    kEventControlGetAutoToggleValue:
        -->     kEventParamDirectObject         typeControlRef
        -->     kEventParamControlPart          typeControlPartCode
        <--     kEventParamControlValue         typeLongInteger

    kEventControlInterceptSubviewClick
        -->     kEventParamEventRef             typeEventRef

    kEventControlGetClickActivation
        This event is a copy of the actual mouse down event. The only
        difference is the mouse location parameter is in view coordinates,
        and there is a direct object parameter which is the control ref.
        For mouse down event parameters, see the kEventMouseDown comments.
        
        <--     kEventParamClickActivation      typeClickActivationResult

    kEventControlDragEnter
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamDragRef                  typeDragRef
    
    kEventControlDragWithin
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamDragRef                  typeDragRef

    kEventControlDragLeave
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamDragRef                  typeDragRef

    kEventControlDragReceive
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamDragRef                  typeDragRef

    kEventControlSetFocusPart
        Required parameters:
        -->     kEventParamDirectObject             typeControlRef
        <->     kEventParamControlPart              typeControlPartCode
        
        Optional parameters:
        -->     kEventParamControlFocusEverything   typeBoolean

    kEventControlGetFocusPart
        -->     kEventParamDirectObject     typeControlRef
        <--     kEventParamControlPart      typeControlPartCode

    kEventControlActivate
        -->     kEventParamDirectObject     typeControlRef

    kEventControlDeactivate
        -->     kEventParamDirectObject     typeControlRef

    kEventControlSetCursor
        -->     kEventParamDirectObject     typeControlRef
        -->     kEventParamMouseLocation    typeQDPoint
        -->     kEventParamKeyModifiers     typeUInt32

    kEventControlContextualMenuClick
        -->     kEventParamDirectObject     typeControlRef
        -->     kEventParamMouseLocation    typeQDPoint

    kEventControlClick
        -->     kEventParamDirectObject     typeControlRef
        -->     [other parameters from kEventMouseDown]

    kEventControlTrack
        -->     kEventParamDirectObject     typeControlRef
        -->     kEventParamMouseLocation    typeQDPoint
        <->     kEventParamKeyModifiers     typeUInt32
        -->     kEventParamControlAction    typeControlActionUPP
        <--     kEventParamControlPart      typeControlPartCode

    kEventControlGetScrollToHereStartPoint
        -->     kEventParamDirectObject     typeControlRef
        <->     kEventParamMouseLocation    typeQDPoint
        -->     kEventParamKeyModifiers     typeUInt32

    kEventControlGetIndicatorDragConstraint
        -->     kEventParamDirectObject                     typeControlRef
        -->     kEventParamMouseLocation                    typeQDPoint
        -->     kEventParamKeyModifiers                     typeUInt32
        <--     kEventParamControlIndicatorDragConstraint   typeIndicatorDragConstraint

    kEventControlIndicatorMoved
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamControlIndicatorRegion   typeQDRgnHandle
        -->     kEventParamControlIsGhosting        typeBoolean

    kEventControlGhostingFinished
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamControlIndicatorOffset   typeQDPoint

    kEventControlGetActionProcPart
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamKeyModifiers             typeUInt32
        <->     kEventParamControlPart              typeControlPartCode

    kEventControlGetPartRegion
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamControlPart              typeControlPartCode
        -->     kEventParamControlRegion            typeQDRgnHandle

    kEventControlGetPartBounds
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamControlPart              typeControlPartCode
        <--     kEventParamControlPartBounds        typeQDRectangle

    kEventControlSetData
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamControlPart              typeControlPartCode
        -->     kEventParamControlDataTag           typeEnumeration
        -->     kEventParamControlDataBuffer        typePtr
        -->     kEventParamControlDataBufferSize    typeLongInteger

    kEventControlGetData
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamControlPart              typeControlPartCode
        -->     kEventParamControlDataTag           typeEnumeration
        -->     kEventParamControlDataBuffer        typePtr
        <->     kEventParamControlDataBufferSize    typeLongInteger

    kEventControlGetSizeConstraints
        -->     kEventParamDirectObject         typeControlRef
        <--     kEventParamMinimumSize          typeHISize
        <--     kEventParamMaximumSize          typeHISize

    kEventControlValueFieldChanged
        -->     kEventParamDirectObject             typeControlRef

    kEventControlAddedSubControl
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamControlSubControl        typeControlRef

    kEventControlRemovingSubControl
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamControlSubControl        typeControlRef

    kEventControlBoundsChanged
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamAttributes               typeUInt32
        -->     kEventParamOriginalBounds           typeQDRectangle
        -->     kEventParamPreviousBounds           typeQDRectangle
        -->     kEventParamCurrentBounds            typeQDRectangle

    kEventControlTitleChanged
        -->     kEventParamDirectObject                 typeControlRef

    kEventControlOwningWindowChanged
        -->     kEventParamDirectObject                 typeControlRef
        -->     kEventParamAttributes                   typeUInt32
        -->     kEventParamControlOriginalOwningWindow  typeWindowRef
        -->     kEventParamControlCurrentOwningWindow   typeWindowRef

    kEventControlHiliteStateChanged
        -->     kEventParamDirectObject                 typeControlRef
        -->     kEventParamControlPreviousPart          typeControlPartCode
        -->     kEventParamControlCurrentPart           typeControlPartCode

    kEventControlEnabledStateChanged
        -->     kEventParamDirectObject                 typeControlRef

    kEventControlArbitraryMessage
        -->     kEventParamDirectObject             typeControlRef
        -->     kEventParamControlMessage           typeShortInteger
        -->     kEventParamControlParam             typeLongInteger
        <--     kEventParamControlResult            typeLongInteger
*/
/*--------------------------------------------------------------------------------------*/
/*  Tablet Events (and tablet data in mouse events)                                     */
/*--------------------------------------------------------------------------------------*/

/*
 *  Summary:
 *    Tablet events (kEventClassTablet)
 */
enum {

  /*
   * Indicates that the pen has moved on or near a tablet. Same as
   * deprecated kEventTabletPointer. Available in Mac OS X only.
   */
  kEventTabletPoint             = 1,

  /*
   * Indicates that the pen has entered or exited proximity of a
   * tablet. Available in Mac OS X only.
   */
  kEventTabletProximity         = 2,
  kEventTabletPointer           = 1     /* deprecated form for compatibility only, use kEventTabletPoint*/
};

struct TabletPointRec {
  SInt32              absX;                   /* absolute x coordinate in tablet space at full tablet resolution */
  SInt32              absY;                   /* absolute y coordinate in tablet space at full tablet resolution */
  SInt32              absZ;                   /* absolute z coordinate in tablet space at full tablet resolution */
  UInt16              buttons;                /* one bit per button - bit 0 is first button - 1 = closed */
  UInt16              pressure;               /* scaled pressure value; MAXPRESSURE=(2^16)-1, MINPRESSURE=0 */
  SInt16              tiltX;                  /* scaled tilt x value; range is -((2^15)-1) to (2^15)-1 (-32767 to 32767) */
  SInt16              tiltY;                  /* scaled tilt y value; range is -((2^15)-1) to (2^15)-1 (-32767 to 32767) */
  UInt16              rotation;               /* Fixed-point representation of device rotation in a 10.6 format */
  SInt16              tangentialPressure;     /* tangential pressure on the device; range same as tilt */
  UInt16              deviceID;               /* system-assigned unique device ID - matches to deviceID field in proximity event */
  SInt16              vendor1;                /* vendor-defined signed 16-bit integer */
  SInt16              vendor2;                /* vendor-defined signed 16-bit integer */
  SInt16              vendor3;                /* vendor-defined signed 16-bit integer */
};
typedef struct TabletPointRec           TabletPointRec;
typedef TabletPointRec                  TabletPointerRec;
struct TabletProximityRec {
  UInt16              vendorID;               /* vendor-defined ID - typically will be USB vendor ID */
  UInt16              tabletID;               /* vendor-defined tablet ID - typically will be USB product ID for the tablet */
  UInt16              pointerID;              /* vendor-defined ID of the specific pointing device */
  UInt16              deviceID;               /* system-assigned unique device ID - matches to deviceID field in tablet event */
  UInt16              systemTabletID;         /* system-assigned unique tablet ID */
  UInt16              vendorPointerType;      /* vendor-defined pointer type */
  UInt32              pointerSerialNumber;    /* vendor-defined serial number of the specific pointing device */
  UInt64              uniqueID;               /* vendor-defined unique ID for this pointer */
  UInt32              capabilityMask;         /* mask representing the capabilities of the device */
  UInt8               pointerType;            /* type of pointing device - enum to be defined */
  UInt8               enterProximity;         /* non-zero = entering; zero = leaving */
};
typedef struct TabletProximityRec       TabletProximityRec;
/*--------------------------------------------------------------------------------------*/
/* Volume Events                                                                        */
/*--------------------------------------------------------------------------------------*/

/*
 *  Summary:
 *    Volume events (kEventClassVolume)
 */
enum {

  /*
   * A new volume has been mounted (or new media inserted). Available
   * on Mac OS X and CarbonLib 1.3.1 and later.
   */
  kEventVolumeMounted           = 1,    /* new volume mounted*/

  /*
   * An existing volume has been unmounted (or media ejected).
   * Available on Mac OS X and CarbonLib 1.3.1 and later.
   */
  kEventVolumeUnmounted         = 2     /* volume has been ejected or unmounted*/
};

/*
    Parameters for volume events:

    kEventVolumeMounted
        -->     kEventParamDirectObject     typeFSVolumeRefNum
    
    kEventVolumeUnmounted
        -->     kEventParamDirectObject     typeFSVolumeRefNum
*/

/* types for volume events*/

enum {
  typeFSVolumeRefNum            = FOUR_CHAR_CODE('voln') /* FSVolumeRefNum*/
};

/*--------------------------------------------------------------------------------------*/
/* Appearance Events                                                                    */
/*--------------------------------------------------------------------------------------*/
/* Appearance Events */

/*
 *  Discussion:
 *    Appearance events (kEventClassAppearance)
 */
enum {

  /*
   * The scroll bar variant has changed. Available on Mac OS X 10.1 and
   * later.
   */
  kEventAppearanceScrollBarVariantChanged = 1
};

/*
    Parameters for appearance events:

    kEventAppearanceScrollBarVariantChanged
        -->     kEventParamNewScrollBarVariant      typeShortInteger
*/
/*--------------------------------------------------------------------------------------*/
/*  Services Events                                                                     */
/*--------------------------------------------------------------------------------------*/

/*
 *  Summary:
 *    Services events (kEventClassService)
 *  
 *  Discussion:
 *    Services are a feature by which applications can communicate with
 *    one another to request and provide services. This communication
 *    happens through the Scrap Manager. When an event is sent that
 *    requires communication for processing or servicing a request, you
 *    will need to use the scrap that is given in the
 *    kEventParamScrapRef parameter of the event in order to
 *    communicate. When your app requests a service, it is usually
 *    acting on data that is currently selected or in focus. Therefore
 *    all of the Services events are sent to the UserFocus event
 *    target. 
 *    Service events are available on Mac OS X 10.1 and later.
 */
enum {

  /*
   * The user has invoked a service that requires the application to
   * update the given scrap in the kEventParamScrapRef parameter with
   * appropriate data from the focus.
   */
  kEventServiceCopy             = 1,

  /*
   * The user has invoked a service that requires the application to
   * update the current focus with data from the scrap that is given in
   * the kEventParamScrapRef parameter.
   */
  kEventServicePaste            = 2,

  /*
   * The Services Manager needs to know what types of data the
   * application can copy and paste to and from the scrap in order to
   * update the Services menu to enable items that the user can select.
   * This event sends two CFMutableArrayRefs for the application to add
   * the types to: the kEventParamServiceCopyTypes and the
   * kEventParamServicePasteTypes parameters. The types that are added
   * are CFStringRefs. There is a convenience function,
   * CreateTypeStringWithOSType(), which takes an OSType and will
   * create a CFStringRef that you can add to the array(s).
   */
  kEventServiceGetTypes         = 3,

  /*
   * If the application is a service provider, this event will get sent
   * when a Service is requested of the app. The scrap that should be
   * used to send and receive data from the requester is provided in
   * the kEventParamScrapRef parameter. The
   * kEventParamServiceMessageName parameter contains a CFStringRef
   * that indicates which advertised service was invoked.
   */
  kEventServicePerform          = 4
};

/*
    Parameters for service events:
    
    kEventServiceCopy
        -->     kEventParamScrapRef                 typeScrapRef
    
    kEventServicePaste
        -->     kEventParamScrapRef                 typeScrapRef
        
    kEventServiceGetTypes
        -->     kEventParamServiceCopyTypes         typeCFMutableArrayRef of CFStringRefs
        -->     kEventParamServicePasteTypes        typeCFMutableArrayRef of CFStringRefs
        
    kEventServicePerform
        -->     kEventParamScrapRef                 typeScrapRef
        -->     kEventParamServiceMessageName       typeCFStringRef
        -->     kEventParamServiceUserData          typeCFStringRef
*/
/*
 *  CreateTypeStringWithOSType()
 *  
 *  Discussion:
 *    This routine is used to convert an OSType to a CFStringRef that
 *    Services will understand.
 *  
 *  Parameters:
 *    
 *    inType:
 *      The OSType that needs to be converted to a CFString.
 *  
 *  Result:
 *    A CFStringRef that contains the string that corresponds to the
 *    given OSType. This follows CoreFoundation semantics in that it
 *    will return NULL for failure, and because it is a "Create"
 *    function you will need to CFRelease() this string when it is no
 *    longer needed.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API( CFStringRef )
CreateTypeStringWithOSType(OSType inType);


/*--------------------------------------------------------------------------------------*/
/*  Accessibility Events                                                                */
/*--------------------------------------------------------------------------------------*/

/*
 *  Discussion:
 *    Accessibility events (kEventClassAccessibility)
 */
enum {

  /*
   * The kEventParamMouseLocation parameter will contain a global
   * point. Find the child of yourself which is underneath that point
   * and return it in the kEventParamAccessibleChild parameter. If
   * there is no child at the given point, you should still return
   * noErr, but leave the parameter empty. Only return immediate
   * children; do not return grandchildren of yourself.
   */
  kEventAccessibleGetChildAtPoint = 1,

  /*
   * Find the child of yourself which is part of the focus chain and
   * return it in the kEventParamAccessibleChild parameter. If there is
   * no child in the focus chain, you should still return noErr, but
   * leave the parameter empty. Only return immediate children; do not
   * return grandchildren of yourself.
   */
  kEventAccessibleGetFocusedChild = 2,

  /*
   * The kEventParamAccessibleAttributeNames parameter will contain a
   * CFMutableArrayRef. Add each of the attribute names you support to
   * this array in the form of a CFStringRef.
   */
  kEventAccessibleGetAllAttributeNames = 21,

  /*
   * The kEventParamAccessibleAttributeName parameter will contain an
   * attribute name in the form of a CFStringRef. If you support the
   * named attribute, return the attribute's value in the
   * kEventParamAccessibleAttributeValue parameter.
   */
  kEventAccessibleGetNamedAttribute = 22,

  /*
   * The kEventParamAccessibleAttributeName parameter will contain an
   * attribute name in the form of a CFStringRef. The
   * kEventParamAccessibleAttributeValue parameter will contain data in
   * an arbitrary format. If you support the named attribute, set the
   * named attribute's value to the data provided in the event.
   */
  kEventAccessibleSetNamedAttribute = 23,

  /*
   * The kEventParamAccessibleAttributeName parameter will contain an
   * attribute name in the form of a CFStringRef. If you support the
   * named attribute, set the kEventParamAccessibleAttributeSettable
   * attribute to a Boolean indicating whether the named attribute can
   * have its value changed via the kEventAccessibleSetNamedAttribute
   * event.
   */
  kEventAccessibleIsNamedAttributeSettable = 24,

  /*
   * The kEventParamAccessibleActionNames parameter will contain a
   * CFMutableArrayRef. Add each of the action names you support to
   * this array in the form of a CFStringRef.
   */
  kEventAccessibleGetAllActionNames = 41,

  /*
   * The kEventParamAccessibleActionName parameter will contain an
   * attribute name in the form of a CFStringRef. If you support the
   * named action, perform the action.
   */
  kEventAccessiblePerformNamedAction = 42,

  /*
   * The kEventParamAccessibleActionName parameter will contain an
   * attribute name in the form of a CFStringRef. The
   * kEventParamAccessibleActionDescription parameter will contain a
   * CFMutableStringRef. If you support the named action, alter the
   * mutable string to contain a textual description of the action's
   * significance.
   */
  kEventAccessibleGetNamedActionDescription = 44
};


/*
    Parameters for Accessibility events:

    kEventAccessibleGetChildAtPoint
        -->     kEventParamAccessibleObject             typeCFTypeRef with an AXUIElementRef
        -->     kEventParamMouseLocation                typeHIPoint
        <--     kEventParamAccessibleChild              typeCFTypeRef with an AXUIElementRef

    kEventAccessibleGetFocusedChild
        -->     kEventParamAccessibleObject             typeCFTypeRef with an AXUIElementRef
        <--     kEventParamAccessibleChild              typeCFTypeRef with an AXUIElementRef

    kEventAccessibleGetAllAttributeNames
        -->     kEventParamAccessibleObject             typeCFTypeRef with an AXUIElementRef
        <->     kEventParamAccessibleAttributeNames     typeCFMutableArrayRef

    kEventAccessibleGetNamedAttribute
        -->     kEventParamAccessibleObject             typeCFTypeRef with an AXUIElementRef
        -->     kEventParamAccessibleAttributeName      typeCFStringRef
        <--     kEventParamAccessibleAttributeValue     variable

    kEventAccessibleSetNamedAttribute
        -->     kEventParamAccessibleObject             typeCFTypeRef with an AXUIElementRef
        -->     kEventParamAccessibleAttributeName      typeCFStringRef
        -->     kEventParamAccessibleAttributeValue     variable

    kEventAccessibleIsNamedAttributeSettable
        -->     kEventParamAccessibleObject             typeCFTypeRef with an AXUIElementRef
        -->     kEventParamAccessibleAttributeName      typeCFStringRef
        <--     kEventParamAccessibleAttributeSettable  typeBoolean

    kEventAccessibleGetAllActionNames
        -->     kEventParamAccessibleObject             typeCFTypeRef with an AXUIElementRef
        <->     kEventParamAccessibleActionNames        typeCFMutableArrayRef

    kEventAccessiblePerformNamedAction
        -->     kEventParamAccessibleObject             typeCFTypeRef with an AXUIElementRef
        -->     kEventParamAccessibleActionName         typeCFStringRef

    kEventAccessibleGetNamedActionDescription
        -->     kEventParamAccessibleObject             typeCFTypeRef with an AXUIElementRef
        -->     kEventParamAccessibleActionName         typeCFStringRef
        <->     kEventParamAccessibleActionDescription  typeCFMutableStringRef
*/
typedef struct __AXUIElement*           AXUIElementRef;
/*
 *  AXUIElementCreateWithHIObjectAndIdentifier()
 *  
 *  Discussion:
 *    This routine creates an AXUIElementRef to represent an accessible
 *    object for a Carbon application. A Carbon accessible object is
 *    comprised of an HIObjectRef and a 64-bit identifier. The
 *    resulting AXUIElementRef is a CFTypeRef, and must be managed as
 *    such.
 *  
 *  Parameters:
 *    
 *    inHIObject:
 *      The HIObjectRef of the accessible object.
 *    
 *    inIdentifier:
 *      The 64-bit identifier of the accessible object.
 *  
 *  Result:
 *    An AXUIElementRef that represents the Carbon accessible object
 *    identified by the given HIObjectRef and 64-bit identifier. This
 *    follows CoreFoundation semantics in that it will return NULL for
 *    failure, and because it is a "Create" function you will need to
 *    CFRelease() this AXUIElementRef when it is no longer needed.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( AXUIElementRef )
AXUIElementCreateWithHIObjectAndIdentifier(
  HIObjectRef   inHIObject,
  UInt64        inIdentifier);


/*
 *  AXUIElementGetHIObject()
 *  
 *  Discussion:
 *    If the incoming AXUIElementRef is a Carbon accessible object,
 *    this routine will return the HIObjectRef of the accessible object.
 *  
 *  Parameters:
 *    
 *    inUIElement:
 *      The AXUIElementRef of whom you'd like to get the HIObjectRef.
 *  
 *  Result:
 *    The HIObjectRef of the AXUIElementRef. If the incoming
 *    AXUIElementRef is not a Carbon accessible object, this routine
 *    will return NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIObjectRef )
AXUIElementGetHIObject(AXUIElementRef inUIElement);


/*
 *  AXUIElementGetIdentifier()
 *  
 *  Discussion:
 *    If the incoming AXUIElementRef is a Carbon accessible object,
 *    this routine will pass back the 64-bit identifier of the
 *    accessible object.
 *  
 *  Parameters:
 *    
 *    inUIElement:
 *      The AXUIElementRef of whom you'd like to get the 64-bit
 *      identifier.
 *    
 *    outIdentifier:
 *      The 64-bit identifier of the AXUIElementRef. If the incoming
 *      AXUIElementRef is not a Carbon accessible object, this routine
 *      will pass back zero. Note that zero is often a legal value for
 *      Carbon accessible object, so do not assume that the accessible
 *      object is not a Carbon accessible object just because you get a
 *      result of zero.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
AXUIElementGetIdentifier(
  AXUIElementRef   inUIElement,
  UInt64 *         outIdentifier);


/*
 *  AXNotificationHIObjectNotify()
 *  
 *  Discussion:
 *    Posts a notification for the given pseudo-AXUIElementRef. Though
 *    an actual AXUIElementRef is not actually passed in to this
 *    function, its component parts are. This saves the implementation
 *    the hassle of dismantling the AXUIElementRef into its component
 *    parts.
 *  
 *  Parameters:
 *    
 *    inNotification:
 *      The notification name string.
 *    
 *    inHIObject:
 *      The HIObjectRef component of the AXUIElementRef to whom the
 *      notification applies.
 *    
 *    inIdentifier:
 *      The 64-bit identifier component of the AXUIElementRef to whom
 *      the notification applies.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
AXNotificationHIObjectNotify(
  CFStringRef   inNotification,
  HIObjectRef   inHIObject,
  UInt64        inIdentifier);


/*--------------------------------------------------------------------------------------*/
/* Parameter names and types                                                            */
/*--------------------------------------------------------------------------------------*/
enum {
  kEventParamDirectObject       = FOUR_CHAR_CODE('----') /* type varies depending on event*/
};


/*
 */
enum {

  /*
   * This event parameter may be added to any event that is posted to
   * the main event queue. When the event is removed from the queue and
   * sent to the event dispatcher, the dispatcher will retrieve the
   * EventTargetRef contained in this parameter and send the event
   * directly to that event target. If this parameter is not available
   * in the event, the dispatcher will send the event to a suitable
   * target, or to the application target if no more specific target is
   * appropriate. Available in CarbonLib 1.3.1 and later, and Mac OS X.
   */
  kEventParamPostTarget         = FOUR_CHAR_CODE('ptrg'), /* typeEventTargetRef*/

  /*
   * Indicates an event parameter of type EventTargetRef.
   */
  typeEventTargetRef            = FOUR_CHAR_CODE('etrg') /* EventTargetRef*/
};

/* Generic toolbox parameters and types*/

enum {
  kEventParamWindowRef          = FOUR_CHAR_CODE('wind'), /* typeWindowRef*/
  kEventParamGrafPort           = FOUR_CHAR_CODE('graf'), /* typeGrafPtr*/
  kEventParamDragRef            = FOUR_CHAR_CODE('drag'), /* typeDragRef*/
  kEventParamMenuRef            = FOUR_CHAR_CODE('menu'), /* typeMenuRef*/
  kEventParamEventRef           = FOUR_CHAR_CODE('evnt'), /* typeEventRef*/
  kEventParamControlRef         = FOUR_CHAR_CODE('ctrl'), /* typeControlRef*/
  kEventParamRgnHandle          = FOUR_CHAR_CODE('rgnh'), /* typeQDRgnHandle*/
  kEventParamEnabled            = FOUR_CHAR_CODE('enab'), /* typeBoolean*/
  kEventParamDimensions         = FOUR_CHAR_CODE('dims'), /* typeQDPoint*/
  kEventParamAvailableBounds    = FOUR_CHAR_CODE('avlb'), /* typeQDRectangle*/
  kEventParamAEEventID          = keyAEEventID, /* typeType*/
  kEventParamAEEventClass       = keyAEEventClass, /* typeType*/
  kEventParamCGContextRef       = FOUR_CHAR_CODE('cntx'), /* typeCGContextRef*/
  kEventParamDeviceDepth        = FOUR_CHAR_CODE('devd'), /* typeShortInteger*/
  kEventParamDeviceColor        = FOUR_CHAR_CODE('devc'), /* typeBoolean*/
  kEventParamMutableArray       = FOUR_CHAR_CODE('marr'), /* typeCFMutableArrayRef*/
  kEventParamResult             = FOUR_CHAR_CODE('ansr'), /* any type - depends on event like direct object*/
  kEventParamMinimumSize        = FOUR_CHAR_CODE('mnsz'), /* typeHISize*/
  kEventParamMaximumSize        = FOUR_CHAR_CODE('mxsz'), /* typeHISize*/
  typeWindowRef                 = FOUR_CHAR_CODE('wind'), /* WindowRef*/
  typeGrafPtr                   = FOUR_CHAR_CODE('graf'), /* CGrafPtr*/
  typeGWorldPtr                 = FOUR_CHAR_CODE('gwld'), /* GWorldPtr*/
  typeDragRef                   = FOUR_CHAR_CODE('drag'), /* DragRef*/
  typeMenuRef                   = FOUR_CHAR_CODE('menu'), /* MenuRef*/
  typeControlRef                = FOUR_CHAR_CODE('ctrl'), /* ControlRef*/
  typeCollection                = FOUR_CHAR_CODE('cltn'), /* Collection*/
  typeQDRgnHandle               = FOUR_CHAR_CODE('rgnh'), /* RgnHandle*/
  typeOSStatus                  = FOUR_CHAR_CODE('osst'), /* OSStatus*/
  typeCFStringRef               = FOUR_CHAR_CODE('cfst'), /* CFStringRef*/
  typeCFMutableStringRef        = FOUR_CHAR_CODE('cfms'), /* CFMutableStringRef*/
  typeCFIndex                   = FOUR_CHAR_CODE('cfix'), /* CFIndex*/
  typeCFTypeRef                 = FOUR_CHAR_CODE('cfty'), /* CFTypeRef*/
  typeCGContextRef              = FOUR_CHAR_CODE('cntx'), /* CGContextRef*/
  typeHIPoint                   = FOUR_CHAR_CODE('hipt'), /* HIPoint*/
  typeHISize                    = FOUR_CHAR_CODE('hisz'), /* HISize*/
  typeHIRect                    = FOUR_CHAR_CODE('hirc'), /* HIRect*/
  typeVoidPtr                   = FOUR_CHAR_CODE('void') /* void * (used for hiobject fun)*/
};

/* Mouse event parameters and types*/

enum {
  kEventParamMouseLocation      = FOUR_CHAR_CODE('mloc'), /* typeHIPoint*/
  kEventParamWindowMouseLocation = FOUR_CHAR_CODE('wmou'), /* typeHIPoint (Mac OS X 10.1 or later)*/
  kEventParamMouseButton        = FOUR_CHAR_CODE('mbtn'), /* typeMouseButton*/
  kEventParamClickCount         = FOUR_CHAR_CODE('ccnt'), /* typeUInt32*/
  kEventParamMouseWheelAxis     = FOUR_CHAR_CODE('mwax'), /* typeMouseWheelAxis*/
  kEventParamMouseWheelDelta    = FOUR_CHAR_CODE('mwdl'), /* typeSInt32*/
  kEventParamMouseDelta         = FOUR_CHAR_CODE('mdta'), /* typeQDPoint*/
  kEventParamMouseChord         = FOUR_CHAR_CODE('chor'), /* typeUInt32*/
  kEventParamTabletEventType    = FOUR_CHAR_CODE('tblt'), /* typeUInt32*/
  kEventParamMouseTrackingRef   = FOUR_CHAR_CODE('mtrf'), /* typeMouseTrackingRef*/
  typeMouseButton               = FOUR_CHAR_CODE('mbtn'), /* EventMouseButton*/
  typeMouseWheelAxis            = FOUR_CHAR_CODE('mwax'), /* EventMouseWheelAxis*/
  typeMouseTrackingRef          = FOUR_CHAR_CODE('mtrf') /* MouseTrackingRef*/
};

/* Keyboard event parameter and types*/

enum {
  kEventParamKeyCode            = FOUR_CHAR_CODE('kcod'), /* typeUInt32*/
  kEventParamKeyMacCharCodes    = FOUR_CHAR_CODE('kchr'), /* typeChar*/
  kEventParamKeyModifiers       = FOUR_CHAR_CODE('kmod'), /* typeUInt32*/
  kEventParamKeyUnicodes        = FOUR_CHAR_CODE('kuni'), /* typeUnicodeText*/
  kEventParamKeyboardType       = FOUR_CHAR_CODE('kbdt'), /* typeUInt32*/
  typeEventHotKeyID             = FOUR_CHAR_CODE('hkid') /* EventHotKeyID*/
};

/* TextInput event parameters*/

enum {
  kEventParamTextInputSendRefCon = FOUR_CHAR_CODE('tsrc'), /*    typeLongInteger*/
  kEventParamTextInputSendComponentInstance = FOUR_CHAR_CODE('tsci'), /*    typeComponentInstance*/
  kEventParamTextInputSendSLRec = FOUR_CHAR_CODE('tssl'), /*    typeIntlWritingCode*/
  kEventParamTextInputReplySLRec = FOUR_CHAR_CODE('trsl'), /*    typeIntlWritingCode*/
  kEventParamTextInputSendText  = FOUR_CHAR_CODE('tstx'), /*    typeUnicodeText (if TSMDocument is Unicode), otherwise typeChar*/
  kEventParamTextInputReplyText = FOUR_CHAR_CODE('trtx'), /*    typeUnicodeText (if TSMDocument is Unicode), otherwise typeChar*/
  kEventParamTextInputSendUpdateRng = FOUR_CHAR_CODE('tsup'), /*    typeTextRangeArray*/
  kEventParamTextInputSendHiliteRng = FOUR_CHAR_CODE('tshi'), /*    typeTextRangeArray*/
  kEventParamTextInputSendClauseRng = FOUR_CHAR_CODE('tscl'), /*    typeOffsetArray*/
  kEventParamTextInputSendPinRng = FOUR_CHAR_CODE('tspn'), /*    typeTextRange*/
  kEventParamTextInputSendFixLen = FOUR_CHAR_CODE('tsfx'), /*    typeLongInteger*/
  kEventParamTextInputSendLeadingEdge = FOUR_CHAR_CODE('tsle'), /*    typeBoolean*/
  kEventParamTextInputReplyLeadingEdge = FOUR_CHAR_CODE('trle'), /*    typeBoolean*/
  kEventParamTextInputSendTextOffset = FOUR_CHAR_CODE('tsto'), /*    typeLongInteger*/
  kEventParamTextInputReplyTextOffset = FOUR_CHAR_CODE('trto'), /*    typeLongInteger*/
  kEventParamTextInputReplyRegionClass = FOUR_CHAR_CODE('trrg'), /*    typeLongInteger*/
  kEventParamTextInputSendCurrentPoint = FOUR_CHAR_CODE('tscp'), /*    typeQDPoint*/
  kEventParamTextInputSendDraggingMode = FOUR_CHAR_CODE('tsdm'), /*    typeBoolean*/
  kEventParamTextInputReplyPoint = FOUR_CHAR_CODE('trpt'), /*    typeQDPoint*/
  kEventParamTextInputReplyFont = FOUR_CHAR_CODE('trft'), /*    typeLongInteger*/
  kEventParamTextInputReplyFMFont = FOUR_CHAR_CODE('trfm'), /*    typeUInt32*/
  kEventParamTextInputReplyPointSize = FOUR_CHAR_CODE('trpz'), /*    typeFixed*/
  kEventParamTextInputReplyLineHeight = FOUR_CHAR_CODE('trlh'), /*    typeShortInteger*/
  kEventParamTextInputReplyLineAscent = FOUR_CHAR_CODE('trla'), /*    typeShortInteger*/
  kEventParamTextInputReplyTextAngle = FOUR_CHAR_CODE('trta'), /*    typeFixed*/
  kEventParamTextInputSendShowHide = FOUR_CHAR_CODE('tssh'), /*    typeBoolean*/
  kEventParamTextInputReplyShowHide = FOUR_CHAR_CODE('trsh'), /*    typeBoolean*/
  kEventParamTextInputSendKeyboardEvent = FOUR_CHAR_CODE('tske'), /*    typeEventRef*/
  kEventParamTextInputSendTextServiceEncoding = FOUR_CHAR_CODE('tsse'), /*    typeUInt32*/
  kEventParamTextInputSendTextServiceMacEncoding = FOUR_CHAR_CODE('tssm'), /*    typeUInt32*/
  kEventParamTextInputGlyphInfoArray = FOUR_CHAR_CODE('glph') /*    typeGlyphInfoArray*/
};

/* Command event parameters and types*/

enum {
  kEventParamHICommand          = FOUR_CHAR_CODE('hcmd'), /* typeHICommand*/
  typeHICommand                 = FOUR_CHAR_CODE('hcmd') /* HICommand*/
};

/* Window event parameters and types*/

enum {
  kEventParamWindowFeatures     = FOUR_CHAR_CODE('wftr'), /* typeUInt32*/
  kEventParamWindowDefPart      = FOUR_CHAR_CODE('wdpc'), /* typeWindowDefPartCode*/
  kEventParamCurrentBounds      = FOUR_CHAR_CODE('crct'), /* typeQDRectangle*/
  kEventParamOriginalBounds     = FOUR_CHAR_CODE('orct'), /* typeQDRectangle*/
  kEventParamPreviousBounds     = FOUR_CHAR_CODE('prct'), /* typeQDRectangle*/
  kEventParamClickActivation    = FOUR_CHAR_CODE('clac'), /* typeClickActivationResult*/
  kEventParamWindowRegionCode   = FOUR_CHAR_CODE('wshp'), /* typeWindowRegionCode*/
  kEventParamWindowDragHiliteFlag = FOUR_CHAR_CODE('wdhf'), /* typeBoolean*/
  kEventParamWindowModifiedFlag = FOUR_CHAR_CODE('wmff'), /* typeBoolean*/
  kEventParamWindowProxyGWorldPtr = FOUR_CHAR_CODE('wpgw'), /* typeGWorldPtr*/
  kEventParamWindowProxyImageRgn = FOUR_CHAR_CODE('wpir'), /* typeQDRgnHandle*/
  kEventParamWindowProxyOutlineRgn = FOUR_CHAR_CODE('wpor'), /* typeQDRgnHandle*/
  kEventParamWindowStateChangedFlags = FOUR_CHAR_CODE('wscf'), /* typeUInt32 */
  kEventParamWindowTitleFullWidth = FOUR_CHAR_CODE('wtfw'), /* typeSInt16*/
  kEventParamWindowTitleTextWidth = FOUR_CHAR_CODE('wttw'), /* typeSInt16*/
  kEventParamWindowGrowRect     = FOUR_CHAR_CODE('grct'), /* typeQDRectangle*/
  kEventParamAttributes         = FOUR_CHAR_CODE('attr'), /* typeUInt32*/
  kEventParamPreviousDockRect   = FOUR_CHAR_CODE('pdrc'), /* typeHIRect*/
  kEventParamCurrentDockRect    = FOUR_CHAR_CODE('cdrc'), /* typeHIRect*/
  typeWindowRegionCode          = FOUR_CHAR_CODE('wshp'), /* WindowRegionCode*/
  typeWindowDefPartCode         = FOUR_CHAR_CODE('wdpt'), /* WindowDefPartCode*/
  typeClickActivationResult     = FOUR_CHAR_CODE('clac') /* ClickActivationResult*/
};

/* Control event parameters and types*/

enum {
  kEventParamControlPart        = FOUR_CHAR_CODE('cprt'), /* typeControlPartCode*/
  kEventParamInitCollection     = FOUR_CHAR_CODE('icol'), /* typeCollection*/
  kEventParamControlMessage     = FOUR_CHAR_CODE('cmsg'), /* typeShortInteger*/
  kEventParamControlParam       = FOUR_CHAR_CODE('cprm'), /* typeLongInteger*/
  kEventParamControlResult      = FOUR_CHAR_CODE('crsl'), /* typeLongInteger*/
  kEventParamControlRegion      = FOUR_CHAR_CODE('crgn'), /* typeQDRgnHandle*/
  kEventParamControlAction      = FOUR_CHAR_CODE('caup'), /* typeControlActionUPP*/
  kEventParamControlIndicatorDragConstraint = FOUR_CHAR_CODE('cidc'), /* typeIndicatorDragConstraint*/
  kEventParamControlIndicatorRegion = FOUR_CHAR_CODE('cirn'), /* typeQDRgnHandle*/
  kEventParamControlIsGhosting  = FOUR_CHAR_CODE('cgst'), /* typeBoolean*/
  kEventParamControlIndicatorOffset = FOUR_CHAR_CODE('ciof'), /* typeQDPoint*/
  kEventParamControlClickActivationResult = FOUR_CHAR_CODE('ccar'), /* typeClickActivationResult*/
  kEventParamControlSubControl  = FOUR_CHAR_CODE('csub'), /* typeControlRef*/
  kEventParamControlOptimalBounds = FOUR_CHAR_CODE('cobn'), /* typeQDRectangle*/
  kEventParamControlOptimalBaselineOffset = FOUR_CHAR_CODE('cobo'), /* typeShortInteger*/
  kEventParamControlDataTag     = FOUR_CHAR_CODE('cdtg'), /* typeEnumeration*/
  kEventParamControlDataBuffer  = FOUR_CHAR_CODE('cdbf'), /* typePtr*/
  kEventParamControlDataBufferSize = FOUR_CHAR_CODE('cdbs'), /* typeLongInteger*/
  kEventParamControlDrawDepth   = FOUR_CHAR_CODE('cddp'), /* typeShortInteger*/
  kEventParamControlDrawInColor = FOUR_CHAR_CODE('cdic'), /* typeBoolean*/
  kEventParamControlFeatures    = FOUR_CHAR_CODE('cftr'), /* typeUInt32*/
  kEventParamControlPartBounds  = FOUR_CHAR_CODE('cpbd'), /* typeQDRectangle*/
  kEventParamControlOriginalOwningWindow = FOUR_CHAR_CODE('coow'), /* typeWindowRef*/
  kEventParamControlCurrentOwningWindow = FOUR_CHAR_CODE('ccow'), /* typeWindowRef*/
  kEventParamControlFocusEverything = FOUR_CHAR_CODE('cfev'), /* typeBoolean*/
  kEventParamNextControl        = FOUR_CHAR_CODE('cnxc'), /* typeControlRef*/
  kEventParamStartControl       = FOUR_CHAR_CODE('cstc'), /* typeControlRef*/
  kEventParamControlSubview     = FOUR_CHAR_CODE('csvw'), /* typeControlRef*/
  kEventParamControlPreviousPart = FOUR_CHAR_CODE('copc'), /* typeControlPartCode*/
  kEventParamControlCurrentPart = FOUR_CHAR_CODE('cnpc'), /* typeControlPartCode*/
  kEventParamControlInvalRgn    = FOUR_CHAR_CODE('civr'), /* typeQDRgnHandle*/
  kEventParamControlValue       = FOUR_CHAR_CODE('cval'), /* typeLongInteger*/
  typeControlActionUPP          = FOUR_CHAR_CODE('caup'), /* ControlActionUPP*/
  typeIndicatorDragConstraint   = FOUR_CHAR_CODE('cidc'), /* IndicatorDragConstraint*/
  typeControlPartCode           = FOUR_CHAR_CODE('cprt') /* ControlPartCode*/
};

/* Menu event parameters and types*/

enum {
  kEventParamCurrentMenuTrackingMode = FOUR_CHAR_CODE('cmtm'), /* typeMenuTrackingMode*/
  kEventParamNewMenuTrackingMode = FOUR_CHAR_CODE('nmtm'), /* typeMenuTrackingMode*/
  kEventParamMenuFirstOpen      = FOUR_CHAR_CODE('1sto'), /* typeBoolean*/
  kEventParamMenuItemIndex      = FOUR_CHAR_CODE('item'), /* typeMenuItemIndex*/
  kEventParamMenuCommand        = FOUR_CHAR_CODE('mcmd'), /* typeMenuCommand*/
  kEventParamEnableMenuForKeyEvent = FOUR_CHAR_CODE('fork'), /* typeBoolean*/
  kEventParamMenuEventOptions   = FOUR_CHAR_CODE('meop'), /* typeMenuEventOptions*/
  kEventParamMenuContext        = FOUR_CHAR_CODE('mctx'), /* typeUInt32*/
  kEventParamMenuItemBounds     = FOUR_CHAR_CODE('mitb'), /* typeQDRectangle*/
  kEventParamMenuMarkBounds     = FOUR_CHAR_CODE('mmkb'), /* typeQDRectangle*/
  kEventParamMenuIconBounds     = FOUR_CHAR_CODE('micb'), /* typeQDRectangle*/
  kEventParamMenuTextBounds     = FOUR_CHAR_CODE('mtxb'), /* typeQDRectangle*/
  kEventParamMenuTextBaseline   = FOUR_CHAR_CODE('mtbl'), /* typeShortInteger*/
  kEventParamMenuCommandKeyBounds = FOUR_CHAR_CODE('mcmb'), /* typeQDRectangle*/
  kEventParamMenuVirtualTop     = FOUR_CHAR_CODE('mvrt'), /* typeLongInteger*/
  kEventParamMenuVirtualBottom  = FOUR_CHAR_CODE('mvrb'), /* typeLongInteger*/
  kEventParamMenuDrawState      = FOUR_CHAR_CODE('mdrs'), /* typeThemeDrawState*/
  kEventParamMenuItemType       = FOUR_CHAR_CODE('mitp'), /* typeThemeMenuItemType*/
  kEventParamMenuItemWidth      = FOUR_CHAR_CODE('mitw'), /* typeShortInteger*/
  kEventParamMenuItemHeight     = FOUR_CHAR_CODE('mith'), /* typeShortInteger*/
  typeMenuItemIndex             = FOUR_CHAR_CODE('midx'), /* MenuItemIndex*/
  typeMenuCommand               = FOUR_CHAR_CODE('mcmd'), /* MenuCommand*/
  typeMenuTrackingMode          = FOUR_CHAR_CODE('mtmd'), /* MenuTrackingMode*/
  typeMenuEventOptions          = FOUR_CHAR_CODE('meop'), /* MenuEventOptions*/
  typeThemeMenuState            = FOUR_CHAR_CODE('tmns'), /* ThemeMenuState*/
  typeThemeMenuItemType         = FOUR_CHAR_CODE('tmit') /* ThemeMenuItemType*/
};

/* Application event parameters*/

enum {
  kEventParamProcessID          = FOUR_CHAR_CODE('psn '), /* typeProcessSerialNumber*/
  kEventParamLaunchRefCon       = FOUR_CHAR_CODE('lref'), /* typeUInt32*/
  kEventParamLaunchErr          = FOUR_CHAR_CODE('err '), /* typeOSStatus*/
  kEventParamSystemUIMode       = FOUR_CHAR_CODE('uimd') /* typeUInt32*/
};

/* Tablet event parameters and types*/

enum {
  kEventParamTabletPointRec     = FOUR_CHAR_CODE('tbrc'), /* typeTabletPointRec*/
  kEventParamTabletProximityRec = FOUR_CHAR_CODE('tbpx'), /* typeTabletProximityRec*/
  typeTabletPointRec            = FOUR_CHAR_CODE('tbrc'), /* kEventParamTabletPointRec*/
  typeTabletProximityRec        = FOUR_CHAR_CODE('tbpx'), /* kEventParamTabletProximityRec*/
  kEventParamTabletPointerRec   = FOUR_CHAR_CODE('tbrc'), /* typeTabletPointerRec      -- deprecated, for compatibility only*/
  typeTabletPointerRec          = FOUR_CHAR_CODE('tbrc') /* kEventParamTabletPointerRec    -- deprecated, for compatibility only*/
};

/* Appearance event parameters*/

enum {
  kEventParamNewScrollBarVariant = FOUR_CHAR_CODE('nsbv') /* typeShortInteger*/
};

/* Service event parameters*/

enum {
  kEventParamScrapRef           = FOUR_CHAR_CODE('scrp'), /*    typeScrapRef*/
  kEventParamServiceCopyTypes   = FOUR_CHAR_CODE('svsd'), /*    typeCFMutableArrayRef*/
  kEventParamServicePasteTypes  = FOUR_CHAR_CODE('svpt'), /*    typeCFMutableArrayRef*/
  kEventParamServiceMessageName = FOUR_CHAR_CODE('svmg'), /*    typeCFStringRef*/
  kEventParamServiceUserData    = FOUR_CHAR_CODE('svud'), /*    typeCFStringRef*/
  typeScrapRef                  = FOUR_CHAR_CODE('scrp'), /*    ScrapRef*/
  typeCFMutableArrayRef         = FOUR_CHAR_CODE('cfma') /*    CFMutableArrayRef*/
};

/* Toolbar event parameters and types*/

enum {
  kEventParamToolbar            = FOUR_CHAR_CODE('tbar'), /* typeHIToolbarRef*/
  kEventParamToolbarItem        = FOUR_CHAR_CODE('tbit'), /* typeHIToolbarItemRef*/
  kEventParamToolbarItemIdentifier = FOUR_CHAR_CODE('tbii'), /* typeCFStringRef*/
  kEventParamToolbarItemConfigData = FOUR_CHAR_CODE('tbid'), /* typeCFTypeRef*/
  typeHIToolbarRef              = FOUR_CHAR_CODE('tbar'), /* HIToolbarRef*/
  typeHIToolbarItemRef          = FOUR_CHAR_CODE('tbit') /* HIToolbarItemRef*/
};

/* Accessibility event parameters*/

enum {
  kEventParamAccessibleObject   = FOUR_CHAR_CODE('aobj'), /* typeCFTypeRef with an AXUIElementRef*/
  kEventParamAccessibleChild    = FOUR_CHAR_CODE('achl'), /* typeCFTypeRef with an AXUIElementRef*/
  kEventParamAccessibleAttributeName = FOUR_CHAR_CODE('atnm'), /* typeCFStringRef*/
  kEventParamAccessibleAttributeNames = FOUR_CHAR_CODE('atns'), /* typeCFMutableArrayRef of CFStringRefs*/
  kEventParamAccessibleAttributeValue = FOUR_CHAR_CODE('atvl'), /* variable*/
  kEventParamAccessibleAttributeSettable = FOUR_CHAR_CODE('atst'), /* typeBoolean*/
  kEventParamAccessibleActionName = FOUR_CHAR_CODE('acnm'), /* typeCFStringRef*/
  kEventParamAccessibleActionNames = FOUR_CHAR_CODE('acns'), /* typeCFMutableArrayRef of CFStringRefs*/
  kEventParamAccessibleActionDescription = FOUR_CHAR_CODE('acds') /* typeCFMutableStringRef*/
};


/*
 *  GetWindowEventTarget()
 *  
 *  Discussion:
 *    Returns the EventTargetRef for the specified window. Once you
 *    obtain this reference, you can send events to the target and
 *    install an event handler on it.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window to return the target for.
 *  
 *  Result:
 *    An EventTargetRef.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( EventTargetRef )
GetWindowEventTarget(WindowRef inWindow);


/*
 *  GetControlEventTarget()
 *  
 *  Discussion:
 *    Returns the EventTargetRef for the specified control. Once you
 *    obtain this reference, you can send events to the target and
 *    install event handler on it.
 *  
 *  Parameters:
 *    
 *    inControl:
 *      The control to return the target for.
 *  
 *  Result:
 *    An EventTargetRef.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( EventTargetRef )
GetControlEventTarget(ControlRef inControl);


/*
 *  GetMenuEventTarget()
 *  
 *  Discussion:
 *    Returns the EventTargetRef for the specified menu. Once you
 *    obtain this reference, you can send events to the target and
 *    install event handler on it.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu to return the target for.
 *  
 *  Result:
 *    An EventTargetRef.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( EventTargetRef )
GetMenuEventTarget(MenuRef inMenu);


/*
 *  GetApplicationEventTarget()
 *  
 *  Discussion:
 *    Returns the EventTargetRef for the application. Once you obtain
 *    this reference, you can send events to the target and install
 *    event handler on it.
 *  
 *  Result:
 *    An EventTargetRef.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( EventTargetRef )
GetApplicationEventTarget(void);


/*
 *  GetUserFocusEventTarget()
 *  
 *  Discussion:
 *    Returns the EventTargetRef for the current user focus at the time
 *    of the call. Keyboard events are always sent to this target.
 *  
 *  Result:
 *    An EventTargetRef.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( EventTargetRef )
GetUserFocusEventTarget(void);


/*
 *  GetEventDispatcherTarget()
 *  
 *  Discussion:
 *    Returns the EventTargetRef for the standard toolbox dispatcher.
 *    You typically would never need to use this, but there are some
 *    exotic apps that need to pick events off the event queue and call
 *    the dispatcher themselves. This allows you to do just that
 *    instead of calling RunApplicationEventLoop to handle it all.
 *  
 *  Result:
 *    An EventTargetRef.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( EventTargetRef )
GetEventDispatcherTarget(void);



#define InstallApplicationEventHandler( h, n, l, u, r ) \
     InstallEventHandler( GetApplicationEventTarget(), (h), (n), (l), (u), (r) )

#define InstallHIObjectEventHandler( t, h, n, l, u, r ) \
     InstallEventHandler( HIObjectGetEventTarget( t ), (h), (n), (l), (u), (r) )

#define InstallWindowEventHandler( t, h, n, l, u, r ) \
       InstallEventHandler( GetWindowEventTarget( t ), (h), (n), (l), (u), (r) )

#define InstallControlEventHandler( t, h, n, l, u, r ) \
        InstallEventHandler( GetControlEventTarget( t ), (h), (n), (l), (u), (r) )

#define InstallMenuEventHandler( t, h, n, l, u, r ) \
      InstallEventHandler( GetMenuEventTarget( t ), (h), (n), (l), (u), (r) )


/*
    You can use the following macro in your code to allow you to set up an
 event handler lazily. You pass the name of your event handler in. You should
   use this with caution on Mac OS 9 systems since it could cause heap fragmentation.
*/
#define DEFINE_ONE_SHOT_HANDLER_GETTER( x )       \
EventHandlerUPP Get ## x ## UPP()             \
{                                             \
  static EventHandlerUPP  sHandler = NULL;    \
                                              \
  if ( sHandler == NULL )                     \
      sHandler = NewEventHandlerUPP( x );     \
                                              \
  return sHandler;                            \
}




#define SendEventToApplication( e ) \
        SendEventToEventTarget( (e), GetApplicationEventTarget() )

#define SendEventToHIObject( e, t ) \
      SendEventToEventTarget( (e), HIObjectGetEventTarget( t ) )

#define SendEventToWindow( e, t ) \
        SendEventToEventTarget( (e), GetWindowEventTarget( t ) )

#define SendEventToControl( e, t ) \
     SendEventToEventTarget( (e), GetControlEventTarget( t ) )

#define SendEventToMenu( e, t ) \
       SendEventToEventTarget( (e), GetMenuEventTarget( t ) )

#define SendEventToUserFocus( e ) \
        SendEventToEventTarget( (e), GetUserFocusEventTarget() )

/*======================================================================================*/
/*  EVENT-BASED OBJECT CLASSES                                                          */
/*                                                                                      */
/*  Here it is - the replacement for classic defprocs. This is also a convenient way    */
/*  to create toolbox objects (windows, etc.) that have a specific behavior without     */
/*  installing handlers on each instance of the object. With a toolbox object class,    */
/*  you register your class, then use special creation routines to create objects of    */
/*  that class. The event handlers are automatically installed and ready to go.         */
/*======================================================================================*/

typedef struct OpaqueToolboxObjectClassRef*  ToolboxObjectClassRef;

/*
 *  RegisterToolboxObjectClass()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
RegisterToolboxObjectClass(
  CFStringRef              inClassID,
  ToolboxObjectClassRef    inBaseClass,              /* can be NULL */
  UInt32                   inNumEvents,
  const EventTypeSpec *    inEventList,
  EventHandlerUPP          inEventHandler,
  void *                   inEventHandlerData,
  ToolboxObjectClassRef *  outClassRef);


/*
 *  UnregisterToolboxObjectClass()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
UnregisterToolboxObjectClass(ToolboxObjectClassRef inClassRef);


/*======================================================================================*/
/*  o Command Routines                                                                  */
/*======================================================================================*/

/*
 *  ProcessHICommand()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
ProcessHICommand(const HICommand * inCommand);


/*--------------------------------------------------------------------------------------*/
/*  o Event Loop Routines                                                               */
/*--------------------------------------------------------------------------------------*/

/*
 *  RunApplicationEventLoop()
 *  
 *  Discussion:
 *    This routine is used as the main event loop for a Carbon
 *    Event-based application. Once entered, this function waits for
 *    events to arrive and dispatches them to your event handlers
 *    automatically.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
RunApplicationEventLoop(void);


/*
 *  QuitApplicationEventLoop()
 *  
 *  Discussion:
 *    This routine is used to quit the RunApplicationEventLoop
 *    function. Typically, your application doesn't need to call this.
 *    If your application has the Quit menu item tagged with the
 *    kHICommandQuit Menu Command ID, the toolbox will automatically
 *    call this for your application, automatically terminating your
 *    event loop. If your application wants to do pre-processing before
 *    the event loop exits, it should intercept either the
 *    kHICommandQuit menu command, or the kEventApplicationQuit event.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
QuitApplicationEventLoop(void);


/*--------------------------------------------------------------------------------------*/
/*  o Event Modality routines                                                           */
/*--------------------------------------------------------------------------------------*/

/*
 *  RunAppModalLoopForWindow()
 *  
 *  Discussion:
 *    This routine is used as a replacement to ModalDialog to drive a
 *    Carbon Event-based modal dialog. Once called, this routine will
 *    not exit until QuitAppModalLoopForWindow is called. Calls to
 *    RunAppModalLoopForWindow can be nested, as long as each call is
 *    made on a different window. In Mac OS X 10.0.x,
 *    RunAppModalLoopForWindow will fail to re-enable the menubar
 *    before exiting if you dispose of the window during the modal loop
 *    (for example, from a Carbon event handler). You can work around
 *    this bug by retaining the window before calling
 *    RunAppModalLoopForWindow, and releasing it afterwards.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window you wish to behave modally.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
RunAppModalLoopForWindow(WindowRef inWindow);


/*
 *  QuitAppModalLoopForWindow()
 *  
 *  Discussion:
 *    This routine is used to quit a currently running call to
 *    RunAppModalLoopForWindow, i.e. it terminates a modal loop.
 *    Typically this would be called from a handler you have installed
 *    on the modal window in question when the user clicks the
 *    appropriate button, etc.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window for which to quit the modal state.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
QuitAppModalLoopForWindow(WindowRef inWindow);


/*
 *  BeginAppModalStateForWindow()
 *  
 *  Discussion:
 *    This routine is a lower level routine than
 *    RunAppModalLoopForWindow. It can be used if you wish to enter an
 *    app modal state for a window, but need to control the event loop
 *    yourself for whatever reason. In most cases, you would use
 *    RunAppModalLoopForWindow. Once you begin your app modal state,
 *    the menu bar will disable and prepare for the modal situation.
 *    The window must be visible when calling
 *    BeginAppModalStateForWindow; otherwise, windowWrongStateErr is
 *    returned. Calls to BeginAppModalStateForWindow can be nested, as
 *    long as each call is made on a different window. In Mac OS 10.0.x
 *    and CarbonLib 1.3.1, BeginAppModalStateForWindow can only be
 *    called on a window once; future calls will return an error. This
 *    bug is fixed in Mac OS 10.1 and CarbonLib 1.4.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window you wish to behave modally.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
BeginAppModalStateForWindow(WindowRef inWindow);


/*
 *  EndAppModalStateForWindow()
 *  
 *  Discussion:
 *    This routine ends an app modal state started with
 *    BeginAppModalStateForWindow.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window you wish to stop acting as app modal.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
EndAppModalStateForWindow(WindowRef inWindow);



/*--------------------------------------------------------------------------------------*/
/*  o User Focus                                                                        */
/*                                                                                      */
/* The 'user focus' is where keyboard input goes. We also use the term 'key' applied    */
/* to windows to mean this. The user focus window is normally the active non-floating   */
/* window or dialog. It is possible to make a floater get the focus, however, by calling*/
/* SetUserFocusWindow. After that call, the event model will automatically route key    */
/* input to the active keyboard focus of that window, if any. Passing kUserFocusAuto    */
/* into the window parameter tells the toolbox to pick what it considers to be the best */
/* candidate for focus. You should call this to restore focus, rather than getting the  */
/* focus, setting it to a special window, and then restoring to the saved focus. There  */
/* are cases, however, when you might want to restore to an explicit window, but the    */
/* typical usage should just be to restore to the kUserFocusAuto focus.                 */
/*                                                                                      */
/* Keep in mind that setting the focus will only last until you restore focus, or the   */
/* user starts clicking in other windows. When that happens, the toolbox will auto-     */
/* redirect the user focus to a newly selected window.                                  */
/*--------------------------------------------------------------------------------------*/
/* pick the most appropriate window for focus*/
#define kUserFocusAuto                  ((WindowRef)(-1))
/*
 *  SetUserFocusWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetUserFocusWindow(WindowRef inWindow);


/*
 *  GetUserFocusWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( WindowRef )
GetUserFocusWindow(void);



/*--------------------------------------------------------------------------------------*/
/*  o Default/Cancel buttons                                                            */
/*                                                                                      */
/* In our quest to eliminate the need for dialogs when using the new event model, we    */
/* have added the following routines which add dialog-like button control to normal     */
/* windows. With these routines, you can set the default and cancel buttons for a       */
/* window; these work just like the corresponding concepts in dialogs, and when         */
/* present, the standard toolbox handlers will handle keyboard input mapping to these   */
/* buttons. This means that pressing return or enter will 'press' the default button    */
/* and escape or command-period will 'press' the cancel button.                         */
/*--------------------------------------------------------------------------------------*/

/*
 *  SetWindowDefaultButton()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetWindowDefaultButton(
  WindowRef    inWindow,
  ControlRef   inControl);      /* can be NULL */


/*
 *  SetWindowCancelButton()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetWindowCancelButton(
  WindowRef    inWindow,
  ControlRef   inControl);      /* can be NULL */


/*
 *  GetWindowDefaultButton()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetWindowDefaultButton(
  WindowRef     inWindow,
  ControlRef *  outControl);


/*
 *  GetWindowCancelButton()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetWindowCancelButton(
  WindowRef     inWindow,
  ControlRef *  outControl);



/*--------------------------------------------------------------------------------------*/
/*  o Global HotKey API                                                                 */
/*--------------------------------------------------------------------------------------*/
struct EventHotKeyID {
  OSType              signature;
  UInt32              id;
};
typedef struct EventHotKeyID            EventHotKeyID;
typedef struct OpaqueEventHotKeyRef*    EventHotKeyRef;
/*
 *  RegisterEventHotKey()
 *  
 *  Discussion:
 *    Registers a global hot key based on the virtual key code and
 *    modifiers you pass in. Only one such combination can exist for
 *    the current application, i.e. multiple entities in the same
 *    application cannot register for the same hot key combination. The
 *    same hot key can, however, be registered by multiple
 *    applications. This means that multiple applications can
 *    potentially be notified when a particular hot key is requested.
 *    This might not necessarily be desirable, but it is how it works
 *    at present.
 *  
 *  Parameters:
 *    
 *    inHotKeyCode:
 *      The virtual key code of the key to watch
 *    
 *    inHotKeyModifiers:
 *      The keyboard modifiers to look for. There must be a modifier
 *      specified, or this function will return paramErr.
 *    
 *    inHotKeyID:
 *      The application-specified hot key ID. You will receive this in
 *      the kEventHotKeyPressed event as the direct object parameter.
 *    
 *    inTarget:
 *      The target to notify when the hot key is pressed.
 *    
 *    inOptions:
 *      Currently unused. Pass 0 or face the consequences.
 *    
 *    outRef:
 *      The EventHotKeyRef that represents your new, shiny hot key. You
 *      need this if you later wish to unregister it.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
RegisterEventHotKey(
  UInt32            inHotKeyCode,
  UInt32            inHotKeyModifiers,
  EventHotKeyID     inHotKeyID,
  EventTargetRef    inTarget,
  OptionBits        inOptions,
  EventHotKeyRef *  outRef);


/*
 *  UnregisterEventHotKey()
 *  
 *  Discussion:
 *    Unregisters a global hot key that was previously registered with
 *    the RegisterEventHotKey API. You do not need to unregister a hot
 *    key when your application terminates, the system will take care
 *    of that for you. This would be used if the user changes a hot key
 *    for something in your application - you would unregister the
 *    previous key and register your new key.
 *  
 *  Parameters:
 *    
 *    inHotKey:
 *      The EventHotKeyRef to unregister.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
UnregisterEventHotKey(EventHotKeyRef inHotKey);



/*--------------------------------------------------------------------------------------*/
/*  o MouseTrackingRegions                                                              */
/*--------------------------------------------------------------------------------------*/

/*
 *  MouseTrackingRef
 *  
 *  Discussion:
 *    A MouseTrackingRef is an object that controls the generation of
 *    mouse-enter and mouse-exit events. When the user moves the mouse
 *    into a tracking region, a kEventClassMouse/kEventMouseEntered
 *    event is sent to the app. When the user moves the mouse out of a
 *    tracking region, an event of type
 *    kEventClassMouse/kEventMouseExited is sent. Mouse tracking
 *    regions are uniquely identified within the scope of a window by a
 *    MouseTrackingRegionID which is a client signature/id pair. The
 *    client signature is the usual DTS-registered creator OSType.
 *    Mouse tracking regions can overlap, but are not exclusive. Mouse
 *    motion events are generated for each of the tracking areas
 *    intersected by the mouse. Mouse tracking regions are initially
 *    enabled. You can explicitly disable a mouse tracking area to
 *    prevent mouse-enter/exit events from being generated. Unlike
 *    global mouse-moved events, mouse-enter and mouse-exit events are
 *    generated while your app is in the background. If this is not
 *    considered desirable, disable the tracking areas while the
 *    application is in the background. MouseTrackingRefs become
 *    invalid when the window that they are bound to is disposed of.
 */
typedef struct OpaqueMouseTrackingRef*  MouseTrackingRef;


/*
 *  MouseTrackingOptions
 *  
 *  Discussion:
 *    These values define how the user's region is handled by the Mouse
 *    Tracking Region API. They define the behavior throughout the life
 *    of the Mouse Tracking Region. For example, if a region is created
 *    with the option of kMouseTrackingOptionsGlobalClip then all
 *    operations on this region will be interpreted in global
 *    coordinates and will be clipped to the owning window's structure.
 */
typedef UInt32 MouseTrackingOptions;
enum {

  /*
   * The region is expected in local coordinates and mouse movement
   * tracking is clipped to the owning window's content region.
   */
  kMouseTrackingOptionsLocalClip = 0,

  /*
   * The region is expected in global coordinates and mouse movement
   * tracking is clipped to the owning window's structure region.
   */
  kMouseTrackingOptionsGlobalClip = 1,

  /*
   * Standard options. The region will be handled in local coordinates
   * and remain clipped against the windows content region.
   */
  kMouseTrackingOptionsStandard = kMouseTrackingOptionsLocalClip
};

struct MouseTrackingRegionID {
  OSType              signature;
  SInt32              id;
};
typedef struct MouseTrackingRegionID    MouseTrackingRegionID;
/* Creation*/

/*
 *  CreateMouseTrackingRegion()
 *  
 *  Discussion:
 *    Creates a mouse tracking region and installs it into the window
 *    system.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window that will contain the created region.
 *    
 *    inRegion:
 *      The region for which you will receive entered/exit events.
 *    
 *    inClip:
 *      A region to clip inRegion against (can be NULL).
 *    
 *    inOptions:
 *      The options which define whether inRegion is given in global or
 *      local coordinates.
 *    
 *    inID:
 *      The signature/id pair which uniquely defines this region.
 *    
 *    inRefCon:
 *      Any user defined value.
 *    
 *    inTargetToNotify:
 *      The event target that should receive kEventMouseEntered/Exited
 *      events for this tracking region. If NULL, the window's event
 *      target receives these events.
 *    
 *    outTrackingRef:
 *      A reference to the newly created mouse tracking region.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
CreateMouseTrackingRegion(
  WindowRef               inWindow,
  RgnHandle               inRegion,
  RgnHandle               inClip,                 /* can be NULL */
  MouseTrackingOptions    inOptions,
  MouseTrackingRegionID   inID,
  void *                  inRefCon,
  EventTargetRef          inTargetToNotify,       /* can be NULL */
  MouseTrackingRef *      outTrackingRef);



/*
 *  RetainMouseTrackingRegion()
 *  
 *  Discussion:
 *    Retains the MouseTrackingRef.
 *  
 *  Parameters:
 *    
 *    inMouseRef:
 *      A valid MouseTrackingRef to retain.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
RetainMouseTrackingRegion(MouseTrackingRef inMouseRef);



/*
 *  ReleaseMouseTrackingRegion()
 *  
 *  Discussion:
 *    Releases the MouseTrackingRef. Since mouse tracking regions are
 *    bound to a window, they are automatically released when the
 *    window is disposed of.
 *  
 *  Parameters:
 *    
 *    inMouseRef:
 *      A valid MouseTrackingRef to release.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ReleaseMouseTrackingRegion(MouseTrackingRef inMouseRef);



/*
 *  ChangeMouseTrackingRegion()
 *  
 *  Discussion:
 *    Changes the MouseTrackingRefs region and optionally, its clip.
 *    This will not change the enabled state or options of the region.
 *  
 *  Parameters:
 *    
 *    inMouseRef:
 *      A valid MouseTrackingRef to modify.
 *    
 *    inRegion:
 *      The region to set as the mouse tracking region.
 *    
 *    inClip:
 *      An optional clip to clip inRegion against (may be NULL).
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ChangeMouseTrackingRegion(
  MouseTrackingRef   inMouseRef,
  RgnHandle          inRegion,
  RgnHandle          inClip);          /* can be NULL */



/*
 *  ClipMouseTrackingRegion()
 *  
 *  Discussion:
 *    Adjust the region to clip the MouseTrackingRef against. This can
 *    be used in the case where a window is resized or when a
 *    previously obscured region becomes exposed.
 *  
 *  Parameters:
 *    
 *    inMouseRef:
 *      A valid MouseTrackingRef to adjust.
 *    
 *    inRegion:
 *      A new region to clip inMouseRef against (can be NULL). If NULL,
 *      standard clipping will be provided.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ClipMouseTrackingRegion(
  MouseTrackingRef   inMouseRef,
  RgnHandle          inRegion);


/* Accessors*/

/*
 *  GetMouseTrackingRegionID()
 *  
 *  Discussion:
 *    Retrieves the MouseTrackingRegionID of the given
 *    MouseTrackingRef. Can be used to determine if the region belongs
 *    to your app, and if so, which region it is.
 *  
 *  Parameters:
 *    
 *    inMouseRef:
 *      A valid MouseTrackingRef from which to obtain the
 *      MouseTrackingRegionID.
 *    
 *    outID:
 *      Receives the MouseTrackingRegionID.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
GetMouseTrackingRegionID(
  MouseTrackingRef         inMouseRef,
  MouseTrackingRegionID *  outID);



/*
 *  GetMouseTrackingRegionRefCon()
 *  
 *  Discussion:
 *    Retrieves the RefCon from the given mouse tracking region.
 *  
 *  Parameters:
 *    
 *    inMouseRef:
 *      A valid MouseTrackingRef from which to obtain the refcon.
 *    
 *    outRefCon:
 *      Receives the refcon.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
GetMouseTrackingRegionRefCon(
  MouseTrackingRef   inMouseRef,
  void **            outRefCon);



/* Geometry*/

/*
 *  MoveMouseTrackingRegion()
 *  
 *  Discussion:
 *    Moves the given mouse tracking region by the specified delta. It
 *    can also optionally reclip the region, such as if the region is
 *    scrolled within a pane.
 *  
 *  Parameters:
 *    
 *    inMouseRef:
 *      A valid MouseTrackingRef to move.
 *    
 *    deltaH:
 *      The horizontal delta to move the MouseTrackingRef.
 *    
 *    deltaV:
 *      The vertical delta to move the MouseTrackingRef.
 *    
 *    inClip:
 *      A region to optionally clip against (can be NULL).
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
MoveMouseTrackingRegion(
  MouseTrackingRef   inMouseRef,
  SInt16             deltaH,
  SInt16             deltaV,
  RgnHandle          inClip);          /* can be NULL */



/* Enable/disable*/

/*
 *  SetMouseTrackingRegionEnabled()
 *  
 *  Discussion:
 *    Set the enabled state of the mouse tracking region
 *  
 *  Parameters:
 *    
 *    inMouseRef:
 *      A valid MouseTrackingRef to modify.
 *    
 *    inEnabled:
 *      Indicate whether this region should be enabled (true) or
 *      disabled (false).
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
SetMouseTrackingRegionEnabled(
  MouseTrackingRef   inMouseRef,
  Boolean            inEnabled);



/*
   
   Namespace operators
*/

/*
 *  ClipWindowMouseTrackingRegions()
 *  
 *  Discussion:
 *    Bulk clip operation to modify the region that all mouse tracking
 *    regions with the given signature will be clipped against.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window that contains the regions with the given signature
 *      that you are interested in updating.
 *    
 *    inSignature:
 *      The signature of the mouse tracking regions that will be
 *      reclipped.
 *    
 *    inClip:
 *      The region to clip all of the regions against (can be NULL). If
 *      NULL, standard clipping will be provided.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ClipWindowMouseTrackingRegions(
  WindowRef   inWindow,
  OSType      inSignature,
  RgnHandle   inClip);           /* can be NULL */



/*
 *  MoveWindowMouseTrackingRegions()
 *  
 *  Discussion:
 *    Bulk move operation to move all regions with the given signature
 *    the specified delta. An optional clip can be provided to reclip
 *    the regions against, such as in the case of the regions are being
 *    scrolled within a pane.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window that contains the regions with the given signature
 *      that you are interested in moving.
 *    
 *    inSignature:
 *      The signature of the mouse tracking regions that will be moved.
 *    
 *    deltaH:
 *      The horizontal delta to move all of the regions.
 *    
 *    deltaV:
 *      The vertical delta to move all of the regions.
 *    
 *    inClip:
 *      An optional clipping region to clip against (can be NULL). If
 *      NULL, standard clipping will be provided.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
MoveWindowMouseTrackingRegions(
  WindowRef   inWindow,
  OSType      inSignature,
  SInt16      deltaH,
  SInt16      deltaV,
  RgnHandle   inClip);           /* can be NULL */



/*
 *  SetWindowMouseTrackingRegionsEnabled()
 *  
 *  Discussion:
 *    Bulk set the enabled state of the mouse tracking regions of the
 *    given signature belonging to the given window.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window which contains the mouse tracking regions that you
 *      are interested in modifying.
 *    
 *    inSignature:
 *      The signature of the mouse tracking regions whose enabled state
 *      you wish to modify.
 *    
 *    inEnabled:
 *      Indicates whether the regions should be enabled (true) or
 *      disabled (false).
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
SetWindowMouseTrackingRegionsEnabled(
  WindowRef   inWindow,
  OSType      inSignature,
  Boolean     inEnabled);



/*
 *  ReleaseWindowMouseTrackingRegions()
 *  
 *  Discussion:
 *    Bulk release the mouse tracking regions with the given signature.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window to which the regions to be released belong.
 *    
 *    inSignature:
 *      The signature of the regions to be released.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
ReleaseWindowMouseTrackingRegions(
  WindowRef   inWindow,
  OSType      inSignature);



/* OBSOLETE CONSTANTS*/


enum {
  kMouseTrackingMousePressed    = kMouseTrackingMouseDown,
  kMouseTrackingMouseReleased   = kMouseTrackingMouseUp
};

enum {
  kEventControlGetSubviewForMouseEvent = kEventControlInterceptSubviewClick
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

#ifdef __cplusplus
}
#endif

#endif /* __CARBONEVENTS__ */

