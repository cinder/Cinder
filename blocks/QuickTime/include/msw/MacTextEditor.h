/*
     File:       MacTextEditor.h
 
     Contains:   Interfaces for MLTE - TextEdit replacement
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1996-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __MACTEXTEDITOR__
#define __MACTEXTEDITOR__

#ifndef __CONDITIONALMACROS__
#include <ConditionalMacros.h>
#endif

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __CFDICTIONARY__
#include <CFDictionary.h>
#endif

#ifndef __ATSUNICODE__
#include <ATSUnicode.h>
#endif

#ifndef __DRAG__
#include <Drag.h>
#endif

#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
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



typedef struct OpaqueTXNObject*         TXNObject;
typedef struct OpaqueTXNFontMenuObject*  TXNFontMenuObject;
typedef UInt32                          TXNFrameID;
typedef UInt32                          TXNVersionValue;
enum {
  kTXNWillDefaultToATSUIBit     = 0,
  kTXNWillDefaultToCarbonEventBit = 1
};


typedef OptionBits TXNFeatureBits;
enum {
  kTXNWillDefaultToATSUIMask    = 1L << kTXNWillDefaultToATSUIBit,
  kTXNWillDefaultToCarbonEventMask = 1L << kTXNWillDefaultToCarbonEventBit
};

enum {
  kTXNWantMoviesBit             = 0,
  kTXNWantSoundBit              = 1,
  kTXNWantGraphicsBit           = 2,
  kTXNAlwaysUseQuickDrawTextBit = 3,
  kTXNUseTemporaryMemoryBit     = 4
};


typedef OptionBits TXNInitOptions;
enum {
  kTXNWantMoviesMask            = 1L << kTXNWantMoviesBit,
  kTXNWantSoundMask             = 1L << kTXNWantSoundBit,
  kTXNWantGraphicsMask          = 1L << kTXNWantGraphicsBit,
  kTXNAlwaysUseQuickDrawTextMask = 1L << kTXNAlwaysUseQuickDrawTextBit,
  kTXNUseTemporaryMemoryMask    = 1L << kTXNUseTemporaryMemoryBit
};

enum {
  kTXNDrawGrowIconBit           = 0,
  kTXNShowWindowBit             = 1,
  kTXNWantHScrollBarBit         = 2,
  kTXNWantVScrollBarBit         = 3,
  kTXNNoTSMEverBit              = 4,
  kTXNReadOnlyBit               = 5,
  kTXNNoKeyboardSyncBit         = 6,
  kTXNNoSelectionBit            = 7,
  kTXNSaveStylesAsSTYLResourceBit = 8,
  kOutputTextInUnicodeEncodingBit = 9,
  kTXNDoNotInstallDragProcsBit  = 10,
  kTXNAlwaysWrapAtViewEdgeBit   = 11,
  kTXNDontDrawCaretWhenInactiveBit = 12,
  kTXNDontDrawSelectionWhenInactiveBit = 13,
  kTXNSingleLineOnlyBit         = 14,
  kTXNDisableDragAndDropBit     = 15,
  kTXNUseQDforImagingBit        = 16,
  kTXNMonostyledTextBit         = 17
};



/*
 *  TXNFrameOptions
 *  
 *  Summary:
 *    Defines the initial behavior of an MLTE object created with
 *    TXNNewObject.
 *  
 *  Discussion:
 *    These masks can be combined and passed to TXNNewObject to define
 *    the initial behavior of a new object.
 */
typedef OptionBits TXNFrameOptions;
enum {

  /*
   * Indicates that the frame will have a size box.
   */
  kTXNDrawGrowIconMask          = 1L << kTXNDrawGrowIconBit,

  /*
   * Indicates that the window associated with the text object will be
   * displayed when the object is created.  The application no longer
   * needs to call the ShowWindow function from the Window Manager;
   * MLTE will do this for you.
   */
  kTXNShowWindowMask            = 1L << kTXNShowWindowBit,

  /*
   * Indicates that the frame will have a horizontal scrollbar.
   */
  kTXNWantHScrollBarMask        = 1L << kTXNWantHScrollBarBit,

  /*
   * Indicates that the frame will have a vertical scrollbar.
   */
  kTXNWantVScrollBarMask        = 1L << kTXNWantVScrollBarBit,

  /*
   * Indicates that the Text Services Manager will not be used.  You
   * cannot use this mask when your application accepts Unicode input.
   */
  kTXNNoTSMEverMask             = 1L << kTXNNoTSMEverBit,

  /*
   * Indicates that the text object will be read-only.
   */
  kTXNReadOnlyMask              = 1L << kTXNReadOnlyBit,

  /*
   * Indicates that keyboard synchronization will not occur.
   */
  kTXNNoKeyboardSyncMask        = 1L << kTXNNoKeyboardSyncBit,

  /*
   * Indicates that the user shouldn't be able to set the insertion
   * point or make a selection.
   */
  kTXNNoSelectionMask           = 1L << kTXNNoSelectionBit,

  /*
   * Indicates that the text style will be saved as a
   * kTXNMultipleStylesPerTextDocumentResType resource.  You can set
   * this to assure compatibility with SimpleText.  If you use
   * kTXNMultipleStylesPerTextDocumentResType resources to save style
   * info, your documents can have as many styles as you'd like. 
   * However tabs are not saved.  If you don't use this mask, plain
   * text files are saved as kTXNSingleStylePerTextDocumentResType
   * resources, and only the first style in the document is saved. 
   * (Your application is expected to apply all style changes to the
   * entire document.)  If you save files with a
   * kTXNSingleStylePerTextDocumentResType resource, their output is
   * similar to those output by CodeWarrior, BBEdit, and MPW.
   */
  kTXNSaveStylesAsSTYLResourceMask = 1L << kTXNSaveStylesAsSTYLResourceBit,

  /*
   * Indicates that plain text will be saved as Unicode.
   */
  kOutputTextInUnicodeEncodingMask = 1L << kOutputTextInUnicodeEncodingBit,

  /*
   * Indicates that MLTE will not install its own drag handler for the
   * text object.  This can be used if the client wants to install
   * their own handler.
   */
  kTXNDoNotInstallDragProcsMask = 1L << kTXNDoNotInstallDragProcsBit,

  /*
   * Indicates that lines will wrap at the edge of the view rectangle.
   */
  kTXNAlwaysWrapAtViewEdgeMask  = 1L << kTXNAlwaysWrapAtViewEdgeBit,

  /*
   * Indicates that the caret shouldn't be drawn when the text object
   * doesn't have focus.
   */
  kTXNDontDrawCaretWhenInactiveMask = 1L << kTXNDontDrawCaretWhenInactiveBit,

  /*
   * Indicates that the selection (if one) shouldn't be drawn when the
   * text object doesn't have focus.
   */
  kTXNDontDrawSelectionWhenInactiveMask = 1L << kTXNDontDrawSelectionWhenInactiveBit,

  /*
   * Indicates that the text object will not scroll vertically,
   * horizontal scrolling will stop when the end of the text is visible
   * (plus any right margin), and there will be no limit to the width
   * of the text.
   */
  kTXNSingleLineOnlyMask        = 1L << kTXNSingleLineOnlyBit,

  /*
   * Indicates that drag and drop will not be allowed in the text
   * object.
   */
  kTXNDisableDragAndDropMask    = 1L << kTXNDisableDragAndDropBit,

  /*
   * Indicates that QuickDraw will be used for imaging instead of the
   * default CoreGraphics (Quartz). [X-only]
   */
  kTXNUseQDforImagingMask       = 1L << kTXNUseQDforImagingBit,

  /*
   * Indicates that the text object will keep in single style no matter
   * what kind of changes made to the object.
   */
  kTXNMonostyledTextMask        = 1L << kTXNMonostyledTextBit
};

enum {
  kTXNSetFlushnessBit           = 0,
  kTXNSetJustificationBit       = 1,
  kTXNUseFontFallBackBit        = 2,
  kTXNRotateTextBit             = 3,
  kTXNUseVerticalTextBit        = 4,
  kTXNDontUpdateBoxRectBit      = 5,
  kTXNDontDrawTextBit           = 6,
  kTXNUseCGContextRefBit        = 7,
  kTXNImageWithQDBit            = 8,
  kTXNDontWrapTextBit           = 9
};


/*
 *  TXNTextBoxOptions
 *  
 *  Summary:
 *    Defines how text will be drawn by one of the TXNxxxDrawTextBox
 *    API.
 *  
 *  Discussion:
 *    These masks can be combined and added to a TXNTextBoxOptionsData
 *    structure to be passed to a TXNxxxDrawTextBox API.
 */
typedef OptionBits TXNTextBoxOptions;
enum {

  /*
   * Indicates that the text will be flush according to the line
   * direction.
   */
  kTXNSetFlushnessMask          = 1L << kTXNSetFlushnessBit,

  /*
   * Indicates that the text will be justified in the direction that
   * the text is displayed.  Horizontal text will be justified
   * horizontally, but not vertically.  Vertical text will be justified
   * vertically, but not horizontally.
   */
  kTXNSetJustificationMask      = 1L << kTXNSetJustificationBit,

  /*
   * Indicates that ATSUI transient font matching (that searches for a
   * font that has a matching character) will be used.
   */
  kTXNUseFontFallBackMask       = 1L << kTXNUseFontFallBackBit,

  /*
   * Indicates that the text will be rotated.  The amount of rotation
   * is given in the rotation field of the TXNTextBoxOptionsData
   * structure and is in units of degrees (negative values indicate
   * clockwise rotation).
   */
  kTXNRotateTextMask            = 1L << kTXNRotateTextBit,

  /*
   * Indicates that the text will be displayed vertically from top to
   * bottom.
   */
  kTXNUseVerticalTextMask       = 1L << kTXNUseVerticalTextBit,

  /*
   * Indicates that the specified rectangle will not be updated.  If
   * you use this mask when you call a TXNDrawxxxTextBox function, the
   * funtion does not update the right coordinate (bottom coordinate if
   * kTXNUseVerticalTextMask is used) of the specified rectangle to
   * accommodate the longest line for text.
   */
  kTXNDontUpdateBoxRectMask     = 1L << kTXNDontUpdateBoxRectBit,

  /*
   * Indicates that the size of the text will be returned but the text
   * box will not be drawn.
   */
  kTXNDontDrawTextMask          = 1L << kTXNDontDrawTextBit,

  /*
   * Indicates that the client has provided a CGContext to be used for
   * CG imaging inside the text box. [X-only]
   */
  kTXNUseCGContextRefMask       = 1L << kTXNUseCGContextRefBit,

  /*
   * Indicates that imaging will be done using QuickDraw instead of the
   * default CoreGraphics. [X-only]
   */
  kTXNImageWithQDMask           = 1L << kTXNImageWithQDBit,

  /*
   * Indicates that text should not be wrapped. [X-only]
   */
  kTXNDontWrapTextMask          = 1L << kTXNDontWrapTextBit
};

struct TXNTextBoxOptionsData {
  TXNTextBoxOptions   optionTags;
  Fract               flushness;
  Fract               justification;
  Fixed               rotation;
  void *              options;                /* for future use*/
};
typedef struct TXNTextBoxOptionsData    TXNTextBoxOptionsData;
enum {
  kTXNFontContinuousBit         = 0,
  kTXNSizeContinuousBit         = 1,
  kTXNStyleContinuousBit        = 2,
  kTXNColorContinuousBit        = 3
};

typedef OptionBits TXNContinuousFlags;
enum {
  kTXNFontContinuousMask        = 1L << kTXNFontContinuousBit,
  kTXNSizeContinuousMask        = 1L << kTXNSizeContinuousBit,
  kTXNStyleContinuousMask       = 1L << kTXNStyleContinuousBit,
  kTXNColorContinuousMask       = 1L << kTXNColorContinuousBit
};

enum {
  kTXNIgnoreCaseBit             = 0,
  kTXNEntireWordBit             = 1,
  kTXNUseEncodingWordRulesBit   = 31
};

typedef OptionBits TXNMatchOptions;
enum {
  kTXNIgnoreCaseMask            = 1L << kTXNIgnoreCaseBit,
  kTXNEntireWordMask            = 1L << kTXNEntireWordBit,
  kTXNUseEncodingWordRulesMask  = (unsigned long)(1L << kTXNUseEncodingWordRulesBit)
};


