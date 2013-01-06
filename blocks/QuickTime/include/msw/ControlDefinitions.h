/*
     File:       ControlDefinitions.h
 
     Contains:   Definitions of controls provided by the Control Manager
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __CONTROLDEFINITIONS__
#define __CONTROLDEFINITIONS__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __APPEARANCE__
#include <Appearance.h>
#endif

#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
#endif

#ifndef __CONTROLS__
#include <Controls.h>
#endif

#ifndef __LISTS__
#include <Lists.h>
#endif

#ifndef __MACHELP__
#include <MacHelp.h>
#endif

#ifndef __MENUS__
#include <Menus.h>
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
 *  ControlDefinitions.h
 *  
 *  Discussion:
 *    System software supplies a variety of controls for your
 *    applications to use. They are described herein.
 */
/*------------------------------------------------------------------------------------------------------*/
/*  o Resource Types                                                                                    */
/*------------------------------------------------------------------------------------------------------*/

enum {
  kControlTabListResType        = FOUR_CHAR_CODE('tab#'), /* used for tab control (Appearance 1.0 and later)*/
  kControlListDescResType       = FOUR_CHAR_CODE('ldes') /* used for list box control (Appearance 1.0 and later)*/
};

/*--------------------------------------------------------------------------------------*/
/*  o Check Box Values                                                                  */
/*--------------------------------------------------------------------------------------*/
enum {
  kControlCheckBoxUncheckedValue = 0,
  kControlCheckBoxCheckedValue  = 1,
  kControlCheckBoxMixedValue    = 2
};

/*--------------------------------------------------------------------------------------*/
/*  o Radio Button Values                                                               */
/*--------------------------------------------------------------------------------------*/
enum {
  kControlRadioButtonUncheckedValue = 0,
  kControlRadioButtonCheckedValue = 1,
  kControlRadioButtonMixedValue = 2
};

/*--------------------------------------------------------------------------------------*/
/*  o Pop-Up Menu Control Constants                                                     */
/*--------------------------------------------------------------------------------------*/
/* Variant codes for the System 7 pop-up menu*/
enum {
  popupFixedWidth               = 1 << 0,
  popupVariableWidth            = 1 << 1,
  popupUseAddResMenu            = 1 << 2,
  popupUseWFont                 = 1 << 3
};

/* Menu label styles for the System 7 pop-up menu*/
enum {
  popupTitleBold                = 1 << 8,
  popupTitleItalic              = 1 << 9,
  popupTitleUnderline           = 1 << 10,
  popupTitleOutline             = 1 << 11,
  popupTitleShadow              = 1 << 12,
  popupTitleCondense            = 1 << 13,
  popupTitleExtend              = 1 << 14,
  popupTitleNoStyle             = 1 << 15
};

/* Menu label justifications for the System 7 pop-up menu*/
enum {
  popupTitleLeftJust            = 0x00000000,
  popupTitleCenterJust          = 0x00000001,
  popupTitleRightJust           = 0x000000FF
};

/*------------------------------------------------------------------------------------------------------*/
/*  o PopUp Menu Private Data Structure                                                                 */
/*------------------------------------------------------------------------------------------------------*/
#if !OPAQUE_TOOLBOX_STRUCTS
struct PopupPrivateData {
  MenuRef             mHandle;
  SInt16              mID;
};
typedef struct PopupPrivateData         PopupPrivateData;
typedef PopupPrivateData *              PopupPrivateDataPtr;
typedef PopupPrivateDataPtr *           PopupPrivateDataHandle;
#endif  /* !OPAQUE_TOOLBOX_STRUCTS */

/*------------------------------------------------------------------------------------------------------*/
/*  o Control Definition ID's                                                                           */
/*------------------------------------------------------------------------------------------------------*/
/* Standard System 7 procIDs*/

enum {
  pushButProc                   = 0,
  checkBoxProc                  = 1,
  radioButProc                  = 2,
  scrollBarProc                 = 16,
  popupMenuProc                 = 1008
};

/*--------------------------------------------------------------------------------------*/
/*  o Control Part Codes                                                                */
/*--------------------------------------------------------------------------------------*/
enum {
  kControlLabelPart             = 1,
  kControlMenuPart              = 2,
  kControlTrianglePart          = 4,
  kControlEditTextPart          = 5,    /* Appearance 1.0 and later*/
  kControlPicturePart           = 6,    /* Appearance 1.0 and later*/
  kControlIconPart              = 7,    /* Appearance 1.0 and later*/
  kControlClockPart             = 8,    /* Appearance 1.0 and later*/
  kControlListBoxPart           = 24,   /* Appearance 1.0 and later*/
  kControlListBoxDoubleClickPart = 25,  /* Appearance 1.0 and later*/
  kControlImageWellPart         = 26,   /* Appearance 1.0 and later*/
  kControlRadioGroupPart        = 27,   /* Appearance 1.0.2 and later*/
  kControlButtonPart            = 10,
  kControlCheckBoxPart          = 11,
  kControlRadioButtonPart       = 11,
  kControlUpButtonPart          = 20,
  kControlDownButtonPart        = 21,
  kControlPageUpPart            = 22,
  kControlPageDownPart          = 23,
  kControlClockHourDayPart      = 9,    /* Appearance 1.1 and later*/
  kControlClockMinuteMonthPart  = 10,   /* Appearance 1.1 and later*/
  kControlClockSecondYearPart   = 11,   /* Appearance 1.1 and later*/
  kControlClockAMPMPart         = 12,   /* Appearance 1.1 and later*/
  kControlDataBrowserPart       = 24,   /* CarbonLib 1.0 and later*/
  kControlDataBrowserDraggedPart = 25   /* CarbonLib 1.0 and later*/
};



/*------------------------------------------------------------------------------------------------------*/
/*  o Control Types and ID's available only with Appearance 1.0 and later                               */
/*------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/*  o BEVEL BUTTON INTERFACE (CDEF 2)                                                   */
/*--------------------------------------------------------------------------------------*/
/*  Bevel buttons allow you to control the content type (pict/icon/etc.), the behavior  */
/* (pushbutton/toggle/sticky), and the bevel size. You also have the option of          */
/*  attaching a menu to it. When a menu is present, you can specify which way the       */
/*  popup arrow is facing (down or right).                                              */
/*                                                                                      */
/*  This is all made possible by overloading the Min, Max, and Value parameters for the */
/*  control, as well as adjusting the variant. Here's the breakdown of what goes where: */
/*                                                                                      */
/*  Parameter                   What Goes Here                                          */
/*  -------------------         ----------------------------------------------------    */
/*  Min                         Hi Byte = Behavior, Lo Byte = content type.             */
/*  Max                         ResID for resource-based content types.                 */
/*  Value                       MenuID to attach, 0 = no menu, please.                  */
/*                                                                                      */
/*  The variant is broken down into two halfs. The low 2 bits control the bevel type.   */
/*  Bit 2 controls the popup arrow direction (if a menu is present) and bit 3 controls  */
/*  whether or not to use the control's owning window's font.                           */
/*                                                                                      */
/*  Constants for all you need to put this together are below. The values for behaviors */
/*  are set up so that you can simply add them to the content type and pass them into   */
/*  the Min parameter of NewControl.                                                    */
/*                                                                                      */
/*  An example call:                                                                    */
/*                                                                                      */
/*  control = NewControl( window, &bounds, "\p", true, 0, kControlContentIconSuiteRes + */
/*                          kBehaviorToggles, myIconSuiteID, bevelButtonSmallBevelProc, */
/*                          0L );                                                       */
/*                                                                                      */
/*  Attaching a menu:                                                                   */
/*                                                                                      */
/*  control = NewControl( window, &bounds, "\p", true, kMyMenuID,                       */
/*          kControlContentIconSuiteRes, myIconSuiteID, bevelButtonSmallBevelProc +     */
/*          kBevelButtonMenuOnRight, 0L );                                              */
/*                                                                                      */
/*  This will attach menu ID kMyMenuID to the button, with the popup arrow facing right.*/
/*  This also puts the menu up to the right of the button. You can also specify that a  */
/*  menu can have multiple items checked at once by adding kBehaviorMultiValueMenus     */
/*  into the Min parameter. If you do use multivalue menus, the GetBevelButtonMenuValue */
/*  helper function will return the last item chosen from the menu, whether or not it   */
/*  was checked.                                                                        */
/*                                                                                      */
/*  NOTE:   Bevel buttons with menus actually have *two* values. The value of the       */
/*          button (on/off), and the value of the menu. The menu value can be gotten    */
/*          with the GetBevelButtonMenuValue helper function.                           */
/*                                                                                      */
/*  Handle-based Content                                                                */
/*  --------------------                                                                */
/*  You can create your control and then set the content to an existing handle to an    */
/*  icon suite, etc. using the macros below. Please keep in mind that resource-based    */
/*  content is owned by the control, handle-based content is owned by you. The CDEF will*/
/*  not try to dispose of handle-based content. If you are changing the content type of */
/*  the button on the fly, you must make sure that if you are replacing a handle-       */
/*  based content with a resource-based content to properly dispose of the handle,      */
/*  else a memory leak will ensue.                                                      */
/*                                                                                      */
/* Bevel Button Proc IDs */
enum {
  kControlBevelButtonSmallBevelProc = 32,
  kControlBevelButtonNormalBevelProc = 33,
  kControlBevelButtonLargeBevelProc = 34
};

/* Add these variant codes to kBevelButtonSmallBevelProc to change the type of button */
enum {
  kControlBevelButtonSmallBevelVariant = 0,
  kControlBevelButtonNormalBevelVariant = (1 << 0),
  kControlBevelButtonLargeBevelVariant = (1 << 1),
  kControlBevelButtonMenuOnRightVariant = (1 << 2)
};

/* Bevel Thicknesses */
typedef UInt16 ControlBevelThickness;
enum {
  kControlBevelButtonSmallBevel = 0,
  kControlBevelButtonNormalBevel = 1,
  kControlBevelButtonLargeBevel = 2
};

/* Behaviors of bevel buttons. These are set up so you can add  */
/* them together with the content types.                        */
enum {
  kControlBehaviorPushbutton    = 0,
  kControlBehaviorToggles       = 0x0100,
  kControlBehaviorSticky        = 0x0200,
  kControlBehaviorSingleValueMenu = 0,
  kControlBehaviorMultiValueMenu = 0x4000, /* only makes sense when a menu is attached.*/
  kControlBehaviorOffsetContents = 0x8000
};

/* Behaviors for 1.0.1 or later */
enum {
  kControlBehaviorCommandMenu   = 0x2000 /* menu holds commands, not choices. Overrides multi-value bit.*/
};

typedef UInt16                          ControlBevelButtonBehavior;
typedef UInt16                          ControlBevelButtonMenuBehavior;
/* Bevel Button Menu Placements */
typedef UInt16 ControlBevelButtonMenuPlacement;
enum {
  kControlBevelButtonMenuOnBottom = 0,
  kControlBevelButtonMenuOnRight = (1 << 2)
};

/* Control Kind Tag */
enum {
  kControlKindBevelButton       = FOUR_CHAR_CODE('bevl')
};

/* Creation API: Carbon Only */
/*
 *  CreateBevelButtonControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateBevelButtonControl(
  WindowRef                         window,
  const Rect *                      boundsRect,
  CFStringRef                       title,
  ControlBevelThickness             thickness,
  ControlBevelButtonBehavior        behavior,
  ControlButtonContentInfoPtr       info,
  SInt16                            menuID,
  ControlBevelButtonMenuBehavior    menuBehavior,
  ControlBevelButtonMenuPlacement   menuPlacement,
  ControlRef *                      outControl);




/* Graphic Alignments */
typedef SInt16 ControlButtonGraphicAlignment;
enum {
  kControlBevelButtonAlignSysDirection = -1, /* only left or right*/
  kControlBevelButtonAlignCenter = 0,
  kControlBevelButtonAlignLeft  = 1,
  kControlBevelButtonAlignRight = 2,
  kControlBevelButtonAlignTop   = 3,
  kControlBevelButtonAlignBottom = 4,
  kControlBevelButtonAlignTopLeft = 5,
  kControlBevelButtonAlignBottomLeft = 6,
  kControlBevelButtonAlignTopRight = 7,
  kControlBevelButtonAlignBottomRight = 8
};

/* Text Alignments */
typedef SInt16 ControlButtonTextAlignment;
enum {
  kControlBevelButtonAlignTextSysDirection = teFlushDefault,
  kControlBevelButtonAlignTextCenter = teCenter,
  kControlBevelButtonAlignTextFlushRight = teFlushRight,
  kControlBevelButtonAlignTextFlushLeft = teFlushLeft
};

/* Text Placements */
typedef SInt16 ControlButtonTextPlacement;
enum {
  kControlBevelButtonPlaceSysDirection = -1, /* if graphic on right, then on left*/
  kControlBevelButtonPlaceNormally = 0,
  kControlBevelButtonPlaceToRightOfGraphic = 1,
  kControlBevelButtonPlaceToLeftOfGraphic = 2,
  kControlBevelButtonPlaceBelowGraphic = 3,
  kControlBevelButtonPlaceAboveGraphic = 4
};


/* Data tags supported by the bevel button controls */
enum {
  kControlBevelButtonContentTag = FOUR_CHAR_CODE('cont'), /* ButtonContentInfo*/
  kControlBevelButtonTransformTag = FOUR_CHAR_CODE('tran'), /* IconTransformType*/
  kControlBevelButtonTextAlignTag = FOUR_CHAR_CODE('tali'), /* ButtonTextAlignment*/
  kControlBevelButtonTextOffsetTag = FOUR_CHAR_CODE('toff'), /* SInt16*/
  kControlBevelButtonGraphicAlignTag = FOUR_CHAR_CODE('gali'), /* ButtonGraphicAlignment*/
  kControlBevelButtonGraphicOffsetTag = FOUR_CHAR_CODE('goff'), /* Point*/
  kControlBevelButtonTextPlaceTag = FOUR_CHAR_CODE('tplc'), /* ButtonTextPlacement*/
  kControlBevelButtonMenuValueTag = FOUR_CHAR_CODE('mval'), /* SInt16*/
  kControlBevelButtonMenuHandleTag = FOUR_CHAR_CODE('mhnd'), /* MenuRef*/
  kControlBevelButtonMenuRefTag = FOUR_CHAR_CODE('mhnd'), /* MenuRef*/
  kControlBevelButtonCenterPopupGlyphTag = FOUR_CHAR_CODE('pglc') /* Boolean: true = center, false = bottom right*/
};

/* These are tags in 1.0.1 or later */
enum {
  kControlBevelButtonLastMenuTag = FOUR_CHAR_CODE('lmnu'), /* SInt16: menuID of last menu item selected from*/
  kControlBevelButtonMenuDelayTag = FOUR_CHAR_CODE('mdly') /* SInt32: ticks to delay before menu appears*/
};

/* tags available with Appearance 1.1 or later */
enum {
                                        /* Boolean: True = if an icon of the ideal size for*/
                                        /* the button isn't available, scale a larger or*/
                                        /* smaller icon to the ideal size. False = don't*/
                                        /* scale; draw a smaller icon or clip a larger icon.*/
                                        /* Default is false. Only applies to IconSuites and*/
  kControlBevelButtonScaleIconTag = FOUR_CHAR_CODE('scal') /* IconRefs.*/
};

/* tags available in Mac OS X and later */
enum {
  kControlBevelButtonOwnedMenuRefTag = FOUR_CHAR_CODE('omrf'), /* MenuRef (control will dispose)*/
  kControlBevelButtonKindTag    = FOUR_CHAR_CODE('bebk') /* ThemeButtonKind ( kTheme[Small,Medium,Large,Rounded]BevelButton )*/
};

/* Helper routines are available only thru the shared library/glue. */
/*
 *  GetBevelButtonMenuValue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetBevelButtonMenuValue(
  ControlRef   inButton,
  SInt16 *     outValue);


/*
 *  SetBevelButtonMenuValue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetBevelButtonMenuValue(
  ControlRef   inButton,
  SInt16       inValue);


/*
 *  GetBevelButtonMenuHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetBevelButtonMenuHandle(
  ControlRef    inButton,
  MenuHandle *  outHandle);


#define GetBevelButtonMenuRef GetBevelButtonMenuHandle
/*
 *  GetBevelButtonContentInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetBevelButtonContentInfo(
  ControlRef                    inButton,
  ControlButtonContentInfoPtr   outContent);


/*
 *  SetBevelButtonContentInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetBevelButtonContentInfo(
  ControlRef                    inButton,
  ControlButtonContentInfoPtr   inContent);


/*
 *  SetBevelButtonTransform()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetBevelButtonTransform(
  ControlRef          inButton,
  IconTransformType   transform);


/*
 *  SetBevelButtonGraphicAlignment()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetBevelButtonGraphicAlignment(
  ControlRef                      inButton,
  ControlButtonGraphicAlignment   inAlign,
  SInt16                          inHOffset,
  SInt16                          inVOffset);


/*
 *  SetBevelButtonTextAlignment()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetBevelButtonTextAlignment(
  ControlRef                   inButton,
  ControlButtonTextAlignment   inAlign,
  SInt16                       inHOffset);


/*
 *  SetBevelButtonTextPlacement()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetBevelButtonTextPlacement(
  ControlRef                   inButton,
  ControlButtonTextPlacement   inWhere);



/*--------------------------------------------------------------------------------------*/
/*  o SLIDER (CDEF 3)                                                                   */
/*--------------------------------------------------------------------------------------*/
/*  There are several variants that control the behavior of the slider control. Any     */
/*  combination of the following three constants can be added to the basic CDEF ID      */
/*  (kSliderProc).                                                                      */
/*                                                                                      */
/*  Variants:                                                                           */
/*                                                                                      */
/*      kSliderLiveFeedback     Slider does not use "ghosted" indicator when tracking.  */
/*                              ActionProc is called (set via SetControlAction) as the  */
/*                              indicator is dragged. The value is updated so that the  */
/*                              actionproc can adjust some other property based on the  */
/*                              value each time the action proc is called. If no action */
/*                              proc is installed, it reverts to the ghost indicator.   */
/*                                                                                      */
/*      kSliderHasTickMarks     Slider is drawn with 'tick marks'. The control          */
/*                              rectangle must be large enough to accomidate the tick   */
/*                              marks.                                                  */
/*                                                                                      */
/*      kSliderReverseDirection Slider thumb points in opposite direction than normal.  */
/*                              If the slider is vertical, the thumb will point to the  */
/*                              left, if the slider is horizontal, the thumb will point */
/*                              upwards.                                                */
/*                                                                                      */
/*      kSliderNonDirectional   This option overrides the kSliderReverseDirection and   */
/*                              kSliderHasTickMarks variants. It creates an indicator   */
/*                              which is rectangular and doesn't point in any direction */
/*                              like the normal indicator does.                         */
/* Slider proc ID and variants */
enum {
  kControlSliderProc            = 48,
  kControlSliderLiveFeedback    = (1 << 0),
  kControlSliderHasTickMarks    = (1 << 1),
  kControlSliderReverseDirection = (1 << 2),
  kControlSliderNonDirectional  = (1 << 3)
};

/* Slider Orientation */
typedef UInt16 ControlSliderOrientation;
enum {
  kControlSliderPointsDownOrRight = 0,
  kControlSliderPointsUpOrLeft  = 1,
  kControlSliderDoesNotPoint    = 2
};

/* Control Kind Tag */
enum {
  kControlKindSlider            = FOUR_CHAR_CODE('sldr')
};

