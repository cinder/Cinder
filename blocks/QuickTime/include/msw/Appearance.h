/*
     File:       Appearance.h
 
     Contains:   Appearance Manager Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1994-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __APPEARANCE__
#define __APPEARANCE__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __QDOFFSCREEN__
#include <QDOffscreen.h>
#endif

#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

#ifndef __CONTROLS__
#include <Controls.h>
#endif


/*----------------------------------------------------------------------------------*/
/* Appearance Manager constants, etc.                                               */
/*----------------------------------------------------------------------------------*/
/* Appearance Manager Apple Events (1.1 and later)              */


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
  kAppearanceEventClass         = FOUR_CHAR_CODE('appr'), /* Event Class */
  kAEAppearanceChanged          = FOUR_CHAR_CODE('thme'), /* Appearance changed (e.g. platinum to hi-tech) */
  kAESystemFontChanged          = FOUR_CHAR_CODE('sysf'), /* system font changed */
  kAESmallSystemFontChanged     = FOUR_CHAR_CODE('ssfn'), /* small system font changed */
  kAEViewsFontChanged           = FOUR_CHAR_CODE('vfnt') /* views font changed */
};

/*----------------------------------------------------------------------------------*/
/* Appearance Manager file types                                                    */
/*----------------------------------------------------------------------------------*/
enum {
  kThemeDataFileType            = FOUR_CHAR_CODE('thme'), /* file type for theme files */
  kThemePlatinumFileType        = FOUR_CHAR_CODE('pltn'), /* file type for platinum appearance */
  kThemeCustomThemesFileType    = FOUR_CHAR_CODE('scen'), /* file type for user themes */
  kThemeSoundTrackFileType      = FOUR_CHAR_CODE('tsnd')
};

/*----------------------------------------------------------------------------------*/
/* Appearance Manager Supported Themes                                              */
/* Use CopyThemeIdentifier to get the current theme ID                              */
/*----------------------------------------------------------------------------------*/

#define kThemeAppearancePlatinum       CFSTR( "com.apple.theme.appearance.platinum" )
#define kThemeAppearanceAqua         CFSTR( "com.apple.theme.appearance.aqua" )
#define kThemeAppearanceAquaBlue     CFSTR( "com.apple.theme.appearance.aqua.blue" )
#define kThemeAppearanceAquaGraphite    CFSTR( "com.apple.theme.appearance.aqua.graphite" )

enum {
  kThemeBrushDialogBackgroundActive = 1, /* use with kModalWindowClass */
  kThemeBrushDialogBackgroundInactive = 2, /* use with kModalWindowClass */
  kThemeBrushAlertBackgroundActive = 3, /* use with kAlertWindowClass and kMovableAlertWindowClass */
  kThemeBrushAlertBackgroundInactive = 4, /* use with kAlertWindowClass and kMovableAlertWindowClass */
  kThemeBrushModelessDialogBackgroundActive = 5, /* use with kDocumentWindowClass */
  kThemeBrushModelessDialogBackgroundInactive = 6, /* use with kDocumentWindowClass */
  kThemeBrushUtilityWindowBackgroundActive = 7, /* use with kFloatingWindowClass and kUtilityWindowClass */
  kThemeBrushUtilityWindowBackgroundInactive = 8, /* use with kFloatingWindowClass and kUtilityWindowClass */
  kThemeBrushListViewSortColumnBackground = 9, /* Finder list views */
  kThemeBrushListViewBackground = 10,
  kThemeBrushIconLabelBackground = 11,
  kThemeBrushListViewSeparator  = 12,
  kThemeBrushChasingArrows      = 13,
  kThemeBrushDragHilite         = 14,
  kThemeBrushDocumentWindowBackground = 15, /* use with kDocumentWindowClass */
  kThemeBrushFinderWindowBackground = 16
};

/* Brushes introduced in Appearance 1.1 (Mac OS 8.5) and later */
enum {
  kThemeBrushScrollBarDelimiterActive = 17,
  kThemeBrushScrollBarDelimiterInactive = 18,
  kThemeBrushFocusHighlight     = 19,
  kThemeBrushPopupArrowActive   = 20,
  kThemeBrushPopupArrowPressed  = 21,
  kThemeBrushPopupArrowInactive = 22,
  kThemeBrushAppleGuideCoachmark = 23,
  kThemeBrushIconLabelBackgroundSelected = 24,
  kThemeBrushStaticAreaFill     = 25,
  kThemeBrushActiveAreaFill     = 26,
  kThemeBrushButtonFrameActive  = 27,
  kThemeBrushButtonFrameInactive = 28,
  kThemeBrushButtonFaceActive   = 29,
  kThemeBrushButtonFaceInactive = 30,
  kThemeBrushButtonFacePressed  = 31,
  kThemeBrushButtonActiveDarkShadow = 32,
  kThemeBrushButtonActiveDarkHighlight = 33,
  kThemeBrushButtonActiveLightShadow = 34,
  kThemeBrushButtonActiveLightHighlight = 35,
  kThemeBrushButtonInactiveDarkShadow = 36,
  kThemeBrushButtonInactiveDarkHighlight = 37,
  kThemeBrushButtonInactiveLightShadow = 38,
  kThemeBrushButtonInactiveLightHighlight = 39,
  kThemeBrushButtonPressedDarkShadow = 40,
  kThemeBrushButtonPressedDarkHighlight = 41,
  kThemeBrushButtonPressedLightShadow = 42,
  kThemeBrushButtonPressedLightHighlight = 43,
  kThemeBrushBevelActiveLight   = 44,
  kThemeBrushBevelActiveDark    = 45,
  kThemeBrushBevelInactiveLight = 46,
  kThemeBrushBevelInactiveDark  = 47
};

/* Brushes introduced in Appearance 1.1.1 (Mac OS 9.0) and later */
enum {
  kThemeBrushNotificationWindowBackground = 48
};

/* Brushes introduced in Carbon */
enum {
  kThemeBrushMovableModalBackground = 49, /* use with kMovableModalWindowClass; available in Mac OS X, and CarbonLib 1.3 and later */
  kThemeBrushSheetBackgroundOpaque = 50, /* use with kSheetWindowClass and kSheetAlertWindowClass; available in Mac OS X, and CarbonLib 1.3 and later */
  kThemeBrushDrawerBackground   = 51,   /* use with kDrawerWindowClass; available in Mac OS X, and CarbonLib 1.3 and later */
  kThemeBrushToolbarBackground  = 52,   /* use with kToolbarWindowClass; available in Mac OS X, and CarbonLib 1.6 and later */
  kThemeBrushSheetBackgroundTransparent = 53, /* use with kSheetWindowClass and kSheetAlertWindowClass; available in Mac OS X 10.1 and CarbonLib 1.6, and later */
  kThemeBrushMenuBackground     = 54,   /* available in Mac OS X 10.1 and CarbonLib 1.6, and later */
  kThemeBrushMenuBackgroundSelected = 55 /* available in Mac OS X 10.1 and CarbonLib 1.6, and later */
};

/* Appearance X or later theme brush compatibility synonyms */
enum {
  kThemeBrushSheetBackground    = kThemeBrushSheetBackgroundOpaque
};

/* These values are meta-brushes, specific colors that do not       */
/* change from theme to theme. You can use them instead of using    */
/* direct RGB values.                                               */
enum {
  kThemeBrushBlack              = -1,
  kThemeBrushWhite              = -2,
  kThemeBrushPrimaryHighlightColor = -3, /* available in Mac OS 10.1 and CarbonLib 1.6, and later*/
  kThemeBrushSecondaryHighlightColor = -4 /* available in Mac OS 10.1 and CarbonLib 1.6, and later*/
};

typedef SInt16                          ThemeBrush;
enum {
  kThemeTextColorDialogActive   = 1,
  kThemeTextColorDialogInactive = 2,
  kThemeTextColorAlertActive    = 3,
  kThemeTextColorAlertInactive  = 4,
  kThemeTextColorModelessDialogActive = 5,
  kThemeTextColorModelessDialogInactive = 6,
  kThemeTextColorWindowHeaderActive = 7,
  kThemeTextColorWindowHeaderInactive = 8,
  kThemeTextColorPlacardActive  = 9,
  kThemeTextColorPlacardInactive = 10,
  kThemeTextColorPlacardPressed = 11,
  kThemeTextColorPushButtonActive = 12,
  kThemeTextColorPushButtonInactive = 13,
  kThemeTextColorPushButtonPressed = 14,
  kThemeTextColorBevelButtonActive = 15,
  kThemeTextColorBevelButtonInactive = 16,
  kThemeTextColorBevelButtonPressed = 17,
  kThemeTextColorPopupButtonActive = 18,
  kThemeTextColorPopupButtonInactive = 19,
  kThemeTextColorPopupButtonPressed = 20,
  kThemeTextColorIconLabel      = 21,
  kThemeTextColorListView       = 22
};

/* Text Colors available in Appearance 1.0.1 or later */
enum {
  kThemeTextColorDocumentWindowTitleActive = 23,
  kThemeTextColorDocumentWindowTitleInactive = 24,
  kThemeTextColorMovableModalWindowTitleActive = 25,
  kThemeTextColorMovableModalWindowTitleInactive = 26,
  kThemeTextColorUtilityWindowTitleActive = 27,
  kThemeTextColorUtilityWindowTitleInactive = 28,
  kThemeTextColorPopupWindowTitleActive = 29,
  kThemeTextColorPopupWindowTitleInactive = 30,
  kThemeTextColorRootMenuActive = 31,
  kThemeTextColorRootMenuSelected = 32,
  kThemeTextColorRootMenuDisabled = 33,
  kThemeTextColorMenuItemActive = 34,
  kThemeTextColorMenuItemSelected = 35,
  kThemeTextColorMenuItemDisabled = 36,
  kThemeTextColorPopupLabelActive = 37,
  kThemeTextColorPopupLabelInactive = 38
};


/* Text colors available in Appearance 1.1 or later */
enum {
  kThemeTextColorTabFrontActive = 39,
  kThemeTextColorTabNonFrontActive = 40,
  kThemeTextColorTabNonFrontPressed = 41,
  kThemeTextColorTabFrontInactive = 42,
  kThemeTextColorTabNonFrontInactive = 43,
  kThemeTextColorIconLabelSelected = 44,
  kThemeTextColorBevelButtonStickyActive = 45,
  kThemeTextColorBevelButtonStickyInactive = 46
};

/* Text colors available in Appearance 1.1.1 or later */
enum {
  kThemeTextColorNotification   = 47
};


/* Text colors only available later than OS X 10.1.3 */
enum {
  kThemeTextColorSystemDetail   = 48
};

/* These values are specific colors that do not change from             */
/* theme to theme. You can use them instead of using direct RGB values. */
enum {
  kThemeTextColorBlack          = -1,
  kThemeTextColorWhite          = -2
};

typedef SInt16                          ThemeTextColor;
/* States to draw primitives: disabled, active, and pressed (hilited) */
enum {
  kThemeStateInactive           = 0,
  kThemeStateActive             = 1,
  kThemeStatePressed            = 2,
  kThemeStateRollover           = 6,
  kThemeStateUnavailable        = 7,
  kThemeStateUnavailableInactive = 8
};

/* obsolete name */
enum {
  kThemeStateDisabled           = 0
};

enum {
  kThemeStatePressedUp          = 2,    /* draw with up pressed     (increment/decrement buttons) */
  kThemeStatePressedDown        = 3     /* draw with down pressed (increment/decrement buttons) */
};

typedef UInt32                          ThemeDrawState;
/*----------------------------------------------------------------------------------*/
/* Theme cursor selectors available in Appearance 1.1 or later                      */
/*----------------------------------------------------------------------------------*/
enum {
  kThemeArrowCursor             = 0,
  kThemeCopyArrowCursor         = 1,
  kThemeAliasArrowCursor        = 2,
  kThemeContextualMenuArrowCursor = 3,
  kThemeIBeamCursor             = 4,
  kThemeCrossCursor             = 5,
  kThemePlusCursor              = 6,
  kThemeWatchCursor             = 7,    /* Can Animate */
  kThemeClosedHandCursor        = 8,
  kThemeOpenHandCursor          = 9,
  kThemePointingHandCursor      = 10,
  kThemeCountingUpHandCursor    = 11,   /* Can Animate */
  kThemeCountingDownHandCursor  = 12,   /* Can Animate */
  kThemeCountingUpAndDownHandCursor = 13, /* Can Animate */
  kThemeSpinningCursor          = 14,   /* Can Animate */
  kThemeResizeLeftCursor        = 15,
  kThemeResizeRightCursor       = 16,
  kThemeResizeLeftRightCursor   = 17,
  kThemeNotAllowedCursor        = 18    /* Jaguar or Later */
};

typedef UInt32                          ThemeCursor;
/*----------------------------------------------------------------------------------*/
/* Theme menu bar drawing states                                                    */
/*----------------------------------------------------------------------------------*/
enum {
  kThemeMenuBarNormal           = 0,
  kThemeMenuBarSelected         = 1
};

typedef UInt16                          ThemeMenuBarState;
/* attributes */
enum {
  kThemeMenuSquareMenuBar       = (1 << 0)
};

/*----------------------------------------------------------------------------------*/
/* Theme menu drawing states                                                        */
/*----------------------------------------------------------------------------------*/
enum {
  kThemeMenuActive              = 0,
  kThemeMenuSelected            = 1,
  kThemeMenuDisabled            = 3
};

typedef UInt16                          ThemeMenuState;
/*------------------------------------------------------------------------------------------*/
/* MenuType: add kThemeMenuTypeInactive to menu type for DrawThemeMenuBackground if entire  */
/* menu is inactive                                                                         */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeMenuTypePullDown        = 0,
  kThemeMenuTypePopUp           = 1,
  kThemeMenuTypeHierarchical    = 2,
  kThemeMenuTypeInactive        = 0x0100
};

typedef UInt16                          ThemeMenuType;
enum {
  kThemeMenuItemPlain           = 0,
  kThemeMenuItemHierarchical    = 1,    /* item has hierarchical arrow*/
  kThemeMenuItemScrollUpArrow   = 2,    /* for scrollable menus, indicates item is scroller*/
  kThemeMenuItemScrollDownArrow = 3,
  kThemeMenuItemAtTop           = 0x0100, /* indicates item is being drawn at top of menu*/
  kThemeMenuItemAtBottom        = 0x0200, /* indicates item is being drawn at bottom of menu*/
  kThemeMenuItemHierBackground  = 0x0400, /* item is within a hierarchical menu*/
  kThemeMenuItemPopUpBackground = 0x0800, /* item is within a popped up menu*/
  kThemeMenuItemHasIcon         = 0x8000, /* add into non-arrow type when icon present*/
  kThemeMenuItemNoBackground    = 0x4000 /* don't draw the menu background while drawing this item (Mac OS X only)*/
};

typedef UInt16                          ThemeMenuItemType;
/*------------------------------------------------------------------------------------------*/
/* Theme Backgrounds                                                                        */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeBackgroundTabPane       = 1,
  kThemeBackgroundPlacard       = 2,
  kThemeBackgroundWindowHeader  = 3,
  kThemeBackgroundListViewWindowHeader = 4,
  kThemeBackgroundSecondaryGroupBox = 5
};

