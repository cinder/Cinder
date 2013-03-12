/*
     File:       HIView.h
 
     Contains:   HIView routines
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2001-2002 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __HIVIEW__
#define __HIVIEW__

#ifndef __CONTROLS__
#include <Controls.h>
#endif

#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
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

typedef ControlRef                      HIViewRef;
typedef ControlID                       HIViewID;
typedef ControlPartCode                 HIViewPartCode;
/*
 *  kHIViewWindowContentID
 *  
 *  Discussion:
 *    The standard view ID for the content view of a window.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
extern const HIViewID kHIViewWindowContentID;
/*
 *  kHIViewWindowGrowBoxID
 *  
 *  Discussion:
 *    The standard view ID for the grow box view of a window. Not all
 *    windows have grow boxes, so be aware that you might not find this
 *    view if you look for it.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
extern const HIViewID kHIViewWindowGrowBoxID;


/*
 *  Discussion:
 *    HIViewZOrderOp
 */
enum {

  /*
   * Indicates we wish to order a view above another view.
   */
  kHIViewZOrderAbove            = 1,

  /*
   * Indicates we wish to order a view below another view.
   */
  kHIViewZOrderBelow            = 2
};

typedef UInt32                          HIViewZOrderOp;
/*==============================================================================*/
/*  CONTROL ATTRIBUTES                                                          */
/*==============================================================================*/


/*
 *  Discussion:
 *    View Attributes
 */
enum {

  /*
   * When set, the control will send the command it generates to the
   * user focus and propagate as it would naturally from there. The
   * default is to send the command to itself and then to its parent
   * and so forth.
   */
  kHIViewSendCommandToUserFocus = (1 << 0)
};

/*==============================================================================*/
/*  HIOBJECT SUPPORT                                                            */
/*==============================================================================*/
/* The HIObject class ID for the HIView class. */
#define kHIViewClassID                  CFSTR("com.apple.hiview")
/*==============================================================================*/
/*  EMBEDDING                                                                   */
/*==============================================================================*/
/*
 *  HIViewGetRoot()
 *  
 *  Discussion:
 *    Returns the root view for a window.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window to get the root for.
 *  
 *  Result:
 *    The root view for the window, or NULL if an invalid window is
 *    passed.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIViewRef )
HIViewGetRoot(WindowRef inWindow);


/*
 *  HIViewAddSubview()
 *  
 *  Discussion:
 *    Adds a subview to the given parent. The new subview is added to
 *    the front of the list of subviews (i.e it is made topmost).
 *  
 *  Parameters:
 *    
 *    inParent:
 *      The view which will receive the new subview.
 *    
 *    inNewChild:
 *      The subview being added.
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
HIViewAddSubview(
  HIViewRef   inParent,
  HIViewRef   inNewChild);


/*
 *  HIViewRemoveFromSuperview()
 *  
 *  Discussion:
 *    Removes a view from its parent.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to remove.
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
HIViewRemoveFromSuperview(HIViewRef inView);


/*
 *  HIViewGetSuperview()
 *  
 *  Discussion:
 *    Returns a view's parent view.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view whose parent you are interested in getting.
 *  
 *  Result:
 *    An HIView reference, or NULL if this view has no parent or is
 *    invalid.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIViewRef )
HIViewGetSuperview(HIViewRef inView);


/*
 *  HIViewGetFirstSubview()
 *  
 *  Discussion:
 *    Returns the first subview of a container. The first subview is
 *    the topmost subview in z-order.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view whose subview you are fetching.
 *  
 *  Result:
 *    An HIView reference, or NULL if this view has no subviews or is
 *    invalid.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIViewRef )
HIViewGetFirstSubview(HIViewRef inView);


/*
 *  HIViewGetLastSubview()
 *  
 *  Discussion:
 *    Returns the last subview of a container. The last subview is the
 *    bottommost subview in z-order.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view whose subview you are fetching.
 *  
 *  Result:
 *    An HIView reference, or NULL if this view has no subviews or is
 *    invalid.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIViewRef )
HIViewGetLastSubview(HIViewRef inView);


/*
 *  HIViewGetNextView()
 *  
 *  Discussion:
 *    Returns the next view after the one given, in z-order.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to use as reference.
 *  
 *  Result:
 *    An HIView reference, or NULL if this view has no view behind it
 *    or is invalid.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIViewRef )
HIViewGetNextView(HIViewRef inView);


/*
 *  HIViewGetPreviousView()
 *  
 *  Discussion:
 *    Returns the previous view before the one given, in z-order.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to use as reference.
 *  
 *  Result:
 *    An HIView reference, or NULL if this view has no view in front of
 *    it or is invalid.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIViewRef )
HIViewGetPreviousView(HIViewRef inView);


/*
 *  HIViewSetZOrder()
 *  
 *  Discussion:
 *    Allows you to change the front-to-back ordering of sibling views.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view whose Z-order you wish to change.
 *    
 *    inOp:
 *      Indicates to order inView above or below inOther.
 *    
 *    inOther:
 *      Another optional view to use as a reference. You can pass NULL
 *      to mean an absolute position. For example, passing
 *      kHIViewZOrderAbove and NULL will move a view to the front of
 *      all of its siblings. Likewise, passing kHIViewZOrderBelow and
 *      NULL will move it to the back.
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
HIViewSetZOrder(
  HIViewRef        inView,
  HIViewZOrderOp   inOp,
  HIViewRef        inOther);      /* can be NULL */