/* Creation API: Carbon Only */
/*
 *  CreateSliderControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateSliderControl(
  WindowRef                  window,
  const Rect *               boundsRect,
  SInt32                     value,
  SInt32                     minimum,
  SInt32                     maximum,
  ControlSliderOrientation   orientation,
  UInt16                     numTickMarks,
  Boolean                    liveTracking,
  ControlActionUPP           liveTrackingProc,
  ControlRef *               outControl);



/*--------------------------------------------------------------------------------------*/
/*  o DISCLOSURE TRIANGLE (CDEF 4)                                                      */
/*--------------------------------------------------------------------------------------*/
/*  This control can be used as either left or right facing. It can also handle its own */
/*  tracking if you wish. This means that when the 'autotoggle' variant is used, if the */
/*  user clicks the control, it's state will change automatically from open to closed   */
/*  and vice-versa depending on its initial state. After a successful call to Track-    */
/*  Control, you can just check the current value to see what state it was switched to. */
/* Triangle proc IDs */
enum {
  kControlTriangleProc          = 64,
  kControlTriangleLeftFacingProc = 65,
  kControlTriangleAutoToggleProc = 66,
  kControlTriangleLeftFacingAutoToggleProc = 67
};

typedef UInt16 ControlDisclosureTriangleOrientation;
enum {
  kControlDisclosureTrianglePointDefault = 0, /* points right on a left-to-right script system (Mac OS X and later or CarbonLib 1.5 and later only)*/
  kControlDisclosureTrianglePointRight = 1,
  kControlDisclosureTrianglePointLeft = 2
};

/* Control Kind Tag */
enum {
  kControlKindDisclosureTriangle = FOUR_CHAR_CODE('dist')
};

/*
 *  CreateDisclosureTriangleControl()
 *  
 *  Summary:
 *    Creates a Disclosure Triangle control at a specific position in
 *    the specified window.
 *  
 *  Discussion:
 *    Disclosure Triangles are small controls that give the user a way
 *    to toggle the visibility of information or other user interface.
 *    When information is in a hidden state, a Disclosure Triangle is
 *    considered "closed" and should point to the right (or sometimes
 *    to the left). When the user clicks on it, the Disclosure Triangle
 *    rotates downwards into the "open" state. The application should
 *    repond by revealing the appropriate information or interface. On
 *    Mac OS X, a root control will be created for the window if one
 *    does not already exist. If a root control exists for the window,
 *    the Disclosure Triangle control will be embedded into it.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The WindowRef into which the Disclosure Triangle will be
 *      created.
 *    
 *    inBoundsRect:
 *      The desired position (in coordinates local to the window's
 *      port) for the Disclosure Triangle.
 *    
 *    inOrientation:
 *      The direction the Disclosure Triangle should point when it is
 *      "closed". Passing kControlDisclosureTrianglePointDefault is
 *      only legal as of Mac OS X and CarbonLib 1.5.
 *    
 *    inTitle:
 *      The title for the Disclosure Triangle. The title will only be
 *      displayed if the inDrawTitle parameter is true. Title display
 *      only works on Mac OS X.
 *    
 *    inInitialValue:
 *      The starting value determines whether the Disclosure Triangle
 *      is initially in its "open" or "closed" state. The value 0
 *      represents the "closed" state and 1 represents the "open" state.
 *    
 *    inDrawTitle:
 *      A Boolean indicating whether the Disclosure Triangle should
 *      draw its title next to the widget. Title display only works on
 *      Mac OS X.
 *    
 *    inAutoToggles:
 *      A Boolean indicating whether the Disclosure Triangle should
 *      change its own value (from "open" to "closed" and vice-versa)
 *      automatically when it is clicked on.
 *    
 *    outControl:
 *      On successful output, outControl will contain a reference to
 *      the Disclosure Triangle control.
 *  
 *  Result:
 *    An OSStatus code indicating success or failure.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateDisclosureTriangleControl(
  WindowRef                              inWindow,
  const Rect *                           inBoundsRect,
  ControlDisclosureTriangleOrientation   inOrientation,
  CFStringRef                            inTitle,
  SInt32                                 inInitialValue,
  Boolean                                inDrawTitle,
  Boolean                                inAutoToggles,
  ControlRef *                           outControl);



/* Tagged data supported by disclosure triangles */
enum {
  kControlTriangleLastValueTag  = FOUR_CHAR_CODE('last') /* SInt16*/
};

/* Helper routines are available only thru the shared library/glue. */
/*
 *  SetDisclosureTriangleLastValue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetDisclosureTriangleLastValue(
  ControlRef   inTabControl,
  SInt16       inValue);


/*--------------------------------------------------------------------------------------*/
/*  o PROGRESS INDICATOR (CDEF 5)                                                       */
/*--------------------------------------------------------------------------------------*/
/*  This CDEF implements both determinate and indeterminate progress bars. To switch,   */
/*  just use SetControlData to set the indeterminate flag to make it indeterminate call */
/*  IdleControls to step thru the animation. IdleControls should be called at least     */
/*  once during your event loop.                                                        */
/*                                                                                      */
/*  We also use this same CDEF for Relevance bars. At this time this control does not   */
/*  idle.                                                                               */
/* Progress Bar proc IDs */
enum {
  kControlProgressBarProc       = 80,
  kControlRelevanceBarProc      = 81
};

/* Control Kind Tag */
enum {
  kControlKindProgressBar       = FOUR_CHAR_CODE('prgb'),
  kControlKindRelevanceBar      = FOUR_CHAR_CODE('relb')
};

/* Creation API: Carbon only */
/*
 *  CreateProgressBarControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateProgressBarControl(
  WindowRef     window,
  const Rect *  boundsRect,
  SInt32        value,
  SInt32        minimum,
  SInt32        maximum,
  Boolean       indeterminate,
  ControlRef *  outControl);


/*
 *  CreateRelevanceBarControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateRelevanceBarControl(
  WindowRef     window,
  const Rect *  boundsRect,
  SInt32        value,
  SInt32        minimum,
  SInt32        maximum,
  ControlRef *  outControl);


/* Tagged data supported by progress bars */
enum {
  kControlProgressBarIndeterminateTag = FOUR_CHAR_CODE('inde'), /* Boolean*/
  kControlProgressBarAnimatingTag = FOUR_CHAR_CODE('anim') /* Boolean*/
};

/*--------------------------------------------------------------------------------------*/
/*  o LITTLE ARROWS (CDEF 6)                                                            */
/*--------------------------------------------------------------------------------------*/
/*  This control implements the little up and down arrows you'd see in the Memory       */
/*  control panel for adjusting the cache size.                                         */
/* Little Arrows proc IDs */
enum {
  kControlLittleArrowsProc      = 96
};

/* Control Kind Tag */
enum {
  kControlKindLittleArrows      = FOUR_CHAR_CODE('larr')
};

/* Creation API: Carbon only */
/*
 *  CreateLittleArrowsControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateLittleArrowsControl(
  WindowRef     window,
  const Rect *  boundsRect,
  SInt32        value,
  SInt32        minimum,
  SInt32        maximum,
  SInt32        increment,
  ControlRef *  outControl);


/*--------------------------------------------------------------------------------------*/
/*  o CHASING ARROWS (CDEF 7)                                                           */
/*--------------------------------------------------------------------------------------*/
/*  To animate this control, make sure to call IdleControls repeatedly.                 */
/*                                                                                      */
/* Chasing Arrows proc IDs */
enum {
  kControlChasingArrowsProc     = 112
};

/* Control Kind Tag */
enum {
  kControlKindChasingArrows     = FOUR_CHAR_CODE('carr')
};

/* Creation API: Carbon only */
/*
 *  CreateChasingArrowsControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateChasingArrowsControl(
  WindowRef     window,
  const Rect *  boundsRect,
  ControlRef *  outControl);


/* Tagged data supported by the Chasing Arrows control */
enum {
  kControlChasingArrowsAnimatingTag = FOUR_CHAR_CODE('anim') /* Boolean*/
};


/*--------------------------------------------------------------------------------------*/
/*  o TABS (CDEF 8)                                                                     */
/*--------------------------------------------------------------------------------------*/
/*  Tabs use an auxiliary resource (tab#) to hold tab information such as the tab name  */
/*  and an icon suite ID for each tab.                                                  */
/*                                                                                      */
/*  The ID of the tab# resource that you wish to associate with a tab control should    */
/*  be passed in as the Value parameter of the control. If you are using GetNewControl, */
/*  then the Value slot in the CNTL resource should have the ID of the 'tab#' resource  */
/*  on creation.                                                                        */
/*                                                                                      */
/*  Passing zero in for the tab# resource tells the control not to read in a tab# res.  */
/*  You can then use SetControlMaximum to add tabs, followed by a call to SetControlData*/
/*  with the kControlTabInfoTag, passing in a pointer to a ControlTabInfoRec. This sets */
/*  the name and optionally an icon for a tab.                                          */
/* Tabs proc IDs */
enum {
  kControlTabLargeProc          = 128,  /* Large tab size, north facing   */
  kControlTabSmallProc          = 129,  /* Small tab size, north facing   */
  kControlTabLargeNorthProc     = 128,  /* Large tab size, north facing   */
  kControlTabSmallNorthProc     = 129,  /* Small tab size, north facing   */
  kControlTabLargeSouthProc     = 130,  /* Large tab size, south facing   */
  kControlTabSmallSouthProc     = 131,  /* Small tab size, south facing   */
  kControlTabLargeEastProc      = 132,  /* Large tab size, east facing    */
  kControlTabSmallEastProc      = 133,  /* Small tab size, east facing    */
  kControlTabLargeWestProc      = 134,  /* Large tab size, west facing    */
  kControlTabSmallWestProc      = 135   /* Small tab size, west facing    */
};

/* Tab Directions */
typedef UInt16 ControlTabDirection;
enum {
  kControlTabDirectionNorth     = 0,
  kControlTabDirectionSouth     = 1,
  kControlTabDirectionEast      = 2,
  kControlTabDirectionWest      = 3
};

/* Tab Sizes */
typedef UInt16 ControlTabSize;
enum {
  kControlTabSizeLarge          = kControlSizeNormal,
  kControlTabSizeSmall          = kControlSizeSmall
};

/* Control Tab Entry - used during creation                             */
/* Note that the client is responsible for allocating/providing         */
/* the ControlButtonContentInfo and string storage for this             */
/* structure.                                                           */
struct ControlTabEntry {
  ControlButtonContentInfo * icon;
  CFStringRef         name;
  Boolean             enabled;
};
typedef struct ControlTabEntry          ControlTabEntry;
/* Control Kind Tag */
enum {
  kControlKindTabs              = FOUR_CHAR_CODE('tabs')
};

/* Creation API: Carbon only */
/*
 *  CreateTabsControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateTabsControl(
  WindowRef                window,
  const Rect *             boundsRect,
  ControlTabSize           size,
  ControlTabDirection      direction,
  UInt16                   numTabs,
  const ControlTabEntry *  tabArray,
  ControlRef *             outControl);



/* Tagged data supported by tabs */
enum {
  kControlTabContentRectTag     = FOUR_CHAR_CODE('rect'), /* Rect*/
  kControlTabEnabledFlagTag     = FOUR_CHAR_CODE('enab'), /* Boolean*/
  kControlTabFontStyleTag       = kControlFontStyleTag /* ControlFontStyleRec*/
};

/* New tags in 1.0.1 or later */
enum {
  kControlTabInfoTag            = FOUR_CHAR_CODE('tabi') /* ControlTabInfoRec*/
};

/* New tags in X 10.1 or later */
enum {
  kControlTabImageContentTag    = FOUR_CHAR_CODE('cont') /* ControlButtonContentInfo*/
};

enum {
  kControlTabInfoVersionZero    = 0,    /* ControlTabInfoRec*/
  kControlTabInfoVersionOne     = 1     /* ControlTabInfoRecV1*/
};

struct ControlTabInfoRec {
  SInt16              version;                /* version of this structure.*/
  SInt16              iconSuiteID;            /* icon suite to use. Zero indicates no icon*/
  Str255              name;                   /* name to be displayed on the tab*/
};
typedef struct ControlTabInfoRec        ControlTabInfoRec;
struct ControlTabInfoRecV1 {
  SInt16              version;                /* version of this structure. == kControlTabInfoVersionOne*/
  SInt16              iconSuiteID;            /* icon suite to use. Zero indicates no icon*/
  CFStringRef         name;                   /* name to be displayed on the tab. Will be retained so caller*/
                                              /* should always release it.*/
};
typedef struct ControlTabInfoRecV1      ControlTabInfoRecV1;
/* Helper routines are available only thru the shared library/glue. */
/*
 *  GetTabContentRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetTabContentRect(
  ControlRef   inTabControl,
  Rect *       outContentRect);


/*
 *  SetTabEnabled()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetTabEnabled(
  ControlRef   inTabControl,
  SInt16       inTabToHilite,
  Boolean      inEnabled);


/*--------------------------------------------------------------------------------------*/
/*  o VISUAL SEPARATOR (CDEF 9)                                                         */
/*--------------------------------------------------------------------------------------*/
/*  Separator lines determine their orientation (horizontal or vertical) automatically  */
/*  based on the relative height and width of their contrlRect.                         */
/* Visual separator proc IDs */
enum {
  kControlSeparatorLineProc     = 144
};

/* Control Kind Tag */
enum {
  kControlKindSeparator         = FOUR_CHAR_CODE('sepa')
};

/* Creation API: Carbon only */
/*
 *  CreateSeparatorControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateSeparatorControl(
  WindowRef     window,
  const Rect *  boundsRect,
  ControlRef *  outControl);


/*--------------------------------------------------------------------------------------*/
/*  o GROUP BOX (CDEF 10)                                                               */
/*--------------------------------------------------------------------------------------*/
/*  The group box CDEF can be use in several ways. It can have no title, a text title,  */
/*  a check box as the title, or a popup button as a title. There are two versions of   */
/*  group boxes, primary and secondary, which look slightly different.                  */
/* Group Box proc IDs */
enum {
  kControlGroupBoxTextTitleProc = 160,
  kControlGroupBoxCheckBoxProc  = 161,
  kControlGroupBoxPopupButtonProc = 162,
  kControlGroupBoxSecondaryTextTitleProc = 164,
  kControlGroupBoxSecondaryCheckBoxProc = 165,
  kControlGroupBoxSecondaryPopupButtonProc = 166
};

/* Control Kind Tag */
enum {
  kControlKindGroupBox          = FOUR_CHAR_CODE('grpb'),
  kControlKindCheckGroupBox     = FOUR_CHAR_CODE('cgrp'),
  kControlKindPopupGroupBox     = FOUR_CHAR_CODE('pgrp')
};

/* Creation APIs: Carbon only */
/*
 *  CreateGroupBoxControl()
 *  
 *  Summary:
 *    Creates a group box control.
 *  
 *  Parameters:
 *    
 *    window:
 *      The window that should contain the control.
 *    
 *    boundsRect:
 *      The bounding box of the control.
 *    
 *    title:
 *      The title of the control.
 *    
 *    primary:
 *      Whether to create a primary or secondary group box.
 *    
 *    outControl:
 *      On exit, contains the new control.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateGroupBoxControl(
  WindowRef     window,
  const Rect *  boundsRect,
  CFStringRef   title,
  Boolean       primary,
  ControlRef *  outControl);


/*
 *  CreateCheckGroupBoxControl()
 *  
 *  Summary:
 *    Creates a group box control that has a check box as its title.
 *  
 *  Parameters:
 *    
 *    window:
 *      The window that should contain the control.
 *    
 *    boundsRect:
 *      The bounding box of the control.
 *    
 *    title:
 *      The title of the control (used as the title of the check box).
 *    
 *    initialValue:
 *      The initial value of the check box.
 *    
 *    primary:
 *      Whether to create a primary or secondary group box.
 *    
 *    autoToggle:
 *      Whether to create an auto-toggling check box. Auto-toggling
 *      check box titles are only supported on Mac OS X; this parameter
 *      must be false when used with CarbonLib.
 *    
 *    outControl:
 *      On exit, contains the new control.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateCheckGroupBoxControl(
  WindowRef     window,
  const Rect *  boundsRect,
  CFStringRef   title,
  SInt32        initialValue,
  Boolean       primary,
  Boolean       autoToggle,
  ControlRef *  outControl);


/*
 *  CreatePopupGroupBoxControl()
 *  
 *  Summary:
 *    Creates a group box control that has a popup button as its title.
 *  
 *  Parameters:
 *    
 *    window:
 *      The window that should contain the control.
 *    
 *    boundsRect:
 *      The bounding box of the control.
 *    
 *    title:
 *      The title of the control (used as the title of the popup
 *      button).
 *    
 *    primary:
 *      Whether to create a primary or secondary group box.
 *    
 *    menuID:
 *      The menu ID of the menu to be displayed by the popup button.
 *    
 *    variableWidth:
 *      Whether the popup button should have a variable-width title.
 *      Fixed-width titles are only supported by Mac OS X; this
 *      parameter must be true when used with CarbonLib.
 *    
 *    titleWidth:
 *      The width in pixels of the popup button title.
 *    
 *    titleJustification:
 *      The justification of the popup button title. Use one of the
 *      TextEdit justification constants here (teFlushDefault,
 *      teCenter, teFlushRight, or teFlushLeft).
 *    
 *    titleStyle:
 *      The QuickDraw text style of the popup button title.
 *    
 *    outControl:
 *      On exit, contains the new control.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreatePopupGroupBoxControl(
  WindowRef     window,
  const Rect *  boundsRect,
  CFStringRef   title,
  Boolean       primary,
  SInt16        menuID,
  Boolean       variableWidth,
  SInt16        titleWidth,
  SInt16        titleJustification,
  Style         titleStyle,
  ControlRef *  outControl);



/* Tagged data supported by group box */
enum {
  kControlGroupBoxMenuHandleTag = FOUR_CHAR_CODE('mhan'), /* MenuRef (popup title only)*/
  kControlGroupBoxMenuRefTag    = FOUR_CHAR_CODE('mhan'), /* MenuRef (popup title only)*/
  kControlGroupBoxFontStyleTag  = kControlFontStyleTag /* ControlFontStyleRec*/
};

/* tags available with Appearance 1.1 or later */
enum {
  kControlGroupBoxTitleRectTag  = FOUR_CHAR_CODE('trec') /* Rect. Rectangle that the title text/control is drawn in. (get only)*/
};

/*--------------------------------------------------------------------------------------*/
/*  o IMAGE WELL (CDEF 11)                                                              */
/*--------------------------------------------------------------------------------------*/
/*  Image Wells allow you to control the content type (pict/icon/etc.) shown in the     */
/*  well.                                                                               */
/*                                                                                      */
/*  This is made possible by overloading the Min and Value parameters for the control.  */
/*                                                                                      */
/*  Parameter                   What Goes Here                                          */
/*  -------------------         --------------------------------------------------      */
/*  Min                         content type (see constants for bevel buttons)          */
/*  Value                       Resource ID of content type, if resource-based.         */
/*                                                                                      */
/*                                                                                      */
/*  Handle-based Content                                                                */
/*  --------------------                                                                */
/*  You can create your control and then set the content to an existing handle to an    */
/*  icon suite, etc. using the macros below. Please keep in mind that resource-based    */
/*  content is owned by the control, handle-based content is owned by you. The CDEF will*/
/*  not try to dispose of handle-based content. If you are changing the content type of */
/*  the button on the fly, you must make sure that if you are replacing a handle-       */
/*  based content with a resource-based content to properly dispose of the handle,      */
/*  else a memory leak will ensue.                                                      */
/*                                                                                      */
/* Image Well proc IDs */
enum {
  kControlImageWellProc         = 176
};

/* Control Kind Tag */
enum {
  kControlKindImageWell         = FOUR_CHAR_CODE('well')
};

/* Creation API: Carbon only */
/*
 *  CreateImageWellControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateImageWellControl(
  WindowRef                         window,
  const Rect *                      boundsRect,
  const ControlButtonContentInfo *  info,
  ControlRef *                      outControl);


/* Tagged data supported by image wells */
enum {
  kControlImageWellContentTag   = FOUR_CHAR_CODE('cont'), /* ButtonContentInfo*/
  kControlImageWellTransformTag = FOUR_CHAR_CODE('tran'), /* IconTransformType*/
  kControlImageWellIsDragDestinationTag = FOUR_CHAR_CODE('drag') /* Boolean*/
};