typedef OSType TXNFileType;
enum {
  kTXNTextensionFile            = FOUR_CHAR_CODE('txtn'),
  kTXNTextFile                  = FOUR_CHAR_CODE('TEXT'),
  kTXNPictureFile               = FOUR_CHAR_CODE('PICT'),
  kTXNMovieFile                 = FOUR_CHAR_CODE('MooV'),
  kTXNSoundFile                 = FOUR_CHAR_CODE('sfil'),
  kTXNAIFFFile                  = FOUR_CHAR_CODE('AIFF'),
  kTXNUnicodeTextFile           = FOUR_CHAR_CODE('utxt')
};

/* Only kTXNTextEditStyleFrameType is supported at this time */
typedef UInt32 TXNFrameType;
enum {
  kTXNTextEditStyleFrameType    = 1,
  kTXNPageFrameType             = 2,    /* not supported*/
  kTXNMultipleFrameType         = 3     /* not supported*/
};


typedef OSType TXNDataType;
enum {
  kTXNTextData                  = FOUR_CHAR_CODE('TEXT'),
  kTXNPictureData               = FOUR_CHAR_CODE('PICT'),
  kTXNMovieData                 = FOUR_CHAR_CODE('moov'),
  kTXNSoundData                 = FOUR_CHAR_CODE('snd '),
  kTXNUnicodeTextData           = FOUR_CHAR_CODE('utxt')
};


typedef FourCharCode TXNControlTag;
enum {
  kTXNLineDirectionTag          = FOUR_CHAR_CODE('lndr'),
  kTXNJustificationTag          = FOUR_CHAR_CODE('just'),
  kTXNIOPrivilegesTag           = FOUR_CHAR_CODE('iopv'),
  kTXNSelectionStateTag         = FOUR_CHAR_CODE('slst'),
  kTXNInlineStateTag            = FOUR_CHAR_CODE('inst'),
  kTXNWordWrapStateTag          = FOUR_CHAR_CODE('wwrs'),
  kTXNKeyboardSyncStateTag      = FOUR_CHAR_CODE('kbsy'),
  kTXNAutoIndentStateTag        = FOUR_CHAR_CODE('auin'),
  kTXNTabSettingsTag            = FOUR_CHAR_CODE('tabs'),
  kTXNRefConTag                 = FOUR_CHAR_CODE('rfcn'),
  kTXNMarginsTag                = FOUR_CHAR_CODE('marg'),
  kTXNFlattenMoviesTag          = FOUR_CHAR_CODE('flat'),
  kTXNDoFontSubstitution        = FOUR_CHAR_CODE('fSub'), /*note : this could degrade performance greatly in the case of large documents.*/
  kTXNNoUserIOTag               = FOUR_CHAR_CODE('nuio'),
  kTXNUseCarbonEvents           = FOUR_CHAR_CODE('cbcb'),
  kTXNDrawCaretWhenInactiveTag  = FOUR_CHAR_CODE('dcrt'),
  kTXNDrawSelectionWhenInactiveTag = FOUR_CHAR_CODE('dsln'),
  kTXNDisableDragAndDropTag     = FOUR_CHAR_CODE('drag'),
  kTXNSingleLevelUndoTag        = FOUR_CHAR_CODE('undo'),
  kTXNVisibilityTag             = FOUR_CHAR_CODE('visb') /*set the visibility state of the object*/
};

typedef UInt32 TXNActionKey;
enum {
  kTXNTypingAction              = 0,
  kTXNCutAction                 = 1,
  kTXNPasteAction               = 2,
  kTXNClearAction               = 3,
  kTXNChangeFontAction          = 4,
  kTXNChangeFontColorAction     = 5,
  kTXNChangeFontSizeAction      = 6,
  kTXNChangeStyleAction         = 7,
  kTXNAlignLeftAction           = 8,
  kTXNAlignCenterAction         = 9,
  kTXNAlignRightAction          = 10,
  kTXNDropAction                = 11,
  kTXNMoveAction                = 12,
  kTXNFontFeatureAction         = 13,
  kTXNFontVariationAction       = 14,
  kTXNUndoLastAction            = 1024  /*use if none of the above apply*/
};

enum {
  kTXNClearThisControl          = (long)0xFFFFFFFF,
  kTXNClearTheseFontFeatures    = (long)0x80000000
};

/*
  convenience constants for TXNGet/SetTXNControls
   kTXNIOPrivilegesTag
*/
enum {
  kTXNReadWrite                 = false,
  kTXNReadOnly                  = true
};

/* kTXNSelectionStateTag*/
enum {
  kTXNSelectionOn               = true,
  kTXNSelectionOff              = false
};

/* kTXNInlineStateTag*/
enum {
  kTXNUseInline                 = false,
  kTXNUseBottomline             = true
};


/* kTXNWordWrapStateTag*/
enum {
  kTXNAutoWrap                  = false,
  kTXNNoAutoWrap                = true
};

/* kTXNKeyboardSyncStateTag*/
enum {
  kTXNSyncKeyboard              = false,
  kTXNNoSyncKeyboard            = true
};

/* kTXNAutoIndentStateTag*/
enum {
  kTXNAutoIndentOff             = false,
  kTXNAutoIndentOn              = true
};

/* kTXNDrawCaretWhenInactiveTag*/
enum {
  kTXNDontDrawCaretWhenInactive = false,
  kTXNDrawCaretWhenInactive     = true
};

/* kTXNDrawSelectionWhenInactiveTag*/
enum {
  kTXNDontDrawSelectionWhenInactive = false,
  kTXNDrawSelectionWhenInactive = true
};

/* kTXNDisableDragAndDropTag*/
enum {
  kTXNEnableDragAndDrop         = false,
  kTXNDisableDragAndDrop        = true
};

typedef SInt8 TXNTabType;
enum {
  kTXNRightTab                  = -1,
  kTXNLeftTab                   = 0,
  kTXNCenterTab                 = 1
};

struct TXNTab {
  SInt16              value;
  TXNTabType          tabType;
  UInt8               filler;
};
typedef struct TXNTab                   TXNTab;
enum {
  kTXNLeftToRight               = 0,
  kTXNRightToLeft               = 1
};

enum {
  kTXNFlushDefault              = 0,    /*flush according to the line direction */
  kTXNFlushLeft                 = 1,
  kTXNFlushRight                = 2,
  kTXNCenter                    = 4,
  kTXNFullJust                  = 8,
  kTXNForceFullJust             = 16    /*flush left for all scripts */
};

/*
  In version 1.2 of MLTE and later you can change the top, left and right margins. 
  The bottom margin is a placeholder for possible future enhancements. 
*/
struct TXNMargins {
  SInt16              topMargin;
  SInt16              leftMargin;
  SInt16              bottomMargin;
  SInt16              rightMargin;
};
typedef struct TXNMargins               TXNMargins;
union TXNControlData {
  UInt32              uValue;
  SInt32              sValue;
  TXNTab              tabValue;
  TXNMargins *        marginsPtr;
};
typedef union TXNControlData            TXNControlData;
typedef Boolean TXNScrollBarState;
enum {
  kScrollBarsAlwaysActive       = true,
  kScrollBarsSyncWithFocus      = false
};

/*
  kTXNNoFontVariations is returned in the dataValue field when the caller as asked
  to see if the variation is continuous and there was no variation in the continuous range
*/
enum {
  kTXNDontCareTypeSize          = (long)0xFFFFFFFF,
  kTXNDontCareTypeStyle         = 0xFF,
  kTXNIncrementTypeSize         = 0x00000001,
  kTXNDecrementTypeSize         = (long)0x80000000,
  kTXNUseScriptDefaultValue     = -1,
  kTXNNoFontVariations          = 0x7FFF
};

typedef UInt32 TXNOffset;
enum {
  kTXNUseCurrentSelection       = (unsigned long)0xFFFFFFFF,
  kTXNStartOffset               = 0,
  kTXNEndOffset                 = 0x7FFFFFFF
};


enum {
  kTXNSingleStylePerTextDocumentResType = FOUR_CHAR_CODE('MPSR'),
  kTXNMultipleStylesPerTextDocumentResType = FOUR_CHAR_CODE('styl')
};


typedef void *                          TXNObjectRefcon;
/*constants for TXNShowSelection*/
enum {
  kTXNShowStart                 = false,
  kTXNShowEnd                   = true
};

typedef OSStatus                        TXNErrors;
/*default constants for TXTNInit.  */
#define kTXNDefaultFontName             ((StringPtr)NULL)
enum {
  kTXNDefaultFontSize           = 0x000C0000
};

enum {
  kTXNDefaultFontStyle          = normal
};


typedef UInt32 TXNHyperLinkState;
enum {
  kTXNLinkNotPressed            = 0,
  kTXNLinkWasPressed            = 1,
  kTXNLinkTracking              = 3
};

typedef FourCharCode TXNTypeRunAttributes;
enum {
  kTXNQDFontNameAttribute       = FOUR_CHAR_CODE('fntn'),
  kTXNQDFontFamilyIDAttribute   = FOUR_CHAR_CODE('font'),
  kTXNQDFontSizeAttribute       = FOUR_CHAR_CODE('size'),
  kTXNQDFontStyleAttribute      = FOUR_CHAR_CODE('face'),
  kTXNQDFontColorAttribute      = FOUR_CHAR_CODE('klor'),
  kTXNTextEncodingAttribute     = FOUR_CHAR_CODE('encd'),
  kTXNATSUIFontFeaturesAttribute = FOUR_CHAR_CODE('atfe'),
  kTXNATSUIFontVariationsAttribute = FOUR_CHAR_CODE('atva'),
  kTXNURLAttribute              = FOUR_CHAR_CODE('urla')
};

/*
  kTXNQDFontSizeAttributeSize is obsolete and incorrect
  font sizes are always returned as a Fixed value, just as
  they are passed to MLTE.  Use kTXNFontSizeAttributeSize.
*/
typedef ByteCount TXNTypeRunAttributeSizes;
enum {
  kTXNQDFontNameAttributeSize   = sizeof(Str255),
  kTXNQDFontFamilyIDAttributeSize = sizeof(SInt16),
  kTXNQDFontSizeAttributeSize   = sizeof(SInt16),
  kTXNQDFontStyleAttributeSize  = sizeof(Style),
  kTXNQDFontColorAttributeSize  = sizeof(RGBColor),
  kTXNTextEncodingAttributeSize = sizeof(TextEncoding),
  kTXNFontSizeAttributeSize     = sizeof(Fixed)
};

typedef UInt32 TXNPermanentTextEncodingType;
enum {
  kTXNSystemDefaultEncoding     = 0,
  kTXNMacOSEncoding             = 1,
  kTXNUnicodeEncoding           = 2
};



typedef FourCharCode                    TXTNTag;
struct TXNATSUIFeatures {
  ItemCount           featureCount;
  ATSUFontFeatureType * featureTypes;
  ATSUFontFeatureSelector * featureSelectors;
};
typedef struct TXNATSUIFeatures         TXNATSUIFeatures;
struct TXNATSUIVariations {
  ItemCount           variationCount;
  ATSUFontVariationAxis * variationAxis;
  ATSUFontVariationValue * variationValues;
};
typedef struct TXNATSUIVariations       TXNATSUIVariations;
union TXNAttributeData {
  void *              dataPtr;
  UInt32              dataValue;
  TXNATSUIFeatures *  atsuFeatures;
  TXNATSUIVariations * atsuVariations;
  CFURLRef            urlReference;
};
typedef union TXNAttributeData          TXNAttributeData;
struct TXNTypeAttributes {
  TXTNTag             tag;
  ByteCount           size;
  TXNAttributeData    data;
};
typedef struct TXNTypeAttributes        TXNTypeAttributes;
struct TXNMacOSPreferredFontDescription {
  UInt32              fontID;
  Fixed               pointSize;
  TextEncoding        encoding;
  Style               fontStyle;
};
typedef struct TXNMacOSPreferredFontDescription TXNMacOSPreferredFontDescription;
struct TXNMatchTextRecord {
  const void *        iTextPtr;
  SInt32              iTextToMatchLength;
  TextEncoding        iTextEncoding;
};
typedef struct TXNMatchTextRecord       TXNMatchTextRecord;
/*constants & typedefs for setting the background*/
typedef UInt32 TXNBackgroundType;
enum {
  kTXNBackgroundTypeRGB         = 1
};