/*==============================================================================*/
/*  VISIBILITY                                                                  */
/*==============================================================================*/
/*
 *  HIViewSetVisible()
 *  
 *  Discussion:
 *    Hides or shows a view. Marks the area the view will occupy or
 *    used to occupy as needing to be redrawn later.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to hide or show.
 *    
 *    inVisible:
 *      A boolean value which indicates whether you wish to hide the
 *      view (false) or show the view (true).
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
HIViewSetVisible(
  HIViewRef   inView,
  Boolean     inVisible);


/*
 *  HIViewIsVisible()
 *  
 *  Discussion:
 *    Returns whether a view is visible.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view whose visibility you wish to determine.
 *  
 *  Result:
 *    A boolean value indicating whether the view is visible (true) or
 *    hidden (false).
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIViewIsVisible(HIViewRef inView);



/*==============================================================================*/
/*  POSITIONING                                                                 */
/*==============================================================================*/

/*
 *  HIViewGetBounds()
 *  
 *  Discussion:
 *    Returns the local bounds of a view. The local bounds are the
 *    coordinate system that is completely view-relative. A view's top
 *    left coordinate starts out at 0, 0. Most operations are done in
 *    these local coordinates. Moving a view is done via the frame
 *    instead.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view whose bounds you wish to determine.
 *    
 *    outRect:
 *      The local bounds of the view.
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
HIViewGetBounds(
  HIViewRef   inView,
  HIRect *    outRect);


/*
 *  HIViewGetFrame()
 *  
 *  Discussion:
 *    Returns the frame of a view. The frame is the bounds of a view
 *    relative to its parent's local coordinate system.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view whose frame you wish to determine.
 *    
 *    outRect:
 *      The frame of the view.
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
HIViewGetFrame(
  HIViewRef   inView,
  HIRect *    outRect);


/*
 *  HIViewSetFrame()
 *  
 *  Discussion:
 *    Sets the frame of a view. This effectively moves the view within
 *    its parent. It also marks the view (and anything that was exposed
 *    behind it) to be redrawn.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view whose frame you wish to change.
 *    
 *    inRect:
 *      The new frame of the view.
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
HIViewSetFrame(
  HIViewRef       inView,
  const HIRect *  inRect);



/*
 *  HIViewMoveBy()
 *  
 *  Discussion:
 *    Moves a view by a certain distance, relative to its current
 *    location. This affects a view's frame, but not its bounds.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view you wish to move.
 *    
 *    inDX:
 *      The horizontal distance to move the view. Negative values move
 *      the view to the left, positive values to the right.
 *    
 *    inDY:
 *      The vertical distance to move the view. Negative values move
 *      the view upward, positive values downward.
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
HIViewMoveBy(
  HIViewRef   inView,
  float       inDX,
  float       inDY);


/*
 *  HIViewPlaceInSuperviewAt()
 *  
 *  Discussion:
 *    Places a view at an absolute location within its parent. This
 *    affects the view's frame, but not its bounds.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view you wish to position.
 *    
 *    inX:
 *      The absolute horizontal coordinate at which to position the
 *      view.
 *    
 *    inY:
 *      The absolute vertical coordinate at which to position the view.
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
HIViewPlaceInSuperviewAt(
  HIViewRef   inView,
  float       inX,
  float       inY);



/*
 *  HIViewReshapeStructure()
 *  
 *  Discussion:
 *    This is for use by custom views. If a view decides that its
 *    structure will change shape, it should call this. This tells the
 *    Toolbox to recalc things and invalidate as appropriate. You might
 *    use this when gaining/losing a focus ring, for example.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to reshape and invalidate.
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
HIViewReshapeStructure(HIViewRef inView);


/*==============================================================================*/
/*  HIT TESTING/EVENT HANDLING                                                  */
/*==============================================================================*/
/*
 *  HIViewGetViewForMouseEvent()
 *  
 *  Discussion:
 *    Returns the appropriate view to handle a mouse event. This is a
 *    little higher-level than GetDeep/ShallowSubviewHit. This routine
 *    will find the deepest view that should handle the mouse event,
 *    but along the way, it sends Carbon Events to each view asking it
 *    to return the appropriate subview. This allows parent views to
 *    catch clicks on their subviews. This is the recommended function
 *    to use before processing mouse events. Using one of the more
 *    primitive functions may result in an undefined behaviour.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to start from. You should pass the window's root view.
 *    
 *    inEvent:
 *      The mouse event in question.
 *    
 *    outView:
 *      The view that the mouse event should be sent to.
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
HIViewGetViewForMouseEvent(
  HIViewRef    inView,
  EventRef     inEvent,
  HIViewRef *  outView);


/*
 *  HIViewClick()
 *  
 *  Discussion:
 *    After a successful call to HIViewGetViewForMouseEvent for a mouse
 *    down event, you should call this function to have the view handle
 *    the click.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to handle the event.
 *    
 *    inEvent:
 *      The mouse event to handle.
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
HIViewClick(
  HIViewRef   inView,
  EventRef    inEvent);


/*
 *  HIViewSimulateClick()
 *  
 *  Discussion:
 *    This function is used to simulate a mouse click on a given view.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to test the part hit.
 *    
 *    inPartToClick:
 *      The part the view should consider to be clicked.
 *    
 *    inModifiers:
 *      The modifiers the view can consider for its click action.
 *    
 *    outPartClicked:
 *      The part that was hit, can be kControlNoPart if no action
 *      occurred.
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
HIViewSimulateClick(
  HIViewRef          inView,
  HIViewPartCode     inPartToClick,
  UInt32             inModifiers,
  ControlPartCode *  outPartClicked);


/*
 *  HIViewGetPartHit()
 *  
 *  Discussion:
 *    Given a view, and a view-relative point, this function returns
 *    the part code hit as determined by the view.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to test the part hit.
 *    
 *    inPoint:
 *      The view-relative point to use.
 *    
 *    outPart:
 *      The part hit by inPoint.
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
HIViewGetPartHit(
  HIViewRef         inView,
  const HIPoint *   inPoint,
  HIViewPartCode *  outPart);


/*
 *  HIViewGetSubviewHit()
 *  
 *  Discussion:
 *    Returns the child of the given view hit by the point passed in.
 *    This is more primitive than using HIViewGetViewForMouseEvent, and
 *    should be used only in non-event-handling cases.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view you wish to position.
 *    
 *    inPoint:
 *      The mouse coordinate to use. This is passed in the local
 *      coordinate system of inView.
 *    
 *    inDeep:
 *      Pass true to find the deepest child hit, false to go only one
 *      level deep (just check direct children of inView).
 *    
 *    outView:
 *      The view hit by inPoint, or NULL if no subview was hit.
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
HIViewGetSubviewHit(
  HIViewRef        inView,
  const HIPoint *  inPoint,
  Boolean          inDeep,
  HIViewRef *      outView);


/*==============================================================================*/
/*  DISPLAY                                                                     */
/*==============================================================================*/
/*
 *  HIViewGetNeedsDisplay()
 *  
 *  Discussion:
 *    Returns true if the view passed in or any subview of it requires
 *    redrawing (i.e. part of it has been invalidated).
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to inspect.
 *  
 *  Result:
 *    A boolean result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIViewGetNeedsDisplay(HIViewRef inView);


/*
 *  HIViewSetNeedsDisplay()
 *  
 *  Discussion:
 *    Marks a view as needing to be completely redrawn, or completely
 *    valid.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to mark dirty.
 *    
 *    inNeedsDisplay:
 *      A boolean which indicates whether inView needs to be redrawn or
 *      not.
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
HIViewSetNeedsDisplay(
  HIViewRef   inView,
  Boolean     inNeedsDisplay);



/*
 *  HIViewSetNeedsDisplayInRegion()
 *  
 *  Discussion:
 *    Marks a portion of a view as needing to be redrawn, or valid.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to mark dirty.
 *    
 *    inRgn:
 *      The region to mark dirty or clean.
 *    
 *    inNeedsDisplay:
 *      A boolean which indicates whether or not inRgn should be added
 *      to the display region or removed from it.
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
HIViewSetNeedsDisplayInRegion(
  HIViewRef   inView,
  RgnHandle   inRgn,
  Boolean     inNeedsDisplay);


/*
 *  HIViewFlashDirtyArea()
 *  
 *  Discussion:
 *    Debugging aid. Flashes the region which would be redrawn at the
 *    next draw time for an entire window.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window to flash the dirty region for.
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
HIViewFlashDirtyArea(WindowRef inWindow);


/*
 *  HIViewGetSizeConstraints()
 *  
 *  Discussion:
 *    Return the minimum and maximum size for a control. A control must
 *    respond to this protocol to get meaningful results. These sizes
 *    can be used to help autoposition subviews, for example.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to inspect.
 *    
 *    outMinSize:
 *      The minimum size the view can be.
 *    
 *    outMaxSize:
 *      The maximum size the view can be.
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
HIViewGetSizeConstraints(
  HIViewRef   inView,
  HISize *    outMinSize,
  HISize *    outMaxSize);




/*==============================================================================*/
/*  COORDINATE SYSTEM CONVERSION                                                */
/*==============================================================================*/
/*
 *  HIViewConvertPoint()
 *  
 *  Discussion:
 *    Converts a point from one view to another. Both views must have a
 *    common ancestor, i.e. they must both be in the same window.
 *  
 *  Parameters:
 *    
 *    ioPoint:
 *      The point to convert.
 *    
 *    inSourceView:
 *      The view whose coordinate system ioPoint is starting out in.
 *      You can pass NULL to indicate that ioPoint is a window-relative
 *      point.
 *    
 *    inDestView:
 *      The view whose coordinate system ioPoint should end up in. You
 *      can pass NULL to indicate that ioPoint is a window-relative
 *      point.
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
HIViewConvertPoint(
  HIPoint *   ioPoint,
  HIViewRef   inSourceView,
  HIViewRef   inDestView);


/*
 *  HIViewConvertRect()
 *  
 *  Discussion:
 *    Converts a rectangle from one view to another. Both views must
 *    have a common ancestor, i.e. they must both be in the same window.
 *  
 *  Parameters:
 *    
 *    ioRect:
 *      The rectangle to convert.
 *    
 *    inSourceView:
 *      The view whose coordinate system ioRect is starting out in. You
 *      can pass NULL to indicate that ioRect is a window-relative
 *      rectangle.
 *    
 *    inDestView:
 *      The view whose coordinate system ioRect should end up in. You
 *      can pass NULL to indicate that ioRect is a window-relative
 *      rectangle.
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
HIViewConvertRect(
  HIRect *    ioRect,
  HIViewRef   inSourceView,
  HIViewRef   inDestView);


/*
 *  HIViewConvertRegion()
 *  
 *  Discussion:
 *    Converts a region from one view to another. Both views must have
 *    a common ancestor, i.e. they must both be in the same window.
 *  
 *  Parameters:
 *    
 *    ioRgn:
 *      The region to convert.
 *    
 *    inSourceView:
 *      The view whose coordinate system ioRgn is starting out in. You
 *      can pass NULL to indicate that ioRgn is a window-relative
 *      region.
 *    
 *    inDestView:
 *      The view whose coordinate system ioRgn should end up in. You
 *      can pass NULL to indicate that ioRgn is a window-relative
 *      region.
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
HIViewConvertRegion(
  RgnHandle   ioRgn,
  HIViewRef   inSourceView,
  HIViewRef   inDestView);



/*
 *  HIViewSetDrawingEnabled()
 *  
 *  Discussion:
 *    Turns control drawing on or off. You can use this to ensure that
 *    no drawing occurs wrt to the control. Even Draw1Control will not
 *    draw! HIViewSetNeedsDisplay is also rendered useless when drawing
 *    is off.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to enable or disable drawing for.
 *    
 *    inEnabled:
 *      A boolean value indicating whether drawing should be on (true)
 *      or off (false).
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
HIViewSetDrawingEnabled(
  HIViewRef   inView,
  Boolean     inEnabled);


/*
 *  HIViewIsDrawingEnabled()
 *  
 *  Discussion:
 *    Determines if drawing is currently enabled for a control.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to get the drawing state for.
 *  
 *  Result:
 *    A boolean value indicating whether drawing is on (true) or off
 *    (false).
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIViewIsDrawingEnabled(HIViewRef inView);



/*
 *  HIViewScrollRect()
 *  
 *  Discussion:
 *    Scrolls a view's contents, or a portion thereof. This will
 *    actually blit the contents of the view as appropriate to scroll,
 *    and then invalidate those portions which need to be redrawn. Be
 *    warned that this is a raw bit scroll. Anything that might overlap
 *    the target view will get thrashed as well.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to scroll.
 *    
 *    inRect:
 *      The rect to scroll. Pass NULL to mean the entire view. The rect
 *      passed cannot be bigger than the view's bounds. It must be in
 *      the local coordinate system of the view.
 *    
 *    inDX:
 *      The horizontal distance to scroll. Positive values shift to the
 *      right, negative values shift to the left.
 *    
 *    inDY:
 *      The vertical distance to scroll. Positive values shift
 *      downward, negative values shift upward.
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
HIViewScrollRect(
  HIViewRef       inView,
  const HIRect *  inRect,       /* can be NULL */
  float           inDX,
  float           inDY);