typedef UInt32                          ThemeBackgroundKind;
/*------------------------------------------------------------------------------------------*/
/* Theme Collection tags for Get/SetTheme                                                   */
/*                                                                                          */
/*  X ALERT: Please note that Get/SetTheme are severely neutered under Mac OS X at present. */
/*           The first group of tags below are available to get under both 9 and X. The     */
/*           second group is 9 only. None of the tags can be used in SetTheme on X, as it   */
/*           is completely inert on X, and will return unimpErr.                            */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeNameTag                 = FOUR_CHAR_CODE('name'), /* Str255*/
  kThemeVariantNameTag          = FOUR_CHAR_CODE('varn'), /* Str255*/
  kThemeVariantBaseTintTag      = FOUR_CHAR_CODE('tint'), /* RGBColor (10.1 and later)*/
  kThemeHighlightColorTag       = FOUR_CHAR_CODE('hcol'), /* RGBColor*/
  kThemeScrollBarArrowStyleTag  = FOUR_CHAR_CODE('sbar'), /* ThemeScrollBarArrowStyle*/
  kThemeScrollBarThumbStyleTag  = FOUR_CHAR_CODE('sbth'), /* ThemeScrollBarThumbStyle*/
  kThemeSoundsEnabledTag        = FOUR_CHAR_CODE('snds'), /* Boolean*/
  kThemeDblClickCollapseTag     = FOUR_CHAR_CODE('coll') /* Boolean*/
};

enum {
  kThemeAppearanceFileNameTag   = FOUR_CHAR_CODE('thme'), /* Str255*/
  kThemeSystemFontTag           = FOUR_CHAR_CODE('lgsf'), /* Str255*/
  kThemeSmallSystemFontTag      = FOUR_CHAR_CODE('smsf'), /* Str255*/
  kThemeViewsFontTag            = FOUR_CHAR_CODE('vfnt'), /* Str255*/
  kThemeViewsFontSizeTag        = FOUR_CHAR_CODE('vfsz'), /* SInt16*/
  kThemeDesktopPatternNameTag   = FOUR_CHAR_CODE('patn'), /* Str255*/
  kThemeDesktopPatternTag       = FOUR_CHAR_CODE('patt'), /* <variable-length data> (flattened pattern)*/
  kThemeDesktopPictureNameTag   = FOUR_CHAR_CODE('dpnm'), /* Str255*/
  kThemeDesktopPictureAliasTag  = FOUR_CHAR_CODE('dpal'), /* <alias handle>*/
  kThemeDesktopPictureAlignmentTag = FOUR_CHAR_CODE('dpan'), /* UInt32 (see the Picture Alignments below)*/
  kThemeHighlightColorNameTag   = FOUR_CHAR_CODE('hcnm'), /* Str255*/
  kThemeExamplePictureIDTag     = FOUR_CHAR_CODE('epic'), /* SInt16*/
  kThemeSoundTrackNameTag       = FOUR_CHAR_CODE('sndt'), /* Str255*/
  kThemeSoundMaskTag            = FOUR_CHAR_CODE('smsk'), /* UInt32*/
  kThemeUserDefinedTag          = FOUR_CHAR_CODE('user'), /* Boolean (this should _always_ be true if present - used by Control Panel).*/
  kThemeSmoothFontEnabledTag    = FOUR_CHAR_CODE('smoo'), /* Boolean*/
  kThemeSmoothFontMinSizeTag    = FOUR_CHAR_CODE('smos') /* UInt16 (must be >= 12 and <= 24)*/
};

/* Picture Aligmnents that might be reported in the data for kThemeDesktopPictureAlignmentTag*/
enum {
  kTiledOnScreen                = 1,    /* draws picture repeatedly*/
  kCenterOnScreen               = 2,    /* "actual size", shows pattern on sides or clips picture if necessary*/
  kFitToScreen                  = 3,    /* shrinks if necessary*/
  kFillScreen                   = 4,    /* messes up aspect ratio if necessary*/
  kUseBestGuess                 = 5     /* heuristically determines the best way to display the picture based on picture and monitor sizes*/
};

/*------------------------------------------------------------------------------------------*/
/* Theme Control Settings                                                                   */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeCheckBoxClassicX        = 0,    /* check box with an 'X'*/
  kThemeCheckBoxCheckMark       = 1     /* check box with a real check mark*/
};

typedef UInt16                          ThemeCheckBoxStyle;
enum {
  kThemeScrollBarArrowsSingle   = 0,    /* single arrow on each end*/
  kThemeScrollBarArrowsLowerRight = 1   /* double arrows only on right or bottom*/
};

typedef UInt16                          ThemeScrollBarArrowStyle;
enum {
  kThemeScrollBarThumbNormal    = 0,    /* normal, classic thumb size*/
  kThemeScrollBarThumbProportional = 1  /* proportional thumbs*/
};

typedef UInt16                          ThemeScrollBarThumbStyle;
/*------------------------------------------------------------------------------------------*/
/* Font constants                                                                           */
/*------------------------------------------------------------------------------------------*/

/*
 *  Summary:
 *    A ThemeFontID value is a virtual font ID which can be passed to
 *    one of the Appearance Manager's text-related routines. Within
 *    those routines, the ThemeFontID is mapped into the appropriate
 *    real font (or fonts), size, and style based on the system
 *    appearance (Platinum on Mac OS 9, Aqua on Mac OS X), the string
 *    to be rendered (if any), the language/ script that the app is
 *    running in, and possibly other factors. The ThemeFontIDs allow
 *    you to get the correct text appearance for the platform your app
 *    is currently running on.
 */
enum {

  /*
   * The font used to draw most interface elements. If you can't find a
   * more appropriate font from the list below, you should use this
   * one. This font is suitable for drawing titles on most custom
   * widgets/buttons, as well as most static text in dialogs and
   * windows.
   */
  kThemeSystemFont              = 0,

  /*
   * The font used to draw interface elements when space is at a
   * premium. It draws a slightly smaller font compared to
   * kThemeSystemFont.
   */
  kThemeSmallSystemFont         = 1,

  /*
   * Identical to kThemeSmallSystemFont, except it draws bolded (or
   * otherwise emphasized in some fashion appropriate to your
   * application's language/script).
   */
  kThemeSmallEmphasizedSystemFont = 2,

  /*
   * The font used to draw file and folder names in Finder windows or
   * other browsable lists.
   */
  kThemeViewsFont               = 3,    /* The following ID's are only available with MacOS X or CarbonLib 1.3 and later*/

  /*
   * Identical to kThemeSystemFont, except it draws bolded (or
   * otherwise emphasized in some fashion appropriate to your
   * application's language/script). Only available on Mac OS X or
   * CarbonLib 1.3 or later.
   */
  kThemeEmphasizedSystemFont    = 4,

  /*
   * An analog to the Script Manager's notion of the Application Font.
   * This font is a suitable default choice for your application's
   * document-style text editing areas. Only available on Mac OS X or
   * CarbonLib 1.3 or later.
   */
  kThemeApplicationFont         = 5,

  /*
   * Generally smaller than kThemeSmallSystemFont, this font is
   * appropriate for drawing text labels next to image content that
   * reinforces the text's meaning (such as on a bevel button). Only
   * available on Mac OS X or CarbonLib 1.3 or later.
   */
  kThemeLabelFont               = 6,

  /*
   * The font used to draw menu titles in the menu bar. Only available
   * on Mac OS X or CarbonLib 1.3 or later.
   */
  kThemeMenuTitleFont           = 100,

  /*
   * The font used to draw menu items in the menus. Only available on
   * Mac OS X or CarbonLib 1.3 or later.
   */
  kThemeMenuItemFont            = 101,

  /*
   * The font used to draw menu item marks in the menus. Only available
   * on Mac OS X or CarbonLib 1.3 or later.
   */
  kThemeMenuItemMarkFont        = 102,

  /*
   * The font used to draw menu item command key equivalents in the
   * menus. Only available on Mac OS X or CarbonLib 1.3 or later.
   */
  kThemeMenuItemCmdKeyFont      = 103,

  /*
   * The font used to draw text in most window title bars. Only
   * available on Mac OS X or CarbonLib 1.3 or later.
   */
  kThemeWindowTitleFont         = 104,

  /*
   * The font used to draw text labels on push buttons. Only available
   * on Mac OS X or CarbonLib 1.3 or later.
   */
  kThemePushButtonFont          = 105,

  /*
   * The font used to draw text in utility window title bars. Only
   * available on Mac OS X or CarbonLib 1.3 or later.
   */
  kThemeUtilityWindowTitleFont  = 106,

  /*
   * The font used to draw the first (and most important) message of an
   * alert window. Only available on Mac OS X or CarbonLib 1.3 or later.
   */
  kThemeAlertHeaderFont         = 107,
  kThemeSystemFontDetail        = 7,
  kThemeSystemFontDetailEmphasized = 8,

  /*
   * Unlike the other ThemeFontIDs, this one doesn't map to a font
   * appropriate to your application's language or script. It maps
   * directly to the font, size, and style of the current Quickdraw
   * port. This allows you to get somewhat customized behavior out of
   * the APIs which take ThemeFontIDs. Note, however, that
   * kThemeCurrentPortFont does not (and will never) support all
   * Quickdraw styles on all platforms; in particular, outline and
   * shadow style are not supported on Mac OS X. Additionally,
   * kThemeCurrentPortFont is not (and will never be) completely
   * unicode savvy; use of kThemeCurrentPortFont may result in errors
   * having to do with the current port's font not being appropriate
   * for rendering or measuring all glyphs in a given unicode string.
   * Because of overhead associated with gathering Quickdraw font
   * information and converting it to the native font format on Mac OS
   * X, use of kThemeCurrentPortFont may slow down your text drawing
   * and measuring significantly compared to other ThemeFontIDs.
   * Instead of using kThemeCurrentPortFont, your application will
   * probably be better served by using one of the other ThemeFontIDs;
   * use kThemeCurrentPortFont only as a last resort. Only available on
   * Mac OS X or CarbonLib 1.3 or later.
   */
  kThemeCurrentPortFont         = 200,  /* Available in JAGUAR or later*/

  /*
   * The font used to draw the label of a toolbar item. Available in
   * Mac OS X 10.2 or later.
   */
  kThemeToolbarFont             = 108
};

/* This is the total of the PUBLIC ThemeFontIDs!*/
enum {
  kPublicThemeFontCount         = 17
};

typedef UInt16                          ThemeFontID;
/*------------------------------------------------------------------------------------------*/
/* Tab constants                                                                            */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeTabNonFront             = 0,
  kThemeTabNonFrontPressed      = 1,
  kThemeTabNonFrontInactive     = 2,
  kThemeTabFront                = 3,
  kThemeTabFrontInactive        = 4,
  kThemeTabNonFrontUnavailable  = 5,
  kThemeTabFrontUnavailable     = 6
};

typedef UInt16                          ThemeTabStyle;
enum {
  kThemeTabNorth                = 0,
  kThemeTabSouth                = 1,
  kThemeTabEast                 = 2,
  kThemeTabWest                 = 3
};

typedef UInt16                          ThemeTabDirection;
/* NOTE ON TAB HEIGHT                                                                       */
/* Use the kThemeSmallTabHeightMax and kThemeLargeTabHeightMax when calculating the rects   */
/* to draw tabs into. This height includes the tab frame overlap. Tabs that are not in the  */
/* front are only drawn down to where they meet the frame, as if the height was just        */
/* kThemeLargeTabHeight, for example, as opposed to the ...Max constant. Remember that for  */
/* East and West tabs, the height referred to below is actually the width.                  */
enum {
  kThemeSmallTabHeight          = 16,   /* amount small tabs protrude from frame.*/
  kThemeLargeTabHeight          = 21,   /* amount large tabs protrude from frame.*/
  kThemeTabPaneOverlap          = 3,    /* amount tabs overlap frame.*/
  kThemeSmallTabHeightMax       = 19,   /* small tab height + overlap*/
  kThemeLargeTabHeightMax       = 24    /* large tab height + overlap*/
};

/*------------------------------------------------------------------------------------------*/
/* Track kinds                                                                              */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeMediumScrollBar         = 0,
  kThemeSmallScrollBar          = 1,
  kThemeMediumSlider            = 2,
  kThemeMediumProgressBar       = 3,
  kThemeMediumIndeterminateBar  = 4,
  kThemeRelevanceBar            = 5,
  kThemeSmallSlider             = 6,
  kThemeLargeProgressBar        = 7,
  kThemeLargeIndeterminateBar   = 8
};

typedef UInt16                          ThemeTrackKind;
/*------------------------------------------------------------------------------------------*/
/* Track enable states                                                                      */
/*------------------------------------------------------------------------------------------*/
enum {
                                        /* track states */
  kThemeTrackActive             = 0,
  kThemeTrackDisabled           = 1,
  kThemeTrackNothingToScroll    = 2,
  kThemeTrackInactive           = 3
};

typedef UInt8                           ThemeTrackEnableState;
/*------------------------------------------------------------------------------------------*/
/* Track pressed states                                                                     */
/*------------------------------------------------------------------------------------------*/
enum {
                                        /* press states (ignored unless track is active) */
  kThemeLeftOutsideArrowPressed = 0x01,
  kThemeLeftInsideArrowPressed  = 0x02,
  kThemeLeftTrackPressed        = 0x04,
  kThemeThumbPressed            = 0x08,
  kThemeRightTrackPressed       = 0x10,
  kThemeRightInsideArrowPressed = 0x20,
  kThemeRightOutsideArrowPressed = 0x40,
  kThemeTopOutsideArrowPressed  = kThemeLeftOutsideArrowPressed,
  kThemeTopInsideArrowPressed   = kThemeLeftInsideArrowPressed,
  kThemeTopTrackPressed         = kThemeLeftTrackPressed,
  kThemeBottomTrackPressed      = kThemeRightTrackPressed,
  kThemeBottomInsideArrowPressed = kThemeRightInsideArrowPressed,
  kThemeBottomOutsideArrowPressed = kThemeRightOutsideArrowPressed
};

typedef UInt8                           ThemeTrackPressState;
/*------------------------------------------------------------------------------------------*/
/* Thumb directions                                                                         */
/*------------------------------------------------------------------------------------------*/
enum {
                                        /* thumb direction */
  kThemeThumbPlain              = 0,
  kThemeThumbUpward             = 1,
  kThemeThumbDownward           = 2
};

typedef UInt8                           ThemeThumbDirection;
/*------------------------------------------------------------------------------------------*/
/* Track attributes                                                                         */
/*------------------------------------------------------------------------------------------*/

/*
 *  Discussion:
 *    Theme track attributes control the look of the track elements as
 *    drawn by the DrawThemeTrackFoo as well as the region returned by
 *    GetThemeTrackFoo.
 */
enum {

  /*
   * The track is drawn horizontally.
   */
  kThemeTrackHorizontal         = (1 << 0),

  /*
   * The track progresses from right to left.
   */
  kThemeTrackRightToLeft        = (1 << 1),

  /*
   * The track's thumb should be drawn.
   */
  kThemeTrackShowThumb          = (1 << 2),

  /*
   * The provided thumbRgn should be drawn opaque, not as a ghost.
   */
  kThemeTrackThumbRgnIsNotGhost = (1 << 3),

  /*
   * The track scroll bar doesn't have arrows.  This attribute
   * currently has no effect
   */
  kThemeTrackNoScrollBarArrows  = (1 << 4),

  /*
   * The thumb has focus.  This attribute currently has effect only on
   * sliders.  Available only in Mac OS X after 10.1.
   */
  kThemeTrackHasFocus           = (1 << 5)
};

