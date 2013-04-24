/*
     File:       FontPanel.h
 
     Contains:   Carbon Font Panel package Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (C) 2002 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __FONTPANEL__
#define __FONTPANEL__

#ifndef __ATSTYPES__
#include <ATSTypes.h>
#endif

#ifndef __CARBONEVENTS__
#include <CarbonEvents.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
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

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Font Panel-Related Events
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *  Discussion:
 *    Event classes
 */
enum {

  /*
   * Events related to font selection or handling.
   */
  kEventClassFont               = FOUR_CHAR_CODE('font')
};


/*
 *  Summary:
 *    Common command IDs
 */
enum {

  /*
   * The state of the Font Panel should be toggled, displaying it or
   * hiding it as necessary. If the user closes the Font Panel directly
   * from the window, the application will receive a
   * kEventFontPanelClosed event.
   */
  kHICommandShowHideFontPanel   = FOUR_CHAR_CODE('shfp')
};

/* Font Events */

/*
 *  Summary:
 *    Font events (kEventClassFont)
 *  
 *  Discussion:
 *    When the user closes the Font Panel, a kEventWindowClosed event
 *    will be detected by the Carbon event handler installed by the
 *    system. The system then notifies the application that the Font
 *    Panel has closed by posting a Carbon Event Manager event. This
 *    allows the application to update any menu items or other controls
 *    whose state may have to change because the Font Panel has closed.
 *    kEventWindowClosed has no parameters. When the user selects an
 *    item in the Font Panel, the system will send a
 *    kEventFontSelection event to the event target specified when the
 *    application called SetFontPanelInfo(). kEventFontSelection will
 *    contain parameters reflecting the current Font Panel selection in
 *    all supported formats. Font events are available after Mac OS X
 *    10.2 in the Carbon framework.
 */
enum {

  /*
   * The Font Panel has been closed. The application should update its
   * corresponding UI element (e.g., a menu item) accordingly.
   */
  kEventFontPanelClosed         = 1,

  /*
   * The user has specified font settings in the Font Panel. The
   * application can obtain these settings from the event, in which
   * they are stored as parameters. Not all parameters are guaranteed
   * to be present; the application should check for all those which it
   * recognizes and apply the ones found as appropriate to the target
   * text.
   */
  kEventFontSelection           = 2
};

/*
    Parameters for font events:

    kEventFontPanelClosed
        None.
        
    kEventFontSelection
        -->     kEventParamATSUFontID               typeATSUFontID
        -->     kEventParamATSUFontSize             typeATSUSize
        -->     kEventParamFMFontFamily             typeFMFontFamily
        -->     kEventParamFMFontSize               typeFMFontSize
        -->     kEventParamFontColor                typeFontColor
*/
enum {
  typeATSUFontID                = typeUInt32, /* ATSUI font ID.*/
  typeATSUSize                  = typeFixed, /* ATSUI font size.*/
  typeFMFontFamily              = typeSInt16, /* Font family reference.*/
  typeFMFontStyle               = typeSInt16, /* Quickdraw font style*/
  typeFMFontSize                = typeSInt16, /* Integer font size.*/
  typeFontColor                 = typeRGBColor, /* Font color spec (optional).*/
  kEventParamATSUFontID         = FOUR_CHAR_CODE('auid'), /* typeATSUFontID*/
  kEventParamATSUFontSize       = FOUR_CHAR_CODE('ausz'), /* typeATSUSize*/
  kEventParamFMFontFamily       = FOUR_CHAR_CODE('fmfm'), /* typeFMFontFamily*/
  kEventParamFMFontStyle        = FOUR_CHAR_CODE('fmst'), /* typeFMFontStyle*/
  kEventParamFMFontSize         = FOUR_CHAR_CODE('fmsz'), /* typeFMFontSize*/
  kEventParamFontColor          = FOUR_CHAR_CODE('fclr') /* typeFontColor*/
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Other Font Panel Constants
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
Error codes (Font Panel codes in range [-8880,-8899]).
*/
enum {
  fontPanelShowErr              = -8880, /* Can't display the Font Panel.*/
  fontPanelSelectionStyleErr    = -8881, /* Bad font selection style info.*/
  fontPanelFontSelectionQDStyleVersionErr = -8882 /* Unsupported record version.*/
};

/*
Type of font information passed in SetFontPanelInfo(). If the client is
sending ATSUI style data, it specifies kFontSelectionATSUIType; if it is
sending Quickdraw style data, it specifies kFontSelectionQDType.
*/
enum {
  kFontSelectionATSUIType       = FOUR_CHAR_CODE('astl'), /* Use ATSUIStyle collection.*/
  kFontSelectionQDType          = FOUR_CHAR_CODE('qstl') /* Use FontSelectionQDStyle record.*/
};

/*
Supported versions of the FontSelectionQDStyle record. Clients should always set
the <version> field to one of these values.
*/
enum {
  kFontSelectionQDStyleVersionZero = 0
};


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Font Panel Types
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
Record specifying the font information to be specified in the Font
Panel. This record is used if the client is sending Quickdraw style data
(i.e., it specified kFontSelectionQDType in SetFontPanelInfo()).
*/
struct FontSelectionQDStyle {
  UInt32              version;                /* Version number of struct.*/
  FMFontFamilyInstance  instance;             /* Font instance data.*/
  FMFontSize          size;                   /* Size of font in points.*/
  Boolean             hasColor;               /* true if color info supplied.*/
  UInt8               reserved;               /* Filler byte.*/
  RGBColor            color;                  /* Color specification for font.*/
};
typedef struct FontSelectionQDStyle     FontSelectionQDStyle;
typedef FontSelectionQDStyle *          FontSelectionQDStylePtr;
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Font Panel Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 *  FPIsFontPanelVisible()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
FPIsFontPanelVisible(void);


/*
 *  FPShowHideFontPanel()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
FPShowHideFontPanel(void);


/*
 *  SetFontInfoForSelection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
SetFontInfoForSelection(
  OSType        iStyleType,
  UInt32        iNumStyles,
  void *        iStyles,
  HIObjectRef   iFPEventTarget);



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

#endif /* __FONTPANEL__ */