/*
 *  HIViewSetBoundsOrigin()
 *  
 *  Discussion:
 *    This API sets the origin of the view. This effectively also moves
 *    all subcontrols of a view as well. This call will invalidate the
 *    view.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view whose origin you wish to adjust.
 *    
 *    inX:
 *      The X coordinate.
 *    
 *    inY:
 *      The Y coordinate.
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
HIViewSetBoundsOrigin(
  HIViewRef   inView,
  float       inX,
  float       inY);



/*==============================================================================*/
/*  KEYBOARD FOCUS                                                              */
/*==============================================================================*/
/*
 *  HIViewAdvanceFocus()
 *  
 *  Discussion:
 *    Advances the focus to the next most appropriate view. Unless
 *    overriden in some fashion (either by overriding certain carbon
 *    events or using the HIViewSetNextFocus API), the Toolbox will use
 *    a spacially determinant method of focusing, attempting to focus
 *    left to right, top to bottom in a window, taking groups of
 *    controls into account.
 *  
 *  Parameters:
 *    
 *    inRootForFocus:
 *      The subtree to manipulate. The focus will never leave
 *      inRootToFocus. Typically you would pass the content of the
 *      window, or the root. If focused on the toolbar, for example,
 *      the focus is limited to the toolbar only. In this case, the
 *      Toolbox passes the toolbar view in as the focus root for
 *      example.
 *    
 *    inModifiers:
 *      The EventModifiers of the keyboard event that ultimately caused
 *      the call to HIViewAdvanceFocus. These modifiers are used to
 *      determine the focus direction as well as other alternate
 *      focusing behaviors.
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
HIViewAdvanceFocus(
  HIViewRef        inRootForFocus,
  EventModifiers   inModifiers);


/*
 *  HIViewGetFocusPart()
 *  
 *  Discussion:
 *    Returns the currently focused part of the given view.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to inquire about.
 *    
 *    outFocusPart:
 *      The part currently focused.
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
HIViewGetFocusPart(
  HIViewRef         inView,
  HIViewPartCode *  outFocusPart);


/*
 *  HIViewSubtreeContainsFocus()
 *  
 *  Discussion:
 *    Given a view, this function checks to see if it or any of its
 *    children currently are the keyboard focus. If so, true is
 *    returned as the function result.
 *  
 *  Parameters:
 *    
 *    inSubtreeStart:
 *      The view to start searching at.
 *  
 *  Result:
 *    A boolean result.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIViewSubtreeContainsFocus(HIViewRef inSubtreeStart);


/*
 *  HIViewSetNextFocus()
 *  
 *  Discussion:
 *    This function hard-wires the next view to shift focus to whenever
 *    the keyboard focus is advanced.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to set the next focus view for.
 *    
 *    inNextFocus:
 *      The view to set focus to next. Pass NULL to tell the view
 *      system to use the default rules.
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
HIViewSetNextFocus(
  HIViewRef   inView,
  HIViewRef   inNextFocus);      /* can be NULL */