typedef UInt16                          ThemeTrackAttributes;
/*------------------------------------------------------------------------------------------*/
/* Track info block                                                                         */
/*------------------------------------------------------------------------------------------*/
struct ScrollBarTrackInfo {
  SInt32              viewsize;               /* current view range size */
  ThemeTrackPressState  pressState;           /* pressed parts state */
};
typedef struct ScrollBarTrackInfo       ScrollBarTrackInfo;
struct SliderTrackInfo {
  ThemeThumbDirection  thumbDir;              /* thumb direction */
  ThemeTrackPressState  pressState;           /* pressed parts state */
};
typedef struct SliderTrackInfo          SliderTrackInfo;
struct ProgressTrackInfo {
  UInt8               phase;                  /* phase for indeterminate progress */
};
typedef struct ProgressTrackInfo        ProgressTrackInfo;
struct ThemeTrackDrawInfo {
  ThemeTrackKind      kind;                   /* what kind of track this info is for */
  Rect                bounds;                 /* track basis rectangle */
  SInt32              min;                    /* min track value */
  SInt32              max;                    /* max track value */
  SInt32              value;                  /* current thumb value */
  UInt32              reserved;

  ThemeTrackAttributes  attributes;           /* various track attributes */
  ThemeTrackEnableState  enableState;         /* enable state */
  UInt8               filler1;

  union {
    ScrollBarTrackInfo  scrollbar;
    SliderTrackInfo     slider;
    ProgressTrackInfo   progress;
  }                       trackInfo;
};
typedef struct ThemeTrackDrawInfo       ThemeTrackDrawInfo;
/*------------------------------------------------------------------------------------------*/
/* ThemeWindowAttributes                                                                    */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeWindowHasGrow           = (1 << 0), /* can the size of the window be changed by the user? */
  kThemeWindowHasHorizontalZoom = (1 << 3), /* window can zoom only horizontally */
  kThemeWindowHasVerticalZoom   = (1 << 4), /* window can zoom only vertically */
  kThemeWindowHasFullZoom       = kThemeWindowHasHorizontalZoom + kThemeWindowHasVerticalZoom, /* window zooms in all directions */
  kThemeWindowHasCloseBox       = (1 << 5), /* window has a close box */
  kThemeWindowHasCollapseBox    = (1 << 6), /* window has a collapse box */
  kThemeWindowHasTitleText      = (1 << 7), /* window has a title/title icon */
  kThemeWindowIsCollapsed       = (1 << 8), /* window is in the collapsed state */
  kThemeWindowHasDirty          = (1 << 9)
};


typedef UInt32                          ThemeWindowAttributes;
/*------------------------------------------------------------------------------------------*/
/* Window Types Supported by the Appearance Manager                                         */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeDocumentWindow          = 0,
  kThemeDialogWindow            = 1,
  kThemeMovableDialogWindow     = 2,
  kThemeAlertWindow             = 3,
  kThemeMovableAlertWindow      = 4,
  kThemePlainDialogWindow       = 5,
  kThemeShadowDialogWindow      = 6,
  kThemePopupWindow             = 7,
  kThemeUtilityWindow           = 8,
  kThemeUtilitySideWindow       = 9,
  kThemeSheetWindow             = 10,
  kThemeDrawerWindow            = 11
};

typedef UInt16                          ThemeWindowType;
/*------------------------------------------------------------------------------------------*/
/* Window Widgets Supported by the Appearance Manager                                       */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeWidgetCloseBox          = 0,
  kThemeWidgetZoomBox           = 1,
  kThemeWidgetCollapseBox       = 2,
  kThemeWidgetDirtyCloseBox     = 6
};

typedef UInt16                          ThemeTitleBarWidget;
/*------------------------------------------------------------------------------------------*/
/* Popup arrow orientations                                                                 */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeArrowLeft               = 0,
  kThemeArrowDown               = 1,
  kThemeArrowRight              = 2,
  kThemeArrowUp                 = 3
};

typedef UInt16                          ThemeArrowOrientation;
/*------------------------------------------------------------------------------------------*/
/* Popup arrow sizes                                                                        */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeArrow3pt                = 0,
  kThemeArrow5pt                = 1,
  kThemeArrow7pt                = 2,
  kThemeArrow9pt                = 3
};

typedef UInt16                          ThemePopupArrowSize;
/*------------------------------------------------------------------------------------------*/
/* Grow box directions                                                                      */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeGrowLeft                = (1 << 0), /* can grow to the left */
  kThemeGrowRight               = (1 << 1), /* can grow to the right */
  kThemeGrowUp                  = (1 << 2), /* can grow up */
  kThemeGrowDown                = (1 << 3) /* can grow down */
};

typedef UInt16                          ThemeGrowDirection;
/*------------------------------------------------------------------------------------------*/
/* Button kinds                                                                             */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemePushButton              = 0,
  kThemeCheckBox                = 1,
  kThemeRadioButton             = 2,
  kThemeBevelButton             = 3,    /* bevel button (obsolete) */
  kThemeArrowButton             = 4,    /* popup button without text (no label). See ThemeButtonAdornment for glyphs. */
  kThemePopupButton             = 5,    /* popup button */
  kThemeDisclosureButton        = 6,
  kThemeIncDecButton            = 7,    /* increment/decrement buttons  (no label) */
  kThemeSmallBevelButton        = 8,    /* small-shadow bevel button */
  kThemeMediumBevelButton       = 3,    /* med-shadow bevel button */
  kThemeLargeBevelButton        = 9,    /* large-shadow bevel button */
  kThemeListHeaderButton        = 10,   /* sort button for top of list */
  kThemeRoundButton             = 11,   /* round button */
  kThemeLargeRoundButton        = 12,   /* large round button */
  kThemeSmallCheckBox           = 13,   /* small checkbox */
  kThemeSmallRadioButton        = 14,   /* small radio button */
  kThemeRoundedBevelButton      = 15,   /* rounded bevel button */
  kThemeNormalCheckBox          = kThemeCheckBox,
  kThemeNormalRadioButton       = kThemeRadioButton
};

typedef UInt16                          ThemeButtonKind;
/*------------------------------------------------------------------------------------------*/
/* Common button values                                                                     */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeButtonOff               = 0,
  kThemeButtonOn                = 1,
  kThemeButtonMixed             = 2,
  kThemeDisclosureRight         = 0,
  kThemeDisclosureDown          = 1,
  kThemeDisclosureLeft          = 2
};

typedef UInt16                          ThemeButtonValue;
/*------------------------------------------------------------------------------------------*/
/* Button adornment types                                                                   */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeAdornmentNone           = 0,
  kThemeAdornmentDefault        = (1 << 0), /* if set, draw default ornamentation ( for push button and generic well ) */
  kThemeAdornmentFocus          = (1 << 2), /* if set, draw focus */
  kThemeAdornmentRightToLeft    = (1 << 4), /* if set, draw right to left label */
  kThemeAdornmentDrawIndicatorOnly = (1 << 5), /* if set, don't draw or erase label ( radio, check, disclosure ) */
  kThemeAdornmentHeaderButtonLeftNeighborSelected = (1 << 6), /* if set, draw the left border of the button as selected ( list header button only ) */
  kThemeAdornmentHeaderButtonRightNeighborSelected = (1 << 7), /* if set, draw the right border of the button ( list header button only ) */
  kThemeAdornmentHeaderButtonSortUp = (1 << 8), /* if set, draw the sort indicator pointing upward ( list header button only ) */
  kThemeAdornmentHeaderMenuButton = (1 << 9), /* if set, draw as a header menu button ( list header button only ) */
  kThemeAdornmentHeaderButtonNoShadow = (1 << 10), /* if set, draw the non-shadow area of the button ( list header button only ) */
  kThemeAdornmentHeaderButtonShadowOnly = (1 << 11), /* if set, draw the only the shadow area of the button ( list header button only ) */
  kThemeAdornmentNoShadow       = kThemeAdornmentHeaderButtonNoShadow, /* old name */
  kThemeAdornmentShadowOnly     = kThemeAdornmentHeaderButtonShadowOnly, /* old name */
  kThemeAdornmentArrowLeftArrow = (1 << 6), /* If set, draw a left arrow on the arrow button */
  kThemeAdornmentArrowDownArrow = (1 << 7), /* If set, draw a down arrow on the arrow button */
  kThemeAdornmentArrowDoubleArrow = (1 << 8), /* If set, draw a double arrow on the arrow button */
  kThemeAdornmentArrowUpArrow   = (1 << 9) /* If set, draw a up arrow on the arrow button */
};

typedef UInt16                          ThemeButtonAdornment;
/*------------------------------------------------------------------------------------------*/
/* Button drawing info block                                                                */
/*------------------------------------------------------------------------------------------*/
struct ThemeButtonDrawInfo {
  ThemeDrawState      state;
  ThemeButtonValue    value;
  ThemeButtonAdornment  adornment;
};
typedef struct ThemeButtonDrawInfo      ThemeButtonDrawInfo;
typedef ThemeButtonDrawInfo *           ThemeButtonDrawInfoPtr;
/*------------------------------------------------------------------------------------------*/
/* Sound Support                                                                            */
/*                                                                                          */
/*  X ALERT: Please note that none of the theme sound APIs currently function on X.         */
/*------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------*/
/* Sound classes                                                                            */
/*                                                                                          */
/* You can use the constants below to set what sounds are active using the SetTheme API.    */
/* Use these with the kThemeSoundMask tag.                                                  */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeNoSounds                = 0,
  kThemeWindowSoundsMask        = (1 << 0),
  kThemeMenuSoundsMask          = (1 << 1),
  kThemeControlSoundsMask       = (1 << 2),
  kThemeFinderSoundsMask        = (1 << 3)
};


/*------------------------------------------------------------------------------------------*/
/* Drag Sounds                                                                              */
/*                                                                                          */
/* Drag sounds are looped for the duration of the drag.                                     */
/*                                                                                          */
/* Call BeginThemeDragSound at the start of the drag.                                       */
/* Call EndThemeDragSound when the drag has finished.                                       */
/*                                                                                          */
/* Note that in order to maintain a consistent user experience, only one drag sound may     */
/* occur at a time.  The sound should be attached to a mouse action, start after the        */
/* mouse goes down and stop when the mouse is released.                                     */
/*------------------------------------------------------------------------------------------*/
enum {
  kThemeDragSoundNone           = 0,
  kThemeDragSoundMoveWindow     = FOUR_CHAR_CODE('wmov'),
  kThemeDragSoundGrowWindow     = FOUR_CHAR_CODE('wgro'),
  kThemeDragSoundMoveUtilWindow = FOUR_CHAR_CODE('umov'),
  kThemeDragSoundGrowUtilWindow = FOUR_CHAR_CODE('ugro'),
  kThemeDragSoundMoveDialog     = FOUR_CHAR_CODE('dmov'),
  kThemeDragSoundMoveAlert      = FOUR_CHAR_CODE('amov'),
  kThemeDragSoundMoveIcon       = FOUR_CHAR_CODE('imov'),
  kThemeDragSoundSliderThumb    = FOUR_CHAR_CODE('slth'),
  kThemeDragSoundSliderGhost    = FOUR_CHAR_CODE('slgh'),
  kThemeDragSoundScrollBarThumb = FOUR_CHAR_CODE('sbth'),
  kThemeDragSoundScrollBarGhost = FOUR_CHAR_CODE('sbgh'),
  kThemeDragSoundScrollBarArrowDecreasing = FOUR_CHAR_CODE('sbad'),
  kThemeDragSoundScrollBarArrowIncreasing = FOUR_CHAR_CODE('sbai'),
  kThemeDragSoundDragging       = FOUR_CHAR_CODE('drag')
};