/*
   The TXNBackgroundData is left as a union so that it can be expanded
   in the future to support other background types
*/
union TXNBackgroundData {
  RGBColor            color;
};
typedef union TXNBackgroundData         TXNBackgroundData;
struct TXNBackground {
  TXNBackgroundType   bgType;
  TXNBackgroundData   bg;
};
typedef struct TXNBackground            TXNBackground;
struct TXNLongRect {
  SInt32              top;
  SInt32              left;
  SInt32              bottom;
  SInt32              right;
};
typedef struct TXNLongRect              TXNLongRect;
/*
  options for TXNGet/ClearActionChangeCount to decide what type(s) of action 
  count to use 
*/
enum {
  kTXNTextInputCountBit         = 0,
  kTXNRunCountBit               = 1
};

typedef OptionBits TXNCountOptions;
enum {
  kTXNTextInputCountMask        = 1L << kTXNTextInputCountBit,
  kTXNRunCountMask              = 1L << kTXNRunCountBit,
  kTXNAllCountMask              = kTXNTextInputCountMask | kTXNRunCountMask
};


typedef UInt32 TXNScrollUnit;
enum {
  kTXNScrollUnitsInPixels       = 0,
  kTXNScrollUnitsInLines        = 1,
  kTXNScrollUnitsInViewRects    = 2
};

typedef UInt32 TXNScrollBarOrientation;
enum {
  kTXNHorizontal                = 0,
  kTXNVertical                  = 1
};

typedef CALLBACK_API( OSStatus , TXNFindProcPtr )(const TXNMatchTextRecord *matchData, TXNDataType iDataType, TXNMatchOptions iMatchOptions, const void *iSearchTextPtr, TextEncoding encoding, TXNOffset absStartOffset, ByteCount searchTextLength, TXNOffset *oStartMatch, TXNOffset *oEndMatch, Boolean *ofound, UInt32 refCon);
typedef CALLBACK_API( CFStringRef , TXNActionKeyMapperProcPtr )(TXNActionKey actionKey, UInt32 commandID);

typedef CALLBACK_API( void , TXNScrollInfoProcPtr )(SInt32 iValue, SInt32 iMaximumValue, TXNScrollBarOrientation iScrollBarOrientation, SInt32 iRefCon);
typedef STACK_UPP_TYPE(TXNFindProcPtr)                          TXNFindUPP;
typedef STACK_UPP_TYPE(TXNActionKeyMapperProcPtr)               TXNActionKeyMapperUPP;
typedef STACK_UPP_TYPE(TXNScrollInfoProcPtr)                    TXNScrollInfoUPP;
/*
 *  NewTXNFindUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( TXNFindUPP )
NewTXNFindUPP(TXNFindProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppTXNFindProcInfo = 0x0FFFFFF0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(TXNFindUPP) NewTXNFindUPP(TXNFindProcPtr userRoutine) { return (TXNFindUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTXNFindProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewTXNFindUPP(userRoutine) (TXNFindUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTXNFindProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewTXNActionKeyMapperUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( TXNActionKeyMapperUPP )
NewTXNActionKeyMapperUPP(TXNActionKeyMapperProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppTXNActionKeyMapperProcInfo = 0x000003F0 };  /* pascal 4_bytes Func(4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(TXNActionKeyMapperUPP) NewTXNActionKeyMapperUPP(TXNActionKeyMapperProcPtr userRoutine) { return (TXNActionKeyMapperUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTXNActionKeyMapperProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewTXNActionKeyMapperUPP(userRoutine) (TXNActionKeyMapperUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTXNActionKeyMapperProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewTXNScrollInfoUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( TXNScrollInfoUPP )
NewTXNScrollInfoUPP(TXNScrollInfoProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppTXNScrollInfoProcInfo = 0x00003FC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(TXNScrollInfoUPP) NewTXNScrollInfoUPP(TXNScrollInfoProcPtr userRoutine) { return (TXNScrollInfoUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTXNScrollInfoProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewTXNScrollInfoUPP(userRoutine) (TXNScrollInfoUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppTXNScrollInfoProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeTXNFindUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeTXNFindUPP(TXNFindUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeTXNFindUPP(TXNFindUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeTXNFindUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeTXNActionKeyMapperUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeTXNActionKeyMapperUPP(TXNActionKeyMapperUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeTXNActionKeyMapperUPP(TXNActionKeyMapperUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeTXNActionKeyMapperUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeTXNScrollInfoUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
DisposeTXNScrollInfoUPP(TXNScrollInfoUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeTXNScrollInfoUPP(TXNScrollInfoUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeTXNScrollInfoUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeTXNFindUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
InvokeTXNFindUPP(
  const TXNMatchTextRecord *  matchData,
  TXNDataType                 iDataType,
  TXNMatchOptions             iMatchOptions,
  const void *                iSearchTextPtr,
  TextEncoding                encoding,
  TXNOffset                   absStartOffset,
  ByteCount                   searchTextLength,
  TXNOffset *                 oStartMatch,
  TXNOffset *                 oEndMatch,
  Boolean *                   ofound,
  UInt32                      refCon,
  TXNFindUPP                  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(OSStatus) InvokeTXNFindUPP(const TXNMatchTextRecord * matchData, TXNDataType iDataType, TXNMatchOptions iMatchOptions, const void * iSearchTextPtr, TextEncoding encoding, TXNOffset absStartOffset, ByteCount searchTextLength, TXNOffset * oStartMatch, TXNOffset * oEndMatch, Boolean * ofound, UInt32 refCon, TXNFindUPP userUPP) { return (OSStatus)CALL_ELEVEN_PARAMETER_UPP(userUPP, uppTXNFindProcInfo, matchData, iDataType, iMatchOptions, iSearchTextPtr, encoding, absStartOffset, searchTextLength, oStartMatch, oEndMatch, ofound, refCon); }
  #else
    #define InvokeTXNFindUPP(matchData, iDataType, iMatchOptions, iSearchTextPtr, encoding, absStartOffset, searchTextLength, oStartMatch, oEndMatch, ofound, refCon, userUPP) (OSStatus)CALL_ELEVEN_PARAMETER_UPP((userUPP), uppTXNFindProcInfo, (matchData), (iDataType), (iMatchOptions), (iSearchTextPtr), (encoding), (absStartOffset), (searchTextLength), (oStartMatch), (oEndMatch), (ofound), (refCon))
  #endif
#endif

/*
 *  InvokeTXNActionKeyMapperUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( CFStringRef )
InvokeTXNActionKeyMapperUPP(
  TXNActionKey           actionKey,
  UInt32                 commandID,
  TXNActionKeyMapperUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(CFStringRef) InvokeTXNActionKeyMapperUPP(TXNActionKey actionKey, UInt32 commandID, TXNActionKeyMapperUPP userUPP) { return (CFStringRef)CALL_TWO_PARAMETER_UPP(userUPP, uppTXNActionKeyMapperProcInfo, actionKey, commandID); }
  #else
    #define InvokeTXNActionKeyMapperUPP(actionKey, commandID, userUPP) (CFStringRef)CALL_TWO_PARAMETER_UPP((userUPP), uppTXNActionKeyMapperProcInfo, (actionKey), (commandID))
  #endif
#endif

/*
 *  InvokeTXNScrollInfoUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.1 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
InvokeTXNScrollInfoUPP(
  SInt32                   iValue,
  SInt32                   iMaximumValue,
  TXNScrollBarOrientation  iScrollBarOrientation,
  SInt32                   iRefCon,
  TXNScrollInfoUPP         userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeTXNScrollInfoUPP(SInt32 iValue, SInt32 iMaximumValue, TXNScrollBarOrientation iScrollBarOrientation, SInt32 iRefCon, TXNScrollInfoUPP userUPP) { CALL_FOUR_PARAMETER_UPP(userUPP, uppTXNScrollInfoProcInfo, iValue, iMaximumValue, iScrollBarOrientation, iRefCon); }
  #else
    #define InvokeTXNScrollInfoUPP(iValue, iMaximumValue, iScrollBarOrientation, iRefCon, userUPP) CALL_FOUR_PARAMETER_UPP((userUPP), uppTXNScrollInfoProcInfo, (iValue), (iMaximumValue), (iScrollBarOrientation), (iRefCon))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewTXNFindProc(userRoutine)                         NewTXNFindUPP(userRoutine)
    #define NewTXNActionKeyMapperProc(userRoutine)              NewTXNActionKeyMapperUPP(userRoutine)
    #define NewTXNScrollInfoProc(userRoutine)                   NewTXNScrollInfoUPP(userRoutine)
    #define CallTXNFindProc(userRoutine, matchData, iDataType, iMatchOptions, iSearchTextPtr, encoding, absStartOffset, searchTextLength, oStartMatch, oEndMatch, ofound, refCon) InvokeTXNFindUPP(matchData, iDataType, iMatchOptions, iSearchTextPtr, encoding, absStartOffset, searchTextLength, oStartMatch, oEndMatch, ofound, refCon, userRoutine)
    #define CallTXNActionKeyMapperProc(userRoutine, actionKey, commandID) InvokeTXNActionKeyMapperUPP(actionKey, commandID, userRoutine)
    #define CallTXNScrollInfoProc(userRoutine, iValue, iMaximumValue, iScrollBarOrientation, iRefCon) InvokeTXNScrollInfoUPP(iValue, iMaximumValue, iScrollBarOrientation, iRefCon, userRoutine)
#endif /* CALL_NOT_IN_CARBON */




/*
  These are currently the only settings for the flags field of TXNCarbonEventInfo
  If you want the AppleEventHandlers removed use kTXNNoAppleEventHandlersMask.
  If you want to subsequently restart AppleEvent Handlers after removing
  your Texthandlers. Use kTXNRestartAppleEventHandlersMask.
*/

enum {
  kTXNNoAppleEventHandlersBit   = 0,
  kTXNRestartAppleEventHandlersBit = 1
};

enum {
  kTXNNoAppleEventHandlersMask  = 1 << kTXNNoAppleEventHandlersBit,
  kTXNRestartAppleEventHandlersMask = 1 << kTXNRestartAppleEventHandlersBit
};

/*dictionary keys currently supported in the TXNCarbonEventInfo dictionary*/
#define   kTXNTextHandlerKey                        CFSTR("TextInput")
#define   kTXNWindowEventHandlerKey                 CFSTR("WindowEvent")
#define   kTXNWindowResizeEventHandlerKey           CFSTR("WindowResize")
#define   kTXNCommandTargetKey                      CFSTR("CommandTarget")
#define   kTXNCommandUpdateKey                      CFSTR("CommandUpdate")
#define   kTXNFontMenuRefKey                        CFSTR("FontMenuRef")
#define   kTXNFontMenuObjectKey                     CFSTR("FontMenuObject")
#define   kTXNActionKeyMapperKey                    CFSTR("ActionKeyMapper")
#define   kTXNWheelMouseEventHandlerKey             CFSTR("WheelMouseEvent")

/* use this to pass an EventTargetRef to MLTE via the TXNSetTXNControl... call*/
struct TXNCarbonEventInfo {
  Boolean             useCarbonEvents;
  UInt8               filler;
  UInt16              flags;
  CFDictionaryRef     fDictionary;
};
typedef struct TXNCarbonEventInfo       TXNCarbonEventInfo;