/*
 *  HIViewSetFirstSubViewFocus()
 *  
 *  Discussion:
 *    This function hard-wires the first subview to shift focus to
 *    whenever the keyboard focus is advanced and the container view is
 *    entered.
 *  
 *  Parameters:
 *    
 *    inParent:
 *      The parent view.
 *    
 *    inSubView:
 *      The first child which should receive focus. Pass NULL to tell
 *      the view system to use the default rules.
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
HIViewSetFirstSubViewFocus(
  HIViewRef   inParent,
  HIViewRef   inSubView);      /* can be NULL */



/*==============================================================================*/
/*  MISCELLANEOUS                                                               */
/*==============================================================================*/
/*
 *  HIViewFindByID()
 *  
 *  Discussion:
 *    Allows you to find a particular view by its ID. Currently, this
 *    call uses the ControlID type as its IDs.
 *  
 *  Parameters:
 *    
 *    inStartView:
 *      The view to start searching at.
 *    
 *    inID:
 *      The ID of the view you are looking for.
 *    
 *    outControl:
 *      Receives the control if found.
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
HIViewFindByID(
  HIViewRef    inStartView,
  HIViewID     inID,
  HIViewRef *  outControl);



/*
 *  HIViewGetAttributes()
 *  
 *  Discussion:
 *    Allows you to get the attributes of a view.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to inspect.
 *    
 *    outAttrs:
 *      The attributes of the view.
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
HIViewGetAttributes(
  HIViewRef     inView,
  OptionBits *  outAttrs);



/*
 *  HIViewChangeAttributes()
 *  
 *  Discussion:
 *    Allows you to change the attributes of a view. You can
 *    simultaneously set and clear attributes.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to muck with.
 *    
 *    inAttrsToSet:
 *      The attributes you wish to set.
 *    
 *    inAttrsToClear:
 *      The attributes you wish to clear.
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
HIViewChangeAttributes(
  HIViewRef    inView,
  OptionBits   inAttrsToSet,
  OptionBits   inAttrsToClear);



/*
 *  HIViewCreateOffscreenImage()
 *  
 *  Discussion:
 *    Creates an CGImageRef for the view passed in. The view and any
 *    children it has are rendered in the resultant image.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view you wish to create an image of.
 *    
 *    inOptions:
 *      Options. Currently you must pass 0.
 *    
 *    outFrame:
 *      The frame of the view within the resultant image. It is in the
 *      coordinate system of the image, where 0,0 is the top left
 *      corner of the image. This is so you can know exactly where the
 *      control lives in the image when the control draws outside its
 *      bounds for things such as shadows.
 *    
 *    outImage:
 *      The image of the view, including anything that would be drawn
 *      outside the view's frame.
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
HIViewCreateOffscreenImage(
  HIViewRef     inView,
  OptionBits    inOptions,
  HIRect *      outFrame,        /* can be NULL */
  CGImageRef *  outImage);