typedef OSType                          ThemeDragSoundKind;
/*--------------------------------------------------------------------------*/
/* State-change sounds                                                      */
/*                                                                          */
/* State-change sounds are played asynchonously as a one-shot.              */
/*                                                                          */
/* Call PlayThemeSound to play the sound.  The sound will play              */
/* asynchronously until complete, then stop automatically.                  */
/*--------------------------------------------------------------------------*/
enum {
  kThemeSoundNone               = 0,
  kThemeSoundMenuOpen           = FOUR_CHAR_CODE('mnuo'), /* menu sounds */
  kThemeSoundMenuClose          = FOUR_CHAR_CODE('mnuc'),
  kThemeSoundMenuItemHilite     = FOUR_CHAR_CODE('mnui'),
  kThemeSoundMenuItemRelease    = FOUR_CHAR_CODE('mnus'),
  kThemeSoundWindowClosePress   = FOUR_CHAR_CODE('wclp'), /* window sounds */
  kThemeSoundWindowCloseEnter   = FOUR_CHAR_CODE('wcle'),
  kThemeSoundWindowCloseExit    = FOUR_CHAR_CODE('wclx'),
  kThemeSoundWindowCloseRelease = FOUR_CHAR_CODE('wclr'),
  kThemeSoundWindowZoomPress    = FOUR_CHAR_CODE('wzmp'),
  kThemeSoundWindowZoomEnter    = FOUR_CHAR_CODE('wzme'),
  kThemeSoundWindowZoomExit     = FOUR_CHAR_CODE('wzmx'),
  kThemeSoundWindowZoomRelease  = FOUR_CHAR_CODE('wzmr'),
  kThemeSoundWindowCollapsePress = FOUR_CHAR_CODE('wcop'),
  kThemeSoundWindowCollapseEnter = FOUR_CHAR_CODE('wcoe'),
  kThemeSoundWindowCollapseExit = FOUR_CHAR_CODE('wcox'),
  kThemeSoundWindowCollapseRelease = FOUR_CHAR_CODE('wcor'),
  kThemeSoundWindowDragBoundary = FOUR_CHAR_CODE('wdbd'),
  kThemeSoundUtilWinClosePress  = FOUR_CHAR_CODE('uclp'), /* utility window sounds */
  kThemeSoundUtilWinCloseEnter  = FOUR_CHAR_CODE('ucle'),
  kThemeSoundUtilWinCloseExit   = FOUR_CHAR_CODE('uclx'),
  kThemeSoundUtilWinCloseRelease = FOUR_CHAR_CODE('uclr'),
  kThemeSoundUtilWinZoomPress   = FOUR_CHAR_CODE('uzmp'),
  kThemeSoundUtilWinZoomEnter   = FOUR_CHAR_CODE('uzme'),
  kThemeSoundUtilWinZoomExit    = FOUR_CHAR_CODE('uzmx'),
  kThemeSoundUtilWinZoomRelease = FOUR_CHAR_CODE('uzmr'),
  kThemeSoundUtilWinCollapsePress = FOUR_CHAR_CODE('ucop'),
  kThemeSoundUtilWinCollapseEnter = FOUR_CHAR_CODE('ucoe'),
  kThemeSoundUtilWinCollapseExit = FOUR_CHAR_CODE('ucox'),
  kThemeSoundUtilWinCollapseRelease = FOUR_CHAR_CODE('ucor'),
  kThemeSoundUtilWinDragBoundary = FOUR_CHAR_CODE('udbd'),
  kThemeSoundWindowOpen         = FOUR_CHAR_CODE('wopn'), /* window close and zoom action */
  kThemeSoundWindowClose        = FOUR_CHAR_CODE('wcls'),
  kThemeSoundWindowZoomIn       = FOUR_CHAR_CODE('wzmi'),
  kThemeSoundWindowZoomOut      = FOUR_CHAR_CODE('wzmo'),
  kThemeSoundWindowCollapseUp   = FOUR_CHAR_CODE('wcol'),
  kThemeSoundWindowCollapseDown = FOUR_CHAR_CODE('wexp'),
  kThemeSoundWindowActivate     = FOUR_CHAR_CODE('wact'),
  kThemeSoundUtilWindowOpen     = FOUR_CHAR_CODE('uopn'),
  kThemeSoundUtilWindowClose    = FOUR_CHAR_CODE('ucls'),
  kThemeSoundUtilWindowZoomIn   = FOUR_CHAR_CODE('uzmi'),
  kThemeSoundUtilWindowZoomOut  = FOUR_CHAR_CODE('uzmo'),
  kThemeSoundUtilWindowCollapseUp = FOUR_CHAR_CODE('ucol'),
  kThemeSoundUtilWindowCollapseDown = FOUR_CHAR_CODE('uexp'),
  kThemeSoundUtilWindowActivate = FOUR_CHAR_CODE('uact'),
  kThemeSoundDialogOpen         = FOUR_CHAR_CODE('dopn'),
  kThemeSoundDialogClose        = FOUR_CHAR_CODE('dlgc'),
  kThemeSoundAlertOpen          = FOUR_CHAR_CODE('aopn'),
  kThemeSoundAlertClose         = FOUR_CHAR_CODE('altc'),
  kThemeSoundPopupWindowOpen    = FOUR_CHAR_CODE('pwop'),
  kThemeSoundPopupWindowClose   = FOUR_CHAR_CODE('pwcl'),
  kThemeSoundButtonPress        = FOUR_CHAR_CODE('btnp'), /* button */
  kThemeSoundButtonEnter        = FOUR_CHAR_CODE('btne'),
  kThemeSoundButtonExit         = FOUR_CHAR_CODE('btnx'),
  kThemeSoundButtonRelease      = FOUR_CHAR_CODE('btnr'),
  kThemeSoundDefaultButtonPress = FOUR_CHAR_CODE('dbtp'), /* default button */
  kThemeSoundDefaultButtonEnter = FOUR_CHAR_CODE('dbte'),
  kThemeSoundDefaultButtonExit  = FOUR_CHAR_CODE('dbtx'),
  kThemeSoundDefaultButtonRelease = FOUR_CHAR_CODE('dbtr'),
  kThemeSoundCancelButtonPress  = FOUR_CHAR_CODE('cbtp'), /* cancel button */
  kThemeSoundCancelButtonEnter  = FOUR_CHAR_CODE('cbte'),
  kThemeSoundCancelButtonExit   = FOUR_CHAR_CODE('cbtx'),
  kThemeSoundCancelButtonRelease = FOUR_CHAR_CODE('cbtr'),
  kThemeSoundCheckboxPress      = FOUR_CHAR_CODE('chkp'), /* checkboxes */
  kThemeSoundCheckboxEnter      = FOUR_CHAR_CODE('chke'),
  kThemeSoundCheckboxExit       = FOUR_CHAR_CODE('chkx'),
  kThemeSoundCheckboxRelease    = FOUR_CHAR_CODE('chkr'),
  kThemeSoundRadioPress         = FOUR_CHAR_CODE('radp'), /* radio buttons */
  kThemeSoundRadioEnter         = FOUR_CHAR_CODE('rade'),
  kThemeSoundRadioExit          = FOUR_CHAR_CODE('radx'),
  kThemeSoundRadioRelease       = FOUR_CHAR_CODE('radr'),
  kThemeSoundScrollArrowPress   = FOUR_CHAR_CODE('sbap'), /* scroll bars */
  kThemeSoundScrollArrowEnter   = FOUR_CHAR_CODE('sbae'),
  kThemeSoundScrollArrowExit    = FOUR_CHAR_CODE('sbax'),
  kThemeSoundScrollArrowRelease = FOUR_CHAR_CODE('sbar'),
  kThemeSoundScrollEndOfTrack   = FOUR_CHAR_CODE('sbte'),
  kThemeSoundScrollTrackPress   = FOUR_CHAR_CODE('sbtp'),
  kThemeSoundSliderEndOfTrack   = FOUR_CHAR_CODE('slte'), /* sliders */
  kThemeSoundSliderTrackPress   = FOUR_CHAR_CODE('sltp'),
  kThemeSoundBalloonOpen        = FOUR_CHAR_CODE('blno'), /* help balloons */
  kThemeSoundBalloonClose       = FOUR_CHAR_CODE('blnc'),
  kThemeSoundBevelPress         = FOUR_CHAR_CODE('bevp'), /* Bevel buttons */
  kThemeSoundBevelEnter         = FOUR_CHAR_CODE('beve'),
  kThemeSoundBevelExit          = FOUR_CHAR_CODE('bevx'),
  kThemeSoundBevelRelease       = FOUR_CHAR_CODE('bevr'),
  kThemeSoundLittleArrowUpPress = FOUR_CHAR_CODE('laup'), /* Little Arrows */
  kThemeSoundLittleArrowDnPress = FOUR_CHAR_CODE('ladp'),
  kThemeSoundLittleArrowEnter   = FOUR_CHAR_CODE('lare'),
  kThemeSoundLittleArrowExit    = FOUR_CHAR_CODE('larx'),
  kThemeSoundLittleArrowUpRelease = FOUR_CHAR_CODE('laur'),
  kThemeSoundLittleArrowDnRelease = FOUR_CHAR_CODE('ladr'),
  kThemeSoundPopupPress         = FOUR_CHAR_CODE('popp'), /* Popup Buttons */
  kThemeSoundPopupEnter         = FOUR_CHAR_CODE('pope'),
  kThemeSoundPopupExit          = FOUR_CHAR_CODE('popx'),
  kThemeSoundPopupRelease       = FOUR_CHAR_CODE('popr'),
  kThemeSoundDisclosurePress    = FOUR_CHAR_CODE('dscp'), /* Disclosure Buttons */
  kThemeSoundDisclosureEnter    = FOUR_CHAR_CODE('dsce'),
  kThemeSoundDisclosureExit     = FOUR_CHAR_CODE('dscx'),
  kThemeSoundDisclosureRelease  = FOUR_CHAR_CODE('dscr'),
  kThemeSoundTabPressed         = FOUR_CHAR_CODE('tabp'), /* Tabs */
  kThemeSoundTabEnter           = FOUR_CHAR_CODE('tabe'),
  kThemeSoundTabExit            = FOUR_CHAR_CODE('tabx'),
  kThemeSoundTabRelease         = FOUR_CHAR_CODE('tabr'),
  kThemeSoundDragTargetHilite   = FOUR_CHAR_CODE('dthi'), /* drag manager */
  kThemeSoundDragTargetUnhilite = FOUR_CHAR_CODE('dtuh'),
  kThemeSoundDragTargetDrop     = FOUR_CHAR_CODE('dtdr'),
  kThemeSoundEmptyTrash         = FOUR_CHAR_CODE('ftrs'), /* finder */
  kThemeSoundSelectItem         = FOUR_CHAR_CODE('fsel'),
  kThemeSoundNewItem            = FOUR_CHAR_CODE('fnew'),
  kThemeSoundReceiveDrop        = FOUR_CHAR_CODE('fdrp'),
  kThemeSoundCopyDone           = FOUR_CHAR_CODE('fcpd'),
  kThemeSoundResolveAlias       = FOUR_CHAR_CODE('fral'),
  kThemeSoundLaunchApp          = FOUR_CHAR_CODE('flap'),
  kThemeSoundDiskInsert         = FOUR_CHAR_CODE('dski'),
  kThemeSoundDiskEject          = FOUR_CHAR_CODE('dske'),
  kThemeSoundFinderDragOnIcon   = FOUR_CHAR_CODE('fdon'),
  kThemeSoundFinderDragOffIcon  = FOUR_CHAR_CODE('fdof')
};

typedef OSType                          ThemeSoundKind;
/*------------------------------------------------------------------------------------------*/
/* Window Metrics                                                                           */
/*------------------------------------------------------------------------------------------*/
/*  Window metrics are used by the Appearance manager to fill in the blanks necessary to    */
/*  draw windows. If a value is not appropriate for the type of window, be sure to fill in  */
/*  the slot in the structure with zero.    For the popupTabOffset parameter, you can pass a*/
/*  real value based on the left edge of the window. This value might be interpreted in a   */
/*  different manner when depending on the value of the popupTabPosition field. The values  */
/*  you can pass into popupTabPosition are:                                                 */
/*                                                                                          */
/*  kThemePopupTabNormalPosition                                                            */
/*      Starts the tab left edge at the position indicated by the popupTabOffset field.     */
/*                                                                                          */
/*  kThemePopupTabCenterOnWindow                                                            */
/*      tells us to ignore the offset field and instead simply center the width of the      */
/*      handle on the window.                                                               */
/*                                                                                          */
/*  kThemePopupTabCenterOnOffset                                                            */
/*      tells us to center the width of the handle around the value passed in offset.       */
/*                                                                                          */
/*  The Appearance Manager will try its best to accomodate the requested placement, but may */
/*  move the handle slightly to make it fit correctly.                                      */
/*                                                                                          */
enum {
  kThemePopupTabNormalPosition  = 0,
  kThemePopupTabCenterOnWindow  = 1,
  kThemePopupTabCenterOnOffset  = 2
};

struct ThemeWindowMetrics {
  UInt16              metricSize;             /* should be always be sizeof( ThemeWindowMetrics )*/
  SInt16              titleHeight;
  SInt16              titleWidth;
  SInt16              popupTabOffset;
  SInt16              popupTabWidth;
  UInt16              popupTabPosition;
};
typedef struct ThemeWindowMetrics       ThemeWindowMetrics;
typedef ThemeWindowMetrics *            ThemeWindowMetricsPtr;
/*------------------------------------------------------------------------------------------*/
/* Theme Metrics                                                                            */
/*------------------------------------------------------------------------------------------*/

/*
 *  Discussion:
 *    Theme metrics allow you to find out sizes of things in the
 *    current environment, such as how wide a scroll bar is, etc.
 */
enum {

  /*
   * The width (or height if horizontal) of a scroll bar.
   */
  kThemeMetricScrollBarWidth    = 0,

  /*
   * The width (or height if horizontal) of a small scroll bar.
   */
  kThemeMetricSmallScrollBarWidth = 1,

  /*
   * The height of the non-label part of a check box control.
   */
  kThemeMetricCheckBoxHeight    = 2,

  /*
   * The height of the non-label part of a radio button control.
   */
  kThemeMetricRadioButtonHeight = 3,

  /*
   * The amount of white space surrounding the text Rect of the text
   * inside of an Edit Text control.  If you select all of the text in
   * an Edit Text control, you can see the white space. The metric is
   * the number of pixels, per side, that the text Rect is outset to
   * create the whitespace Rect.
   */
  kThemeMetricEditTextWhitespace = 4,

  /*
   * The thickness of the Edit Text frame that surrounds the whitespace
   * Rect (that is surrounding the text Rect). The metric is the number
   * of pixels, per side, that the frame Rect is outset from the
   * whitespace Rect.
   */
  kThemeMetricEditTextFrameOutset = 5,

  /*
   * The number of pixels that the list box frame is outset from the
   * content of the list box.
   */
  kThemeMetricListBoxFrameOutset = 6,

  /*
   * This is a deprecated metric.  Don't use it.  It used to describe
   * how far the focus rect used to draw from a control, but control
   * focus drawing no longer uses this information to draw its focus.
   */
  kThemeMetricFocusRectOutset   = 7,

  /*
   * The thickness of the frame drawn by DrawThemeGenericWell.
   */
  kThemeMetricImageWellThickness = 8,

  /*
   * The number of pixels a scrollbar should overlap (actually
   * underlap) any bounding box which surrounds it and scrollable
   * content. This also includes the window frame when a scrolbar is
   * along an edge of the window.
   */
  kThemeMetricScrollBarOverlap  = 9,

  /*
   * The height of the large tab of a tab control.
   */
  kThemeMetricLargeTabHeight    = 10,

  /*
   * The width of the caps (end pieces) of the large tabs of a tab
   * control.
   */
  kThemeMetricLargeTabCapsWidth = 11,

  /*
   * The amount to add to the tab height (kThemeMetricLargeTabHeight)
   * to find out the rectangle height to use with the various Tab
   * drawing primitives. This amount is also the amount that each tab
   * overlaps the tab pane.
   */
  kThemeMetricTabFrameOverlap   = 12,

  /*
   * If less than zero, this indicates that the text should be centered
   * on each tab. If greater than zero, the text should be justified
   * (according to the system script direction) and the amount is the
   * offset from the appropriate edge at which the text should start
   * drawing.
   */
  kThemeMetricTabIndentOrStyle  = 13,

  /*
   * The amount of space that every tab's drawing rectangle overlaps
   * the one on either side of it.
   */
  kThemeMetricTabOverlap        = 14,

  /*
   * The height of the small tab of a tab control.  This includes the
   * pixels that overlap the tab pane and/or tab pane bar.
   */
  kThemeMetricSmallTabHeight    = 15,

  /*
   * The width of the caps (end pieces) of the small tabs of a tab
   * control.
   */
  kThemeMetricSmallTabCapsWidth = 16,

  /*
   * The height and the width of the push button control.
   */
  kThemeMetricPushButtonHeight  = 19,

  /*
   * The height of the list header field of the data browser control.
   */
  kThemeMetricListHeaderHeight  = 20,

  /*
   * The height of a disclosure triangle control.  This triangle is the
   * not the center of the disclosure button, but its own control.
   */
  kThemeMetricDisclosureTriangleHeight = 25,

  /*
   * The width of a disclosure triangle control.
   */
  kThemeMetricDisclosureTriangleWidth = 26,

  /*
   * The height of a little arrows control.
   */
  kThemeMetricLittleArrowsHeight = 27,

  /*
   * The width of a little arrows control.
   */
  kThemeMetricLittleArrowsWidth = 28,

  /*
   * The height of a popup button control.
   */
  kThemeMetricPopupButtonHeight = 30,

  /*
   * The height of a small popup button control.
   */
  kThemeMetricSmallPopupButtonHeight = 31,

  /*
   * The height of the large progress bar, not including its shadow.
   */
  kThemeMetricLargeProgressBarThickness = 32,

  /*
   * This metric is not used.
   */
  kThemeMetricPullDownHeight    = 33,

  /*
   * This metric is not used.
   */
  kThemeMetricSmallPullDownHeight = 34,

  /*
   * The height of the window grow box control.
   */
  kThemeMetricResizeControlHeight = 38,

  /*
   * The width of the window grow box control.
   */
  kThemeMetricSmallResizeControlHeight = 39,

  /*
   * The height of the horizontal slider control.
   */
  kThemeMetricHSliderHeight     = 41,

  /*
   * The height of the tick marks for a horizontal slider control.
   */
  kThemeMetricHSliderTickHeight = 42,

  /*
   * The width of the vertical slider control.
   */
  kThemeMetricVSliderWidth      = 45,

  /*
   * The width of the tick marks for a vertical slider control.
   */
  kThemeMetricVSliderTickWidth  = 46,

  /*
   * The height of the title bar widgets (grow, close, and zoom boxes)
   * for a document window.
   */
  kThemeMetricTitleBarControlsHeight = 49,

  /*
   * The width of the non-label part of a check box control.
   */
  kThemeMetricCheckBoxWidth     = 50,

  /*
   * The width of the non-label part of a radio button control.
   */
  kThemeMetricRadioButtonWidth  = 52,

  /*
   * The height of the normal bar, not including its shadow.
   */
  kThemeMetricNormalProgressBarThickness = 58,

  /*
   * The number of pixels of shadow depth drawn below the progress bar.
   */
  kThemeMetricProgressBarShadowOutset = 59,

  /*
   * The number of pixels of shadow depth drawn below the small
   * progress bar.
   */
  kThemeMetricSmallProgressBarShadowOutset = 60,