/* Helper routines are available only thru the shared library/glue. */
/*
 *  GetImageWellContentInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetImageWellContentInfo(
  ControlRef                    inButton,
  ControlButtonContentInfoPtr   outContent);


/*
 *  SetImageWellContentInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetImageWellContentInfo(
  ControlRef                    inButton,
  ControlButtonContentInfoPtr   inContent);


/*
 *  SetImageWellTransform()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetImageWellTransform(
  ControlRef          inButton,
  IconTransformType   inTransform);


/*--------------------------------------------------------------------------------------*/
/*  o POPUP ARROW (CDEF 12)                                                             */
/*--------------------------------------------------------------------------------------*/
/*  The popup arrow CDEF is used to draw the small arrow normally associated with a     */
/*  popup control. The arrow can point in four directions, and a small or large version */
/*  can be used. This control is provided to allow clients to draw the arrow in a       */
/*  normalized fashion which will take advantage of themes automatically.               */
/*                                                                                      */
/* Popup Arrow proc IDs */
enum {
  kControlPopupArrowEastProc    = 192,
  kControlPopupArrowWestProc    = 193,
  kControlPopupArrowNorthProc   = 194,
  kControlPopupArrowSouthProc   = 195,
  kControlPopupArrowSmallEastProc = 196,
  kControlPopupArrowSmallWestProc = 197,
  kControlPopupArrowSmallNorthProc = 198,
  kControlPopupArrowSmallSouthProc = 199
};

/* Popup Arrow Orientations */
enum {
  kControlPopupArrowOrientationEast = 0,
  kControlPopupArrowOrientationWest = 1,
  kControlPopupArrowOrientationNorth = 2,
  kControlPopupArrowOrientationSouth = 3
};


typedef UInt16                          ControlPopupArrowOrientation;
/* Popup Arrow Size */
enum {
  kControlPopupArrowSizeNormal  = 0,
  kControlPopupArrowSizeSmall   = 1
};

typedef UInt16                          ControlPopupArrowSize;
/* Control Kind Tag */
enum {
  kControlKindPopupArrow        = FOUR_CHAR_CODE('parr')
};

/* Creation API: Carbon only */
/*
 *  CreatePopupArrowControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreatePopupArrowControl(
  WindowRef                      window,
  const Rect *                   boundsRect,
  ControlPopupArrowOrientation   orientation,
  ControlPopupArrowSize          size,
  ControlRef *                   outControl);


/*--------------------------------------------------------------------------------------*/
/*  o PLACARD (CDEF 14)                                                                 */
/*--------------------------------------------------------------------------------------*/
/* Placard proc IDs */
enum {
  kControlPlacardProc           = 224
};

/* Control Kind Tag */
enum {
  kControlKindPlacard           = FOUR_CHAR_CODE('plac')
};

/* Creation API: Carbon only */
/*
 *  CreatePlacardControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreatePlacardControl(
  WindowRef     window,
  const Rect *  boundsRect,
  ControlRef *  outControl);


/*--------------------------------------------------------------------------------------*/
/*  o CLOCK (CDEF 15)                                                                   */
/*--------------------------------------------------------------------------------------*/
/*  NOTE:   You can specify more options in the Value paramter when creating the clock. */
/*          See below.                                                                  */
/*                                                                                      */
/*  NOTE:   Under Appearance 1.1, the clock control knows and returns more part codes.  */
/*          The new clock-specific part codes are defined with the other control parts. */
/*          Besides these clock-specific parts, we also return kControlUpButtonPart     */
/*          and kControlDownButtonPart when they hit the up and down arrows.            */
/*          The new part codes give you more flexibility for focusing and hit testing.  */
/*                                                                                      */
/*          The original kControlClockPart is still valid. When hit testing, it means   */
/*          that some non-editable area of the clock's whitespace has been clicked.     */
/*          When focusing a currently unfocused clock, it changes the focus to the      */
/*          first part; it is the same as passing kControlFocusNextPart. When           */
/*          re-focusing a focused clock, it will not change the focus at all.           */
/* Clock proc IDs */
enum {
  kControlClockTimeProc         = 240,
  kControlClockTimeSecondsProc  = 241,
  kControlClockDateProc         = 242,
  kControlClockMonthYearProc    = 243
};

/* Clock Types */
typedef UInt16 ControlClockType;
enum {
  kControlClockTypeHourMinute   = 0,
  kControlClockTypeHourMinuteSecond = 1,
  kControlClockTypeMonthDayYear = 2,
  kControlClockTypeMonthYear    = 3
};

/* Clock Flags */
/*  These flags can be passed into 'value' field on creation of the control.            */
/*  Value is set to 0 after control is created.                                         */
typedef UInt32 ControlClockFlags;
enum {
  kControlClockFlagStandard     = 0,    /* editable, non-live*/
  kControlClockNoFlags          = 0,
  kControlClockFlagDisplayOnly  = 1,    /* add this to become non-editable*/
  kControlClockIsDisplayOnly    = 1,
  kControlClockFlagLive         = 2,    /* automatically shows current time on idle. only valid with display only.*/
  kControlClockIsLive           = 2
};

/* Control Kind Tag */
enum {
  kControlKindClock             = FOUR_CHAR_CODE('clck')
};

/* Creation API: Carbon only */
/*
 *  CreateClockControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateClockControl(
  WindowRef           window,
  const Rect *        boundsRect,
  ControlClockType    clockType,
  ControlClockFlags   clockFlags,
  ControlRef *        outControl);


/* Tagged data supported by clocks */
enum {
  kControlClockLongDateTag      = FOUR_CHAR_CODE('date'), /* LongDateRec*/
  kControlClockFontStyleTag     = kControlFontStyleTag, /* ControlFontStyleRec*/
  kControlClockAnimatingTag     = FOUR_CHAR_CODE('anim') /* Boolean*/
};

/*--------------------------------------------------------------------------------------*/
/*  o USER PANE (CDEF 16)                                                               */
/*--------------------------------------------------------------------------------------*/
/*  User panes have two primary purposes: to allow easy implementation of a custom      */
/*  control by the developer, and to provide a generic container for embedding other    */
/*  controls.                                                                           */
/*                                                                                      */
/*  In Carbon, with the advent of Carbon-event-based controls, you may find it easier   */
/*  to simply write a new control from scratch than to customize a user pane control.   */
/*  The set of callbacks provided by the user pane will not be extended to support      */
/*  new Control Manager features; instead, you should just write a real control.        */
/*                                                                                      */
/*  User panes do not, by default, support embedding. If you try to embed a control     */
/*  into a user pane, you will get back errControlIsNotEmbedder. You can make a user    */
/*  pane support embedding by passing the kControlSupportsEmbedding flag in the 'value' */
/*  parameter when you create the control.                                              */
/*                                                                                      */
/*  User panes support the following overloaded control initialization options:         */
/*                                                                                      */
/*  Parameter                   What Goes Here                                          */
/*  -------------------         --------------------------------------------------      */
/*  Value                       Control feature flags                                   */

/* User Pane proc IDs */
enum {
  kControlUserPaneProc          = 256
};

/* Control Kind Tag */
enum {
  kControlKindUserPane          = FOUR_CHAR_CODE('upan')
};

/* Creation API: Carbon only */
/*
 *  CreateUserPaneControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateUserPaneControl(
  WindowRef     window,
  const Rect *  boundsRect,
  UInt32        features,
  ControlRef *  outControl);


/* Tagged data supported by user panes */
/* Currently, they are all proc ptrs for doing things like drawing and hit testing, etc. */
enum {
  kControlUserItemDrawProcTag   = FOUR_CHAR_CODE('uidp'), /* UserItemUPP*/
  kControlUserPaneDrawProcTag   = FOUR_CHAR_CODE('draw'), /* ControlUserPaneDrawingUPP*/
  kControlUserPaneHitTestProcTag = FOUR_CHAR_CODE('hitt'), /* ControlUserPaneHitTestUPP*/
  kControlUserPaneTrackingProcTag = FOUR_CHAR_CODE('trak'), /* ControlUserPaneTrackingUPP*/
  kControlUserPaneIdleProcTag   = FOUR_CHAR_CODE('idle'), /* ControlUserPaneIdleUPP*/
  kControlUserPaneKeyDownProcTag = FOUR_CHAR_CODE('keyd'), /* ControlUserPaneKeyDownUPP*/
  kControlUserPaneActivateProcTag = FOUR_CHAR_CODE('acti'), /* ControlUserPaneActivateUPP*/
  kControlUserPaneFocusProcTag  = FOUR_CHAR_CODE('foci'), /* ControlUserPaneFocusUPP*/
  kControlUserPaneBackgroundProcTag = FOUR_CHAR_CODE('back') /* ControlUserPaneBackgroundUPP*/
};

typedef CALLBACK_API( void , ControlUserPaneDrawProcPtr )(ControlRef control, SInt16 part);
typedef CALLBACK_API( ControlPartCode , ControlUserPaneHitTestProcPtr )(ControlRef control, Point where);
typedef CALLBACK_API( ControlPartCode , ControlUserPaneTrackingProcPtr )(ControlRef control, Point startPt, ControlActionUPP actionProc);
typedef CALLBACK_API( void , ControlUserPaneIdleProcPtr )(ControlRef control);
typedef CALLBACK_API( ControlPartCode , ControlUserPaneKeyDownProcPtr )(ControlRef control, SInt16 keyCode, SInt16 charCode, SInt16 modifiers);
typedef CALLBACK_API( void , ControlUserPaneActivateProcPtr )(ControlRef control, Boolean activating);
typedef CALLBACK_API( ControlPartCode , ControlUserPaneFocusProcPtr )(ControlRef control, ControlFocusPart action);
typedef CALLBACK_API( void , ControlUserPaneBackgroundProcPtr )(ControlRef control, ControlBackgroundPtr info);
typedef STACK_UPP_TYPE(ControlUserPaneDrawProcPtr)              ControlUserPaneDrawUPP;
typedef STACK_UPP_TYPE(ControlUserPaneHitTestProcPtr)           ControlUserPaneHitTestUPP;
typedef STACK_UPP_TYPE(ControlUserPaneTrackingProcPtr)          ControlUserPaneTrackingUPP;
typedef STACK_UPP_TYPE(ControlUserPaneIdleProcPtr)              ControlUserPaneIdleUPP;
typedef STACK_UPP_TYPE(ControlUserPaneKeyDownProcPtr)           ControlUserPaneKeyDownUPP;
typedef STACK_UPP_TYPE(ControlUserPaneActivateProcPtr)          ControlUserPaneActivateUPP;
typedef STACK_UPP_TYPE(ControlUserPaneFocusProcPtr)             ControlUserPaneFocusUPP;
typedef STACK_UPP_TYPE(ControlUserPaneBackgroundProcPtr)        ControlUserPaneBackgroundUPP;
/*
 *  NewControlUserPaneDrawUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlUserPaneDrawUPP )
NewControlUserPaneDrawUPP(ControlUserPaneDrawProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppControlUserPaneDrawProcInfo = 0x000002C0 };  /* pascal no_return_value Func(4_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ControlUserPaneDrawUPP) NewControlUserPaneDrawUPP(ControlUserPaneDrawProcPtr userRoutine) { return (ControlUserPaneDrawUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneDrawProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewControlUserPaneDrawUPP(userRoutine) (ControlUserPaneDrawUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneDrawProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewControlUserPaneHitTestUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlUserPaneHitTestUPP )
NewControlUserPaneHitTestUPP(ControlUserPaneHitTestProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppControlUserPaneHitTestProcInfo = 0x000003E0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ControlUserPaneHitTestUPP) NewControlUserPaneHitTestUPP(ControlUserPaneHitTestProcPtr userRoutine) { return (ControlUserPaneHitTestUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneHitTestProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewControlUserPaneHitTestUPP(userRoutine) (ControlUserPaneHitTestUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneHitTestProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewControlUserPaneTrackingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlUserPaneTrackingUPP )
NewControlUserPaneTrackingUPP(ControlUserPaneTrackingProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppControlUserPaneTrackingProcInfo = 0x00000FE0 };  /* pascal 2_bytes Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ControlUserPaneTrackingUPP) NewControlUserPaneTrackingUPP(ControlUserPaneTrackingProcPtr userRoutine) { return (ControlUserPaneTrackingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneTrackingProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewControlUserPaneTrackingUPP(userRoutine) (ControlUserPaneTrackingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneTrackingProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewControlUserPaneIdleUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlUserPaneIdleUPP )
NewControlUserPaneIdleUPP(ControlUserPaneIdleProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppControlUserPaneIdleProcInfo = 0x000000C0 };  /* pascal no_return_value Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ControlUserPaneIdleUPP) NewControlUserPaneIdleUPP(ControlUserPaneIdleProcPtr userRoutine) { return (ControlUserPaneIdleUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneIdleProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewControlUserPaneIdleUPP(userRoutine) (ControlUserPaneIdleUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneIdleProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewControlUserPaneKeyDownUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlUserPaneKeyDownUPP )
NewControlUserPaneKeyDownUPP(ControlUserPaneKeyDownProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppControlUserPaneKeyDownProcInfo = 0x00002AE0 };  /* pascal 2_bytes Func(4_bytes, 2_bytes, 2_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ControlUserPaneKeyDownUPP) NewControlUserPaneKeyDownUPP(ControlUserPaneKeyDownProcPtr userRoutine) { return (ControlUserPaneKeyDownUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneKeyDownProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewControlUserPaneKeyDownUPP(userRoutine) (ControlUserPaneKeyDownUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneKeyDownProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewControlUserPaneActivateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlUserPaneActivateUPP )
NewControlUserPaneActivateUPP(ControlUserPaneActivateProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppControlUserPaneActivateProcInfo = 0x000001C0 };  /* pascal no_return_value Func(4_bytes, 1_byte) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ControlUserPaneActivateUPP) NewControlUserPaneActivateUPP(ControlUserPaneActivateProcPtr userRoutine) { return (ControlUserPaneActivateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneActivateProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewControlUserPaneActivateUPP(userRoutine) (ControlUserPaneActivateUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneActivateProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewControlUserPaneFocusUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlUserPaneFocusUPP )
NewControlUserPaneFocusUPP(ControlUserPaneFocusProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppControlUserPaneFocusProcInfo = 0x000002E0 };  /* pascal 2_bytes Func(4_bytes, 2_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ControlUserPaneFocusUPP) NewControlUserPaneFocusUPP(ControlUserPaneFocusProcPtr userRoutine) { return (ControlUserPaneFocusUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneFocusProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewControlUserPaneFocusUPP(userRoutine) (ControlUserPaneFocusUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneFocusProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewControlUserPaneBackgroundUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlUserPaneBackgroundUPP )
NewControlUserPaneBackgroundUPP(ControlUserPaneBackgroundProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppControlUserPaneBackgroundProcInfo = 0x000003C0 };  /* pascal no_return_value Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ControlUserPaneBackgroundUPP) NewControlUserPaneBackgroundUPP(ControlUserPaneBackgroundProcPtr userRoutine) { return (ControlUserPaneBackgroundUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneBackgroundProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewControlUserPaneBackgroundUPP(userRoutine) (ControlUserPaneBackgroundUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlUserPaneBackgroundProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeControlUserPaneDrawUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeControlUserPaneDrawUPP(ControlUserPaneDrawUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeControlUserPaneDrawUPP(ControlUserPaneDrawUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeControlUserPaneDrawUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeControlUserPaneHitTestUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeControlUserPaneHitTestUPP(ControlUserPaneHitTestUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeControlUserPaneHitTestUPP(ControlUserPaneHitTestUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeControlUserPaneHitTestUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeControlUserPaneTrackingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeControlUserPaneTrackingUPP(ControlUserPaneTrackingUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeControlUserPaneTrackingUPP(ControlUserPaneTrackingUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeControlUserPaneTrackingUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeControlUserPaneIdleUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeControlUserPaneIdleUPP(ControlUserPaneIdleUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeControlUserPaneIdleUPP(ControlUserPaneIdleUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeControlUserPaneIdleUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeControlUserPaneKeyDownUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeControlUserPaneKeyDownUPP(ControlUserPaneKeyDownUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeControlUserPaneKeyDownUPP(ControlUserPaneKeyDownUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeControlUserPaneKeyDownUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeControlUserPaneActivateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeControlUserPaneActivateUPP(ControlUserPaneActivateUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeControlUserPaneActivateUPP(ControlUserPaneActivateUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeControlUserPaneActivateUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeControlUserPaneFocusUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeControlUserPaneFocusUPP(ControlUserPaneFocusUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeControlUserPaneFocusUPP(ControlUserPaneFocusUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeControlUserPaneFocusUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeControlUserPaneBackgroundUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeControlUserPaneBackgroundUPP(ControlUserPaneBackgroundUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeControlUserPaneBackgroundUPP(ControlUserPaneBackgroundUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeControlUserPaneBackgroundUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeControlUserPaneDrawUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeControlUserPaneDrawUPP(
  ControlRef              control,
  SInt16                  part,
  ControlUserPaneDrawUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeControlUserPaneDrawUPP(ControlRef control, SInt16 part, ControlUserPaneDrawUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppControlUserPaneDrawProcInfo, control, part); }
  #else
    #define InvokeControlUserPaneDrawUPP(control, part, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppControlUserPaneDrawProcInfo, (control), (part))
  #endif
#endif

/*
 *  InvokeControlUserPaneHitTestUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlPartCode )
InvokeControlUserPaneHitTestUPP(
  ControlRef                 control,
  Point                      where,
  ControlUserPaneHitTestUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(ControlPartCode) InvokeControlUserPaneHitTestUPP(ControlRef control, Point where, ControlUserPaneHitTestUPP userUPP) { return (ControlPartCode)CALL_TWO_PARAMETER_UPP(userUPP, uppControlUserPaneHitTestProcInfo, control, where); }
  #else
    #define InvokeControlUserPaneHitTestUPP(control, where, userUPP) (ControlPartCode)CALL_TWO_PARAMETER_UPP((userUPP), uppControlUserPaneHitTestProcInfo, (control), (where))
  #endif
#endif

/*
 *  InvokeControlUserPaneTrackingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlPartCode )
InvokeControlUserPaneTrackingUPP(
  ControlRef                  control,
  Point                       startPt,
  ControlActionUPP            actionProc,
  ControlUserPaneTrackingUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(ControlPartCode) InvokeControlUserPaneTrackingUPP(ControlRef control, Point startPt, ControlActionUPP actionProc, ControlUserPaneTrackingUPP userUPP) { return (ControlPartCode)CALL_THREE_PARAMETER_UPP(userUPP, uppControlUserPaneTrackingProcInfo, control, startPt, actionProc); }
  #else
    #define InvokeControlUserPaneTrackingUPP(control, startPt, actionProc, userUPP) (ControlPartCode)CALL_THREE_PARAMETER_UPP((userUPP), uppControlUserPaneTrackingProcInfo, (control), (startPt), (actionProc))
  #endif
#endif

/*
 *  InvokeControlUserPaneIdleUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeControlUserPaneIdleUPP(
  ControlRef              control,
  ControlUserPaneIdleUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeControlUserPaneIdleUPP(ControlRef control, ControlUserPaneIdleUPP userUPP) { CALL_ONE_PARAMETER_UPP(userUPP, uppControlUserPaneIdleProcInfo, control); }
  #else
    #define InvokeControlUserPaneIdleUPP(control, userUPP) CALL_ONE_PARAMETER_UPP((userUPP), uppControlUserPaneIdleProcInfo, (control))
  #endif
#endif

/*
 *  InvokeControlUserPaneKeyDownUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlPartCode )
InvokeControlUserPaneKeyDownUPP(
  ControlRef                 control,
  SInt16                     keyCode,
  SInt16                     charCode,
  SInt16                     modifiers,
  ControlUserPaneKeyDownUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(ControlPartCode) InvokeControlUserPaneKeyDownUPP(ControlRef control, SInt16 keyCode, SInt16 charCode, SInt16 modifiers, ControlUserPaneKeyDownUPP userUPP) { return (ControlPartCode)CALL_FOUR_PARAMETER_UPP(userUPP, uppControlUserPaneKeyDownProcInfo, control, keyCode, charCode, modifiers); }
  #else
    #define InvokeControlUserPaneKeyDownUPP(control, keyCode, charCode, modifiers, userUPP) (ControlPartCode)CALL_FOUR_PARAMETER_UPP((userUPP), uppControlUserPaneKeyDownProcInfo, (control), (keyCode), (charCode), (modifiers))
  #endif
#endif

/*
 *  InvokeControlUserPaneActivateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeControlUserPaneActivateUPP(
  ControlRef                  control,
  Boolean                     activating,
  ControlUserPaneActivateUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeControlUserPaneActivateUPP(ControlRef control, Boolean activating, ControlUserPaneActivateUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppControlUserPaneActivateProcInfo, control, activating); }
  #else
    #define InvokeControlUserPaneActivateUPP(control, activating, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppControlUserPaneActivateProcInfo, (control), (activating))
  #endif
#endif

/*
 *  InvokeControlUserPaneFocusUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlPartCode )
InvokeControlUserPaneFocusUPP(
  ControlRef               control,
  ControlFocusPart         action,
  ControlUserPaneFocusUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(ControlPartCode) InvokeControlUserPaneFocusUPP(ControlRef control, ControlFocusPart action, ControlUserPaneFocusUPP userUPP) { return (ControlPartCode)CALL_TWO_PARAMETER_UPP(userUPP, uppControlUserPaneFocusProcInfo, control, action); }
  #else
    #define InvokeControlUserPaneFocusUPP(control, action, userUPP) (ControlPartCode)CALL_TWO_PARAMETER_UPP((userUPP), uppControlUserPaneFocusProcInfo, (control), (action))
  #endif
#endif

/*
 *  InvokeControlUserPaneBackgroundUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeControlUserPaneBackgroundUPP(
  ControlRef                    control,
  ControlBackgroundPtr          info,
  ControlUserPaneBackgroundUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeControlUserPaneBackgroundUPP(ControlRef control, ControlBackgroundPtr info, ControlUserPaneBackgroundUPP userUPP) { CALL_TWO_PARAMETER_UPP(userUPP, uppControlUserPaneBackgroundProcInfo, control, info); }
  #else
    #define InvokeControlUserPaneBackgroundUPP(control, info, userUPP) CALL_TWO_PARAMETER_UPP((userUPP), uppControlUserPaneBackgroundProcInfo, (control), (info))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewControlUserPaneDrawProc(userRoutine)             NewControlUserPaneDrawUPP(userRoutine)
    #define NewControlUserPaneHitTestProc(userRoutine)          NewControlUserPaneHitTestUPP(userRoutine)
    #define NewControlUserPaneTrackingProc(userRoutine)         NewControlUserPaneTrackingUPP(userRoutine)
    #define NewControlUserPaneIdleProc(userRoutine)             NewControlUserPaneIdleUPP(userRoutine)
    #define NewControlUserPaneKeyDownProc(userRoutine)          NewControlUserPaneKeyDownUPP(userRoutine)
    #define NewControlUserPaneActivateProc(userRoutine)         NewControlUserPaneActivateUPP(userRoutine)
    #define NewControlUserPaneFocusProc(userRoutine)            NewControlUserPaneFocusUPP(userRoutine)
    #define NewControlUserPaneBackgroundProc(userRoutine)       NewControlUserPaneBackgroundUPP(userRoutine)
    #define CallControlUserPaneDrawProc(userRoutine, control, part) InvokeControlUserPaneDrawUPP(control, part, userRoutine)
    #define CallControlUserPaneHitTestProc(userRoutine, control, where) InvokeControlUserPaneHitTestUPP(control, where, userRoutine)
    #define CallControlUserPaneTrackingProc(userRoutine, control, startPt, actionProc) InvokeControlUserPaneTrackingUPP(control, startPt, actionProc, userRoutine)
    #define CallControlUserPaneIdleProc(userRoutine, control)   InvokeControlUserPaneIdleUPP(control, userRoutine)
    #define CallControlUserPaneKeyDownProc(userRoutine, control, keyCode, charCode, modifiers) InvokeControlUserPaneKeyDownUPP(control, keyCode, charCode, modifiers, userRoutine)
    #define CallControlUserPaneActivateProc(userRoutine, control, activating) InvokeControlUserPaneActivateUPP(control, activating, userRoutine)
    #define CallControlUserPaneFocusProc(userRoutine, control, action) InvokeControlUserPaneFocusUPP(control, action, userRoutine)
    #define CallControlUserPaneBackgroundProc(userRoutine, control, info) InvokeControlUserPaneBackgroundUPP(control, info, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*
  ------------------------------------------------------------------------------------------
    o EDIT TEXT (CDEF 17)
  ------------------------------------------------------------------------------------------
*/
/* Edit Text proc IDs */
enum {
  kControlEditTextProc          = 272,
  kControlEditTextPasswordProc  = 274
};