/*
 *  HIViewDrawCGImage()
 *  
 *  Discussion:
 *    Draws an image in the right direction for an HIView. This is
 *    functionally the same as CGContextDrawImage, but it flips the
 *    context appropriately so that the image is drawn correctly.
 *    Because HIViews have their origin at the top, left, you are
 *    really drawing upside-down, so if you were to use the CG image
 *    drawing, you'd see what I mean! This call attempts to insulate
 *    you from that fact.
 *  
 *  Parameters:
 *    
 *    inContext:
 *      The context to draw in.
 *    
 *    inBounds:
 *      The bounds to draw the image into.
 *    
 *    inImage:
 *      The image to draw.
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
HIViewDrawCGImage(
  CGContextRef    inContext,
  const HIRect *  inBounds,
  CGImageRef      inImage);



/*==============================================================================*/
/*  HIGrowBoxView                                                               */
/*==============================================================================*/


/* The HIObject class ID for the HIGrowBoxView class. */
#define kHIGrowBoxViewClassID           CFSTR("com.apple.higrowboxview")


/* Currently there is no direct creation API for the grow box, so you must use  */
/* HIObjectCreate if you wish to create one directly. Normally, a window will   */
/* create one for you, so you should generally never need to do this.           */


/*
 *  HIGrowBoxViewSetTransparent()
 *  
 *  Discussion:
 *    Sets a grow box view as transparent, meaning it will draw the
 *    grow box lines over any content below it. When not transparent,
 *    it's an opaque white square with the grow lines.
 *  
 *  Parameters:
 *    
 *    inGrowBoxView:
 *      The grow box view reference.
 *    
 *    inTransparent:
 *      Pass true to make the grow view use its transparent look, false
 *      to give it the opaque look.
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
HIGrowBoxViewSetTransparent(
  HIViewRef   inGrowBoxView,
  Boolean     inTransparent);


/*
 *  HIGrowBoxViewIsTransparent()
 *  
 *  Discussion:
 *    Returns true if a grow box view is set to be transparent.
 *  
 *  Parameters:
 *    
 *    inGrowBoxView:
 *      The grow box view reference.
 *  
 *  Result:
 *    A boolean result.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIGrowBoxViewIsTransparent(HIViewRef inGrowBoxView);



/*==============================================================================*/
/*  HIScrollView                                                                */
/*==============================================================================*/
enum {
  kEventClassScrollable         = FOUR_CHAR_CODE('scrl')
};