/*
  *****************************************************************************************************
   Allocates a new frame (i.e. new is called to allocate a TXNObject) and returns a pointer to the object 
   in the newDoc parameter.
   Input:
        
    iFileSpec:  If not NULL the file is read to obtain the document contents  after the object is 
                successfully allocated.  If NULL you start with an empty document.
                
    iWindow:        Required.  The window in which the document is going to be  displayed.
                
    iFrame:     If text-area does not fill the entire window.  This specifies the area to fill.  Can be NULL.  
                In  which case, the window's portRect is used as the frame.
                                
    iFrameOptions:  Specify the options to be supported by this frame.  The available options are support 
                    for cutting and pasting  movies and sound, handle scrollbars and handle grow box in  the 
                    frame.
    iFrameType:     Specify the type of frame to be used.  In MLTE version 1.1 and earlier, only 
                    kTXNTextEditStyleFrameType is supported.
   
    iFileType:  Specify the primary file type.  If you  use  kTextensionTextFile files will be saved 
                in a private format (see xxx).  If you  want saved files to be plain text files you should 
                specify 'TEXT' here. If you specify 'TEXT' here you can use the frameOptions parameter to 
                specify  whether the TEXT files should be saved  with 'MPSR' resources or 'styl' resources.  
                These are resources which contain style information for a  file, and they  both have there 
                own limitations.  If you use 'styl' resources to save style info your documents can have as 
                many styles as you like however tabs will not be saved.  If you use 'MPSR' resources only the 
                first style in the document  will be saved (you as client are expected to apply all style  
                changes to the entire document).  If you  truly want  rich documents which can potentially 
                contain graphics and sound you should specify kTextensionTextFileOutput.  If you want a plain 
                text editor like SimpleText specify that style information by saved via 'styl' resources.  
                If you want files similar to those output by CW IDE, BBEdit, and MPW specify that style 
                information be saved in a 'MPSR' resource.
   
   Output:
    
    OSStatus:   function  result.  If anything goes wrong the error is returned.  Success must be complete.  
                That is if everything  works, but there is a failure reading a specified file the  object 
                is freed.
    oTXNObject:     Pointer to the opaque datastructure allocated by the function.  Most of the subsequent 
                    functions require that such a pointer be passed in.
                
    oTXNFrameID:    Unique ID for the frame. <Note in version 1.0 this value is always set to 0>
    iRefCon:        Caller can set this to any value.  It is retained by the
                    TXNNewObject which can later be asked to return it.
  **************************************************************************************************************
*/
/*
 *  TXNNewObject()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNNewObject(
  const FSSpec *                 iFileSpec,                /* can be NULL */
  WindowRef                      iWindow,
  const Rect *                   iFrame,                   /* can be NULL */
  TXNFrameOptions                iFrameOptions,
  TXNFrameType                   iFrameType,
  TXNFileType                    iFileType,
  TXNPermanentTextEncodingType   iPermanentEncoding,
  TXNObject *                    oTXNObject,
  TXNFrameID *                   oTXNFrameID,
  TXNObjectRefcon                iRefCon);



/*
  *************************************************************************************************
   Delete a previously allocated TXNObject and all associated data structures.  If the frameType is 
   multiple frames all frames are released.
        
   Input:
    iTXNObject: opaque structure to free.
  **************************************************************************************************
*/
/*
 *  TXNDeleteObject()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNDeleteObject(TXNObject iTXNObject);



/*
  *************************************************************************************************                         
   Changes the frame's size to match the new width and height.
   Input:
    
    iTXNObject:     opaque Textension structure.
    
    iWidth:         New width in pixels.
    
    iHeight:        New height in pixels.
    
    iTXNFrameID:    FrameID that specifies the frame to move.
  *************************************************************************************************
*/
/*
 *  TXNResizeFrame()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNResizeFrame(
  TXNObject    iTXNObject,
  UInt32       iWidth,
  UInt32       iHeight,
  TXNFrameID   iTXNFrameID);



/*
  *************************************************************************************************                         
   Changes the frame's bounds to match the Rect. 
   Input:
        iTXNObject :                                opaque Textension structure.
        
        (iTop, iLeft, iBottom, iRight):     Rect of the view
         
    
    iTXNFrameID:        FrameID that specifies the frame to move.
  *************************************************************************************************
*/
/*
 *  TXNSetFrameBounds()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNSetFrameBounds(
  TXNObject    iTXNObject,
  SInt32       iTop,
  SInt32       iLeft,
  SInt32       iBottom,
  SInt32       iRight,
  TXNFrameID   iTXNFrameID);



/*
  ****************************************************************************************************
    Initialize the Textension library.  Should be called as soon as possible after the Macintosh toolbox
    is initialized.
   Input:
    iDefaultFonts:      A table of font information including fontFamily ID, point size,
                        style, and script code. The table can be NULL or can have
                        an entry for any script for which you would like to to
                        designate a default font.  Only a valid script number is
                        required.  You can designate that Textension should use
                        the default for a give script by setting the field to kTXNUseScriptDefaultValue (-1).
                                            
   For example, if you wanted to specify New York as the default
   font to use for Roman scripts, but were happy with the 
   default style and size you would call the function like this:
    
    TXNMacOSPreferredFontDescription    defaults;
    GetFNum( "\pNew York", &defaults.fontFamilyID );
    defaults.pointSize = kTXNDefaultFontSize;
    defaults.fontStyle = kTXNDefaultFontStyle;
    defaults.script = smRoman;
    status = TXNInitTextension( &defaults, 1, 0 );
    iCountDefaultFonts: Count of entries in the iDefaultFonts parameter.
    
                    
                            
    iUsageFlags:        Specify whether sound and movies should be supported.
   Output:
    OSStatus:       Function result.  NoErr if everything initialized correctly.  Variety of
                    possible MacOS errors if something goes wrong.
  *********************************************************************************************|
*/
/*
 *  TXNInitTextension()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNInitTextension(
  const TXNMacOSPreferredFontDescription   iDefaultFonts[],          /* can be NULL */
  ItemCount                                iCountDefaultFonts,
  TXNInitOptions                           iUsageFlags);



/*
  *************************************************************************************
   Close the Textension library.  It is necessary to call this function so that Textension 
   can correctly close down any TSM connections and do other clean up.
  **************************************************************************************
*/
/*
 *  TXNTerminateTextension()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNTerminateTextension(void);



/*
  **************************************************************************************
    Process a keydown event. Note that if CJK script is installed and current font is 
    CJK inline input will take place. This is always the case unless the application has 
    requested the bottomline window or has turned off TSM (see initialization options above).
        
   Input:
        iTXNObject:     opaque struct to apply keydown to.
            
        iEvent:     the keydown event.  
  ***************************************************************************************
*/
/*
 *  TXNKeyDown()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNKeyDown(
  TXNObject            iTXNObject,
  const EventRecord *  iEvent);


/*
  ***************************************************************************************
    Handle switching the cursor.  If over text area set to i-beam.  Over graphics, sound,
    movie, scrollbar or outside of window set to arrow.
    
    Input:
        iTXNObject:         Opaque struct obtained from TXNNewObject.
        ioCursorRgn:        Region to be passed to WaitNextEvent.  Resized  accordingly by 
                        TXNAdjustCursor.
  ***************************************************************************************
*/
/*
 *  TXNAdjustCursor()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNAdjustCursor(
  TXNObject   iTXNObject,
  RgnHandle   ioCursorRgn);



/*
  ****************************************************************************************                      
    Process click in content region.  Takes care of scrolling, selecting text,  playing 
    sound and movies, drag & drop, and double-clicks.
    Input:
        iTXNObject:     Opaque struct obtained from TXNNewObject.
        iEvent:     the mousedown event
  *****************************************************************************************
*/
/*
 *  TXNClick()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNClick(
  TXNObject            iTXNObject,
  const EventRecord *  iEvent);



/*
  ********************************************************************************************
    
    Call this when WaitNextEvent returns false or there is no active TSNObject . 
    The TXNObject parameter can be NULL which allows a client to call this function at any 
    time.  This is necessary to insure input methods enough time to be reasonably responsive.
    NOTE : You do not need to call this when working on Carbon, TSM events are passed directly
           to the text object.
    Input:
        iTXNObject:         The currently active TXNObject or NULL.
        ioEvent:            The event record.  Usually a NULL event.  If the 
                            event is not an NULL event on entry, and an input
                            method consumes the event the event should return
                            as a NULL event.
    Output:
        Boolean:        True if TSM handled this event.  False if TSM did not handle this event.
  **********************************************************************************************
*/
#if CALL_NOT_IN_CARBON
/*
 *  TXNTSMCheck()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( Boolean )
TXNTSMCheck(
  TXNObject      iTXNObject,       /* can be NULL */
  EventRecord *  ioEvent);



/*
  ***********************************************************************************************                       
    Selects everything in a frame.
    Input:
        iTXNObject: opaque TXNObject 
  ***********************************************************************************************   
*/
#endif  /* CALL_NOT_IN_CARBON */

/*
 *  TXNSelectAll()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNSelectAll(TXNObject iTXNObject);




/*
  ***********************************************************************************************                       
    Focues the TXNObject.  Scrollbars and insertion point are made active  if iBecomingFocused
    is true, and inactive if false.
    
    Input:
        iTXNObject:         opaque TXNObject
        
        iBecomingFocused:   true if becoming active.  false otherwise.
  ************************************************************************************************  
*/
/*
 *  TXNFocus()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNFocus(
  TXNObject   iTXNObject,
  Boolean     iBecomingFocused);


/*
  ************************************************************************************************                      
    Handle update event (i.e. draw everything in a frame.) This function calls the Toolbox
    BeginUpdate - EndUpdate functions for the window that was passed to TXNNewObject.  This
    makes it inappropriate for windows that contain something else besides the TXNObject.
    Input:
        iTXNObject: opaque TXNObject 
  ************************************************************************************************
*/
/*
 *  TXNUpdate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNUpdate(TXNObject iTXNObject);


/*
  *************************************************************************************************
    Redraw the TXNObject including any scrollbars associated with the text frame.  Call this function
    in response to an update event for a window that contains multiple TXNObjects or some other graphic
    element.  The caller is responsible for calling BeginUpdate/EndUpdate in response to the update
    event.
    Input:
        iTXNObject:     opaque TXNObject to draw
        iDrawPort:  Can be NULL. If NULL the port is drawn to the port currently attached to the 
                    iTXNObject.  If not NULL drawing goes to the iDrawPort.  If drawing is done
                    to the iDrawPort selection is not updated.  This works this way so that it
                    is possible to Draw a TXNObject to a static port (i.e. print the thing without 
                    reflowing the text to match the paper size which is what TXNPrint does) 
                    and not have a line drawn where the selection would be.  If you pass an 
                    iDrawPort to an active TXNObject (i.e. editable) the selection will not be updated. In 
                    this case the selection will behave oddly until text is typed which will serve
                    to realign the selection.  Bottom-line don't pass a port in unless you want
                    static text (printed or non-editable)
  *************************************************************************************************
*/
/*
 *  TXNDraw()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNDraw(
  TXNObject   iTXNObject,
  GWorldPtr   iDrawPort);       /* can be NULL */


/*
  *************************************************************************************************
    Force a frame to be updated.  Very much like toolbox call InvalRect.
    
    Input:
        iTXNObject: opaque TXNObject 
  **************************************************************************************************                        
*/
/*
 *  TXNForceUpdate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNForceUpdate(TXNObject iTXNObject);



/*
  **************************************************************************************************
    Depending on state of window get the appropriate sleep time to be passed to WaitNextEvent.
    Input:
        iTXNObject: opaque TXNObject obtained from TXNNewObject
        
    Output:
    
        UInt32: function result appropriate sleep time.
  ***************************************************************************************************
*/
/*
 *  TXNGetSleepTicks()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( UInt32 )
TXNGetSleepTicks(TXNObject iTXNObject);


/*
  ***************************************************************************************************
    Do necessary Idle time processing. Typically flash the cursor. If a TextService is active
    pass a NULL event to the Text Service so it gets  time.
    Input:
        iTXNObject: opaque TXNObject obtained from TXNNewObject
  ****************************************************************************************************
*/
/*
 *  TXNIdle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNIdle(TXNObject iTXNObject);



/*
  *********************************************************************************************************
    Handle mouse-down in grow region. 
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject
        iEvent:     The mousedown event
  *********************************************************************************************************
*/
/*
 *  TXNGrowWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNGrowWindow(
  TXNObject            iTXNObject,
  const EventRecord *  iEvent);



/*
  ********************************************************************************************************
    Handle mousedown in zoom.
    Input:
        iTXNObject:     opaque TXNObject obtained from  TXNNewObject
        iPart:      Value returned by FindWindow
  *********************************************************************************************************
*/
/*
 *  TXNZoomWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNZoomWindow(
  TXNObject   iTXNObject,
  SInt16      iPart);



/*
  *******************************************************************************************************
    Use this to determine if the Undo item in Edit menu should be highlighted or not.  Tells you if last
    command was undoable.
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject
    Output:
        Boolean     function result.  If True the last command is undoable and the undo item in the menu 
                    should be active.  If false last command cannot be undone and undo should be grayed 
                    in the menu.
        oTXNActionKey The key code that the caller can use to pick a string to describe the undoable
                      action in the undo item.  Pass in NULL if the string isn't needed.
  *********************************************************************************************************
*/
/*
 *  TXNCanUndo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
TXNCanUndo(
  TXNObject       iTXNObject,
  TXNActionKey *  oTXNActionKey);      /* can be NULL */