/* proc IDs available with Appearance 1.1 or later */
enum {
  kControlEditTextInlineInputProc = 276 /* Can't combine with the other variants*/
};

/* Control Kind Tag */
enum {
  kControlKindEditText          = FOUR_CHAR_CODE('etxt')
};

/* Creation API: Carbon only */
/*
 *  CreateEditTextControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateEditTextControl(
  WindowRef                    window,
  const Rect *                 boundsRect,
  CFStringRef                  text,
  Boolean                      isPassword,
  Boolean                      useInlineInput,
  const ControlFontStyleRec *  style,                /* can be NULL */
  ControlRef *                 outControl);



/* Tagged data supported by edit text */
enum {
  kControlEditTextStyleTag      = kControlFontStyleTag, /* ControlFontStyleRec*/
  kControlEditTextTextTag       = FOUR_CHAR_CODE('text'), /* Buffer of chars - you supply the buffer*/
  kControlEditTextTEHandleTag   = FOUR_CHAR_CODE('than'), /* The TEHandle of the text edit record*/
  kControlEditTextKeyFilterTag  = kControlKeyFilterTag,
  kControlEditTextSelectionTag  = FOUR_CHAR_CODE('sele'), /* ControlEditTextSelectionRec*/
  kControlEditTextPasswordTag   = FOUR_CHAR_CODE('pass') /* The clear text password text*/
};

/* tags available with Appearance 1.1 or later */
enum {
  kControlEditTextKeyScriptBehaviorTag = FOUR_CHAR_CODE('kscr'), /* ControlKeyScriptBehavior. Defaults to "PrefersRoman" for password fields,*/
                                        /*       or "AllowAnyScript" for non-password fields.*/
  kControlEditTextLockedTag     = FOUR_CHAR_CODE('lock'), /* Boolean. Locking disables editability.*/
  kControlEditTextFixedTextTag  = FOUR_CHAR_CODE('ftxt'), /* Like the normal text tag, but fixes inline input first*/
  kControlEditTextValidationProcTag = FOUR_CHAR_CODE('vali'), /* ControlEditTextValidationUPP. Called when a key filter can't be: after cut, paste, etc.*/
  kControlEditTextInlinePreUpdateProcTag = FOUR_CHAR_CODE('prup'), /* TSMTEPreUpdateUPP and TSMTEPostUpdateUpp. For use with inline input variant...*/
  kControlEditTextInlinePostUpdateProcTag = FOUR_CHAR_CODE('poup') /* ...The refCon parameter will contain the ControlRef.*/
};


/*
 *  Discussion:
 *    EditText ControlData tags available with MacOSX and later.
 */
enum {

  /*
   * Extract the content of the edit text field as a CFString.  Don't
   * forget that you own the returned CFStringRef and are responsible
   * for CFReleasing it.
   */
  kControlEditTextCFStringTag   = FOUR_CHAR_CODE('cfst'), /* CFStringRef (Also available on CarbonLib 1.5)*/

  /*
   * Extract the content of the edit text field as a CFString, if it is
   * a password field.  Don't forget that you own the returned
   * CFStringRef and are responsible for CFReleasing it.
   */
  kControlEditTextPasswordCFStringTag = FOUR_CHAR_CODE('pwcf') /* CFStringRef*/
};




/* Structure for getting the edit text selection */
struct ControlEditTextSelectionRec {
  SInt16              selStart;
  SInt16              selEnd;
};
typedef struct ControlEditTextSelectionRec ControlEditTextSelectionRec;
typedef ControlEditTextSelectionRec *   ControlEditTextSelectionPtr;
typedef CALLBACK_API( void , ControlEditTextValidationProcPtr )(ControlRef control);
typedef STACK_UPP_TYPE(ControlEditTextValidationProcPtr)        ControlEditTextValidationUPP;
/*
 *  NewControlEditTextValidationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ControlEditTextValidationUPP )
NewControlEditTextValidationUPP(ControlEditTextValidationProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppControlEditTextValidationProcInfo = 0x000000C0 };  /* pascal no_return_value Func(4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ControlEditTextValidationUPP) NewControlEditTextValidationUPP(ControlEditTextValidationProcPtr userRoutine) { return (ControlEditTextValidationUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlEditTextValidationProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewControlEditTextValidationUPP(userRoutine) (ControlEditTextValidationUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppControlEditTextValidationProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeControlEditTextValidationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeControlEditTextValidationUPP(ControlEditTextValidationUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeControlEditTextValidationUPP(ControlEditTextValidationUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeControlEditTextValidationUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeControlEditTextValidationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeControlEditTextValidationUPP(
  ControlRef                    control,
  ControlEditTextValidationUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeControlEditTextValidationUPP(ControlRef control, ControlEditTextValidationUPP userUPP) { CALL_ONE_PARAMETER_UPP(userUPP, uppControlEditTextValidationProcInfo, control); }
  #else
    #define InvokeControlEditTextValidationUPP(control, userUPP) CALL_ONE_PARAMETER_UPP((userUPP), uppControlEditTextValidationProcInfo, (control))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewControlEditTextValidationProc(userRoutine)       NewControlEditTextValidationUPP(userRoutine)
    #define CallControlEditTextValidationProc(userRoutine, control) InvokeControlEditTextValidationUPP(control, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*--------------------------------------------------------------------------------------*/
/*  o STATIC TEXT (CDEF 18)                                                             */
/*--------------------------------------------------------------------------------------*/
/* Static Text proc IDs */
enum {
  kControlStaticTextProc        = 288
};

/* Control Kind Tag */
enum {
  kControlKindStaticText        = FOUR_CHAR_CODE('stxt')
};

/* Creation API: Carbon only */
/*
 *  CreateStaticTextControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateStaticTextControl(
  WindowRef                    window,
  const Rect *                 boundsRect,
  CFStringRef                  text,
  const ControlFontStyleRec *  style,
  ControlRef *                 outControl);



/* Tagged data supported by static text */
enum {
  kControlStaticTextStyleTag    = kControlFontStyleTag, /* ControlFontStyleRec*/
  kControlStaticTextTextTag     = FOUR_CHAR_CODE('text'), /* Copy of text*/
  kControlStaticTextTextHeightTag = FOUR_CHAR_CODE('thei') /* SInt16*/
};

/* Tags available with appearance 1.1 or later */
enum {
  kControlStaticTextTruncTag    = FOUR_CHAR_CODE('trun') /* TruncCode (-1 means no truncation)*/
};

/* Tags available with Mac OS X or later */
enum {
  kControlStaticTextCFStringTag = FOUR_CHAR_CODE('cfst') /* CFStringRef (Also available on CarbonLib 1.5)*/
};


/*--------------------------------------------------------------------------------------*/
/*  o PICTURE CONTROL (CDEF 19)                                                         */
/*--------------------------------------------------------------------------------------*/
/*  Value parameter should contain the ID of the picture you wish to display when       */
/*  creating controls of this type. If you don't want the control tracked at all, use   */
/*  the 'no track' variant.                                                             */
/* Picture control proc IDs */
enum {
  kControlPictureProc           = 304,
  kControlPictureNoTrackProc    = 305   /* immediately returns kControlPicturePart*/
};

/* Control Kind Tag */
enum {
  kControlKindPicture           = FOUR_CHAR_CODE('pict')
};

/* Creation API: Carbon only */
/*
 *  CreatePictureControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreatePictureControl(
  WindowRef                         window,
  const Rect *                      boundsRect,
  const ControlButtonContentInfo *  content,
  Boolean                           dontTrack,
  ControlRef *                      outControl);


/* Tagged data supported by picture controls */
enum {
  kControlPictureHandleTag      = FOUR_CHAR_CODE('pich') /* PicHandle*/
};

/*--------------------------------------------------------------------------------------*/
/*  o ICON CONTROL (CDEF 20)                                                            */
/*--------------------------------------------------------------------------------------*/
/*  Value parameter should contain the ID of the ICON or cicn you wish to display when  */
/*  creating controls of this type. If you don't want the control tracked at all, use   */
/*  the 'no track' variant.                                                             */
/* Icon control proc IDs */
enum {
  kControlIconProc              = 320,
  kControlIconNoTrackProc       = 321,  /* immediately returns kControlIconPart*/
  kControlIconSuiteProc         = 322,
  kControlIconSuiteNoTrackProc  = 323   /* immediately returns kControlIconPart*/
};

enum {
                                        /* icon ref controls may have either an icon, color icon, icon suite, or icon ref.*/
                                        /* for data other than icon, you must set the data by passing a*/
                                        /* ControlButtonContentInfo to SetControlData*/
  kControlIconRefProc           = 324,
  kControlIconRefNoTrackProc    = 325   /* immediately returns kControlIconPart*/
};

/* Control Kind Tag */
enum {
  kControlKindIcon              = FOUR_CHAR_CODE('icon')
};

/*
 *  CreateIconControl()
 *  
 *  Summary:
 *    Creates an Icon control at a specific position in the specified
 *    window.
 *  
 *  Discussion:
 *    Icon controls display an icon that (optionally) hilites when
 *    clicked on. On Mac OS X, a root control will be created for the
 *    window if one does not already exist. If a root control exists
 *    for the window, the Icon control will be embedded into it.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The WindowRef into which the Icon control will be created.
 *    
 *    inBoundsRect:
 *      The desired position (in coordinates local to the window's
 *      port) for the Icon control.
 *    
 *    inIconContent:
 *      The descriptor for the icon you want the control to display.
 *      Mac OS X and CarbonLib 1.5 (and beyond) support all of the icon
 *      content types. Prior to CarbonLib 1.5, the only content types
 *      that are properly respected are kControlContentIconSuiteRes,
 *      kControlContentCIconRes, and kControlContentICONRes.
 *    
 *    inDontTrack:
 *      A Boolean value indicating whether the control should hilite
 *      when it is clicked on. False means hilite and track the mouse.
 *    
 *    outControl:
 *      On successful output, outControl will contain a reference to
 *      the Icon control.
 *  
 *  Result:
 *    An OSStatus code indicating success or failure.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateIconControl(
  WindowRef                         inWindow,
  const Rect *                      inBoundsRect,
  const ControlButtonContentInfo *  inIconContent,
  Boolean                           inDontTrack,
  ControlRef *                      outControl);


/* Tagged data supported by icon controls */
enum {
  kControlIconTransformTag      = FOUR_CHAR_CODE('trfm'), /* IconTransformType*/
  kControlIconAlignmentTag      = FOUR_CHAR_CODE('algn') /* IconAlignmentType*/
};

/* Tags available with appearance 1.1 or later */
enum {
  kControlIconResourceIDTag     = FOUR_CHAR_CODE('ires'), /* SInt16 resource ID of icon to use*/
  kControlIconContentTag        = FOUR_CHAR_CODE('cont') /* accepts a ControlButtonContentInfo*/
};

/*--------------------------------------------------------------------------------------*/
/*  o WINDOW HEADER (CDEF 21)                                                           */
/*--------------------------------------------------------------------------------------*/
/* Window Header proc IDs */
enum {
  kControlWindowHeaderProc      = 336,  /* normal header*/
  kControlWindowListViewHeaderProc = 337 /* variant for list views - no bottom line*/
};

/* Control Kind Tag */
enum {
  kControlKindWindowHeader      = FOUR_CHAR_CODE('whed')
};

/* Creation API: Carbon Only */
/*
 *  CreateWindowHeaderControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateWindowHeaderControl(
  WindowRef     window,
  const Rect *  boundsRect,
  Boolean       isListHeader,
  ControlRef *  outControl);


/*--------------------------------------------------------------------------------------*/
/*  o LIST BOX (CDEF 22)                                                                */
/*--------------------------------------------------------------------------------------*/
/*  Lists use an auxiliary resource to define their format. The resource type used is   */
/*  'ldes' and a definition for it can be found in Appearance.r. The resource ID for    */
/*  the ldes is passed in the 'value' parameter when creating the control. You may pass */
/*  zero in value. This tells the List Box control to not use a resource. The list will */
/*  be created with default values, and will use the standard LDEF (0). You can change  */
/*  the list by getting the list handle. You can set the LDEF to use by using the tag   */
/*  below (kControlListBoxLDEFTag)                                                      */
/* List Box proc IDs */
enum {
  kControlListBoxProc           = 352,
  kControlListBoxAutoSizeProc   = 353
};

/* Control Kind Tag */
enum {
  kControlKindListBox           = FOUR_CHAR_CODE('lbox')
};

/* Creation API: Carbon Only */
/*
 *  CreateListBoxControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateListBoxControl(
  WindowRef            window,
  const Rect *         boundsRect,
  Boolean              autoSize,
  SInt16               numRows,
  SInt16               numColumns,
  Boolean              horizScroll,
  Boolean              vertScroll,
  SInt16               cellHeight,
  SInt16               cellWidth,
  Boolean              hasGrowSpace,
  const ListDefSpec *  listDef,
  ControlRef *         outControl);


/* Tagged data supported by list box */
enum {
  kControlListBoxListHandleTag  = FOUR_CHAR_CODE('lhan'), /* ListHandle*/
  kControlListBoxKeyFilterTag   = kControlKeyFilterTag, /* ControlKeyFilterUPP*/
  kControlListBoxFontStyleTag   = kControlFontStyleTag /* ControlFontStyleRec*/
};

/* New tags in 1.0.1 or later */
enum {
  kControlListBoxDoubleClickTag = FOUR_CHAR_CODE('dblc'), /* Boolean. Was last click a double-click?*/
  kControlListBoxLDEFTag        = FOUR_CHAR_CODE('ldef') /* SInt16. ID of LDEF to use.*/
};

/*--------------------------------------------------------------------------------------*/
/*  o PUSH BUTTON (CDEF 23)                                                             */
/*--------------------------------------------------------------------------------------*/
/*  The new standard checkbox and radio button controls support a "mixed" value that    */
/*  indicates that the current setting contains a mixed set of on and off values. The   */
/*  control value used to display this indication is defined in Controls.h:             */
/*                                                                                      */
/*      kControlCheckBoxMixedValue = 2                                                  */
/*                                                                                      */
/*  Two new variants of the standard pushbutton have been added to the standard control */
/*  suite that draw a color icon next to the control title. One variant draws the icon  */
/*  on the left side, the other draws it on the right side (when the system justifica-  */
/*  tion is right to left, these are reversed).                                         */
/*                                                                                      */
/*  When either of the icon pushbuttons are created, the contrlMax field of the control */
/*  record is used to determine the ID of the 'cicn' resource drawn in the pushbutton.  */
/*                                                                                      */
/*  In addition, a push button can now be told to draw with a default outline using the */
/*  SetControlData routine with the kControlPushButtonDefaultTag below.                 */
/*                                                                                      */
/*  A push button may also be marked using the kControlPushButtonCancelTag. This has    */
/*  no visible representation, but does cause the button to play the CancelButton theme */
/*  sound instead of the regular pushbutton theme sound when pressed.                   */
/*                                                                                      */
/* Theme Push Button/Check Box/Radio Button proc IDs */
enum {
  kControlPushButtonProc        = 368,
  kControlCheckBoxProc          = 369,
  kControlRadioButtonProc       = 370,
  kControlPushButLeftIconProc   = 374,  /* Standard pushbutton with left-side icon*/
  kControlPushButRightIconProc  = 375   /* Standard pushbutton with right-side icon*/
};