enum {
  kEventScrollableGetInfo       = 1,
  kEventScrollableInfoChanged   = 2,
  kEventScrollableScrollTo      = 10
};

enum {
  kEventParamImageSize          = FOUR_CHAR_CODE('imsz'), /* type HISize*/
  kEventParamViewSize           = FOUR_CHAR_CODE('vwsz'), /* type HISize*/
  kEventParamLineSize           = FOUR_CHAR_CODE('lnsz'), /* type HISize*/
  kEventParamOrigin             = FOUR_CHAR_CODE('orgn') /* type HIPoint*/
};


/*
 *  Discussion:
 *    HIScrollView options
 */
enum {

  /*
   * This indicates that a vertical scroll bar is desired.
   */
  kHIScrollViewOptionsVertScroll = (1 << 0),

  /*
   * This indicates that a horizontal scroll bar is desired.
   */
  kHIScrollViewOptionsHorizScroll = (1 << 1),

  /*
   * This indicates that space for a grow box should be taken into
   * account when layout out scroll bars. If both the horizontal and
   * vertical scroll bars are requested, this attribute is assumed.
   */
  kHIScrollViewOptionsAllowGrow = (1 << 2),
  kHIScrollViewValidOptions     = (kHIScrollViewOptionsVertScroll | kHIScrollViewOptionsHorizScroll | kHIScrollViewOptionsAllowGrow)
};

/*
 *  HIScrollViewCreate()
 *  
 *  Discussion:
 *    Creates a scroll view. This view has 3 parts, essentially. It can
 *    have one or two scroll bars (horizontal/vertical), and a view to
 *    be scrolled. The view to be scrolled is merely added via
 *    HIViewAddSubview. The scroll view will automatically connect it
 *    up appropriately. **** THIS MAY CHANGE
 *  
 *  Parameters:
 *    
 *    inOptions:
 *      Options for our scroll view. You must specify either a
 *      horizontal or a vertical scroll bar. If neither is passed, an
 *      error is returned.
 *    
 *    outView:
 *      The new scroll view.
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
HIScrollViewCreate(
  OptionBits   inOptions,
  HIViewRef *  outView);



/*
 *  HIScrollViewSetScrollBarAutoHide()
 *  
 *  Discussion:
 *    Sets a scroll view's scroll bars to auto-hide when the entire
 *    scrollable view it is managing can be fully displayed in its
 *    bounds. This is similar to the behavior you see in the Preview
 *    application.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to affect.
 *    
 *    inAutoHide:
 *      The new auto-hide setting (true == auto-hide).
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
HIScrollViewSetScrollBarAutoHide(
  HIViewRef   inView,
  Boolean     inAutoHide);


/*
 *  HIScrollViewGetScrollBarAutoHide()
 *  
 *  Discussion:
 *    Gets a scroll view's current scroll bar auto-hide setting.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The view to examine.
 *  
 *  Result:
 *    A boolean result.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIScrollViewGetScrollBarAutoHide(HIViewRef inView);


/*==============================================================================*/
/*  HIImageView                                                                 */
/*==============================================================================*/

/*
 *  HIImageViewCreate()
 *  
 *  Discussion:
 *    Creates an image view. The view responds to the scrollable
 *    interface and can be used in a scrolling view. You can pass an
 *    image initially, or set one later.
 *  
 *  Parameters:
 *    
 *    inImage:
 *      An initial image, or NULL. You can set the image later via
 *      SetControlData.
 *    
 *    outControl:
 *      The new image view.
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
HIImageViewCreate(
  CGImageRef    inImage,          /* can be NULL */
  ControlRef *  outControl);


enum {
  kHIImageViewImageTag          = FOUR_CHAR_CODE('imag') /* CGImageRef (THIS TAG IS GOING AWAY!!! USE THE APIS BELOW!)*/
};