  /*
   * The number of pixels that the content of a primary group box is
   * from the bounds of the control.
   */
  kThemeMetricPrimaryGroupBoxContentInset = 61,

  /*
   * The number of pixels that the content of a secondary group box is
   * from the bounds of the control.
   */
  kThemeMetricSecondaryGroupBoxContentInset = 62,

  /*
   * Width allocated to draw the mark character in a menu.
   */
  kThemeMetricMenuMarkColumnWidth = 63,

  /*
   * Width allocated for the mark character in a menu item when the
   * menu has kMenuAttrExcludesMarkColumn.
   */
  kThemeMetricMenuExcludedMarkColumnWidth = 64,

  /*
   * Indent into the interior of the mark column at which the mark
   * character is drawn.
   */
  kThemeMetricMenuMarkIndent    = 65,

  /*
   * Whitespace at the leading edge of menu item text.
   */
  kThemeMetricMenuTextLeadingEdgeMargin = 66,

  /*
   * Whitespace at the trailing edge of menu item text.
   */
  kThemeMetricMenuTextTrailingEdgeMargin = 67,

  /*
   * Width per indent level (set by SetMenuItemIndent) of a menu item.
   */
  kThemeMetricMenuIndentWidth   = 68,

  /*
   * Whitespace at the trailing edge of a menu icon (if the item also
   * has text).
   */
  kThemeMetricMenuIconTrailingEdgeMargin = 69
};


/*
 *  Discussion:
 *    The following metrics are only available in OS X.
 */
enum {

  /*
   * The height of a disclosure button.
   */
  kThemeMetricDisclosureButtonHeight = 17,

  /*
   * The height and the width of the round button control.
   */
  kThemeMetricRoundButtonSize   = 18,

  /*
   * The height of the non-label part of a small check box control.
   */
  kThemeMetricSmallCheckBoxHeight = 21,

  /*
   * The width of a disclosure button.
   */
  kThemeMetricDisclosureButtonWidth = 22,

  /*
   * The height of a small disclosure button.
   */
  kThemeMetricSmallDisclosureButtonHeight = 23,

  /*
   * The width of a small disclosure button.
   */
  kThemeMetricSmallDisclosureButtonWidth = 24,

  /*
   * The height (or width if vertical) of a pane splitter.
   */
  kThemeMetricPaneSplitterHeight = 29,

  /*
   * The height of the small push button control.
   */
  kThemeMetricSmallPushButtonHeight = 35,

  /*
   * The height of the non-label part of a small radio button control.
   */
  kThemeMetricSmallRadioButtonHeight = 36,

  /*
   * The height of the relevance indicator control.
   */
  kThemeMetricRelevanceIndicatorHeight = 37,

  /*
   * The height and the width of the large round button control.
   */
  kThemeMetricLargeRoundButtonSize = 40,

  /*
   * The height of the small, horizontal slider control.
   */
  kThemeMetricSmallHSliderHeight = 43,

  /*
   * The height of the tick marks for a small, horizontal slider
   * control.
   */
  kThemeMetricSmallHSliderTickHeight = 44,

  /*
   * The width of the small, vertical slider control.
   */
  kThemeMetricSmallVSliderWidth = 47,

  /*
   * The width of the tick marks for a small, vertical slider control.
   */
  kThemeMetricSmallVSliderTickWidth = 48,

  /*
   * The width of the non-label part of a small check box control.
   */
  kThemeMetricSmallCheckBoxWidth = 51,

  /*
   * The width of the non-label part of a small radio button control.
   */
  kThemeMetricSmallRadioButtonWidth = 53,

  /*
   * The minimum width of the thumb of a small, horizontal slider
   * control.
   */
  kThemeMetricSmallHSliderMinThumbWidth = 54,

  /*
   * The minimum width of the thumb of a small, vertical slider control.
   */
  kThemeMetricSmallVSliderMinThumbHeight = 55,

  /*
   * The offset of the tick marks from the appropriate side of a small
   * horizontal slider control.
   */
  kThemeMetricSmallHSliderTickOffset = 56,

  /*
   * The offset of the tick marks from the appropriate side of a small
   * vertical slider control.
   */
  kThemeMetricSmallVSliderTickOffset = 57
};

typedef UInt32                          ThemeMetric;
/*------------------------------------------------------------------------------------------*/
/* Drawing State                                                                            */
/*------------------------------------------------------------------------------------------*/
typedef struct OpaqueThemeDrawingState*  ThemeDrawingState;
/*------------------------------------------------------------------------------------------*/
/* Callback procs                                                                           */
/*------------------------------------------------------------------------------------------*/
typedef CALLBACK_API( void , ThemeTabTitleDrawProcPtr )(const Rect *bounds, ThemeTabStyle style, ThemeTabDirection direction, SInt16 depth, Boolean isColorDev, UInt32 userData);
typedef CALLBACK_API( void , ThemeEraseProcPtr )(const Rect *bounds, UInt32 eraseData, SInt16 depth, Boolean isColorDev);
typedef CALLBACK_API( void , ThemeButtonDrawProcPtr )(const Rect *bounds, ThemeButtonKind kind, const ThemeButtonDrawInfo *info, UInt32 userData, SInt16 depth, Boolean isColorDev);
typedef CALLBACK_API( void , WindowTitleDrawingProcPtr )(const Rect *bounds, SInt16 depth, Boolean colorDevice, UInt32 userData);
typedef CALLBACK_API( Boolean , ThemeIteratorProcPtr )(ConstStr255Param inFileName, SInt16 resID, Collection inThemeSettings, void *inUserData);
typedef STACK_UPP_TYPE(ThemeTabTitleDrawProcPtr)                ThemeTabTitleDrawUPP;
typedef STACK_UPP_TYPE(ThemeEraseProcPtr)                       ThemeEraseUPP;
typedef STACK_UPP_TYPE(ThemeButtonDrawProcPtr)                  ThemeButtonDrawUPP;
typedef STACK_UPP_TYPE(WindowTitleDrawingProcPtr)               WindowTitleDrawingUPP;
typedef STACK_UPP_TYPE(ThemeIteratorProcPtr)                    ThemeIteratorUPP;
/*
 *  NewThemeTabTitleDrawUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ThemeTabTitleDrawUPP )
NewThemeTabTitleDrawUPP(ThemeTabTitleDrawProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppThemeTabTitleDrawProcInfo = 0x00036AC0 };  /* pascal no_return_value Func(4_bytes, 2_bytes, 2_bytes, 2_bytes, 1_byte, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ThemeTabTitleDrawUPP) NewThemeTabTitleDrawUPP(ThemeTabTitleDrawProcPtr userRoutine) { return (ThemeTabTitleDrawUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppThemeTabTitleDrawProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewThemeTabTitleDrawUPP(userRoutine) (ThemeTabTitleDrawUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppThemeTabTitleDrawProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewThemeEraseUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ThemeEraseUPP )
NewThemeEraseUPP(ThemeEraseProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppThemeEraseProcInfo = 0x00001BC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 2_bytes, 1_byte) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ThemeEraseUPP) NewThemeEraseUPP(ThemeEraseProcPtr userRoutine) { return (ThemeEraseUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppThemeEraseProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewThemeEraseUPP(userRoutine) (ThemeEraseUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppThemeEraseProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewThemeButtonDrawUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ThemeButtonDrawUPP )
NewThemeButtonDrawUPP(ThemeButtonDrawProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppThemeButtonDrawProcInfo = 0x0001BEC0 };  /* pascal no_return_value Func(4_bytes, 2_bytes, 4_bytes, 4_bytes, 2_bytes, 1_byte) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ThemeButtonDrawUPP) NewThemeButtonDrawUPP(ThemeButtonDrawProcPtr userRoutine) { return (ThemeButtonDrawUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppThemeButtonDrawProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewThemeButtonDrawUPP(userRoutine) (ThemeButtonDrawUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppThemeButtonDrawProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewWindowTitleDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( WindowTitleDrawingUPP )
NewWindowTitleDrawingUPP(WindowTitleDrawingProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppWindowTitleDrawingProcInfo = 0x000036C0 };  /* pascal no_return_value Func(4_bytes, 2_bytes, 1_byte, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(WindowTitleDrawingUPP) NewWindowTitleDrawingUPP(WindowTitleDrawingProcPtr userRoutine) { return (WindowTitleDrawingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppWindowTitleDrawingProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewWindowTitleDrawingUPP(userRoutine) (WindowTitleDrawingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppWindowTitleDrawingProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewThemeIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ThemeIteratorUPP )
NewThemeIteratorUPP(ThemeIteratorProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppThemeIteratorProcInfo = 0x00003ED0 };  /* pascal 1_byte Func(4_bytes, 2_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(ThemeIteratorUPP) NewThemeIteratorUPP(ThemeIteratorProcPtr userRoutine) { return (ThemeIteratorUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppThemeIteratorProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewThemeIteratorUPP(userRoutine) (ThemeIteratorUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppThemeIteratorProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeThemeTabTitleDrawUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeThemeTabTitleDrawUPP(ThemeTabTitleDrawUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeThemeTabTitleDrawUPP(ThemeTabTitleDrawUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeThemeTabTitleDrawUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeThemeEraseUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeThemeEraseUPP(ThemeEraseUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeThemeEraseUPP(ThemeEraseUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeThemeEraseUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeThemeButtonDrawUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeThemeButtonDrawUPP(ThemeButtonDrawUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeThemeButtonDrawUPP(ThemeButtonDrawUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeThemeButtonDrawUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeWindowTitleDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeWindowTitleDrawingUPP(WindowTitleDrawingUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeWindowTitleDrawingUPP(WindowTitleDrawingUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeWindowTitleDrawingUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeThemeIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeThemeIteratorUPP(ThemeIteratorUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeThemeIteratorUPP(ThemeIteratorUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeThemeIteratorUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeThemeTabTitleDrawUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeThemeTabTitleDrawUPP(
  const Rect *          bounds,
  ThemeTabStyle         style,
  ThemeTabDirection     direction,
  SInt16                depth,
  Boolean               isColorDev,
  UInt32                userData,
  ThemeTabTitleDrawUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeThemeTabTitleDrawUPP(const Rect * bounds, ThemeTabStyle style, ThemeTabDirection direction, SInt16 depth, Boolean isColorDev, UInt32 userData, ThemeTabTitleDrawUPP userUPP) { CALL_SIX_PARAMETER_UPP(userUPP, uppThemeTabTitleDrawProcInfo, bounds, style, direction, depth, isColorDev, userData); }
  #else
    #define InvokeThemeTabTitleDrawUPP(bounds, style, direction, depth, isColorDev, userData, userUPP) CALL_SIX_PARAMETER_UPP((userUPP), uppThemeTabTitleDrawProcInfo, (bounds), (style), (direction), (depth), (isColorDev), (userData))
  #endif
#endif

/*
 *  InvokeThemeEraseUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeThemeEraseUPP(
  const Rect *   bounds,
  UInt32         eraseData,
  SInt16         depth,
  Boolean        isColorDev,
  ThemeEraseUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeThemeEraseUPP(const Rect * bounds, UInt32 eraseData, SInt16 depth, Boolean isColorDev, ThemeEraseUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppThemeEraseProcInfo, bounds, eraseData, depth, isColorDev); }
  #else
    #define InvokeThemeEraseUPP(bounds, eraseData, depth, isColorDev, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppThemeEraseProcInfo, (bounds), (eraseData), (depth), (isColorDev))
  #endif
#endif

/*
 *  InvokeThemeButtonDrawUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeThemeButtonDrawUPP(
  const Rect *                 bounds,
  ThemeButtonKind              kind,
  const ThemeButtonDrawInfo *  info,
  UInt32                       userData,
  SInt16                       depth,
  Boolean                      isColorDev,
  ThemeButtonDrawUPP           userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeThemeButtonDrawUPP(const Rect * bounds, ThemeButtonKind kind, const ThemeButtonDrawInfo * info, UInt32 userData, SInt16 depth, Boolean isColorDev, ThemeButtonDrawUPP userUPP) { CALL_SIX_PARAMETER_UPP(userUPP, uppThemeButtonDrawProcInfo, bounds, kind, info, userData, depth, isColorDev); }
  #else
    #define InvokeThemeButtonDrawUPP(bounds, kind, info, userData, depth, isColorDev, userUPP) CALL_SIX_PARAMETER_UPP((userUPP), uppThemeButtonDrawProcInfo, (bounds), (kind), (info), (userData), (depth), (isColorDev))
  #endif
#endif

/*
 *  InvokeWindowTitleDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeWindowTitleDrawingUPP(
  const Rect *           bounds,
  SInt16                 depth,
  Boolean                colorDevice,
  UInt32                 userData,
  WindowTitleDrawingUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeWindowTitleDrawingUPP(const Rect * bounds, SInt16 depth, Boolean colorDevice, UInt32 userData, WindowTitleDrawingUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppWindowTitleDrawingProcInfo, bounds, depth, colorDevice, userData); }
  #else
    #define InvokeWindowTitleDrawingUPP(bounds, depth, colorDevice, userData, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppWindowTitleDrawingProcInfo, (bounds), (depth), (colorDevice), (userData))
  #endif
#endif

/*
 *  InvokeThemeIteratorUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
InvokeThemeIteratorUPP(
  ConstStr255Param  inFileName,
  SInt16            resID,
  Collection        inThemeSettings,
  void *            inUserData,
  ThemeIteratorUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(Boolean) InvokeThemeIteratorUPP(ConstStr255Param inFileName, SInt16 resID, Collection inThemeSettings, void * inUserData, ThemeIteratorUPP userUPP) { return (Boolean)CALL_FOUR_PARAMETER_UPP(userUPP, uppThemeIteratorProcInfo, inFileName, resID, inThemeSettings, inUserData); }
  #else
    #define InvokeThemeIteratorUPP(inFileName, resID, inThemeSettings, inUserData, userUPP) (Boolean)CALL_FOUR_PARAMETER_UPP((userUPP), uppThemeIteratorProcInfo, (inFileName), (resID), (inThemeSettings), (inUserData))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewThemeTabTitleDrawProc(userRoutine)               NewThemeTabTitleDrawUPP(userRoutine)
    #define NewThemeEraseProc(userRoutine)                      NewThemeEraseUPP(userRoutine)
    #define NewThemeButtonDrawProc(userRoutine)                 NewThemeButtonDrawUPP(userRoutine)
    #define NewWindowTitleDrawingProc(userRoutine)              NewWindowTitleDrawingUPP(userRoutine)
    #define NewThemeIteratorProc(userRoutine)                   NewThemeIteratorUPP(userRoutine)
    #define CallThemeTabTitleDrawProc(userRoutine, bounds, style, direction, depth, isColorDev, userData) InvokeThemeTabTitleDrawUPP(bounds, style, direction, depth, isColorDev, userData, userRoutine)
    #define CallThemeEraseProc(userRoutine, bounds, eraseData, depth, isColorDev) InvokeThemeEraseUPP(bounds, eraseData, depth, isColorDev, userRoutine)
    #define CallThemeButtonDrawProc(userRoutine, bounds, kind, info, userData, depth, isColorDev) InvokeThemeButtonDrawUPP(bounds, kind, info, userData, depth, isColorDev, userRoutine)
    #define CallWindowTitleDrawingProc(userRoutine, bounds, depth, colorDevice, userData) InvokeWindowTitleDrawingUPP(bounds, depth, colorDevice, userData, userRoutine)
    #define CallThemeIteratorProc(userRoutine, inFileName, resID, inThemeSettings, inUserData) InvokeThemeIteratorUPP(inFileName, resID, inThemeSettings, inUserData, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*----------------------------------------------------------------------------------*/