/* Variants with Appearance 1.1 or later */
enum {
  kControlCheckBoxAutoToggleProc = 371,
  kControlRadioButtonAutoToggleProc = 372
};

/* Push Button Icon Alignments */
typedef UInt16 ControlPushButtonIconAlignment;
enum {
  kControlPushButtonIconOnLeft  = 6,
  kControlPushButtonIconOnRight = 7
};

/* Control Kind Tag */
enum {
  kControlKindPushButton        = FOUR_CHAR_CODE('push'),
  kControlKindPushIconButton    = FOUR_CHAR_CODE('picn'),
  kControlKindRadioButton       = FOUR_CHAR_CODE('rdio'),
  kControlKindCheckBox          = FOUR_CHAR_CODE('cbox')
};

/* Creation APIs: Carbon Only */
/*
 *  CreatePushButtonControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreatePushButtonControl(
  WindowRef     window,
  const Rect *  boundsRect,
  CFStringRef   title,
  ControlRef *  outControl);


/*
 *  CreatePushButtonWithIconControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreatePushButtonWithIconControl(
  WindowRef                        window,
  const Rect *                     boundsRect,
  CFStringRef                      title,
  ControlButtonContentInfo *       icon,
  ControlPushButtonIconAlignment   iconAlignment,
  ControlRef *                     outControl);


/*
 *  CreateRadioButtonControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateRadioButtonControl(
  WindowRef     window,
  const Rect *  boundsRect,
  CFStringRef   title,
  SInt32        initialValue,
  Boolean       autoToggle,
  ControlRef *  outControl);


/*
 *  CreateCheckBoxControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateCheckBoxControl(
  WindowRef     window,
  const Rect *  boundsRect,
  CFStringRef   title,
  SInt32        initialValue,
  Boolean       autoToggle,
  ControlRef *  outControl);




/* Tagged data supported by standard buttons */
enum {
  kControlPushButtonDefaultTag  = FOUR_CHAR_CODE('dflt'), /* default ring flag*/
  kControlPushButtonCancelTag   = FOUR_CHAR_CODE('cncl') /* cancel button flag (1.1 and later)*/
};

/*--------------------------------------------------------------------------------------*/
/*  o SCROLL BAR (CDEF 24)                                                              */
/*--------------------------------------------------------------------------------------*/
/*  This is the new Appearance scroll bar.                                              */
/*                                                                                      */
/* Theme Scroll Bar proc IDs */
enum {
  kControlScrollBarProc         = 384,  /* normal scroll bar*/
  kControlScrollBarLiveProc     = 386   /* live scrolling variant*/
};

/* Control Kind Tag */
enum {
  kControlKindScrollBar         = FOUR_CHAR_CODE('sbar')
};

/* Creation API: Carbon Only */
/*
 *  CreateScrollBarControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateScrollBarControl(
  WindowRef          window,
  const Rect *       boundsRect,
  SInt32             value,
  SInt32             minimum,
  SInt32             maximum,
  SInt32             viewSize,
  Boolean            liveTracking,
  ControlActionUPP   liveTrackingProc,
  ControlRef *       outControl);


/* These tags are available in Mac OS X or later */
enum {
  kControlScrollBarShowsArrowsTag = FOUR_CHAR_CODE('arro') /* Boolean whether or not to draw the scroll arrows*/
};


/*--------------------------------------------------------------------------------------*/
/*  o POPUP BUTTON (CDEF 25)                                                            */
/*--------------------------------------------------------------------------------------*/
/*  This is the new Appearance Popup Button. It takes the same variants and does the    */
/*  same overloading as the previous popup menu control. There are some differences:    */
/*                                                                                      */
/*  Passing in a menu ID of -12345 causes the popup not to try and get the menu from a  */
/*  resource. Instead, you can build the menu and later stuff the MenuRef field in      */
/*  the popup data information.                                                         */
/*                                                                                      */
/*  You can pass -1 in the Max parameter to have the control calculate the width of the */
/*  title on its own instead of guessing and then tweaking to get it right. It adds the */
/*  appropriate amount of space between the title and the popup.                        */
/*                                                                                      */
/* Theme Popup Button proc IDs */
enum {
  kControlPopupButtonProc       = 400,
  kControlPopupFixedWidthVariant = 1 << 0,
  kControlPopupVariableWidthVariant = 1 << 1,
  kControlPopupUseAddResMenuVariant = 1 << 2,
  kControlPopupUseWFontVariant  = kControlUsesOwningWindowsFontVariant
};

/* Control Kind Tag */
enum {
  kControlKindPopupButton       = FOUR_CHAR_CODE('popb')
};

/* Creation API: Carbon Only */
/*
 *  CreatePopupButtonControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreatePopupButtonControl(
  WindowRef     window,
  const Rect *  boundsRect,
  CFStringRef   title,
  SInt16        menuID,
  Boolean       variableWidth,
  SInt16        titleWidth,
  SInt16        titleJustification,
  Style         titleStyle,
  ControlRef *  outControl);




/* These tags are available in 1.0.1 or later of Appearance */
enum {
  kControlPopupButtonMenuHandleTag = FOUR_CHAR_CODE('mhan'), /* MenuRef*/
  kControlPopupButtonMenuRefTag = FOUR_CHAR_CODE('mhan'), /* MenuRef*/
  kControlPopupButtonMenuIDTag  = FOUR_CHAR_CODE('mnid') /* SInt16*/
};

/* These tags are available in 1.1 or later of Appearance */
enum {
  kControlPopupButtonExtraHeightTag = FOUR_CHAR_CODE('exht'), /* SInt16 - extra vertical whitespace within the button*/
  kControlPopupButtonOwnedMenuRefTag = FOUR_CHAR_CODE('omrf') /* MenuRef*/
};

/* These tags are available in Mac OS X */
enum {
  kControlPopupButtonCheckCurrentTag = FOUR_CHAR_CODE('chck') /* Boolean    - whether the popup puts a checkmark next to the current item (defaults to true)*/
};


/*--------------------------------------------------------------------------------------*/
/*  o RADIO GROUP (CDEF 26)                                                             */
/*--------------------------------------------------------------------------------------*/
/*  This control implements a radio group. It is an embedding control and can therefore */
/*  only be used when a control hierarchy is established for its owning window. You     */
/*  should only embed radio buttons within it. As radio buttons are embedded into it,   */
/*  the group sets up its value, min, and max to represent the number of embedded items.*/
/*  The current value of the control is the index of the sub-control that is the current*/
/*  'on' radio button. To get the current radio button control handle, you can use the  */
/*  control manager call GetIndSubControl, passing in the value of the radio group.     */
/*                                                                                      */
/*  NOTE: This control is only available with Appearance 1.0.1.                         */
/* Radio Group Proc ID */
enum {
  kControlRadioGroupProc        = 416
};

/* Control Kind Tag */
enum {
  kControlKindRadioGroup        = FOUR_CHAR_CODE('rgrp')
};

/* Creation API: Carbon Only */
/*
 *  CreateRadioGroupControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateRadioGroupControl(
  WindowRef     window,
  const Rect *  boundsRect,
  ControlRef *  outControl);


/*--------------------------------------------------------------------------------------*/
/*  o SCROLL TEXT BOX (CDEF 27)                                                         */
/*--------------------------------------------------------------------------------------*/
/*  This control implements a scrolling box of (non-editable) text. This is useful for  */
/*  credits in about boxes, etc.                                                        */
/*  The standard version of this control has a scroll bar, but the autoscrolling        */
/*  variant does not. The autoscrolling variant needs two pieces of information to      */
/*  work: delay (in ticks) before the scrolling starts, and time (in ticks) between     */
/*  scrolls. It will scroll one pixel at a time, unless changed via SetControlData.     */
/*                                                                                      */
/*  Parameter                   What Goes Here                                          */
/*  -------------------         ----------------------------------------------------    */
/*  Value                       Resource ID of 'TEXT'/'styl' content.                   */
/*  Min                         Scroll start delay (in ticks)                       .   */
/*  Max                         Delay (in ticks) between scrolls.                       */
/*                                                                                      */
/*  NOTE: This control is only available with Appearance 1.1.                           */
/* Scroll Text Box Proc IDs */
enum {
  kControlScrollTextBoxProc     = 432,
  kControlScrollTextBoxAutoScrollProc = 433
};

/* Control Kind Tag */
enum {
  kControlKindScrollingTextBox  = FOUR_CHAR_CODE('stbx')
};

/* Creation API: Carbon Only */
/*
 *  CreateScrollingTextBoxControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateScrollingTextBoxControl(
  WindowRef     window,
  const Rect *  boundsRect,
  SInt16        contentResID,
  Boolean       autoScroll,
  UInt32        delayBeforeAutoScroll,
  UInt32        delayBetweenAutoScroll,
  UInt16        autoScrollAmount,
  ControlRef *  outControl);


/* Tagged data supported by Scroll Text Box */
enum {
  kControlScrollTextBoxDelayBeforeAutoScrollTag = FOUR_CHAR_CODE('stdl'), /* UInt32 (ticks until autoscrolling starts)*/
  kControlScrollTextBoxDelayBetweenAutoScrollTag = FOUR_CHAR_CODE('scdl'), /* UInt32 (ticks between scrolls)*/
  kControlScrollTextBoxAutoScrollAmountTag = FOUR_CHAR_CODE('samt'), /* UInt16 (pixels per scroll) -- defaults to 1*/
  kControlScrollTextBoxContentsTag = FOUR_CHAR_CODE('tres'), /* SInt16 (resource ID of 'TEXT'/'styl') -- write only!*/
  kControlScrollTextBoxAnimatingTag = FOUR_CHAR_CODE('anim') /* Boolean (whether the text box should auto-scroll)*/
};


/*--------------------------------------------------------------------------------------*/
/*  o DISCLOSURE BUTTON                                                                 */
/*--------------------------------------------------------------------------------------*/
/*
 *  CreateDisclosureButtonControl()
 *  
 *  Summary:
 *    Creates a new instance of the Disclosure Button Control.
 *  
 *  Discussion:
 *    CreateDisclosureButtonControl is preferred over NewControl
 *    because it allows you to specify the exact set of parameters
 *    required to create the control without overloading parameter
 *    semantics. The initial minimum of the Disclosure Button will be
 *    kControlDisclosureButtonClosed, and the maximum will be
 *    kControlDisclosureButtonDisclosed.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The WindowRef in which to create the control.
 *    
 *    inBoundsRect:
 *      The bounding rectangle for the control. The height of the
 *      control is fixed and the control will be centered vertically
 *      within the rectangle you specify.
 *    
 *    inValue:
 *      The initial value; either kControlDisclosureButtonClosed or
 *      kControlDisclosureButtonDisclosed.
 *    
 *    inAutoToggles:
 *      A boolean value indicating whether its value should change
 *      automatically after tracking the mouse.
 *    
 *    outControl:
 *      On successful exit, this will contain the new control.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateDisclosureButtonControl(
  WindowRef     inWindow,
  const Rect *  inBoundsRect,
  SInt32        inValue,
  Boolean       inAutoToggles,
  ControlRef *  outControl);



/* Control Kind Tag */
enum {
  kControlKindDisclosureButton  = FOUR_CHAR_CODE('disb')
};


/*
 *  Discussion:
 *    Disclosure Button Values
 */
enum {

  /*
   * The control be drawn suggesting a closed state.
   */
  kControlDisclosureButtonClosed = 0,

  /*
   * The control will be drawn suggesting an open state.
   */
  kControlDisclosureButtonDisclosed = 1
};


/*--------------------------------------------------------------------------------------*/
/*  o ROUND BUTTON                                                                      */
/*--------------------------------------------------------------------------------------*/

/*
 *  ControlRoundButtonSize
 *  
 *  Discussion:
 *    Button Sizes
 */
typedef SInt16 ControlRoundButtonSize;
enum {

  /*
   * A 20 pixel diameter button.
   */
  kControlRoundButtonNormalSize = kControlSizeNormal,

  /*
   * A 25 pixel diameter button.
   */
  kControlRoundButtonLargeSize  = kControlSizeLarge
};

/* Data tags supported by the round button controls */
enum {
  kControlRoundButtonContentTag = FOUR_CHAR_CODE('cont'), /* ControlButtonContentInfo*/
  kControlRoundButtonSizeTag    = kControlSizeTag /* ControlRoundButtonSize*/
};

/* Control Kind Tag */
enum {
  kControlKindRoundButton       = FOUR_CHAR_CODE('rndb')
};

/*
 *  CreateRoundButtonControl()
 *  
 *  Summary:
 *    Creates a new instance of the Round Button Control.
 *  
 *  Discussion:
 *    CreateRoundButtonControl is preferred over NewControl because it
 *    allows you to specify the exact set of parameters required to
 *    create the control without overloading parameter semantics.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The WindowRef in which to create the control.
 *    
 *    inBoundsRect:
 *      The bounding rectangle for the control. The height and width of
 *      the control is fixed (specified by the ControlRoundButtonSize
 *      parameter) and the control will be centered within the
 *      rectangle you specify.
 *    
 *    inSize:
 *      The button size; either kControlRoundButtonNormalSize or
 *      kControlRoundButtonLargeSize.
 *    
 *    inContent:
 *      Any optional content displayed in the button. Currently only
 *      kControlContentIconRef is supported.
 *    
 *    outControl:
 *      On successful exit, this will contain the new control.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateRoundButtonControl(
  WindowRef                   inWindow,
  const Rect *                inBoundsRect,
  ControlRoundButtonSize      inSize,
  ControlButtonContentInfo *  inContent,
  ControlRef *                outControl);



/*--------------------------------------------------------------------------------------*/
/*    o DATA BROWSER                                                                    */
/*     (CDEF 29)                                                                        */
/*--------------------------------------------------------------------------------------*/
/*  This control implements a user interface component for browsing (optionally)        */
/*  hiearchical data structures. The browser supports multiple presentation styles      */
/*  including, but not limited to:                                                      */
/*                                                                                      */
/*      kDataBrowserListView   - items and item properties in                           */
/*                               multi-column (optionally outline) format               */
/*      kDataBrowserColumnView - in-place browsing using fixed navigation columns       */
/*                                                                                      */
/*  The browser manages all view styles through a single high-level interface.          */
/*  The high-level interface makes the following assumptions:                           */
/*                                                                                      */
/*      - Items have unique 32-bit identifiers (0 is reserved)                          */
/*                                                                                      */
/*      - Items have two kinds of named and typed properties:                           */
/*           - Predefined attribute properties ( < 1024 )                               */
/*             (including some display properties)                                      */
/*           - Client-defined display properties ( >= 1024 )                            */
/*                                                                                      */
/*      - Some items are containers of other items                                      */
/*      - Items may be sorted by any property                                           */
/*                                                                                      */
/*  Because a browser doesn't know all details about the type of objects it manages,    */
/*  some implementation responsibility is best handled by its client. The client must   */
/*  provide a set of callback routines which define the item hierarchy and help to      */
/*  populate the browser with items. The client may also provide callbacks for handling */
/*  custom data types and doing low-level event management.                             */
/*                                                                                      */
/*  The API is subdivided into a "universal" set of routines that applies to all view   */
/*  styles, and a set of routines unique to each view style. kDataBrowserListView and   */
/*  kDataBrowserColumnView share an (internal) TableView abstract base class. The       */
/*  TableView formatting options and API applies to both of these view styles.          */
/*                                                                                      */
/*  NOTE: This control is only available with CarbonLib 1.1.                            */
/*                                                                                      */
/*  NOTE: This control must be created with the CreateDataBrowserControl API in         */
/*        CarbonLib 1.1 through 1.4. In Mac OS X and CarbonLib 1.5 and later, you       */
/*        may use the control's procID (29) to create the control with NewControl       */
/*        or with a 'CNTL' resource.                                                    */
/* Control Kind Tag */
enum {
  kControlKindDataBrowser       = FOUR_CHAR_CODE('datb')
};

/* Error Codes */
enum {
  errDataBrowserNotConfigured   = -4970,
  errDataBrowserItemNotFound    = -4971,
  errDataBrowserItemNotAdded    = -4975,
  errDataBrowserPropertyNotFound = -4972,
  errDataBrowserInvalidPropertyPart = -4973,
  errDataBrowserInvalidPropertyData = -4974,
  errDataBrowserPropertyNotSupported = -4979 /* Return from DataBrowserGetSetItemDataProc */
};

enum {
                                        /* Generic Control Tags */
  kControlDataBrowserIncludesFrameAndFocusTag = FOUR_CHAR_CODE('brdr'), /* Boolean */
  kControlDataBrowserKeyFilterTag = kControlEditTextKeyFilterTag,
  kControlDataBrowserEditTextKeyFilterTag = kControlDataBrowserKeyFilterTag,
  kControlDataBrowserEditTextValidationProcTag = kControlEditTextValidationProcTag
};

/* Data Browser View Styles */
typedef OSType                          DataBrowserViewStyle;
enum {
  kDataBrowserNoView            = 0x3F3F3F3F, /* Error State */
  kDataBrowserListView          = FOUR_CHAR_CODE('lstv'),
  kDataBrowserColumnView        = FOUR_CHAR_CODE('clmv')
};

/* Selection Flags */
typedef UInt32 DataBrowserSelectionFlags;
enum {
  kDataBrowserDragSelect        = 1 << 0, /* * ListMgr lNoRect */
  kDataBrowserSelectOnlyOne     = 1 << 1, /* * ListMgr lOnlyOne */
  kDataBrowserResetSelection    = 1 << 2, /* * ListMgr lNoExtend */
  kDataBrowserCmdTogglesSelection = 1 << 3, /* * ListMgr lUseSense */
  kDataBrowserNoDisjointSelection = 1 << 4, /* * ListMgr lNoDisjoint */
  kDataBrowserAlwaysExtendSelection = 1 << 5, /* * ListMgr lExtendDrag */
  kDataBrowserNeverEmptySelectionSet = 1 << 6 /* * ListMgr lNoNilHilite */
};

/* Data Browser Sorting */
typedef UInt16 DataBrowserSortOrder;
enum {
  kDataBrowserOrderUndefined    = 0,    /* Not currently supported */
  kDataBrowserOrderIncreasing   = 1,
  kDataBrowserOrderDecreasing   = 2
};

/* Data Browser Item Management */
typedef UInt32 DataBrowserItemID;
enum {
  kDataBrowserNoItem            = 0L    /* Reserved DataBrowserItemID */
};

typedef UInt32 DataBrowserItemState;
enum {
  kDataBrowserItemNoState       = 0,
  kDataBrowserItemAnyState      = (unsigned long)(-1),
  kDataBrowserItemIsSelected    = 1 << 0,
  kDataBrowserContainerIsOpen   = 1 << 1,
  kDataBrowserItemIsDragTarget  = 1 << 2 /* During a drag operation */
};

/* Options for use with RevealDataBrowserItem */
typedef UInt8 DataBrowserRevealOptions;
enum {
  kDataBrowserRevealOnly        = 0,
  kDataBrowserRevealAndCenterInView = 1 << 0,
  kDataBrowserRevealWithoutSelecting = 1 << 1
};

/* Set operations for use with SetDataBrowserSelectedItems */
typedef UInt32 DataBrowserSetOption;
enum {
  kDataBrowserItemsAdd          = 0,    /* add specified items to existing set */
  kDataBrowserItemsAssign       = 1,    /* assign destination set to specified items */
  kDataBrowserItemsToggle       = 2,    /* toggle membership state of specified items */
  kDataBrowserItemsRemove       = 3     /* remove specified items from existing set */
};

/* Commands for use with MoveDataBrowserSelectionAnchor */
typedef UInt32 DataBrowserSelectionAnchorDirection;
enum {
  kDataBrowserSelectionAnchorUp = 0,
  kDataBrowserSelectionAnchorDown = 1,
  kDataBrowserSelectionAnchorLeft = 2,
  kDataBrowserSelectionAnchorRight = 3
};