/*
 *  HIImageViewSetOpaque()
 *  
 *  Discussion:
 *    Allows you to set whether an image view should be treated as
 *    opaque. If this is set to true, the image view can make certain
 *    optimizations for compositing and scrolling. The alpha-related
 *    image view APIs are rendered useless if opacity it set to true.
 *    An image view, when created, is opaque by default. You must pass
 *    false to this function in order to change the alpha, etc. or if
 *    your image does not fill the full bounds of the view.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The image view to affect.
 *    
 *    inOpaque:
 *      The new opacity setting. Pass true to indicate you want the
 *      image to be treated as opaque.
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
HIImageViewSetOpaque(
  HIViewRef   inView,
  Boolean     inOpaque);


/*
 *  HIImageViewIsOpaque()
 *  
 *  Discussion:
 *    Allows you to determine whether an image view is opaque or not.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The image view to query.
 *  
 *  Result:
 *    A boolean result, where true indicates the image view is opaque.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIImageViewIsOpaque(HIViewRef inView);


/*
 *  HIImageViewSetAlpha()
 *  
 *  Discussion:
 *    Allows you to set the alpha for an image, making it more or less
 *    transparent. An alpha of 1.0 is fully opaque, and 0.0 is fully
 *    transparent. The default alpha for an image is 1.0.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The image view to affect.
 *    
 *    inAlpha:
 *      The new alpha value.
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
HIImageViewSetAlpha(
  HIViewRef   inView,
  float       inAlpha);


/*
 *  HIImageViewGetAlpha()
 *  
 *  Discussion:
 *    Allows you to get the alpha for an image. An alpha of 1.0 is
 *    fully opaque, and 0.0 is fully transparent.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The image view to query.
 *  
 *  Result:
 *    A floating point number representing the alpha from 0.0 through
 *    1.0.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( float )
HIImageViewGetAlpha(HIViewRef inView);


/*
 *  HIImageViewSetScaleToFit()
 *  
 *  Discussion:
 *    Normally an image view will clip to the view's bounds. Using this
 *    API, you can instead tell the image view to size the image to fit
 *    into the view bounds specified.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The image view to affect.
 *    
 *    inScaleToFit:
 *      A boolean indicating whether the image should be scaled to fit
 *      the view bounds (true) or merely clip to the view bounds
 *      (false).
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
HIImageViewSetScaleToFit(
  HIViewRef   inView,
  Boolean     inScaleToFit);


/*
 *  HIImageViewGetScaleToFit()
 *  
 *  Discussion:
 *    Returns whether or not an image view will scale the image it
 *    displays to the view bounds or merely clip to the view bounds. A
 *    true result means it scales.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The image view to query.
 *  
 *  Result:
 *    A boolean result.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIImageViewGetScaleToFit(HIViewRef inView);


/*
 *  HIImageViewSetImage()
 *  
 *  Discussion:
 *    Sets the image to display in an image view. The image passed in
 *    is retained by the view, so you may release the image after
 *    calling this API if you no longer need to reference it.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The image view to affect.
 *    
 *    inImage:
 *      The image to set.
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
HIImageViewSetImage(
  HIViewRef    inView,
  CGImageRef   inImage);      /* can be NULL */


/*
 *  HIImageViewCopyImage()
 *  
 *  Discussion:
 *    Gets the image for an image view. If there is no image set on the
 *    view, or the view ref is invalid, NULL is returned. The image is
 *    retained, so you should take care to release it when you are
 *    finished with it.
 *  
 *  Parameters:
 *    
 *    inView:
 *      The image view to query.
 *  
 *  Result:
 *    A CoreGraphics (Quartz) image ref.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( CGImageRef )
HIImageViewCopyImage(HIViewRef inView);


/*==============================================================================*/
/* HIComboBox                                                                   */
/*==============================================================================*/

/*
 *  Summary:
 *    ComboBox attributes
 */
enum {

  /*
   * A constant with value zero; the lack of any attributes.
   */
  kHIComboBoxNoAttributes       = 0L,

  /*
   * The control will attempt to auto complete the text the user is
   * typing with an item in the ComboBox list that is the closest
   * appropriate match.
   */
  kHIComboBoxAutoCompletionAttribute = (1L << 0),

  /*
   * The control will disclose the ComboBox list after the user enters
   * text.
   */
  kHIComboBoxAutoDisclosureAttribute = (1L << 1),

  /*
   * The items in the ComboBox list will be automatically sorted in
   * alphabetical order.
   */
  kHIComboBoxAutoSortAttribute  = (1L << 2),

  /*
   * The ComboBox list will be automatically sized to fit the Human
   * Interface Guidelines.
   */
  kHIComboBoxAutoSizeListAttribute = (1L << 3),

  /*
   * The minimum set of ComboBox attributes commonly used.
   */
  kHIComboBoxStandardAttributes = (kHIComboBoxAutoCompletionAttribute | kHIComboBoxAutoDisclosureAttribute | kHIComboBoxAutoSizeListAttribute)
};

/* ControlKind*/
enum {
  kControlKindHIComboBox        = FOUR_CHAR_CODE('cbbx')
};

/* ComboBox Part codes*/
enum {
  kHIComboBoxEditTextPart       = 5,
  kHIComboBoxDisclosurePart     = 28
};


/*
 *  Discussion:
 *    ComboBox ControlData tags available with Mac OS X 10.2 and later.
 */
enum {

  /*
   * Extract the contents of the ComboBox list as a CFArray. The
   * CFArray will be retained: if you get the array, you own it and
   * will be required to release it; if you set it the toolbox makes a
   * copy of it and you are free to release your reference.
   */
  kHIComboBoxListTag            = FOUR_CHAR_CODE('cbls'), /* CFArrayRef; bumps the refCount on get/retains on set*/

  /*
   * The width of the ComboBox list. This can be customized. This
   * disables the autosize attribute.
   */
  kHIComboBoxListPixelWidthTag  = FOUR_CHAR_CODE('cblw'), /* UInt32 */