/* Menu Drawing callbacks                                                           */
/*----------------------------------------------------------------------------------*/
typedef CALLBACK_API( void , MenuTitleDrawingProcPtr )(const Rect *inBounds, SInt16 inDepth, Boolean inIsColorDevice, SInt32 inUserData);
typedef CALLBACK_API( void , MenuItemDrawingProcPtr )(const Rect *inBounds, SInt16 inDepth, Boolean inIsColorDevice, SInt32 inUserData);
typedef STACK_UPP_TYPE(MenuTitleDrawingProcPtr)                 MenuTitleDrawingUPP;
typedef STACK_UPP_TYPE(MenuItemDrawingProcPtr)                  MenuItemDrawingUPP;
/*
 *  NewMenuTitleDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( MenuTitleDrawingUPP )
NewMenuTitleDrawingUPP(MenuTitleDrawingProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppMenuTitleDrawingProcInfo = 0x000036C0 };  /* pascal no_return_value Func(4_bytes, 2_bytes, 1_byte, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(MenuTitleDrawingUPP) NewMenuTitleDrawingUPP(MenuTitleDrawingProcPtr userRoutine) { return (MenuTitleDrawingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppMenuTitleDrawingProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewMenuTitleDrawingUPP(userRoutine) (MenuTitleDrawingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppMenuTitleDrawingProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewMenuItemDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( MenuItemDrawingUPP )
NewMenuItemDrawingUPP(MenuItemDrawingProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppMenuItemDrawingProcInfo = 0x000036C0 };  /* pascal no_return_value Func(4_bytes, 2_bytes, 1_byte, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(MenuItemDrawingUPP) NewMenuItemDrawingUPP(MenuItemDrawingProcPtr userRoutine) { return (MenuItemDrawingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppMenuItemDrawingProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewMenuItemDrawingUPP(userRoutine) (MenuItemDrawingUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppMenuItemDrawingProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeMenuTitleDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeMenuTitleDrawingUPP(MenuTitleDrawingUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeMenuTitleDrawingUPP(MenuTitleDrawingUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeMenuTitleDrawingUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeMenuItemDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeMenuItemDrawingUPP(MenuItemDrawingUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeMenuItemDrawingUPP(MenuItemDrawingUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeMenuItemDrawingUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeMenuTitleDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeMenuTitleDrawingUPP(
  const Rect *         inBounds,
  SInt16               inDepth,
  Boolean              inIsColorDevice,
  SInt32               inUserData,
  MenuTitleDrawingUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeMenuTitleDrawingUPP(const Rect * inBounds, SInt16 inDepth, Boolean inIsColorDevice, SInt32 inUserData, MenuTitleDrawingUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppMenuTitleDrawingProcInfo, inBounds, inDepth, inIsColorDevice, inUserData); }
  #else
    #define InvokeMenuTitleDrawingUPP(inBounds, inDepth, inIsColorDevice, inUserData, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppMenuTitleDrawingProcInfo, (inBounds), (inDepth), (inIsColorDevice), (inUserData))
  #endif
#endif

/*
 *  InvokeMenuItemDrawingUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeMenuItemDrawingUPP(
  const Rect *        inBounds,
  SInt16              inDepth,
  Boolean             inIsColorDevice,
  SInt32              inUserData,
  MenuItemDrawingUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeMenuItemDrawingUPP(const Rect * inBounds, SInt16 inDepth, Boolean inIsColorDevice, SInt32 inUserData, MenuItemDrawingUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppMenuItemDrawingProcInfo, inBounds, inDepth, inIsColorDevice, inUserData); }
  #else
    #define InvokeMenuItemDrawingUPP(inBounds, inDepth, inIsColorDevice, inUserData, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppMenuItemDrawingProcInfo, (inBounds), (inDepth), (inIsColorDevice), (inUserData))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewMenuTitleDrawingProc(userRoutine)                NewMenuTitleDrawingUPP(userRoutine)
    #define NewMenuItemDrawingProc(userRoutine)                 NewMenuItemDrawingUPP(userRoutine)
    #define CallMenuTitleDrawingProc(userRoutine, inBounds, inDepth, inIsColorDevice, inUserData) InvokeMenuTitleDrawingUPP(inBounds, inDepth, inIsColorDevice, inUserData, userRoutine)
    #define CallMenuItemDrawingProc(userRoutine, inBounds, inDepth, inIsColorDevice, inUserData) InvokeMenuItemDrawingUPP(inBounds, inDepth, inIsColorDevice, inUserData, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

/*----------------------------------------------------------------------------------*/
/*  Appearance Manager APIs                                                         */
/*----------------------------------------------------------------------------------*/
/* Registering Appearance-Savvy Applications */
/*
 *  RegisterAppearanceClient()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
RegisterAppearanceClient(void)                                THREEWORDINLINE(0x303C, 0x0015, 0xAA74);


/*
 *  UnregisterAppearanceClient()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
UnregisterAppearanceClient(void)                              THREEWORDINLINE(0x303C, 0x0016, 0xAA74);


/*
 *  IsAppearanceClient()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsAppearanceClient(const ProcessSerialNumber * process)       THREEWORDINLINE(0x303C, 0xFFFF, 0xAA74);


/*****************************************************************************
    NOTES ON THEME BRUSHES
    Theme brushes can be either colors or patterns, depending on the theme.
    Because of this, you should be prepared to handle the case where a brush
    is a pattern and save and restore the pnPixPat and bkPixPat fields of
    your GrafPorts when saving the fore and back colors. Also, since patterns
    in bkPixPat override the background color of the window, you should use
    BackPat to set your background pattern to a normal white pattern. This
    will ensure that you can use RGBBackColor to set your back color to white,
    call EraseRect and get the expected results.
*****************************************************************************/

/*
 *  SetThemePen()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetThemePen(
  ThemeBrush   inBrush,
  SInt16       inDepth,
  Boolean      inIsColorDevice)                               THREEWORDINLINE(0x303C, 0x0001, 0xAA74);


/*
 *  SetThemeBackground()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetThemeBackground(
  ThemeBrush   inBrush,
  SInt16       inDepth,
  Boolean      inIsColorDevice)                               THREEWORDINLINE(0x303C, 0x0002, 0xAA74);


/*
 *  SetThemeTextColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetThemeTextColor(
  ThemeTextColor   inColor,
  SInt16           inDepth,
  Boolean          inIsColorDevice)                           THREEWORDINLINE(0x303C, 0x0003, 0xAA74);


/*
 *  SetThemeWindowBackground()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetThemeWindowBackground(
  WindowRef    inWindow,
  ThemeBrush   inBrush,
  Boolean      inUpdate)                                      THREEWORDINLINE(0x303C, 0x0004, 0xAA74);


/* Window Placards, Headers and Frames */
/*
 *  DrawThemeWindowHeader()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeWindowHeader(
  const Rect *     inRect,
  ThemeDrawState   inState)                                   THREEWORDINLINE(0x303C, 0x0005, 0xAA74);


/*
 *  DrawThemeWindowListViewHeader()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeWindowListViewHeader(
  const Rect *     inRect,
  ThemeDrawState   inState)                                   THREEWORDINLINE(0x303C, 0x0006, 0xAA74);


/*
 *  DrawThemePlacard()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemePlacard(
  const Rect *     inRect,
  ThemeDrawState   inState)                                   THREEWORDINLINE(0x303C, 0x0007, 0xAA74);


/*
 *  DrawThemeEditTextFrame()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeEditTextFrame(
  const Rect *     inRect,
  ThemeDrawState   inState)                                   THREEWORDINLINE(0x303C, 0x0009, 0xAA74);


/*
 *  DrawThemeListBoxFrame()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeListBoxFrame(
  const Rect *     inRect,
  ThemeDrawState   inState)                                   THREEWORDINLINE(0x303C, 0x000A, 0xAA74);


/* Keyboard Focus Drawing */
/*
 *  DrawThemeFocusRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeFocusRect(
  const Rect *  inRect,
  Boolean       inHasFocus)                                   THREEWORDINLINE(0x303C, 0x000B, 0xAA74);


/* Dialog Group Boxes and Separators */
/*
 *  DrawThemePrimaryGroup()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemePrimaryGroup(
  const Rect *     inRect,
  ThemeDrawState   inState)                                   THREEWORDINLINE(0x303C, 0x000C, 0xAA74);


/*
 *  DrawThemeSecondaryGroup()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeSecondaryGroup(
  const Rect *     inRect,
  ThemeDrawState   inState)                                   THREEWORDINLINE(0x303C, 0x000D, 0xAA74);


/*
 *  DrawThemeSeparator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeSeparator(
  const Rect *     inRect,
  ThemeDrawState   inState)                                   THREEWORDINLINE(0x303C, 0x000E, 0xAA74);


/*------------------------------ BEGIN APPEARANCE 1.0.1 --------------------------------------------*/
/* The following Appearance Manager APIs are only available */
/* in Appearance 1.0.1 or later                             */
/*
 *  DrawThemeModelessDialogFrame()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeModelessDialogFrame(
  const Rect *     inRect,
  ThemeDrawState   inState)                                   THREEWORDINLINE(0x303C, 0x0008, 0xAA74);


/*
 *  DrawThemeGenericWell()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeGenericWell(
  const Rect *     inRect,
  ThemeDrawState   inState,
  Boolean          inFillCenter)                              THREEWORDINLINE(0x303C, 0x0022, 0xAA74);


/*
 *  DrawThemeFocusRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeFocusRegion(
  RgnHandle   inRegion,
  Boolean     inHasFocus)                                     THREEWORDINLINE(0x303C, 0x0023, 0xAA74);


/*
 *  IsThemeInColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsThemeInColor(
  SInt16    inDepth,
  Boolean   inIsColorDevice)                                  THREEWORDINLINE(0x303C, 0x0024, 0xAA74);


/* IMPORTANT: GetThemeAccentColors will only work in the platinum theme. Any other theme will */
/* most likely return an error */
/*
 *  GetThemeAccentColors()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeAccentColors(CTabHandle * outColors)                  THREEWORDINLINE(0x303C, 0x0025, 0xAA74);


/*
 *  DrawThemeMenuBarBackground()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeMenuBarBackground(
  const Rect *        inBounds,
  ThemeMenuBarState   inState,
  UInt32              inAttributes)                           THREEWORDINLINE(0x303C, 0x0018, 0xAA74);


/*
 *  DrawThemeMenuTitle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeMenuTitle(
  const Rect *          inMenuBarRect,
  const Rect *          inTitleRect,
  ThemeMenuState        inState,
  UInt32                inAttributes,
  MenuTitleDrawingUPP   inTitleProc,         /* can be NULL */
  UInt32                inTitleData)                          THREEWORDINLINE(0x303C, 0x0019, 0xAA74);



/*
 *  GetThemeMenuBarHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeMenuBarHeight(SInt16 * outHeight)                     THREEWORDINLINE(0x303C, 0x001A, 0xAA74);


/*
 *  DrawThemeMenuBackground()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeMenuBackground(
  const Rect *    inMenuRect,
  ThemeMenuType   inMenuType)                                 THREEWORDINLINE(0x303C, 0x001B, 0xAA74);


/*
 *  GetThemeMenuBackgroundRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeMenuBackgroundRegion(
  const Rect *    inMenuRect,
  ThemeMenuType   menuType,
  RgnHandle       region)                                     THREEWORDINLINE(0x303C, 0x001C, 0xAA74);


/*
 *  DrawThemeMenuItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeMenuItem(
  const Rect *         inMenuRect,
  const Rect *         inItemRect,
  SInt16               inVirtualMenuTop,
  SInt16               inVirtualMenuBottom,
  ThemeMenuState       inState,
  ThemeMenuItemType    inItemType,
  MenuItemDrawingUPP   inDrawProc,                /* can be NULL */
  UInt32               inUserData)                            THREEWORDINLINE(0x303C, 0x001D, 0xAA74);


/*
 *  DrawThemeMenuSeparator()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeMenuSeparator(const Rect * inItemRect)               THREEWORDINLINE(0x303C, 0x001E, 0xAA74);


/*
 *  GetThemeMenuSeparatorHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeMenuSeparatorHeight(SInt16 * outHeight)               THREEWORDINLINE(0x303C, 0x001F, 0xAA74);


/*
 *  GetThemeMenuItemExtra()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeMenuItemExtra(
  ThemeMenuItemType   inItemType,
  SInt16 *            outHeight,
  SInt16 *            outWidth)                               THREEWORDINLINE(0x303C, 0x0020, 0xAA74);


/*
 *  GetThemeMenuTitleExtra()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeMenuTitleExtra(
  SInt16 *  outWidth,
  Boolean   inIsSquished)                                     THREEWORDINLINE(0x303C, 0x0021, 0xAA74);


/*------------------------------- BEGIN APPEARANCE 1.1 ---------------------------------------------*/
/*---------------------------------- THEME SWITCHING -----------------------------------------------*/
/*                                                                                                  */
/*  X ALERT: Please note that Get/SetTheme are severely neutered under Mac OS X at present.         */
/*           See the note above regarding what collection tags are supported under X.               */

/*
 *  GetTheme()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetTheme(Collection ioCollection);


/*
 *  SetTheme()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetTheme(Collection ioCollection);


/*
 *  IterateThemes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
IterateThemes(
  ThemeIteratorUPP   inProc,
  void *             inUserData);      /* can be NULL */


/*---------------------------------------- TABS ----------------------------------------------------*/
/*
 *  DrawThemeTabPane()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeTabPane(
  const Rect *     inRect,
  ThemeDrawState   inState);


/*
 *  DrawThemeTab()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeTab(
  const Rect *           inRect,
  ThemeTabStyle          inStyle,
  ThemeTabDirection      inDirection,
  ThemeTabTitleDrawUPP   labelProc,         /* can be NULL */
  UInt32                 userData);


/*
 *  GetThemeTabRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeTabRegion(
  const Rect *        inRect,
  ThemeTabStyle       inStyle,
  ThemeTabDirection   inDirection,
  RgnHandle           ioRgn);


/*--------------------------------------- CURSORS --------------------------------------------------*/
/*
 *  SetThemeCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetThemeCursor(ThemeCursor inCursor);


/*
 *  SetAnimatedThemeCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetAnimatedThemeCursor(
  ThemeCursor   inCursor,
  UInt32        inAnimationStep);


/*-------------------------------- CONTROL STYLE SETTINGS ------------------------------------------*/
/*
 *  GetThemeScrollBarThumbStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeScrollBarThumbStyle(ThemeScrollBarThumbStyle * outStyle);


/*
 *  GetThemeScrollBarArrowStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeScrollBarArrowStyle(ThemeScrollBarArrowStyle * outStyle);


/*
 *  GetThemeCheckBoxStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeCheckBoxStyle(ThemeCheckBoxStyle * outStyle);


/*---------------------------------------- FONTS/TEXT ----------------------------------------------*/
/*
 *  UseThemeFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
UseThemeFont(
  ThemeFontID   inFontID,
  ScriptCode    inScript);


/*
 *  GetThemeFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeFont(
  ThemeFontID   inFontID,
  ScriptCode    inScript,
  Str255        outFontName,       /* can be NULL */
  SInt16 *      outFontSize,
  Style *       outStyle);