/* Edit menu command IDs for use with Enable/ExecuteDataBrowserEditCommand */
typedef UInt32 DataBrowserEditCommand;
enum {
  kDataBrowserEditMsgUndo       = kHICommandUndo,
  kDataBrowserEditMsgRedo       = kHICommandRedo,
  kDataBrowserEditMsgCut        = kHICommandCut,
  kDataBrowserEditMsgCopy       = kHICommandCopy,
  kDataBrowserEditMsgPaste      = kHICommandPaste,
  kDataBrowserEditMsgClear      = kHICommandClear,
  kDataBrowserEditMsgSelectAll  = kHICommandSelectAll
};

/* Notifications used in DataBrowserItemNotificationProcPtr */
typedef UInt32 DataBrowserItemNotification;
enum {
  kDataBrowserItemAdded         = 1,    /* The specified item has been added to the browser */
  kDataBrowserItemRemoved       = 2,    /* The specified item has been removed from the browser */
  kDataBrowserEditStarted       = 3,    /* Starting an EditText session for specified item */
  kDataBrowserEditStopped       = 4,    /* Stopping an EditText session for specified item */
  kDataBrowserItemSelected      = 5,    /* Item has just been added to the selection set */
  kDataBrowserItemDeselected    = 6,    /* Item has just been removed from the selection set */
  kDataBrowserItemDoubleClicked = 7,
  kDataBrowserContainerOpened   = 8,    /* Container is open */
  kDataBrowserContainerClosing  = 9,    /* Container is about to close (and will real soon now, y'all) */
  kDataBrowserContainerClosed   = 10,   /* Container is closed (y'all come back now!) */
  kDataBrowserContainerSorting  = 11,   /* Container is about to be sorted (lock any volatile properties) */
  kDataBrowserContainerSorted   = 12,   /* Container has been sorted (you may release any property locks) */
  kDataBrowserUserToggledContainer = 16, /* _User_ requested container open/close state to be toggled */
  kDataBrowserTargetChanged     = 15,   /* The target has changed to the specified item */
  kDataBrowserUserStateChanged  = 13,   /* The user has reformatted the view for the target */
  kDataBrowserSelectionSetChanged = 14  /* The selection set has been modified (net result may be the same) */
};

/* DataBrowser Property Management */
/* 0-1023 reserved; >= 1024 for client use */
typedef UInt32 DataBrowserPropertyID;
enum {
                                        /* Predefined attribute properties, optional & non-display unless otherwise stated */
  kDataBrowserItemNoProperty    = 0L,   /* The anti-property (no associated data) */
  kDataBrowserItemIsActiveProperty = 1L, /* Boolean typed data (defaults to true) */
  kDataBrowserItemIsSelectableProperty = 2L, /* Boolean typed data (defaults to true) */
  kDataBrowserItemIsEditableProperty = 3L, /* Boolean typed data (defaults to false, used for editable properties) */
  kDataBrowserItemIsContainerProperty = 4L, /* Boolean typed data (defaults to false) */
  kDataBrowserContainerIsOpenableProperty = 5L, /* Boolean typed data (defaults to true) */
  kDataBrowserContainerIsClosableProperty = 6L, /* Boolean typed data (defaults to true) */
  kDataBrowserContainerIsSortableProperty = 7L, /* Boolean typed data (defaults to true) */
  kDataBrowserItemSelfIdentityProperty = 8L, /* kDataBrowserIconAndTextType (display property; ColumnView only) */
  kDataBrowserContainerAliasIDProperty = 9L, /* DataBrowserItemID (alias/symlink an item to a container item) */
  kDataBrowserColumnViewPreviewProperty = 10L, /* kDataBrowserCustomType (display property; ColumnView only) */
  kDataBrowserItemParentContainerProperty = 11L /* DataBrowserItemID (the parent of the specified item, used by ColumnView) */
};

/* DataBrowser Property Types (for display properties; i.e. ListView columns) */
/*      These are primarily presentation types (or styles) although         */
/*      they also imply a particular set of primitive types or structures.  */
typedef OSType                          DataBrowserPropertyType;
enum {
                                        /* == Corresponding data type or structure == */
  kDataBrowserCustomType        = 0x3F3F3F3F, /* No associated data, custom callbacks used */
  kDataBrowserIconType          = FOUR_CHAR_CODE('icnr'), /* IconRef, IconTransformType, RGBColor */
  kDataBrowserTextType          = FOUR_CHAR_CODE('text'), /* CFStringRef */
  kDataBrowserDateTimeType      = FOUR_CHAR_CODE('date'), /* DateTime or LongDateTime */
  kDataBrowserSliderType        = FOUR_CHAR_CODE('sldr'), /* Min, Max, Value */
  kDataBrowserCheckboxType      = FOUR_CHAR_CODE('chbx'), /* ThemeButtonValue */
  kDataBrowserProgressBarType   = FOUR_CHAR_CODE('prog'), /* Min, Max, Value */
  kDataBrowserRelevanceRankType = FOUR_CHAR_CODE('rank'), /* Min, Max, Value */
  kDataBrowserPopupMenuType     = FOUR_CHAR_CODE('menu'), /* MenuRef, Value */
  kDataBrowserIconAndTextType   = FOUR_CHAR_CODE('ticn') /* IconRef, CFStringRef, etc */
};

/* DataBrowser Property Parts */
/*      Visual components of a property type.      */
/*      For use with GetDataBrowserItemPartBounds. */
typedef OSType                          DataBrowserPropertyPart;
enum {
  kDataBrowserPropertyEnclosingPart = 0L,
  kDataBrowserPropertyContentPart = FOUR_CHAR_CODE('----'),
  kDataBrowserPropertyDisclosurePart = FOUR_CHAR_CODE('disc'),
  kDataBrowserPropertyTextPart  = kDataBrowserTextType,
  kDataBrowserPropertyIconPart  = kDataBrowserIconType,
  kDataBrowserPropertySliderPart = kDataBrowserSliderType,
  kDataBrowserPropertyCheckboxPart = kDataBrowserCheckboxType,
  kDataBrowserPropertyProgressBarPart = kDataBrowserProgressBarType,
  kDataBrowserPropertyRelevanceRankPart = kDataBrowserRelevanceRankType
};

/* Modify appearance/behavior of display properties */
typedef unsigned long                   DataBrowserPropertyFlags;
/* Low 8 bits apply to all property types */
enum {
  kDataBrowserUniversalPropertyFlagsMask = 0xFF,
  kDataBrowserPropertyIsMutable = 1 << 0,
  kDataBrowserDefaultPropertyFlags = 0 << 0,
  kDataBrowserUniversalPropertyFlags = kDataBrowserUniversalPropertyFlagsMask, /* support for an old name*/
  kDataBrowserPropertyIsEditable = kDataBrowserPropertyIsMutable /* support for an old name*/
};

/* Next 8 bits contain property-specific modifiers */
enum {
  kDataBrowserPropertyFlagsOffset = 8,
  kDataBrowserPropertyFlagsMask = 0xFF << kDataBrowserPropertyFlagsOffset,
  kDataBrowserCheckboxTriState  = 1 << kDataBrowserPropertyFlagsOffset, /* kDataBrowserCheckboxType*/
  kDataBrowserDateTimeRelative  = 1 << (kDataBrowserPropertyFlagsOffset), /* kDataBrowserDateTimeType */
  kDataBrowserDateTimeDateOnly  = 1 << (kDataBrowserPropertyFlagsOffset + 1), /* kDataBrowserDateTimeType */
  kDataBrowserDateTimeTimeOnly  = 1 << (kDataBrowserPropertyFlagsOffset + 2), /* kDataBrowserDateTimeType */
  kDataBrowserDateTimeSecondsToo = 1 << (kDataBrowserPropertyFlagsOffset + 3), /* kDataBrowserDateTimeType */
  kDataBrowserSliderPlainThumb  = kThemeThumbPlain << kDataBrowserPropertyFlagsOffset, /* kDataBrowserSliderType */
  kDataBrowserSliderUpwardThumb = kThemeThumbUpward << kDataBrowserPropertyFlagsOffset, /* kDataBrowserSliderType */
  kDataBrowserSliderDownwardThumb = kThemeThumbDownward << kDataBrowserPropertyFlagsOffset, /* kDataBrowserSliderType */
  kDataBrowserDoNotTruncateText = 3 << kDataBrowserPropertyFlagsOffset, /* kDataBrowserTextType && kDataBrowserIconAndTextType */
  kDataBrowserTruncateTextAtEnd = 2 << kDataBrowserPropertyFlagsOffset, /* kDataBrowserTextType && kDataBrowserIconAndTextType */
  kDataBrowserTruncateTextMiddle = 0 << kDataBrowserPropertyFlagsOffset, /* kDataBrowserTextType && kDataBrowserIconAndTextType */
  kDataBrowserTruncateTextAtStart = 1 << kDataBrowserPropertyFlagsOffset, /* kDataBrowserTextType && kDataBrowserIconAndTextType */
  kDataBrowserPropertyModificationFlags = kDataBrowserPropertyFlagsMask, /* support for an old name*/
  kDataBrowserRelativeDateTime  = kDataBrowserDateTimeRelative /* support for an old name*/
};

/*
   Next 8 bits contain viewStyle-specific modifiers 
   See individual ViewStyle sections below for flag definitions 
*/
enum {
  kDataBrowserViewSpecificFlagsOffset = 16,
  kDataBrowserViewSpecificFlagsMask = 0xFF << kDataBrowserViewSpecificFlagsOffset,
  kDataBrowserViewSpecificPropertyFlags = kDataBrowserViewSpecificFlagsMask /* support for an old name*/
};

/* High 8 bits are reserved for client application use */
enum {
  kDataBrowserClientPropertyFlagsOffset = 24,
  kDataBrowserClientPropertyFlagsMask = (unsigned long)(0xFF << kDataBrowserClientPropertyFlagsOffset)
};

/* Client defined property description */
struct DataBrowserPropertyDesc {
  DataBrowserPropertyID  propertyID;
  DataBrowserPropertyType  propertyType;
  DataBrowserPropertyFlags  propertyFlags;
};
typedef struct DataBrowserPropertyDesc  DataBrowserPropertyDesc;
/* Callback definition for use with ForEachDataBrowserItem */
typedef CALLBACK_API( void , DataBrowserItemProcPtr )(DataBrowserItemID item, DataBrowserItemState state, void *clientData);
typedef STACK_UPP_TYPE(DataBrowserItemProcPtr)                  DataBrowserItemUPP;
/*
 *  NewDataBrowserItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DataBrowserItemUPP )
NewDataBrowserItemUPP(DataBrowserItemProcPtr userRoutine);

/*
 *  DisposeDataBrowserItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserItemUPP(DataBrowserItemUPP userUPP);

/*
 *  InvokeDataBrowserItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDataBrowserItemUPP(
  DataBrowserItemID     item,
  DataBrowserItemState  state,
  void *                clientData,
  DataBrowserItemUPP    userUPP);

/* Creation/Configuration */
/*
 *  CreateDataBrowserControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateDataBrowserControl(
  WindowRef              window,
  const Rect *           boundsRect,
  DataBrowserViewStyle   style,
  ControlRef *           outControl);


/*
 *  GetDataBrowserViewStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserViewStyle(
  ControlRef              browser,
  DataBrowserViewStyle *  style);


/*
 *  SetDataBrowserViewStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserViewStyle(
  ControlRef             browser,
  DataBrowserViewStyle   style);


/* Item Manipulation */
/* Passing NULL for "items" argument to RemoveDataBrowserItems and */
/* UpdateDataBrowserItems refers to all items in the specified container. */
/* Passing NULL for "items" argument to AddDataBrowserItems means */
/* "generate IDs starting from 1." */
/*
 *  AddDataBrowserItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
AddDataBrowserItems(
  ControlRef                 browser,
  DataBrowserItemID          container,
  UInt32                     numItems,
  const DataBrowserItemID *  items,                 /* can be NULL */
  DataBrowserPropertyID      preSortProperty);


/*
 *  RemoveDataBrowserItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
RemoveDataBrowserItems(
  ControlRef                 browser,
  DataBrowserItemID          container,
  UInt32                     numItems,
  const DataBrowserItemID *  items,                 /* can be NULL */
  DataBrowserPropertyID      preSortProperty);


/*
 *  UpdateDataBrowserItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
UpdateDataBrowserItems(
  ControlRef                 browser,
  DataBrowserItemID          container,
  UInt32                     numItems,
  const DataBrowserItemID *  items,                 /* can be NULL */
  DataBrowserPropertyID      preSortProperty,
  DataBrowserPropertyID      propertyID);


/* Edit Menu Enabling and Handling */
/*
 *  EnableDataBrowserEditCommand()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
EnableDataBrowserEditCommand(
  ControlRef               browser,
  DataBrowserEditCommand   command);


/*
 *  ExecuteDataBrowserEditCommand()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
ExecuteDataBrowserEditCommand(
  ControlRef               browser,
  DataBrowserEditCommand   command);


/*
 *  GetDataBrowserSelectionAnchor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserSelectionAnchor(
  ControlRef           browser,
  DataBrowserItemID *  first,
  DataBrowserItemID *  last);


/*
 *  MoveDataBrowserSelectionAnchor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
MoveDataBrowserSelectionAnchor(
  ControlRef                            browser,
  DataBrowserSelectionAnchorDirection   direction,
  Boolean                               extendSelection);


/* Container Manipulation */
/*
 *  OpenDataBrowserContainer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
OpenDataBrowserContainer(
  ControlRef          browser,
  DataBrowserItemID   container);


/*
 *  CloseDataBrowserContainer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CloseDataBrowserContainer(
  ControlRef          browser,
  DataBrowserItemID   container);


/*
 *  SortDataBrowserContainer()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SortDataBrowserContainer(
  ControlRef          browser,
  DataBrowserItemID   container,
  Boolean             sortChildren);


/* Aggregate Item Access and Iteration */
/*
 *  GetDataBrowserItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItems(
  ControlRef             browser,
  DataBrowserItemID      container,
  Boolean                recurse,
  DataBrowserItemState   state,
  Handle                 items);


/*
 *  GetDataBrowserItemCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemCount(
  ControlRef             browser,
  DataBrowserItemID      container,
  Boolean                recurse,
  DataBrowserItemState   state,
  UInt32 *               numItems);


/*
 *  ForEachDataBrowserItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
ForEachDataBrowserItem(
  ControlRef             browser,
  DataBrowserItemID      container,
  Boolean                recurse,
  DataBrowserItemState   state,
  DataBrowserItemUPP     callback,
  void *                 clientData);


/* Individual Item Access and Display */
/*
 *  IsDataBrowserItemSelected()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsDataBrowserItemSelected(
  ControlRef          browser,
  DataBrowserItemID   item);


/*
 *  GetDataBrowserItemState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemState(
  ControlRef              browser,
  DataBrowserItemID       item,
  DataBrowserItemState *  state);


/*
 *  RevealDataBrowserItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
RevealDataBrowserItem(
  ControlRef                 browser,
  DataBrowserItemID          item,
  DataBrowserPropertyID      propertyID,
  DataBrowserRevealOptions   options);


/* Selection Set Manipulation */
/*
 *  SetDataBrowserSelectedItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserSelectedItems(
  ControlRef                 browser,
  UInt32                     numItems,
  const DataBrowserItemID *  items,
  DataBrowserSetOption       operation);



/* DataBrowser Attribute Manipulation */
/* The user customizable portion of the current view style settings */
/*
 *  SetDataBrowserUserState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserUserState(
  ControlRef   browser,
  CFDataRef    stateInfo);


/*
 *  GetDataBrowserUserState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserUserState(
  ControlRef   browser,
  CFDataRef *  stateInfo);


/* All items are active/enabled or not */
/*
 *  SetDataBrowserActiveItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserActiveItems(
  ControlRef   browser,
  Boolean      active);


/*
 *  GetDataBrowserActiveItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserActiveItems(
  ControlRef   browser,
  Boolean *    active);


/* Inset the scrollbars within the DataBrowser bounds */
/*
 *  SetDataBrowserScrollBarInset()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserScrollBarInset(
  ControlRef   browser,
  Rect *       insetRect);


/*
 *  GetDataBrowserScrollBarInset()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserScrollBarInset(
  ControlRef   browser,
  Rect *       insetRect);


/* The "user focused" item */
/* For the ListView, this means the root container */
/* For the ColumnView, this means the rightmost container column */
/*
 *  SetDataBrowserTarget()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserTarget(
  ControlRef          browser,
  DataBrowserItemID   target);


/*
 *  GetDataBrowserTarget()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserTarget(
  ControlRef           browser,
  DataBrowserItemID *  target);


/* Current sort ordering */
/* ListView tracks this per-column */
/*
 *  SetDataBrowserSortOrder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserSortOrder(
  ControlRef             browser,
  DataBrowserSortOrder   order);


/*
 *  GetDataBrowserSortOrder()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserSortOrder(
  ControlRef              browser,
  DataBrowserSortOrder *  order);


/* Scrollbar values */
/*
 *  SetDataBrowserScrollPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserScrollPosition(
  ControlRef   browser,
  UInt32       top,
  UInt32       left);


/*
 *  GetDataBrowserScrollPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserScrollPosition(
  ControlRef   browser,
  UInt32 *     top,
  UInt32 *     left);


/* Show/Hide each scrollbar */
/*
 *  SetDataBrowserHasScrollBars()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserHasScrollBars(
  ControlRef   browser,
  Boolean      horiz,
  Boolean      vert);


/*
 *  GetDataBrowserHasScrollBars()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserHasScrollBars(
  ControlRef   browser,
  Boolean *    horiz,
  Boolean *    vert);


/* Property passed to sort callback (ListView sort column) */
/*
 *  SetDataBrowserSortProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserSortProperty(
  ControlRef              browser,
  DataBrowserPropertyID   property);


/*
 *  GetDataBrowserSortProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserSortProperty(
  ControlRef               browser,
  DataBrowserPropertyID *  property);


/* Modify selection behavior */
/*
 *  SetDataBrowserSelectionFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserSelectionFlags(
  ControlRef                  browser,
  DataBrowserSelectionFlags   selectionFlags);


/*
 *  GetDataBrowserSelectionFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserSelectionFlags(
  ControlRef                   browser,
  DataBrowserSelectionFlags *  selectionFlags);


/* Dynamically modify property appearance/behavior */
/*
 *  SetDataBrowserPropertyFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserPropertyFlags(
  ControlRef                 browser,
  DataBrowserPropertyID      property,
  DataBrowserPropertyFlags   flags);


/*
 *  GetDataBrowserPropertyFlags()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserPropertyFlags(
  ControlRef                  browser,
  DataBrowserPropertyID       property,
  DataBrowserPropertyFlags *  flags);


/* Text of current in-place edit session */
/*
 *  SetDataBrowserEditText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserEditText(
  ControlRef    browser,
  CFStringRef   text);


/*
 *  CopyDataBrowserEditText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CopyDataBrowserEditText(
  ControlRef     browser,
  CFStringRef *  text);


/*
 *  GetDataBrowserEditText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserEditText(
  ControlRef           browser,
  CFMutableStringRef   text);


/* Item/property currently being edited */
/*
 *  SetDataBrowserEditItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserEditItem(
  ControlRef              browser,
  DataBrowserItemID       item,
  DataBrowserPropertyID   property);


/*
 *  GetDataBrowserEditItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserEditItem(
  ControlRef               browser,
  DataBrowserItemID *      item,
  DataBrowserPropertyID *  property);


/* Get the current bounds of a visual part of an item's property */
/*
 *  GetDataBrowserItemPartBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemPartBounds(
  ControlRef                browser,
  DataBrowserItemID         item,
  DataBrowserPropertyID     property,
  DataBrowserPropertyPart   part,
  Rect *                    bounds);



/* DataBrowser ItemData Accessors (used within DataBrowserItemData callback) */