  /*
   * The height of the ComboBox list. This can be customized. This
   * disables the autosize attribute.
   */
  kHIComboBoxListPixelHeightTag = FOUR_CHAR_CODE('cblh'), /* UInt32*/

  /*
   * The number of visible items in the list. This can be customized.
   * This disables the autosize attribute.
   */
  kHIComboBoxNumVisibleItemsTag = FOUR_CHAR_CODE('cbni') /* UInt32*/
};

/*
 *  HIComboBoxCreate()
 *  
 *  Summary:
 *    Creates a combo box control.
 *  
 *  Parameters:
 *    
 *    boundsRect:
 *      The bounding box of the control.
 *    
 *    text:
 *      The default text in the editable portion of the control. Can be
 *      NULL.
 *    
 *    style:
 *      The font style of the both editable text and the text in the
 *      disclosure list. Can be NULL.
 *    
 *    list:
 *      The default values available in the disclosure list. Can be
 *      NULL.
 *    
 *    inAttributes:
 *      The default attributes of the combo box.
 *    
 *    outComboBox:
 *      On exit, contains the new control.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIComboBoxCreate(
  const HIRect *               boundsRect,
  CFStringRef                  text,               /* can be NULL */
  const ControlFontStyleRec *  style,              /* can be NULL */
  CFArrayRef                   list,               /* can be NULL */
  OptionBits                   inAttributes,
  HIViewRef *                  outComboBox);


/*
 *  HIComboBoxGetItemCount()
 *  
 *  Summary:
 *    Get the number of items in the combo box disclosure list.
 *  
 *  Parameters:
 *    
 *    inComboBox:
 *      The combo box.
 *  
 *  Result:
 *    The number of items in the combo box disclosure list.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( ItemCount )
HIComboBoxGetItemCount(HIViewRef inComboBox);


/*
 *  HIComboBoxInsertTextItemAtIndex()
 *  
 *  Summary:
 *    Inserts a CFString in the disclosure list
 *  
 *  Parameters:
 *    
 *    inComboBox:
 *      The combo box whose disclosure list the text will be inserted
 *      in.
 *    
 *    inIndex:
 *      The index that the text should be inserted in. If the index
 *      does not fall within the number of items in the combo box list,
 *      it will be appended to the end of the list.
 *    
 *    inText:
 *      The text item to be inserted in the combo box disclosure list.
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
HIComboBoxInsertTextItemAtIndex(
  HIViewRef     inComboBox,
  CFIndex       inIndex,
  CFStringRef   inText);


/*
 *  HIComboBoxAppendTextItem()
 *  
 *  Summary:
 *    Appends a text item to the combo box disclosure list.
 *  
 *  Parameters:
 *    
 *    inComboBox:
 *      The combo box whose disclosure list the text will be appended
 *      to.
 *    
 *    inText:
 *      The text item to be appended to the combo box disclosure list.
 *    
 *    outIndex:
 *      On exit, the index of the new item. Can be NULL if the caller
 *      does not require this information.
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
HIComboBoxAppendTextItem(
  HIViewRef     inComboBox,
  CFStringRef   inText,
  CFIndex *     outIndex);        /* can be NULL */


/*
 *  HIComboBoxCopyTextItemAtIndex()
 *  
 *  Summary:
 *    Copy the text from the combo box disclosure list
 *  
 *  Parameters:
 *    
 *    inComboBox:
 *      The combo box that contains the text item you would like to
 *      copy.
 *    
 *    inIndex:
 *      The index of the text item. Will return paramErr if the index
 *      is out of bounds of the combo box list.
 *    
 *    outString:
 *      A copy of the string at the given index. Remember this is now
 *      your copy that you will need to release.
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
HIComboBoxCopyTextItemAtIndex(
  HIViewRef      inComboBox,
  CFIndex        inIndex,
  CFStringRef *  outString);


/*
 *  HIComboBoxRemoveItemAtIndex()
 *  
 *  Summary:
 *    Remove an item from a combo box disclosure list.
 *  
 *  Parameters:
 *    
 *    inComboBox:
 *      The combo box that contains the disclosure list that you would
 *      like to remove an item from.
 *    
 *    inIndex:
 *      The index of the item to remove.
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
HIComboBoxRemoveItemAtIndex(
  HIViewRef   inComboBox,
  CFIndex     inIndex);


/*
 *  HIComboBoxChangeAttributes()
 *  
 *  Summary:
 *    Change the attributes of a combo box
 *  
 *  Parameters:
 *    
 *    inComboBox:
 *      The combo box whose attributes you would like to change.
 *    
 *    inAttributesToSet:
 *      The attributes to set.
 *    
 *    inAttributesToClear:
 *      The attributes to clear.
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
HIComboBoxChangeAttributes(
  HIViewRef    inComboBox,
  OptionBits   inAttributesToSet,
  OptionBits   inAttributesToClear);


/*
 *  HIComboBoxGetAttributes()
 *  
 *  Summary:
 *    Get the attributes of a combo box.
 *  
 *  Parameters:
 *    
 *    inComboBox:
 *      The combo box whose attributes you would like to obtain.
 *    
 *    outAttributes:
 *      The attributes of the combo box.
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
HIComboBoxGetAttributes(
  HIViewRef     inComboBox,
  OptionBits *  outAttributes);



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

#endif /* __HIVIEW__ */