/*
  ********************************************************************************************************
    Undo the last command.
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject
  *********************************************************************************************************
*/
/*
 *  TXNUndo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNUndo(TXNObject iTXNObject);



/*
  *********************************************************************************************************
    Use this to determine if the current item on the undo stack is redoable.  If it returns true.
    than the redo item in the edit menu should be active.
    
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject.
        oTXNActionKey The key code that the caller can use to pick a string to describe the redoable
                      action in the redo item.  Pass in NULL if the string isn't needed.
*/

/***********************************************************************************************************/
/*
 *  TXNCanRedo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
TXNCanRedo(
  TXNObject       iTXNObject,
  TXNActionKey *  oTXNActionKey);      /* can be NULL */



/*
  ********************************************************************************************************
    Redo the last command.
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject
  *********************************************************************************************************
*/
/*
 *  TXNRedo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNRedo(TXNObject iTXNObject);



/*
  *********************************************************************************************************
    Cut the current selection to the clipboard.
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject
    Output:
        OSStatus:   function result.  Variety of memory or scrap MacOS errors.
  **********************************************************************************************************
*/
/*
 *  TXNCut()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNCut(TXNObject iTXNObject);



/*
  *********************************************************************************************************
    TXNCopy
        Copy current selection
   
    Input:
        iTXNObject:         current document
                    
  **********************************************************************************************************                        
*/
/*
 *  TXNCopy()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNCopy(TXNObject iTXNObject);



/*
  ***********************************************************************************************************
    TXNPaste
        Paste the clipboard
   
    Input:
        iTXNObject:         current document
                    
  **********************************************************************************************************
*/
/*
 *  TXNPaste()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNPaste(TXNObject iTXNObject);



/*
  **********************************************************************************************************
    TXNClear
        clear the current selection
   
    Input:
        iTXNObject:         current document
                    
  **********************************************************************************************************
*/
/*
 *  TXNClear()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNClear(TXNObject iTXNObject);



/*
  *********************************************************************************************************
    TXNGetSelection
        Get the absolute offsets of the current selection.  
        Embedded graphics, sound, etc. each count as one character.
   
    Input:
        iTXNObject:         current document
    
    Output:
        oStartOffset:   absolute beginning of the current selection.
        oEndOffset:     end of current selection.
  *********************************************************************************************************                     
*/
/*
 *  TXNGetSelection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNGetSelection(
  TXNObject    iTXNObject,
  TXNOffset *  oStartOffset,
  TXNOffset *  oEndOffset);



/*
  *****************************************************************************************************
    Scroll the current selection into view.
    Input:
        iTXNObject:         opaque TXNObject obtained from TXNNewObject
        iShowEnd:   If true the end of the selection is scrolled into view. If false the
                    beginning of selection is scrolled into view.
  ****************************************************************************************************                      
*/
/*
 *  TXNShowSelection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNShowSelection(
  TXNObject   iTXNObject,
  Boolean     iShowEnd);



/*
  *****************************************************************************************************
    Call to find out if the current selection is empty. Use this to determine if Paste, Cut, Copy, 
    Clear should be highlighted in Edit menu.
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject
    Output:
        Boolean:    function result.  True if current selection is empty (i.e. start offset == end offset).  
                    False if selection is not empty.
  ********************************************************************************************************
*/
/*
 *  TXNIsSelectionEmpty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
TXNIsSelectionEmpty(TXNObject iTXNObject);



/*
  ********************************************************************************************************
    Set the current selection. 
   
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject
        iStartOffset:   new beginning
        iEndOffset:     new end
  ********************************************************************************************************
*/
/*
 *  TXNSetSelection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNSetSelection(
  TXNObject   iTXNObject,
  TXNOffset   iStartOffset,
  TXNOffset   iEndOffset);



/*
  *******************************************************************************************************
    TXNGetContinuousTypeAttributes
        Test the current selection to see if type size, style, color and/or font are continuous. 
        That is is the current selection made up of one font, one font size, one Style, and/or one color.  
        On return examine the flags to see if the attributes specified were continuous.  If an attribute
        is continuous then the dataValue field in the TXNTypeAttributes can be examined to get the continous
        value.  Remember that for color you pass a ptr to an RGBColor in attr[0].data.dataPtr.
    Input:
        iTXNObject:             current document
        oContinuousFlags:   Bits which can be examined to see if type size, style, color, and/or font are continuous
                            
                            e.g 
                            if (  TXNGetContinuousTypeAttributes( txnObject, &flags, 1, &attr ) == noErr )
                            {
                                    if ( flags & kTXNFontContinuousMask )
                                        ....check a font name
        ioCount:            Count of TXNTypeAttributes records in the ioTypeAttributes array.
        ioTypeAttributes:   Array of TXNTypeAttributes that indicate the type attributes the
                            caller is interested in.  For example, if you wanted to know if
                            the current selection was continuous in terms of being all
                            one same font size you could do something like this.
                            
                            TXNTypeAttributes       attr[1] = { TXNFontSizeAttribute, sizeof(Fixed),{ 0 } }
                            
                            on return  from the function if size is continuous (i.e. if the bit 3 of flags is set)
                            then the third field (attr[0].data.dataValue) will contain the size of the font as a Fixed value.
    
                                
  ***********************************************************************************************************
*/
/*
 *  TXNGetContinuousTypeAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNGetContinuousTypeAttributes(
  TXNObject             iTxnObject,
  TXNContinuousFlags *  oContinuousFlags,
  ItemCount             iCount,
  TXNTypeAttributes     ioTypeAttributes[]);    /* can be NULL */



/*
  *************************************************************************************************
    TXNSetTypeAttributes
        Set the current ranges font information.  Values are passed
        in the attributes array.  Values <= sizeof(UInt32) are passed
        by value. > sizeof(UInt32) are passed as a pointer.  That is
        the TXNTypeAttributes' 3rd field is a union that servers as
        either a 32-bit integer where values can be written or a 32-bit pointer 
        a value.
    Input:
        iTXNObject:         current document
        iAttrCount:     Count of type attributes in the TXNTypeAttributes array.
        iAttributes:    Attributes that caller would like to set.
        iStartOffset:   Start of the range where text attributes should be changed.
        iEndOffset:     End of the range.
    
    Output:
        OSStatus:           various MacOS  errs.  Notably memory manager and paramErrs.                         
  *************************************************************************************************                         
*/
/*
 *  TXNSetTypeAttributes()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNSetTypeAttributes(
  TXNObject                 iTXNObject,
  ItemCount                 iAttrCount,
  const TXNTypeAttributes   iAttributes[],
  TXNOffset                 iStartOffset,
  TXNOffset                 iEndOffset);



/*
 *  TXNSetTXNObjectControls()
 *  
 *  Summary:
 *    Sets formatting and privileges attributes (such as justification,
 *    line direction, tab values, and read-only status) that apply to
 *    the entire text object.
 *  
 *  Discussion:
 *    On systems that use Apple Type Services for Unicode Imaging
 *    (ATSUI), the ATSUI line control attribute tags can be passed to
 *    this function in the iControlTag parameter. This is the case for
 *    all the ATSUI tags except kATSULineRotationTag. ATSUI tags are
 *    applied to the entire text object.
 *  
 *  Parameters:
 *    
 *    iTXNObject:
 *      The text object that identifies the document for which you want
 *      to set formatting and privileges attributes.
 *    
 *    iClearAll:
 *      A Boolean value. If you set this to true, all formatting and
 *      privileges attributes are reset to their default value. That
 *      is, true clears existing tags and resets each to its default
 *      value.
 *    
 *    iControlCount:
 *      The number of items in the iControlTags array.
 *    
 *    iControlTags:
 *      An array of values that specifies kind of data that is passed
 *      in the iControlData parameter. See "Formatting and Privileges
 *      Settings" for a description of possible values. On systems that
 *      use Apple Type Services for Unicode Imaging (ATSUI), you can
 *      also pass ATSUI attribute tag constants. See the ATSUI
 *      documentation for a description of the ATSUI constants. Can be
 *      NULL if iClearAll is true.
 *    
 *    iControlData:
 *      An array of TXNControlData unions that contain the information
 *      your application wants to set. The value you supply to the
 *      iControlTags parameter specifies how the union of type
 *      TXNControlData is treated. You must make sure that the value
 *      you assign to the iControlData parameter is the appropriate
 *      type implied by the value you passed in the iControlTags
 *      parameter. Can be NULL if iClearAll is true.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNSetTXNObjectControls(
  TXNObject              iTXNObject,
  Boolean                iClearAll,
  ItemCount              iControlCount,
  const TXNControlTag    iControlTags[],      /* can be NULL */
  const TXNControlData   iControlData[]);     /* can be NULL */



/*
 *  TXNGetTXNObjectControls()
 *  
 *  Summary:
 *    Gets the current formatting and privileges attributes (such as
 *    justification, line direction, tab values, and read-only status)
 *    for a text object.
 *  
 *  Parameters:
 *    
 *    iTXNObject:
 *      The text object that identifies the document to be activated.
 *    
 *    iControlCount:
 *      The number of items in the iControlTags array.
 *    
 *    iControlTags:
 *      An array of values that specify the kind of formatting
 *      information you want returned in the oControlData array. See
 *      "Formatting and Privileges Settings" for a description of
 *      possible values.
 *    
 *    oControlData:
 *      An array of TXNControlData unions. On return, the array
 *      contains the information that was requested through the
 *      iControlTags array. Your application must allocate the
 *      oControlData array.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNGetTXNObjectControls(
  TXNObject             iTXNObject,          /* can be NULL */
  ItemCount             iControlCount,
  const TXNControlTag   iControlTags[],
  TXNControlData        oControlData[]);



/*
  ******************************************************************************************************
    TXNCountRunsInRange
        Given a range specified by the starting and ending offset return a count of the runs in that
        range.  Run in this case means changes in TextSyles or a graphic or sound.
        Result:
            OSStatus:       paramerr mostly
        Input:
            iTXNObject          The TXNObject you are interested in.
            iStartOffset        start of range
            iEndOffset          end of range
        
        Output:
            oRunCount           count of runs in the range
  *******************************************************************************************************
*/
/*
 *  TXNCountRunsInRange()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNCountRunsInRange(
  TXNObject    iTXNObject,
  TXNOffset    iStartOffset,
  TXNOffset    iEndOffset,
  ItemCount *  oRunCount);



/*
 *  TXNGetIndexedRunInfoFromRange()
 *  
 *  Summary:
 *    Gets information about a run in a range of data.
 *  
 *  Discussion:
 *    You should first call the TXNCountRunsInRange function to get the
 *    count. The TXNTypeAttributes structure must specify the text
 *    attribute in which the application is interested. In other words,
 *    the tag field must be set.
 *  
 *  Parameters:
 *    
 *    iTXNObject:
 *      The text object for the current text area.
 *    
 *    iIndex:
 *      The value that corresponds to the run for which you want to get
 *      information. You call the TXNCountRunsInRange function to get
 *      the number of runs in a range. The iIndex parameter is
 *      zero-based, so its possible values are from 0 to the number of
 *      runs in a range minus 1.
 *    
 *    iStartOffset:
 *      The offset at which you want to start to obtain run information.
 *    
 *    iEndOffset:
 *      The offset at which you want run information to end.
 *    
 *    oRunStartOffset:
 *      On return, a pointer to a value that identifies the start of
 *      run relative to the beginning of the text, not the beginning of
 *      the range you specified in the iStartOffset parameter.
 *    
 *    oRunEndOffset:
 *      On return, a pointer to a value that identifies the end of the
 *      run relative to the beginning of the text, not the beginning of
 *      the range you specified in the iStartOffset parameter.
 *    
 *    oRunDataType:
 *      On return, a pointer to a value that identifies the type of
 *      data in the run. See "Supported Data Types" for a description
 *      of possible values.
 *    
 *    iTypeAttributeCount:
 *      The number of font attributes.
 *    
 *    ioTypeAttributes:
 *      A pointer to a structure of type TXNTypeAttributes. On input,
 *      you specify the attribute (such as color) in the tag field and
 *      the attribute size in the size field. You can pass NULL for the
 *      data field. On return, the data field contains the attribute
 *      data. The data field is a union that serves either as a 32-bit
 *      integer or a 32-bit pointer, depending on the size field.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNGetIndexedRunInfoFromRange(
  TXNObject            iTXNObject,
  ItemCount            iIndex,
  TXNOffset            iStartOffset,
  TXNOffset            iEndOffset,
  TXNOffset *          oRunStartOffset,           /* can be NULL */
  TXNOffset *          oRunEndOffset,             /* can be NULL */
  TXNDataType *        oRunDataType,              /* can be NULL */
  ItemCount            iTypeAttributeCount,
  TXNTypeAttributes *  ioTypeAttributes);         /* can be NULL */