typedef void *                          DataBrowserItemDataRef;
/*
 *  SetDataBrowserItemDataIcon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataIcon(
  DataBrowserItemDataRef   itemData,
  IconRef                  theData);


/*
 *  GetDataBrowserItemDataIcon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataIcon(
  DataBrowserItemDataRef   itemData,
  IconRef *                theData);


/*
 *  SetDataBrowserItemDataText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataText(
  DataBrowserItemDataRef   itemData,
  CFStringRef              theData);


/*
 *  GetDataBrowserItemDataText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataText(
  DataBrowserItemDataRef   itemData,
  CFStringRef *            theData);



/*
 *  SetDataBrowserItemDataValue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataValue(
  DataBrowserItemDataRef   itemData,
  SInt32                   theData);


/*
 *  GetDataBrowserItemDataValue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataValue(
  DataBrowserItemDataRef   itemData,
  SInt32 *                 theData);


/*
 *  SetDataBrowserItemDataMinimum()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataMinimum(
  DataBrowserItemDataRef   itemData,
  SInt32                   theData);


/*
 *  GetDataBrowserItemDataMinimum()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataMinimum(
  DataBrowserItemDataRef   itemData,
  SInt32 *                 theData);


/*
 *  SetDataBrowserItemDataMaximum()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataMaximum(
  DataBrowserItemDataRef   itemData,
  SInt32                   theData);


/*
 *  GetDataBrowserItemDataMaximum()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataMaximum(
  DataBrowserItemDataRef   itemData,
  SInt32 *                 theData);


/*
 *  SetDataBrowserItemDataBooleanValue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataBooleanValue(
  DataBrowserItemDataRef   itemData,
  Boolean                  theData);


/*
 *  GetDataBrowserItemDataBooleanValue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataBooleanValue(
  DataBrowserItemDataRef   itemData,
  Boolean *                theData);


/*
 *  SetDataBrowserItemDataMenuRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataMenuRef(
  DataBrowserItemDataRef   itemData,
  MenuRef                  theData);


/*
 *  GetDataBrowserItemDataMenuRef()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataMenuRef(
  DataBrowserItemDataRef   itemData,
  MenuRef *                theData);


/*
 *  SetDataBrowserItemDataRGBColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataRGBColor(
  DataBrowserItemDataRef   itemData,
  const RGBColor *         theData);


/*
 *  GetDataBrowserItemDataRGBColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataRGBColor(
  DataBrowserItemDataRef   itemData,
  RGBColor *               theData);



/*
 *  SetDataBrowserItemDataDrawState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataDrawState(
  DataBrowserItemDataRef   itemData,
  ThemeDrawState           theData);


/*
 *  GetDataBrowserItemDataDrawState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataDrawState(
  DataBrowserItemDataRef   itemData,
  ThemeDrawState *         theData);


/*
 *  SetDataBrowserItemDataButtonValue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataButtonValue(
  DataBrowserItemDataRef   itemData,
  ThemeButtonValue         theData);


/*
 *  GetDataBrowserItemDataButtonValue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataButtonValue(
  DataBrowserItemDataRef   itemData,
  ThemeButtonValue *       theData);


/*
 *  SetDataBrowserItemDataIconTransform()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataIconTransform(
  DataBrowserItemDataRef   itemData,
  IconTransformType        theData);


/*
 *  GetDataBrowserItemDataIconTransform()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataIconTransform(
  DataBrowserItemDataRef   itemData,
  IconTransformType *      theData);



/*
 *  SetDataBrowserItemDataDateTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataDateTime(
  DataBrowserItemDataRef   itemData,
  long                     theData);


/*
 *  GetDataBrowserItemDataDateTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataDateTime(
  DataBrowserItemDataRef   itemData,
  long *                   theData);


/*
 *  SetDataBrowserItemDataLongDateTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataLongDateTime(
  DataBrowserItemDataRef   itemData,
  const LongDateTime *     theData);


/*
 *  GetDataBrowserItemDataLongDateTime()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataLongDateTime(
  DataBrowserItemDataRef   itemData,
  LongDateTime *           theData);



/*
 *  SetDataBrowserItemDataItemID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserItemDataItemID(
  DataBrowserItemDataRef   itemData,
  DataBrowserItemID        theData);


/*
 *  GetDataBrowserItemDataItemID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataItemID(
  DataBrowserItemDataRef   itemData,
  DataBrowserItemID *      theData);


/*
 *  GetDataBrowserItemDataProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserItemDataProperty(
  DataBrowserItemDataRef   itemData,
  DataBrowserPropertyID *  theData);



/* Standard DataBrowser Callbacks */

/* Basic Item Management & Manipulation */
typedef CALLBACK_API( OSStatus , DataBrowserItemDataProcPtr )(ControlRef browser, DataBrowserItemID item, DataBrowserPropertyID property, DataBrowserItemDataRef itemData, Boolean setValue);
typedef STACK_UPP_TYPE(DataBrowserItemDataProcPtr)              DataBrowserItemDataUPP;

/* Item Comparison */
typedef CALLBACK_API( Boolean , DataBrowserItemCompareProcPtr )(ControlRef browser, DataBrowserItemID itemOne, DataBrowserItemID itemTwo, DataBrowserPropertyID sortProperty);
typedef STACK_UPP_TYPE(DataBrowserItemCompareProcPtr)           DataBrowserItemCompareUPP;

/* ItemEvent Notification */
/*  A Very Important Note about DataBrowserItemNotificationProcPtr:                                     */
/*                                                                                                      */
/*  Under all currently shipping versions of CarbonLib (eg. up through 1.3), your callback is called    */
/*  just as the prototype appears in this header. It should only be expecting three parameters because  */
/*  DataBrowser will only pass three parameters.                                                        */
/*                                                                                                      */
/*  Under Mac OS X, your callback is called with an additional parameter. If you wish to interpret      */
/*  the additional parameter, your callback should have the same prototype as the                       */
/*  DataBrowserItemNotificationWithItemProcPtr (below). You may freely take a callback with this        */
/*  prototype and pass it to NewDataBrowserItemNotificationUPP in order to generate a                   */
/*  DataBrowserItemNotificationUPP that you can use just like any other DataBrowserItemNotificationUPP. */
/*                                                                                                      */
/*  If you use this technique under CarbonLib, you will *not* receive valid data in the fourth          */
/*  parameter, and any attempt to use the invalid data will probably result in a crash.                 */
typedef CALLBACK_API( void , DataBrowserItemNotificationWithItemProcPtr )(ControlRef browser, DataBrowserItemID item, DataBrowserItemNotification message, DataBrowserItemDataRef itemData);
typedef CALLBACK_API( void , DataBrowserItemNotificationProcPtr )(ControlRef browser, DataBrowserItemID item, DataBrowserItemNotification message);
typedef STACK_UPP_TYPE(DataBrowserItemNotificationWithItemProcPtr)  DataBrowserItemNotificationWithItemUPP;
typedef STACK_UPP_TYPE(DataBrowserItemNotificationProcPtr)      DataBrowserItemNotificationUPP;


/* Drag & Drop Processing */
typedef CALLBACK_API( Boolean , DataBrowserAddDragItemProcPtr )(ControlRef browser, DragReference theDrag, DataBrowserItemID item, ItemReference *itemRef);
typedef CALLBACK_API( Boolean , DataBrowserAcceptDragProcPtr )(ControlRef browser, DragReference theDrag, DataBrowserItemID item);
typedef CALLBACK_API( Boolean , DataBrowserReceiveDragProcPtr )(ControlRef browser, DragReference theDrag, DataBrowserItemID item);
typedef CALLBACK_API( void , DataBrowserPostProcessDragProcPtr )(ControlRef browser, DragReference theDrag, OSStatus trackDragResult);
typedef STACK_UPP_TYPE(DataBrowserAddDragItemProcPtr)           DataBrowserAddDragItemUPP;
typedef STACK_UPP_TYPE(DataBrowserAcceptDragProcPtr)            DataBrowserAcceptDragUPP;
typedef STACK_UPP_TYPE(DataBrowserReceiveDragProcPtr)           DataBrowserReceiveDragUPP;
typedef STACK_UPP_TYPE(DataBrowserPostProcessDragProcPtr)       DataBrowserPostProcessDragUPP;

/* Contextual Menu Support */
typedef CALLBACK_API( void , DataBrowserGetContextualMenuProcPtr )(ControlRef browser, MenuRef *menu, UInt32 *helpType, CFStringRef *helpItemString, AEDesc *selection);
typedef CALLBACK_API( void , DataBrowserSelectContextualMenuProcPtr )(ControlRef browser, MenuRef menu, UInt32 selectionType, SInt16 menuID, MenuItemIndex menuItem);
typedef STACK_UPP_TYPE(DataBrowserGetContextualMenuProcPtr)     DataBrowserGetContextualMenuUPP;
typedef STACK_UPP_TYPE(DataBrowserSelectContextualMenuProcPtr)  DataBrowserSelectContextualMenuUPP;

/* Help Manager Support */
typedef CALLBACK_API( void , DataBrowserItemHelpContentProcPtr )(ControlRef browser, DataBrowserItemID item, DataBrowserPropertyID property, HMContentRequest inRequest, HMContentProvidedType *outContentProvided, HMHelpContentPtr ioHelpContent);
typedef STACK_UPP_TYPE(DataBrowserItemHelpContentProcPtr)       DataBrowserItemHelpContentUPP;
/*
 *  NewDataBrowserItemDataUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DataBrowserItemDataUPP )
NewDataBrowserItemDataUPP(DataBrowserItemDataProcPtr userRoutine);

/*
 *  NewDataBrowserItemCompareUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DataBrowserItemCompareUPP )
NewDataBrowserItemCompareUPP(DataBrowserItemCompareProcPtr userRoutine);

/*
 *  NewDataBrowserItemNotificationWithItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( DataBrowserItemNotificationWithItemUPP )
NewDataBrowserItemNotificationWithItemUPP(DataBrowserItemNotificationWithItemProcPtr userRoutine);

/*
 *  NewDataBrowserItemNotificationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DataBrowserItemNotificationUPP )
NewDataBrowserItemNotificationUPP(DataBrowserItemNotificationProcPtr userRoutine);

/*
 *  NewDataBrowserAddDragItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DataBrowserAddDragItemUPP )
NewDataBrowserAddDragItemUPP(DataBrowserAddDragItemProcPtr userRoutine);

/*
 *  NewDataBrowserAcceptDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DataBrowserAcceptDragUPP )
NewDataBrowserAcceptDragUPP(DataBrowserAcceptDragProcPtr userRoutine);

/*
 *  NewDataBrowserReceiveDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DataBrowserReceiveDragUPP )
NewDataBrowserReceiveDragUPP(DataBrowserReceiveDragProcPtr userRoutine);

/*
 *  NewDataBrowserPostProcessDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DataBrowserPostProcessDragUPP )
NewDataBrowserPostProcessDragUPP(DataBrowserPostProcessDragProcPtr userRoutine);

/*
 *  NewDataBrowserGetContextualMenuUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DataBrowserGetContextualMenuUPP )
NewDataBrowserGetContextualMenuUPP(DataBrowserGetContextualMenuProcPtr userRoutine);

/*
 *  NewDataBrowserSelectContextualMenuUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DataBrowserSelectContextualMenuUPP )
NewDataBrowserSelectContextualMenuUPP(DataBrowserSelectContextualMenuProcPtr userRoutine);

/*
 *  NewDataBrowserItemHelpContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DataBrowserItemHelpContentUPP )
NewDataBrowserItemHelpContentUPP(DataBrowserItemHelpContentProcPtr userRoutine);

/*
 *  DisposeDataBrowserItemDataUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserItemDataUPP(DataBrowserItemDataUPP userUPP);

/*
 *  DisposeDataBrowserItemCompareUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserItemCompareUPP(DataBrowserItemCompareUPP userUPP);

/*
 *  DisposeDataBrowserItemNotificationWithItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserItemNotificationWithItemUPP(DataBrowserItemNotificationWithItemUPP userUPP);

/*
 *  DisposeDataBrowserItemNotificationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserItemNotificationUPP(DataBrowserItemNotificationUPP userUPP);

/*
 *  DisposeDataBrowserAddDragItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserAddDragItemUPP(DataBrowserAddDragItemUPP userUPP);

/*
 *  DisposeDataBrowserAcceptDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserAcceptDragUPP(DataBrowserAcceptDragUPP userUPP);

/*
 *  DisposeDataBrowserReceiveDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserReceiveDragUPP(DataBrowserReceiveDragUPP userUPP);

/*
 *  DisposeDataBrowserPostProcessDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserPostProcessDragUPP(DataBrowserPostProcessDragUPP userUPP);

/*
 *  DisposeDataBrowserGetContextualMenuUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserGetContextualMenuUPP(DataBrowserGetContextualMenuUPP userUPP);

/*
 *  DisposeDataBrowserSelectContextualMenuUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserSelectContextualMenuUPP(DataBrowserSelectContextualMenuUPP userUPP);

/*
 *  DisposeDataBrowserItemHelpContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserItemHelpContentUPP(DataBrowserItemHelpContentUPP userUPP);

/*
 *  InvokeDataBrowserItemDataUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeDataBrowserItemDataUPP(
  ControlRef              browser,
  DataBrowserItemID       item,
  DataBrowserPropertyID   property,
  DataBrowserItemDataRef  itemData,
  Boolean                 setValue,
  DataBrowserItemDataUPP  userUPP);

/*
 *  InvokeDataBrowserItemCompareUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeDataBrowserItemCompareUPP(
  ControlRef                 browser,
  DataBrowserItemID          itemOne,
  DataBrowserItemID          itemTwo,
  DataBrowserPropertyID      sortProperty,
  DataBrowserItemCompareUPP  userUPP);

/*
 *  InvokeDataBrowserItemNotificationWithItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
InvokeDataBrowserItemNotificationWithItemUPP(
  ControlRef                              browser,
  DataBrowserItemID                       item,
  DataBrowserItemNotification             message,
  DataBrowserItemDataRef                  itemData,
  DataBrowserItemNotificationWithItemUPP  userUPP);

/*
 *  InvokeDataBrowserItemNotificationUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDataBrowserItemNotificationUPP(
  ControlRef                      browser,
  DataBrowserItemID               item,
  DataBrowserItemNotification     message,
  DataBrowserItemNotificationUPP  userUPP);

/*
 *  InvokeDataBrowserAddDragItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeDataBrowserAddDragItemUPP(
  ControlRef                 browser,
  DragReference              theDrag,
  DataBrowserItemID          item,
  ItemReference *            itemRef,
  DataBrowserAddDragItemUPP  userUPP);

/*
 *  InvokeDataBrowserAcceptDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeDataBrowserAcceptDragUPP(
  ControlRef                browser,
  DragReference             theDrag,
  DataBrowserItemID         item,
  DataBrowserAcceptDragUPP  userUPP);

/*
 *  InvokeDataBrowserReceiveDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeDataBrowserReceiveDragUPP(
  ControlRef                 browser,
  DragReference              theDrag,
  DataBrowserItemID          item,
  DataBrowserReceiveDragUPP  userUPP);

/*
 *  InvokeDataBrowserPostProcessDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDataBrowserPostProcessDragUPP(
  ControlRef                     browser,
  DragReference                  theDrag,
  OSStatus                       trackDragResult,
  DataBrowserPostProcessDragUPP  userUPP);

/*
 *  InvokeDataBrowserGetContextualMenuUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDataBrowserGetContextualMenuUPP(
  ControlRef                       browser,
  MenuRef *                        menu,
  UInt32 *                         helpType,
  CFStringRef *                    helpItemString,
  AEDesc *                         selection,
  DataBrowserGetContextualMenuUPP  userUPP);

/*
 *  InvokeDataBrowserSelectContextualMenuUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDataBrowserSelectContextualMenuUPP(
  ControlRef                          browser,
  MenuRef                             menu,
  UInt32                              selectionType,
  SInt16                              menuID,
  MenuItemIndex                       menuItem,
  DataBrowserSelectContextualMenuUPP  userUPP);

/*
 *  InvokeDataBrowserItemHelpContentUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDataBrowserItemHelpContentUPP(
  ControlRef                     browser,
  DataBrowserItemID              item,
  DataBrowserPropertyID          property,
  HMContentRequest               inRequest,
  HMContentProvidedType *        outContentProvided,
  HMHelpContentPtr               ioHelpContent,
  DataBrowserItemHelpContentUPP  userUPP);

/* Standard Callback (vtable) Structure */
enum {
  kDataBrowserLatestCallbacks   = 0
};

struct DataBrowserCallbacks {
  UInt32              version;                /* Use kDataBrowserLatestCallbacks */

  union {
    struct {
      DataBrowserItemDataUPP  itemDataCallback;
      DataBrowserItemCompareUPP  itemCompareCallback;
      DataBrowserItemNotificationUPP  itemNotificationCallback;

      DataBrowserAddDragItemUPP  addDragItemCallback;
      DataBrowserAcceptDragUPP  acceptDragCallback;
      DataBrowserReceiveDragUPP  receiveDragCallback;
      DataBrowserPostProcessDragUPP  postProcessDragCallback;

      DataBrowserItemHelpContentUPP  itemHelpContentCallback;
      DataBrowserGetContextualMenuUPP  getContextualMenuCallback;
      DataBrowserSelectContextualMenuUPP  selectContextualMenuCallback;
    }                       v1;
  }                       u;
};
typedef struct DataBrowserCallbacks     DataBrowserCallbacks;
/*
 *  InitDataBrowserCallbacks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
InitDataBrowserCallbacks(DataBrowserCallbacks * callbacks);


/* Macro for initializing callback structure */
#define InitializeDataBrowserCallbacks(callbacks, vers) \
{ (callbacks)->version = (vers); InitDataBrowserCallbacks(callbacks); }

/*
 *  GetDataBrowserCallbacks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserCallbacks(
  ControlRef              browser,
  DataBrowserCallbacks *  callbacks);


/*
 *  SetDataBrowserCallbacks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserCallbacks(
  ControlRef                    browser,
  const DataBrowserCallbacks *  callbacks);



/* Custom Format Callbacks (kDataBrowserCustomType display properties) */

typedef unsigned long                   DataBrowserDragFlags;
typedef SInt16 DataBrowserTrackingResult;
enum {
  kDataBrowserContentHit        = 1,
  kDataBrowserNothingHit        = 0,
  kDataBrowserStopTracking      = -1
};