/*
 *  DrawThemeTextBox()
 *  
 *  Summary:
 *    Draws text into the area you specify.
 *  
 *  Discussion:
 *    DrawThemeTextBox allows you to draw theme-savvy (ie. Aqua-savvy
 *    on Mac OS X) text. It is unicode savvy (although only partially
 *    so under CarbonLib), and allows you to customize certain text
 *    rendering characteristics such as the font, wrapping behavior,
 *    and justification. The text is drawn into the CGContextRef you
 *    provide, or into the current Quickdraw port if no CGContextRef is
 *    provided. None of DrawThemeTextBox's parameters imply a color, so
 *    you must set up the desired text color separately before calling
 *    DrawThemeTextBox. If you provide a CGContextRef, its fill color
 *    will be used to draw the text. If you do not provide a
 *    CGContextRef, a color based on the current Quickdraw port's
 *    foreground color and the grayishTextOr mode (if set) will be used
 *    to draw the text.
 *  
 *  Parameters:
 *    
 *    inString:
 *      A CFStringRef containing the unicode characters you wish to
 *      render. You MUST NOT pass in a CFStringRef that was allocated
 *      with any of the "NoCopy" CFString creation APIs; a string
 *      created with a "NoCopy" API has transient storage which is
 *      incompatible with DrawThemeTextBox's caches.
 *    
 *    inFontID:
 *      The ThemeFontID describing the font you'd like to render the
 *      text with. See the discussion of ThemeFontIDs elsewhere in this
 *      header.
 *    
 *    inState:
 *      The ThemeDrawState describing the the state of the interface
 *      element you are drawing the text for. If, for example, you are
 *      drawing text for an inactive window, you would pass
 *      kThemeStateInactive. The ThemeDrawState is generally only used
 *      to determine the shadow characteristics for the text on Mac OS
 *      X. Note that the ThemeDrawState does NOT imply a color. It is
 *      NOT used as a mechanism for graying the text. If you wish to
 *      draw grayed text, you must set up the desired gray color and
 *      apply it to either the current Quickdraw port or the
 *      CGContextRef as appropriate.
 *    
 *    inWrapToWidth:
 *      A Boolean indicating whether you want to draw multiple lines of
 *      text wrapped to a bounding box. False indicates that only one
 *      line of text should be drawn without any sort of wrapping.
 *    
 *    inBoundingBox:
 *      The rectangle (in coordinates relative to the current Quickdraw
 *      port) describing the area to draw the text within. The first
 *      line of text will be top-justified to this rectangle. Wrapping
 *      (if desired) will happen at the horizontal extent of this
 *      rectangle. Regardless of the amount of text in your
 *      CFStringRef, all drawn text will be clipped to this rectangle.
 *    
 *    inJust:
 *      The horizontal justification you would like for your text. You
 *      can use one of the standard justification constants from
 *      TextEdit.h.
 *    
 *    inContext:
 *      The CGContextRef into which you would like to draw the text. On
 *      Mac OS X, all text drawing happens in CGContextRefs; if you
 *      pass NULL, a transient CGContextRef will be allocated and
 *      deallocated for use within the single API call. Relying on the
 *      system behavior if transiently createing CGContextRefs may
 *      result in performance problems. On Mac OS 9, the CGContextRef
 *      parameter is ignored.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeTextBox(
  CFStringRef      inString,
  ThemeFontID      inFontID,
  ThemeDrawState   inState,
  Boolean          inWrapToWidth,
  const Rect *     inBoundingBox,
  SInt16           inJust,
  void *           inContext);


/*
 *  TruncateThemeText()
 *  
 *  Summary:
 *    Truncates text to fit within the width you specify.
 *  
 *  Discussion:
 *    TruncateThemeText alters a unicode string to fit within a width
 *    that you specify. It is unicode savvy (although only partially so
 *    under CarbonLib), and makes its calculations (and any subsequent
 *    string alteration) based on the font and state you specify. If
 *    the string needs to be truncated, it will be reduced to the
 *    maximum number of characters which (with the addition of an
 *    ellipsis character) fits within the specified width.
 *  
 *  Parameters:
 *    
 *    inString:
 *      A CFMutableStringRef containing the unicode characters you wish
 *      to truncate. On output, inString may have been altered to fit
 *      within the specified width. You MUST NOT pass in a CFStringRef
 *      that was allocated with any of the "NoCopy" CFString creation
 *      APIs (see note in DrawThemeTextBox above).
 *    
 *    inFontID:
 *      The ThemeFontID to use for text measurements. See the
 *      discussion of ThemeFontIDs elsewhere in this header.
 *    
 *    inState:
 *      The ThemeDrawState which matches the state you will ultimately
 *      render the string width. This may affect text measurement
 *      during truncation, so you should be sure the value you pass to
 *      TruncateThemeText matches the value you will eventually use for
 *      drawing.
 *    
 *    inPixelWidthLimit:
 *      The maximum width (in pixels) that the resulting truncated
 *      string may have.
 *    
 *    inTruncWhere:
 *      A TruncCode indicating where you would like truncation to occur.
 *    
 *    outTruncated:
 *      On output, this Boolean value indicates whether the string was
 *      truncated. True means the string was truncated. False means the
 *      string was not (and did not need to be) truncated.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
TruncateThemeText(
  CFMutableStringRef   inString,
  ThemeFontID          inFontID,
  ThemeDrawState       inState,
  SInt16               inPixelWidthLimit,
  TruncCode            inTruncWhere,
  Boolean *            outTruncated);           /* can be NULL */


/*
 *  GetThemeTextDimensions()
 *  
 *  Summary:
 *    Tells you the height, width, and baseline for a string.
 *  
 *  Discussion:
 *    GetThemeTextDimensions measures the given string using a font and
 *    state you specify. It always reports the actual height and
 *    baseline. It sometimes reports the actual width (see below). It
 *    can measure a string that wraps. It is unicode savvy (although
 *    only partially so under CarbonLib).
 *  
 *  Parameters:
 *    
 *    inString:
 *      A CFStringRef containing the unicode characters you wish to
 *      measure. You MUST NOT pass in a CFStringRef that was allocated
 *      with any of the "NoCopy" CFString creation APIs (see note in
 *      DrawThemeTextBox above).
 *    
 *    inFontID:
 *      The ThemeFontID describing the font you'd like to measure the
 *      text with. See the discussion of ThemeFontIDs elsewhere in this
 *      header.
 *    
 *    inState:
 *      The ThemeDrawState which matches the state you will ultimately
 *      render the string width. This may affect text measurement, so
 *      you should be sure the value you pass to TruncateThemeText
 *      matches the value you will eventually use for drawing.
 *    
 *    inWrapToWidth:
 *      A Boolean indicating whether you want the measurements based on
 *      wrapping the text to a specific width. If you pass true, you
 *      must specify the desired width in ioBounds->h.
 *    
 *    ioBounds:
 *      On output, ioBounds->v contains the height of the text. If you
 *      pass false to inWrapToWidth, ioBounds->h will contain the width
 *      of the text on output. If you pass true to inWrapToWidth,
 *      ioBounds->h must (on input) contain the desired width for
 *      wrapping; on output, ioBounds->h contains the same value you
 *      specified on input.
 *    
 *    outBaseline:
 *      On output, outBaseline contains the offset (in Quickdraw space)
 *      from the bottom edge of the last line of text to the baseline
 *      of the first line of text. outBaseline will generally be a
 *      negative value.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeTextDimensions(
  CFStringRef      inString,
  ThemeFontID      inFontID,
  ThemeDrawState   inState,
  Boolean          inWrapToWidth,
  Point *          ioBounds,
  SInt16 *         outBaseline);


/*
 *  GetThemeTextShadowOutset()
 *  
 *  Summary:
 *    Tells you the amount of space taken up by the shadow for a given
 *    font/state combination.
 *  
 *  Discussion:
 *    GetThemeTextShadowOutset passes back the maximum amount of space
 *    the shadow will take up for text drawn in the specified font and
 *    state. While GetThemeTextDimensions tells you how much space is
 *    taken up by the character glyphs themselves, it does not
 *    incorporate the font/state shadow into its calculations. If you
 *    need to know how much total space including the shadow will be
 *    taken up, call GetThemeTextDimensions followed by
 *    GetThemeTextShadowOutset.
 *  
 *  Parameters:
 *    
 *    inFontID:
 *      The ThemeFontID describing the font you'd like the shadow
 *      characteristics of. Font and state both determine the amount of
 *      shadow that will be used on rendered text. See the discussion
 *      of ThemeFontIDs elsewhere in this header.
 *    
 *    inState:
 *      The ThemeDrawState which matches the state you'd like the
 *      shadow characteristics of. Font and state both determine the
 *      amount of shadow that will be used on rendered text.
 *    
 *    outOutset:
 *      On output, outOutset contains the amount of space the shadow
 *      will take up beyond each edge of the text bounding rectangle
 *      returned by GetThemeTextDimensions. The fields of outOutset
 *      will either be positive values or zero.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeTextShadowOutset(
  ThemeFontID      inFontID,
  ThemeDrawState   inState,
  Rect *           outOutset);


/*---------------------------------------- TRACKS --------------------------------------------------*/
/*
 *  DrawThemeTrack()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeTrack(
  const ThemeTrackDrawInfo *  drawInfo,
  RgnHandle                   rgnGhost,        /* can be NULL */
  ThemeEraseUPP               eraseProc,       /* can be NULL */
  UInt32                      eraseData);


/*
 *  HitTestThemeTrack()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
HitTestThemeTrack(
  const ThemeTrackDrawInfo *  drawInfo,
  Point                       mousePoint,
  ControlPartCode *           partHit);


/*
 *  GetThemeTrackBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeTrackBounds(
  const ThemeTrackDrawInfo *  drawInfo,
  Rect *                      bounds);


/*
 *  GetThemeTrackThumbRgn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeTrackThumbRgn(
  const ThemeTrackDrawInfo *  drawInfo,
  RgnHandle                   thumbRgn);


/*
 *  GetThemeTrackDragRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeTrackDragRect(
  const ThemeTrackDrawInfo *  drawInfo,
  Rect *                      dragRect);


/*
 *  DrawThemeTrackTickMarks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeTrackTickMarks(
  const ThemeTrackDrawInfo *  drawInfo,
  ItemCount                   numTicks,
  ThemeEraseUPP               eraseProc,       /* can be NULL */
  UInt32                      eraseData);


/*
 *  GetThemeTrackThumbPositionFromOffset()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeTrackThumbPositionFromOffset(
  const ThemeTrackDrawInfo *  drawInfo,
  Point                       thumbOffset,
  SInt32 *                    relativePosition);


/*
 *  GetThemeTrackThumbPositionFromRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeTrackThumbPositionFromRegion(
  const ThemeTrackDrawInfo *  drawInfo,
  RgnHandle                   thumbRgn,
  SInt32 *                    relativePosition);


/*
 *  GetThemeTrackLiveValue()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeTrackLiveValue(
  const ThemeTrackDrawInfo *  drawInfo,
  SInt32                      relativePosition,
  SInt32 *                    value);


/*----------------------------------- SCROLLBAR ARROWS ---------------------------------------------*/
/*
 *  DrawThemeScrollBarArrows()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeScrollBarArrows(
  const Rect *            bounds,
  ThemeTrackEnableState   enableState,
  ThemeTrackPressState    pressState,
  Boolean                 isHoriz,
  Rect *                  trackBounds);


/*
 *  GetThemeScrollBarTrackRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeScrollBarTrackRect(
  const Rect *            bounds,
  ThemeTrackEnableState   enableState,
  ThemeTrackPressState    pressState,
  Boolean                 isHoriz,
  Rect *                  trackBounds);


/*
 *  HitTestThemeScrollBarArrows()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
HitTestThemeScrollBarArrows(
  const Rect *            scrollBarBounds,
  ThemeTrackEnableState   enableState,
  ThemeTrackPressState    pressState,
  Boolean                 isHoriz,
  Point                   ptHit,
  Rect *                  trackBounds,
  ControlPartCode *       partcode);


/*---------------------------------------- WINDOWS -------------------------------------------------*/
/*
 *  GetThemeWindowRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeWindowRegion(
  ThemeWindowType             flavor,
  const Rect *                contRect,
  ThemeDrawState              state,
  const ThemeWindowMetrics *  metrics,
  ThemeWindowAttributes       attributes,
  WindowRegionCode            winRegion,
  RgnHandle                   rgn);


/*
 *  DrawThemeWindowFrame()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeWindowFrame(
  ThemeWindowType             flavor,
  const Rect *                contRect,
  ThemeDrawState              state,
  const ThemeWindowMetrics *  metrics,
  ThemeWindowAttributes       attributes,
  WindowTitleDrawingUPP       titleProc,        /* can be NULL */
  UInt32                      titleData);


/*
 *  DrawThemeTitleBarWidget()
 *  
 *  Summary:
 *    Draws the requested theme title bar widget.
 *  
 *  Discussion:
 *    DrawThemeTitleBarWidget renders the requested theme title bar
 *    widget in the proper location of a window.  A common
 *    misconception when using this API is that the client must specify
 *    the exact location of the widget in the window. The widget will
 *    locate itself in the window based relative to the content rect
 *    passed in content rectangle -- the contRect parameter.  Another
 *    common problem is to ignore the window's attributes.  The
 *    attributes must be set up properly to describe the window for
 *    which the widget is to be drawn.
 *  
 *  Parameters:
 *    
 *    flavor:
 *      A valid ThemeWindowtype describing the type of theme window for
 *      which you would like to draw a widget.
 *    
 *    contRect:
 *      A rectangle describing the window's content area.  The widget
 *      is drawn relative to the content rectangle of the window, so
 *      this parameter does not describe the actual widget bounds, it
 *      describes the window's content rectangle.
 *    
 *    state:
 *      A valid ThemeDrawState which describes the state of the window
 *      for which the widget is to be drawn.
 *    
 *    metrics:
 *      A pointer to a set of valid ThemeWindowMetrics.  At this time,
 *      none of the fields of the metrics are pertinent to the widgets,
 *      so the only important field is the metricSize field to mark the
 *      structure as valid.
 *    
 *    attributes:
 *      A valid ThemeWindowAttributes set which describes the window
 *      for which the widget is to be drawn.
 *    
 *    widget:
 *      A valid ThemeTitleBarWidget set which describes which widget to
 *      draw.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeTitleBarWidget(
  ThemeWindowType             flavor,
  const Rect *                contRect,
  ThemeDrawState              state,
  const ThemeWindowMetrics *  metrics,
  ThemeWindowAttributes       attributes,
  ThemeTitleBarWidget         widget);


/*
 *  GetThemeWindowRegionHit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
GetThemeWindowRegionHit(
  ThemeWindowType             flavor,
  const Rect *                inContRect,
  ThemeDrawState              state,
  const ThemeWindowMetrics *  metrics,
  ThemeWindowAttributes       inAttributes,
  Point                       inPoint,
  WindowRegionCode *          outRegionHit);


/*
 *  DrawThemeScrollBarDelimiters()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeScrollBarDelimiters(
  ThemeWindowType         flavor,
  const Rect *            inContRect,
  ThemeDrawState          state,
  ThemeWindowAttributes   attributes);



/*---------------------------------------- BUTTONS -------------------------------------------------*/
/*
 *  DrawThemeButton()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeButton(
  const Rect *                 inBounds,
  ThemeButtonKind              inKind,
  const ThemeButtonDrawInfo *  inNewInfo,
  const ThemeButtonDrawInfo *  inPrevInfo,        /* can be NULL */
  ThemeEraseUPP                inEraseProc,       /* can be NULL */
  ThemeButtonDrawUPP           inLabelProc,       /* can be NULL */
  UInt32                       inUserData);