/*
  **********************************************************************************************************
    TXNDataSize
    Return the size in bytes of the characters in a given TXNObject.
    Result:
        ByteCount:          The bytes required to hold the characters
    Input:
        iTXNObject:         The TXNObject
    
  **********************************************************************************************************
*/
/*
 *  TXNDataSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ByteCount )
TXNDataSize(TXNObject iTXNObject);



/*
  ***********************************************************************************************************
    Copy the data in the range specified by startOffset and endOffset. This function should be used
    in conjunction with TXNNextDataRun.  The client would call TXNCountRunsInRange to the number of data
    runs in a given range.  The client can then walk the runs with the function TXNGetIndexedRunInfoFromRange.
    TXNGetIndexedRunInfoFromRange lets you examine each runs type and text attributes.  
    For each data run of interest (i.e. one whose data the caller wanted to look at) 
    the client would call TXNGetData. The handle passed to TXNGetData should not be allocated.  
    TXNGetData takes care of allocating the dataHandle as necessary.  However, the caller is  responsible 
    for disposing the handle.  
    No effort is made to insure that data copies align on a word boundary.  Data is simply copied as
    specified in the offsets.
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject.
        iStartOffset:   absolute offset from which data copy should begin.
        iEndOffset: absolute offset at which data copy should end.
                        
    Output:
        OSStatus    Memory errors or  TXN_IllegalToCrossDataBoundaries if offsets specify a range that 
                    crosses a data type boundary.
        oDataHandle: If noErr a new handle containing the requested data. The caller is responsible
                     for disposing the handle.  Note that the handle is a copy so it can be safely
                     disposed at any time.
  **********************************************************************************************************
*/
/*
 *  TXNGetData()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNGetData(
  TXNObject   iTXNObject,
  TXNOffset   iStartOffset,
  TXNOffset   iEndOffset,
  Handle *    oDataHandle);



/*
  ***********************************************************************************************************
    Copy the data in the range specified by startOffset and endOffset. 
    The handle passed to TXNGetDataEncoded should not be allocated.  
    TXNGetData takes care of allocating the dataHandle as necessary.  However, the caller is  responsible 
    for disposing the handle.  
    No effort is made to insure that data copies align on a word boundary.  Data is simply copied as
    specified in the offsets.
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject.
        iStartOffset:   absolute offset from which data copy should begin.
        iEndOffset: absolute offset at which data copy should end.
        iEncoding : should be kTXNTextData or kTXNUnicodeTextData               
    Output:
        OSStatus    Memory errors or  TXN_IllegalToCrossDataBoundaries if offsets specify a range that 
                crosses a data type boundary.
        oDataHandle:        If noErr a new handle containing the requested data.
  **********************************************************************************************************
*/
/*
 *  TXNGetDataEncoded()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNGetDataEncoded(
  TXNObject     iTXNObject,
  TXNOffset     iStartOffset,
  TXNOffset     iEndOffset,
  Handle *      oDataHandle,
  TXNDataType   iEncoding);



/*
  *********************************************************************************************************
    Replace the specified range with the contents of the specified file.  The data fork of the file 
    must be opened by the caller.
    Input:
        iTXNObject:     opaque TXNObject obtained from  TXNNewObject
        fileSpec:   HFS file reference obtained when file is opened.
        fileType:   files type.
        iFileLength: The length of data in the file that should be considered data.  This
                     parameter is available to enable callers to embed text inside their
                     own private data structures.  Note that if the data is in the Textension(txtn)
                     format this parameter is ignored since length, etc. information is
                     part of the format. Further note that if you you just want Textension
                     to read a file and you are not interested in embedding you can just pass
                     kTXNEndOffset(0x7FFFFFFF), and Textension will use the file manager to
                     determine the files length.
        iStartOffset:   start position at which to insert the file into the document.
        iEndOffset:     end position of range being replaced by the file.
    Output:
        OSStatus:   File manager error or noErr.
  ***********************************************************************************************************
*/
/*
 *  TXNSetDataFromFile()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNSetDataFromFile(
  TXNObject   iTXNObject,
  SInt16      iFileRefNum,
  OSType      iFileType,
  ByteCount   iFileLength,
  TXNOffset   iStartOffset,
  TXNOffset   iEndOffset);



/*
 *  TXNSetData()
 *  
 *  Summary:
 *    Replaces a range of data (text, graphics, and so forth).
 *  
 *  Parameters:
 *    
 *    iTXNObject:
 *      The text object that identifies the document in which you want
 *      to replace data.
 *    
 *    iDataType:
 *      The type of the replacement data. See "Supported Data Types"
 *      for a description of possible values.
 *    
 *    iDataPtr:
 *      A pointer to the data that will replace the data that is in the
 *      range specified by the iStartOffset and iEndOffset parameters. 
 *      Can be NULL if the start and end offsets are different.
 *    
 *    iDataSize:
 *      The size of the data to which iDataPtr points.
 *    
 *    iStartOffset:
 *      The beginning of the range of data to replace. You can use the
 *      TXNGetSelection function to get the absolute offsets of the
 *      current selection.
 *    
 *    iEndOffset:
 *      The end of the range to replace. You can use the
 *      TXNGetSelection function to get the absolute offsets of the
 *      current selection. If you want to insert text, the ending and
 *      starting offsets should be the same value.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNSetData(
  TXNObject     iTXNObject,
  TXNDataType   iDataType,
  const void *  iDataPtr,           /* can be NULL */
  ByteCount     iDataSize,
  TXNOffset     iStartOffset,
  TXNOffset     iEndOffset);


/*
  ************************************************************************************************************                          
    Retrieve number of times document has been changed.  That is for every committed command 
    (keydown, cut, copy) the value returned is count of those. This is useful for deciding if  the Save 
    item in the File menu should be active.
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject
                            
    Output:
        ItemCount:  count of changes.  This is total changes since document  was created or last saved.  
                    Not count since this routine was last called or anything like that.
  ***********************************************************************************************************
*/
/*
 *  TXNGetChangeCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( ItemCount )
TXNGetChangeCount(TXNObject iTXNObject);



/*
  *********************************************************************************************************
    Save the contents of the document as the given type. 
    Input:
        iTXNObject:         opaque TXNObject obtained from TXNNewObject.
        iType:              The type of file to create.
        iResType:           When saving file as plain TEXT the type of resource to
                            save style information.  Use kTXNMultipleStylesPerTextDocumentResType
                            if your document contains multiple styles and you want a SimpleText
                            like document.  Use kTXNSingleStylePerTextDocumentResType if the 
                            document has a single style and you would like a BBEdit, MPW, CW type
                            of document.
        iPermanentEncoding: The encoding in which the document should be saved (Unicode, Text or System
                            default).
        iFileSpecification:  The file specification to which the document should be saved. The
                             file must have been opened by the caller.  The file specification is remembered by the TXNObject
                             and is used for any subsequent calls to TXNRevert.
        iDataReference:     The data fork ref num.  This is used to write data to the data fork of the
                            file. The data is written beginning at the current mark.  
        iResourceReference: The resource fork ref num.  If the caller has specified that style information be
                            saved as a resource (MPW or SimpleText) than this should be a valid reference to
                            an open resource fork.  If the txtn format is being used than this input value
                            is ignored.
    Output:
        OSStatus        The result of writing the file.
  **********************************************************************************************************    
*/
/*
 *  TXNSave()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNSave(
  TXNObject                      iTXNObject,
  TXNFileType                    iType,
  OSType                         iResType,
  TXNPermanentTextEncodingType   iPermanentEncoding,
  const FSSpec *                 iFileSpecification,
  SInt16                         iDataReference,
  SInt16                         iResourceReference);



/*
  ***********************************************************************************************************
    Revert  to the last saved version of this document.  If the file was not previously saved the document
    is reverted to an empty document.
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject
    Output:
        OSStatus:   File manager errors, paramErr, or noErr.
  **********************************************************************************************************                    
*/
/*
 *  TXNRevert()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNRevert(TXNObject iTXNObject);



/*
  *********************************************************************************************************                 
    Display the Page Setup dialog of the current default printer and react to any changes 
    (i.e. Reformat the text if the page layout changes.)
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject.
    
    Output:
        OSStatus:   Print Manager errors, paramErr, noErr.
  **********************************************************************************************************
*/
/*
 *  TXNPageSetup()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNPageSetup(TXNObject iTXNObject);



/*
  **********************************************************************************************************
    Print the document.
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject.
    Output:
        OSStatus:   Print Manager errors, paramErr, noErr.
  **********************************************************************************************************
*/
/*
 *  TXNPrint()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNPrint(TXNObject iTXNObject);



/*
  ***********************************************************************************************************                           
    Test to see if the current scrap contains data that is supported by Textension.  Used to determine
    if Paste item in Edit menu should be active or inactive. The types of data supported depends on what 
    data types were specified in the TXNInitTextension options.
    Output:
        Boolean:        function result.  True if data type in Clipboard is supported.  False if 
                        not a supported data type.  If result is True the Paste item in the menu can 
                        be highlighted.
  **********************************************************************************************************
*/
/*
 *  TXNIsScrapPastable()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
TXNIsScrapPastable(void);



/*
  ***********************************************************************************************************
    Convert the Textension private scrap to the public clipboard.  This should be called on suspend 
    events and before the application displays a dialog that might support cut and paste.  Or more 
    generally, whenever someone other than the Textension Shared Library needs access to the scrap data.
    Output:
         OSStatus:  Function result.  Memory Manager errors, Scrap Manager errors, noErr.
  ************************************************************************************************************
*/
/*
 *  TXNConvertToPublicScrap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNConvertToPublicScrap(void);



/*
  ***********************************************************************************************************
    Convert the  public clipboard to our private scrap .  This should be called on resume 
    events and after an application has modified the scrap. Before doing work we check the validity of the public 
    scrap (date modification and type)
    Output:
         OSStatus:  Function result.  Memory Manager errors, Scrap Manager errors, noErr.
  ************************************************************************************************************  
*/
/*
 *  TXNConvertFromPublicScrap()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNConvertFromPublicScrap(void);



/*
  ************************************************************************************************************
    Get the  rectangle describing the current view into the document. The coordinates of this rectangle will be 
    local to the the window.
    Input:
        iTXNObject:     opaque TXNObject obtained from TXNNewObject.
    Output:
        oViewRect:      the requested view rectangle.
  *************************************************************************************************************
*/
/*
 *  TXNGetViewRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNGetViewRect(
  TXNObject   iTXNObject,
  Rect *      oViewRect);



/*
  !! TXNSetViewRect is now deprecated for 10.2 and later.  Please use TXNSetFrameBounds or TXNSetRectBounds API !!
  *************************************************************************************************                         
   Set the rectangle describing the current view into the document. This
   will change how much text is viewable.  Not where a line of text wraps.
   That is controlled by TXNSetFrameBoundsSize.
   Input:
        iTXNObject :    opaque Textension structure.
        
        iViewRect:      Rect of the view
         
    
  *************************************************************************************************
*/
/*
 *  TXNSetViewRect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.3 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
TXNSetViewRect(
  TXNObject     iTXNObject,
  const Rect *  iViewRect);



/*
  ***********************************************************************************************************
    Find a piece of text or a graphics object.
    Input:
        iTXNObject:         opaque TXNObject obtained from TXNNewObject.
        iMatchTextDataPtr   ptr to a MatchTextRecord which contains the text to match, the length of that text
                            and the TextEncoding the text is encoded in.  This must be there if you are looking
                            for Text, but can be NULL if you are looking for a graphics object.
        iDataType           The type of data to find.  This can be any of the types defined in TXNDataType enum
                            (TEXT, PICT, moov, snd ).  However, if PICT, moov, or snd is passed then the default
                            behavior is to match on any non-Text object.  If you really want to find a specific
                            type you can provide a custom find callback or ignore matches which aren't the precise
                            type you are interested in.
        iStartSearchOffset  The offset at which a search should begin. The constant kTXNStartOffset specifies the start
                            of the objects data.
        iEndSearchOffset    The offset at which the search should end. The constant kTXNEndOffset specifies the end
                            of the objects data.
        iFindProc           A custom callback.  If will be called to match things rather than the default matching
                            behavior.
        iRefCon             This can be use for whatever the caller likes.  It is passed to the FindProc (if a FindProc
                            is provided.
    Output:
        oStartMatchOffset   absolute offset to start of match.  set to 0xFFFFFFFF if not match.
        oEndMatchOffset     absolute offset to end of match.  Set to 0xFFFFFFFF is no match.
    The default matching behavior is pretty simple for Text a basic binary compare is done.  If the matchOptions say 
    to ignore case the characters to be searched are duplicated and case neutralized.  This naturally can fail due
    to lack of memory if there is a large amount of text.  It also slows things down.  If MatchOptions say
    find an entire word that once a match is found an effort is made to determine if the match is a word.  The default
    behavior is to test the character before and after the to see if it is White space.  If the kTXNUseEncodingWordRulesBit
    is set than the Script Manager's FindWord function is called to make this determination.
    If the caller is looking for a non-text type than each non-text type in the document is returned.
    If more elaborate ( a regular expression processor or whatever ) is what you want then that is what the FindProc is
    for.
  *******************************************************************************************************************
*/
/*
 *  TXNFind()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNFind(
  TXNObject                   iTXNObject,
  const TXNMatchTextRecord *  iMatchTextDataPtr,        /* can be NULL */
  TXNDataType                 iDataType,
  TXNMatchOptions             iMatchOptions,
  TXNOffset                   iStartSearchOffset,
  TXNOffset                   iEndSearchOffset,
  TXNFindUPP                  iFindProc,
  SInt32                      iRefCon,
  TXNOffset *                 oStartMatchOffset,
  TXNOffset *                 oEndMatchOffset);