typedef CALLBACK_API( void , DataBrowserDrawItemProcPtr )(ControlRef browser, DataBrowserItemID item, DataBrowserPropertyID property, DataBrowserItemState itemState, const Rect *theRect, SInt16 gdDepth, Boolean colorDevice);
typedef CALLBACK_API( Boolean , DataBrowserEditItemProcPtr )(ControlRef browser, DataBrowserItemID item, DataBrowserPropertyID property, CFStringRef theString, Rect *maxEditTextRect, Boolean *shrinkToFit);
typedef CALLBACK_API( Boolean , DataBrowserHitTestProcPtr )(ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID property, const Rect *theRect, const Rect *mouseRect);
typedef CALLBACK_API( DataBrowserTrackingResult , DataBrowserTrackingProcPtr )(ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID property, const Rect *theRect, Point startPt, EventModifiers modifiers);
typedef CALLBACK_API( void , DataBrowserItemDragRgnProcPtr )(ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID property, const Rect *theRect, RgnHandle dragRgn);
typedef CALLBACK_API( DataBrowserDragFlags , DataBrowserItemAcceptDragProcPtr )(ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID property, const Rect *theRect, DragReference theDrag);
typedef CALLBACK_API( Boolean , DataBrowserItemReceiveDragProcPtr )(ControlRef browser, DataBrowserItemID itemID, DataBrowserPropertyID property, DataBrowserDragFlags dragFlags, DragReference theDrag);
typedef STACK_UPP_TYPE(DataBrowserDrawItemProcPtr)              DataBrowserDrawItemUPP;
typedef STACK_UPP_TYPE(DataBrowserEditItemProcPtr)              DataBrowserEditItemUPP;
typedef STACK_UPP_TYPE(DataBrowserHitTestProcPtr)               DataBrowserHitTestUPP;
typedef STACK_UPP_TYPE(DataBrowserTrackingProcPtr)              DataBrowserTrackingUPP;
typedef STACK_UPP_TYPE(DataBrowserItemDragRgnProcPtr)           DataBrowserItemDragRgnUPP;
typedef STACK_UPP_TYPE(DataBrowserItemAcceptDragProcPtr)        DataBrowserItemAcceptDragUPP;
typedef STACK_UPP_TYPE(DataBrowserItemReceiveDragProcPtr)       DataBrowserItemReceiveDragUPP;
/*
 *  NewDataBrowserDrawItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( DataBrowserDrawItemUPP )
NewDataBrowserDrawItemUPP(DataBrowserDrawItemProcPtr userRoutine);

/*
 *  NewDataBrowserEditItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( DataBrowserEditItemUPP )
NewDataBrowserEditItemUPP(DataBrowserEditItemProcPtr userRoutine);

/*
 *  NewDataBrowserHitTestUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( DataBrowserHitTestUPP )
NewDataBrowserHitTestUPP(DataBrowserHitTestProcPtr userRoutine);

/*
 *  NewDataBrowserTrackingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( DataBrowserTrackingUPP )
NewDataBrowserTrackingUPP(DataBrowserTrackingProcPtr userRoutine);

/*
 *  NewDataBrowserItemDragRgnUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( DataBrowserItemDragRgnUPP )
NewDataBrowserItemDragRgnUPP(DataBrowserItemDragRgnProcPtr userRoutine);

/*
 *  NewDataBrowserItemAcceptDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( DataBrowserItemAcceptDragUPP )
NewDataBrowserItemAcceptDragUPP(DataBrowserItemAcceptDragProcPtr userRoutine);

/*
 *  NewDataBrowserItemReceiveDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( DataBrowserItemReceiveDragUPP )
NewDataBrowserItemReceiveDragUPP(DataBrowserItemReceiveDragProcPtr userRoutine);

/*
 *  DisposeDataBrowserDrawItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserDrawItemUPP(DataBrowserDrawItemUPP userUPP);

/*
 *  DisposeDataBrowserEditItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserEditItemUPP(DataBrowserEditItemUPP userUPP);

/*
 *  DisposeDataBrowserHitTestUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserHitTestUPP(DataBrowserHitTestUPP userUPP);

/*
 *  DisposeDataBrowserTrackingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserTrackingUPP(DataBrowserTrackingUPP userUPP);

/*
 *  DisposeDataBrowserItemDragRgnUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserItemDragRgnUPP(DataBrowserItemDragRgnUPP userUPP);

/*
 *  DisposeDataBrowserItemAcceptDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserItemAcceptDragUPP(DataBrowserItemAcceptDragUPP userUPP);

/*
 *  DisposeDataBrowserItemReceiveDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
DisposeDataBrowserItemReceiveDragUPP(DataBrowserItemReceiveDragUPP userUPP);

/*
 *  InvokeDataBrowserDrawItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
InvokeDataBrowserDrawItemUPP(
  ControlRef              browser,
  DataBrowserItemID       item,
  DataBrowserPropertyID   property,
  DataBrowserItemState    itemState,
  const Rect *            theRect,
  SInt16                  gdDepth,
  Boolean                 colorDevice,
  DataBrowserDrawItemUPP  userUPP);

/*
 *  InvokeDataBrowserEditItemUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( Boolean )
InvokeDataBrowserEditItemUPP(
  ControlRef              browser,
  DataBrowserItemID       item,
  DataBrowserPropertyID   property,
  CFStringRef             theString,
  Rect *                  maxEditTextRect,
  Boolean *               shrinkToFit,
  DataBrowserEditItemUPP  userUPP);

/*
 *  InvokeDataBrowserHitTestUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( Boolean )
InvokeDataBrowserHitTestUPP(
  ControlRef             browser,
  DataBrowserItemID      itemID,
  DataBrowserPropertyID  property,
  const Rect *           theRect,
  const Rect *           mouseRect,
  DataBrowserHitTestUPP  userUPP);

/*
 *  InvokeDataBrowserTrackingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( DataBrowserTrackingResult )
InvokeDataBrowserTrackingUPP(
  ControlRef              browser,
  DataBrowserItemID       itemID,
  DataBrowserPropertyID   property,
  const Rect *            theRect,
  Point                   startPt,
  EventModifiers          modifiers,
  DataBrowserTrackingUPP  userUPP);

/*
 *  InvokeDataBrowserItemDragRgnUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
InvokeDataBrowserItemDragRgnUPP(
  ControlRef                 browser,
  DataBrowserItemID          itemID,
  DataBrowserPropertyID      property,
  const Rect *               theRect,
  RgnHandle                  dragRgn,
  DataBrowserItemDragRgnUPP  userUPP);

/*
 *  InvokeDataBrowserItemAcceptDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( DataBrowserDragFlags )
InvokeDataBrowserItemAcceptDragUPP(
  ControlRef                    browser,
  DataBrowserItemID             itemID,
  DataBrowserPropertyID         property,
  const Rect *                  theRect,
  DragReference                 theDrag,
  DataBrowserItemAcceptDragUPP  userUPP);

/*
 *  InvokeDataBrowserItemReceiveDragUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( Boolean )
InvokeDataBrowserItemReceiveDragUPP(
  ControlRef                     browser,
  DataBrowserItemID              itemID,
  DataBrowserPropertyID          property,
  DataBrowserDragFlags           dragFlags,
  DragReference                  theDrag,
  DataBrowserItemReceiveDragUPP  userUPP);

/* Custom Callback (vtable) Structure */
enum {
  kDataBrowserLatestCustomCallbacks = 0
};

struct DataBrowserCustomCallbacks {

  UInt32              version;                /* Use kDataBrowserLatestCustomCallbacks */

  union {
    struct {
      DataBrowserDrawItemUPP  drawItemCallback;
      DataBrowserEditItemUPP  editTextCallback;
      DataBrowserHitTestUPP  hitTestCallback;
      DataBrowserTrackingUPP  trackingCallback;

      DataBrowserItemDragRgnUPP  dragRegionCallback;
      DataBrowserItemAcceptDragUPP  acceptDragCallback;
      DataBrowserItemReceiveDragUPP  receiveDragCallback;
    }                       v1;
  }                       u;
};
typedef struct DataBrowserCustomCallbacks DataBrowserCustomCallbacks;
/*
 *  InitDataBrowserCustomCallbacks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
InitDataBrowserCustomCallbacks(DataBrowserCustomCallbacks * callbacks);


/* Macro for initializing custom callback structure */
#define InitializeDataBrowserCustomCallbacks(callbacks, vers) \
{ (callbacks)->version = (vers); InitDataBrowserCustomCallbacks(callbacks); }

/*
 *  GetDataBrowserCustomCallbacks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserCustomCallbacks(
  ControlRef                    browser,
  DataBrowserCustomCallbacks *  callbacks);


/*
 *  SetDataBrowserCustomCallbacks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserCustomCallbacks(
  ControlRef                          browser,
  const DataBrowserCustomCallbacks *  callbacks);





/* TableView Formatting */
typedef UInt32 DataBrowserTableViewHiliteStyle;
enum {
  kDataBrowserTableViewMinimalHilite = 0,
  kDataBrowserTableViewFillHilite = 1
};

typedef UInt32 DataBrowserTableViewPropertyFlags;
enum {
                                        /* kDataBrowserTableView DataBrowserPropertyFlags */
  kDataBrowserTableViewSelectionColumn = 1 << kDataBrowserViewSpecificFlagsOffset
};

/* The row and column indicies are zero-based */

typedef UInt32                          DataBrowserTableViewRowIndex;
typedef UInt32                          DataBrowserTableViewColumnIndex;
typedef DataBrowserPropertyID           DataBrowserTableViewColumnID;
typedef DataBrowserPropertyDesc         DataBrowserTableViewColumnDesc;

/* TableView API */
/* Use when setting column position */
enum {
  kDataBrowserTableViewLastColumn = -1
};

/*
 *  RemoveDataBrowserTableViewColumn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
RemoveDataBrowserTableViewColumn(
  ControlRef                     browser,
  DataBrowserTableViewColumnID   column);


/*
 *  GetDataBrowserTableViewColumnCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserTableViewColumnCount(
  ControlRef   browser,
  UInt32 *     numColumns);



/*
 *  SetDataBrowserTableViewHiliteStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserTableViewHiliteStyle(
  ControlRef                        browser,
  DataBrowserTableViewHiliteStyle   hiliteStyle);


/*
 *  GetDataBrowserTableViewHiliteStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserTableViewHiliteStyle(
  ControlRef                         browser,
  DataBrowserTableViewHiliteStyle *  hiliteStyle);



/*
 *  SetDataBrowserTableViewRowHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserTableViewRowHeight(
  ControlRef   browser,
  UInt16       height);


/*
 *  GetDataBrowserTableViewRowHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserTableViewRowHeight(
  ControlRef   browser,
  UInt16 *     height);


/*
 *  SetDataBrowserTableViewColumnWidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserTableViewColumnWidth(
  ControlRef   browser,
  UInt16       width);


/*
 *  GetDataBrowserTableViewColumnWidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserTableViewColumnWidth(
  ControlRef   browser,
  UInt16 *     width);


/*
 *  SetDataBrowserTableViewItemRowHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserTableViewItemRowHeight(
  ControlRef          browser,
  DataBrowserItemID   item,
  UInt16              height);


/*
 *  GetDataBrowserTableViewItemRowHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserTableViewItemRowHeight(
  ControlRef          browser,
  DataBrowserItemID   item,
  UInt16 *            height);


/*
 *  SetDataBrowserTableViewNamedColumnWidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserTableViewNamedColumnWidth(
  ControlRef                     browser,
  DataBrowserTableViewColumnID   column,
  UInt16                         width);


/*
 *  GetDataBrowserTableViewNamedColumnWidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserTableViewNamedColumnWidth(
  ControlRef                     browser,
  DataBrowserTableViewColumnID   column,
  UInt16 *                       width);


/*
 *  SetDataBrowserTableViewGeometry()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserTableViewGeometry(
  ControlRef   browser,
  Boolean      variableWidthColumns,
  Boolean      variableHeightRows);


/*
 *  GetDataBrowserTableViewGeometry()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserTableViewGeometry(
  ControlRef   browser,
  Boolean *    variableWidthColumns,
  Boolean *    variableHeightRows);



/*
 *  GetDataBrowserTableViewItemID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserTableViewItemID(
  ControlRef                     browser,
  DataBrowserTableViewRowIndex   row,
  DataBrowserItemID *            item);


/*
 *  SetDataBrowserTableViewItemRow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserTableViewItemRow(
  ControlRef                     browser,
  DataBrowserItemID              item,
  DataBrowserTableViewRowIndex   row);


/*
 *  GetDataBrowserTableViewItemRow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserTableViewItemRow(
  ControlRef                      browser,
  DataBrowserItemID               item,
  DataBrowserTableViewRowIndex *  row);


/*
 *  SetDataBrowserTableViewColumnPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserTableViewColumnPosition(
  ControlRef                        browser,
  DataBrowserTableViewColumnID      column,
  DataBrowserTableViewColumnIndex   position);


/*
 *  GetDataBrowserTableViewColumnPosition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserTableViewColumnPosition(
  ControlRef                         browser,
  DataBrowserTableViewColumnID       column,
  DataBrowserTableViewColumnIndex *  position);


/*
 *  GetDataBrowserTableViewColumnProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserTableViewColumnProperty(
  ControlRef                        browser,
  DataBrowserTableViewColumnIndex   column,
  DataBrowserTableViewColumnID *    property);





/* kDataBrowserListView Formatting */
typedef UInt32 DataBrowserListViewPropertyFlags;
enum {
                                        /* kDataBrowserListView DataBrowserPropertyFlags */
  kDataBrowserListViewMovableColumn = 1 << (kDataBrowserViewSpecificFlagsOffset + 1),
  kDataBrowserListViewSortableColumn = 1 << (kDataBrowserViewSpecificFlagsOffset + 2),
  kDataBrowserListViewSelectionColumn = kDataBrowserTableViewSelectionColumn,
  kDataBrowserListViewDefaultColumnFlags = kDataBrowserListViewMovableColumn + kDataBrowserListViewSortableColumn
};


enum {
  kDataBrowserListViewLatestHeaderDesc = 0
};

struct DataBrowserListViewHeaderDesc {
  UInt32              version;                /* Use kDataBrowserListViewLatestHeaderDesc */

  UInt16              minimumWidth;
  UInt16              maximumWidth;

  SInt16              titleOffset;
  CFStringRef         titleString;
  DataBrowserSortOrder  initialOrder;
  ControlFontStyleRec  btnFontStyle;
  ControlButtonContentInfo  btnContentInfo;
};
typedef struct DataBrowserListViewHeaderDesc DataBrowserListViewHeaderDesc;
struct DataBrowserListViewColumnDesc {
  DataBrowserTableViewColumnDesc  propertyDesc;
  DataBrowserListViewHeaderDesc  headerBtnDesc;
};
typedef struct DataBrowserListViewColumnDesc DataBrowserListViewColumnDesc;
/* kDataBrowserListView API */
enum {
  kDataBrowserListViewAppendColumn = kDataBrowserTableViewLastColumn
};

/*
 *  AutoSizeDataBrowserListViewColumns()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
AutoSizeDataBrowserListViewColumns(ControlRef browser);


/*
 *  AddDataBrowserListViewColumn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
AddDataBrowserListViewColumn(
  ControlRef                        browser,
  DataBrowserListViewColumnDesc *   columnDesc,
  DataBrowserTableViewColumnIndex   position);


/*
 *  GetDataBrowserListViewHeaderDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserListViewHeaderDesc(
  ControlRef                       browser,
  DataBrowserTableViewColumnID     column,
  DataBrowserListViewHeaderDesc *  desc);


/*
 *  SetDataBrowserListViewHeaderDesc()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserListViewHeaderDesc(
  ControlRef                       browser,
  DataBrowserTableViewColumnID     column,
  DataBrowserListViewHeaderDesc *  desc);


/*
 *  SetDataBrowserListViewHeaderBtnHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserListViewHeaderBtnHeight(
  ControlRef   browser,
  UInt16       height);


/*
 *  GetDataBrowserListViewHeaderBtnHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserListViewHeaderBtnHeight(
  ControlRef   browser,
  UInt16 *     height);


/*
 *  SetDataBrowserListViewUsePlainBackground()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserListViewUsePlainBackground(
  ControlRef   browser,
  Boolean      usePlainBackground);


/*
 *  GetDataBrowserListViewUsePlainBackground()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserListViewUsePlainBackground(
  ControlRef   browser,
  Boolean *    usePlainBackground);


/*
 *  SetDataBrowserListViewDisclosureColumn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserListViewDisclosureColumn(
  ControlRef                     browser,
  DataBrowserTableViewColumnID   column,
  Boolean                        expandableRows);


/*
 *  GetDataBrowserListViewDisclosureColumn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserListViewDisclosureColumn(
  ControlRef                      browser,
  DataBrowserTableViewColumnID *  column,
  Boolean *                       expandableRows);



/* kDataBrowserColumnView API */
/*
 *  GetDataBrowserColumnViewPath()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserColumnViewPath(
  ControlRef   browser,
  Handle       path);


/*
 *  GetDataBrowserColumnViewPathLength()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserColumnViewPathLength(
  ControlRef   browser,
  UInt32 *     pathLength);


/*
 *  SetDataBrowserColumnViewPath()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserColumnViewPath(
  ControlRef                 browser,
  UInt32                     length,
  const DataBrowserItemID *  path);


/*
 *  SetDataBrowserColumnViewDisplayType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDataBrowserColumnViewDisplayType(
  ControlRef                browser,
  DataBrowserPropertyType   propertyType);


/*
 *  GetDataBrowserColumnViewDisplayType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDataBrowserColumnViewDisplayType(
  ControlRef                 browser,
  DataBrowserPropertyType *  propertyType);



/* DataBrowser UPP macros */
/*---------------------------------------------------------------------------------------*/
/* EditUnicodeText Control                                                               */
/*---------------------------------------------------------------------------------------*/
/* This control is only available in Mac OS X.  It is super similar to Edit Text control */
/* Use all the same Get/Set tags.  But don't ask for the TEHandle.                       */
/*---------------------------------------------------------------------------------------*/
/* This callback supplies the functionality of the TSMTEPostUpdateProcPtr that is used */
/* in the EditText control.  A client should supply this call if they want to look at  */
/* inline text that has been fixed before it is included in the actual body text       */
/* if the new text (i.e. the text in the handle) should be included in the body text    */
/* the client should return true.  If the client wants to block the inclusion of the    */
/* text they should return false.                                                       */
typedef CALLBACK_API( Boolean , EditUnicodePostUpdateProcPtr )(UniCharArrayHandle uniText, UniCharCount uniTextLength, UniCharArrayOffset iStartOffset, UniCharArrayOffset iEndOffset, void *refcon);
typedef STACK_UPP_TYPE(EditUnicodePostUpdateProcPtr)            EditUnicodePostUpdateUPP;
/*
 *  NewEditUnicodePostUpdateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( EditUnicodePostUpdateUPP )
NewEditUnicodePostUpdateUPP(EditUnicodePostUpdateProcPtr userRoutine);

/*
 *  DisposeEditUnicodePostUpdateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeEditUnicodePostUpdateUPP(EditUnicodePostUpdateUPP userUPP);

/*
 *  InvokeEditUnicodePostUpdateUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeEditUnicodePostUpdateUPP(
  UniCharArrayHandle        uniText,
  UniCharCount              uniTextLength,
  UniCharArrayOffset        iStartOffset,
  UniCharArrayOffset        iEndOffset,
  void *                    refcon,
  EditUnicodePostUpdateUPP  userUPP);

/* Use this tag when calling ControlSet/GetData to specify the UnicodePostUpdateProcPtr */
/* tags available with Appearance 1.1 or later */
enum {
  kControlEditUnicodeTextPostUpdateProcTag = FOUR_CHAR_CODE('upup')
};


enum {
  kControlEditUnicodeTextProc   = 912,
  kControlEditUnicodeTextPasswordProc = 914
};

/* Control Kind Tag */
enum {
  kControlKindEditUnicodeText   = FOUR_CHAR_CODE('eutx')
};

/* Creation API for X */
/*
 *  CreateEditUnicodeTextControl()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateEditUnicodeTextControl(
  WindowRef                    window,
  const Rect *                 boundsRect,
  CFStringRef                  text,
  Boolean                      isPassword,
  const ControlFontStyleRec *  style,            /* can be NULL */
  ControlRef *                 outControl);


/* Tagged data supported by Unicode text Control only*/
enum {
  kControlEditTextSingleLineTag = FOUR_CHAR_CODE('sglc')
};

#if OLDROUTINENAMES
/*--------------------------------------------------------------------------------------*/
/*  o OLDROUTINENAMES                                                                   */
/*--------------------------------------------------------------------------------------*/
enum {
  kControlCheckboxUncheckedValue = kControlCheckBoxUncheckedValue,
  kControlCheckboxCheckedValue  = kControlCheckBoxCheckedValue,
  kControlCheckboxMixedValue    = kControlCheckBoxMixedValue
};

enum {
  inLabel                       = kControlLabelPart,
  inMenu                        = kControlMenuPart,
  inTriangle                    = kControlTrianglePart,
  inButton                      = kControlButtonPart,
  inCheckBox                    = kControlCheckBoxPart,
  inUpButton                    = kControlUpButtonPart,
  inDownButton                  = kControlDownButtonPart,
  inPageUp                      = kControlPageUpPart,
  inPageDown                    = kControlPageDownPart
};

enum {
  kInLabelControlPart           = kControlLabelPart,
  kInMenuControlPart            = kControlMenuPart,
  kInTriangleControlPart        = kControlTrianglePart,
  kInButtonControlPart          = kControlButtonPart,
  kInCheckBoxControlPart        = kControlCheckBoxPart,
  kInUpButtonControlPart        = kControlUpButtonPart,
  kInDownButtonControlPart      = kControlDownButtonPart,
  kInPageUpControlPart          = kControlPageUpPart,
  kInPageDownControlPart        = kControlPageDownPart
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

#endif /* __CONTROLDEFINITIONS__ */