/*
 *  GetThemeButtonRegion()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeButtonRegion(
  const Rect *                 inBounds,
  ThemeButtonKind              inKind,
  const ThemeButtonDrawInfo *  inNewInfo,
  RgnHandle                    outRegion);


/*
 *  GetThemeButtonContentBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeButtonContentBounds(
  const Rect *                 inBounds,
  ThemeButtonKind              inKind,
  const ThemeButtonDrawInfo *  inDrawInfo,
  Rect *                       outBounds);


/*
 *  GetThemeButtonBackgroundBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeButtonBackgroundBounds(
  const Rect *                 inBounds,
  ThemeButtonKind              inKind,
  const ThemeButtonDrawInfo *  inDrawInfo,
  Rect *                       outBounds);



/*------------------------------------- INTERFACE SOUNDS -------------------------------------------*/
/*                                                                                                  */
/*  X ALERT: Please note that the sound APIs do not work on Mac OS X at present.                    */
/*
 *  PlayThemeSound()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
PlayThemeSound(ThemeSoundKind kind)                           THREEWORDINLINE(0x303C, 0x0026, 0xAA74);


/*
 *  BeginThemeDragSound()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
BeginThemeDragSound(ThemeDragSoundKind kind)                  THREEWORDINLINE(0x303C, 0x0027, 0xAA74);


/*
 *  EndThemeDragSound()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
EndThemeDragSound(void)                                       THREEWORDINLINE(0x303C, 0x0028, 0xAA74);


/*-------------------------------------- PRIMITIVES ------------------------------------------------*/
/*
 *  DrawThemeTickMark()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeTickMark(
  const Rect *     bounds,
  ThemeDrawState   state);


/*
 *  DrawThemeChasingArrows()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeChasingArrows(
  const Rect *     bounds,
  UInt32           index,
  ThemeDrawState   state,
  ThemeEraseUPP    eraseProc,       /* can be NULL */
  UInt32           eraseData);


/*
 *  DrawThemePopupArrow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemePopupArrow(
  const Rect *            bounds,
  ThemeArrowOrientation   orientation,
  ThemePopupArrowSize     size,
  ThemeDrawState          state,
  ThemeEraseUPP           eraseProc,         /* can be NULL */
  UInt32                  eraseData);


/*
 *  DrawThemeStandaloneGrowBox()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeStandaloneGrowBox(
  Point                origin,
  ThemeGrowDirection   growDirection,
  Boolean              isSmall,
  ThemeDrawState       state);


/*
 *  DrawThemeStandaloneNoGrowBox()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DrawThemeStandaloneNoGrowBox(
  Point                origin,
  ThemeGrowDirection   growDirection,
  Boolean              isSmall,
  ThemeDrawState       state);


/*
 *  GetThemeStandaloneGrowBoxBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeStandaloneGrowBoxBounds(
  Point                origin,
  ThemeGrowDirection   growDirection,
  Boolean              isSmall,
  Rect *               bounds);


/*------------------------------------- DRAWING STATE ----------------------------------------------*/
/* The following routines help you save and restore the drawing state in a theme-savvy manner. With */
/* these weapons in your arsenal, there is no grafport you cannot tame. Use ThemeGetDrawingState to */
/* get the current drawing settings for the current port. It will return an opaque object for you   */
/* to pass into SetThemeDrawingState later on. When you are finished with the state, call the       */
/* DisposeThemeDrawingState routine. You can alternatively pass true into the inDisposeNow          */
/* parameter of the SetThemeDrawingState routine.  You can use this routine to copy the drawing     */
/* state from one port to another as well.                                                          */
/*                                                                                                  */
/* As of this writing (Mac OS 9.1 and Mac OS X), Get/SetThemeDrawingState will save and restore     */
/* this data in the port:                                                                           */
/*                                                                                                  */
/*      pen size                                                                                    */
/*      pen location                                                                                */
/*      pen mode                                                                                    */
/*      pen Pattern and PixPat                                                                      */
/*      background Pattern and PixPat                                                               */
/*      RGB foreground and background colors                                                        */
/*      text mode                                                                                   */
/*      pattern origin                                                                              */
/*                                                                                                  */
/* Get/SetThemeDrawingState may save and restore additional port state in the future, but you can   */
/* rely on them to always save at least this port state.                                            */
/*
 *  NormalizeThemeDrawingState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
NormalizeThemeDrawingState(void);


/*
 *  GetThemeDrawingState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeDrawingState(ThemeDrawingState * outState);


/*
 *  SetThemeDrawingState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetThemeDrawingState(
  ThemeDrawingState   inState,
  Boolean             inDisposeNow);


/*
 *  DisposeThemeDrawingState()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DisposeThemeDrawingState(ThemeDrawingState inState);


/*------------------------------------- MISCELLANEOUS ----------------------------------------------*/
/* ApplyThemeBackground is used to set up the background for embedded controls  */
/* It is normally called by controls that are embedders. The standard controls  */
/* call this API to ensure a correct background for the current theme. You pass */
/* in the same rectangle you would if you were calling the drawing primitive.   */
/*
 *  ApplyThemeBackground()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
ApplyThemeBackground(
  ThemeBackgroundKind   inKind,
  const Rect *          bounds,
  ThemeDrawState        inState,
  SInt16                inDepth,
  Boolean               inColorDev);


/*
 *  SetThemeTextColorForWindow()
 *  
 *  Summary:
 *    Sets a text color which contrasts with a theme brush.
 *  
 *  Discussion:
 *    SetThemeTextColorForWindow sets a text color in the specified
 *    window's port which contrasts with the specified brush and also
 *    matches the inActive parameter. Only a subset of the theme
 *    brushes have theme text colors: currently (as of Mac OS 9 and Mac
 *    OS X 10.1), the Alert, Dialog, ModelessDialog, and Notification
 *    brushes have corresponding text colors. For any other brush,
 *    SetThemeTextColorForWindow returns themeNoAppropriateBrushErr and
 *    does not modify the text color.
 *  
 *  Parameters:
 *    
 *    inWindow:
 *      The window whose text color to change.
 *    
 *    inActive:
 *      Whether the text color should indicate an active or inactive
 *      state.
 *    
 *    inDepth:
 *      The bit depth of the window's port.
 *    
 *    inColorDev:
 *      Whether the window's port is color or black&white.
 *  
 *  Result:
 *    An operating system result code, including
 *    themeNoAppropriateBrushErr if the specified theme brush does not
 *    have a corresponding theme text color.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetThemeTextColorForWindow(
  WindowRef   inWindow,
  Boolean     inActive,
  SInt16      inDepth,
  Boolean     inColorDev);


/*
 *  IsValidAppearanceFileType()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsValidAppearanceFileType(OSType fileType);


/*
 *  GetThemeBrushAsColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeBrushAsColor(
  ThemeBrush   inBrush,
  SInt16       inDepth,
  Boolean      inColorDev,
  RGBColor *   outColor);


/*
 *  GetThemeTextColor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeTextColor(
  ThemeTextColor   inColor,
  SInt16           inDepth,
  Boolean          inColorDev,
  RGBColor *       outColor);


/*--------------------------------------- BEGIN CARBON ---------------------------------------------*/
/*
 *  GetThemeMetric()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetThemeMetric(
  ThemeMetric   inMetric,
  SInt32 *      outMetric);


/*
 *  CopyThemeIdentifier()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.4 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
CopyThemeIdentifier(CFStringRef * outIdentifier);


/*--------------------------------------------------------------------------------------------------*/
/* Obsolete symbolic names                                                                          */
/*--------------------------------------------------------------------------------------------------*/
/* Obsolete error codes - use the new ones, s'il vous plait / kudasai */
enum {
  appearanceBadBrushIndexErr    = themeInvalidBrushErr, /* pattern index invalid */
  appearanceProcessRegisteredErr = themeProcessRegisteredErr,
  appearanceProcessNotRegisteredErr = themeProcessNotRegisteredErr,
  appearanceBadTextColorIndexErr = themeBadTextColorErr,
  appearanceThemeHasNoAccents   = themeHasNoAccentsErr,
  appearanceBadCursorIndexErr   = themeBadCursorIndexErr
};

enum {
  kThemeActiveDialogBackgroundBrush = kThemeBrushDialogBackgroundActive,
  kThemeInactiveDialogBackgroundBrush = kThemeBrushDialogBackgroundInactive,
  kThemeActiveAlertBackgroundBrush = kThemeBrushAlertBackgroundActive,
  kThemeInactiveAlertBackgroundBrush = kThemeBrushAlertBackgroundInactive,
  kThemeActiveModelessDialogBackgroundBrush = kThemeBrushModelessDialogBackgroundActive,
  kThemeInactiveModelessDialogBackgroundBrush = kThemeBrushModelessDialogBackgroundInactive,
  kThemeActiveUtilityWindowBackgroundBrush = kThemeBrushUtilityWindowBackgroundActive,
  kThemeInactiveUtilityWindowBackgroundBrush = kThemeBrushUtilityWindowBackgroundInactive,
  kThemeListViewSortColumnBackgroundBrush = kThemeBrushListViewSortColumnBackground,
  kThemeListViewBackgroundBrush = kThemeBrushListViewBackground,
  kThemeIconLabelBackgroundBrush = kThemeBrushIconLabelBackground,
  kThemeListViewSeparatorBrush  = kThemeBrushListViewSeparator,
  kThemeChasingArrowsBrush      = kThemeBrushChasingArrows,
  kThemeDragHiliteBrush         = kThemeBrushDragHilite,
  kThemeDocumentWindowBackgroundBrush = kThemeBrushDocumentWindowBackground,
  kThemeFinderWindowBackgroundBrush = kThemeBrushFinderWindowBackground
};

enum {
  kThemeActiveScrollBarDelimiterBrush = kThemeBrushScrollBarDelimiterActive,
  kThemeInactiveScrollBarDelimiterBrush = kThemeBrushScrollBarDelimiterInactive,
  kThemeFocusHighlightBrush     = kThemeBrushFocusHighlight,
  kThemeActivePopupArrowBrush   = kThemeBrushPopupArrowActive,
  kThemePressedPopupArrowBrush  = kThemeBrushPopupArrowPressed,
  kThemeInactivePopupArrowBrush = kThemeBrushPopupArrowInactive,
  kThemeAppleGuideCoachmarkBrush = kThemeBrushAppleGuideCoachmark
};

enum {
  kThemeActiveDialogTextColor   = kThemeTextColorDialogActive,
  kThemeInactiveDialogTextColor = kThemeTextColorDialogInactive,
  kThemeActiveAlertTextColor    = kThemeTextColorAlertActive,
  kThemeInactiveAlertTextColor  = kThemeTextColorAlertInactive,
  kThemeActiveModelessDialogTextColor = kThemeTextColorModelessDialogActive,
  kThemeInactiveModelessDialogTextColor = kThemeTextColorModelessDialogInactive,
  kThemeActiveWindowHeaderTextColor = kThemeTextColorWindowHeaderActive,
  kThemeInactiveWindowHeaderTextColor = kThemeTextColorWindowHeaderInactive,
  kThemeActivePlacardTextColor  = kThemeTextColorPlacardActive,
  kThemeInactivePlacardTextColor = kThemeTextColorPlacardInactive,
  kThemePressedPlacardTextColor = kThemeTextColorPlacardPressed,
  kThemeActivePushButtonTextColor = kThemeTextColorPushButtonActive,
  kThemeInactivePushButtonTextColor = kThemeTextColorPushButtonInactive,
  kThemePressedPushButtonTextColor = kThemeTextColorPushButtonPressed,
  kThemeActiveBevelButtonTextColor = kThemeTextColorBevelButtonActive,
  kThemeInactiveBevelButtonTextColor = kThemeTextColorBevelButtonInactive,
  kThemePressedBevelButtonTextColor = kThemeTextColorBevelButtonPressed,
  kThemeActivePopupButtonTextColor = kThemeTextColorPopupButtonActive,
  kThemeInactivePopupButtonTextColor = kThemeTextColorPopupButtonInactive,
  kThemePressedPopupButtonTextColor = kThemeTextColorPopupButtonPressed,
  kThemeIconLabelTextColor      = kThemeTextColorIconLabel,
  kThemeListViewTextColor       = kThemeTextColorListView
};

enum {
  kThemeActiveDocumentWindowTitleTextColor = kThemeTextColorDocumentWindowTitleActive,
  kThemeInactiveDocumentWindowTitleTextColor = kThemeTextColorDocumentWindowTitleInactive,
  kThemeActiveMovableModalWindowTitleTextColor = kThemeTextColorMovableModalWindowTitleActive,
  kThemeInactiveMovableModalWindowTitleTextColor = kThemeTextColorMovableModalWindowTitleInactive,
  kThemeActiveUtilityWindowTitleTextColor = kThemeTextColorUtilityWindowTitleActive,
  kThemeInactiveUtilityWindowTitleTextColor = kThemeTextColorUtilityWindowTitleInactive,
  kThemeActivePopupWindowTitleColor = kThemeTextColorPopupWindowTitleActive,
  kThemeInactivePopupWindowTitleColor = kThemeTextColorPopupWindowTitleInactive,
  kThemeActiveRootMenuTextColor = kThemeTextColorRootMenuActive,
  kThemeSelectedRootMenuTextColor = kThemeTextColorRootMenuSelected,
  kThemeDisabledRootMenuTextColor = kThemeTextColorRootMenuDisabled,
  kThemeActiveMenuItemTextColor = kThemeTextColorMenuItemActive,
  kThemeSelectedMenuItemTextColor = kThemeTextColorMenuItemSelected,
  kThemeDisabledMenuItemTextColor = kThemeTextColorMenuItemDisabled,
  kThemeActivePopupLabelTextColor = kThemeTextColorPopupLabelActive,
  kThemeInactivePopupLabelTextColor = kThemeTextColorPopupLabelInactive
};

enum {
  kAEThemeSwitch                = kAEAppearanceChanged /* Event ID's: Theme Switched */
};

enum {
  kThemeNoAdornment             = kThemeAdornmentNone,
  kThemeDefaultAdornment        = kThemeAdornmentDefault,
  kThemeFocusAdornment          = kThemeAdornmentFocus,
  kThemeRightToLeftAdornment    = kThemeAdornmentRightToLeft,
  kThemeDrawIndicatorOnly       = kThemeAdornmentDrawIndicatorOnly
};

enum {
  kThemeBrushPassiveAreaFill    = kThemeBrushStaticAreaFill
};

enum {
  kThemeMetricCheckBoxGlyphHeight = kThemeMetricCheckBoxHeight,
  kThemeMetricRadioButtonGlyphHeight = kThemeMetricRadioButtonHeight,
  kThemeMetricDisclosureButtonSize = kThemeMetricDisclosureButtonHeight,
  kThemeMetricBestListHeaderHeight = kThemeMetricListHeaderHeight,
  kThemeMetricSmallProgressBarThickness = kThemeMetricNormalProgressBarThickness, /* obsolete */
  kThemeMetricProgressBarThickness = kThemeMetricLargeProgressBarThickness /* obsolete */
};

enum {
  kThemeScrollBar               = kThemeMediumScrollBar,
  kThemeSlider                  = kThemeMediumSlider,
  kThemeProgressBar             = kThemeMediumProgressBar,
  kThemeIndeterminateBar        = kThemeMediumIndeterminateBar
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

#endif /* __APPEARANCE__ */