/*
  ***************************************************************************************************************
   TXNSetFontDefaults
    
   For a given TXNObject specify the font defaults for each script.
   Input:
        iTXNObject:         opaque TXNObject obtained from TXNNewObject.
        iCount:             count of FontDescriptions.
        iFontDefaults:      array of FontDescriptions.
   Output:
        OSStatus:           function result ( memory error, paramErr )
  ****************************************************************************************************************
*/
/*
 *  TXNSetFontDefaults()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNSetFontDefaults(
  TXNObject                                iTXNObject,
  ItemCount                                iCount,
  const TXNMacOSPreferredFontDescription   iFontDefaults[]);



/*
  ***************************************************************************************************************
   TXNGetFontDefaults
    
   For a given TXNObject make a copy of the font defaults.
   Input:
        iTXNObject:         opaque TXNObject obtained from TXNNewObject.
        iCount:             count of FontDescriptions in the array.
        iFontDefaults:      array of FontDescriptins to be filled out.
   Output:
        OSStatus:           function result ( memory error, paramErr )
    To determine how many font descriptions need to be in the array you should call this function with
    a NULL for the array.  iCount will return with the number of font defaults currently stored.
  ****************************************************************************************************************
*/
/*
 *  TXNGetFontDefaults()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNGetFontDefaults(
  TXNObject                          iTXNObject,
  ItemCount *                        ioCount,
  TXNMacOSPreferredFontDescription   ioFontDefaults[]);    /* can be NULL */



/*
  ****************************************************************************************************************
    TXNAttachObjectToWindow
    If a TXNObject was initialized with a NULL window pointer use this function to attach a window
    to that object.  In version 1.0 of Textension attaching a TXNObject to more than one window
    is not supported.  Note that if a CGContextRef was passed to the TXNObject previously thru the
    API TXNSetTXNObjectControls, that CGContextRef will be ignored.  The CGContextRef associated with
    the iWindow will be used instead.  You may revert back to the previous CGContextRef by calling the
    API TXNSetTXNObjectControls with the desired CGContextRef again after calling TXNAttachObjectToWindow.
    
    Input:
        iTXNObject:         opaque TXNObject obtained from TXNNewObject.
        iWindow:            GWorldPtr that the object should be attached to
        iIsActualWindow:    Let the library know if the GWorldPtr is actually
                            a WindowRef or actually a GWorldPtr.  This is important
                            if the client is taking advantage of the editing packages
                            scrollbar support.
    Output:
        OSStatus:           function result.  paramErrs. 
  ****************************************************************************************************************
*/
/*
 *  TXNAttachObjectToWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNAttachObjectToWindow(
  TXNObject   iTXNObject,
  GWorldPtr   iWindow,
  Boolean     iIsActualWindow);



/*
  ****************************************************************************************************************
    TXNIsObjectAttachedToWindow
    A utility function that allows a caller to check a TXNObject to see if it is attached
    to a window.
    Input:
        iTXNObject:         opaque TXNObject obtained from TXNNewObject.
    Output:
        Boolean:            function result.  True is object is attached.
                            False if TXNObject is not attached.
  ****************************************************************************************************************
*/
/*
 *  TXNIsObjectAttachedToWindow()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( Boolean )
TXNIsObjectAttachedToWindow(TXNObject iTXNObject);



/*
  ****************************************************************************************************************
    TXNDragTracker
    If you ask that Drag handling procs not be installed.  Call this when your drag tracker is called
    and you want Textension to take over
    Input:
        iTXNObject:         opaque TXNObject obtained from TXNNewObject.
        iTXNFrameID:            TXNFrameID obtained from TXNNewObject
        iMessage:           drag message obtained from Drag Manager
        iWindow :           WindowRef obtained from Drag Manager
        iDragReference:     dragReference obtained from Drag Manager
        iDifferentObjectSameWindow: Pass true if the drag is still in the same window
                                   that it started in. False if the drag has moved into
                                   a different window.
    Output:
        OSErr:              function result.  OSErr is used over
                            OSStatus so that it matches the Drag Manager definition of Tracking callback
  ****************************************************************************************************************
*/
/*
 *  TXNDragTracker()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
TXNDragTracker(
  TXNObject             iTXNObject,
  TXNFrameID            iTXNFrameID,
  DragTrackingMessage   iMessage,
  WindowRef             iWindow,
  DragReference         iDragReference,
  Boolean               iDifferentObjectSameWindow);



/*
  ****************************************************************************************************************
    TXNDragReceiver
    If you ask that Drag handling procs not be installed.  Call this when your drag receiver is called
    and you want Textension to take over
    Input:
        iTXNObject:         opaque TXNObject obtained from TXNNewObject.
        iTXNFrameID         TXNFrameID obtained from TXNNewObject
        iWindow             WindowRef obtained from Drag Manager
        iDragReference      dragReference obtained from Drag Manager
        iDifferentObjectSameWindow: Pass true if the drag is still in the same window
                                   that it started in. False if the drag has moved into
                                   a different window.
    Output:
        OSErr:              function result.  OSErr is used over
                            OSStatus so that it matches the Drag Manager definition of Tracking callback
  ****************************************************************************************************************
*/
/*
 *  TXNDragReceiver()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSErr )
TXNDragReceiver(
  TXNObject       iTXNObject,
  TXNFrameID      iTXNFrameID,
  WindowRef       iWindow,
  DragReference   iDragReference,
  Boolean         iDifferentObjectSameWindow);



/*
  ****************************************************************************************************************
    TXNActivate
    Make the TXNObject object active in the sense that it can be scrolled if it has scrollbars.  If the TXNScrollBarState parameter
    is true than the scrollbars will be active even when the TXNObject is not focused (i.e. insertion point not active)
    
    This function should be used if you have multiple TXNObjects in a window, and you want them all to be scrollable
    even though only one at a time can have the keyboard focus.
    Input:
        iTXNObject:         opaque TXNObject obtained from TXNNewObject.
        iTXNFrameID         TXNFrameID obtained from TXNNewObject
        iActiveState        Boolean if true Scrollbars active even though TXNObject does not have the keyboard focus.  
                            if false scrollbars are synced with active state (i.e. a focused object has an
                            active insertion point or selection and active scrollbars. An unfocused object has inactive
                            selection (grayed or framed selection) and inactive scrollbars.  The latter state is the 
                            default and usually the one you use if you have one TXNObject in a window.
    Output:
        OSStatus:           function result.  ParamErr if bad iTXNObject or frame ID.
  ****************************************************************************************************************
*/
/*
 *  TXNActivate()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNActivate(
  TXNObject           iTXNObject,
  TXNFrameID          iTXNFrameID,
  TXNScrollBarState   iActiveState);



/*
  *****************************************************************************************************************
    TXNSetBackgound
    Set the type of background the TXNObject's text, etc. is drawn onto.  At this point the background
    can be a color or a picture.  
    
    Input:
        iTXNObject:         opaque TXNObject obtained from IncomingDataFilter callback.
        iBackgroundInfo:    struct containing information that describes the background
    Output:
        OSStatus:           function result.  paramErrs.
  ********************************************************************************************************************
*/
/*
 *  TXNSetBackground()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNSetBackground(
  TXNObject              iTXNObject,
  const TXNBackground *  iBackgroundInfo);



/*
  *****************************************************************************************************************
    TXNEchoMode
    Put the TXNObject into echo mode.  What that means is that all characters in the TXNObject have the character
    'echoCharacter' substituted for the actual glyph when drawing occurs. Note that the echoCharacter is typed
    as a UniChar, but this is done merely to facilitate passing any 2 byte character.  The encoding parameter
    actually determines the encoding used to locate a font and display a character.  Thus if you wanted to
    display the diamond found in the Shift-JIS encoding for MacOS you would pass in 0x86A6 for the character
    but an encoding that was built to represent the MacOS Japanese encoding.
    
    Input:
        iTXNObject:         opaque TXNObject obtained from IncomingDataFilter callback.
        iEchoCharacter:     character to use in substitution
        iEncoding:          encoding from which character is drawn.
        iOn:                true if turning EchoMode on.  False if turning it off.
    Output:
        OSStatus:           function result.  paramErrs.
  ********************************************************************************************************************
*/
/*
 *  TXNEchoMode()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNEchoMode(
  TXNObject      iTXNObject,
  UniChar        iEchoCharacter,
  TextEncoding   iEncoding,
  Boolean        iOn);



/*
  ********************************************************************************************************************
    TXNNewFontMenuObject
    Get a FontMenuObject.  Caller can extract a fontmenu from this object and pass this object to the active
    TXNObject to handle events in the font menu.
    Input:
        iFontMenuHandle:    An empty menu handle (well the title is there) that the caller created via
                            NewMenu or GetNewMenu. This menu handle should not be disposed before
                            the returned TXNFontMenuObject has been disposed via TXNDisposeFontMenuObject.
        iMenuID:            The MenuID for iFontMenuHandle.
        iStartHierMenuID:   The first MenuID to use if any hierarchical menus need to be created. TXNNewFontMenuObject
                            uses SetMenuItemHierarchicalID when creating hierarchial menus.  The iStartHierMenuID must
                            therefor follow the rules for this function.  On systems less than system 8.5 the submenuID
                            must be less than 255.  For systems above system 8.5 the range can be as large can be as large
                            32767.  However, it is important to remember that TXNNewFontMenuObject only uses iStartHierMenuID
                            as a starting id when adding hierarchical menus.  Therefore provide plenty of room to increment
                            this value. For example, on a system less than 8.5 it would be good to start at 175.  On systems greater
                            than 8.5 it is probably a good idea to not use a value higher than 32000. 
        
    Output:
        OSStatus:           function result.  memory, parameter errors.
        TXNFontMenuObject:  A font menu object
  *********************************************************************************************************************
*/
/*
 *  TXNNewFontMenuObject()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNNewFontMenuObject(
  MenuRef              iFontMenuHandle,
  SInt16               iMenuID,
  SInt16               iStartHierMenuID,
  TXNFontMenuObject *  oTXNFontMenuObject);



/*
  *********************************************************************************************************************
    TXNGetFontMenuHandle
    Get the MenuRef from the TXNFontMenuObject.
    Input:
        iTXNFontMenuObject:     A Font Menu Object obtained from TXNNewFontMenuObject.
    Output:
        OSStatus:           function result. parameter errors.
        oFontMenuHandle:    The returned font menu. (returned value could be NULL)
  *********************************************************************************************************************
*/
/*
 *  TXNGetFontMenuHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNGetFontMenuHandle(
  TXNFontMenuObject   iTXNFontMenuObject,
  MenuRef *           oFontMenuHandle);


#define TXNGetFontMenuRef TXNGetFontMenuHandle

/*
  *********************************************************************************************************************
    TXNDisposeFontMenuObject
    Dispose a TXNFontMenuObject and its font menu handle
    Input:
        iTXNFontMenuObject:     A Font Menu Object obtained from TXNNewFontMenuObject.
    Output:
        OSStatus:   function result. parameter errors.
  *********************************************************************************************************************
*/
/*
 *  TXNDisposeFontMenuObject()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNDisposeFontMenuObject(TXNFontMenuObject iTXNFontMenuObject);



/*
  *********************************************************************************************************************
    TXNDoFontMenuSelection
        Given the menuID and menu item returned by MenuSelect determine the selected font
        and change the current selection to be that Font.  If the input TXNObject is
        not active a parameter error is returned.
    Input:
        iTXNObject: An opaque TXNObject obtained from TXNNewObject.
        iTXNFontMenuObject:     A Font Menu Object obtained from TXNNewFontMenuObject.
        iMenuID:    SInt16 the ID of the selected menu.
        iMenuItem:  The item that was selected.
    Output:
        OSStatus:   ParamErr and memory errors are possible.
  **********************************************************************************************************************
*/
/*
 *  TXNDoFontMenuSelection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNDoFontMenuSelection(
  TXNObject           iTXNObject,
  TXNFontMenuObject   iTXNFontMenuObject,
  SInt16              iMenuID,
  SInt16              iMenuItem);



/*
 *  TXNPrepareFontMenu()
 *  
 *  Summary:
 *    Prepares a Font menu for display.
 *  
 *  Discussion:
 *    You should call the TXNPrepareFontMenu function just before your
 *    application opens the Font menu for your user. If the text
 *    object's current selection is a single font, MLTE places a
 *    checkmark next to the menu item for that font.
 *  
 *  Parameters:
 *    
 *    iTXNObject:
 *      The text object that identifies the document with the Font menu
 *      you want to prepare. Pass NULL to display an inactive menu
 *      (dimmed).
 *    
 *    iTXNFontMenuObject:
 *      A Font menu object.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNPrepareFontMenu(
  TXNObject           iTXNObject,               /* can be NULL */
  TXNFontMenuObject   iTXNFontMenuObject);



/*
  **********************************************************************************************************************
    TXNVersionValue
        Get the version number and a set of feature bits.  TXNVersionValue uses a NumVersion structure.
        See MacTypes.h for the format of the version.  Currently there are two feature bits:  one for 
        ATSUI default, another one for CarbonEvent default.
    Input:
        NONE
        
    Output:
        TXNVersionValue:    Current version.
        TXNFeatureBits*:    Pointer to a bit mask.  See TXNFeatureMask enum above. If kTXNWillDefaultToATSUIBit
                            is set it means that by default MLTE will use ATSUI to image and measure text and will
                            default to using Unicode to store characters.  If kTXNWillDefaultToCarbonEventBit is set,
                            then MLTE will use carbon events by default and apple event will not be supported.
  **********************************************************************************************************************
*/
/*
 *  TXNVersionInformation()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( TXNVersionValue )
TXNVersionInformation(TXNFeatureBits * oFeatureFlags);



/*
 *  TXNIsObjectAttachedToSpecificWindow()
 *  
 *  Summary:
 *    Determines whether the given object is attached to the given
 *    window.
 *  
 *  Parameters:
 *    
 *    iTXNObject:
 *      Opaque TXNObject obtained from TXNNewObject.
 *    
 *    iWindow:
 *      The window to check attachment against.
 *    
 *    oAttached:
 *      true if the object is attached to the given window, false
 *      otherwise.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension 1.2 and later
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( OSStatus )
TXNIsObjectAttachedToSpecificWindow(
  TXNObject   iTXNObject,
  WindowRef   iWindow,
  Boolean *   oAttached);


/*
 *  TXNSetRectBounds()
 *  
 *  Summary:
 *    Set the View rectangle and or the Destination rectangle.
 *  
 *  Discussion:
 *    The View rectangle controls the text you see.  The Destination
 *    rectangle controls how text is laid out.  The Scrollbar is drawn
 *    inside the View rectangle. You only need to pass in pointers for
 *    the rectangles you want to set.
 *  
 *  Parameters:
 *    
 *    iTXNObject:
 *      Opaque TXNObject obtained from TXNNewObject.
 *    
 *    iViewRect:
 *      The new view rectangle.  If you do not want to change the view
 *      rectangle pass NULL.
 *    
 *    iDestinationRect:
 *      The new destination rectangle.  Pass NULL if you don't want to
 *      change the destination retangle.
 *    
 *    iUpdate:
 *      If you would like the the text and where the scrollbars are
 *      placed recalculated and redrawn pass true.  If you prefer to
 *      wait on this pass false.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
TXNSetRectBounds(
  TXNObject            iTXNObject,
  const Rect *         iViewRect,              /* can be NULL */
  const TXNLongRect *  iDestinationRect,       /* can be NULL */
  Boolean              iUpdate);


/*
 *  TXNGetRectBounds()
 *  
 *  Summary:
 *    Get the values for the current View rectangle, Destination
 *    rectangle and Text rectangle.
 *  
 *  Discussion:
 *    You only need to pass in pointers for the rectangles you're
 *    interested in.
 *  
 *  Parameters:
 *    
 *    iTXNObject:
 *      Opaque TXNObject obtained from TXNNewObject.
 *    
 *    oViewRect:
 *      The current view rectangle
 *    
 *    oDestinationRect:
 *      The current destination rectangle
 *    
 *    oTextRect:
 *      The smallest rectangle needed to contain the current text. 
 *      This rectangle is calculated by walking the lines of text and
 *      measuring each line.  So this can be expensive.  The width of
 *      this rectangle will be the width of the longest line in the
 *      text.
 *  
 *  Result:
 *    An operating system status code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
TXNGetRectBounds(
  TXNObject      iTXNObject,
  Rect *         oViewRect,              /* can be NULL */
  TXNLongRect *  oDestinationRect,       /* can be NULL */
  TXNLongRect *  oTextRect);             /* can be NULL */



/*
 *  TXNRecalcTextLayout()
 *  
 *  Summary:
 *    Recalculates the text layout based on the new View and
 *    Destination rectangles.
 *  
 *  Discussion:
 *    Call this if you called TXNSetRectBounds with the iUpdate
 *    parameter set to false. It will also recalcuate where the
 *    scrollbars, if any, should be placed. Finally an update event
 *    will be generated so that the TXNObject is redrawn.
 *  
 *  Parameters:
 *    
 *    iTXNObject:
 *      Opaque TXNObject obtained from TXNNewObject.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( void )
TXNRecalcTextLayout(TXNObject iTXNObject);




/*
 *  TXNScroll()
 *  
 *  Discussion:
 *    TXNScroll scrolls the text within a view rectangle of the
 *    specified object by the designated number of units.  For example,
 *    you might want to scroll the text in an object in response to
 *    user input in a control other than the standard scrollbars that
 *    MLTE supplies.
 *  
 *  Parameters:
 *    
 *    iTXNObject:
 *      Opaque TXNObject obtained from TXNNewObject.
 *    
 *    iVerticalScrollUnit:
 *      Specifies what units the values in ioVerticalDelta are in.  If
 *      iVerticalScrollUnit is equal to kTXNScrollUnitsArePixels the
 *      value is treated as pixels.  If the value is
 *      kTXNScrollUnitsAreLines the value is treated as a count of
 *      lines. Note that using this value is the slowest because each
 *      line must be measured before it scrolls.  Finally if
 *      kTXNScrollUnitsAreViewRects the value is treated as the height
 *      of the current viewRect.
 *    
 *    iHorizontalScrollUnit:
 *      Specifies what units the values in iDh are in.  If
 *      iHorizontalScrollUnit is equal to kTXNScrollUnitsArePixels the
 *      value is treated as pixels.  If the value is
 *      kTXNScrollUnitsAreLines the value is treated as a count of
 *      lines. Note that using this value for horizontal scrolling
 *      means that 16 pixels will be used to represent a line.  Finally
 *      if kTXNScrollUnitsAreViewRects the value is treated as the
 *      width of the current viewRect.
 *    
 *    ioVerticalDelta:
 *      The vertical amount to scroll.  The values in ioVerticalDelta
 *      can be treated as pixels, lines or viewrects.  See the
 *      discussion of the TXNScrollUnit parameters for more information
 *      for this.  On return this will contain the number of pixels
 *      actually scrolled in the vertical direction. A positive value
 *      moves the text down.
 *    
 *    ioHorizontalDelta:
 *      The horizontal amount to scroll. The values in
 *      ioHorizontalDelta can specify a scroll amount that is pixels,
 *      lines or view rects.  Set TXNScrollUnit discussion for more
 *      information. On return this will contain the number of pixels
 *      actually scrolled in the horizontal direction. A positive value
 *      moves the text to the right.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension not yet available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
TXNScroll(
  TXNObject       iTXNObject,
  TXNScrollUnit   iVerticalScrollUnit,
  TXNScrollUnit   iHorizontalScrollUnit,
  SInt32 *        ioVerticalDelta,
  SInt32 *        ioHorizontalDelta);



/*
 *  TXNRegisterScrollInfoProc()
 *  
 *  Discussion:
 *    If your application is drawing and handling its own scrolling
 *    widgets use this function to register a TXNScrollInfoUPP.  If you
 *    register a TXNScrollInfoUPP it will be called every time MLTE
 *    would normally update the values and maximum values of an MLTE
 *    scrollbar. For example when the user types the return key to add
 *    a new line at the end of their text MLTE will calculate a new
 *    maximum value.  If you have registered a TXNScrollInfoUPP it will
 *    be called with this nex maximum value. To turn off the callbacks
 *    call TXNRegisterScrollInfoProc with a value of NULL for the
 *    iTXNScrollInfoUPP.
 *  
 *  Parameters:
 *    
 *    iTXNObject:
 *      Opaque TXNObject obtained from TXNNewObject.
 *    
 *    iTXNScrollInfoUPP:
 *      A universal procedure pointer.
 *    
 *    iRefCon:
 *      A refcon that is passed to the callback.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension not yet available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
TXNRegisterScrollInfoProc(
  TXNObject          iTXNObject,
  TXNScrollInfoUPP   iTXNScrollInfoUPP,
  SInt32             iRefCon);





/*
 *  TXNClearUndo()
 *  
 *  Summary:
 *    Purge the undo stack
 *  
 *  Parameters:
 *    
 *    iTXNObject:
 *      Opaque TXNObject obtained from TXNNewObject.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in Textension not yet available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
TXNClearUndo(TXNObject iTXNObject);



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

#endif /* __MACTEXTEDITOR__ */

